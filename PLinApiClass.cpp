#define _AFXDLL
#include "StdAfx.h"
#include "PLinApiClass.h"

PLinApiClass::PLinApiClass()
{
	//Init members
	//
	m_bWasLoaded = false;
	m_hDll = NULL;

	// Loads the API
	//
	LoadAPI();
}

PLinApiClass::~PLinApiClass()
{
	// Unloads the API
	//
	UnloadAPI();
}

//	Private functions	
//
void PLinApiClass::LoadAPI() 
{
	// Initializes pointers
	//
	InitializePointers();

	// Loads the DLL
	//
	if(!LoadDllHandle())
	{
		::MessageBox(NULL, _T("Error: \"Unable to find the DLL PLinApi.dll!\""), _T("Error!"), MB_ICONERROR);
		return;
	}

	// Loads API functions
	//
	m_pRemoveClient = (fpRemoveClient)GetFunction("LIN_RemoveClient");
	m_pResetClient = (fpResetClient)GetFunction("LIN_ResetClient");	
	m_pConnectClient = (fpConnectClient)GetFunction("LIN_ConnectClient");	
	m_pDisconnectClient = (fpDisconnectClient)GetFunction("LIN_DisconnectClient");	
	m_pResetHardware = (fpResetHardware)GetFunction("LIN_ResetHardware");	
	m_pSuspendKeepAlive = (fpSuspendKeepAlive)GetFunction("LIN_SuspendKeepAlive");	
	m_pResumeKeepAlive = (fpResumeKeepAlive)GetFunction("LIN_ResumeKeepAlive");	
	m_pResetHardwareConfig = (fpResetHardwareConfig)GetFunction("LIN_ResetHardwareConfig");	
	m_pSuspendSchedule = (fpSuspendSchedule)GetFunction("LIN_SuspendSchedule");	
	m_pResumeSchedule = (fpResumeSchedule)GetFunction("LIN_ResumeSchedule");	
	m_pXmtWakeUp = (fpXmtWakeUp)GetFunction("LIN_XmtWakeUp");	
	m_pDeleteSchedule = (fpDeleteSchedule)GetFunction("LIN_DeleteSchedule");	
	m_pStartSchedule = (fpStartSchedule)GetFunction("LIN_StartSchedule");	
	m_pRegisterClient = (fpRegisterClient)GetFunction("LIN_RegisterClient");
	m_pIdentifyHardware = (fpIdentifyHardware)GetFunction("LIN_IdentifyHardware");
	m_pSetClientParam = (fpSetClientParam)GetFunction("LIN_SetClientParam");
	m_pGetClientParam = (fpGetClientParam)GetFunction("LIN_GetClientParam");
	m_pSetClientFilter = (fpSetClientFilter)GetFunction("LIN_SetClientFilter");
	m_pGetClientFilter = (fpGetClientFilter)GetFunction("LIN_GetClientFilter");
	m_pRead = (fpRead)GetFunction("LIN_Read");
	m_pReadMulti = (fpReadMulti)GetFunction("LIN_ReadMulti");
	m_pWrite = (fpWrite)GetFunction("LIN_Write");
	m_pInitializeHardware = (fpInitializeHardware)GetFunction("LIN_InitializeHardware");
	m_pGetAvailableHardware = (fpGetAvailableHardware)GetFunction("LIN_GetAvailableHardware");
	m_pSetHardwareParam = (fpSetHardwareParam)GetFunction("LIN_SetHardwareParam");
	m_pGetHardwareParam = (fpGetHardwareParam)GetFunction("LIN_GetHardwareParam");
	m_pRegisterFrameId = (fpRegisterFrameId)GetFunction("LIN_RegisterFrameId");
	m_pSetFrameEntry = (fpSetFrameEntry)GetFunction("LIN_SetFrameEntry");
	m_pGetFrameEntry = (fpGetFrameEntry)GetFunction("LIN_GetFrameEntry");
	m_pUpdateByteArray = (fpUpdateByteArray)GetFunction("LIN_UpdateByteArray");
	m_pStartKeepAlive = (fpStartKeepAlive)GetFunction("LIN_StartKeepAlive");
	m_pSetSchedule = (fpSetSchedule)GetFunction("LIN_SetSchedule");
	m_pGetSchedule = (fpGetSchedule)GetFunction("LIN_GetSchedule");
	m_pSetScheduleBreakPoint = (fpSetScheduleBreakPoint)GetFunction("LIN_SetScheduleBreakPoint");
	m_pStartAutoBaud = (fpStartAutoBaud)GetFunction("LIN_StartAutoBaud");
	m_pGetStatus = (fpGetStatus)GetFunction("LIN_GetStatus");
	m_pCalculateChecksum = (fpCalculateChecksum)GetFunction("LIN_CalculateChecksum");
	m_pGetVersion = (fpGetVersion)GetFunction("LIN_GetVersion");
	m_pGetVersionInfo = (fpGetVersionInfo)GetFunction("LIN_GetVersionInfo");
	m_pGetErrorText = (fpGetErrorText)GetFunction("LIN_GetErrorText");
	m_pGetPID = (fpGetPID)GetFunction("LIN_GetPID");
	m_pGetTargetTime = (fpGetTargetTime)GetFunction("LIN_GetTargetTime");
	m_pSetResponseRemap = (fpSetResponseRemap)GetFunction("LIN_SetResponseRemap");
	m_pGetResponseRemap = (fpGetResponseRemap)GetFunction("LIN_GetResponseRemap");
	m_pGetSystemTime = (fpGetSystemTime)GetFunction("LIN_GetSystemTime");

	// Checks that all functions are loaded
	m_bWasLoaded = 	m_pRemoveClient && m_pResetClient && m_pConnectClient && m_pDisconnectClient && 
		m_pResetHardware && m_pSuspendKeepAlive && m_pResumeKeepAlive && m_pResetHardwareConfig && 
		m_pSuspendSchedule && m_pResumeSchedule && m_pXmtWakeUp && m_pDeleteSchedule && m_pStartSchedule && 
		m_pRegisterClient && m_pIdentifyHardware && m_pSetClientParam && m_pGetClientParam && m_pSetClientFilter && 
		m_pGetClientFilter && m_pRead && m_pReadMulti && m_pWrite && m_pInitializeHardware && m_pGetAvailableHardware && 
		m_pSetHardwareParam && m_pGetHardwareParam && m_pRegisterFrameId && m_pSetFrameEntry && m_pGetFrameEntry && 
		m_pUpdateByteArray && m_pStartKeepAlive && m_pSetSchedule && m_pGetSchedule && m_pSetScheduleBreakPoint && 
		m_pStartAutoBaud && m_pGetStatus && m_pCalculateChecksum && m_pGetVersion && m_pGetVersionInfo && 
		m_pGetErrorText && m_pGetPID && m_pGetTargetTime && m_pSetResponseRemap && m_pGetResponseRemap && 
		m_pGetSystemTime;

	// If the API was not loaded (Wrong version), an error message is shown. 
	//
	if(!m_bWasLoaded)
		::MessageBox(NULL,_T("Error: \"DLL functions could not be loaded!\""), _T("Error!"), MB_ICONERROR);
}

