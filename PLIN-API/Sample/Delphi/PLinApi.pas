///////////////////////////////////////////////////////////////////////////////
//  PLinApi.pas
//  Version 2.2.28.71
//
//  Definition of the PLIN-API.
//
//
//    Principle:
//    ~~~~~~~~~~
//    The driver supports multiple clients (= Windows or DOS programs
//    that communicate with LIN-busses), and multiple LIN-Devices.
//    A LIN-Device represent two LIN-Hardware (each channel is a Hardware)
//    Multiple 'Clients' can be connected to one or more LIN-Hardware, which
//    itself have an interface to a physical LIN-channel of a device.
//
//    Features:
//    ~~~~~~~~~
//     - 1 Client can be connected to multiple Hardware
//     - 1 Hardware supports multiple clients
//     - When a Client sends a message to a Hardware, the message will not be routed
//       to other clients. The response of the Hardware is routed to the connected
//       clients depending on the registered Hardware message filter.
//     - each Client only receives the messages that pass its acceptance filter
//     - each Client has a Receive Queue to buffer received messages
//     - hClient: 'Client handle'. This number is used by the driver to
//                identify and manage a Client
//     - hHw:     'Hardware handle'. This number is used by the driver to
//                identify and manage a Hardware
//     - all handles are 1-based. 0 = illegal handle
//
//     All functions return a value of type TLINError
//
//     Authors: K.Wagner / P.Steil / K.Wolf
//
//     -----------------------------------------------------------------------
//     Copyright (C) 2008-2011 by PEAK-System Technik GmbH, Darmstadt/Germany
//     -----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

unit PLinApi;

interface

const
    // Invalid Handle values
    //
    INVALID_LIN_HANDLE            = 0;              // Invalid value for all LIN handles (Client, Hardware)

    // Hardware types
    //
    LIN_HW_TYPE_USB               = 1;              // LIN USB type

    // Minimum and Maximum values
    //
    LIN_MAX_FRAME_ID              = 63;             // Maximum allowed Frame ID
    LIN_MAX_SCHEDULES             = 8;              // Maximum allowed Schedules per Hardware
    LIN_MIN_SCHEDULE_NUMBER       = 0;              // Minimum Schedule number
    LIN_MAX_SCHEDULE_NUMBER       = 7;              // Maximum Schedule number
    LIN_MAX_SCHEDULE_SLOTS        = 256;            // Maximum allowed Schedule slots per Hardware
    LIN_MIN_BAUDRATE              = 1000;           // Minimum LIN Baudrate
    LIN_MAX_BAUDRATE              = 20000;          // Maximum LIN Baudrate
    LIN_MAX_NAME_LENGTH           = 48;             // Maximum number of bytes for Name / ID of a Hardware or Client
    LIN_MAX_USER_DATA             = 24;             // Maximum number of bytes that a user can read/write on a Hardware

    // Frame flags for LIN Frame Entries
    //
    FRAME_FLAG_RESPONSE_ENABLE    = $1;             // Slave Enable Publisher Response
    FRAME_FLAG_SINGLE_SHOT        = $2;             // Slave Publisher Single shot
    FRAME_FLAG_IGNORE_INIT_DATA   = $4;             // Ignore InitialData on set frame entry

    // Error flags for LIN Rcv Msgs
    //
    MSG_ERR_INCONSISTENT_SYNC     = $1;             // Error on Synchronization field
    MSG_ERR_ID_PARITY_BIT0        = $2;             // Wrong parity Bit 0
    MSG_ERR_ID_PARITY_BIT1        = $4;             // Wrong parity Bit 1
    MSG_ERR_SLAVE_NOT_RESPONDING  = $8;             // Slave not responding error
    MSG_ERR_TIMEOUT               = $10;            // A timeout was reached
    MSG_ERR_CHECKSUM              = $20;            // Wrong checksum
    MSG_ERR_GND_SHORT             = $40;            // Bus short to ground
    MSG_ERR_VBAT_SHORT            = $80;            // Bus short to VBat.
    MSG_ERR_SLOT_DELAY            = $100;           // A slot time (delay) was too small
    MSG_ERR_OTHER_RESPONSE        = $200;           // Response was received from other station


type
    // Client Handle
    //
    HLINCLIENT = Byte;

    //  Hardware Handle
    //
    HLINHW = Word;
    PHLINHW = ^HLINHW;
    PBYTE = ^Byte;

{$Z4}
    // Error flags for LIN Rcv Msgs
    //
    TLINMsgErrors = (
        InconsistentSynch = $01,                    // Error on Synchronization field
        IdParityBit0 = $02,                         // Wrong parity Bit 0
        IdParityBit1 = $04,                         // Wrong parity Bit 1
        SlaveNotResponding = $08,                   // Slave not responding error
        Timeout = $10,                              // A timeout was reached
        Checksum = $20,                             // Wrong checksum
        GroundShort = $40,                          // Bus shorted to ground
        VBatShort = $80,                            // Bus shorted to VBat
        SlotDelay = $100,                           // A slot time (delay) was too small
        OtherResponse = $200                        // Response was received from other station
    );

