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
File    : JLink_Start.c
Purpose : Sample application to show J-Link DLL startup sequence
--------  END-OF-HEADER  ---------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include "JLinkARMDLL.h"
#include "SYS.h"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef struct {
  U32 HostIF;                 // Host interface used to connect to J-Link. 1 = USB, 2 = IP
  U32 TargetIF;               // See JLINKARM_Const.h "Interfaces" for valid values
  U32 SerialNo;               // Serial number of J-Link we want to connect to via USB
  U32 Speed;                  // Target interface speed in kHz
  const char* sHost;          // Points to the IPAddr / nickname of the J-Link we want to connect to.
  const char* sSettingsFile;  // Points to J-Link settings file to store connection settings
  const char* sDevice;        // Target device J-Link is connected to
} INIT_PARAS;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static INIT_PARAS _Paras;
static char       _acSettingsFileName[256];
static char       _acHostname[64];
static char       _acDevice[128];

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/********************************************************************* 
* 
*       _LogOut()
* 
* Function description 
*   Print log output to console.
*/
static void _LogOut(const char* sLog) {
  printf("Log: %s\n", sLog);
}

/********************************************************************* 
* 
*       _ErrorOut()
* 
* Function description 
*   Print error output to console.
*/
static void _ErrorOut(const char* sError) {
  printf("\nERROR: %s\n", sError);
}

/********************************************************************* 
* 
*       _ParseDec()
* 
* Function description 
*   Parse a decimal number from a string
*/
static const char* _ParseDec(const char** ps, U32* pData) {
  U32 Data;
  int NumDigits;

  Data = 0;
  NumDigits = 0;
  
  do {
    int v;
    char c;

    c = **ps;
    v =  ((c >= '0') && (c <= '9')) ? c - '0' : -1;
    if (v >= 0) {
      Data = (Data * 10) + v;
      (*ps)++;
      NumDigits++;
    } else {
      if (NumDigits == 0) {
        return "Expected a dec value";
      }
      *pData = Data;
      return NULL;
    }
  } while (1);
}

/********************************************************************* 
* 
*       _InitConfig()
* 
* Function description 
*   Get the configuration used for the startup sequence from user input.
*   Can be extended to parse the command line.
*/
static int _InitConfig(int argc, char* argv[]) {
  char  ac[256];
  char* s;
  const char* sErr;
  U32   v;

  printf("Enter the device name.          Default: unspecified\n");
  s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
  if (*s) {
    sprintf(_acDevice, "%s", s);
    _Paras.sDevice = _acDevice;
  } else {
    sprintf(_acDevice, "unspecified");
    _Paras.sDevice = _acDevice;
  }
  printf("Target interface. 0: JTAG, 1: SWD.      Default: SWD\n");
  s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
  if (*s) {
    switch (*s) {
    case '0':
      _Paras.TargetIF = JLINKARM_TIF_JTAG;
      break;
    case '1':
      _Paras.TargetIF = JLINKARM_TIF_SWD;
      break;
    default:
      _ErrorOut("Could not get target interface.\n");
      return -1;
      break;
    }
  } else {
    _Paras.TargetIF = JLINKARM_TIF_SWD;
  }
  printf("Interface speed [kHz].             Default: 4000 kHz\n");
  s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
  if (*s) {
    sErr = _ParseDec((const char**)&s, &v);
    if (sErr) {
      _ErrorOut(sErr);
      _ErrorOut("Could not get interface speed.\n");
      return -1;
    }
    _Paras.Speed = v;
  } else {
    _Paras.Speed = 4000;
  }
  printf("Host interface. 0: USB, 1: IP.          Default: USB\n");
  s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
  if (*s) {
    switch (*s) {
    case '0':
      _Paras.HostIF = JLINKARM_HOSTIF_USB;
      break;
    case '1':
      _Paras.HostIF = JLINKARM_HOSTIF_IP;
      break;
    default:
      _ErrorOut("Could not get target interface.\n");
      return -1;
      break;
    }
  } else {
    _Paras.HostIF = JLINKARM_HOSTIF_USB;
  }
  if (_Paras.HostIF == JLINKARM_HOSTIF_USB) {
    printf("J-Link serial number.                  Default: none\n");
    s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
    if (*s) {
      sErr = _ParseDec((const char**)&s, &v);
      if (sErr) {
        _ErrorOut(sErr);
        _ErrorOut("Could not get serial number.\n");
        return -1;
      }
      _Paras.SerialNo = v;
    } else {
      _Paras.SerialNo = 0;
    }
  } else {
    printf("J-Link IP or host name.\n");
    s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
    if (*s) {
      sprintf(_acHostname, "%s", s);
      _Paras.sHost = _acHostname;
    } else {
      _ErrorOut("Could not get IP or host name.\n");
      return -1;
    }
  }
  printf("J-Link settings file.         Default: Settings.jlink\n");
  s = SYS_ConsoleGetString("> ", ac, sizeof(ac));
  if (*s) {
    sprintf(_acSettingsFileName, "%s", s);
    _Paras.sSettingsFile = _acSettingsFileName;
  } else {
    sprintf(_acSettingsFileName, "Settings.jlink");
    _Paras.sSettingsFile = _acSettingsFileName;
  }

  return 0;
}

