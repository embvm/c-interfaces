// SPDX-FileCopyrightText: © 2022 Embedded Artistry LLC <contact@embeddedartistry.com>
// SPDX-License-Identifier: MIT

#include <stdint.h>

/** Virtual Barometric Altitude Sensor Interface
*
* A standard interface for a device which can measure barometric pressure.
*
* This virtual device provides both pressure and altitude measurements.
*
* ## Fundamental Assumptions
*
* - This device produces barometric altitude readings
* - This device will report barometric altitude in meters
* - The device will compute altitude on a configurable value for Sea Level
*   Pressure supplied by the system. If no value is provided, an SLP of
*   1013.25 hPa will be assumed.
* - The device will be properly initialized by the system prior to any
*   module invoking functions provided by this interface.
*/
typedef struct {
	/** Get the current altitude
	*
	* If no value for SLP has been supplied, calculations will assume 1013.25 hPa.
	*
	* @returns Current altitude in meters (m), corrected for sea level pressure.
	*	Altitude is specified as a signed 32-bit fixed-point number in format Q21.10.
	*/
	int32_t (*getAltitude)();

	/** Set the sea level pressure
	*
	* @input slp The current sea level pressure in hPa.
	* 	slp should be specified as an unsigned 32-bit fixed-point number in format UQ22.10.
	*/
	void (*setSeaLevelPressure)(uint32_t slp);
} BaroAltimeter;
