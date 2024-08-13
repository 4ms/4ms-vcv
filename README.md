This folder contains emulations of [4ms Company's](https://4mscompany.com/) Eurorack modules for the [VCV Rack](https://vcvrack.com/) platform.

Make sure to install all prerequisites as described on the [Setup guide](../docs/Setup.md).

### Building VCV Rack Plugin

You must have the Rack-SDK on your computer already. Version 2.4.1 is known to
work. Set the environment variable `RACK_DIR` equal to the path to the
location of Rack-SDK. For instance, add this to your .bashrc or .zshrc:

```
export RACK_DIR=/Users/MyName/projects/Rack-SDK
```

You will need a recent c++ compiler such as gcc-12 or later, or clang-14 or later.

To run the unit tests:

```
make tests
```

To build the plugin, run:

```
make -j16 install
```

This will create the plugin file and install it in your local VCV Rack plugin
directory. The next time you start VCV Rack, it will load the modified plugin.
