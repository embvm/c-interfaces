// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file temperature_sensor.h
 * Example temperature sensor interface.
 *
 * This header defines the core interfaces for a virtual temperature sensor. Two samples
 * are provided:
 *
 * 1. A basic interface for reading temperature (TemperatureSensor)
 * 2. An interface expanded with support for callbacks (TemperatureSensor_withCb)
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type or format of the temperature value
 * - Return the temperature sample, and use a separate function to determine validity
 * - Supporting a purely asynchronous processing model (see barometric_sensor.h or
 *   asynchronous_interface.h for an example)
 *
 * ## Possible Humidity Specifications
 *
 * For the data format, we can consider a few options:
 * - floating point
 * - Provide a signed 16-bit fixed point integer in format Q7.8 (1 sign bit, 7 integral bits, and 8
 *   fractional bits). This gives us a range of [-128°C, 128°C] (well beyond the ranges typically
 *   supported by temperature sensors, and beyond the needs of our intended application). We end
 *   up with a resolution of 0.0039°C.
 * - Q8.7 would provide a greater range (±256°C) with still-more-than-sufficient precision
 *   (0.0078°C). However, this is still well beyond the temperature range of sensors we work with.
 * - If higher resolution as important (or 32-bit values are preferred for processing reasons),
 *   providing a 32-bit fixed point integer with format Q21.10, gives a resolution of 0.001°C.
 */

#pragma mark - Basic Interface -

/** Virtual Temperature Sensor Interface
 *
 * A standard interface for a device which can measure temperature.
 *
 * This device measures temperature in °C.
 *
 * ## Fundamental Assumptions
 * - The device produces temperature readings
 * - The device reports temperature readings in °C
 * - Temperature readings will be provided as a signed 16-bit fixed point integer in format Q7.8
 * - The device will indicate whether the current reading is valid or invalid
 *
 * ## Implementation Notes
 *
 * - This interface, at its core, appears to be blocking. However, you can still implement this
 *   interface in a non-blocking way. For example, you could always return the most recent
 *   measurement, while there is another thread (or a timer) that handles updates to that
 *   measurement asynchronously.
 *
 */
typedef struct
{
	/** Get the current temperature in °C
	 *
	 * @pre The sensor has been properly initialized by the system.
	 * @pre The temperature parameter is not NULL.
	 * @post If the measurement is valid, the data pointed to by the temperature parameter
	 *       will be updated with the latest reading.
	 * @post If the measurement is invalid, the data pointed to by the temperature parameter
	 * 		 will remain unchanged.
	 *
	 * @param[inout] Current temperature in °C.
	 *	Temperature readings will be provided as a signed 16-bit fixed point integer in format Q7.8
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*readTemperature)(int32_t* const temperature);
} TemperatureSensor;

#pragma mark - With Callback Support -

/** Callback function prototype for processing new temperature samples
 *
 * When a new (and valid) temperature sample is available, this callback
 * function will be invoked.
 *
 * The callback is not guaranteed to run on its own thread of control. We recommend
 * keeping the implementation small. Your function implementation could take the
 * new sample and perform some dispatching operation (e.g., add the value to a queue),
 * ensuring that any "heavy" processing happens on a new thread.
 *
 * @param[in] temperature The latest temperature sample.
 *
 *  Temperature readings will be provided as a signed 16-bit fixed point integer in format Q7.8.
 *
 */
typedef void (*NewTemperatureSampleCb)(int16_t temperature);

/** Callback function prototype for temperature sensor errors
 *
 * When an error in the virtual temperature device occurs, this callback function
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
typedef void (*TemperatureErrorCb)(void);

/** Virtual Temperature Sensor Interface with Callback Support
 *
 * A standard interface for a device which can measure temperature. Interested parties can receive
 * callbacks when new samples are available.
 *
 * This device measures temperature in °C.
 *
 * ## Fundamental Assumptions
 * - The device produces temperature readings
 * - The device reports temperature readings in °C
 * - Temperature readings will be provided as a signed 16-bit fixed point integer in format Q7.8
 * - The device will indicate whether the current reading is valid or invalid
 * - The device will notify interested parties when a new reading is available
 *
 * ## Undesired event assumptions
 * - If an error occurs internally, the virtual device will notify interested parties
 *   by issuing an error callback. The registered parties can take desired action
 *   when this occurs (e.g., attempt recovery, stop querying the sensor).
 *
 * ## Implementation Notes
 *
 * - This interface, at its core, appears to be blocking. However, you can still implement this
 *   interface in a non-blocking way. For example, you could always return the most recent
 *   measurement, while there is another thread that handles updates to that measurement
 *   asynchronously.
 * - Note that the callback registration functions do not support error handling.
 *   We recommend that implementers trigger an assert() or other crash if a callback
 *   cannot be added to a list due to exceeding fixed size constraints.
 */
typedef struct
{
	/** Get the current temperature in °C
	 *
	 * @pre The sensor has been properly initialized by the system.
	 * @post If the measurement is valid and the temperature parameter is not NULL, the data pointed
	 *       to will be updated with the latest reading
	 * @post If the measurement is invalid, the data pointed to by the temperature parameter
	 * 		 will remain unchanged.
	 * @post If the measurement is valid, registered New Sample callbacks will be invoked
	 * 		  or dispatched with the new measurement.
	 * @post If the measurement is not valid, registered Error callbacks will be invoked
	 * 		  or dispatched.
	 *
	 * @param[inout] temperature
	 *  Pointer which will be used for storing the latest temperature reading.
	 *
	 * 	If temperature is NULL, the function will only supply the temperature sample to
	 *  registered callback functions.
	 *
	 *  Temperature readings will be provided as a signed 16-bit fixed point integer in format Q7.8
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 *
	 * @param[inout] Current temperature in °C.
	 *	Temperature readings will be provided as a signed 16-bit fixed point integer in format Q7.8
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*readTemperature)(int32_t* const temperature);

	/** Register a NewTemperatureSampleCb function
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
	void (*registerNewSampleCb)(const NewTemperatureSampleCb callback);

	/** Remove a registered NewTemperatureSampleCb function
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
	void (*unregisterNewSampleCb)(const NewTemperatureSampleCb callback);

	/** Register a TemperatureErrorCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a temperature sensor error occurs.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of error callbacks.
	 *
	 * @param[in] callback
	 * 	The callback function pointer to register on the "error" callback list.
	 */
	void (*registerErrorCb)(const TemperatureErrorCb callback);

	/** Remove a registered TemperatureErrorCb function
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
	void (*unregisterErrorCb)(const TemperatureErrorCb callback);
} TemperatureSensor_withCb;
