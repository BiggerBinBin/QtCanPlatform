unit GlobalFrameTable;

interface

uses
    Classes, PLinApi;

    const
        GLOBALFRAMETABLE_SIZE	= 64;
        
        propDirection 			= 1;
        propChecksumType		= 2;
        propLength				= 3;

    type
        // frame property type
        EFRAMEPROPERTY			= WORD;
        
        // LIN Frame Definition class for handling property changes and message communication
        CFrameDefinition = class(TObject)
            private
                // The CGlobalFrameTable holding this frame
                m_pParent: Pointer;
                // LIN-Frame Identifier
                m_nID: Integer;
                // Datalength of the LIN-Frame
                m_nLength: Integer;
                // Type of the Checksum Calculation
                m_nChecksumType: TLINChecksumType;
                // Direction of the LIN-Frame
                m_bDirection: TLINDirection;
                // Protected LIN-Frame m_nID
                m_nProtectedID: Integer;

            public
                constructor Create(
                    pParent: Pointer;	                // CGlobalFrameTable holding this frame
                    id: Integer;						// LIN-Frame ID
                    pid: Integer;						// LIN-Frame Protected ID
                    len: Integer;						// LIN-Frame length
                    checksumType: TLINChecksumType;	    // LIN-Frame checksum type
                    direction: TLINDirection);		    // LIN-Frame direction

                // Returns the LIN-Frame identifier as a string.
                function getId(): string;
                property Id: string read getId;
                // Returns the LIN-Frame identifier as an integer.
                function getIdAsInt(): Integer;
                property IdAsInt: Integer read getIdAsInt;
                // Returns the LIN-Frame identifier as a BYTE.
                function getIdAsByte(): Byte;       
                property IdAsByte: Byte read getIdAsByte;

                // Returns the protected LIN-Frame identifier as a string.
                function getProtectedId(): string;
                property ProtectedId: string read getProtectedId;
                // Returns the protected LIN-Frame identifier as an integer.
                function getProtectedIdAsInt(): Integer;
                property ProtectedIdAsInt: Integer read getProtectedIdAsInt;

                // Returns the LIN-Frame length as an integer.
                function getLength(): Integer;    
                property Length: Integer read getLength;
                // Returns the LIN-Frame length as a string.
                function getLengthAsString(): string;
                property LengthAsString: string read getLengthAsString;

                // Returns the LIN-Frame checksumType.
                function getChecksumType(): TLINChecksumType;
                property ChecksumType: TLINChecksumType read getChecksumType;
                // Returns the LIN-Frame direction.
                function getDirection(): TLINDirection;
                property Direction: TLINDirection read getDirection;

                // Setters with calls to "CallFramePropertyChanged" parent function
                //

                // Sets the LIN-FRAME length
                procedure setLength(length: Integer; doCallFramePropertyChanged:Boolean = false);
                // Sets the LIN-FRAME checksumType
                procedure setChecksumType(checksumType: TLINChecksumType; doCallFramePropertyChanged: Boolean = false);
                // Sets the LIN-FRAME direction
                procedure setDirection(direction: TLINDirection; doCallFramePropertyChanged: Boolean = false);
        end;

        PCFrameDefinition = ^CFrameDefinition;
        
        // LIN Global Frame Table class, contains and handles the 64 LIN Frames/CFrameDefinitions
        CGlobalFrameTable = class(TObject)
            private
                // LIN Client handle
                m_hClient: HLINCLIENT;
                // LIN Hardware handle
                m_hHw: HLINHW;
                // LIN Hardware Modus
                m_HwMode: TLINHardwareMode;
                // pointer to LIN Client filter mask
                m_pMask: ^UInt64;

                // Collection of all Frame Definition
                m_lFrameDefinitions: array[0..GLOBALFRAMETABLE_SIZE-1] of CFrameDefinition;

                // size of the m_lFrameDefinitions table
                function getCount(): Integer;

                // Function called when a property of CFrameDefinition is changed to assert the modification is allowed
                procedure CallFramePropertyChanged(
                    propertyType: EFRAMEPROPERTY;	// the property to modify
                    var pFrame: CFrameDefinition;	// frame to update
                    value: TObject;					// the new value of the property
                    var allowed: Boolean);			// defines if the modification is allowed or not

                // Adds a new frame definition to the global frame table. Returns the frame index in the table (the same as AId unless an error occured)
                function AddFrameDefinition(
                    AId: Integer;			                // position (and ID) of the frame in the table
                    ALength: Integer;		                // length
                    AChecksumType: TLINChecksumType;	    // checksum type
                    ADirection: TLINDirection ): Integer;	// direction

            public
                constructor Create();
                
                destructor  Destroy(); override;

                // Returns the size of the Frame Table
                property Count : Integer read getCount;

                // Returns the CFrameDefinition based on the specified index (with 0 <= index < 64)
                function get(index: Integer): PCFrameDefinition;  
                property Frames[Index: Integer]: PCFrameDefinition read get; default;

                // Returns the index in the table for the specified Frame object
                function indexOf(var pFrame: CFrameDefinition): Integer;

                // Sets internal LIN informations
                procedure UpdateLinInfo(const hClient: HLINCLIENT; const hHw: HLINHW; const hwMode: TLINHardwareMode; pMask: Pointer);
        end;


