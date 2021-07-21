#pragma once

#include <../winrt/windows.devices.bluetooth.h>
using namespace ABI::Windows::Devices::Bluetooth::Advertisement;

namespace Parser 
{
	bool Parser(IBluetoothLEAdvertisementReceivedEventArgs* args);
	void BluetoothAddressToString(UINT8 bda[], UINT64* btha);
	unsigned int AdvertisementTypeToString(BluetoothLEAdvertisementType type, char* name);
}
