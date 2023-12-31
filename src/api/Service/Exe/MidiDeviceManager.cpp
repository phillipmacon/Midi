// Copyright (c) Microsoft Corporation. All rights reserved.
#include "stdafx.h"

_Use_decl_annotations_
HRESULT
CMidiDeviceManager::Initialize(std::shared_ptr<CMidiPerformanceManager>& PerformanceManager)
{
    std::vector<GUID> availableAbstractionLayers;

    // TODO: retrieve a list of available abstraction layers from the registry or some
    // other registration mechanism, like a saved configuration?
    availableAbstractionLayers.push_back(__uuidof(Midi2KSAbstraction));

    availableAbstractionLayers.push_back(__uuidof(Midi2SimpleLoopbackAbstraction));
    availableAbstractionLayers.push_back(__uuidof(Midi2NetworkMidiAbstraction));
    availableAbstractionLayers.push_back(__uuidof(Midi2VirtualMidiAbstraction));

    for (auto const& AbstractionLayer : availableAbstractionLayers)
    {
        wil::com_ptr_nothrow<IMidiAbstraction> midiAbstraction;
        wil::com_ptr_nothrow<IMidiEndpointManager> endpointManager;

        RETURN_IF_FAILED(CoCreateInstance(AbstractionLayer, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&midiAbstraction)));
        RETURN_IF_FAILED(midiAbstraction->Activate(__uuidof(IMidiEndpointManager), (void**)&endpointManager));
        RETURN_IF_FAILED(endpointManager->Initialize((IUnknown*)this));

        m_MidiEndpointManagers.emplace(AbstractionLayer, std::move(endpointManager));
    }

    m_PerformanceManager = PerformanceManager;
    return S_OK;
}

typedef struct _CREATECONTEXT
{
    PMIDIPORT MidiPort;
    wil::unique_event creationCompleted{wil::EventOptions::None};
    DEVPROPERTY* InterfaceDevProperties;
    ULONG IntPropertyCount;
} CREATECONTEXT, * PCREATECONTEXT;

void SwMidiPortCreateCallback(__in HSWDEVICE hSwDevice, __in HRESULT CreationResult, __in_opt PVOID pContext, __in_opt PCWSTR /* pszDeviceInstanceId */)
{
    PCREATECONTEXT creationContext = (PCREATECONTEXT)pContext;

    // interface registration has started, assume
    // failure
    creationContext->MidiPort->SwDeviceState = SWDEVICESTATE::Failed;

    LOG_IF_FAILED(CreationResult);

    if (SUCCEEDED(CreationResult))
    {
        CreationResult = SwDeviceInterfaceRegister(
            hSwDevice,
            &(creationContext->MidiPort->InterfaceCategory),
            nullptr,
            creationContext->IntPropertyCount,
            creationContext->InterfaceDevProperties,
            TRUE,
            wil::out_param(creationContext->MidiPort->DeviceInterfaceId));
        LOG_IF_FAILED(CreationResult);
    }

    if (SUCCEEDED(CreationResult))
    {
        // success, mark the port as created
        creationContext->MidiPort->SwDeviceState = SWDEVICESTATE::Created;
    }

    // success or failure, signal we have completed.
    creationContext->creationCompleted.SetEvent();
}