implementation

uses
    SysUtils;

//////////////////////////////////////////////////////////////////////////////////////////////
// CFrameDefinition class

constructor CFrameDefinition.Create(
    pParent: Pointer;	                // CGlobalFrameTable holding this frame
    id: Integer;						// LIN-Frame ID
    pid: Integer;						// LIN-Frame Protected ID
    len: Integer;						// LIN-Frame length
    checksumType: TLINChecksumType;	    // LIN-Frame checksum type
    direction: TLINDirection);
begin
    m_pParent := pParent;
    m_nId := id;    
    m_nProtectedID := pid;
    m_nLength := len;     
    m_nChecksumType := checksumType;
    m_bDirection := direction;
end;

function CFrameDefinition.getId(): string;
begin
    Result := Format('%.02Xh', [m_nID]);
end;

function CFrameDefinition.getIdAsInt(): Integer;
begin
    Result := m_nID;
end;

function CFrameDefinition.getIdAsByte(): Byte;   
begin
    Result := Byte(m_nID);
end;
     
function CFrameDefinition.getProtectedId(): string;   
begin
    Result := Format('%.02Xh', [m_nProtectedID]);
end;

function CFrameDefinition.getProtectedIdAsInt(): Integer;   
begin
    Result := m_nProtectedID;
end;

function CFrameDefinition.getLength(): Integer;      
begin 
    Result := m_nLength;
end;

function CFrameDefinition.getLengthAsString(): string;     
begin              
    Result := IntToStr(m_nLength);           
end;


function CFrameDefinition.getChecksumType(): TLINChecksumType;   
begin
    Result := m_nChecksumType;
end;

function CFrameDefinition.getDirection(): TLINDirection;
begin
    Result := m_bDirection;
end;

procedure CFrameDefinition.setLength(length: Integer; doCallFramePropertyChanged:Boolean = false);
var
    allowed: Boolean;
begin          
    allowed := True;
    if ((m_pParent <> nil) and (doCallFramePropertyChanged)) then
        CGlobalFrameTable(m_pParent).CallFramePropertyChanged(propLength, self, TObject(length), allowed);
    if (allowed) then                        
        m_nLength := length;
end;

procedure CFrameDefinition.setChecksumType(checksumType: TLINChecksumType; doCallFramePropertyChanged: Boolean = false);
var
    allowed: Boolean;
begin
    allowed := True;
    if ((m_pParent <> nil) and (doCallFramePropertyChanged)) then
        CGlobalFrameTable(m_pParent).CallFramePropertyChanged(propChecksumType, self, TObject(checksumType), allowed);
    if (allowed) then
        m_nChecksumType := checksumType;
end;

procedure CFrameDefinition.setDirection(direction: TLINDirection; doCallFramePropertyChanged: Boolean = false);
var
    allowed: Boolean;
