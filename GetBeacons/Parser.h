#pragma once

#include <../winrt/windows.devices.bluetooth.h>
using namespace ABI::Windows::Devices::Bluetooth::Advertisement;

// Six oct for the ble address xx:xx:xx:xx:xx:xx
#define BD_ADDR_LEN 6

namespace Parser 
{
	bool Parser(IBluetoothLEAdvertisementReceivedEventArgs* args);
}
