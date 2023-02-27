
// PLIN-API Example.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPLINAPIExampleApp:
// See PLIN-API Example.cpp for the implementation of this class
//

class CPLINAPIExampleApp : public CWinAppEx
{
public:
	CPLINAPIExampleApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPLINAPIExampleApp theApp;