_Use_decl_annotations_
HRESULT
CMidiDeviceManager::ActivateEndpoint
(
    PCWSTR ParentInstanceId,
    BOOL UMPOnly,
    MidiFlow MidiFlow,
    ULONG IntPropertyCount,
    ULONG DevPropertyCount,
    PVOID InterfaceDevProperties,
    PVOID DeviceDevProperties,
    PVOID CreateInfo
)
{
    auto lock = m_MidiPortsLock.lock();

    const bool alreadyActivated = std::find_if(m_MidiPorts.begin(), m_MidiPorts.end(), [&](const std::unique_ptr<MIDIPORT>& Port)
    {
        // if this instance id already activated, then we cannot activate/create a second time,
        if (((SW_DEVICE_CREATE_INFO*)CreateInfo)->pszInstanceId == Port->InstanceId)
        {
            return true;
        }

        return false;
    }) != m_MidiPorts.end();

    if (alreadyActivated)
    {
        return S_FALSE;
    }
    else
    {
        std::wstring deviceInterfaceId;

        auto cleanupOnFailure = wil::scope_exit([&]()
        {
            // in the event that creation fails, clean up all of the interfaces associated with this InstanceId
            DeactivateEndpoint(((SW_DEVICE_CREATE_INFO*)CreateInfo)->pszInstanceId);
        });

        // Activate the UMP version of this endpoint.
        RETURN_IF_FAILED(ActivateEndpointInternal(ParentInstanceId, nullptr, FALSE, MidiFlow, IntPropertyCount, DevPropertyCount, (DEVPROPERTY*)InterfaceDevProperties, (DEVPROPERTY*)DeviceDevProperties, (SW_DEVICE_CREATE_INFO*)CreateInfo, &deviceInterfaceId));

        if (!UMPOnly)
        {
            // now activate the midi 1 SWD's for this endpoint
            if (MidiFlow == MidiFlowBidirectional)
            {
                // if this is a bidirectional endpoint, it gets an in and an out midi 1 swd's.
                RETURN_IF_FAILED(ActivateEndpointInternal(ParentInstanceId, deviceInterfaceId.c_str(), TRUE, MidiFlowOut, IntPropertyCount, DevPropertyCount, (DEVPROPERTY*)InterfaceDevProperties, (DEVPROPERTY*)DeviceDevProperties, (SW_DEVICE_CREATE_INFO*)CreateInfo, nullptr));
                RETURN_IF_FAILED(ActivateEndpointInternal(ParentInstanceId, deviceInterfaceId.c_str(), TRUE, MidiFlowIn, IntPropertyCount, DevPropertyCount, (DEVPROPERTY*)InterfaceDevProperties, (DEVPROPERTY*)DeviceDevProperties, (SW_DEVICE_CREATE_INFO*)CreateInfo, nullptr));
            }
            else
            {
                RETURN_IF_FAILED(ActivateEndpointInternal(ParentInstanceId, deviceInterfaceId.c_str(), TRUE, MidiFlow, IntPropertyCount, DevPropertyCount, (DEVPROPERTY*)InterfaceDevProperties, (DEVPROPERTY*)DeviceDevProperties, (SW_DEVICE_CREATE_INFO*)CreateInfo, nullptr));
            }
        }

        cleanupOnFailure.release();
    }

    return S_OK;
}

