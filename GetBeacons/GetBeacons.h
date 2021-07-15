#pragma once

#include <../winrt/windows.devices.bluetooth.advertisement.h>

using namespace ABI::Windows::Devices::Bluetooth::Advertisement;

struct CallBackObject
{
public:
    CallBackObject() {};
    HRESULT AdvertisementRecived(IBluetoothLEAdvertisementWatcher* watcher, IBluetoothLEAdvertisementReceivedEventArgs* args);
};
