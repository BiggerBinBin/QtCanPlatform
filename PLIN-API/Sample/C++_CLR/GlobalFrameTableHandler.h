#pragma once
#include "PLinApiCLR.h"

namespace PLINAPIExample
{
	using namespace System;
	using namespace System::Collections::Generic;
	using namespace System::ComponentModel;

#pragma region Enums

	/// <summary>
	/// Represents the enumerate values of the property kind from the
	/// CFrameDefinition object.
	/// </summary>
	public enum class EProperty
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
	};

#pragma endregion

#pragma region Event Parameter Definition

	/// <summary>
	/// Represents an event class for property change events.
	/// </summary>
	public ref class PropertyChangeEventArg : System::EventArgs
	{
	public:
		/// <summary>
		/// Type of property to change.
		/// </summary>
		EProperty Type;
		/// <summary>
		/// New value for the property.
		/// </summary>
		int Value;
		/// <summary>
		/// Boolean value to indicate if the property can be set.
		/// </summary>
		bool Allowed;
	};

#pragma endregion

#pragma region Delegate Definition

	/// <summary>
	/// Declares the delegate function of the property change event.
	/// </summary>
	/// <param name="sender">The source of the event.</param>
	/// <param name="e">A PropertyChangeEventArg that contains the event data.</param>
	public delegate void PropertyChangeEventHandler(Object^ sender, PropertyChangeEventArg^ e);

#pragma endregion

#pragma region CDispatch Class
	/// <summary>
	/// Represents a Dispatch class that can be inherited by
	/// other classes in order to dispatch messages to
	/// another class or object.
	/// </summary>
	public ref class CDispatch
	{
#pragma region Attibuts
		/// <summary>
		/// The owner of the dispatch class.
		/// </summary>
	protected:
		ISynchronizeInvoke^ m_pOwner;
		/// <summary>
		/// Indicates if the owner is set.
		/// </summary>
		bool m_fSynchronized;

#pragma endregion

#pragma region Methods

		/// <summary>
		/// Makes the choice how should be referenced an event according with the selected 
		/// environment (if will be direct invoked or added to the invoke list of a 
		/// synchronization control)
		/// </summary>
		/// <param name="ToDispatch">The delegate to be invoked</param>
		/// <param name="parameters">the parameters of the given delegate</param>
		void DispatchEvent(Delegate^ ToDispatch, array<Object^>^ parameters)
		{
			try
			{
				if (ToDispatch != nullptr)
				{
					if (m_fSynchronized)
						if (m_pOwner->InvokeRequired)
						{
							m_pOwner->Invoke(ToDispatch, parameters);
							return;
						}
						ToDispatch->DynamicInvoke(parameters);
				}
			}
			catch (Exception^ Ex)
			{
				throw (gcnew Exception(String::Format("Error dispatching event: \"{0}\".  Error Message: \"{1}\"", ToDispatch->Method->Name, Ex->Message)));
			}
		}

		/// <summary>
		/// Constructor to create a base class which can dispatch message like
		/// events through different threads.
		/// </summary>
		CDispatch() :
		m_pOwner(nullptr),
			m_fSynchronized(false)
		{
			// In the base class the attibuts can be null.
		}
#pragma endregion
	};

#pragma endregion

