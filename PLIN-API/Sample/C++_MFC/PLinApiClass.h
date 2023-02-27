// PLinApiClass.h
//
// ~~~~~~~~~~~~
//
// Class for dynamic load the PLinApi library 
//
// ~~~~~~~~~~~~
//
// ------------------------------------------------------------------
// Author : Fabrice Vergnaud
//	Last change: 30.06.2013 Wagner
//
// Language: C++
// ------------------------------------------------------------------
//
// Copyright (C) 1999-2010 PEAK-System Technik GmbH, Darmstadt
// more Info at http://www.peak-system.com 
//
#ifndef __PLINAPICLASSH_
#define __PLINAPICLASSH_

// Inclusion of the PLinApi.h header file
//
#ifndef __LINAPIH__
#include "PLinApi.h"
#endif

// Function pointers
//
typedef TLINError (__stdcall * fpOneParamCli) (HLINCLIENT);
typedef TLINError (__stdcall * fpTwoParam) (HLINCLIENT, HLINHW);
typedef TLINError (__stdcall * fpDelStartSchedule) (HLINCLIENT, HLINHW, int);
typedef TLINError (__stdcall * fpRegisterClient) (LPSTR, DWORD, HLINCLIENT *);
typedef TLINError (__stdcall * fpIdentifyHardware) (HLINHW);
typedef TLINError (__stdcall * fpSetClientParam) (HLINCLIENT, TLINClientParam, DWORD);
typedef TLINError (__stdcall * fpGetClientParam) (HLINCLIENT, TLINClientParam, void *, WORD);
typedef TLINError (__stdcall * fpSetClientFilter) (HLINCLIENT, HLINHW, unsigned __int64);
typedef TLINError (__stdcall * fpGetClientFilter) (HLINCLIENT, HLINHW, unsigned __int64 *);
typedef TLINError (__stdcall * fpRead) (HLINCLIENT, TLINRcvMsg *);
typedef TLINError (__stdcall * fpReadMulti) (HLINCLIENT, TLINRcvMsg *, int, int *);
typedef TLINError (__stdcall * fpWrite) (HLINCLIENT, HLINHW, TLINMsg *);
typedef TLINError (__stdcall * fpInitializeHardware) (HLINCLIENT, HLINHW, TLINHardwareMode, WORD);
typedef TLINError (__stdcall * fpGetAvailableHardware) (HLINHW *, WORD, int *);
typedef TLINError (__stdcall * fpSetHardwareParam) (HLINCLIENT, HLINHW, TLINHardwareParam, void *, WORD);
typedef TLINError (__stdcall * fpGetHardwareParam) (HLINHW, TLINHardwareParam, void *, WORD);
typedef TLINError (__stdcall * fpRegisterFrameId) (HLINCLIENT, HLINHW, BYTE, BYTE);
typedef TLINError (__stdcall * fpSetFrameEntry) (HLINCLIENT, HLINHW, TLINFrameEntry *);
typedef TLINError (__stdcall * fpGetFrameEntry) (HLINHW, TLINFrameEntry *);
typedef TLINError (__stdcall * fpUpdateByteArray) (HLINCLIENT, HLINHW, BYTE, BYTE, BYTE, BYTE *);
typedef TLINError (__stdcall * fpStartKeepAlive) (HLINCLIENT, HLINHW, BYTE, WORD);
typedef TLINError (__stdcall * fpSetSchedule) (HLINCLIENT, HLINHW, int, TLINScheduleSlot *, int);
typedef TLINError (__stdcall * fpGetSchedule) (HLINHW, int, TLINScheduleSlot *, int, int *);
typedef TLINError (__stdcall * fpSetScheduleBreakPoint) (HLINCLIENT, HLINHW, int, DWORD);
typedef TLINError (__stdcall * fpStartAutoBaud) (HLINCLIENT, HLINHW, WORD);
typedef TLINError (__stdcall * fpGetStatus) (HLINHW, TLINHardwareStatus *);
typedef TLINError (__stdcall * fpCalculateChecksum) (TLINMsg *);
typedef TLINError (__stdcall * fpGetVersion) (TLINVersion *);
typedef TLINError (__stdcall * fpGetVersionInfo) (LPSTR, WORD);
typedef TLINError (__stdcall * fpGetErrorText) (TLINError, BYTE, LPSTR, WORD);
typedef TLINError (__stdcall * fpGetPID) (BYTE *);
typedef TLINError (__stdcall * fpGetTargetTime) (HLINHW, unsigned __int64 *);
typedef TLINError (__stdcall * fpSetResponseRemap) (HLINCLIENT, HLINHW, BYTE *);
typedef TLINError (__stdcall * fpGetResponseRemap) (HLINHW, BYTE *);
typedef TLINError (__stdcall * fpGetSystemTime) (unsigned __int64 *);

