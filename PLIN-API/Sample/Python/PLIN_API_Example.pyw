# *****************************************************************
#  PLIN-API Example
#
#  ~~~~~~~~~~~~
#
#  ------------------------------------------------------------------
#  Author : Fabrice Vergnaud
#  Language: Python 2.7
#  ------------------------------------------------------------------
#
#  Copyright (C) 1999-2013  PEAK-System Technik GmbH, Darmstadt
# *****************************************************************

import sys
import traceback
import time
import collections
import threading
from ctypes import *

# Imports for UI
from Tkinter import *          # TK UI libraryre
import Tix                     # TK extensions library
import tkMessageBox            # Simple-Messages library
import traceback               # Error-Tracing library
import string                  # String functions
import tkFont                  # Font-Management library
import tkSimpleDialog          # Dialog library

# PLin-API library import
import PLinApi

TCL_DONT_WAIT = 1 << 1
TCL_WINDOW_EVENTS = 1 << 2
TCL_FILE_EVENTS = 1 << 3
TCL_TIMER_EVENTS = 1 << 4
TCL_IDLE_EVENTS = 1 << 5
TCL_ALL_EVENTS = 0

# *****************************************************************
# LIN Frame Definition class
# *****************************************************************


class CFrameDefinition:

    """
    Represents the global Definition of a LIN-Frame.

    Attributes:
        m_pParent           Parent object holding the Frame definiton
        m_nID               LIN-Frame Identifier (int)
        m_nProtectedID      the protected LIN-Frame m_nID  (int)
        m_nLength           Data length of the LIN-Frame
        m_nChecksumType     Type of the Checksum Calculation (int, value field of TLINChecksumType)
        m_bDirection        The direction of the LIN-Frame (int, value field of TLINChecksumType)
    """

    # define frame property type
    PROP_CST = 1
    PROP_DIRECTION = 2
    PROP_LENGTH = 3

    # Constructor
    #
    def __init(self, pParent, id, pid, len, checksumType, direction):
        """
        Creates a CFrameDefinition object.

        Parameters:
            pParent         CGlobalFrameTable holding this frame
                    id				LIN-Frame ID (c_ubyte)
                    pid 			LIN-Frame Protected ID (c_ubyte)
                    len             LIN-Frame length (int)
                    checksumType    LIN-Frame checksum type (TLINChecksumType)
                    direction       LIN-Frame direction (TLINDirection)

        Returns:
            None
        """
        self.m_pParent = pParent
        self.m_nID = id
        self.m_nProtectedID = pid
        self.m_nLength = len
        self.m_nChecksumType = checksumType
        self.m_bDirection = direction

    # Updates the checksumType of the frame entry corresponding to the frame definition
    #
    def setChecksumType(self, checksumType):
        """
        Updates the checksumType of the frame entry corresponding to the frame definition.
        FrameDefinition value is set only if frame entry update was valid

        Parameters:
            checksumType new value as int or TLinChecksumType

        Returns:
            None
        """
        if (isinstance(checksumType, PLinApi.TLINChecksumType)):
            value = checksumType.value
        else:
            value = int(checksumType)
        if (self.m_nChecksumType != value):
            allowed = self.m_pParent.callPropertyChangeHandler(
                self, self.PROP_CST, value)
            if (allowed):
                self.m_nChecksumType = value

    # Updates the direction of the frame entry corresponding to the frame definition
    #
    def setDirection(self, direction):
        """
        Updates the direction of the frame entry corresponding to the frame definition.
        FrameDefinition value is set only if frame entry update was valid

        Parameters:
            direction new value as int or TLINDirection

        Returns:
            None
        """
        if (isinstance(direction, PLinApi.TLINDirection)):
            value = direction.value
        else:
            value = int(direction)
        if (self.m_bDirection != value):
            allowed = self.m_pParent.callPropertyChangeHandler(
                self, self.PROP_DIRECTION, value)
            if (allowed):
                self.m_bDirection = value

    # Updates the length of the frame entry corresponding to the frame definition
    #
    def setLength(self, length):
        """
        Updates the length of the frame entry corresponding to the frame definition.
        FrameDefinition value is set only if frame entry update was valid

        Parameters:
            length new value as int or c_ubyte

        Returns:
            None
        """
        if (isinstance(length, c_ubyte)):
            value = length.value
        else:
            value = int(length)
        if (self.m_nLength != value):
            allowed = self.m_pParent.callPropertyChangeHandler(
                self, self.PROP_LENGTH, value)
            if (allowed):
                self.m_nLength = value

    # Returns the string name of a PLinApi.TLINDirection value
    #
    @staticmethod
    def getFrameDirectionAsString(direction):
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
    @staticmethod
    def getFrameCSTAsString(checksumType):
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

# *****************************************************************
# LIN Global Frame Table class
# *****************************************************************


class CGlobalFrameTable:

    """
    A class that holds the LIN Global Frames table, that contains
    a list of 64 CFrameDefinition objects.

    Attributes:
        _objPLinApi         a private PLinApi object
        _hClient            a private HLINCLIENT object
        _hHw                a private HLINHW object
        _hwMode             a private TLINHardwareMode object
        m_lFrameDefinitions Collection of all Frame Definition
    """

    # Constructor
    #
    def __init__(self, objPLinApi, hClient, hHw, hwMode, pMask):
        """
        Constructor

        Parameters:
            objPLinApi  a PLinApi object
            hClient     a HLINCLIENT object
            hHw         a HLINHW object
            hwMode      a TLINHardwareMode object
            pMask       a c_uint64 object
        """
        self._hClient = hClient
        self._objPLinApi = objPLinApi
        self.updateLinInfo(hClient, hHw, hwMode, pMask)
        self.m_lFrameDefinitions = []
        for i in range(64):
            # Before a frame entry can be read from the
            # hardware, the Frame-ID of the wanted entry
            # must be set
            lFrameEntry = PLinApi.TLINFrameEntry()
            lFrameEntry.FrameId = c_ubyte(i)
            lFrameEntry.ChecksumType = PLinApi.TLIN_CHECKSUMTYPE_AUTO
            lFrameEntry.Direction = PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH
            # length values is set to LIN 1.2.
            # From Frame-ID 0 to 31 set the length 2.
            if ((i >= 0x00) and (i <= 0x1F)):
                self._addFrameDefinition(
                    i, 2, PLinApi.TLIN_CHECKSUMTYPE_AUTO, PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH)
            # From Frame-ID 32 to 47 set the length 4.
            elif ((i >= 0x20) and (i <= 0x2F)):
                self._addFrameDefinition(
                    i, 4, PLinApi.TLIN_CHECKSUMTYPE_AUTO, PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH)
            # From Frame-ID 48 to 63 set the length 8.
            elif ((i >= 0x30) and (i <= 0x3F)):
                self._addFrameDefinition(
                    i, 8, PLinApi.TLIN_CHECKSUMTYPE_AUTO, PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH)

    # Updates LIN information
    #
    def updateLinInfo(self, hClient, hHw, hwMode, pMask):
        """
        Updates LIN information

        Parameters:
            hClient     a HLINCLIENT object
            hHw         a HLINHW object
            hwMode      a TLINHardwareMode object
            pMask       a c_uint64 object
        """
        self._hClient = hClient
        self._hHw = hHw
        self._hwMode = hwMode
        self._pMask = pMask

    # Handles frame definition's property changes
    #
    def callPropertyChangeHandler(self, frame, propertyType, value):
        """
        Updates FrameEntry property and returns true
        if the new value is valid and sucessfully changed

        Remarks:
            Allowed                                     Parameter
            values in value:                            propertyType:
            -----------------                           ----------
            int, value of a TLINDirection object        PROP_DIRECTION
            int, value of a TLINChecksumType object     PROP_CST
            int                                         PROP_LENGTH

        Parameters:
            frame   a CFrameDefinition object
            propertyType    a property type (see CFrameDefinition.PROP_*)
            value   the new value for the property
            allowed
        """
        if (not isinstance(frame, CFrameDefinition)):
            return False
        # initialization
        lFrameEntry = PLinApi.TLINFrameEntry()
        # the returned value
        allowed = True
        # If data length is to be set, check the value.
        if (propertyType == CFrameDefinition.PROP_LENGTH):
            allowed = (value >= 0 and value <= 8)
        # If DO NOT allow then return.
        if (not allowed):
            return allowed
        # Only if the hardware was initialized as Slave
        # set the direction of the LIN-Frame.
        # By the Master the direction will be used with
        # the LIN_Write and do not have to set here.
        if (self._hwMode.value == PLinApi.TLIN_HARDWAREMODE_SLAVE.value):
            # Temporary set Allowed to false is to easy
            # return only later.
            allowed = False
            # Set the Frame-ID of the Frame to get and set.
            # The ID have to set before get the entry.
            lFrameEntry.FrameId = c_ubyte(frame.m_nID)
            # Get the Frame Entry with the Frame-ID from
            # the Hardware via the LinApi.
            lErr = self._objPLinApi.GetFrameEntry(self._hHw, lFrameEntry)
            # If an error occurs do not allow to change
            # the property and return.
            # The Allowed parameter was set some lines before.
            if (lErr != PLinApi.TLIN_ERROR_OK):
                return allowed
            # Direction property should be set
            if propertyType == CFrameDefinition.PROP_DIRECTION:
                lFrameEntry.Direction = PLinApi.TLINDirection(value)
            # Length property should be set
            elif propertyType == CFrameDefinition.PROP_LENGTH:
                lFrameEntry.Length = c_ubyte(value)
            # ChecksumType property should be set
            elif propertyType == CFrameDefinition.PROP_CST:
                lFrameEntry.ChecksumType = PLinApi.TLINChecksumType(value)
            lFrameEntry.Flags = PLinApi.FRAME_FLAG_RESPONSE_ENABLE
            lErr = self._objPLinApi.SetFrameEntry(
                self._hClient, self._hHw, lFrameEntry)
            # If an error occurs do not allow to change
            # the property and return.
            # The Allowed parameter was set some lines before.
            if (lErr != PLinApi.TLIN_ERROR_OK):
                return allowed
            # Temporary set Allowed to true for next check.
            # The action was successfull on this line.
            allowed = True
        # If the property 'Direction' of one
        # CFrameDefinition will be changed,
        # here we need a special request to set
        # the client filter.
        if (propertyType == CFrameDefinition.PROP_DIRECTION):
            # If the new value for the property 'Direction'
            # should NOT be 'Disabled' check first if
            # the CFrameDefinition is defined already with some
            # other value then 'Disabled'.
            if (value != PLinApi.TLIN_DIRECTION_DISABLED.value):
                if (frame.m_bDirection == PLinApi.TLIN_DIRECTION_DISABLED.value):
                    # If the old property of CFrameDefintion
                    # was set to 'Disabled' the new value
                    # means that the Frame-ID have to add to
                    # the client filter by the LinApi.
                    # Set the client filter.
                    # The Filter is a bit mask.
                    lMask = 1 << frame.m_nID
                    self._pMask.value = (self._pMask.value | lMask)
                    lErr = self._objPLinApi.SetClientFilter(
                        self._hClient, self._hHw, self._pMask)
                    # Only allow to change the property if the Frame-ID
                    # was added successfull to the Filter.
                    allowed = (lErr == PLinApi.TLIN_ERROR_OK)
            else:
                # If the value of direction should set on 'disable'
                # Remove the Frame-ID from the client filter.
                lMask = 1 << frame.m_nID
                self._pMask.value = (self._pMask.value & ~lMask)
                lErr = self._objPLinApi.SetClientFilter(
                    self._hClient, self._hHw, self._pMask)
                # Only allow to change the property if the Frame-ID
                # was removed successfull from the Filter.
                allowed = (lErr == PLinApi.TLIN_ERROR_OK)
        return allowed

    # Adds a CFrameDefinition object to the end of the CGlobalFrameTable
    #
    def _addFrameDefinition(self, id, length, checksumType, direction):
        """
        Adds a CFrameDefinition object with the defined values to the end of the CGlobalFrameTable.

        Parameters:
            id              The Frame ID of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.
            length          The Frame Length of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.
            checksumType    The Frame Checksum Type of the CFrameDefinition object to be added to the end of the CGlobalFrameTable (TLINDirection).
            direction       The Frame Direction of the CFrameDefinition object to be added to the end of the CGlobalFrameTable (TLINChecksumType).
        Returns:
            None
        """
        # Check the Frame-ID for adding.
        # Only ID's from 0 to 63 are allowed to add.
        if (id < 0 or id > 63):
            # ID is invalid. Do not add it.
            return None
        else:
            # Create a Frame Definition object.
            # and assigned the delivered values to it.
            lFD = CFrameDefinition()
            lFD.m_pParent = self
            lFD.m_nID = id
            lFD.m_nLength = length
            lFD.m_nChecksumType = checksumType.value
            lFD.m_bDirection = direction.value
            # Calculate the Protected-ID with
            # the delivered Frame-ID.
            lbID = c_ubyte(id)
            self._objPLinApi.GetPID(lbID)
            # Assign the calculated Protected-ID.
            lFD.m_nProtectedID = lbID.value
            # Add the created object to the list.
            self.m_lFrameDefinitions.append(lFD)
        # Return the position of the new added object.
        # It should be added at the end so the position
        # must be the last entry in the list.
        return len(self.m_lFrameDefinitions) - 1

