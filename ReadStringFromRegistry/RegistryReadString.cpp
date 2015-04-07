////////////////////////////////////////////////////////////////////////////////
//
// FILE: RegistryReadString.cpp
//
// DESC: Implementation of the RegistryGetString() function
//
// Copyright (C) by Giovanni Dicanio. All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////


#include "RegistryReadString.h"     // Module header
#include <Windows.h>                // Win32 SDK
#include <atldef.h>                 // Some ATL stuff


namespace win32 
{


CString RegistryGetString(
        _In_        HKEY    key,
        _In_opt_    PCWSTR  subKey,
        _In_opt_    PCWSTR  valueName
    )
{
    //
    // Try getting the size of the string value to read,
    // to properly allocate a buffer for the string
    //
    DWORD keyType = 0;
    DWORD dataSize = 0;
    const DWORD flags = RRF_RT_REG_SZ; // Only read strings (REG_SZ)
    LONG result = ::RegGetValue(
            key, 
            subKey,
            valueName, 
            flags, 
            &keyType, 
            nullptr,    // pvData == nullptr --> Request buffer size for string
            &dataSize);
    if (result != ERROR_SUCCESS)
    {
        // Error: throw an exception to signal that
        AtlThrow(HRESULT_FROM_WIN32(result));
    }

    // Make sure we are reading a REG_SZ
    ATLASSERT(keyType == REG_SZ);


    //
    // Allocate enough memory to store the text
    //
    CString text;
    const DWORD bufferLength = dataSize / sizeof(WCHAR); // length in WCHAR's
    WCHAR* const textBuffer = text.GetBuffer(bufferLength);
    ATLASSERT(textBuffer != nullptr);
    

    //
    // Read that string value text from the Windows registry
    //
    result = ::RegGetValue(
            key, 
            subKey,
            valueName, 
            flags, 
            nullptr, 
            textBuffer, // Write string in this destination buffer
            &dataSize);
    if (result != ERROR_SUCCESS)
    {
        // Error
        AtlThrow(HRESULT_FROM_WIN32(result));
    }

    //
    // Recalculate string buffer length, since it seems in the second call
    // to RegGetValue() (when a valid buffer pointer is passed to 
    // that function), the returned dataSize is *different* (smaller) 
    // than in the previous call.
    //
    // See this question on Stack Overflow:
    //  http://stackoverflow.com/q/29223180/1629821
    //
    const DWORD actualStringLength = dataSize / sizeof(WCHAR);

    // -1 to exclude the terminating NUL
    text.ReleaseBufferSetLength(actualStringLength - 1);

    // All right, return text read from the registry
    return text;
}



} // namespace win32

