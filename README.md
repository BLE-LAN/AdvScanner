## AdvScanner

A PoC Bluetooth Low Energy Advertisements Scanner (for windows).

## Dependencies

* [paramkit](https://github.com/hasherezade/paramkit/tree/39c7c0d0eacb03908c2ec728013e78f56254d515)
* [RapidSON](https://github.com/Tencent/rapidjson/tree/48fbd8cd202ca54031fe799db2ad44ffa8e77c13)

## Build

RapidJSON is a only header library, but paramkit must be build.

First clone the repo with submodules.

```
  git clonehttps://github.com/BLE-LAN/AdvScanner --recursive
```

Go to main paramkit directory and use cmake to generate the VS project or if u dont like the CLI option use CMake-GUI is good option.

```
 cd AdvScanner/paramkit/
 mkdir build
 cd build
 cmake ..
```

Now open the Visual Studio project generate in the build folder, then select the 'paramkit' project and build it in Relase/Debug mode.

¡ AdvScanner is ready to use !