_Use_decl_annotations_
HRESULT
CMidiDeviceManager::ActivateEndpointInternal
(
    PCWSTR ParentInstanceId,
    PCWSTR AssociatedInstanceId,
    BOOL MidiOne,
    MidiFlow MidiFlow,
    ULONG IntPropertyCount,
    ULONG DevPropertyCount,
    DEVPROPERTY *InterfaceDevProperties,
    DEVPROPERTY *DeviceDevProperties,
    SW_DEVICE_CREATE_INFO *CreateInfo,
    std::wstring *DeviceInterfaceId
)
{
    std::unique_ptr<MIDIPORT> midiPort = std::make_unique<MIDIPORT>();
    CREATECONTEXT creationContext;

    RETURN_IF_NULL_ALLOC(midiPort);

    // copy the incoming array into a vector so that we can add additional items.
    std::vector<DEVPROPERTY> interfaceProperties (InterfaceDevProperties, InterfaceDevProperties + IntPropertyCount);

    if (AssociatedInstanceId)
    {
        // add the items to the vector
        interfaceProperties.push_back({ {PKEY_MIDI_AssociatedUMP, DEVPROP_STORE_SYSTEM, nullptr},
                   DEVPROP_TYPE_STRING, static_cast<ULONG>((wcslen(AssociatedInstanceId) + 1) * sizeof(WCHAR)), (PVOID)AssociatedInstanceId });
    }

    // lambdas can only be converted to a function pointer if they
    // don't do capture, so copy everything into the CREATECONTEXT
    // to share with the SwDeviceCreate callback.
    creationContext.MidiPort = midiPort.get();
    creationContext.InterfaceDevProperties = interfaceProperties.data();
    creationContext.IntPropertyCount = (ULONG) interfaceProperties.size();

    midiPort->SwDeviceState = SWDEVICESTATE::CreatePending;

    midiPort->InstanceId = CreateInfo->pszInstanceId;
    midiPort->MidiFlow = MidiFlow;

    const GUID* interfaceCategory;
    std::wstring enumeratorName;

    if (MidiOne)
    {
        enumeratorName = AUDIO_DEVICE_ENUMERATOR;
        if (MidiFlow == MidiFlow::MidiFlowOut)
        {
            interfaceCategory = &DEVINTERFACE_MIDI_OUTPUT;
        }
        else if (MidiFlow == MidiFlow::MidiFlowIn)
        {
            interfaceCategory = &DEVINTERFACE_MIDI_INPUT;
        }
        else
        {
            RETURN_IF_FAILED(E_UNEXPECTED);
        }
    }
    else
    {
        enumeratorName = MIDI_DEVICE_ENUMERATOR;
        if (MidiFlow == MidiFlow::MidiFlowOut)
        {
            interfaceCategory = &DEVINTERFACE_UNIVERSALMIDIPACKET_OUTPUT;
        }
        else if (MidiFlow == MidiFlow::MidiFlowIn)
        {
            interfaceCategory = &DEVINTERFACE_UNIVERSALMIDIPACKET_INPUT;
        }
        else if (MidiFlow == MidiFlow::MidiFlowBidirectional)
        {
            interfaceCategory = &DEVINTERFACE_UNIVERSALMIDIPACKET_BIDI;
        }
        else
        {
            RETURN_IF_FAILED(E_UNEXPECTED);
        }
    }

    midiPort->InterfaceCategory = *interfaceCategory;

    RETURN_IF_FAILED(SwDeviceCreate(
        enumeratorName.c_str(),
        ParentInstanceId,
        CreateInfo,
        DevPropertyCount,
        DeviceDevProperties,
        SwMidiPortCreateCallback,
        &creationContext,
        wil::out_param(midiPort->SwDevice)));

    // wait for creation to complete
    creationContext.creationCompleted.wait();

    // confirm we were able to register the interface
    RETURN_HR_IF(E_FAIL, midiPort->SwDeviceState != SWDEVICESTATE::Created);

    if (DeviceInterfaceId)
    {
        *DeviceInterfaceId = midiPort->DeviceInterfaceId.get();
    }

    // success, transfer the midiPort to the list
    m_MidiPorts.push_back(std::move(midiPort));

    return S_OK;
}

_Use_decl_annotations_
HRESULT
CMidiDeviceManager::DeactivateEndpoint
(
        PCWSTR InstanceId
)
{
    // there may be more than one SWD associated with this instance id, as we reuse
    // the instance id for the legacy SWD, it just has a different activator and InterfaceClass.
    do
    {
        // locate the MIDIPORT that identifies the swd
        auto item = std::find_if(m_MidiPorts.begin(), m_MidiPorts.end(), [&](const std::unique_ptr<MIDIPORT>& Port)
        {
            // if this instance id already activated, then we cannot activate/create a second time,
            if (InstanceId == Port->InstanceId)
            {
                return true;
            }

            return false;
        });

        // if the item was found
        if (item == m_MidiPorts.end())
        {
            break;
        }
        else
        {
            // Erasing this item from the list will free the unique_ptr and also trigger a SwDeviceClose on the item->SwDevice,
            // which will deactivate the device, done.
            m_MidiPorts.erase(item);
        }
    } while (TRUE);

    return S_OK;
}

_Use_decl_annotations_
HRESULT
CMidiDeviceManager::RemoveEndpoint
(
    PCWSTR InstanceId 
)
{
    // first deactivate, to ensure it's removed from the tracking list
    LOG_IF_FAILED(DeactivateEndpoint(InstanceId));

    // TODO: Locate the device with this instance id using windows.devices.enumeration,
    // and delete it.

    return S_OK;
}

_Use_decl_annotations_
HRESULT
CMidiDeviceManager::Cleanup()
{
    m_MidiEndpointManagers.clear();
    m_MidiPorts.clear();

    return S_OK;
}

