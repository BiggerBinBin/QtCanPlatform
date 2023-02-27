using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace PLIN_API_Example
{

    #region Event Parameter Definition

    /// <summary>
    /// Represents an event class for property change events.
    /// </summary>
    internal class PropertyChangeEventArg : System.EventArgs
    {
        /// <summary>
        /// Type of property to change.
        /// </summary>
        internal EProperty Type;
        /// <summary>
        /// New value for the property.
        /// </summary>
        internal int Value;
        /// <summary>
        /// Boolean value to indicate if the property can be set.
        /// </summary>
        internal bool Allowed;
    }

    #endregion

    #region Delegate Definition

    /// <summary>
    /// Declares the delegate function of the property change event.
    /// </summary>
    /// <param name="sender">The source of the event.</param>
    /// <param name="e">A PropertyChangeEventArg that contains the event data.</param>
    internal delegate void PropertyChangeEventHandler(Object sender, PropertyChangeEventArg e);

    #endregion
        
    #region Enums

    /// <summary>
    /// Represents the enumerate values of the property kind from the
    /// CFrameDefinition object.
    /// </summary>
    internal enum EProperty
    {
        /// <summary>
        /// Length property
        /// </summary>
        Length = 0,
        /// <summary>
        /// Checksum type property
        /// </summary>
        ChecksumType = 1,
        /// <summary>
        /// Direction property
        /// </summary>
        Direction = 2,
    }

    #endregion
    
    #region CDispatch Class
    /// <summary>
    /// Represents a Dispatch class that can be inherited by
    /// other classes in order to dispatch messages to
    /// another class or object.
    /// </summary>
    class CDispatch
    {
        #region Attibuts
        /// <summary>
        /// The owner of the dispatch class.
        /// </summary>
        protected ISynchronizeInvoke m_pOwner;
        /// <summary>
        /// Indicates if the owner is set.
        /// </summary>
        protected Boolean m_fSynchronized;
        #endregion

        #region Methods
        
        /// <summary>
        /// Makes the choice how should be referenced an event according with the selected 
        /// environment (if will be direct invoked or added to the invoke list of a 
        /// synchronization control)
        /// </summary>
        /// <param name="ToDispatch">The delegate to be invoked</param>
        /// <param name="parameters">the parameters of the given delegate</param>
        protected void DispatchEvent(Delegate ToDispatch, object[] parameters)
        {
            try
            {
                if (ToDispatch != null)
                {
                    if (m_fSynchronized)
                        if (m_pOwner.InvokeRequired)
                        {
                            m_pOwner.Invoke(ToDispatch, parameters);
                            return;
                        }
                    ToDispatch.DynamicInvoke(parameters);
                }
            }
            catch (Exception Ex)
            {
                throw (new Exception(string.Format("Error dispatching event: \"{0}\".  Error Message: \"{1}\"", ToDispatch.Method.Name, Ex.Message)));
            }
        }
       
        /// <summary>
        /// Constructor to create a base class which can dispatch message like
        /// events through different threads.
        /// </summary>
        internal CDispatch()
        {
            // In the base class the attibuts can be null.
            m_pOwner = null;
            m_fSynchronized = false;
        }
        #endregion
    }

    #endregion

    #region CFrameDefinition Class
    /// <summary>
    /// Represent the global Definition of a LIN-Frame.
    /// </summary>
    internal class CFrameDefinition
    {
        #region Attributes
        internal object m_pParent;
        /// <summary>
        /// LIN-Frame Identifier
        /// </summary>
        internal int m_nID;
        /// <summary>
        /// Data length of the LIN-Frame
        /// </summary>
        internal int m_nLength;
        /// <summary>
        /// Type of the Checksum Calculation
        /// </summary>
        internal Peak.Lin.TLINChecksumType m_nChecksumType;
        /// <summary>
        /// The m_bDirection of the LIN-Frame
        /// </summary>
        internal Peak.Lin.TLINDirection m_bDirection;
        /// <summary>
        /// Is the protected LIN-Frame m_nID
        /// </summary>
        internal int m_nProtectedID;
        #endregion

        #region Properties
        /// <summary>
        /// Gets the Frame-ID as integer value.
        /// </summary>
        internal int IdAsInt
        {
            get { return m_nID; }
        }
        /// <summary>
        /// Gets the Frame-ID as byte value.
        /// </summary>
        internal byte IdAsByte
        {
            get { return Convert.ToByte(m_nID); }
        }
        /// <summary>
        /// Gets the Protected Frame-ID as integer value.
        /// This value includes the parity of the LIN-Frame.
        /// </summary>
        internal int ProtectedIdAsInt
        {
            get { return m_nProtectedID; }
        }

        [DisplayName("ID")]
        [Category("ReadOnly")]
        [Description("Defines a LIN-Frame identifier of the Frame definition and can be between 00h and 3Fh.")]
        public String ID
        {
            get { return String.Format("{0:X2}h", m_nID); }
        }

        [DisplayName("Length")]
        [Category("Changeable")]
        [Description("Defines the length of the LIN-Frame definition.")]
        public int Length
        {
            get { return m_nLength; }
            set
            {
                if (m_nLength != value)
                {
                    bool fAllowed = true;
                    ((CGlobalFrameTable)m_pParent).CallPropertyChangeHandler(this, EProperty.Length, value, ref fAllowed);
                    if (fAllowed == true)
                        m_nLength = value;
                }
            }
        }

        [DisplayName("Checksum Type")]
        [Category("Changeable")]
        [Description("Defines the type for the checksum calculation of the LIN-Frame definition and can be classic, enhanced or auto.")]
        public Peak.Lin.TLINChecksumType ChecksumType
        {
            get { return m_nChecksumType; }
            set
            {
                if (m_nChecksumType != value)
                {
                    bool fAllowed = true;
                    ((CGlobalFrameTable)m_pParent).CallPropertyChangeHandler(this, EProperty.ChecksumType, (int)value, ref fAllowed);
                    if (fAllowed == true)
                        m_nChecksumType = value;
                }
            }
        }

        [DisplayName("Direction")]
        [Category("Changeable")]
        [Description("Defines the direction of the LIN-Frame definition and can be publisher or subscriber. Default is subscriber.")]
        public Peak.Lin.TLINDirection Direction
        {
            get { return m_bDirection; }
            set
            {
                if (m_bDirection != value)
                {
                    bool fAllowed = true;
                    ((CGlobalFrameTable)m_pParent).CallPropertyChangeHandler(this, EProperty.Direction, (int)value, ref fAllowed);
                    if (fAllowed == true)
                        m_bDirection = value;
                }
            }
        }

        [DisplayName("Protected ID")]
        [Category("ReadOnly")]
        [Description("Defines the protected LIN-Frame identifier.")]
        public String ProtectedID
        {
            get { return String.Format("{0:X2}h", m_nProtectedID); }
        }
        #endregion
    }
    #endregion

    #region CGlobalFrameTable
    
    /// <summary>
    /// A class that holds the LIN Global Frames table, that contains
    /// a list of 64 CFrameDefinition objects. It inherits CDispatch class 
    /// in order to dispatch messages when CFrameDefinition objects are modified
    /// </summary>
    internal class CGlobalFrameTable :CDispatch
    {
        #region Attibuts
        /// <summary>
        /// Collection of all Frame Definition
        /// </summary>
        private List<CFrameDefinition> m_lFrameDefinitions;
        #endregion

        #region Methods

        internal CGlobalFrameTable(ISynchronizeInvoke AOwner)
        {
            // Initialize the inherited attributes from the
            // base class CDispatch.
            m_pOwner = AOwner;
            m_fSynchronized = m_pOwner != null;
            // Create a list to handle the CFrameDefinition objects.
            m_lFrameDefinitions = new List<CFrameDefinition>(64);
            // Create all CFrameDefinition objects and set
            // them values to default. The length values is
            // set to LIN 1.2.
            // Default values:
            // - Direction = SubscriberAutoLength
            // - ChecksumType = Auto
            // - Length = see inside the loop.
            for (int i = 0; i < 64; i++)
            {
                // From Frame-ID 0 to 31 set the length 2.
                if ((i >= 0x00) && (i <= 0x1F))
                    AddFrameDefinition(i, 2, Peak.Lin.TLINChecksumType.cstAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength);
                // From Frame-ID 32 to 47 set the length 4.
                else if ((i >= 0x20) && (i <= 0x2F))
                    AddFrameDefinition(i, 4, Peak.Lin.TLINChecksumType.cstAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength);
                // From Frame-ID 48 to 63 set the length 8.
                if ((i >= 0x30) && (i <= 0x3F))
                    AddFrameDefinition(i, 8, Peak.Lin.TLINChecksumType.cstAuto, Peak.Lin.TLINDirection.dirSubscriberAutoLength);
            }
        }

        /// <summary>
        /// Adds an CFrameDefinition object to the end of the CGlobalFrameTable.
        /// </summary>
        /// <param name="AFrameDefinition">The CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        /// <returns>Returns the Index of position in the list.</returns>
        private int AddFrameDefinition(CFrameDefinition AFrameDefinition)
        {
            // Check the parameter at first.
            if (AFrameDefinition == null)
                // Delivered object invalid. Do not add it.
                return -1;
            else
                // Add the delivered object (CFrameDefinition) to the list.
                m_lFrameDefinitions.Add(AFrameDefinition);
            // Return the position of the new added object.
            // It should be added at the end so the position
            // must be the last entry in the list.
            return m_lFrameDefinitions.Count - 1;
        }

        /// <summary>
        /// Adds an CFrameDefinition object with the defined values to the end of the CGlobalFrameTable.
        /// </summary>
        /// <param name="AId">The Frame ID of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        /// <param name="ALength">The Frame Length of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        /// <param name="AChecksumType">The Frame Checksum Type of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        /// <param name="ADirection">The Frame Direction of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
        /// <returns>Returns the Index of position in the list.</returns>
        private int AddFrameDefinition(int AId, int ALength, Peak.Lin.TLINChecksumType AChecksumType, Peak.Lin.TLINDirection ADirection)
        {
            // Check the Frame-ID for adding.
            // Only ID's from 0 to 63 are allowed to add.
            if ((AId < 0) || (AId > 63))
                // ID is invalid. Do not add it.
                return -1;
            else
            {
                // The delivered Frame-ID is valid.
                CFrameDefinition lFD;
                byte lbID;

                // Create a Frame Definition object.
                // and assigned the delivered values to it.
                lFD = new CFrameDefinition();
                lFD.m_pParent = this;
                lFD.m_nID = AId;
                lFD.m_nLength = ALength;
                lFD.m_nChecksumType = AChecksumType;
                lFD.m_bDirection = ADirection;
                // Calculate the Protected-ID with
                // the delivered Frame-ID.
                lbID = Convert.ToByte(AId);
                Peak.Lin.PLinApi.GetPID(ref lbID);
                // Assign the calculated Protected-ID.
                lFD.m_nProtectedID = lbID;
                // Add the created object to the list.
                m_lFrameDefinitions.Add(lFD);
            }
            // Return the position of the new added object.
            // It should be added at the end so the position
            // must be the last entry in the list.
            return m_lFrameDefinitions.Count - 1;
        }

        #endregion

        #region Properties
        /// <summary>
        /// </summary>
        /// <param name="Index">The zero-based index of the CFrameDefinition to get or set.</param>
        internal CFrameDefinition this[int Index]
        {
            get
            {
                return m_lFrameDefinitions[Index];
            }
            set
            {
                m_lFrameDefinitions[Index] = value;
            }
        }
        /// <summary>
        /// Gets the number of CFrameDefinition objects actually contained in the CGlobalFrameTable.
        /// </summary>
        internal int Count
        {
            get { return m_lFrameDefinitions.Count; }
        }
        #endregion

        /// <summary>
        /// Retrieves the index within the CGlobalFrameTable of the specified item.
        /// </summary>
        /// <param name="Item">An object representing the item to locate in the CGlobalFrameTable.</param>
        /// <returns>The index of the specified item.</returns>
        internal int IndexOf(CFrameDefinition Item)
        {
            // Get the index of Item in the list. 
            return m_lFrameDefinitions.IndexOf(Item);
        }

        #region Events

        public PropertyChangeEventHandler OnPropertyChange;
        internal void CallPropertyChangeHandler(object sender, EProperty AType, int AValue, ref bool AAllowed)
        {
            // Check Event Handler at first.
            if (OnPropertyChange != null)
            {
                // The Event Handler is assigned.
                // Create the EventArg object.
                PropertyChangeEventArg pArg = new PropertyChangeEventArg();
                // Assign the parameter values to
                // EventArg object.
                pArg.Type = AType;
                pArg.Value = AValue;
                pArg.Allowed = AAllowed;
                // Call the event.
                DispatchEvent(OnPropertyChange, new object[] { sender, pArg });
                // Check if the reference parameter "Allowed"
                // was set from the process the validate
                // the event.
                AAllowed = pArg.Allowed;
            }
        }

        #endregion
    }

    #endregion
}
