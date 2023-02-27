unit Frame_Dlg;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Grids, ValEdit, ComCtrls,
  PLinApi, GlobalFrameTable;

Resourcestring
    SLinCSTAuto             = 'Auto';
    SLinCSTClassic          = 'Classic';
    SLinCSTCustom           = 'Custom';
    SLinCSTEnhanced         = 'Enhanced';
    SLinDirectionAuto       = 'Subscriber Automatic Length';
    SLinDirectionDisabled   = 'Disabled';
    SLinDirectionPublisher  = 'Publisher';
    SLinDirectionSubscriber = 'Subscriber';

type
    TFrameDlg = class(TForm)
        Label1: TLabel;
        lvGFT: TListView;
        Label2: TLabel;
        pgGFTDef: TValueListEditor;
    procedure lvGFTSelectItem(Sender: TObject; Item: TListItem;
      Selected: Boolean);
    procedure pgGFTDefStringsChange(Sender: TObject);
    procedure lvGFTKeyDown(Sender: TObject; var Key: Word; Shift: TShiftState);

        private                                    
            // Global Frame Table object that holds all frame definition.
            m_pGFT: CGlobalFrameTable;
            /// Boolean to register the current edit mode of different controls.
            /// To protect the control's event for recursive calling.
            m_fAutomatic: Boolean;
            // Stores the selected frames in the Frame List Control
            m_pSelectedFrames: TList;

            /// Fills the Listview 'Global Frame Table' with the data from CGloabalFrameTable.
            procedure FillListView();
            // Updates the property grid with the selected frames from m_pSelectedFrames
            procedure UpdatePropertyGrid();
            /// Returns the Frame Direction as a human readable string
            function getDirectionAsString(direction: TLINDirection): string;
            /// Returns the Frame checksum type as a human readable string
            function getChecksumAsString(checksumType: TLINChecksumType): string;

        public
            constructor Create(var pGFT:CGlobalFrameTable; AOwner: TComponent); reintroduce;
            destructor  Destroy(); override;

  end;

var
  FrameDlg: TFrameDlg;

implementation

{$R *.dfm}


const 
    propChecksumType    = 'Checksum Type';
    propDirection       = 'Direction Type';
    propLength          = 'Length';
    propID              = 'ID';
    propPID             = 'Protected ID';

constructor TFrameDlg.Create(var pGFT:CGlobalFrameTable; AOwner: TComponent);
var
    i, j: Integer;
begin
    inherited Create(AOwner);
    m_pGFT := pGFT;
    m_pSelectedFrames := TList.Create();
    // lock updates to prevent unnecessary events
    m_fAutomatic := true;
    FillListView();                              
    // changeable properties : Checksum Type
    i := pgGFTDef.Strings.Add(propChecksumType+'=');  
    pgGFTDef.ItemProps[i].EditStyle := esPickList;
    pgGFTDef.ItemProps[i].ReadOnly := false;
    pgGFTDef.ItemProps[i].PickList.AddObject(getChecksumAsString(cstAuto), TObject(cstAuto));
    pgGFTDef.ItemProps[i].PickList.AddObject(getChecksumAsString(cstClassic), TObject(cstClassic));
    pgGFTDef.ItemProps[i].PickList.AddObject(getChecksumAsString(cstEnhanced), TObject(cstEnhanced));
    pgGFTDef.ItemProps[i].PickList.AddObject(getChecksumAsString(cstCustom), TObject(cstCustom));
    // changeable properties : Direction
    i := pgGFTDef.Strings.Add(propDirection+'=');  
    pgGFTDef.ItemProps[i].EditStyle := esPickList;
    pgGFTDef.ItemProps[i].ReadOnly := false;
    pgGFTDef.ItemProps[i].PickList.AddObject(getDirectionAsString(dirDisabled), TObject(dirDisabled));
    pgGFTDef.ItemProps[i].PickList.AddObject(getDirectionAsString(dirPublisher), TObject(dirPublisher));
    pgGFTDef.ItemProps[i].PickList.AddObject(getDirectionAsString(dirSubscriber), TObject(dirSubscriber));
    pgGFTDef.ItemProps[i].PickList.AddObject(getDirectionAsString(dirSubscriberAutoLength), TObject(dirSubscriberAutoLength));
    // changeable properties : Length
    i := pgGFTDef.Strings.Add(propLength+'=');    
    pgGFTDef.ItemProps[i].EditStyle := esPickList;
    pgGFTDef.ItemProps[i].ReadOnly := false;
    for j := 1 to 8 do
        pgGFTDef.ItemProps[i].PickList.AddObject(IntToStr(j), TObject(j));
    // read only properties : ID
    i := pgGFTDef.Strings.Add(propID+'=');    
    pgGFTDef.ItemProps[i].EditStyle := esSimple;
    pgGFTDef.ItemProps[i].ReadOnly := true;
    // read only properties : Protected ID
    i := pgGFTDef.Strings.Add(propPID+'=');         
    pgGFTDef.ItemProps[i].EditStyle := esSimple;
    pgGFTDef.ItemProps[i].ReadOnly := true;
    
    // remove lock
    m_fAutomatic := False;
