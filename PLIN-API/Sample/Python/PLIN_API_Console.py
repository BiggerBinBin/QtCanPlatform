# *****************************************************************
#  PLIN-API Example
#
#  ~~~~~~~~~~~~
#
#  ------------------------------------------------------------------
#  Author : Fabrice Vergnaud
#  Language: Python 2.7 / 3.3
#  ------------------------------------------------------------------
#
#  Copyright (C) 1999-2013  PEAK-System Technik GmbH, Darmstadt
# *****************************************************************

import sys
import os
import traceback
import time
from ctypes import *

import PLinApi


# *****************************************************************
# PLIN-API Console example
# *****************************************************************
class PLinApiConsole:

    """
    PLIN-API Console example
    """

    def __init__(self):
        self.initialize()

    # Initialize LIN attributes
    #
    def initialize(self):
        # API configuration
        self.m_objPLinApi = PLinApi.PLinApi()
        if (not self.m_objPLinApi.isLoaded()):
            raise Exception("PLin-API could not be loaded ! Exiting...")
        # configure LIN variables
        self.m_hClient = PLinApi.HLINCLIENT(0)
        self.m_hHw = PLinApi.HLINHW(0)
        self.m_HwMode = PLinApi.TLIN_HARDWAREMODE_NONE
        self.m_HwBaudrate = c_ushort(0)
        self.FRAME_FILTER_MASK = c_uint64(0xFFFFFFFFFFFFFFFF)
        self.m_lMask = self.FRAME_FILTER_MASK
        # register LIN client
        self.m_objPLinApi.RegisterClient(
            "PLIN-API Console", None, self.m_hClient)
        # initialize a dictionnary to get LIN ID from PID
        self.PIDs = {}
        for i in range(64):
            nPID = c_ubyte(i)
            self.m_objPLinApi.GetPID(nPID)
            self.PIDs[nPID.value] = i

    # Uninitialize LIN attributes
    #
    def uninitialize(self):
        # disconnect from LIN if necessary
        if (self.m_hClient.value != PLinApi.HLINCLIENT(0).value):
            self.doLinDisconnect()
            self.m_hHw = PLinApi.HLINHW(0)
            # Unregister the application
            self.m_objPLinApi.RemoveClient(self.m_hClient)
            self.m_hClient = PLinApi.HLINCLIENT(0)

    # *****************************************************************
    # LIN functions
    # *****************************************************************

    # Connects to a LIN hardware
    #
    def doLinConnect(self, hwHandle, hwMode, hwBaudrate):
        """
        Connects to a LIN hardware.

        Parameters:
            hwHandle    LIN hardware handle (HLINHW)
            hwMode      LIN hardware mode (see TLIN_HARDWAREMODE_MASTER and TLIN_HARDWAREMODE_SLAVE)
            hwMode      LIN hardware baudrate (c_ushort)

        Returns:
            True if connection is successful, False otherwise            
        """
        result = False
        if (self.m_hHw.value != 0):
            # If a connection to hardware already exits
            # disconnect this connection first.
            if (not self.doLinDisconnect()):
                return result
        # Try to connect the application client to the hardware with the local
        # handle.
        linResult = self.m_objPLinApi.ConnectClient(self.m_hClient, hwHandle)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            # If the connection successfull assign
            # the local handle to the member handle.
            self.m_hHw = hwHandle
            # read hardware's parameter
            lnMode = c_int(0)
            lnCurrBaud = c_int(0)
            linResult = self.m_objPLinApi.GetHardwareParam(
                hwHandle, PLinApi.TLIN_HARDWAREPARAM_MODE, lnMode, 0)
            linResult = self.m_objPLinApi.GetHardwareParam(
                hwHandle, PLinApi.TLIN_HARDWAREPARAM_BAUDRATE, lnCurrBaud, 0)
            # check if initialization is required
            if (lnMode.value == PLinApi.TLIN_HARDWAREMODE_NONE.value or lnCurrBaud.value != hwBaudrate.value):
                # Only if the current hardware is not initialized
                # try to Intialize the hardware with mode and baudrate
                linResult = self.m_objPLinApi.InitializeHardware(
                    self.m_hClient, self.m_hHw, hwMode, hwBaudrate)
            if (linResult == PLinApi.TLIN_ERROR_OK):
                self.m_HwMode = hwMode
                self.m_HwBaudrate = hwBaudrate
                # Set the client filter with the mask.
                linResult = self.m_objPLinApi.SetClientFilter(
                    self.m_hClient, self.m_hHw, self.m_lMask)
                # Read the frame table from the connected hardware.
                self.readFrameTableFromHw()
                # Reset the last LIN error code to default.
                linResult = PLinApi.TLIN_ERROR_OK
                result = True
            else:
                # An error occured while initializing hardware.
                # Set the member variable to default.
                self.m_hHw = PLinApi.HLINHW(0)
                result = False
        else:
            # The local hardware handle is invalid
            # and/or an error occurs while connecting
            # hardware with client.
            # Set the member variable to default.
            self.m_hHw = PLinApi.HLINHW(0)
            result = False
        if (linResult != PLinApi.TLIN_ERROR_OK):
            self.displayError(linResult)
        return result

    # Disconnects an existing connection to a LIN hardware
    #
    def doLinDisconnect(self):
        # If the application was registered with LIN as client.
        if (self.m_hHw.value != 0):
            # The client was connected to a LIN hardware.
            # Before disconnect from the hardware check
            # the connected clients and determine if the
            # hardware configuration have to reset or not.
            #
            # Initialize the locale variables.
            lfOtherClient = False
            lfOwnClient = False
            lhClientsSize = c_ushort(255)
            lhClients = (PLinApi.HLINCLIENT * lhClientsSize.value)()
            # Get the connected clients from the LIN hardware.
            linResult = self.m_objPLinApi.GetHardwareParam(
                self.m_hHw, PLinApi.TLIN_HARDWAREPARAM_CONNECTED_CLIENTS, lhClients, lhClientsSize)
            if (linResult == PLinApi.TLIN_ERROR_OK):
                # No errors !
                # Check all client handles.
                for i in range(1, lhClientsSize.value):
                    # If client handle is invalid
                    if (lhClients[i] == 0):
                        continue
                    # Set the boolean to true if the handle isn't the
                    # handle of this application.
                    # Even the boolean is set to true it can never
                    # set to false.
                    lfOtherClient = lfOtherClient | (
                        lhClients[i] != self.m_hClient.value)
                    # Set the boolean to true if the handle is the
                    # handle of this application.
                    # Even the boolean is set to true it can never
                    # set to false.
                    lfOwnClient = lfOwnClient | (
                        lhClients[i] == self.m_hClient.value)
            # If another application is also connected to
            # the LIN hardware do not reset the configuration.
            if (lfOtherClient == False):
                # No other application connected !
                # Reset the configuration of the LIN hardware.
                linResult = self.m_objPLinApi.ResetHardwareConfig(
                    self.m_hClient, self.m_hHw)
            # If this application is connected to the hardware
            # then disconnect the client. Otherwise not.
            if (lfOwnClient == True):
                # Disconnect if the application was connected to a LIN
                # hardware.
                linResult = self.m_objPLinApi.DisconnectClient(
                    self.m_hClient, self.m_hHw)
                if (linResult == PLinApi.TLIN_ERROR_OK):
                    self.m_hHw = PLinApi.HLINHW(0)
                    return True
                else:
                    # Error while disconnecting from hardware.
                    self.displayError(linResult)
                    return False
            else:
                return True
        else:
            # m_hHw not connected
            return True

    # Reads all values from the frame table of the hardware.
    #
    def readFrameTableFromHw(self):
        """
        Reads all values from the frame table of the hardware.

        Returns:
            A global frame List of frame entry retrieved from the hardware
        """
        # Initialize result
        result = []
        # Initialize the member attribute for the
        # client mask with 0.
        self.m_lMask = c_uint64(0x0)
        llMask = c_uint64(0x0)
        # Read each Frame Definition
        for i in range(64):
            # Before a frame entry can be read from the
            # hardware, the Frame-ID of the wanted entry
            # must be set
            lFrameEntry = PLinApi.TLINFrameEntry()
            lFrameEntry.FrameId = c_ubyte(i)
            lFrameEntry.ChecksumType = PLinApi.TLIN_CHECKSUMTYPE_AUTO
            lFrameEntry.Direction = PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH
            # length values is set to LIN 1.2.
            if ((i >= 0x00) and (i <= 0x1F)):
                lFrameEntry.Length = c_ubyte(2)
            elif ((i >= 0x20) and (i <= 0x2F)):
                lFrameEntry.Length = c_ubyte(4)
            elif ((i >= 0x30) and (i <= 0x3F)):
                lFrameEntry.Length = c_ubyte(8)
            # Read the information of the specified frame entry from the
            # hardware.
            linResult = self.m_objPLinApi.GetFrameEntry(
                self.m_hHw, lFrameEntry)
            # Check the result value of the LinApi function call.
            if (linResult == PLinApi.TLIN_ERROR_OK):
                result.append(lFrameEntry)
                # Update the local application mask according to the frame
                # configuration
                if (lFrameEntry.Direction != PLinApi.TLIN_DIRECTION_DISABLED.value):
                    llMask = c_uint64((1 << i) & self.FRAME_FILTER_MASK.value)
                    self.m_lMask = c_uint64(self.m_lMask.value | llMask.value)
            # If the Client and Hardware handles are valid.
            if ((self.m_hClient.value != 0) and (self.m_hHw.value != 0)):
                # Set the client filter.
                self.m_objPLinApi.SetClientFilter(
                    self.m_hClient, self.m_hHw, self.m_lMask)
        return result

    # Get available LIN hardwares
    #
    def getAvailableHardware(self):
        """
        Returns a 2D list of available hardware.
        Each row describes a LIN hardware with, in order, its handle, type, device and channel number

        Returns:
            A list of LIN hardware information, the imbricated list is composed like so [handle, type, device_number, channel_number]
        """
        # Initialization
        res = []
        # Get all available LIN hardware.
        availableHWsSize = c_ushort(8)
        availableHWs = (PLinApi.HLINHW * availableHWsSize.value)()
        lwCount = c_ushort(0)
        linResult = self.m_objPLinApi.GetAvailableHardware(
            availableHWs, availableHWsSize, lwCount)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            # Get information for each LIN hardware found
            lnHwType = c_int(0)
            lnDevNo = c_int(0)
            lnChannel = c_int(0)
            lnMode = c_int(0)
            for i in range(lwCount.value):
                lwHw = availableHWs[i]
                # Read the type of the hardware with the handle lwHw.
                self.m_objPLinApi.GetHardwareParam(
                    lwHw, PLinApi.TLIN_HARDWAREPARAM_TYPE, lnHwType, 0)
                # Read the device number of the hardware with the handle lwHw.
                self.m_objPLinApi.GetHardwareParam(
                    lwHw, PLinApi.TLIN_HARDWAREPARAM_DEVICE_NUMBER, lnDevNo, 0)
                # Read the channel number of the hardware with the handle lwHw.
                self.m_objPLinApi.GetHardwareParam(
                    lwHw, PLinApi.TLIN_HARDWAREPARAM_CHANNEL_NUMBER, lnChannel, 0)
                # Read the mode of the hardware with the handle lwHw (Master,
                # Slave or None).
                self.m_objPLinApi.GetHardwareParam(
                    lwHw, PLinApi.TLIN_HARDWAREPARAM_MODE, lnMode, 0)
                # translate type value to string
                if (lnHwType.value == PLinApi.LIN_HW_TYPE_USB.value):
                    strName = "PCAN-USB Pro"
                else:
                    strName = "Unknown"
                # add information to result list
                res.append([strName, lnDevNo.value, lnChannel.value, lwHw])
        return res

    # *****************************************************************
    # Display Menus functions
    # *****************************************************************

    # Launches the console application
    #
    def run(self):
        """
        Launches the console application.

        Returns:
            None
        """
        bQuit = False
        while(not bQuit):
            bQuit = self.showMainMenu()
            self.clearMenu()

    # Displays the main console menu
    #
    def showMainMenu(self):
        """
        Displays the main console menu.
        
        Returns:
            True if user requested to quit, False otherwise
        """
        # print menu
        print("\n\nPLin-API Console:")
        print("\t")
        print("\t1) View available LIN hardware")
        print("\t2) Identify LIN hardware...")
        print("\t   ---")
        # Check if connected to a LIN hardware
        if (self.m_hHw.value == 0):
            print("\t3) Connect to a LIN hardware...")
        else:
            print("\t3) Release LIN hardware")
            print("\t   ---")
            print("\t4) Global frames table")
            print("\t5) Filter status")
            print("\t6) Read messages")
            if (self.m_HwMode.value == PLinApi.TLIN_HARDWAREMODE_SLAVE.value):
                print("\t7) Publish messages")
            else:
                print("\t7) Transmit messages")
            print('\t8) Status')
            print('\t9) Reset')
            print('\t10) Versions')
        print("\t   ---")
        print("\tq) Quit")
        # get user input
        choice = self.displayMenuInput()
        # process answer
        if choice == "1":
            # list LIN hardware
            self.menuAvailableHw()
        elif choice == "2":
            # identify LIN hardware
            self.menuIdentify()
        elif choice == "3":
            # connect/disconnect LIN hardware
            if self.m_hHw.value == 0:
                self.menuConnect()
            else:
                self.menuDisconnect()
        elif choice == "4":
            # view/configure Global Frame Table
            self.menuGlobalFrameTable()
        elif choice == "5":
            # view status filter
            self.menuFilter()
        elif choice == "6":
            # read messages
            self.menuRead()
        elif choice == "7":
            # write messages
            self.menuWrite()
        elif choice == "8":
            # show bus status
            self.menuStatus()
        elif choice == "9":
            # reset bus
            self.menuReset()
        elif choice == "10":
            # show api versions
            self.menuVersion()
        elif choice == "Q":
            # quit
            return True
        else:
            # invalid answer
            self.displayNotification()
        return False

    # Displays menu showing available LIN hardwares
    #
    def menuAvailableHw(self):
        """
        Displays a menu showing available LIN hardwares

        Returns:
            None
        """
        # show available hardware
        self.displayAvailableConnection()
        self.displayMenuInput("** Press <enter> to continue **")

    # Displays menu to identify LIN hardwares
    #
    def menuIdentify(self):
        """
        Displays a menu to identify available LIN hardwares

        Returns:
            None
        """
        bQuit = False
        while(not bQuit):
            # display menu information
            self.clearMenu()
            self.displayMenuHeader()
            self.displayAvailableConnection()
            self.displayMenuExit()
            # query user action
            choice = self.displayMenuInput("Select an hardware to identify: ")
            if (choice == "Q"):
                bQuit = True
            else:
                try:
                    res = int(choice)
                    # identify LIN hardware
                    linResult = self.m_objPLinApi.IdentifyHardware(res)
                    if (linResult != PLinApi.TLIN_ERROR_OK):
                        raise AssertionError()
                    else:
                        self.displayNotification(
                            str.format("Blinking LED for LIN hardware {0}", res))
                except:
                    # catch all exception (illegal int convertion or lin
                    # failure)
                    self.displayNotification()

    # Displays menu to connect to a LIN hardware
    #
    def menuConnect(self):
        """
        Displays a menu to connect to a LIN hardware

        Returns:
            None
        """
        bQuit = False
        while(not bQuit):
            # Display menu information
            self.clearMenu()
            self.displayMenuHeader()
            self.displayAvailableConnection()
            self.displayMenuExit()
            # Query user action
            choice = self.displayMenuInput(
                "Select an hardware to connect to: ")
            if (choice == "Q"):
                bQuit = True
            else:
                try:
                    lHw = PLinApi.HLINHW(int(choice))
                    # Query connection mode
                    print(" Available hardware mode:")
                    print("\t1) Master")
                    print("\t2) Slave")
                    choice = self.displayMenuInput("Specify connection mode: ")
                    if choice == "1":
                        lHwMode = PLinApi.TLIN_HARDWAREMODE_MASTER
                    elif choice == "2":
                        lHwMode = PLinApi.TLIN_HARDWAREMODE_SLAVE
                    else:
                        # interupt connection queries
                        raise ValueError()
                    # Query connection baudrate
                    choice = self.displayMenuInput(
                        "Specify baudrate [19200]: ")
                    if (choice == ""):
                        lHwBaudrate = c_ushort(19200)
                    else:
                        lHwBaudrate = c_ushort(int(choice))
                    # Do the connection
                    if (self.doLinConnect(lHw, lHwMode, lHwBaudrate)):
                        self.displayNotification("Connection succesfull")
                        bQuit = True
                    else:
                        self.displayNotification("Connection failed")
                except:
                    # catch all exception (LIN, bad input)
                    self.displayNotification()

    # Displays menu to disconnect from a LIN hardware
    #
    def menuDisconnect(self):
        """
        Displays a menu to disconnect from a LIN hardware

        Returns:
            None
        """
        # do the disconnection
        if (self.doLinDisconnect()):
            self.displayNotification("Disconnection succesfull")
        else:
            self.displayNotification("Disconnection failed")

    # Displays menu listing the global frame table
    #
    def menuGlobalFrameTable(self):
        """
        Displays menu listing the global frame table

        Returns:
            None
        """
        bQuit = False
        while(not bQuit):
            # print results
            self.clearMenu()
            self.displayMenuHeader()
            self.displayGlobalFramesTable()
            # display menu
            print("\t")
            print(
                "\t1) Configure default example configuration for global frames table")
            self.displayMenuExit()
            choice = self.displayMenuInput()
            if (choice == "Q"):
                bQuit = True
            # set default example configuration for the global frame table
            elif choice == "1":
                # retrieve global frames table
                pGFT = self.readFrameTableFromHw()
                # disable all frames
                for lFrameEntry in pGFT:
                    # disable all frames and set CST to enhanced
                    lFrameEntry.ChecksumType = PLinApi.TLIN_CHECKSUMTYPE_ENHANCED
                    lFrameEntry.Direction = PLinApi.TLIN_DIRECTION_DISABLED
                    # length values is set to LIN 1.2.
                    if ((lFrameEntry.FrameId >= 0x00) and (lFrameEntry.FrameId <= 0x1F)):
                        lFrameEntry.Length = c_ubyte(2)
                    elif ((lFrameEntry.FrameId >= 0x20) and (lFrameEntry.FrameId <= 0x2F)):
                        lFrameEntry.Length = c_ubyte(4)
                    elif ((lFrameEntry.FrameId >= 0x30) and (lFrameEntry.FrameId <= 0x3F)):
                        lFrameEntry.Length = c_ubyte(8)
                    # Set the information of the specified frame entry from the
                    # hardware.
                    linResult = self.m_objPLinApi.SetFrameEntry(
                        self.m_hClient, self.m_hHw, lFrameEntry)
                    if (linResult != PLinApi.TLIN_ERROR_OK):
                        self.displayError(linResult)
                # update filter mask (as every frame are disabled)
                lMask = c_uint64(0x0)
                linResult = self.m_objPLinApi.SetFrameEntry(
                    self.m_hClient, self.m_hHw, lFrameEntry)
                if (linResult != PLinApi.TLIN_ERROR_OK):
                    self.displayError(linResult)
                # define example frame entries
                # the example allows to communicate with a PLIN-SLAVE
                res = True
                # depending on the hardware mode, the direction of the frame is
                # changed
                if (self.m_HwMode.value == PLinApi.TLIN_HARDWAREMODE_MASTER.value):
                    directionPub = PLinApi.TLIN_DIRECTION_PUBLISHER
                    directionSub = PLinApi.TLIN_DIRECTION_SUBSCRIBER
                else:
                    directionPub = PLinApi.TLIN_DIRECTION_SUBSCRIBER
                    directionSub = PLinApi.TLIN_DIRECTION_PUBLISHER
                # set frame 0x01 (Status_xxx_LIN) :
                # master_direction=Subscriber, CST=enhanced, len=8
                res = res & self.setFrameEntry(
                    0x01, directionSub, PLinApi.TLIN_CHECKSUMTYPE_ENHANCED, 8)
                # set frame 0x02 (Error_Status_xxx_LIN) :
                # master_direction=Subscriber, CST=enhanced, len=2
                res = res & self.setFrameEntry(
                    0x02, directionSub, PLinApi.TLIN_CHECKSUMTYPE_ENHANCED, 2)
                # set frame 0x03 (Status_xxx_LIN_Event) :
                # master_direction=Subscriber, CST=enhanced, len=8
                res = res & self.setFrameEntry(
                    0x03, directionSub, PLinApi.TLIN_CHECKSUMTYPE_ENHANCED, 8)
                # set frame 0x05 (Control_xxx_LIN) :
                # master_direction=Publisher, CST=enhanced, len=2
                res = res & self.setFrameEntry(
                    0x05, directionPub, PLinApi.TLIN_CHECKSUMTYPE_ENHANCED, 2)
                # set frame 0x3C (MasterReq) :
                # master_direction=Publisher, CST=classic, len=8
                res = res & self.setFrameEntry(
                    0x3C, directionPub, PLinApi.TLIN_CHECKSUMTYPE_CLASSIC, 8)
                # set frame 0x3D (SlaveResp) :
                # master_direction=Subscriber, CST=classic, len=8
                res = res & self.setFrameEntry(
                    0x3D, directionSub, PLinApi.TLIN_CHECKSUMTYPE_CLASSIC, 8)
                if (res):
                    self.displayNotification(
                        "Global frames table successfully configured")
                else:
                    self.displayNotification(
                        "Global frames table configuration failed")
            else:
                self.displayNotification()

    # Displays menu showing current filter mask
    #
    def menuFilter(self):
        """
        Displays menu showing current filter mask

        Returns:
            None
        """
        # Retrieves the status of the LIN Bus and outputs its state
        lMask = c_uint64(0)
        linResult = self.m_objPLinApi.GetClientFilter(
            self.m_hClient, self.m_hHw, lMask)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            filter = bin(lMask.value)
            # remove first '0b' character
            filter = filter[2:]
            # pad with 0
            filter = filter.zfill(64)
            self.displayNotification(
                'The status of the filter is :\n\t' + filter)
            self.displayMenuInput("** Press <enter> to continue **")
        else:
            self.displayError(linResult)

    # Displays menu for reading messages
    #
    def menuRead(self):
        """
        Displays menu for reading messages

        Returns:
            None
        """
        bQuit = False
        # buffer of received message
        listMsg = []
        while(not bQuit):
            # Display menu information
            self.clearMenu()
            self.displayMenuHeader()
            print('\t0-9) Read a specified number of messages')
            self.displayMenuExit()
            # display received messages from previous loop
            if (len(listMsg) > 0):
                print('\n * Received messages:')
                print('\n   ID\tLength\tData\tTimestamp\tDirection\tErrors')
                print(
                    '   -----------------------------------------------------------')
                for msg in listMsg:
                    print('\n - ' + msg)
                # clear messages so that they do not appear on th next loop
                listMsg = []
            # Query user action
            choice = self.displayMenuInput("Number of messages to read [1]: ")
            if (choice == "Q"):
                bQuit = True
            else:
                try:
                    # read a single message
                    if (choice == '' or choice == "1"):
                        # initialize and read a LIN message
                        pRcvMsg = PLinApi.TLINRcvMsg()
                        linResult = self.m_objPLinApi.Read(
                            self.m_hClient, pRcvMsg)
                        if (linResult == PLinApi.TLIN_ERROR_OK):
                            # append received message to message list
                            listMsg.append(self.getFormattedRcvMsg(pRcvMsg))
                        elif (linResult == PLinApi.TLIN_ERROR_RCVQUEUE_EMPTY):
                            self.displayError(linResult, False, 0.5)
                        else:
                            self.displayError(linResult)
                    else:
                        # read multiple LIN messages
                        nbRead = int(choice)
                        pRcvMsgArray = (PLinApi.TLINRcvMsg * nbRead)()
                        pRcvMsgCount = c_int(0)
                        linResult = self.m_objPLinApi.ReadMulti(
                            self.m_hClient, pRcvMsgArray, nbRead, pRcvMsgCount)
                        if (linResult == PLinApi.TLIN_ERROR_OK or
                            linResult == PLinApi.TLIN_ERROR_RCVQUEUE_EMPTY):
                            # Check wether there was no message or not enough to fill the array
                            # resulting in a TLIN_ERROR_RCVQUEUE_EMPTY
                            if (pRcvMsgCount.value == 0):
                                self.displayError(linResult, False, 0.5)
                            else:
                                # append received messages to message list
                                for i in range(pRcvMsgCount.value):
                                    listMsg.append(
                                        self.getFormattedRcvMsg(pRcvMsgArray[i]))
                        else:
                            self.displayError(linResult)
                except:
                    self.displayNotification()

    # Displays menu for writing messages
    #
    def menuWrite(self):
        """
        Displays menu for writing messages

        Returns:
            None
        """
        bQuit = False
        bShowFrames = True
        while(not bQuit):
             # Display menu information
            self.clearMenu()
            self.displayMenuHeader()
            print("\t0x00-0x3F) Write a LIN message with a valid frame ID")
            print("\tt) Toggle display of global frames table")
            self.displayMenuExit()
            # display the available LIN frames
            if (bShowFrames):
                self.displayGlobalFramesTable(False)
            choice = self.displayMenuInput('Action or Frame ID (hex) : ')
            if (choice == "Q"):
                bQuit = True
            elif choice == "T":
                # toggle display of available LIN frames
                bShowFrames = not bShowFrames
            else:
                # user chose to write a message by typing a Frame ID
                try:
                    # read frame ID
                    frameId = int(choice, 16)
                    lFrameEntry = PLinApi.TLINFrameEntry()
                    lFrameEntry.FrameId = c_ubyte(frameId)
                    # get data length from frame
                    linResult = self.m_objPLinApi.GetFrameEntry(
                        self.m_hHw, lFrameEntry)
                    # initialize LIN message to sent
                    pMsg = PLinApi.TLINMsg()
                    pMsg.Direction = PLinApi.TLINDirection(
                        lFrameEntry.Direction)
                    pMsg.ChecksumType = PLinApi.TLINChecksumType(
                        lFrameEntry.ChecksumType)
                    pMsg.Length = c_ubyte(lFrameEntry.Length)
                    # query and fill data, only if direction is publisher
                    if (pMsg.Direction == PLinApi.TLIN_DIRECTION_PUBLISHER.value):
                        for i in range(lFrameEntry.Length):
                            choice = self.displayMenuInput(
                                str.format('Data[{0}] (hex) : ', i + 1))
                            pMsg.Data[i] = c_ubyte(int(choice, 16))
                    # Check if the hardware is initialized as master
                    if (self.m_HwMode.value == PLinApi.TLIN_HARDWAREMODE_MASTER.value):
                        # set frame id to Protected ID
                        nPID = c_ubyte(frameId)
                        linResult = self.m_objPLinApi.GetPID(nPID)
                        pMsg.FrameId = c_ubyte(nPID.value)
                        # set checksum
                        linResult = self.m_objPLinApi.CalculateChecksum(pMsg)
                        # write LIN message
                        linResult = self.m_objPLinApi.Write(
                            self.m_hClient, self.m_hHw, pMsg)
                    else:
                        # connected as slave : writing corresponds to updating
                        # the data from LIN frame
                        linResult = self.m_objPLinApi.UpdateByteArray(
                            self.m_hClient, self.m_hHw, c_ubyte(frameId), c_ubyte(0), c_ubyte(pMsg.Length), pMsg.Data)
                    if (linResult == PLinApi.TLIN_ERROR_OK):
                        self.displayNotification(
                            "Message successfully written")
                    else:
                        self.displayError(linResult)
                        self.displayNotification("Failed to write message")
                except:
                    # catch all exception (LIN, bad input)
                    self.displayNotification()

    # Displays menu showing current bus status
    #
    def menuStatus(self):
        """
        Displays menu showing current bus status

        Returns:
            None
        """
        # Retrieves the status of the LIN Bus and outputs its state
        pStatus = PLinApi.TLINHardwareStatus()
        linResult = self.m_objPLinApi.GetStatus(self.m_hHw, pStatus)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            # output bus status
            if (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_ACTIVE.value):
                self.displayNotification('Bus: Active')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_AUTOBAUDRATE.value):
                self.displayNotification('Hardware: Baudrate Detection')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_NOT_INITIALIZED.value):
                self.displayNotification('Hardware: Not Initialized')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_SHORT_GROUND.value):
                self.displayNotification('Bus-Line: Shorted Ground')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_SLEEP.value):
                self.displayNotification('Bus: Sleep')
        else:
            self.displayError(linResult)

    # Displays menu that resets current hardware
    #
    def menuReset(self):
        """
        Displays menu that resets current hardware

        Returns:
            None
        """
        # Flushes the Receive Queue of the Client and resets its counters.
        linResult = self.m_objPLinApi.ResetClient(self.m_hClient)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            self.displayNotification('Receive Queue successfully flushed')
        else:
            self.displayError(linResult)

    # Displays menu showing LIN Api version
    #
    def menuVersion(self):
        """
        Displays menu showing LIN Api version

        Returns:
            None
        """
        # get the vesion of the PLIN API
        lpVersion = PLinApi.TLINVersion()
        linResult = self.m_objPLinApi.GetVersion(lpVersion)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            print(str.format(" API Version: {0}.{1}.{2}.{3}",
                             lpVersion.Major,
                             lpVersion.Minor,
                             lpVersion.Build,
                             lpVersion.Revision))
            # get the driver version
            pTextBuff = create_string_buffer(255)
            linResult = self.m_objPLinApi.GetVersionInfo(pTextBuff, 255)
            if (linResult == PLinApi.TLIN_ERROR_OK):
                # Because this information contains line control characters (several lines)
                # we split this also in several entries in the Information
                # List-Box
                if (type(pTextBuff.value) is str):
                    # python 2 returns 'str'
                    strTemp = pTextBuff.value
                else:
                    # python 3 returns 'bytes'
                    strTemp = bytes.decode(pTextBuff.value)
                lines = str.split(strTemp, '\n')
                print("  Channel/Driver Version: ")
                for line in lines:
                    print("      * " + line)
        if (linResult != PLinApi.TLIN_ERROR_OK):
            self.displayError(linResult)
        else:
            self.displayMenuInput("** Press <enter> to continue **")

    # *****************************************************************
    # Helper Menus functions
    # *****************************************************************

    # Returns the string name of a PLinApi.TLINDirection value
    #
    def getFrameDirectionAsString(self, direction):
        """
        Returns the string name of a PLinApi.TLINDirection value

        Parameters:
            value   a PLinApi.TLINDirection value (or a number)

        Returns:
            a string name of the direction value
        """
        # check given parameter
        if (isinstance(direction, PLinApi.TLINDirection)):
            value = direction.value
        else:
            value = int(direction)
        # translate value to string
        if (value == PLinApi.TLIN_DIRECTION_DISABLED.value):
            return 'Disabled'
        elif (value == PLinApi.TLIN_DIRECTION_PUBLISHER.value):
            return 'Publisher'
        elif (value == PLinApi.TLIN_DIRECTION_SUBSCRIBER.value):
            return 'Subscriber'
        elif (value == PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH.value):
            return 'Subscriber Automatic Length'

    # Returns the string name of a PLinApi.TLINChecksumType value
    #
    def getFrameCSTAsString(self, checksumType):
        """
        Returns the string name of a PLinApi.TLINChecksumType value

        Parameters:
            value   a PLinApi.TLINChecksumType value (or a number)

        Returns:
            a string name of the checksum type value
        """
        # check given parameter
        if (isinstance(checksumType, PLinApi.TLINDirection)):
            value = checksumType.value
        else:
            value = int(checksumType)
        # translate value to string
        if (value == PLinApi.TLIN_CHECKSUMTYPE_AUTO.value):
            return 'Auto'
        elif (value == PLinApi.TLIN_CHECKSUMTYPE_CLASSIC.value):
            return 'Classic'
        elif (value == PLinApi.TLIN_CHECKSUMTYPE_CUSTOM.value):
            return 'Custom'
        elif (value == PLinApi.TLIN_CHECKSUMTYPE_ENHANCED.value):
            return 'Enhanced'

    # Returns a string formatted LIN receive message
    #
    def getFormattedRcvMsg(self, msg):
        """
        Returns a string formatted LIN receive message

        Parameters:
            msg a Lin receive message (TLINRcvMsg)

        Returns:
            a string formatted LIN message
        """
        # Check if the received frame is a standard type.
        # If it is not a standard type then ignore it.
        if (msg.Type != PLinApi.TLIN_MSGTYPE_STANDARD.value):
            if (msg.Type == PLinApi.TLIN_MSGTYPE_BUS_SLEEP.value):
                strTemp = 'Bus Sleep status message'
            elif (msg.Type == PLinApi.TLIN_MSGTYPE_BUS_WAKEUP.value):
                strTemp = 'Bus WakeUp status message'
            elif (msg.Type == PLinApi.TLIN_MSGTYPE_AUTOBAUDRATE_TIMEOUT.value):
                strTemp = 'Auto-baudrate Timeout status message'
            elif (msg.Type == PLinApi.TLIN_MSGTYPE_AUTOBAUDRATE_REPLY.value):
                strTemp = 'Auto-baudrate Reply status message'
            elif (msg.Type == PLinApi.TLIN_MSGTYPE_OVERRUN.value):
                strTemp = 'Bus Overrun status message'
            elif (msg.Type == PLinApi.TLIN_MSGTYPE_QUEUE_OVERRUN.value):
                strTemp = 'Queue Overrun status message'
            else:
                strTemp = 'Non standard message'
            return strTemp
        # format Data field as string
        dataStr = ""
        for i in range(msg.Length):
            dataStr = str.format("{0}{1} ", dataStr,  hex(msg.Data[i]))
        # remove ending space
        dataStr = dataStr[:-1]
        # format Error field as string
        error = ""
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_CHECKSUM):
            error = error + 'Checksum,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_GROUND_SHORT):
            error = error + 'GroundShort,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_ID_PARITY_BIT_0):
            error = error + 'IdParityBit0,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_ID_PARITY_BIT_1):
            error = error + 'IdParityBit1,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_INCONSISTENT_SYNCH):
            error = error + 'InconsistentSynch,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_OTHER_RESPONSE):
            error = error + 'OtherResponse,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_SLAVE_NOT_RESPONDING):
            error = error + 'SlaveNotResponding,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_SLOT_DELAY):
            error = error + 'SlotDelay,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_TIMEOUT):
            error = error + 'Timeout,'
        if (msg.ErrorFlags & PLinApi.TLIN_MSGERROR_VBAT_SHORT):
            error = error + 'VBatShort,'
        if (msg.ErrorFlags == 0):
            error = 'O.k. '
        # remove ending comma
        error = error[:-1]
        # format message
        return (str.format("{0}\t{1}\t{2}\n ...{3}\t{4}\t{5}",
                           hex(self.PIDs[msg.FrameId]),
                           msg.Length,
                           dataStr,
                           msg.TimeStamp,
                           self.getFrameDirectionAsString(msg.Direction),
                           error
                           ))

    # Updates a frame entry of the global frames table
    #
    def setFrameEntry(self, frameId, direction, checksumType, length):
        """
        Updates a frame entry of the global frames table.

        Parameters:
            frameId         frame ID to update (int)
            direction       a TLINDirection object
            checksumType    a TLINChecksumType object
            length          length (int)

        Returns:
            True if frame successfully updated, False otherwise
        """
        # initialize default result
        res = True
        # initialize and set Frame entry
        lFrameEntry = PLinApi.TLINFrameEntry()
        lFrameEntry.FrameId = c_ubyte(frameId)
        lFrameEntry.ChecksumType = checksumType
        lFrameEntry.Direction = direction
        lFrameEntry.Length = c_ubyte(length)
        lFrameEntry.Flags = PLinApi.FRAME_FLAG_RESPONSE_ENABLE
        linResult = self.m_objPLinApi.SetFrameEntry(
            self.m_hClient, self.m_hHw, lFrameEntry)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            # update filter mask to match the new status of the frame
            lMask = c_uint64(1 << frameId)
            self.m_lMask = c_uint64(self.m_lMask.value | lMask.value)
            linResult = self.m_objPLinApi.SetClientFilter(
                self.m_hClient, self.m_hHw, self.m_lMask)
        if (linResult != PLinApi.TLIN_ERROR_OK):
            self.displayError(linResult)
            res = False
        return res

    # Displays a list of available LIN hardwares
    #
    def displayAvailableConnection(self):
        """
        Displays a list of available LIN hardwares.
        
        Returns:
            None
        """
        # retrieve a list of available hardware
        hWInfoList = self.getAvailableHardware()
        # display result
        print(" List of available LIN hardware:")
        if (len(hWInfoList) == 0):
                print("\t<No hardware found>")
        else:
            # for each hardware display it's type, device and channel
            for hWInfo in hWInfoList:
                if (self.m_hHw.value == hWInfo[3]):
                    if (self.m_HwMode.value == PLinApi.TLIN_HARDWAREMODE_MASTER.value):
                        hwType = "master"
                    else:
                        hwType = "slave"
                    # add information if the application is connecter to the
                    # hardware
                    isConnected = str.format(
                        "(connected as {0}, {1})", hwType, self.m_HwBaudrate.value)
                else:
                    isConnected = ""
                print(str.format('\t{3}) {0} - dev. {1}, chan. {2} {4}', hWInfo[
                      0], hWInfo[1], hWInfo[2], hWInfo[3], isConnected))

    # Displays LIN Global Frames table
    #
    def displayGlobalFramesTable(self, showDisabled=True):
        """
        Displays LIN Global Frames table

        Parameters:
            showDisabled defines if disabled frame should be included (boolean)

        Returns:
            None
        """
        # get global frames table
        pGFT = self.readFrameTableFromHw()
        # print results
        print("\n * Global Frames Table:\n")
        print(" ID\tPID\tDirection\t\tLength\tCST")
        print(" ------------------------------------------------------------")
        nPID = c_ubyte(0)
        # output each frame
        for frame in pGFT:
            nPID = c_ubyte(frame.FrameId)
            self.m_objPLinApi.GetPID(nPID)
            # if showDisabled is false, check wether the frame is enabled
            if (frame.Direction != PLinApi.TLIN_DIRECTION_DISABLED.value or
                    (frame.Direction == PLinApi.TLIN_DIRECTION_DISABLED.value
                     and showDisabled)
                ):
                print(str.format(" {0}\t{1}\t{2}\t\t{3}\t{4}\t",
                                 hex(frame.FrameId),
                                 hex(nPID.value),
                                 self.getFrameDirectionAsString(
                                     frame.Direction),
                                 frame.Length,
                                 self.getFrameCSTAsString(
                                     frame.ChecksumType)
                                 ))

    # Prints a default header menu
    #
    def displayMenuHeader(self):
        """
        Prints a default menu header

        Returns:
            None
        """
        print("\n\nPLin-API Console:")

    # Prints a default choice menu to quit
    #
    def displayMenuExit(self, text="q) Exit menu"):
        """
        Prints a default menu header

        Returns:
            None
        """
        print(str.format("\t{0}", text))

    # Requests input from user
    #
    def displayMenuInput(self, text="Select an action: "):
        """
        Requests input from user

        Paramaters:
            text    custom text to display to the user

        Returns:
            User input as string (to upper)
        """
        # get user input
        if sys.version_info > (3, 0):
            choice = input(str.format("\n * {0}", text))
        else:
            choice = raw_input(str.format("\n * {0}", text))
        # return the response as a upper string
        choice = str(choice).upper()
        print("\n")
        return choice

    # Prints a notification
    #
    def displayNotification(self, text="** Invalid choice **", waitSeconds=0.5):
        """
        Prints a notification

        Parameters:
            text    custom text to display to the user
            waitSeconds number of seconds to wait after displaying notification            

        Returns:
            None
        """
        print(str.format("\t{0}", text))
        time.sleep(waitSeconds)

    # Prints a LIN error  message
    #
    def displayError(self, linError, isInput=True, waitTime=0):
        """
        Prints a LIN error message

        Parameters:
            linError    a LIN Error (TLinError)

        Results:
            None
        """
        # get error string from code
        pTextBuff = create_string_buffer(255)
        linResult = self.m_objPLinApi.GetErrorText(
            linError, 0x09, pTextBuff, 255)
        # display error message
        if (linResult == PLinApi.TLIN_ERROR_OK and len(pTextBuff.value) != 0):
            self.displayNotification(
                str.format("** Error ** - {0} ", bytes.decode(pTextBuff.value)), waitTime)
        else:
            self.displayNotification(
                str.format("** Error ** - code={0}", linError), waitTime)
        if (isInput):
            # wait for user approval
            self.displayMenuInput("** Press <enter> to continue **")

    # Clears the terminal
    #
    def clearMenu(self):
        """
        Clears the terminal (independantly of the OS)
        
        Returns:
            None
        """
        os.system('cls' if os.name == 'nt' else 'clear')

# *****************************************************************
# Main script
# *****************************************************************

# Main entry point


def main():
    print('PLIN-API Python Console Example...\n')
    try:
        app = PLinApiConsole()
        app.run()
        app.uninitialize()
    except Exception as e:
        print("\n\n*** Error ***\n\n\t")
        if (hasattr(e, 'message')):
            print(e.message)
            print("\n\n")
        traceback.print_exc(file=sys.stdout)
        print("\n\nPress Enter to quit...")
        if sys.version_info > (3, 0):
            input("$>")
        else:
            raw_input()
    return 0

# calling main entry point
sys.exit(main())
