using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using PLIN_API_Example.Properties;

namespace PLIN_API_Example
{
    /// <summary>
    /// This class handles a dialog that displays and manages the LIN Global Frame Table
    /// </summary>
    public partial class Frame_Dlg : Form
    {
        #region Members
        /// <summary>
        /// Global Frame Table object that holds all frame definition.
        /// </summary>
        private CGlobalFrameTable m_pGFT;
        /// <summary>
        /// Boolean to register the current edit mode of different controls.
        /// To protect the control's event for recursive calling.
        /// </summary>
        private Boolean m_fAutomatic;

        #endregion

        #region Methods

        /// <summary>
        /// Constructor of the Frame_Dlg object to create the Global Frame dialog
        /// </summary>
        internal Frame_Dlg(CGlobalFrameTable pGFT)
        {
            InitializeComponent();
            // Initialize the list view component with items from the global frame table object
            m_pGFT = pGFT;
            FillListViews();
        }

        #region helper methods
        /// <summary>
        /// Returns the Frame Direction as a human readable string
        /// </summary>
        /// <param name="direction">the direction object</param>
        /// <returns>The direction as a string</returns>
        private string getDirectionAsString(Peak.Lin.TLINDirection direction)
        {
            switch (direction)
            {
                case Peak.Lin.TLINDirection.dirDisabled:
                    return Resources.SLinDirectionDisabled;
                case Peak.Lin.TLINDirection.dirPublisher:
                    return Resources.SLinDirectionPublisher;
                case Peak.Lin.TLINDirection.dirSubscriber:
                    return Resources.SLinDirectionSubscriber;
                case Peak.Lin.TLINDirection.dirSubscriberAutoLength:
                    return Resources.SLinDirectionAuto;
            }
            return "";
        }
        /// <summary>
        /// Returns the Frame checksum type as a human readable string
        /// </summary>
        /// <param name="checksumType">the checksumType object</param>
        /// <returns>The checksumType as a string</returns>
        private string getChecksumAsString(Peak.Lin.TLINChecksumType checksumType)
        {
            switch (checksumType)
            {
                case Peak.Lin.TLINChecksumType.cstAuto:
                    return Resources.SLinCSTAuto;
                case Peak.Lin.TLINChecksumType.cstClassic:
                    return Resources.SLinCSTClassic;
                case Peak.Lin.TLINChecksumType.cstEnhanced:
                    return Resources.SLinCSTEnhanced;
                case Peak.Lin.TLINChecksumType.cstCustom:
                    return Resources.SLinCSTCustom;
            }
            return "";
        }
        #endregion

        #region ListView event-handlers

        /// <summary>
        /// Occurs when a key is pressed while the ListView is active.
        /// Implements "select all items" feature.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">An EventArgs that contains no event data.</param>
        private void lvGFT_KeyDown(object sender, KeyEventArgs e)
        {
            // Adds "select all items" (Ctrl-A) feature
            if (e.KeyCode == Keys.A && e.Control)
            {
                // Lock the ListView.
                lvGFT.BeginUpdate();
                // prevent multiple calls of selectionChange
                m_fAutomatic = true;
                foreach (ListViewItem item in lvGFT.Items)
                {
                    item.Selected = true;
                }
                m_fAutomatic = false;
                // Unlock the ListView
                lvGFT.EndUpdate();
                // Call selection change
                lvGFT_SelectedIndexChanged(this, new EventArgs());
            }
        }

