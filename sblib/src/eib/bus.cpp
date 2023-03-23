/**
 * bus.cpp - Low level EIB bus access.
 *
 * Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 * Copyright (c) 2021 Horst Rauch
 * ********************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 **/

#include <sblib/eib/bus.h>
#include <sblib/eib/knx_lpdu.h>
#include <sblib/eib/knx_npdu.h>
#include <sblib/core.h>
#include <sblib/interrupt.h>
#include <sblib/platform.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/eib/bcu_base.h>
#include <sblib/timer.h>
#include <sblib/eib/bus_debug.h>

// pin output macros for debugging with e.g logic analyzer - not used
#define D(x)

/* L1/L2 msg header control field data bits meaning */
#define ALWAYS0       	   0          // bit 0 is always 0
#define ACK_REQ_FLAG       1          // 0 LL_ACK is requested - for TP-Bus ACK is mandatory for normal telegrams - always 0
#define PRIO0_FLAG         2          // 00 system priority, 10 alarm priority, 01 high priority, 11 low priority
#define PRIO1_FLAG         3          //
#define ACK_FRAME_FLAG     4          // 1 frame is data or poll, 0 ack frame
#define REPEAT_FLAG        5          // 1 not repeated, 0 repeated frame
#define DATA_FRAME_FLAG    6          // 1 poll data frame, 0 data frame
#define LONG_FRAME_FLAG    7          // 1,  short frame <=15 char, 0 long frame > 15 char
#define HEADER_LENGHT      7          // KNX msg header length by length indicator =0

// telegram control byte bit definitions
#define SB_TEL_NULL_FLAG        (1 << ALWAYS0)
#define SB_TEL_ACK_REQ_FLAG     (1 << ACK_REQ_FLAG)
#define SB_TEL_PRIO0_FLAG       (1 << PRIO0_FLAG)
#define SB_TEL_PRIO1_FLAG       (1 << PRIO1_FLAG)
#define SB_TEL_ACK_FRAME_FLAG   (1 << ACK_FRAME_FLAG)
#define SB_TEL_REPEAT_FLAG      (1 << REPEAT_FLAG)
#define SB_TEL_DATA_FRAME_FLAG  (1 << DATA_FRAME_FLAG)
#define SB_TEL_LONG_FRAME_FLAG  (1 << LONG_FRAME_FLAG)
#define SB_TEL_PRIO_FLAG        (SB_TEL_PRIO0_FLAG | SB_TEL_PRIO1_FLAG)

// we accept only normal data frames for telegram processing
#define VALID_DATA_FRAME_TYPE_MASK (SB_TEL_LONG_FRAME_FLAG | SB_TEL_DATA_FRAME_FLAG | SB_TEL_ACK_FRAME_FLAG | SB_TEL_ACK_REQ_FLAG | SB_TEL_NULL_FLAG)
#define VALID_DATA_FRAME_TYPE_VALUE  (SB_TEL_LONG_FRAME_FLAG | SB_TEL_ACK_FRAME_FLAG )

#define PREAMBLE_MASK  (( 1<< ALWAYS0) | ( 1<< ACK_REQ_FLAG))

#define PRIO_FLAG_HIGH	 (SB_TEL_PRIO0_FLAG)
#define PRIO_FLAG_LOW	 (SB_TEL_PRIO0_FLAG | SB_TEL_PRIO1_FLAG )


//  **************define some default values for data/link layer based on the knx spec ***********

#define NACK_RETRY_DEFAULT 3        //!> default NACK retry
#define BUSY_RETRY_DEFAULT 3        //!> default BUSY retry
#define ROUTE_CNT_DEFAULT 6         //!> default Route Count

#define PHY_ADDR_AREA(address) ((address >> 12) & 0x0f) //!> return the area number of a given physical KNX address
#define PHY_ADDR_LINE(address) ((address >> 8) & 0x0f)  //!> return the line number of a given physical KNX address
#define PHY_ADDR_DEVICE(address) (address & 0xff)       //!> return device number of a given physical KNX address


//  **************define some flags and timing values based on the knx spec ***********

// Mask for the timer flag of the capture channel
#define CAPTURE_FLAG (8 << captureChannel)

// Mask for the timer flag of the time channel
#define TIME_FLAG (8 << timeChannel)



#define BUS_BUSY_DETECTION_FRAME 7  // >=7 us before start bit check for bus busy for normal frame
#define BUS_BUSY_DETECTION_ACK 16 // >=16 us before start bit check for bus busy for ACK frame

// Default time between two bits (104 usec)
#define BIT_TIME 104

// Time between two bits (69 usec) - high level part of the pulse on the bus
#define BIT_WAIT_TIME 69

// Pulse duration of a bit (35 usec)
#define BIT_PULSE_TIME 35

// Maximum time from start bit to stop bit, including a safety extra: BIT_TIME*10 + BIT_TIME/2
//#define BYTE_TIME 1090

// Maximum time from start bit to end of stop bit: BIT_TIME*11
#define BYTE_TIME_INCL_STOP 1144

// Maximum time from start bit to start of stop bit: BIT_TIME*10
#define BYTE_TIME_EXCL_STOP 1040

// Maximum time from start bit to next bytes start bit  + 13*104 +30 margin
#define START_BIT_BYTE_TIME_MAX 1382

// Maximum time from end of stop bit to start bit of next byte 2 Bit time + 100us marging = timeout for end of telegram
#define MAX_INTER_CHAR_TIME 308

// Time to wait before sending an ACK after valid rx telegram: 15* BIT Time
#define SEND_ACK_WAIT_TIME 15*BIT_TIME

// Time to wait  for window to prepare  sending an ACK  15* BIT Time: approximately - 69us should be enough
#define SEND_ACK_WINDOW_START_TIME  SEND_ACK_WAIT_TIME  -  BIT_WAIT_TIME

//for rx process: Time from end of stop bit to start of ACK = 15* bit time -5us/+30us acc to spec,
//we add add marging of 100us as we have seen some early acks on the bus
//
#define ACK_WAIT_TIME 15*BIT_TIME
#define ACK_WAIT_TIME_MIN 15*BIT_TIME -5 - 100
#define ACK_WAIT_TIME_MAX 15*BIT_TIME +30 + 100

// after TX wait for ack reception from remote side
#define RX_ACK_WAIT_TIME  		15* BIT_TIME  //
#define RX_ACK_WAIT_TIME_MAX  	15* BIT_TIME +30 // we wait 15*BT -5 min,   1560 +30us max
#define RX_ACK_WAIT_TIME_MIN  	15* BIT_TIME -5 //

// Time to wait before starting to send: BIT_TIME * 50
#define SEND_WAIT_TIME 5200

// Time to wait before bus is in idle state: BIT_TIME * 50
#define WAIT_50BIT_FOR_IDLE 5200

// Time to wait before repetition of sending a telegram due to busy from remote side  BIT_TIME * 150
#define BUSY_WAIT_150BIT  (150 * BIT_TIME)

// Time to listen for bus activity before sending starts: BIT_TIME * 1
#define PRE_SEND_TIME 104

// The value for the prescaler
#define TIMER_PRESCALER (SystemCoreClock / 1000000 - 1)


// constructor for Bus object. Initialize basic interface parameter to bus and set SM to IDLE
Bus::Bus(BcuBase* bcuInstance, Timer& aTimer, int aRxPin, int aTxPin, TimerCapture aCaptureChannel, TimerMatch aPwmChannel)
:bcu(bcuInstance)
,timer(aTimer)
,rxPin(aRxPin)
,txPin(aTxPin)
,captureChannel(aCaptureChannel)
,pwmChannel(aPwmChannel)

{
	timeChannel = (TimerMatch) ((pwmChannel + 2) & 3);  // +2 to be compatible to old code during refactoring
	state = Bus::INIT;
}


