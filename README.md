# fpga-lmc

This is an adapter piece of software that consumes Leap Media Controller data
and outputs it on a serial port.

## Building

This software can only be built on MSVC and requires Windows. Something like
the following should work (in `x64 Native Tools CMD`):

```bat
cd fpga-lmc
mkdir build
cd build
cmake ..
msbuild fpga-lmc.sln
```

If you want to build for release, running:

```bat
cd fpga-lmc
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
msbuild fpga-lmc.sln
```

should work.

The output binary will be in `build/Release`.
