# Template Methods

The items in this folder represent "steps" in a Template Method pattern (link!) that are commonly used for decoupling purposes.

For example

- SPI transfers - if your device interacts over SPI, it can define this template method. Now your driver can work with any SPI API - you just have to supply an implementation for your platform from the outside.
- I2C - if your device interacts over I
