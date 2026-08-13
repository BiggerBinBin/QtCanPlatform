#  PLinApi.py
#
#  ~~~~~~~~~~~~
#
#  Definition of the PLIN-API.
#
# 
#    Principle:
#    ~~~~~~~~~~
#    The driver supports multiple clients (= Windows or DOS programs
#    that communicate with LIN-busses), and multiple LIN-Devices.
#    A LIN-Device represent two LIN-Hardware (each channel is a Hardware)
#    Multiple 'Clients' can be connected to one or more LIN-Hardware, which 
#    itself have an interface to a physical LIN-channel of a device.
#
#    Features:
#    ~~~~~~~~~
#     - 1 Client can be connected to multiple Hardware
#     - 1 Hardware supports multiple clients
#     - When a Client sends a message to a Hardware, the message will not be routed
#       to other clients. The response of the Hardware is routed to the connected
#       clients depending on the registered Hardware message filter.
#     - each Client only receives the messages that pass its acceptance filter
#     - each Client has a Receive Queue to buffer received messages
#     - hClient: 'Client handle'. This number is used by the driver to
#                identify and manage a Client
#     - hHw: 'Hardware handle'. This number is used by the driver to
#                identify and manage a Hardware
#     - all handles are 1-based. 0 = illegal handle
#
#     All functions return a value of type TLINError
#
#  ~~~~~~~~~~~~
#
#  ------------------------------------------------------------------
#  Author : Fabrice Vergnaud
#  Last change: 17.07.2013 Vergnaud
#
#  Language: Python 2.7 / 3.3
#  ------------------------------------------------------------------
#
#  Copyright (C) 1999-2013  PEAK-System Technik GmbH, Darmstadt
#  more Info at http://www.peak-system.com 
#

# Module Imports
#

from ctypes import *

#///////////////////////////////////////////////////////////
# Type definitions
#///////////////////////////////////////////////////////////

HLINCLIENT          = c_ubyte       # LIN client handle
HLINHW              = c_ushort      # LIN hardware handle
TLINMsgErrors       = c_int         # Error flags for LIN Rcv Msgs
TLINClientParam     = c_ushort      # Client Parameters (GetClientParam Function)
TLINHardwareParam   = c_ushort      # Hardware Parameters (GetHardwareParam function)
TLINMsgType         = c_ubyte       # Received Message Types
TLINSlotType        = c_ubyte       # Schedule Slot Types
TLINDirection       = c_ubyte       # LIN Message Direction Types
TLINChecksumType    = c_ubyte       # LIN Message checksum Types
TLINHardwareMode    = c_ubyte       # LIN Hardware operation mode
TLINHardwareState   = c_ubyte       # LIN Hardware status
TLINError           = int           # LIN Error Codes

#///////////////////////////////////////////////////////////
# Value definitions
#///////////////////////////////////////////////////////////


# Invalid Handle values
#
INVALID_LIN_HANDLE              = c_ubyte(0)        # Invalid value for all LIN handles (Client, Hardware)

# Hardware Types
#
LIN_HW_TYPE_USB                 = c_ubyte(1)        # LIN USB type

# Minimum and Maximum values
#
LIN_MAX_FRAME_ID                = c_ubyte(63)       # Maximum allowed Frame ID (0x3F)
LIN_MAX_SCHEDULES               = c_int(8)          # Maximum allowed Schedules per Hardware
LIN_MIN_SCHEDULE_NUMBER         = c_int(0)          # Minimum Schedule number
LIN_MAX_SCHEDULE_NUMBER         = c_int(7)          # Maximum Schedule number
LIN_MAX_SCHEDULE_SLOTS          = c_int(256)        # Maximum allowed Schedule slots per Hardware
LIN_MIN_BAUDRATE                = c_ushort(1000)    # Minimum LIN Baudrate
LIN_MAX_BAUDRATE                = c_ushort(20000)   # Maximum LIN Baudrate
LIN_MAX_NAME_LENGTH             = c_ushort(48)      # Maximum number of bytes for Name / ID of a Hardware or Client
LIN_MAX_USER_DATA               = c_int(24)         # Maximum number of bytes that a user can read/write on a Hardware

# Frame flags for LIN Frame Entries
#
FRAME_FLAG_RESPONSE_ENABLE      = c_ushort(0x1)        # Slave Enable Publisher Response
FRAME_FLAG_SINGLE_SHOT          = c_ushort(0x2)        # Slave Publisher Single shot
FRAME_FLAG_IGNORE_INIT_DATA     = c_ushort(0x4)        # Ignore InitialData on set frame entry

# Error flags for LIN Rcv Msgs
#
TLIN_MSGERROR_OK                            = TLINError(0x000)  # No error
TLIN_MSGERROR_INCONSISTENT_SYNCH            = TLINError(0x001)  # Error on Synchronization field
TLIN_MSGERROR_ID_PARITY_BIT_0               = TLINError(0x002)  # Wrong parity Bit 0
TLIN_MSGERROR_ID_PARITY_BIT_1               = TLINError(0x004)  # Wrong parity Bit 1
TLIN_MSGERROR_SLAVE_NOT_RESPONDING          = TLINError(0x008)  # Slave not responding error
TLIN_MSGERROR_TIMEOUT                       = TLINError(0x010)  # A timeout was reached
TLIN_MSGERROR_CHECKSUM                      = TLINError(0x020)  # Wrong checksum
TLIN_MSGERROR_GROUND_SHORT                  = TLINError(0x040)  # Bus shorted to ground
TLIN_MSGERROR_VBAT_SHORT                    = TLINError(0x080)  # Bus shorted to VBat
TLIN_MSGERROR_SLOT_DELAY                    = TLINError(0x100)  # A slot time (delay) was too small
TLIN_MSGERROR_OTHER_RESPONSE                = TLINError(0x200)  # Response was received from other station

# Client parameters
#
TLIN_CLIENTPARAM_NAME                       = TLINClientParam(1)    # Client Name
TLIN_CLIENTPARAM_MESSAGE_ON_QUEUE           = TLINClientParam(2)    # Unread messages in the Receive Queue
TLIN_CLIENTPARAM_WINDOW_HANDLE              = TLINClientParam(3)    # Registered windows handle (information purpose)
TLIN_CLIENTPARAM_CONNECTED_HARDWARE         = TLINClientParam(4)    # Handles of the connected Hardware
TLIN_CLIENTPARAM_TRANSMITTED_MESSAGES       = TLINClientParam(5)    # Number of transmitted messages
TLIN_CLIENTPARAM_RECEIVED_MESSAGES          = TLINClientParam(6)    # Number of received messages
TLIN_CLIENTPARAM_RECEIVE_STATUS_FRAME       = TLINClientParam(7)    # Status of the property "Status Frames"
TLIN_CLIENTPARAM_ON_RECEIVE_EVENT_HANDLE    = TLINClientParam(8)    # Handle of the Receive event
TLIN_CLIENTPARAM_ON_PLUGIN_EVENT_HANDLE     = TLINClientParam(9)    # Handle of the Hardware plug-in event