# *****************************************************************
# LIN Global Frames Table Dialog
# *****************************************************************


class CFrameDialog(tkSimpleDialog.Dialog):

    """
    A dialog to configure LIN Global Frames Table
    """

    # define listBox column indexes
    COL_ID = 0
    COL_PID = 1
    COL_DIR = 2
    COL_LEN = 3
    COL_CST = 4

    # Constructor
    #
    def __init__(self, parent, pGFT):
        """
        Constructor

        Parameters:
            parent  a Tix.Tk object
            pGFT    a LIN Global Frame Table object

        Returns:
            None
        """
        # initialization
        self.m_pGFT = pGFT
        # Boolean to register the current edit mode of different controls.
        # To protect the control's event for recursive calling.
        self.m_fAutomatic = False
        # list holding the reference of selected frames
        self.m_selectedFrames = []
        # initializes and shows the dialog
        tkSimpleDialog.Dialog.__init__(self, parent, "Global Frames Table")

    # Override parent method
    #
    def body(self, master):
        """
        See tkSimpleDialog.Dialog.body
        """
        # Control variables
        self.m_ListColCaption = ["ID", "|PID", "|Direction", "|Length", "|CST"]
        self.m_ListColSpace = [10, 10, 33, 6, 10]
        self.m_ListFont = tkFont.Font(family="Lucida Console", size="10")
        self.m_ListCaptionPadxSpaces = []
        # values are ordered so that the TLIN object value is the same as the
        # index
        self.m_linDirections = [PLinApi.TLIN_DIRECTION_DISABLED,
                                PLinApi.TLIN_DIRECTION_PUBLISHER,
                                PLinApi.TLIN_DIRECTION_SUBSCRIBER,
                                PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH]
        # values are ordered so that the TLIN object value is the same as the
        # index
        self.m_linCSTs = [PLinApi.TLIN_CHECKSUMTYPE_CUSTOM,
                          PLinApi.TLIN_CHECKSUMTYPE_CLASSIC,
                          PLinApi.TLIN_CHECKSUMTYPE_ENHANCED,
                          PLinApi.TLIN_CHECKSUMTYPE_AUTO]
        for colText, colWidth in zip(self.m_ListColCaption, self.m_ListColSpace):
            self.m_ListCaptionPadxSpaces.append(colWidth - len(colText))
        self.m_ListCaptionPadxSpaces[0] = self.m_ListCaptionPadxSpaces[0] - 1
        # dialog not resizable
        self.resizable(False, False)
        # Frame table group
        box = Frame(self)
        Label(box, anchor=W, text="Global Frame Table:").grid(row=0, sticky=W)
        # scrollbar for the frame table listbox
        self.yLvGFTScroll = Scrollbar(box, orient=VERTICAL)
        self.yLvGFTScroll.grid(
            row=1, column=1, rowspan=5, padx=3, sticky=N + S)
        # label as header for listBox
        tempString = ""
        for caption, spaces in zip(self.m_ListColCaption, self.m_ListCaptionPadxSpaces):
            tempString = tempString + "{0}{1}".format(caption, " " * spaces)
        Label(box, anchor=W, text=tempString, bg="#E2E2E3", fg="#000000",
              font=self.m_ListFont, relief=GROOVE).grid(row=1, column=0, padx=5, sticky="nwes")
        # message listbox
        self.lvGFT = Tix.TList(
            box, relief=GROOVE, height=25, orient="horizontal",
            yscrollcommand=self.yLvGFTScroll.set, itemtype="text",
            font=self.m_ListFont, browsecmd=self.lvGFT_SelectionChanged,
            selectmode='extended')
        self.lvGFT.grid(row=2, column=0, rowspan=4, padx=5, sticky="NSWE")
        # add support for select all shortcut : 'Ctrl+a'
        self.lvGFT.bind('<Control-a>', self.lvGFT_SelectAll)
        # associate scrollbar to listbox
        self.yLvGFTScroll['command'] = self.lvGFT.yview
        # Properties group
        Label(box, anchor=W, text="Properties:").grid(
            row=0, column=3, sticky=W)
        propertyBox = LabelFrame(box, text="")
        propertyBox.grid_propagate(1)
        propertyBox.grid(row=1, column=3, rowspan=5, sticky=N + W)
        # property : checksum type
        Label(propertyBox, anchor=W, text="Checksum Type:").grid(
            row=1, column=3, sticky=W)
        self.cbbCST = Tix.ComboBox(
            propertyBox, command=self.cbbCST_SelectedIndexChanged)
        self.cbbCST.subwidget('entry')['width'] = 15
        self.cbbCST.subwidget('listbox')['width'] = 15
        self.cbbCST.grid(row=1, column=4, sticky=W)
        for value in self.m_linCSTs:
            self.cbbCST.insert(
                Tix.END, CFrameDefinition.getFrameCSTAsString(value))
        # property : frame direction
        Label(propertyBox, anchor=W, text="Direction:").grid(
            row=2, column=3, sticky=W)
        self.cbbDirection = Tix.ComboBox(
            propertyBox, command=self.cbbDirection_SelectedIndexChanged)
        self.cbbDirection.subwidget('entry')['width'] = 15
        self.cbbDirection.subwidget('listbox')['width'] = 15
        self.cbbDirection.grid(row=2, column=4, sticky=W)
        for value in self.m_linDirections:
            self.cbbDirection.insert(
                Tix.END, CFrameDefinition.getFrameDirectionAsString(value))
        # property : length
        Label(propertyBox, anchor=W, text="Length:").grid(
            row=3, column=3, sticky=W)
        self.cbbLength = Tix.ComboBox(
            propertyBox, command=self.cbbLength_SelectedIndexChanged)
        self.cbbLength.subwidget('entry')['width'] = 15
        self.cbbLength.subwidget('listbox')['width'] = 4
        self.cbbLength.grid(row=3, column=4, sticky=W)
        for value in range(0, 9):
            self.cbbLength.insert(Tix.END, str(value))
        # property readonly : ID
        self.strID = StringVar(value="")
        Label(propertyBox, anchor=W, text="ID:").grid(
            row=4, column=3, sticky=W)
        self.txtID = Entry(
            propertyBox, width=13, state='readonly', textvariable=self.strID)
        self.txtID.grid(row=4, column=4, padx=5, pady=0, sticky=W)
        # property readonly : PID
        self.strPID = StringVar(value="")
        Label(propertyBox, anchor=W, text="PID:").grid(
            row=5, column=3, sticky=W)
        self.txtPID = Entry(
            propertyBox, width=13, state='readonly', textvariable=self.strPID)
        self.txtPID.grid(row=5, column=4, padx=5, pady=0, sticky=W)
        # populates controls
        self.fillListView()
        box.pack()
        # return the widget that should have focus
        return self.lvGFT

    # Override parent method
    #
    def buttonbox(self):
        """
        See tkSimpleDialog.Dialog.buttonbox
        """
        # override and remove OK/Cancel buttons
        pass

# *****************************************************************
# Listbox lvGFT functions and handlers
# *****************************************************************

    # Fills the global frame table listView
    #
    def fillListView(self):
        """
        Fills the global frame table listView

        Returns:
            None
        """
        # add each frame from the global frame table
        for pFrame in self.m_pGFT.m_lFrameDefinitions:
            self.lvGFT.insert(END, text=self.formatFrameDefinition(pFrame))

    # Updates selected rows with data from corresponding frames
    #
    def updateListView(self, frame):
        """
        Updates selected rows with data from corresponding frames.

        Parameters:
            frame the CFrameDefinition frame that corresponds to the row that needs to be updated
        Returns:
            None
        """
        # update the specified frame in the list view
        self.lvGFT.delete(frame.m_nID)
        self.lvGFT.insert(frame.m_nID, text=self.formatFrameDefinition(frame))
        self.lvGFT.selection_set(frame.m_nID)

    # Formats a frame definition to a string suitable for the frame listView
    #
    def formatFrameDefinition(self, pFrame):
        """
        Formats a frame definition to a string suitable for the frame listView

        Parameters:
            pFrame a CFrameDefinition object

        Returns:
            the frame definition as a string suited for the frame listView
        """
        # reminder: row is formatted as such ["ID", "|PID", "|Direction",
        # "|Length", "|CST"]
        result = ''
        strTemp = hex(pFrame.m_nID)
        result = result + \
            (strTemp + " " * (self.m_ListColSpace[self.COL_ID] - len(strTemp)))
        strTemp = hex(pFrame.m_nProtectedID)
        result = result + \
            (strTemp + " " *
             (self.m_ListColSpace[self.COL_PID] - len(strTemp)))
        strTemp = CFrameDefinition.getFrameDirectionAsString(
            pFrame.m_bDirection)
        result = result + \
            (strTemp + " " *
             (self.m_ListColSpace[self.COL_DIR] - len(strTemp)))
        strTemp = str(pFrame.m_nLength)
        result = result + \
            (strTemp + " " *
             (self.m_ListColSpace[self.COL_LEN] - len(strTemp)))
        strTemp = CFrameDefinition.getFrameCSTAsString(pFrame.m_nChecksumType)
        result = result + \
            (strTemp + " " *
             (self.m_ListColSpace[self.COL_DIR] - len(strTemp)))
        return result

    # listView selection handler
    #
    def lvGFT_SelectionChanged(self, currentValue):
        """
        Selection handler for the lvGFT listView

        Parameters:
            currentValue    last selected row index

        Returns:
            None
        """
        if (not self.m_fAutomatic):
            self.m_fAutomatic = True
            # clear selected frames
            self.m_selectedFrames = []
            # get selected items
            items = self.lvGFT.info_selection()
            if (len(items) == 1):
                self.m_selectedFrames.append(
                    self.m_pGFT.m_lFrameDefinitions[int(items[0])])
            elif (len(items) > 1):
                for i in items:
                    self.m_selectedFrames.append(
                        self.m_pGFT.m_lFrameDefinitions[int(i)])
            # Update Property controls with the newly selected frames
            self.updatePropertyGrid()
            self.m_fAutomatic = False

    # Selects all row from lvGFT listBox
    #
    def lvGFT_SelectAll(self, event):
        """
        Selects all row from lvGFT listBox.

        Returns:
            None
        """
        self.lvGFT.selection_set(0, END)
        self.lvGFT_SelectionChanged(0)

