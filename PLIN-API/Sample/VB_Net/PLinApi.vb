'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
'  PLinApi.vb
'  Version 2.2.28.71
'
'  Definition of the PLIN-API.
'
' 
'    Principle:
'    ~~~~~~~~~~
'    The driver supports multiple clients (= Windows or DOS programs
'    that communicate with LIN-busses), and multiple LIN-Devices.
'    A LIN-Device represent two LIN-Hardware (each channel is a Hardware)
'    Multiple 'Clients' can be connected to one or more LIN-Hardware, which
'    itself have an interface to a physical LIN-channel of a device.
'
'    Features:
'    ~~~~~~~~~
'     - 1 Client can be connected to multiple Hardware
'     - 1 Hardware supports multiple clients
'     - When a Client sends a message to a Hardware, the message will not be routed
'       to other clients. The response of the Hardware is routed to the connected
'       clients depending on the registered Hardware message filter
'     - each Client only receives the messages that pass its acceptance filter
'     - each Client has a Receive Queue to buffer received messages
'     - hClient: 'Client handle'. This number is used by the driver to
'                identify and manage a Client
'     - hHw:     'Hardware handle'. This number is used by the driver to
'                identify and manage a Hardware
'     - all handles are 1-based. 0 = illegal handle
'
'     All functions return a value of type TLINError
'
'     Authors: K.Wagner / P.Steil
'
'     -----------------------------------------------------------------------
'     Copyright (C) 2008-2010 by PEAK-System Technik GmbH, Darmstadt/Germany
'     -----------------------------------------------------------------------
'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Imports System
Imports System.Text
Imports System.Runtime.InteropServices

' Client Handle
' 
Imports HLINCLIENT = System.Byte
' Hardware Handle
'
Imports HLINHW = System.UInt16

