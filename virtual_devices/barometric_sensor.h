// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file barometric_sensor.h
 * Example barometric pressure sensor interfaces that also support altitude calculations.
 *
 * This header defines three variations of a barometric sensor:
 * - A simple interface, which only provides the capabilities of reading pressure/altitude
 *   (BarometricSensor).
 * - A variation which supports callbacks (BarometricSensor_withCb).
 * - A variation that supports callbacks and expects to be used in an asynchronous
 *   system (BarometricSensor_asyncWithCb).
 *
 * Note that there are differences in fundamental assumptions and function behaviors
 * across the variations. Even small changes in an interface can impact expected
 * behaviors.
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type of the pressure parameter (e.g., to a float)
 * - Change the fixed-point format of the parameter (e.g., to 24.8, giving a wider range
 *   reduced resolution)
 * - Return the pressure sample, and use a separate function to determine validity.
 * - Specifying additional callback parameters
 * - Varying the types of callbacks used
 * - Separating callbacks for altitude and pressure updates
 * - Eliminating the ability to unregister a callback
 * - Supporting only a single callback function (this is primarily a change in
 *   documentation to note the change in assumption)
 */

#pragma mark - Basic Interface -

/** Virtual Barometric Pressure Sensor Interface
 *
 * A standard interface for a device which can measure barometric pressure.
 *
 * This is a simple interface, which only provides the ability to read the current
 * sample.
 *
 * ## Fundamental Assumptions
 *
 * - The device produces barometric pressure readings
 * 	- This device reports barometric pressure in hectopascal (hPa)
 * 	- Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
 *   giving a resolution of 0.001 hPa.
 * - This device produces barometric altitude readings
 * 	- This device will report barometric altitude in meters
 * 	- Altitude will be formatted as a 32-bit fixed-point integer with format Q21.10,
 *   giving a resolution of 0.001 m.
 * 	- Altitude will be corrected for Sea Level Pressure. If no value for SLP has been supplied,
 * calculations will assume 1013.25 hPa.
 * - The device will indicate whether the current sample is valid or invalid
 *
 * - This interface, at its core, appears to be blocking. However, you can still implement this
 *   interface in a non-blocking way. For example, you could always return the most recent altitude
 *   measurement, while there is another thread (or a timer) that handles updates to that
 *   measurement asynchronously.
 */
typedef struct
{
	/** Read the current pressure from the device.
	 *
	 * @pre The pressure sensor has been properly initialized by the system.
	 * @pre The pressure parameter is not NULL.
	 * @post If the measurement is valid, the data pointed to by the pressure parameter
	 *       will be updated with the latest reading.
	 * @post If the measurement is invalid, the data pointed to by the pressure parameter
	 * 		 will remain unchanged.
	 *
	 * @param[inout] pressure
	 *  Pointer which will be used for storing the latest pressure reading. This pointer
	 *  must not be null.
	 *
	 *  Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
	 *  giving a resolution of 0.001 hPa.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*readPressure)(uint32_t* const pressure);

	/** Get the current altitude, corrected for Sea Level Pressure
	 *
	 * If no value for SLP has been supplied, calculations will assume 1013.25 hPa.
	 *
	 * @pre The pressure sensor has been properly initialized by the system.
	 * @pre The altitude parameter is not NULL.
	 * @post If the measurement is valid, the data pointed to by the altitude parameter
	 *       will be updated with the latest reading.
	 * @post If the measurement is invalid, the data pointed to by the altitude parameter
	 * 		 will remain unchanged.
	 *
	 * @returns Current altitude in meters (m), corrected for sea level pressure.
	 *	Altitude is specified as a signed 32-bit fixed-point number in format Q21.10.
	 */
	bool (*readAltitude)(int32_t* const altitude);

	/** Set the sea level pressure
	 *
	 * @param[in] slp The current sea level pressure in hPa.
	 * 	slp should be specified as an unsigned 32-bit fixed-point number in format UQ22.10.
	 */
	void (*setSeaLevelPressure)(uint32_t slp);
} BarometricSensor;

#pragma mark - Interface with Callback Support -

/** Callback function prototype for processing new barometric samples
 *
 * When a new (and valid) barometric pressure/altitude sample is available, this callback
 * function will be invoked.
 *
 * The callback is not guaranteed to run on its own thread of control. We recommend
 * keeping the implementation small. Your function implementation could take the
 * new sample and perform some dispatching operation (e.g., add the value to a queue),
 * ensuring that any "heavy" processing happens on a new thread.
 *
 * @param[in] pressure The latest pressure sample.
 *
 *  Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
 *  giving a resolution of 0.001 hPa.
 *
 * @param[in] altitude The latest altitude sample.
 *
 *  Altitude will be formatted as a signed 32-bit fixed-point number in format Q21.10.
 *
 */
