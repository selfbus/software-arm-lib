/*

  This is a library for the BH1750FVI Digital Light Sensor breakout board.

  The BH1750 board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The BH1750 library doesn't do this automatically.

  Written by Christopher Laws, March, 2013.
  Ported to selfbus by Mario Theodoridis on May 26 2021

*/

#include <sblib/i2c/bh1750.h>

/**
 * Constructor
 * @params addr Sensor address (0x76 or 0x72, see datasheet)
 *
 * On most sensor boards, it was 0x76
 */
BH1750::BH1750(byte addr) {

  i2c_lpcopen_init();
  BH1750_I2CADDR = addr;
}

/**
 * Configure sensor
 * @param mode Measurement mode
 * @param addr Address of the sensor
 */
bool BH1750::begin(Mode mode, byte addr) {

  LOG("begin");
  i2c_lpcopen_init();
  if (addr) {
    BH1750_I2CADDR = addr;
  }

  // Configure sensor in specified mode and set default MTreg
  return (configure(mode) && setMTreg(BH1750_DEFAULT_MTREG));
}

#ifdef BH1750_DEBUG
static void _logAck(byte ack) {
  switch (ack) {
    case I2C_STATUS_DONE:
      LOG("[BH1750] SUCCESS: All good");
      break;
    case I2C_STATUS_NAK:
      LOG("[BH1750] ERROR: NAK received during transfer");
      break;
    case I2C_STATUS_ARBLOST: // not likely
      LOG("[BH1750] ERROR: arbitration lost during transfer");
      break;
    case I2C_STATUS_BUSERR:
      LOG("[BH1750] ERROR: Bus error in I2C transfer");
      break;
    case I2C_STATUS_BUSY:
      LOG("[BH1750] ERROR: I2C is busy doing transfer");
      break;
    default:
      LOG("[BH1750] ERROR: other error");
      break;
  }
}
#endif

/**
 * Configure BH1750 with specified mode
 * @param mode Measurement mode
 */
bool BH1750::configure(Mode mode) {
  LOG("configure %d", mode);

  // default transmission result to a value out of normal range
  byte ack = 5;
  I2C_XFER_T xfer = {0};

  // Check measurement mode is valid
  switch (mode) {
    case BH1750::CONTINUOUS_HIGH_RES_MODE:
    case BH1750::CONTINUOUS_HIGH_RES_MODE_2:
    case BH1750::CONTINUOUS_LOW_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE_2:
    case BH1750::ONE_TIME_LOW_RES_MODE:

      // Send mode to sensor
      xfer.slaveAddr = BH1750_I2CADDR;
      xfer.txBuff = (uint8_t *) &mode;
      xfer.txSz = 1;
      while ((ack = Chip_I2C_MasterTransfer(I2C0, &xfer)) == I2C_STATUS_ARBLOST) {
        // FIXME i wonder whether a timeout condition might be of use here
      }
      // Wait a few moments to wake up
      delay(10);
      break;

    default:
      // Invalid measurement mode
      LOG("[BH1750] ERROR: Invalid mode");
      break;
  }

  // Check result code
  if (ack == I2C_STATUS_DONE) {
    BH1750_MODE = mode;
    lastReadTimestamp = millis();
    return true;
  }
#ifdef BH1750_DEBUG
  _logAck(ack);
#endif
  return false;
}

/**
 * Configure BH1750 MTreg value
 * MT reg = Measurement Time register
 * @param MTreg a value between 32 and 254. Default: 69
 * @return bool true if MTReg successful set
 * 		false if MTreg not changed or parameter out of range
 */
bool BH1750::setMTreg(byte MTreg) {
  LOG("setMTreg %d", MTreg);
  // Bug: lowest value seems to be 32!
  if (MTreg <= 31 || MTreg > 254) {
    LOG("[BH1750] ERROR: MTreg out of range");
    return false;
  }
  byte ack = 5;
  // Send MTreg and the current mode to the sensor
  //   High bit: 01000_MT[7,6,5]
  //    Low bit: 011_MT[4,3,2,1,0]
  int buf[] = {
          (0b01000 << 3) | (MTreg >> 5),
          (0b011 << 5) | (MTreg & 0b11111),
          BH1750_MODE};
  I2C_XFER_T xfer = {0};
  xfer.slaveAddr = BH1750_I2CADDR;
  for (int i = 0; i < 3; i++) {
    xfer.txBuff = (uint8_t *) &buf[i];
    xfer.txSz = 1;
    while ((ack = Chip_I2C_MasterTransfer(I2C0, &xfer)) == I2C_STATUS_ARBLOST) {
      // i wonder whether a timeout condition might be of use here
    }
  }

  // Wait a few moments to wake up
  delay(10);

  // Check result code
  if (ack == I2C_STATUS_DONE) {
    BH1750_MTreg = MTreg;
    return true;
  }
#ifdef BH1750_DEBUG
  _logAck(ack);
#endif
  return false;
}

