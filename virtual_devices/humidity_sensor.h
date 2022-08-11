// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file humidity_sensor.h
 * Example relative humidity interface.
 *
 * This header defines the core interfaces for a virtual humdity. Two samples
 * are provided:
 *
 * 1. A basic interface for reading humidity (HumiditySensor)
 * 2. An interface expanded with support for callbacks (HumiditySensor_withCb)
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type or format of the humidity value
 * - Return the humidity sample, and use a separate function to determine validity
 * - Supporting a purely asynchronous processing model (see barometric_sensor.h or
 *   asynchronous_interface.h for an example)
 *
 * ## Possible Humidity Specifications
 *
 * For data format, we can consider a few options:
 *
 * - Simply return an integral relative humidity (because the application does not care for higher
 *   resolution than that), rounded according to your preferred rounding rules.
 * - Use a float
 * - Use a 32-bit integer with fixed-point format UQ22.10 or UQ16.16
 *   (giving us a range of [0, ~4 million] and resolution of 0.001)
 * - Use a 16-bit integer with fixed-point format UQ7.9 (giving us a range of [0, 128]
 * and resolution of [0.002])
 */

#pragma mark - Basic Interface -

/** Virtual Relative Humidity Sensor Interface
 *
 * A standard interface for a device which can measure relative humidity.
 *
 * This device returns relative humdity, rounded to the nearest whole percentage.
 *
 * ## Fundamental Assumptions
 * - The device produces relative humidity (RH) readings
 * - The device reports RH as a percentage
 * - The reported RH reading will be compensated for ambient temperature
 *   by the implementation if it is required.
 * - The device will indicate whether the current reading is valid or invalid
 *
 * ## Implementation Notes
 *
 * - This interface, at its core, appears to be blocking. However, you can still implement this
 *   interface in a non-blocking way. For example, you could always return the most recent altitude
 *   measurement, while there is another thread (or a timer) that handles updates to that
 *   measurement asynchronously.
 */
typedef struct
{
	/** Get the current relative humidity
	 *
	 * @pre The sensor has been properly initialized by the system.
	 * @pre The humidity parameter is not NULL.
	 * @post If the measurement is valid, the data pointed to by the humidity parameter
	 *       will be updated with the latest reading.
	 * @post If the measurement is invalid, the data pointed to by the humidity parameter
	 *       will remain unchanged.
	 *
	 * @param[inout] Current relative humidity in %.
	 *	Humidity is specified as an integral perecentage.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*getHumidity)(uint8_t* const humidity);
} HumiditySensor;

#pragma mark - With Callbacks -

#pragma mark - With Callback Support -

/** Callback function prototype for processing new humidity samples
 *
 * When a new (and valid) humidity sample is available, this callback
 * function will be invoked.
 *
 * The callback is not guaranteed to run on its own thread of control. We recommend
 * keeping the implementation small. Your function implementation could take the
 * new sample and perform some dispatching operation (e.g., add the value to a queue),
 * ensuring that any "heavy" processing happens on a new thread.
 *
 * @param[in] humidity The latest humidity sample.
 *  Humidity is specified as an integral perecentage.
 */
typedef void (*NewHumiditySampleCb)(uint8_t humidity);

/** Callback function prototype for humidity sensor errors
 *
 * When an error in the virtual humidity device occurs, this callback function
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
typedef void (*HumidityErrorCb)(void);

/** Virtual Relative Humidity Sensor Interface with Callback Support
 *
 * A standard interface for a device which can measure relative humidity. Interested parties can
 * receive callbacks when new samples are available.
 *
 * This device returns relative humdity, rounded to the nearest whole percentage.
 *
 * ## Fundamental Assumptions
 * - The device produces relative humidity (RH) readings
 * - The device reports RH as a percentage
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
 *   interface in a non-blocking way. For example, you could always return the most recent humidity
 *   measurement, while there is another thread that handles updates to that measurement
 *   asynchronously.
 * - Note that the callback registration functions do not support error handling.
 *   We recommend that implementers trigger an assert() or other crash if a callback
 *   cannot be added to a list due to exceeding fixed size constraints.
 */
typedef struct
{
	/** Get the current relative humidity
	 *
	 * @pre The sensor has been properly initialized by the system.
	 * @post If the measurement is valid and the humidity parameter is not NULL, the data pointed
	 *       to will be updated with the latest reading
	 * @post If the measurement is invalid, the data pointed to by the humidity parameter
	 *       will remain unchanged.
	 * @post If the measurement is valid, registered New Sample callbacks will be invoked
	 *        or dispatched with the new measurement.
	 * @post If the measurement is not valid, registered Error callbacks will be invoked
	 *        or dispatched.
	 *
	 * @param[inout] humidity Current relative humidity in %.
	 *  If humidity is NULL, the function will only supply the humidity sample to
	 *  registered callback functions.
	 *
	 *  Humidity is specified as an integral perecentage.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*readHumidity)(uint8_t* const humidity);

	/** Register a NewHumiditySampleCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a new and valid sample is available.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of "new sample" callbacks.
	 *
	 * @param[in] callback
	 *  The callback function pointer to register on the "new sample" callback list.
	 */
	void (*registerNewSampleCb)(const NewHumiditySampleCb callback);

	/** Remove a registered NewHumiditySampleCb function
	 *
	 * This function will remove a callback function from the registered list of
	 * "new sample" callbacks. If the function has not been previously registered,
	 * the parameter will be ignored and the list will be unchanged.
	 *
	 * @post callback function pointer is not present on the list of "new sample" callbacks.
	 *
	 * @param[in] callback
	 *  The callback function pointer to remove from the "new sample" callback list.
	 */
	void (*unregisterNewSampleCb)(const NewHumiditySampleCb callback);

	/** Register a HumidityErrorCb function
	 *
	 * This function will add the callback input to a list of functions to execute
	 * when a humidity sensor error occurs.
	 *
	 * @pre callback is not NULL
	 * @post callback is added to the list of error callbacks.
	 *
	 * @param[in] callback
	 *  The callback function pointer to register on the "error" callback list.
	 */
	void (*registerErrorCb)(const HumidityErrorCb callback);

	/** Remove a registered HumidityErrorCb function
	 *
	 * This function will remove a callback function from the registered list of
	 * "error" callbacks. If the function has not been previously registered,
	 * the parameter will be ignored and the list will be unchanged.
	 *
	 * @post callback function pointer is not present on the list of "error" callbacks.
	 *
	 * @param[in] callback
	 *  The callback function pointer to remove from the "error" callback list.
	 */
	void (*unregisterErrorCb)(const HumidityErrorCb callback);
} HumiditySensor_withCb;