#pragma region CFrameDefinition Class
	/// <summary>
	/// Represent the global Definition of a LIN-Frame.
	/// </summary>
	public ref class CFrameDefinition
	{
#pragma region Attributes
	internal:
		Object^ m_pParent;
		/// <summary>
		/// LIN-Frame Identifier
		/// </summary>
		int m_nID;
		/// <summary>
		/// Datalength of the LIN-Frame
		/// </summary>
		int m_nLength;
		/// <summary>
		/// Type of the Checksum Calculation
		/// </summary>
		Peak::Lin::TLINChecksumType m_nChecksumType;
		/// <summary>
		/// The m_bDirection of the LIN-Frame
		/// </summary>
		Peak::Lin::TLINDirection m_bDirection;
		/// <summary>
		/// Is the protected LIN-Frame m_nID
		/// </summary>
		int m_nProtectedID;
#pragma endregion

#pragma region Properties
	internal:
		/// <summary>
		/// Gets the Frame-ID as integer value.
		/// </summary>
		property int IdAsInt
		{
			int get() { return m_nID; }
		}
		/// <summary>
		/// Gets the Frame-ID as byte value.
		/// </summary>
		property Byte IdAsByte
		{
			Byte get() { return Convert::ToByte(m_nID); }
		}
		/// <summary>
		/// Gets the Protected Frame-ID as integer value.
		/// This value includes the parity of the LIN-Frame.
		/// </summary>
		property int ProtectedIdAsInt
		{
			int get() { return m_nProtectedID; }
		}

		/////////////////////////////////////////////////
		//	Public properties visible in a propertyGrid
	public:
		[DisplayName("ID")]
		[Category("ReadOnly")]
		[Description("Defines a LIN-Frame identifier of the Frame definition and can be between 00h and 3Fh.")]
		property String^ ID
		{
			String^ get() { return String::Format("{0:X2}h", m_nID); }
		}

		[DisplayName("Length")]
		[Category("Changeable")]
		[Description("Defines the length of the LIN-Frame definition.")]
		property int Length
		{
			int get() { return m_nLength; }
			void set(int value);
		}

		[DisplayName("Checksum Type")]
		[Category("Changeable")]
		[Description("Defines the type for the checksum calculation of the LIN-Frame definition and can be classic, enhanced or auto.")]
		property Peak::Lin::TLINChecksumType ChecksumType
		{
			Peak::Lin::TLINChecksumType get() { return m_nChecksumType; }
			void set(Peak::Lin::TLINChecksumType value);
		}

		[DisplayName("Direction")]
		[Category("Changeable")]
		[Description("Defines the direction of the LIN-Frame definition and can be publisher or subscriber. Default is subscriber.")]
		property Peak::Lin::TLINDirection Direction
		{
			Peak::Lin::TLINDirection get() { return m_bDirection; }
			void set(Peak::Lin::TLINDirection value);
		}

		[DisplayName("Protected ID")]
		[Category("ReadOnly")]
		[Description("Defines the protected LIN-Frame identifier.")]
		property String^ ProtectedID
		{
			String^ get() { return String::Format("{0:X2}h", m_nProtectedID); }
		}
#pragma endregion
	};
#pragma endregion

#pragma region CGlobalFrameTable
	
    /// <summary>
    /// A class that holds the LIN Global Frames table, that contains
    /// a list of 64 CFrameDefinition objects. It inherits CDispatch class 
    /// in order to dispatch messages when CFrameDefinition objects are modified
    /// </summary>
	public ref class CGlobalFrameTable : CDispatch
	{
		/// <summary>
		/// Collection of all Frame Definition
		/// </summary>
	private:
		List<CFrameDefinition^>^ m_lFrameDefinitions;

	public:
		CGlobalFrameTable(ISynchronizeInvoke^ AOwner);

	private:
		/// <summary>
		/// Adds an CFrameDefinition object to the end of the CGlobalFrameTable.
		/// </summary>
		/// <param name="AFrameDefinition">The CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
		/// <returns>Returns the Index of position in the list.</returns>
		int AddFrameDefinition(CFrameDefinition^ AFrameDefinition);

		/// <summary>
		/// Adds an CFrameDefinition object with the defined values to the end of the CGlobalFrameTable.
		/// </summary>
		/// <param name="AId">The Frame ID of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
		/// <param name="ALength">The Frame Length of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
		/// <param name="AChecksumType">The Frame Checksum Type of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
		/// <param name="ADirection">The Frame Direction of the CFrameDefinition object to be added to the end of the CGlobalFrameTable.</param>
		/// <returns>Returns the Index of position in the list.</returns>
		int AddFrameDefinition(int AId, int ALength, Peak::Lin::TLINChecksumType AChecksumType, Peak::Lin::TLINDirection ADirection);

	public:
		/// <summary>
		/// Returns the CFrameDefinition object in the Frame collection with the specified index
		/// </summary>
		/// <param name="Index">The zero-based index of the CFrameDefinition to get or set.</param>
		property CFrameDefinition^ default[int]
		{
			CFrameDefinition^ get(int i)
			{
				return m_lFrameDefinitions[i];
			}
			void set(int i, CFrameDefinition^ value)
			{
				m_lFrameDefinitions[i] = value;
			}
		}
		/// <summary>
		/// Gets the number of CFrameDefinition objects actually contained in the CGlobalFrameTable.
		/// </summary>
		property int Count
		{
			int get() { return m_lFrameDefinitions->Count; }
		}

		/// <summary>
		/// Retrieves the index within the CGlobalFrameTable of the specified item.
		/// </summary>
		/// <param name="Item">An object representing the item to locate in the CGlobalFrameTable.</param>
		/// <returns>The index of the specified item.</returns>
		int IndexOf(CFrameDefinition^ Item)
		{
			// Get the index of Item in the list. 
			return m_lFrameDefinitions->IndexOf(Item);
		}

		/// <summary>
		/// Property Change Event
		/// </summary>
		PropertyChangeEventHandler^ OnPropertyChange;
		/// <summary>
		/// Handles a property change and dispatch the event
		/// </summary>
		void CallPropertyChangeHandler(Object^ sender, EProperty AType, int AValue, bool &AAllowed);

	};

#pragma endregion
}
