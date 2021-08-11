#pragma once

#include <../winrt/windows.devices.bluetooth.advertisement.h>

namespace Adv = ABI::Windows::Devices::Bluetooth::Advertisement;

namespace Watcher {
    struct CallBackObject
    {
        CallBackObject() {};
        HRESULT AdvertisementRecived(
            Adv::IBluetoothLEAdvertisementWatcher* watcher, 
            Adv::IBluetoothLEAdvertisementReceivedEventArgs* args
        );
    };

    void Run(unsigned int miliseconds, std::string outputFile);
    
    int WatchADV(unsigned int miliseconds);
}