typedef void (*NewBarometricSampleCb)(uint32_t pressure, int32_t altitude);

/** Callback function prototype for barometric sensor errors
 *
 * When an error in the virtual barometric device occurs, this callback function
 * will be invoked. The virtual device itself does not support error handling
 * capabilities, so we recommend using this callback in tightly-coupled system
 * code to take the appropriate recovery action (restart the device, restart the
 * system, etc.).
 *
 * The callback is not guaranteed to run on its own thread of control. We recommend
 * keeping the implementation small. Your function implementation could take the
 * new sample and perform some dispatching operation (e.g., add the value to a queue),
 * ensuring that any "heavy" processing happens on a new thread.
 */
typedef void (*BarometricErrorCb)(void);

/** Virtual Barometric Pressure Sensor Interface (with callback support)
 *
 * A standard interface for a device which can measure barometric pressure. In this variant, the
 * readPressure() and readAltitude() functions can be used in the following ways:
 * - Have the caller receive the current sample, and trigger registered callbacks
 * - Have the caller trigger a reading (input parameter == NULL), but only have registered
 *   callbacks receive the data.
 *
 * ## Fundamental Assumptions
 *
 * - The device produces barometric pressure readings
 * 	- This device reports barometric pressure in hectopascal (hPa)
 * 	- Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
 *    giving a resolution of 0.001 hPa.
 * - This device produces barometric altitude readings
 * 	- This device will report barometric altitude in meters
 * 	- Altitude will be formatted as a 32-bit fixed-point integer with format Q21.10,
 *    giving a resolution of 0.001 m.
 * 	- Altitude will be corrected for Sea Level Pressure. If no value for SLP has been supplied,
 *    calculations will assume 1013.25 hPa.
 * - The device will indicate whether the current sample is valid or invalid
 * - The device will notify interested parties when a new valid sample is available.
 *
 * ## Undesired event assumptions
 * - If an error occurs internally, the virtual device will notify interested parties
 *   by issuing an error callback. The registered parties can take desired action
 *   when this occurs (e.g., attempt recovery, stop querying the sensor).
 *
 * ## Implementation Notes
 *
 * - This interface, at its core, appears to be blocking. However, you can still implement this
 *   interface in a non-blocking way. For example, you could always return the most recent pressure
 *   measurement, while there is another thread that handles updates to that measurement
 *   asynchronously.
 * - Note that the callback registration functions do not support error handling.
 *   We recommend that implementers trigger an assert() or other crash if a callback
 *   cannot be added to a list due to exceeding fixed size constraints.
 */
typedef struct
{
	/** Request an pressure sample from the device.
	 *
	 * @pre The pressure sensor has been properly initialized by the system.
	 * @post If the measurement is valid and pressure is not NULL, the data pointed
	 *       to by the pressure parameter will be updated with the latest reading
	 * @post If the measurement is invalid, the data pointed to by the pressure parameter
	 * 		 will remain unchanged.
	 * @post If the measurement is valid, registered New Sample callbacks will be invoked
	 * 		  or dispatched with the new measurement.
	 * @post If the measurement is not valid, registered Error callbacks will be invoked
	 * 		  or dispatched.
	 *
	 * @param[inout] pressure
	 *  Pointer which will be used for storing the latest pressure reading.
	 *
	 * 	If pressure is NULL, the function will only supply the pressure sample to
	 *  registered callback functions. I
	 *
	 *  Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
	 *  giving a resolution of 0.001 hPa.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*readPressure)(int32_t* const pressure);

	/** Request an altitude sample from the device.
	 *
	 * Altitude will be corrected for Sea Level Pressure. If no value for SLP has been supplied,
	 * calculations will assume 1013.25 hPa.
	 *
	 * @pre The altitude sensor has been properly initialized by the system.
	 * @post If the measurement is valid and the altitude parameter is not NULL, the data pointed
	 *       to will be updated with the latest reading
	 * @post If the measurement is invalid, the data pointed to by the altitude parameter
	 * 		 will remain unchanged.
	 * @post If the measurement is valid, registered New Sample callbacks will be invoked
	 * 		  or dispatched with the new measurement.
	 * @post If the measurement is not valid, registered Error callbacks will be invoked
	 * 		  or dispatched.
	 *
	 * @param[inout] altitude
	 *  Pointer which will be used for storing the latest altitude reading.
	 *
	 * 	If altitude is NULL, the function will only supply the altitude sample to
	 *  registered callback functions.
	 *
	 *  Altitude is specified as a signed 32-bit fixed-point number in format Q21.10
	 *
	 *  Altitude will be corrected for Sea Level Pressure.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*readAltitude)(int32_t* const altitude);

	/** Register a NewBarometricSampleCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a new and valid sample is available.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of "new sample" callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to register on the "new sample" callback list.
	 */
	void (*registerNewSampleCb)(const NewBarometricSampleCb callback);

	/** Remove a registered NewBarometricSampleCb function
	 *
	 * This function will remove a callback function from the registered list of
	 * "new sample" callbacks. If the function has not been previously registered,
	 * the parameter will be ignored and the list will be unchanged.
	 *
	 * @post callback function pointer is not present on the list of "new sample" callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to remove from the "new sample" callback list.
	 */
	void (*unregisterNewSampleCb)(const NewBarometricSampleCb callback);

	/** Register a BarometricErrorCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a pressure sensor error occurs.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of error callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to register on the "error" callback list.
	 */
	void (*registerErrorCb)(const BarometricErrorCb callback);

	/** Remove a registered BarometricErrorCb function
	 *
	 * This function will remove a callback function from the registered list of
	 * "error" callbacks. If the function has not been previously registered,
	 * the parameter will be ignored and the list will be unchanged.
	 *
	 * @post callback function pointer is not present on the list of "error" callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to remove from the "error" callback list.
	 */
	void (*unregisterErrorCb)(const BarometricErrorCb callback);
} BarometricSensor_withCb;

