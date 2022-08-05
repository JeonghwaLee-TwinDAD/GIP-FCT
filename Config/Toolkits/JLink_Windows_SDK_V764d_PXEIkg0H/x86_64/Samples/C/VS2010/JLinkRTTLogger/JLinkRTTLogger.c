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
File    : main.c
Purpose : J-Link RTT Logger
---------------------------END-OF-HEADER------------------------------
*/

#include "JLinkARMDLL.h"
#include "UTIL.h"
#include "SYS.h"

GLOBAL_DISABLE_WARNINGS()   // Avoid that 3rd party headers throw warnings
#include <stdio.h>
#include <signal.h>

#ifdef WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <termios.h>
  #include <limits.h>
  #include <unistd.h>
  #include <stdlib.h>
  #include <sys/ioctl.h>
#endif

#ifdef __APPLE__
  #include <readline/readline.h>
  #include <readline/history.h>
#endif

GLOBAL_ENABLE_WARNINGS()

static void _ErrorOut(const char* sText);

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

typedef struct {
  char  acDevice[128];
  U32   SerialNo;
  U32   HostIF;
  char  sHost[128];
  U32   TIF;
  U32   TIFSpeed;
  U32   RTTCBAddr;
  U32   RTTCBAddrValid;
  char  RTTCBRange[512];
  int   RTTChannel;
  char  acChannelName[32];
  char  acFileName[SYS_MAX_PATH];
} RTT_LOGGER_CONFIG;

typedef struct {
  U32 HostIF;                 // Host interface used to connect to J-Link. 0 = USB, 1 = IP
  U32 TargetIF;               // See JLINKARM_Const.h "Interfaces" for valid values
  U32 SerialNo;               // Serial number of J-Link we want to connect to via USB
  U32 Speed;                  // Interface speed in kHz
  const char* sHost;          // Points to the IPAddr / nickname of the J-Link we want to connect to.
  const char* sSettingsFile;
  const char* sDevice;        // Target device J-Link is connected to
} INIT_PARAS;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static char _CloseNow;
static char _abData[1024*1024];
static RTT_LOGGER_CONFIG  _Config;

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

/*********************************************************************
*
*       _ReportOutf
*/
static void _ReportOutf(const char* sFormat, ...) {
  char ac[2048];
  va_list ParamList;

  if (sFormat) {
    if (UTIL_strlen(sFormat) < 4096) {
      va_start(ParamList, sFormat);
      UTIL_vsnprintf(ac, UTIL_SIZEOF(ac), sFormat, &ParamList);
      fputs(ac, stdout);
#ifndef WIN32
      fflush(stdout);  // Linux / OS X buffers the output until the next new line
#endif
    }
  }
}

/*********************************************************************
* 
*       _ErrorOut
*/
static void _ErrorOut(const char* sText) {
  _ReportOutf("\nERROR: ");
  _ReportOutf("%s", sText);
}

/*********************************************************************
*
*       _cbLogOutHandler
*
*  Function description
*    Call-back function used to output log messages from the J-Link DLL
*/
static void _cbLogOutHandler(const char* sLog) {
  (void) sLog;
}

/*********************************************************************
*
*       _cbErrorOutHandler
*
*  Function description
*    Call-back function used to output error messages from the J-Link DLL
*/
static void _cbErrorOutHandler(const char* sError) {
  _ErrorOut(sError);
}

/*********************************************************************
*
*       _kbhit
*
*    Function description
*      Linux (POSIX) implementation of _kbhit().
*/
#ifndef WIN32
static int _kbhit(void) {
  static int Inited = 0;
  int BytesInBuffer;

  if (Inited == 0) {
    //
    // Use termios to turn off line buffering
    //
    struct termios Term;

    tcgetattr(STDIN_FILENO, &Term);
    Term.c_lflag &= ~ICANON;
    tcsetattr(STDIN_FILENO, TCSANOW, &Term);
    setbuf(stdin, NULL);
    Inited = 1;
  }
  ioctl(STDIN_FILENO, FIONREAD, &BytesInBuffer);
  return BytesInBuffer;
}
#endif

