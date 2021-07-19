/*
    ##########################
	## Parser de ADV a JSON
    ## IBluetoothLEAdvertisementReceivedEventArgs => JSON
    ##########################

    INT16 RawSignalStrengthInDBm => int

    UINT64 BluetoothAddress => String
        XX:XX:XX:XX:XX:XX

    BluetoothLEAdvertisementType AdvertisementType => Enum de unsigned int
        ConnectableDirected	        1
        ConnectableUndirected	    0
        Extended	                5
        NonConnectableUndirected	3
        ScannableUndirected	        2
        ScanResponse	            4
                        
    IBluetoothLEAdvertisement Advertisement
        DataTypes (los que se han podido testar)
            flags                       0x01
            16-bit UUIDs                0x03
            Complete local name         0x09
            Manufacturer Specific Data  0xFF
*/

#include <iostream>
#include <stdio.h>

#include <Windows.Foundation.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <../winrt/windows.devices.bluetooth.h>

#include "ADVToJSON.h"

using namespace std;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;

bool ADVToJSON::Parse(IBluetoothLEAdvertisementReceivedEventArgs* args) 
{
    ComPtr<IBluetoothLEAdvertisement> bleAdvert;
    args->get_Advertisement(&bleAdvert);

    ComPtr <ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection*>> vecData;
    HRESULT hr = bleAdvert->get_DataSections(&vecData);

    char auxBuff[256] = { 0 };
    unsigned int countDataSections = 0;
    hr = vecData->get_Size(&countDataSections);

    for (auto i = 0; i < countDataSections; ++i)
    {
        ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> dataSection;
        hr = vecData->GetAt(i, &dataSection);

        ComPtr<ABI::Windows::Storage::Streams::IBuffer> ibuf;
        BYTE datatype = 0;

        hr = dataSection->get_DataType(&datatype);
        memset(auxBuff, 0, sizeof(auxBuff));
        sprintf_s(auxBuff, sizeof(auxBuff), "%d", datatype);
        printf("Data Type: %s ", auxBuff);

        hr = dataSection->get_Data(&ibuf);
    }

    UINT64 address;
    args->get_BluetoothAddress(&address);

    INT16 dbm;
    args->get_RawSignalStrengthInDBm(&dbm);

    printf("addr = > %llX, DBm => %d\n", address, dbm);

    return true;
}