/*********************************************************************
*
*       _InitDebugSession()
*
*  Function description
*    Initializes the debug session by connecting to a J-Link,
*    setting up the J-Link DLL and J-Link and finally connecting to the target system
*
*  Return value
*      0 O.K.
*    < 0 Error
*/
int _InitDebugSession(INIT_PARAS* pParas) {
  const char* sError;
  U8 acIn[0x400];
  U8 acOut[0x400];
  int r;
  //
  // Select and configure host interface (USB is default)
  //
  if (pParas->HostIF == JLINKARM_HOSTIF_IP) {  // Host interface == IP?
    //
    // If sHost is NULL, J-Link selection dialog will pop-up
    // on JLINKARM_Open() / JLINKARM_OpenEx(), showing all
    // emulators that have been found in the network    
    // Passing 0 as port selects the default port (19020).
    //
    r = JLINKARM_SelectIP(pParas->sHost, 0);
    if (r == 1) {
      return -1;  // Error occurred during configuration of IP interface
   }
  } else {        // Connect via USB
    //
    // Was a specific serial number set we shall to connect to?
    //
    if (pParas->SerialNo) {
      r = JLINKARM_EMU_SelectByUSBSN(pParas->SerialNo);
      if (r < 0) {
        return -1;    // Error: Specific serial number not found on USB
     }
    }
  }
  //
  // Open communication with J-Link
  //
  sError = JLINKARM_OpenEx(_LogOut, _ErrorOut);
  if (sError) {                   // Error occurred while connecting to J-Link?
    _ErrorOut(sError);
    return -1;
  }
  //
  // Select settings file
  // Used by the control panel to store its settings and can be used by the user to
  // enable features like flash breakpoints in external CFI flash, if not selectable
  // by the debugger. There should be different settings files for different debug
  // configurations, for example different settings files for LEDSample_DebugFlash
  // and LEDSample_DebugRAM. If this file does not exist, the DLL will create one
  // with default settings. If the file is already present, it will be used to load
  // the control panel settings
  //
  if (pParas->sSettingsFile) {
    strcpy(acIn, "ProjectFile = ");
    strcat(acIn, pParas->sSettingsFile);
    JLINKARM_ExecCommand(acIn, acOut, sizeof(acOut));
    if (acOut[0]) {
      _ErrorOut(acOut);
      return -1;
    }
  }
  //
  // Select device or core
  //
  if (pParas->sDevice) {
    strcpy(acIn, "device = ");
    strcat(acIn, pParas->sDevice);
    JLINKARM_ExecCommand(acIn, &acOut[0], sizeof(acOut));
    if (acOut[0]) {
      _ErrorOut(acOut);
      return -1;
    }
  }
  //
  // Select and configure target interface
  // If not called, J-Link will use the interface which was configured before. If
  // J-Link is power-cycled, JTAG is the default target interface.
  // It is recommended to always select the interface at debug startup.
  //
  JLINKARM_TIF_Select(pParas->TargetIF);
  //
  // Select target interface speed which
  // should be used by J-Link for target communication
  //
  JLINKARM_SetSpeed(pParas->Speed);
  //
  // Connect to target CPU
  //
  r = JLINKARM_Connect();
  if (r) {
    _ErrorOut("Could not connect to target.");
    return -1;
  }
  return 0;
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
  //
  // Zero-initialize structure to make sure that default values are configured
  // for fields that are not used.
  //
  memset(&_Paras, 0, sizeof(INIT_PARAS));
  r = _InitConfig(argc, argv);
  if (r < 0) {
    return 0;
  }
  printf("Initializing debugger...\n");
  r = _InitDebugSession(&_Paras);
  if (r < 0) {
    printf("Initialization of debug session failed.\n");
    if (JLINKARM_IsOpen()) {                                // If InitDebugSession() failed after DLL/connection to J-Link has been opened, make sure we close it gracefully
      JLINKARM_Close();
    }
    return 1;
  }
  printf("Debugger initialized successfully.\n");
  //
  // From now on the target is identified and we can start working with it.
  //
  /* Insert your code here... */
  //
  // Everything is done. Close connection to J-Link.
  //
  JLINKARM_Close();
  return 0;
}