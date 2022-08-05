/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*            (c) 1995 - 2018 SEGGER Microcontroller GmbH             *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : MultiConnection_DLLCopies.c
Purpose : Sample application to show how to load multiple instances of the J-Link DLL using copies of the same .dll file
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "JLinkARMDLL.h"
#include "SYS.h"
#include "UTIL.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <winbase.h>

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define _MAX_NUM_CONNECTIONS      (8)       // Max. number of connections to handle. This is *not* a limitation by the J-Link library.
#define _DLL_NAME                 "JLinkARM"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef const char* (_JLINKARM_OPEN_FUNC)         (JLINKARM_LOG* pfLog, JLINKARM_LOG* pfErr);
typedef void        (_JLINKARM_CLOSE_FUNC)        (void);
typedef void        (_JLINKARM_GETFWSTRING_FUNC)  (char* s, int BufferSize);
typedef int         (_JLINKARM_EMU_SELECTBYUSBSN) (U32 SerialNo);

typedef struct {
  _JLINKARM_OPEN_FUNC*           pfOpen;
  _JLINKARM_CLOSE_FUNC*          pfClose;
  _JLINKARM_GETFWSTRING_FUNC*    pfGetFWString;
  _JLINKARM_EMU_SELECTBYUSBSN*   pfSelectByUSBSN;
} _JLINK_API;