// Re-define of name for better code-read
//
#define fpRemoveClient		fpOneParamCli
#define fpResetClient		fpOneParamCli
#define fpConnectClient			fpTwoParam
#define fpDisconnectClient		fpTwoParam
#define fpResetHardware			fpTwoParam
#define fpSuspendKeepAlive		fpTwoParam
#define fpResumeKeepAlive		fpTwoParam
#define fpResetHardwareConfig	fpTwoParam
#define fpSuspendSchedule		fpTwoParam
#define fpResumeSchedule		fpTwoParam
#define fpXmtWakeUp				fpTwoParam
#define fpDeleteSchedule	fpDelStartSchedule
#define fpStartSchedule		fpDelStartSchedule

// PLinApiClass dynamic-load class
//
class PLinApiClass 
{
	private:
		// DLL PLinApi
		HINSTANCE m_hDll;

		// Function pointers
		fpRemoveClient			m_pRemoveClient;
		fpResetClient			m_pResetClient;	
		fpConnectClient			m_pConnectClient;	
		fpDisconnectClient		m_pDisconnectClient;	
		fpResetHardware			m_pResetHardware;	
		fpSuspendKeepAlive		m_pSuspendKeepAlive;	
		fpResumeKeepAlive		m_pResumeKeepAlive;	
		fpResetHardwareConfig	m_pResetHardwareConfig;	
		fpSuspendSchedule		m_pSuspendSchedule;	
		fpResumeSchedule		m_pResumeSchedule;	
		fpXmtWakeUp				m_pXmtWakeUp;	
		fpDeleteSchedule		m_pDeleteSchedule;	
		fpStartSchedule			m_pStartSchedule;	
		fpRegisterClient		m_pRegisterClient;
		fpIdentifyHardware		m_pIdentifyHardware;
		fpSetClientParam		m_pSetClientParam;
		fpGetClientParam		m_pGetClientParam;
		fpSetClientFilter		m_pSetClientFilter;
		fpGetClientFilter		m_pGetClientFilter;
		fpRead					m_pRead;
		fpReadMulti				m_pReadMulti;
		fpWrite					m_pWrite;
		fpInitializeHardware	m_pInitializeHardware;
		fpGetAvailableHardware	m_pGetAvailableHardware;
		fpSetHardwareParam		m_pSetHardwareParam;
		fpGetHardwareParam		m_pGetHardwareParam;
		fpRegisterFrameId		m_pRegisterFrameId;
		fpSetFrameEntry			m_pSetFrameEntry;
		fpGetFrameEntry			m_pGetFrameEntry;
		fpUpdateByteArray		m_pUpdateByteArray;
		fpStartKeepAlive		m_pStartKeepAlive;
		fpSetSchedule			m_pSetSchedule;
		fpGetSchedule			m_pGetSchedule;
		fpSetScheduleBreakPoint	m_pSetScheduleBreakPoint;
		fpStartAutoBaud			m_pStartAutoBaud;
		fpGetStatus				m_pGetStatus;
		fpCalculateChecksum		m_pCalculateChecksum;
		fpGetVersion			m_pGetVersion;
		fpGetVersionInfo		m_pGetVersionInfo;
		fpGetErrorText			m_pGetErrorText;
		fpGetPID				m_pGetPID;
		fpGetTargetTime			m_pGetTargetTime;
		fpSetResponseRemap		m_pSetResponseRemap;
		fpGetResponseRemap		m_pGetResponseRemap;
		fpGetSystemTime			m_pGetSystemTime;

