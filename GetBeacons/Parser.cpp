/*
    ##########################
	## Parser de ADV a JSON
    ## IBluetoothLEAdvertisementReceivedEventArgs => JSON
    ##########################

    IBluetoothLEAdvertisement Advertisement
        DataTypes (los que se han podido testar)
            flags                       0x01
            16-bit UUIDs                0x03
            Complete local name         0x09
            Manufacturer Specific Data  0xFF
*/


#include <iostream>

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <hstring.h>

#include "Parser.h"
#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/writer.h"
#include "../rapidjson/include/rapidjson/stringbuffer.h"



using namespace std;
using namespace Microsoft::WRL;
using namespace rapidjson;
using namespace Microsoft::WRL::Wrappers;


char _bluetoothLEAdvertisementStrings[6][27] = {
    "ConnectableUndirected",
    "ConnectableDirected",
    "ScannableUndirected",
    "NonConnectable",
    "ScanResponse",
    "Extended"
};

void addressGetBytesInBigEndian(UINT8 bda[], UINT64* bluetoothAddress)
{
    BYTE* p = (BYTE*)bluetoothAddress;
    UINT8 *pbda = bda + BD_ADDR_LEN - 1; 

    for (size_t i = 0; i < BD_ADDR_LEN; i++) 
    {
        *pbda-- = *p++;
    }
}

void addressParse(IBluetoothLEAdvertisementReceivedEventArgs* args, Document& document) {
    UINT64 bluetoothAddress;
    args->get_BluetoothAddress(&bluetoothAddress);

    UINT8 bda[BD_ADDR_LEN];
    char buf[64] = { 0 };

    addressGetBytesInBigEndian(bda, &bluetoothAddress);

    // Formar the string into a buffer
    unsigned int size = sprintf_s(buf, sizeof(buf) / sizeof(buf[0]), "%02x:%02x:%02x:%02x:%02x:%02x", bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

    Value v_address;
    v_address.SetString(buf, size, document.GetAllocator());
    document.AddMember("address", v_address, document.GetAllocator());
}

void adtypeParse(IBluetoothLEAdvertisementReceivedEventArgs* args, Document& document){
    BluetoothLEAdvertisementType advtype;
    args->get_AdvertisementType(&advtype);

    Value v_advtype;
    v_advtype.SetString(_bluetoothLEAdvertisementStrings[advtype], strlen(_bluetoothLEAdvertisementStrings[advtype]), document.GetAllocator());
    document.AddMember("advtype", v_advtype, document.GetAllocator());
}

void rawsignalParse(IBluetoothLEAdvertisementReceivedEventArgs* args, Document& document) {
    INT16 dbm;
    args->get_RawSignalStrengthInDBm(&dbm);

    Value v_dbm;
    v_dbm.SetInt(dbm);
    document.AddMember("RSSI", v_dbm, document.GetAllocator());
}

void timestampParse(IBluetoothLEAdvertisementReceivedEventArgs* args, Document& document) 
{
    time_t result = time(NULL);
    char dateBuff[26];
    ctime_s(dateBuff, sizeof dateBuff, &result);

    Value v_timestamp;
    // ctime_s add a '\n' at the of the string, thats the -1
    v_timestamp.SetString(dateBuff, strlen(dateBuff)-1, document.GetAllocator());
    document.AddMember("timestamp", v_timestamp, document.GetAllocator());
}


void localNameParse(ComPtr<IBluetoothLEAdvertisement> bleAdvert, Document& document)
{
    // Get Name of the device
    HString name;
    char buff_name[128] = { 0 };
    
    bleAdvert->get_LocalName(name.GetAddressOf());
   
    if (wcslen(name.GetRawBuffer(nullptr)) != 0) {
        sprintf_s(buff_name, sizeof(buff_name), "%S", name.GetRawBuffer(nullptr));

        Value v_localname;
        v_localname.SetString(buff_name, strlen(buff_name), document.GetAllocator());
        document.AddMember("completename", v_localname, document.GetAllocator());
    }
}

bool Parser::Parser(IBluetoothLEAdvertisementReceivedEventArgs* args) 
{
    Document document;
    document.SetObject();

    /*
        Values not stored in the data section
    */

    addressParse(args, document);

    adtypeParse(args, document);

    rawsignalParse(args, document);

    timestampParse(args, document);
    
    /*
        Data Section
    */

    // get_Flags 0x01
    // get_ServiceUuids 0x03
    // get_ManufacturerData 0xff

    ComPtr<IBluetoothLEAdvertisement> bleAdvert;
    args->get_Advertisement(&bleAdvert);

    localNameParse(bleAdvert, document);

    HRESULT hr;

    ComPtr <ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection*>> vecData;
     hr = bleAdvert->get_DataSections(&vecData);

    char auxBuff[256] = { 0 };
    unsigned int countDataSections = 0;
    hr = vecData->get_Size(&countDataSections);

    for (unsigned int i = 0; i < countDataSections; ++i)
    {
        ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> significantPart;
        hr = vecData->GetAt(i, &significantPart);

        ComPtr<ABI::Windows::Storage::Streams::IBuffer> ibuf;
        
        BYTE datatype = 0;
        hr = significantPart->get_DataType(&datatype);

        // 

        memset(auxBuff, 0, sizeof(auxBuff));
        sprintf_s(auxBuff, sizeof(auxBuff), "%X", datatype);
        printf("Data Type: %s ", auxBuff);

       // hr = significantPart->get_Data(&ibuf);
    }


    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    document.Accept(writer);
    std::cout << "\n" << buffer.GetString() << std::endl;

    return true;
}

