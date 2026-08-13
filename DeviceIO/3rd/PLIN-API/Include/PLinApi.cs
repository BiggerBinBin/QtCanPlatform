///////////////////////////////////////////////////////////////////////////////
//  PLinApi.cs
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
//     - hHw: 'Hardware handle'. This number is used by the driver to
//                identify and manage a Hardware
//     - all handles are 1-based. 0 = illegal handle
//
//     All functions return a value of type TLINError
//
//     Authors: K.Wagner / P.Steil
//
//     -----------------------------------------------------------------------
//     Copyright (C) 2008-2013 by PEAK-System Technik GmbH, Darmstadt/Germany
//     -----------------------------------------------------------------------
//     
//     Last Change: 15.03.2013 K.Wagner
///////////////////////////////////////////////////////////////////////////////
using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Peak.Lin
{
    /// <summary>
    /// Client Handle
    /// </summary>
    using HLINCLIENT = System.Byte;
    /// <summary>
    /// Hardware Handle
    /// </summary>
    using HLINHW = System.UInt16;

    #region Enumerations
    /// <summary>
    /// Error flags for LIN Rcv Msgs
    /// </summary>
    [Flags]
    public enum TLINMsgErrors : int
    {
        /// <summary>
        /// Error on Synchronization field
        /// </summary>
        InconsistentSynch = 0x1,
        /// <summary>
        /// Wrong parity Bit 0
        /// </summary>
        IdParityBit0 = 0x2,
        /// <summary>
        /// Wrong parity Bit 1
        /// </summary>
        IdParityBit1 = 0x4,
        /// <summary>
        /// Slave not responding error
        /// </summary>
        SlaveNOtResponding = 0x8,
        /// <summary>
        /// A timeout was reached
        /// </summary>
        Timeout = 0x10,
        /// <summary>
        /// Wrong checksum
        /// </summary>
        Checksum = 0x20,
        /// <summary>
        /// Bus shorted to ground
        /// </summary>
        GroundShort = 0x40,
        /// <summary>
        /// Bus shorted to VBat
        /// </summary>
        VBatShort = 0x80,
        /// <summary>
        /// A slot time (delay) was too small
        /// </summary>
        SlotDelay = 0x100,
        /// <summary>
        /// Response was received from other station
        /// </summary>
        OtherResponse = 0x200,
    }

    /// <summary>
    /// Client Parameters (GetClientParam Function)
    /// </summary>
    public enum TLINClientParam : ushort
    {
        /// <summary>
        /// Client Name
        /// </summary>
        clpName = 1,
        /// <summary>
        /// Unread messages in the Receive Queue
        /// </summary>
        clpMessagesOnQueue = 2,
        /// <summary>
        /// Registered windows handle (information purpose)
        /// </summary>
        clpWindowHandle = 3,
        /// <summary>
        /// Handles of the connected Hardware
        /// </summary>
        clpConnectedHardware = 4,
        /// <summary>
        /// Number of transmitted messages
        /// </summary>
        clpTransmittedMessages = 5,
        /// <summary>
        /// Number of received messages
        /// </summary>
        clpReceivedMessages = 6,
        /// <summary>
        /// Status of the property "Status Frames"
        /// </summary>
        clpReceiveStatusFrames = 7,
        /// <summary>
        /// Handle of the Receive event
        /// </summary>
        clpOnReceiveEventHandle = 8,
        /// <summary>
        /// Handle of the Hardware plug-in event
        /// </summary>
        clpOnPluginEventHandle = 9,
    }

    /// <summary>
    /// Hardware Parameters (GetHardwareParam function)
    /// </summary>
    public enum TLINHardwareParam : ushort
    {
        /// <summary>
        /// Hardware / Device Name
        /// </summary>
        hwpName = 1,
        /// <summary>
        /// Index of the owner Device
        /// </summary>
        hwpDeviceNumber = 2,
        /// <summary>
        /// Channel Index on the owner device (0 or 1)
        /// </summary>
        hwpChannelNumber = 3,
        /// <summary>
        /// Handles of the connected clients
        /// </summary>
        hwpConnectedClients = 4,
        /// <summary>
        /// Message filter
        /// </summary>
        hwpMessageFilter = 5,
        /// <summary>
        /// Baudrate
        /// </summary>
        hwpBaudrate = 6,
        /// <summary>
        /// Master status
        /// </summary>
        hwpMode = 7,
        /// <summary>
        /// Lin Firmware Version (Text with the form xx.yy where:
        /// xx = major version. yy = minor version)
        /// </summary>
        hwpFirmwareVersion = 8,
        /// <summary>
        /// Receive Buffer Overrun Counter
        /// </summary>
        hwpBufferOverrunCount = 9,
        /// <summary>
        /// Registered master Client
        /// </summary>
        hwpBossClient = 10,
        /// <summary>
        /// Serial number of a Hardware
        /// </summary>
        hwpSerialNumber = 11,
        /// <summary>
        /// Version of a Hardware
        /// </summary>
        hwpVersion = 12,
        /// <summary>
        /// Type of a Hardware
        /// </summary>
        hwpType = 13,
        /// <summary>
        /// Receive Queue Buffer Overrun Counter
        /// </summary>
        hwpQueueOverrunCount = 14,
        /// <summary>
        /// Hardware identificaiton number
        /// </summary>
        hwpIdNumber = 15,
        /// <summary>
        /// User data on a hardware
        /// </summary>
        hwpUserData = 16,      
    }

    /// <summary>
    /// Received Message Types
    /// </summary>
    public enum TLINMsgType : byte
    {
        /// <summary>
        /// Standard LIN Message
        /// </summary>
        mstStandard = 0,
        /// <summary>
        /// Bus Sleep status message
        /// </summary>
        mstBusSleep = 1,
        /// <summary>
        /// Bus WakeUp status message
        /// </summary>
        mstBusWakeUp = 2,
        /// <summary>
        /// Auto-baudrate Timeout status message
        /// </summary>
        mstAutobaudrateTimeOut = 3,
        /// <summary>
        /// Auto-baudrate Reply status message
        /// </summary>
        mstAutobaudrateReply = 4,
        /// <summary>
        /// Bus Overrun status message
        /// </summary>
        mstOverrun = 5,
        /// <summary>
        /// Queue Overrun status message
        /// </summary>
        mstQueueOverrun = 6,
    }

    /// <summary>
    /// Schedule Slot Types
    /// </summary>
    public enum TLINSlotType : byte
    {
        /// <summary>
        /// Unconditional frame
        /// </summary>
        sltUnconditional = 0,
        /// <summary>
        /// Event frame
        /// </summary>
        sltEvent = 1,
        /// <summary>
        /// Sporadic frame
        /// </summary>
        sltSporadic = 2,
        /// <summary>
        /// Diagnostic Master Request frame
        /// </summary>
        sltMasterRequest = 3,
        /// <summary>
        /// Diagnostic Slave Response frame
        /// </summary>
        sltSlaveResponse = 4,
    }

    /// <summary>
    /// LIN Message Direction Types
    /// </summary>
    public enum TLINDirection : byte
    {
        /// <summary>
        /// Direction disabled
        /// </summary>
        dirDisabled = 0,
        /// <summary>
        /// Direction is Publisher
        /// </summary>
        dirPublisher = 1,
        /// <summary>
        /// Direction is Subscriber
        /// </summary>
        dirSubscriber = 2,
        /// <summary>
        /// Direction is Subscriber (detect Length)
        /// </summary>
        dirSubscriberAutoLength = 3,
    }

    /// <summary>
    /// LIN Message checksum Types
    /// </summary>
    public enum TLINChecksumType : byte
    {
        /// <summary>
        /// Custom checksum
        /// </summary>
        cstCustom = 0,
        /// <summary>
        /// Classic checksum (ver 1.x)
        /// </summary>
        cstClassic = 1,
        /// <summary>
        /// Enhanced checksum
        /// </summary>
        cstEnhanced = 2,
        /// <summary>
        /// Detect checksum
        /// </summary>
        cstAuto = 3,
    }

    /// <summary>
    /// LIN Hardware operation mode
    /// </summary>
    public enum TLINHardwareMode : byte
    {
        /// <summary>
        /// Hardware is not initialized
        /// </summary>
        modNone = 0,
        /// <summary>
        /// Hardware working as Slave
        /// </summary>
        modSlave = 1,
        /// <summary>
        /// Hardware working as Master
        /// </summary>
        modMaster = 2,
    }

    /// <summary>
    /// LIN Hardware status
    /// </summary>
    public enum TLINHardwareState : byte
    {
        /// <summary>
        /// Hardware is not initialized
        /// </summary>
        hwsNotInitialized = 0,
        /// <summary>
        /// Hardware is detecting the baudrate
        /// </summary>
        hwsAutobaudrate = 1,
        /// <summary>
        /// Hardware (bus) is active
        /// </summary>
        hwsActive = 2,
        /// <summary>
        /// Hardware (bus) is in sleep mode
        /// </summary>
        hwsSleep = 3,
        /// Hardware (bus-line) shorted to ground
        /// </summary>
        hwsShortGround = 6,
    }

    /// <summary>
    /// LIN Error Codes
    /// </summary>
    public enum TLINError : int
    {
        /// <summary>
        /// No error. Success.
        /// </summary>
        errOK = 0,
        /// <summary>
        /// Transmit Queue is full.
        /// </summary>
        errXmtQueueFull = 1,
        /// <summary>
        /// Period of time is invalid. 
        /// </summary>
        errIllegalPeriod = 2,
        /// <summary>
        /// Client Receive Queue is empty.
        /// </summary>
        errRcvQueueEmpty = 3,
        /// <summary>
        /// Checksum type is invalid.
        /// </summary>
        errIllegalChecksumType = 4,
        /// <summary>
        /// Hardware handle is invalid.
        /// </summary>
        errIllegalHardware = 5,
        /// <summary>
        /// Client handle is invalid.
        /// </summary>
        errIllegalClient = 6,
        /// <summary>
        /// Parameter type is invalid.
        /// </summary>
        errWrongParameterType = 7,
        /// <summary>
        /// Parameter value is invalid.
        /// </summary>
        errWrongParameterValue = 8,
        /// <summary>
        /// Direction is invalid.
        /// </summary>
        errIllegalDirection = 9,
        /// <summary>
        /// Length is outside of the valid range.
        /// </summary>
        errIllegalLength = 10,
        /// <summary>
        /// Baudrate is outside of the valid range.
        /// </summary>
        errIllegalBaudrate = 11,
        /// <summary>
        /// ID is outside of the valid range. 
        /// </summary>
        errIllegalFrameID = 12,
        /// <summary>
        /// Buffer parameter is too small.
        /// </summary>
        errBufferInsufficient = 13,
        /// <summary>
        /// Scheduler Number is outside of the valid range.
        /// </summary>
        errIllegalScheduleNo = 14,
        /// <summary>
        /// Slots count is bigger than the actual number of available slots.
        /// </summary>
        errIllegalSlotCount = 15,
        /// <summary>
        /// Array index is out of the valid range.
        /// </summary>
        errIllegalIndex = 16,
        /// <summary>
        /// Range of bytes to be updated is invalid.
        /// </summary>
        errIllegalRange = 17,
        /// <summary>
        /// LIN Manager does not have enough resources for the current task.
        /// </summary>
        errOutOfResource = 1001,
        /// <summary>
        /// The LIN Device Manager is not running.
        /// </summary>
        errManagerNotLoaded = 1002,
        /// <summary>
        /// The communication to the LIN Device Manager was interrupted.
        /// </summary>
        errManagerNotResponding = 1003,
        /// <summary>
        /// A "MemoryAccessViolation" exception occurred within an API method.
        /// </summary>
        errMemoryAccess = 1004,
        /// <summary>
        /// An API method is not implemented.
        /// </summary>
        errNotImplemented = 0xFFFE,
        /// <summary>
        /// An internal error occurred within the LIN Device Manager.
        /// </summary>
        errUnknown = 0xFFFF,
    }
    #endregion

    #region Structures
    /// <summary>
    /// Version Information structure
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TLINVersion
    {
        /// <summary>
        /// Major part of a version number
        /// </summary>
        public short Major;
        /// <summary>
        /// Minor part of a version number
        /// </summary>
        public short Minor;
        /// <summary>
        /// Revision part of a version number
        /// </summary>
        public short Revision;
        /// <summary>
        /// Build part of a version number
        /// </summary>
        public short Build;
    }

    /// <summary>
    /// A LIN Message to be sent
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TLINMsg
    {
        /// <summary>
        /// Frame ID (6 bit) + Parity (2 bit)
        /// </summary>
        public byte FrameId;
        /// <summary>
        /// Frame Length (1..8)
        /// </summary>
        public byte Length;
        /// <summary>
        /// Frame Direction (see TLINDirection)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINDirection Direction;
        /// <summary>
        /// Frame Checksum type (see TLINChecksumType)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINChecksumType ChecksumType;
        /// <summary>
        /// Data bytes (0..7)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public byte[] Data;
        /// <summary>
        /// Frame Checksum
        /// </summary>
        public byte Checksum;
    }

    /// <summary>
    ///  A received LIN Message
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TLINRcvMsg
    {
        /// <summary>
        /// Frame type (see TLINMsgType)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINMsgType Type;
        /// <summary>
        /// Frame ID (6 bit) + Parity (2 bit)
        /// </summary>
        public byte FrameId;
        /// <summary>
        /// Frame Length (1..8)
        /// </summary>
        public byte Length;
        /// <summary>
        /// Frame Direction (see TLINDirection)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINDirection Direction;
        /// <summary>
        /// Frame Checksum type (see TLINChecksumType)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINChecksumType ChecksumType;
        /// <summary>
        /// Data bytes (0..7)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public byte[] Data;
        /// <summary>
        /// Frame Checksum
        /// </summary>
        public byte Checksum;
        /// <summary>
        /// Frame error flags (see TLINMsgErrors)
        /// </summary>
        [MarshalAs(UnmanagedType.I4)]
        public TLINMsgErrors ErrorFlags;
        /// <summary>
        /// Timestamp in microseconds
        /// </summary>
        public UInt64 TimeStamp;
        /// <summary>
        /// Handle of the Hardware which received the message
        /// </summary>
        public HLINHW hHw;
    }

    /// <summary>
    /// A LIN Frame Entry 
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TLINFrameEntry
    {
        /// <summary>
        /// Frame ID (without parity)
        /// </summary>
        public byte FrameId;
        /// <summary>
        /// Frame Length (1..8)
        /// </summary>
        public byte Length;
        /// <summary>
        /// Frame Direction (see TLINDirection)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINDirection Direction;
        /// <summary>
        /// Frame Checksum type (see TLINChecksumType)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINChecksumType ChecksumType;
        /// <summary>
        /// Frame flags (see FRAME_FLAG_...)
        /// </summary>
        public ushort Flags;
        /// <summary>
        /// Data bytes (0..7)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public byte[] InitialData;
    }

    /// <summary>
    /// A LIN Schedule slot
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TLINScheduleSlot
    {
        /// <summary>
        /// Slot Type (see TLINSlotType)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINSlotType Type;
        /// <summary>
        /// Slot Delay in Milliseconds
        /// </summary>
        public ushort Delay;
        /// <summary>
        /// Frame IDs (without parity)
        /// </summary>
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public byte[] FrameId;
        /// <summary>
        /// ID count for sporadic frames
        /// Resolve schedule number for Event frames
        /// </summary>
        public byte CountResolve;
        /// <summary>
        /// Slot handle (read-only)
        /// </summary>
        public uint Handle;
    }

    // LIN Status data
    //
    [StructLayout(LayoutKind.Sequential)]
    public struct TLINHardwareStatus
    {
        /// <summary>
        /// Node state (see TLINHardwareMode)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINHardwareMode Mode;
        /// <summary>
        /// Bus state (see TLINHardwareState)
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public TLINHardwareState Status;
        /// <summary>
        /// Count of free places in the Transmit Queue
        /// </summary>
        public byte FreeOnSendQueue;
        /// <summary>
        /// Free slots in the Schedule pool (see LIN_MAX_SCHEDULE_SLOTS)
        /// </summary>
        public ushort FreeOnSchedulePool;
        /// <summary>
        /// USB receive buffer overrun counter
        /// </summary>
        public ushort ReceiveBufferOverrun;
    }
    #endregion

    public class PLinApi
    {
        #region Constants
        /// <summary>
        /// Invalid value for all LIN handles (Client, Hardware)
        /// </summary>
        public const byte INVALID_LIN_HANDLE = 0;

        /// <summary>
        /// Hardware Type: LIN USB
        /// </summary>
        public const byte LIN_HW_TYPE_USB = 1;

        /// <summary>
        /// Maximum allowed Frame ID
        /// </summary>
        public const byte LIN_MAX_FRAME_ID = 63;

        /// <summary>
        /// Maximum allowed Schedules per Hardware
        /// </summary>
        public const int LIN_MAX_SCHEDULES = 8;

        /// <summary>
        /// Minimum Schedule number
        /// </summary>
        public const int LIN_MIN_SCHEDULE_NUMBER = 0;

        /// <summary>
        /// Maximum Schedule number
        /// </summary>
        public const int LIN_MAX_SCHEDULE_NUMBER = 7;

        /// <summary>
        /// Maximum allowed Schedule slots per Hardware
        /// </summary>
        public const int LIN_MAX_SCHEDULE_SLOTS = 256;

        /// <summary>
        /// Minimum LIN Baudrate
        /// </summary>
        public const ushort LIN_MIN_BAUDRATE = 1000;

        /// <summary>
        /// Maximum LIN Baudrate
        /// </summary>
        public const ushort LIN_MAX_BAUDRATE = 20000;

        /// <summary>
        /// Maximum number of bytes for Name / ID of a Hardware or Client.
        /// </summary>
        public const ushort LIN_MAX_NAME_LENGTH = 48;

        /// <summary>
        /// Slave Enable Publisher Response
        /// </summary>
        public const int FRAME_FLAG_RESPONSE_ENABLE = 1;

        /// <summary>
        /// Slave Publisher Single shot
        /// </summary>
        public const int FRAME_FLAG_SINGLE_SHOT = 2;
        
        /// <summary>
        /// Ignore InitialData on set frame entry
        /// </summary>
        public const int FRAME_FLAG_IGNORE_INIT_DATA = 4;

        /// <summary>
        /// Maximum number of bytes that a user can read/write on a Hardware.
        /// </summary>
        public const int LIN_MAX_USER_DATA = 24;
        #endregion

        #region Function prototypes
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
        /// <param name="hClient">Client handle buffer</param>
        /// <returns>A LIN Error Code</returns>
        [DllImport("plinapi.dll", EntryPoint = "LIN_RegisterClient")]
        public static extern TLINError RegisterClient(
            string strName,
            IntPtr hWnd,
            out HLINCLIENT hClient);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_RemoveClient")]
        public static extern TLINError RemoveClient(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ConnectClient")]
        public static extern TLINError ConnectClient(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_DisconnectClient")]
        public static extern TLINError DisconnectClient(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ResetClient")]
        public static extern TLINError ResetClient(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetClientParam")]
        public static extern TLINError SetClientParam(
            HLINCLIENT hClient,
            TLINClientParam wParam,
            int dwValue);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetClientParam")]
        public static extern TLINError GetClientParam(
            HLINCLIENT hClient,
            TLINClientParam wParam,
            out int pBuff,
            ushort wBuffSize);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetClientParam")]
        public static extern TLINError GetClientParam(
            HLINCLIENT hClient,
            TLINClientParam wParam,
            [MarshalAs(UnmanagedType.LPStr, SizeParamIndex = 3)]
            StringBuilder pBuff,
            ushort wBuffSize);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetClientParam")]
        public static extern TLINError GetClientParam(
            HLINCLIENT hClient,
            TLINClientParam wParam,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)]
            HLINHW[] pBuff,
            ushort wBuffSize);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetClientFilter")]
        public static extern TLINError SetClientFilter(
            HLINCLIENT hClient,
            HLINHW hHw,
            [MarshalAs(UnmanagedType.I8)]
            UInt64 iRcvMask);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetClientFilter")]
        public static extern TLINError GetClientFilter(
            HLINCLIENT hClient,
            HLINHW hHw,
            [MarshalAs(UnmanagedType.I8)]
            out UInt64 pRcvMask);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_Read")]
        public static extern TLINError Read(
            HLINCLIENT hClient,
            out TLINRcvMsg pMsg);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ReadMulti")]
        public static extern TLINError ReadMulti(
            HLINCLIENT hClient,
            [In, Out] 
            TLINRcvMsg[] pMsgBuff,
            int iMaxCount,
            out int pCount);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_Write")]
        public static extern TLINError Write(
            HLINCLIENT hClient,
            HLINHW hHw,
            ref TLINMsg pMsg);

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
        /// <param name="byMode">Hardware mode (see TLINHardwareMode)</param>
        /// <param name="wBaudrate">Baudrate (see LIN_MIN_BAUDRATE and LIN_MAX_BAUDRATE)</param>
        /// <returns>A LIN Error Code</returns>
        [DllImport("plinapi.dll", EntryPoint = "LIN_InitializeHardware")]
        public static extern TLINError InitializeHardware(
            HLINCLIENT hClient,
            HLINHW hHw,
            [MarshalAs(UnmanagedType.U1)]
            TLINHardwareMode byMode,
            ushort wBaudrate);

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
        /// <param name="wBuffSize">Size of the buffer in bytes (pBuff.Length * 2)</param>
        /// <param name="pCount">Number of Hardware available</param>
        /// <returns>A LIN Error Code</returns>
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetAvailableHardware")]
        public static extern TLINError GetAvailableHardware(
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)]
            HLINHW[] pBuff,
            ushort wBuffSize,
            out ushort pCount);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetHardwareParam")]
        public static extern TLINError SetHardwareParam(
            HLINCLIENT hClient,
            HLINHW hHw,
            TLINHardwareParam wParam,
            ref UInt64 pBuff,
            ushort wBuffSize);

        [DllImport("plinapi.dll", EntryPoint = "LIN_SetHardwareParam")]
        public static extern TLINError SetHardwareParam(
            HLINCLIENT hClient,
            HLINHW hHw,
            TLINHardwareParam wParam,
            ref HLINCLIENT pBuff,
            ushort wBuffSize);

        [DllImport("plinapi.dll", EntryPoint = "LIN_SetHardwareParam")]
        public static extern TLINError SetHardwareParam(
            HLINCLIENT hClient,
            HLINHW hHw,
            TLINHardwareParam wParam,
            ref int pBuff,
            ushort wBuffSize);

        [DllImport("plinapi.dll", EntryPoint = "LIN_SetHardwareParam")]
        public static extern TLINError SetHardwareParam(
            HLINCLIENT hClient,
            HLINHW hHw,
            TLINHardwareParam wParam,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)]
            byte[] pBuff,
            ushort wBuffSize);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetHardwareParam")]
        public static extern TLINError GetHardwareParam(
            HLINHW hHw,
            TLINHardwareParam wParam,
            out int pBuff,
            ushort wBuffSize);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetHardwareParam")]
        public static extern TLINError GetHardwareParam(
            HLINHW hHw,
            TLINHardwareParam wParam,
            [MarshalAs(UnmanagedType.LPStr, SizeParamIndex = 3)]
            StringBuilder pBuff,
            ushort wBuffLen);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetHardwareParam")]
        public static extern TLINError GetHardwareParam(
            HLINHW hHw,
            TLINHardwareParam wParam,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 3)]
            byte[] pBuff,
            ushort wBuffLen);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetHardwareParam")]
        public static extern TLINError GetHardwareParam(
            HLINHW hHw,
            TLINHardwareParam wParam,
            out TLINVersion pBuff,
            ushort wBuffLen);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetHardwareParam")]
        public static extern TLINError GetHardwareParam(
            HLINHW hHw,
            TLINHardwareParam wParam,            
            out UInt64 pBuff,
            ushort wBuffLen);

        [DllImport("plinapi.dll", EntryPoint = "LIN_GetHardwareParam")]
        public static extern TLINError GetHardwareParam(
            HLINHW hHw,
            TLINHardwareParam wParam,
            out HLINCLIENT pBuff,
            ushort wBuffLen);


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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ResetHardware")]
        public static extern TLINError ResetHardware(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ResetHardwareConfig")]
        public static extern TLINError ResetHardwareConfig(
            HLINCLIENT hClient,
            HLINHW hHw);

        /// <summary>
        /// Phisically identifies a LIN Hardware (a channel on a LIN Device) by 
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_IdentifyHardware")]
        public static extern TLINError IdentifyHardware(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_RegisterFrameId")]
        public static extern TLINError RegisterFrameId(
            HLINCLIENT hClient,
            HLINHW hHw,
            byte bFromFrameId,
            byte bToFrameId);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetFrameEntry")]
        public static extern TLINError SetFrameEntry(
            HLINCLIENT hClient,
            HLINHW hHw,
            ref TLINFrameEntry pFrameEntry);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetFrameEntry")]
        public static extern TLINError GetFrameEntry(
            HLINHW hHw,
            ref TLINFrameEntry pFrameEntry);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_UpdateByteArray")]
        public static extern TLINError UpdateByteArray(
            HLINCLIENT hClient,
            HLINHW hHw,
            byte bFrameId,
            byte bIndex,
            byte bLen,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)]
            byte[] pData);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_StartKeepAlive")]
        public static extern TLINError StartKeepAlive(
            HLINCLIENT hClient,
            HLINHW hHw,
            byte bFrameId,
            ushort wPeriod);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SuspendKeepAlive")]
        public static extern TLINError SuspendKeepAlive(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ResumeKeepAlive")]
        public static extern TLINError ResumeKeepAlive(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetSchedule")]
        public static extern TLINError SetSchedule(
            HLINCLIENT hClient,
            HLINHW hHw,
            int iScheduleNumber,
            [MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 4)]
            [In, Out]TLINScheduleSlot[] pSchedule,
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetSchedule")]
        public static extern TLINError GetSchedule(
            HLINHW hHw,
            int iScheduleNumber,
            [In, Out] 
            TLINScheduleSlot[] pScheduleBuff,
            int iMaxSlotCount,
            out int pSlotCount);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_DeleteSchedule")]
        public static extern TLINError DeleteSchedule(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetScheduleBreakPoint")]
        public static extern TLINError SetScheduleBreakPoint(
            HLINCLIENT hClient,
            HLINHW hHw,
            int iBreakPointNumber,
            uint dwHandle);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_StartSchedule")]
        public static extern TLINError StartSchedule(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SuspendSchedule")]
        public static extern TLINError SuspendSchedule(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_ResumeSchedule")]
        public static extern TLINError ResumeSchedule(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_XmtWakeUp")]
        public static extern TLINError XmtWakeUp(
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
        [DllImport("plinapi.dll", EntryPoint = "LIN_StartAutoBaud")]
        public static extern TLINError StartAutoBaud(
            HLINCLIENT hClient,
            HLINHW hHw,
            ushort wTimeOut);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetStatus")]
        public static extern TLINError GetStatus(
            HLINHW hHw,
            out TLINHardwareStatus pStatusBuff);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_CalculateChecksum")]
        public static extern TLINError CalculateChecksum(
            ref TLINMsg pMsg);

        /// <summary>
        ///  Returns a TLINVersion sttructure containing the PLIN-API DLL version.
        ///  
        /// Possible DLL interaction errors:
        ///   errManagerNotLoaded, errManagerNotResponse, errMemoryAccess
        /// 
        /// Possible API errors:
        ///   errWrongParameterValue
        /// </summary>
        /// <param name="pVerBuffer">Version buffer</param>
        /// <returns>A LIN Error Code</returns>
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetVersion")]
        public static extern TLINError GetVersion(
            ref TLINVersion pVerBuffer);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetVersionInfo")]
        public static extern TLINError GetVersionInfo(
            [MarshalAs(UnmanagedType.LPStr, SizeParamIndex = 1)]
            StringBuilder pTextBuff,
            int wBuffSize);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetErrorText")]
        public static extern TLINError GetErrorText(
            TLINError dwError,
            byte bLanguage,
            [MarshalAs(UnmanagedType.LPStr, SizeParamIndex = 3)]
            StringBuilder strTextBuff,
            int wBuffSize);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetPID")]
        public static extern TLINError GetPID(
            ref byte pFrameId);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetTargetTime")]
        public static extern TLINError GetTargetTime(
            HLINHW hHw,
            out UInt64 pTargetTime);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_SetResponseRemap")]
        public static extern TLINError SetResponseRemap (
	        HLINCLIENT  hClient,                
	        HLINHW hHw,
            [MarshalAs(UnmanagedType.LPArray, SizeConst = 64)]      
	        byte[] pRemapTab);

        /// <summary>
        /// Gets the Response Remap of a LIN Slave
        /// </summary>
        /// <param name="hHw">Hardware Handle</param>
        /// <param name="pRemapTab">Remap Response buffer</param>
        /// <returns>A LIN Error Code</returns>
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetResponseRemap")]
        public static extern TLINError GetResponseRemap (
	        HLINHW hHw,
            [MarshalAs(UnmanagedType.LPArray, SizeConst = 64)]      
	        byte[] pRemapTab);

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
        [DllImport("plinapi.dll", EntryPoint = "LIN_GetSystemTime")]
        public static extern TLINError GetSystemTime(
            out UInt64 pTargetTime);
        #endregion
    }
}