#pragma mark - Asynchronous Processing Support -

/** Virtual Barometric Pressure Sensor Interface (asynchronous mode)
 *
 * A standard interface for a device which can measure barometric pressure. This interface
 * is intended for use with _asynchronous_ implementations. That is, the readPressure() function
 * cannot be a blocking function, but will instead be expected to enqueue a request to geneate
 * a new pressure sample. The pressure sample, whenver it becomes available, will be passed
 * along to interested parties via the registered callback functions.
 *
 * ## Fundamental Assumptions
 *
 * - The device produces barometric pressure readings
 * 	- This device reports barometric pressure in hectopascal (hPa)
 * 	- Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
 *   giving a resolution of 0.001 hPa.
 * - This device produces barometric altitude readings
 * 	- This device will report barometric altitude in meters
 * 	- Altitude will be formatted as a 32-bit fixed-point integer with format Q21.10,
 *   giving a resolution of 0.001 m.
 * 	- Altitude will be corrected for Sea Level Pressure. If no value for SLP has been supplied,
 * calculations will assume 1013.25 hPa.
 * - readPressure() will be used to request a new pressure sample from the device.
 * - The device will notify interested parties when a new valid sample is available after
 *   a successful readPressure() request has completed.
 *
 * ## Undesired event assumptions
 *
 * - If an error occurs internally, the virtual device will notify interested parties
 *   by issuing an error callback. The registered parties can take desired action
 *   when this occurs (e.g., attempt recovery, stop querying the sensor).
 * - The readPressure() function will provide inidication if the request cannot be made
 *   (e.g., queue full)
 *
 * ## Implementation Notes
 *
 * - readPressure() should be a non-blocking call. It should not directly query the sensor and
 *   wait for a response, but should instead send a request to a queue of some type.
 * - Note that the callback registration functions do not support error handling.
 *   We recommend that implementers trigger an assert() or other crash if a callback
 *   cannot be added to a list due to exceeding fixed size constraints.
 */
typedef struct
{
	/** Request a new current pressure/altitude sample from the device.
	 *
	 * Submit a request to the device for a new sample. When available, the data will be provided
	 * to registered callback functions.
	 *
	 * @pre The barometric sensor has been properly initialized by the system.
	 *
	 * @returns True if the request was successful, False if the request could not be successfully
	 *  enqueued (e.g., the queue is full).
	 */
	bool (*readSample)(void);

	/** Register a NewBarometricSampleCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a new and valid sample is available.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of "new sample" callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to register on the "new sample" callback list.
	 */
	void (*registerNewSampleCb)(const NewBarometricSampleCb callback);

	/** Remove a registered NewBarometricSampleCb function
	 *
	 * This function will remove a callback function from the registered list of
	 * "new sample" callbacks. If the function has not been previously registered,
	 * the parameter will be ignored and the list will be unchanged.
	 *
	 * @post callback function pointer is not present on the list of "new sample" callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to remove from the "new sample" callback list.
	 */
	void (*unregisterNewSampleCb)(const NewBarometricSampleCb callback);

	/** Register a BarometricErrorCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a pressure sensor error occurs.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of error callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to register on the "error" callback list.
	 */
	void (*registerErrorCb)(const BarometricErrorCb callback);

	/** Remove a registered BarometricErrorCb function
	 *
	 * This function will remove a callback function from the registered list of
	 * "error" callbacks. If the function has not been previously registered,
	 * the parameter will be ignored and the list will be unchanged.
	 *
	 * @post callback function pointer is not present on the list of "error" callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to remove from the "error" callback list.
	 */
	void (*unregisterErrorCb)(const BarometricErrorCb callback);
} BarometricSensor_asyncWithCb;
