## 4ms Company VCV Rack Modules

These are [4ms Company](https://4mscompany.com/) Eurorack modules for the [VCV Rack](https://vcvrack.com/) platform.

This also contains the MetaModule hub module, used for creating patches to run on the MetaModule hardware.

More information: [MetaModule website](https://metamodule.info)

### Building the VCV Rack Plugin

You must have the Rack-SDK on your computer already. Set the environment
variable `RACK_DIR` equal to the path to the location of Rack-SDK. For
instance, add this to your .bashrc or .zshrc:

```
export RACK_DIR=/Users/MyName/projects/Rack-SDK
```

You will need a recent c++ compiler such as gcc-12 or later, or clang-14 or later.

Make sure to clone all submodules:

```
git submodule update --init --recursive
```

To build and the plugin, run:

```
make dep
make dist
```

To install the plugin in your local VCV Rack plugin directory:

```
make install
```

The next time you start VCV Rack, it will load the modified plugin.

If you are developing or modifying the code, then use the cmake interface for building (it will handle dependencies and rebuild only what needs to):

```
# Configure cmake (only need to do this once):
make dep

# Then do this to rebuild after making changes:
cmake --build build

# Install normally:
make install

```

To run the unit tests:

```
make tests
```
