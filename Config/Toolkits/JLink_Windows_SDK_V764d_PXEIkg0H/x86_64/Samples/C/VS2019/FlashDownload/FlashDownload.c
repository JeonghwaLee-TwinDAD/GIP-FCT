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
File    : JLink_FlashDownload.c
Purpose : Sample application to show how to load 
            a binary file (.bin, .hex, .mot) to the target's flash.
--------  END-OF-HEADER  ---------------------------------------------
*/
#include <stdio.h>
#include <string.h>
#include "JLinkARMDLL.h"

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
*       _InitConfig()
* 
* Function description 
*   Set up the configuration used for the startup sequence.
*   For interactive setup refer to the startup sample.
*/
static int _InitConfig(int argc, char* argv[]) {

  _Paras.sDevice = "STM32F407IE";
  _Paras.TargetIF = JLINKARM_TIF_SWD;
  _Paras.Speed = 4000;
  _Paras.HostIF = JLINKARM_HOSTIF_USB;
  _Paras.SerialNo = 0;
  _Paras.sSettingsFile = "Settings.jlink";
  
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
  char* sApp;
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
  printf("Initializing...\n");
  r = _InitDebugSession(&_Paras);
  if (r < 0) {
    printf("Initialization failed.\n");
    return 1;
  }
  printf("Initialized successfully.\n");
  //
  // From now on the target is identified and we can start working with it.
  //
  sApp = "C:\\Temp\\ApplicationImage.bin";  // The filename of the file to load.
  printf("Resetting target...");
  JLINKARM_Reset();
  printf("O.K.\n");
  printf("Downloading target application to 0x00000000...");
  JLINKARM_BeginDownload(0);                                // Indicates start of flash download
  r = JLINK_DownloadFile(sApp, 0);                              // Load the application binary to address 0
  JLINKARM_EndDownload();                                   // Indicates end of flash download
  if (r < 0) {
    printf("Failed to load file.\n");
    JLINKARM_Close();
    return 1;
  } else {
    printf("O.K.\n");
  }
  //
  // Let target application run
  //
  printf("Starting target application...");
  JLINKARM_Reset();
  JLINKARM_Go();
  printf("O.K.\n");
  //
  // Everything is done. Close connection to J-Link.
  //
  JLINKARM_Close();
  return 0;
}