Namespace Peak.Lin
#Region "Enumerations"
    ' Error flags for LIN Rcv Msgs
    '
    <Flags()> _
    Public Enum TLINMsgErrors As Integer
        ' Error on Synchronization field 
        InconsistentSynch = &H1
        ' Wrong parity Bit 0
        IdParityBit0 = &H2
        ' Wrong parity Bit 1
        IdParityBit1 = &H4
        ' Slave not responding error
        SlaveNotResponding = &H8
        ' A timeout was reached
        Timeout = &H10
        ' Wrong checksum
        Checksum = &H20
        ' Bus shorted to ground
        GroundShort = &H40
        ' Bus shorted to VBat
        VBatShort = &H80
        ' A slot time (delay) was too small
        SlotDelay = &H100
        ' Response was received from other station
        OtherResponse = &H200
    End Enum

    ' Client Parameters (GetClientParam Function)
    '
    Public Enum TLINClientParam As UShort
        ' Client Name
        clpName = 1
        ' Unread messages in the Receive Queue
        clpMessagesOnQueue = 2
        ' Registered windows handle (information purpose)
        clpWindowHandle = 3
        ' Handles of the connected Hardware
        clpConnectedHardware = 4
        ' Number of transmitted messages
        clpTransmittedMessages = 5
        ' Number of received messages
        clpReceivedMessages = 6
        ' Status of the property "Status Frames"
        clpReceiveStatusFrames = 7
        ' Handle of the Receive event
        clpOnReceiveEventHandle = 8
        ' Handle of the Hardware plug-in event
        clpOnPluginEventHandle = 9
    End Enum

    ' Hardware Parameters (GetHardwareParam function)
    '
    Public Enum TLINHardwareParam As UShort
        ' Hardware / Device Name
        hwpName = 1
        ' Index of the owner Device
        hwpDeviceNumber = 2
        ' Channel Index on the owner device (0 or 1)
        hwpChannelNumber = 3
        ' Handles of the connected clients
        hwpConnectedClients = 4
        ' Message filter
        hwpMessageFilter = 5
        ' Baudrate
        hwpBaudrate = 6
        ' Master status
        hwpMode = 7
        ' Lin Firmware Version (Text with the form xx.yy where:
        ' xx = major version. yy = minor version)
        hwpFirmwareVersion = 8
        ' Receive Buffer Overrun Counter
        hwpBufferOverrunCount = 9
        ' Registered master Client
        hwpBossClient = 10
        ' Serial number of a Hardware
        hwpSerialNumber = 11
        ' Version of a Hardware
        hwpVersion = 12
        ' Type of a Hardware
        hwpType = 13
        ' Receive Queue Buffer Overrun Counter
        hwpQueueOverrunCount = 14
        ' Hardware identification number
        hwpIdNumber = 15
        ' User data on a hardware
        hwpUserData = 16
    End Enum

    ' Received Message Types
    '
    Public Enum TLINMsgType As Byte
        ' Standard LIN Message
        mstStandard = 0
        ' Bus Sleep status message
        mstBusSleep = 1
        ' Bus WakeUp status message
        mstBusWakeUp = 2
        ' Auto-baudrate Timeout status message
        mstAutobaudrateTimeOut = 3
        ' Auto-baudrate Reply status message
        mstAutobaudrateReply = 4
        ' Bus Overrun status message
        mstOverrun = 5
        ' Queue Overrun status message
        mstQueueOverrun = 6
    End Enum

    ' Schedule Slot Types
    '
    Public Enum TLINSlotType As Byte
        ' Unconditional frame
        sltUnconditional = 0
        ' Event frame
        sltEvent = 1
        ' Sporadic frame
        sltSporadic = 2
        ' Diagnostic Master Request frame
        sltMasterRequest = 3
        ' Diagnostic Slave Response frame
        sltSlaveResponse = 4

    End Enum

    ' LIN Message Direction Types
    '
    Public Enum TLINDirection As Byte
        ' Direction disabled
        dirDisabled = 0
        ' Direction is Publisher
        dirPublisher = 1
        ' Direction is Subscriber
        dirSubscriber = 2
        ' Direction is Subscriber (detect Length)
        dirSubscriberAutoLength = 3
    End Enum

    ' LIN Message checksum Types
    '
    Public Enum TLINChecksumType As Byte
        ' Custom checksum
        cstCustom = 0
        ' Classic checksum (ver 1.x)
        cstClassic = 1
        ' Enhanced checksum
        cstEnhanced = 2
        ' Detect checksum
        cstAuto
    End Enum

    ' LIN Hardware operation mode
    '
    Public Enum TLINHardwareMode As Byte
        ' Hardware is not initialized
        modNone = 0
        ' Hardware working as Slave
        modSlave = 1
        ' Hardware working as Master
        modMaster = 2
    End Enum

    ' LIN Hardware status
    '
    Public Enum TLINHardwareState As Byte
        ' Hardware is not initialized
        hwsNotInitialized = 0
        ' Hardware is detecting the baudrate
        hwsAutobaudrate = 1
        ' Hardware (bus) is active
        hwsActive = 2
        ' Hardware (bus) is in sleep mode
        hwsSleep = 3
        ' Hardware (bus-line) shorted to ground
        ShortGround = 6
    End Enum

    ' LIN Error Codes
    '
    Public Enum TLINError As Integer
        ' No error. Success.
        errOK = 0
        ' Transmit Queue is full.
        errXmtQueueFull = 1
        ' Period of time is invalid. 
        errIllegalPeriod = 2
        ' Client Receive Queue is empty.
        errRcvQueueEmpty = 3
        ' Checksum type is invalid. 
        errIllegalChecksumType = 4
        ' Hardware handle is invalid.
        errIllegalHardware = 5
        ' Client handle is invalid.
        errIllegalClient = 6
        ' Parameter type is invalid.
        errWrongParameterType = 7
        ' Parameter value is invalid.
        errWrongParameterValue = 8
        ' Direction is invalid.
        errIllegalDirection = 9
        ' Length is outside of the valid range
        errIllegalLength = 10
        ' Baudrate is outside of the valid range
        errIllegalBaudrate = 11
        ' ID is outside of the valid range.
        errIllegalFrameID = 12
        ' Buffer parameter is too small
        errBufferInsufficient = 13
        ' Scheduler Number is outside of the valid range.
        errIllegalScheduleNo = 14
        ' Slots count is bigger than the actual number of available slots.
        errIllegalSlotCount = 15
        ' Array index is outside of the valid range.
        errIllegalIndex = 16
        ' Range of bytes to be updated is invalid.
        errIllegalRange = 17
        ' LIN Manager does not have enough resources for the current task.
        errOutOfResource = 1001
        ' The LIN Device Manager is not running.
        errManagerNotLoaded = 1002
        ' The communication to the LIN Device Manager was interrupted.
        errManagerNotResponding = 1003
        ' A "MemoryAccessViolation" exception occurred within an API method.
        errMemoryAccess = 1004
        ' An API method is not implemented.
        errNotImplemented = &HFFFE
        ' An internal error occurred within the LIN Device Manager.
        errUnknown = &HFFFF
    End Enum
#End Region

