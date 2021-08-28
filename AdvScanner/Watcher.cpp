
#include <iostream>
#include <functional>
#include <windows.h>
#include <stdio.h>
#include <thread>

// required Windows Runtime
#include <Windows.Foundation.h>
#include <wrl/event.h>
using namespace Microsoft::WRL;

// Sin los .. usa cppwinrt
#include <../winrt/windows.devices.bluetooth.h>
using namespace ABI::Windows::Devices::Bluetooth::Advertisement;

#include "Logger.hpp"
#include "Parser.h"
#include "Watcher.h"


#define error(message)  logError("WATCHER", message)
#define info(message)   logInfo("WATCHER", message)
#define MAX_JSON_BUFFER_SIZE    512

ComPtr<IBluetoothLEAdvertisementWatcher> _WATCHER;
FILE* _FILEPTR;


HRESULT Watcher::CallBackObject::AdvertisementRecived(
    IBluetoothLEAdvertisementWatcher* watcher, 
    IBluetoothLEAdvertisementReceivedEventArgs* args)
{
    char buffer[MAX_JSON_BUFFER_SIZE];
    memset(buffer, 0, MAX_JSON_BUFFER_SIZE);
    if (!Parser::Parse(args, buffer, MAX_JSON_BUFFER_SIZE))
    {
        error("EventArgs no se ha podido parsear");
    }
    else {
        fwrite(buffer, sizeof(char), strlen(buffer), _FILEPTR);
        fprintf(_FILEPTR, "\n");
    }
    return S_OK;
 }


void Watcher::Run(unsigned int miliseconds, std::string outputFile)
{
    info("Run()");

    fopen_s(&_FILEPTR, outputFile.c_str(), "w");

    if (_FILEPTR == NULL)
    {
        error("Cloud not open file '" + outputFile + "'");
    }
    else 
    {
        Watcher::WatchADV(miliseconds);
        fclose(_FILEPTR);
    }
}

int Watcher::WatchADV(unsigned int timeToWatch)
{
    EventRegistrationToken* watcherToken = new EventRegistrationToken();
    HRESULT hr;

    Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(
        RO_INIT_MULTITHREADED
    );

    if (FAILED(initialize)) 
    {
        error("RoInitializeWrapper failed");
        return -1;
    }


    ComPtr<IBluetoothLEAdvertisementWatcherFactory> bleAdvWatcherFactory;
    hr = ABI::Windows::Foundation::GetActivationFactory(
        Microsoft::WRL::Wrappers::HStringReference(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementWatcher).Get(),
        &bleAdvWatcherFactory
    );

    if (FAILED(hr)) 
    {
        error("GetActivationFactory failed");
        return -1;
    }


    ComPtr<IBluetoothLEAdvertisementFilter> bleFilter;
    Wrappers::HStringReference class_id_filter(RuntimeClass_Windows_Devices_Bluetooth_Advertisement_BluetoothLEAdvertisementFilter);    
    hr = RoActivateInstance(class_id_filter.Get(), reinterpret_cast<IInspectable**>(bleFilter.GetAddressOf()));

    if (FAILED(hr)) 
    {
        error("RoActivateInstance error");
        return -1;
    }
    

    hr = bleAdvWatcherFactory->Create(bleFilter.Get(), _WATCHER.GetAddressOf());

    if (_WATCHER == NULL || (FAILED(hr))) 
    {
        error("Create Watcher error");
        return -1;
    }
   

    struct CallBackObject object;
    ComPtr< ABI::Windows::Foundation::ITypedEventHandler<BluetoothLEAdvertisementWatcher*, BluetoothLEAdvertisementReceivedEventArgs*> > handler;
    handler = Callback< ABI::Windows::Foundation::ITypedEventHandler<BluetoothLEAdvertisementWatcher*, BluetoothLEAdvertisementReceivedEventArgs*> > 
        (
            std::bind(
                &CallBackObject::AdvertisementRecived,
                &object,
                std::placeholders::_1,
                std::placeholders::_2
            )
        );
        

    hr = _WATCHER->add_Received(handler.Get(), watcherToken);

    if (FAILED(hr)) 
    {
        error("Adding Received callback");
        return -1;
    }

    _WATCHER->put_ScanningMode(ABI::Windows::Devices::Bluetooth::Advertisement::BluetoothLEScanningMode_Active);

    _WATCHER->Start();
    info("watcher started");
    
    // sleep main thread and let the callback running
    Sleep(timeToWatch);

    _WATCHER->Stop();
    info("watcher stopped");

    return true;
}