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
File    : Parent.c
Purpose : Sample (parent) application to show how to load multiple instances of the J-Link DLL using child processes
--------  END-OF-HEADER  ---------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include "JLinkARMDLL.h"
#include "SYS.h"
#include "UTIL.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define _MAX_NUM_CONNECTIONS      (8)       // Max. number of connections to handle. This is *not* a limitation by the J-Link library.
#define _PROC_COMM_PORT           (19099)   // TCP/IP port used for communication between parent process and child processes
#define _NUM_CONN_BACKLOG         (4)       // Max. number of TCP/IP connections to hold in backlog
#define _PROC_COMM_SETUP_TIMEOUT  (1000)    // Max. [ms] to wait before TCP/IP communication is ready
#define _PROC_COMM_CLOSE_TIMEOUT  (1000)    // Max. [ms] to wait before TCP/IP communication is shutdown gracefully
#ifdef _DEBUG
  #define _CHILD_EXE_NAME           "MultiConnection_ChildProc_Debug.exe"
#else
  #define _CHILD_EXE_NAME           "MultiConnection_ChildProc.exe"
#endif

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct {
  SYS_HANDLE hEventRunning;
  SYS_HANDLE hEventStopped;
  SYS_SOCKET_HANDLE hSockListen;
  volatile U8 StopRequest;
} _THREAD_INFO;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _cbThreadComm
*
*  Function description
*    Handles TCP/IP communication between child processes and this process.
*
*  Parameters
*    p   Event to set as soon as the inter-process communication is set up
*/
static SYS_THREAD_PROC_EX_TYPE _cbThreadComm(void* p) {
  int r;
  U32 i;
  U32 NumConns;
  U32 NumBytes;
  char ac[256];
  _THREAD_INFO* pThreadInfo;
  SYS_SOCKET_HANDLE hSockTmp;
  SYS_SOCKET_HANDLE aChildSockets[_MAX_NUM_CONNECTIONS];

  NumConns    = 0;
  pThreadInfo = (_THREAD_INFO*)p;
  SYS_SetEvent(pThreadInfo->hEventRunning);  // Inform main thread that this thread is running
  do {
    //
    // Accept incoming connections
    //
    hSockTmp = SYS_SOCKET_AcceptEx(pThreadInfo->hSockListen, 10);  // Wait max. 10ms for an incomming connection
    if (hSockTmp != SYS_SOCKET_INVALID_HANDLE) {  // Connection accepted?
      if (NumConns >= _MAX_NUM_CONNECTIONS) {     // Max. number of connections reached? => Close connection
        SYS_SOCKET_Close(hSockTmp);
      } else {
        aChildSockets[NumConns] = hSockTmp;
        NumConns++;
      }
    }
    //
    // Handle communication for open connections
    //
    for (i = 0; i < NumConns; i++) {
      hSockTmp = aChildSockets[i];
      r = SYS_SOCKET_ReceiveAll(hSockTmp, &NumBytes, UTIL_SIZEOF(NumBytes));
      if (r == UTIL_SIZEOF(NumBytes)) {
        SYS_SOCKET_ReceiveAll(hSockTmp, &ac[0], NumBytes);
        ac[NumBytes] = 0;
        printf(ac);
      }
    }
  } while (pThreadInfo->StopRequest == 0);
  SYS_SetEvent(pThreadInfo->hEventStopped);  // Inform main thread that this thread has closed
  return (SYS_THREAD_PROC_EX_R_TYPE)0;
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
  char ac[16];
  char acCL[64];
  char acPath[SYS_MAX_PATH];
  char acPathChild[SYS_MAX_PATH];
  U32 i;
  U32 NumInfo;
  _THREAD_INFO ThreadInfo;
  SYS_HANDLE hThread;
  JLINKARM_EMU_CONNECT_INFO* paInfo;
  //
  // Init locals
  //
  ZEROFILL(ThreadInfo);
  ThreadInfo.hSockListen = SYS_INVALID_SOCKET;
  ThreadInfo.hEventRunning = SYS_INVALID_HANDLE;
  ThreadInfo.hEventStopped = SYS_INVALID_HANDLE;
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
  printf("Setting up child processes for %d connections...\n", NumInfo);
  //
  // Before creating child processes, setup inter-process communication
  //
  ThreadInfo.hSockListen = SYS_SOCKET_OpenTCP();
  if (ThreadInfo.hSockListen == SYS_INVALID_SOCKET) {  // Failed to open socket?
    printf("ERROR: Failed to open socket...\n");
    goto Done;
  }
  r = SYS_SOCKET_ListenAtTCPAddr(ThreadInfo.hSockListen, SYS_SOCKET_IP_ADDR_LOCALHOST, _PROC_COMM_PORT, _NUM_CONN_BACKLOG);
  if (r < 0) {
    printf("ERROR: Failed to listen on socket...\n");
    goto Done;
  }
  ThreadInfo.hEventRunning = SYS_CreateEvent(0, 0);
  if (ThreadInfo.hEventRunning == SYS_INVALID_HANDLE) {
    printf("ERROR: Failed to create event...\n");
    goto Done;
  }
  ThreadInfo.hEventStopped = SYS_CreateEvent(0, 0);
  if (ThreadInfo.hEventStopped == SYS_INVALID_HANDLE) {
    printf("ERROR: Failed to create event...\n");
    goto Done;
  }
  hThread = SYS_CreateThreadEx(_cbThreadComm, (void*)&ThreadInfo, NULL, "Inter-process communication thread", 0);
  SYS_CloseThreadHandle(hThread);  // This does not close the thread itself. This simply prevents a thread-handle leak.
  //
  // Wait until inter-process communication is set up before starting child processes
  //
  printf("Setting up inter-process communication...\n");
  printf("\n");
  r = SYS_WaitForEvent(ThreadInfo.hEventRunning, _PROC_COMM_SETUP_TIMEOUT);
  if (r != SYS_WAIT_OK) {
    printf("ERROR: Inter-process communication was not ready in time...\n");
    goto Done;
  }
  //
  // Create child processes
  //
  SYS_GetModulePath(acPath, UTIL_SIZEOF(acPath));
  SYS_MakeAbsFilePath(acPath, _CHILD_EXE_NAME, acPathChild, UTIL_SIZEOF(acPathChild));
  for (i = 0; i < NumInfo; i++) {
    UTIL_snprintf(&ac[0], UTIL_SIZEOF(ac),     "Child %d", i);
    UTIL_snprintf(&acCL[0], UTIL_SIZEOF(acCL), "%d %d", _PROC_COMM_PORT, (paInfo + i)->SerialNumber);
    //
    // We pass the following data to the child process via command line:
    // (1) TCP/IP port used for inter-process communication 
    // (2) Serial number of J-Link/ Flasher to connect to
    //
    r = SYS_CreateCLIProcess(ac, acPathChild, acCL, NULL, 0);
    if (r != 0)  {
      printf("ERROR: Failed to create process '%s'...\n", ac);
    }
  }
  SYS_ConsoleGetString(NULL, &ac[0], UTIL_SIZEOF(ac));  // Wait for user input before shutting down
  ThreadInfo.StopRequest = 1;                           // Tell communication thread to close
  SYS_WaitForEvent(ThreadInfo.hEventStopped, _PROC_COMM_CLOSE_TIMEOUT);  // Wait for thread to close
  Result = 0;
Done:
  //
  // Clean up
  //
  if (ThreadInfo.hEventRunning != SYS_INVALID_HANDLE) {
    SYS_CloseEvent(ThreadInfo.hEventRunning);
  }
  if (ThreadInfo.hSockListen != SYS_INVALID_SOCKET) {
    SYS_SOCKET_Close(ThreadInfo.hSockListen);
  }
  if (paInfo) {
    SYS_MEM_Free(paInfo);
  }
  return Result;
}