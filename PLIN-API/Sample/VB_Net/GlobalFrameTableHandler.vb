Imports System
Imports System.Collections.Generic
Imports System.ComponentModel

Namespace PLIN_API_Example

#Region "Event Parameter Definition"

    ''' <summary>
    ''' Represents an event class for property change events.
    ''' </summary>
    Friend Class PropertyChangeEventArg
        Inherits System.EventArgs

        ''' <summary>
        ''' Type of property to change.
        ''' </summary>
        Public Type As EProperty
        ''' <summary>
        ''' New value for the property.
        ''' </summary>
        Public Value As Integer
        ''' <summary>
        ''' Boolean value to indicate if the property can be set.
        ''' </summary>
        Public Allowed As Boolean
    End Class

#End Region

#Region "Delegate Definition"

    ''' <summary>
    ''' Declares the delegate function of the property change event.
    ''' </summary>
    ''' <param name="sender">The source of the event.</param>
    ''' <param name="e">A PropertyChangeEventArg that contains the event data.</param>
    Friend Delegate Sub PropertyChangeEventHandler(ByVal sender As Object, ByVal e As PropertyChangeEventArg)

#End Region

#Region "Enums"

    ''' <summary>
    ''' Represents the enumerate values of the property kind from the
    ''' CFrameDefinition object.
    ''' </summary>
    Friend Enum EProperty
        ''' <summary>
        ''' Length property
        ''' </summary>
        Length = 0
        ''' <summary>
        ''' Checksum type property
        ''' </summary>
        ChecksumType = 1
        ''' <summary>
        ''' Direction property
        ''' </summary>
        Direction = 2
    End Enum

#End Region

