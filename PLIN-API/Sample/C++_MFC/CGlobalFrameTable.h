
// CGlobalFrameTable.h : header file
//

#ifndef __CGLOBALFRAMETABLEH_
#define __CGLOBALFRAMETABLEH_

// Inclusion of the PLinApi.h header file
//
#ifndef __PLINAPICLASSH_
#include "PLinApiClass.h"
#endif

////////////////////////////////////////////////////////////
// Value definitions
////////////////////////////////////////////////////////////

// Size of the LIN Global Frame Table
#define GLOBALFRAMETABLE_SIZE	64

#pragma region CFrameDefinition class

// Changeable LIN Frame properties
//
#define propDirection			1
#define propChecksumType		2
#define propLength				3
#define EFRAMEPROPERTY			WORD

// LIN Frame Definition class for handling property changes and message communication
//
class CFrameDefinition
{
protected:
	// The CGlobalFrameTable holding this frame
    void * m_pParent;
    // LIN-Frame Identifier
    int m_nID;
    // Datalength of the LIN-Frame
    int m_nLength;
    // Type of the Checksum Calculation
    TLINChecksumType m_nChecksumType;
    // Direction of the LIN-Frame
    TLINDirection m_bDirection;
    // Protected LIN-Frame m_nID
    int m_nProtectedID;

public:
	// Constructor
	CFrameDefinition(void * pParent,	// CGlobalFrameTable holding this frame
		int id,							// LIN-Frame ID
		int pid,						// LIN-Frame Protected ID
		int len,						// LIN-Frame length
		TLINChecksumType checksumType,	// LIN-Frame checksum type
		TLINDirection direction);		// LIN-Frame direction
	// Standard destructor
	~CFrameDefinition();

	// Getters
	//

	// Returns the LIN-Frame identifier as a string.
	CString getId();
	// Returns the LIN-Frame identifier as an integer.
	int getIdAsInt();
	// Returns the LIN-Frame identifier as a BYTE.
	BYTE getIdAsByte();

	// Returns the protected LIN-Frame identifier as a string.
	CString getProtectedId();
	// Returns the protected LIN-Frame identifier as an integer.
	int getProtectedIdAsInt();
	
	// Returns the LIN-Frame length as an integer.
	int getLength();
	// Returns the LIN-Frame length as a string.
	CString getLengthAsString();

	// Returns the LIN-Frame checksumType.
	TLINChecksumType getChecksumType();
	// Returns the LIN-Frame direction.
	TLINDirection getDirection();
	
	// Setters with calls to "CallFramePropertyChanged" parent function 
	//

	// Sets the LIN-FRAME length
	void setLength(int length, bool doCallFramePropertyChanged = false);
	// Sets the LIN-FRAME checksumType
	void setChecksumType(TLINChecksumType checksumType, bool doCallFramePropertyChanged = false);
	// Sets the LIN-FRAME direction
	void setDirection(TLINDirection direction, bool doCallFramePropertyChanged = false);
};

#pragma endregion 


// LIN Global Frame Table class, contains and handles the 64 LIN Frames/CFrameDefinitions
//
class CGlobalFrameTable
{
	friend class CFrameDefinition;

private:	
    // LIN API object
	PLinApiClass * m_objPLinApi;

	// LIN Client handle
	HLINCLIENT m_hClient;
    // LIN Hardware handle
    HLINHW m_hHw;
    // LIN Hardware Modus
	TLINHardwareMode m_HwMode;
    // pointer to LIN Client filter mask
	unsigned __int64 * m_pMask;

	// Collection of all Frame Definition
	CFrameDefinition * m_lFrameDefinitions[GLOBALFRAMETABLE_SIZE];	

public:
	CGlobalFrameTable(PLinApiClass * objPLinApi);
	~CGlobalFrameTable();
	
	// Returns the CFrameDefinition based on the specified index (with 0 <= index < 64)
	CFrameDefinition* get(int index);

	// Returns the size of the Frame Table
	int getCount();

	// Returns the index in the table for the specified Frame object
	int indexOf(CFrameDefinition * pFrame);

	// Sets internal LIN informations
	void UpdateLinInfo(const HLINCLIENT hClient, const HLINHW hHw, const TLINHardwareMode hwMode, unsigned __int64 * pMask);

private:
	// Function called when a property of CFrameDefinition is changed to assert the modification is allowed
	void CallFramePropertyChanged(
		EFRAMEPROPERTY propertyType,	// the property to modify
		CFrameDefinition * pFrame,		// frame to update
		void * value,					// the new value of the property
		bool * allowed);				// defines if the modification is allowed or not

	// Adds a new frame definition to the global frame table. Returns the frame index in the table (the same as AId unless an error occured)
	int AddFrameDefinition(
		int AId,			// position (and ID) of the frame in the table
		int ALength,		// length
		TLINChecksumType AChecksumType,	// checksum type
		TLINDirection ADirection);		// direction
	
};

#endif