begin    
    allowed := True;
    if ((m_pParent <> nil) and (doCallFramePropertyChanged)) then
        CGlobalFrameTable(m_pParent).CallFramePropertyChanged(propDirection, self, TObject(direction), allowed);
    if (allowed) then
        m_bDirection := direction;
end;



//////////////////////////////////////////////////////////////////////////////////////////////
// CGlobalFrameTable class

constructor CGlobalFrameTable.Create();
var
  i: Integer;
begin
    m_hClient := 0;
    m_hHw := 0;
    m_HwMode := modNone;
    m_pMask := nil;

    // Create all CFrameDefinition objects and set
    // them values to default. The length values is
    // set to LIN 1.2.
    // Default values:
    // - Direction = SubscriberAutoLength
    // - ChecksumType = Auto
    // - Length = see inside the loop.
    for i := 0 to GLOBALFRAMETABLE_SIZE - 1 do
    begin
        // From Frame-ID 0 to 31 set the length 2.
        if ((i >= $00) and (i <= $1F)) then
            AddFrameDefinition(i, 2, cstAuto, dirSubscriberAutoLength)
        // From Frame-ID 32 to 47 set the length 4.
        else
        begin
            if ((i >= $20) and (i <= $2F)) then
                AddFrameDefinition(i, 4, cstAuto, dirSubscriberAutoLength)
            // From Frame-ID 48 to 63 set the length 8.
            else
                if ((i >= $30) and (i <= $3F)) then
                    AddFrameDefinition(i, 8, cstAuto, dirSubscriberAutoLength);
        end;
    end;
end;      


destructor CGlobalFrameTable.Destroy();
var
  i: Integer;
begin
    for i := 0 to getCount() - 1 do
        m_lFrameDefinitions[i].Destroy();
    inherited;
end;


function CGlobalFrameTable.getCount;
begin
    Result := Length(m_lFrameDefinitions);
end;

procedure CGlobalFrameTable.CallFramePropertyChanged(
    propertyType: EFRAMEPROPERTY;	// the property to modify
    var pFrame: CFrameDefinition;	// frame to update
    value: TObject;					// the new value of the property
    var allowed: Boolean);			// defines if the modification is allowed or not
var
    lFrameEntry: TLINFrameEntry;
    lErr: TLINError ;
    lMask: UInt64;
