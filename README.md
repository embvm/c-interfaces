# Abstract Interfaces Written in C

This is a collection of abstract interfaces written in C. You can view this collection in multiple ways:

- A ongoing study in interface design and documentation
- A reference library of abstract interfaces we've found useful
- A source of inspiration for creating your own interfaces
- Examples of how the same basic interface concept can be varied to support different use cases (e.g., blocking vs non-blocking)
- Example interfaces you can take, modify, and use on your own projects

This collection is intended to serve as a reference only. The interfaces are subject to change. Do not rely on this repository remaining fixed - no guarantees are provided. If you need something, make a copy!

## Organization

- [virtual_devices](virtual_devices/) contains abstract interfaces that can be mapped onto hardware devices.

## Interface Conventions

In general, our interface design adheres to the following conventions:

- Interface types are nouns, and the first letter of the name will be capitalized (e.g., as in German)
- Function names are verbs that describe the action taken. This means we prefer the use of `getTemperature()` over `temperature()`.
- We have tried to be specific: `temp0.getTemperature()` is preferred over `temp0.read()`.
	* There are examples in this repository that provide multiple values (such as the [barometric_sensor](virtual_devices/barometric_sensor.h), which can produce both *pressure* and *altitude*), and as such we have opted for more specific naming across the board.
	+ We think it is reasonable to use something like `read` as the method name when accessing data from sensors. For example, you might have a consistent sensor "base" interface that expects `read` to be defined for all sensor types. This is a suitable approach, and these interfaces can be easily modified to support such cases.
	
## Interface Scope

In general, we have opted to keep the interfaces as simple as possible. You will probably see interfaces with fewer functions and parameters than you might expect. Smaller interfaces are much easier to understand, implement, and use. Our goal is also to abstract _only the common functionality required by general application code_ - code that is designed to be independent of any particular hardware platform, OS, or SDK.

We have generally left out initialization, configuration, and special operating modes. In our view, these details are hard to generalize (since each component, library, driver, etc. has slightly different options), and general application code does not need to access this information to do its job. Some part of the system will always be responsible for instantiating the specific implementation and mapping the implementation to an abstract interface. That part of the system can use the implementation's APIs to put the component in the system's expected state during initialization. That expectation means we can leave out a number of details.

Because we have tried to keep the interfaces as lean as possible, we have focused on the core idea for each type of interface. These interfaces can naturally be extended in many ways. Whenever we find a reusable interface pattern, we will note it here. Do not feel constrained by our lean focus - free to compose, edit, and tune these interfaces to suit the needs of _your_ system!

## Abstract Interface Approach

The abstract interfaces we specify here are generally going to be `struct`s of function pointers.

```c
typedef struct {
	/** Get the current altitude
	*
	* If no value for SLP has been supplied, calculations will assume 1013.25 hPa.
	*
	* @returns Current altitude in meters (m), corrected for sea level pressure.
	*	Altitude is specified as a signed 32-bit fixed-point number in format Q21.10.
	*/
	int32_t (*altitude)();

	/** Set the sea level pressure
	*
	* @input slp The current sea level pressure in hPa.
	* 	slp should be specified as an unsigned 32-bit fixed-point number in format UQ22.10.
	*/
	void (*seaLevelPressure)(uint32_t slp);
} BaroAltimeter;
```

You would need to create your own functions with the same prototypes as those described by the `struct` function pointers. Then, you declare an instance of the interface `struct` and specify your functions as the values for the member variables.

```c
const BaroAltimeter alt0 =
{
	BME280_alt0_altitude,
	BME280_alt0_seaLevelPressure,
};
```

You invoke the desired function by accessing the appropriate member variable:

```c
int32_t current_altitude = alt0.getAltitude();
```

This basic approach can be extended to support inheritance and polymorphism. For more information, see ["Technique: Inheritance and Polymorphism in C"](https://embeddedartistry.com/fieldatlas/technique-inheritance-and-polymorphism-in-c/).

## Further Reading

For more on interface design, abstraction, and decoupling:

- [Designing Embedded Software for Change](https://embeddedartistry.com/course/designing-embedded-systems-for-change/)
- [Abstractions and Interfaces](https://embeddedartistry.com/course/abstractions-and-interfaces/)
- [Information Hiding](https://embeddedartistry.com/fieldmanual-terms/information-hiding/)

For more on C-specific techniques:

- [Technique: Objects in C](https://embeddedartistry.com/fieldatlas/technique-objects-in-c/)
- [Technique: Encapsulation and Information Hiding in C](https://embeddedartistry.com/fieldatlas/encapsulation-and-information-hiding-in-c/)
- [Technique: Inheritance and Polymorphism in C](https://embeddedartistry.com/fieldatlas/technique-inheritance-and-polymorphism-in-c/)