void PLinApiClass::UnloadAPI()
{
	// Frees a loaded DLL
	//
	if(m_hDll != NULL)
		FreeLibrary(m_hDll);
	m_hDll = NULL;	

	// Initializes pointers
	//
	InitializePointers();

	m_bWasLoaded = false;
}

void PLinApiClass::InitializePointers()
{
	// Initializes thepointers for the PCANBasic functions
	//
	m_pRemoveClient = NULL;
	m_pResetClient = NULL;	
	m_pConnectClient = NULL;	
	m_pDisconnectClient = NULL;	
	m_pResetHardware = NULL;	
	m_pSuspendKeepAlive = NULL;	
	m_pResumeKeepAlive = NULL;	
	m_pResetHardwareConfig = NULL;	
	m_pSuspendSchedule = NULL;	
	m_pResumeSchedule = NULL;	
	m_pXmtWakeUp = NULL;	
	m_pDeleteSchedule = NULL;	
	m_pStartSchedule = NULL;	
	m_pRegisterClient = NULL;
	m_pIdentifyHardware = NULL;
	m_pSetClientParam = NULL;
	m_pGetClientParam = NULL;
	m_pSetClientFilter = NULL;
	m_pGetClientFilter = NULL;
	m_pRead = NULL;
	m_pReadMulti = NULL;
	m_pWrite = NULL;
	m_pInitializeHardware = NULL;
	m_pGetAvailableHardware = NULL;
	m_pSetHardwareParam = NULL;
	m_pGetHardwareParam = NULL;
	m_pRegisterFrameId = NULL;
	m_pSetFrameEntry = NULL;
	m_pGetFrameEntry = NULL;
	m_pUpdateByteArray = NULL;
	m_pStartKeepAlive = NULL;
	m_pSetSchedule = NULL;
	m_pGetSchedule = NULL;
	m_pSetScheduleBreakPoint = NULL;
	m_pStartAutoBaud = NULL;
	m_pGetStatus = NULL;
	m_pCalculateChecksum = NULL;
	m_pGetVersion = NULL;
	m_pGetVersionInfo = NULL;
	m_pGetErrorText = NULL;
	m_pGetPID = NULL;
	m_pGetTargetTime = NULL;
	m_pSetResponseRemap = NULL;
	m_pGetResponseRemap = NULL;
	m_pGetSystemTime = NULL;
}