/*********************************************************************
*
*       _cbCtrlHandler
*
*  Function description
*    Windows version.
*    Executed on reception of a system signal, like Ctrl + C hit in a command line application.
*/
#ifdef WIN32
static BOOL WINAPI _cbCtrlHandler(DWORD Type) {
  switch (Type) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
    _CloseNow = 1;
    return TRUE;
  case CTRL_CLOSE_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    _CloseNow = 1;
    //
    // Application closes directly after returning. 
    // Give application some time.
    //
    SYS_Sleep(100);
    return TRUE;
  }
  return FALSE;
}
#endif

/*********************************************************************
*
*       _cbHandleCloseRequest
*
*  Function description
*    Linux / macOS version.
*    Executed on reception of a system close request signal, like SIGINT (Ctrl + C) or SIGTERM.
*    Note that registering a handler for a signal, *replaces* the default behavior of the application on reception of the signal.
*    E.g. if a handler for SIGTERM is registered, it will no longer close the application but calls the handler instead.
*/
static void _cbHandleCloseRequest(int Signal) {
  (void)Signal;
  _CloseNow = 1;  // Mark thread for termination.
}

/*********************************************************************
*
*       _RegisterSysCloseReqHandlers
*
*  Function description
*    Register handlers that are called on system close requests. Necessary to allow clean close of GDB Server and clean exit of children threads.
*/
static void _RegisterSysCloseReqHandlers(void) {
  //
  // J-Link RTT Logger may be closed via Ctrl + C
  // Even though the RTT Logger is not multithreaded currently, we want to do a clean shutdown
  // in case we receive such a "termination signal".
  // For the logger, we want to make sure, that the DLL and log file have a chance to properly close.
  //
#ifdef WIN32
  //
  // For Windows, we also want to catch the CTRL events, which can be generated via
  // GenerateConsoleCtrlEvent().
  // NOTE:
  //   Setting a CTRL handler overrides the related signal handlers and vice versa.
  //   CTRL + C which triggers SIGINT under Windows, will now trigger CTRL_C_EVENT
  //   and closing the console window which triggers SIGBREAK under Windows, will
  //   create a CTRL_CLOSE_EVENT.
  //   However, raise(SIGINT) will still lead to a SIGINT, so we also set the
  //   Signal handlers for Windows.
  //
  signal(SIGINT,   _cbHandleCloseRequest);
  signal(SIGTERM,  _cbHandleCloseRequest);
  signal(SIGBREAK, _cbHandleCloseRequest);
  signal(SIGABRT,  _cbHandleCloseRequest);
  SetConsoleCtrlHandler(_cbCtrlHandler, TRUE);
#else
  struct sigaction SigHandler;

  SigHandler.sa_handler = _cbHandleCloseRequest;  // Set signal handler function
  sigemptyset(&SigHandler.sa_mask);
  SigHandler.sa_flags = 0;                        // Makes sure interrupted system calls are not repeated (e.g. fgets() )
  sigaction(SIGINT,   &SigHandler, NULL);
  sigaction(SIGTERM , &SigHandler, NULL);
#endif
}

