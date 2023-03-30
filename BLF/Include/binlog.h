/*----------------------------------------------------------------------------
|
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
| Copyright (c) Vector Informatik GmbH. All rights reserved.
 ----------------------------------------------------------------------------*/

#if !defined ( BINARY_LOGGING_H )
       #define BINARY_LOGGING_H

#if defined ( _MSC_VER )
  #pragma warning( disable: 4103)
  #pragma pack( push, 8)
#elif defined (__linux__)
  #pragma pack( push, 8)
#endif

#if defined (_WIN32)
    #include <wtypes.h>
#endif // _WIN32

#include <stdint.h>
#include <stddef.h>

/*----------------------------------------------------------------------------
|
| BL API
|
-----------------------------------------------------------------------------*/

#define BL_MAJOR_NUMBER 5
#define BL_MINOR_NUMBER 6
#define BL_BUILD_NUMBER 5
#define BL_PATCH_NUMBER 0

#if defined (__linux__)
  #define BLAPI(ret) __attribute__((visibility("default"))) ret
#else
  #if defined ( BINLOG_EXPORTS )
    #define BLAPI( ret) ret __stdcall
  #else
    #define BLAPI(ret) __declspec(dllimport) ret __stdcall
  #endif
#endif


#if defined (__linux__)
  // from win32-API
  #if !defined (GENERIC_READ)
    #define GENERIC_READ                     (0x80000000L)
  #endif

  #define GENERIC_WRITE                    (0x40000000L)
  #define GENERIC_EXECUTE                  (0x20000000L)
  #define GENERIC_ALL                      (0x10000000L)

  #if !defined(SYSTEMTIME_DEFINED_FOR_LINUX)
    #define SYSTEMTIME_DEFINED_FOR_LINUX 1

    typedef struct _SYSTEMTIME
    {
      uint16_t wYear;
      uint16_t wMonth;
      uint16_t wDayOfWeek;
      uint16_t wDay;
      uint16_t wHour;
      uint16_t wMinute;
      uint16_t wSecond;
      uint16_t wMilliseconds;
    } SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
  #endif

#endif

#define BLHANDLE void *
#define BLINVALID_HANDLE_VALUE ((BLHANDLE)(uintptr_t)-1)

/*----------------------------------------------------------------------------
|
| BL OBJECTS
|
-----------------------------------------------------------------------------*/

#include "binlog_objects.h"

/*----------------------------------------------------------------------------
|
| API
|
-----------------------------------------------------------------------------*/

#define BL_FLUSH_STREAM                         0x00000001
#define BL_FLUSH_FILE                           0x00000002

#define BL_WRITE_OPTION_IMMEDIATE_STREAM_COMMIT 0x00000001