# *****************************************************************
# Properties controls functions and handlers
# *****************************************************************

    # Updates the property grid with the selected frames from m_selectedFrames
    #
    def updatePropertyGrid(self):
        """
        Updates the property grid with the selected frames from m_selectedFrames.

        Returns:
            None
        """
        nbFrames = len(self.m_selectedFrames)
        if (nbFrames == 0):
            # clear properties
            self.cbbCST['selection'] = ''
            self.cbbDirection['selection'] = ''
            self.cbbLength['selection'] = ''
            self.strID.set('')
            self.strPID.set('')
        elif (nbFrames == 1):
            frame = self.m_selectedFrames[0]
            self.cbbCST.pick(frame.m_nChecksumType)
            self.cbbDirection.pick(frame.m_bDirection)
            self.cbbLength.pick(frame.m_nLength)
            self.strID.set(hex(frame.m_nID))
            self.strPID.set(hex(frame.m_nProtectedID))
        else:
            # get the first selected frame
            frame = self.m_selectedFrames[0]
            direction = frame.m_bDirection
            checksumType = frame.m_nChecksumType
            length = frame.m_nLength
            # We have to loop through all selected frames to
            # search for identical properties in the selected frames.
            # If values for a property are different a blank value is displayed
            bSameChecksumType = True
            bSameDirection = True
            bSameLength = True
            for frame in self.m_selectedFrames:
                if (checksumType != frame.m_nChecksumType):
                    bSameChecksumType = False
                if (direction != frame.m_bDirection):
                    bSameDirection = False
                if (length != frame.m_nLength):
                    bSameLength = False
            # if all the frames have the same CST, display it otherwise reset
            # to the original blank value
            if (bSameChecksumType):
                self.cbbCST.pick(checksumType)
            else:
                self.cbbCST['selection'] = ''
            # if all the frames have the same direction, display it otherwise
            # reset to the original blank value
            if (bSameDirection):
                self.cbbDirection.pick(direction)
            else:
                self.cbbDirection['selection'] = ''
            # if all the frames have the same length, display it otherwise
            # reset to the original blank value
            if (bSameLength):
                self.cbbLength.pick(length)
            else:
                self.cbbLength['selection'] = ''
            # those properties are always different, reset to the original
            # blank value
            self.strID.set('')
            self.strPID.set('')

    # Combobox cbbCST handler
    #
    def cbbCST_SelectedIndexChanged(self, currentValue):
        """
        Combobox cbbCST handler

        Parameters:
            currentValue selected string value

        Returns:
            None
        """
        # assert change was not called by selection in lvGFT listBox
        if (self.m_fAutomatic):
            return
        # get combobox value
        lCST = PLinApi.TLIN_CHECKSUMTYPE_CUSTOM
        if (currentValue == CFrameDefinition.getFrameCSTAsString(PLinApi.TLIN_CHECKSUMTYPE_AUTO)):
            lCST = PLinApi.TLIN_CHECKSUMTYPE_AUTO
        elif (currentValue == CFrameDefinition.getFrameCSTAsString(PLinApi.TLIN_CHECKSUMTYPE_CLASSIC)):
            lCST = PLinApi.TLIN_CHECKSUMTYPE_CLASSIC
        elif (currentValue == CFrameDefinition.getFrameCSTAsString(PLinApi.TLIN_CHECKSUMTYPE_ENHANCED)):
            lCST = PLinApi.TLIN_CHECKSUMTYPE_ENHANCED
        elif (currentValue == CFrameDefinition.getFrameCSTAsString(PLinApi.TLIN_CHECKSUMTYPE_CUSTOM)):
            lCST = PLinApi.TLIN_CHECKSUMTYPE_CUSTOM
        # set each selected frame with the new value
        for frame in self.m_selectedFrames:
            frame.setChecksumType(lCST)
            # if changes were not validated, rollback/update display
            if (frame.m_nChecksumType != lCST.value):
                self.updatePropertyGrid()
            # update lvGFT listView
            self.updateListView(frame)

    # Combobox cbbDirection handler
    #
    def cbbDirection_SelectedIndexChanged(self, currentValue):
        """
        Combobox cbbDirection handler

        Parameters:
            currentValue selected string value

        Returns:
            None
        """
        # assert change was not called by selection in lvGFT listBox
        if (self.m_fAutomatic):
            return
        # get combobox value
        lDirection = PLinApi.TLIN_DIRECTION_DISABLED
        if (currentValue == CFrameDefinition.getFrameDirectionAsString(PLinApi.TLIN_DIRECTION_DISABLED)):
            lDirection = PLinApi.TLIN_DIRECTION_DISABLED
        elif (currentValue == CFrameDefinition.getFrameDirectionAsString(PLinApi.TLIN_DIRECTION_PUBLISHER)):
            lDirection = PLinApi.TLIN_DIRECTION_PUBLISHER
        elif (currentValue == CFrameDefinition.getFrameDirectionAsString(PLinApi.TLIN_DIRECTION_SUBSCRIBER)):
            lDirection = PLinApi.TLIN_DIRECTION_SUBSCRIBER
        elif (currentValue == CFrameDefinition.getFrameDirectionAsString(PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH)):
            lDirection = PLinApi.TLIN_DIRECTION_SUBSCRIBER_AUTOLENGTH
        # set each selected frame with the new value
        for frame in self.m_selectedFrames:
            frame.setDirection(lDirection)
            # if changes were not validated, rollback/update display
            if (frame.m_bDirection != lDirection.value):
                self.updatePropertyGrid()
            # update lvGFT listView
            self.updateListView(frame)

    # Combobox cbbLength handler
    #
    def cbbLength_SelectedIndexChanged(self, currentValue):
        """
        Combobox cbbLength handler

        Parameters:
            currentValue selected string value

        Returns:
            None
        """
        # assert change was not called by selection in lvGFT listBox
        if (self.m_fAutomatic):
            return
        # get combobox value
        lLength = int(currentValue)
        # set each selected frame with the new value
        for frame in self.m_selectedFrames:
            frame.setLength(lLength)
            # if changes were not validated, rollback/update display
            if (frame.m_nLength != lLength):
                self.updatePropertyGrid()
            # update lvGFT listView
            self.updateListView(frame)

# *****************************************************************
# Timer class
# *****************************************************************


class TimerRepeater():

    """
    A simple timer implementation that repeats itself
    """

    # Constructor
    #
    def __init__(self, name, interval, target, args=[], kwargs={}):
        """
        Creates a timer.

        Parameters:
            name        name of the thread
            interval    interval in second between execution of target
            target      function that is called every 'interval' seconds
            args        non keyword-argument list for target function
            kwargs      keyword-argument list for target function
        """
        # define thread and stopping thread event
        self._name = name
        self._thread = None
        self._event = None
        # initialize target and its arguments
        self._target = target
        self._args = args
        self._kwargs = kwargs
        # initialize timer
        self._interval = interval
        self._bStarted = False

    # Runs the thread that emulates the timer
    #
    def _run(self):
        """
        Runs the thread that emulates the timer.

        Returns:
            None
        """
        while not self._event.wait(self._interval):
            # print "thread '"+self._name+"' execute."
            self._target(*self._args, **self._kwargs)
        # print "thread '"+self._name+"' stop."

    # Starts the timer
    #
    def start(self):
        """
        Starts the timer

        Returns:
            None
        """
        # avoid multiple start calls
        if (self._thread == None):
            self._event = threading.Event()
            self._thread = threading.Thread(None, self._run, self._name)
            self._thread.start()

    # Stops the timer
    #
    def stop(self):
        """
        Stops the timer

        Returns:
            None
        """
        if (self._thread != None):
            self._event.set()
            self._thread = None

# *****************************************************************
# LIN Frame Definition class
# *****************************************************************


class MessageStatus:

    """
    Message Status structure used to show LIN Messages in a ListBox.

    Attributes:
        m_Msg           The received LIN message (TLinRcvMsg)
        m_oldTimeStamp  Timestamp of a previously received message (c_uint64)
        m_iIndex        index of the message in the ListView component (int)
        m_Count         Number of LIN message received with the same frame ID (int)
        m_bShowPeriod   Defines if the timestamp is displayed as a period (boolean)
        m_bWasChanged   Defines if the message has been modified
                        and its display needs to be updated (boolean)
    """
    # Constructor
    #

    def __init__(self, linMsg, listIndex):
        """
        Creates a new MessageStatus object

        Parameters:
            linMsg      received LIN message (TLinRcvMsg)
            listIndex   index of the message in the ListView
        """
        self.m_Msg = linMsg
        self.m_oldTimeStamp = linMsg.TimeStamp
        self.m_iIndex = listIndex
        self.m_Count = 1
        self.m_bShowPeriod = True
        self.m_bWasChanged = False

    # Updates an existing MessageStatus with a new LIN message
    #
    def update(self, linMsg):
        """
        Updates an existing MessageStatus with a new LIN message.

        Parameters:
            linMsg LIN message to update (TLINRcvMsg)
        """
        self.m_oldTimeStamp = self.m_Msg.TimeStamp
        self.m_Msg = linMsg
        self.m_Count += 1
        self.m_bWasChanged = True

    # States wether the timestamp is displayed as a period or not
    #
    def setShowPeriod(self, value):
        """
        States wether the timestamp is displayed as a period or not

        Parameters:
            value True if the period should be displayed, False for the timestamp
        """
        if (self.m_bShowPeriod ^ value):
            self.m_bShowPeriod = value
            self.m_bWasChanged = True

    # Returns the timestamp or the period of the frame
    #
    def getTimeString(self):
        """
        Returns the timestamp or the period of the frame.

        Returns:
            timestamp or period in milliseconds
        """
        time = self.m_Msg.TimeStamp
        if (self.m_bShowPeriod):
            time = (time - self.m_oldTimeStamp) / 1000
        return str(time)

    # Formats Data field as string
    #
    def getDataString(self):
        """
        Formats Data field as string

        Returns:
            None
        """
        dataStr = ""
        for i in range(self.m_Msg.Length):
            # data item to hex
            data = hex(self.m_Msg.Data[i])
            # remove first '0b' character and pad with '0'
            data = data[2:].zfill(2)
            # format
            dataStr = str.format("{0}{1} ", dataStr, data)

        # remove ending space
        dataStr = dataStr[:-1]
        return dataStr

    # Formats LIN message (m_Msg) Error field as string
    #
    def getErrorString(self):
        """
        Formats LIN message (m_Msg) Error field as string.

        Returns:
            Error field as string
        """
        error = ""
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_CHECKSUM):
            error = error + 'Checksum,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_GROUND_SHORT):
            error = error + 'GroundShort,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_ID_PARITY_BIT_0):
            error = error + 'IdParityBit0,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_ID_PARITY_BIT_1):
            error = error + 'IdParityBit1,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_INCONSISTENT_SYNCH):
            error = error + 'InconsistentSynch,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_OTHER_RESPONSE):
            error = error + 'OtherResponse,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_SLAVE_NOT_RESPONDING):
            error = error + 'SlaveNotResponding,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_SLOT_DELAY):
            error = error + 'SlotDelay,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_TIMEOUT):
            error = error + 'Timeout,'
        if (self.m_Msg.ErrorFlags & PLinApi.TLIN_MSGERROR_VBAT_SHORT):
            error = error + 'VBatShort,'
        if (self.m_Msg.ErrorFlags == 0):
            error = 'O.k. '
        # remove ending comma
        error = error[:-1]
        return error

# *****************************************************************
# PLin-API Example app
# *****************************************************************