/**
 * Start BUS operation
 *
 * reset operating parameters, start the timer
 * set the pwm parameter so that we have no pulse on bus and no interrupt from timer
 * activate capture interrupt, set bus pins to respective mode
 *
 *	//todo get defined values from usereprom for busy-retry and nack-retry
 */
void Bus::begin()
{
	//todo load send-retries from eprom
	//sendTriesMax =  userEeprom.maxRetransmit & 0x03;
	//sendBusyTriesMax = (userEeprom.maxRetransmit >>5) & 0x03; // default
	sendTriesMax =  NACK_RETRY_DEFAULT;
	sendBusyTriesMax = BUSY_RETRY_DEFAULT; // default

	telegramLen = 0;
	sendAck = 0;
	//need_to_send_ack_to_remote=false;
	sendCurTelegram = 0;
	sendNextTel = 0;
	collision = false;
	state = Bus::INIT;  // we wait bus idle time (50 bit times) before setting bus to idle
	//initialize bus-timer( e.g. defined as 16bit timer1)
	timer.begin();
	timer.pwmEnable(pwmChannel);
	// any cap intr during start up time is ignored and will reset start up time
	timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT);
	//timer.counterMode(DISABLE,  captureChannel | FALLING_EDGE); // todo  enabled the timer reset by the falling edge of cap event
	timer.start();
	timer.interrupts();
	timer.prescaler(TIMER_PRESCALER);
	timer.reset();
	timer.match(timeChannel, WAIT_50BIT_FOR_IDLE);
	timer.matchMode(timeChannel, INTERRUPT | RESET); // at timeout we have a bus idle state
	timer.match(pwmChannel, 0xffff);

    lastRXTimeVal = millis();// time measurement between telegrams

	// wait until output is driven low before enabling output pin.
	// Using digitalWrite(txPin, 0) does not work with MAT channels.
	timer.value(0xffff); // trigger the next event immediately
	while (timer.getMatchChannelLevel(pwmChannel) == true);
	pinMode(txPin, OUTPUT_MATCH);   // Configure bus output
	pinMode(rxPin, INPUT_CAPTURE | HYSTERESIS);  // Configure bus input

    DB_TELEGRAM(serial.println("DUMP_TELEGRAMS Bus telegram dump enabled."));
#ifdef DEBUG_BUS
    IF_DEBUG(serial.println("DEBUG_BUS dump enabled."));
#endif

#ifdef DEBUG_BUS_BITLEVEL
    IF_DEBUG(serial.println("DEBUG_BUS_BITLEVEL dump enabled."));
#endif

    DB_BUS(
        //we use 32bit timer 0 as debugging timer running with 1Mhz or system clock (48MHz)
        ttimer.begin();
        ttimer.start();
        ttimer.noInterrupts();
        ttimer.reset();
        // ttimer.prescaler(0);
        ttimer.prescaler(TIMER_PRESCALER);
        serial.print("Bus begin - Timer prescaler: ", (unsigned int)TIMER_PRESCALER, DEC, 6);
        serial.print(" ttimer prescaler: ", ttimer.prescaler(), DEC, 6);
        serial.println(" ttimer value: ", ttimer.value(), DEC, 6);
        serial.print("nak retries: ", sendTriesMax, DEC, 6);
        serial.print(" busy retries: ", sendBusyTriesMax, DEC, 6);
        serial.print(" phy addr: ", PHY_ADDR_AREA(bcu->ownAddress()), DEC);
        serial.print(".", PHY_ADDR_LINE(bcu->ownAddress()), DEC);
        serial.print(".", PHY_ADDR_DEVICE(bcu->ownAddress()), DEC);
        serial.print(" (0x",  bcu->ownAddress(), HEX, 4);
        serial.println(")");
    ); // DB_BUS

#ifdef PIO_FOR_TEL_END_IND
    pinMode(PIO_FOR_TEL_END_IND, OUTPUT);
    digitalWrite(PIO_FOR_TEL_END_IND, 0);
#endif

	//
	// Init GPIOs for debugging ?? logic analyser???
	//
	D(digitalWrite(PIO3_1, 1));
	D(digitalWrite(PIO3_0, 1));

	D(pinMode(PIO1_5, OUTPUT));
	D(pinMode(PIO1_4, OUTPUT));
	D(pinMode(PIO0_6, OUTPUT));
	D(pinMode(PIO0_7, OUTPUT));
	//D(pinMode(PIO2_8, OUTPUT));
	//D(pinMode(PIO2_9, OUTPUT));

	D(digitalWrite(PIO3_0, 0));
	D(digitalWrite(PIO3_1, 0));
	D(digitalWrite(PIO1_5, 0));
	D(digitalWrite(PIO1_4, 0));
	D(digitalWrite(PIO0_6, 0));
	D(digitalWrite(PIO0_7, 0));
	//D(digitalWrite(PIO2_8, 0));
	//D(digitalWrite(PIO2_9, 0));
	//D(digitalWrite(PIO2_10, 0));
}


void Bus::prepareTelegram(unsigned char* telegram, unsigned short length) const
{
	setSenderAddress(telegram, (uint16_t)bcu->ownAddress());

	// Calculate the checksum
	unsigned char checksum = 0xff;
	for (unsigned short i = 0; i < length; ++i)
	{
		checksum ^= telegram[i];
	}
	telegram[length] = checksum;
}

/**
 *       Interface to upper layer for sending a telegram
 *
 * Is called from within the BCU-loop method. Is blocking if there is no space
 * in the Telegram buffer (as we have only one buffer at BCU level, the check for buffer free is
 * in the BCU-loop on bus.sendingTelegram())
 *
 * Send a telegram. The checksum byte will be added at the end of telegram[].
 * Ensure that there is at least one byte space at the end of telegram[].
 *
 * @param telegram - the telegram to be sent.
 * @param length - the length of the telegram in sbSendTelegram[], without the checksum
 */
void Bus::sendTelegram(unsigned char* telegram, unsigned short length)
{

	prepareTelegram(telegram, length);

	// Wait until there is space in the sending queue
	while (sendNextTel);

	if (!sendCurTelegram)
    {
	    sendCurTelegram = telegram;
    }
	else if (!sendNextTel)
    {
        sendNextTel = telegram;
    }
	else
    {
	    fatalError(); // soft fault: send buffer overflow
    }

#ifdef DUMP_TELEGRAMS
	unsigned int t;
	t = ttimer.value();
	serial.print("QUE: (", t, DEC, 8); // queued to send
	serial.print(") ");
	for (int i = 0; i <= length; ++i)
	{
		if (i) serial.print(" ");
		serial.print(telegram[i], HEX, 2);
	}
	serial.println();
#endif

	// Start sending if the bus is idle or sending will be triggered in WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE after finishing current TX/RX
	noInterrupts();
	if (state == IDLE)
	{
		sendTries = 0;
		sendBusyTries =0;
		repeatTelegram = false;

		wait_for_ack_from_remote = false;
		state = Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE;
		timer.match(timeChannel, 1);
		timer.matchMode(timeChannel, INTERRUPT | RESET);
		timer.value(0);
	}
	interrupts();
}

/*
 *  set the Bus state machine to idle state.
 *  We waited at least 50 Bit times  (without cap event enabled), now we wait for next Telegram to receive.
 *  configure the capture to falling edge and interrupt
 *  match register for low pwm output
 *
 *todo we could stop the timer for energy save reasons
 */