/**
 * Checks whether enough time has gone to read a new value
 * @param maxWait a boolean if to wait for typical or maximum delay
 * @return a boolean if a new measurement is possible
 *
 */
bool BH1750::measurementReady(bool maxWait) {
  unsigned long delaytime = 0;
  switch (BH1750_MODE) {
    case BH1750::CONTINUOUS_HIGH_RES_MODE:
    case BH1750::CONTINUOUS_HIGH_RES_MODE_2:
    case BH1750::ONE_TIME_HIGH_RES_MODE:
    case BH1750::ONE_TIME_HIGH_RES_MODE_2:
      maxWait ? delaytime = (180 * BH1750_MTreg / (byte) BH1750_DEFAULT_MTREG)
              : delaytime = (120 * BH1750_MTreg / (byte) BH1750_DEFAULT_MTREG);
      break;
    case BH1750::CONTINUOUS_LOW_RES_MODE:
    case BH1750::ONE_TIME_LOW_RES_MODE:
      // Send mode to sensor
      maxWait ? delaytime = (24 * BH1750_MTreg / (byte) BH1750_DEFAULT_MTREG)
              : delaytime = (16 * BH1750_MTreg / (byte) BH1750_DEFAULT_MTREG);
      break;
    default:
      break;
  }
  // Wait for new measurement to be possible.
  // Measurements have a maximum measurement time and a typical measurement
  // time. The maxWait argument determines which measurement wait time is
  // used when a one-time mode is being used. The typical (shorter)
  // measurement time is used by default and if maxWait is set to True then
  // the maximum measurement time will be used. See data sheet pages 2, 5
  // and 7 for more details.
  unsigned long currentTimestamp = millis();
  if (currentTimestamp - lastReadTimestamp >= delaytime) {
    return true;
  } else
    return false;
}

/**
 * Read light level from sensor
 * The return value range differs if the MTreg value is changed. The global
 * maximum value is noted in the square brackets.
 * @return Light level in lux (0.0 ~ 54612,5 [117758,203])
 * 	   -1 : no valid return value
 * 	   -2 : sensor not configured
 */
float BH1750::readLightLevel() {

  if (BH1750_MODE == UNCONFIGURED) {
    LOG("[BH1750] Device is not configured!");
    return -2.0;
  }

  // Measurement result will be stored here
  float level = -1.0;

  // Read two bytes from the sensor, which are low and high parts of the sensor
  // value
  uint8_t buf[] = {0, 0};
  if (2 == Chip_I2C_MasterRead(I2C0, BH1750_I2CADDR, &buf[0], 2)) {
    level = (float) (buf[0] << 8 | buf[1]);
  }
  lastReadTimestamp = millis();

  if (level != -1.0) {
// Print raw value if debug enabled
#ifdef BH1750_DEBUG
    LOG("[BH1750] Raw value: %d", pretty(level));
#endif

    if (BH1750_MTreg != BH1750_DEFAULT_MTREG) {
      level *= (float) ((byte) BH1750_DEFAULT_MTREG / (float) BH1750_MTreg);
      // Print MTreg factor if debug enabled
#ifdef BH1750_DEBUG
      LOG("[BH1750] MTreg factor: %d",
          pretty((float)((byte)BH1750_DEFAULT_MTREG / (float)BH1750_MTreg)));
#endif
    }
    if (BH1750_MODE == BH1750::ONE_TIME_HIGH_RES_MODE_2 ||
        BH1750_MODE == BH1750::CONTINUOUS_HIGH_RES_MODE_2) {
      level /= 2;
    }
    // Convert raw value to lux
    level /= BH1750_CONV_FACTOR;

// Print converted value if debug enabled
#ifdef BH1750_DEBUG
    LOG("[BH1750] Converted float value: %d", pretty(level));
#endif
  }

  return level;
}