class PLinApiExample:

    """
    PLin-API Example - main window application
    """

    # define listBox column indexes
    COL_ID = 0
    COL_LEN = 1
    COL_DATA = 2
    COL_COUNT = 3
    COL_TIME = 4
    COL_DIR = 5
    COL_ERR = 6

    # Constructor
    #
    def __init__(self, parent):
        """
        Constructor

        Parameters:
            parent  a Tix.Tk object

        Returns:
            None
        """
        # Parent's configuration
        self.m_Parent = parent
        self.m_Parent.wm_title("PLIN-API Example")
        self.m_Parent.resizable(False, False)
        self.m_Parent.protocol("WM_DELETE_WINDOW", self.Form_OnClosing)
        # Frame's configuration
        self.m_Frame = Frame(self.m_Parent)
        self.m_Frame.grid(row=0, column=0, padx=5, pady=2, sticky="nwes")
        # GUI configuration
        self.initializeBasicComponents()
        self.centerTheWindow()
        self.initializeWidgets()
        # initial startup initialization
        self.refreshHardware()
        self.setConnectionStatus(False)

    # Destructor
    #
    def destroy(self):
        """
        Destructor.

        Returns:
            None
        """
        self.m_Parent.destroy()

    # Message loop
    #
    def loop(self):
        """
        Message loop.

        Returns:
            None
        """
        # This is an explict replacement for _tkinter mainloop()
        # It lets catch keyboard interrupts easier, and avoids
        # the 20 msec. dead sleep() which burns a constant CPU.
        while self.exit < 0:
            # There are 2 whiles here. The outer one lets you continue
            # after a ^C interrupt.
            try:
                # This is the replacement for _tkinter mainloop()
                # It blocks waiting for the next Tcl event using select.
                while self.exit < 0:
                    # prevent UI concurrency errors with timers (read and
                    # display)
                    with self._lock:
                        self.m_Parent.tk.dooneevent(TCL_ALL_EVENTS)
            except SystemExit:
                # Tkinter uses SystemExit to exit
                # print 'Exit'
                self.exit = 1
                return
            except KeyboardInterrupt:
                if tkMessageBox.askquestion('Interrupt', 'Really Quit?') == 'yes':
                    # self.tk.eval('exit')
                    self.exit = 1
                    return
                continue
            except:
                # Otherwise it's some other error
                t, v, tb = sys.exc_info()
                text = ""
                for line in traceback.format_exception(t, v, tb):
                    text += line + '\n'
                try:
                    tkMessageBox.showerror('Error', text)
                except:
                    pass
                self.exit = 1
                raise SystemExit, 1

