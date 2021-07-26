#pragma once

#include <../winrt/windows.devices.bluetooth.advertisement.h>

using namespace ABI::Windows::Devices::Bluetooth::Advertisement;

namespace Watcher {
    struct CallBackObject
    {
        CallBackObject() {};
        HRESULT AdvertisementRecived(IBluetoothLEAdvertisementWatcher* watcher, IBluetoothLEAdvertisementReceivedEventArgs* args);
    };

    void Run(int miliseconds);
    
    int WatchADV(int miliseconds);

    void CreateFileOutput();
}


