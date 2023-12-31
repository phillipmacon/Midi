// Copyright (c) Microsoft Corporation. All rights reserved.
#pragma once

#define AUDIO_DEVICE_ENUMERATOR L"MMDEVAPI"
#define MIDI_DEVICE_ENUMERATOR L"MIDISRV"

// ----------------------------------------------------------------------
//
//  MIDIPORT
//
//      Describes a MIDI port SWD that is created or being created
//
// ----------------------------------------------------------------------
typedef enum _SWDEVICESTATE
{
    NotCreated = 0, // SwDeviceCreate not yet called
    CreatePending,  // SwDeviceCreate called successfully, but creation callback not yet invoked
    Created,        // SwDeviceCreate creation callback has been invoked and device interface has been created
    Failed
} SWDEVICESTATE;

struct GUIDCompare
{
    bool operator()(const GUID& Guid1, const GUID& Guid2) const
    {
        return ::memcmp(&Guid1, &Guid2, sizeof(GUID)) < 0;
    }
};

using unique_hswdevice = wil::unique_any<HSWDEVICE, decltype(&::SwDeviceClose), ::SwDeviceClose>;
using unique_swd_string = wil::unique_any<PWSTR, decltype(&::SwMemFree), ::SwMemFree>;

typedef struct _MIDIPORT
{
    MidiFlow MidiFlow;
    GUID InterfaceCategory;
    SWDEVICESTATE SwDeviceState;         // SWD creation state
    unique_hswdevice SwDevice;           // Handle to the SWD created for the MIDI port
    unique_swd_string DeviceInterfaceId; // SWD interface ID for the MIDI port
    std::wstring InstanceId;
} MIDIPORT, *PMIDIPORT;

class CMidiDeviceManager  : 
    public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
        IMidiDeviceManagerInterface>
{
public:

    CMidiDeviceManager() {}
    ~CMidiDeviceManager() {}
    STDMETHOD(Initialize)(_In_ std::shared_ptr<CMidiPerformanceManager>& performanceManager);
    STDMETHOD(ActivateEndpoint)(
        _In_ PCWSTR,
        _In_ BOOL,
        _In_ MidiFlow,
        _In_ ULONG,
        _In_ ULONG,
        _In_ PVOID,
        _In_ PVOID,
        _In_ PVOID
    );
    STDMETHOD(DeactivateEndpoint)(_In_ PCWSTR);
    STDMETHOD(RemoveEndpoint)(_In_ PCWSTR);
    STDMETHOD(Cleanup)();

private:

    HRESULT ActivateEndpointInternal(
        _In_ PCWSTR,
        _In_ PCWSTR,
        _In_ BOOL,
        _In_ MidiFlow,
        _In_ ULONG,
        _In_ ULONG,
        _In_ DEVPROPERTY*,
        _In_ DEVPROPERTY*,
        _In_ SW_DEVICE_CREATE_INFO*,
        _In_ std::wstring *
    );

    std::shared_ptr<CMidiPerformanceManager> m_PerformanceManager;
    std::map<GUID, wil::com_ptr_nothrow<IMidiEndpointManager>, GUIDCompare> m_MidiEndpointManagers;
 
    wil::critical_section m_MidiPortsLock;
    std::vector<std::unique_ptr<MIDIPORT>> m_MidiPorts;
};