void Bus::idleState()
{
	tb_t( 99, ttimer.value(), tb_in);
	tb_h( 99, sendAck, tb_in);

	timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT ); // for any receiving start bit on the Bus
	timer.matchMode(timeChannel, RESET); // no timeout interrupt, reset at match todo we could stop timer for power saving
	timer.match(timeChannel, 0xfffe); // stop pwm pulse generation, set output to low
	timer.match(pwmChannel, 0xffff);
	//timer.counterMode(DISABLE,  captureChannel | FALLING_EDGE); //todo enabled the  timer reset by the falling edge of cap event
	state = Bus::IDLE;
	sendAck = 0;
	//need_to_send_ack_to_remote=false;
}

/*
 * *********** Layer 2 rx/tx handling, part of the interrupt processing -> keep as short as possible**********
 *
 * We arrive here after we received  data from bus indicated by a time out of >2BT after RX of bits
 *
 * Check for valid data reception in rx_error. If we received some bytes, process data (valid telegram) and if tel is for us
 * check for need of sending ACK to sender side and start respective process:  time for ACK (15BT) or inter telegram wait (50BT)
 * and set WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE state
 * If we had a collision,  no action
 *  **  End of telegram is indicated by a timeout. As the timer is still running we have the accurate time since the last stop bit.
 *
 *  copy local rx-data to 2nd rx buffer for parallel processing on higher layer and SM

 * Provide some Data-layer functions:
 *  check telegram type
 *  check if it is for us (phy adr, or Grp Adr belong to us)
 *  check if we want it anyhow (TL, LL info)
 *  send ACK/NACK if requested
 *  send BUSYACK if we are still busy on higher layers - no free buffer available
 *  handling of repeated telegrams
 *
 * Input by global data:
 *	timer flags: timeout indicates end of last telegram, timer value hold the time since rx of last stop bit
 *	rx_error: hold error status of rx process
 *	collision: indicated a collision during last tx process which is then continued in RX process
 *	parameter valid: true if parity and checksum of received tel are ok
 *
 * Output data:
 * 	processTel: indicate telegram reception to the looping function by setting <processTel> to true
 *	telegram[]: received telegram in telegram buffer <telegram[]>, length in telegramLen
 *	telegramLen: rx telegram lenght
 *	sendAck:  !0:  RX process need to send ack to sending side back, set wait timer accordingly
 * 	indicate result of telegram reception/error state to upper layer via bus_rx_state/bus_tx_state
 * 	and bus_rxstae_valid/bus_txstate_valid
 *
 *
 * @param bool of all received char parity and frame checksum error
 *
 */
void Bus::handleTelegram(bool valid)
{
#ifdef DEBUG_BUS
	b1 = (((unsigned int)rx_telegram[0]<<24) |((unsigned int)rx_telegram[1]<<16) |((unsigned int)rx_telegram[2]<<8)| (rx_telegram[3]));
	b2= ( ((unsigned int)rx_telegram[4]<<8) | (rx_telegram[5]));
	b3= (( unsigned int)rx_telegram[6]<<8)|((unsigned int)rx_telegram[7]);
	b4= nextByteIndex;
	b5= ( collision + (valid ? 2 : 0));
	tb2( 9000, b5,  b1, b2, b3, b4, tb_in);
	//    D(digitalWrite(PIO1_4, 1));         // purple: end of telegram
#endif

#ifdef DUMP_TELEGRAMS
	//telRXtime= ttimer.value();
	if (nextByteIndex){
		for (int i = 0; i < nextByteIndex; ++i)
		{
			telBuffer[i] = rx_telegram[i];
		}
		telLength = nextByteIndex;
		telcollision = collision;
	}
#endif

	//need_to_send_ack_to_remote=false;
	sendAck = 0; // clear any pending ACK TX
	int time = SEND_WAIT_TIME -  PRE_SEND_TIME; // default wait time after bus action
	state = Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE;//  default next state is wait for 50 bit times for pending tx or new  rx
	tb_h( 908, currentByte, tb_in);
	tb_h( 909, parity, tb_in);

#ifndef BUSMONITOR   // no processing if we are in monitor mode

	// Received a valid telegram with correct checksum and valid control byte ( normal data frame with preamble bits)?
	//todo extended tel, check tel len, give upper layer error info
	if ( nextByteIndex >= 8 && valid  &&  (( rx_telegram[0] & VALID_DATA_FRAME_TYPE_MASK) == VALID_DATA_FRAME_TYPE_VALUE)
			&& nextByteIndex <= bcu->maxTelegramSize()  )
	{
		int destAddr = (rx_telegram[3] << 8) | rx_telegram[4];
		bool processTel = false;

		// We ACK the telegram only if it's for us
		if (rx_telegram[5] & 0x80) // groupr addr or phy addr
		{
		    processTel = (destAddr == 0); // broadcast
		    processTel |= (bcu->addrTables != nullptr) && (bcu->addrTables->indexOfAddr(destAddr) >= 0); // known group addr
		}
		else if (destAddr == bcu->ownAddress())
		{
			processTel = true;
		}
		else
		{
            DB_BUS(serial.println(" Bus::handleTelegram not processed: 0x", destAddr, HEX));
		}

		// Only process the telegram if it is for us or if we want to get all telegrams
		if (!(bcu->userRam->status() & BCU_STATUS_TRANSPORT_LAYER))
		{ // TL is disabled we might process the telegram
			processTel = true;
			// if LL is in normal mode (not busmonitor mode) we send ack back
			if (bcu->userRam->status() & BCU_STATUS_LINK_LAYER)
				if (rx_telegram[0] & SB_TEL_ACK_REQ_FLAG )
				{
					sendAck = SB_BUS_ACK;
					//need_to_send_ack_to_remote=true;
				}
		}
		if (processTel)
		{// check for repeated telegram, did we already received it -check that time between telegram is about 50bit times
			// check the repeat bit in header and compare with previous received telegram still stored in the telegram[] buffer
			bool already_received = false;
			//check time in between the telegrams received should be less than 6ms for repeated tel and the orig tel
			if ((lastRXTimeVal <= millis() + 6) && !(rx_telegram[0] & SB_TEL_REPEAT_FLAG)) // a repeated tel
			{// compare telegrams
				if ((rx_telegram[0] & ~SB_TEL_REPEAT_FLAG) == (telegram[0] & ~SB_TEL_REPEAT_FLAG))
				{// same header- compare data

					int i;
					for (i =1; i <= nextByteIndex-1 && rx_telegram[i] == telegram[i]; i++);
					if (i>=nextByteIndex) {
						already_received = true;
					}
				}
			}

			if (!already_received){
				//  check for space in rx buffer for next telegram, if no space available, send busy back
				if (telegramLen)
				{
					sendAck = SB_BUS_BUSY;
					//need_to_send_ack_to_remote= true;//  no free rx buffer, send busy back
					//busy_wait_to_remote = true;
					rx_error |= RX_BUFFER_BUSY;
				}
				else
				{// store data in telegram buffer for higher layers, set telegramLen to indicate data available
					//todo check if telegram[] buffer is free
					for (int i = 0; i < nextByteIndex; i++) telegram[i] = rx_telegram[i];
					telegramLen = nextByteIndex;
					bus_rx_state = rx_error;
					rx_error = 0;
					setBusRXStateValid(true);

					lastRXTimeVal = millis();
					//check if an ACK is requested -> not needed for poll frame
					//if (! ( (rx_telegram[0] & SB_TEL_ACK_REQ_FLAG)  ||   (rx_telegram[0] & SB_TEL_DATA_FRAME_FLAG)) )
					if (!(rx_telegram[0] & SB_TEL_DATA_FRAME_FLAG) )
					{
					    sendAck = SB_BUS_ACK;
					    if(destAddr == 0)
					    {
                            // test no ack for BC
                            // sendAck = 0;
					    }
						//need_to_send_ack_to_remote = (sendAck != 0);
					}
				}
				// we received a valid telegram need to send an ack/busy to remote - has priority, no rx/tx  in between
				if ( sendAck){
					state = Bus::RECV_WAIT_FOR_ACK_TX_START;
					time = SEND_ACK_WAIT_TIME - PRE_SEND_TIME;
				}
			}
		}
	}
	else if (nextByteIndex == 1 && parity)   // Received a spike or a bus acknowledgment, only parity, no checksum
	{
		currentByte &= 0xff;
		tb_h( 907, currentByte, tb_in);
		if ((currentByte == SB_BUS_ACK) && ((rx_error & RX_CHECKSUM_ERROR) == RX_CHECKSUM_ERROR))
        {
            // received a ACK so clear checksum bit previously set in Isr Bus::timerInterruptHandler
            rx_error &= ~RX_CHECKSUM_ERROR;
        }

		//  did we send a telegram ( sendTries>=1 and the received telegram is ACK or repetition max  -> send next telegram
		if (wait_for_ack_from_remote) {
			if ((currentByte == SB_BUS_ACK ) ||  sendTries >=sendTriesMax || sendBusyTries >= sendBusyTriesMax)
			{ // last sending to remote was ok or max retry, prepare for next tx telegram
				if ( sendTries >=sendTriesMax || sendBusyTries >= sendBusyTriesMax) tx_error|= TX_RETRY_ERROR;
				tb_h( 906, tx_error, tb_in);
				sendNextTelegram();
			}
			else if (currentByte == SB_BUS_BUSY)
			{
				time = BUSY_WAIT_150BIT - PRE_SEND_TIME;
				tx_error |=TX_REMOTE_BUSY_ERROR;
				busy_wait_from_remote = true;
				repeatTelegram = true;
			}
			else
			{// we received nack or wrong ack, need to repeat last telegram
				tx_error |= TX_NACK_ERROR;
				busy_wait_from_remote = false;
				repeatTelegram = true;
			}
		}
	}
	else if (collision) // A collision occurred during sending. Ignore the received bytes
	{
		collision = false;
	}
	else // We received wrong checksum or parity, or more than one byte but too short for a telegram
	{
		rx_error |=RX_INVALID_TELEGRAM_ERROR;
	}

    DB_TELEGRAM(telrxerror = rx_error);

	tb_d( 901, state, tb_in);	tb_d( 902, sendTries, tb_in);	tb_d( 903, sendBusyTries, tb_in);tb_h( 904, sendAck, tb_in);
	tb_h( 905, rx_error, tb_in); tb_d( 910, telegramLen, tb_in);tb_d( 911, nextByteIndex, tb_in);

#endif
#ifdef BUSMONITOR
	rx_error = 0;
#endif


	//we received a telegram, next action wait to send ack back or wait 50 bit times for next rx/tx (todo check for improved noise margin with cap event disabled)
	timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer for next action: ack or telegram  start bit time -PRE_SEND_TIME
	//timer.captureMode(captureChannel, FALLING_EDGE); 	// no capture during wait- improves bus noise margin l
	timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT ); 	// todo enable timer reset by cap event
	timer.match(timeChannel,time); // todo adjust time value by processing timer since we had the end of telegram detection
}

