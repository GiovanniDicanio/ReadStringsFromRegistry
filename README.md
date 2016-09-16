# Reading String Values From the Windows Registry #

By Giovanni Dicanio

**NOTE:**  For more recent code, see the _new_ [**WinReg** repository](https://github.com/GiovanniDicanio/WinReg).

To retrieve a string value from the Windows registry, the Win32 [`RegGetValue()`](https://msdn.microsoft.com/en-us/library/windows/desktop/ms724868%28v=vs.85%29.aspx) API can be used.

The prototype of this function is showed here:

```cpp
LONG WINAPI RegGetValue(
  _In_         HKEY hkey,
  _In_opt_     LPCTSTR lpSubKey,
  _In_opt_     LPCTSTR lpValue,
  _In_opt_     DWORD dwFlags,
  _Out_opt_    LPDWORD pdwType,
  _Out_opt_    PVOID pvData,
  _Inout_opt_  LPDWORD pcbData
);
```

Since in general the size of the string to be retrieved is not known *a priori*, we could call the `RegGetValue()` function twice.

The purpose of the **first call** is to get a size value to allocate a buffer large enough to store the string. To get this buffer size, the `RegGetValue()` function is called passing `nullptr` as argument for the `pvData` parameter. The `pcbData` parameter points to a `DWORD` that will store the size of the data (i.e. the size for the string buffer), in bytes.

```cpp
    // First call: get a large enough size to allocate destination string buffer
    DWORD keyType = 0;
    DWORD dataSize = 0;
    const DWORD flags = RRF_RT_REG_SZ; // Only read strings (REG_SZ)
    LONG result = ::RegGetValue(
            key, 
            subKey,
            valueName, 
            flags, 
            &keyType, 
            nullptr, // pvData == nullptr ? Request buffer size
            &dataSize);
```

Once we have this size value, a `CString` instance can be allocated with a proper buffer size. For that purpose, the `CString::GetBuffer()` method can be used:

```cpp
    // Create a CString with large enough internal buffer
    CString text;
    const DWORD bufferLength = dataSize / sizeof(WCHAR); // length in WCHAR's
    WCHAR* const textBuffer = text.GetBuffer(bufferLength);
```

Then, the `RegGetValue()` function is called again, this time passing a valid buffer pointer for the `pvData` parameter, and a valid maximum buffer size. Upon successful completion of this second call, the `RegGetValue()` function will store the actual string value into the provided buffer:

```cpp
    // Second call: read string value from the Registry into local string buffer
    result = ::RegGetValue(
            key, 
            subKey,
            valueName, 
            flags, 
            nullptr, 
            textBuffer, // Write string in this destination buffer
            &dataSize);
```

Note that the `RegGetValue()` function may overestimate the size of the string buffer in the first call, to make extra room for a terminating NUL character (in case that would be absent in the string value stored in the registry). Instead, the size value returned by the second `RegGetValue()` call reflects the actual size of the string written to the destination buffer (including the terminating NUL). 
Since the returned string is NUL-terminated, a simple call to `CString::ReleaseBuffer()` would be fine, since this method scans the string buffer, and stops when it finds the NUL-terminator. However, we can slightly optimize our code here (and avoid a “premature pessimization”), calling `CString::ReleaseBufferSetLength()`, since we already know the actual string length (in fact, this piece of information was passed by the second call of `RegGetValue()`):

```cpp
    const DWORD actualStringLength = dataSize / sizeof(WCHAR);
 
    // -1 to exclude the terminating NUL
    text.ReleaseBufferSetLength(actualStringLength - 1);
```

Now the `CString` `text` instance contains the string value read from the Windows registry, ready for further processing.

Note that the aforementioned calls to the `RegGetValue()` function may fail. To handle those failures, C++ exceptions can be thrown. There are several options to do that in code. For example, it’s possible to derive some C++ exception class from `std::exception` (the STL base class for exceptions); or, in a more ATL-like style, it’s possible to throw instances of `CAtlException` with an embedded `HRESULT`, carrying information about the particular error occurred, e.g.:

```cpp
    if (result != ERROR_SUCCESS)
    {
        // Error
        AtlThrow(HRESULT_FROM_WIN32(result));
    }
```


**P.S.**

While the STL’s `std::wstring` class could be also used to store string values read from the Windows registry, in general the `CString` class is better suited for interacting with the Win32 API layer of C++ applications. For example: `CString` is used at MFC class methods interfaces, as well as in ATL and WTL interfaces.

