Imports PLIN_API_Example
Imports PLIN_API_Example.PLIN_API_Example

''' <summary>
'''  This class handles a dialog that displays and manages the LIN Global Frame Table
''' </summary>
''' <remarks></remarks>
Public Class Frame_Dlg

#Region "Members"
    ''' <summary>
    ''' Global Frame Table object that holds all frame definition.
    ''' </summary>
    Private m_pGFT As CGlobalFrameTable
    ''' <summary>
    ''' Boolean to register the current edit mode of different controls.
    ''' To protect the control's event for recursive calling.
    ''' </summary>
    Private m_fAutomatic As Boolean

#End Region

    Public Sub New(ByVal pGFT As CGlobalFrameTable)
        ' This call is required by the Windows Form Designer.
        InitializeComponent()
        ' Initialize the list view component with items from the global frame table object
        m_pGFT = pGFT
        FillListViews()
    End Sub


#Region "helper methods"
    ''' <summary>
    ''' Returns the Frame Direction as a human readable string
    ''' </summary>
    ''' <param name="direction">the direction object</param>
    ''' <returns>The direction as a string</returns>
    Private Function getDirectionAsString(ByVal direction As Peak.Lin.TLINDirection) As String
        Select (direction)
            Case Peak.Lin.TLINDirection.dirDisabled
                Return My.Resources.SLinDirectionDisabled
            Case Peak.Lin.TLINDirection.dirPublisher
                Return My.Resources.SLinDirectionPublisher
            Case Peak.Lin.TLINDirection.dirSubscriber
                Return My.Resources.SLinDirectionSubscriber
            Case Peak.Lin.TLINDirection.dirSubscriberAutoLength
                Return My.Resources.SLinDirectionAuto
        End Select

        Return ""
    End Function

    ''' <summary>
    ''' Returns the Frame checksum type as a human readable string
    ''' </summary>
    ''' <param name="checksumType">the checksumType object</param>
    ''' <returns>The checksumType as a string</returns>
    Private Function getChecksumAsString(ByVal checksumType As Peak.Lin.TLINChecksumType) As String
        Select Case (checksumType)
            Case Peak.Lin.TLINChecksumType.cstAuto
                Return My.Resources.SLinCSTAuto
            Case Peak.Lin.TLINChecksumType.cstClassic
                Return My.Resources.SLinCSTClassic
            Case Peak.Lin.TLINChecksumType.cstEnhanced
                Return My.Resources.SLinCSTEnhanced
            Case Peak.Lin.TLINChecksumType.cstCustom
                Return My.Resources.SLinCSTCustom
        End Select

        Return ""
    End Function

#End Region

#Region "ListView event-handlers"

    Private Sub lvGFT_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles lvGFT.SelectedIndexChanged
        Dim nCnt, nIdx As Integer

        If (Not m_fAutomatic) Then
            m_fAutomatic = True
            ' Get the count of selected elements
            ' in the ListView 'lvGFT'.
            ' Each element is assigned with an
            ' element in the CGlobalFrameTable.
            nCnt = lvGFT.SelectedItems.Count
            If (nCnt = 1) Then
                ' If only one element is selected
                ' it can be assigned directly to the
                ' Property-Control.
                nIdx = lvGFT.SelectedItems(0).Index
                ' Assign the CFrameDefinition element
                ' from the CGlobalFrameTable.
                pgGFTDef.SelectedObject = m_pGFT(nIdx)
            ElseIf (nCnt > 1) Then
                ' If more than one element are selected
                ' in the ListView means that the user
                ' want to change at least one property of
                ' all elements to a new value with
                ' the Property-Control.
                ' So get first the selected elements and the
                ' assigned objects from the CGlobalFrameTable
                ' into a local Array of elements.
                Dim lDefFrames(nCnt - 1) As CFrameDefinition
                For i As Integer = 0 To nCnt - 1
                    nIdx = lvGFT.SelectedItems(i).Index
                    lDefFrames(i) = m_pGFT(nIdx)
                Next
                ' Assign the local array of elements
                ' from the CGlobalFrameTable.
                pgGFTDef.SelectedObjects = lDefFrames
            End If
            m_fAutomatic = False
        End If
    End Sub

    Private Sub lvGFT_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles lvGFT.KeyDown
        ' Adds "select all items" (Ctrl-A) feature
        If (e.KeyCode = Keys.A And e.Control) Then
            ' Lock the ListView.
            lvGFT.BeginUpdate()
            ' avoid to call multiple times event "lvGFT_SelectedIndexChanged"
            m_fAutomatic = True
            For Each item As ListViewItem In lvGFT.Items
                item.Selected = True
            Next
            m_fAutomatic = False
            ' Unlock the ListView
            lvGFT.EndUpdate()
            ' call selection change
            lvGFT_SelectedIndexChanged(Me, New EventArgs())
        End If
    End Sub


    ''' <summary>
    ''' Fills the Listview 'Global Frame Table' with the data from CGloabalFrameTable.
    ''' </summary>
    Private Sub FillListViews()
        Dim lvi As ListViewItem
        '' Lock the ListView.
        lvGFT.BeginUpdate()
        ' Clear the ListView that
        ' will show the Frame Definition of the
        ' Global Frame Table.
        lvGFT.Items.Clear()
        ' Add every object, Frame Definition, from the
        ' Global Frame Table into the ListView.
        For i As Integer = 0 To m_pGFT.Count - 1
            lvi = lvGFT.Items.Add(Convert.ToString(m_pGFT(i).ID))
            lvi.SubItems.Add(Convert.ToString(m_pGFT(i).ProtectedID))
            lvi.SubItems.Add(getDirectionAsString(m_pGFT(i).Direction))
            lvi.SubItems.Add(Convert.ToString(m_pGFT(i).Length))
            lvi.SubItems.Add(getChecksumAsString(m_pGFT(i).ChecksumType))
        Next
        ' Unlock and update the ListView now.
        lvGFT.EndUpdate()
    End Sub