/*
 * Current telegram was send, send next telegram
 *
 * load data for next telegram, save  send resultState - driven by interrupts of timer and capture input
 * Free send-buffer is indicated by "0" in the header byte: send-buffer[0]=0
 *
 */
void Bus::sendNextTelegram()
{
    bus_tx_state = tx_error;
    tx_error = TX_OK;
    setBusTXStateValid(true);

    if (sendCurTelegram)
    {
        sendCurTelegram[0] = 0;
    }
    sendCurTelegram = sendNextTel;
    sendNextTel = 0;
    sendTries = 0;
    sendBusyTries = 0;
    sendTelegramLen = 0;
    wait_for_ack_from_remote = false;
    repeatTelegram = false;
}


/*
 * State Machine - driven by interrupts of timer and capture input
 *
 * Interrupt prolog  (from event at cap pin or timer match) takes about 3-5us processing time (incl SM state select)
 * Selecting the right state of SM Bus
 *
 */
__attribute__((optimize("O3"))) void Bus::timerInterruptHandler()
{
	D(static unsigned short tick = 0);
	volatile bool timeout;
	volatile int time;
	unsigned int dt, tv, cv;

	// Debug output
	D(digitalWrite(PIO0_6, ++tick & 1));  // brown: interrupt tick
	D(digitalWrite(PIO3_0, state==Bus::SEND_BIT_0)); // red
	D(digitalWrite(PIO3_1, 0));           // orange
	D(digitalWrite(PIO1_5, 0));           // yellow

#ifdef PIO_FOR_TEL_END_IND
    digitalWrite(PIO_FOR_TEL_END_IND, 0);           // rest PIO
#endif

	//D(digitalWrite(PIO2_8, 0));           // blue
	//D(digitalWrite(PIO2_9, 0));           //

	// debug processing takes about 7-8us
	tbint( state+8000, ttimer.value(), timer.flag(captureChannel),  timer.capture(captureChannel), timer.value(), timer.match(timeChannel), tb_in);

	STATE_SWITCH:
	switch (state)
	{

	// BCU is in start-up phase, we wait for 50 bits inactivity of the bus
	case Bus::INIT:
		tb_t( state, ttimer.value(), tb_in);
#ifdef DUMP_TELEGRAMS
		telRXWaitInitTime= ttimer.value(); // if is is less than 50 we have a failure on the bus
#endif

		if (timer.flag(captureChannel))
		{// cap event: bus not in idle state before we have a timeout, restart waiting time
		timer.value (0); // restart the timer
			break;
		}
		// timeout- we set bus to idle state
		idleState();
		break;


		// The bus is idle for at least 50BT. Usually we come here when we finished a TX/RX on the Bus and waited 50BT for next event without receiving a start bit on the Bus
		// or at least one pending Telegram in the queue.
		// A timeout  (after 0xfffe us) should not be received( indicating no bus activity) match interrupt is disabled
		// A reception of a new telegram is triggered by the falling edge of the received start bit and we collect the bits in the receiving process
		// Sending is triggered in idle state by a call to sendTelegram and state switch from idle to WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE to send pending the telegram

	case Bus::IDLE:
		tb_d( state+100, ttimer.value(), tb_in);
#ifdef DUMP_TELEGRAMS
		telRXWaitIdleTime= ttimer.value(); //
#endif

		if (!timer.flag(captureChannel)) // Not a bus-in signal or Tel in the queue: do nothing - timeout??
			break;


		// RX process functions
		//initialize the RX process for a new telegram reception.
		//triggered by a capture event while waiting for a new telegram or ACK or an early
		//capture while trying to send a start bit in the TX process
	case Bus::INIT_RX_FOR_RECEIVING_NEW_TEL:
		tb_d( state+100, ttimer.value(), tb_in);

#ifdef DUMP_TELEGRAMS
		// correct the timer start value by the process time (about 13us) we had since the capture event
		//unsigned int dt, tv, cv;
		tv=timer.value(); cv= timer.capture(captureChannel);
		if ( tv > cv ) dt= tv - cv; // check for timer overflow since cap event
		else dt = (0xffff-cv) +tv;

		telRXStartTime= ttimer.value()- dt;
#endif

		nextByteIndex = 0;
		collision = false;
		rx_error  = 0;
		checksum = 0xff;
		repeatTelegram = false;
		sendAck = 0;
		valid = 1;

		//todo if timer was  disabled for power saving and enable in this state
		// no break here as we have received a capture event - falling edge of the start bit
		// we continue with receiving of start bit.

		// A start bit (by cap event) is expected to arrive here. If we have a timeout instead, the
		// transmission of a frame is over.  (after 11  bit plus 2 fill bits :13*104us  + margin (1452us) after start of last char)
		// we expect that the timer was restarted by the end of the last frame (end of stop bit) and not restarted by a cap event
		// timer will be pre-set with for the capture timing (few us), mode reset, interrupt, macht of frame time (11bits), capture interrupt
	case Bus:: RECV_WAIT_FOR_STARTBIT_OR_TELEND:
		D(digitalWrite(PIO3_1, 1));   // orange

		if (!timer.flag(captureChannel))  // No start bit: then it is a timeout of end of frame
		{ // did the sending process waited for an ack from remote?
			{
				if (checksum)
                {
                    rx_error |= RX_CHECKSUM_ERROR; ///\todo 0xCC (short ACKs, maybe also NAK and BUSY?) are flagged with a checksum_error
                }
#ifdef DUMP_TELEGRAMS
		telRXEndTime= ttimer.value() - timer.value(); // timer was restarted at end of last stop bit -
#   ifdef PIO_FOR_TEL_END_IND
		digitalWrite(PIO_FOR_TEL_END_IND, 1);           // set Pin to high till next interrupt as end of Tel indication
#   endif
#endif

				handleTelegram(valid && !checksum);
			}
			break;
		}

		//tb_h( state +100, currentByte, tb_in);
		// we captured a startbit falling edge trigger
		// read the start bit now (about few us after cap event) if it is still at low level to avoid that spikes will start analysis of rx data
		if (digitalRead(rxPin)) rx_error |= RX_STARTBIT_ERROR; // we set error flag and continue rx process

		// we received a start bit interrupt - reset timer for next byte reception,
		// set byte time incl stop bit to 1144us and use that as ref for all succeeding timings in RX process
		// any rx-bit start should be within n*104 -7us, n*104 + 33us -> max 1177us
		// correct the timer start value by the process time (about 13us) we had since the capture event
		//todo  restart timer by capture in order to have a ref point for the frame timeout
		// and check the capture event to be in the allowed time window for the start bit

		tv=timer.value(); cv= timer.capture(captureChannel);
		if ( tv > cv ) dt= tv - cv; // check for timer overflow since cap event
		else dt = (0xffff-cv) +tv;
		timer.restart();  // restart timer and pre-load with processing time of 2us
		timer.value(dt+2);
		timer.match(timeChannel, BYTE_TIME_INCL_STOP);
		timer.matchMode(timeChannel, INTERRUPT | RESET);
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT); // next state interrupt at first low bit  - falling edge, no reset
		//timer.counterMode(DISABLE,  DISABLE); // disabled the timer reset by the falling edge of cap event
		state = Bus::RECV_BITS_OF_BYTE;
		currentByte = 0;
		bitTime = 0;
		bitMask = 1;
		parity = 1;

#ifdef DUMP_TELEGRAMS
		// correct the timer start value by the process time (about 13us) we had since the capture event
		//unsigned int dt, tv, cv;
		telRXTelByteStartTime= ttimer.value()- dt;
#endif

		break;

		// we received next capture event for a low bit at position n*104us or timeout if we have end of byte received
		// bitMask hold the position of the expected low bit, bitTime the start-time of the expected expected low bit
		// if we received an edge interrupt later than the expected bitTime we had some high bits in between and need to calculate how many.
		// bitMask: bit8 : parity; bit9 : stop bit

	case Bus::RECV_BITS_OF_BYTE:
		//tb_t( RECV_BITS_OF_BYTE, ttimer.value(), tb_in);

		timeout = timer.flag(timeChannel); // timeout--> end of rx byte
		if (timeout) time = BYTE_TIME_INCL_STOP; // BYTE_TIME = 11 bits
		else
		{
			time = timer.capture(captureChannel); // we received an capt. event: new low bit
			// read the bit again (about few us after cap event) if it is not at low level we received a spike - set error flag and continue
			if (digitalRead(rxPin)) rx_error |= RX_TIMING_ERROR_SPIKE; // we set error flag and continue rx process
		}

		// find the bit position after last low bit and add high bits accordingly, window for the reception of falling edge of a bit is:
		//min: n*104us-7us, typ: n*104us, max: n*104us+33us. bitTime holds the start time of the last bit, so the new received
		//cap event should be between bitTime + BIT_TIME -7us and bitTime + BIT_TIME+33us
		//bittime hold the time of the n-th bit (0..9) time=n*104, timer is counting from startbit edge- should be 104us in advance

		if (time >= bitTime + BIT_TIME - 35 ) // check window should be at least  n*104-7us  *** we use -35us to be more tolerant
		{ // bit is not to early- check for to late - we might have some high bits received since last low bit
			bitTime += BIT_TIME; //set bittime to next expected bit edge
			while (time >= bitTime + BIT_WAIT_TIME && bitMask <= 0x100) // high bit found or bit 9 (stop bit) found - move check to next bit position
			{
				currentByte |= bitMask; // add high bit until we found current position
				parity = !parity;
				bitTime += BIT_TIME; // next bit time
				bitMask <<= 1; // next bit is in bitmask
			}

			if (time > bitTime  +33 && bitMask <= 0x100)
			{
				rx_error |= RX_TIMING_ERROR_SPIKE; // bit edge receive but pulse to short late- window error

				//report timing error for debugging
#ifdef DUMP_TELEGRAMS
		telRXTelBitTimingErrorLate = time;
#endif
			}
			bitMask <<= 1; //next bit or stop bit
			//tb_d( RECV_BITS_OF_BYTE +400, time, tb_in);
			//tb_d( RECV_BITS_OF_BYTE +500, bitTime, tb_in);
		} else // we might have received a additional edge due to bus reflection, tx-delay, edge should be within bit pulse +30us else ignore edge
		{
			rx_error |= RX_TIMING_ERROR_SPIKE; // bit edge receive but pulse to short late- window error
			// report timing error for debugging
#ifdef DUMP_TELEGRAMS
		telRXTelBitTimingErrorEarly = time;
#endif
		}

		if (timeout)  // Timer timeout: end of byte
		{
			D(digitalWrite(PIO1_5, 1));     // yellow: end of byte
			D(digitalWrite(PIO3_1, parity));// orange: parity bit ok

			// check bit0 and bit 1 of first byte for low level preamble bits
			if ( (!nextByteIndex) && (currentByte & PREAMBLE_MASK) )
				rx_error |= RX_PREAMBLE_ERROR;// preamble error, continue to read bytes - possibility to discard the telegram at higher layer

			if (nextByteIndex < bcu->maxTelegramSize())
			{
				rx_telegram[nextByteIndex++] = currentByte;
				checksum ^= currentByte;
			} else {
				nextByteIndex = bcu->maxTelegramSize() -1;
				rx_error |= RX_LENGHT_ERROR;
			}

			if (!parity) rx_error |= RX_PARITY_ERROR;
			valid &= parity;
			tb_h( RECV_BITS_OF_BYTE +300, currentByte, tb_in);

			//wait for the next byte's start bit or end of telegram and set timer to inter byte time + margin
			//timeout was at 11 bit times (1144us), timeout for end of telegram - no more bytes after 2bit times after
			//last stop bit: >2 bit times, << ack waiting time -> 2*BITTIME + 100us margin (308- 450 us) seems to be ok.
			//we disable reset of timer by match to have fixed ref point at end of last RX-byte,
			//timer was restarted by timeout event at end of stop bit, we just set new match value
			//next state interrupt at start bit falling edge
			state = Bus:: RECV_WAIT_FOR_STARTBIT_OR_TELEND;
			timer.match(timeChannel, MAX_INTER_CHAR_TIME);
			timer.matchMode(timeChannel, INTERRUPT);
			timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );

		}// cap event during stop bit: error, we should received byte-timeout later
		else if (time > BYTE_TIME_EXCL_STOP ) rx_error |= RX_STOPBIT_ERROR;
		//tb_h( RECV_BITS_OF_BYTE +200, rx_error, tb_in);
		break;

