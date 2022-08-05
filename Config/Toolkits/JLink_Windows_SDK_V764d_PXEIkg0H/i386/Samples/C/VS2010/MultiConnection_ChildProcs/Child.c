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
File    : Child.c
Purpose : Sample (child) application to show how to load multiple instances of the J-Link DLL using child processes
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

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static SYS_SOCKET_HANDLE _hSock;
static U32 _ProcID;

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

static void _SendToParent(const char* s) {
  char ac[256];
  U32  NumBytes;

  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "PID %d: %s\n", _ProcID, s);
  NumBytes = UTIL_strlen(ac);
  SYS_SOCKET_Send(_hSock, &NumBytes, UTIL_SIZEOF(NumBytes));
  SYS_SOCKET_Send(_hSock, ac, NumBytes);
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
  U32 Port;
  U32 SerialNo;
  const char* sErr;
  char ac[128];
  //
  // The parent process passes the following command line options:
  // (1) TCP/IP port used for inter-process communication 
  // (2) Serial number of J-Link/ Flasher to connect to
  //
  _hSock = SYS_INVALID_SOCKET;
  Result = -1;
  if (argc < 3) {  // Missing options? => Done
    printf("ERROR: Missing command line options!\n");
    goto Done;
  }
  r = UTIL_ParseInt(argv[1], &Port);
  if (r < 0) {     // Failed to parse port? => Done
    printf("ERROR: Failed to parse port!\n");
    goto Done;
  }
  r = UTIL_ParseInt(argv[2], &SerialNo);
  if (r < 0) {     // Failed to parse S/N? => Done
    printf("ERROR: Failed to parse S/N!\n");
    goto Done;
  }
  //
  // Setup communication with parent process
  //
  _ProcID = SYS_GetCurrentProcessId();
  _hSock = SYS_SOCKET_OpenTCP();
  if (_hSock == SYS_INVALID_SOCKET) {
    printf("ERROR: Failed to open socket!\n");
    goto Done;
  }
  r = SYS_SOCKET_Connect(_hSock, SYS_SOCKET_IP_ADDR_LOCALHOST, Port);
  if (r < 0) {
    printf("ERROR: Failed to connect to parent process!\n");
    goto Done;
  }
  //
  // Send "welcome" message to parent
  //
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "Handling S/N %d", SerialNo);
  _SendToParent(ac);
  //
  // Connect to J-Link
  //
  r = JLINKARM_EMU_SelectByUSBSN(SerialNo);
  if (r < 0) {  // Failed to select J-Link? => Done
    _SendToParent("Failed to select J-Link");
    goto Done;
  }
  sErr = JLINK_Open();
  if (sErr) {
    _SendToParent("Failed to connect to J-Link");
    goto Done;
  }
  //
  // Send J-Link firmware string to parent
  //
  JLINKARM_GetFirmwareString(ac, UTIL_SIZEOF(ac));
  _SendToParent(ac);
  Result = 0;
Done:
  //
  // Clean up
  //
  r = JLINKARM_IsOpen();
  if (r) {
    JLINKARM_Close();
  }
  if (_hSock != SYS_INVALID_SOCKET) {
    SYS_SOCKET_Close(_hSock);
  }
  return Result;
}