#Region "Structures"
    ' Version Information structure
    '
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure TLINVersion
        ' Major part of a version number
        Public Major As Short
        ' Minor part of a version number
        Public Minor As Short
        ' Revision part of a version number
        Public Revision As Short
        ' Build part of a version number
        Public Build As Short
    End Structure

    ' A LIN Message to be sent
    '
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure TLINMsg
        ' Frame ID (6 bit) + Parity (2 bit)
        Public FrameId As Byte
        ' Frame Length (1..8)
        Public Length As Byte
        ' Frame Direction (see TLINDirection)
        <MarshalAs(UnmanagedType.U1)> _
        Public Direction As TLINDirection
        ' Frame Checksum type (see TLINChecksumType)
        <MarshalAs(UnmanagedType.U1)> _
        Public ChecksumType As TLINChecksumType
        ' Data bytes (0..7)
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Public Data As Byte()
        ' Frame Checksum
        Public Checksum As Byte
    End Structure

    ' A received LIN Message
    '
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure TLINRcvMsg
        ' Frame type (see TLINMsgType)
        <MarshalAs(UnmanagedType.U1)> _
        Public Type As TLINMsgType
        ' Frame ID (6 bit) + Parity (2 bit)
        Public FrameId As Byte
        'Frame Length (1..8)
        Public Length As Byte
        ' Frame Direction (see TLINDirection)
        <MarshalAs(UnmanagedType.U1)> _
        Public Direction As TLINDirection
        ' Frame Checksum type (see TLINChecksumType)
        <MarshalAs(UnmanagedType.U1)> _
        Public ChecksumType As TLINChecksumType
        ' Data bytes (0..7)
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Public Data As Byte()
        ' Frame Checksum
        Public Checksum As Byte
        ' Frame error flags (see TLINMsgErrors)
        <MarshalAs(UnmanagedType.I4)> _
        Public ErrorFlags As TLINMsgErrors
        ' Timestamp in microseconds
        Public TimeStamp As ULong
        ' Handle of the Hardware which received the message
        Public hHw As HLINHW
    End Structure

    ' A LIN Frame Entry
    '
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure TLINFrameEntry
        ' Frame ID (without parity)
        Public FrameId As Byte
        ' Frame Length (1..8)
        Public Length As Byte
        ' Frame Direction (see TLINDirection)
        <MarshalAs(UnmanagedType.U1)> _
        Public Direction As TLINDirection
        ' Frame Checksum type (see TLINChecksumType)
        <MarshalAs(UnmanagedType.U1)> _
        Public ChecksumType As TLINChecksumType
        ' Frame flags (see FRAME_FLAG_...)
        Public Flags As UShort
        ' Data bytes (0..7)
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Public InitialData As Byte()
    End Structure

    ' A LIN Schedule slot
    '
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure TLINScheduleSlot
        ' Slot Type (see TLINSlotType)
        <MarshalAs(UnmanagedType.U1)> _
        Public Type As TLINSlotType
        ' Slot Delay in Milliseconds
        Public Delay As UShort
        ' Frame IDs (without parity)
        <MarshalAs(UnmanagedType.ByValArray, SizeConst:=8)> _
        Public FrameId As Byte()
        ' ID count for sporadic frames
        ' Resolve schedule number for Event frames
        Public CountResolve As Byte
        ' Slot handle (read-only)
        Public Handle As UInteger
    End Structure

    ' LIN Status data
    '
    <StructLayout(LayoutKind.Sequential)> _
    Public Structure TLINHardwareStatus
        ' Node state (see TLINHardwareMode)
        <MarshalAs(UnmanagedType.U1)> _
        Public Mode As TLINHardwareMode
        ' Bus state (see TLINHardwareState)
        <MarshalAs(UnmanagedType.U1)> _
        Public Status As TLINHardwareState
        ' Count of free places in the Transmit Queue
        Public FreeOnSendQueue As Byte
        ' Free slots in the Schedule pool (see LIN_MAX_SCHEDULE_SLOTS)
        Public FreeOnSchedulePool As UShort
        ' USB receive buffer overrun counter
        Public ReceiveBufferOverrun As UShort
    End Structure
#End Region
    Public Class PLinApi
#Region "Constants"
        ' Invalid value for all LIN handles (Client, Hardware)
        '
        Public Const INVALID_LIN_HANDLE As Byte = 0

        ' Hardware Types:  LIN USB type
        '
        Public Const LIN_HW_TYPE_USB As Byte = 1

        ' Maximum allowed Frame ID
        '
        Public Const LIN_MAX_FRAME_ID = 63

        ' Maximum allowed Schedules per Hardware
        '
        Public Const LIN_MAX_SCHEDULES As Integer = 8

        ' Minimum Schedule number
        '
        Public Const LIN_MIN_SCHEDULE_NUMBER As Integer = 0

        ' Maximum Schedule number
        '
        Public Const LIN_MAX_SCHEDULE_NUMBER As Integer = 7

        ' Maximum allowed Schedule slots per Hardware
        '
        Public Const LIN_MAX_SCHEDULE_SLOTS As Integer = 256

        ' Minimum LIN Baudrate
        '
        Public Const LIN_MIN_BAUDRATE As UShort = 1000

        ' Maximum LIN Baudrate
        '
        Public Const LIN_MAX_BAUDRATE As UShort = 20000

        ' Maximum number of bytes for Name / ID of a Hardware or Client.
        '
        Public Const LIN_MAX_NAME_LENGTH As UShort = 48

        ' Slave Enable Publisher Response
        '
        Public Const FRAME_FLAG_RESPONSE_ENABLE As Integer = &H1

        ' Slave Publisher Single shot
        '
        Public Const FRAME_FLAG_SINGLE_SHOT As Integer = &H2

        ' Ignore InitialData on set frame entry
        '
        Public Const FRAME_FLAG_IGNORE_INIT_DATA As Integer = &H4

        ' Maximum number of bytes that a user can read/write on a Hardware.
        '
        Public Const LIN_MAX_USER_DATA As Integer = 24
#End Region

