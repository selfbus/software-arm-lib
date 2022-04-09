/**************************************************************************//**
 * @addtogroup SBLIB_SENSOR Selfbus library sensors
 * @defgroup SBLIB_SENSOR_SHT1x SHT1x Temperature/humidity sensor
 * @ingroup SBLIB_SENSOR
 * @brief   Implementation for the usage of the Sensirion SHT1x series temperature and humidity sensor.
 *
 * @{
 *
 * @file   SHT1x.h
 * @bug Command @ref sht1xReadStatusRegister doesn't work.
 ******************************************************************************/

/*
 * SHT1x Library
 *
 * https://github.com/practicalarduino/SHT1x (04/01/2022) commit sha: be7042c3e3cb32c778b9c9ca270b3df57f769ea5
 * Copyright 2009 Jonathan Oxer <jon@oxer.com.au> / <www.practicalarduino.com>
 * Based on previous work by:
 *    Maurice Ribble: <www.glacialwanderer.com/hobbyrobotics/?p=5>
 *    Wayne ?: <ragingreality.blogspot.com/2008/01/ardunio-and-sht15.html>
 *
 * Manages communication with SHT1x series (SHT10, SHT11, SHT15)
 * temperature / humidity sensors from Sensirion (www.sensirion.com).
 */

#ifndef sblib_SHT1x_h
#define sblib_SHT1x_h

#include <stdint.h>
#include <sblib/timeout.h>
#include <sblib/sensors/units.h>

#define WAIT_STEP_MS (20)
#define MAX_WAIT_MS (340)

#define INVALID_TEMPERATURE (-27300)
#define INVALID_HUMIDITY (0)
#define INVALID_DEW_POINT (0)

/**
 * Class for accessing the SHT1x sensor series for temperature and humidity
 * @note The SHT1x series uses a own version of a i2c start sequence
 */
class SHT1x
{
  public:
    /**
     * Constructor for the SHT1x class to
     * @param dataP     Pin to use as DATA line
     * @param clockP    Pin to use as CLOCK line
     */
    SHT1x(int dataP, int clockP);
    ~SHT1x() = default;

    /**
     * Read temperature-corrected relative humidity
     * @param humidity - pointer to buffer for the new humidity
     * @return True if successful, otherwise false
     * @warning Function is blocking and can take up to twice of @ref MAX_WAIT_MS to return
     */
    bool readHumidity(float* humidity);

    /**
     * Read temperature in degree Celsius
     * @param temperature - pointer to buffer for the new temperature
     * @return True if successful, otherwise false
     * @warning Function is blocking and can take up to @ref MAX_WAIT_MS to return
     */
    bool readTemperatureC(float* temperature);

    /**
     * Read temperature in degree Fahrenheit
     * @param temperature - pointer to buffer for the new temperature
     * @return True if successful, otherwise false
     * @warning Function is blocking and can take up to @ref MAX_WAIT_MS to return
     */
    bool readTemperatureF(float* newTemperature);

    /**
     * Initialize the SHTxx sensor
     */
    void Init();

    /**
     * Softreset the SHT1x sensor, resets the interface, clears the status register to default
     * @return True if successful, otherwise false
     * @note The sensor needs ~11ms to reset
     */
    bool softReset();

    /**
     * Get humidity as integer from sensor in %rH
     * @return The humidity with factor 100 (2045 = 20,45%rH)
     * @note only implemented for "compatibility with @ref SHT2x class
     * @warning Function is blocking and can take up to twice of @ref MAX_WAIT_MS to return
     */
    uint16_t GetHumidity();

    /**
     * Get the last measured humidity
     * @return The last humidity with factor 100 (2045 = 20,45%rH)
     */
    uint16_t getLastHumidity() {return lastHumidity;}

    /**
    * Get temperature as integer from sensor in degree Celsius
    * @return Temperature with factor 100 (2045 = 20,45°C)
    * @note only implemented for "compatibility with @ref SHT2x class
    * @warning Function is blocking and can take up to @ref MAX_WAIT_MS to return
    */
    int16_t GetTemperature();

    /**
     * Get the last measured temperature in degree Celsius
     * @return The last measured temperature with factor 100 (2045 = 20,45°C)
     */
    int16_t getLastTemperature() {return lastTemperatureC;}

    /**
     * Gets the current dew point based on the current humidity and temperature
     * @return The dew point in TD
     * @warning Function is blocking and can take up to twice of @ref MAX_WAIT_MS to return
     */
    float GetDewPoint();

    /**
     * Get the SHT1x status register e.g. resolution, heater and OTP
     * @param status    Content of the status register
     * @return True if successful, otherwise false
     * @warning Function is blocking and can take up to 340ms to return (@ref MAX_WAIT_MS)
     */
    bool getStatusRegister(uint16_t* status);

    /**
     * Set content of the SHT1x status register e.g. resolution, heater and OTP
     * @param status    new value of the status register
     * @return True if successful, otherwise false
     * @warning Function is blocking and can take up to 340ms to return (@ref MAX_WAIT_MS)
     */
    bool setStatusRegister(const uint16_t& status);

private:
    /**
     * SHT1x control commands
     */
    enum SHT1xCommand {
        sht1xMeasureTemperature      = 0x03, //!< Request Temperature from SHT1x
        sht1xMeasureRelativeHumidity = 0x05, //!< Request relative humidity from SHT1x
        sht1xReadStatusRegister      = 0x07, //!< Read the status register of the SHT1x
        sht1xWriteStatusRegister     = 0x06, //!< Write to the status register of the SHT1x
        sht1xSoftReset               = 0x1e, //!< Reset the interface, clears the status register to default values
    };

    SHT1x() = delete;

    /**
     * Read raw temperature value
     * @param   temperatureRaw - pointer to buffer for the new temperature
     * @param   sendTransmissionStart - Set to true to send the special transmission start sequence, otherwise false
     * @return  True if successful, otherwise false
     * @warning Function is blocking and can take up to 340ms to return (@ref MAX_WAIT_MS)
     */
    bool readTemperatureRaw(int16_t* temperatureRaw);
    float convertRawTemperature(const float& temperature, eScale type);
    bool sendByteToSHT(uint8_t toSend, bool isCommand);

    /**
     * Wait for a measurement to complete. It can take up to a maximum of 20/80/320 ms for a 8/12/14bit measurement.
     * @return true if successful, otherwise false
     */
    bool waitForResultSHT();
    uint16_t getData16SHT();
    void skipCrcSHT();

    void activateDataPin();
    void releaseDataPin();
    void clockCycle();

    void sendTransmissionStart();

    int dataPin;
    int clockPin;
    int16_t lastTemperatureC;
    uint16_t lastHumidity;
};

#endif /*sblib_SHT1x_h*/
