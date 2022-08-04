// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file temperature_sensor.h
 * Example temperature sensor interface.
 *
 * This header defines the core interfaces for a virtual temperature sensor.
 * The interface can be expanded in multiple ways. For a useful study in variations, such as the
 * use of callback functions and support for asynchronous processing, see barometric_sensor.h.
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type or format of the temperature value
 * - Return the temperature sample, and use a separate function to determine validity
 * - Using callbacks (see barometric_sensor.h or callback_function.h for an example)
 * - Supporting an asynchronous processing model (see barometric_sensor.h or
 *   asynchronous_interface.h for an example)
 *
 * ## Possible Humidity Specifications
 *
 * For the data format, we can consider a few options:
 * - floating point
 * - Provide a signed 16-bit fixed point integer with Q7.8 (1 sign bit, 7 integral bits, and 8
 *   fractional bits). This gives us a range of [-128°C, 128°C] (well beyond the ranges typically
 *   supported by temperature sensors, and beyond the needs of our intended application). We end
 *   up with a resolution of 0.0039°C.
 * - Q8.7 would provide a greater range (±256°C) with still-more-than-sufficient precision
 *   (0.0078°C). However, this is still well beyond the temperature range of sensors we work with.
 * - If higher resolution as important (or 32-bit values are preferred for processing reasons),
 *   providing a 32-bit fixed point integer with format Q21.10, gives a resolution of 0.001°C.
 */

/** Virtual Temperature Sensor Interface
 *
 * A standard interface for a device which can measure temperature.
 *
 * This device measures temperature in °C.
 *
 * ## Fundamental Assumptions
 * - The device produces temperature readings
 * - The device reports temperature readings in °C
 * - The device will indicate whether the current reading is valid or invalid
 * - The device will notify interested parties when a new reading is available
 *
 * ## Implementation Notes
 *
 * - This interface, at its core, appears to be blocking. However, you can still implement this
 *   interface in a non-blocking way. For example, you could always return the most recent altitude
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
	 * @param inout Current temperature in °C.
	 *	Humidity is specified as an integral perecentage.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*getTemperature)(int32_t* const temperature);
} TemperatureSensor;