{$Z2}
    // Client Parameters (GetClientParam Function)
    //
    TLINClientParam = (
        clpName = 1,                                // Client Name
        clpMessagesOnQueue = 2,                     // Unread messages in the Receive Queue
        clpWindowHandle = 3,                        // Registered windows handle (information purpose)
        clpConnectedHardware = 4,                   // Handles of the connected Hardware
        clpTransmittedMessages = 5,                 // Number of transmitted messages
        clpReceivedMessages = 6,                    // Number of received messages
        clpReceiveStatusFrames = 7,                 // Status of the property "Status Frames"
        clpOnReceiveEventHandle = 8,                // Handle of the Receive event
        clpOnPluginEventHandle = 9                  // Handle of the Hardware plug-in event
    );

{$Z2}
    // Hardware Parameters (GetHardwareParam function)
    //
    TLINHardwareParam = (
        hwpName = 1,                                // Hardware / Device Name.
        hwpDeviceNumber = 2,                        // Index of the owner Device
        hwpChannelNumber = 3,                       // Channel Index on the owner device (0 or 1)
        hwpConnectedClients = 4,                    // Handles of the connected clients
        hwpMessageFilter = 5,                       // Message filter
        hwpBaudrate = 6,                            // Baudrate
        hwpMode = 7,                                // Master status
        hwpFirmwareVersion = 8,                     // Lin Firmware Version (Text with the form xx.yy where:
                                                    // xx = major version. yy = minor version)
        hwpBufferOverrunCount = 9,                  // Receive Buffer Overrun Counter
        hwpBossClient = 10,                         // Registered master Client
        hwpSerialNumber = 11,                       // Serial number of a Hardware
        hwpVersion = 12,                            // Version of a Hardware
        hwpType = 13,                               // Type of a Hardware
        hwpQueueOverrunCount = 14,                  // Receive Queue Buffer Overrun Counter
        hwpIdNumber = 15,                           // Hardware identification number
        hwpUserData = 16                            // User data on a hardware
    );

{$Z1}
    // Received Message Types
    //
    TLINMsgType = (
        mstStandard = 0,                            // Standard LIN Message
        mstBusSleep = 1,                            // Bus Sleep status message
        mstBusWakeUp = 2,                           // Bus WakeUp status message
        mstAutobaudrateTimeOut = 3,                 // Auto-baudrate Timeout status message
        mstAutobaudrateReply = 4,                   // Auto-baudrate Reply status message
        mstOverrun = 5,                             // Bus overrun status message
        mstQueueOverrun = 6                         // Queue overrun status message
    );

    // Schedule Slot Types
    //
    TLINSlotType = (
        sltUnconditional = 0,                       // Unconditional frame
        sltEvent = 1,                               // Event frame
        sltSporadic = 2,                            // Sporadic frame
        sltMasterRequest = 3,                       // Diagnostic Master Request frame
        sltSlaveResponse = 4                        // Diagnostic Slave Response frame
    );

    // LIN Message Direction Types
    //
    TLINDirection = (
        dirDisabled = 0,                            // Direction disabled
        dirPublisher = 1,                           // Direction is Publisher
        dirSubscriber = 2,                          // Direction is Subscriber
        dirSubscriberAutoLength = 3                 // Direction is Subscriber (detect Length)
    );

    // LIN Message checksum Types
    //
    TLINChecksumType = (
        cstCustom = 0,                              // Custom checksum
        cstClassic = 1,                             // Classic checksum (ver 1.x)
        cstEnhanced = 2,                            // Enhanced checksum
        cstAuto = 3                                 // Detect checksum
    );

    // LIN Hardware operation mode
    //
    TLINHardwareMode = (
        modNone = 0,                                // Hardware is not initialized
        modSlave = 1,                               // Hardware working as Slave
        modMaster = 2                               // Hardware working as Master
    );

    // LIN Bus states
    //
    TLINHardwareState = (
        hwsNotInitialized = 0,                      // Hardware is not initialized
        hwsAutobaudrate = 1,                        // Hardware is detecting the baudrate
        hwsActive = 2,                              // Hardware (bus) is active
        hwsSleep = 3,                               // Hardware (bus) is in sleep mode
        hwsShortGround = 6                          // Hardware (bus-line) shorted to ground
    );