#Region "Function prototypes"
        ' <summary>
        ' Registers a Client at the LIN Manager. Creates a Client handle and
        ' allocates the Receive Queue (only one per Client). The hWnd parameter
        ' can be zero for DOS Box Clients. The Client does not receive any
        ' messages until LIN_RegisterFrameId() or LIN_SetClientFilter() is called.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:  
        '   errWrongParameterValue 
        ' </summary>
        ' <param name="strName">Name of the Client</param>
        ' <param name="hWnd">Window handle of the Client (only for information purposes)</param>
        ' <param name="hClient">Client handle buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_RegisterClient")> _
        Public Shared Function RegisterClient( _
            ByVal strName As String, _
            ByVal hWnd As Integer, _
            ByRef hClient As HLINCLIENT) As TLINError
        End Function


        ' <summary>
        ' Removes a Client from the Client list of the LIN Manager. Frees all
        ' resources (Receive Queues, message counters, etc.). If the Client was 
        ' a Boss-Client for one or more Hardware, the Boss-Client property for
        ' those Hardware will be set to INVALID_LIN_HANDLE.
        ' 
        ' Possible DLL interaction errors:  
        '   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:  
        '    errWrongParameterValue, errIllegalClient 
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_RemoveClient")> _
        Public Shared Function RemoveClient( _
            ByVal hClient As HLINCLIENT) As TLINError
        End Function


        ' <summary>
        ' Connects a Client to a Hardware.
        ' The Hardware is assigned by its Handle.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:  
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware to be connected</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ConnectClient")> _
        Public Shared Function ConnectClient( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Disconnects a Client from a Hardware. This means: no more messages 
        ' will be received by this Client from this Hardware.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware to be disconnected</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_DisconnectClient")> _
        Public Shared Function DisconnectClient( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Flushes the Receive Queue of the Client and resets its counters.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ResetClient")> _
        Public Shared Function ResetClient( _
            ByVal hClient As HLINCLIENT) As TLINError
        End Function


        ' <summary>
        ' Sets a Client parameter to a given value.
        ' 
        '    Allowed TLINClientParam    Parameter
        '    values in wParam:          type:       Description:
        '    -------------------------  ----------  -------------------------------
        '    clpReceiveStatusFrames     Integer     0 = Status Frames deactivated,
        '                                           otherwise active
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterType, errWrongParameterValue, errIllegalClient
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="wParam">Parameter to set</param>
        ' <param name="dwValue">parameter value</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetClientParam")> _
        Public Shared Function SetClientParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal wParam As TLINClientParam, _
            ByVal dwValue As Integer) As TLINError
        End Function


        ' <summary>
        ' Gets a Client parameter.
        ' 
        '    Allowed TLINClientParam    Parameter
        '    values in wParam:          type:       Description:
        '    -------------------------  ----------  -------------------------------
        '    clpName                    String      Name of the Client
        '    clpMessagesOnQueue         Integer     Unread messages in the Receive Queue
        '    clpWindowHandle            Integer     Window handle of the Client application
        '                                           (can be zero for DOS Box Clients)
        '    clpConnectedHardware       HLINHW()    Array of Hardware Handles connected by a Client
        '                                           The first item in the array refers to the 
        '                                           amount of handles. So [*] = Total handles + 1
        '    clpTransmittedMessages     Integer     Number of transmitted messages
        '    clpReceivedMessages        Integer     Number of received messages
        '    clpReceiveStatusFrames     Integer     0 = Status Frames deactivated, otherwise active
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterType, errWrongParameterValue, errIllegalClient,
        '    errBufferInsufficient
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="wParam">Parameter to get</param>
        ' <param name="pBuff">Buffer for the parameter value</param>
        ' <param name="wBuffSize">Buffer size in bytes</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetClientParam")> _
        Public Shared Function GetClientParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal wParam As TLINClientParam, _
            ByRef pBuff As Integer, _
            ByVal wBuffSize As Integer) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetClientParam")> _
        Public Shared Function GetClientParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal wParam As TLINClientParam, _
            <MarshalAs(UnmanagedType.LPStr, SizeParamIndex:=3)> _
            ByVal pBuff As StringBuilder, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetClientParam")> _
        Public Shared Function GetClientParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal wParam As TLINClientParam, _
            <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=3)> _
            ByVal pBuff As HLINHW(), _
            ByVal wBuffSize As UShort) As TLINError
        End Function


        ' <summary>
        ' Sets the filter of a Client and modifies the filter of
        ' the connected Hardware.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="iRcvMask">Filter. Each bit corresponds to a Frame ID (0..63)</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetClientFilter")> _
        Public Shared Function SetClientFilter( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            <MarshalAs(UnmanagedType.I8)> _
            ByVal iRcvMask As ULong) As TLINError
        End Function


        ' <summary>
        ' Gets the filter corresponding to a given Client-Hardware pair.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="pRcvMask">Filter buffer. Each bit corresponds to a Frame ID (0..63)</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetClientFilter")> _
        Public Shared Function GetClientFilter( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            <MarshalAs(UnmanagedType.I8)> _
            ByRef pRcvMask As ULong) As TLINError
        End Function


        ' <summary>
        ' Reads the next message/status information from a Client's Receive
        ' Queue. The message will be written to 'pMsg'.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errRcvQueueEmpty
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="pMsg">Message read buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_Read")> _
        Public Shared Function Read( _
            ByVal hClient As HLINCLIENT, _
            ByRef pMsg As TLINRcvMsg) As TLINError
        End Function


        ' <summary>
        ' Reads several received messages.
        ' pMsgBuff must be an array of 'iMaxCount' entries (must have at least 
        ' a size of iMaxCount * sizeof(TLINRcvMsg) bytes). 
        ' The size 'iMaxCount' of the array = max. messages that can be received.
        ' The real number of read messages will be returned in 'pCount'.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errRcvQueueEmpty
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="pMsgBuff">Messages buffer</param>
        ' <param name="iMaxCount"> Maximum number of messages to read (pMsgBuff length)</param>
        ' <param name="pCount">Buffer for the real number of messages read</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ReadMulti")> _
        Public Shared Function ReadMulti( _
            ByVal hClient As HLINCLIENT, _
            <[In](), Out()> _
            ByVal pMsgBuff As TLINRcvMsg(), _
            ByVal iMaxCount As Integer, _
            ByRef pCount As Integer) As TLINError
        End Function


        ' <summary>
        ' The Client 'hClient' transmits a message 'pMsg' to the Hardware 'hHw'.
        ' The message is written into the Transmit Queue of the Hardware.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalDirection, errIllegalLength
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="pMsg">Message write buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_Write")> _
        Public Shared Function Write( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByRef pMsg As TLINMsg) As TLINError
        End Function


        ' <summary>
        ' Initializes a Hardware as Master or Slave with a given Baudrate.
        ' 
        ' <remarks>
        ' If the Hardware was initialized by another Client, the function
        ' will re-initialize the Hardware. All connected clients will be affected.
        ' It is the job of the user to manage the setting and/or configuration of
        ' Hardware, e.g. by using the Boss-Client parameter of the Hardware.
        ' </remarks>
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalBaudrate
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="byMode">Hardware mode (see TLINHardwareMode)</param>
        ' <param name="wBaudrate">Baudrate (see LIN_MIN_BAUDRATE and LIN_MAX_BAUDRATE)</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_InitializeHardware")> _
        Public Shared Function InitializeHardware( _
             ByVal hClient As HLINCLIENT, _
             ByVal hHw As HLINHW, _
            <MarshalAs(UnmanagedType.U1)> _
            ByVal byMode As TLINHardwareMode, _
            ByVal wBaudrate As UShort) As TLINError
        End Function


        ' <summary>
        ' Gets an array containing the handles of the current Hardware 
        ' available in the system.
        ' The count of Hardware handles returned in the array is written in 
        ' 'pCount'.
        ' 
        ' <remarks>
        ' To ONLY get the count of available Hardware, call this
        ' function using 'pBuff' = NULL and wBuffSize = 0.
        ' </remarks>
        '
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errBufferInsufficient
        ' </summary>
        ' <param name="pBuff">Buffer for the handles</param>
        ' <param name="wBuffSize">Size of the buffer in bytes (pBuff.Length * 2)</param>
        ' <param name="pCount">Number of Hardware available</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetAvailableHardware")> _
        Public Shared Function GetAvailableHardware( _
            <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=1)> _
            ByVal pBuff As HLINHW(), _
            ByVal wBuffSize As UShort, _
            ByRef pCount As UShort) As TLINError
        End Function


        ' <summary>
        ' Sets a Hardware parameter to a given value.
        ' 
        '    Allowed TLINHardwareParam  Parameter
        '    values in wParam:          type:       Description:
        '    -------------------------  ----------  -------------------------------
        '    hwpMessageFilter           ULong       Hardware message filter. Each bit
        '                                           corresponds to a Frame ID (0..63)
        '    hwpBossClient              HLINCLIENT  Handle of the new Boss-Client
        '    hwpIdNumber                Integer     Identification number for a hardware
        '    hwpUserData                Byte()      User data to write on a hardware. See LIN_MAX_USER_DATA
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterType, errWrongParameterValue, errIllegalClient,
        '    errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="wParam">Parameter to set</param>
        ' <param name="pBuff">Parameter value buffer</param>
        ' <param name="wBuffSize">Value buffer size</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetHardwareParam")> _
            Public Shared Function SetHardwareParam( _
                ByVal hClient As HLINCLIENT, _
                ByVal hHw As HLINHW, _
                ByVal wParam As TLINHardwareParam, _
                <MarshalAs(UnmanagedType.I8)> _
                ByRef pBuff As ULong, _
                ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_SetHardwareParam")> _
        Public Shared Function SetHardwareParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            <MarshalAs(UnmanagedType.I8)> _
            ByRef pBuff As HLINCLIENT, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_SetHardwareParam")> _
        Public Shared Function SetHardwareParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            <MarshalAs(UnmanagedType.I4)> _
            ByRef pBuff As Integer, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_SetHardwareParam")> _
        Public Shared Function SetHardwareParam( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=4)> _
            ByVal pBuff As Byte(), _
            ByVal wBuffSize As UShort) As TLINError
        End Function


        ' <summary>
        ' Gets a Hardware parameter.
        ' 
        '    Allowed TLINHardwareParam  Parameter
        '    values in wParam:          type:       Description:
        '    -------------------------  ----------  -------------------------------
        '    hwpName                    String      Name of the Hardware. See LIN_MAX_NAME_LENGTH
        '    hwpDeviceNumber            Integer     Index of the Device owner of the Hardware
        '    hwpChannelNumber           Integer     Channel Index of the Hardware on the owner device
        '    hwpConnectedClients        HLINCLIENT()Array of Client Handles conencted to a Hardware
        '                                           The first item in the array refers to the 
        '                                           amount of handles. So [*] = Total handles + 1
        '    hwpMessageFilter           ULong       Configured message filter. Each bit corresponds 
        '                                           to a Frame ID (0..63)
        '    hwpBaudrate                Integer     Configured baudrate
        '    hwpMode                    Integer     0 = Slave, otehrwise Master
        '    hwpFirmwareVersion         TLINVersion A TLINVersion structure containing the Firmware Version
        '    hwpBufferOverrunCount      Integer     Receive Buffer Overrun Counter
        '    hwpBossClient              HLINCLIENT  Handle of the current Boss-Client
        '    hwpSerialNumber            Integer     Serial number of the Hardware
        '    hwpVersion                 Integer     Version of the Hardware
        '    hwpType                    Integer     Type of the Hardware
        '    hwpQueueOverrunCount       Integer     Receive Queue Buffer Overrun Counter
        '    hwpIdNumber                Integer     Identification number for a hardware
        '    hwpUserData                Byte()      User data saved on the hardware. See LIN_MAX_USER_DATA
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterType, errWrongParameterValue, errIllegalHardware,
        '    errBufferInsufficient
        ' </summary>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="wParam">Parameter to get</param>
        ' <param name="pBuff">Parameter buffer</param>
        ' <param name="wBuffSize">Buffer size</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetHardwareParam")> _
        Public Shared Function GetHardwareParam( _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            ByRef pBuff As Integer, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetHardwareParam")> _
        Public Shared Function GetHardwareParam( _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            <MarshalAs(UnmanagedType.LPStr, SizeParamIndex:=3)> _
            ByVal pBuff As StringBuilder, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetHardwareParam")> _
        Public Shared Function GetHardwareParam( _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=3)> _
            ByVal pBuff As Byte(), _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetHardwareParam")> _
        Public Shared Function GetHardwareParam( _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            <MarshalAs(UnmanagedType.LPStruct, SizeParamIndex:=3)> _
            ByVal pBuff As TLINVersion, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetHardwareParam")> _
        Public Shared Function GetHardwareParam( _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            ByRef pBuff As ULong, _
            ByVal wBuffSize As UShort) As TLINError
        End Function

        <DllImport("plinapi.dll", EntryPoint:="LIN_GetHardwareParam")> _
        Public Shared Function GetHardwareParam( _
            ByVal hHw As HLINHW, _
            ByVal wParam As TLINHardwareParam, _
            ByRef pBuff As HLINCLIENT, _
            ByVal wBuffSize As UShort) As TLINError
        End Function


        ' <summary>
        ' Flushes the Queues of the Hardware and resets its counters.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ResetHardware")> _
        Public Shared Function ResetHardware( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Deletes the current configuration of the Hardware and sets its defaults.
        ' The Client 'hClient' must be registered and connected to the Hardware to
        ' be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ResetHardwareConfig")> _
        Public Shared Function ResetHardwareConfig( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Phisically identifies a LIN Hardware (a channel on a LIN Device) by
        ' blinking its associated LED.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalHardware
        ' </summary>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_IdentifyHardware")> _
        Public Shared Function IdentifyHardware( _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Modifies the filter of a Client and, eventually, the filter of the 
        ' connected Hardware. The messages with FrameID 'bFromFrameId' to 
        ' 'bToFrameId' will be received.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalFrameID
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="bFromFrameId">First ID of the frame range</param>
        ' <param name="bToFrameId">Last ID of the frame range</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_RegisterFrameId")> _
        Public Shared Function RegisterFrameId( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal bFromFrameId As Byte, _
            ByVal bToFrameId As Byte) As TLINError
        End Function


        ' <summary>
        ' Configures a LIN Frame in a given Hardware. The Client 'hClient' must
        ' be registered and connected to the Hardware to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalFrameID, errIllegalLength
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="pFrameEntry">Frame entry information buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetFrameEntry")> _
        Public Shared Function SetFrameEntry( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByRef pFrameEntry As TLINFrameEntry) As TLINError
        End Function


        ' <summary>
        ' Gets the configuration of a LIN Frame from a given Hardware.
        ' 
        ' <remarks>
        ' The 'pFrameEntry.FrameId' must be set to the ID of the frame, whose
        ' configuration should be returned.
        ' </remarks>
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalHardware, errIllegalFrameID
        ' </summary>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="pFrameEntry">Frame entry buffer. The member FrameId 
        ' must be set to the ID of the frame, whose configuration should 
        ' be returned </param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetFrameEntry")> _
        Public Shared Function GetFrameEntry( _
            ByVal hHw As HLINHW, _
            ByRef pFrameEntry As TLINFrameEntry) As TLINError
        End Function


        ' <summary>
        ' Updates the data of a LIN Frame for a given Hardware. The Client 
        ' 'hClient' must be registered and connected to the Hardware to be 
        ' accessed. 'pData' must have at least a size of 'bLen'.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalFrameID, errIllegalLength, errIllegalIndex,
        '    errIllegalRange
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="bFrameId">Frame ID</param>
        ' <param name="bIndex">Index where the update data Starts (0..7)</param>
        ' <param name="bLen">Count of Data bytes to be updated.</param>
        ' <param name="pData">Data buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_UpdateByteArray")> _
        Public Shared Function UpdateByteArray( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal bFrameId As Byte, _
            ByVal bIndex As Byte, _
            ByVal bLen As Byte, _
            <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=4)> _
            ByVal pData As Byte()) As TLINError
        End Function


        ' <summary>
        ' Sets the Frame 'bFrameId' as Keep-Alive frame for the given Hardware and
        ' starts to send it every 'wPeriod' Milliseconds. The Client 'hClient' must 
        ' be registered and connected to the Hardware to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalFrameID
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="bFrameId">ID of the Keep-Alive Frame</param>
        ' <param name="wPeriod">Keep-Alive Interval in Milliseconds</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_StartKeepAlive")> _
        Public Shared Function StartKeepAlive( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal bFrameId As Byte, _
            ByVal wPeriod As UShort) As TLINError
        End Function


        ' <summary>
        ' Suspends the sending of a Keep-Alive frame in the given Hardware.
        ' The Client 'hClient' must be registered and connected to the Hardware
        ' to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SuspendKeepAlive")> _
        Public Shared Function SuspendKeepAlive( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Resumes the sending of a Keep-Alive frame in the given Hardware. 
        ' The Client 'hClient' must be registered and connected to the Hardware
        ' to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ResumeKeepAlive")> _
        Public Shared Function ResumeKeepAlive( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Configures the slots of a Schedule in a given Hardware. The Client
        ' 'hClient' must be registered and connected to the Hardware to be
        ' accessed. The Slot handles will be returned in the parameter
        ' "pSchedule" (Slots buffer), when this function successfully completes.  
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalScheduleNo, errIllegalSlotCount
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="iScheduleNumber">Schedule number (see 
        ' LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        ' <param name="pSchedule">Slots buffer</param>
        ' <param name="iSlotCount">Count of Slots in the slots buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetSchedule")> _
        Public Shared Function SetSchedule( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal iScheduleNumber As Integer, _
            <MarshalAs(UnmanagedType.LPArray, SizeParamIndex:=4)> _
            <[In](), Out()> _
            ByVal pSchedule As TLINScheduleSlot(), _
            ByVal iSlotCount As Integer) As TLINError
        End Function


        ' <summary>
        ' Gets the slots of a Schedule from a given Hardware. The count of slots 
        ' returned in the array is written in 'pSlotCount'.
        ' 
        ' <remarks>
        ' To ONLY get the count of slots contained in the given Schedule,
        ' call this function using 'pScheduleBuff' = NULL and iMaxSlotCount = 0.
        ' </remarks>
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalHardware, errIllegalScheduleNo,
        '    errIllegalSlotCount
        ' </summary>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="iScheduleNumber">Schedule Number (see 
        ' LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        ' <param name="pScheduleBuff">Slots Buffer.</param>
        ' <param name="iMaxSlotCount">Maximum number of slots to read.</param>
        ' <param name="pSlotCount">Real number of slots read.</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetSchedule")> _
        Public Shared Function GetSchedule( _
            ByVal hHw As HLINHW, _
            ByVal iScheduleNumber As Integer, _
            <[In](), Out()> _
            ByVal pScheduleBuff As TLINScheduleSlot(), _
            ByVal iMaxSlotCount As Integer, _
            ByRef pSlotCount As Integer) As TLINError
        End Function


        ' <summary>
        ' Removes all slots contained by a Schedule of a given Hardware. The
        ' Client 'hClient' must be registered and connected to the Hardware to
        ' be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalScheduleNo
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="iScheduleNumber">Schedule Number (see 
        ' LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_DeleteSchedule")> _
        Public Shared Function DeleteSchedule( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal iScheduleNumber As Integer) As TLINError
        End Function


        ' <summary>
        ' Sets a 'breakpoint' on a slot from a Schedule in a given Hardware. The 
        ' Client 'hClient' must be registered and connected to the Hardware to
        ' be accessed.
        ' 
        ' <remarks>
        ' Giving 'dwHandle' a value of 0 ('Nothing'), causes the deletion of
        ' the breakpoint.
        ' </remarks>
        '
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="iBreakPointNumber">Breakpoint Number (0 or 1)</param>
        ' <param name="dwHandle">Slot Handle</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetScheduleBreakPoint")> _
        Public Shared Function SetScheduleBreakPoint( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal iBreakPointNumber As Integer, _
            ByVal dwHandle As Integer) As TLINError
        End Function


        ' <summary>
        ' Activates a Schedule in a given Hardware. The Client 'hClient' must
        ' be registered and connected to the Hardware to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware,
        '    errIllegalScheduleNo
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="iScheduleNumber">Schedule Number (see 
        ' LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_StartSchedule")> _
        Public Shared Function StartSchedule( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal iScheduleNumber As Integer) As TLINError
        End Function


        ' <summary>
        ' Suspends an active Schedule in a given Hardware. The Client 'hClient'
        ' must be registered and connected to the Hardware to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SuspendSchedule")> _
        Public Shared Function SuspendSchedule( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Restarts a configured Schedule in a given Hardware. The Client 'hClient'
        ' must be registered and connected to the Hardware to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_ResumeSchedule")> _
        Public Shared Function ResumeSchedule( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Sends a wake-up impulse (single data byte &HF0). The Client 'hClient'
        ' must be registered and connected to the Hardware to be accessed.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_XmtWakeUp")> _
        Public Shared Function XmtWakeUp( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW) As TLINError
        End Function


        ' <summary>
        ' Starts a process to detect the Baud rate of the LIN bus that is
        ' connected to the indicated Hardware.
        ' The Client 'hClient' must be registered and connected to the Hardware
        ' to be accessed. The Hardware must be not initialized in order to do an
        ' Auto-baudrate procedure.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalClient, errIllegalHardware
        ' </summary>
        ' <param name="hClient">Handle of the Client</param>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="wTimeOut">Auto-baudrate Timeout in Milliseconds</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_StartAutoBaud")> _
        Public Shared Function StartAutoBaud( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            ByVal wTimeOut As UShort) As TLINError
        End Function


        ' <summary>
        ' Retrieves current status information from the given Hardware.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalHardware
        ' </summary>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="pStatusBuff">Status data buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetStatus")> _
        Public Shared Function GetStatus( _
            ByVal hHw As HLINHW, _
            ByRef pStatusBuff As TLINHardwareStatus) As TLINError
        End Function


        ' <summary>
        ' Calculates the checksum of a LIN Message and writes it into the
        ' 'Checksum' field of 'pMsg'.
        '
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalLength
        ' </summary>
        ' <param name="pMsg">Message buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_CalculateChecksum")> _
        Public Shared Function CalculateChecksum( _
            ByRef pMsg As TLINMsg) As TLINError
        End Function

        ' <summary>
        ' Returns a TLINVersion sttructure containing the PLIN-API DLL version.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue
        ' </summary>
        ' <param name="pVerBuff"> Version buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetVersion")> _
        Public Shared Function GetVersion( _
            ByRef pVerBuff As TLINVersion) As TLINError
        End Function


        ' <summary>
        ' Returns a string containing Copyright information.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue
        ' </summary>
        ' <param name="pTextBuff">String buffer</param>
        ' <param name="wBuffSize">Size in bytes of the buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetVersionInfo")> _
        Public Shared Function GetVersionInfo( _
            <MarshalAs(UnmanagedType.LPStr, SizeParamIndex:=1)> _
            ByVal pTextBuff As StringBuilder, _
            ByVal wBuffSize As Integer) As TLINError
        End Function


        ' <summary>
        ' Converts the error code 'dwError' to a text containing an error
        ' description in the language given as parameter (when available).
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errBufferInsufficient
        ' </summary>
        ' <param name="dwError">A TLINError Code</param>
        ' <param name="bLanguage">Indicates a "Primary language ID"</param>
        ' <param name="strTextBuff">Error string buffer</param>
        ' <param name="wBuffSize">Buffer size in bytes</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetErrorText")> _
        Public Shared Function GetErrorText( _
            ByVal dwError As TLINError, _
            ByVal bLanguage As Byte, _
            <MarshalAs(UnmanagedType.LPStr, SizeParamIndex:=3)> _
            ByVal strTextBuff As StringBuilder, _
            ByVal wBuffSize As Integer) As TLINError
        End Function

        ' <summary>
        ' Gets the 'FrameId with Parity' corresponding to the given
        ' 'pFrameId' and writes the result on it.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalFrameID
        ' </summary>
        ' <param name="pFrameId">Frame ID (0..LIN_MAX_FRAME_ID)</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetPID")> _
        Public Shared Function GetPID( _
            ByRef pFrameId As Byte) As TLINError
        End Function

        ' <summary>
        ' Gets the system time used by the LIN-USB adapter.
        ' 
        ' Possible DLL interaction errors:  
        '    errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue, errIllegalHardware
        ' </summary>
        ' <param name="hHw">Handle of the Hardware</param>
        ' <param name="pTargetTime">Target Time buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetTargetTime")> _
        Public Shared Function GetTargetTime( _
            ByVal hHw As HLINHW, _
            ByRef pTargetTime As ULong) As TLINError
        End Function


        ' <summary>
        ' Sets the Response Remap of a LIN Slave
        ' 
        ' Possible DLL interaction errors:
        '   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        '   
        ' Possible API errors:
        '   errWrongParameterValue, errIllegalFrameID, errIllegalClient, errIllegalHardware,
        '   errMemoryAccess
        ' </summary>
        ' <param name="hClient">Client Handle</param>
        ' <param name="hHw">Hardware Handle</param>
        ' <param name="pRemapTab">Remap Response buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_SetResponseRemap")> _
        Public Shared Function SetResponseRemap( _
            ByVal hClient As HLINCLIENT, _
            ByVal hHw As HLINHW, _
            <MarshalAs(UnmanagedType.LPArray, SizeConst:=64)> _
            ByVal pRemapTab As Byte()) As TLINError
        End Function

        ' <summary>
        ' Gets the Response Remap of a LIN Slave
        ' </summary>
        ' <param name="hHw">Hardware Handle</param>
        ' <param name="pRemapTab">Remap Response buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetResponseRemap")> _
        Public Shared Function GetResponseRemap( _
            ByVal hHw As HLINHW, _
            <MarshalAs(UnmanagedType.LPArray, SizeConst:=64)> _
            ByVal pRemapTab As Byte()) As TLINError
        End Function

        ' <summary>
        ' Gets the current system time. The system time is returned by
        ' Windows as the elapsed number of microseconds since system start.
        ' 
        ' Possible DLL interaction errors:  
        '    errMemoryAccess
        '
        ' Possible API errors:
        '    errWrongParameterValue
        ' </summary>
        ' <param name="pSystemTime">System Timer buffer</param>
        ' <returns>A LIN Error Code</returns>
        <DllImport("plinapi.dll", EntryPoint:="LIN_GetSystemTime")> _
        Public Shared Function GetSystemTime( _
            ByRef pSystemTime As ULong) As TLINError
        End Function
#End Region
    End Class
End Namespace