/*
		//We arrive here after a telegram was received and we probably need to send an ACK back to remote side or continue waiting
		//ACK tx/rx  windows starts now after the timeout event
		// enable cap event and wait for sending  ACK or receiving ack
	case Bus::RECV_WAIT_FOR_TX_ACK_WINDOW:
		tb_t( state, ttimer.value(), tb_in);

		state = Bus::RECV_WAIT_FOR_ACK_TX_START;
		timer.match(timeChannel, SEND_ACK_WAIT_TIME - PRE_SEND_TIME); // we wait 15BT for our ack tx , cap intr enabled
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT ); // we might receive an ack if the last rx-msg was a broadcast
		//todo disable cap event,   possible ack will be capture in send-startbit
		timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer at ack start bit time -PRE_SEND_TIME
		break;
*/

		//timeout: we waited 15BT - PRE_SEND_TIME after rx process, start sending an ack
		//timer was reseted by match for ref for tx process
		//if cap event, we received an early ack - continue with rx process
		//todo disable cap event in previous state - not needed during waiting for ack start
	case Bus::RECV_WAIT_FOR_ACK_TX_START:
		tb_t( state, ttimer.value(), tb_in);

		//cap event- should not happen here;  start receiving,  maybe ack or early tx from other device,
		//fixme: should not happen here, probably timing error

		if (timer.flag(captureChannel)){
			sendAck = 0;  // we stop sending an Ack to remote side and stat receiving the char
			state = Bus::INIT_RX_FOR_RECEIVING_NEW_TEL;  // init RX of new telegram
			goto STATE_SWITCH;
		}
		sendTelegramLen = 0;