{$Z4}
    // LIN Error Codes
    //
    TLINError = (
        errOK                   = 0,                // No error. Success.
        errXmtQueueFull         = 1,                // Transmit Queue is full.
        errIllegalPeriod        = 2,                // Period of time is invalid.
        errRcvQueueEmpty        = 3,                // Client Receive Queue is empty.
        errIllegalChecksumType  = 4,                // Checksum type is invalid.
        errIllegalHardware      = 5,                // Hardware handle is invalid.
        errIllegalClient        = 6,                // Client handle is invalid.
        errWrongParameterType   = 7,                // Parameter type is invalid.
        errWrongParameterValue  = 8,                // Parameter value is invalid.
        errIllegalDirection     = 9,                // Direction is invalid.
        errIllegalLength        = 10,               // Length is outside of the valid range.
        errIllegalBaudrate      = 11,               // Baudrate is outside of the valid range.
        errIllegalFrameID       = 12,               // ID is outside of the valid range.
        errBufferInsufficient   = 13,               // Buffer parameter is too small.
        errIllegalScheduleNo    = 14,               // Scheduler Number is outside of the valid range.
        errIllegalSlotCount     = 15,               // Slots count is bigger than the actual number of available slots.
        errIllegalIndex         = 16,               // Array index is outside of the valid range.
        errIllegalRange         = 17,               // Range of bytes to be updated is invalid.
        errOutOfResource        = 1001,             // LIN Manager does not have enough resources for the current task.
        errManagerNotLoaded     = 1002,             // The LIN Device Manager is not running.
        errManagerNotResponding = 1003,             // The communication to the LIN Device Manager was interrupted.
        errMemoryAccess         = 1004,             // A "MemoryAccessViolation" exception occurred within an API method.
        errNotImplemented       = $FFFE,            // An API method is not implemented.
        errUnknown              = $FFFF             // An internal error occurred within the LIN Device Manager.
    );

{$A8} // These Records are 8-Bytes aligned!

    //  Version Information structure
    //
    TLINVersion = record
        Major: Smallint;                            // Major part of a version number
        Minor: Smallint;                            // Minor part of a version number
        Revision: Smallint;                         // Revision part of a version number
        Build: Smallint;                            // Build part of a version number
    end;

    // A LIN Message to be sent
    //
    TLINMsg = record
        FrameId: Byte;                              // Frame ID (6 bit) + Parity (2 bit)
        Length: Byte;                               // Frame Length (1..8)
        Direction: TLINDirection;                   // Frame Direction (see TLINDirection)
        ChecksumType: TLINChecksumType;             // Frame Checksum type (see TLINChecksumType)
        Data: array[0..7] of Byte;                  // Data bytes (0..7)
        Checksum: Byte;                             // Frame Checksum
    end;

    // A received LIN Message
    //
    TLINRcvMsg = record
        MsgType: TLINMsgType;                       // Frame type (see TLINMsgType)
        FrameId: Byte;                              // Frame ID (6 bit) + Parity (2 bit)
        Length: Byte;                               // Frame Length (1..8)
        Direction: TLINDirection;                   // Frame Direction (see TLINDirection)
        ChecksumType: TLINChecksumType;             // Frame Checksum type
        Data: array[0..7] of Byte;                  // Data bytes (0..7)
        Checksum: Byte;                             // Frame Checksum
        ErrorFlags: TLINMsgErrors;                  // Frame error flags (see TLINMsgErrors)
        TimeStamp: UInt64;                          // Timestamp in Microseconds
        hHw: HLINHW;                                // Handle of the Hardware which received the message
    end;

    // A LIN Frame Entry
    //
    TLINFrameEntry = record
        FrameId: Byte;                              // Frame ID (without parity)
        Length: Byte;                               // Frame Length (1..8)
        Direction: TLINDirection;                   // Frame Direction (see TLINDirection)
        ChecksumType: TLINChecksumType;             // Frame Checksum type (see TLINChecksumType)
        Flags: Word;                                // Frame flags (see FRAME_FLAG_...)
        InitialData: array[0..7] of Byte;           // Data bytes (0..7)
    end;

    // A LIN Schedule slot
    //
    TLINScheduleSlot = record
        &Type: TLINSlotType;                        // Slot Type (see TLINSlotType)
        Delay: Word;                                // Slot Delay in Milliseconds
        FrameId: array[0..7] of Byte;               // Frame IDs (without parity)
        CountResolve: Byte;                         // ID count for sporadic frames
                                                    // Resolve schedule number for Event frames
        Handle: Longword;                           // Slot handle (read-only)
    end;

    // LIN Status data
    //
    TLINHardwareStatus = record
        Mode: TLINHardwareMode;                     // Node state (see TLINHardwareMode)
        Status: TLINHardwareState;                  // Bus state (see TLINHardwareState)
        FreeOnSendQueue: Byte;                      // Count of free places in the Transmit Queue
        FreeOnSchedulePool: Word;                   // Free slots in the Schedule pool (see LIN_MAX_SCHEDULE_SLOTS)
        ReceiveBufferOverrun: Word;                 // USB receive buffer overrun counter
    end;

///////////////////////////////////////////////////////////////////////////////
// Function prototypes