/*********************************************************************
*
*       _InitDebugSession
*
*  Function description
*    Initializes the debug session by connecting to a J-Link,
*    setting up the J-Link DLL and J-Link and finally connecting to the target system
*
*  Return value
*      0 O.K.
*    < 0 Error
*/
static int _InitDebugSession(INIT_PARAS* pParas) {
  const char* sError;
  U8 acIn[0x400];
  U8 acOut[0x400];
  int r;
  //
  // Select and configure host interface (USB is default)
  //
  if (pParas->HostIF == 1) {  // Host interface == IP?
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
  sError = JLINKARM_OpenEx(_cbLogOutHandler, _cbErrorOutHandler);
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
    UTIL_CopyString(acIn, "ProjectFile = ", UTIL_SIZEOF(acIn));
    UTIL_strcat(acIn, pParas->sSettingsFile, UTIL_SIZEOF(acIn));
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
    UTIL_CopyString(acIn, "device = ", UTIL_SIZEOF(acIn));
    UTIL_strcat(acIn, pParas->sDevice, UTIL_SIZEOF(acIn));
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
  // Select target interface speed which should be used by J-Link for target communication
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
  //
  // If this is the only connection to the J-Link, make sure that the CPU is running
  //
  if (JLINKARM_EMU_GetNumConnections() == 1) {
    if (JLINKARM_IsHalted() == 1) {
      JLINKARM_Go();
      SYS_Sleep(500);  // Give target some time for PLL init etc. since on some targets JLINKARM_Connect() causes the target to be reset and halted immediately after reset.
    }
  }
  return 0;
}

/*********************************************************************
*
*       _GetString
*
*  Function description
*    Wrapper function for SYS_ConsoleGetString(), which
*    Sets _CloseNow == 1, if the function fails.
*    This happens e.g. CTRL + C is presssed or when there is no stdout.
*/
static char* _GetString(const char* pPrompt, char* pBuffer, U32 BufferSize) {
  int r;

  r = SYS_ConsoleGetString(pPrompt, pBuffer, BufferSize);
  if (r < 0) {
    _CloseNow = 1;  // Mark thread for termination.
  }
  return pBuffer;
}

/*********************************************************************
*
*       _ParseDec
*/
static const char* _ParseDec(const char** ps, U32* pData) {
  U32 Data;
  int NumDigits;

  Data = 0;
  NumDigits = 0;
  
  UTIL_EatWhite(ps);
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
*       _ParseHex
*/
static const char* _ParseHex(const char** ps, U32* pData) {
  U32 Data;
  int NumDigits;

  Data = 0;
  NumDigits = 0;
  
  UTIL_EatWhite(ps);
  if ((**ps == '0') && ((*((*ps)+1) == 'x') || (*((*ps)+1) == 'X'))) {
    (*ps) += 2;
  }
  do {
    int v;
    char c;

    c = **ps;
    if ((c >= '0') && (c <= '9')) {
      v = c - '0';
    } else if ((c >= 'a') && (c <= 'f')) {
      v = c - 'a' + 10;
    } else if ((c >= 'A') && (c <= 'F')) {
      v = c - 'A' + 10;
    } else {
      v = -1;
    }
    if (v >= 0) {
      Data = (Data * 16) + v;
      (*ps)++;
      NumDigits++;
    } else {
      if (NumDigits == 0) {
        return "Expected a hex value";
      }
      *pData = Data;
      return NULL;
    }
  } while (1);
}

/*********************************************************************
*
*       _LoadMRUDevice
*/
static void _LoadMRUDevice(void) {
  int r;
  JLINK_FUNC_MRU_GETLIST* pFunc;

  pFunc = (JLINK_FUNC_MRU_GETLIST*)JLINK_GetpFunc(JLINK_IFUNC_MRU_GETLIST);
  if (pFunc) {
    r = pFunc(JLINK_MRU_GROUP_DEVICE, _Config.acDevice, sizeof(_Config.acDevice));
    if (r < 0) {
      _Config.acDevice[0] = 0;
    }
  }
}

/*********************************************************************
*
*       _Name2TIF
*/
static int _Name2TIF(const char* s) {
  if (UTIL_stricmp(s, "JTAG") == 0) {
    return JLINKARM_TIF_JTAG;
  } else if (UTIL_stricmp(s, "SWD") == 0) {
    return JLINKARM_TIF_SWD;
  } else if (UTIL_stricmp(s, "BDM3") == 0) {
    return JLINKARM_TIF_BDM3;
  } else if (UTIL_stricmp(s, "UART") == 0) {                // Formerly name of FINE interface
    return JLINKARM_TIF_FINE;
  } else if (UTIL_stricmp(s, "FINE") == 0) {
    return JLINKARM_TIF_FINE;
  } else if (UTIL_stricmp(s, "ICSP") == 0) {
    return JLINKARM_TIF_2_WIRE_JTAG_PIC32;
  } else if (UTIL_stricmp(s, "SPI") == 0) {
    return JLINKARM_TIF_SPI;
  } else if (UTIL_stricmp(s, "C2") == 0) {
    return JLINKARM_TIF_C2;
  } else {
    return -1;
  }
}

/*********************************************************************
* 
*       _InitSettings
*/
static int _InitSettings(int argc, char* argv[]) {
  char  ac[1024];
  char* s;
  const char* sErr;
  U32   v;
  int   i;
  int   Interactive;

  Interactive = 0;
  //
  // Default config
  //
  _Config.TIF        = -1;
  _Config.RTTChannel =  1;

  //
  // Parse command line
  //
  i = 1;
  while (i < argc) {
    s = argv[i++];

    if ((UTIL_stricmp(s, "-?") == 0) || (UTIL_stricmp(s, "?") == 0)) {
      //
      // Handle -?
      //
      _ReportOutf("Available options:\n");
      _ReportOutf("-Device <devicename>\n");
      _ReportOutf("-If <ifname>\n");
      _ReportOutf("-Speed <speed>\n");
//      _ReportOutf("-SelectEmuBySN <SN>\n"); // deprecated. Only kept for backwards compatibility.
      _ReportOutf("-USB <SN>\n");
      _ReportOutf("-IP <SN>\n");
      _ReportOutf("-RTTAddress <RTTAddress>\n");
      _ReportOutf("-RTTSearchRanges \"<Rangestart> <RangeSize>[, <Range1Start> <Range1Size>, ...]\n\"");
      _ReportOutf("-RTTChannel <RTTChannel>\n");
      _ReportOutf("<OutFilename>\n");
      return 1;

    } else if (UTIL_stricmp(s, "-Device") == 0) {
      //
      // Handle -Device
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-Device\".\n");
        return -1;
      }
      UTIL_CopyString(_Config.acDevice, argv[i++], sizeof(_Config.acDevice));
    } else if (UTIL_stricmp(s, "-if") == 0) {
      //
      // Handle -If
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-If\".\n");
        return -1;
      }
      _Config.TIF = _Name2TIF((const char*)argv[i++]);
      if (_Config.TIF == -1) {
        _ErrorOut("Could not parse target interface.\n");
        return -1;
      }
    } else if (UTIL_stricmp(s, "-Speed") == 0) {
      //
      // Handle -Speed
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-Speed\".\n");
        return -1;
      }
      sErr = _ParseDec((const char**)&argv[i++], &v);
      if (sErr) {
        _ErrorOut(sErr);
        _ErrorOut("Could not parse interface speed.\n");
        return -1;
      }
      _Config.TIFSpeed = v;
    } else if (UTIL_stricmp(s, "-SelectEmuBySN") == 0 || UTIL_stricmp(s, "-USB") == 0) {
      //
      // Handle -SelectEmuBySN
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-SelectEmuBySN\".\n");
        return -1;
      }
      sErr = _ParseDec((const char**)&argv[i++], &v);
      if (sErr) {
        _ErrorOut(sErr);
        _ErrorOut("Could not parse serial number.\n");
        return -1;
      } 
      _Config.SerialNo = v;
    } else if (UTIL_stricmp(s, "-IP") == 0) {
      //
      // Handle -IP
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-IP\".\n");
        return -1;
      }
      _Config.HostIF = 1;
      UTIL_snprintf(_Config.sHost, UTIL_SIZEOF(_Config.sHost), argv[i++]);
    } else if (UTIL_stricmp(s, "-RTTAddress") == 0) {
      //
      // Handle -RTTAddress
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-RTTAddress\".\n");
        return -1;
      }
      sErr = _ParseHex((const char**)&argv[i++], &v);
      if (sErr) {
        _ErrorOut(sErr);
        _ErrorOut("Could not parse RTT address.\n");
        return -1;
      }
      _Config.RTTCBAddr = v;
      _Config.RTTCBAddrValid = 1;
    } else if (UTIL_stricmp(s, "-RTTSearchRanges") == 0) {
      //
      // Handle -RTTSearchRange
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-RTTSearchRange\".\n");
        return -1;
      }
      UTIL_snprintf(_Config.RTTCBRange, UTIL_SIZEOF(_Config.RTTCBRange), "SetRTTSearchRanges %s", argv[i++]);
    } else if (UTIL_stricmp(s, "-RTTChannel") == 0) {
      //
      // Handle -RTTChannel
      //
      if (i >= argc) {
        _ErrorOut("Missing command line parameter after option \"-RTTChannel\".\n");
        return -1;
      }
      sErr = _ParseDec((const char**)&argv[i++], &v);
      if (sErr) {
        _Config.RTTChannel = -1;
        UTIL_CopyString(_Config.acChannelName, argv[i++], sizeof(_Config.acChannelName));
      } else {
        _Config.RTTChannel = v;
        _Config.acChannelName[0] = 0;
      }
    } else {
      //
      // If any command do not match: Use it as output file name
      //
      UTIL_CopyString(_Config.acFileName, argv[i - 1], sizeof(_Config.acFileName));
      break;
    }
  }
  //
  // Interactive mode
  //
  // Handle Device name
  //
  if (_Config.acDevice[0] == 0) {
    _LoadMRUDevice();
    if (_Config.acDevice[0]) {
      _ReportOutf("Device name. Default: %s ", _Config.acDevice);
    } else {
      _ReportOutf("Device name. ");
    }
    s = _GetString("> ", ac, sizeof(ac));
    if (_CloseNow) {
      return -1;
    }
    UTIL_EatWhite((const char**)&s);
    if (*s) {
      UTIL_snprintf(_Config.acDevice, UTIL_SIZEOF(_Config.acDevice), "%s", s);
    } else if (_Config.acDevice[0] == 0) {
      _ErrorOut("Could not get device name.\n");
      return -1;
    }
    Interactive = 1;
  }
  //
  // Handle TIF
  //
  if (_Config.TIF == -1) {
    _ReportOutf("Target interface. ");
    s = _GetString("> ", ac, sizeof(ac));
    if (_CloseNow) {
      return -1;
    }
    UTIL_EatWhite((const char**)&s);
    if (*s) {
      _Config.TIF = _Name2TIF(s);
      if (_Config.TIF == -1) {
        _ErrorOut("Could not get target interface.\n");
        return -1;
      }
    } else {
      _Config.TIF = JLINKARM_TIF_SWD;
    }
    Interactive = 1;
  }
  //
  // Handle TIF speed
  //
  if (!_Config.TIFSpeed) {
    _ReportOutf("Interface speed [kHz]. Default: 4000 kHz ");
    s = _GetString("> ", ac, sizeof(ac));
    if (_CloseNow) {
      return -1;
    }
    UTIL_EatWhite((const char**)&s);
    if (*s) {
      sErr = _ParseDec((const char**)&s, &v);
      if (sErr) {
        _ErrorOut(sErr);
        _ErrorOut("Could not get interface speed.\n");
        return -1;
      }
      _Config.TIFSpeed = v;
    } else {
      _Config.TIFSpeed = 4000;
    }
    Interactive = 1;
  }
  //
  // If we did not require any interactive input up to now, we are done here.
  //
  if (!Interactive) {
    return 0;
  }
  //
  // Handle RTT Addr
  //
  if (_Config.RTTCBAddrValid == 0 && _Config.RTTCBRange[0] == 0) {
    _ReportOutf("RTT Control Block address. Default: auto-detection ");
    s = _GetString("> ", ac, sizeof(ac));
    if (_CloseNow) {
      return -1;
    }
    UTIL_EatWhite((const char**)&s);
    if (*s) {
      sErr = _ParseHex((const char**)&s, &v);
      if (sErr) {
        _ErrorOut(sErr);
        _ErrorOut("Could not get RTT address.\n");
        return -1;
      }
      _Config.RTTCBAddr = v;
      _Config.RTTCBAddrValid = 1;
    }
  }
  //
  // Handle RTT Channel index
  //
  _ReportOutf("RTT Channel name or index. Default: channel 1 ");
  s = _GetString("> ", ac, sizeof(ac));
  if (_CloseNow) {
    return -1;
  }
  UTIL_EatWhite((const char**)&s);
  if (*s) {
    sErr = _ParseDec((const char**)&s, &v);
    if (sErr) {
      _Config.RTTChannel = -1;
      UTIL_snprintf(_Config.acChannelName, UTIL_SIZEOF(_Config.acChannelName), "%s", s);
    } else {
      _Config.RTTChannel = v;
      _Config.acChannelName[0] = 0;
    }
  }
  //
  // Handle Output file name
  //
    _ReportOutf("Output file. Default: RTT_<ChannelName>_<Time>.log ");
  s = _GetString("> ", ac, sizeof(ac));
  if (_CloseNow) {
    return -1;
  }
    UTIL_EatWhite((const char**)&s);
  if (*s) {
    UTIL_snprintf(_Config.acFileName, UTIL_SIZEOF(_Config.acFileName), "%s", s);
  } else {
    _Config.acFileName[0] = 0;
  }
  return 0;
}