#End Region

#Region "PropertyGrid 'pgGFTDef'"

    Private Sub pgGFTDef_PropertyValueChanged(ByVal s As System.Object, ByVal e As System.Windows.Forms.PropertyValueChangedEventArgs) Handles pgGFTDef.PropertyValueChanged
        Dim nCnt, nIdx As Integer
        Dim lFD As CFrameDefinition
        Dim lvi As ListViewItem

        ' Lock the ListView for update elements.
        lvGFT.BeginUpdate()
        ' Get the selected count of CFrameDefinition.
        nCnt = pgGFTDef.SelectedObjects.GetLength(0)
        ' For each selected CFrameDefinition in
        ' the ListView, update their property values.
        For i As Integer = 0 To nCnt - 1
            ' Get the selected CFrameDefinition X from the
            ' Property-Control that has changed the
            ' property values.
            lFD = DirectCast(pgGFTDef.SelectedObjects(i), CFrameDefinition)
            If (lFD IsNot Nothing) Then
                ' Get the position in the ListView of
                ' the CFrameDefinition to refresh the
                ' property value.
                nIdx = m_pGFT.IndexOf(lFD)
                If ((nIdx >= 0) And (nIdx < lvGFT.Items.Count)) Then
                    ' Get the item that is assign with the
                    ' CFrameDefinition from ListView 'lvGFT'
                    ' and refesh it.
                    lvi = lvGFT.Items(nIdx)
                    lvi.SubItems(1).Text = Convert.ToString(lFD.ProtectedID)
                    lvi.SubItems(2).Text = getDirectionAsString(lFD.Direction)
                    lvi.SubItems(3).Text = Convert.ToString(lFD.Length)
                    lvi.SubItems(4).Text = getChecksumAsString(lFD.ChecksumType)
                End If
            End If
        Next
        ' Free the lock of ListView and
        ' update all elements now.
        lvGFT.EndUpdate()
    End Sub

#End Region

End Class
