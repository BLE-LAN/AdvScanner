

#include <iostream>
#include <stdio.h>
#include <functional>
// required Windows Runtime
#include <Windows.Foundation.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <wrl/event.h> // requerido por Callback()
// Sin los .. usa cppwinrt
#include <../winrt/windows.devices.bluetooth.h>

#include "Logger.hpp"
#include "GetBeacons.h"

using namespace std;
// required Windows Runtime
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;
// 
using namespace ABI::Windows::Devices::Bluetooth::Advertisement;


// https://stackoverflow.com/questions/39895191/getting-ble-beacons-in-c-windows-10-desktop-application
 
Logger logger = Logger();

 HRESULT CallBackObject::AdvertisementRecived(IBluetoothLEAdvertisementWatcher* watcher, IBluetoothLEAdvertisementReceivedEventArgs* args)
 {
     /*
        address : identificar el dispositivo
        timestamp : tiempo desde la última vez que se actualiza si poosición
        Strength y transmitpowerlevel: para calcular la distancia 
        
     */
     ComPtr<IBluetoothLEAdvertisement> bleAdvert;
     args->get_Advertisement(&bleAdvert);

     


     ComPtr <ABI::Windows::Foundation::Collections::IVector<ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection*>> vecData;
     HRESULT hr = bleAdvert->get_DataSections(&vecData);

     if (FAILED(hr))
     {
         logger.Log(__LINE__, "get_DataSections failed");
     }
     else
     {
         char buff[256] = { 0 };
         UINT count = 0;
         hr = vecData->get_Size(&count);

         logger.Log(__LINE__, "Datasections Count " + std::to_string(count));

         if (SUCCEEDED(hr))
         {
             for (UINT i = 0; i < count; ++i)
             {
                 ComPtr<ABI::Windows::Devices::Bluetooth::Advertisement::IBluetoothLEAdvertisementDataSection> ds;

                 hr = vecData->GetAt(i, &ds);
                 if (SUCCEEDED(hr))
                 {
                     ComPtr<ABI::Windows::Storage::Streams::IBuffer> ibuf;
                     BYTE datatype = 0;

                     hr = ds->get_DataType(&datatype);
                     memset(buff, 0, sizeof(buff));
                     sprintf_s(buff, sizeof(buff), "%d", datatype);
                     printf("Data Type: %s ", buff);

                     hr = ds->get_Data(&ibuf);
                 }
             }

         }
     }


         UINT64 address;
         args->get_BluetoothAddress(&address);

         INT16 dbm;
         args->get_RawSignalStrengthInDBm(&dbm);

         printf("addr = > %llX, DBm => %d\n", address, dbm);

         return S_OK;
 }

int main()
{
    EventRegistrationToken* watcherToken = new EventRegistrationToken();
    HRESULT hr;

    RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize)) {
        logger.Log(__LINE__, std::string("RoInitializeWrapper failed"));
        return -1;
    }

    ComPtr<IBluetoothLEAdvertisementWatcherFactory> bleAdvWatcherFactory;
    hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcher).Get(), &bleAdvWatcherFactory);
    if (FAILED(hr)) {
        logger.Log(__LINE__, std::string("GetActivationFactory error"));
        return -1;
    }

    ComPtr<IBluetoothLEAdvertisementWatcher> bleWatcher;
    ComPtr<IBluetoothLEAdvertisementFilter> bleFilter;

    Wrappers::HStringReference class_id_filter(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementFilter);
    
    hr = RoActivateInstance(class_id_filter.Get(), reinterpret_cast<IInspectable**>(bleFilter.GetAddressOf()));
    if (FAILED(hr)) {
        logger.Log(__LINE__, std::string("RoActivateInstance error"));
        return -1;
    }
    
    hr = bleAdvWatcherFactory->Create(bleFilter.Get(), bleWatcher.GetAddressOf());
    if (bleWatcher == NULL || (FAILED(hr))) {
        logger.Log(__LINE__, std::string("Create Watcher error"));
        return -1;
    }
   
    struct CallBackObject object;
    ComPtr<ITypedEventHandler<BluetoothLEAdvertisementWatcher*, BluetoothLEAdvertisementReceivedEventArgs*>> handler;

    handler = Callback<ITypedEventHandler<BluetoothLEAdvertisementWatcher*, BluetoothLEAdvertisementReceivedEventArgs*> >
        (std::bind(
            &CallBackObject::AdvertisementRecived,
            &object,
            placeholders::_1,
            placeholders::_2
        ));
        
    hr = bleWatcher->add_Received(handler.Get(), watcherToken);
    if (FAILED(hr)) {
        logger.Log(__LINE__, std::string("Add received callback error"));
        return -1;
    }

    bleWatcher->Start();

    while (1) {
        Sleep(1000);
    }
}