/*********************************************************************
* 
*       _Connect
*/
static int _Connect(void) {
  INIT_PARAS Paras;
  JLINK_RTTERMINAL_START Info;
  int r;
  
  memset(&Paras, 0, sizeof(INIT_PARAS));
  Paras.SerialNo = _Config.SerialNo;
  Paras.HostIF   = _Config.HostIF;
  Paras.sHost    = _Config.sHost;
  Paras.TargetIF = _Config.TIF;
  Paras.Speed    = _Config.TIFSpeed;
  Paras.sDevice  = _Config.acDevice;
  
  if (_InitDebugSession(&Paras) != 0) {
    return -1;
  }

  if (_Config.RTTCBAddrValid == 1) {
    Info.ConfigBlockAddress = _Config.RTTCBAddr;
    JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_START, &Info);
  } else if (_Config.RTTCBRange[0] != 0) {
    r = JLINK_ExecCommand(_Config.RTTCBRange, NULL, 0);
    if (r) {
      _ErrorOut("Wrong syntax for \"-RTTSearchRange\".\n");
      return -1;
    }
    JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_START, NULL);
  } else {
    JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_START, NULL);
  }

  return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       main
*
*  Function description
*    main() function of command line version of SWOViewer.
*    This version does not remember any settings in a settings file.
*/
int main(int argc, char* argv[], char* envp[]) {
  int   r;
  int   i;
  char  c;
  int   NumBytesRead;
  U32   NumBytesTotal;
  U32   NumBytesPerSec;
  int   Time;
  int   TimeStart;
  int   Rate;
  SYS_FILE_HANDLE hFile;
  U32   SN;
  char  ac[SYS_MAX_PATH];
  char  acDefLogPath[256];
  JLINK_RTTERMINAL_BUFDESC Desc;
  int   Direction;
  int   NumChannels;
  
  (void)envp;
  hFile          = SYS_INVALID_HANDLE;
  NumBytesTotal  = 0;
  NumBytesPerSec = 0;
  //
  // Set output buffering to non-buffered
  // to allow directly getting output 
  // when redirecting stdout to somewhere else than terminal.
  // setvbuf has to be called before any use of the stream.
  //
  setvbuf(stdout, (char*)NULL, _IONBF, 0);
  _RegisterSysCloseReqHandlers();      // Give GDB Server a chance to react to system close requests via Ctrl + C and similar
  SYS_DIR_GetConfigPath(acDefLogPath, UTIL_SIZEOF(acDefLogPath));
  _ReportOutf("SEGGER J-Link RTT Logger");
  _ReportOutf("\nCompiled " __DATE__ " " __TIME__ "\n");
  _ReportOutf("(c) 2016-2017 SEGGER Microcontroller GmbH, www.segger.com\n");
  _ReportOutf("         Solutions for real time microcontroller applications\n\n");
  _ReportOutf("Default logfile path: %s\n\n", acDefLogPath);
  _ReportOutf("------------------------------------------------------------ \n\n");
  TimeStart = SYS_GetTickCount();
  //
  // Initialize target settings
  //
  r = _InitSettings(argc, argv);
  if (r != 0) {
    goto Done;
  }
  //
  // Connect and set up RTT
  //
  if (_Connect() != 0) {
    r = -1;
    goto Done;
  }
  JLINKARM_EMU_GetProductName(ac, sizeof(ac));
  _ReportOutf("\n------------------------------------------------------------ \n");
  _ReportOutf("Connected to:\n  ");
  _ReportOutf("%s", ac);
  SN = JLINKARM_GetSN();
  _ReportOutf("\n  S/N: %u\n", SN);
  //
  // Get the number of down buffers and the description of up buffer 1
  // Try until the the RTT CB is found
  //
  Direction = JLINKARM_RTTERMINAL_BUFFER_DIR_UP;
  _ReportOutf("\nSearching for RTT Control Block...");
  do {
    if(_CloseNow) {
      goto Done;
    }
    NumChannels = JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_GETNUMBUF, &Direction);
    if (_kbhit()) {
      c = SYS_GetChar();
      if (c) {
        NumChannels = -1;
        break;
      }
    }
    SYS_Sleep(2);
  } while (NumChannels == -2);
  if (NumChannels < 0) {
    _ReportOutf("RTT Control Block not found. Cannot get data.\n");
    r = -1;
    goto Done;
  }
  _ReportOutf("OK.\n%d up-channels found:\n", NumChannels);
  for (i = 0; i < NumChannels; i++) {
    memset(&Desc, 0, sizeof(Desc));
    Desc.BufferIndex = i;
    Desc.Direction = JLINKARM_RTTERMINAL_BUFFER_DIR_UP;
    r = JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_GETDESC, &Desc);
    if (r < 0) {
      _ReportOutf("Failed to get RTT Channel description.\n");
      r = -1;
      goto Done;
    }
    _ReportOutf("%i: %s\n", i, Desc.acName, Desc.BufferIndex);
  }
  if (_Config.RTTChannel >= 0) {
    if (_Config.RTTChannel >= NumChannels) {
      _ReportOutf("RTT Channel %d not found. Cannot get data.\n", _Config.RTTChannel);
      r = -1;
      goto Done;
    }
    Desc.BufferIndex = _Config.RTTChannel;
    Desc.Direction = JLINKARM_RTTERMINAL_BUFFER_DIR_UP;
    r = JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_GETDESC, &Desc);
    if (r >= 0) {
      UTIL_CopyString(_Config.acChannelName, Desc.acName, sizeof(_Config.acChannelName));
      _ReportOutf("Selected RTT Channel description: \n"
             "  Index: %d\n"
             "  Name:  %s\n"
             "  Size:  %d bytes.\n",
             Desc.BufferIndex, Desc.acName, Desc.SizeOfBuffer);
    } else {
      _ReportOutf("RTT Channel description unknown.\n");
    }
  } else if (_Config.acChannelName[0] != 0) {
    for (i = 0; i < NumChannels; i++) {
      memset(&Desc, 0, sizeof(Desc));
      Desc.BufferIndex = i;
      Desc.Direction = JLINKARM_RTTERMINAL_BUFFER_DIR_UP;
      r = JLINK_RTTERMINAL_Control(JLINKARM_RTTERMINAL_CMD_GETDESC, &Desc);
      if (r < 0) {
        _ReportOutf("Failed to get RTT Channel description.\n");
        r = -1;
        goto Done;
      }
      if (strcmp(_Config.acChannelName, Desc.acName) == 0) {
        _Config.RTTChannel = Desc.BufferIndex;
        UTIL_snprintf(_Config.acChannelName, UTIL_SIZEOF(_Config.acChannelName), "%s", Desc.acName);
        _ReportOutf("Selected RTT Channel description:\n"
               "  Index: %d\n"
               "  Name:  %s\n"
               "  Size:  %d bytes.\n",
               Desc.BufferIndex, Desc.acName, Desc.SizeOfBuffer);
        break;
      }
    }
    if (_Config.RTTChannel < 0) {
      _ReportOutf("Could not find RTT Channel %s. Cannot get data.\n", _Config.acChannelName);
    }
  } else {
    _ReportOutf("RTT Channel not selected. Cannot get data.\n");
    r = -1;
    goto Done;
  }
  TimeStart = SYS_GetTickCount();
  if (_Config.acFileName[0] == 0) {
    UTIL_snprintf(ac, UTIL_SIZEOF(ac), "RTTLogger_Channel_%s.log", _Config.acChannelName);
    SYS_MakeAbsFilePath(acDefLogPath, ac, _Config.acFileName, UTIL_SIZEOF(_Config.acFileName));
  }
  hFile = SYS_FILE_Open(_Config.acFileName, SYS_FILE_FLAG_WRITE | SYS_FILE_FLAG_SHARE_READ | SYS_FILE_FLAG_CREATE | SYS_FILE_FLAG_TRUNC);
  if (hFile == SYS_INVALID_HANDLE) {
    _ReportOutf("Could not open/create log file. (Insufficient permission?)\n");
    r = -1;
    goto Done;
  }
  _ReportOutf("\nOutput file: %s\n", _Config.acFileName);
  _ReportOutf("\nGetting RTT data from target. Press any key to quit.\n");
  _ReportOutf("------------------------------------------------------------ \n\n");
  _CloseNow = 0;
  do {
    //
    // Get RTT data and write them to the file
    //
    NumBytesRead = JLINK_RTTERMINAL_Read(_Config.RTTChannel, _abData, sizeof(_abData));
    if (NumBytesRead < 0) {
      _ErrorOut("Failed to read data.\n");
      r = 1;
      goto Done;
    }
    Time = SYS_GetTickCount() - TimeStart;
    //
    // We do an explicit SYS_FILE_Seek() to the end of file before each write,
    // because of the logrotate utility.
    // logrotate can be used to rotate log files, e.g. a large log file can cut into multiple smaller ones.
    // This can be done while a process is writing to a file, because logrotate does not change the file handle.
    // Example:
    //   JLinkRTTLogger runs and writes data to log file handle.
    //   After e.g. 1MB, logrotate runs on the log file and moves the current 1MB data into a new file.
    //
    // Without the explicit Seek(), the following would happen:
    //   JLinkRTTLogger continues to write to log file handle,
    //   still pointing at an offset of 1MB.
    //   This causes the next Write() to write 1MB of 0s into the file before writing the RTT data.
    //
    // With the explicit Seek(), the following happens:
    //   JLinkRTTLogger continues to write to log file handle,
    //   and after logrotate moved 1MB of data into another file,
    //   Seek() corrects the offset for the following Write() to 0.
    //   RTT data is written to the start of the new file, as expected.
    //
    // This has not changed the behavior for use-cases that do not use logrotate.
    //
    SYS_FILE_Seek(hFile, 0, SYS_FILE_SEEK_END);
    SYS_FILE_Write(hFile, _abData, NumBytesRead);

    NumBytesPerSec += NumBytesRead;
    if (Time >= 500) {
      NumBytesTotal += NumBytesPerSec;
      Rate = (NumBytesPerSec*1000/Time);
      _ReportOutf("\r");
      _ReportOutf("Transfer rate: %d KByte/s ", Rate/1024);
      if (NumBytesTotal < 1024) {
        _ReportOutf("Bytes written: %d Byte   ", NumBytesTotal);
      } else if (NumBytesTotal < 1024*1024) {
        _ReportOutf("Bytes written: %d KByte  ", NumBytesTotal/1024);
      } else {
        _ReportOutf("Bytes written: %d.%d MByte   ", NumBytesTotal/(1024*1024), ((NumBytesTotal/1024)%1000));
      }
      TimeStart += Time;
      NumBytesPerSec = 0;
    }
    //
    // Wait for closing event
    // Will be set by any key press or Ctrl-C 
    //
    if (_kbhit()) {
      c = SYS_GetChar();
      switch (c) {
#if _DEBUG
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        c -= '0';
        if (JLINK_RTTERMINAL_Write(_Config.RTTChannel, &c, 1) < 1) {
          _ReportOutf("\nFailed to write char.\n");
        }
        break;
      case 'Q':
        _CloseNow = 1;
        break;
#endif
      default:
        _CloseNow = 1;
        break;
      }
    }
    SYS_Sleep(2);   // Sleep some time
  } while (_CloseNow == 0);
  r = 0;
Done:
  //
  // Shut down because of external close request or fatal error
  //
  _ReportOutf("\nShutting down...");
  JLINKARM_Close();
  if (hFile != SYS_INVALID_HANDLE) {
    SYS_FILE_Close(hFile);
  }
  _ReportOutf(" Done.");
  SYS_Sleep(3000);  // Give the user some time to read the messages.
  return r;
}

/*************************** end of file ****************************/