#ifdef DUMP_TELEGRAMS
        telTXAck = sendAck;
        // set starttime of sending telegram
        telTXStartTime = ttimer.value() + PRE_SEND_TIME;
#endif

		//set timer for TX process: init PWM pulse generation, interrupt at pulse end and cap event (pulse start)
		timer.match(pwmChannel, PRE_SEND_TIME); // waiting time till start of first bit- falling edge 104us + n*104us ( n=0 or3)
		timer.match(timeChannel, PRE_SEND_TIME  + BIT_PULSE_TIME); // end of bit pulse 35us later
		timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer after bit pulse end
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		nextByteIndex = 0;
		tx_error = TX_OK;
		state = Bus::SEND_START_BIT;

		break;


		/*
		 * ************Sending states**************
		 * To allow for the use of the timer PWM generator, the timing of the sending process is in phase shift of -69us with respect to
		 * normal bit start: The PWM pulse starts at -69us, high pulse phase starts at 0us, pulse high end at 35us ->period is 104us. In order to be
		 * in sync with the bus after a complete telegram is send we need to correct the timing again by the phase shift.
		 *
		 * All bus wait time before a TX could start are therefore reduced by PRE_SEND_TIME (104us). This allows to set the PWM and timer match
		 * to begin the start bit of first byte  in PRE_SEND_TIME and check for any bus activity before the edge of the our start bit is received
		 * if any other device did send a start bit before us (bus busy window before start bit).
		 *
		 *
		 ***/

		/*  WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE
		 * is entered by match interrupt some usec (PRE_SEND_TIME or 1us coming from idle state) before sending the start bit of the first byte
		 * of a pending telegram. It is always entered after receiving or sending is done and we waited the respective time for next action. If no tel
		 * is pending, we enter idle state. Any new sending or receiving process will be started there. If there is a tel pending, we check for prio
		 *  and start sending after pre-send-time of 104us or if we have a normal Telegram after pre-send-time + 3*BitTime,
		 *  resulting in 50/53BT between telegrams
		 *
		 *   State should be entered with timer reset by match to have valid ref point for tx cap event with timer still running
		 **/

	case Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE:
		tb_t( state, ttimer.value(), tb_in);
		D(digitalWrite(PIO1_5, 1)); // yellow: prepare transmission

		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT ); // enable cap event after waiting time for next rx

		if (timer.flag(captureChannel)){ // cap event- start receiving,  maybe ack or early tx from other device - fixme: should not happen here!
			state = Bus::INIT_RX_FOR_RECEIVING_NEW_TEL;
			goto STATE_SWITCH;
		}

		// timeout -  check if there is anything to send
		// check if we have max resend for last telegram.
			if (sendTries >= sendTriesMax || sendBusyTries >= sendBusyTriesMax)
			{
				tb_h( state+ 100,sendTries + 10* sendBusyTries, tb_in);
				tx_error |= TX_RETRY_ERROR;
				sendNextTelegram();	// then send next  todo: info upper layer on sending error of last telegram
			}
			if (sendCurTelegram)  // Send a telegram pending?
			{		//tb_t( state+200, ttimer.value(), tb_in);
				tb_h( state+ 200, repeatTelegram, tb_in);
				//tb_h( state+ 300,sendCurTelegram[0], tb_in);

				sendTelegramLen = telegramSize(sendCurTelegram) + 1;
				//tb_h( state+ 1000,sendTelegramLen, tb_in);

				if (repeatTelegram && (sendCurTelegram[0] & SB_TEL_REPEAT_FLAG) )
				{// If it is the first repeat, then mark the telegram as being repeated and correct the checksum
					tb_d( state+ 700, sendTries, tb_in);
					tb_d( state+ 800, sendBusyTries, tb_in);
					sendCurTelegram[0] &= ~SB_TEL_REPEAT_FLAG;
					sendCurTelegram[sendTelegramLen - 1] ^= SB_TEL_REPEAT_FLAG;
				}
				// if we have repetition of telegram or system or alarm prio, we wait only 50bit time
				if (((sendCurTelegram[0] & SB_TEL_REPEAT_FLAG)) && ((sendCurTelegram[0] & PRIO_FLAG_HIGH)) ) {
					time = PRE_SEND_TIME + 3 * BIT_TIME;
				}
				else time = PRE_SEND_TIME;
				//tb_d( state+ 300, time, tb_in);
				//tb_h( state+ 400,sendCurTelegram[0], tb_in);
			}
			else  // Send nothing:  wait PRE_SEND_TIME before we set the bus to idle state
			{
			    DB_BUS(
				   if (sendCurTelegram != 0)
				   {
				       tb_h( state+ 900,sendCurTelegram[0], tb_in);
				   }
				   //tb_t( state*100+4, ttimer.value(), tb_in);
				);

//				timer.match(timeChannel, PRE_SEND_TIME); // end of 50 bit waiting for idle
//				state = Bus::INIT; // we use init state for the timeout to set the bus to IDLE state
//				break;
				idleState();
				break;
			}

		tb_t( state+500, ttimer.value(), tb_in);
		tb_d( state+600, time, tb_in);
		// set timer for TX process: init PWM pulse generation, interrupt at pulse end and cap event (pulse start)
		timer.match(pwmChannel, time); // waiting time till start of first bit- falling edge 104us + n*104us ( n=0 or3)
		timer.match(timeChannel, time + BIT_PULSE_TIME); // end of bit pulse 35us later
		timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer after bit pulse end
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		nextByteIndex = 0;
		tx_error = TX_OK;
		state = Bus::SEND_START_BIT;

#ifdef DUMP_TELEGRAMS
			// set starttime of sending telegram
			telTXStartTime = ttimer.value() + time;