# Hardware Parameters (GetHardwareParam function)
#
TLIN_HARDWAREPARAM_NAME                 = TLINHardwareParam(1)  # Hardware / Device Name
TLIN_HARDWAREPARAM_DEVICE_NUMBER        = TLINHardwareParam(2)  # Index of the owner Device
TLIN_HARDWAREPARAM_CHANNEL_NUMBER       = TLINHardwareParam(3)  # Channel Index on the owner device (0 or 1)
TLIN_HARDWAREPARAM_CONNECTED_CLIENTS    = TLINHardwareParam(4)  # Handles of the connected clients
TLIN_HARDWAREPARAM_MESSAGE_FILTER       = TLINHardwareParam(5)  # Message filter
TLIN_HARDWAREPARAM_BAUDRATE             = TLINHardwareParam(6)  # Baudrate
TLIN_HARDWAREPARAM_MODE                 = TLINHardwareParam(7)  # Master status
TLIN_HARDWAREPARAM_FIRMWARE_VERSION     = TLINHardwareParam(8)  # Lin Firmware Version (Text with the form xx.yy where: xx = major version. yy = minor version)
TLIN_HARDWAREPARAM_BUFFER_OVERRUN_COUNT = TLINHardwareParam(9)  # Receive Buffer Overrun Counter
TLIN_HARDWAREPARAM_BOSS_CLIENT          = TLINHardwareParam(10) # Registered master Client
TLIN_HARDWAREPARAM_SERIAL_NUMBER        = TLINHardwareParam(11) # Serial number of a Hardware
TLIN_HARDWAREPARAM_VERSION              = TLINHardwareParam(12) # Version of a Hardware
TLIN_HARDWAREPARAM_TYPE                 = TLINHardwareParam(13) # Type of a Hardware
TLIN_HARDWAREPARAM_OVERRUN_COUNT        = TLINHardwareParam(14) # Receive Queue Buffer Overrun Counter
TLIN_HARDWAREPARAM_ID_NUMBER            = TLINHardwareParam(15) # Hardware identificaiton number
TLIN_HARDWAREPARAM_USER_DATA            = TLINHardwareParam(16) # User data on a hardware

# Received Message Types
#
TLIN_MSGTYPE_STANDARD                   = TLINMsgType(0)    # Standard LIN Message
TLIN_MSGTYPE_BUS_SLEEP                  = TLINMsgType(1)    # Bus Sleep status message
TLIN_MSGTYPE_BUS_WAKEUP                 = TLINMsgType(2)    # Bus WakeUp status message
TLIN_MSGTYPE_AUTOBAUDRATE_TIMEOUT       = TLINMsgType(3)    # Auto-baudrate Timeout status message
TLIN_MSGTYPE_AUTOBAUDRATE_REPLY         = TLINMsgType(4)    # Auto-baudrate Reply status message
TLIN_MSGTYPE_OVERRUN                    = TLINMsgType(5)    # Bus Overrun status message
TLIN_MSGTYPE_QUEUE_OVERRUN              = TLINMsgType(6)    # Queue Overrun status message

# Schedule Slot Types
# 
TLIN_SLOTTYPE_UNCONDITIONAL             = TLINSlotType(0)   # Unconditional frame
TLIN_SLOTTYPE_EVENT                     = TLINSlotType(1)   # Event frame
TLIN_SLOTTYPE_SPORADIC                  = TLINSlotType(2)   # Sporadic frame
TLIN_SLOTTYPE_MASTER_REQUEST            = TLINSlotType(3)   # Diagnostic Master Request frame
TLIN_SLOTTYPE_SLAVE_RESPONSE            = TLINSlotType(4)   # Diagnostic Slave Response frame

# LIN Message Direction Types
#
TLIN_DIRECTION_DISABLED                 = TLINDirection(0)  # Direction disabled
TLIN_DIRECTION_PUBLISHER                = TLINDirection(1)  # Direction is Publisher
TLIN_DIRECTION_SUBSCRIBER               = TLINDirection(2)  # Direction is Subscriber
TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH    = TLINDirection(3)  # Direction is Subscriber (detect Length)

# LIN Message checksum Types
#
TLIN_CHECKSUMTYPE_CUSTOM                = TLINChecksumType(0)   # Custom checksum
TLIN_CHECKSUMTYPE_CLASSIC               = TLINChecksumType(1)   # Classic checksum (ver 1.x)
TLIN_CHECKSUMTYPE_ENHANCED              = TLINChecksumType(2)   # Enhanced checksum
TLIN_CHECKSUMTYPE_AUTO                  = TLINChecksumType(3)   # Detect checksum

# LIN Hardware operation mode
#
TLIN_HARDWAREMODE_NONE                  = TLINHardwareMode(0)   # Hardware is not initialized
TLIN_HARDWAREMODE_SLAVE                 = TLINHardwareMode(1)   # Hardware working as slave
TLIN_HARDWAREMODE_MASTER                = TLINHardwareMode(2)   # Hardware working as master

# LIN Hardware status
#
TLIN_HARDWARESTATE_NOT_INITIALIZED      = TLINHardwareState(0)  # Hardware is not initialized
TLIN_HARDWARESTATE_AUTOBAUDRATE         = TLINHardwareState(1)  # Hardware is detecting the baudrate
TLIN_HARDWARESTATE_ACTIVE               = TLINHardwareState(2)  # Hardware (bus) is active
TLIN_HARDWARESTATE_SLEEP                = TLINHardwareState(3)  # Hardware (bus) is in sleep mode
TLIN_HARDWARESTATE_SHORT_GROUND         = TLINHardwareState(6)  # Hardware (bus-line) shorted to ground

