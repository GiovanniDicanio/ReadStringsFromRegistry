////////////////////////////////////////////////////////////////////////////////
//
// FILE: Test.cpp
//
// DESC: Test code for the RegistryGetString() function.
//
// Copyright (C) by Giovanni Dicanio. All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////


#include "RegistryReadString.h"
#include <stdio.h>


int main()
{
    static const int kExitOk = 0;
    static const int kExitError = 1;

    try
    {
        // Read some test string from the registry
        CString str = win32::RegistryGetString(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion",
            L"CommonFilesDir");

        // Print the read string
        wprintf(L"String read from registry:\n\n");
        wprintf(L"  \"%s\"\n", str.GetString());

        // Make sure that the string length is correct
        const CString underline(L'*', str.GetLength());
        wprintf(L"   %s\n\n", underline.GetString());
        wprintf(L"(Length = %d)\n\n", str.GetLength());

        return kExitOk;
    }
    catch (const CAtlException& e)
    {
        wprintf(L"\n*** ERROR: HResult=0x%08X\n", static_cast<HRESULT>(e));
        return kExitError;
    }
}