# *****************************************************************
# GUI Help functions
# *****************************************************************

    # Initializes app members
    #
    def initializeBasicComponents(self):
        """
        Initializes app members.

        Returns:
            None
        """
        self.m_Width = 760
        self.m_Height = 600
        self.exit = -1
        # load PLIN-API
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
            "PLIN-API Example", None, self.m_hClient)
        # LIN GUI variables
        # sorted dictionnary of available hardware
        d = {'<No available harware>': PLinApi.HLINHW(0)}
        self.m_dataHws = collections.OrderedDict(
            sorted(d.items(), key=lambda t: t[0]))
        # sorted dictionnary of hardware baudrates
        d = {'2400': 2400, '9600': 9600, '10400': 10400, '19200': 19200}
        self.m_dataHwBaudrates = collections.OrderedDict(
            sorted(d.items(), key=lambda t: t[1]))
        # sorted dictionnary of hardware mode
        d = {'Master': PLinApi.TLIN_HARDWAREMODE_MASTER,
             'Slave': PLinApi.TLIN_HARDWAREMODE_SLAVE}
        self.m_dataHwModes = collections.OrderedDict(
            sorted(d.items(), key=lambda t: t[0]))
        # LIN Global Frame Table
        self.m_pGFT = CGlobalFrameTable(
            self.m_objPLinApi, self.m_hClient, self.m_hHw, self.m_HwMode, self.m_lMask)
        # LIN message receive queue
        self.m_LastMsgsList = []

    # Centers the app from in the middle of the screen
    #
    def centerTheWindow(self):
        """
        Centers the app in the middle of the screen.

        Returns:
            None
        """
        Desktop = self.m_Parent.winfo_toplevel()
        desktopWidth = Desktop.winfo_screenwidth()
        desktopHeight = Desktop.winfo_screenheight()
        self.m_Parent.geometry("{0}x{1}+{2}+{3}".format(self.m_Width,
                                                        self.m_Height,
                                                        (desktopWidth - self.m_Width) / 2,
                                                        (desktopHeight - self.m_Height) / 2))

    # Initializes the complete UI
    #
    def initializeWidgets(self):
        """
        Initializes the complete UI.

        Returns:
            None
        """
        # Connection groupbox
        self.gbConnection = LabelFrame(
            self.m_Frame, height=70, width=745, text=" Connection ")
        self.gbConnection.grid_propagate(0)
        self.gbConnection.grid(row=0, column=0, padx=2, pady=2)
        self.initializeConnectionWidgets()
        # Message Filtering groupbox
        self.gbMsgFilter = LabelFrame(
            self.m_Frame, height=75, width=745, text=" Message Filtering ")
        self.gbMsgFilter.grid_propagate(0)
        self.gbMsgFilter.grid(row=1, column=0, padx=2, pady=2)
        self.initializeFilteringWidgets()
        # Messages Reading groupbox
        self.gbReading = LabelFrame(
            self.m_Frame, height=175, width=745, text=" Messages Reading ")
        self.gbReading.grid_propagate(0)
        self.gbReading.grid(row=3, column=0, padx=2, pady=2)
        self.initializeReadingWidgets()
        # Messages Writing groupbox
        self.gbWriting = LabelFrame(
            self.m_Frame, height=115, width=745, text=" Write Messages ")
        self.gbWriting.grid_propagate(0)
        self.gbWriting.grid(row=4, column=0, padx=2, pady=2)
        self.initializeWritingWidgets()
        # Information groupbox
        self.gbInfo = LabelFrame(
            self.m_Frame, height=140, width=745, text=" Information ")
        self.gbInfo.grid_propagate(0)
        self.gbInfo.grid(row=5, column=0, padx=2, pady=2)
        self.initializeInformationWidgets()
        # timers
        self.tmrRead = TimerRepeater("tmrRead", 0.050, self.tmrRead_Tick)
        self.tmrDisplay = TimerRepeater(
            "tmrDisplay", 0.100, self.tmrDisplay_Tick)
        # a reentrant lock to safely access/edit lstMessages and m_LastMsgsList
        self._lock = threading.RLock()

    # Initializes controls and variables in the groupbox "Connection"
    #
    def initializeConnectionWidgets(self):
        """
        Initializes controls and variables in the groupbox "Connection".

        Returns:
            None
        """
        # hardware selection comboBox
        Label(self.gbConnection, anchor=W,
              text="Hardware:").grid(row=0, sticky=W)
        self.cbbChannel = Tix.ComboBox(
            self.gbConnection, command=self.cbbChannel_SelectedIndexChanged)
        self.cbbChannel.subwidget('entry')['width'] = 30
        self.cbbChannel.subwidget('listbox')['width'] = 30
        self.cbbChannel.grid(row=1, column=0, sticky=W)
        for name, value in self.m_dataHws.iteritems():
            self.cbbChannel.insert(Tix.END, name)
            # note: do not pick cbbChannel item yet
            # as it will fire a cbbChannel_SelectionChanged event
            # and all required controls are not yet created
        # refresh hardware button
        self.btnHwRefresh = Button(
            self.gbConnection, text="Refresh", command=self.btnHwRefresh_Click)
        self.btnHwRefresh.grid(row=1, padx=5, column=1, sticky=W)
        # baudrate combobox
        Label(self.gbConnection, anchor=W, text="Baudrate:").grid(
            row=0, column=2, sticky=W)
        self.cbbBaudrates = Tix.ComboBox(self.gbConnection)
        self.cbbBaudrates.subwidget('entry')['width'] = 16
        self.cbbBaudrates.subwidget('listbox')['width'] = 16
        self.cbbBaudrates.grid(row=1, column=2, sticky=W)
        for name, value in self.m_dataHwBaudrates.iteritems():
            self.cbbBaudrates.insert(Tix.END, name)
        self.cbbBaudrates.pick(len(self.m_dataHwBaudrates) - 1)
        # Hardware mode combobox
        Label(self.gbConnection, anchor=W, text="Mode:").grid(
            row=0, column=3, sticky=W)
        self.cbbHwMode = Tix.ComboBox(self.gbConnection)
        self.cbbHwMode.subwidget('entry')['width'] = 10
        self.cbbHwMode.subwidget('listbox')['width'] = 10
        self.cbbHwMode.grid(row=1, column=3, sticky=W)
        for name, value in self.m_dataHwModes.iteritems():
            self.cbbHwMode.insert(Tix.END, name)
        self.cbbHwMode.pick(len(self.m_dataHwModes) - 1)
        # separator
        Label(self.gbConnection, width=2, text=" ").grid(row=1, column=4)
        # identify button
        self.btnIdent = Button(
            self.gbConnection, width=8, text="Identify", command=self.btnIdent_Click)
        self.btnIdent.grid(row=1, padx=5, column=5, sticky=W)
        # initialize button
        self.btnInit = Button(
            self.gbConnection, width=8, text="Initialize", command=self.btnInit_Click)
        self.btnInit.grid(row=1, padx=5, column=6, sticky=W)
        # release button
        self.btnRelease = Button(
            self.gbConnection, width=8, state=DISABLED, text="Release", command=self.btnRelease_Click)
        self.btnRelease.grid(row=1, padx=5, column=7, sticky=W)
        # call cbbChannel change after initialization
        self.cbbChannel.pick(0)

    # Initializes controls and variables in the groupbox "Message Filtering"
    #
    def initializeFilteringWidgets(self):
        """
        Initializes controls and variables in the groupbox "Message Filtering".

        Returns:
            None
        """
        # Control variables
        self.m_FilteringRDB = IntVar(value=1)
        self.m_IdFromNUD = StringVar(value="0")
        self.m_IdToNUD = StringVar(value="63")
        # connection button
        self.btnConfigure = Button(
            self.gbMsgFilter, state=DISABLED, text="Configure Global Frame", command=self.btnConfigure_Click)
        self.btnConfigure.grid(row=1, column=0, padx=5, pady=2, sticky=W)
        # separator
        Label(self.gbMsgFilter, width=2, text=" ").grid(row=1, column=1)
        # radio button open
        self.rdbFilterOpen = Radiobutton(
            self.gbMsgFilter, text="Open", value=1, variable=self.m_FilteringRDB, command=self.rdbFilter_CheckedChanged)
        self.rdbFilterOpen.grid(row=1, column=2, padx=0, pady=2)
        # radio button close
        self.rdbFilterClose = Radiobutton(
            self.gbMsgFilter, text="Close", value=0, variable=self.m_FilteringRDB, command=self.rdbFilter_CheckedChanged)
        self.rdbFilterClose.grid(row=1, column=3, padx=0, pady=2)
        # radio button custom
        self.rdbFilterCustom = Radiobutton(
            self.gbMsgFilter, anchor=W, text="Custom (expand)",
            value=2, variable=self.m_FilteringRDB, command=self.rdbFilter_CheckedChanged)
        self.rdbFilterCustom.grid(row=1, column=4, padx=0, pady=2, sticky=W)
        # numeric upDown validation command
        vcmd = (self.m_Parent.register(self.nudId_KeyPress),
                '%d', '%i', '%P', '%s', '%S', '%v', '%V', '%W')
        # numeric upDown filter from
        Label(self.gbMsgFilter, anchor=W, text="From:").grid(
            row=0, column=5, sticky=W)
        self.nudIdFrom = Spinbox(
            self.gbMsgFilter, width=10, from_=0, to=63, textvariable=self.m_IdFromNUD,
            validate="key", validatecommand=vcmd)
        self.nudIdFrom.grid(row=1, column=5, padx=5, pady=2)
        self.nudIdFrom.bind("<FocusOut>", self.nudId_Leave)
        # numeric upDown filter to
        Label(self.gbMsgFilter, anchor=W, text="To:").grid(
            row=0, column=6, sticky=W)
        self.nudIdTo = Spinbox(
            self.gbMsgFilter, width=10, from_=0, to=63, textvariable=self.m_IdToNUD,
            validate="key", validatecommand=vcmd)
        self.nudIdTo.grid(row=1, column=6, padx=5, pady=2, sticky=W)
        self.nudIdTo.bind("<FocusOut>", self.nudId_Leave)
        # separator
        Label(self.gbMsgFilter, width=1,
              padx=2, text=" ").grid(row=1, column=7)
        # button filter apply
        self.btnFilterApply = Button(
            self.gbMsgFilter, width=8, state=DISABLED, text="Apply", command=self.btnFilterApply_Click)
        self.btnFilterApply.grid(row=1, column=8, padx=5, sticky=W)
        # button filter query
        self.btnFilterQuery = Button(
            self.gbMsgFilter, width=8, state=DISABLED, text="Query", command=self.btnFilterQuery_Click)
        self.btnFilterQuery.grid(row=1, column=9, padx=5, sticky=W)
        self.rdbFilter_CheckedChanged()

    # Initializes controls and variables in the groupbox "Messages Reading"
    #
    def initializeReadingWidgets(self):
        """
        Initializes controls and variables in the groupbox "Messages Reading".

        Returns:
            None
        """
        # Control variables
        self.m_ReadingRDB = IntVar(value=1)
        self.m_showPeriodCHB = IntVar(value=1)
        self.tmrRead = threading.Timer(5.0, self.tmrRead_Tick)
        self.m_ListColCaption = [
            "ID", "|Length", "|Data", "|Count", "|RcvTime", "|Direction", "|Errors"]
        self.m_ListColSpace = [6, 7, 27, 7, 10, 15, 2]
        self.m_ListFont = tkFont.Font(family="Lucida Console", size="10")
        self.m_ListCaptionPadxSpaces = []
        for colText, colWidth in zip(self.m_ListColCaption, self.m_ListColSpace):
            self.m_ListCaptionPadxSpaces.append(colWidth - len(colText))
        self.m_ListCaptionPadxSpaces[0] = self.m_ListCaptionPadxSpaces[0] - 1
        # radio button timer
        self.rdbTimer = Radiobutton(self.gbReading, text="Read using a Timer",
                                    value=1, variable=self.m_ReadingRDB, command=self.rdbTimer_CheckedChanged)
        self.rdbTimer.grid(row=0, column=0, padx=5, pady=2, sticky=W)
        # radio button manual read
        self.rdbManual = Radiobutton(
            self.gbReading, text="Manual Read", value=0, variable=self.m_ReadingRDB, command=self.rdbTimer_CheckedChanged)
        self.rdbManual.grid(row=0, column=1, padx=5, pady=2, sticky=W)
        # checkbox timestamp
        self.chbShowPeriod = Checkbutton(
            self.gbReading, width=16, text="Timestamp as period",
            variable=self.m_showPeriodCHB, command=self.chbShowPeriod_CheckedChanged)
        self.chbShowPeriod.grid(row=0, column=2, padx=5, pady=2)
        # scrollbar for the message listbox
        self.yReadScroll = Scrollbar(self.gbReading, orient=VERTICAL)
        self.yReadScroll.grid(row=1, column=3, rowspan=2, sticky=N + S)
        self.xReadScroll = Scrollbar(self.gbReading, orient=HORIZONTAL)
        self.xReadScroll.grid(row=3, column=0, columnspan=3, sticky=W + E)
        # label as header for listBox
        tempString = ""
        for caption, spaces in zip(self.m_ListColCaption, self.m_ListCaptionPadxSpaces):
            tempString = tempString + "{0}{1}".format(caption, " " * spaces)
        Label(
            self.gbReading, anchor=W, text=tempString, bg="#E2E2E3", fg="#000000",
            font=self.m_ListFont, relief=GROOVE).grid(row=1, column=0, columnspan=3, padx=5, sticky="nwes")
        # message listbox
        self.lstMessages = Tix.TList(
            self.gbReading, relief=GROOVE, height=5, orient="horizontal",
            yscrollcommand=self.yReadScroll.set,
            xscrollcommand=self.xReadScroll.set,
            itemtype="text", font=self.m_ListFont,
            command=self.btnMsgClear_Click)
        self.lstMessages.grid(
            row=2, column=0, padx=5, columnspan=3, sticky="nwes")
        # associate scrollbar to listbox
        self.yReadScroll['command'] = self.lstMessages.yview
        self.xReadScroll['command'] = self.lstMessages.xview
        # separator
        Label(self.gbReading, width=0, padx=0, text=" ").grid(row=0,  column=4)
        # button read
        self.btnRead = Button(
            self.gbReading, width=8, state=DISABLED, text="Read", command=self.btnRead_Click)
        self.btnRead.grid(row=1, column=5, padx=5, pady=5, sticky=NW)
        # button clear
        self.btnMsgClear = Button(
            self.gbReading, width=8, state=ACTIVE, text="Clear", command=self.btnMsgClear_Click)
        self.btnMsgClear.grid(row=2, column=5, padx=5, sticky=NW)

    # Initializes controls and variables in the groupbox "Write Messages"
    #
    def initializeWritingWidgets(self):
        """
        Initializes controls and variables in the groupbox "Write Messages".

        Returns:
            None
        """
        # Control variables
        self.m_IDTXT = StringVar(value="000")
        self.m_LengthNUD = StringVar(value="8")
        self.m_Data0TXT = StringVar(value="00")
        self.m_Data1TXT = StringVar(value="00")
        self.m_Data2TXT = StringVar(value="00")
        self.m_Data3TXT = StringVar(value="00")
        self.m_Data4TXT = StringVar(value="00")
        self.m_Data5TXT = StringVar(value="00")
        self.m_Data6TXT = StringVar(value="00")
        self.m_Data7TXT = StringVar(value="00")
        # frame info : an invisible frame containing first row of frame
        # information
        self.gbFrameInfo = Frame(self.gbWriting)
        self.gbFrameInfo.grid_propagate(1)
        self.gbFrameInfo.grid(row=0, column=0, padx=3, pady=0, sticky=W)
        # frame ID combobox
        Label(self.gbFrameInfo, anchor=W,
              text="ID (hex):").grid(row=0, sticky=W)
        self.cbbID = Tix.ComboBox(
            self.gbFrameInfo, command=self.cbbID_SelectedIndexChanged)
        self.cbbID.subwidget('entry')['width'] = 10
        self.cbbID.subwidget('listbox')['width'] = 10
        self.cbbID.grid(row=1, column=0, sticky=W)
        # frame PID entry
        Label(self.gbFrameInfo, anchor=W, text="PID (Hex):").grid(
            row=0, column=1, sticky=W)
        self.txtID = Entry(
            self.gbFrameInfo, width=13, state=DISABLED, textvariable=self.m_IDTXT)
        self.txtID.grid(row=1, column=1, padx=5, pady=0, sticky=W)
        # direction combobox
        Label(self.gbFrameInfo, anchor=W, text="Direction:").grid(
            row=0, column=2, sticky=W)
        self.cbbDirection = Tix.ComboBox(
            self.gbFrameInfo, state=DISABLED, command=self.cbbID_SelectedIndexChanged)
        self.cbbDirection.subwidget('entry')['width'] = 20
        self.cbbDirection.subwidget('listbox')['width'] = 20
        self.cbbDirection.grid(row=1, column=2, sticky=W)
        # checksum type combobox
        Label(self.gbFrameInfo, anchor=W, text="Checksum Type:").grid(
            row=0, column=3, sticky=W)
        self.cbbCST = Tix.ComboBox(
            self.gbFrameInfo, state=DISABLED, command=self.cbbID_SelectedIndexChanged)
        self.cbbCST.subwidget('entry')['width'] = 20
        self.cbbCST.subwidget('listbox')['width'] = 20
        self.cbbCST.grid(row=1, column=3, sticky=W)
        # frame data : an invisible frame containing second row of frame
        # information
        self.gbFrameData = Frame(self.gbWriting)
        self.gbFrameData.grid_propagate(1)
        self.gbFrameData.grid(row=1, column=0, padx=3, pady=0, sticky=W)
        # length numeric upDown
        Label(self.gbFrameData, anchor=W, text="Length:").grid(
            row=0, column=0, sticky=W)
        self.nudLength = Spinbox(
            self.gbFrameData, width=5, from_=0, to=8, state=DISABLED,
            textvariable=self.m_LengthNUD, command=self.nudLength_ValueChanged)
        self.nudLength.grid(row=1, column=0, padx=7, pady=2, sticky=W)
        # data entry validation command
        vcmd = (self.m_Parent.register(self.txtData0_KeyPress),
                '%d', '%i', '%P', '%s', '%S', '%v', '%V', '%W')
        # data entry 1
        self.txtData0 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data0TXT,
            validate="key", validatecommand=vcmd)
        self.txtData0.grid(row=1, column=1, padx=3, pady=0, sticky=W)
        self.txtData0.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 2
        self.txtData1 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data1TXT,
            validate="key", validatecommand=vcmd)
        self.txtData1.grid(row=1, column=2, padx=3, pady=0, sticky=W)
        self.txtData1.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 3
        self.txtData2 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data2TXT,
            validate="key", validatecommand=vcmd)
        self.txtData2.grid(row=1, column=3, padx=3, pady=0, sticky=W)
        self.txtData2.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 4
        self.txtData3 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data3TXT,
            validate="key", validatecommand=vcmd)
        self.txtData3.grid(row=1, column=4, padx=3, pady=0, sticky=W)
        self.txtData3.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 5
        self.txtData4 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data4TXT,
            validate="key", validatecommand=vcmd)
        self.txtData4.grid(row=1, column=5, padx=3, pady=0, sticky=W)
        self.txtData4.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 6
        self.txtData5 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data5TXT,
            validate="key", validatecommand=vcmd)
        self.txtData5.grid(row=1, column=6, padx=3, pady=0, sticky=W)
        self.txtData5.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 7
        self.txtData6 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data6TXT,
            validate="key", validatecommand=vcmd)
        self.txtData6.grid(row=1, column=7, padx=3, pady=0, sticky=W)
        self.txtData6.bind("<FocusOut>", self.txtData0_Leave)
        # data entry 8
        self.txtData7 = Entry(
            self.gbFrameData, width=4, state=DISABLED, textvariable=self.m_Data7TXT,
            validate="key", validatecommand=vcmd)
        self.txtData7.grid(row=1, column=8, padx=3, pady=0, sticky=W)
        self.txtData7.bind("<FocusOut>", self.txtData0_Leave)
        # separator
        Label(self.gbWriting, width=24, padx=1, text=" ").grid(row=0, column=2)
        # write button
        self.btnWrite = Button(
            self.gbWriting, width=8, state=DISABLED, text="Write", command=self.btnWrite_Click)
        self.btnWrite.grid(row=0, column=3, sticky=W)
        # Populates FrameID combobox with global frame IDs
        self.updateFrameIds()
        # add command to cbbHwMode (btnWrite needed to be created before
        # setting the command)
        self.cbbHwMode.configure(command=self.cbbHwMode_SelectedIndexChanged)

    # Initializes controls and variables in the groupbox "Information"
    #
    def initializeInformationWidgets(self):
        """
        Initializes controls and variables in the groupbox "Information".

        Returns:
            None
        """
        # scrollbar for information listbox
        self.yInfoScroll = Scrollbar(self.gbInfo, orient=VERTICAL)
        self.yInfoScroll.grid(row=0, column=1, sticky=N + S)
        # information listbox
        self.lbxInfo = Listbox(self.gbInfo, width=90, height=7,
                               activestyle="none", yscrollcommand=self.yInfoScroll.set)
        self.lbxInfo.grid(row=0, column=0, padx=5, sticky="nwes")
        self.lbxInfo.bind("<Double-1>", self.btnInfoClear_Click)
        # associate scrollbar to listbox
        self.yInfoScroll['command'] = self.lbxInfo.yview
        self.lbxInfo.insert(
            END, "Select a Hardware and a configuration for it. Then click ""Initialize"" button")
        # separator
        Label(self.gbInfo, width=2, text=" ").grid(row=0, column=2)
        # version button
        self.btnGetVersions = Button(
            self.gbInfo, width=8, state=DISABLED, text="Versions", command=self.btnGetVersions_Click)
        self.btnGetVersions.grid(row=0, column=3, padx=4, sticky=NW)
        # clear button
        self.btnInfoClear = Button(
            self.gbInfo, width=8, state=ACTIVE, text="Clear", command=self.btnInfoClear_Click)
        self.btnInfoClear.grid(row=0, column=4, sticky=NW)
        # status button
        self.btnStatus = Button(
            self.gbInfo, width=8, state=DISABLED, text="Status", command=self.btnStatus_Click)
        self.btnStatus.grid(row=0, column=3, padx=4, sticky=W)
        # reset button
        self.btnReset = Button(
            self.gbInfo, width=8, state=DISABLED, text="Reset", command=self.btnReset_Click)
        self.btnReset.grid(row=0, column=4, sticky=W)

    # Activates/deactivates the different controls of the main-form
    #
    def setConnectionStatus(self, bConnected=True):
        """
        Activates/deactivates the different controls of the main-form according
        to the current connection status.

        Parameters:
            bConnected  defines if the hardware is connected to (boolean)

        Returns:
            None
        """
        # Gets the status values for each case
        if bConnected:
            stsConnected = ACTIVE
            stsNotConnected = DISABLED
        else:
            stsConnected = DISABLED
            stsNotConnected = ACTIVE
        # updates buttons state
        self.btnInit['state'] = stsNotConnected
        self.btnConfigure['state'] = stsConnected
        if (self.m_ReadingRDB.get() == 0) and bConnected:
            self.btnRead['state'] = ACTIVE
        else:
            self.btnRead['state'] = DISABLED
        self.btnWrite['state'] = stsConnected
        self.btnRelease['state'] = stsConnected
        self.btnFilterApply['state'] = stsConnected
        self.btnFilterQuery['state'] = stsConnected
        self.btnGetVersions['state'] = stsConnected
        self.btnHwRefresh['state'] = stsNotConnected
        self.btnStatus['state'] = stsConnected
        self.btnReset['state'] = stsConnected
        # Updates ComboBoxs state
        self.cbbBaudrates['state'] = stsNotConnected
        self.cbbChannel['state'] = stsNotConnected
        self.cbbHwMode['state'] = stsNotConnected
        # Hardware configuration and read mode
        if (not bConnected):
            self.cbbChannel_SelectedIndexChanged(self.cbbChannel['selection'])
        else:
            self.rdbTimer_CheckedChanged()
        # Display messages in grid
        if (bConnected):
            self.tmrDisplay.start()
        else:
            self.tmrDisplay.stop()

    # Updates 'cbbID' comboBox with values from the global frame table
    #
    def updateFrameIds(self):
        """
        Updates 'cbbID' comboBox with values from the global frame table.

        Returns:
            None
        """
        # Retrieves selected ID if it exists
        strIdSel = self.cbbID['selection']
        bIdSelected = False
        # Clears and populates FrameID combobox with global frame IDs
        self.cbbID.subwidget('listbox').delete(0, END)
        for pFrame in self.m_pGFT.m_lFrameDefinitions:
            # add only non disabled frames
            if (pFrame.m_bDirection == PLinApi.TLIN_DIRECTION_DISABLED.value):
                continue
            strId = hex(pFrame.m_nID)
            self.cbbID.insert(Tix.END, strId)
            if (strId == strIdSel):
                bIdSelected = True
                self.cbbID.pick(self.cbbID.subwidget('listbox').size() - 1)
        if (bIdSelected == False):
            self.cbbID.pick(0)

    # Inserts a message in the information listBox.
    #
    def includeTextMessage(self, strMsg):
        """
        Inserts a message in the information listBox.

        Returns:
            None
        """
        self.lbxInfo.insert(END, strMsg)
        self.lbxInfo.see(END)

    # Shows an error message box corresponding to a LIN error
    #
    def showMessageError(self, linError):
        """
        Shows an error message box corresponding to a LIN error.

        Returns:
            None
        """
        tkMessageBox.showinfo("Error!", self.getFormattedError(linError))

    # Formats a LIN Error
    #
    def getFormattedError(self, linError):
        """
        Formats a LIN Error

        Returns:
            a string corresponding to the LIN Error
        """
        result = ''
        # get error string from code
        pTextBuff = create_string_buffer(255)
        linResult = self.m_objPLinApi.GetErrorText(
            linError, 0x09, pTextBuff, 255)
        # display error message
        if (linResult == PLinApi.TLIN_ERROR_OK and len(pTextBuff.value) != 0):
            result = str.format("{0}", bytes.decode(pTextBuff.value))
        else:
            result = str.format(
                "An error occurred. Error-code's text ({0}) couldn't be retrieved", linError)
        return result