bool PLinApiClass::LoadDllHandle()
{   
	// Was already loaded
	//
	if(m_bWasLoaded)
		return true;

	//Loads Dll
	//
	m_hDll = LoadLibrary("PLinApi");

	// Return true if the DLL was loaded or false otherwise
	//
	return (m_hDll != NULL);
}

FARPROC PLinApiClass::GetFunction(LPSTR szName)
{
	// There is no DLL loaded
	//
	if(m_hDll == NULL)
		return NULL;

	// Gets the address of the given function in the loeaded DLL
	//
	return GetProcAddress(m_hDll, szName);
}

//	Public API functions
//
TLINError PLinApiClass::RegisterClient(LPSTR strName, DWORD hWnd, HLINCLIENT *hClient)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pRegisterClient(strName, hWnd, hClient);
}

TLINError PLinApiClass::RemoveClient(HLINCLIENT hClient)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pRemoveClient(hClient);
}

TLINError PLinApiClass::ConnectClient(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pConnectClient(hClient, hHw);
}

TLINError PLinApiClass::DisconnectClient(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pDisconnectClient(hClient, hHw);
}                

TLINError PLinApiClass::ResetClient(HLINCLIENT hClient)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pResetClient(hClient);
}

TLINError PLinApiClass::SetClientParam(HLINCLIENT hClient, WORD wParam, DWORD dwValue)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetClientParam(hClient, wParam, dwValue);
}

TLINError PLinApiClass::GetClientParam(HLINCLIENT hClient, WORD wParam, void *pBuff, WORD wBuffSize)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetClientParam(hClient, wParam, pBuff, wBuffSize);
}

TLINError PLinApiClass::SetClientFilter(HLINCLIENT hClient, HLINHW hHw, unsigned __int64 iRcvMask)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetClientFilter(hClient, hHw, iRcvMask);
}

TLINError PLinApiClass::GetClientFilter(HLINCLIENT hClient, HLINHW hHw, unsigned __int64 *pRcvMask)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetClientFilter(hClient, hHw, pRcvMask);
}

TLINError PLinApiClass::Read(HLINCLIENT hClient, TLINRcvMsg *pMsg)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pRead(hClient, pMsg);
}

TLINError PLinApiClass::ReadMulti(HLINCLIENT hClient, TLINRcvMsg *pMsgBuff, int iMaxCount, int *pCount)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pReadMulti(hClient, pMsgBuff, iMaxCount, pCount);
}

TLINError PLinApiClass::Write(HLINCLIENT hClient, HLINHW hHw, TLINMsg *pMsg)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pWrite(hClient, hHw, pMsg);
}

TLINError PLinApiClass::InitializeHardware(HLINCLIENT hClient, HLINHW hHw, BYTE bMode, WORD wBaudrate)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pInitializeHardware(hClient, hHw, bMode, wBaudrate);
}

TLINError PLinApiClass::GetAvailableHardware(HLINHW *pBuff, WORD wBuffSize, int *pCount)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetAvailableHardware(pBuff, wBuffSize, pCount);
}

TLINError PLinApiClass::SetHardwareParam(HLINCLIENT hClient, HLINHW hHw, WORD wParam, void *pBuff, WORD wBuffSize)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetHardwareParam(hClient, hHw, wParam, pBuff, wBuffSize);
}

TLINError PLinApiClass::GetHardwareParam(HLINHW hHw, WORD wParam, void *pBuff, WORD wBuffSize)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetHardwareParam(hHw, wParam, pBuff, wBuffSize);
}

TLINError PLinApiClass::ResetHardware(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pResetHardware(hClient, hHw);
}

TLINError PLinApiClass::ResetHardwareConfig(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pResetHardwareConfig(hClient, hHw);
}

TLINError PLinApiClass::IdentifyHardware(HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pIdentifyHardware( hHw);
}

TLINError PLinApiClass::RegisterFrameId(HLINCLIENT hClient, HLINHW hHw, BYTE bFromFrameId, BYTE bToFrameId)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pRegisterFrameId(hClient, hHw, bFromFrameId, bToFrameId);
}

TLINError PLinApiClass::SetFrameEntry(HLINCLIENT hClient, HLINHW hHw, TLINFrameEntry *pFrameEntry)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetFrameEntry(hClient, hHw, pFrameEntry);
}

