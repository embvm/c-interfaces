// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdint.h>

/** @file barometric_pressure_sensor.h
 * Example barometric pressure sensor interface.
 *
 * This header defines the core interfaces for a virtual barometric pressure sensor.
 * The interface can be expanded in multiple ways. For a useful study in variations, such as the
 * use of callback functions and support for asynchronous processing, see barometric_sensor.h.
 *
 * ## Modifying the Interfaces
 *
 * There are a number of ways you might modify this interface to suit your needs:
 *
 * - Change the type of the barometric pressure parameter (e.g., to a float)
 * - Change the fixed-point format
 * - Return the barometric pressure sample, and use a separate function to determine validity
 * - Using callbacks (see barometric_sensor.h or callback_function.h for an example)
 * - Supporting an asynchronous processing model (see barometric_sensor.h or
 *   asynchronous_interface.h for an example)
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
 * - This device reports barometric pressure in hectopascal (hPa)
 * - The reported barometric pressure reading will be compensated for ambient temperature
 *   by the implementation if it is required.
 * - Pressure will be formatted as a 32-bit fixed-point integer with format UQ22.10,
 *   giving a resolution of 0.001 hPa.
 * - The device will indicate whether the current sample is valid or invalid
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
} BarometricPressureSensor;