begin
    if (pFrame <> nil) then
    begin
        // If data length is to be set, check the value.
		if (propertyType = propLength) then
            // Only a value between 0 and 8 are valid.
			allowed := ((Integer(value) >= 0) and (Integer(value) <= 8));
        // If DO NOT allow then return.
        if (not allowed) then
            Exit;

        // Only if the hardware was initialized as Slave
        // set the direction of the LIN-Frame.
        // By the Master the direction will be used with
        // the LIN_Write and do not have to set here.
        if (m_HwMode = modSlave) then
        begin
            // Temporary set Allowed to false is to easy
            // return only later.
            allowed := False;

            // Set the Frame-ID of the Frame to get and set.
            // The ID have to set before get the entry.
			lFrameEntry.FrameId := pFrame.IdAsByte;
            // Get the Frame Entry with the Frame-ID from
            // the Hardware via the LinApi.
            lErr := LIN_GetFrameEntry(m_hHw, lFrameEntry);
            // If an error occurs do not allow to change
            // the property and return.
            // The Allowed parameter was set some lines before.
            if (lErr <> errOK) then
                Exit;
            // Switch between the different kind of property types.
            case (propertyType) of
                // Direction property should be set
                propDirection:
					lFrameEntry.Direction := TLINDirection(value);
                // Length property should be set
                propLength:
                    lFrameEntry.Length := Byte(value);
                // ChecksumType property should be set
                propChecksumType:
                    lFrameEntry.ChecksumType := TLINChecksumType(value);
            end;
            
            lFrameEntry.Flags := FRAME_FLAG_RESPONSE_ENABLE;
			lErr := LIN_setFrameEntry(m_hClient, m_hHw, lFrameEntry);
            // If an error occurs do not allow to change
            // the property and return.
            // The Allowed parameter was set some lines before.
            if (lErr <> errOK) then
                Exit;

            // Temporary set Allowed to true for next check.
            // The action was successfull on this line.
            allowed := true;
        end;
        
        // If the property 'Direction' of one
        // CFrameDefinition will be changed,
        // here we need a special request to set
        // the client filter.
        if (propertyType = propDirection) then
        begin
            // If the new value for the property 'Direction'
            // should NOT be 'Disabled' check first if
            // the CFrameDefinition is defined already with some
            // other value then 'Disabled'.
            if (TLINDirection(value) <> dirDisabled) then
            begin
                if (pFrame.Direction = dirDisabled) then
                begin
                    // If the old property of CFrameDefintion
                    // was set to 'Disabled' the new value
                    // means that the Frame-ID have to add to
                    // the client filter by the LinApi.
                    // Set the client filter.
                    // The Filter is a bit mask.
                    lMask := UInt64(1) shl pFrame.IdAsInt;
                    m_pMask^ := (m_pMask^ or lMask);
                    lErr := LIN_SetClientFilter(m_hClient, m_hHw, UInt64(m_pMask^));
                    // Only allow to change the property if the Frame-ID
                    // was added successfull to the Filter.
                    allowed := (lErr = errOK);
                end;
            end
            else
            begin
                // If the value of direction should set on 'disable'
                // Remove the Frame-ID from the client filter.
				lMask := UInt64(1) shl pFrame.IdAsInt;
                m_pMask^ := (m_pMask^ and not lMask);
                lErr := LIN_SetClientFilter(m_hClient, m_hHw, UInt64(m_pMask^));
                // Only allow to change the property if the Frame-ID
                // was removed successfull from the Filter.
                allowed := (lErr = errOK);
            end;
        
        end;
    end;
end;

function CGlobalFrameTable.AddFrameDefinition(
    AId: Integer;			                // position (and ID) of the frame in the table
    ALength: Integer;		                // length
    AChecksumType: TLINChecksumType;	    // checksum type
    ADirection: TLINDirection ): Integer;	// direction
var
    lbID: Byte;
    lFD: CFrameDefinition;
begin
	// Check the Frame-ID for before adding it.
	// Only ID's from 0 to 63 are allowed.
	if ((AId < 0) or (AId > 63)) then
		// ID is invalid. Do not add it.
		Result := -1
	else
    begin
        // The delivered Frame-ID is valid.
		// Calculate the Protected-ID with the delivered Frame-ID.
		lbID := Byte(AId);
		LIN_GetPID(lbID);
		// Create a Frame Definition object.
		// and assigned the delivered values to it.
		lFD := CFrameDefinition.Create(self, AId, lbID, ALength, AChecksumType, ADirection);
		// Add the created object to the list
		// and return the position of the new added object.
		// It should be added at the end so the position
		// must be the last entry in the list.
		m_lFrameDefinitions[AId] := lFD;  
        Result := AId;
    end;
end;

function CGlobalFrameTable.get(index: Integer): PCFrameDefinition;    
begin
    Result := nil;
   	if (index >= 0) and (index < getCount() ) then
		Result := @(m_lFrameDefinitions[index]);
end;

function CGlobalFrameTable.indexOf(var pFrame: CFrameDefinition): Integer;
var
    i: Integer;
begin
    Result := -1;
	for i := 0 to getCount - 1 do
		if (m_lFrameDefinitions[i] = pFrame) then
        begin
			Result := i;
            exit;
        end;
end;

procedure CGlobalFrameTable.UpdateLinInfo(const hClient: HLINCLIENT; const hHw: HLINHW; const hwMode: TLINHardwareMode; pMask: Pointer);
begin
    // up-to-date LIN information is required by function CallFramePropertyChanged
	m_hClient := hClient;
	m_hHw := hHw;
	m_HwMode := hwMode;
	m_pMask := pMask;
end;
   
end.
