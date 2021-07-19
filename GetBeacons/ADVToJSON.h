#pragma once

#include <../winrt/windows.devices.bluetooth.h>
using namespace ABI::Windows::Devices::Bluetooth::Advertisement;

namespace ADVToJSON {
	bool Parse(IBluetoothLEAdvertisementReceivedEventArgs* args);
}

