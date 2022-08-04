/*
## Creating a Basic Device Interface

Also show a 'base class' device interface (stop/start/restart)


The idea here is that all devices _probably_ need these basic interfaces for managing a device… only
useful if you want to manage devices generically though!

- start() the driver - power on and initialize
- stop() the driver - de-init and power off
- restart() the driver
- started() - is the driver started?
- DriverType() - get the virtual type of the driver
- id() or name()??? Not there now - but some way to identify the driver (`temp0`) - but maybe this
is just in the registry and not in the driver… because then we need twice the strings (Decision to
document)
*/