TLINError PLinApiClass::GetFrameEntry(HLINHW hHw, TLINFrameEntry *pFrameEntry)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetFrameEntry(hHw, pFrameEntry);
}

TLINError PLinApiClass::UpdateByteArray(HLINCLIENT hClient, HLINHW hHw, BYTE bFrameId, BYTE bIndex, BYTE bLen, BYTE *pData)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pUpdateByteArray(hClient, hHw, bFrameId, bIndex, bLen, pData);
}

TLINError PLinApiClass::StartKeepAlive(HLINCLIENT hClient, HLINHW hHw, BYTE bFrameId, WORD wPeriod)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pStartKeepAlive(hClient, hHw, bFrameId, wPeriod);
}

TLINError PLinApiClass::SuspendKeepAlive(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSuspendKeepAlive(hClient, hHw);
}

TLINError PLinApiClass::ResumeKeepAlive(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pResumeKeepAlive(hClient, hHw);
}

TLINError PLinApiClass::SetSchedule(HLINCLIENT hClient, HLINHW hHw, int iScheduleNumber, TLINScheduleSlot *pSchedule, int iSlotCount)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetSchedule(hClient, hHw, iScheduleNumber, pSchedule, iSlotCount);
}

TLINError PLinApiClass::GetSchedule(HLINHW hHw, int iScheduleNumber, TLINScheduleSlot *pScheduleBuff, int iMaxSlotCount, int *pSlotCount)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetSchedule(hHw, iScheduleNumber, pScheduleBuff, iMaxSlotCount, pSlotCount);
}

TLINError PLinApiClass::DeleteSchedule(HLINCLIENT hClient, HLINHW hHw, int iScheduleNumber)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pDeleteSchedule(hClient, hHw, iScheduleNumber);
}

TLINError PLinApiClass::SetScheduleBreakPoint(HLINCLIENT hClient, HLINHW hHw, int iBreakPointNumber, DWORD dwHandle)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetScheduleBreakPoint(hClient, hHw, iBreakPointNumber, dwHandle);
}

TLINError PLinApiClass::StartSchedule(HLINCLIENT hClient, HLINHW hHw, int iScheduleNumber)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pStartSchedule(hClient, hHw, iScheduleNumber);
}

TLINError PLinApiClass::SuspendSchedule(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSuspendSchedule(hClient, hHw);
}
TLINError PLinApiClass::ResumeSchedule(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pResumeSchedule(hClient, hHw);
}

TLINError PLinApiClass::XmtWakeUp(HLINCLIENT hClient, HLINHW hHw)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pXmtWakeUp(hClient, hHw);
}

TLINError PLinApiClass::StartAutoBaud(HLINCLIENT hClient, HLINHW hHw, WORD wTimeOut)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pStartAutoBaud(hClient, hHw, wTimeOut);
}

TLINError PLinApiClass::GetStatus(HLINHW hHw, TLINHardwareStatus *pStatusBuff)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetStatus(hHw, pStatusBuff);
}

TLINError PLinApiClass::CalculateChecksum(TLINMsg *pMsg)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pCalculateChecksum(pMsg);
}

TLINError PLinApiClass::GetVersion(TLINVersion *pVerBuff)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetVersion(pVerBuff);
}

TLINError PLinApiClass::GetVersionInfo(LPSTR strTextBuff, WORD wBuffSize)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetVersionInfo(strTextBuff, wBuffSize);
}

TLINError PLinApiClass::GetErrorText(DWORD dwError, BYTE bLanguage, LPSTR strTextBuff, WORD wBuffSize)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetErrorText(dwError, bLanguage, strTextBuff, wBuffSize);
}

TLINError PLinApiClass::GetPID(BYTE *pFrameId)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetPID(pFrameId);
}

TLINError PLinApiClass::GetTargetTime(HLINHW hHw, unsigned __int64 *pTargetTime)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetTargetTime(hHw, pTargetTime);
}

TLINError PLinApiClass::SetResponseRemap(HLINCLIENT hClient, HLINHW hHw, BYTE *pRemapTab)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pSetResponseRemap(hClient, hHw, pRemapTab);
}


TLINError PLinApiClass::GetResponseRemap(HLINHW hHw, BYTE *pRemapTab)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetResponseRemap(hHw, pRemapTab);
}

TLINError PLinApiClass::GetSystemTime(unsigned __int64 *pSystemTime)
{
	if (!m_bWasLoaded)
		return errUnknown;

	return m_pGetSystemTime(pSystemTime);
}