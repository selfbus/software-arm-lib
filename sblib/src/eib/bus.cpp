/**
 *  bus.cpp - Low level EIB bus access.
 *
 *  Copyright (c) 2014 Stefan Taferner <stefan.taferner@gmx.at>
 *
 * last changes: Jan 2021 by Horst Rauch
 * 				- added some comments
 * 				- fixed collision detection issue while sending ACK/NACK/BUSY
 * 				- introduction of collision detection window according to spec for pulse duration of 25us - 70us due to propagation delay
 * 				- added default phy-adr for normal devices and router (compilation with ROUTER defined)
 * 				- added some states to SM for better readability of the code, Busy handling,...
 * 				- added error handling, convey info to upper layers
 * 				- added local rx buffer for parallel processing of rx telegram and receiving and reply with busy to remote sender
 * 				- added busy acknowledgment handling
 * 				- added repeated telegram reception handling
 * 				- added some debug macros, use of timer32_0 for debugging of state machine timing
 * 				- debugging  with high speed serial terminal interface (1.5Mbaud) at PIO2_7 and PIO2_8
 *
 * Functions supported by this module:
 *   - Supported frames:  standard data frame, acknowledge frame. Poll frame  and extended  frame are  not supported
 *   - Bus-Busy detection for start of normal frames and ack-frames
 *   - Collision detection for all send bytes, no support for receiving of remaining telegram after collision, rx telegram is discarded.
 *   - Extended collision detection for Ack frame due to possible parallel sending of devices and bus delay
 *   - repetition of telegram after NACK, no Ack or busy-ack after idle time (50bit) or busy wait (150bit)
 *   - sending busy ack to remote if rx-buffer is not free (higher layer still processing last telegram)
 *   - support of ETS provided repeat value for normal repeat and busy repeat  (loaded from EPROM)
 *   - support of default phy addr (15.15.255) for normal device in case of uninitialized phy. addr. 0.0.0 from EPROM, 0.0.0 is
 *     only allowed for a Router device
 *   -
 *
 * !!!!The used timer should have the highest priority of the used interrupts in the lib and the app!!!!
 *
 * As the prio is set at reset to the highest level for all peripheral interrupt sources, any used peripheral interrupt should be set to
 * a lower level by a call of the CMSIS "void NVIC_SetPriority (IRQn_t IRQn, uint32_t priority)" function with respective IRQn
 * and a prio >0 (0 is the highest level for periph.).
 *
 * Suport for debugging with some macros and dumping of data on the serial line - ports will be mapped in the serial.cpp module
 * and serial support need to be started with a high baud rate (recommended 1.5Mb) in the app module. Usage of timer32_0 for time
 * measurement on bit/byte level
 *
 ** Compile time switches:
 *  ROUTER :  			Lib will be compiled for usage in a router- Phy Addr 0.0.0 is allowed
 *  DEBUG_BUS:			enable dumping of state machine interrupt data e.g timer values, mapping of ports in serial.cpp
 *  DEBUG_BUS_BITLEVEL	extension used with DEBUG_BUS to dump interrupt of each bit - use with care due to easy overflow
 *  					of the trace buffer
 *  DUMP_TELEGRAMS		dump received and sending telegram data
 *
 *
 * Interface to upper layers (see bus.h for details)
 *
 *  For interfacing to higher layers there are data buffer for sending and receiving of telegrams:
 *   tx: bcu.sendTelegram[]  - send buffer
 *   	 sendTelegram(char* telegram,..)  - send a new telegram (pointer to tel to be sent),
 *   	                                    is blocking if no buffer is available (current transmit ongoing).
 *       sendTelegramState() - state of the last send process
 *       sendingTelegram()   - test if there is a tel being sent
 *
 *   rx: bus.telegram[]          - receive buffer
 *       telegramLen             - indicating the number of bytes received in bus.telegram[], telegramLen ==0 indicates an empty buffer,
 *                                 should be set to 0 by the higher layer after telegram was processed, set to the number of rx bytes by the rx process
 *       receivedTelegramState() - state of th last rx process
 *       telegramReceived()      - test if there is a tel waiting in the buffer
 * *
 *
 * Bus access module initializing:
 * Load Phy. Addr. from eprom- if 0.0.0 set to default 15.15.254 if not compiled as router, initialize the timer,
 * tx and rx port and wait for 50 bit times before entering the IDLE sate.
 *
 *  todo load retry and busy-retry from eprom
 *//*
 *
 * The timer16_x is used as follows:
 *
 * Capture register CR0 is used for receiving
 * Match register MR0 or MR1 is used as PWM for sending, depending on which output pin is used
 * Match register MR3 is used for timeouts while sending / receiving
 * depending on state: cap event and/or timeout event are used to trigger interrupt, match to reset the timer.
 * todo- Use of cap event to reset timer in order to have a more precise timing related to the start bit.
 *
 * For receiving: timer is restarted with capture event at falling edge of start-bit, loaded with character time (11 bits) for timeout
 * and reset with timeout after stop-bit.  After send of character, timer is running to measure the next events: eg. next
 * character (after 208us), end of telegram ( after 308us), send of acknowledge  1560us -pre-send-time(104us ->1456us)
 * next state either tx or wait for timeout at idle time (50bit) - pre-send-time (5096us) for next action.
 * No pending action:    continue waiting in idle state for cap event: rx start, or timeout ( set in send telegram): tx start .
 *
 * For transmitting:
 *  Sending of char is using the PWM function in a tricky way. As we need a positive pulse for 35us as first part of the bit
 *  and 69us low pulse as second part of bit we need to shift the phase by 69us relative to normal bit timing on the bus:
 *  -> the  sending is split in the two half bit:  high phase of bit n-1 followed by low phase of bit n. In addition the falling edge
 *  of the 35us pulse is triggering the capture event for the start bit or any low bit following a high bit.
 *
 *  Timer is started  pre-send (104us) time before the falling edge of start bit, start-bit is generated by PWM pulse
 * set for 104us, end of pulse 104us+ 35us pulse time. We are now at the rising edge of the start bit,  following bits by PWM set
 * for 69us low phase, 35us high terminated by pwm period of 104us till end of byte incl parity and stop. After stop-bit, timer is
 * running to measure next time out of 2 filling bits (208us) before next character is send or waiting for ack to be received or
 * start of next action in 50 bit times.
 * For rx of ack, the ack-window start is at 15bit times -5us -margin of 100us,  waiting for ack window ends
 * at 15us +30us+margin of 100us.
 *
 *
 * Telegram/byte/bit sending principle:
 *
 *  The sending is started from higher layers by a call to sendTelegram with a pointer to the tel-buffer and the tel-lenght.
 *  We add our phy addr. and checksum set the match intr, timeout in 1us and set WAIT_FOR_NEXT_RX_OR_PENDING_TX state,
 *  initialize  some parameters (sendTries=0...) for  the telegram. In WAIT_FOR_NEXT_RX_OR_PENDING_TX we check for ACK to be sent
 *  or a necessary repetition of the telegram and set the pre-send waiting time accordingly. A cap event is indicating the reception of the
 *  start bit from us or any other device. We check the receive window (for an ack this is optional with a longer window) if the trigger is not
 *  from our startbit falling edge. If not we continue with the rx process in RECV_WAIT_FOR_STARTBIT_OR_TELEND, else
 *  we wait for a timeout at the rising edge of the start bit and continue with bit0 in SEND_BIT_0 followed by SEND_BITS_OF_BYTE for sending
 *  remaining bits. When we reached the stop bit to send we check for more character to be send -> then we continue after additional
 *  wait time of 3 high bits (stop + 2 filling bits) and start bit with the sending  in SEND_BIT_0. If we reached end of telegram, we send/wait only
 *  for the stop bit  and align the phase shift (-69us) to be in sync with normal bus timing in SEND_END_OF_TX. Next we wait for the ack window
 *  to start in SEND_WAIT_FOR_RX_ACK_WINDOW, with the timeout, we enable cap event intr again and wait for the ACK to be received till end
 *  of ack window in SEND_WAIT_FOR_RX_ACK.  At timeout we need to repeat the telegram and wait in WAIT_FOR_IDLE50 or we continue
 *  receiving the ack with a cap event in WAIT_FOR_NEXT_RX_OR_PENDING_TX. If we receive a positve ack we prepare for next
 *  telegram to send and wait 50 bit times in WAIT_FOR_IDLE50 WAIT. If we received a BUSY from remote, we wait 150 bit time in
 *  WAIT_FOR_IDLE50 WAIT the before we repeat the telegram in WAIT _FOR_NEXT_RX_OR_PENDING_TX.
 *  The end of transmission is indicated to higher layer by the bus_tx_state parameter, which is set after a tx process in send next tel
 *
 *
 * Telegram/byte/bit  high level receiving principle:
 *  A falling edge of a start bit is triggering the capture interrupt. For the first bit of a new telegram, the interrupt is
 *  received in the WAIT_FOR_NEXT_RX_OR_PENDING_TX or IDLE state and all parameter for a new telegram are initialized.
 *  The timer is restarted/re-loaded with the cap and match is to the character length of 11bits.
 *  Bits are received and after the stop bit (timeout) the timer is restarted and match set for next start bit in 2 bit times+margin.
 *  For intermediate bytes,  the cap. interrupt for the first bit of a new byte is received in state  RECV_WAIT_FOR_STARTBIT_OR_TELEND.
 *  If we receive a time out instead, we have the end of the telegram reached (>2bit times after last stop bit) and process it.
 *  If the sending side is waiting for an ack we process the received data as ack or as normal telegram at the data layer.
 *  At the data layer, we check for correct telegram (parity, checksum, length), if it is for us, data is moved to the tx buffer of higher layers and
 *  possible transmission of an ack is prepared and waited to send the ack or waited for next rx or pending tx.
 *
 *  If it is a cap event for the start bit, we initialize the timer- match intr, cap intr, restart, preset with time elapsed
 *  since cap intr. to relate all timings to the start bit falling edge on the bus, set match value to byte time (11 bit: start,
 *  8data, parity, stop:1144us). Next state is RECV_BITS_OF_BYTE to collect all data bits of the character incl parity and stop.
 *  If we receive a timeout in this state, the character ( 11 bits) is completely collected, we set the match to wait the 2 bits
 *  ( incl some margin) between two characters and set next state to  RECV_WAIT_FOR_STARTBIT_OR_TELEND.  A cap. event  is triggering
 *   the reception of a new character and the bits will be collected in RECV_BITS_OF_BYTE. A timeout is indicating the end of the
 *  telegram and we check the received data. After checking of the data, we  typically wait for the window to send an ACK back to the remote
 *  sender in RECV_WAIT_FOR_TX_ACK_WINDOW. The window ends PRE_SEND_TIME before we possibly need to ack the telegram in
 *  RECV_WAIT_FOR_ACK_TX_START,  cap event is re-enabled and sending prepared, or  if no reply needed, we wait 50 bit times in
 *  WAIT_FOR_IDLE50, cap event is re-enabled and enter next state  WAIT_FOR_NEXT_RX_OR_PENDING_TX.
 *
 * The cap. interrupt is disabled during the waiting period between the last action of RX/TX and the start of the WAIT_FOR_IDLE50 state
 * to improve  for bus noise handling.
 * For the rx process a local rx buffer is used to allow for data processing on higher layer of previous buffer and parallel interrupt driven rx of
 * a new telegram. If the processing of the rx buffer at higher layer is not completed when a new telegram was received, a BUSY is send
 * back to the sender as acknowledge and we wait 150 bit times for the repeated telegram. todo we could disable any cap event during
 * waiting.
 *
 *
 * *************** Main requirements based on knx spec****************
 * Physical Layer
 *
 *  Bit decoding timing:				min				typ.			max
 *   Bit time  										104us
 *   bit pulse duration 	 			25us				35us			70us	   due to overlay of same pulse and propagation delay
 *
 *   acceptance window for leading
 *  edge relative to start bit			n*104us-7us		n*104us		n*104us+33us
 *
 *  time distance from start bit to
 *   start bit of next char				13*104us-30us	13*104us		13*104us+30us 	:1322us, 1352us, 1382us
 *
 *
 *  Bit coding timing:				min				typ.			max
 *   Bit time  										104us
 *   bit pulse duration 		 		34us				35us			36us
 *
 *   time from start bit to following bits
 *   within single char				n*104us-2us		n*104us		n*104us+2us
 *
 *  time distance from start bit to	start bit
 *   of next char					13*104us-2us		13*104us		13*104us+5us  : 1350us, 1352us, 1357us
 *
 *	time for bus idle(no traffic on bus)			>53* 104us						: 5512us
 *
 *
 * Character/telegram (frame) timing
 *  (without poll frame timings):
 *   time between 2 normal frames:	53*104us -50us	>=53 * 104us			--			: >=5512us
 *
 *   time between telegram and
 *    repeated telegram:				50*104us -50us	50 * 104us			53*104 -50--		: 5200us
 *
 *   time between end of telegram
 *    and start of ACK/NACK/BUSY:
 *    		for bit coding:				15*104us-5us		15*104us		15*104us+20us	: 1560us
 *    		for bit decoding:			15*104us-5us		15*104us		15*104us+30us	: 1555us, 1560us, 1590us
 *
 *   timeout between end of frame
 *    and wait for ACK/NACK/BUSY:						>=30u+15*104us				: >=1590us
 *
 *   time between end of ACK/NACK/BUSY
 *    and begin of next telegram:					>=50 * 104us					: >=5200us
 *
 *   time between two characters within a telegram  >=2*104us <= 2,5*104us			: 208us <= t <= 260us
 *
 *
 *  Line BUSY detection:
 *   Immediately before transmission of the start bit of a first character of a frame the bus is check   if any other device is already transmitting.
 *   This must be done for start of normal and repeated frame (and fill characters in poll situations - not used).
 *   In case the bus is busy no transmission is started and line_busy is indicated to upper layers.
 *   For the transmission of an ACK/NACK/BUSY and inner frame characters line-busy detection is optional
 *  (is implemented  for Ack/Nack/busy with extended detection window before the start bit).
 *  In case of line-busy, no transmission should be started. Line busy window end 7us before  falling edge of start bit of normal frames
 *  and 16us for ack frames. Any reception after that window will be detected as possible collision.
 *
 *  Collision detection:
 *   Shall be never disabled during transmission ( disable for sent of ACK is an option).
 *   In cased of a collision, the transmission should be stop immediately and indicated to the uppler layer functions.
 *   A collision is detected if a logical 1 (high level) was sent but at the same time a low level
 *   pulse was detected.
 * *
 * Data Link Layer transmit
 *  Typical frames of a device will be acknowledged by the remote receiver with ACK/NACK/BUSY. If the acknowledgment
 *  is positive, the ACK should be indicated to upper layers. In case of BUSY, the frame should be resent after waiting
 *  at least 150 bit times line idle. Resent should be done for max busy_retry times. If BUSY is received for more than
 *  busy_retry times, the upper layer should be informed with transmission status not_ok/error.
 *  In all other cases (NACK, corrupted frame or time out after 30 bit times..) the frame shall be repeated for nak_retry
 *  times after 50 bit times waiting. The status should be indicated to the upper layers.
 *
 * Data Link Layer receive
 *  A received frame should be processed only if it is addressed to the own phy. Address or Group Addr. found in the addr. table
 *   or (broadcast) Group Addr. zero  (not appl. to BUS Monitor etc). If the frame is for us and:
 *   -If the received frame is not correct (parity/checksum,length...error) a NACK should be sent to remote device ???Dest
 *    addr could be wrong???.
 *   -If frame is correct but the device is busy (e.g. input queue full,...) a BUSY should be sent to remote device
 *   -If frame is correct an ACK should be sent
 *
 * -> We send no acknowledge when we did receive an incorrect frame  or correct frame which was not for us
 *
 * ********************************************************************
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3 as
 *  published by the Free Software Foundation.
 *
 **/