#endif

		break;


		/* SEND_START_BIT
		 * start bit edge is in sync with bus timing!
		 * The start bit of the first byte is being sent. We should come here when the edge of the start bit is captured by bus-in of the pwmChannel.
		 * We started the timer PRE_SEND_TIME before the start bit is send by the PWM, we might come here when somebody else started
		 * sending before us, or if a timeout occurred. In case of a timeout, we have a hardware problem as receiving our sent signal does not work.
		 * For start of normal frames we check for bus free in a window  < -7us of before the start bit is send.  If bus is busy (capture received
		 * in window), we stop sending and start receiving instead. For sending of ACK, bus free detection  is optional (windows < - 16us before
		 * start bit) as other devices probably responding with ack as well (defined in Vol8.2.2).
		 **/
	case Bus::SEND_START_BIT:
		//tb_d( SEND_START_BIT+100, timer.match (pwmChannel), tb_in);
		//tb_h(SEND_START_BIT+200, timer.captureMode(captureChannel),  tb_in);

		if (timer.flag(captureChannel))
		{
			// Bus busy check: Abort sending if we receive a start bit early enough to abort.
			// We will receive our own start bit here too.
			if ((( timer.capture(captureChannel)< timer.match(pwmChannel) - BUS_BUSY_DETECTION_FRAME)&& !sendAck )||
					((timer.capture(captureChannel) < timer.match(pwmChannel) - BUS_BUSY_DETECTION_ACK) && sendAck)) // optional
			{
				// received edge of bit before our own bit was triggered - stop sending process and go to receiving process
				tb_d( state+300, ttimer.value(), tb_in);
				timer.match(pwmChannel, 0xffff); // stop our bit set pwm output to low

				/* set up of timer for RX  is done in RECV_WAIT_FOR_STARTBIT_OR_TELEND state, skip here
				// correct the timer start value by the process time (about 13us) we had since the capture event
				tv=timer.value(); cv= timer.capture(captureChannel);
				if ( tv > cv ) dt= tv - cv; // check for timer overflow since cap event
				else dt = (0xffff-cv) +tv;
				timer.reset();  // reset timer and pre-load with processing time
				timer.value(dt);
				timer.matchMode(timeChannel, INTERRUPT | RESET);
				timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT); // next state interrupt at start bit  - falling edge, no reset fixme should be bit0 as we received startbit here
				timer.match(timeChannel, BYTE_TIME_INCL_STOP);
				//timer.counterMode(DISABLE,  DISABLE); // disabled the  timer reset by the falling edge of cap event
				*/
//				state = Bus:: RECV_WAIT_FOR_STARTBIT_OR_TELEND; // start reception of byte
				state = Bus::INIT_RX_FOR_RECEIVING_NEW_TEL;  // init RX for reception of telegram
				goto STATE_SWITCH;
			}
			tb_t( state+400, ttimer.value(), tb_in);
			state = Bus::SEND_BIT_0; //  we received our start bit edge in time, prepare for to send bit 0
			break;

		}  else if (timer.flag(timeChannel)){
			// Timeout: we have a hardware problem as receiving our sent signal does not work. set error and just continue sending bit0
			tb_t( state+400, ttimer.value(), tb_in);
			D(digitalWrite(PIO2_8, 1));  // blue: sending bits does not work
			state = Bus::SEND_BIT_0; //   prepare for to send bit 0
			tx_error |= TX_PWM_STARTBIT_ERROR;
		}// no break, continue with bit0 as we have a timeout here


		/* SEND_BIT_0
		 *  state is in phase shift with respect to bus timing, entered by match/period interrupt from pwm
		 *  start bit low pulse end now after 35us by time match interrupt, we are in the middle of the start bit at rising edge of start
		 *  bit pulse. Prepare for sending bits of frame.
		 *
		 **/
	case Bus::SEND_BIT_0:
		//tb_d( state+100, timer.match (pwmChannel), tb_in);
		// get byte to send
		if (sendAck)
		{
			currentByte = sendAck;
		}
		else
		{
		    currentByte = sendCurTelegram[nextByteIndex++];
		}

		// Calculate the parity bit
		for (bitMask = 1; bitMask < 0x100; bitMask <<= 1)
		{
			if (currentByte & bitMask)  // current bit high
				currentByte ^= 0x100;  // toggle/xor parity bit
		}
		bitMask = 1;
		state = Bus::SEND_BITS_OF_BYTE; //set next state, no break here, continue sending first bit/ LSB
		tb_h( SEND_BIT_0 +200, currentByte, tb_in);

		/* SEND_BITS_OF_BYTE
		 * state is in phase shift,  entered by match/period interrupt from pwm
		 * n-bit low pulse end now after 35us by time match interrupt, send next bit of byte till end of byte (stop bit)
		 * **/
	case Bus::SEND_BITS_OF_BYTE:
		tb_t( SEND_BITS_OF_BYTE, ttimer.value(), tb_in);
		D(digitalWrite(PIO1_5, 1));    // yellow: send next bits
		//tb_h( SEND_BITS_OF_BYTE+100, bitMask, tb_in);
		//tb_d( SEND_BITS_OF_BYTE+200, time, tb_in);
		//tb_h( state+100, sendAck, tb_in);

		// Search for the next zero bit and count the one bits for the wait time only till we reach the parity bit
		// next bit after parity will be low in telegram-byte.  for stop bit and 2 waiting bits the bus will be high, no need to trigger in between
		time = BIT_TIME ;
		while ((currentByte & bitMask) && bitMask <= 0x100)
		{
			bitMask <<= 1;
			time += BIT_TIME;
		}
		bitMask <<= 1; // next low bit or stop bit if mask > 0x200

		if (time > BIT_TIME ) //  low bit or high bit to send?
			state = Bus::SEND_WAIT_FOR_HIGH_BIT_END; // high bit to send, detect collisions while sending high bits
		//tb_h( SEND_BITS_OF_BYTE+300, bitMask, tb_in);
		//tb_d( SEND_BITS_OF_BYTE+400, time, tb_in);

		if (bitMask > 0x200) // stop bit reached
		{
			if (nextByteIndex < sendTelegramLen && !sendAck) // if tel. end or ack, stop sending
			{//  we are at the rising edge of parity bit-> need to send 4 bit:3 high bits (stop bit, 2 fill bits) and start bit pulse of next byte by pwm
				time += BIT_TIME *3;
				state = Bus::SEND_BIT_0;  // state for bit-0 of next byte to send
			}
			else
			{//  need to send stop bit and sync with bus timing
				state = Bus::SEND_END_OF_TX;
				time += BIT_TIME  - BIT_PULSE_TIME;
			}
		}
		// set next match/interrupt
		// if we are sending high bits, we wait for next low bit edge by cap interrupt which might be a collision
		// or timeout interrupt indicating end of bit pulse sending (high or low)
		if (state == Bus::SEND_WAIT_FOR_HIGH_BIT_END)
			timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT);
		else timer.captureMode(captureChannel, FALLING_EDGE);

		if (state == Bus::SEND_END_OF_TX)
			timer.match(pwmChannel, 0xffff); //stop pwm pulses - low output
		// as we are at the raising edge of the last pulse, the next falling edge will be n*104 - 35us (min69us) away
		else timer.match(pwmChannel, time - BIT_PULSE_TIME); // start of pulse for next low bit - falling edge on bus will not trigger cap interrupt
		//tb_d( SEND_BITS_OF_BYTE+500, time, tb_in);
		//tb_h( SEND_BITS_OF_BYTE+600, timer.captureMode(captureChannel),  tb_in);

		timer.match(timeChannel, time); // interrupt at end of low/high bit pulse - next raising edge or after stop bit + 2 wait bits
		break;


		/* SEND_WAIT_FOR_HIGH_BIT_END
		 * state is in phase shift,  entered by  cap event or match/period interrupt from pwm
		 * Wait for a capture event from bus-in. This should be from us sending a zero bit, but it might as well be from somebody else in case of a
		 * collision. Our low bit starts at pwmChannel time and ends at match of timeChannel.
		 * Check for collision during sending of high bits. As our timing is related to the rising edge of a bit we need to measure accordingly:
		 * next bit start window is in 69us, and the n-bit low pulse starts at n*104 - 35us and ends at n*104 -> check for edge window: the high phase
		 * of the last bit : 69us - margin till 69us before next falling edge at pwmChannel time + margin
		 * Timeout event indicated a bus timing error
		 **/
	case Bus::SEND_WAIT_FOR_HIGH_BIT_END:
		//tb_t( state, ttimer.value(), tb_in);
		//tb_d( state+100,timer.match(pwmChannel), tb_in);
		//tb_h( SEND_WAIT_FOR_HIGH_BIT_END+200, timer.captureMode(captureChannel),  tb_in);
		//tb_h( state+100, sendAck, tb_in);

		if (timer.flag(captureChannel)){
			if (( timer.capture(captureChannel) < timer.match(pwmChannel) - BIT_WAIT_TIME +30 ) &&  // add bit margin: early 7us, late 30us
					(timer.capture(captureChannel) > BIT_WAIT_TIME - 7))  // subtract 7 margin for early bit
			{
				tb_d( state+400,timer.capture(captureChannel), tb_in);
				tb_t( state+300, ttimer.value(), tb_in);
				// A collision. Stop sending and ignore the current transmission.
				D(digitalWrite(PIO1_4, 1));  // purple
				timer.match(pwmChannel, 0xffff); // set PWM bit to low next interrupt is on timeChannel match (value :time)
				timer.match(timeChannel, BYTE_TIME_INCL_STOP); // todo we could set the timeout to the time value for remaining bits to receive
				state = Bus::RECV_BITS_OF_BYTE; // -  try to receive remaining bits of sender - will be discard anyhow, just to be in sync again
				collision = true;
				tx_error |= TX_COLLISION_ERROR;
				break;

			}
			//tb_t( state+200, ttimer.value(), tb_in);
			//tb_d( state+500,timer.match(pwmChannel), tb_in);
			// we captured our sending low bit edge, continue sending, wait for bit ends with match intr
			state = Bus::SEND_BITS_OF_BYTE;;
			break;
		}

		// timeout event, must be an error
		state = Bus::SEND_BITS_OF_BYTE;
		tx_error |= TX_TIMING_ERROR;
		goto STATE_SWITCH;
		break;

		//state is in sync with resp. to bus timing,  entered by match interrupt after last bytes stop bit was send
		//for normal frames we should wait for ack from remote layer2 after ack-waiting time or if we sent an ACK we wait 50bittimnes for idle
		//timer was reset by match
	case Bus::SEND_END_OF_TX:
		tb_t( state, ttimer.value(), tb_in);
		tb_h( SEND_END_OF_TX+700, repeatTelegram, tb_in);
		D(digitalWrite(PIO2_9, 1));