end;

destructor TFrameDlg.Destroy();
begin
    m_pSelectedFrames.Destroy();
    inherited;
end;

procedure TFrameDlg.FillListView();
var
    i: Integer;
    CurrentItem: TListItem;
begin
    // Lock the ListView.
	lvGFT.Items.BeginUpdate();
    try
        // Clear the ListView that
        // will show the Frame Definition of the
        // Global Frame Table.
        lvGFT.Items.Clear();
        // Add every object, Frame Definition, from the
        // Global Frame Table into the ListView.
        for i := 0 to m_pGFT.Count - 1 do
        begin
            // Add the new ListView Item with the Type of the message
            CurrentItem := lvGFT.Items.Add();
            CurrentItem.Caption := m_pGFT[i].Id;
            CurrentItem.SubItems.Add(m_pGFT[i].ProtectedId);
            CurrentItem.SubItems.Add(getDirectionAsString(m_pGFT[i].Direction));
            CurrentItem.SubItems.Add(m_pGFT[i].LengthAsString); 
            CurrentItem.SubItems.Add(getChecksumAsString(m_pGFT[i].ChecksumType));
        end;
    finally
        // Unlock and update the ListView now.
	lvGFT.Items.EndUpdate();
    end;
        
end;

procedure TFrameDlg.UpdatePropertyGrid();
var
    frame: PCFrameDefinition;
	direction: TLINDirection ;
	checksumType: TLINChecksumType;
	bSameDirection, bSameChecksumType, bSameLength: Boolean;
	len: Integer;
    i: Integer;
