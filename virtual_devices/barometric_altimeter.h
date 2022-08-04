// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file barometric_altimeter.h
 * Example barometric altimeter interface.
 *
 * This header defines the core interfaces for a virtual barometric altimeter.
 * The interface can be expanded in multiple ways. For a useful study in variations, such as the
 * use of callback functions and support for asynchronous processing, see barometric_sensor.h.
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type of the altitude parameter (e.g., to a float)
 * - Change the fixed-point format
 * - Return the altitude sample, and use a separate function to determine validity
 * - Using callbacks (see barometric_sensor.h or callback_function.h for an example)
 * - Supporting an asynchronous processing model (see barometric_sensor.h or
 *   asynchronous_interface.h for an example)
 */

/** Virtual Barometric Altimeter Sensor Interface
 *
 * A standard interface for a device which can compute altitude by measuring barometric pressure.
 * This virtual device provides both pressure and altitude measurements.
 *
 * ## Fundamental Assumptions
 *
 * - The device produces barometric altitude readings
 * 	- This device reports barometric altitude in meters
 *    - Altitude will be corrected for Sea Level Pressure. If no value for SLP has been supplied,
 *      calculations will assume 1013.25 hPa.
 * - Altitude will be formatted as a 32-bit fixed-point integer with format Q21.10,
 *   giving a resolution of 0.001 m.
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
	/** Get the current altitude, corrected for SLP
	 *
	 * If no value for SLP has been supplied, calculations will assume 1013.25 hPa.
	 *
	 * @pre The altimeter has been properly initialized by the system.
	 * @pre The altitude parameter is not NULL.
	 * @post If the measurement is valid, the data pointed to by the altitude parameter
	 *       will be updated with the latest reading.
	 * @post If the measurement is invalid, the data pointed to by the altitude parameter
	 *       will remain unchanged.
	 *
	 * @param inout Current altitude in meters (m), corrected for sea level pressure.
	 *	Altitude is specified as a signed 32-bit fixed-point number in format Q21.10.
	 *
	 * @returns True if the sample is valid, false if invalid (e.g., an error occured)
	 */
	bool (*getAltitude)(int32_t* const altitude);

	/** Set the sea level pressure
	 *
	 * @param in slp The current sea level pressure in hPa.
	 * 	slp should be specified as an unsigned 32-bit fixed-point number in format UQ22.10.
	 */
	void (*setSeaLevelPressure)(const uint32_t slp);
} BarometricAltimeter;
