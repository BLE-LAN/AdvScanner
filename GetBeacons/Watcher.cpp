

#include <iostream>
#include <functional>
// required Windows Runtime
#include <Windows.Foundation.h>
#include <wrl/wrappers/corewrappers.h>
#include <wrl/client.h>
#include <wrl/event.h>
// Sin los .. usa cppwinrt
#include <../winrt/windows.devices.bluetooth.h>

#include "Logger.hpp"
#include "Parser.h"
#include "Watcher.h"

using namespace std;
using namespace ABI::Windows::Foundation;
using namespace Microsoft::WRL;
using namespace Microsoft::WRL::Wrappers;

#define error(message) Logger::Log("WATCHER", 1, message)

 HRESULT Watcher::CallBackObject::AdvertisementRecived(IBluetoothLEAdvertisementWatcher* watcher, IBluetoothLEAdvertisementReceivedEventArgs* args)
 {
     if (!Parser::Parser(args))
     {
         error("EventArgs no se ha podido parsear");
     }
     return S_OK;
 }

int Watcher::StartWatcher()
{
    EventRegistrationToken* watcherToken = new EventRegistrationToken();
    HRESULT hr;

    RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
    if (FAILED(initialize)) {
        error("RoInitializeWrapper failed");
        return -1;
    }

    ComPtr<IBluetoothLEAdvertisementWatcherFactory> bleAdvWatcherFactory;
    hr = GetActivationFactory(HStringReference(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcher).Get(), &bleAdvWatcherFactory);
    if (FAILED(hr)) {
        error("GetActivationFactory failed");
        return -1;
    }

    ComPtr<IBluetoothLEAdvertisementWatcher> bleWatcher;
    ComPtr<IBluetoothLEAdvertisementFilter> bleFilter;

    Wrappers::HStringReference class_id_filter(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementFilter);
    
    hr = RoActivateInstance(class_id_filter.Get(), reinterpret_cast<IInspectable**>(bleFilter.GetAddressOf()));
    if (FAILED(hr)) {
        error("RoActivateInstance error");
        return -1;
    }
    
    hr = bleAdvWatcherFactory->Create(bleFilter.Get(), bleWatcher.GetAddressOf());
    if (bleWatcher == NULL || (FAILED(hr))) {
        error("Create Watcher error");
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
        error("Add received callback error");
        return -1;
    }

    bleWatcher->Start();

    while (true) {
        Sleep(1000);
    }
}