begin
    if (m_pSelectedFrames.Count = 0) then
    begin
        // reset "property grid" values
        pgGFTDef.Values[propChecksumType] := '';
        pgGFTDef.Values[propDirection] := '';
        pgGFTDef.Values[propLength] := '';
        pgGFTDef.Values[propID] := '';
        pgGFTDef.Values[propPID] := '';
    end
    else
    begin
        if (m_pSelectedFrames.Count = 1) then
        begin
            // fill "property grid" with selected frame  values
            frame := PCFrameDefinition(m_pSelectedFrames.Items[0]);
            pgGFTDef.Values[propChecksumType] := getChecksumAsString(frame.ChecksumType);
            pgGFTDef.Values[propDirection] := getDirectionAsString(frame.Direction);
            pgGFTDef.Values[propLength] := frame.LengthAsString;
            pgGFTDef.Values[propID] := frame.Id;
            pgGFTDef.Values[propPID] := frame.ProtectedId;
        end
        else
        begin
            // get the first selected frame
            frame := PCFrameDefinition(m_pSelectedFrames.Items[0]);
            // get its properties
            checksumType := frame.ChecksumType;
            direction := frame.Direction;
            len := frame.Length;		              	
            // We have to loop through all selected frames to 
            // search for identical properties in the selected frames.
            // If values for a property are different a blank value is displayed
            bSameDirection := True;
            bSameChecksumType := True;
            bSameLength := True;
            for i := 0 to m_pSelectedFrames.Count - 1 do
            begin
                frame := PCFrameDefinition(m_pSelectedFrames.Items[i]);  
                if (checksumType <> frame.ChecksumType) then
                    bSameChecksumType := False;
                if (direction <> frame.Direction) then
                    bSameDirection := False;
                if (len <> frame.Length) then 
    				bSameLength := False;            
            end;
            frame := PCFrameDefinition(m_pSelectedFrames.Items[0]);	
            // if all the frames have the same CST, display it otherwise reset to the original blank value
            if (bSameChecksumType) then
                pgGFTDef.Values[propChecksumType] := getChecksumAsString(frame.ChecksumType)
            else 
                pgGFTDef.Values[propChecksumType] := '';
            // if all the frames have the same direction, display it otherwise reset to the original blank value
            if (bSameDirection) then
                pgGFTDef.Values[propDirection] := getDirectionAsString(frame.Direction)
            else 
                pgGFTDef.Values[propDirection] := '';
            // if all the frames have the same length, display it otherwise reset to the original blank value
            if (bSameLength) then 
                pgGFTDef.Values[propLength] := frame.LengthAsString
            else 
                pgGFTDef.Values[propLength] := ''; 
            // those properties are always different, reset to the original blank value
            pgGFTDef.Values[propID] := '';
            pgGFTDef.Values[propPID] := '';        
        end;
    end;
end;

function TFrameDlg.getDirectionAsString(direction: TLINDirection): string;
begin
    Result := '';

    Case direction of
        dirDisabled:
            Result := SLinDirectionDisabled;
        dirPublisher:
            Result := SLinDirectionPublisher;
        dirSubscriber:
            Result := SLinDirectionSubscriber;
        dirSubscriberAutoLength:
            Result := SLinDirectionAuto;
    End;
end;

function TFrameDlg.getChecksumAsString(checksumType: TLINChecksumType): string;
begin
    Result := '';

    case checksumType of
        cstCustom:
            Result := SLinCSTCustom;
        cstClassic:
            Result := SLinCSTClassic;
        cstEnhanced:
            Result := SLinCSTEnhanced;
        cstAuto:
            Result := SLinCSTAuto;
    end;
end;

procedure TFrameDlg.lvGFTKeyDown(Sender: TObject; var Key: Word;
  Shift: TShiftState);
begin
    // select all items
    if (Key = Ord('A')) and (ssCtrl in Shift) then
    begin
        TListView(Sender).SelectAll;
        Key := 0;
   end;
end;

procedure TFrameDlg.lvGFTSelectItem(Sender: TObject; Item: TListItem;
  Selected: Boolean);
var
    nIdx : Integer;
    curItem: TListItem;
begin

    if (Not m_fAutomatic) then
    begin
        m_fAutomatic := True;
		// Clear the selected frames list
		m_pSelectedFrames.Clear();
        // Get the count of selected elements
        // in the ListView 'lvGFT'.
        // Each element is assigned with an
        // element in the CGlobalFrameTable.
        curItem := lvGFT.Selected;
        if Assigned(curItem) then
        begin
    		// add all items to the internal item selection list
            nIdx := lvGFT.Items.IndexOf(curItem);
            m_pSelectedFrames.Add(m_pGFT[nIdx]);
            curItem := lvGFT.GetNextItem(curItem, sdAll, [isSelected]);
            // if there are more than one
            while Assigned(curItem) do
            begin
                nIdx := lvGFT.Items.IndexOf(curItem);
                m_pSelectedFrames.Add(m_pGFT[nIdx]);
                curItem := lvGFT.GetNextItem(curItem, sdAll, [isSelected]);
            end;
        end;
        // Update the PropertyGrid control with the newly selected frames
		UpdatePropertyGrid();
        m_fAutomatic := False;
    end;
end;

