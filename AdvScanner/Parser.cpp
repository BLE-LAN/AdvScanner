
#include "Parser.h"
namespace Advertisement = ABI::Windows::Devices::Bluetooth::Advertisement;

#include <iostream>

#include "../rapidjson/include/rapidjson/document.h"
#include "../rapidjson/include/rapidjson/writer.h"
#include "../rapidjson/include/rapidjson/stringbuffer.h"
namespace rj = rapidjson;

#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <hstring.h>
#include <windows.storage.h>
#include <Robuffer.h>
using namespace Microsoft::WRL;



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

void addressParse(
    Advertisement::IBluetoothLEAdvertisementReceivedEventArgs* args, 
    rj::Document& document) 
{
    UINT64 bluetoothAddress;
    args->get_BluetoothAddress(&bluetoothAddress);

    UINT8 bda[BD_ADDR_LEN];
    char buf[64] = { 0 };

    addressGetBytesInBigEndian(bda, &bluetoothAddress);

    // Formar the string into a buffer
    unsigned int size = sprintf_s(
        buf, sizeof(buf) / sizeof(buf[0]), "%02X:%02X:%02X:%02X:%02X:%02X", bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

    rj::Value v_address;
    v_address.SetString(buf, size, document.GetAllocator());
    document.AddMember("address", v_address, document.GetAllocator());
}

void advtypeParse(
    Advertisement::IBluetoothLEAdvertisementReceivedEventArgs* args, 
    rj::Document& document)
{
    Advertisement::BluetoothLEAdvertisementType advtype;
    args->get_AdvertisementType(&advtype);

    rj::Value v_advtype;
    v_advtype.SetString(_bluetoothLEAdvertisementStrings[advtype], strlen(_bluetoothLEAdvertisementStrings[advtype]), document.GetAllocator());
    document.AddMember("advtype", v_advtype, document.GetAllocator());
}

void rawsignalParse(
    Advertisement::IBluetoothLEAdvertisementReceivedEventArgs* args, 
    rj::Document& document) 
{
    INT16 dbm;
    args->get_RawSignalStrengthInDBm(&dbm);

    rj::Value v_dbm;
    v_dbm.SetInt(dbm);
    document.AddMember("rssi", v_dbm, document.GetAllocator());
}

void timestampParse(
    Advertisement::IBluetoothLEAdvertisementReceivedEventArgs* args,
    rj::Document& document)
{
    time_t result = time(NULL);
    char dateBuff[26];
    ctime_s(dateBuff, sizeof dateBuff, &result);

    rj::Value v_timestamp;
    // ctime_s add a '\n' at the of the string, thats the -1
    v_timestamp.SetString(dateBuff, strlen(dateBuff)-1, document.GetAllocator());
    document.AddMember("timestamp", v_timestamp, document.GetAllocator());
}


void localNameParse(ComPtr<Advertisement::IBluetoothLEAdvertisement> bleAdvert, rj::Document& document)
{
    // Get Name of the device
    Microsoft::WRL::Wrappers::HString name;
    char buff_name[128] = { 0 };
    
    bleAdvert->get_LocalName(name.GetAddressOf());
   
    if (wcslen(name.GetRawBuffer(nullptr)) != 0) {
        sprintf_s(buff_name, sizeof(buff_name), "%S", name.GetRawBuffer(nullptr));

        rj::Value v_localname;
        v_localname.SetString(buff_name, strlen(buff_name), document.GetAllocator());
        document.AddMember("completename", v_localname, document.GetAllocator());
    }
}

void servicesuidParse(
    ComPtr<Advertisement::IBluetoothLEAdvertisement> bleAdvert, 
    rj::Document& document)
{
    ComPtr<ABI::Windows::Foundation::Collections::IVector<GUID>> vecGuid;
    bleAdvert->get_ServiceUuids(&vecGuid);

    UINT guidCount = 0;
    vecGuid->get_Size(&guidCount);

    if (guidCount == 0) { return; }

    char buff[256] = { 0 };
    rj::Value v_array(rj::kArrayType);
    rj::Value v_guid(rj::kStringType);

    for (unsigned int i = 0; i < guidCount; ++i)
    {
        GUID guid = { 0 };
        vecGuid->GetAt(i, &guid);

        WCHAR szService[8] = { 0 };
        swprintf(szService, 8, L"%04X", guid.Data1);
        sprintf_s(buff, sizeof(buff), "%S", szService);

        v_guid.SetString(buff, strlen(buff), document.GetAllocator());
        v_array.PushBack(v_guid, document.GetAllocator());
    }

    document.AddMember("uuids", v_array, document.GetAllocator());
}

/*
    "unknowns":[{"type":"0x00", “raw”:"57 01 02 ff"}, ...]
*/
void unknowdataParse(
    ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> dataSection, 
    rj::Document& document,
    rj::Value& unknowns)
{
    rj::Value v_object(rj::Type::kObjectType);
    rj::Value v_type(rj::Type::kStringType);
    rj::Value v_raw(rj::Type::kStringType);

    char buff[256] = { 0 };

    ComPtr<ABI::Windows::Storage::Streams::IBuffer> ibuf;
    BYTE datatype = 0;

    dataSection->get_DataType(&datatype);
    memset(buff, 0, sizeof(buff));
    sprintf_s(buff, sizeof(buff), "%X", datatype);
    //printf("\nunknow %d -> ", datatype);

    // set datatype value in hex format
    v_type.SetString(buff, strlen(buff), document.GetAllocator());

    dataSection->get_Data(&ibuf);

    Microsoft::WRL::ComPtr<Windows::Storage::Streams::IBufferByteAccess> pBufferByteAccess;
    ibuf.As(&pBufferByteAccess);

    BYTE* pdatabuf = nullptr;
    pBufferByteAccess->Buffer(&pdatabuf);

    memset(buff, 0, sizeof(buff));

    UINT32 length = 0;

    ibuf->get_Length(&length);

    //printf("length -> %d", length);

    for (UINT32 i = 0; i < length; ++i)
    {
        sprintf_s(buff + (char)3 * i, sizeof(buff) - 3 * i, "%02X ", *(pdatabuf + i));
    }

    //printf(" bytes -> %s", buff);

    // set raw value
    v_raw.SetString(buff, strlen(buff) - 1, document.GetAllocator());

    // add values in the object
    v_object.AddMember("type", v_type, document.GetAllocator());
    v_object.AddMember("raw", v_raw, document.GetAllocator());
    unknowns.PushBack(v_object, document.GetAllocator());
}

void dataTypeParse(
    BYTE& id, 
    ComPtr<Advertisement::IBluetoothLEAdvertisement> bleAdvert,
    ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> adData, 
    rj::Document& document,
    rj::Value& unkows)
{
    switch(id){
    case 0x09:
        localNameParse(bleAdvert, document);
        break;
    case 0x03:
        servicesuidParse(bleAdvert, document);
        break;
    default:
        unknowdataParse(adData, document, unkows);
    }
}

bool Parser::Parse(
    Advertisement::IBluetoothLEAdvertisementReceivedEventArgs* args, 
    char* jsonBuffer, 
    UINT32 jsonBufferSize)
{
    // This Document will store all the ADV
    rj::Document document;
    document.SetObject();

    /*
        Values that an ADV must have
    */
    addressParse(args, document);

    advtypeParse(args, document);

    rawsignalParse(args, document);

    timestampParse(args, document);
    
    /*
        Data Section
    */
    // "args" parameter is a interface over the ADV, need the ADV to get the data section
    ComPtr<Advertisement::IBluetoothLEAdvertisement> bleAdvert;
    args->get_Advertisement(&bleAdvert);

    ComPtr<ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection*>> vecData;
    bleAdvert->get_DataSections(&vecData);

    unsigned int countDataSections = 0;
    vecData->get_Size(&countDataSections);

    // unkown data types arrays
    rj::Value v_unknows(rj::Type::kArrayType);

    for (unsigned int i = 0; i < countDataSections; ++i)
    {
        ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> significantPart;
        vecData->GetAt(i, &significantPart);

        ComPtr<ABI::Windows::Storage::Streams::IBuffer> ibuf;
        
        BYTE adType = 0;
        significantPart->get_DataType(&adType);

        dataTypeParse(adType, bleAdvert, significantPart, document, v_unknows);
    }

    // add unknowns array to the document
    document.AddMember("unknowns", v_unknows, document.GetAllocator());

    rj::StringBuffer buffer;
    rj::Writer<rj::StringBuffer> writer(buffer);

    document.Accept(writer);

    const char* stringBufferPtr = buffer.GetString();
    size_t bufferLen = buffer.GetLength();
    memcpy_s(jsonBuffer, jsonBufferSize, stringBufferPtr, bufferLen+1);

    printf("%s\n", jsonBuffer);

    return true;
}