# LIN Error Codes
#
TLIN_ERROR_OK                           = TLINError(0)      # No error. Success.
TLIN_ERROR_XMTQUEUE_FULL                = TLINError(1)      # Transmit Queue is full.
TLIN_ERROR_ILLEGAL_PERIOD               = TLINError(2)      # Period of time is invalid. 
TLIN_ERROR_RCVQUEUE_EMPTY               = TLINError(3)      # Client Receive Queue is empty.
TLIN_ERROR_ILLEGAL_CHECKSUMTYPE         = TLINError(4)      # Checksum type is invalid.
TLIN_ERROR_ILLEGAL_HARDWARE             = TLINError(5)      # Hardware handle is invalid.
TLIN_ERROR_ILLEGAL_CLIENT               = TLINError(6)      # Client handle is invalid.
TLIN_ERROR_WRONG_PARAM_TYPE             = TLINError(7)      # Parameter type is invalid.
TLIN_ERROR_WRONG_PARAM_VALUE            = TLINError(8)      # Parameter value is invalid.
TLIN_ERROR_ILLEGAL_DIRECTION            = TLINError(9)      # Direction is invalid.
TLIN_ERROR_ILLEGAL_LENGTH               = TLINError(10)     # Length is outside of the valid range.
TLIN_ERROR_ILLEGAL_BAUDRATE             = TLINError(11)     # Baudrate is outside of the valid range.
TLIN_ERROR_ILLEGAL_FRAMEID              = TLINError(12)     # ID is outside of the valid range. 
TLIN_ERROR_BUFFER_INSUFFICIENT          = TLINError(13)     # Buffer parameter is too small.
TLIN_ERROR_ILLEGAL_SCHEDULENUMBER       = TLINError(14)     # Scheduler Number is outside of the valid range.
TLIN_ERROR_ILLEGAL_SLOTCOUNT            = TLINError(15)     # Slots count is bigger than the actual number of available slots.
TLIN_ERROR_ILLEGAL_INDEX                = TLINError(16)     # Array index is out of the valid range.
TLIN_ERROR_ILLEGAL_RANGE                = TLINError(17)     # Range of bytes to be updated is invalid.
TLIN_ERROR_OUT_OF_RESPONSE              = TLINError(1001)   # LIN Manager does not have enough resources for the current task.
TLIN_ERROR_MANAGER_NOT_LOADED           = TLINError(1002)   # The LIN Device Manager is not running.
TLIN_ERROR_MANAGER_NOT_RESPONDING       = TLINError(1003)   # The communication to the LIN Device Manager was interrupted.
TLIN_ERROR_MEMORY_ACCESS                = TLINError(1004)   # A "MemoryAccessViolation" exception occurred within an API method.
TLIN_ERROR_NOT_IMPLEMENTED              = TLINError(0xFFFE) # An API method is not implemented.
TLIN_ERROR_UNKNOWN                      = TLINError(0xFFFF) # An internal error occurred within the LIN Device Manager.

# Represents a Version Information
#
class TLINVersion (Structure):
    """
    Represents a version information
    """
    _fields_ = [ ("Major",      c_short),   # Major part of a version number
                 ("Minor",      c_short),   # Minor part of a version number
                 ("Revision",   c_short),   # Revision part of a version number
                 ("Build",      c_short) ]  # Revision part of a version number
    
# Represents a LIN Message to be sent
#
class TLINMsg (Structure):
    """
    Represents a LIN Message to be sent
    """
    _fields_ = [ ("FrameId",        c_ubyte),           # Frame ID (6 bit) + Parity (2 bit)
                 ("Length",         c_ubyte),           # Frame Length (1..8)
                 ("Direction",      TLINDirection),     # Frame Direction (see LIN Message Direction Types)
                 ("ChecksumType",   TLINChecksumType),  # Frame Checksum type (see LIN Message Checksum Types)
                 ("Data",           c_ubyte * 8),       # Data bytes (0..7)
                 ("Checksum",       c_ubyte) ]          # Frame Checksum

# Represents a received LIN Message
#
class  TLINRcvMsg (Structure):
    """
    Represents a received LIN Message
    """
    _fields_ = [ ("Type",           TLINMsgType),       # Frame type (see LIN Received Message Types)
                 ("FrameId",        c_ubyte),           # Frame ID (6 bit) + Parity (2 bit)
                 ("Length",         c_ubyte),           # Frame Length (1..8)
                 ("Direction",      TLINDirection),     # Frame Direction (see LIN Message Direction Types) 
                 ("ChecksumType",   TLINChecksumType),  # Frame Checksum type (see LIN Message Checksum Types)
                 ("Data",           c_ubyte * 8),       # Data bytes (0..7)
                 ("Checksum",       c_ubyte),           # Frame Checksum
                 ("ErrorFlags",     TLINMsgErrors),     # Frame error flags (see Error flags for LIN Rcv Msgs)
                 ("TimeStamp",      c_uint64),          # Timestamp in microseconds
                 ("hHw",            HLINHW)]            # Handle of the Hardware which received the message

# Represents a LIN Frame Entry
#
class TLINFrameEntry (Structure):
    """
    Represents a LIN Frame Entry
    """
    _fields_ = [ ("FrameId",        c_ubyte),           # Frame ID (without parity)
                 ("Length",         c_ubyte),           # Frame Length (1..8) 
                 ("Direction",      TLINDirection),     # Frame Direction (see LIN Message Direction Types) 
                 ("ChecksumType",   TLINChecksumType),  # Frame Checksum type (see LIN Message Checksum Types)
                 ("Flags",          c_ushort),          # Frame flags (see Frame flags for LIN Msgs)
                 ("InitialData",    c_ubyte * 8)]       # Data bytes (0..7)

# Represents a LIN Schedule slot
#
class TLINScheduleSlot (Structure):
    """
    Represents a LIN Schedule slot
    """
    _fields_ = [ ("Type",           TLINSlotType),  # Slot Type (see LIN Schedule Slot Types)
                 ("Delay",          c_ushort),      # Slot Delay in Milliseconds
                 ("FrameId",        c_ubyte * 8),   # Frame IDs (without parity)
                 ("CountResolve",   c_ubyte),       # ID count for sporadic frames, Resolve schedule number for Event frames
                 ("Handle",         c_uint)]        # Slot handle (read-only)

# Represents LIN Status data
#
class TLINHardwareStatus (Structure):
    """
    Represents LIN Status data
    """
    _fields_ = [ ("Mode",               TLINHardwareMode),  # Hardware mode (see LIN Hardware operation mode)
                 ("Status",             TLINHardwareState), # Hardware status (see LIN Hardware status)
                 ("FreeOnSendQueue",    c_ubyte),           # Count of free places in the Transmit Queue
                 ("CountResolve",       c_ushort),          # Free slots in the Schedule pool (see Minimum and Maximum values)
                 ("Handle",             c_ushort)]          # USB receive buffer overrun counter

#///////////////////////////////////////////////////////////
# PLIN-API function declarations
#///////////////////////////////////////////////////////////