procedure TFrameDlg.pgGFTDefStringsChange(Sender: TObject);
var
    value: string;
	direction: TLINDirection ;
	checksumType: TLINChecksumType;
	length, i: Integer;
    frame: PCFrameDefinition; 
    CurrentItem: TListItem;
    bSetLen, bSetCST, bSetDir: Boolean;
begin
    // avoid internal calls
    if (Not m_fAutomatic) then
    begin    
        m_fAutomatic := true;
        // if no frame is selected, only update propertyGrid to display original/blank values and return
        if (m_pSelectedFrames.Count = 0) then
        begin
            // update and return
            UpdatePropertyGrid();
            // remove lock
            m_fAutomatic := False;
            exit;
        end;

        // since the event do not state which row was changed
        // the following booleans define wether the values are valid or not
        // (needed for multiple row selection)
        bSetDir := False;
        bSetCST := False;
        bSetLen := False;    
        checksumType := cstAuto;
        direction := dirSubscriberAutoLength;
        length := 0;
                       
        // Checksum property changed
        value := pgGFTDef.Values[propChecksumType];
        if (value <> '') then
        begin
            bSetCST := True;
            if value = getChecksumAsString(cstCustom) then
                checksumType := cstCustom
            else if value = getChecksumAsString(cstClassic) then
                checksumType := cstClassic
            else if value = getChecksumAsString(cstEnhanced) then
                checksumType := cstEnhanced
            else if value = getChecksumAsString(cstAuto) then
                checksumType := cstAuto;
        end;
            
        // Direction property changed
        value := pgGFTDef.Values[propDirection];  
        if (value <> '') then
        begin            
            bSetDir := True;
            if value = getDirectionAsString(dirDisabled) then
                direction := dirDisabled
            else if value = getDirectionAsString(dirPublisher) then
                direction := dirPublisher
            else if value = getDirectionAsString(dirSubscriber) then 
                direction := dirSubscriber   
            else if value = getDirectionAsString(dirSubscriberAutoLength) then      
                direction := dirSubscriberAutoLength;
        end;
            
        // Length property changed
        value := pgGFTDef.Values[propLength]; 
        if (value <> '') then
        begin
            if TryStrToInt(value, length) then
                bSetLen := True;
        end;

        for i := 0 to m_pSelectedFrames.Count - 1 do
        begin
            //
            frame := PCFrameDefinition(m_pSelectedFrames.Items[i]);
            // Update CST : avoid unnecessary updates
            if (bSetCST = True) and (frame.ChecksumType <> checksumType) then
            begin
            	// call for propertyChanged to update dialog controls
                frame.setChecksumType(checksumType, True);
                // if changes were not validated, rollback/update display
                if (frame.ChecksumType <> checksumType) then
				    UpdatePropertyGrid();
            end;
            // Update Direction : avoid unnecessary updates
            if (bSetDir = True) and (frame.Direction <> direction) then
            begin
            	// call for propertyChanged to update dialog controls
                frame.setDirection(direction, True);
                // if changes were not validated, rollback/update display
                if (frame.Direction <> direction) then
				    UpdatePropertyGrid();
            end;
            // Update length : avoid unnecessary updates
            if (bSetLen = True) and (frame.Length <> length) then
            begin
            	// call for propertyChanged to update dialog controls
                frame.setLength(length, True);
                // if changes were not validated, rollback/update display
                if (frame.Length <> length) then
				    UpdatePropertyGrid();
            end;

            // update the frame in the list view
            lvGFT.Items.BeginUpdate();
            try
                CurrentItem := lvGFT.Items[frame.getIdAsInt()];
                CurrentItem.SubItems[0] := frame.ProtectedId;
                CurrentItem.SubItems[1] := getDirectionAsString(frame.Direction);
                CurrentItem.SubItems[2] := frame.LengthAsString;
                CurrentItem.SubItems[3] := getChecksumAsString(frame.ChecksumType);
            finally
                lvGFT.Items.EndUpdate();
            end;
        end;
        // remove lock
        m_fAutomatic := False;
    end;               
end;

end.