#include <sblib/eib/bus.h>

#include <sblib/core.h>
#include <sblib/interrupt.h>
#include <sblib/platform.h>
#include <sblib/eib/addr_tables.h>
#include <sblib/eib/user_memory.h>
#include <sblib/eib/properties.h>

#include <sblib/timer.h>


// Enable informational debug statements
#if defined (DEBUG_BUS) || defined (DEBUG_BUS_BITLEVEL) || defined (DUMP_TELEGRAMS)
#define DB(x) x
int hw_error =0;
#else
#define DB(x)
#endif

// pin output macros for debugging with e.g logic analyser - not used
#define D(x)

// buffer to dump a rx telgramn on the serial line incl collision and rx-timing (in ms) info
#ifdef DUMP_TELEGRAMS
volatile unsigned char telBuffer[32];
volatile unsigned int telLength = 0;
volatile unsigned int telRXtime = 0;
volatile bool telcollision;
//volatile unsigned int db_state= 2000;
#endif

// buffer to dump interrupt event data on the serial line
#if defined(DEBUG_BUS) || defined(DEBUG_BUS_BITLEVEL)
//#ifdef DEBUG_BUS
volatile unsigned int b1=0, b2=0, b3=0, b4=0;
volatile unsigned short b5=0;
volatile struct s_td td_b[tb_lngth];
volatile unsigned int tb_in = 0;
volatile unsigned int tb_out = 0;
volatile unsigned int tbd = 0;
volatile bool  tb_in_ov = false;
volatile bool  rec_end = false;