# PLIN-API class implementation
#
class PLinApi:
    """
    PLIN-API class implementation
    """
    def __init__(self):
        # Loads the PLinApi.dll
        #     
        try:
            self.__m_dllLin = windll.LoadLibrary("PLinApi")
        except:
            self.__m_dllLin = None;
        if self.__m_dllLin == None:
            print("Exception: The PLIN-API DLL couldn't be loaded!")
            
    # Defines if the dll was successfully loaded
    #
    def isLoaded(self):
        """
        Defines if the dll was successfully loaded

        Returns:
            True if the dll was successfully loaded, False otherwise
        """
        return self.__m_dllLin != None

    # Registers a Client at the LIN Manager
    #
    def RegisterClient(self,
                       strName,
                       hWnd,
                       hClient):
        """
        Registers a Client at the LIN Manager. Creates a Client handle and 
        allocates the Receive Queue (only one per Client). 
        
        Remarks: 
            The hWnd parameter can be zero for DOS Box Clients. 
            The Client does not receive any messages until LIN_RegisterFrameId() 
            or LIN_SetClientFilter() is called.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING, 
            TLIN_ERROR_MEMORY_ACCESS
        
        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE 
        
        Parameters: 
            strName     : Name of the Client (python string) 
            hWnd        : Window handle of the Client (only for information purposes) (c_ulong) 
            hClient     : Client handle buffer (HLINCLIENT) 
        
        Returns:
            A LIN Error Code
        """        
        try:            
            buffer = create_string_buffer(str.encode(strName))
            res = self.__m_dllLin.LIN_RegisterClient(buffer, hWnd, byref(hClient))
            return TLINError(res)
        except:
            print("Exception on PLinApi.RegisterClient")
            raise

    # Removes a Client from the Client list of the LIN Manager
    #
    def RemoveClient(self,
                     hClient):
        """
        Removes a Client from the Client list of the LIN Manager. 
        Frees all resources (receive queues, message counters, etc.). 
        
        Remarks:
            If the Client was a Boss-Client for one or more Hardware, 
            the Boss-Client property for those Hardware will be set to 
            INVALID_LIN_HANDLE.
    
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS
    
        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT 
    
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 

        Returns:
            A LIN Error Code
        """ 
        try:
            res = self.__m_dllLin.LIN_RemoveClient(hClient)
            return TLINError(res)
        except:
            print("Exception on PLinApi.RemoveClient")
            raise

    # Connects a Client to a Hardware
    #
    def ConnectClient(self,
                      hClient,
                      hHw):
        """
        Connects a Client to a Hardware.
        The Hardware is assigned by its Handle.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware to be connected (HLINHW)
        
        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ConnectClient(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.ConnectClient")
            raise

    # Disconnects a Client from a Hardware
    #
    def DisconnectClient(self,
                         hClient,
                         hHw):
        """
        Disconnects a Client from a Hardware.

        Remarks:
            No more messages will be received by this Client from this Hardware.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware to be disconnected (HLINHW)
        
        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_DisconnectClient(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.DisconnectClient")
            raise

    # Flushes the Receive Queue of the Client and resets its counters
    #
    def ResetClient(self,
                    hClient):
        """
        Flushes the Receive Queue of the Client and resets its counters.

        Remarks:
            No more messages will be received by this Client from this Hardware.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
        
        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ResetClient(hClient)
            return TLINError(res)
        except:
            print("Exception on PLinApi.ResetClient")
            raise

    # Sets a Client parameter to a given value
    #
    def SetClientParam(self,
                       hClient,
                       wParam,
                       dwValue):
        """
        Sets a Client parameter to a given value.

        Remarks:
            Allowed TLINClientParam                 Parameter
            values in wParam:                       type:       Description:
            ------------------------               ----------  ------------------------------
            TLIN_CLIENTPARAM_RECEIVE_STATUS_FRAME   c_int       0 = Status Frames deactivated,
                                                                otherwise active
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_TYPE, TLIN_ERROR_WRONG_PARAM_VALUE, 
            TLIN_ERROR_ILLEGAL_CLIENT
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            wParam      : Parameter to set (TLINClientParam)
            dwValue     : Parameter value (see remarks)
        
        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetClientParam(hClient, wParam, dwValue)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetClientParam")
            raise

    # Gets a Client parameter
    #
    def GetClientParam(self,
                       hClient,
                       wParam, 
                       pBuff,
                       wBuffSize):
        """
        Gets a Client parameter.

        Remarks:
            Allowed TLINClientParam                 Parameter
            values in wParam:                       type:       Description:
            -------------------------              ----------  ------------------------------
            TLIN_CLIENTPARAM_NAME                   string      Name of the Client
            TLIN_CLIENTPARAM_MESSAGE_ON_QUEUE       c_int       Unread messages in the Receive Queue
            TLIN_CLIENTPARAM_WINDOW_HANDLE          c_int       Window handle of the Client application 
                                                                (can be zero for DOS Box Clients)
            TLIN_CLIENTPARAM_CONNECTED_HARDWARE     HLINHW[]    Array of Hardware Handles connected by a Client 
                                                                The first item in the array refers to the 
                                                                amount of handles. So [*] = Total handles + 1
            TLIN_CLIENTPARAM_TRANSMITTED_MESSAGES   c_int       Number of transmitted messages
            TLIN_CLIENTPARAM_RECEIVED_MESSAGES      c_int       Number of received messages
            TLIN_CLIENTPARAM_RECEIVE_STATUS_FRAME   c_int       0 = Status Frames deactivated, otherwise active
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_TYPE, TLIN_ERROR_WRONG_PARAM_VALUE, 
            TLIN_ERROR_ILLEGAL_CLIENT, TLIN_ERROR_BUFFER_INSUFFICIENT
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            wParam      : Parameter to get (TLINClientParam)
            pBuff       : Buffer for the parameter value (see remarks)
            wBuffSize   : Buffer size in bytes (c_ushort)
        
        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetClientParam(hClient, wParam, 
                                                     byref(pBuff), wBuffSize)
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetClientParam")
            raise

    # Sets the filter of a Client and modifies the filter of the connected Hardware
    #
    def SetClientFilter(self,
                        hClient,
                        hHw,
                        iRcvMask):
        """
        Sets the filter of a Client and modifies the filter of the connected Hardware.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            iRcvMask    : Filter. Each bit corresponds to a Frame ID (0..63) (c_uint64)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetClientFilter(hClient, hHw, iRcvMask)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetClientFilter")
            raise

    # Gets the filter corresponding to a given Client-Hardware pair
    #
    def GetClientFilter(self,
                        hClient,
                        hHw,
                        pRcvMask):
        """
        Gets the filter corresponding to a given Client-Hardware pair.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            pRcvMask    : Filter buffer. Each bit corresponds to a Frame ID (0..63) (c_uint64)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetClientFilter(hClient, hHw, byref(pRcvMask))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetClientFilter")
            raise        

    # Reads the next message/status information from a Client's Receive Queue
    #
    def Read(self,
             hClient,
             pMsg):
        """
        Reads the next message/status information from a Client's Receive Queue.

        Remarks:
             The message will be written to 'pMsg'.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_RCVQUEUE_EMPTY
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            pMsg        : Message read buffer (TLINRcvMsg)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_Read(hClient, byref(pMsg))
            return TLINError(res)
        except:
            print("Exception on PLinApi.Read")
            raise    

    # Reads several received messages
    #
    def ReadMulti(self,
                  hClient,
                  pMsgBuff,
                  iMaxCount,
                  pCount):
        """
        Reads several received messages.

        Remarks:
            pMsgBuff must be an array of 'iMaxCount' entries (must have at least 
            a size of iMaxCount * sizeof(TLINRcvMsg) bytes).
            The size 'iMaxCount' of the array = max. messages that can be received.
            The real number of read messages will be returned in 'pCount'.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_RCVQUEUE_EMPTY
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            pMsgBuff    : Messages buffer (TLINRcvMsg[])
            iMaxCount   : Maximum number of messages to read (pMsgBuff length) (c_int)
            pCount      : Buffer for the real number of messages read (c_int)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ReadMulti(hClient, byref(pMsgBuff), 
                                                iMaxCount, byref(pCount))
            return TLINError(res)
        except:
            print("Exception on PLinApi.ReadMulti")
            raise

    # The Client 'hClient' transmits a message 'pMsg' to the Hardware 'hHw'
    #
    def Write(self,
              hClient,
              hHw,
              pMsg):
        """
        The Client 'hClient' transmits a message 'pMsg' to the Hardware 'hHw'.

        Remarks:
             The message is written into the Transmit Queue of the Hardware.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_ILLEGAL_DIRECTION, 
            TLIN_ERROR_ILLEGAL_LENGTH
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            pMsg        : Message write buffer (TLINMsg)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_Write(hClient, hHw, byref(pMsg))
            return TLINError(res)
        except:
            print("Exception on PLinApi.Write")
            raise     

    # Initializes a Hardware with a given Mode and Baudrate.
    #
    def InitializeHardware(self,
                           hClient,
                           hHw,
                           byMode,
                           wBaudrate):
        """
        Initializes a Hardware with a given Mode and Baudrate.

        Remarks:
            If the Hardware was initialized by another Client, the function 
            will re-initialize the Hardware. All connected clients will be affected.
            It is the job of the user to manage the setting and/or configuration of 
            Hardware, e.g. by using the Boss-Client parameter of the Hardware.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_ILLEGAL_BAUDRATE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            byMode      : Hardware mode (see TLINHardwareMode)
            wBaudrate   : Baudrate (see LIN_MIN_BAUDRATE and LIN_MAX_BAUDRATE) (c_ushort)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_InitializeHardware(hClient, hHw, byMode, wBaudrate)
            return TLINError(res)
        except:
            print("Exception on PLinApi.InitializeHardware")
            raise  

    # Gets an array containing the handles of the current Hardware available in the system
    #
    def GetAvailableHardware(self,
                             pBuff,
                             wBuffSize,
                             pCount):
        """
        Gets an array containing the handles of the current Hardware available in the system.
        The count of Hardware handles returned in the array is written in 'pCount'.

        Remarks:
            To ONLY get the count of available Hardware, call this 
            function using 'pBuff' = NULL and wBuffSize = 0.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_BUFFER_INSUFFICIENT
        
        Parameters:
            pBuff       : Buffer for the handles (HLINHW[])
            wBuffSize   : Size of the buffer in bytes (pBuff.Length * 2) (c_ushort)
            pCount      : Number of Hardware available (c_ushort)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetAvailableHardware(byref(pBuff), 
                                                           wBuffSize, 
                                                           byref(pCount))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetAvailableHardware")
            raise     

    # Sets a Hardware parameter to a given value.
    #
    def SetHardwareParam(self,
                         hClient,
                         hHw,
                         wParam,
                         pBuff,
                         wBuffSize):
        """
        Sets a Hardware parameter to a given value.

        Remarks:
            Allowed TLINHardwareParam           Parameter
            values in wParam:                   type:       Description:
            -------------------------           ----------  ------------------------------
            TLIN_HARDWAREPARAM_MESSAGE_FILTER   c_uint64    Hardware message filter. Each bit
                                                            corresponds to a Frame ID (0..63)
            TLIN_HARDWAREPARAM_BOSS_CLIENT      HLINCLIENT  Handle of the new Boss-Client
            TLIN_HARDWAREPARAM_ID_NUMBER        c_int       Identification number for a hardware
            TLIN_HARDWAREPARAM_USER_DATA        c_ubyte[]   User data to write on a hardware. See LIN_MAX_USER_DATA
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_TYPE, TLIN_ERROR_WRONG_PARAM_VALUE, 
            TLIN_ERROR_ILLEGAL_CLIENT, TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            wParam      : Parameter to set (TLINHardwareParam)
            pBuff       : Parameter value buffer (see remarks)
            wBuffSize   : Value buffer size (ushort)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetHardwareParam(hClient, hHw, 
                                                       wParam,
                                                       byref(pBuff), 
                                                       wBuffSize)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetHardwareParam")
            raise     

    # Gets a Hardware parameter
    #
    def GetHardwareParam(self,
                         hHw,
                         wParam,
                         pBuff,
                         wBuffSize):
        """
        Gets a Hardware parameter.

        Remarks:
            Allowed TLINHardwareParam               Parameter
            values in wParam:                       type:           Description:
            ------------------------               ----------     ------------------------------
            TLIN_HARDWAREPARAM_NAME                 string          Name of the Hardware. See LIN_MAX_NAME_LENGTH
            TLIN_HARDWAREPARAM_DEVICE_NUMBER        c_int           Index of the Device owner of the Hardware
            TLIN_HARDWAREPARAM_CHANNEL_NUMBER       c_int           Channel Index of the Hardware on the owner device
            TLIN_HARDWAREPARAM_CONNECTED_CLIENTS    HLINCLIENT[]    Array of Client Handles conencted to a Hardware 
                                                                    The first item in the array refers to the 
                                                                    amount of handles. So [*] = Total handles + 1
            TLIN_HARDWAREPARAM_MESSAGE_FILTER       c_uint64        Configured message filter. Each bit corresponds 
                                                                    to a Frame ID (0..63)
            TLIN_HARDWAREPARAM_BAUDRATE             c_int           Configured baudrate
            TLIN_HARDWAREPARAM_MODE                 c_int           0 = Slave, otehrwise Master
            TLIN_HARDWAREPARAM_FIRMWARE_VERSION     TLINVersion     A TLINVersion structure containing the Firmware Version
            TLIN_HARDWAREPARAM_BUFFER_OVERRUN_COUNT c_int           Receive Buffer Overrun Counter
            TLIN_HARDWAREPARAM_BOSS_CLIENT          HLINCLIENT      Handle of the current Boss-Client
            TLIN_HARDWAREPARAM_SERIAL_NUMBER        c_int           Serial number of the Hardware
            TLIN_HARDWAREPARAM_VERSION              c_int           Version of the Hardware
            TLIN_HARDWAREPARAM_TYPE                 c_int           Type of the Hardware
            TLIN_HARDWAREPARAM_OVERRUN_COUNT        c_int           Receive Queue Buffer Overrun Counter
            TLIN_HARDWAREPARAM_ID_NUMBER            c_int           Identification number for a hardware
            TLIN_HARDWAREPARAM_USER_DATA            c_ubyte[]       User data saved on the hardware. See LIN_MAX_USER_DATA
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_TYPE, TLIN_ERROR_WRONG_PARAM_VALUE, 
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_BUFFER_INSUFFICIENT 
        
        Parameters:
            hHw         : Handle of the Hardware (HLINHW)
            wParam      : Parameter to get (TLINHardwareParam)
            pBuff       : Parameter buffer (see remarks)
            wBuffSize   : Buffer size (ushort)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetHardwareParam(hHw, wParam,
                                                       byref(pBuff),
                                                       wBuffSize)
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetHardwareParam")
            raise     

    # Flushes the queues of the Hardware and resets its counters
    #
    def ResetHardware(self,
                      hClient,
                      hHw):
        """
        Flushes the queues of the Hardware and resets its counters.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING, 
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ResetHardware(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.ResetHardware")
            raise     

    # Deletes the current configuration of the Hardware and sets its defaults
    #
    def ResetHardwareConfig(self,
                            hClient,
                            hHw):
        """
        Deletes the current configuration of the Hardware and sets its defaults.
        The Client 'hClient' must be registered and connected to the Hardware to 
        be accessed.
                
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ResetHardwareConfig(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.ResetHardwareConfig")
            raise     

    # Physically identifies a LIN Hardware (a channel on a LIN Device) by blinking its associated LED.
    #
    def IdentifyHardware(self,
                         hHw):
        """
        Physically identifies a LIN Hardware (a channel on a LIN Device) by 
        blinking its associated LED.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_IdentifyHardware(hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.IdentifyHardware")
            raise     

    #  Modifies the filter of a Client
    #
    def RegisterFrameId(self,
                        hClient,
                        hHw, 
                        bFromFrameId, 
                        bToFrameId):
        """
        Modifies the filter of a Client and, eventually, the filter of the 
        connected Hardware. The messages with FrameID 'bFromFrameId' to 
        'bToFrameId' will be received.

        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_ILLEGAL_FRAMEID
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            bFromFrameId: First ID of the frame range (c_ubyte)
            bToFrameId  : Last ID of the frame range (c_ubyte)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_RegisterFrameId(hClient, hHw,
                                                      bFromFrameId,
                                                      bToFrameId)
            return TLINError(res)
        except:
            print("Exception on PLinApi.RegisterFrameId")
            raise     

    # Configures a LIN Frame in a given Hardware
    #
    def SetFrameEntry(self,
                      hClient,
                      hHw,
                      pFrameEntry):
        """
        Configures a LIN Frame in a given Hardware. The Client 'hClient' must 
        be registered and connected to the Hardware to be accessed.
                
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_ILLEGAL_FRAMEID,
            TLIN_ERROR_ILLEGAL_LENGTH
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            pFrameEntry : Frame entry information buffer (TLINFrameEntry)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetFrameEntry(hClient, hHw,
                                                    byref(pFrameEntry))
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetFrameEntry")
            raise     

    # Gets the configuration of a LIN Frame from a given Hardware
    #
    def GetFrameEntry(self,
                      hHw,
                      pFrameEntry):
        """
        Gets the configuration of a LIN Frame from a given Hardware.

        Remarks:
             The 'pFrameEntry.FrameId' must be set to the ID of the frame, whose 
             configuration should be returned.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_HARDWARE, 
            TLIN_ERROR_ILLEGAL_FRAMEID
        
        Parameters:
            hHw         : Handle of the Hardware (HLINHW)
            pFrameEntry : Frame entry buffer (TLINFrameEntry).
                          The member FrameId  must be set to the ID of the frame, 
                          whose configuration should  be returned

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetFrameEntry(hHw, byref(pFrameEntry))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetFrameEntry")
            raise     

    # Updates the data of a LIN Frame for a given Hardware
    #
    def UpdateByteArray(self,
                        hClient, 
                        hHw,
                        bFrameId, 
                        bIndex,
                        bLen, 
                        pData):
        """
        Updates the data of a LIN Frame for a given Hardware. The Client 
        'hClient' must be registered and connected to the Hardware to be 
        accessed. 'pData' must have at least a size of 'bLen'.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_ILLEGAL_FRAMEID, 
            TLIN_ERROR_ILLEGAL_LENGTH, TLIN_ERROR_ILLEGAL_INDEX, 
            TLIN_ERROR_ILLEGAL_RANGE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            bFrameId    : Frame ID (c_ubyte)
            bIndex      : Index where the update data Starts (0..7) (c_ubyte)
            bLen        : Count of Data bytes to be updated. (c_ubyte)
            pData       : Data buffer (c_ubyte[])

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_UpdateByteArray(hClient, hHw,
                                                      bFrameId, bIndex,
                                                      bLen, byref(pData))
            return TLINError(res)
        except:
            print("Exception on PLinApi.UpdateByteArray")
            raise     

    # Sets the Frame 'bFrameId' as Keep-Alive frame for the given Hardware
    #
    def StartKeepAlive(self,
                       hClient,
                       hHw,
                       bFrameId,
                       wPeriod):
        """
        Sets the Frame 'bFrameId' as Keep-Alive frame for the given Hardware and 
        starts to send it every 'wPeriod' Milliseconds. The Client 'hClient' must 
        be registered and connected to the Hardware to be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, TLIN_ERROR_ILLEGAL_FRAMEID
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            bFrameId    : ID of the Keep-Alive Frame (c_ubyte)
            wPeriod     : Keep-Alive Interval in Milliseconds (c_ushort)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_StartKeepAlive(hClient, hHw,
                                                     bFrameId, wPeriod)
            return TLINError(res)
        except:
            print("Exception on PLinApi.StartKeepAlive")
            raise     

    # Suspends the sending of a Keep-Alive frame in the given Hardware
    #
    def SuspendKeepAlive(self,
                         hClient,
                         hHw):
        """
        Suspends the sending of a Keep-Alive frame in the given Hardware.
        The Client 'hClient' must be registered and connected to the Hardware 
        to be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SuspendKeepAlive(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SuspendKeepAlive")
            raise     

    # Resumes the sending of a Keep-Alive frame in the given Hardware
    #
    def ResumeKeepAlive(self,
                        hClient,
                        hHw):
        """
        Resumes the sending of a Keep-Alive frame in the given Hardware. 
        The Client 'hClient' must be registered and connected to the Hardware 
        to be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ResumeKeepAlive(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.ResumeKeepAlive")
            raise     

    # Configures the slots of a Schedule in a given Hardware
    #
    def SetSchedule(self,
                    hClient,
                    hHw, 
                    iScheduleNumber,
                    pSchedule, 
                    iSlotCount):
        """
        Configures the slots of a Schedule in a given Hardware. 

        Remarks:
            The Client 'hClient' must be registered and connected to the Hardware
            to be accessed. The Slot handles will be returned in the parameter 
            "pSchedule" (Slots buffer), when this function successfully completes. 
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, 
            TLIN_ERROR_ILLEGAL_SCHEDULENUMBER, 
            TLIN_ERROR_ILLEGAL_SLOTCOUNT

        Parameters:
            hClient         : Handle of the Client  (HLINCLIENT) 
            hHw             : Handle of the Hardware (HLINHW)
            iScheduleNumber : Schedule number (c_int)
                              (see LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)
            pSchedule       : Slots buffer (TLINScheduleSlot[])
            iSlotCount      : Count of Slots in the slots buffer (c_int)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetSchedule(hClient, hHw, 
                                                  iScheduleNumber,
                                                  byref(pSchedule), 
                                                  iSlotCount)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetSchedule")
            raise 

    # Gets the slots of a Schedule from a given Hardware
    #
    def GetSchedule(self,
                    hHw,
                    iScheduleNumber,
                    pScheduleBuff,
                    iMaxSlotCount,
                    pSlotCount):
        """
        Gets the slots of a Schedule from a given Hardware. The count of slots 
        returned in the array is written in 'pSlotCount'.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_HARDWARE, 
            TLIN_ERROR_ILLEGAL_SCHEDULENUMBER, 
            TLIN_ERROR_ILLEGAL_SLOTCOUNT
        
        Parameters:
            hHw             : Handle of the Hardware (HLINHW)
            iScheduleNumber : Schedule Number (see LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)
            pScheduleBuff   : Slots Buffer (TLINScheduleSlot[])
            iMaxSlotCount   : Maximum number of slots to read. (c_int)
            pSlotCount      : Real number of slots read. (c_int)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetSchedule(hHw, iScheduleNumber,
                                                  byref(pScheduleBuff),
                                                  iMaxSlotCount,
                                                  byref(pSlotCount))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetSchedule")
            raise 

    # Removes all slots contained by a Schedule of a given Hardware
    #
    def DeleteSchedule(self,
                       hClient,
                       hHw,
                       iScheduleNumber):
        """
        Removes all slots contained by a Schedule of a given Hardware. The 
        Client 'hClient' must be registered and connected to the Hardware to 
        be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, 
            TLIN_ERROR_ILLEGAL_SCHEDULENUMBER
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            iScheduleNumber : Schedule Number (c_int)
                              (see LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_DeleteSchedule(hClient, hHw,
                                                     iScheduleNumber)
            return TLINError(res)
        except:
            print("Exception on PLinApi.DeleteSchedule")
            raise 

    # Sets a 'breakpoint' on a slot from a Schedule in a given Hardware
    #
    def SetScheduleBreakPoint(self,
                              hClient,
                              hHw,
                              iBreakPointNumber,
                              dwHandle):
        """
        Sets a 'breakpoint' on a slot from a Schedule in a given Hardware. The 
        Client 'hClient' must be registered and connected to the Hardware to 
        be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient             : Handle of the Client  (HLINCLIENT) 
            hHw                 : Handle of the Hardware (HLINHW)
            iBreakPointNumber   : Breakpoint Number (0 or 1) (c_int)
            dwHandle            : Slot Handle (c_uint)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetScheduleBreakPoint(hClient, hHw,
                                                            iBreakPointNumber,
                                                            dwHandle)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetScheduleBreakPoint")
            raise 

    # Activates a Schedule in a given Hardware
    #
    def StartSchedule(self,
                      hClient,
                      hHw,
                      iScheduleNumber):
        """
        Activates a Schedule in a given Hardware. The Client 'hClient' must 
        be registered and connected to the Hardware to be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT, 
            TLIN_ERROR_ILLEGAL_HARDWARE, 
            TLIN_ERROR_ILLEGAL_SCHEDULENUMBER
        
        Parameters:
            hClient         : Handle of the Client  (HLINCLIENT) 
            hHw             : Handle of the Hardware (HLINHW)
            iScheduleNumber : Schedule Number (c_int)
                              (see LIN_MIN_SCHEDULE_NUMBER and LIN_MAX_SCHEDULE_NUMBER)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_StartSchedule(hClient, hHw, 
                                                    iScheduleNumber)
            return TLINError(res)
        except:
            print("Exception on PLinApi.StartSchedule")
            raise 

    # Suspends an active Schedule in a given Hardware
    #
    def SuspendSchedule(self,
                        hClient,
                        hHw):
        """
        Suspends an active Schedule in a given Hardware. The Client 'hClient' 
        must be registered and connected to the Hardware to be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SuspendSchedule(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SuspendSchedule")
            raise 

    # Restarts a configured Schedule in a given Hardware
    #
    def ResumeSchedule(self,
                       hClient,
                       hHw):
        """
        Restarts a configured Schedule in a given Hardware. The Client 'hClient' 
        must be registered and connected to the Hardware to be accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_ResumeSchedule(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.ResumeSchedule")
            raise 

    # Sends a wake-up message impulse (single data byte 0xF0)
    #
    def XmtWakeUp(self,
                  hClient,
                  hHw):
        """
        Sends a wake-up message impulse (single data byte 0xF0). The Client 
        'hClient' must be registered and connected to the Hardware to be 
        accessed.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING, 
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_XmtWakeUp(hClient, hHw)
            return TLINError(res)
        except:
            print("Exception on PLinApi.XmtWakeUp")
            raise 

    # Starts a process to detect the Baud rate of the LIN bus
    #
    def StartAutoBaud(self,
                      hClient,
                      hHw, 
                      wTimeOut):
        """
        Starts a process to detect the Baud rate of the LIN bus that is 
        connected to the indicated Hardware.
        The Client 'hClient' must be registered and connected to the Hardware 
        to be accessed. The Hardware must be not initialized in order 
        to do an Auto-baudrate procedure.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            wTimeOut    : Auto-baudrate Timeout in Milliseconds (c_ushort)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_StartAutoBaud(hClient, hHw, wTimeOut)
            return TLINError(res)
        except:
            print("Exception on PLinApi.StartAutoBaud")
            raise 

    # Retrieves current status information from the given Hardware
    #
    def GetStatus(self,
                  hHw,
                  pStatusBuff):
        """
        Retrieves current status information from the given Hardware.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hHw         : Handle of the Hardware (HLINHW)
            pStatusBuff : Status data buffer (TLINHardwareStatus)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetStatus(hHw, byref(pStatusBuff))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetStatus")
            raise 

    # Calculates the checksum of a LIN Message
    #
    def CalculateChecksum(self,
                          pMsg):
        """
        Calculates the checksum of a LIN Message and writes it into the 
        'Checksum' field of 'pMsg'.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING, 
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_LENGTH        

        Parameters:
            pMsg        : Message buffer (TLINMsg)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_CalculateChecksum(byref(pMsg))
            return TLINError(res)
        except:
            print("Exception on PLinApi.CalculateChecksum")
            raise 

    # Returns a TLINVersion structure containing the PLIN-API DLL version
    #
    def GetVersion(self,
                   pVerBuffer):
        """
        Returns a TLINVersion structure containing the PLIN-API DLL version.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE
        
        Parameters:
            pVerBuffer  : Version buffer (TLINVersion)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetVersion(byref(pVerBuffer))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetVersion")
            raise 

    # Returns a string containing Copyright information
    #
    def GetVersionInfo(self,
                       pTextBuff,
                       wBuffSize):
        """
        Returns a string containing Copyright information.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE
        
        Parameters:
            pTextBuff   : String buffer (character array from create_string_buffer)
            wBuffSize   : Size in bytes of the buffer (c_int)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetVersionInfo(byref(pTextBuff), wBuffSize)
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetVersionInfo")
            raise 

    # Converts the error code 'dwError' to a text containing an error description
    #
    def GetErrorText(self,
                     dwError,
                     bLanguage,
                     strTextBuff, 
                     wBuffSize):
        """
        Converts the error code 'dwError' to a text containing an error 
        description in the language given as parameter (when available).
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING, 
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_BUFFER_INSUFFICIENT
        
        Parameters:
            dwError     : A TLINError Code (TLINError)
            bLanguage   : Indicates a "Primary language ID" (c_ubyte)
            strTextBuff : Error string buffer (character array from create_string_buffer)
            wBuffSize   : Buffer size in bytes (c_int)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetErrorText(dwError, bLanguage,
                                                   byref(strTextBuff), wBuffSize)
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetErrorText")
            raise 

    # Gets the 'FrameId with Parity'
    #
    def GetPID(self,
               pframeid):
        """
        Gets the 'FrameId with Parity' corresponding to the given 
        'pFrameId' and writes the result on it.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED, 
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_FRAMEID
        
        Parameters:
            pframeid    : Frame ID (0..LIN_MAX_FRAME_ID) (c_ubyte)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetPID(byref(pframeid))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetPID")
            raise 

    # Gets the system time used by the LIN-USB adapter
    #
    def GetTargetTime(self,
                      hHw,
                      pTargetTime):
        """
        Gets the system time used by the LIN-USB adapter.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hHw         : Handle of the Hardware (HLINHW)
            pTargetTime : Target Time buffer (c_uint64)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetTargetTime(hHw, 
                                                    byref(pTargetTime))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetTargetTime")
            raise 

    # Sets the Response Remap of a LIN Slave
    #
    def SetResponseRemap(self,
                         hClient,
                         hHw, 
                         pRemapTab):
        """
        Sets the Response Remap of a LIN Slave.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING, 
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_FRAMEID, 
            TLIN_ERROR_ILLEGAL_CLIENT, TLIN_ERROR_ILLEGAL_HARDWARE,
            TLIN_ERROR_MEMORY_ACCESS
        
        Parameters:
            hClient     : Handle of the Client  (HLINCLIENT) 
            hHw         : Handle of the Hardware (HLINHW)
            pRemapTab   : Remap Response buffer (c_ubyte[64])

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_SetResponseRemap(hClient, hHw, 
                                                       pRemapTab)
            return TLINError(res)
        except:
            print("Exception on PLinApi.SetResponseRemap")
            raise 

    # Gets the Response Remap of a LIN Slave
    #
    def GetResponseRemap(self,
                         hHw,
                         pRemapTab):
        """
        Gets the Response Remap of a LIN Slave.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MANAGER_NOT_LOADED,
            TLIN_ERROR_MANAGER_NOT_RESPONDING,
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE, TLIN_ERROR_ILLEGAL_CLIENT,
            TLIN_ERROR_ILLEGAL_HARDWARE
        
        Parameters:
            hHw         : Handle of the Hardware (HLINHW)
            pRemapTab   : Remap Response buffer (c_ubyte[64])

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetResponseRemap(hHw, byref(pRemapTab))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetResponseRemap")
            raise 

    # Gets the current system time
    #
    def GetSystemTime(self,
                      pTargetTime):
        """
        Gets the current system time. The system time is returned by 
        Windows as the elapsed number of microseconds since system start.
        
        Possible DLL interaction errors:
            TLIN_ERROR_MEMORY_ACCESS

        Possible API errors:
            TLIN_ERROR_WRONG_PARAM_VALUE
        
        Parameters:
            pTargetTime : System Time buffer (c_uint64)

        Returns:
            A LIN Error Code
        """
        try:
            res = self.__m_dllLin.LIN_GetSystemTime(byref(pTargetTime))
            return TLINError(res)
        except:
            print("Exception on PLinApi.GetSystemTime")
            raise 
