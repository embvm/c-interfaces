// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file humidity_sensor.h
 * Example relative humidity interface.
 *
 * This header defines the core interfaces for a virtual humdity altimeter.
 * The interface can be expanded in multiple ways. For a useful study in variations, such as the
 * use of callback functions and support for asynchronous processing, see barometric_sensor.h.
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type or format of the humidity value
 * - Return the humidity sample, and use a separate function to determine validity
 * - Using callbacks (see barometric_sensor.h or callback_function.h for an example)
 * - Supporting an asynchronous processing model (see barometric_sensor.h or
 *   asynchronous_interface.h for an example)
 *
 * ## Possible Humidity Specifications
 *
 * For data format, we can consider a few options:
 *
 * - Simply return an integral relative humidity (because the application does not care for higher
 *   resolution than that), rounded according to your preferred rounding rules.
 * - Use a float
 * - Use a 32-bit integer with fixed-point format UQ22.10
 *   (giving us a range of [0, ~4 million] and resolution of 0.001)
 * - Use a 16-bit integer with fixed-point format UQ7.9 (giving us a range of [0, 128]
 * and resolution of [0.002])
 */

/** Virtual Relative Humidity Sensor Interface
 *
 * A standard interface for a device which can measure relative humidity.
 *
 * This device returns relative humdity, rounded to the nearest whole percentage.
 *
 * ## Fundamental Assumptions
 * - The device produces relative humidity (RH) readings
 * - The device reports RH as a percentage
 * - The device will indicate whether the current reading is valid or invalid
 * - The device will notify interested parties when a new reading is available
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
	 * @param inout Current relative humidity in %.
	 *	Humidity is specified as an integral perecentage.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*getHumidity)(int8_t* const humidity);
} HumiditySensor;
