## AdvScanner

A PoC Bluetooth Low Energy Advertisements Scanner (for windows). During a scan each ADV is parse to JSON and stored in a new life of a .txt file.

## Dependencies

* [paramkit](https://github.com/hasherezade/paramkit/tree/39c7c0d0eacb03908c2ec728013e78f56254d515)
* [RapidSON](https://github.com/Tencent/rapidjson/tree/48fbd8cd202ca54031fe799db2ad44ffa8e77c13)

## Build

RapidJSON is a only header library, but paramkit must be build.

First clone the repo with submodules.

```
  git clonehttps://github.com/BLE-LAN/AdvScanner --recursive
```

Go to main paramkit directory and use cmake to generate the VS project. If u dont like the CLI the CMake-GUI is a nice option.

```
 cd AdvScanner/paramkit/
 mkdir build
 cd build
 cmake ..
```

Now open the Visual Studio project generated in the build folder, then select the 'paramkit' project and build it in Release/Debug mode.

¡ AdvScanner is ready to use !

## Demo

This is an example of a release run. Filed args are printed before the scanner.

![Run Ok](https://github.com/BLE-LAN/AdvScanner/blob/main/readme_resources/ok_run.png)

If no args are given or is /help, the args usage will be printed.

![Run without params](https://github.com/BLE-LAN/AdvScanner/blob/main/readme_resources/help_run.png)

In debub mode each parsed package is printed.

![Run in debug mode](https://github.com/BLE-LAN/AdvScanner/blob/main/readme_resources/debug_run.png)