#ifdef DEBUG_BUS_BITLEVEL

//dump each interrupt with timer data
#define tbint(d1, d2, d3, d4, d5, d6,  i)  \
	td_b[i].ts=d1; td_b[i].tt=d2; td_b[i].tc=d3; td_b[i].tcv=d4; td_b[i].ttv=d5; td_b[i].ttmv=d6;\
	 if (++i>=tb_lngth){ i=0; tb_in_ov = true;}

#else
//dump on bit level only first bit interrupt to reduce OVF
#define tbint(s, t, cf, cv, tv, tmv,  i) \
	if (!( (s==8000 + RECV_BITS_OF_BYTE || s==8000 + SEND_BITS_OF_BYTE )&& !rec_end) ) {\
		if(s==800 + RECV_WAIT_FOR_STARTBIT_OR_TELEND || s==8000 + SEND_BIT_0) rec_end = true; else rec_end=false;\
		td_b[i].tt=t; td_b[i].tc=cf; td_b[i].tcv=cv; td_b[i].ttv=tv; td_b[i].ttmv=tmv; td_b[i].ts=s; \
		if (++i>=tb_lngth){ i=0; tb_in_ov = true;} }
#endif

// dump telegram data in buffer
#define tb2(s, ptc, t,  ptcv, pttv, pttmv,  i) td_b[i].ts=s; \
		td_b[i].tt=t; td_b[i].tc=ptc; td_b[i].tcv=ptcv; td_b[i].ttv=pttv; td_b[i].ttmv=pttmv;  if ( ++i>=tb_lngth){ i=0; tb_in_ov = true;}

//dump a timer value
//#define tb_t(s, t,  i ) td_b[i].ts=s+2000; td_b[i].tt=t; if (++i>=tb_lngth){ i=0; tb_in_ov = true;}
#define tb_t(s, t,  i ) ;

// dump a hex value
#define tb_h(s, t,  i ) td_b[i].ts=s+4000; td_b[i].tt=t; if (++i>=tb_lngth){ i=0; tb_in_ov = true;}
//#define tb_h(s, t,  i ) ;

// dump a decimal value
#define tb_d(s, t,  i ) td_b[i].ts=s+5000; td_b[i].tt=t;  if (++i>=tb_lngth){ i=0; tb_in_ov = true;}
//#define tb_d(s, t,  i ) ;

extern Timer timer32_0;