#ifdef DUMP_TELEGRAMS
		telTXEndTime= ttimer.value();
#endif

		if (sendAck){ // we send an ack for last received frame, wait for idle for next action
			tb_h( SEND_END_OF_TX+200, tx_error, tb_in);
			DB_TELEGRAM(
                txtelBuffer[0] = sendAck;
                txtelLength = 1;
                tx_rep_count = sendTries;
                tx_busy_rep_count = sendBusyTries;
                tx_telrxerror = tx_error;
		    );

		    sendAck = 0;
			state= Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE;
			time =  SEND_WAIT_TIME - PRE_SEND_TIME;// we wait 50 BT- pre-send-time for next rx/tx window, cap intr disabled
			// todo inform receiving process of pos ack tx
		}else
		{
			tb_h( SEND_END_OF_TX+600, repeatTelegram, tb_in);

			// normal data frame,  L2 need to wait for ACK from remote for our telegram
			//if (!(sendCurTelegram[0] & SB_TEL_ACK_REQ_FLAG)) {
			wait_for_ack_from_remote = true; // default for data layer: acknowledge each telegram
			time=  ACK_WAIT_TIME_MIN; //we wait 15BT- marging for ack rx window, cap intr disabled
			state = Bus::SEND_WAIT_FOR_RX_ACK_WINDOW;
			timer.matchMode(timeChannel, INTERRUPT); // no timer reset after timeout
			if (repeatTelegram) // if last telegram was repeated, increase respective counter
			{
				if (busy_wait_from_remote)
					sendBusyTries++;
				else
					sendTries++;
			}
			//}else wait_for_ack_from_remote = false;
			// dump previous tx-telegram and repeat counter and busy retry
			DB_TELEGRAM(
			    for (int i =0; i< sendTelegramLen; i++)
                {
                    txtelBuffer[i] = sendCurTelegram[i];
                }
                txtelLength = sendTelegramLen;
                tx_rep_count = sendTries;
                tx_busy_rep_count = sendBusyTries;
                tx_telrxerror = tx_error;
			);
		}
		tb_d( SEND_END_OF_TX+300, wait_for_ack_from_remote , tb_in);
		tb_d( SEND_END_OF_TX+400, sendTries , tb_in);
		tb_d(SEND_END_OF_TX+500, sendBusyTries , tb_in);

		timer.match(timeChannel, time); // we wait respective time - pre-send-time for next rx/tx window, cap intr disabled
		break;

		//ACK receive windows starts now after the timeout event
		//enable cap event and wait till end of ACK receive window for the ACK
		//timer is counting since end of last stop bit
	case Bus::SEND_WAIT_FOR_RX_ACK_WINDOW:
		tb_t( state, ttimer.value(), tb_in);

		state = Bus::SEND_WAIT_FOR_RX_ACK;
		//timer.matchMode(timeChannel, INTERRUPT | RESET); // timer reset after timeout to have ref point in next RX/TX state
		//timer.counterMode(DISABLE,  captureChannel  | FALLING_EDGE); // enabled the  timer reset by the falling edge of cap event
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		timer.match(timeChannel, ACK_WAIT_TIME_MAX); // we wait 15BT+ marging for ack rx window, cap intr enabled
		break;

		// we wait here for the cap event of the ACK. If we receive a timeout- no ack was received and we need
		// to start a repetition of the last telegram
	case Bus::SEND_WAIT_FOR_RX_ACK:
		tb_t( state, ttimer.value(), tb_in);

		if (timer.flag(captureChannel)){
			state = Bus::INIT_RX_FOR_RECEIVING_NEW_TEL;  // start bit of ack received - continue rx process for rest of byte
			// todo rx-ack process ongoing inform to RX process for optimization??
			goto STATE_SWITCH;
		}
		repeatTelegram = true;
		tx_error|= TX_ACK_TIMEOUT_ERROR; // todo  ack timeout - inform upper layer on error state and repeat tx if needed
		state = Bus::WAIT_50BT_FOR_NEXT_RX_OR_PENDING_TX_OR_IDLE;

		//timer is counting since last stop bit so we need to add the ACK_WAIT_TIME and the char-tx time of 11bits to the 50BT till idle for repated frames
		timer.match(timeChannel, ACK_WAIT_TIME + BYTE_TIME_INCL_STOP + SEND_WAIT_TIME - PRE_SEND_TIME);

		timer.captureMode(captureChannel, FALLING_EDGE| INTERRUPT  );// todo disable cap int during wait to increase the robustness on bus spikes
		timer.matchMode(timeChannel, INTERRUPT | RESET); // timer reset after timeout to have ref point in next RX/TX state
		break;

	default:
		idleState();
		tb_d( 9999, ttimer.value(), tb_in);
		break;
	}

	timer.resetFlags();
}

void Bus::loop()
{
    ///\todo implement enabled property
    /*
    if (!enabled)
    {
        return;
    }
    */
    DB_TELEGRAM(dumpTelegrams());
#if defined (DEBUG_BUS) || defined (DEBUG_BUS_BITLEVEL)
    debugBus();
#endif
}
