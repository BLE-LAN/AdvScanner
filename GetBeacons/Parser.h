#pragma once

#include <../winrt/windows.devices.bluetooth.h>

// Six oct ble address xx:xx:xx:xx:xx:xx
#define BD_ADDR_LEN 6

namespace Parser
{
	bool Parser(
		ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementReceivedEventArgs* args, 
		char* buffer, UINT32 size
	);
}