		// Load flag
		bool m_bWasLoaded;

		// Load the PCANBasic API
		void LoadAPI();

		// Releases the loaded API
		void UnloadAPI();	

		// Initializes the pointers for the PCANBasic functions
		void InitializePointers();

		// Loads the DLL
        bool LoadDllHandle();

		// Gets the address of a given function name in a loaded DLL
		FARPROC GetFunction(LPSTR szName);	

    public:       
		// PLinApiClass constructor
		PLinApiClass();
		// PLinApiClass destructor
		~PLinApiClass();

		/// <summary>
        /// Registers a Client at the LIN Manager. Creates a Client handle and 
        /// allocates the Receive Queue (only one per Client). The hWnd parameter 
        /// can be zero for DOS Box Clients. The Client does not receive any 
        /// messages until LIN_RegisterFrameId() or LIN_SetClientFilter() is called.
        ///
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue 
        /// </summary>
        /// <param name="strName">Name of the Client</param>
        /// <param name="hWnd">Window handle of the Client (only for information purposes)</param>
        /// <param name="hClient">Pointer to the Client handle buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError RegisterClient (
			LPSTR strName,
			DWORD hWnd,
			HLINCLIENT *hClient);

        /// <summary>
        /// Removes a Client from the Client list of the LIN Manager. Frees all 
        /// resources (receive queues, message counters, etc.). If the Client was 
        /// a Boss-Client for one or more Hardware, the Boss-Client property for 
        /// those Hardware will be set to INVALID_LIN_HANDLE.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient 
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <returns>A LIN Error Code</returns>
		TLINError RemoveClient (
			HLINCLIENT hClient);       

        /// <summary>
        /// Connects a Client to a Hardware.
        /// The Hardware is assigned by its Handle.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware to be connected</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ConnectClient (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Disconnects a Client from a Hardware. This means: no more messages 
        /// will be received by this Client from this Hardware.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware to be disconnected</param>
        /// <returns>A LIN Error Code</returns>
		TLINError DisconnectClient (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Flushes the Receive Queue of the Client and resets its counters.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ResetClient (
			HLINCLIENT hClient);

        /// <summary>
        /// Sets a Client parameter to a given value.
        /// 
        ///   Allowed TLINClientParam    Parameter
        ///   values in wParam:          type:       Description:
        ///   -------------------------  ----------  ------------------------------
        ///   clpReceiveStatusFrames     int         0 = Status Frames deactivated,
        ///                                          otherwise active
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterType, errWrongParameterValue, errIllegalClient
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="wParam">Parameter to set</param>
        /// <param name="dwValue">parameter value</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetClientParam (
			HLINCLIENT hClient,
			TLINClientParam wParam,
			DWORD dwValue);

        /// <summary>
        /// Gets a Client parameter.
        /// 
        ///   Allowed TLINClientParam    Parameter
        ///   values in wParam:          type:       Description:
        ///   -------------------------  ----------  ------------------------------
        ///   clpName                    string      Name of the Client
        ///   clpMessagesOnQueue         int         Unread messages in the Receive Queue
        ///   clpWindowHandle            int         Window handle of the Client application 
        ///                                          (can be zero for DOS Box Clients)
        ///   clpConnectedHardware       HLINHW[]    Array of Hardware Handles connected by a Client 
        ///                                          The first item in the array refers to the 
        ///                                          amount of handles. So [*] = Total handles + 1
        ///   clpTransmittedMessages     int         Number of transmitted messages
        ///   clpReceivedMessages        int         Number of received messages
        ///   clpReceiveStatusFrames     int         0 = Status Frames deactivated, otherwise active
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterType, errWrongParameterValue, errIllegalClient, 
        ///   errBufferInsufficient
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="wParam">Parameter to get</param>
        /// <param name="pBuff">Buffer for the parameter value</param>
        /// <param name="wBuffSize">Buffer size in bytes</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetClientParam (
			HLINCLIENT hClient,
			TLINClientParam wParam,
			void *pBuff,
			WORD wBuffSize);

        /// <summary>
        /// Sets the filter of a Client and modifies the filter of 
        /// the connected Hardware.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="iRcvMask">Filter. Each bit corresponds to a Frame ID (0..63)</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetClientFilter (
			HLINCLIENT hClient,
			HLINHW hHw,
			unsigned __int64 iRcvMask);

        /// <summary>
        /// Gets the filter corresponding to a given Client-Hardware pair.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="pRcvMask">Filter buffer. Each bit corresponds to a Frame ID (0..63)</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetClientFilter (
			HLINCLIENT hClient,
			HLINHW hHw,
			unsigned __int64 *pRcvMask);

        /// <summary>
        /// Reads the next message/status information from a Client's Receive 
        /// Queue. The message will be written to 'pMsg'.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errRcvQueueEmpty
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="pMsg">Message read buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError Read (
			HLINCLIENT hClient,
			TLINRcvMsg *pMsg);

        /// <summary>
        /// Reads several received messages.
        /// pMsgBuff must be an array of 'iMaxCount' entries (must have at least 
        /// a size of iMaxCount * sizeof(TLINRcvMsg) bytes).
        /// The size 'iMaxCount' of the array = max. messages that can be received.
        /// The real number of read messages will be returned in 'pCount'.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errRcvQueueEmpty
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="pMsgBuff">Messages buffer</param>
        /// <param name="iMaxCount"> Maximum number of messages to read (pMsgBuff length)</param>
        /// <param name="pCount">Buffer for the real number of messages read</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ReadMulti ( 
			HLINCLIENT hClient,
			TLINRcvMsg *pMsgBuff,
			int iMaxCount,
			int *pCount);

        /// <summary>
        /// The Client 'hClient' transmits a message 'pMsg' to the Hardware 'hHw'.
        /// The message is written into the Transmit Queue of the Hardware.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalDirection, errIllegalLength
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="pMsg">Message write buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError Write (
			HLINCLIENT hClient,
			HLINHW hHw,
			TLINMsg *pMsg);

        /// <summary>
        /// Initializes a Hardware with a given Mode and Baudrate.
        /// 
        /// <remarks>
        /// If the Hardware was initialized by another Client, the function 
        /// will re-initialize the Hardware. All connected clients will be affected.
        /// It is the job of the user to manage the setting and/or configuration of 
        /// Hardware, e.g. by using the Boss-Client parameter of the Hardware.
        /// </remarks>
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalBaudrate
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="byMode">Hardware mode (see LIN Hardware Operation Modes)</param>
        /// <param name="wBaudrate">Baudrate (see LIN_MIN_BAUDRATE and LIN_MAX_BAUDRATE)</param>
        /// <returns>A LIN Error Code</returns>
		TLINError InitializeHardware (
			HLINCLIENT hClient,
			HLINHW hHw,
			TLINHardwareMode bMode,
			WORD wBaudrate);

        /// <summary>
        /// Gets an array containing the handles of the current Hardware 
        /// available in the system.
		/// The count of Hardware handles returned in the array is written in 
		/// 'pCount'.  
		///
        /// <remarks>
        /// To ONLY get the count of available Hardware, call this 
        /// function using 'pBuff' = NULL and wBuffSize = 0.
        /// </remarks>
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errBufferInsufficient
        /// </summary>
        /// <param name="pBuff">Buffer for the handles</param>
		/// <param name="wBuffSize">Size of the buffer in bytes </param>
        /// <param name="pCount">Number of Hardware available</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetAvailableHardware(
			HLINHW *pBuff,
			WORD wBuffSize,
			int *pCount);

        /// <summary>
        /// Sets a Hardware parameter to a given value.
        /// 
        ///   Allowed TLINHardwareParam  Parameter
        ///   values in wParam:          type:       Description:
        ///   -------------------------  ----------  ------------------------------
        ///   hwpMessageFilter           UInt64      Hardware message filter. Each bit
        ///                                          corresponds to a Frame ID (0..63)
        ///   hwpBossClient              HLINCLIENT  Handle of the new Boss-Client
        ///   hwpIdNumber                int         Identification number for a hardware
        ///   hwpUserData                byte[]      User data to write on a hardware. See LIN_MAX_USER_DATA
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterType, errWrongParameterValue, errIllegalClient, 
        ///   errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="wParam">Parameter to set</param>
        /// <param name="pBuff">Parameter value buffer</param>
        /// <param name="wBuffSize">Value buffer size</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetHardwareParam (
			HLINCLIENT hClient,
			HLINHW hHw,
			TLINHardwareParam wParam,
			void *pBuff,
			WORD wBuffSize);

        /// <summary>
        /// Gets a Hardware parameter.
        /// 
        ///   Allowed TLINHardwareParam  Parameter
        ///   values in wParam:          type:       Description:
        ///   -------------------------  ----------  ------------------------------
        ///   hwpName                    string      Name of the Hardware. See LIN_MAX_NAME_LENGTH
        ///   hwpDeviceNumber            int         Index of the Device owner of the Hardware
        ///   hwpChannelNumber           int         Channel Index of the Hardware on the owner device
        ///   hwpConnectedClients        HLINCLIENT[]Array of Client Handles conencted to a Hardware 
        ///                                          The first item in the array refers to the 
        ///                                          amount of handles. So [*] = Total handles + 1
        ///   hwpMessageFilter           UInt64      Configured message filter. Each bit corresponds 
        ///                                          to a Frame ID (0..63)
        ///   hwpBaudrate                int         Configured baudrate
        ///   hwpMode                    int         0 = Slave, otehrwise Master
        ///   hwpFirmwareVersion         TLINVersion A TLINVersion structure containing the Firmware Version
        ///   hwpBufferOverrunCount      int         Receive Buffer Overrun Counter
        ///   hwpBossClient              HLINCLIENT  Handle of the current Boss-Client
        ///   hwpSerialNumber            int         Serial number of the Hardware
        ///   hwpVersion                 int         Version of the Hardware
        ///   hwpType                    int         Type of the Hardware
        ///   hwpQueueOverrunCount       int         Receive Queue Buffer Overrun Counter
        ///   hwpIdNumber                int         Identification number for a hardware
        ///   hwpUserData                byte[]      User data saved on the hardware. See LIN_MAX_USER_DATA
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterType, errWrongParameterValue, errIllegalHardware, 
        ///   errBufferInsufficient
        /// </summary>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="wParam">Parameter to get</param>
        /// <param name="pBuff">Parameter buffer</param>
        /// <param name="wBuffSize">Buffer size</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetHardwareParam (
			HLINHW hHw,
			TLINHardwareParam wParam,
			void *pBuff,
			WORD wBuffSize);

        /// <summary>
        /// Flushes the queues of the Hardware and resets its counters.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ResetHardware (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Deletes the current configuration of the Hardware and sets its defaults.
        /// The Client 'hClient' must be registered and connected to the Hardware to 
        /// be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ResetHardwareConfig (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Physically identifies a LIN Hardware (a channel on a LIN Device) by 
        /// blinking its associated LED.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalHardware
        /// </summary>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError IdentifyHardware (
			HLINHW hHw);

        /// <summary>
        /// Modifies the filter of a Client and, eventually, the filter of the 
        /// connected Hardware. The messages with FrameID 'bFromFrameId' to 
        /// 'bToFrameId' will be received.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalFrameID
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="bFromFrameId">First ID of the frame range</param>
        /// <param name="bToFrameId">Last ID of the frame range</param>
        /// <returns>A LIN Error Code</returns>
		TLINError RegisterFrameId (
			HLINCLIENT hClient,
			HLINHW hHw,
			BYTE bFromFrameId,
			BYTE bToFrameId);

        /// <summary>
        /// Configures a LIN Frame in a given Hardware. The Client 'hClient' must 
        /// be registered and connected to the Hardware to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalFrameID, errIllegalLength
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="pFrameEntry">Frame entry information buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetFrameEntry (
			HLINCLIENT hClient,
			HLINHW hHw,
			TLINFrameEntry *pFrameEntry);

        /// <summary>
        /// Gets the configuration of a LIN Frame from a given Hardware.
        /// 
        /// <remarks>
        /// The 'pFrameEntry.FrameId' must be set to the ID of the frame, whose 
        /// configuration should be returned.
        /// </remarks>
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalHardware, errIllegalFrameID
        /// </summary>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="pFrameEntry">Frame entry buffer. The member FrameId 
        /// must be set to the ID of the frame, whose configuration should 
        /// be returned </param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetFrameEntry (
			HLINHW hHw,
			TLINFrameEntry *pFrameEntry);

        /// <summary>
        /// Updates the data of a LIN Frame for a given Hardware. The Client 
        /// 'hClient' must be registered and connected to the Hardware to be 
        /// accessed. 'pData' must have at least a size of 'bLen'.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalFrameID, errIllegalLength, errIllegalIndex, 
        ///   errIllegalRange
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="bFrameId">Frame ID</param>
        /// <param name="bIndex">Index where the update data Starts (0..7)</param>
        /// <param name="bLen">Count of Data bytes to be updated.</param>
        /// <param name="pData">Data buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError UpdateByteArray (
			HLINCLIENT hClient,
			HLINHW hHw,
			BYTE bFrameId,
			BYTE bIndex,
			BYTE bLen,
			BYTE *pData);

        /// <summary>
        /// Sets the Frame 'bFrameId' as Keep-Alive frame for the given Hardware and 
        /// starts to send it every 'wPeriod' Milliseconds. The Client 'hClient' must 
        /// be registered and connected to the Hardware to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalFrameID
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="bFrameId">ID of the Keep-Alive Frame</param>
        /// <param name="wPeriod">Keep-Alive Interval in Milliseconds</param>
        /// <returns>A LIN Error Code</returns>
		TLINError StartKeepAlive ( 
			HLINCLIENT hClient,
			HLINHW hHw,
			BYTE bFrameId,
			WORD wPeriod);

        /// <summary>
        /// Suspends the sending of a Keep-Alive frame in the given Hardware.
        /// The Client 'hClient' must be registered and connected to the Hardware 
        /// to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SuspendKeepAlive (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Resumes the sending of a Keep-Alive frame in the given Hardware. 
        /// The Client 'hClient' must be registered and connected to the Hardware 
        /// to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ResumeKeepAlive (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Configures the slots of a Schedule in a given Hardware. The Client 
        /// 'hClient' must be registered and connected to the Hardware to be 
        /// accessed. The Slot handles will be returned in the parameter 
        /// "pSchedule" (Slots buffer), when this function successfully completes. 
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalScheduleNo, errIllegalSlotCount
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="iScheduleNumber">Schedule number (see 
        /// LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        /// <param name="pSchedule">Slots buffer</param>
        /// <param name="iSlotCount">Count of Slots in the slots buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetSchedule (
			HLINCLIENT hClient,
			HLINHW hHw,
			int iScheduleNumber,
			TLINScheduleSlot *pSchedule,
			int iSlotCount);

        /// <summary>
        /// Gets the slots of a Schedule from a given Hardware. The count of slots 
        /// returned in the array is written in 'pSlotCount'.
        /// 
        /// <remarks>
        /// To ONLY get the count of slots contained in the given Schedule, 
        /// call this function using 'pScheduleBuff' = NULL and iMaxSlotCount = 0.
        /// </remarks>
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalHardware, errIllegalScheduleNo, 
        ///   errIllegalSlotCount
        /// </summary>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="iScheduleNumber">Schedule Number (see 
        /// LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        /// <param name="pScheduleBuff">Slots Buffer.</param>
        /// <param name="iMaxSlotCount">Maximum number of slots to read.</param>
        /// <param name="pSlotCount">Real number of slots read.</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetSchedule (
			HLINHW  hHw,
			int iScheduleNumber,
			TLINScheduleSlot *pScheduleBuff,
			int iMaxSlotCount,
			int *pSlotCount);

        /// <summary>
        /// Removes all slots contained by a Schedule of a given Hardware. The 
        /// Client 'hClient' must be registered and connected to the Hardware to 
        /// be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalScheduleNo
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="iScheduleNumber">Schedule Number (see 
        /// LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        /// <returns>A LIN Error Code</returns>
		TLINError DeleteSchedule (
			HLINCLIENT hClient,
			HLINHW hHw,
			int iScheduleNumber);

        /// <summary>
        /// Sets a 'breakpoint' on a slot from a Schedule in a given Hardware. The 
        /// Client 'hClient' must be registered and connected to the Hardware to 
        /// be accessed.
        /// 
        /// <remarks>
        /// Giving 'dwHandle' a value of 0 ('null'), causes the deletion of 
        //  the breakpoint.
        /// </remarks>
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="iBreakPointNumber">Breakpoint Number (0 or 1)</param>
        /// <param name="dwHandle">Slot Handle</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetScheduleBreakPoint (
			HLINCLIENT hClient,
			HLINHW hHw,
			int iBreakPointNumber,
			DWORD dwHandle);

        /// <summary>
        /// Activates a Schedule in a given Hardware. The Client 'hClient' must 
        /// be registered and connected to the Hardware to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware, 
        ///   errIllegalScheduleNo
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="iScheduleNumber">Schedule Number (see 
        /// LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        /// <returns>A LIN Error Code</returns>
		TLINError StartSchedule (
			HLINCLIENT hClient,
			HLINHW hHw,
			int iScheduleNumber);

        /// <summary>
        /// Suspends an active Schedule in a given Hardware. The Client 'hClient' 
        /// must be registered and connected to the Hardware to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SuspendSchedule (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Restarts a configured Schedule in a given Hardware. The Client 'hClient' 
        /// must be registered and connected to the Hardware to be accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError ResumeSchedule (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Sends a wake-up message impulse (single data byte 0xF0). The Client 
        /// 'hClient' must be registered and connected to the Hardware to be 
        /// accessed.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <returns>A LIN Error Code</returns>
		TLINError XmtWakeUp (
			HLINCLIENT hClient,
			HLINHW hHw);

        /// <summary>
        /// Starts a process to detect the Baud rate of the LIN bus that is 
        /// connected to the indicated Hardware.
        /// The Client 'hClient' must be registered and connected to the Hardware 
        /// to be accessed. The Hardware must be not initialized in order 
        /// to do an Auto-baudrate procedure.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalClient, errIllegalHardware
        /// </summary>
        /// <param name="hClient">Handle of the Client</param>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="wTimeOut">Auto-baudrate Timeout in Milliseconds</param>
        /// <returns>A LIN Error Code</returns>
		TLINError StartAutoBaud (
			HLINCLIENT hClient,
			HLINHW hHw,
			WORD wTimeOut);

        /// <summary>
        /// Retrieves current status information from the given Hardware.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalHardware
        /// </summary>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="pStatusBuff">Status data buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetStatus (
			HLINHW hHw,
			TLINHardwareStatus *pStatusBuff);

        /// <summary>
        /// Calculates the checksum of a LIN Message and writes it into the 
        /// 'Checksum' field of 'pMsg'.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalLength
        /// </summary>
        /// <param name="pMsg">Message buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError CalculateChecksum (
			TLINMsg *pMsg);

        /// <summary>
        ///  Returns a TLINVersion structure containing the PLIN-API DLL version.
        ///  
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        /// 
        /// Possible API errors:
        ///   errWrongParameterValue
        /// </summary>
        /// <param name="pVerBuffer">Version buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetVersion ( 
			TLINVersion *pVerBuff
			);

        /// <summary>
        /// Returns a string containing Copyright information.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue
        /// </summary>
        /// <param name="pTextBuff">String buffer</param>
        /// <param name="wBuffSize">Size in bytes of the buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetVersionInfo (
			LPSTR  strTextBuff,
			WORD wBuffSize);

        /// <summary>
        /// Converts the error code 'dwError' to a text containing an error 
        /// description in the language given as parameter (when available).
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errBufferInsufficient
        /// </summary>
        /// <param name="dwError">A TLINError Code</param>
        /// <param name="bLanguage">Indicates a "Primary language ID"</param>
        /// <param name="strTextBuff">Error string buffer</param>
        /// <param name="wBuffSize">Buffer size in bytes</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetErrorText (
			TLINError dwError,
			BYTE bLanguage,
			LPSTR strTextBuff,
			WORD wBuffSize);

        /// <summary>
        /// Gets the 'FrameId with Parity' corresponding to the given 
        /// 'pFrameId' and writes the result on it.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalFrameID
        /// </summary>
        /// <param name="pframeid">Frame ID (0..LIN_MAX_FRAME_ID)</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetPID(
			BYTE  *pFrameId);

        /// <summary>
        /// Gets the system time used by the LIN-USB adapter.
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalHardware
        /// </summary>
        /// <param name="hHw">Handle of the Hardware</param>
        /// <param name="pTargetTime">Target Time buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetTargetTime (
			HLINHW hHw,
			unsigned __int64  *pTargetTime);

        /// <summary>
        /// Sets the Response Remap of a LIN Slave
        /// 
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        ///   
        /// Possible API errors:
        ///   errWrongParameterValue, errIllegalFrameID, errIllegalClient, errIllegalHardware,
        ///   errMemoryAccess
        /// </summary>
        /// <param name="hClient">Client Handle</param>
        /// <param name="hHw">Hardware Handle</param>
        /// <param name="pRemapTab">Remap Response buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError SetResponseRemap (
			HLINCLIENT  hClient,
			HLINHW hHw,
			BYTE *pRemapTab);

        /// <summary>
        /// Gets the Response Remap of a LIN Slave
        /// </summary>
        /// <param name="hHw">Hardware Handle</param>
        /// <param name="pRemapTab">Remap Response buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetResponseRemap (
			HLINHW hHw,
			BYTE *pRemapTab);

        /// <summary>
        /// Gets the current system time. The system time is returned by 
        /// Windows as the elapsed number of microseconds since system start.
        /// 
        /// Possible DLL interaction errors: 
        ///   errMemoryAccess
        ///   
        /// Possible API errors:
        ///   errWrongParameterValue
        /// </summary>
        /// <param name="pTargetTime">System Time buffer</param>
        /// <returns>A LIN Error Code</returns>
		TLINError GetSystemTime(
			unsigned __int64  *pSystemTime);
};

#endif