#else
// no dump at all
#define tb2(s, ptc, t,  ptcv, pttv, pttmv,  i);
#define tb_t(s, t,  i ) ;
#define tb_h(s, t,  i ) ;
#define tb_d(s, t,  i ) ;
#endif



/* L1/L2 msg header control field data bits meaning */
#define ALLWAYS0       	   0          //  bit 0 is always 0
#define ACK_REQ_FLAG       1          // 0 ack is requested
#define PRIO0_FLAG         2          // 00 system priority, 10 alarm prio, 01 high prio, 11 low prio
#define PRIO1_FLAG         3	//
#define ACK_FRAME_FLAG     4          // 1 frame is data or poll, 0 ack frame
#define REPEAT_FLAG        5          // 1 not repeated, 0 repeated frame
#define DATA_FRAME_FLAG    6          // 1 poll data frame, 0 data frame
#define LONG_FRAME_FLAG    7          // 1,  short frame <=15 char, 0 long frame > 15 char
#define HEADER_LENGHT      7          // eib msg header lenght by length indicator =0

// telegram control byte bit definitions
#define SB_TEL_NULL_FLAG 	( 1<< ALLWAYS0)
#define SB_TEL_ACK_REQ_FLAG 	( 1<< ACK_REQ_FLAG)
#define SB_TEL_PRIO0_FLAG   	( 1<< PRIO0_FLAG )
#define SB_TEL_PRIO1_FLAG   	( 1<< PRIO1_FLAG)
#define SB_TEL_ACK_FRAME_FLAG   ( 1<< ACK_FRAME_FLAG )
#define SB_TEL_REPEAT_FLAG 		( 1<< REPEAT_FLAG)
#define SB_TEL_DATA_FRAME_FLAG 	( 1<< DATA_FRAME_FLAG)
#define SB_TEL_LONG_FRAME_FLAG 	( 1<< LONG_FRAME_FLAG)
#define SB_TEL_PRIO_FLAG   	    ( SB_TEL_PRIO0_FLAG | SB_TEL_PRIO1_FLAG )

#define VALID_DATA_FRAME_TYPE_MASK (SB_TEL_LONG_FRAME_FLAG | SB_TEL_DATA_FRAME_FLAG | SB_TEL_ACK_FRAME_FLAG | SB_TEL_NULL_FLAG)
#define VALID_DATA_FRAME_TYPE_VALUE  (SB_TEL_LONG_FRAME_FLAG | SB_TEL_ACK_FRAME_FLAG )

#define PRIO_FLAG_HIGH	 (SB_TEL_PRIO0_FLAG)
#define PRIO_FLAG_LOW	 (SB_TEL_PRIO0_FLAG | SB_TEL_PRIO1_FLAG )


//  **************define some default values for dat/link layer based on the knx spec ***********

#define NACK_RETRY_DEFAULT 3 	// default NACK retry
#define BUSY_RETRY_DEFAULT 3 	// default BUSY retry
#define ROUTE_CNT_DEFAULT 6 	// default RouteCnt
#define PHY_ADDR_HI_DEFAULT 0xff	// sets default physical addr in userEprom at compile time to 15.15.255
#define PHY_ADDR_LO_DEFAULT 0xff	// sets default physical addr in userEprom at compile time c/f knxspec 3/05/01


//  **************define some  flags and timing values based on the knx spec ***********

// Mask for the timer flag of the capture channel
#define CAPTURE_FLAG (8 << captureChannel)

// Mask for the timer flag of the time channel
#define TIME_FLAG (8 << timeChannel)


// after TX wait for ack reception from remote side
#define RX_ACK_WAIT_TIME_MAX  15* BIT_TIME +30 // we wait 15*BT -5 min,   1560 +30us max
#define RX_ACK_WAIT_TIME_MIN  15* BIT_TIME -5 //

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

//  for rx process: Time from end of stop bit  to start of ACK = 15* bit time -5us/+30us acc to spec,
//we add add marging of 100us as we have seen some early acks on the bus
//
#define ACK_WAIT_TIME_MIN 15*BIT_TIME-5 - 100
#define ACK_WAIT_TIME_MAX 15*BIT_TIME+30 + 100

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
Bus::Bus(Timer& aTimer, int aRxPin, int aTxPin, TimerCapture aCaptureChannel, TimerMatch aPwmChannel)
:timer(aTimer)
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
 * get our own phy addr, reset operating parameters, start the timer
 * set the pwm parameter so that we have no pulse on bus and no interrupt from timer
 * activate capture interrupt, set bus pins to respective mode
 *
 * In case we are a normal device (ROUTER not defined for compilation) we check for 0.0.0 as phy addr
 * and change to default 15.15.252 if  zero is our addr
 *	//todo get defined values from usereprom for busy-retry and nack-retry
 *
 */
