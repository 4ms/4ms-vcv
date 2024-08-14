## 4ms Company VCV Rack Modules

These are the [4ms Company's](https://4mscompany.com/) Eurorack modules for the [VCV Rack](https://vcvrack.com/) platform.

There is also the MetaModule hub, used for creating patches to run on the MetaModule.

### Building VCV Rack Plugin

You must have the Rack-SDK on your computer already. Set the environment
variable `RACK_DIR` equal to the path to the location of Rack-SDK. For
instance, add this to your .bashrc or .zshrc:

```
export RACK_DIR=/Users/MyName/projects/Rack-SDK
```

You will need a recent c++ compiler such as gcc-12 or later, or clang-14 or later.


To build the plugin, run:

```
make install
```

This will create the plugin file and install it in your local VCV Rack plugin
directory. The next time you start VCV Rack, it will load the modified plugin.


To run the unit tests:

```
make tests
```