# *****************************************************************
# Form Event Handlers
# *****************************************************************

    # Form-Closing Function / Finish function
    #
    def Form_OnClosing(self, event=None):
        """
        Form-Closing Function / Finish function

        Returns:
            None
        """
        # disconnect from LIN if necessary
        if (self.m_hClient.value != PLinApi.HLINCLIENT(0).value):
            self.doLinDisconnect()
            self.m_hHw = PLinApi.HLINHW(0)
            # Unregister the application
            self.m_objPLinApi.RemoveClient(self.m_hClient)
            self.m_hClient = PLinApi.HLINCLIENT(0)
        self.tmrRead.stop()
        self.tmrDisplay.stop()
        # Quit our mainloop
        self.exit = 0

# *****************************************************************
# Connection Event Handlers
# *****************************************************************

    # Combobox cbbChannel handler
    #
    def cbbChannel_SelectedIndexChanged(self, currentValue):
        """
        Combobox cbbChannel handler

        Returns:
            None
        """
        # get the handle from the comboBoxItem
        lwHw = self.m_dataHws[currentValue]
        if (lwHw.value != 0):
            self.btnInit['state'] = ACTIVE
            self.btnIdent['state'] = ACTIVE
            lnMode = c_int(0)
            lnCurrBaud = c_int(0)
            # Read the mode of the hardware with the handle lwHw (Master, Slave
            # or None).
            linResult = self.m_objPLinApi.GetHardwareParam(
                lwHw, PLinApi.TLIN_HARDWAREPARAM_MODE, lnMode, 0)
            # Read the baudrate of the hardware with the handle lwHw.
            linResult = self.m_objPLinApi.GetHardwareParam(
                lwHw, PLinApi.TLIN_HARDWAREPARAM_BAUDRATE, lnCurrBaud, 0)
            # Update hardware mode comboBox
            if (lnMode.value == PLinApi.TLIN_HARDWAREMODE_MASTER.value):
                self.cbbHwMode.pick(0)
            else:
                self.cbbHwMode.pick(1)
            # Assign the Baudrate to the Control.
            if (self.m_dataHwBaudrates.has_key(str(lnCurrBaud.value))):
                self.cbbBaudrates.pick(
                    self.m_dataHwBaudrates.keys().index(str(lnCurrBaud.value)))
            elif (lnCurrBaud.value == 0):
                self.cbbBaudrates.pick(len(self.m_dataHwBaudrates) - 1)
            else:
                self.cbbBaudrates['selection'] = str(lnCurrBaud.value)
        else:
            self.btnInit['state'] = DISABLED
            self.btnIdent['state'] = DISABLED

    # Button btnHwRefresh handler
    #
    def btnHwRefresh_Click(self):
        """
        Button btnHwRefresh handler

        Returns:
            None
        """
        self.refreshHardware()

    # Combobox cbbHwMode handler
    def cbbHwMode_SelectedIndexChanged(self, currentValue):
        """
        Combobox cbbChannel handler

        Returns:
            None
        """
        if (currentValue == 'Master'):
            self.btnWrite.configure(text='Transmit')
        else:
            self.btnWrite.configure(text='Publish')

    # Button btnIdent handler
    #
    def btnIdent_Click(self):
        """
        Button btnIdent handler

        Returns:
            None
        """
        # get selected item
        lHw = self.m_dataHws[self.cbbChannel['selection']]
        if (lHw.value != 0):
            # makes the corresponding PCAN-USB-Pro's LED blink
            self.m_objPLinApi.IdentifyHardware(lHw)

    # Button btnInit handler
    #
    def btnInit_Click(self):
        """
        Button btnInit handler

        Returns:
            None
        """
        # Do the connection
        if (self.doLinConnect()):
            # Sets the connection status of the main-form
            self.setConnectionStatus(True)

    # Button btnRelease handler
    #
    def btnRelease_Click(self):
        """
        Button btnRelease handler

        Returns:
            None
        """
        # Releases the current connection
        if (self.doLinDisconnect()):
            # stop reading timer
            self.tmrRead.stop()
            # Sets the connection status of the main-form
            self.setConnectionStatus(False)