#Region "CDispatch Class"
    ''' <summary>
    ''' Represents a Dispatch class that can be inherited by
    ''' other classes in order to dispatch messages to
    ''' another class or object.
    ''' </summary>
    Public Class CDispatch
#Region "Attributs"
        ''' <summary>
        ''' The owner of the dispatch class.
        ''' </summary>
        Protected m_pOwner As ISynchronizeInvoke
        ''' <summary>
        ''' Indicates if the owner is set.
        ''' </summary>
        Protected m_fSynchronized As Boolean
#End Region

#Region "Methods"
        ''' <summary>
        ''' Makes the choice how should be referenced an event according with the selected 
        ''' environment (if will be direct invoked or added to the invoke list of a 
        ''' synchronization control)
        ''' </summary>
        ''' <param name="ToDispatch">The delegate to be invoked</param>
        ''' <param name="parameters">the parameters of the given delegate</param>
        Protected Sub DispatchEvent(ByRef ToDispatch As System.Delegate, ByRef parameters As Object())
            Try
                If ToDispatch = Nothing Then
                    If m_fSynchronized Then
                        If m_pOwner.InvokeRequired Then
                            m_pOwner.Invoke(ToDispatch, parameters)
                            Return
                        End If
                        ToDispatch.DynamicInvoke(parameters)
                    End If
                End If
            Catch ex As Exception
                Throw New System.Exception(String.Format("Error dispatching event: '{0}'.  Error Message: '{1}'", ToDispatch.Method.Name, ex.Message))
            End Try

        End Sub


        ''' <summary>
        ''' Constructor to create a base class which can dispatch message like
        ''' events through different threads.
        ''' </summary>
        Friend Sub New()
            ' In the base class the attibuts can be null.
            m_pOwner = Nothing
            m_fSynchronized = False
        End Sub

#End Region

    End Class

#End Region

#Region "CFrameDefinition Class"

    ''' <summary>
    ''' Represent the global Definition of a LIN-Frame.
    ''' </summary>
    Friend Class CFrameDefinition

#Region "Attributes"
        Friend m_pParent As CGlobalFrameTable 'Object
        ''' <summary>
        ''' LIN-Frame Identifier
        ''' </summary>
        Friend m_nID As Integer
        ''' <summary>
        ''' Datalength of the LIN-Frame
        ''' </summary>
        Friend m_nLength As Integer
        ''' <summary>
        ''' Type of the Checksum Calculation
        ''' </summary>
        Friend m_nChecksumType As Peak.Lin.TLINChecksumType
        ''' <summary>
        ''' The m_bDirection of the LIN-Frame
        ''' </summary>
        Friend m_bDirection As Peak.Lin.TLINDirection
        ''' <summary>
        ''' Is the protected LIN-Frame m_nID
        ''' </summary>
        Friend m_nProtectedID As Integer
#End Region

#Region "Properties"
        ''' <summary>
        ''' Gets the Frame-ID as integer value.
        ''' </summary>
        Friend ReadOnly Property IdAsInt() As Integer
            Get
                Return m_nID
            End Get
        End Property
        ''' <summary>
        ''' Gets the Frame-ID as byte value.
        ''' </summary>
        Friend ReadOnly Property IdAsByte() As Byte
            Get
                Return Convert.ToByte(m_nID)
            End Get
        End Property
        ''' <summary>
        ''' Gets the Protected Frame-ID as integer value.
        ''' This value includes the parity of the LIN-Frame.
        ''' </summary>
        Friend ReadOnly Property ProtectedIdAsInt() As Integer
            Get
                Return m_nProtectedID
            End Get
        End Property

        <DisplayName("ID")> _
        <Category("ReadOnly")> _
        <Description("Defines a LIN-Frame identifier of the Frame definition and can be between 00h and 3Fh.")> _
        Public ReadOnly Property ID() As String
            Get
                Return String.Format("{0:X2}h", m_nID)
            End Get
        End Property

        <DisplayName("Length")> _
        <Category("Changeable")> _
        <Description("Defines the length of the LIN-Frame definition.")> _
        Public Property Length() As Integer
            Get
                Return m_nLength
            End Get
            Set(ByVal value As Integer)
                If (m_nLength <> value) Then
                    Dim fAllowed As Boolean = True
                    DirectCast(m_pParent, CGlobalFrameTable).CallPropertyChangeHandler(Me, EProperty.Length, value, fAllowed)
                    If (fAllowed = True) Then
                        m_nLength = value
                    End If
                End If
            End Set
        End Property

        <DisplayName("Checksum Type")> _
        <Category("Changeable")> _
        <Description("Defines the type for the checksum calculation of the LIN-Frame definition and can be classic, enhanced or auto.")> _
        Public Property ChecksumType() As Peak.Lin.TLINChecksumType
            Get
                Return m_nChecksumType
            End Get
            Set(ByVal value As Peak.Lin.TLINChecksumType)
                If m_nChecksumType <> value Then
                    Dim fAllowed As Boolean = True
                    DirectCast(m_pParent, CGlobalFrameTable).CallPropertyChangeHandler(Me, EProperty.ChecksumType, value, fAllowed)
                    If (fAllowed = True) Then
                        m_nChecksumType = value
                    End If
                End If
            End Set
        End Property

        <DisplayName("Direction")> _
        <Category("Changeable")> _
        <Description("Defines the direction of the LIN-Frame definition and can be publisher or subscriber. Default is subscriber.")> _
        Public Property Direction() As Peak.Lin.TLINDirection
            Get
                Return m_bDirection
            End Get
            Set(ByVal value As Peak.Lin.TLINDirection)
                If m_bDirection <> value Then
                    Dim fAllowed As Boolean = True
                    DirectCast(m_pParent, CGlobalFrameTable).CallPropertyChangeHandler(Me, EProperty.Direction, value, fAllowed)
                    If (fAllowed = True) Then
                        m_bDirection = value
                    End If
                End If
            End Set
        End Property

        <DisplayName("Protected ID")> _
        <Category("ReadOnly")> _
        <Description("Defines the protected LIN-Frame identifier.")> _
        Public ReadOnly Property ProtectedID() As String
            Get
                Return String.Format("{0:X2}h", m_nProtectedID)
            End Get
        End Property

#End Region

    End Class

#End Region

#Region "CGlobalFrameTable Class"

    ''' <summary>
    ''' A class that holds the LIN Global Frames table, that contains
    ''' a list of 64 CFrameDefinition objects. It inherits CDispatch class 
    ''' in order to dispatch messages when CFrameDefinition objects are modified
    ''' </summary>
    Public Class CGlobalFrameTable
        Inherits CDispatch

#Region "Attibuts"
        ''' <summary>
        ''' Collection of all Frame Definition
        ''' </summary>
        Private m_lFrameDefinitions As List(Of CFrameDefinition)
#End Region

#Region "Methods"

        Public Sub New(ByRef AOwner As ISynchronizeInvoke)
            ' Initialize the inherited attributes from the
            ' base class CDispatch.
            m_pOwner = AOwner
            m_fSynchronized = (m_pOwner IsNot Nothing)
            ' Create a list to handle the CFrameDefinition objects.
            m_lFrameDefinitions = New List(Of CFrameDefinition)(64)
            ' Create all CFrameDefinition objects and set
            ' them values to default. The length values is
            ' set to LIN 1.2.
            ' Default values:
            ' - Direction = SubscriberAutoLength
            ' - ChecksumType = Auto
            ' - Length = see inside the loop.
            For i As Integer = 0 To 63
                ' From Frame-ID 0 to 31 set the length 2.
                If ((i >= &H0) And (i <= &H1F)) Then
                    AddFrameDefinition(i, 2, Peak.Lin.TLINChecksumType.cstAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength)
                    ' From Frame-ID 32 to 47 set the length 4.
                ElseIf ((i >= &H20) And (i <= &H2F)) Then
                    AddFrameDefinition(i, 4, Peak.Lin.TLINChecksumType.cstAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength)
                    ' From Frame-ID 48 to 63 set the length 8.
                ElseIf ((i >= &H30) And (i <= &H3F)) Then
                    AddFrameDefinition(i, 8, Peak.Lin.TLINChecksumType.cstAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength)
                End If
            Next

        End Sub

        ''' <summary>
        ''' Adds an CFrameDefinition object to the end of the CGlobalFrameTable.
        ''' </summary>
        ''' <param name="AFrameDefinition">The CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        ''' <returns>Returns the Index of position in the list.</returns>
        Private Function AddFrameDefinition(ByRef AFrameDefinition As CFrameDefinition) As Integer
            ' Check the parameter at first.
            If (AFrameDefinition Is Nothing) Then
                ' Delivered object invalid. Do not add it.
                Return -1
            Else
                ' Add the delivered object (CFrameDefinition) to the list.
                m_lFrameDefinitions.Add(AFrameDefinition)
                ' Return the position of the new added object.
                ' It should be added at the end so the position
                ' must be the last entry in the list.
                Return m_lFrameDefinitions.Count - 1
            End If
        End Function


        ''' <summary>
        ''' Adds an CFrameDefinition object with the defined values to the end of the CGlobalFrameTable.
        ''' </summary>
        ''' <param name="AId">The Frame ID of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        ''' <param name="ALength">The Frame Length of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        ''' <param name="AChecksumType">The Frame Checksum Type of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        ''' <param name="ADirection">The Frame Direction of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        ''' <returns>Returns the Index of position in the list.</returns>
        Private Function AddFrameDefinition(ByVal AId As Integer, ByVal ALength As Integer, ByVal AChecksumType As Peak.Lin.TLINChecksumType, ByVal ADirection As Peak.Lin.TLINDirection) As Integer
            ' Check the Frame-ID for adding.
            ' Only ID's from 0 to 63 are allowed to add.
            If ((AId < 0) Or (AId > 63)) Then
                ' ID is invalid. Do not add it.
                Return -1
            Else
                ' The delivered Frame-ID is valid.
                Dim lFD As CFrameDefinition
                Dim lbID As Byte

                ' Create a Frame Definition object.
                ' and assigned the delivered values to it.
                lFD = New CFrameDefinition()
                lFD.m_pParent = Me
                lFD.m_nID = AId
                lFD.m_nLength = ALength
                lFD.m_nChecksumType = AChecksumType
                lFD.m_bDirection = ADirection
                ' Calculate the Protected-ID with
                ' the delivered Frame-ID.
                lbID = Convert.ToByte(AId)
                Peak.Lin.PLinApi.GetPID(lbID)
                ' Assign the calculated Protected-ID.
                lFD.m_nProtectedID = lbID
                ' Add the created object to the list.
                m_lFrameDefinitions.Add(lFD)
            End If
            ' Return the position of the new added object.
            ' It should be added at the end so the position
            ' must be the last entry in the list.
            Return m_lFrameDefinitions.Count - 1
        End Function

#End Region

#Region "Properties"
        ''' <summary>
        ''' </summary>
        ''' <param name="Index">The zero-based index of the CFrameDefinition to get or set.</param>
        Default Friend Property Item(ByVal Index As Integer) As CFrameDefinition
            Get
                Return m_lFrameDefinitions(Index)
            End Get
            Set(ByVal value As CFrameDefinition)
                m_lFrameDefinitions(Index) = value
            End Set
        End Property

        ''' <summary>
        ''' Gets the number of CFrameDefinition objects actually contained in the CGlobalFrameTable.
        ''' </summary>
        Friend ReadOnly Property Count() As Integer
            Get
                Return m_lFrameDefinitions.Count
            End Get
        End Property
#End Region

        ''' <summary>
        ''' Retrieves the index within the CGlobalFrameTable of the specified item.
        ''' </summary>
        ''' <param name="Item">An object representing the item to locate in the CGlobalFrameTable.</param>
        ''' <returns>The index of the specified item.</returns>
        Friend Function IndexOf(ByVal Item As CFrameDefinition) As Integer
            ' Get the index of Item in the list. 
            Return m_lFrameDefinitions.IndexOf(Item)
        End Function

#Region "Events"

        Friend Event OnPropertyChange As PropertyChangeEventHandler
        Friend Sub CallPropertyChangeHandler(ByRef sender As Object, ByVal type As EProperty, ByVal value As Integer, ByRef allowed As Boolean)
            ' The Event Handler is assigned.
            ' Create the EventArg object.
            Dim pArg As PropertyChangeEventArg = New PropertyChangeEventArg()
            ' Assign the parameter values to
            ' EventArg object.
            pArg.Type = type
            pArg.Value = value
            pArg.Allowed = allowed
            ' Call the event.
            RaiseEvent OnPropertyChange(sender, pArg)
            'TODO DispatchEvent(OnPropertyChange, New Object() {sender, pArg})
            ' Check if the reference parameter "Allowed"
            ' was set from the process the validate
            ' the event.
            allowed = pArg.Allowed
        End Sub

#End Region

    End Class

#End Region

End Namespace