void Bus::begin()
{
	ownAddr = (userEeprom.addrTab[0] << 8) | userEeprom.addrTab[1];
#if BCU_TYPE != BCU1_TYPE
	if (userEeprom.loadState[OT_ADDR_TABLE] == LS_LOADING)
	{
		byte * addrTab = addrTable() + 1;
		ownAddr = (*(addrTab) << 8) | *(addrTab + 1);
	}
#endif

	//check own addr - are we a router then 0 is allowed
	//0.0.0 is not allowed for normal devices
	//set default addr  15.15.255 in case we have PhyAdr of 0.0.0

#ifndef ROUTER
	if (ownAddr == 0) ownAddr = PHY_ADDR_HI_DEFAULT <<8 | PHY_ADDR_LO_DEFAULT;
#endif

	//todo load sendtries from eprom
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
	//timer.counterMode(DISABLE,  captureChannel | FALLING_EDGE); // todo  enabled the  timer reset by the falling edge of cap event
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

#if defined (DEBUG_BUS) || defined (DEBUG_BUS_BITLEVEL) || defined (DUMP_TELEGRAMS)
	//we used 32bit timer 0 as debugging timer running with 1Mhz or system clock (48MHz)
	ttimer.begin();
	ttimer.start();
	ttimer.noInterrupts();
	ttimer.reset();
	//	ttimer.prescaler(0);
	ttimer.prescaler(TIMER_PRESCALER);

	DB(serial.print("Bus begin - Timer prescaler: ");)
	DB(serial.print((unsigned int)TIMER_PRESCALER, DEC, 6);)
	DB(serial.print(" Ttimer prescaler: ");)
	DB(serial.print(ttimer.prescaler(), DEC, 6);)
	DB(serial.print(" Ttimer valuer: ");)
	DB(serial.print((unsigned int)ttimer.value(), DEC, 6);)
	DB(serial.println("");)
	DB(serial.print("nak retries: ");)
	DB(serial.print((unsigned int) sendTriesMax, DEC, 6);)
	DB(serial.print("busy retries: ");)
	DB(serial.println((unsigned int) sendBusyTriesMax, DEC, 6);)
	DB(serial.print("phy addr: ");)
	DB(serial.println((unsigned int) ownAddr, HEX, 4);)
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



/*
 * Prepare the telegram for sending. Set the sender address to our own
 * address, and calculate the checksum of the telegram.
 * Stores the checksum at telegram[length].
 *
 * @param telegram - the telegram to process
 * @param length - the length of the telegram
 */
void Bus::prepareTelegram(unsigned char* telegram, unsigned short length) const
{
	unsigned char checksum = 0xff;
	unsigned short i;

	// Set the sender address
	telegram[1] = ownAddr >> 8;
	telegram[2] = ownAddr;

	// Calculate the checksum
	for (i = 0; i < length; ++i)
		checksum ^= telegram[i];
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

	if (!sendCurTelegram) sendCurTelegram = telegram;
	else if (!sendNextTel) sendNextTel = telegram;
	else fatalError();   // soft fault: send buffer overflow

#ifdef DUMP_TELEGRAMS
	unsigned int t;
	t= ttimer.value();
	//serial.println();
	serial.print("QSD: (");
	serial.print(t, DEC, 9);
	serial.print(") ");
	for (int i = 0; i <= length; ++i)
	{
		if (i) serial.print(" ");
		serial.print(telegram[i], HEX, 2);
	}
	serial.println();
#endif

	// Start sending if the bus is idle or sending will be triggered in WAIT_FOR_NEXT_RX_OR_PENDING_TX after finishing current TX/RX
	noInterrupts();
	if (state == IDLE)
	{
		sendTries = 0;
		sendBusyTries =0;
		repeatTelegram = false;

		wait_for_ack_from_remote = false;
		state = Bus::WAIT_FOR_NEXT_RX_OR_PENDING_TX;
		timer.match(timeChannel, 1);
		timer.matchMode(timeChannel, INTERRUPT | RESET);
		timer.value(0);
	}
	interrupts();
}

/*
 *  set the Bus SM to idle state.
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

	timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
	timer.matchMode(timeChannel, RESET); // no timeout interrupt, reset at match todo we could stop timer for power saving
	timer.match(timeChannel, 0xfffe); // stop pwm pulse generation, set output to low
	timer.match(pwmChannel, 0xffff);
	//timer.counterMode(DISABLE,  captureChannel | FALLING_EDGE); // todo enabled the  timer reset by the falling edge of cap event
	state = Bus::IDLE;
	sendAck = 0;
	//need_to_send_ack_to_remote=false;
}

/*
 * *********** Layer 2 rx/tx handling, part of the interrupt processing -> keep as short as possible**********
 *
 * We arrive here after we received  data from bus indicated by a time out of >2BT after RX of bits
 *
 * Check for valid data reception in  rx_error . If we received some bytes, process data ( valid telegram) and if tel is for us
 * check for need of sending ACK to sender side and start respective process:  time for ACK (15BT) or inter telegram wait (50BT)
 *  and set WAIT_FOR_NEXT_RX_OR_PENDING_TX state
 * If we had a collision,  no action
 *  **  End of telegram is indicated by a timeout. As the timer is still running we have the accurate time since the last stop bit.
 * *
 *  copy local rx-data to 2nd rx buffer for paralell processing on higher layer and SM

 * Provide some Data-layer functions:
 *  check telegram type
 *  check if it is for us (phy adr, or Grp Adr belong to us)
 *  check if we want it anyhow (TL, LL info)
 *  send ACK/NACK if requested
 *  send BUSYACK if we are still busy on higher layers
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
 * 	indicate result of telegram reception/error state to upper layer
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
	telRXtime= ttimer.value();
	if (nextByteIndex){
		for (int i = 0; i < nextByteIndex; ++i)
		{
			telBuffer[i] = rx_telegram[i];
		}
		telLength = nextByteIndex;
		telcollision = collision;
	}
#endif


	sendAck = 0;
	//need_to_send_ack_to_remote=false;
	int time = SEND_WAIT_TIME -  PRE_SEND_TIME; // default wait time after bus action
	state = Bus::WAIT_FOR_NEXT_RX_OR_PENDING_TX;//  default next state is wait for 50 bit times for pending tx or new  rx
	tb_h( 908, currentByte, tb_in);
	tb_h( 909, parity, tb_in);

	// Received a valid telegram with correct checksum?  todo extended tel, check tel len, give upper layer error info
	if ( nextByteIndex >= 8 && valid  &&  (( rx_telegram[0] & VALID_DATA_FRAME_TYPE_MASK) == VALID_DATA_FRAME_TYPE_VALUE)
			&& nextByteIndex< TELEGRAM_SIZE  )
	{
		int destAddr = (rx_telegram[3] << 8) | rx_telegram[4];
		bool processTel = false;

		// We ACK the telegram only if it's for us
		if (rx_telegram[5] & 0x80) // groupr addr or phy addr
		{
			if (destAddr == 0 || indexOfAddr(destAddr) >= 0)
			{
				processTel = true; // broadcast or known group addr
			}
		}else if (destAddr == ownAddr)
		{
			processTel = true;
		}

		// Only process the telegram if it is for us or if we want to get all telegrams
		if (!(userRam.status & BCU_STATUS_TL))
		{ // TL is disabled we might process the telegram
			processTel = true;
			// if LL is in normal mode (not busmonitor mode) we send ack back
			if (userRam.status & BCU_STATUS_LL)
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
				} else
				{// store data in telegram buffer for higher layers, set telegramLen to indicate data available
					for (int i =0; i <= nextByteIndex; i++) telegram[i] = rx_telegram[i];
					telegramLen = nextByteIndex;
					bus_rx_state = rx_error;
					rx_error = 0;
					lastRXTimeVal = millis();
					//check if an ACK is requested by the sender of the telegram ->  not needed by bit1=1 in the control field, or poll frame
					if (! ( (rx_telegram[0] & SB_TEL_ACK_REQ_FLAG)  ||   (rx_telegram[0] & SB_TEL_DATA_FRAME_FLAG)) )
					{
						sendAck = SB_BUS_ACK;
						//need_to_send_ack_to_remote= true;
					}
				}
				// we received a valid telegram need to send an ack/busy to remote - has priority, no rx/tx  in between
				if ( sendAck){
					state = Bus::RECV_WAIT_FOR_ACK_TX_START;
					time = SEND_ACK_WAIT_TIME - PRE_SEND_TIME;
				}
			}
		} //
	}
	else if (nextByteIndex == 1 && parity)   // Received a spike or a bus acknowledgment, only parity, no checksum
	{
		currentByte &= 0xff;
		tb_h( 907, currentByte, tb_in);
		//  did we send a telegram ( sendTries>=1 and the received telegram is ACK or repetition max  -> send next telegram
		if ( wait_for_ack_from_remote) {
			if ((currentByte == SB_BUS_ACK ) ||  sendTries >=sendTriesMax || sendBusyTries >= sendBusyTriesMax)
			{ // last sending  to remote was ok or max retry, prepare for next tx telegram
				if ( sendTries >=sendTriesMax || sendBusyTries >= sendBusyTriesMax) tx_error|= TX_RETRY_ERROR;
				tb_h( 906, tx_error, tb_in);
				sendNextTelegram();

			}else if ( currentByte == SB_BUS_BUSY)
			{
				time = BUSY_WAIT_150BIT - PRE_SEND_TIME;
				tx_error |=TX_REMOTE_BUSY_ERROR;
				busy_wait_from_remote = true;
				repeatTelegram = true;

			} else
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
	tb_d( 901, state, tb_in);	tb_d( 902, sendTries, tb_in);	tb_d( 903, sendBusyTries, tb_in);tb_h( 904, sendAck, tb_in);
	tb_h( 905, rx_error, tb_in); tb_d( 910, telegramLen, tb_in);tb_d( 911, nextByteIndex, tb_in);

	//we received a telegram, next action wait to send ack back or wait 50 bit times for next rx/tx with cap event disabled
	timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer for next action: ack or telegram  start bit time -PRE_SEND_TIME
	//timer.captureMode(captureChannel, FALLING_EDGE); 	// no capture during wait- improves bus noise margin l
	timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT ); 	// todo enable timer reset by cap event
	timer.match(timeChannel,time);
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
	tx_error = 0;
	sendCurTelegram[0] = 0;
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
 *  Selecting the right state of SM ?us
 *
 */
void Bus::timerInterruptHandler()
{
	D(static unsigned short tick = 0);
	volatile bool timeout;
	volatile int time;

	// Debug output
	D(digitalWrite(PIO0_6, ++tick & 1));  // brown: interrupt tick
	D(digitalWrite(PIO3_0, state==Bus::SEND_BIT_0)); // red
	D(digitalWrite(PIO3_1, 0));           // orange
	D(digitalWrite(PIO1_5, 0));           // yellow
	D(digitalWrite(PIO1_4, 0));           // purple
	//D(digitalWrite(PIO2_8, 0));           // blue
	//    D(digitalWrite(PIO2_9, 0));           //

	// debug processing takes about 7-8us
#ifdef DEBUG_BUS
	tbint( state+8000, ttimer.value(), timer.flag(captureChannel),  timer.capture(captureChannel), timer.value(), timer.match(timeChannel), tb_in);
#endif

	STATE_SWITCH:
	switch (state)
	{

	// BCU is in start-up phase, we wait for 50 bits inactivity of the bus
	case Bus::INIT:
		tb_t( state, ttimer.value(), tb_in);

		if (timer.flag(captureChannel))
		{// cap event: bus not in idle state before we have a timeout, restart waiting time
			timer.value (0); // restart the timer
			break;
		}
		// timeout- we set bus to idle state
		idleState();
		break;


		// The bus is idle. Usually we come here when there is a capture event on bus-in, we continue with initializing the RX process.
		//A timeout  (after 0xfffe us) should not be received.!!( indicating no bus activity) match interrupt is disabled
	case Bus::IDLE:
		tb_d( state+100, ttimer.value(), tb_in);

		if (!timer.flag(captureChannel)) // Not a bus-in signal: do nothing - timeout??
			break;
		nextByteIndex = 0;
		collision = false;
		//need_to_send_ack_to_remote=false;
		rx_error  = 0;
		checksum = 0xff;
		repeatTelegram = false;
		sendAck = 0;
		valid = 1;
		//todo if timer was  disabled for power saving and enable in this state
		// no break here as we have received a capture event - falling edge of the start bit
		// we continue with receiving of start bit.

		// A start bit  (by cap event) is expected to arrive here. If we have a timeout instead, the
		// transmission of a frame is over.  (after 11  bit plus 2 fill bits :13*104us  + margin (1452us) after start of last char)
		// we expect that the timer was  not restarted by a cap event  )
	case Bus:: RECV_WAIT_FOR_STARTBIT_OR_TELEND:
		D(digitalWrite(PIO3_1, 1));   // orange

		if (!timer.flag(captureChannel))  // No start bit: then it is a timeout of end of frame
		{ // did the sending process waited for an ack from remote?
			{
				if (checksum) rx_error |= RX_CHECKSUM_ERROR;
				handleTelegram(valid && !checksum);
			}
			break;
		}
		//tb_h( state +100, currentByte, tb_in);

		// read the start bit now (about few us after cap event) if it is still at low level to avoid that spikes will start analysis of rx data
		if (digitalRead(rxPin)) rx_error |= RX_STARTBIT_ERROR; // we set error flag and continue rx process

		// we received a start bit interrupt - reset timer for next byte reception,
		// set byte time incl stop bit to 1144us and use that as ref for all succeeding timings in RX process
		// any rx-bit start should be within n*104 -7us, n*104 + 33us -> max 1177us
		// correct the timer start value by the process time (about 13us) we had since the capture event
		//todo  restart timer by cap  in order to have a ref point for the frame timeout
		unsigned int dt, tv, cv;
		tv=timer.value(); cv= timer.capture(captureChannel);
		if ( tv > cv ) dt= tv - cv; // check for timer overflow since cap event
		else dt = (0xffff-cv) +tv;
		timer.restart();  // restart timer and pre-load with interrupt processing time of 2us
		timer.value(dt+2);
		timer.match(timeChannel, BYTE_TIME_INCL_STOP);
		timer.matchMode(timeChannel, INTERRUPT | RESET);
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT); // next state interrupt at start bit  - falling edge, no reset
		//timer.counterMode(DISABLE,  DISABLE); // disabled the  timer reset by the falling edge of cap event
		state = Bus::RECV_BITS_OF_BYTE;
		currentByte = 0;
		bitTime = 0;
		bitMask = 1;
		parity = 1;
		break;

		// we received next capture event for a low bit at position n*104us or timeout if we have end of byte received
	case Bus::RECV_BITS_OF_BYTE:
		//tb_t( RECV_BITS_OF_BYTE, ttimer.value(), tb_in);

		timeout = timer.flag(timeChannel); // end of rx byte
		if (timeout) time = BYTE_TIME_INCL_STOP; // BYTE_TIME
		else time = timer.capture(captureChannel); // we received an capt. event: new low bit

		// find the bit position after last low bit and add high bits accordingly, window for the reception of falling edge of a bit is:
		//min: n*104us-7us, typ: n*104us, max: n*104us+33us. bitTime holds the start time of the last bit, so the new received
		//cap event should be between bitTime + BIT_TIME -7us and bitTime + BIT_TIME+33us
		if (time >= bitTime + BIT_WAIT_TIME) // todo check window should be at least 104-7us
		{
			bitTime += BIT_TIME;
			while (time >= bitTime + BIT_WAIT_TIME && bitMask <= 0x100) // high bit found or bit 9(parity bit) found
			{
				currentByte |= bitMask; // add high bit until we found current position
				parity = !parity;
				bitTime += BIT_TIME; // next bit time
				bitMask <<= 1; // next bit is in bitmask
			}

			if (time > bitTime  +33 && bitMask <= 0x100) rx_error |=RX_TIMING_ERROR; // bit edge receive to late- window error
			bitMask <<= 1; //next bit or stop bit
			//tb_d( RECV_BITS_OF_BYTE +400, time, tb_in);
			//tb_d( RECV_BITS_OF_BYTE +500, bitTime, tb_in);
		} // we might have received a additional edge due to bus reflection, tx-delay, edge should be within bit pulse +30us else  ignore edge

		if (timeout)  // Timer timeout: end of byte
		{
			D(digitalWrite(PIO1_5, 1));     // yellow: end of byte
			D(digitalWrite(PIO3_1, parity));// orange: parity bit ok

			if (nextByteIndex < SB_TELEGRAM_SIZE)
			{
				rx_telegram[nextByteIndex++] = currentByte;
				checksum ^= currentByte;
			} else {
				nextByteIndex = SB_TELEGRAM_SIZE -1;
				rx_error |= RX_LENGHT_ERROR;
			}

			if (!parity) rx_error |= RX_PARITY_ERROR;
			valid &= parity;
			tb_h( RECV_BITS_OF_BYTE +300, currentByte, tb_in);

			// wait for the next byte's start bit  or end of telegram and set timer to inter byte time + margin
			// timeout was at 11 bit times (1144us),  timeout for end of telegram - no more bytes after 2bit times after
			//last stop bit: >2 bit times, << ack waiting time -> 2*BITTIME + 100us margin (308- 450 us) seems to be ok.
			// we disable reset of timer by match to have fixed ref point at end of last RX-byte,
			//timer was restarted by timeout event at end of stop bit, we just set new match value
			// next state interrupt at start bit  falling edge
			state = Bus:: RECV_WAIT_FOR_STARTBIT_OR_TELEND;
			timer.match(timeChannel, MAX_INTER_CHAR_TIME);
			timer.matchMode(timeChannel, INTERRUPT);
			timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		}// cap event during stop bit: error, we should received byte-timeout later
		else if (time > BYTE_TIME_EXCL_STOP ) rx_error |= RX_STOPBIT_ERROR;
		//tb_h( RECV_BITS_OF_BYTE +200, rx_error, tb_in);
		break;

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

		// timeout: we waited 15BT - PRE_SEND_TIME after rx process, start sending an ack
		// timer was reseted by match for ref for tx process
		//, if cap event, we received an  early ack - continue with rx process
		//todo disable cap event in previous state - not needed during waiting for ack start
	case Bus::RECV_WAIT_FOR_ACK_TX_START:
		tb_t( state, ttimer.value(), tb_in);

		// cap event- should not happen here;  start receiving,  maybe ack or early tx from other device, todo probably timing error
		if (timer.flag(captureChannel)){
			state = Bus::IDLE;
			goto STATE_SWITCH;
		}

		sendTelegramLen = 0;

		// set timer for TX process: init PWM pulse generation, interrupt at pulse end and cap event (pulse start)
		timer.match(pwmChannel, PRE_SEND_TIME); // waiting time till start of first bit- falling edge 104us + n*104us ( n=0 or3)
		timer.match(timeChannel, PRE_SEND_TIME  + BIT_PULSE_TIME); // end of bit pulse 35us later
		//timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer after bit pulse end
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		nextByteIndex = 0;
		tx_error=0;
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

		/*  WAIT_FOR_NEXT_RX_OR_PENDING_TX
		 * is entered by match interrupt some usec (PRE_SEND_TIME or 1us coming from idle state) before sending the start bit of the first byte
		 * of a pending telegram. It is always entered after receiving or sending is done and we waited the respective time for next action. If no tel
		 * is pending, we enter idle state. Any new sending or receiving process will be started there. If there is a tel pending,we check for prio
		 *  and start sending after pre-send-time of 104us or if we have a normal Telegram after pre-send-time + 3*BitTime,
		 *  resulting in 50/53BT between telegrams
		 *
		 *   State should be entered with timer reset by match to have valid ref point for tx cap event with timer still running
		 **/

	case Bus::WAIT_FOR_NEXT_RX_OR_PENDING_TX:
		tb_t( state, ttimer.value(), tb_in);
		D(digitalWrite(PIO1_5, 1)); // yellow: prepare transmission

		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT ); // enable cap event after waiting time  for next rx

		if (timer.flag(captureChannel)){ // cap event- start receiving,  maybe ack or early tx from other device
			state = Bus::IDLE;
			goto STATE_SWITCH;
		}
		// any pending tx?
		if(sendAck) {
			time = PRE_SEND_TIME;
			sendTelegramLen = 0;
		}
		else
		{// check if we have max resend for last telegram.
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
			else  // Send nothing: set idle state
			{
				tb_h( state+ 900,sendCurTelegram[0], tb_in);
				//tb_t( state*100+4, ttimer.value(), tb_in);
				idleState();
				break;
			}
		}
		tb_t( state+500, ttimer.value(), tb_in);
		tb_d( state+600, time, tb_in);
		// set timer for TX process: init PWM pulse generation, interrupt at pulse end and cap event (pulse start)
		timer.match(pwmChannel, time); // waiting time till start of first bit- falling edge 104us + n*104us ( n=0 or3)
		timer.match(timeChannel, time + BIT_PULSE_TIME); // end of bit pulse 35us later
		timer.matchMode(timeChannel, RESET | INTERRUPT); //reset timer after bit pulse end
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		nextByteIndex = 0;
		tx_error=0;
		state = Bus::SEND_START_BIT;
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
				// received edge of bit before our own bit was triggered - go to receiving process
				tb_d( state+300, ttimer.value(), tb_in);
				timer.match(pwmChannel, 0xffff); // stop our bit  set pwm output to low

				// correct the timer start value by the process time (about 13us) we had since the capture event
				unsigned int dt, tv, cv;
				tv=timer.value(); cv= timer.capture(captureChannel);
				if ( tv > cv ) dt= tv - cv; // check for timer overflow since cap event
				else dt = (0xffff-cv) +tv;
				timer.reset();  // reset timer and pre-load with processing time
				timer.value(dt);
				timer.matchMode(timeChannel, INTERRUPT | RESET);
				timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT); // next state interrupt at start bit  - falling edge, no reset
				timer.match(timeChannel, BYTE_TIME_INCL_STOP);
				//timer.counterMode(DISABLE,  DISABLE); // disabled the  timer reset by the falling edge of cap event
				state = Bus:: RECV_WAIT_FOR_STARTBIT_OR_TELEND; // start reception of byte
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
		 *  bit pulse. Prepare for  sending bits of frame.
		 *
		 **/
	case Bus::SEND_BIT_0:
		//tb_d( state+100, timer.match (pwmChannel), tb_in);
		// get byte to send

		if (sendAck){
			currentByte = sendAck;
		}
		else currentByte = sendCurTelegram[nextByteIndex++];

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
				state = Bus::SEND_BIT_0;  // state for  bit-0 of next byte to send
			}
			else
			{//  need to send stop  bit and sync with bus timing
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
		// as we are at the raisng edge of the last pulse, the next falling edge will be n*104 - 35us (min69us) away
		else timer.match(pwmChannel, time - BIT_PULSE_TIME); // start of pulse for next low bit - falling edge on bus will not trigger cap interrupt
		//tb_d( SEND_BITS_OF_BYTE+500, time, tb_in);
		//tb_h( SEND_BITS_OF_BYTE+600, timer.captureMode(captureChannel),  tb_in);

		timer.match(timeChannel, time); // interrupt at end of low/high bit pulse - next raising edge or after stop bit + 2 wait bits
		break;


		/* SEND_WAIT_FOR_HIGH_BIT_END
		 * state is in phase shift,  entered by  cap event or match/period interrupt from pwm
		 * Wait for a capture event from bus-in. This should be from us sending a zero bit, but it  might as well be from somebody else in case of a
		 * collision. Our low bit starts at pwmChannel time and ends at match of timeChannel.
		 *  Check for collision during sending of high bits. As our timing is related to the rising edge of a bit we need to measure accordingly:
		 *  next bit start window is in 69us, and the n-bit low pulse starts at n*104 - 35us and ends at n*104 -> check for edge window: the high phase
		 *  of the last bit : 69us - margin till 69us before next falling edge at pwmChannel time + margin
		 *  Timeout event indicated a bus timing error
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

		//state is in sync with resp. to bus timing,  entered by match interrupt  after last bytes stop bit was send
		// for normal frames we should wait for ack from remote layer2 after ack-waiting time
		// timer was reset
	case Bus::SEND_END_OF_TX:
		tb_t( state, ttimer.value(), tb_in);
		tb_h( SEND_END_OF_TX+700, repeatTelegram, tb_in);
		D(digitalWrite(PIO2_9, 1));

		//wait_for_ack_from_remote = true; // default   for data layer: send ack back to remote
		state= Bus::WAIT_FOR_NEXT_RX_OR_PENDING_TX;
		time =  SEND_WAIT_TIME - PRE_SEND_TIME;// we wait 50 BT- pre-send-time for next rx/tx window, cap intr disabled

		if (sendAck){
			sendAck = 0; // we send an ack for last received frame
			//need_to_send_ack_to_remote = false;
			tb_h( SEND_END_OF_TX+200, tx_error, tb_in);
			// todo inform receiving process of pos ack tx
		}else
		{
			tb_h( SEND_END_OF_TX+600, repeatTelegram, tb_in);

			// normal data frame, check for ack request for our telegram
			if (!(sendCurTelegram[0] & SB_TEL_ACK_REQ_FLAG)) {
				wait_for_ack_from_remote = true; // default for data layer: send ack back to remote
				time=  ACK_WAIT_TIME_MIN; //we wait 15BT- marging for ack rx window, cap intr disabled
				state = Bus::SEND_WAIT_FOR_RX_ACK_WINDOW;
				timer.matchMode(timeChannel, INTERRUPT); // no timer reset after timeout
				if (repeatTelegram){
					if (busy_wait_from_remote)
						sendBusyTries++;
					else
						sendTries++;
				}
			}else wait_for_ack_from_remote = false;

		}
		tb_d( SEND_END_OF_TX+300, wait_for_ack_from_remote , tb_in);
		tb_d( SEND_END_OF_TX+400, sendTries , tb_in);
		tb_d(SEND_END_OF_TX+500, sendBusyTries , tb_in);
		timer.match(timeChannel, time); // we wait respective time - pre-send-time for next rx/tx window, cap intr disabled
		break;

		//ACK receive windows starts now after the timeout event
		// enable cap event and wait till end of ACK receive window for the ACK
	case Bus::SEND_WAIT_FOR_RX_ACK_WINDOW:
		tb_t( state, ttimer.value(), tb_in);

		state = Bus::SEND_WAIT_FOR_RX_ACK;
		//timer.matchMode(timeChannel, INTERRUPT | RESET); // timer reset after timeout to have ref point in next RX/TX state
		//		timer.counterMode(DISABLE,  captureChannel  | FALLING_EDGE); // enabled the  timer reset by the falling edge of cap event
		timer.captureMode(captureChannel, FALLING_EDGE | INTERRUPT );
		timer.match(timeChannel, ACK_WAIT_TIME_MAX); // we wait 15BT+ marging for ack rx window, cap intr enabled
		break;

		// we wait here for the cap event of the ACK. If we receive a timeout- no ack was received and we need
		// to start a repetition of the last telegram
	case Bus::SEND_WAIT_FOR_RX_ACK:
		tb_t( state, ttimer.value(), tb_in);

		if (timer.flag(captureChannel)){
			state = Bus::IDLE;  // start bit of ack received - continue rx process for rest of byte
			goto STATE_SWITCH;
		}
		repeatTelegram = true;
		tx_error|= TX_ACK_TIMEOUT_ERROR; // todo  ack timeout - inform upper layer on error state and repeat tx  if needed
		state = Bus::WAIT_FOR_NEXT_RX_OR_PENDING_TX;
		timer.match(timeChannel, SEND_WAIT_TIME - PRE_SEND_TIME); // we wait 50 BT- pre-send-time for next rx/tx window, cap intr disabled
		timer.captureMode(captureChannel, FALLING_EDGE| INTERRUPT  );// todo disable cap int during wait
		timer.matchMode(timeChannel, INTERRUPT | RESET); // timer reset after timeout to have ref point in next RX/TX state
		//state = Bus::WAIT_FOR_IDLE50;

		break;


	default:
		idleState();
		tb_d( 9999, ttimer.value(), tb_in);
		break;
	}

	timer.resetFlags();
}