        /// <summary>
        /// Occurs when the SelectedIndices collection changes. The selection
        /// will be assigned to the Property-Control 'pgGFTDef'.
        /// </summary>
        /// <param name="sender">The source of the event.</param>
        /// <param name="e">An EventArgs that contains no event data.</param>
        private void lvGFT_SelectedIndexChanged(object sender, EventArgs e)
        {
            int nCnt, nIdx, i;

            if (!m_fAutomatic)
            {
                m_fAutomatic = true;
                // Get the count of selected elements
                // in the ListView 'lvGFT'.
                // Each element is assigned with an
                // element in the CGlobalFrameTable.
                nCnt = lvGFT.SelectedItems.Count;
                if (nCnt == 1)
                {
                    // If only one element is selected
                    // it can be assigned directly to the
                    // Property-Control.
                    nIdx = lvGFT.SelectedItems[0].Index;
                    // Assign the CFrameDefinition element
                    // from the CGlobalFrameTable.
                    pgGFTDef.SelectedObject = m_pGFT[nIdx];
                }
                else if (nCnt > 1)
                {
                    // If more than one element are selected
                    // in the ListView means that the user
                    // want to change at least one property of
                    // all elements to a new value with
                    // the Property-Control.
                    // So get first the selected elements and the
                    // assigned objects from the CGlobalFrameTable
                    // into a local Array of elements.
                    CFrameDefinition[] lDefFrames = new CFrameDefinition[nCnt];
                    for (i = 0; i < nCnt; i++)
                    {
                        nIdx = lvGFT.SelectedItems[i].Index;
                        lDefFrames[i] = m_pGFT[nIdx];
                    }
                    // Assign the local array of elements
                    // from the CGlobalFrameTable.
                    pgGFTDef.SelectedObjects = lDefFrames;
                }
                m_fAutomatic = false;
            }
        }

        /// <summary>
        /// Fills the Listview 'Global Frame Table' with the data from CGloabalFrameTable.
        /// </summary>
        private void FillListViews()
        {
            ListViewItem lvi;
            // Lock the ListView.
            lvGFT.BeginUpdate();
            // Clear the ListView that
            // will show the Frame Definition of the
            // Global Frame Table.
            lvGFT.Items.Clear();
            // Add every object, Frame Definition, from the
            // Global Frame Table into the ListView.
            for (int i = 0; i < m_pGFT.Count; i++)
            {
                lvi = lvGFT.Items.Add(Convert.ToString(m_pGFT[i].ID));
                lvi.SubItems.Add(Convert.ToString(m_pGFT[i].ProtectedID));
                lvi.SubItems.Add(getDirectionAsString(m_pGFT[i].Direction));
                lvi.SubItems.Add(Convert.ToString(m_pGFT[i].Length));
                lvi.SubItems.Add(getChecksumAsString(m_pGFT[i].ChecksumType));
            }
            // Unlock and update the ListView now.
            lvGFT.EndUpdate();
        }

        #endregion 

        #region PropertyGrid 'pgGFTDef'

        /// <summary>
        /// Occurs when a property value of at least one CFrameDefinition object
        /// was changed by the Property-Control.
        /// </summary>
        /// <param name="s">The source of the event.</param>
        /// <param name="e">A PropertyValueChangedEventArgs that contains the event data.</param>
        private void pgGFTDef_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            int i, nCnt, nIdx;
            CFrameDefinition lFD;
            ListViewItem lvi;

            lFD = null;
            // Lock the ListView for update elements.
            lvGFT.BeginUpdate();
            // Get the selected count of CFrameDefinition.
            nCnt = pgGFTDef.SelectedObjects.GetLength(0);
            // For each selected CFrameDefinition in
            // the ListView, update their property values.
            for (i = 0; i < nCnt; i++)
            {
                // Get the selected CFrameDefinition X from the
                // Property-Control that has changed the
                // property values.
                lFD = (CFrameDefinition)pgGFTDef.SelectedObjects[i];
                if (lFD != null)
                {
                    // Get the position in the ListView of
                    // the CFrameDefinition to refresh the
                    // property value.
                    nIdx = m_pGFT.IndexOf(lFD);
                    if ((nIdx >= 0) && (nIdx < lvGFT.Items.Count))
                    {
                        // Get the item that is assign with the
                        // CFrameDefinition from ListView 'lvGFT'
                        // and refesh it.
                        lvi = lvGFT.Items[nIdx];
                        lvi.SubItems[1].Text = Convert.ToString(lFD.ProtectedID);
                        lvi.SubItems[2].Text = getDirectionAsString(lFD.Direction);
                        lvi.SubItems[3].Text = Convert.ToString(lFD.Length);
                        lvi.SubItems[4].Text = getChecksumAsString(lFD.ChecksumType);
                    }

                }
            }
            // Free the lock of ListView and
            // update all elements now.
            lvGFT.EndUpdate();
        }

        #endregion

        #endregion
    }
}