#if defined ( __cplusplus )
extern "C" {
#endif


#if defined ( __cplusplus )
struct IBLCallback
{
    enum BLMessageLevel
    {
        eInfo    = 0, // Error
        eWarning = 1, // Warning
        eError   = 2  // Information only
    };

    enum BLMessageType
    {
        eDefault        = 0, // typically in write window
        eSimple         = 1, // typically in message box
        eAskStop        = 2, // typically in message box with question to stop
        eAskStopFurther = 3  // typically in message box with questions to stop or show further errors
    };

    virtual void OutputMessage(BLMessageType messageType, BLMessageLevel messageLevel, const char * lpMessage) = 0;
};

struct IBLCallback2 : IBLCallback
{
    enum BLEvent
    {
        eUnknownEvent    = 0, // unknown event
        eStreamConnected = 1, // stream is connected
        eStreamBroken    = 2, // stream is broken
        eStreamInit      = 3  // stream is initialized
    };
    virtual void SignalEvent(BLHANDLE hFile, BLEvent event) = 0;
};
#endif

/**
* Use this function to open a BL file with the desired access.
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFile(const char * lpFileName, uint32_t dwDesiredAccess);
/**
* Use this function to open a BL file with the desired access.
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFileW(const wchar_t * lpFileName, uint32_t dwDesiredAccess);
/**
* Use this function to open a BL file with the desired access.
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \param[in] lpServer optional external logging provider, syntax <GUID>|<dll name>, pass null-pointer to ignore
* \param[in] lpHost logging host, currently unused
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFileEx(const char * lpFileName, uint32_t dwDesiredAccess, const char * lpServer, const char * lpHost);
/**
* Use this function to open a BL file with the desired access.
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \param[in] lpServer optional external logging provider, syntax <GUID>|<dll name>, pass null-pointer to ignore
* \param[in] lpHost logging host, currently unused
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFileExW(const wchar_t * lpFileName, uint32_t dwDesiredAccess, const wchar_t * lpServer, const wchar_t * lpHost);
#if defined ( __cplusplus )
/**
* Use this function to open a BL file with the desired access.
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \param[in] lpServer optional external logging provider, syntax <GUID>|<dll name>, pass null-pointer to ignore
* \param[in] lpHost logging host, currently unused
* \param[in] pCallback pointer to callback function where binlog can write status and error messages, pass null-pointer to ignore
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFileEx2(const char * lpFileName, uint32_t dwDesiredAccess, const char * lpServer, const char * lpHost, IBLCallback* pCallback);
/**
* Use this function to open a BL file with the desired access.
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \param[in] lpServer optional external logging provider, syntax <GUID>|<dll name>, pass null-pointer to ignore
* \param[in] lpHost logging host, currently unused
* \param[in] pCallback pointer to callback function where binlog can write status and error messages, pass null-pointer to ignore
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFileEx2W(const wchar_t * lpFileName, uint32_t dwDesiredAccess, const wchar_t * lpServer, const wchar_t * lpHost, IBLCallback* pCallback);
/**
* Use this function to open a BL file with the desired access, callback function, and additional info for an external provider
* \param[in] lpFileName file name to create/open
* \param[in] dwDesiredAccess access type, GENERIC_READ or GENERIC_WRITE
* \param[in] lpServer optional external logging provider, syntax <GUID>|<dll name>, pass null-pointer to ignore
* \param[in] pProviderInfo  additional info for additional provider, pass nullptr to ignore
* \param[in] lpHost logging host, currently unused
* \param[in] pCallback pointer to callback function where binlog can write status and error messages, pass null-pointer to ignore
* \return handle on specified file on success, otherwise BLINVALID_HANDLE_VALUE
*/
BLAPI(BLHANDLE) BLCreateFileEx3W(const wchar_t * lpFileName, uint32_t dwDesiredAccess, const wchar_t * lpServer, void* pProviderInfo, const wchar_t * lpHost, IBLCallback* pCallback);
#endif
/**
* Use this function to close a BL file opened with BLCreateFile.
* \param[in] hFile file handle returned by BLCreateFile
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLCloseHandle(BLHANDLE hFile);

/**
* Use this function to write a BL object to the file.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_WRITE access
* \param[in] pBase pointer to BL object containing the data to be written
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLWriteObject(BLHANDLE hFile, VBLObjectHeaderBase* pBase);
/**
* Use this function to read the base header of a BL object.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pBase pointer to BL object that receives the header description
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLPeekObject (BLHANDLE hFile, VBLObjectHeaderBase* pBase);
/**
* Use this function to skip a BL object.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pBase pointer to BL object describing the object to be skipped
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLSkipObject (BLHANDLE hFile, VBLObjectHeaderBase* pBase);
/**
* Obsolete: this function is replaced by BLReadObjectSecure.
* Use this function to read a BL object.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pBase pointer to BL object describing the object to be read
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLReadObject (BLHANDLE hFile, VBLObjectHeaderBase* pBase); /* obsolete - replaced by BLReadObjectSecure */
/**
* Use this function to read a BL object.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pBase pointer to BL object describing the object to be read
* \param[in] expectedSize size of BL object which is provided by pBase
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLReadObjectSecure(BLHANDLE hFile, VBLObjectHeaderBase* pBase, size_t expectedSize);
/**
* Use this function to free the memory which was allocated to read a BL object.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pBase pointer to BL object describing the object to be freed
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLFreeObject (BLHANDLE hFile, VBLObjectHeaderBase* pBase);
/**
* Use this function to seek forward in a BLF file to the first object with a certain time stamp.
* Not supported under Linux.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] timestamp the time stamp to search for
* \param[in] arg Argument passed to pProgressCallback
* \param[in] pProgressCallback callback function passing the arg pointer and the progress value from 0 to 1.0
* \param[in] callbackRate rate how often the callback is processed (in ms)
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLSeekTime(BLHANDLE hFile, uint64_t timeStamp, void* arg, int32_t (*pProgressCallback)(void*, float), uint16_t callbackRate);

/**
* Use this function to specify the application which writes the file.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_WRITE access
* \param[in] appID application identifier
* \param[in] appMajor application major version number
* \param[in] appMinor application minor version number
* \param[in] appBuild application build version number
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLSetApplication(BLHANDLE hFile, uint8_t appID, uint8_t appMajor, uint8_t appMinor, uint8_t appBuild);
/**
* Use this function to set the compression.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_WRITE access
* \param[in] dwCompression compression level for writing. value range is [0,10] with 10 as maximum compression
* \param[in] dwReserved reserved, must be zero.
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLSetWriteOptions(BLHANDLE hFile, uint32_t dwCompression, uint32_t dwFlags);
/**
* Use this function to set the measurement start time.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_WRITE access
* \param[in] lpStartTime pointer to the system time structure
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLSetMeasurementStartTime(BLHANDLE hFile, const LPSYSTEMTIME lpStartTime);
/**
* Use this function to retrieve the file statistics.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pStatistics pointer to the file statistics structure
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLGetFileStatistics(BLHANDLE hFile, VBLFileStatistics* pStatistics);
/**
* Use this function to retrieve the extended file statistics.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pStatistics pointer to the extended file statistics structure
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLGetFileStatisticsEx(BLHANDLE hFile, VBLFileStatisticsEx* pStatistics);
/**
* Use this function to flush the file buffers.
* \param[in] hFile file handle returned by BLCreateFile with GENERIC_WRITE access
* \param[in] dwFlags valid values are: BL_FLUSH_STREAM and BL_FLUSH_FILE
* \return TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLFlushFileBuffers(BLHANDLE hFile, uint32_t dwFlags);
/**
* Use this function to set a comment attribute.
* \param[in] hFile  file handle returned by BLCreateFile with GENERIC_WRITE access
* \param[in] lpName  name of the attribute
* \param[in] lpValue  attribute value
* \return  TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLSetCommentAttributeString(BLHANDLE hFile, const wchar_t* lpName, const wchar_t* lpValue);
/**
* Use this function to read the number of comment attributes in a file.
* \param[in] hFile  file handle returned by BLCreateFile with GENERIC_READ access
* \param[out] the number of comment attributes in the file
* \return  TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLGetNumCommentAttributes(BLHANDLE hFile, uint32_t* lpNumAttributes);
/**
* Use this function to read the name of a comment attribute.
* The attribute name will be written to lpName. On success, TRUE will be returned, lpName contains the attribute
* value, and lpNameSize contains the size in bytes of the name (including terminating 0 character). If the buffer
* size is too small, lpNameSize will contain the required size of the buffer in bytes and FALSE will be returned.
* In case of any other failure, FALSE will be returned and lpValue will be set to 0.
* \param[in] hFile  file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] dwIndex  index of the attribute
* \param[out] lpName  buffer where the result is written to
* \param[inout] lpNameSize  initial size of the buffer; after the call, size of the written attribute or (if initially too small) required buffer size
* \return  TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLGetCommentAttributeName(BLHANDLE hFile, uint32_t dwIndex, wchar_t* lpName, uint32_t* lpNameSize);
/**
* Use this function to read a comment attribute.
* The attribute value will be written to lpValue. On success, TRUE will be returned, lpValue contains the attribute
* value, and lpValueSize contains the size in bytes of the value (including terminating 0 character). If the buffer
* size is too small, lpValueSize will contain the required size of the buffer in bytes and FALSE will be returned.
* If the logfile does not contain an attribute with the given name, FALSE will be returned and lpValue will be set to 0.
* \param[in] hFile  file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] lpName  name of the attribute
* \param[out] lpValue  buffer where the result is written to
* \param[inout] lpValueSize  initial size of the buffer; after the call, size of the written attribute or (if initially too small) required buffer size
* \return  TRUE on success, otherwise FALSE
*/
BLAPI(int32_t)  BLGetCommentAttributeString(BLHANDLE hFile, const wchar_t* lpName, wchar_t* lpValue, uint32_t* lpValueSize);
/**
* Use this function to read the timestamp in ns of the next object.
* The attribute value will be written to lpTimestamp.
* \param[in] hFile  file handle returned by BLCreateFile with GENERIC_READ access
* \param[in] pBase  base header obtained by a preceding call to BLPeekObject
* \param[out] lpTimestamp  pointer to where the result should be written to
* \return  TRUE on success, otherwise FALSE
*/
BLAPI(int32_t) BLPeekTimestamp(BLHANDLE hFile, const VBLObjectHeaderBase* pBase, uint64_t* lpTimestamp);

#if defined ( __cplusplus )
}
#endif

/*----------------------------------------------------------------------------
|
| 
|
-----------------------------------------------------------------------------*/

#pragma pack( pop)

#endif // BINARY_LOGGING_H