typedef struct {
  char       acPath[SYS_MAX_PATH];
  HMODULE    hLib;
  _JLINK_API Api;
} _DLL_INSTANCE_INFO;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static _DLL_INSTANCE_INFO _aDLLInfo[_MAX_NUM_CONNECTIONS];

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _InitDLLApi()
*
*  Function description
*    This function initializes the API function pointers (adresses) for the given DLL instance.
*/
static void _InitDLLApi(_DLL_INSTANCE_INFO* pDLLInst) {
  ZEROFILL(pDLLInst->Api);
  pDLLInst->Api.pfOpen          = (_JLINKARM_OPEN_FUNC*)        GetProcAddress(pDLLInst->hLib, "JLINKARM_Open");
  pDLLInst->Api.pfClose         = (_JLINKARM_CLOSE_FUNC*)       GetProcAddress(pDLLInst->hLib, "JLINKARM_Close");
  pDLLInst->Api.pfGetFWString   = (_JLINKARM_GETFWSTRING_FUNC*) GetProcAddress(pDLLInst->hLib, "JLINKARM_GetFirmwareString");
  pDLLInst->Api.pfSelectByUSBSN = (_JLINKARM_EMU_SELECTBYUSBSN*)GetProcAddress(pDLLInst->hLib, "JLINKARM_EMU_SelectByUSBSN");
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       main()
*/
int main(int argc, char* argv[], char* envp[]) {
  int r;
  int Result;
  U32 i;
  U32 NumInfo;
  char acOrgDLL[SYS_MAX_PATH];
  char acDrive[SYS_MAX_DRIVE];
  char acDir[SYS_MAX_DIR];
  char acFName[SYS_MAX_FNAME];
  char acExt[SYS_MAX_EXT];
  char ac[SYS_MAX_PATH];
  const char* sErr;
  JLINKARM_EMU_CONNECT_INFO* paInfo;
  const _JLINK_API* pAPI;
  //
  // Init locals
  //
  NumInfo = 0;
  paInfo  = NULL;
  Result  = -1;
  //
  // First, determine number of connections to open
  //
  printf("Looking for J-Links and Flashers connected via USB...\n");
  r = JLINKARM_EMU_GetList(JLINKARM_HOSTIF_USB, NULL, 0);
  if (r < 0) {  // Failed to get list? => Done
    printf("ERROR: Failed to get list of emulators...\n");
    goto Done;
  }
  if (r == 0) {
    printf("No J-Links or Flashers connected.");
    Result = 0;
    goto Done;
  }
  //
  // Reserve mem to store information about J-Links/ Flashers
  //
  NumInfo = MIN(_MAX_NUM_CONNECTIONS, (U32)r);
  paInfo  = (JLINKARM_EMU_CONNECT_INFO*)SYS_MEM_Alloc(NumInfo * UTIL_SIZEOF(JLINKARM_EMU_CONNECT_INFO));
  if (paInfo == NULL) {  // Failed to alloc? => Done
    printf("ERROR: Failed to allocate memory...\n");
    goto Done;
  }
  //
  // Retrieve list of information for J-Links/ Flashers
  //
  r = JLINKARM_EMU_GetList(JLINKARM_HOSTIF_USB, paInfo, NumInfo);
  if (r < 0) {  // Failed to get list? => Done
    printf("ERROR: Failed to get list of emulators...\n");
    goto Done;
  }
  if (r == 0) {
    printf("No J-Links or Flashers connected.");
    Result = 0;
    goto Done;
  }
  NumInfo = MIN(NumInfo, (U32)r);
  //
  // Create copies of DLL file
  //
  printf("Creating DLL copies for %d connections...\n", NumInfo);
  SYS_GetModulePath(ac, UTIL_SIZEOF(ac));
  SYS_splitpath(ac, acDrive, UTIL_SIZEOF(acDrive), acDir, UTIL_SIZEOF(acDir), acFName, UTIL_SIZEOF(acFName), acExt, UTIL_SIZEOF(acExt));
  UTIL_CopyString(acExt, ".dll", UTIL_SIZEOF(acExt));
  SYS_makepath(acOrgDLL, UTIL_SIZEOF(acOrgDLL), acDrive, acDir, _DLL_NAME, acExt);
  for (i = 0; i < NumInfo; i++) {
    UTIL_snprintf(acFName, UTIL_SIZEOF(acFName), _DLL_NAME "_%d", i + 1);
    SYS_makepath(_aDLLInfo[i].acPath, UTIL_SIZEOF(_aDLLInfo[i].acPath), acDrive, acDir, acFName, acExt);
    //
    // First, we try to delete any existing file with
    // the same name as the copy we are about to create.
    //
    SYS_FILE_Delete(_aDLLInfo[i].acPath);
    r = CopyFile(acOrgDLL, _aDLLInfo[i].acPath, 1);
    if (r == 0) {  // Failed to create copy? => Done
      printf("Failed to create copy '%s'...\n", _aDLLInfo[i].acPath);
      goto Done;
    }
  }
  //
  // Load DLL instances into memory and initialize API pointers
  //
  printf("Loading %d DLL copies...\n", NumInfo);
  for (i = 0; i < NumInfo; i++) {
    _aDLLInfo[i].hLib = LoadLibrary(_aDLLInfo[i].acPath);
    if (_aDLLInfo[i].hLib == NULL) {
      printf("Failed to load '%s'...\n", _aDLLInfo[i].acPath);
      goto Done;
    }
    _InitDLLApi(&_aDLLInfo[i]);
  }
  //
  // Handle multiple open connections
  //
  printf("Opening and handling %d connections...\n", NumInfo);
  for (i = 0; i < NumInfo; i++) {
    pAPI = &_aDLLInfo[i].Api;
    r    = pAPI->pfSelectByUSBSN((paInfo + i)->SerialNumber);
    if (r < 0) {
      printf("DLL %d: Failed to select J-Link.\n", i);
      continue;
    }
    sErr = pAPI->pfOpen(NULL, NULL);
    if (sErr) {
      printf("DLL %d: Failed to connect to J-Link.\n", i);
      continue;
    }
    printf("DLL %d: Connected to J-Link (S/N %d).\n", i, (paInfo + i)->SerialNumber);
    pAPI->pfGetFWString(ac, UTIL_SIZEOF(ac));
    printf("DLL %d: %s\n", i, ac);
  }
  SYS_ConsoleGetString("Press <Return> to quit...", &ac[0], UTIL_SIZEOF(ac));  // Wait for user input before shutting down
  //
  // Close connections
  //
  for (i = 0; i < NumInfo; i++) {
    pAPI = &_aDLLInfo[i].Api;
    pAPI->pfClose();
    printf("DLL %d: Closed connection\n", i);
  }
  Result = 0;
Done:
  //
  // Clean up
  //
  for (i = 0; i < NumInfo; i++) {
    if (_aDLLInfo[i].hLib) {
      FreeLibrary(_aDLLInfo[i].hLib);
    }
    SYS_FILE_Delete(_aDLLInfo[i].acPath);
  }
  if (paInfo) {
	  SYS_MEM_Free(paInfo);
  }
  return Result;
}