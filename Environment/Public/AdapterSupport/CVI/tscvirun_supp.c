#include <cviauto.h>
#include <ansi_c.h>
#include <userint.h>

/**********************************************************************
 * PURPOSE:
 * The purpose of this file is to load and initialize the tscvirun.dll
 * file. The tscvirun.dll file is an ActiveX automation server whose
 * purpose is to load C, OBJ, DLL, and LIB modules and execute functions
 * in them when requested by the TestStand CVI Adapter. This file must
 * be in the project in CVI which is run by TestStand in order to run
 * tests in an external instance of CVI.
 **********************************************************************/

/* Declare the variables that hold the addresses of the function   */
/* pointers.                                                       */
static int InitCVITEAutomationServer(void);
static int ProcessCVITEAutomationServerEvents(void);

#define TECRUN_INIT_FAILED	"TSCVIrun automation server initialization failed."

#if _CVI_ < 550
	#error This version of tscvirun_supp.c requires CVI 5.5 or greater.
#endif

/**********************************************************************/

int main(void)
{
	char * errMsgPtr = NULL;
	
	if (!InitCVITEAutomationServer())
	{
		errMsgPtr = TECRUN_INIT_FAILED;
		goto Error;
	}

	SetSleepPolicy (VAL_SLEEP_NONE);
	SetSystemAttribute (ATTR_TASKBAR_BUTTON_VISIBLE, 0);

	ProcessCVITEAutomationServerEvents();
	
	return 0;

Error:

	MessagePopup ("An Error Occurred", errMsgPtr);
	return 1;
}

/**********************************************************************/

#define TSCVIRUN_INSTANCE_NAME_FMT_STR	"tscvirun instance - 0x%x"
const CLSID CLSID_InterfaceServer = {0xB5B6C10D,0x3E82,0x11D3,{0xBC,0x07,0x00,0x60,0x08,0xCA,0x11,0xD4}};
const IID IID_IInterfaceServer = {0xB5B6C10C,0x3E82,0x11D3,{0xBC,0x07,0x00,0x60,0x08,0xCA,0x11,0xD4}};

static int RegisterTSCVIRUNInstance(CAObjHandle tscvirunH, long myProcId)
{
	int success = FALSE;
	char nameBuf[100];
	CAObjHandle tsautomgrH = 0;
	unsigned int __paramTypes[] = {CAVT_OBJHANDLE, CAVT_CSTRING};

	if(CA_CreateObjectByClassIdEx (&CLSID_InterfaceServer, NULL,
								   &IID_IInterfaceServer, 1, LOCALE_NEUTRAL,
								   0, &tsautomgrH) < 0)
		goto Error;
	
	sprintf(nameBuf, TSCVIRUN_INSTANCE_NAME_FMT_STR, myProcId);

	if(CA_MethodInvoke (tsautomgrH, NULL, 0x4, CAVT_EMPTY, NULL, 2,
						__paramTypes, tscvirunH, nameBuf) < 0)
		goto Error;
		
	success = TRUE;

Error:

	CA_DiscardObjHandle(tsautomgrH);

	if(!success)
	{
		MessageBox(NULL, "Unable to connect to TSAutoMgr.exe! Verify that TestStand is installed correctly.",
				   "TSCVIRUN Error:", MB_OK|MB_ICONERROR);
	}

	return success;
}

/**********************************************************************/

const CLSID CLSID_CVITestManager = {0xDFB6E4CD,0x3888,0x11D4,{0x8F,0xB1,0x00,0x01,0x02,0x46,0xBC,0xD1}};
const IID IID_ICVITestManager = {0xDFB6E4CC,0x3888,0x11D4,{0x8F,0xB1,0x00,0x01,0x02,0x46,0xBC,0xD1}};
static int InitCVITEAutomationServer(void)
{
	int success = FALSE;
	CAObjHandle tscvirunH = 0;

	// Create an instance of CVITestManager and register it with TSAutoMgr
	if(CA_CreateObjectByClassIdEx (&CLSID_CVITestManager, NULL,
								   &IID_ICVITestManager, 1, LOCALE_NEUTRAL,
								   0, &tscvirunH) < 0)
		goto Error;

	if(RegisterTSCVIRUNInstance(tscvirunH, GetCurrentProcessId()))
		success = TRUE;

Error:

	CA_DiscardObjHandle(tscvirunH);
	return success;
}

/**********************************************************************/

static int ProcessCVITEAutomationServerEvents(void)
{
	// Make sure preliminary window's messages for the ole server are processed 
	ProcessSystemEvents();
	// Synchronize with client
	{
		HANDLE syncWithClientEvent = NULL;
		char buffer[100];

		sprintf(buffer, "tecvirun.dll SyncWithClient - 0x%x", GetCurrentProcessId());
		syncWithClientEvent = CreateEvent(NULL, FALSE, FALSE, buffer);
		if(syncWithClientEvent)
			SetEvent(syncWithClientEvent);
		// Do not discard event handle because we don't want event to go away.
	}

	while(1)
	{
		WaitMessage();
		ProcessSystemEvents();
	}
	
	return 0;
}

/**********************************************************************/