//-----------------------------------------------------------------------------
// LIN_RegisterClient()
//   Registers a Client at the LIN Manager. Creates a Client handle and
//   allocates the Receive Queue (only one per Client). The hWnd parameter
//   can be zero for DOS Box Clients. The Client does not receive any
//   messages until LIN_RegisterFrameId() or LIN_SetClientFilter() is called.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue
//
function LIN_RegisterClient(
    const strName: PAnsiChar;           // Name of the Client
    hWnd: Longword;                     // Window handle of the Client (only for information purposes)
    var hClient: HLINCLIENT             // Pointer to the Client handle buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_RemoveClient()
//   Removes a Client from the Client list of the LIN Manager. Frees all
//   resources (receive queues, message counters, etc.). If the Client was
//   a Boss-Client for one or more Hardware, the Boss-Client property for
//   those Hardware will be set to INVALID_LIN_HANDLE.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient
//
function LIN_RemoveClient(
    hClient: HLINCLIENT                 // Handle of the Client
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ConnectClient()
//   Connects a Client to a Hardware.
//   The Hardware is assigned by its Handle.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_ConnectClient(
    hClient: HLINCLIENT;                // connect this Client ...
    hHw: HLINHW                         // to this Hardware
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_DisconnectClient()
//   Disconnects a Client from a Hardware. This means: no more messages
//   will be received by this Client from this Hardware.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_DisconnectClient(
    hClient: HLINCLIENT;                // Disconnect this Client ...
    hHw: HLINHW                         // from this Hardware.
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ResetClient()
//   Flushes the Receive Queue of the Client and resets its counters.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient
//
function LIN_ResetClient(
    hClient: HLINCLIENT                 // Handle of the Client
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SetClientParam()
//   Sets a Client parameter to a given value.
//
//   Allowed TLINClientParam    Parameter
//   values in wParam:          type:       Description:
//   -------------------------  ----------  -----------------------------------
//   clpReceiveStatusFrames     Integer     0 = Status Frames deactivated,
//                                          otherwise active
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterType, errWrongParameterValue, errIllegalClient
//
function LIN_SetClientParam(
    hClient: HLINCLIENT;                // Handle of the Client
    wParam: TLINClientParam;            // TLINClientParam parameter
    dwValue: Integer                    // Parameter value
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetClientParam()
//   Gets a Client parameter.
//
//   Allowed TLINClientParam    Parameter
//   values in wParam:          type:       Description:
//   -------------------------  ----------  -----------------------------------
//   clpName                    array of    Name of the Client. See LIN_MAX_NAME_LENGTH
//                              Char
//   clpMessagesOnQueue         Integer     Unread messages in the Receive Queue
//   clpWindowHandle            Integer     Window handle of the Client application
//                                          (can be zero for DOS Box Clients)
//   clpConnectedHardware       array of
//                              HLINHW      Array of Hardware Handles connected by a Client
//                                          The first item in the array refers to the
//                                          amount of handles. So [*] = Total handles + 1
//   clpTransmittedMessages     Integer     Number of transmitted messages
//   clpReceivedMessages        Integer     Number of received messages
//   clpReceiveStatusFrames     Integer     0 = Status Frames deactivated, otherwise active
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterType, errWrongParameterValue, errIllegalClient,
//   errBufferInsufficient
//
function LIN_GetClientParam(
    hClient: HLINCLIENT;                // Client Handle
    wParam: TLINClientParam;            // TLINClientParam parameter
    pBuff: Pointer;                     // Buffer for the parameter value
    wBuffSize: Word                     // Size of the buffer in bytes
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SetClientFilter()
//   Sets the filter of a Client and modifies the filter of
//   the connected Hardware.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_SetClientFilter(
    hClient: HLINCLIENT;                // Set for this Client
    hHw: HLINHW;                        // within this Hardware
    iRcvMask: UInt64                    // this message filter: each bit corresponds to a Frame ID (0..63).
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetClientFilter()
//   Gets the filter corresponding to a given Client-Hardware pair.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_GetClientFilter(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    var pRcvMask: UInt64                // Buffer for the message filter: each bit corresponds to a Frame ID (0..63)
    ): TLINError; stdcall;

//-----------------------------------------------------------------------------
// LIN_Read()
//   Reads the next message/status information from a Client's Receive
//   Queue. The message will be written to 'pMsg'.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errRcvQueueEmpty
//
function LIN_Read(
    hClient: HLINCLIENT;                // Client Handle
    var pMsg: TLINRcvMsg                // Buffer for the message
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ReadMulti()
//   Reads several received messages.
//   pMsgBuff must be an array of 'iMaxCount' entries (must have at least
//   a size of iMaxCount * sizeof(TLINRcvMsg) bytes).
//   The size 'iMaxCount' of the array = max. messages that can be received.
//   The real number of read messages will be returned in 'pCount'.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errRcvQueueEmpty
//
function LIN_ReadMulti(
    hClient: HLINCLIENT;                // Client Handle
    pMsgBuff: Pointer;                  // Buffer for the messages
    iMaxCount: Integer;                 // Maximum number of messages to read
    var pCount: Integer                 // Buffer for the real number of messages read
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_Write()
//   The Client 'hClient' transmits a message 'pMsg' to the Hardware 'hHw'.
//   The message is written into the Transmit Queue of the Hardware.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalDirection, errIllegalLength
//
function LIN_Write(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    var pMsg: TLINMsg                   // Message Buffer to be written
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_InitializeHardware()
//   Initializes a Hardware with a given Mode and Baudrate.
//   REMARK: If the Hardware was initialized by another Client, the function
//   will re-initialize the Hardware. All connected clients will be affected.
//   It is the job of the user to manage the setting and/or configuration of
//   Hardware, e.g. by using the Boss-Client parameter of the Hardware.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalBaudrate
//
function LIN_InitializeHardware(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    bMode: TLINHardwareMode;            // Hardware Mode (see TLINHardwareMode)
    wBaudrate: Word                     // LIN Baudrate (see LIN_MIN_BAUDRATE and LIN_MAX_BAUDRATE)
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetAvailableHardware()
//   Gets an array containing the handles of the current Hardware
//   available in the system.
//   The count of Hardware handles returned in the array is written in
//   'pCount'.
//
//   REMARK: To ONLY get the count of available Hardware, call this
//   function using 'pBuff' = NULL and wBuffSize = 0.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errBufferInsufficient
//
function LIN_GetAvailableHardware(
    pBuff: PHLINHW;                     // Buffer for the handles
    wBuffSize: Word;                    // Size of the buffer in bytes
    var pCount: Integer                 // Number of Hardware available
    ): TLINError; stdcall;

//-----------------------------------------------------------------------------
// LIN_SetHardwareParam()
//   Sets a Hardware parameter to a given value.
//
//   Allowed TLINHardwareParam  Parameter
//   values in wParam:          type:       Description:
//   -------------------------  ----------  -----------------------------------
//   hwpMessageFilter           UInt64      Hardware message filter. Each bit
//                                          corresponds to a Frame ID (0..63)
//   hwpBossClient              HLINCLIENT  Handle of the new Boss-Client
//   hwpIdNumber                Integer     Identification number for a hardware
//   hwpUserData                array of    User data to write on a hardware. See LIN_MAX_USER_DATA
//                              Byte
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterType, errWrongParameterValue, errIllegalClient,
//   errIllegalHardware
//
function LIN_SetHardwareParam(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    wParam: TLINHardwareParam;          // TLINHardwareParam parameter
    pBuff: Pointer;                     // Buffer for the parameter value
    wBuffSize: Word                     // Size of the buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetHardwareParam()
//   Gets a Hardware parameter.
//
//   Allowed TLINHardwareParam  Parameter
//   values in wParam:          type:       Description:
//   -------------------------  ----------  -----------------------------------
//   hwpName                    array of    Name of the Hardware. See LIN_MAX_NAME_LENGTH
//                              Char
//   hwpDeviceNumber            Integer     Index of the Device owner of the Hardware
//   hwpChannelNumber           Integer     Channel Index of the Hardware on the owner device
//   hwpConnectedClients        array of 
//                              HLINCLIENT  Array of Client Handles conencted to a Hardware
//                                          The first item in the array refers to the
//                                          amount of handles. So [*] = Total handles + 1
//   hwpMessageFilter           UInt64      Configured message filter. Each bit corresponds
//                                          to a Frame ID (0..63)
//   hwpBaudrate                Integer     Configured baudrate
//   hwpMode                    TLINHardwareMode
//                                          A TLINHardwareMode value
//   hwpFirmwareVersion         TLINVersion A TLINVersion structure containing the Firmware Version
//   hwpBufferOverrunCount      Integer     Receive Buffer Overrun Counter.
//   hwpBossClient              HLINCLIENT  Handle of the current Boss-Client
//   hwpSerialNumber            Integer     Serial number of the Hardware
//   hwpVersion                 Integer     Version of the Hardware
//   hwpType                    Integer     Type of the Hardware
//   hwpQueueOverrunCount       Integer     Receive Queue Buffer Overrun Counter
//   hwpIdNumber                Integer     Identification number for a hardware
//   hwpUserData                array of    User data saved on the hardware. See LIN_MAX_USER_DATA
//                              Byte
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterType, errWrongParameterValue, errIllegalHardware,
//   errBufferInsufficient
//
function LIN_GetHardwareParam(
    hHw: HLINHW;                        // Hardware Handle
    wParam: TLINHardwareParam;          // TLINHardwareParam parameter
    pBuff: Pointer;                     // Buffer for the parameter value
    wBuffSize: Word                     // Size of the buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ResetHardware()
//   Flushes the queues of the Hardware and resets its counters.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_ResetHardware(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ResetHardwareConfig()
//   Deletes the current configuration of the Hardware and sets its defaults.
//   The Client 'hClient' must be registered and connected to the Hardware to
//   be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_ResetHardwareConfig(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_IdentifyHardware()
//   Phisically identifies a LIN Hardware (a channel on a LIN Device) by
//   blinking its associated LED.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalHardware
//
function LIN_IdentifyHardware(
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_RegisterFrameId()
//   Modifies the filter of a Client and, eventually, the filter of the
//   connected Hardware. The messages with FrameID 'bFromFrameId' to
//   'bToFrameId' will be received.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalFrameID
//
function LIN_RegisterFrameId(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    bFromFrameId: Byte;                 // First ID of the frame range
    bToFrameId: Byte                    // Last ID of the frame range
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SetFrameEntry()
//   Configures a LIN Frame in a given Hardware. The Client 'hClient' must
//   be registered and connected to the Hardware to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalFrameID, errIllegalLength
//
function LIN_SetFrameEntry(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    var pFrameEntry: TLINFrameEntry     // Frame entry buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetFrameEntry()
//   Gets the configuration of a LIN Frame from a given Hardware.
//   The 'pFrameEntry.FrameId' must be set to the ID of the frame, whose
//   configuration should be returned.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalHardware, errIllegalFrameID
//
function LIN_GetFrameEntry(
    hHw: HLINHW;                        // Hardware Handle
    var pFrameEntry: TLINFrameEntry     // Frame Entry buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_UpdateByteArray()
//   Updates the data of a LIN Frame for a given Hardware. The Client
//   'hClient' must be registered and connected to the Hardware to be
//   accessed. 'pData' must have at least a size of 'bLen'.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalFrameID, errIllegalLength, errIllegalIndex,
//   errIllegalRange
//
function LIN_UpdateByteArray(
    hClient: HLINCLIENT;                // Client handle
    hHw: HLINHW;                        // Hardware Handle
    bFrameId: Byte;                     // Frame ID
    bIndex: Byte;                       // Index where the update data Starts (0..7)
    bLen: Byte;                         // Count of Data bytes to be updated.
    pData: Pointer                      // Data buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_StartKeepAlive()
//   Sets the Frame 'bFrameId' as Keep-Alive frame for the given Hardware and
//   starts to send it every 'wPeriod' Milliseconds. The Client 'hClient' must
//   be registered and connected to the Hardware to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalFrameID
//
function LIN_StartKeepAlive(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    bFrameId: Byte;                     // ID of the Keep-Alive Frame
    wPeriod: Word                       // Keep-Alive Interval in Milliseconds
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SuspendKeepAlive()
//   Suspends the sending of a Keep-Alive frame in the given Hardware.
//   The Client 'hClient' must be registered and connected to the Hardware
//   to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_SuspendKeepAlive(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ResumeKeepAlive()
//   Resumes the sending of a KeepAlive frame in the given Hardware.
//   The Client 'hClient' must be registered and connected to the Hardware
//   to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_ResumeKeepAlive(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SetSchedule()
//   Configures the slots of a Schedule in a given Hardware. The Client
//   'hClient' must be registered and connected to the Hardware to be
//   accessed. The Slot handles will be returned in the parameter
//   "pSchedule" (Slots buffer), when this function successfully completes.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalScheduleNo, errIllegalSlotCount
//
function LIN_SetSchedule(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    iScheduleNumber: Integer;           // Schedule number (see LIN_MIN_SCHEDULE_NUMBER
                                        // and LIN_MAX_SCHEDULE_NUMBER)
    pSchedule: Pointer;                 // Slots buffer (TLINScheduleSlot pointer)
    iSlotCount: Integer                 // Count of Slots in the slots buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetSchedule()
//   Gets the slots of a Schedule from a given Hardware. The count of slots
//   returned in the array is written in 'pSlotCount'.
//
//   REMARK: To ONLY get the count of slots contained in the given Schedule,
//   call this function using 'pScheduleBuff' = NULL and iMaxSlotCount = 0.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalHardware, errIllegalScheduleNo,
//   errIllegalSlotCount
//
function LIN_GetSchedule(
    hHw: HLINHW;                        // Hardware Handle
    iScheduleNumber: Integer;           // Schedule Number (see LIN_MIN_SCHEDULE_NUMBER
                                        // and LIN_MAX_SCHEDULE_NUMBER)
    pScheduleBuff: Pointer;             // Slots Buffer (TLINScheduleSlot pointer)
    iMaxSlotCount: Integer;             // Maximum number of slots to read
    var pSlotCount: Integer             // Real number of slots read
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_DeleteSchedule()
//   Removes all slots contained by a Schedule of a given Hardware. The
//   Client 'hClient' must be registered and connected to the Hardware to
//   be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//   errIllegalScheduleNo
//
function LIN_DeleteSchedule(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    iScheduleNumber: Integer            // Schedule Number (see LIN_MIN_SCHEDULE_NUMBER
                                        // and LIN_MAX_SCHEDULE_NUMBER)
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SetScheduleBreakPoint()
//   Sets a 'breakpoint' on a slot from a Schedule in a given Hardware. The
//   Client 'hClient' must be registered and connected to the Hardware to
//   be accessed.
//
//   REMARK: Giving 'dwHandle' a value of 0 ('nil'), causes the deletion of
//   the breakpoint.

// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_SetScheduleBreakPoint(
    hClient: HLINCLIENT;               // Client Handle
    hHw: HLINHW;                       // Hardware Handle
    iBreakPointNumber: Integer;        // Breakpoint Number (0 or 1)
    dwHandle: Integer                  // Slot Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_StartSchedule()
//   Activates a Schedule in a given Hardware. The Client 'hClient' must
//   be registered and connected to the Hardware to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware,
//   errIllegalScheduleNo
//
function LIN_StartSchedule (
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // HArdware Handle
    iScheduleNumber: Integer            // Schedule Number (see LIN_MIN_SCHEDULE_NUMBER
                                        // and LIN_MAX_SCHEDULE_NUMBER)
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_SuspendSchedule()
//   Suspends an active Schedule in a given Hardware. The Client 'hClient'
//   must be registered and connected to the Hardware to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_SuspendSchedule (
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_ResumeSchedule()
//   Restarts a configured Schedule in a given Hardware. The Client 'hClient'
//   must be registered and connected to the Hardware to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_ResumeSchedule(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_XmtWakeUp()
//   Sends a wake-up impulse (single data byte $F0). The Client 'hClient'
//   must be registered and connected to the Hardware to be accessed.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_XmtWakeUp(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW                         // Hardware Handle
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_StartAutoBaud()
//   Starts a process to detect the Baud rate of the LIN bus that is
//   connected to the indicated Hardware.
//   The Client 'hClient' must be registered and connected to the Hardware
//   to be accessed. The Hardware must be not initialized in order to do an
//   Auto-baudrate procedure.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalClient, errIllegalHardware
//
function LIN_StartAutoBaud(
    hClient: HLINCLIENT;                // Client Handle
    hHw: HLINHW;                        // Hardware Handle
    wTimeOut: Word                      // Auto-baudrate Timeout in Milliseconds
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetStatus()
//   Retrieves current status information from the given Hardware.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalHardware
//
function LIN_GetStatus(
    hHw: HLINHW;                        // Hardware handle
    var pStatusBuff: TLINHardwareStatus // Status data buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_CalculateChecksum()
//   Calculates the checksum of a LIN Message and writes it into the
//   'Checksum' field of 'pMsg'.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalLength
//
function LIN_CalculateChecksum(
    var pMsg: TLINMsg                   // Message buffer
    ): TLINError; stdcall;

//-----------------------------------------------------------------------------
// LIN_GetVersion()
//   Returns a TLINVersion sttructure containing the PLIN-API DLL version.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue
//
function LIN_GetVersion(
    var pVerBuff: TLINVersion           // Version buffer
    ): TLINError; stdcall;

//-----------------------------------------------------------------------------
// LIN_GetVersionInfo()
//   Returns a string containing Copyright information.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue
//
function LIN_GetVersionInfo(
    strTextBuff: PAnsiChar;             // String buffer
    wBuffSize: Word                     // Size in bytes of the buffer
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetErrorText()
//   Converts the error code 'dwError' to a text containing an error
//   description in the language given as parameter (when available).
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errBufferInsufficient
//
function LIN_GetErrorText (
    dwError: TLINError;                 // A TLINError code
    bLanguage: Byte;                    // Indicates a "Primary language ID"
    strTextBuff: PAnsiChar;             // Error string buffer
    wBuffSize: Word                     // Buffer size in bytes
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetPID()
//   Gets the 'FrameId with Parity' corresponding to the given
//   'pFrameId' and writes the result on it.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalFrameID
//
function LIN_GetPID(
    var pFrameId: Byte                  // Frame ID (0..LIN_MAX_FRAME_ID)
    ): TLINError; stdcall;


//-----------------------------------------------------------------------------
// LIN_GetTargetTime()
//   Gets the system time used by the LIN-USB adapter.
//
// Possible DLL interaction errors:
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalHardware
//
function LIN_GetTargetTime(
    hHw: HLINHW;                        // Hardware handle
    var pTargetTime: UInt64             // Target Time buffer
    ): TLINError; stdcall;

//-----------------------------------------------------------------------------
// LIN_SetResponseRemap()
//   Sets the Response Remap of a LIN Slave
//
// Possible DLL interaction errors: 
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalFrameID, errIllegalClient, errIllegalHardware,
//   errMemoryAccess
//
function LIN_SetResponseRemap(
    hClient: HLINCLIENT;                // Client handle
    hHw: HLINHW;                        // Hardware handle 
    pRemapTab: PBYTE                    // Remap Response buffer (64 bytes)  
    ): TLINError; stdcall;

//-----------------------------------------------------------------------------
// LIN_GetResponseRemap()
//   Gets the Response Remap of a LIN Slave
//
//  REMARK: The Buffer must be at least 64 bytes length 
//  the breakpoint.
//
// Possible DLL interaction errors: 
//   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue, errIllegalHardware, errMemoryAccess
//
function LIN_GetResponseRemap(
    hHw: HLINHW;                        // Hardware handle
    pRemapTab: PBYTE                    // Remap Response buffer (64 bytes)
    ): TLINError; stdcall;
    
//-----------------------------------------------------------------------------
// LIN_GetSystemTime()
//   Gets the current system time. The system time is returned by 
//   Windows as the elapsed number of microseconds since system start.
//
// Possible DLL interaction errors:
//   errMemoryAccess
//
// Possible API errors:
//   errWrongParameterValue
//
function LIN_GetSystemTime(
    var pTargetTime: UInt64             // Target Time buffer
    ): TLINError; stdcall;    

implementation

const DLL_Name = 'PLinApi.dll';


function LIN_RegisterClient(const strName: PAnsiChar; hWnd: Longword; var hClient: HLINCLIENT): TLINError; stdcall;
external DLL_Name;

function LIN_RemoveClient(hClient: HLINCLIENT): TLINError; stdcall;
external DLL_Name;

function LIN_ConnectClient(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_DisconnectClient(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_ResetClient(hClient: HLINCLIENT): TLINError; stdcall;
external DLL_Name;

function LIN_SetClientParam(hClient: HLINCLIENT; wParam: TLINClientParam; dwValue: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_GetClientParam(hClient: HLINCLIENT; wParam: TLINClientParam; pBuff: Pointer; wBuffSize: Word): TLINError; stdcall;
external DLL_Name;

function LIN_SetClientFilter(hClient: HLINCLIENT; hHw: HLINHW; iRcvMask: UInt64): TLINError; stdcall;
external DLL_Name;

function LIN_GetClientFilter(hClient: HLINCLIENT; hHw: HLINHW; var pRcvMask: UInt64): TLINError; stdcall;
external DLL_Name;

function LIN_Read(hClient: HLINCLIENT; var pMsg: TLINRcvMsg): TLINError; stdcall;
external DLL_Name;

function LIN_ReadMulti(hClient: HLINCLIENT; pMsgBuff: Pointer; iMaxCount: Integer; var pCount: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_Write(hClient: HLINCLIENT; hHw: HLINHW; var pMsg: TLINMsg ): TLINError; stdcall;
external DLL_Name;

function LIN_InitializeHardware(hClient: HLINCLIENT; hHw: HLINHW; bMode: TLINHardwareMode; wBaudrate: Word): TLINError; stdcall;
external DLL_Name;

function LIN_GetAvailableHardware(pBuff: PHLINHW; wBuffSize: Word; var pCount: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_SetHardwareParam(hClient: HLINCLIENT; hHw: HLINHW; wParam: TLINHardwareParam; pBuff: Pointer; wBuffSize: Word): TLINError; stdcall;
external DLL_Name;

function LIN_GetHardwareParam(hHw: HLINHW; wParam: TLINHardwareParam; pBuff: Pointer; wBuffSize: Word): TLINError; stdcall;
external DLL_Name;

function LIN_ResetHardware(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_ResetHardwareConfig(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_IdentifyHardware(hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_RegisterFrameId(hClient: HLINCLIENT; hHw: HLINHW; bFromFrameId: Byte; bToFrameId: Byte): TLINError; stdcall;
external DLL_Name;

function LIN_SetFrameEntry(hClient: HLINCLIENT; hHw: HLINHW; var pFrameEntry: TLINFrameEntry): TLINError; stdcall;
external DLL_Name;

function LIN_GetFrameEntry(hHw: HLINHW; var pFrameEntry: TLINFrameEntry): TLINError; stdcall;
external DLL_Name;

function LIN_UpdateByteArray(hClient: HLINCLIENT; hHw: HLINHW; bFrameId: Byte; bIndex: Byte; bLen: Byte; pData: Pointer): TLINError; stdcall;
external DLL_Name;

function LIN_StartKeepAlive(hClient: HLINCLIENT; hHw: HLINHW; bFrameId: Byte; wPeriod: Word): TLINError; stdcall;
external DLL_Name;

function LIN_SuspendKeepAlive(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_ResumeKeepAlive(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_SetSchedule(hClient: HLINCLIENT; hHw: HLINHW; iScheduleNumber: Integer; pSchedule: Pointer; iSlotCount: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_GetSchedule(hHw: HLINHW; iScheduleNumber: Integer; pScheduleBuff: Pointer; iMaxSlotCount: Integer; var pSlotCount: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_DeleteSchedule(hClient: HLINCLIENT; hHw: HLINHW; iScheduleNumber: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_SetScheduleBreakPoint(hClient: HLINCLIENT; hHw: HLINHW; iBreakPointNumber: Integer; dwHandle: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_StartSchedule (hClient: HLINCLIENT; hHw: HLINHW; iScheduleNumber: Integer): TLINError; stdcall;
external DLL_Name;

function LIN_SuspendSchedule (hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_ResumeSchedule(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_XmtWakeUp(hClient: HLINCLIENT; hHw: HLINHW): TLINError; stdcall;
external DLL_Name;

function LIN_StartAutoBaud(hClient: HLINCLIENT; hHw: HLINHW; wTimeOut: Word): TLINError; stdcall;
external DLL_Name;

function LIN_GetStatus(hHw: HLINHW; var pStatusBuff: TLINHardwareStatus): TLINError; stdcall;
external DLL_Name;

function LIN_CalculateChecksum(var pMsg: TLINMsg): TLINError; stdcall;
external DLL_Name;

function LIN_GetVersion(var pVerBuff: TLINVersion): TLINError; stdcall;
external DLL_Name;

function LIN_GetVersionInfo(strTextBuff: PAnsiChar; wBuffSize: Word): TLINError; stdcall;
external DLL_Name;

function LIN_GetErrorText (dwError: TLINError; bLanguage: Byte; strTextBuff: PAnsiChar; wBuffSize: Word): TLINError; stdcall;
external DLL_Name;

function LIN_GetPID(var pFrameId: Byte): TLINError; stdcall;
external DLL_Name;

function LIN_GetTargetTime(hHw: HLINHW; var pTargetTime: UInt64): TLINError; stdcall;
external DLL_Name;

function LIN_SetResponseRemap(hClient: HLINCLIENT; hHw: HLINHW; pRemapTab: PBYTE): TLINError; stdcall;
external DLL_Name;

function LIN_GetResponseRemap(hHw: HLINHW; pRemapTab: PBYTE): TLINError; stdcall;
external DLL_Name;

function LIN_GetSystemTime(var pTargetTime: UInt64): TLINError; stdcall;
external DLL_Name;

end.
