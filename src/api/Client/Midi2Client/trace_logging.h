// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

// tracelogging information:
// https://learn.microsoft.com/en-us/windows/win32/api/traceloggingprovider/nf-traceloggingprovider-tracelogging_define_provider

#pragma once


//#include "WinEventLogLevels.h"
#include <TraceLoggingProvider.h>

// qualified with API so as not to intefere with service or SDK tracing
#define TRACELOGGING_PROVIDER_NAME "Windows.Devices.Midi2.Api"


#define TRACE_KEYWORD_API         0x0000000000000001

namespace Windows::Devices::Midi2::Internal
{
    TRACELOGGING_DECLARE_PROVIDER(g_hLoggingProvider);

    extern GUID g_LoggingProviderActivityId;

    void RegisterTraceLogging();
    void UnregisterTraceLogging();


    void LogHresultError(
        _In_ const char* location,
        _In_ const wchar_t* message,
        _In_ winrt::hresult_error const& ex) noexcept;

    void LogGeneralError(
        _In_ const char* location,
        _In_ const wchar_t* message) noexcept;

    void LogUmpDataValidationError(
        _In_ const char* location,
        _In_ const wchar_t* message,
        _In_ const uint32_t firstWord,
        _In_ const uint64_t timestamp) noexcept;

    void LogUmpSizeValidationError(
        _In_ const char* location,
        _In_ const wchar_t* message,
        _In_ const uint32_t providedSizeInWords,
        _In_ const uint64_t timestamp) noexcept;

}