# *****************************************************************
# Configuration & Filter messages Event Handlers
# *****************************************************************

    # Button btnConnection handler
    #
    def btnConfigure_Click(self):
        """
        Button btnConnection handler

        Returns:
            None
        """
        # update global frame lin information (needed as m_pGFT makes calls to
        # PLinApi functions)
        self.m_pGFT.updateLinInfo(
            self.m_hClient, self.m_hHw, self.m_HwMode,  self.m_lMask)
        # Open the "Global Frame Table" modal Dialog
        dlg = CFrameDialog(self.m_Parent, self.m_pGFT)
        # Output filter information (as changes to "Global Frames Table" items
        # modifies it)
        self.btnFilterQuery_Click()
        # Update the available frame ids (i.e. the IDs combobox in the "write
        # message" UI group)
        self.updateFrameIds()

    # Filtering RadioButtons handler
    #
    def rdbFilter_CheckedChanged(self):
        """
        Filtering RadioButtons handler

        Returns:
            None
        """
        # enable/disable NUD filters
        if (self.m_FilteringRDB.get() == 2):
            # custom filter checked
            self.nudIdFrom['state'] = NORMAL
            self.nudIdTo['state'] = NORMAL
        else:
            self.nudIdFrom['state'] = DISABLED
            self.nudIdTo['state'] = DISABLED

    # Key Pressed handlers on nudFilters
    #
    def nudId_KeyPress(self, action, index, value_if_allowed, prior_value, text, validation_type, trigger_type, widget_name):
        """
        Key Pressed on nudFilters

        Parameters:
            action              Type of action: 1 for insert, 0 for delete, or -1 for focus, forced or textvariable validation.
            index               Index of char string to be inserted/deleted, if any, otherwise -1.
            value_if_allowed    The value of the entry if the edit is allowed. If you are configuring the entry widget to have a new textvariable, this will be the value of that textvariable.
            prior_value         The current value of entry prior to editing.
            text                The text string being inserted/deleted, if any, {} otherwise.
            validation_type     The type of validation currently set.
            trigger_type        The type of validation that triggered the callback (key, focusin, focusout, forced).
            widget_name         The name of the entry widget.

        Returns:
            True if data is valid
        """
        # assert key pressed is numeric and positive
        if (len(value_if_allowed) == 0):
            return True
        return value_if_allowed.isdigit() and int(value_if_allowed) >= 0

    # Leaving event-handler on nudFilters
    #
    def nudId_Leave(self, event):
        """
        Leaving event-handler on nudFilters

        Paramers:
            event TkEvent

        Returns:
            None
        """
        if (event.widget == self.nudIdFrom):
            try:
                value = int(self.m_IdFromNUD.get())
            except:
                value = -1
            if (value < 0):
                self.m_IdFromNUD.set(0)
            elif (value > 63):
                self.m_IdFromNUD.set(63)
        elif (event.widget == self.nudIdTo):
            try:
                value = int(self.m_IdToNUD.get())
            except:
                value = -1
            if (value < 0):
                self.m_IdToNUD.set(0)
            elif (value > 63):
                self.m_IdToNUD.set(63)

    # Button btnFilterApply handler
    #
    def btnFilterApply_Click(self):
        """
        Button btnFilterApply handler

        Returns:
            None
        """
        if (self.m_hHw.value != 0):
            lMask = c_uint64(0)
            # Sets the according to the selected filter
            if (self.m_FilteringRDB.get() == 1):
                # set no filtering : all frames are received
                lMask = self.FRAME_FILTER_MASK
                linError = self.m_objPLinApi.SetClientFilter(
                    self.m_hClient, self.m_hHw, lMask)
                if (linError == PLinApi.TLIN_ERROR_OK):
                    self.m_lMask = lMask
                    self.includeTextMessage(
                        'The filter was successfully opened.')
            elif (self.m_FilteringRDB.get() == 0):
                # filters all : all frames are denied
                lMask = c_uint64(0)
                linError = self.m_objPLinApi.SetClientFilter(
                    self.m_hClient, self.m_hHw, lMask)
                if (linError == PLinApi.TLIN_ERROR_OK):
                    self.m_lMask = lMask
                    self.includeTextMessage(
                        'The filter was successfully closed.')
            else:
                # custom filtering (i.e. rdbFilterCustom.Checked) : the frames
                # for the given range will be opened
                linError = self.m_objPLinApi.RegisterFrameId(
                    self.m_hClient, self.m_hHw,
                    c_ubyte(
                        int(self.m_IdFromNUD.get())),
                    c_ubyte(int(self.m_IdToNUD.get())))
                if (linError == PLinApi.TLIN_ERROR_OK):
                    self.includeTextMessage(
                        str.format(
                            'The filter was customized. IDs from {0} to {1} will be received',
                            self.m_IdFromNUD.get(), self.m_IdToNUD.get()))
                    linError = self.m_objPLinApi.GetClientFilter(
                        self.m_hClient, self.m_hHw, lMask)
                    if (linError == PLinApi.TLIN_ERROR_OK):
                        m_lMask = lMask
            # If success, an information message is written, if it is not, an
            # error message is shown
            if (linError != PLinApi.TLIN_ERROR_OK):
                self.showMessageError(linError)

    # Button btnFilterQuery handler
    #
    def btnFilterQuery_Click(self):
        """
        Button btnFilterQuery handler

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
            self.includeTextMessage('The status of the filter is: ' + filter)

# *****************************************************************
# Read messages Event Handlers
# *****************************************************************

    # Radiobuttons timer/read handler
    #
    def rdbTimer_CheckedChanged(self):
        """
        Radiobuttons timer/read handler

        Returns:
            None
        """
        if (self.btnRelease['state'] == DISABLED):
            return
        # According to the kind of reading, a timer or a button will be enabled
        if (self.m_ReadingRDB.get() == 0):
            # disable Timer
            self.tmrRead.stop()
        else:
            # Enable timer
            self.tmrRead.start()
        # set read button state
        if (self.m_ReadingRDB.get() == 0 and not (self.btnRelease['state'] == DISABLED)):
            self.btnRead['state'] = ACTIVE
        else:
            self.btnRead['state'] = DISABLED

    # Radiobuttons timer/read handler
    #
    def chbShowPeriod_CheckedChanged(self):
        """
        Checkbox chbShowPeriod handler

        Returns:
            None
        """
        for msg in self.m_LastMsgsList:
            msg.setShowPeriod(self.m_showPeriodCHB.get())

    # Button btnRead handler
    #
    def btnRead_Click(self):
        """
        Button btnRead handler

        Returns:
            None
        """
        self.readMessages()

    # Button btnMsgClear handler
    #
    def btnMsgClear_Click(self, *args):
        """
        Button btnMsgClear handler

        Returns:
            None
        """
        # prevent concurrency errors with timers (read and display)
        with self._lock:
            self.m_LastMsgsList = []
            self.lstMessages.delete(0, END)

    # Timer read event handler
    #
    def tmrRead_Tick(self):
        """
        Timer read event handler

        Returns:
            None
        """
        # Checks if there are messages to read in the receive-queue
        self.readMessages()

    # Timer display event handler
    #
    def tmrDisplay_Tick(self):
        """
        Timer display event handler

        Returns:
            None
        """
        #  Update display of received messages
        self.displayMessages()

# *****************************************************************
# Write messages Event Handlers
# *****************************************************************

    # comboBox cbbID selection changed handler
    #
    def cbbID_SelectedIndexChanged(self, currentValue):
        """
        ComboBox cbbID selection changed handler

        Returns:
            None
        """
        # get selected frame
        try:
            frameId = int(currentValue, 16)
            frame = self.m_pGFT.m_lFrameDefinitions[frameId]
        except ValueError:
            # no frame selected
            return
        self.m_IDTXT.set(hex(frame.m_nProtectedID))
        self.m_LengthNUD.set(frame.m_nLength)
        self.cbbDirection['selection'] = CFrameDefinition.getFrameDirectionAsString(
            frame.m_bDirection)
        self.cbbCST['selection'] = CFrameDefinition.getFrameCSTAsString(
            frame.m_nChecksumType)
        # Force update on Data fields (both Length and Direction impact on Enabled state of these components)
        # if length value is not changed but direction switch from subscriber to publisher
        # data textfield would not be disabled
        self.nudLength_ValueChanged()

    # Button btnWrite handler
    #
    def btnWrite_Click(self):
        """
        ComboBox cbbID selection changed handler

        Returns:
            None
        """
        # Assert that a frame ID has been selected
        # Get the CFrameDefinition associated to the selected FrameID
        frame = None
        try:
            frameId = int(self.cbbID['selection'], 16)
            frame = self.m_pGFT.m_lFrameDefinitions[frameId]
        except:
            self.cbbID.focus()
            return
        # Create a new LIN frame message and copy the data.
        pMsg = PLinApi.TLINMsg()
        pMsg.FrameId = c_ubyte(frame.m_nProtectedID)
        pMsg.Direction = PLinApi.TLINDirection(frame.m_bDirection)
        pMsg.ChecksumType = PLinApi.TLINChecksumType(frame.m_nChecksumType)
        pMsg.Length = c_ubyte(frame.m_nLength)
        # fill data
        edits = [
            self.m_Data0TXT, self.m_Data1TXT, self.m_Data2TXT, self.m_Data3TXT,
            self.m_Data4TXT, self.m_Data5TXT, self.m_Data6TXT, self.m_Data7TXT]
        for i in range(frame.m_nLength):
            pMsg.Data[i] = c_ubyte(int(edits[i].get(), 16))
        # Check if the hardware is initialized as master
        if (self.m_HwMode.value == PLinApi.TLIN_HARDWAREMODE_MASTER.value):
            # Calculate the checksum contained with the
            # checksum type that was set some line before.
            linResult = self.m_objPLinApi.CalculateChecksum(pMsg)
            # Try to send the LIN frame message with LIN_Write.
            linResult = self.m_objPLinApi.Write(
                self.m_hClient, self.m_hHw, pMsg)
        else:
            # If the hardware is initialized as slave
            # only update the data in the LIN frame.
            linResult = self.m_objPLinApi.UpdateByteArray(
                self.m_hClient, self.m_hHw, c_ubyte(frameId), c_ubyte(0), c_ubyte(pMsg.Length), pMsg.Data)
        # Show error if any
        if (linResult != PLinApi.TLIN_ERROR_OK):
            self.showMessageError(linResult)

    def txtData0_KeyPress(self, action, index, value_if_allowed, prior_value, text, validation_type, trigger_type, widget_name):
        """
        Key Pressed on nudFilters

        Parameters:
            action              Type of action: 1 for insert, 0 for delete, or -1 for focus, forced or textvariable validation.
            index               Index of char string to be inserted/deleted, if any, otherwise -1.
            value_if_allowed    The value of the entry if the edit is allowed. If you are configuring the entry widget to have a new textvariable, this will be the value of that textvariable.
            prior_value         The current value of entry prior to editing.
            text                The text string being inserted/deleted, if any, {} otherwise.
            validation_type     The type of validation currently set.
            trigger_type        The type of validation that triggered the callback (key, focusin, focusout, forced).
            widget_name         The name of the entry widget.

        Returns:
            True if data is valid
        """
        # assert key pressed is numeric and positive
        if (len(value_if_allowed) == 0):
            return True
        try:
            val = int(value_if_allowed, 16)
            return val >= 0
        except:
            return False

    # Entry txtDataX OnLeave handler
    #
    def txtData0_Leave(self, *args):
        """
        Entry txtDataX OnLeave handler

        Returns:
            None
        """
        edits = [
            self.m_Data0TXT, self.m_Data1TXT, self.m_Data2TXT, self.m_Data3TXT,
            self.m_Data4TXT, self.m_Data5TXT, self.m_Data6TXT, self.m_Data7TXT]
        for edit in edits:
            # The format of all Textboxes Data fields are checked.
            self.txtData0_LeaveHelper(edit)

    # Helper function for the function txtData0_Leave
    #
    def txtData0_LeaveHelper(self, editVar):
        """
        Helper function for the function txtData0_Leave

        Returns:
            None
        """
        try:
            iValue = int(editVar.get(), 16)
        except ValueError:
            iValue = 0
        finally:
            # All the Textbox Data fields are represented with 2 characters.
            # The maximum value allowed is 255 (0xFF)
            if iValue > 255:
                iValue = 255
            editVar.set("{0:0{1}X}".format(iValue, 2))

    # Spinbutton nudLength handler
    #
    def nudLength_ValueChanged(self):
        """
        Spinbutton nudLength handler

        Returns:
            None
        """
        self.txtData0['state'] = DISABLED
        self.txtData1['state'] = DISABLED
        self.txtData2['state'] = DISABLED
        self.txtData3['state'] = DISABLED
        self.txtData4['state'] = DISABLED
        self.txtData5['state'] = DISABLED
        self.txtData6['state'] = DISABLED
        self.txtData7['state'] = DISABLED
        # Only Publisher direction allows edition of Data textFields
        if (self.cbbDirection['selection'] != CFrameDefinition.getFrameDirectionAsString(PLinApi.TLIN_DIRECTION_PUBLISHER)):
            return
        # Enable the same number of data textBox as the nudLength value
        len = int(self.m_LengthNUD.get())
        if (len == 1):
            self.txtData0['state'] = NORMAL
        elif (len == 2):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
        elif (len == 3):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
            self.txtData2['state'] = NORMAL
        elif (len == 4):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
            self.txtData2['state'] = NORMAL
            self.txtData3['state'] = NORMAL
        elif (len == 5):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
            self.txtData2['state'] = NORMAL
            self.txtData3['state'] = NORMAL
            self.txtData4['state'] = NORMAL
        elif (len == 6):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
            self.txtData2['state'] = NORMAL
            self.txtData3['state'] = NORMAL
            self.txtData4['state'] = NORMAL
            self.txtData5['state'] = NORMAL
        elif (len == 7):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
            self.txtData2['state'] = NORMAL
            self.txtData3['state'] = NORMAL
            self.txtData4['state'] = NORMAL
            self.txtData5['state'] = NORMAL
            self.txtData6['state'] = NORMAL
        if (len == 8):
            self.txtData0['state'] = NORMAL
            self.txtData1['state'] = NORMAL
            self.txtData2['state'] = NORMAL
            self.txtData3['state'] = NORMAL
            self.txtData4['state'] = NORMAL
            self.txtData5['state'] = NORMAL
            self.txtData6['state'] = NORMAL
            self.txtData7['state'] = NORMAL

# *****************************************************************
# Information Event Handlers
# *****************************************************************

    # Button btnGetVersions handler
    #
    def btnGetVersions_Click(self):
        """
        Button btnGetVersions handler

        Returns:
            None
        """
        # get the vesion of the PLIN API
        lpVersion = PLinApi.TLINVersion()
        linResult = self.m_objPLinApi.GetVersion(lpVersion)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            self.includeTextMessage(str.format(" API Version: {0}.{1}.{2}.{3}",
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
                self.includeTextMessage("  Channel/Driver Version: ")
                for line in lines:
                    self.includeTextMessage("      * " + line)
        if (linResult != PLinApi.TLIN_ERROR_OK):
            self.showMessageError(linResult)

    # Button btnInfoClear handler
    #
    def btnInfoClear_Click(self, event=None):
        """
        Button btnInfoClear handler

        Returns:
            None
        """
        # Clear information messages
        self.lbxInfo.delete(0, END)

    # Button btnReset handler
    #
    def btnReset_Click(self):
        """
        Button btnReset handler

        Returns:
            None
        """
        # Flushes the Receive Queue of the Client and resets its counters.
        linResult = self.m_objPLinApi.ResetClient(self.m_hClient)
        if (linResult != PLinApi.TLIN_ERROR_OK):
            self.showMessageError(linResult)
        else:
            # clears the message list-view
            self.btnMsgClear_Click()
            self.includeTextMessage('Receive Queue successfully flushed')

    # Button btnStatus handler
    #
    def btnStatus_Click(self):
        """
        Button btnStatus handler

        Returns:
            None
        """
        # Retrieves the status of the LIN Bus and outputs its state
        pStatus = PLinApi.TLINHardwareStatus()
        linResult = self.m_objPLinApi.GetStatus(self.m_hHw, pStatus)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            # output bus status
            if (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_ACTIVE.value):
                self.includeTextMessage('Bus: Active')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_AUTOBAUDRATE.value):
                self.includeTextMessage('Hardware: Baudrate Detection')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_NOT_INITIALIZED.value):
                self.includeTextMessage('Hardware: Not Initialized')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_SHORT_GROUND.value):
                self.includeTextMessage('Bus-Line: Shorted Ground')
            elif (pStatus.Status == PLinApi.TLIN_HARDWARESTATE_SLEEP.value):
                self.includeTextMessage('Bus: Sleep')
        else:
            self.showMessageError(linResult)

# *****************************************************************
# LIN Connection functions
# *****************************************************************
    # Connects to a LIN hardware
    #
    def doLinConnect(self):
        """
        Connects to a LIN hardware.

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
        hwHandle = self.m_dataHws[self.cbbChannel['selection']]
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
            hwMode = self.m_dataHwModes[self.cbbHwMode['selection']]
            try:
                # convert baudrates selection to int
                hwBaudrate = c_ushort(int(self.cbbBaudrates['selection']))
            except:
                hwBaudrate = c_ushort(0)
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
            self.showMessageError(linResult)
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
                    self.showMessageError(linResult)
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
        and assign it to the GlobalFrameTable. Also refresh
        the available Global Frame Table combobox with those values.

        Returns:
            None
        """
        # Read each Frame Definition
        for pFrame in self.m_pGFT.m_lFrameDefinitions:
            # Before a frame entry can be read from the
            # hardware, the Frame-ID of the wanted entry
            # must be set
            lFrameEntry = PLinApi.TLINFrameEntry()
            lFrameEntry.FrameId = c_ubyte(pFrame.m_nID)
            # Read the information of the specified frame entry from the
            # hardware.
            linResult = self.m_objPLinApi.GetFrameEntry(
                self.m_hHw, lFrameEntry)
            # Check the result value of the LinApi function call.
            if (linResult == PLinApi.TLIN_ERROR_OK):
                # Copy the frame entry information to the Frame Definition.
                pFrame.m_nLength = lFrameEntry.Length
                pFrame.m_bDirection = lFrameEntry.Direction
                pFrame.m_nChecksumType = lFrameEntry.ChecksumType

                if (pFrame.m_bDirection != PLinApi.TLIN_DIRECTION_DISABLED.value):
                    # If the direction is not disabled then set the
                    # bit in the client filter mask.
                    llMask = c_uint64(
                        (1 << pFrame.m_nID) & self.FRAME_FILTER_MASK.value)
                    self.m_lMask = c_uint64(self.m_lMask.value | llMask.value)
            # If the Client and Hardware handles are valid.
            if ((self.m_hClient.value != 0) and (self.m_hHw.value != 0)):
                # Set the client filter.
                self.m_objPLinApi.SetClientFilter(
                    self.m_hClient, self.m_hHw, self.m_lMask)
        # Updates the displayed frame IDs
        self.updateFrameIds()

    # Fills combobox cbbChannel with available LIN hardware
    #
    def refreshHardware(self):
        """
        Fills combobox cbbChannel with available LIN hardware

        Returns:
            None
        """
        # Get all available LIN hardware.
        availableHWsSize = c_ushort(8)
        availableHWs = (PLinApi.HLINHW * availableHWsSize.value)()
        lwCount = c_ushort(0)
        linResult = self.m_objPLinApi.GetAvailableHardware(
            availableHWs, availableHWsSize, lwCount)
        if (linResult == PLinApi.TLIN_ERROR_OK):
            # clear combobox
            self.cbbChannel.subwidget('listbox').delete(0, END)
            self.m_dataHws.clear()
            # Get information for each LIN hardware found
            lnHwType = c_int(0)
            lnDevNo = c_int(0)
            lnChannel = c_int(0)
            lnMode = c_int(0)
            if (lwCount.value == 0):
                strHw = '<No Hardware found>'
                self.cbbChannel.insert(Tix.END, strHw)
                self.m_dataHws[strHw] = PLinApi.HLINHW(0)
            else:
                for i in range(lwCount.value):
                    lwHw = availableHWs[i]
                    # Read the type of the hardware with the handle lwHw.
                    self.m_objPLinApi.GetHardwareParam(
                        lwHw, PLinApi.TLIN_HARDWAREPARAM_TYPE, lnHwType, 0)
                    # Read the device number of the hardware with the handle
                    # lwHw.
                    self.m_objPLinApi.GetHardwareParam(
                        lwHw, PLinApi.TLIN_HARDWAREPARAM_DEVICE_NUMBER, lnDevNo, 0)
                    # Read the channel number of the hardware with the handle
                    # lwHw.
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
                    # add information to channel combobox
                    strHw = str.format(
                        '{0} - dev. {1}, chan. {2}', strName, lnDevNo.value, lnChannel.value)
                    self.cbbChannel.insert(Tix.END, strHw)
                    self.m_dataHws[strHw] = PLinApi.HLINHW(lwHw)
            # select first item
            self.cbbChannel.pick(0)

# *****************************************************************
# LIN Read/write functions
# *****************************************************************

    # Reads PLIN messages
    #
    def readMessages(self):
        """
        Reads PLIN messages

        Returns:
            None
        """
        # We read at least one time the queue looking for messages.
        # If a message is found, we look again trying to find more.
        # If the queue is empty or an error occurs, we get out from
        # the dowhile statement.
        m_LastLINErr = PLinApi.TLIN_ERROR_OK
        # note: using self.btnRelease['state'] is not advised
        # to detect hardware connection as Tk may hold thread execution
        # if GUI operation are made (like moving the window)
        while (not (self.m_hHw.value == 0) and not (m_LastLINErr & PLinApi.TLIN_ERROR_RCVQUEUE_EMPTY)):
            lpMsg = PLinApi.TLINRcvMsg()
            m_LastLINErr = self.m_objPLinApi.Read(self.m_hClient, lpMsg)
            # If at least one Frame is received by the LinApi.
            # Check if the received frame is a standard type.
            # If it is not a standard type than ignore it.
            if (lpMsg.Type != PLinApi.TLIN_MSGTYPE_STANDARD.value):
                continue
            # process message
            if (m_LastLINErr == PLinApi.TLIN_ERROR_OK):
                self.processMessage(lpMsg)
    # Processes a received message, in order to show it in the Message-ListView
    #

    def processMessage(self, linMsg):
        """
        Processes a received message, in order to show it in the Message-ListView.

        Parameters:
            linMsg  a TLINRcvMsg object

        Returns:
            None
        """
        # prevent concurrency errors with timers (read and display)
        with self._lock:
            # search if a message (Same ID and Type) has
            # already been received or if this is a new message
            for msg in self.m_LastMsgsList:
                if (msg.m_Msg.FrameId == linMsg.FrameId):
                    # Modify the message and exit
                    msg.update(linMsg)
                    return
            # Message not found. It will be created
            self.insertMsgEntry(linMsg)

    # Inserts a new entry for a new message in the Message-ListView
    # Displays and updates LIN messages in the Message-ListView
    #
    def insertMsgEntry(self, newMsg):
        """
        Displays and updates LIN messages in the Message-ListView.

        Parameters:
            newMsg The messasge to be inserted

        Returns:
            None
        """
        # prevent concurrency errors with timers (read and display)
        with self._lock:
            # add this status in the last message list
            msgStsCurrentMsg = MessageStatus(newMsg, len(self.m_LastMsgsList))
            self.m_LastMsgsList.append(msgStsCurrentMsg)
            self.lstMessages.insert(
                END, text=self.formatMsgStatus(msgStsCurrentMsg))

    # Displays and updates LIN messages in the Message-ListView
    #
    def displayMessages(self):
        """
        Displays and updates LIN messages in the Message-ListView.

        Returns:
            None
        """
        # prevent concurrency errors with timers (read and display)
        with self._lock:
            for msgStatus in self.m_LastMsgsList:
                # Get the data to actualize
                if (msgStatus.m_bWasChanged):
                    try:
                        self.lstMessages.delete(msgStatus.m_iIndex)
                        self.lstMessages.insert(
                            msgStatus.m_iIndex, text=self.formatMsgStatus(msgStatus))
                        msgStatus.m_bWasChanged = False
                    except:
                        # prevent thread 'tmrDisplay' interruption
                        # by catching unexpected TclErrors
                        traceback.print_exc(file=sys.stdout)

    # Formats the object to be displayed in a ListBox
    #
    def formatMsgStatus(self, msgStatus):
        """
        Formats the object to be displayed in a ListBox

        Returns:
            the MessageStatus formatted as a string for a listBox
        """
        strId = ''
        # Search and retrieve the ID in the global frame table associated with
        # the frame Protected-ID
        for frame in self.m_pGFT.m_lFrameDefinitions:
            if (msgStatus.m_Msg.FrameId == frame.m_nProtectedID):
                strId = frame.m_nID
                break
        if strId == '':
            raise RuntimeError()
        # reminder: row is formatted as such ["ID", "|Length", "|Data",
        # "|Count", "|RcvTime", "|Direction", "|Errors"]
        result = ''
        seperatorMinimum = '| '
        # ID
        strTemp = hex(strId)
        separator = " " * (self.m_ListColSpace[self.COL_ID] - len(strTemp))
        if (len(separator) == 0):
            separator = seperatorMinimum
        result = result + (strTemp + separator)
        # Length
        strTemp = str(msgStatus.m_Msg.Length)
        separator = " " * (self.m_ListColSpace[self.COL_LEN] - len(strTemp))
        if (len(separator) == 0):
            separator = seperatorMinimum
        result = result + (strTemp + separator)
        # Data
        strTemp = msgStatus.getDataString()
        separator = " " * (self.m_ListColSpace[self.COL_DATA] - len(strTemp))
        if (len(separator) == 0):
            separator = seperatorMinimum
        result = result + (strTemp + separator)
        # Count
        strTemp = str(msgStatus.m_Count)
        separator = " " * (self.m_ListColSpace[self.COL_COUNT] - len(strTemp))
        if (len(separator) == 0):
            separator = seperatorMinimum
        result = result + (strTemp + separator)
        # RcvTime
        strTemp = str(msgStatus.getTimeString())
        separator = " " * (self.m_ListColSpace[self.COL_TIME] - len(strTemp))
        if (len(separator) == 0):
            separator = seperatorMinimum
        result = result + (strTemp + separator)
        # Direction
        strTemp = CFrameDefinition.getFrameDirectionAsString(
            msgStatus.m_Msg.Direction)
        separator = " " * (self.m_ListColSpace[self.COL_DIR] - len(strTemp))
        if (len(separator) == 0):
            separator = seperatorMinimum
        result = result + (strTemp + separator)
        # Error
        strTemp = msgStatus.getErrorString()
        separator = " " * (self.m_ListColSpace[self.COL_ERR] - len(strTemp))
        result = result + (strTemp + separator)

        return result

# *****************************************************************
# ROOT
# *****************************************************************

# Loop-Functionallity


def RunMain(root):
    # Creates a PCAN-Basic application
    plinExpl = PLinApiExample(root)
    # Runs the Application / loop-start
    plinExpl.loop()
    # Application's destrution / loop-end
    plinExpl.destroy()

if __name__ == '__main__':
    # Creates the Tkinter-extension Root
    root = Tix.Tk()
    # Uses the root to launch the PCAN-Basic Example application
    RunMain(root)
# *****************************************************************
