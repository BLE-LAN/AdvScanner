

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
#include "Parser.h"
#include "Watcher.h"

using namespace std;
// required Windows Runtime
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

// https://stackoverflow.com/questions/39895191/getting-ble-beacons-in-c-windows-10-desktop-application
 
Logger logger = Logger();

 HRESULT Watcher::CallBackObject::AdvertisementRecived(IBluetoothLEAdvertisementWatcher* watcher, IBluetoothLEAdvertisementReceivedEventArgs* args)
 {
     if (!Parser::Parse(args)) 
     {
         logger.Log(__LINE__, std::string("AdvertisementReceivedEventArgs parse error"));
     }
     return S_OK;
 }

int Watcher::StartWatcher()
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

    while (true) {
        Sleep(1000);
    }
}