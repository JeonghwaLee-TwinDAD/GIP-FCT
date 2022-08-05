/*********************************************************************
*              SEGGER MICROCONTROLLER SYSTEME GmbH                   *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 2002-2005 SEGGER Microcontroller Systeme GmbH           *
*                                                                    *
* Internet: www.segger.com Support: support@segger.com               *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File    : main.c
Purpose : Jlink main program
---------------------------END-OF-HEADER------------------------------
*/

#include "JLinkARMDLL.h"
#include "Version.h"
#include "main.h"
#include "UTIL.h"
#include "SYS.h"
GLOBAL_DISABLE_WARNINGS()   // Avoid that 3rd party headers throw warnings
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#ifdef WIN32
  #include <conio.h>
  #include <windows.h>
#else
  #include <stdlib.h>
  #include <termios.h>
  #include <sys/ioctl.h>
  #include <fcntl.h>
  #include <limits.h>
  #include <time.h>
  #include <unistd.h>
  #include <ctype.h>
  #include <signal.h>
#endif
#ifdef linux
  #include <dlfcn.h>
#endif
#ifdef __APPLE__
  #include <CoreServices/CoreServices.h>
  #include <mach/mach.h>
  #include <mach/mach_time.h>
  #include <readline/readline.h>
  #include <readline/history.h>
#endif
GLOBAL_ENABLE_WARNINGS()

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#ifdef _DEBUG
  #define _PRINT_RETVAL(a) //printf a
  #define MEASURE_DLL_INIT_TIME  0 // 1
#else
  #define _PRINT_RETVAL(a)
  #define MEASURE_DLL_INIT_TIME  0
#endif

#define MAX_TIF_SPEED_KHZ_SUPPORTED (50000)

#define _MAC_BYTE_PROD_INVALID      0xFF           // Invalid product MAC-byte. Used to mark end of lists
#define DEFAULT_TIF_SPEED_KHZ       4000
#define DEFAULT_TIF_SPEED_KHZ_FINE  1000
#define DEFAULT_TIF_SPEED_KHZ_C2    1000

#define DEFAULT_FAST_SPEED          100
#define DEFAULT_SLOW_SPEED          4
#define DEFAULT_RAMADDR             0x0
#define MAX_NUM_WRITE_ITEMS         32
#define MAX_NUM_READ_DESCS          8
#define MAX_NUM_DEVICES             16
#define MAX_BLOCK_SIZE_FILE_IO      0x100000
#define UDP_DISCOVER_PORT           19020
#define SCRIPTFILE_DEFAULT_NAME     "Default.c"
#define SETTINGSFILE_DEFAULT_NAME   "Default.ini"
#define POWERTRACE_SWO_DEFAULT_SPEED 500000
#ifdef WIN32
  #define LOGFILE_NAME              "C:\\Work\\JLink.log"
  #define FILE_NAME_POWERTRACE_TEST "C:\\Work\\POWERTRACE_Test.txt"
#else
  #define LOGFILE_NAME              "JLink.log"
  #define FILE_NAME_POWERTRACE_TEST "POWERTRACE_Test.txt"
#endif

#define FLASH_DL_INTERVAL       5
#define FLASH_DL_NUM_INTERVALS  (100/FLASH_DL_INTERVAL)

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define _SIZEOF(a)    (int)sizeof(a)                    // x64 warning suppress: Safe to cast as we are sure to never exceed destination type boundaries here

#define JLINK_DEFAULT_SIZEOF_CONFIG       (0x100)

#define EMU_CAP_EX_TEST_NET                   (36)              // Supports command "EMU_CMD_TEST_NET"
#define EMU_CMD_TEST_NET                      239               // 0xEF
#define MAX_NUM_BYTES_TEST_NET                (4100)

#define JLINK_NOERROR                     0
#define JLINK_ERROR_UNKNOWN              -1
#define JLINK_ERROR_SYNTAX               -2
#define JLINK_ERROR_CANCELED_BY_USER     -3
#define JLINK_ERROR_WARNING              -4
#define JLINK_ERROR_INFO                 -5
#define JLINK_ERROR_ALL                  -5

#define HW_INFO_POWER_ENABLED             0
#define HW_INFO_POWER_OVERCURRENT         1
#define HW_INFO_ITARGET                   2
#define HW_INFO_ITARGET_PEAK              3
#define HW_INFO_ITARGET_PEAK_OPERATION    4
#define HW_INFO_ITARGET_MAX_TIME0        10
#define HW_INFO_ITARGET_MAX_TIME1        11
#define HW_INFO_ITARGET_MAX_TIME2        12
#define HW_INFO_ITARGET_NCAL_MA          13
#define HW_INFO_IP_ADDR                  16
#define HW_INFO_IP_MASK                  17
#define HW_INFO_IP_GW                    18
#define HW_INFO_IP_DNS0                  19
#define HW_INFO_IP_DNS1                  20

#define UDP_DISCOVER_OFF_IP_ADDR         (0x10)
#define UDP_DISCOVER_OFF_HW_ADDR         (0x20)
#define UDP_DISCOVER_OFF_SN              (0x30)
#define UDP_DISCOVER_OFF_HW_VERSION      (0x34)
#define UDP_DISCOVER_OFF_PRODUCT_NAME    (0x40)
#define UDP_DISCOVER_OFF_NICKNAME        (0x60)

#define EMU_IP_ERROR                     -1
#define EMU_IP_FIXED                      0
#define EMU_IP_DHCP_NO_CONFIG             1
#define EMU_IP_DHCP_CONFIGURED            2

#define MIN(a, b)     (((a) < (b)) ? (a) : (b))
#define MAX(a, b)     (((a) > (b)) ? (a) : (b))

#define FILE_FLAG_READ          (1 << 0)                    // Open the file for reading.
#define FILE_FLAG_SHARE_READ    (1 << 2)                    // Allow other processes to open the same file for reading.
#define FILE_FLAG_SHARE_WRITE   (1 << 3)                    // Allow other processes to open the same file for writing.
#define FILE_FLAG_CREATE        (1 << 4)                    // Create new file. If the file already exist, open it.
#define FILE_FLAG_TRUNC         (1 << 5)                    // Truncate the file to zero length.
#define FILE_FLAG_WRITE         (1 << 1)                    // Open the file for writing.
#define FILE_FLAG_APPEND        (1 << 6)                    // Set file pointer to end of file.
#define FILE_FLAG_EXCL          (1 << 7)                    // Can be specified in combination with FILE_FLAG_CREATE. Open fails if the specified file already exists.

#ifndef WIN32
  #define MAX_PATH              PATH_MAX
  #define _getch                getchar
  #define INVALID_HANDLE_VALUE  (-1)
  #define _MAX_PATH             PATH_MAX
  #define HANDLE                int
#endif

#define ITARGET_MAX_FLUCTUATION         100000  // Max. fluctuation is app. 100 mA due to 1% fluctuation of the resistors
#define ITARGET_SAMPLES                 20
#define ITARGET_OFFSET_FLAG             (1 << 31)
#define ITARGET_OFFSET_ADDR             0x80

#define DP_REG_IDCODE      0      // On read
#define DP_REG_ABORT       0      // On write
#define DP_REG_CTRL_STAT   1
#define DP_REG_WCR         1
#define DP_REG_RESEND      2
#define DP_REG_SELECT      2
#define DP_REG_RD_BUF      3      // Read buffer register

#define DP_ABORT_MASK_ORUNERRCLR        (1<<4)
#define DP_ABORT_MASK_WDERRCLR          (1<<3)
#define DP_ABORT_MASK_STKERRCLR         (1<<2)
#define DP_ABORT_MASK_STKCMPCLR         (1<<1)
#define DP_ABORT_MASK_DAPABORT          (1<<0)

#define DP_CTRLSTAT_MASK_CSYSPWRUPREQ   (1 << 30)            // System power up
#define DP_CTRLSTAT_MASK_CDBGPWRUPREQ   (1 << 28)            // Debug power up
#define DP_CTRLSTAT_MASK_STICKYERR      (1 << 5)             // Sticky error

#define ACK_OK                          1
#define ACK_WAIT                        2
#define ACK_FAULT                       4

#define POWERTRACE_REFC_NO_REFC                  0
#define POWERTRACE_REFC_SWO_NUMBYTES_TRANSMITTED 1

#define S32_CALLH(Off, NumParas)              (3 | ((NumParas) << 4) | (0 << 7) | ((Off) << 8))
#define S32_MOVI7(iRegDest, v)                (6 | ((iRegDest) << 12) | ((v) << 5))
#define S32_MOVI32(iRegDest, v)               (22| ((iRegDest) << 12)), (((U32)(v) << 16) >> 16), ((U32)(v) >> 16)
#define S32_MOV(iRegDest, iRegSrc, Off)       (5 | ((Off) << 4) | ((iRegDest) << 8) | ((iRegSrc) << 12))
#define S32_PUSH(iRegFirst, NumRegs)          (7 | ((iRegFirst) << 8) | ((NumRegs) << 12))
#define S32_POP(iRegFirst, NumRegs)           (23| ((iRegFirst) << 8) | ((NumRegs) << 12))
#define S32_BRK                               0
#define S32_BNZ(Disp)                         (0x02 | (U16)((U32)(Disp) << 8))
#define S32_BZ(Disp)                          (0x12 | (U16)((U32)(Disp) << 8))
#define S32_ADD(iRegDest, iRegSrc)            (0x01 | ((iRegDest) << 8) | ((iRegSrc) << 12))
#define S32_ADDI4(iRegDest, UImm4)            (0x11 | ((iRegDest) << 8) | ((UImm4) << 12))
#define S32_ADDI7_NO_FLAG(iRegDest, UImm7)    (0x09 | ((iRegDest) <<12) | ((UImm7) << 5))
#define S32_SUB(iRegDest, iRegSrc)            (0x21 | ((iRegDest) << 8) | ((iRegSrc) << 12))
#define S32_SUBI4(iRegDest, UImm4)            (0x31 | ((iRegDest) << 8) | ((UImm4) << 12))
#define S32_AND(iRegDest, iRegSrc)            (0x41 | ((iRegDest) << 8) | ((iRegSrc) << 12))
#define S32_ANDI4(iRegDest, UImm4)            (0x51 | ((iRegDest) << 8) | ((UImm4) << 12))
#define S32_LOADB(iRegVal, iRegAddr)          (8 | (0 << 4) | (1 << 7) | ((iRegVal) << 8) | ((iRegAddr) << 12))
#define S32_LOADH(iRegVal, iRegAddr)          (8 | (1 << 4) | (1 << 7) | ((iRegVal) << 8) | ((iRegAddr) << 12))
#define S32_LOADW(iRegVal, iRegAddr)          (8 | (2 << 4) | (1 << 7) | ((iRegVal) << 8) | ((iRegAddr) << 12))
#define S32_LOADWBITPOS(iRegVal, iRegBitPos)  (8 | (3 << 4) | (1 << 7) | ((iRegVal) << 8) | ((iRegBitPos) << 12))
#define S32_STOREB(iRegVal, iRegAddr)         (8 | (0 << 4) | (0 << 7) | ((iRegVal) << 8) | ((iRegAddr) << 12))
#define S32_STOREH(iRegVal, iRegAddr)         (8 | (1 << 4) | (0 << 7) | ((iRegVal) << 8) | ((iRegAddr) << 12))
#define S32_STOREW(iRegVal, iRegAddr)         (8 | (2 << 4) | (0 << 7) | ((iRegVal) << 8) | ((iRegAddr) << 12))
#define S32_DBNZ(iReg, Disp)                  (10 | ((-(Disp)) << 5) | ((iReg) << 12))

//
// For LPC5460 unlock
//
#define LPC5460_ISP_AP_ID_RESETVALUE        0x002A0000 
#define LPC5460_ISP_AP_CSW_RESYNCH_REQ      (1 << 0)  
#define LPC5460_ISP_AP_CSW_REQ_PENDING      (1 << 1) 
#define LPC5460_ISP_AP_CSW_CHIP_RESET_REQ   (1 << 5) 
#define LPC5460_ISP_AP_CMD_ENTER_ISP_AP     (1) 
#define LPC5460_ISP_AP_CMD_BULK_ERASE       (3) 
#define LPC5460_ISP_AP_CMD_EXIT_ISP_AP      (4) 


#define CONFIG_OFF_HWFC           (0x88)
#define VIRTUAL_COM_PORT_OFFSET   (0x89)
#define CONFIG_OFF_HW_FEATURES    (0x8E)
#define CONFIG_OFF_LIC_AREA       (0xB0)
#define CONFIG_OFF_FIXED_VTREF    (0x9A) // 0x00/0xFF == Fixed VTref feature is off. Otherwise: VTref in 100 mV units (1 == 100 mV, 2 == 200 mV, ...)
#define CONFIG_OFF_WIFI_SSID      (0x588)
#define CONFIG_OFF_WIFI_PW        (0x5A8)

#define NUM_PAD_CHARS_NAME   15
#define NUM_PAD_CHARS_SYNTAX 50

//
// Command info defines
//
#define INFO_GETINFO          "Show information about all or specific commands."
#define INFO_CONNECT          "Connect to target device."
#define INFO_SHOWFWINFO       "Show firmware info."
#define INFO_HALT             "Halt CPU."
#define INFO_ISHALTED         "Return current CPU state."
#define INFO_WAITHALT         "Wait until CPU is halted or timeout is reached."
#define INFO_GO               "Start CPU if halted."
#define INFO_SLEEP            "Waits the given time (in milliseconds)."
#define INFO_STEP             "Execute step(s) on the CPU."
#define INFO_SHOWHWSTATUS     "Show hardware status."
#define INFO_MEM              "Read memory and show corresponding ASCII values."
#define INFO_MEM8             "Read  8-bit items."
#define INFO_MEM16            "Read 16-bit items."
#define INFO_MEM32            "Read 32-bit items."
#define INFO_WRITE1           "Write  8-bit items."
#define INFO_WRITE2           "Write 16-bit items."
#define INFO_WRITE4           "Write 32-bit items."
#define INFO_ERASE            "Erase flash (range) of selected device."
#define INFO_IS               "Identify length of scan chain select register."
#define INFO_MS               "Measure length of scan chain."
#define INFO_MR               "Measure RTCK react time."
#define INFO_EXIT             "Close J-Link connection and quit."
#define INFO_EXITONERROR      "Exit on error."
#define INFO_RESET            "Reset CPU."
#define INFO_RESETX           "Reset CPU with delay after reset."
#define INFO_RSETTYPE         "Set the current reset type."
#define INFO_REGS             "Display CPU register contents."
#define INFO_RREG             "Read register."
#define INFO_WREG             "Write register."
#define INFO_MOE              "Shows mode-of-entry (CPU halt reason)."
#define INFO_SETBP            "Set breakpoint."
#define INFO_CLEARBP          "Clear breakpoint."
#define INFO_SETWP            "Set Watchpoint."
#define INFO_CLEARWP          "Clear watchpoint."
#define INFO_VCATCH           "Write vector catch."
#define INFO_LOADFILE         "Load data file into target memory. Supported ext.: *.bin, *.mot, *.hex, *.srec"
#define INFO_SAVEBIN          "Save target memory range into binary file."
#define INFO_VERIFYBIN        "Verfy if specified .bin file is at the specified target memory location."
#define INFO_SETPC            "Set the PC to specified value."
#define INFO_LE               "Change mode to little endian."
#define INFO_BE               "Change mode to big endian."
#define INFO_LOG              "Enables log to file."
#define INFO_TERM             "Visualize printf output using DCC (SEGGER DCC handler running on target)."
#define INFO_READAP           "Read CoreSight AP register. Note: First read returns data of the previous read. An additional read of DP reg 3 is necessary to get the data."
#define INFO_WRITEAP          "Write CoreSight AP register."
#define INFO_READDP           "Read CoreSight DP register. Note: SWD: Data is returned immediately. JTAG: Data of the previous read is returned. An additional read of DP reg 3 is necessary to get the data."
#define INFO_WRITEDP          "Write CoreSight DP register."
#define INFO_DEVICE           "Select specific device J-Link shall connect to."
#define INFO_EXPDEVLIST       "Export device names from DLL internal device list to text file."
#define INFO_EXPDEVLISTXML    "Export device names from DLL internal device list to XML file."
#define INFO_POWERTRACE       "Perform power trace (not supported by all models)"
#define INFO_RCP15EX          "Read CP15 register."
#define INFO_WCP15EX          "Write CP15 register."
#define INFO_ICE              "Show state of the embedded ICE macrocell (ICE breaker)."
#define INFO_RI               "Read Ice reg."
#define INFO_WI               "Write Ice reg."
#define INFO_TCLEAR           "TRACE - Clear buffer."
#define INFO_TSETSIZE         "TRACE - Set Size of trace buffer."
#define INFO_TSETFORMAT       "TRACE - SetFormat."
#define INFO_TSR              "TRACE - Show Regions (and analyze trace buffer)." 
#define INFO_TSTART           "TRACE - Start."
#define INFO_TSTOP            "TRACE - Stop."
#define INFO_SWOSPEED         "SWO - Show supported speeds."
#define INFO_SWOSTART         "SWO - Start."
#define INFO_SWOSTOP          "SWO - Stop."
#define INFO_SWOSTAT          "SWO - Display SWO status."
#define INFO_SWOREAD          "SWO - Read and display SWO data."
#define INFO_SWOSHOW          "SWO - Read and analyze SWO data."
#define INFO_SWOFLUSH         "SWO - Flush data."
#define INFO_SWOVIEW          "SWO - View terminal data."
#define INFO_FWRITE           "(Flasher only) Write file to emulator."
#define INFO_FREAD            "(Flasher only) Read file from emulator."
#define INFO_FSHOW            "(Flasher only) Read and display file from emulator."
#define INFO_FDELETE          "(Flasher only) Delete file on emulator."
#define INFO_FSIZE            "(Flasher only) Display size of file on emulator."
#define INFO_FLIST            "(Flasher only) List directory on emulator."
#define INFO_SECUREAREA       "(Flasher only) Creates/Removes secure area on probe."
#define INFO_TESTWSPEED       "Measure download speed into target memory."
#define INFO_TESTRSPEED       "Measure upload speed from target memory."
#define INFO_TESTCSPEED       "Measure CPU speed."
#define INFO_TESTNWSPEED      "Measure network download speed."
#define INFO_TESTNRSPEED      "Measure network upload speed."
#define INFO_JTAGCONF         "Set number of IR/DR bits before Target device."
#define INFO_SPEED            "Set target interface speed."
#define INFO_JTAGID           "Read JTAG Id."
#define INFO_WJTAGIR          "Write JTAG command (IR)."
#define INFO_WJTAGDR          "Write JTAG data (DR)."
#define INFO_WJTAGRAW         "Write Raw JTAG data."
#define INFO_RESETTAP         "Reset TAP Controller using state machine (111110)."
#define INFO_RT               "Reset TAP Controller using nTRST."
#define INFO_C00              "TCK - Clock with TDI = TMS = 0."
#define INFO_C                "TCK - Clock."
#define INFO_CLRTCK           "TCK - Clear."
#define INFO_SETTCK           "TCK - Set."
#define INFO_CLRTDI           "TDI - Clear."
#define INFO_SETTDI           "TDI - Set."
#define INFO_CLRTMS           "TMS - Clear."
#define INFO_SETTMS           "TMS - Set."
#define INFO_CLRTRST          "TRST - Clear."
#define INFO_SETTRST          "TRST - Set."
#define INFO_CLRRESET         "RESET - Clear."
#define INFO_SETRESET         "RESET - Set."
#define INFO_USB              "Connect to J-Link via USB."
#define INFO_IP               "Connect to J-Link  via TCP/IP or to Remote Server."
#define INFO_SELECTINTERFACE  "Select target interface."
#define INFO_POWER            "Switch power supply for target."
#define INFO_IPADDR           "Show/Assign IP address and subnetmask of/to connected Probe."
#define INFO_GWADDR           "Show/Assign network gateway address of/to connected Probe."
#define INFO_DNSADDR          "Show/Assign network DNS server address of/to connected Probe."
#define INFO_SHOWCONF         "Show configuration of the connected Probe."
#define INFO_CALIBRATE        "Calibrate the target current measurement."
#define INFO_SELECTPROBE      "Show list of all connected probes via specified interface. The Probe to communicate with can then be selected."
#define INFO_SHOWEMULIST      "Show list of all connected probes via specified interface."
#define INFO_VCOM             "Enable/disable VCOM. Takes effect after power cycle of the Probe."
#define INFO_VTREF            "Set fixed value for VTref on J-Link."
//
// Command syntax defines
//
#define SYNTAX_GETINFO         "? [<Command>]"
#define SYNTAX_CONNECT         NULL
#define SYNTAX_SHOWFWINFO      NULL
#define SYNTAX_HALT            NULL
#define SYNTAX_ISHALTED        NULL
#define SYNTAX_WAITHALT        "WaitHalt [<TimeoutMs>] (default: 1000 ms)"
#define SYNTAX_GO              NULL
#define SYNTAX_SLEEP           "Sleep <Delay>"
#define SYNTAX_STEP            "Step [<NumSteps> (decimal)] (default is 1)"
#define SYNTAX_SHOWHWSTATUS    NULL
#define SYNTAX_MEM             "Mem  [<Zone>:]<Addr>, <NumBytes> (hex)"
#define SYNTAX_MEM8            "Mem8  [<Zone>:]<Addr>, <NumBytes> (hex)"
#define SYNTAX_MEM16           "Mem16 [<Zone>:]<Addr>, <NumItems> (hex)"
#define SYNTAX_MEM32           "Mem32 [<Zone>:]<Addr>, <NumItems> (hex)"
#define SYNTAX_WRITE1          "W1 [<Zone>:]<Addr>, <Data> (hex)"
#define SYNTAX_WRITE2          "W2 [<Zone>:]<Addr>, <Data> (hex)"
#define SYNTAX_WRITE4          "W4 [<Zone>:]<Addr>, <Data> (hex)"
#define SYNTAX_ERASE           "Erase [<SAddr>, <EAddr>]"
#define SYNTAX_IS              NULL
#define SYNTAX_MS              "MS <Scan chain>"
#define SYNTAX_MR              NULL
#define SYNTAX_EXIT            NULL
#define SYNTAX_EXITONERROR     "EoE <1/0>"
#define SYNTAX_RESET           NULL
#define SYNTAX_RESETX          "ResetX <DelayAfterReset>"
#define SYNTAX_RSETTYPE        "RSetType <Type>"
#define SYNTAX_REGS            NULL
#define SYNTAX_RREG            "RReg <RegName>"
#define SYNTAX_WREG            "WReg <RegName>, <Value>"
#define SYNTAX_MOE             NULL
#define SYNTAX_SETBP           "SetBP <addr> [A/T] [S/H]"
#define SYNTAX_CLEARBP         "ClearBP <BP_Handle>"
#define SYNTAX_SETWP           "SetWP <Addr> [R/W] [<Data> [<D-Mask>] [A-Mask]]"
#define SYNTAX_CLEARWP         "ClearWP <WP_Handle>"
#define SYNTAX_VCATCH          "VCatch <Value>"
#define SYNTAX_LOADFILE        "LoadFile <FileName>, [<Addr> (.bin only)]."
#define SYNTAX_SAVEBIN         "SaveBin <filename>, <addr>, <NumBytes>"
#define SYNTAX_VERIFYBIN       "VerifyBin <filename>, <addr>"
#define SYNTAX_SETPC           "SetPC <Addr>"
#define SYNTAX_LE              NULL
#define SYNTAX_BE              NULL
#define SYNTAX_LOG             "Log <filename>"
#define SYNTAX_TERM            NULL
#define SYNTAX_READAP          "ReadAP <RegIndex>"
#define SYNTAX_WRITEAP         "WriteAP <RegIndex>"
#define SYNTAX_READDP          "ReadDP <RegIndex>"
#define SYNTAX_WRITEDP         "WriteDP <RegIndex>"
#define SYNTAX_DEVICE          "Device <DeviceName>"
#define SYNTAX_EXPDEVLIST      "ExpDevList <Filename>"
#define SYNTAX_EXPDEVLISTXML   "ExpDevListXML <Filename>"
#define SYNTAX_POWERTRACE      "PowerTrace <LogFile> [<ChannelMask> <RefCountSel>]"
#define SYNTAX_RCP15EX         "RCE <Op1>, <CRn>, <CRm>, <Op2>"
#define SYNTAX_WCP15EX         "WCE <Op1>, <CRn>, <CRm>, <Op2>, <Data>"
#define SYNTAX_ICE              NULL
#define SYNTAX_RI               "RI <RegIndex>(hex)"
#define SYNTAX_WI               "WI <RegIndex>, <Data>(hex)"
#define SYNTAX_TCLEAR           NULL
#define SYNTAX_TSETSIZE         "TSS <Size(hex)>"
#define SYNTAX_TSETFORMAT       "TSF <4|8|16>"
#define SYNTAX_TSR              NULL
#define SYNTAX_TSTART           NULL
#define SYNTAX_TSTOP            NULL
#define SYNTAX_SWOSPEED         NULL
#define SYNTAX_SWOSTART         "SWOStart [<Speed>]. Default: <Speed> == Autodetect"
#define SYNTAX_SWOSTOP          NULL
#define SYNTAX_SWOSTAT          NULL
#define SYNTAX_SWOREAD          NULL
#define SYNTAX_SWOSHOW          NULL
#define SYNTAX_SWOFLUSH         NULL
#define SYNTAX_SWOVIEW          NULL
#define SYNTAX_FWRITE           "FWrite <EmuFile> <HostFile> [<Offset> [<NumBytes>]]"
#define SYNTAX_FREAD            "FRead <EmuFile> <HostFile> [<Offset> [<NumBytes>]]"
#define SYNTAX_FSHOW            "FShow <FileName> [<Offset> [<NumBytes>]]"
#define SYNTAX_FDELETE          "FDelete <FileName>"
#define SYNTAX_FSIZE            "FSize <FileName>"
#define SYNTAX_FLIST            NULL
#define SYNTAX_SECUREAREA       "SecureArea <Operation>"
#define SYNTAX_TESTWSPEED       "TestWSpeed [<Addr> [<Size>]]"
#define SYNTAX_TESTRSPEED       "TestRSpeed [<Addr> [<Size>] [<NumBlocks>]]"
#define SYNTAX_TESTCSPEED       "TestCSpeed [<RAMAddr>]"
#define SYNTAX_TESTNWSPEED      "TestNWSpeed [<NumBytes> [<NumReps>]]"
#define SYNTAX_TESTNRSPEED      "TestNRSpeed [<NumBytes> [<NumReps>]]"
#define SYNTAX_JTAGCONF         "JTAGConf <IRpre>, <DRpre>"
#define SYNTAX_SPEED            "Speed <freq|auto|adaptive>"
#define SYNTAX_JTAGID           NULL
#define SYNTAX_WJTAGIR          "WJTAGIR <Insturction(hex)>, [<IRLen(dec)>] (default IRLen=4)"
#define SYNTAX_WJTAGDR          "WJTAGDR <Data64(hex)>, <NumBits(dec)>"
#define SYNTAX_WJTAGRAW         NULL
#define SYNTAX_RESETTAP         NULL
#define SYNTAX_RT               NULL
#define SYNTAX_C00              NULL
#define SYNTAX_C                NULL
#define SYNTAX_CLRTCK           NULL
#define SYNTAX_SETTCK           NULL
#define SYNTAX_CLRTDI           NULL
#define SYNTAX_SETTDI           NULL
#define SYNTAX_CLRTMS           NULL
#define SYNTAX_SETTMS           NULL
#define SYNTAX_CLRTRST          NULL
#define SYNTAX_SETTRST          NULL
#define SYNTAX_CLRRESET         NULL
#define SYNTAX_SETRESET         NULL
#define SYNTAX_USB              "USB [<SN>]"
#define SYNTAX_IP               "IP <IPAddr|RemoteServerString>"
#define SYNTAX_SELECTINTERFACE  "SelectInterface <Interface>"
#define SYNTAX_POWER            "Power <On|Off> [perm]"
#define SYNTAX_IPADDR           NULL
#define SYNTAX_GWADDR           NULL
#define SYNTAX_DNSADDR          NULL
#define SYNTAX_SHOWCONF         NULL
#define SYNTAX_CALIBRATE        NULL
#define SYNTAX_SELECTPROBE      "SelectProbe [<Interface0> <Interface1> ...]"
#define SYNTAX_SHOWEMULIST      "ShowEmuList [<Interface0> <Interface1> ...]"
#define SYNTAX_VCOM             "VCOM <enable|disable>"
#define SYNTAX_VTREF            "VTREF <Value[mV]>. 0 == Auto detection"

/*********************************************************************
*
*       Pragmas
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
  int          (*pfCmd)    (const char* s);
  const char*  sLongName;
  const char*  sShortName;
  const char*  sInfo;
  const char*  sSyntax;
  char         NeedsJLinkConnection;
  char         NeedsTargetConnection;
} _COMMAND_INFO;

typedef struct {
  U16          ManufacturerID;
  const char * sName;
} DEVICE_MANUFACTURER;

typedef struct {
  U32 StackPtr;
  U16 aCode[100];
  U8 aDataIn[0x100];
  U8 aDataDir[0x100];
  U8 aDataOut[0x100];
  U32 aStack[20];
} PCODE_PROGRAM;

typedef struct {
  U32  InitTIFSpeedkHz;
  U32  InitTIFSpeedkHzSet;
  U32  TargetIF;
  U32  HostIF;
  U32  EmuSN;                        // EmuSN 0-3 are mapped to USB 0-3 connection method. This method is no longer recommended since it limits the number of connected J-Links to 4.
  U32  CoreIndex;
  int  JTAGConfIRPre;
  int  JTAGConfDRPre;
  U8   ScriptModeActive;             // Commander script passed to J-Link Commander. Will be automatically executed.
  U8   TargetIFSet;
  U8   CloseRequested;
  U8   DoAutoConnect;
  U8   JTAGConfSet;
  U8   IsSetEmuSN;
  char acIPAddr[128];
  char acDeviceName[128];
  char acJLinkScriptFile[512];
  char acCommanderScriptFile[512];
  char acSettingsFile[512];
  struct {
    unsigned ConnectedToJLink;
    unsigned ConnectedToTarget;
  } Status;
} _SETTINGS;

typedef struct {
  const char* sName;
  int (*pfCmd)(_SETTINGS* pCommanderSettings, const char* sVal);
} CMDLINE_OPTION;

typedef struct {
  const char* sName;
  char Shortcut;
  U32  TIFVal;
  const U32* paSupportedCores;
} _TIF_LIST;

typedef struct {
  U32 DevFamily;
  U32 iRegPC;
} _DEV_INFO;

typedef struct {
  const char* sAction;
  const char* sDescription;
} FLASHDL_ACTION;

typedef struct {
  const char* sCmdStr;
  unsigned NeedsJLinkConn;   // 1 == command string needs an active J-Link connection
} CMD_STR_ENTRY;

typedef struct {
  const U8* pMACByteProdTbl;
} _MAC_TBL;

typedef struct {
  U64 R0;
  U64 R1;
  U64 R2;
  U64 R3;
  U64 R4;
  U64 R5;
  U64 R6;
  U64 R7;
  U64 R8;
  U64 R9;
  U64 R10;
  U64 R11;
  U64 R12;
  U64 SP;
  U64 LR;
  U64 PC;
  U64 SPSR_ABT;
  U64 SPSR_SVC;
  U64 SPSR_HYP;
  U64 SPSR_FIQ;
  U64 SPSR_IRQ;
  U64 SPSR_UND;
} _CPU_REGS_ARM_V8AR_AARCH32;

typedef struct {
  const U32    RegIndex;  // See JLINK_RISCV_REG_ defines
  const char* sName;     // Name to display for this register
  U32         RegGroup;  // Group to display the register in
} _REG_CONTEXT;

typedef struct {
  const U32* paRegIndex;             // Points at static array
  const _REG_CONTEXT* paRegContext;  // Points at static array
  U64* paRegContent;
  U64* paRegDiff;
  U32 NumRegs;
} _REGS_STAT;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static U32                _RAMAddr = DEFAULT_RAMADDR;
static U32                _ResetType;
static U64                _Var;
static const char*        _pLoop;
static char               _ExitTerminal;
static _SETTINGS          _CommanderSettings;
static U32                _Polynom;
static int                _ExitOnErrorLvl = 0;
static int                _FlashDLCurPercentage = FLASH_DL_NUM_INTERVALS;
static U32                _FlashDLCurStep = 0;
static int                _TimeoutStepOver = 5000;
static U32                _ASCIIViewDisable;
static U8                 _SuppressGUI;

/*********************************************************************
*
*       Static const data
*
**********************************************************************
*/


static U32 _aRegARMv8ARAArch32[] = {
  JLINK_ARM_V8AR_REG_AARCH32_R0,
  JLINK_ARM_V8AR_REG_AARCH32_R1,
  JLINK_ARM_V8AR_REG_AARCH32_R2,
  JLINK_ARM_V8AR_REG_AARCH32_R3,
  JLINK_ARM_V8AR_REG_AARCH32_R4,
  JLINK_ARM_V8AR_REG_AARCH32_R5,
  JLINK_ARM_V8AR_REG_AARCH32_R6,
  JLINK_ARM_V8AR_REG_AARCH32_R7,
  JLINK_ARM_V8AR_REG_AARCH32_R8,         // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_R9,         // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_R10,        // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_R11,        // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_R12,        // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_R13,        // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_R14,        // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
  JLINK_ARM_V8AR_REG_AARCH32_PC,         // Current registers (automatically mapped to USR, IRQ, FIQ, ... by DLL)
};

static const U32 _aRVRegIndex[] = {
   JLINK_RISCV_REG_PC
  ,JLINK_RISCV_REG_X2
  ,JLINK_RISCV_REG_X1
  ,JLINK_RISCV_REG_X3
  ,JLINK_RISCV_REG_X4
  ,JLINK_RISCV_REG_X8
  ,JLINK_RISCV_REG_X5
  ,JLINK_RISCV_REG_X6
  ,JLINK_RISCV_REG_X7
  ,JLINK_RISCV_REG_X28
  ,JLINK_RISCV_REG_X29
  ,JLINK_RISCV_REG_X30
  ,JLINK_RISCV_REG_X31
  ,JLINK_RISCV_REG_X10
  ,JLINK_RISCV_REG_X11
  ,JLINK_RISCV_REG_X12
  ,JLINK_RISCV_REG_X13
  ,JLINK_RISCV_REG_X14
  ,JLINK_RISCV_REG_X15
  ,JLINK_RISCV_REG_X16
  ,JLINK_RISCV_REG_X17
  ,JLINK_RISCV_REG_X9
  ,JLINK_RISCV_REG_X18
  ,JLINK_RISCV_REG_X19
  ,JLINK_RISCV_REG_X20
  ,JLINK_RISCV_REG_X21
  ,JLINK_RISCV_REG_X22
  ,JLINK_RISCV_REG_X23
  ,JLINK_RISCV_REG_X24
  ,JLINK_RISCV_REG_X25
  ,JLINK_RISCV_REG_X26
  ,JLINK_RISCV_REG_X27
};

static const _REG_CONTEXT _aRVRegContexts[] = {
//    RegIndex               sName     RegGroup  
   {  JLINK_RISCV_REG_PC,    "pc",     0  }
  ,{  JLINK_RISCV_REG_X2,    "sp",     0  }
  ,{  JLINK_RISCV_REG_X1,    "ra",     0  }
  ,{  JLINK_RISCV_REG_X3,    "gp",     1  }
  ,{  JLINK_RISCV_REG_X4,    "tp",     1  }
  ,{  JLINK_RISCV_REG_X8,    "fp",     1  }
  ,{  JLINK_RISCV_REG_X5,    "t0",     2  }
  ,{  JLINK_RISCV_REG_X6,    "t1",     2  }
  ,{  JLINK_RISCV_REG_X7,    "t2",     2  }
  ,{  JLINK_RISCV_REG_X28,   "t3",     3  }
  ,{  JLINK_RISCV_REG_X29,   "t4",     3  }
  ,{  JLINK_RISCV_REG_X30,   "t5",     3  }
  ,{  JLINK_RISCV_REG_X31,   "t6",     3  }
  ,{  JLINK_RISCV_REG_X10,   "a0",     4  }
  ,{  JLINK_RISCV_REG_X11,   "a1",     4  }
  ,{  JLINK_RISCV_REG_X12,   "a2",     4  }
  ,{  JLINK_RISCV_REG_X13,   "a3",     4  }
  ,{  JLINK_RISCV_REG_X14,   "a4",     5  }
  ,{  JLINK_RISCV_REG_X15,   "a5",     5  }
  ,{  JLINK_RISCV_REG_X16,   "a6",     5  }
  ,{  JLINK_RISCV_REG_X17,   "a7",     5  }
  ,{  JLINK_RISCV_REG_X9,    "s1",     6  }
  ,{  JLINK_RISCV_REG_X18,   "s2",     6  }
  ,{  JLINK_RISCV_REG_X19,   "s3",     6  }
  ,{  JLINK_RISCV_REG_X20,   "s4",     6  }
  ,{  JLINK_RISCV_REG_X21,   "s5",     7  }
  ,{  JLINK_RISCV_REG_X22,   "s6",     7  }
  ,{  JLINK_RISCV_REG_X23,   "s7",     7  }
  ,{  JLINK_RISCV_REG_X24,   "s8",     7  }
  ,{  JLINK_RISCV_REG_X25,   "s9",     8  }
  ,{  JLINK_RISCV_REG_X26,   "s10",    8  }
  ,{  JLINK_RISCV_REG_X27,   "s11",    8  }
};

static const char* _asModeARMv8A_AArch64[16] = { "EL0t (SP_EL0)", "---", "---", "---", "EL1t (SP_EL0)", "EL1h (SP_EL1)", "---", "---", "EL2t (SP_EL0)", "EL2h (SP_EL2)", "---", "---", "EL3t (SP_EL0)", "EL3h (SP_EL3)", "---", "---" };
static const char* _asModeARMv8A_AArch32[16] = { "USR", "FIQ", "IRQ", "SVC", "---", "---", "MON", "ABT", "---", "---", "HYP", "UND", "---", "---", "---", "SYS" };

static const DEVICE_MANUFACTURER _aManufacturer[] = {
   { 0x001F, "Atmel"   }
  ,{ 0x0223, "Renesas" }
  ,{ 0x023B, "ARM"     }
};

static const char* _aIPSR2Str[32] = {
  "NoException",  "Reset",     "NMI",        "HardFault",
  "MemManage",    "BusFault",  "UsageFault", "Reserved",
  "Reserved",     "Reserved",  "Reserved",   "SVCall",
  "DebugMonitor", "Reserved",  "PendSV",     "SysTick",
  "INTISR0",      "INTISR1",   "INTISR2",    "INTISR3",
  "INTISR4",      "INTISR5",   "INTISR6",    "INTISR7",
  "INTISR8",      "INTISR9",   "INTISR10",   "INTISR11",
  "INTISR12",     "INTISR13",  "INTISR14",   "INTISR15"
};

static const _DEV_INFO _aTblDevInfo[] = {
                                          { JLINKARM_DEV_FAMILY_CM1,          JLINKARM_CM3_REG_R15       },
                                          { JLINKARM_DEV_FAMILY_CF,           JLINK_CF_REG_PC            },
                                          { JLINKARM_DEV_FAMILY_CM3,          JLINKARM_CM3_REG_R15       },
                                          { JLINKARM_DEV_FAMILY_CM0,          JLINKARM_CM3_REG_R15       },
                                          { JLINKARM_DEV_FAMILY_ARM7,         ARM_REG_R15                },
                                          { JLINKARM_DEV_FAMILY_CORTEX_A8,    JLINKARM_CORTEX_R4_REG_R15 },
                                          { JLINKARM_DEV_FAMILY_ARM9,         ARM_REG_R15                },
                                          { JLINKARM_DEV_FAMILY_ARM10,        ARM_REG_R15                },
                                          { JLINKARM_DEV_FAMILY_ARM11,        ARM_REG_R15                },
                                          { JLINKARM_DEV_FAMILY_CORTEX_R4,    JLINKARM_CORTEX_R4_REG_R15 },
                                          { JLINKARM_DEV_FAMILY_RX,           JLINKARM_RX_REG_PC         },
                                          { JLINKARM_DEV_FAMILY_CM4,          JLINKARM_CM3_REG_R15       },
                                          { JLINKARM_DEV_FAMILY_CORTEX_A5,    JLINKARM_CORTEX_R4_REG_R15 },
                                          { JLINKARM_DEV_FAMILY_POWER_PC,     JLINK_POWER_PC_REG_PC      },
                                          { JLINK_DEV_FAMILY_MIPS,            JLINK_MIPS_REG_PC          },
                                          { JLINK_DEV_FAMILY_EFM8,            JLINK_EFM8_PC              },
                                          { JLINK_DEV_FAMILY_RISC_V,          JLINK_RISCV_REG_PC         },
                                          { JLINK_DEV_FAMILY_CORTEX_AR_ARMV8, JLINK_ARM_V8AR_REG_PC      },
                                          { JLINK_DEV_FAMILY_BT5511,          JLINK_8051_REG_PC          }
                                        };

static const U32 _aSuppDevFamiliesJTAG[] = {
                                            JLINK_CORE_ANY,
                                            JLINK_CORE_CORTEX_M1,
                                            JLINK_CORE_COLDFIRE,
                                            JLINK_CORE_CORTEX_M3,
                                            JLINK_CORE_CORTEX_M3_R1P0,
                                            JLINK_CORE_CORTEX_M3_R1P1,
                                            JLINK_CORE_CORTEX_M3_R2P0,
                                            JLINK_CORE_CORTEX_M3_R2P1,
                                            JLINK_CORE_SIM,
                                            JLINK_CORE_XSCALE,
                                            JLINK_CORE_CORTEX_M0,
                                            JLINK_CORE_CORTEX_M23,
                                            JLINK_CORE_ARM7,
                                            JLINK_CORE_ARM7TDMI,
                                            JLINK_CORE_ARM7TDMI_R3,
                                            JLINK_CORE_ARM7TDMI_R4,
                                            JLINK_CORE_ARM7TDMI_S,
                                            JLINK_CORE_ARM7TDMI_S_R3,
                                            JLINK_CORE_ARM7TDMI_S_R4,
                                            JLINK_CORE_CORTEX_A8,
                                            JLINK_CORE_CORTEX_A7,
                                            JLINK_CORE_CORTEX_A9,
                                            JLINK_CORE_CORTEX_A12,
                                            JLINK_CORE_CORTEX_A15,
                                            JLINK_CORE_CORTEX_A17,
                                            JLINK_CORE_ARM9,
                                            JLINK_CORE_ARM9TDMI_S,
                                            JLINK_CORE_ARM920T,
                                            JLINK_CORE_ARM922T,
                                            JLINK_CORE_ARM926EJ_S,
                                            JLINK_CORE_ARM946E_S,
                                            JLINK_CORE_ARM966E_S,
                                            JLINK_CORE_ARM968E_S,
                                            JLINK_CORE_ARM11,
                                            JLINK_CORE_ARM1136,
                                            JLINK_CORE_ARM1136J,
                                            JLINK_CORE_ARM1136J_S,
                                            JLINK_CORE_ARM1136JF,
                                            JLINK_CORE_ARM1136JF_S,
                                            JLINK_CORE_ARM1156,
                                            JLINK_CORE_ARM1176,
                                            JLINK_CORE_ARM1176J,
                                            JLINK_CORE_ARM1176J_S,
                                            JLINK_CORE_ARM1176JF,
                                            JLINK_CORE_ARM1176JF_S,
                                            JLINK_CORE_CORTEX_R4,
                                            JLINK_CORE_CORTEX_R5,
                                            JLINK_CORE_CORTEX_R8,
                                            JLINK_CORE_CORTEX_R7,  // Numerical value larger than the one for _R8 (Historical reasons...)
                                            JLINK_CORE_RX,
                                            JLINK_CORE_RX610,
                                            JLINK_CORE_RX62N,
                                            JLINK_CORE_RX62T,
                                            JLINK_CORE_RX63N,
                                            JLINK_CORE_RX630,
                                            JLINK_CORE_RX63T,
                                            JLINK_CORE_RX621,
                                            JLINK_CORE_RX62G,
                                            JLINK_CORE_RX631,
                                            JLINK_CORE_RX65N,
                                            JLINK_CORE_RX64M,
                                            JLINK_CORE_RX66T,
                                            JLINK_CORE_RX71M,
                                            JLINK_CORE_CORTEX_M4,
                                            JLINK_CORE_CORTEX_M7,
                                            JLINK_CORE_CORTEX_M33,
                                            JLINK_CORE_CORTEX_M55,
                                            JLINK_CORE_STAR,
                                            JLINK_CORE_OLYMPUS,
                                            JLINK_CORE_CORTEX_A5,
                                            JLINK_CORE_POWER_PC,
                                            JLINK_CORE_POWER_PC_N1,
                                            JLINK_CORE_POWER_PC_N2,
                                            JLINK_CORE_MIPS,
                                            JLINK_CORE_MIPS_M4K,
                                            JLINK_CORE_MIPS_M14K,
                                            JLINK_CORE_MIPS_MICROAPTIV,
                                            JLINK_CORE_RV32,
                                            JLINK_CORE_RV64,
                                            JLINK_CORE_CORTEX_A32,
                                            JLINK_CORE_CORTEX_A53,
                                            JLINK_CORE_CORTEX_A55,
                                            JLINK_CORE_CORTEX_A57,
                                            JLINK_CORE_CORTEX_A72,
                                            JLINK_CORE_CORTEX_R52,
                                            JLINK_CORE_BT5511,
                                            0
                                           };

static const U32 _aSuppDevFamiliesMC2WJTAGTDI[] = {
                                                    JLINK_CORE_BT5511,
                                                    0
                                                  };
static const U32 _aSuppDevFamiliesCJTAG[] = {
                                            JLINK_CORE_ANY,
                                            JLINK_CORE_CORTEX_M1,
                                            JLINK_CORE_CORTEX_M3,
                                            JLINK_CORE_CORTEX_M3_R1P0,
                                            JLINK_CORE_CORTEX_M3_R1P1,
                                            JLINK_CORE_CORTEX_M3_R2P0,
                                            JLINK_CORE_CORTEX_M3_R2P1,
                                            JLINK_CORE_CORTEX_M0,
                                            JLINK_CORE_CORTEX_M23,
                                            JLINK_CORE_CORTEX_A8,
                                            JLINK_CORE_CORTEX_A7,
                                            JLINK_CORE_CORTEX_A9,
                                            JLINK_CORE_CORTEX_A12,
                                            JLINK_CORE_CORTEX_A15,
                                            JLINK_CORE_CORTEX_A17,
                                            JLINK_CORE_CORTEX_R4,
                                            JLINK_CORE_CORTEX_R5,
                                            JLINK_CORE_CORTEX_R8,
                                            JLINK_CORE_CORTEX_R7,  // Numerical value larger than the one for _R8 (Historical reasons...)
                                            JLINK_CORE_CORTEX_M4,
                                            JLINK_CORE_CORTEX_M7,
                                            JLINK_CORE_CORTEX_M33,
                                            JLINK_CORE_CORTEX_M55,
                                            JLINK_CORE_STAR,
                                            JLINK_CORE_OLYMPUS,
                                            JLINK_CORE_CORTEX_A5,
                                            JLINK_CORE_CORTEX_A32,
                                            JLINK_CORE_CORTEX_A53,
                                            JLINK_CORE_CORTEX_A55,
                                            JLINK_CORE_CORTEX_A57,
                                            JLINK_CORE_CORTEX_A72,
                                            JLINK_CORE_CORTEX_R52,
                                            JLINK_CORE_RV32,
                                            JLINK_CORE_RV64,
                                            0
                                           };

static const U32 _aSuppDevFamiliesSWD[]  = {
                                            JLINK_CORE_ANY,
                                            JLINK_CORE_CORTEX_M1,
                                            JLINK_CORE_CORTEX_M3,
                                            JLINK_CORE_CORTEX_M3_R1P0,
                                            JLINK_CORE_CORTEX_M3_R1P1,
                                            JLINK_CORE_CORTEX_M3_R2P0,
                                            JLINK_CORE_CORTEX_M3_R2P1,
                                            JLINK_CORE_CORTEX_M0,
                                            JLINK_CORE_CORTEX_M23,
                                            JLINK_CORE_CORTEX_A8,
                                            JLINK_CORE_CORTEX_A7,
                                            JLINK_CORE_CORTEX_A9,
                                            JLINK_CORE_CORTEX_A12,
                                            JLINK_CORE_CORTEX_A15,
                                            JLINK_CORE_CORTEX_A17,
                                            JLINK_CORE_CORTEX_R4,
                                            JLINK_CORE_CORTEX_R5,
                                            JLINK_CORE_CORTEX_R8,
                                            JLINK_CORE_CORTEX_R7,  // Numerical value larger than the one for _R8 (Historical reasons...)
                                            JLINK_CORE_CORTEX_M4,
                                            JLINK_CORE_CORTEX_M7,
                                            JLINK_CORE_CORTEX_M33,
                                            JLINK_CORE_CORTEX_M55,
                                            JLINK_CORE_STAR,
                                            JLINK_CORE_OLYMPUS,
                                            JLINK_CORE_CORTEX_A5,
                                            JLINK_CORE_CORTEX_A32,
                                            JLINK_CORE_CORTEX_A53,
                                            JLINK_CORE_CORTEX_A55,
                                            JLINK_CORE_CORTEX_A57,
                                            JLINK_CORE_CORTEX_A72,
                                            JLINK_CORE_CORTEX_R52,
                                            JLINK_CORE_RV32,
                                            JLINK_CORE_RV64,
                                            0
                                           };

static const U32 _aSuppDevFamiliesFINE[] = {
                                            JLINK_CORE_ANY,
                                            JLINK_CORE_RX,
                                            JLINK_CORE_RX610,
                                            JLINK_CORE_RX62N,
                                            JLINK_CORE_RX62T,
                                            JLINK_CORE_RX63N,
                                            JLINK_CORE_RX630,
                                            JLINK_CORE_RX63T,
                                            JLINK_CORE_RX621,
                                            JLINK_CORE_RX62G,
                                            JLINK_CORE_RX631,
                                            JLINK_CORE_RX65N,
                                            JLINK_CORE_RX210,
                                            JLINK_CORE_RX21A,
                                            JLINK_CORE_RX220,
                                            JLINK_CORE_RX230,
                                            JLINK_CORE_RX231,
                                            JLINK_CORE_RX23T,
                                            JLINK_CORE_RX24T,
                                            JLINK_CORE_RX111,
                                            JLINK_CORE_RX110,
                                            JLINK_CORE_RX113,
                                            JLINK_CORE_RX130,
                                            JLINK_CORE_RX64M,
                                            JLINK_CORE_RX66T,
                                            JLINK_CORE_RX71M,
                                            0
                                           };

static const U32 _aSuppDevFamiliesICSP[] = {
                                            JLINK_CORE_ANY,
                                            JLINK_CORE_MIPS,
                                            JLINK_CORE_MIPS_M4K,
                                            JLINK_CORE_MIPS_M14K,
                                            JLINK_CORE_MIPS_MICROAPTIV,
                                            0
                                           };

static const U32 _aSuppDevFamiliesC2[]   = {
                                            JLINK_CORE_ANY,
                                            JLINK_CORE_EFM8_UNSPEC,
                                            JLINK_CORE_CIP51,
                                            0
                                           };

static const _TIF_LIST _aTIFList[] = {
  {"JTAG",                                 'J', JLINKARM_TIF_JTAG,          _aSuppDevFamiliesJTAG         },
  {"SWD",                                  'S', JLINKARM_TIF_SWD,           _aSuppDevFamiliesSWD          },
  {"FINE",                                 'F', JLINKARM_TIF_FINE,          _aSuppDevFamiliesFINE         },
  {"ICSP",                                 'I', JLINKARM_TIF_ICSP,          _aSuppDevFamiliesICSP         },
  {"C2",                                   'C', JLINKARM_TIF_C2,            _aSuppDevFamiliesC2           },
  {"cJTAG",                                'T', JLINKARM_TIF_CJTAG,         _aSuppDevFamiliesCJTAG        },
  {"2-wire",                               '2', JLINKARM_TIF_MC2WJTAG_TDI,  _aSuppDevFamiliesMC2WJTAGTDI  }
};

static const FLASHDL_ACTION _aAction[] = {
  {"", ""},
  {"Compare", "Comparing flash   [000%%]"},
  {"Erase",   "Erasing flash     [000%%]"},
  {"Program", "Programming flash [000%%]"},
  {"Verify",  "Verifying flash   [000%%]"}
};

static const char* _sDone = " Done.\n";

static const CMD_STR_ENTRY _aExec[] = {
    { "SilentUpdateFW",       0 }
  , { "SuppressInfoUpdateFW", 0 }
  , { "SetSimMode",           0 }
};

//
// MAC table for different Flasher ARM revisions
//
//                                             HWMajor0  HWMajor1  HWMajor2 ...
static const U8 _aMACByteProd_FlasherARM[] = { 0x04,     0x04,     0x04,    0x04,                  0x09, 0x11, _MAC_BYTE_PROD_INVALID };
static const U8 _aMACByteProd_FlasherPPC[] = { 0x06,     0x06,     0x06,    0x06,                  0x06, _MAC_BYTE_PROD_INVALID       };
static const U8 _aMACByteProd_FlasherRX[]  = { 0x05,     0x05,     0x05,    0x05,                  0x05, _MAC_BYTE_PROD_INVALID       };
static const U8 _aMACByteProd_FlasherPRO[] = { 0x0A,     0x0A,     0x0A,    0x0A,                  0x0A, 0x12, _MAC_BYTE_PROD_INVALID };
static const U8 _aMACByteProd_JLinkPRO[]   = { 0x01,     0x01,     0x01,    0x02,                  0x07, 0x10, _MAC_BYTE_PROD_INVALID };
static const U8 _aMACByteProd_JTracePRO[]  = { 0x0B,     0x0B,     0x0B,    _MAC_BYTE_PROD_INVALID                                    };
static const U8 _aMACByteProd_JLinkWiFi[]  = { 0x0F,     0x0F,     0x0F,    _MAC_BYTE_PROD_INVALID                                    };
//
// Table which holds pointers to MAC tables for different poroduct types
//
static const _MAC_TBL _aMACTbl[0x100] = {
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_UNKNOWN            (0x00)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK              (0x01)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_CF           (0x02)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_CE           (0x03)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_KS           (0x04)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_DIGI_LINK          (0x05)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_MIDAS              (0x06)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_SAMICE             (0x07)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JTRACE             (0x08)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JTRACE_CS          (0x09)
  _aMACByteProd_FlasherARM,                                                                       // JLINK_EMU_PRODUCT_ID_FLASHER_ARM        (0x0A)
  _aMACByteProd_JLinkPRO,                                                                         // JLINK_EMU_PRODUCT_ID_JLINK_PRO          (0x0B)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_EDU          (0x0C)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_ULTRA        (0x0D)
  _aMACByteProd_FlasherPPC,                                                                       // JLINK_EMU_PRODUCT_ID_FLASHER_PPC        (0x0E)
  _aMACByteProd_FlasherRX,                                                                        // JLINK_EMU_PRODUCT_ID_FLASHER_RX         (0x0F)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_OB_RX200     (0x10)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE         (0x11)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_OB_SAM3U128  (0x12)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_CORTEXM (0x13)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_LPC     (0x14)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_STM32   (0x15)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_FSL     (0x16)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_ADI     (0x17)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_ENERGY_MICRO_EFM32 (0x18)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_XMC4000 (0x19)
  NULL,                                                                                           // 0x1A
  NULL,                                                                                           // 0x1B
  NULL,                                                                                           // 0x1C
  NULL,                                                                                           // 0x1D
  NULL,                                                                                           // 0x1E
  NULL,                                                                                           // 0x1F
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LITE_XMC4200 (0x20)
  NULL,                                                                                           // JLINK_EMU_PRODUCT_ID_JLINK_LPC_LINK2    (0x21)
  _aMACByteProd_FlasherPRO,                                                                       // JLINK_EMU_PRODUCT_ID_FLASHER_PRO        (0x22)
  NULL,                                                                                           // EMU_PRODUCT_ID_JLINK_LPCXpresso_V2      (0x23)
  NULL,                                                                                           // EMU_PRODUCT_ID_FLASHER_SECURE           (0x24)
  NULL,                                                                                           // EMU_PRODUCT_ID_CMSIS_DAP                (0x25)
  _aMACByteProd_JTracePRO,                                                                        // JLINK_EMU_PRODUCT_ID_JTRACE_PRO         (0x26)
  _aMACByteProd_JLinkWiFi,                                                                        // EMU_PRODUCT_ID_JLINK_WIFI (0x27)
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,                                                 // 0x28-0x2F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x30-0x3F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x40-0x4F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x50-0x5F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x60-0x6F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x70-0x7F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x80-0x8F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0x90-0x9F
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0xA0-0xAF
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0xB0-0xBF
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0xC0-0xCF
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0xD0-0xDF
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 0xE0-0xEF
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,             // 0xF0-0xFD
  NULL,
  NULL                                                                                            // 0xFF
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

static int _ExecShowCommands(const char* s);

/*********************************************************************
*
*       _UTIL_CopyString
*/
static void _UTIL_CopyString(char* pDest, const char* pSrc, U32 MaxLen) {
  U32 Len;

  if (pDest && pSrc && MaxLen) {
    Len = (int)strlen(pSrc);        // x64 warning suppress: Safe to cast as we are sure to never exceed destination type boundaries here
    if (Len >= (MaxLen - 1)) {
      Len = MaxLen - 1;             // Reserve space for terminating \0 character
    }
    if (Len) {
      memcpy(pDest, pSrc, Len);
    }
    *(pDest + Len) = 0;             // Add \0
  }
}

/*********************************************************************
*
*       _ReportOut
*/
static void _ReportOut(const char* s) {
  printf("%s", s);
#ifndef WIN32
  fflush(stdout);  // Linux / OS X buffers the output until the next new line
#endif
}

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
*       _cbErrorOut()
*/
static void _cbErrorOut(const char *s) {
  _ReportOutf("\n****** Error: %s\n", s);
}

/*********************************************************************
*
*       _cbWarnOut()
*/
static void _cbWarnOut(const char *s) {
  _ReportOutf("\n**************************\nWARNING: %s\n**************************\n\n", s);
}

/*********************************************************************
*
*       _cbLog()
*/
static void _cbLog(const char *s) {
  _ReportOutf("Info: %s", s);
  if (*(s + UTIL_strlen(s) - 1) != '\n') {
    _ReportOutf("\n");
  }
}

/*********************************************************************
*
*       _Log
*/
void _Log(const char *s) {
  _ReportOutf(s);
  if (*(s + UTIL_strlen(s) - 1) != '\n') {
    _ReportOutf("\n");
  }
}

/*********************************************************************
*
*       _PrintHex32
*/
static void _PrintHex32(char* pBuf, U32 BufSize, U32 Data32) {
  UTIL_snprintf(pBuf, BufSize, "%.8X", Data32);
}

/*********************************************************************
*
*       _PrintHex64
*/
static void _PrintHex64(char* pBuf, U32 BufSize, U64 Data64) {
  UTIL_snprintf(pBuf, BufSize, "%.8X%.8X", (U32)(Data64 >> 32), (U32)(Data64 >> 0));
}

/*********************************************************************
*
*       _Sleep
*/
static void _Sleep(int ms) {
#ifdef WIN32
  Sleep(ms);
#else
  usleep(ms * 1000);
#endif
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

#ifdef linux

typedef char * (READ_LINE)  (const char *);
typedef int    (ADD_HISTORY)(const char *);

/*********************************************************************
*
*       _OpenLibrary
*/
static void* _OpenLibrary(const char* pSoName) {
  char         acPath [PATH_MAX + 1];
  ssize_t      NumBytes;
  char       * p;
  void       * pLib;

  pLib    = NULL;
  //
  // Get the path to the application which uses the shared library.
  //
  NumBytes = readlink("/proc/self/exe", acPath, PATH_MAX);
  if (NumBytes > 0) {
    //
    // Create the path to shared library.
    //
    acPath[NumBytes] = 0; // readlink(2) does not set \0 byte
    p                = strrchr(acPath, '/');
    if (p) {
      ++p;
    } else {
      p = acPath;
    }
    _UTIL_CopyString(p, "ThirdParty/", _SIZEOF(acPath) - (int)(p - acPath));
    strcat(acPath, pSoName);
    //
    // Try to open the shared libray stored in the directory where the application is stored.
    //
    pLib = dlopen(acPath, RTLD_LAZY);
    if (pLib == NULL) {
      //
      // Shared library not found. Try to load it from the installation path.
      //
      sprintf(acPath, "/opt/SEGGER/JLink/ThirdParty/ThirdParty/%s", pSoName);
      pLib = dlopen(acPath, RTLD_LAZY);
      if (pLib == NULL) {
        //
        // Shared library not found. Try to load it from the system path.
        //
        pLib = dlopen(pSoName, RTLD_LAZY);
      }
    }
  }
  return pLib;
}

/*********************************************************************
*
*       _GetSymbol
*/
static void * _GetSymbol(void * pLib, const char * pName) {
  void * pSymbol;
  char * pError;

  dlerror();
  pSymbol = dlsym(pLib, pName);
  pError = dlerror();
  if (pError) {
    pSymbol = NULL;
  }
  return pSymbol;
}

#endif

/*********************************************************************
*
*       _OpenFile
*/
static HANDLE _OpenFile(const char * sFile, U32 Flags) {
#ifdef WIN32
  HANDLE hFile;
  DWORD  AccessFlags = 0;
  DWORD  ShareFlags  = 0;
  DWORD  CreateFlags = 0;

  AccessFlags |= (Flags & FILE_FLAG_READ)        ? GENERIC_READ      : 0;
  AccessFlags |= (Flags & FILE_FLAG_WRITE)       ? GENERIC_WRITE     : 0;
  ShareFlags  |= (Flags & FILE_FLAG_SHARE_READ)  ? FILE_SHARE_READ   : 0;
  ShareFlags  |= (Flags & FILE_FLAG_SHARE_WRITE) ? FILE_SHARE_WRITE  : 0;
  if (Flags & FILE_FLAG_CREATE) {
    if        (Flags & FILE_FLAG_EXCL) {
      CreateFlags = CREATE_NEW;
    } else if (Flags & FILE_FLAG_TRUNC) {
      CreateFlags = CREATE_ALWAYS;
    } else {
      CreateFlags = OPEN_ALWAYS;
    }
  } else if (Flags & FILE_FLAG_TRUNC) {
    CreateFlags = TRUNCATE_EXISTING;
  } else {
    CreateFlags = OPEN_EXISTING;
  }
  hFile = CreateFile(sFile, AccessFlags, ShareFlags, NULL, CreateFlags, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile != INVALID_HANDLE_VALUE) {
    if (Flags & FILE_FLAG_APPEND) {
      SetFilePointer(hFile, 0, 0, FILE_END);
    }
  }
  return hFile;
#else
  HANDLE hFile;
  U32 AccessFlags = 0;
  U32 CreateFlags = 0;

  AccessFlags |= (Flags & FILE_FLAG_READ)        ? O_RDONLY     : 0;
  AccessFlags |= (Flags & FILE_FLAG_WRITE)       ? O_WRONLY     : 0;
  if (Flags & FILE_FLAG_CREATE) {
    if        (Flags & FILE_FLAG_EXCL) {
      CreateFlags = O_CREAT | O_EXCL;
    } else if (Flags & FILE_FLAG_TRUNC) {
      CreateFlags = O_CREAT | O_TRUNC;
    } else {
      CreateFlags = O_CREAT;
    }
  } else if (Flags & FILE_FLAG_TRUNC) {
    CreateFlags = O_TRUNC;
  }
  hFile = (HANDLE)open(sFile, AccessFlags | CreateFlags, 0644); // 0644 = -rw-r--r-- mode
  if (hFile != INVALID_HANDLE_VALUE) {
    if (Flags & FILE_FLAG_APPEND) {
      lseek(hFile, 0, SEEK_END);
    }
  }
  return hFile;
#endif
}

/*********************************************************************
*
*       _CloseFile
*/
static void _CloseFile(HANDLE hFile) {
#ifdef WIN32
  CloseHandle((HANDLE)hFile);
#else
  close(hFile);
#endif
}

/*********************************************************************
*
*       _ReadFile
*/
static int _ReadFile(HANDLE hFile, void * pData, U32 NumBytes) {
#ifdef WIN32
  DWORD NumBytesRead;
  if (ReadFile((HANDLE)hFile, pData, NumBytes, &NumBytesRead, NULL) == 0) {
    return -1;                    // Error
  }
  return (int)NumBytesRead;       // O.K.
#else
  int NumBytesRead;

  NumBytesRead = read((int)hFile, pData, NumBytes);
  return NumBytesRead;
#endif
}

/*********************************************************************
*
*       _WriteFile
*/
static int _WriteFile(HANDLE hFile, const void* pData, U32 NumBytes) {
#ifdef WIN32
  DWORD NumBytesWritten;
  if (WriteFile((HANDLE)hFile, pData, NumBytes, &NumBytesWritten, NULL) == 0) {
    return -1;                    // Error
  }
  return (int)NumBytesWritten;    // O.K.
#else
  int NumBytesWritten;

  NumBytesWritten = write((int)hFile, pData, NumBytes);
  return NumBytesWritten;
#endif
}

/*********************************************************************
*
*       _GetFileSize
*/
static U32 _GetFileSize(HANDLE hFile) {
#ifdef WIN32
  DWORD FileSize;
  FileSize = GetFileSize((HANDLE)hFile, NULL);
  return (U32)FileSize;
#else
  U32   FileSize;
  off_t CurrentPos;

  CurrentPos = lseek((int)hFile, 0, SEEK_CUR);
  FileSize   = lseek((int)hFile, 0, SEEK_END);
  lseek((int)hFile, CurrentPos, SEEK_SET);
  return FileSize;
#endif
}

/*********************************************************************
*
*       _CRC_CalcFast
*/
static U32 _CRC_CalcFast(const U8* pData, unsigned NumBytes, U32 crc, U32 Polynom) {
  static U32 _aCRC8[256] = {0};
  //
  // Build small CRC table if required
  //
  if ((_aCRC8[1] == 0) || (Polynom != _Polynom)) {
    U32 i, n, v;
    for (n = 0; n < 256; n++) {
      v = n;
      i = 8;
      do {
        if (v & 1) {
          v = (v >> 1) ^ Polynom;
        } else {
          v >>= 1;
        }
      } while (--i);
      _aCRC8[n] = v;
    }
  }
  _Polynom = Polynom;
  //
  // Calculate CRC in units of 8 bytes
  //
  if (NumBytes >= 8) {
    U32* p;
    p = (U32*)pData;
    do {
      crc ^= *p++;
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc ^= *p++;
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
      NumBytes -= 8;
    } while (NumBytes >= 8);
    pData = (const U8*)p;
  }
  //
  // Calculate CRC in units of bytes
  //
  if (NumBytes) {
    do {
      crc ^= *pData++;
      crc  = _aCRC8[crc & 0xFF] ^ (crc >> 8);
    } while (--NumBytes);
  }
  return crc;
}

/*********************************************************************
*
*       _CRC_Calc32Fast
*/
static U32 _CRC_Calc32Fast(const U8* pData, unsigned NumBytes, U32 crc) {
  U32 Polynom;
  Polynom = 0xEDB88320;    // Normal form is 0x04C11DB7
  return _CRC_CalcFast(pData, NumBytes, crc, Polynom);
}

/*********************************************************************
*
*       _Load8LE
*/
static U32 _Load8LE(const U8* p) {
  U32 r;
  r = *p;
  return r;
}

/*********************************************************************
*
*       _Load16LE
*/
static U32 _Load16LE(const U8* p) {
  U32 r;
  r = (*(p + 0) <<  0) | (*(p + 1) <<  8);
  return r;
}

/*********************************************************************
*
*       _Load32LE
*/
static U32 _Load32LE(const U8* p) {
  U32 r;
  r = (*(p + 0) <<  0) | (*(p + 1) <<  8) | (*(p + 2) << 16) | (*(p + 3) << 24);
  return r;
}

/*********************************************************************
*
*       _Load64LE
*/
static U64 _Load64LE(const U8* p) {
  U64 r;
  r  = (*(p + 4) <<  0) | (*(p + 5) <<  8) | (*(p + 6) << 16) | (*(p + 7) << 24);
  r <<= 16;
  r <<= 16;
  r |= (*(p + 0) <<  0) | (*(p + 1) <<  8) | (*(p + 2) << 16) | (*(p + 3) << 24);
  return r;
}

/*********************************************************************
*
*       _PrintTime
*/
static void _PrintTime(U64 Time, char* pBuffer, U32 BufferSize) {
  int hh;
  int mm;
  int ss;
  int ms;
  int us;
#ifdef WIN32
  hh = (int) (Time / (U64)3600000000uL);
#else
  hh = (int) (Time / (U64)3600000000uLL); // GCC under Linux generates a warning if 64-bit constants are not explicitly defined as 64-bit values (long long int)
#endif
  mm = (int)((Time /        60000000) % 60);
  ss = (int)((Time /         1000000) % 60);
  ms = (int)((Time /            1000) % 1000);
  us = (int)((Time)                   % 1000);
  if (hh) {
    UTIL_snprintf(pBuffer, BufferSize, "%.1d:%.2d:%.2d.%.3d %.3d", hh, mm, ss, ms, us);
  } else if (mm) {
    UTIL_snprintf(pBuffer, BufferSize, "%.1d:%.2d.%.3d %.3d", mm, ss, ms, us);
  } else { //if (ss) {
    UTIL_snprintf(pBuffer, BufferSize, "%.1d.%.3d %.3d", ss, ms, us);
  }
}

/*********************************************************************
*
*       _Cmd2String
*/
static const char* _Cmd2String(U8 Cmd) {
  switch (Cmd) {
  case 2:  return "SCAN_N";
  case 4:  return "RESTART";
  case 8:  return "ABORT";
  case 10: return "DPACC";
  case 11: return "APACC";
  case 12: return "INTEST";
  case 14: return "IDCODE";
  case 15: return "BYPASS";
  }
  return "Unknown JTAG instruction";
}

/*********************************************************************
*
*       _MakeLowercase
*/
static int _MakeLowercase(int c) {
  if ((c >= 'A') && (c <= 'Z')) {
    c += 0x20;
  }
  return c;
}

/*********************************************************************
*
*       _Hex2Dec
*/
static int _Hex2Dec(U8 c) {
  if ((c >='0') && (c <= '9')) {
    return c - '0';
  }
  if ((c >='a') && (c <= 'f')) {
    return c - 'a' + 10;
  }
  if ((c >= 'A') && (c <= 'F')) {
    return c - 'A' + 10;
  }
  return -1;
}

/*********************************************************************
*
*       _EatWhite
*/
static void _EatWhite(const char** ps) {
  const char* s = *ps;
  while ((*s == ' ') || (*s == '\t') || (*s == '\r') || (*s == '\n')) {
    s++;
  }
  *ps = s;
}

/*********************************************************************
*
*       _EatWhiteAndSeparator
*/
static void _EatWhiteAndSeparator(const char** ps) {
  const char* s;

  _EatWhite(ps);
  s = *ps;
  if (*s == ',') {
    s++;
    *ps = s;
    _EatWhite(ps);
  }
}

/*********************************************************************
*
*       _ParseHex
*/
static const char* _ParseHex(const char** ps, U32* pData) {
  U32 Data = 0;
  int NumDigits = 0;
  char c;

  _EatWhite(ps);
  if (**ps == '0') {
    c = *(*ps + 1);
    if (c == 'x') {
      (*ps) += 2;
    } else if (c == 'X') {
      (*ps) += 2;
    }
  }
  do {
    int v =  _Hex2Dec(**ps);
    if (v >= 0) {
      Data = (Data << 4) | v;
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
*       _TryParseMemZone
*
*  Function description
*    Tries to parse a memory zone, if specified for the current command.
*    In general, zones are specified by "<Zone>:" right before the address.
*
*  Notes
*    (1) If no zone specifier is found, we just fill the psZone buffer with 0, indicating an empty zone.
*/
static void _TryParseMemZone(const char** ps, char* psZone, int SizeOfBuffer) {
  const char* s;
  char* psZoneOrg;
  int ZoneFound;
  int NumBytesRem;
  //
  // Parse until ':' or end of string is hit
  //
  _EatWhite(ps);
  NumBytesRem = SizeOfBuffer;
  ZoneFound = 0;
  s = *ps;
  psZoneOrg = psZone;
  do {
    //
    // End of main string reached? We are done
    //
    if (*s == 0) {
      break;
    }
    if (*s == '/') {  // Comments may not be handled as zones
      break;
    }
    //
    // End of zone description found? We are done
    //
    if (*s == ':') {
      s++;             // Skip ':'
      *psZone = 0;     // Terminate zone string
      ZoneFound = 1;
      break;
    }
    if (NumBytesRem > 1) {
      *psZone++ = *s;
      NumBytesRem--;
    }
    s++;
  } while (1);
  if (ZoneFound) {
    *ps = s;
  } else {
    memset(psZoneOrg, 0, SizeOfBuffer);
  }
}

/*********************************************************************
*
*       _ParseHexString
*/
static const char* _ParseHexString(const char** ps, U8* pData, int NumBits) {
  int i;
  int Off;
  const char * s;

  _EatWhite(ps);
  if (**ps == '0') {
    if (*(*ps + 1) == 'x') {
      (*ps) += 2;
    }
  }
  s = *ps;
  for (i = 0; i < NumBits; i+=4) {
    int v;
    Off = (NumBits - 1 - i) / 4;
    v =  _Hex2Dec(*(s + Off));
    if (v < 0) {
      return "Unexpected character in hex string";
    }
    if ((i & 4) == 0) {
      *pData = v;
    } else {
      *pData |= (v << 4);
      pData++;
    }
  }
  *ps += (NumBits + 3) / 4;
  return 0;
}

/*********************************************************************
*
*       _ParseChar
*/
static const char* _ParseChar(const char** ps, char c) {
  _EatWhite(ps);
  if (**ps != c) {
    return "Unexpected character";
  }
  (*ps)++;
  return NULL;
}

/*********************************************************************
*
*       _ParseHex64
*/
static const char* _ParseHex64(const char** ps, U64* pData) {
  U64 Data = 0;
  int NumDigits = 0;
  _EatWhite(ps);
  if (**ps == '0') {
    if (*(*ps + 1) == 'x') {
      (*ps) += 2;
    }
  }
  do {
    int v =  _Hex2Dec(**ps);
    if (v >= 0) {
      Data = (Data << 4) | v;
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
*       _ParseDec
*/
static const char* _ParseDec(const char** ps, U32* pData) {
  U32 Data = 0;
  int NumDigits = 0;
  int IsNegative;
  _EatWhite(ps);
  //
  // Negative sign?
  //
  IsNegative = 0;
  if (**ps == '-') {
    (*ps)++;
    IsNegative = 1;
  }
  //
  // Parse decimal integer
  //
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
      if (IsNegative) {
        Data *= -1;
      }
      *pData = Data;
      return NULL;
    }
  } while (1);
}

/*********************************************************************
*
*       _ParseString
*/
static const char* _ParseString(const char** ps, char* pBuffer, unsigned BufferSize) {
  if (pBuffer && BufferSize) {
    const char *s = *ps;
    char QuotationMarks = 0;
    *pBuffer = 0;
    _EatWhite(&s);
    if (*s == '"') {
      QuotationMarks++;
      s++;
    }
    while (*s && (*s != '\r') && (*s != '\n') && (((*s != ',') && (*s != ' ')) || QuotationMarks) && --BufferSize) {
      if (*s == '"') {
        s++;
        break;
      }
      *pBuffer++ = *s++;

    }
    *pBuffer = 0;
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    *ps = s;
  }
  return *ps;
}

/*********************************************************************
*
*       _JLinkstricmp
*
*  Notes
*    Call different than _stricmp to avoid linker errors with MinGW
*    which also defines this function non-static and gets a name conflict.
*/
static int _JLinkstricmp(const char * dst, const char * src) {
  int f;
  int l;

  do {
    if (((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z')) {
      f -= ('A' - 'a');
    }
    if (((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z')) {
      l -= ('A' - 'a');
    }
  } while (f && (f == l));
  return (f - l);
}

/*********************************************************************
*
*       _JLinkstrnicmp
*
*  Notes
*    Call different than _stricmp to avoid linker errors with MinGW
*    which also defines this function non-static and gets a name conflict.
*/
static int _JLinkstrnicmp(const char * s0, const char * s1, size_t NumBytes) {
  char c0;
  char c1;

  while (NumBytes-- > 0) {
    c0 = *s0++;
    c1 = *s1++;
    c0 = _MakeLowercase(c0);
    c1 = _MakeLowercase(c1);
    if (c0 != c1) {
      return 1;
    }
  }
  return 0;
}

/*********************************************************************
*
*       _TIF2Name
*/
static const char* _TIF2Name(int Interface) {
  static char _acTIFName[16];
  switch (Interface) {
  case JLINKARM_TIF_JTAG:              _UTIL_CopyString(_acTIFName, "JTAG", _SIZEOF(_acTIFName)); break;
  case JLINKARM_TIF_SWD:               _UTIL_CopyString(_acTIFName, "SWD", _SIZEOF(_acTIFName));  break;
  case JLINKARM_TIF_BDM3:              _UTIL_CopyString(_acTIFName, "BDM3", _SIZEOF(_acTIFName)); break;
  case JLINKARM_TIF_FINE:              _UTIL_CopyString(_acTIFName, "FINE", _SIZEOF(_acTIFName)); break;
  case JLINKARM_TIF_2_WIRE_JTAG_PIC32: _UTIL_CopyString(_acTIFName, "ICSP", _SIZEOF(_acTIFName)); break;
  case JLINKARM_TIF_SPI:               _UTIL_CopyString(_acTIFName, "SPI", _SIZEOF(_acTIFName));  break;
  case JLINKARM_TIF_C2:                _UTIL_CopyString(_acTIFName, "C2", _SIZEOF(_acTIFName));   break;
  case JLINKARM_TIF_CJTAG:             _UTIL_CopyString(_acTIFName, "cJTAG", _SIZEOF(_acTIFName)); break;
  case JLINKARM_TIF_MC2WJTAG_TDI:      _UTIL_CopyString(_acTIFName, "2-wire", _SIZEOF(_acTIFName)); break;
  default:
    UTIL_snprintf(_acTIFName, UTIL_SIZEOF(_acTIFName), "Interface %d", Interface);
  }
  return _acTIFName;
}

/*********************************************************************
*
*       _Name2TIF
*/
static int _Name2TIF(const char* s) {
  if (_JLinkstricmp(s, "JTAG") == 0) {
    return JLINKARM_TIF_JTAG;
  } else if (_JLinkstricmp(s, "SWD") == 0) {
    return JLINKARM_TIF_SWD;
  } else if (_JLinkstricmp(s, "BDM3") == 0) {
    return JLINKARM_TIF_BDM3;
  } else if (_JLinkstricmp(s, "UART") == 0) {                // Formerly name of FINE interface
    return JLINKARM_TIF_FINE;
  } else if (_JLinkstricmp(s, "FINE") == 0) {
    return JLINKARM_TIF_FINE;
  } else if (_JLinkstricmp(s, "2-wire-JTAG-PIC32") == 0) {   // Compatible reasons. Official interface name is: ICSP
    return JLINKARM_TIF_2_WIRE_JTAG_PIC32;
  } else if (_JLinkstricmp(s, "ICSP") == 0) {
    return JLINKARM_TIF_2_WIRE_JTAG_PIC32;
  } else if (_JLinkstricmp(s, "SPI") == 0) {
    return JLINKARM_TIF_SPI;
  } else if (_JLinkstricmp(s, "C2") == 0) {
    return JLINKARM_TIF_C2;
  } else if (_JLinkstricmp(s, "cJTAG") == 0) {
    return JLINKARM_TIF_CJTAG;
  } else if (_JLinkstricmp(s, "MC2WJTAG_TDI") == 0) {
    return JLINKARM_TIF_MC2WJTAG_TDI;
  } else if (_JLinkstricmp(s, "2-wire") == 0) {
    return JLINKARM_TIF_MC2WJTAG_TDI;
  } else {
    return -1;
  }
}

/*********************************************************************
*
*       _SupportsFixedVTref
*/
static int _SupportsFixedVTref(void) {
  int r;

  r = 0;
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_FIXED_VTREF) != 0) {
    r = 1;
  }
  return r;
}

/*********************************************************************
*
*       _ShowVTref
*/
static void _ShowVTref(U32 VTrefmV) {
  int r;
  U32 v;
  char ac[64];

  r = _SupportsFixedVTref();
  ac[0] = 0;
  if (r) {
    v = 0;
    JLINKARM_ReadEmuConfigMem((U8*)&v, CONFIG_OFF_FIXED_VTREF, 1);
    if (v != 0xFF) {                                                // Default which means "fixed VTref disabled"
      v *= 100;                                                     // Stored as mV / 100
      if (v == VTrefmV) {
        _UTIL_CopyString(ac, " (fixed)", sizeof(ac));
      }
    }
  }
  _ReportOutf("VTref=%d.%.3dV%s\n", VTrefmV / 1000, VTrefmV % 1000, ac);
}

/*********************************************************************
*
*       _cbFlashProgProgress
*
*    Function description
*      This call back function is called by the J-Link DLL during flash programming.
*      It is used to show the progress of an ongoing flash programming procedure on OS without GUI support.
*
*/
static void _cbFlashProgProgress(const char * sAction, const char * sProg, int Percentage) {
  U32 Step;
  int CurPercentage;
  char ac[128];

  (void)sProg;
  if (sAction == NULL) {  // Sanity checks. Action has to be given
    return;
  }
  //
  // Check which step is reported
  //
  Step = 1;
  do {
    if (_JLinkstrnicmp(sAction, _aAction[Step].sAction, UTIL_strlen(_aAction[Step].sAction)) == 0) {
      break;
    }
    Step++;
  } while (Step < (COUNTOF(_aAction)));
  memset(ac,0, sizeof(ac));
  //
  // Check if it is a "dummy step" (Previous step, 100%)
  //
  if ((Step < _FlashDLCurStep) && (Percentage == 100)) {
    return;
  }
  //
  // Check if it is a new step
  //
  if ((Step != _FlashDLCurStep)) {
    //
    // Check if current step is not finished yet
    //
    if (_FlashDLCurPercentage < FLASH_DL_NUM_INTERVALS) {
      _ReportOutf("\b\b\b\b\b100%%]");   // Format is "000%]", so we have to make 5 backspaces to overrite the current percentage
      _ReportOutf(_sDone);
    }
    _FlashDLCurStep = Step;
    _FlashDLCurPercentage = 0;
    _ReportOutf(_aAction[Step].sDescription);
  }
  //
  // Check if percentage needs an update
  //
  Percentage = (Percentage/FLASH_DL_INTERVAL);
  if (_FlashDLCurPercentage == FLASH_DL_NUM_INTERVALS) {
    return;
  };
  CurPercentage = Percentage;
  _ReportOutf("\b\b\b\b\b%.3d%%]", Percentage * FLASH_DL_INTERVAL);   // Format is "000%]", so we have to make 5 backspaces to overrite the current percentage
  if ((_FlashDLCurPercentage < FLASH_DL_NUM_INTERVALS) && (Percentage == FLASH_DL_NUM_INTERVALS)) {
    _ReportOutf(_sDone);
  }
  _FlashDLCurPercentage = CurPercentage;
}

/*********************************************************************
*
*       _memcmp
*
*  Function description
*    Compares memory areas
*
*  Return value
*    Offset of first different byte; -1 if all bytes are identical
*/
static int _memcmp(const U8 * p0, const U8 * p1, int NumBytes) {
  int i;
  for (i = 0; i < NumBytes; i++) {
    if (*p0++ != *p1++) {
      return i;
    }
  }
  return -1;
}

/*********************************************************************
*
*       _MeasureITarget()
*/
static void _MeasureITarget(int *pAvg, int *pMin, int *pMax) {
  U32 HWInfo[32];
  int ISum;
  int i;
  int IMin;
  int IMax;
  int ICur;
  int uA = 1000;

  ISum = 0;
  IMin = INT_MAX;
  IMax = INT_MIN;
  for (i = 0; i < ITARGET_SAMPLES; ++i) {
    memset(HWInfo, 0, sizeof HWInfo);
    JLINKARM_GetHWInfo(0xFFFFFFFF, HWInfo);
    //
    // Third word is the power consumption in mA
    //
    ICur = (int)HWInfo[2];
    ISum += ICur;
    if (ICur < IMin) {
      IMin = ICur;
    }
    if (ICur > IMax) {
      IMax = ICur;
    }
  }
  //
  // Return the measured current in uA
  //
  if (pAvg != NULL) {
    *pAvg = ISum * uA / i;
  }
  if (pMin != NULL) {
    *pMin = IMin * uA;
  }
  if (pMax != NULL) {
    *pMax = IMax * uA;
  }
}

/*********************************************************************
*
*       _EatEndMultiLineComment
*/
static void _EatEndMultiLineComment(const char** ppText) {
  const char* s = *ppText;
  char c;
  int  c16;
  while (1) {
    c   = *s;
    c16 = c | (*(s + 1) << 8);
    if (c == 0) {
      break;
    } else if (c16 == ('*' | ('/' << 8))) {             // End of multiline comment ?
      s += 2;
      break;
    }
    s++;
  }
  *ppText = s;
}

/*********************************************************************
*
*       _EatLine
*/
static void _EatLine(const char** ppText) {
  const char* s = *ppText;
  while (*s && (*s != 0x0A)) {
    s++;
  }
  if (*s == 0x0A) {
    s++;
  }
  *ppText = s;
}

/*********************************************************************
*
*       _EatWhiteAndComments
*/
static void _EatWhiteAndComments(const char** ppText) {
  const char* s = *ppText;
  while (1) {
    _EatWhite(&s);
    if (*s == ';') {
      _EatLine(&s);
    } else if (*s != '/') {
      break;
    } else {
      char c = *(s + 1);
      if (c == '/') {
        _EatLine(&s);
      } else if (c == '*') {
        s += 2;
        _EatEndMultiLineComment(&s);
      } else {
        break;
      }
    }
  }
  *ppText = s;
}

/*********************************************************************
*
*       _GetLineAndEatComments
*/
static char _GetLineAndEatComments(const char** ppText, char* pBuffer, unsigned BufferSize) {
  const char* s;

  s = *ppText;
  _EatWhiteAndComments(&s);
  if (*s == 0) {                                        // End of string? => Done
    return 0;
  }
  BufferSize--;                                         // Leave place for one char to terminate string
  while ((*s != '\r') && (*s != '\n') && (*s != 0)) {   // Find end of line
    if (BufferSize) {
      *pBuffer++ = *s;
      BufferSize--;
    }
    s++;
  }
  *pBuffer = 0;
  *ppText = s;
  return 1;
}

/*********************************************************************
*
*       _PrintNumBytes
*/
static void _PrintNumBytes(U32 NumBytes) {
  if ((NumBytes & 1023) == 0) {
    _ReportOutf("%d KBytes", NumBytes / 1024);
  } else {
    _ReportOutf("%d Bytes", NumBytes);
  }
}

/*********************************************************************
*
*       _PrintId
*/
static void _PrintId(U32 Data32) {
  int IsValid = Data32 & 1;
  int ManId   = (Data32 >>  1) & 0x7FF;
  int PartNo  = (Data32 >> 12) & 0xFFFF;
  int Version = (Data32 >> 28) & 0xF;
  if (IsValid) {
    _ReportOutf("JTAG Id: 0x%.8X  Version: 0x%X Part no: 0x%x Man. Id: %.4X \n", Data32, Version, PartNo, ManId);
  } else {
    _ReportOutf("JTAG Id: 0x%.8X: INVALID\n", Data32);
  }
}

/*********************************************************************
*
*       _PrintCPSR
*/
static const char* _PrintCPSR(U32 CPSR) {
  static char ac[255];      // Note: This is not thread safe ! (Which is o.k. for this small program)
  const char * sMode;
  U8 Mode = (U8)(CPSR & 0x1f);
  switch (Mode) {
  case 0x10: sMode = "User mode";   break;
  case 0x11: sMode = "FIQ mode";    break;
  case 0x12: sMode = "IRQ mode";    break;
  case 0x13: sMode = "SVC mode";    break;
  case 0x17: sMode = "ABORT mode";  break;
  case 0x1B: sMode = "UNDEF mode";  break;
  case 0x1f: sMode = "System mode"; break;
  default:
  sMode = "Unknown mode";
  }
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "%s%s%s%s", sMode
          ,CPSR & (1 << 5) ? ", THUMB" : ", ARM"
          ,CPSR & (1 << 6) ? " FIQ dis." : ""
          ,CPSR & (1 << 7) ? " IRQ dis." : ""
          );
  return ac;
}

/*********************************************************************
*
*       _PrintMACAddr
*
*  Function description
*    Prints devices MAC address
*
*  Additional information
*    MACHWIdent is the 4th block in the devices MACAddress and can be found on the sticker on the bottom of the device
*
*/
static void _PrintMACAddr(U64 MACAddr) {
  U32 SerialNo;   // It is important that serial number is unsigned, otherwise modulo operation will possibly lead to unexpected results
  U32 NumElements;
  int HWVersion;
  int HWVersionMajor;
  int ProdId;
  const U8* pMACByteProdTbl;
  const U8* p;
  //
  // MAC override via config area active? => Show this MAC
  //
  if (((unsigned)MACAddr) != 0xFFFFFFFF) {
    _ReportOutf("MAC-Addr:       %.2X:%.2X:%.2X:%.2X:%.2X:%.2X (user assigned)\n", (unsigned) (MACAddr        & 0xFF),
                                                                                  (unsigned)((MACAddr >>  8) & 0xFF),
                                                                                  (unsigned)((MACAddr >> 16) & 0xFF),
                                                                                  (unsigned)((MACAddr >> 24) & 0xFF),
                                                                                  (unsigned)((MACAddr >> 32) & 0xFF),
                                                                                  (unsigned)((MACAddr >> 40) & 0xFF));
  } else {
    //
    // No MAC override active? => Derive MAC from S/N + model
    //
    HWVersion       = JLINKARM_GetHardwareVersion();
    HWVersionMajor  = HWVersion / 10000 % 100;
    SerialNo        = (U32)JLINKARM_GetSN();
    ProdId          = JLINKARM_EMU_GetProductId();
    pMACByteProdTbl = _aMACTbl[ProdId & 0xFF].pMACByteProdTbl;
    if (pMACByteProdTbl) {
      NumElements = 0;
      p = pMACByteProdTbl;
      while (*p != _MAC_BYTE_PROD_INVALID) {
        NumElements++;
        p++;
      }
      if (HWVersionMajor < (int)NumElements) {
        _ReportOutf("MAC-Addr:       00:22:C7:%.2X:%.2X:%.2X (Default)\n", *(pMACByteProdTbl + HWVersionMajor), (U8) ((SerialNo % 100000) >> 8), (U8) ((SerialNo % 100000) >> 0));
      } else {
        _ReportOutf("WARNING: Unknown product. MAC address can not be displayed properly.\n");
      }
    } else {
      _ReportOutf("WARNING: Unknown product. MAC address can not be displayed properly.\n");
    }
  }
}

/*********************************************************************
*
*       _ShowFirmwareInfo
*/
void _ShowFirmwareInfo(void) {
  char ac[256];
  int  Version;
  JLINKARM_GetFirmwareString(ac, sizeof(ac));
  if (ac[0]) {
    _ReportOutf("Firmware: %s\n", ac);
    Version = JLINKARM_GetHardwareVersion();
    if (Version) {
      _ReportOutf("Hardware: V%d.%.2d\n", Version / 10000 % 100, Version / 100 % 100);
    }
  } else {
    _ReportOutf("Unable to retrieve firmware info !\n");
  }
}

/*********************************************************************
*
*       _ShowETMConfig
*/
static U32 _ShowETMConfig(void) {
  //
  // Identify ETM
  //
  U32 ETMVersion;
  ETMVersion = JLINKARM_ETM_IsPresent();
  if (ETMVersion) {
    U32 ConfigCode;
    ConfigCode = JLINKARM_ETM_ReadReg(1);
    _ReportOutf("  ETM V%d.%d: %d pairs addr.comp, %d data comp, %d MM decs, %d counters%s\n"
               ,(ETMVersion >> 4) & 15
               ,(ETMVersion >> 0) & 15
               ,ConfigCode & 15
               ,(ConfigCode >> 4) & 15
               ,(ConfigCode >> 8) & 31
               ,(ConfigCode >> 13) & 7
               ,((ConfigCode >> 16) & 1) ? ", sequencer" : "");
  }
  return ETMVersion;
}

/*********************************************************************
*
*       _ShowETBConfig
*/
static void _ShowETBConfig(void) {
  if (JLINKARM_ETB_IsPresent()) {
    U32 ETMControl;
    U32 Id;
    U32 RAMDepth;
    U32 RAMWidth;

    ETMControl = JLINKARM_ETM_ReadReg(0);
    if (ETMControl & 1) {
      JLINKARM_ETM_WriteReg(0, (ETMControl & ~1), 1);
    }

    Id       = JLINKARM_ETB_ReadReg(0);
    RAMDepth = JLINKARM_ETB_ReadReg(1);
    RAMWidth = JLINKARM_ETB_ReadReg(2);
    _ReportOutf("  ETB V%d.0: %dx%d bit RAM\n"
              ,(Id >> 28) & 15
              ,RAMDepth
              ,RAMWidth
             );

    if (ETMControl & 1) {
      JLINKARM_ETM_WriteReg(0, ETMControl, 0);
    }
  }
}

/*********************************************************************
*
*       _ShowETMState
*/
static void _ShowETMState(void) {
  U32 ETMVersion;
  U32 CtrlReg;
  U32 StatusReg;
  U32 v;

  ETMVersion = _ShowETMConfig();
  if (ETMVersion) {
    CtrlReg   = JLINKARM_ETM_ReadReg(0);
    StatusReg = JLINKARM_ETM_ReadReg(4);
    if (CtrlReg & 1) {
      _ReportOutf("Powered down\n");
    } else {
      if (CtrlReg & (1 << 1)) {
        _ReportOutf(" MonitorCPRTs\n");
      }
      switch ((CtrlReg >> 2) & 3) {
        case 0: _ReportOutf("    Data trace: OFF (default)\n");         break;
        case 1: _ReportOutf("    Data trace: DATA only\n");   break;
        case 2: _ReportOutf("    Data trace: ADDR only\n");   break;
        case 3: _ReportOutf("    Data trace: Data & Addr\n"); break;
      }
      if (CtrlReg & (1 << 20)) {
        _ReportOutf("    Instruction trace disabled\n");
      }
      v = ((CtrlReg >> 4) & 7) | ((CtrlReg >> 18) & (1 << 3));    // Bits are in 21, 6:4;
      switch (v) {
        case  0: _ReportOutf("    Port size: 4-bit\n");         break;
        case  1: _ReportOutf("    Port size: 8-bit\n");         break;
        case  2: _ReportOutf("    Port size: 16-bit\n");         break;
        case  3: _ReportOutf("    Port size: 24-bit\n");         break;
        case  4: _ReportOutf("    Port size: 32-bit\n");         break;
        case  5: _ReportOutf("    Port size: 48-bit\n");         break;
        case  6: _ReportOutf("    Port size: 64-bit\n");         break;
        case  7: _ReportOutf("    Port size: reserved (7)\n");         break;
        case  8: _ReportOutf("    Port size: 1-bit\n");         break;
        case  9: _ReportOutf("    Port size: 2-bit\n");         break;
        case 10: _ReportOutf("    Port size: reserved (10)\n");         break;
        case 11: _ReportOutf("    Port size: reserved (11)\n");         break;
        case 12: _ReportOutf("    Port size: reserved (12)\n");         break;
        case 13: _ReportOutf("    Port size: reserved (13)\n");         break;
        case 14: _ReportOutf("    Port size: User defined 1\n");         break;
        case 15: _ReportOutf("    Port size: User defined 2\n");         break;
      }
      if (CtrlReg & (1 << 7)) {
        _ReportOutf("    Enable FIFOFULL (Stall CPU)\n");
      }
      if (CtrlReg & (1 << 8)) {
        _ReportOutf("    BranchOutput: Output all branch addresses\n");
      }
      if (CtrlReg & (1 << 9)) {
        _ReportOutf("    DebugRequestControl: Halt CPU on trigger\n");
      }
      if (CtrlReg & (1 << 10)) {
        _ReportOutf("    ETM Programming\n");
      }
      if (CtrlReg & (1 << 11)) {
        _ReportOutf("    ETM Port selection (bit 11) = 1:  Trace port pins enabled\n");
      } else {
        _ReportOutf("    ETM Port selection (bit 11) = 0:  Trace port pins disabled\n");
      }
      if (CtrlReg & (1 << 12)) {
        _ReportOutf("    Cycle accurate tracing\n");
      }
      v = ((CtrlReg >> 16) & 3) |  (((CtrlReg >> 13) & 1)  << 2);
      _ReportOutf("    Mode, Clocking: ");
      switch (v) {
        case 0: _ReportOutf("Normal, rising edge\n");   break;
        case 1: _ReportOutf("Multiplexed, both edges\n");   break;
        case 2: _ReportOutf("Demultiplexed, rising edge\n");   break;
        case 3: _ReportOutf("Illegal (3)\n");   break;
        case 4: _ReportOutf("Normal, Half-rate (Data on both clock edges, DDR)\n");   break;
        case 5: _ReportOutf("Illegal (5)\n");   break;
        case 6: _ReportOutf("Demultiplexed, Half-rate (Data on both clock edges, DDR)\n");   break;
        case 7: _ReportOutf("Illegal (7)\n");   break;
      }
      v = (CtrlReg >> 14) & 3;
      switch (v) {
        case 1: _ReportOutf("    ContextId: 8-bit\n");   break;
        case 2: _ReportOutf("    ContextId: 16-bit\n");   break;
        case 3: _ReportOutf("    ContextId: 32-bit\n");   break;
      }
      if (CtrlReg & (1 << 22)) {
        _ReportOutf("    ETM writes by debugger disabled\n");
      }
      if (ETMVersion >= 0x32) {
        if (CtrlReg & (1 << 23)) {
          _ReportOutf("    ETM writes by software disabled\n");
        }
      }
      //
      // Handle status register
      //
      if (ETMVersion >= 0x11) {
        if (StatusReg & (1 << 0)) {
          _ReportOutf("    Status: Overflow\n");
        }
      }
      if (ETMVersion >= 0x12) {
        if (StatusReg & (1 << 1)) {
          _ReportOutf("    Status: Programming\n");
        }
        if (StatusReg & (1 << 2)) {
          _ReportOutf("    Status: Trace start/stop is ON\n");
        } else {
          _ReportOutf("    Status: Trace start/stop is OFF\n");
        }
      }
      _ReportOutf("    Sequencer state: %d (1..3)\n", (JLINKARM_ETM_ReadReg(0x67) & 3) + 1);
    }
  }
}

/*********************************************************************
*
*       _ShowDebugState
*/
static void _ShowDebugState(void) {
  U32 DevFamily;
  DevFamily = JLINKARM_GetDeviceFamily();
  if ((DevFamily == 7) || (DevFamily == 9)) {
    U32 Data;
    Data = JLINKARM_ReadICEReg(0);
    _ReportOutf("DebugControl      (Ice[0x00]) :       %.2X (%s %s %s)\n"
      ,Data
      ,(Data & (1 << 2)) ? "INTDIS" : "      "
      ,(Data & (1 << 1)) ? "DBGRQ " : "      "
      ,(Data & (1 << 0)) ? "DBGACK" : "      "
      );
    Data = JLINKARM_ReadICEReg(1);
    _ReportOutf("DebugStatus       (Ice[0x01]) :       %.2X (%s %s %s %s %s)\n"
      ,Data
      ,(Data & (1 << 4)) ? "THUMB " : "ARM   "
      ,(Data & (1 << 3)) ? "nMREQ " : "      "
      ,(Data & (1 << 2)) ? "IFEN  " : "      "
      ,(Data & (1 << 1)) ? "DBGRQ " : "      "
      ,(Data & (1 << 0)) ? "DBGACK" : "      "
      );
    Data = JLINKARM_ReadICEReg(4);
    _ReportOutf("DCC Control       (Ice[0x04]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(5);
    _ReportOutf("DCC Data          (Ice[0x05]) : %.8X\n", Data);

    Data = JLINKARM_ReadICEReg(8);
    _ReportOutf("WP0 Addr          (Ice[0x08]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(9);
    _ReportOutf("WP0 Addr Mask     (Ice[0x09]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0xa);
    _ReportOutf("WP0 Data          (Ice[0x0a]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0xb);
    _ReportOutf("WP0 Data Mask     (Ice[0x0b]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0xc);
    _ReportOutf("WP0 Control       (Ice[0x0c]) :      %.3X\n", Data & ((1<<9) - 1));
    Data = JLINKARM_ReadICEReg(0xd);
    _ReportOutf("WP0 Control Mask  (Ice[0x0d]) :       %.2X\n", Data & ((1<<8) - 1));

    Data = JLINKARM_ReadICEReg(0x10);
    _ReportOutf("WP1 Addr          (Ice[0x10]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0x11);
    _ReportOutf("WP1 Addr Mask     (Ice[0x11]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0x12);
    _ReportOutf("WP1 Data          (Ice[0x12]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0x13);
    _ReportOutf("WP1 Data Mask     (Ice[0x13]) : %.8X\n", Data);
    Data = JLINKARM_ReadICEReg(0x14);
    _ReportOutf("WP1 Control       (Ice[0x14]) :      %.3X\n", Data & ((1<<9) - 1));
    Data = JLINKARM_ReadICEReg(0x15);
    _ReportOutf("WP1 Control Mask  (Ice[0x15]) :       %.2X\n", Data  & ((1<<8) - 1));
  }
}

/*********************************************************************
*
*       _ShowETBState
*/
static void _ShowETBState(void) {
  if (JLINKARM_ETB_IsPresent()) {
    U32 Data;
    _ReportOutf("ETB is present.\n");
    Data = JLINKARM_ETB_ReadReg(0);
    _ReportOutf("ID register       (ETB[0x00]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(1);
    _ReportOutf("RAM depth         (ETB[0x01]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(2);
    _ReportOutf("RAM width         (ETB[0x02]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(3);
    _ReportOutf("Status            (ETB[0x03]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(4);
    _ReportOutf("RAM data          (ETB[0x04]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(5);
    _ReportOutf("RAM read pointer  (ETB[0x05]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(6);
    _ReportOutf("RAM write pointer (ETB[0x06]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(7);
    _ReportOutf("Trigger counter   (ETB[0x07]) : %.8X\n", Data);
    Data = JLINKARM_ETB_ReadReg(8);
    _ReportOutf("Control           (ETB[0x08]) : %.8X\n", Data);
  } else {
    _ReportOutf("ETB is not present.\n");
  }
}

/*********************************************************************
*
*       _ExecStateIsAArch32
*
*  Function description
*    Determines the current execution state of ARMv8-AR cores
*
*  Return value
*    0   Aarch64
*    1   AArch32

*/
static int _CortexARMV8AR_IsAArch32(void) {
  U32 RegIndex;
  U64 RegData;
  
  RegIndex = JLINK_ARM_V8AR_REG_CPSR;
  JLINK_ReadRegs_64(&RegIndex, &RegData, NULL, 1);
  if (((RegData >> 4) & 1) == 0) {
    return 0;
  } else {
    return 1;
  }
}

/*********************************************************************
*
*       _GetNumRegs
*
*  Return value
*    != NULL   Pointer to an alternate register index set
*       NULL   O.K., use the default register set (from the CPU modules)
*/
static const U32* _GetNumRegs(U32 DevFamily, int* NumRegs) {
  U32* paAlternateRegIndex;

  paAlternateRegIndex = NULL;
  switch (DevFamily) {
  case JLINKARM_DEV_FAMILY_CM0:          *NumRegs = JLINKARM_CM3_NUM_REGS;       break;
  case JLINKARM_DEV_FAMILY_CM1:          *NumRegs = JLINKARM_CM3_NUM_REGS;       break;
  case JLINKARM_DEV_FAMILY_CM3:          *NumRegs = JLINKARM_CM3_NUM_REGS;       break;
  case JLINKARM_DEV_FAMILY_CM4:          *NumRegs = JLINKARM_CM4_NUM_REGS;       break;
  case JLINKARM_DEV_FAMILY_CORTEX_R4:    *NumRegs = JLINKARM_CORTEX_R4_NUM_REGS; break;
  case JLINKARM_DEV_FAMILY_RX:           *NumRegs = JLINKARM_RX_NUM_REGS;        break;
  case JLINKARM_DEV_FAMILY_POWER_PC:     *NumRegs = JLINK_POWER_PC_NUM_REGS;     break;
  case JLINK_DEV_FAMILY_MIPS:            *NumRegs = JLINK_MIPS_NUM_REGS;         break;
  case JLINK_DEV_FAMILY_RISC_V:          *NumRegs = JLINK_RISCV_NUM_REGS;        break;
  case JLINK_DEV_FAMILY_CORTEX_AR_ARMV8: 
    if (_CortexARMV8AR_IsAArch32() == 0) {
      *NumRegs = JLINK_ARM_V8AR_NUM_REGS;
    } else {
      *NumRegs = COUNTOF(_aRegARMv8ARAArch32);
      paAlternateRegIndex = &_aRegARMv8ARAArch32[0];
    }
    break;
  default:                               *NumRegs = ARM_NUM_REGS;                break;
  }
  return paAlternateRegIndex;
}

/*********************************************************************
*
*       _PrintAPSR
*/
static void _PrintAPSR(U8* pBuffer, int BufferSize, U32 Value) {
  U8 ac[16];
  int Tmp;
  int i;

  i = 0;
  ac[i++] = ((Value >> 31) & 1) ? 'N' : 'n';
  ac[i++] = ((Value >> 30) & 1) ? 'Z' : 'z';
  ac[i++] = ((Value >> 29) & 1) ? 'C' : 'c';
  ac[i++] = ((Value >> 28) & 1) ? 'V' : 'v';
  ac[i++] = ((Value >> 27) & 1) ? 'Q' : 'q';
  ac[i] = '\0';
  Tmp = MIN(i, BufferSize - 1);
  memcpy(pBuffer, ac, Tmp);
  *(pBuffer + Tmp) = '\0';
}

/*********************************************************************
*
*       _PrintIPSR
*/
static void _PrintIPSR(U8* pBuffer, int BufferSize, U32 Value) {
  Value &= 0x1F;
  UTIL_snprintf((char*)pBuffer, BufferSize, "%.3llX (%s)", (U64)Value, _aIPSR2Str[Value]);
}

/*********************************************************************
*
*       _IsValidCPUMode_ARM79
*/
static int _IsValidCPUMode_ARM79(U32 Mode) {
  if (
      (Mode == 0x10) ||               // User mode
      (Mode == 0x11) ||               // FIQ mode
      (Mode == 0x12) ||               // IRQ mode
      (Mode == 0x13) ||               // Supervisor mode
      (Mode == 0x17) ||               // Abort mode
      (Mode == 0x1B) ||               // Undefined mode
      (Mode == 0x1F)                  // System mode
     )
  {
    return 1;
  }
  return 0;
}

/*********************************************************************
*
*       _ShowRegsARM_v6M_v7M
*
*  Function description
*    Shows CPU registers for ARM Cortex-M cores based on ARMv6-M / v7-M architecture.
*/
static void _ShowRegsARM_v6M_v7M(void) {
  int r;
  U32 HasFPU;
  U32 aRegIndex[28] = {
    JLINKARM_CM3_REG_R0,  JLINKARM_CM3_REG_R1,  JLINKARM_CM3_REG_R2,  JLINKARM_CM3_REG_R3,                                            // 0-3
    JLINKARM_CM3_REG_R4,  JLINKARM_CM3_REG_R5,  JLINKARM_CM3_REG_R6,  JLINKARM_CM3_REG_R7,                                            // 4-7
    JLINKARM_CM3_REG_R8,  JLINKARM_CM3_REG_R9,  JLINKARM_CM3_REG_R10, JLINKARM_CM3_REG_R11,                                           // 8-11
    JLINKARM_CM3_REG_R12, JLINKARM_CM3_REG_R13, JLINKARM_CM3_REG_MSP, JLINKARM_CM3_REG_PSP,                                           // 12-15
    JLINKARM_CM3_REG_R14, JLINKARM_CM3_REG_R15,                                                                                       // 16-17
    JLINKARM_CM3_REG_XPSR, JLINKARM_CM3_REG_APSR, JLINKARM_CM3_REG_EPSR, JLINKARM_CM3_REG_IPSR,                                       // 18-21
    JLINKARM_CM3_REG_CFBP, JLINKARM_CM3_REG_CONTROL, JLINKARM_CM3_REG_FAULTMASK, JLINKARM_CM3_REG_BASEPRI, JLINKARM_CM3_REG_PRIMASK,  // 22-26
    JLINKARM_CM3_REG_DWT_CYCCNT                                                                                                       // 27
  };
  U32 aRegIndexFPU[33] = {
    JLINKARM_CM4_REG_FPS0,  JLINKARM_CM4_REG_FPS1,  JLINKARM_CM4_REG_FPS2,  JLINKARM_CM4_REG_FPS3,
    JLINKARM_CM4_REG_FPS4,  JLINKARM_CM4_REG_FPS5,  JLINKARM_CM4_REG_FPS6,  JLINKARM_CM4_REG_FPS7,
    JLINKARM_CM4_REG_FPS8,  JLINKARM_CM4_REG_FPS9,  JLINKARM_CM4_REG_FPS10, JLINKARM_CM4_REG_FPS11,
    JLINKARM_CM4_REG_FPS12, JLINKARM_CM4_REG_FPS13, JLINKARM_CM4_REG_FPS14, JLINKARM_CM4_REG_FPS15,
    JLINKARM_CM4_REG_FPS16, JLINKARM_CM4_REG_FPS17, JLINKARM_CM4_REG_FPS18, JLINKARM_CM4_REG_FPS19,
    JLINKARM_CM4_REG_FPS20, JLINKARM_CM4_REG_FPS21, JLINKARM_CM4_REG_FPS22, JLINKARM_CM4_REG_FPS23,
    JLINKARM_CM4_REG_FPS24, JLINKARM_CM4_REG_FPS25, JLINKARM_CM4_REG_FPS26, JLINKARM_CM4_REG_FPS27,
    JLINKARM_CM4_REG_FPS28, JLINKARM_CM4_REG_FPS29, JLINKARM_CM4_REG_FPS30, JLINKARM_CM4_REG_FPS31,
    JLINKARM_CM4_REG_FPSCR
  };
  U32 aRegData[33];
  U8 acAPSR[64];
  U8 acIPSR[64];
  U32 cpacr;
  //
  // Check if the connected device provides an FPU
  //
  r = JLINKARM_GetDebugInfo(JLINKARM_DEBUG_INFO_HAS_FPU_INDEX, &HasFPU);
  if (r < 0) {
    JLINKARM_ReadMemU32(0xE000ED88, 1, &cpacr, NULL);   // Coprocessor Access Control Register
    if (((cpacr >> 20) & 0xF) == 0xF) {
      HasFPU = 1;
    }
  }
  //
  // Print regs contents
  //
  JLINKARM_ReadRegs(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
  _ReportOutf("PC = %.8X, CycleCnt = %.8X\n", aRegData[17], aRegData[27]);
  _ReportOutf("R0 = %.8X, R1 = %.8X, R2 = %.8X, R3 = %.8X\n", aRegData[0],  aRegData[1],  aRegData[2],  aRegData[3]);
  _ReportOutf("R4 = %.8X, R5 = %.8X, R6 = %.8X, R7 = %.8X\n", aRegData[4],  aRegData[5],  aRegData[6],  aRegData[7]);
  _ReportOutf("R8 = %.8X, R9 = %.8X, R10= %.8X, R11= %.8X\n", aRegData[8],  aRegData[9],  aRegData[10], aRegData[11]);
  _ReportOutf("R12= %.8X\n", aRegData[12]);
  _ReportOutf("SP(R13)= %.8X, MSP= %.8X, PSP= %.8X, R14(LR) = %.8X\n", aRegData[13], aRegData[14], aRegData[15], aRegData[16]);
  _PrintAPSR(acAPSR, sizeof(acAPSR), aRegData[19]);
  _PrintIPSR(acIPSR, sizeof(acIPSR), aRegData[21]);
  _ReportOutf("XPSR = %.8X: APSR = %s, EPSR = %.8X, IPSR = %s\n", aRegData[18], acAPSR, aRegData[20], acIPSR);
  _ReportOutf("CFBP = %.8X, CONTROL = %.2X, FAULTMASK = %.2X, BASEPRI = %.2X, PRIMASK = %.2X\n", aRegData[22], aRegData[23] >> 24, aRegData[24] >> 16, aRegData[25] >> 8, aRegData[26] >> 0);
  if (HasFPU) {
    JLINKARM_ReadRegs(&aRegIndexFPU[0], &aRegData[0], NULL, COUNTOF(aRegIndexFPU));
    _ReportOutf("\n");
    _ReportOutf("FPS0 = %.8X, FPS1 = %.8X, FPS2 = %.8X, FPS3 = %.8X\n", aRegData[0],  aRegData[1],  aRegData[2],  aRegData[3]);
    _ReportOutf("FPS4 = %.8X, FPS5 = %.8X, FPS6 = %.8X, FPS7 = %.8X\n", aRegData[4],  aRegData[5],  aRegData[6],  aRegData[7]);
    _ReportOutf("FPS8 = %.8X, FPS9 = %.8X, FPS10= %.8X, FPS11= %.8X\n", aRegData[8],  aRegData[9],  aRegData[10], aRegData[11]);
    _ReportOutf("FPS12= %.8X, FPS13= %.8X, FPS14= %.8X, FPS15= %.8X\n", aRegData[12], aRegData[13], aRegData[14], aRegData[15]);
    _ReportOutf("FPS16= %.8X, FPS17= %.8X, FPS18= %.8X, FPS19= %.8X\n", aRegData[16], aRegData[17], aRegData[18], aRegData[19]);
    _ReportOutf("FPS20= %.8X, FPS21= %.8X, FPS22= %.8X, FPS23= %.8X\n", aRegData[20], aRegData[21], aRegData[22], aRegData[23]);
    _ReportOutf("FPS24= %.8X, FPS25= %.8X, FPS26= %.8X, FPS27= %.8X\n", aRegData[24], aRegData[25], aRegData[26], aRegData[27]);
    _ReportOutf("FPS28= %.8X, FPS29= %.8X, FPS30= %.8X, FPS31= %.8X\n", aRegData[28], aRegData[29], aRegData[30], aRegData[31]);
    _ReportOutf("FPSCR= %.8X\n", aRegData[32]);
  } else {
    _ReportOutf("FPU regs: FPU not enabled / not implemented on connected CPU.\n");
  }
}

/*********************************************************************
*
*       _ShowRegsARM_v8M
*
*  Function description
*    Shows CPU registers for ARM Cortex-M cores based on ARMv8-M architecture.
*/
static void _ShowRegsARM_v8M(void) {
  int r;
  U32 HasSecurityExt;
  U32 HasFPU;
  U32 aRegIndex[28] = {
    JLINKARM_CM3_REG_R0,  JLINKARM_CM3_REG_R1,  JLINKARM_CM3_REG_R2,  JLINKARM_CM3_REG_R3,                                            // 0-3
    JLINKARM_CM3_REG_R4,  JLINKARM_CM3_REG_R5,  JLINKARM_CM3_REG_R6,  JLINKARM_CM3_REG_R7,                                            // 4-7
    JLINKARM_CM3_REG_R8,  JLINKARM_CM3_REG_R9,  JLINKARM_CM3_REG_R10, JLINKARM_CM3_REG_R11,                                           // 8-11
    JLINKARM_CM3_REG_R12, JLINKARM_CM3_REG_R13, JLINKARM_CM3_REG_MSP, JLINKARM_CM3_REG_PSP,                                           // 12-15
    JLINKARM_CM3_REG_R14, JLINKARM_CM3_REG_R15,                                                                                       // 16-17
    JLINKARM_CM3_REG_XPSR, JLINKARM_CM3_REG_APSR, JLINKARM_CM3_REG_EPSR, JLINKARM_CM3_REG_IPSR,                                       // 18-21
    JLINKARM_CM3_REG_CFBP, JLINKARM_CM3_REG_CONTROL, JLINKARM_CM3_REG_FAULTMASK, JLINKARM_CM3_REG_BASEPRI, JLINKARM_CM3_REG_PRIMASK,  // 22-26
    JLINKARM_CM3_REG_DWT_CYCCNT                                                                                                       // 27
  };
  U32 aRegIndexFPU[33] = {
    JLINKARM_CM4_REG_FPS0,  JLINKARM_CM4_REG_FPS1,  JLINKARM_CM4_REG_FPS2,  JLINKARM_CM4_REG_FPS3,
    JLINKARM_CM4_REG_FPS4,  JLINKARM_CM4_REG_FPS5,  JLINKARM_CM4_REG_FPS6,  JLINKARM_CM4_REG_FPS7,
    JLINKARM_CM4_REG_FPS8,  JLINKARM_CM4_REG_FPS9,  JLINKARM_CM4_REG_FPS10, JLINKARM_CM4_REG_FPS11,
    JLINKARM_CM4_REG_FPS12, JLINKARM_CM4_REG_FPS13, JLINKARM_CM4_REG_FPS14, JLINKARM_CM4_REG_FPS15,
    JLINKARM_CM4_REG_FPS16, JLINKARM_CM4_REG_FPS17, JLINKARM_CM4_REG_FPS18, JLINKARM_CM4_REG_FPS19,
    JLINKARM_CM4_REG_FPS20, JLINKARM_CM4_REG_FPS21, JLINKARM_CM4_REG_FPS22, JLINKARM_CM4_REG_FPS23,
    JLINKARM_CM4_REG_FPS24, JLINKARM_CM4_REG_FPS25, JLINKARM_CM4_REG_FPS26, JLINKARM_CM4_REG_FPS27,
    JLINKARM_CM4_REG_FPS28, JLINKARM_CM4_REG_FPS29, JLINKARM_CM4_REG_FPS30, JLINKARM_CM4_REG_FPS31,
    JLINKARM_CM4_REG_FPSCR
  };
  U32 aRegIndexSec[18] = { JLINKARM_CM3_REG_MSP_S,     JLINKARM_CM3_REG_MSPLIM_S,     JLINKARM_CM3_REG_PSP_S,      JLINKARM_CM3_REG_PSPLIM_S,   // 0-3
                           JLINKARM_CM3_REG_MSP_NS,    JLINKARM_CM3_REG_MSPLIM_NS,    JLINKARM_CM3_REG_PSP_NS,     JLINKARM_CM3_REG_PSPLIM_NS,  // 4-7
                           JLINKARM_CM3_REG_CONTROL_S, JLINKARM_CM3_REG_FAULTMASK_S,  JLINKARM_CM3_REG_BASEPRI_S,  JLINKARM_CM3_REG_PRIMASK_S,  // 8-11
                           JLINKARM_CM3_REG_CONTROL_NS,JLINKARM_CM3_REG_FAULTMASK_NS, JLINKARM_CM3_REG_BASEPRI_NS, JLINKARM_CM3_REG_PRIMASK_NS, // 12-15
                           JLINKARM_CM3_REG_MSPLIM,    JLINKARM_CM3_REG_PSPLIM                                                                  // 16-17
                         };
  U32 aRegData[33];
  U8 acAPSR[64];
  U8 acIPSR[64];
  U32 cpacr;
  //
  // Check if the connected device provides an FPU
  //
  r = JLINKARM_GetDebugInfo(JLINKARM_DEBUG_INFO_HAS_FPU_INDEX, &HasFPU);
  if (r < 0) {
    JLINKARM_ReadMemU32(0xE000ED88, 1, &cpacr, NULL);   // Coprocessor Access Control Register
    if (((cpacr >> 20) & 0xF) == 0xF) {
      HasFPU = 1;
    }
  }
  //
  // ARMv8M mainline also specifies secure mode, so we have additional secure / non-secure versions of some registers
  //
  HasSecurityExt = 0;
  JLINKARM_GetDebugInfo(JLINKARM_DEBUG_INFO_HAS_CORTEX_M_SECURITY_EXT_INDEX, &HasSecurityExt);
  //
  // Print regs contents
  //
  JLINKARM_ReadRegs(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
  _ReportOutf("PC = %.8X, CycleCnt = %.8X\n", aRegData[17], aRegData[27]);
  _ReportOutf("R0 = %.8X, R1 = %.8X, R2 = %.8X, R3 = %.8X\n", aRegData[0],  aRegData[1],  aRegData[2],  aRegData[3]);
  _ReportOutf("R4 = %.8X, R5 = %.8X, R6 = %.8X, R7 = %.8X\n", aRegData[4],  aRegData[5],  aRegData[6],  aRegData[7]);
  _ReportOutf("R8 = %.8X, R9 = %.8X, R10= %.8X, R11= %.8X\n", aRegData[8],  aRegData[9],  aRegData[10], aRegData[11]);
  _ReportOutf("R12= %.8X\n", aRegData[12]);
  _ReportOutf("SP(R13)= %.8X, MSP= %.8X, PSP= %.8X, R14(LR) = %.8X\n", aRegData[13], aRegData[14], aRegData[15], aRegData[16]);
  _PrintAPSR(acAPSR, sizeof(acAPSR), aRegData[19]);
  _PrintIPSR(acIPSR, sizeof(acIPSR), aRegData[21]);
  _ReportOutf("XPSR = %.8X: APSR = %s, EPSR = %.8X, IPSR = %s\n", aRegData[18], acAPSR, aRegData[20], acIPSR);
  _ReportOutf("CFBP = %.8X, CONTROL = %.2X, FAULTMASK = %.2X, BASEPRI = %.2X, PRIMASK = %.2X\n", aRegData[22], aRegData[23] >> 24, aRegData[24] >> 16, aRegData[25] >> 8, aRegData[26] >> 0);
  //
  // MSPLIM and PSPLIM are always implemented for v8M. The _S and _NS variants are only there if the security extensions are implemented.
  //
  JLINKARM_ReadRegs(&aRegIndexSec[0], &aRegData[0], NULL, COUNTOF(aRegIndexSec));   // Does not hurt to always try to read them, as the J-Link DLL will return dummy values in case a register does not exist on the current implementation.
  _ReportOutf("MSPLIM = %.8X\n", aRegData[16]);
  _ReportOutf("PSPLIM = %.8X\n", aRegData[17]);
  if (HasSecurityExt) {
    _ReportOut("\n");
    _ReportOut("Security extension regs:\n");
    _ReportOutf("MSP_S = %.8X, MSP_NS = %.8X\n",                                                  aRegData[0], aRegData[4]);
    _ReportOutf("MSPLIM_S = %.8X, MSPLIM_NS = %.8X\n",                                            aRegData[1], aRegData[5]);
    _ReportOutf("PSP_S = %.8X, PSP_NS = %.8X\n",                                                  aRegData[2], aRegData[6]);
    _ReportOutf("PSPLIM_S = %.8X, PSPLIM_NS = %.8X\n",                                            aRegData[3], aRegData[7]);
    _ReportOutf("CONTROL_S  = %.2X, FAULTMASK_S  = %.2X, BASEPRI_S  = %.2X, PRIMASK_S  = %.2X\n", aRegData[8]  >> 24,  aRegData[9]  >> 16, aRegData[10] >> 8, aRegData[11] >> 0);
    _ReportOutf("CONTROL_NS = %.2X, FAULTMASK_NS = %.2X, BASEPRI_NS = %.2X, PRIMASK_NS = %.2X\n", aRegData[12] >> 24,  aRegData[13] >> 16, aRegData[14] >> 8, aRegData[15] >> 0);
  }
  if (HasFPU) {
    JLINKARM_ReadRegs(&aRegIndexFPU[0], &aRegData[0], NULL, COUNTOF(aRegIndexFPU));
    _ReportOutf("\n");
    _ReportOutf("FPS0 = %.8X, FPS1 = %.8X, FPS2 = %.8X, FPS3 = %.8X\n", aRegData[0],  aRegData[1],  aRegData[2],  aRegData[3]);
    _ReportOutf("FPS4 = %.8X, FPS5 = %.8X, FPS6 = %.8X, FPS7 = %.8X\n", aRegData[4],  aRegData[5],  aRegData[6],  aRegData[7]);
    _ReportOutf("FPS8 = %.8X, FPS9 = %.8X, FPS10= %.8X, FPS11= %.8X\n", aRegData[8],  aRegData[9],  aRegData[10], aRegData[11]);
    _ReportOutf("FPS12= %.8X, FPS13= %.8X, FPS14= %.8X, FPS15= %.8X\n", aRegData[12], aRegData[13], aRegData[14], aRegData[15]);
    _ReportOutf("FPS16= %.8X, FPS17= %.8X, FPS18= %.8X, FPS19= %.8X\n", aRegData[16], aRegData[17], aRegData[18], aRegData[19]);
    _ReportOutf("FPS20= %.8X, FPS21= %.8X, FPS22= %.8X, FPS23= %.8X\n", aRegData[20], aRegData[21], aRegData[22], aRegData[23]);
    _ReportOutf("FPS24= %.8X, FPS25= %.8X, FPS26= %.8X, FPS27= %.8X\n", aRegData[24], aRegData[25], aRegData[26], aRegData[27]);
    _ReportOutf("FPS28= %.8X, FPS29= %.8X, FPS30= %.8X, FPS31= %.8X\n", aRegData[28], aRegData[29], aRegData[30], aRegData[31]);
    _ReportOutf("FPSCR= %.8X\n", aRegData[32]);
  } else {
    _ReportOutf("FPU regs: FPU not enabled / not implemented on connected CPU.\n");
  }
}

/*********************************************************************
*
*       _ShowRegsARMv8AR
*
*  Function description
*    Shows CPU registers for ARMv8AR based devices
*/
static void _ShowRegsARMv8AR(void) {
  const char* sIMasked;
  const char* sFMasked;
  const char* sInstSet;
  const char* sEndian;
  const char* sMode;
  U32 aRegIndex[50] = {
                      JLINK_ARM_V8AR_REG_R0, JLINK_ARM_V8AR_REG_R1, JLINK_ARM_V8AR_REG_R2, JLINK_ARM_V8AR_REG_R3,                         // 0
                      JLINK_ARM_V8AR_REG_R4, JLINK_ARM_V8AR_REG_R5, JLINK_ARM_V8AR_REG_R6, JLINK_ARM_V8AR_REG_R7,                         // 4
                      JLINK_ARM_V8AR_REG_R8, JLINK_ARM_V8AR_REG_R9, JLINK_ARM_V8AR_REG_R10, JLINK_ARM_V8AR_REG_R11,                       // 8
                      JLINK_ARM_V8AR_REG_R12, JLINK_ARM_V8AR_REG_R13, JLINK_ARM_V8AR_REG_R14, JLINK_ARM_V8AR_REG_R15,                     // 12
                      JLINK_ARM_V8AR_REG_R16, JLINK_ARM_V8AR_REG_R17, JLINK_ARM_V8AR_REG_R18, JLINK_ARM_V8AR_REG_R19,                     // 16
                      JLINK_ARM_V8AR_REG_R20, JLINK_ARM_V8AR_REG_R21, JLINK_ARM_V8AR_REG_R22, JLINK_ARM_V8AR_REG_R23,                     // 20
                      JLINK_ARM_V8AR_REG_R24, JLINK_ARM_V8AR_REG_R25, JLINK_ARM_V8AR_REG_R26, JLINK_ARM_V8AR_REG_R27,                     // 24
                      JLINK_ARM_V8AR_REG_R28, JLINK_ARM_V8AR_REG_R29, JLINK_ARM_V8AR_REG_R30, JLINK_ARM_V8AR_REG_R31,                     // 28
                      JLINK_ARM_V8AR_REG_SP, JLINK_ARM_V8AR_REG_PC, JLINK_ARM_V8AR_REG_CPSR, JLINK_ARM_V8AR_REG_ELR_EL1,                  // 32
                      JLINK_ARM_V8AR_REG_ELR_EL2, JLINK_ARM_V8AR_REG_ELR_EL3, JLINK_ARM_V8AR_REG_SP_EL0, JLINK_ARM_V8AR_REG_SP_EL1,       // 36
                      JLINK_ARM_V8AR_REG_SP_EL2, JLINK_ARM_V8AR_REG_SP_EL3, JLINK_ARM_V8AR_REG_SPSR_abt, JLINK_ARM_V8AR_REG_SPSR_EL1,     // 40
                      JLINK_ARM_V8AR_REG_SPSR_EL2, JLINK_ARM_V8AR_REG_SPSR_EL3, JLINK_ARM_V8AR_REG_SPSR_fiq, JLINK_ARM_V8AR_REG_SPSR_irq, // 44
                      JLINK_ARM_V8AR_REG_SPSR_und, JLINK_ARM_V8AR_REG_JMISC                                                               // 48
                    };
  U32 aRegIndexAArch32[] = {                                                                                                                             // If changing anything here, also change _CPU_REGS_ARM_V8AR_AARCH32
    JLINK_ARM_V8AR_REG_AARCH32_R0,   JLINK_ARM_V8AR_REG_AARCH32_R1,  JLINK_ARM_V8AR_REG_AARCH32_R2,   JLINK_ARM_V8AR_REG_AARCH32_R3,                     // 0-3
    JLINK_ARM_V8AR_REG_AARCH32_R4,   JLINK_ARM_V8AR_REG_AARCH32_R5,  JLINK_ARM_V8AR_REG_AARCH32_R6,   JLINK_ARM_V8AR_REG_AARCH32_R7,                     // 4-7
    JLINK_ARM_V8AR_REG_AARCH32_R8,   JLINK_ARM_V8AR_REG_AARCH32_R9,  JLINK_ARM_V8AR_REG_AARCH32_R10,  JLINK_ARM_V8AR_REG_AARCH32_R11,                    // 8-11
    JLINK_ARM_V8AR_REG_AARCH32_R12,  JLINK_ARM_V8AR_REG_AARCH32_R13, JLINK_ARM_V8AR_REG_AARCH32_R14,  JLINK_ARM_V8AR_REG_AARCH32_PC,                     // 12-15
    JLINK_ARM_V8AR_REG_AARCH32_SPSR_ABT, JLINK_ARM_V8AR_REG_AARCH32_SPSR_SVC, JLINK_ARM_V8AR_REG_AARCH32_SPSR_HYP, JLINK_ARM_V8AR_REG_AARCH32_SPSR_FIQ,  // 16-19
    JLINK_ARM_V8AR_REG_AARCH32_SPSR_IRQ, JLINK_ARM_V8AR_REG_AARCH32_SPSR_UND                                                                             // 20-23
  };
  U64 aRegData[50];
  U32 RegIndex;
  U64 RegData;
  char acRegName0[32];
  char acRegName1[32];
  _CPU_REGS_ARM_V8AR_AARCH32* pCPURegsAArch32;

  int i;
  //
  // AArch64:
  // CPSR[9], D: 1 == Watchpoint, Breakpoint, and Software Step exceptions targeted at the current Exception level are masked
  // CPSR[8], A: 1 == SError masked
  // CPSR[7], I: 1 == Interrupts masked
  // CPSR[6], F: 1 == FIQ masked
  // CPSR[4], M[4]: 0 == AArch64 mode
  // CPSR[3:0], M[3:0]: 0b0000  EL0t
  //                    0b0100  EL1t
  //                    0b0101  EL1h
  //                    0b1000  EL2t
  //                    0b1001  EL2h
  //                    0b1100  EL3t
  //                    0b1101  EL3h
  //
  // AArch32:
  // CPSR[9], E: 0/1 == Little/Big endian
  // CPSR[8], A: 1 == SError masked
  // CPSR[7], I: 1 == Interrupts masked
  // CPSR[6], F: 1 == FIQ masked
  // CPSR[5], T: 0/1 == A32/T32 state
  // CPSR[4], M[4]: 1 == AArch32 mode
  // CPSR[3:0], M[3:0]: 0b0000  User mode
  //                    0b0001  FIQ mode
  //                    0b0010  IRQ mode
  //                    0b0011  Supervisor mode
  //                    0b0110  Monitor mode
  //                    0b0111  Abort mode
  //                    0b1010  Hyp mode
  //                    0b1011  Undefined mode
  //                    0b1111  System mode
  //
  RegIndex = JLINK_ARM_V8AR_REG_CPSR;
  JLINK_ReadRegs_64(&RegIndex, &RegData, NULL, 1);
  if (((RegData >> 4) & 1) == 0) {
    //
    // AArch64 mode
    //
    JLINK_ReadRegs_64(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
    _ReportOutf("PC = %.8X%.8X\n", (U32)(aRegData[33] >> 32), (U32)(aRegData[33] >> 0));
    sIMasked = ((RegData >> 7) & 1) ? "IRQ masked" : "IRQ enabled";
    sFMasked = ((RegData >> 6) & 1) ? "FIQ masked" : "FIQ enabled";
    sMode    = _asModeARMv8A_AArch64[RegData & 0xF];
    _ReportOutf("CPSR = %.8X (AArch64, %s mode, %s, %s)\n", (U32)RegData, sMode, sIMasked, sFMasked);
    _ReportOutf("SP = %.8X%.8X\n", (U32)(aRegData[32] >> 32), (U32)(aRegData[32] >> 0));
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[35]);
    _ReportOutf("ELR_EL1 = %s ", acRegName0);
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[36]);
    _ReportOutf("ELR_EL2 = %s\n", acRegName0);
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[37]);
    _ReportOutf("ELR_EL3 = %s\n", acRegName0);
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[38]);
    _ReportOutf("SP_EL0 = %s ", acRegName0);
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[39]);
    _ReportOutf("SP_EL1 = %s\n", acRegName0);
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[40]);
    _ReportOutf("SP_EL2 = %s ", acRegName0);
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[41]);
    _ReportOutf("SP_EL3 = %s\n", acRegName0);
    for (i = 0; i < 32; i += 2) {
      _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[i]);
      _PrintHex64(acRegName1, (int)sizeof(acRegName1), aRegData[i + 1]);
      _ReportOutf("R%d = %s, R%d = %s\n", i, acRegName0, i + 1, acRegName1);
    }
    _PrintHex64(acRegName0, (int)sizeof(acRegName0), aRegData[49]);
    _ReportOutf("JMISC = %s\n", acRegName0);
  } else {
    //
    // AArch32 mode
    //
    JLINK_ReadRegs_64(&aRegIndexAArch32[0], &aRegData[0], NULL, COUNTOF(aRegIndexAArch32));
    pCPURegsAArch32 = (_CPU_REGS_ARM_V8AR_AARCH32*)&aRegData[0];
    sIMasked = ((RegData >> 7) & 1) ? "IRQ masked" : "IRQ enabled";
    sFMasked = ((RegData >> 6) & 1) ? "FIQ masked" : "FIQ enabled";
    sInstSet = ((RegData >> 5) & 1) ? "T32" : "A32";
    sEndian  = ((RegData >> 9) & 1) ? "Big endian" : "Little endian";;
    sMode = _asModeARMv8A_AArch32[RegData & 0xF];
    _ReportOutf("PC = %.8X\n", (U32)pCPURegsAArch32->PC);
    _ReportOutf("CPSR = %.8X (AArch32, %s mode, %s, %s, %s, %s)\n", (U32)RegData, sMode, sIMasked, sFMasked, sInstSet, sEndian);
    _ReportOutf("SP (R13) = %.8X\n", (U32)pCPURegsAArch32->SP);
    _ReportOutf("LR (R14) = %.8X\n", (U32)pCPURegsAArch32->LR);
    _ReportOutf("R0  = %.8X R1  = %.8X R2  = %.8X  R3 = %.8X\n", (U32)pCPURegsAArch32->R0,  (U32)pCPURegsAArch32->R1,  (U32)pCPURegsAArch32->R2,  (U32)pCPURegsAArch32->R3);
    _ReportOutf("R4  = %.8X R5  = %.8X R6  = %.8X  R7 = %.8X\n", (U32)pCPURegsAArch32->R4,  (U32)pCPURegsAArch32->R5,  (U32)pCPURegsAArch32->R6,  (U32)pCPURegsAArch32->R7);
    _ReportOutf("R8  = %.8X R9  = %.8X R10 = %.8X R11 = %.8X\n", (U32)pCPURegsAArch32->R8,  (U32)pCPURegsAArch32->R9,  (U32)pCPURegsAArch32->R10, (U32)pCPURegsAArch32->R11);
    _ReportOutf("R12 = %.8X\n", (U32)pCPURegsAArch32->R12);
    _ReportOutf("SPSR_ABT = %.8X SPSR_SVC = %.8X SPSR_HYP = %.8X\n", (U32)pCPURegsAArch32->SPSR_ABT, (U32)pCPURegsAArch32->SPSR_SVC, (U32)pCPURegsAArch32->SPSR_HYP);
    _ReportOutf("SPSR_FIQ = %.8X SPSR_IRQ = %.8X SPSR_UND = %.8X\n", (U32)pCPURegsAArch32->SPSR_FIQ, (U32)pCPURegsAArch32->SPSR_IRQ, (U32)pCPURegsAArch32->SPSR_UND);
  }
}

/*********************************************************************
*
*       _ShowRegsBT5511
*
*  Function description
*    Shows CPU registers for Microchip BT5511 8051 based devices
*    This is basically an enhanced 8051 core with additional registers and bits
*/
static void _ShowRegsBT5511(void) {
  U32 aRegIndex[17] = {
                      JLINK_8051_REG_PC, JLINK_8051_REG_SP, JLINK_8051_REG_PSW, JLINK_8051_REG_A,             // 0
                      JLINK_8051_REG_B, JLINK_8051_REG_DPTR, JLINK_BT5511_REG_DPTR1, JLINK_BT5511_REG_DPS,    // 4
                      JLINK_8051_REG_R0, JLINK_8051_REG_R1, JLINK_8051_REG_R2, JLINK_8051_REG_R3,             // 8
                      JLINK_8051_REG_R4, JLINK_8051_REG_R5, JLINK_8051_REG_R6, JLINK_8051_REG_R7,             // 12
                      JLINK_BT5511_REG_ACON
                    };
  U32 aRegData[17];
  const char* asACON[4] = {
    "PC16, DPTR16",
    "PC16, DPTR16",
    "PC24, DPTR24",
    "PC24, DPTR16"
  };
  U32 v;
  unsigned PSWRegBank;
  const char* sACON;

  JLINKARM_ReadRegs(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
  PSWRegBank = (aRegData[2] >> 3) & 3;
  //
  // ACON[C390]: x00 PC16, DPTR16 Mode
  // ACON[C390]: x01 PC16, DPTR16 Mode
  // ACON[C390]: x10 PC24, DPTR24 Mode
  // ACON[C390]: x11 PC24, DPTR16 Mode
  //
  v = aRegData[16] & 3;
  sACON = asACON[v];
  _ReportOutf("PC = %.8X, SP = %.2X, PSW = %.2X (RBANKSEL: %d), ACON: %.2X (%s)\n", aRegData[0], aRegData[1], aRegData[2], PSWRegBank, aRegData[16], sACON);
  _ReportOutf("A = %.2X, B = %.2X\n", aRegData[3], aRegData[4]);
  _ReportOutf("DPTR = %.8X, DPTR1 = %.8X, DPS = %.2X\n", aRegData[5], aRegData[6], aRegData[7]);
  _ReportOutf("R0 = %.2X, R1 = %.2X, R2 = %.2X, R3 = %.2X, R4 = %.2X, R5 = %.2X, R6 = %.2X, R7 = %.2X\n", aRegData[8], aRegData[9], aRegData[10], aRegData[11], aRegData[12], aRegData[13], aRegData[14], aRegData[15]);
}

/*********************************************************************
*
*       _DiffRegs_Update
*
*  Function description
*    Updates a _REGS_STAT instance
*/
static int _DiffRegs_Update(_REGS_STAT* pRegsStat) {
  int Result;

  Result = 0;
  if (pRegsStat == NULL) {  // Invalid param? => Done
    goto Done;
  }
  Result = JLINK_ReadRegs_64(pRegsStat->paRegIndex, pRegsStat->paRegContent, NULL, pRegsStat->NumRegs);
Done:
  return Result;
}

/*********************************************************************
*
*       _DiffRegs_Init
*
*  Function description
*    Initializes a _REGS_STAT instance
*
*  Return value
*    != NULL   <_REGS_STAT instance>
*       NULL   Error
*/
static _REGS_STAT* _DiffRegs_Init(void) {
  U32 Core;
  _REGS_STAT* pResult;

  Core = JLINKARM_CORE_GetFound();
  if (Core != JLINK_CORE_RV64 && Core != JLINK_CORE_RV32) {  // Not RV? => Not supported
    pResult = NULL;
    goto Done;
  }
  pResult = (_REGS_STAT*)SYS_MEM_AllocEx(UTIL_SIZEOF(_REGS_STAT), "Reg Diff. Context");
  if (pResult == NULL) {  // Alloc failed? => Done
    goto Done;
  }
  //
  // Allcoate mem for register contents
  //
  pResult->paRegContent = (U64*)SYS_MEM_AllocEx(UTIL_SIZEOF(U64) * COUNTOF(_aRVRegContexts), "Reg Contents");
  if (pResult->paRegContent == NULL) { // Alloc failed? => Done
    pResult = NULL;
    goto Done;
  }
  //
  // Allcoate mem for register contents to diff against
  //
  pResult->paRegDiff = (U64*)SYS_MEM_AllocEx(UTIL_SIZEOF(U64) * COUNTOF(_aRVRegContexts), "Reg Contents (Diff)");
  if (pResult->paRegDiff == NULL) { // Alloc failed? => Done
    pResult = NULL;
    goto Done;
  }
  pResult->paRegIndex   = &_aRVRegIndex[0];
  pResult->paRegContext = &_aRVRegContexts[0];
  pResult->NumRegs      = COUNTOF(_aRVRegContexts);
  _DiffRegs_Update(pResult);
Done:
  return pResult;
}

/*********************************************************************
*
*       _DiffRegs_PrintDiff
*
*  Function description
*    Outputs all registers which contents changed since the last update of the passed _REGS_STAT instance#
*
*  Parameters
*    pRegsStat   _REGS_STAT instance to diff against
*
*  Return value
*    0   O.K.
*   -1   Error
*/
static int _DiffRegs_PrintDiff(const _REGS_STAT* pRegsStat) {
  int Result;
  char ac[64];
  U8 IsDiff;
  U32 i;
  U32 Core;

  Result = 0;
  if (pRegsStat == NULL) {  // Invalid param? => Done
    goto Done;
  }
  //
  // Get current register contents
  //
  Result = JLINK_ReadRegs_64(pRegsStat->paRegIndex, pRegsStat->paRegDiff, NULL, pRegsStat->NumRegs);
  if (Result < 0) {  // Failed reading regs? => Done
    goto Done;
  }
  //
  // Compare register contents
  //
  IsDiff = 0;
  Core = JLINKARM_CORE_GetFound();
  for (i = 0; i < pRegsStat->NumRegs; i++) {
    if (pRegsStat->paRegContent[i] != pRegsStat->paRegDiff[i]) {  // Reg contents changed? => Print
      if (IsDiff == 0) {                                          // This is the first different reg we found? => Print prefix
        _ReportOutf("Changed regs: ");
        IsDiff = 1;
      } else {                                                    // A change was printed before already? => Print <space>
        _ReportOutf(" ");
      }
//LG xxxxxxxxxxxx generic way to determine regwidth?
      if (Core == JLINK_CORE_RV64) {
        _PrintHex64(ac, UTIL_SIZEOF(ac),      pRegsStat->paRegDiff[i]);
      } else {
        _PrintHex32(ac, UTIL_SIZEOF(ac), (U32)pRegsStat->paRegDiff[i]);
      }
      _ReportOutf("%s = %s", (pRegsStat->paRegContext + i)->sName, ac);
    }
  }
  if (IsDiff) {  // Register differences were printed? => Append linebreak
    _ReportOutf("\n");
  }
Done:
  return Result;
}

/*********************************************************************
*
*       _DiffRegs_DeInit
*
*  Function description
*    Deinitializes a _REGS_STAT instance
*
*  Parameters
*    pRegsStat   _REGS_STAT instance to deinitialize
*/
static void _DiffRegs_DeInit(_REGS_STAT* pRegsStat) {
  if (pRegsStat == NULL) {  // Invalid param? => Done
    goto Done;
  }
  if (pRegsStat->paRegContent) {
    SYS_MEM_Free(pRegsStat->paRegContent);
  }
  if (pRegsStat->paRegDiff) {
    SYS_MEM_Free(pRegsStat->paRegDiff);
  }
  SYS_MEM_Free(pRegsStat);
Done:
  return;
}

/*********************************************************************
*
*       _ShowRegsRV
*
*  Function description
*    Shows CPU registers for RISC-V based devices
*/
static void _ShowRegsRV(void) {
  U32 Core;
  U32 iReg;
  U32 NumRegs;
  U32 LastGroup;
  char ac[64];
  const _REG_CONTEXT* pRegContext;
  U32 aRegData32[33];
  U64 aRegData64[33];
  //
  // Different output for RV32 / RV64
  //
  Core = JLINKARM_CORE_GetFound();
  NumRegs = COUNTOF(_aRVRegIndex);
  if (Core == JLINK_CORE_RV64) {
    JLINK_ReadRegs_64(&_aRVRegIndex[0], &aRegData64[0], NULL, NumRegs);
  } else {
    JLINKARM_ReadRegs(&_aRVRegIndex[0], &aRegData32[0], NULL, NumRegs);
  }
  //
  // For better visibility, we group the RV registers by category.
  // Each category takes up one line
  //
  pRegContext = &_aRVRegContexts[0];
  LastGroup = pRegContext->RegGroup;
  for (iReg = 0; iReg < NumRegs; iReg++) {
    if (LastGroup != (pRegContext + iReg)->RegGroup) {  // Different reg group? => New line
      _ReportOut("\n");
    }
    if (Core == JLINK_CORE_RV64) {
      _PrintHex64(ac, UTIL_SIZEOF(ac), aRegData64[iReg]);
    } else {
      _PrintHex32(ac, UTIL_SIZEOF(ac), aRegData32[iReg]);
    }
    _ReportOutf("%s = %s ", (pRegContext + iReg)->sName, ac);
    LastGroup = (pRegContext + iReg)->RegGroup;
  }
  _ReportOut("\n");
}

/*********************************************************************
*
*       _ShowRegsCortexAR
*
*  Function description
*    Shows CPU registers for Cortex-A/R (ARMv7-A/R) based devices
*/
static void _ShowRegsCortexAR(void) {
  U32 vCPSR;
  U32 aRegIndexCUR[17];
  U32 aRegIndexUSR[7];
  U32 aRegIndexFIQ[8];
  U32 aRegIndexIRQ[3];
  U32 aRegIndexSVC[3];
  U32 aRegIndexABT[3];
  U32 aRegIndexUND[3];
  U32 aRegDataCUR[18];
  U32 aRegDataUSR[7];
  U32 aRegDataFIQ[8];
  U32 aRegDataIRQ[3];
  U32 aRegDataSVC[3];
  U32 aRegDataABT[3];
  U32 aRegDataUND[3];

  aRegIndexCUR[0] = JLINKARM_CORTEX_R4_REG_R15;
  aRegIndexCUR[1] = JLINKARM_CORTEX_R4_REG_CPSR;
  aRegIndexCUR[2] = JLINKARM_CORTEX_R4_REG_R0;
  aRegIndexCUR[3] = JLINKARM_CORTEX_R4_REG_R1;
  aRegIndexCUR[4] = JLINKARM_CORTEX_R4_REG_R2;
  aRegIndexCUR[5] = JLINKARM_CORTEX_R4_REG_R3;
  aRegIndexCUR[6] = JLINKARM_CORTEX_R4_REG_R4;
  aRegIndexCUR[7] = JLINKARM_CORTEX_R4_REG_R5;
  aRegIndexCUR[8] = JLINKARM_CORTEX_R4_REG_R6;
  aRegIndexCUR[9] = JLINKARM_CORTEX_R4_REG_R7;
  aRegIndexCUR[10] = JLINKARM_CORTEX_R4_REG_R8;
  aRegIndexCUR[11] = JLINKARM_CORTEX_R4_REG_R9;
  aRegIndexCUR[12] = JLINKARM_CORTEX_R4_REG_R10;
  aRegIndexCUR[13] = JLINKARM_CORTEX_R4_REG_R11;
  aRegIndexCUR[14] = JLINKARM_CORTEX_R4_REG_R12;
  aRegIndexCUR[15] = JLINKARM_CORTEX_R4_REG_R13;
  aRegIndexCUR[16] = JLINKARM_CORTEX_R4_REG_R14;
  aRegIndexUSR[0] = JLINKARM_CORTEX_R4_REG_R8_USR;
  aRegIndexUSR[1] = JLINKARM_CORTEX_R4_REG_R9_USR;
  aRegIndexUSR[2] = JLINKARM_CORTEX_R4_REG_R10_USR;
  aRegIndexUSR[3] = JLINKARM_CORTEX_R4_REG_R11_USR;
  aRegIndexUSR[4] = JLINKARM_CORTEX_R4_REG_R12_USR;
  aRegIndexUSR[5] = JLINKARM_CORTEX_R4_REG_R13_USR;
  aRegIndexUSR[6] = JLINKARM_CORTEX_R4_REG_R14_USR;
  aRegIndexFIQ[0] = JLINKARM_CORTEX_R4_REG_R8_FIQ;
  aRegIndexFIQ[1] = JLINKARM_CORTEX_R4_REG_R9_FIQ;
  aRegIndexFIQ[2] = JLINKARM_CORTEX_R4_REG_R10_FIQ;
  aRegIndexFIQ[3] = JLINKARM_CORTEX_R4_REG_R11_FIQ;
  aRegIndexFIQ[4] = JLINKARM_CORTEX_R4_REG_R12_FIQ;
  aRegIndexFIQ[5] = JLINKARM_CORTEX_R4_REG_R13_FIQ;
  aRegIndexFIQ[6] = JLINKARM_CORTEX_R4_REG_R14_FIQ;
  aRegIndexFIQ[7] = JLINKARM_CORTEX_R4_REG_SPSR_FIQ;
  aRegIndexIRQ[0] = JLINKARM_CORTEX_R4_REG_R13_IRQ;
  aRegIndexIRQ[1] = JLINKARM_CORTEX_R4_REG_R14_IRQ;
  aRegIndexIRQ[2] = JLINKARM_CORTEX_R4_REG_SPSR_IRQ;
  aRegIndexSVC[0] = JLINKARM_CORTEX_R4_REG_R13_SVC;
  aRegIndexSVC[1] = JLINKARM_CORTEX_R4_REG_R14_SVC;
  aRegIndexSVC[2] = JLINKARM_CORTEX_R4_REG_SPSR_SVC;
  aRegIndexABT[0] = JLINKARM_CORTEX_R4_REG_R13_ABT;
  aRegIndexABT[1] = JLINKARM_CORTEX_R4_REG_R14_ABT;
  aRegIndexABT[2] = JLINKARM_CORTEX_R4_REG_SPSR_ABT;
  aRegIndexUND[0] = JLINKARM_CORTEX_R4_REG_R13_UND;
  aRegIndexUND[1] = JLINKARM_CORTEX_R4_REG_R14_UND;
  aRegIndexUND[2] = JLINKARM_CORTEX_R4_REG_SPSR_UND;
  JLINKARM_ReadRegs(&aRegIndexCUR[0], &aRegDataCUR[0], NULL, COUNTOF(aRegIndexCUR));
  JLINKARM_ReadRegs(&aRegIndexUSR[0], &aRegDataUSR[0], NULL, COUNTOF(aRegIndexUSR));
  JLINKARM_ReadRegs(&aRegIndexFIQ[0], &aRegDataFIQ[0], NULL, COUNTOF(aRegIndexFIQ));
  JLINKARM_ReadRegs(&aRegIndexIRQ[0], &aRegDataIRQ[0], NULL, COUNTOF(aRegIndexIRQ));
  JLINKARM_ReadRegs(&aRegIndexSVC[0], &aRegDataSVC[0], NULL, COUNTOF(aRegIndexSVC));
  JLINKARM_ReadRegs(&aRegIndexABT[0], &aRegDataABT[0], NULL, COUNTOF(aRegIndexABT));
  JLINKARM_ReadRegs(&aRegIndexUND[0], &aRegDataUND[0], NULL, COUNTOF(aRegIndexUND));
  vCPSR = aRegDataCUR[1];
  _ReportOutf("PC: (R15) = %.8X, CPSR = %.8X (%s)\n"
              "Current:\n"
              "     R0 =%.8X, R1 =%.8X, R2 =%.8X, R3 =%.8X\n"
              "     R4 =%.8X, R5 =%.8X, R6 =%.8X, R7 =%.8X\n"
              "     R8 =%.8X, R9 =%.8X, R10=%.8X, R11=%.8X, R12=%.8X\n"
              "     R13=%.8X, R14=%.8X"
              , aRegDataCUR[0],  aRegDataCUR[1], _PrintCPSR(vCPSR)
              , aRegDataCUR[2],  aRegDataCUR[3],  aRegDataCUR[4],  aRegDataCUR[5]
              , aRegDataCUR[6],  aRegDataCUR[7],  aRegDataCUR[8],  aRegDataCUR[9]
              , aRegDataCUR[10], aRegDataCUR[11], aRegDataCUR[12], aRegDataCUR[13], aRegDataCUR[14]
              , aRegDataCUR[15], aRegDataCUR[16]
             );
  if (((vCPSR & 0x1F) != 0x10) && ((vCPSR & 0x1F) != 0x1F)) {
    _ReportOutf(", SPSR=%.8X\n", aRegDataCUR[17]);
  } else {
    _ReportOutf("\n");
  }
  _ReportOutf("USR: R8 =%.8X, R9 =%.8X, R10=%.8X, R11=%.8X, R12=%.8X\n"
              "     R13=%.8X, R14=%.8X\n"
              "FIQ: R8 =%.8X, R9 =%.8X, R10=%.8X, R11=%.8X, R12=%.8X\n"
              "     R13=%.8X, R14=%.8X, SPSR=%.8X\n"
              "IRQ: R13=%.8X, R14=%.8X, SPSR=%.8X\n"
              "SVC: R13=%.8X, R14=%.8X, SPSR=%.8X\n"
              "ABT: R13=%.8X, R14=%.8X, SPSR=%.8X\n"
              "UND: R13=%.8X, R14=%.8X, SPSR=%.8X\n"
              , aRegDataUSR[0], aRegDataUSR[1], aRegDataUSR[2], aRegDataUSR[3], aRegDataUSR[4]
              , aRegDataUSR[5], aRegDataUSR[6]
              , aRegDataFIQ[0], aRegDataFIQ[1], aRegDataFIQ[2], aRegDataFIQ[3], aRegDataFIQ[4]
              , aRegDataFIQ[5], aRegDataFIQ[6], aRegDataFIQ[7]
              , aRegDataIRQ[0], aRegDataIRQ[1], aRegDataIRQ[2]
              , aRegDataSVC[0], aRegDataSVC[1], aRegDataSVC[2]
              , aRegDataABT[0], aRegDataABT[1], aRegDataABT[2]
              , aRegDataUND[0], aRegDataUND[1], aRegDataUND[2]
             );
}

/*********************************************************************
*
*       _ShowRegs
*/
static void  _ShowRegs(void) {
  U32 DevFamily;
  U32 Core;
  U32 aRegData[70];

  DevFamily = JLINKARM_GetDeviceFamily();
  Core = JLINKARM_CORE_GetFound();
  if ((DevFamily == JLINKARM_DEV_FAMILY_CM0) || (DevFamily == JLINKARM_DEV_FAMILY_CM1) || (DevFamily == JLINKARM_DEV_FAMILY_CM3) || (DevFamily == JLINKARM_DEV_FAMILY_CM4)) {
    switch (Core) {
    case JLINK_CORE_CORTEX_M1:      _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M3:      _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M3_R1P0: _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M3_R1P1: _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M3_R2P0: _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M3_R2P1: _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M0:      _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M23:     _ShowRegsARM_v8M();     break;
    case JLINK_CORE_CORTEX_M4:      _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M7:      _ShowRegsARM_v6M_v7M(); break;
    case JLINK_CORE_CORTEX_M33:     _ShowRegsARM_v8M();     break;
    case JLINK_CORE_CORTEX_M55:     _ShowRegsARM_v8M();     break;
    default:                        _ShowRegsARM_v6M_v7M(); break;  // Save default as ARMv7-M only specifies less features than ARMv8-M
    }
  } else if (DevFamily == JLINKARM_DEV_FAMILY_CORTEX_AR_ARMV8) {
    _ShowRegsARMv8AR();
  } else if (DevFamily == JLINK_DEV_FAMILY_BT5511) {
    _ShowRegsBT5511();
  } else if (DevFamily == JLINKARM_DEV_FAMILY_RX) {
    U32 aRegIndex[] = {
      JLINKARM_RX_REG_R0,  JLINKARM_RX_REG_R1,  JLINKARM_RX_REG_R2,   JLINKARM_RX_REG_R3,
      JLINKARM_RX_REG_R4,  JLINKARM_RX_REG_R5,  JLINKARM_RX_REG_R6,   JLINKARM_RX_REG_R7,
      JLINKARM_RX_REG_R8,  JLINKARM_RX_REG_R9,  JLINKARM_RX_REG_R10,  JLINKARM_RX_REG_R11,
      JLINKARM_RX_REG_R12, JLINKARM_RX_REG_R13, JLINKARM_RX_REG_R14,  JLINKARM_RX_REG_R15,
      JLINKARM_RX_REG_ISP, JLINKARM_RX_REG_USP, JLINKARM_RX_REG_INTB, JLINKARM_RX_REG_PC,
      JLINKARM_RX_REG_PSW, JLINKARM_RX_REG_BPC, JLINKARM_RX_REG_BPSW, JLINKARM_RX_REG_FINTV,
      JLINKARM_RX_REG_FPSW
    };
    JLINKARM_ReadRegs(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
    _ReportOutf("R0 = %.8X, R1 = %.8X, R2 = %.8X, R3 = %.8X\n",            aRegData[0],  aRegData[1],  aRegData[2],  aRegData[3]                );
    _ReportOutf("R4 = %.8X, R5 = %.8X, R6 = %.8X, R7 = %.8X\n",            aRegData[4],  aRegData[5],  aRegData[6],  aRegData[7]                );
    _ReportOutf("R8 = %.8X, R9 = %.8X, R10= %.8X, R11= %.8X\n",            aRegData[8],  aRegData[9],  aRegData[10], aRegData[11]               );
    _ReportOutf("R12= %.8X, R13= %.8X, R14= %.8X, R15= %.8X\n",            aRegData[12], aRegData[13], aRegData[14], aRegData[15]               );
    _ReportOutf("ISP %.8X, USP %.8X, INTB %.8X, PC %.8X,\n",               aRegData[16], aRegData[17], aRegData[18], aRegData[19]               );
    _ReportOutf("PSW %.8X, BPC %.8X, BPSW %.8X, FINTV %.8X, FPSW %.8X\n",  aRegData[20], aRegData[21], aRegData[22], aRegData[23], aRegData[24] );
  } else if (DevFamily == JLINKARM_DEV_FAMILY_POWER_PC) {
    U32 aRegIndex[] = {
      JLINK_POWER_PC_REG_R0,     JLINK_POWER_PC_REG_R1,     JLINK_POWER_PC_REG_R2,    JLINK_POWER_PC_REG_R3,
      JLINK_POWER_PC_REG_R4,     JLINK_POWER_PC_REG_R5,     JLINK_POWER_PC_REG_R6,    JLINK_POWER_PC_REG_R7,
      JLINK_POWER_PC_REG_R8,     JLINK_POWER_PC_REG_R9,     JLINK_POWER_PC_REG_R10,   JLINK_POWER_PC_REG_R11,
      JLINK_POWER_PC_REG_R12,    JLINK_POWER_PC_REG_R13,    JLINK_POWER_PC_REG_R14,   JLINK_POWER_PC_REG_R15,
      JLINK_POWER_PC_REG_R16,    JLINK_POWER_PC_REG_R17,    JLINK_POWER_PC_REG_R18,   JLINK_POWER_PC_REG_R19,
      JLINK_POWER_PC_REG_R20,    JLINK_POWER_PC_REG_R21,    JLINK_POWER_PC_REG_R22,   JLINK_POWER_PC_REG_R23,
      JLINK_POWER_PC_REG_R24,    JLINK_POWER_PC_REG_R25,    JLINK_POWER_PC_REG_R26,   JLINK_POWER_PC_REG_R27,
      JLINK_POWER_PC_REG_R28,    JLINK_POWER_PC_REG_R29,    JLINK_POWER_PC_REG_R30,   JLINK_POWER_PC_REG_R31,
      JLINK_POWER_PC_REG_CR,     JLINK_POWER_PC_REG_CTR,    JLINK_POWER_PC_REG_LR,    JLINK_POWER_PC_REG_XER,
      JLINK_POWER_PC_REG_PC,     JLINK_POWER_PC_REG_PVR,    JLINK_POWER_PC_REG_PIR,   JLINK_POWER_PC_REG_SVR,
      JLINK_POWER_PC_REG_MSR,    JLINK_POWER_PC_REG_HID0,   JLINK_POWER_PC_REG_HID1,  JLINK_POWER_PC_REG_SPRG0,
      JLINK_POWER_PC_REG_SPRG1,  JLINK_POWER_PC_REG_SRR0,   JLINK_POWER_PC_REG_SRR1,  JLINK_POWER_PC_REG_CSRR0,
      JLINK_POWER_PC_REG_CSRR1,  JLINK_POWER_PC_REG_DSRR0,  JLINK_POWER_PC_REG_DSRR1, JLINK_POWER_PC_REG_ESR,
      JLINK_POWER_PC_REG_MCSR,   JLINK_POWER_PC_REG_DEAR,   JLINK_POWER_PC_REG_IVPR,  JLINK_POWER_PC_REG_PID0,
      JLINK_POWER_PC_REG_MMUCFG, JLINK_POWER_PC_REG_L1CFG0, JLINK_POWER_PC_REG_BUCSR, JLINK_POWER_PC_REG_DBCR0,
      JLINK_POWER_PC_REG_DBCR1,  JLINK_POWER_PC_REG_DBCR2,  JLINK_POWER_PC_REG_DBSR,  JLINK_POWER_PC_REG_IAC1,
      JLINK_POWER_PC_REG_IAC2,   JLINK_POWER_PC_REG_IAC3,   JLINK_POWER_PC_REG_IAC4,  JLINK_POWER_PC_REG_DAC1,
      JLINK_POWER_PC_REG_DAC2
    };
    JLINKARM_ReadRegs(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
    _ReportOutf("R0 = %.8X, R1 = %.8X, R2 = %.8X, R3 = %.8X\n",              aRegData[0],  aRegData[1],  aRegData[2],  aRegData[3] );
    _ReportOutf("R4 = %.8X, R5 = %.8X, R6 = %.8X, R7 = %.8X\n",              aRegData[4],  aRegData[5],  aRegData[6],  aRegData[7] );
    _ReportOutf("R8 = %.8X, R9 = %.8X, R10= %.8X, R11= %.8X\n",              aRegData[8],  aRegData[9],  aRegData[10], aRegData[11] );
    _ReportOutf("R12= %.8X, R13= %.8X, R14= %.8X, R15= %.8X\n",              aRegData[12], aRegData[13], aRegData[14], aRegData[15] );
    _ReportOutf("R16= %.8X, R17= %.8X, R18= %.8X, R19= %.8X\n",              aRegData[16], aRegData[17], aRegData[18], aRegData[19] );
    _ReportOutf("R20= %.8X, R21= %.8X, R22= %.8X, R23= %.8X\n",              aRegData[20], aRegData[21], aRegData[22], aRegData[23] );
    _ReportOutf("R24= %.8X, R25= %.8X, R26= %.8X, R27= %.8X\n",              aRegData[24], aRegData[25], aRegData[26], aRegData[27] );
    _ReportOutf("R28= %.8X, R29= %.8X, R30= %.8X, R31= %.8X\n",              aRegData[28], aRegData[29], aRegData[30], aRegData[31] );
    _ReportOutf("CR = %.8X, CTR= %.8X, LR = %.8X, XER= %.8X\n",              aRegData[32], aRegData[33], aRegData[34], aRegData[35] );
    _ReportOutf("PC = %.8X\n\n",                                             aRegData[36]                                           );
    _ReportOutf("PVR   =%.8X, PIR   =%.8X, SVR   =%.8X\n",                   aRegData[37], aRegData[38], aRegData[39]               );
    _ReportOutf("MSR   =%.8X, HID0  =%.8X, HID1  =%.8X\n",                   aRegData[40], aRegData[41], aRegData[42]               );
    _ReportOutf("SPRG0 =%.8X, SPRG1 =%.8X, SRR0  =%.8X, SRR1  =%.8X\n",      aRegData[43], aRegData[44], aRegData[45], aRegData[46] );
    _ReportOutf("CSRR0 =%.8X, CSRR1 =%.8X, DSRR0 =%.8X, DSRR1 =%.8X\n",      aRegData[47], aRegData[48], aRegData[49], aRegData[50] );
    _ReportOutf("ESR   =%.8X, MCSR  =%.8X, DEAR  =%.8X, IVPR  =%.8X\n",      aRegData[51], aRegData[52], aRegData[53], aRegData[54] );
    _ReportOutf("PID0  =%.8X, MMUCFG=%.8X, L1CFG0=%.8X, BUCSR =%.8X\n",      aRegData[55], aRegData[56], aRegData[57], aRegData[58] );
    _ReportOutf("DBCR0 =%.8X, DBCR1 =%.8X, DBCR2 =%.8X, DBSR  =%.8X\n",      aRegData[59], aRegData[60], aRegData[61], aRegData[62] );
    _ReportOutf("IAC1  =%.8X, IAC2  =%.8X, IAC3  =%.8X, IAC4  =%.8X\n",      aRegData[63], aRegData[64], aRegData[65], aRegData[66] );
    _ReportOutf("DAC1  =%.8X, DAC2  =%.8X\n",                                aRegData[67], aRegData[68]                             );
  } else if (DevFamily == JLINK_DEV_FAMILY_RISC_V) {
    _ShowRegsRV();
  } else if (DevFamily == JLINK_DEV_FAMILY_MIPS) {
    U32 aRegIndex[] = {
                        JLINK_MIPS_REG_R0,  JLINK_MIPS_REG_R1,  JLINK_MIPS_REG_R2,  JLINK_MIPS_REG_R3,                                 // 0-3
                        JLINK_MIPS_REG_R4,  JLINK_MIPS_REG_R5,  JLINK_MIPS_REG_R6,  JLINK_MIPS_REG_R7,                                 // 4-7
                        JLINK_MIPS_REG_R8,  JLINK_MIPS_REG_R9,  JLINK_MIPS_REG_R10, JLINK_MIPS_REG_R11,                                // 8-11
                        JLINK_MIPS_REG_R12, JLINK_MIPS_REG_R13, JLINK_MIPS_REG_R14, JLINK_MIPS_REG_R15,                                // 12-15
                        JLINK_MIPS_REG_R16, JLINK_MIPS_REG_R17, JLINK_MIPS_REG_R18, JLINK_MIPS_REG_R19,                                // 16-19
                        JLINK_MIPS_REG_R20, JLINK_MIPS_REG_R21, JLINK_MIPS_REG_R22, JLINK_MIPS_REG_R23,                                // 20-23
                        JLINK_MIPS_REG_R24, JLINK_MIPS_REG_R25, JLINK_MIPS_REG_R26, JLINK_MIPS_REG_R27,                                // 24-27
                        JLINK_MIPS_REG_R28, JLINK_MIPS_REG_R29, JLINK_MIPS_REG_R30, JLINK_MIPS_REG_R31,                                // 28-31
                        JLINK_MIPS_REG_HWRENA, JLINK_MIPS_REG_BADVADDR, JLINK_MIPS_REG_COUNT, JLINK_MIPS_REG_COMPARE,                  // 32-35
                        JLINK_MIPS_REG_STATUS, JLINK_MIPS_REG_INTCTL, JLINK_MIPS_REG_SRSCTL, JLINK_MIPS_REG_SRSMAP,                    // 36-39
                        JLINK_MIPS_REG_CAUSE, JLINK_MIPS_REG_EPC, JLINK_MIPS_REG_PRID, JLINK_MIPS_REG_EBASE,                           // 40-43
                        JLINK_MIPS_REG_CONFIG, JLINK_MIPS_REG_CONFIG1, JLINK_MIPS_REG_CONFIG2, JLINK_MIPS_REG_CONFIG3,                 // 44-47
                        JLINK_MIPS_REG_DEBUG, JLINK_MIPS_REG_TRACECONTROL, JLINK_MIPS_REG_TRACECONTROL2, JLINK_MIPS_REG_USERTRACEDATA, // 48-51
                        JLINK_MIPS_REG_TRACEBPC, JLINK_MIPS_REG_DEBUG2, JLINK_MIPS_REG_PC, JLINK_MIPS_REG_ERROR_PC,                    // 52-55
                        JLINK_MIPS_REG_DESAVE, JLINK_MIPS_REG_HI, JLINK_MIPS_REG_LO                                                    // 56-58
                      };
    JLINKARM_ReadRegs(&aRegIndex[0], &aRegData[0], NULL, COUNTOF(aRegIndex));
    _ReportOutf("PC = %.8X, Status = %.8X, Count = %.8X\n",                  aRegData[54], aRegData[36], aRegData[34]);
    _ReportOutf("R0  = %.8X, R1  = %.8X, R2  = %.8X, R3  = %.8X,\n",         aRegData[0],   aRegData[1],   aRegData[2],  aRegData[3]);
    _ReportOutf("R4  = %.8X, R5  = %.8X, R6  = %.8X, R7  = %.8X,\n",         aRegData[4],   aRegData[5],   aRegData[6],  aRegData[7]);
    _ReportOutf("R8  = %.8X, R9  = %.8X, R10 = %.8X, R11 = %.8X,\n",         aRegData[8],   aRegData[9],   aRegData[10], aRegData[11]);
    _ReportOutf("R12 = %.8X, R13 = %.8X, R14 = %.8X, R15 = %.8X,\n",         aRegData[12],  aRegData[13],  aRegData[14], aRegData[15]);
    _ReportOutf("R16 = %.8X, R17 = %.8X, R18 = %.8X, R19 = %.8X,\n",         aRegData[16],  aRegData[17],  aRegData[18], aRegData[19]);
    _ReportOutf("R20 = %.8X, R21 = %.8X, R22 = %.8X, R23 = %.8X,\n",         aRegData[20],  aRegData[21],  aRegData[22], aRegData[23]);
    _ReportOutf("R24 = %.8X, R25 = %.8X, R26 = %.8X, R27 = %.8X,\n",         aRegData[24],  aRegData[25],  aRegData[26], aRegData[27]);
    _ReportOutf("R28 = %.8X, R29 = %.8X, R30 = %.8X, R31 = %.8X,\n",         aRegData[28],  aRegData[29],  aRegData[30], aRegData[31]);
    _ReportOutf("HWREna = %.8X, BadVAddr = %.8X,\n"              ,           aRegData[32],  aRegData[33]);
    _ReportOutf("Compare = %.8X, IntCtl = %.8X,\n",                          aRegData[35],  aRegData[37]);
    _ReportOutf("SRSCtl = %.8X, SRSMap = %.8X, Cause = %.8X,\n",             aRegData[38],  aRegData[39],  aRegData[40]);
    _ReportOutf("EPC = %.8X, PrID = %.8X, EBASE = %.8X,\n",                  aRegData[41],  aRegData[42],  aRegData[43]);
    _ReportOutf("Config = %.8X, Config1 = %.8X, Config2 = %.8X,\n",          aRegData[44],  aRegData[45],  aRegData[46]);
    _ReportOutf("Config3 = %.8X, Debug = %.8X, TraceCtl = %.8X,\n",          aRegData[47],  aRegData[48],  aRegData[49]);
    _ReportOutf("TraceCtl2 = %.8X, UsrTraceData = %.8X, TraceBPC = %.8X,\n", aRegData[50],  aRegData[51],  aRegData[52]);
    _ReportOutf("Debug2 = %.8X, ErrorPC = %.8X,\n",                          aRegData[53],  aRegData[55]);
    _ReportOutf("HI = %.8X, LO = %.8X\n",                                    aRegData[57],  aRegData[58]);
  } else if ((DevFamily == JLINKARM_DEV_FAMILY_CORTEX_A8) || (DevFamily == JLINKARM_DEV_FAMILY_CORTEX_R4) || (DevFamily == JLINKARM_DEV_FAMILY_CORTEX_A5)) {
    _ShowRegsCortexAR();
  } else if (DevFamily == JLINK_DEV_FAMILY_EFM8) {
    U32 aRegIndexSFRs[] = {
                            JLINK_EFM8_PC, JLINK_EFM8_PSW, JLINK_EFM8_SP, JLINK_EFM8_A, JLINK_EFM8_B, JLINK_EFM8_DPTR,
                            JLINK_EFM8_R0, JLINK_EFM8_R1, JLINK_EFM8_R2, JLINK_EFM8_R3, JLINK_EFM8_R4, JLINK_EFM8_R5, JLINK_EFM8_R6, JLINK_EFM8_R7
                          };
    JLINKARM_ReadRegs(&aRegIndexSFRs[0], &aRegData[0], NULL, COUNTOF(aRegIndexSFRs));
    _ReportOutf("PC = %.4X, PSW = %.2X, SP = %.2X, A = %.2X, B = %.2X, DPTR = %.4X\n",                          aRegData[0], aRegData[1], aRegData[2], aRegData[3], aRegData[4], aRegData[5]);
    _ReportOutf("R0 = %.2X, R1 = %.2X, R2 = %.2X, R3 = %.2X, R4 = %.2X, R5 = %.2X, R6 = %.2X, R7 = %.2X\n",     aRegData[6],   aRegData[7],   aRegData[8],  aRegData[9], aRegData[10],   aRegData[11],   aRegData[12],  aRegData[13]);
  } else {
    U32 CPSR = JLINKARM_ReadReg(ARM_REG_CPSR);
    _ReportOutf("PC: (R15) = %.8X, CPSR = %.8X (%s)\n", JLINKARM_ReadReg(ARM_REG_R15), CPSR, _PrintCPSR(CPSR));
    // R0..R7
    _ReportOutf("Current:\n");
    _ReportOutf("     R0 =%.8X, R1 =%.8X, R2 =%.8X, R3 =%.8X\n"
           "     R4 =%.8X, R5 =%.8X, R6 =%.8X, R7 =%.8X\n", JLINKARM_ReadReg(ARM_REG_R0), JLINKARM_ReadReg(ARM_REG_R1), JLINKARM_ReadReg(ARM_REG_R2), JLINKARM_ReadReg(ARM_REG_R3)
                                                          , JLINKARM_ReadReg(ARM_REG_R4), JLINKARM_ReadReg(ARM_REG_R5), JLINKARM_ReadReg(ARM_REG_R6), JLINKARM_ReadReg(ARM_REG_R7));
    // Current
    if (_IsValidCPUMode_ARM79(CPSR & 0x1F)) {
      _ReportOutf("     R8 =%.8X, R9 =%.8X, R10=%.8X, R11=%.8X, R12=%.8X\n"
             "     R13=%.8X, R14=%.8X"
             , JLINKARM_ReadReg(ARM_REG_R8),  JLINKARM_ReadReg(ARM_REG_R9),  JLINKARM_ReadReg(ARM_REG_R10), JLINKARM_ReadReg(ARM_REG_R11)
             , JLINKARM_ReadReg(ARM_REG_R12), JLINKARM_ReadReg(ARM_REG_R13), JLINKARM_ReadReg(ARM_REG_R14));
      if (((CPSR & 0x1F) != 0x10) && ((CPSR & 0x1F) != 0x1F)) {
        _ReportOutf(", SPSR=%.8X\n", JLINKARM_ReadReg(ARM_REG_SPSR));
      } else {
        _ReportOutf("\n");
      }
    }
    // USR
    _ReportOutf("USR: R8 =%.8X, R9 =%.8X, R10=%.8X, R11=%.8X, R12=%.8X\n"
           , JLINKARM_ReadReg(ARM_REG_R8_USR), JLINKARM_ReadReg(ARM_REG_R9_USR), JLINKARM_ReadReg(ARM_REG_R10_USR), JLINKARM_ReadReg(ARM_REG_R11_USR) , JLINKARM_ReadReg(ARM_REG_R12_USR));
    _ReportOutf("     R13=%.8X, R14=%.8X\n"
           , JLINKARM_ReadReg(ARM_REG_R13_USR), JLINKARM_ReadReg(ARM_REG_R14_USR));
    // FIQ
    _ReportOutf("FIQ: R8 =%.8X, R9 =%.8X, R10=%.8X, R11=%.8X, R12=%.8X\n"
           , JLINKARM_ReadReg(ARM_REG_R8_FIQ), JLINKARM_ReadReg(ARM_REG_R9_FIQ), JLINKARM_ReadReg(ARM_REG_R10_FIQ), JLINKARM_ReadReg(ARM_REG_R11_FIQ), JLINKARM_ReadReg(ARM_REG_R12_FIQ));
    _ReportOutf("     R13=%.8X, R14=%.8X, SPSR=%.8X\n"
           , JLINKARM_ReadReg(ARM_REG_R13_FIQ), JLINKARM_ReadReg(ARM_REG_R14_FIQ), JLINKARM_ReadReg(ARM_REG_SPSR_FIQ));
    _ReportOutf("IRQ: R13=%.8X, R14=%.8X, SPSR=%.8X\n", JLINKARM_ReadReg(ARM_REG_R13_IRQ), JLINKARM_ReadReg(ARM_REG_R14_IRQ), JLINKARM_ReadReg(ARM_REG_SPSR_IRQ));
    _ReportOutf("SVC: R13=%.8X, R14=%.8X, SPSR=%.8X\n", JLINKARM_ReadReg(ARM_REG_R13_SVC), JLINKARM_ReadReg(ARM_REG_R14_SVC), JLINKARM_ReadReg(ARM_REG_SPSR_SVC));
    _ReportOutf("ABT: R13=%.8X, R14=%.8X, SPSR=%.8X\n", JLINKARM_ReadReg(ARM_REG_R13_ABT), JLINKARM_ReadReg(ARM_REG_R14_ABT), JLINKARM_ReadReg(ARM_REG_SPSR_ABT));
    _ReportOutf("UND: R13=%.8X, R14=%.8X, SPSR=%.8X\n", JLINKARM_ReadReg(ARM_REG_R13_UND), JLINKARM_ReadReg(ARM_REG_R14_UND), JLINKARM_ReadReg(ARM_REG_SPSR_UND));
  }
}

/*********************************************************************
*
*       _ShowHWStatus
*/
static void _ShowHWStatus(void) {
  JLINKARM_HW_STATUS Stat;
  U32 ITarget;
  JLINKARM_SPEED_INFO SpeedInfo;

  JLINKARM_GetHWStatus(&Stat);
  JLINKARM_GetHWInfo((1 << 2), &ITarget);
  SpeedInfo.SizeOfStruct = sizeof(SpeedInfo);
  JLINKARM_GetSpeedInfo(&SpeedInfo);
  _ShowVTref(Stat.VTarget);
  if (ITarget != 0xFFFFFFFF) {
    _ReportOutf("ITarget=%dmA\n", ITarget);
  }
  if (Stat.tck == 255) {
    _ReportOutf("TCK=? ");
  } else {
    _ReportOutf("TCK=%d ", Stat.tck);
  }
  if (Stat.tdi == 255) {
    _ReportOutf("TDI=? ");
  } else {
    _ReportOutf("TDI=%d ", Stat.tdi);
  }
  if (Stat.tdo == 255) {
    _ReportOutf("TDO=? ");
  } else {
    _ReportOutf("TDO=%d ", Stat.tdo);
  }
  if (Stat.tms == 255) {
    _ReportOutf("TMS=? ");
  } else {
    _ReportOutf("TMS=%d ", Stat.tms);
  }
  if (Stat.tres == 255) {
    _ReportOutf("TRES=? ");
  } else {
    _ReportOutf("TRES=%d ", Stat.tres);
  }
  if (Stat.trst == 255) {
    _ReportOutf("TRST=?\n");
  } else {
    _ReportOutf("TRST=%d\n", Stat.trst);
  }
  //
  // Show supported target interface speeds
  //
  _ReportOutf("Supported target interface speeds:\n");
  if (SpeedInfo.BaseFreq > 1000) {
    _ReportOutf(" - %d MHz/n, (n>=%d). => %dkHz, %dkHz, %dkHz, ...\n", SpeedInfo.BaseFreq / 1000000, SpeedInfo.MinDiv,
                                                                  SpeedInfo.BaseFreq / 1000 / (SpeedInfo.MinDiv + 0),
                                                                  SpeedInfo.BaseFreq / 1000 / (SpeedInfo.MinDiv + 1),
                                                                  SpeedInfo.BaseFreq / 1000 / (SpeedInfo.MinDiv + 2));
  }
  if (SpeedInfo.SupportAdaptive) {
    _ReportOutf(" - Adaptive clocking\n");
  }
}

/*********************************************************************
*
*       _ShowHWInfo
*/
static void _ShowHWInfo(void) {
  U32 aInfo[32];
  int i;

  JLINKARM_GetHWInfo(0xFFFFFFFF, &aInfo[0]);

  for (i = 0; i < 32; i++) {
    if (aInfo[i] != 0xFFFFFFFF) {
      switch (i) {
      case HW_INFO_POWER_ENABLED:
        _ReportOutf("HWInfo[%.2d] = Target power is %s\n", i, aInfo[i] ? "enabled" : "disabled");
        break;
      case HW_INFO_POWER_OVERCURRENT:
        switch (aInfo[i]) {
        case 0:                                                                                          break;
        case 1:          _ReportOutf("HWInfo[%.2d] = OverCurrent (2ms @ 3000mA)\n", i);                  break;
        case 2:          _ReportOutf("HWInfo[%.2d] = OverCurrent (10ms @ 1000mA)\n", i);                 break;
        case 3:          _ReportOutf("HWInfo[%.2d] = OverCurrent (40ms @ 400mA)\n", i);                  break;
        default:         _ReportOutf("HWInfo[%.2d] = OverCurrent (Unknown reason: %d)\n", i, aInfo[i]);  break;
        }
        break;
      case HW_INFO_ITARGET:
        _ReportOutf("HWInfo[%.2d] = %dmA\t(ITarget)\n", i, aInfo[i]);
        break;
      case HW_INFO_ITARGET_PEAK:
        _ReportOutf("HWInfo[%.2d] = %dmA\t(ITargetPeak)\n", i, aInfo[i]);
        break;
      case HW_INFO_ITARGET_PEAK_OPERATION:
        _ReportOutf("HWInfo[%.2d] = %dmA\t(ITargetPeakOperation)\n", i, aInfo[i]);
        break;
      case HW_INFO_ITARGET_MAX_TIME0:
        _ReportOutf("HWInfo[%.2d] = %dms\t(ITargetMaxTime0)\n", i, aInfo[i]);
        break;
      case HW_INFO_ITARGET_MAX_TIME1:
        _ReportOutf("HWInfo[%.2d] = %dms\t(ITargetMaxTime1)\n", i, aInfo[i]);
        break;
      case HW_INFO_ITARGET_MAX_TIME2:
        _ReportOutf("HWInfo[%.2d] = %dms\t(ITargetMaxTime2)\n", i, aInfo[i]);
        break;
      default:
        _ReportOutf("HWInfo[%.2d] = 0x%.8X\n", i, aInfo[i]);
      }
    }
  }
}

/*********************************************************************
*
*       _ShowMemZones
*
*  Function description
*    Some devices support different memory zones. For such devices, we output the information about the memory zones, if available
*
*  Notes
*    (1) May only be called after a target connection has been established successfully
*/
static int _ShowMemZones(void) {
  JLINK_MEM_ZONE_INFO aZoneInfo[16];
  int r;
  int i;

  r = JLINK_GetMemZones(&aZoneInfo[0], COUNTOF(aZoneInfo));
  if (r > 0) {
    _ReportOutf("Memory zones:\n");
    for (i = 0; i < r; i++) {
      _ReportOutf("  Zone: %s Description: %s\n", aZoneInfo[i].sName, aZoneInfo[i].sDesc);
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _TestNet
*
*  Function description
*    Used to test host communication, especially when bringing up new hardware.
*    Basically, the data received is echoed.
*    For each packet sent by host, one response is expected.
*    The size of the packets varies, as well as the delay used by the target.
*
*  Communication format
*    -> U8  0xEF             // Cmd EMU_CMD_TEST_NET (Already swallowed by caller)
*    -> U8  SubCmd           // b[3..0]: Specifies the delay in repsonse per byte. Delay: 2 ^ (n-1) us. 0: 0, 1: 1, 2: 2, 3: 4, 4: 8, ... 15:16384
*                            // b[7..4]: Type of transfer. 0: Use overlap
*    -> U16 NumBytes
*    -> U8  Data[NumBytes]
*    <- U8  Data[NumBytes]
*    <- U8  Status           // 0
*/
static int _TestNet(int SizeInc, int DelayInc, int DelayMax) {
  U32 AllocSize;
  U8* pWrite;
  U8* pRead;
  U32 NumBytes;
  U32 NumBytesRead;
  char Response;
  int Delay;
  int i;
  int r = -1;
  //
  // Allocate memory
  //
  AllocSize = (MAX_NUM_BYTES_TEST_NET * 2) + 16;
  pWrite = malloc(AllocSize);
  if (pWrite == NULL) {
    _ReportOutf("ERROR: Could not alloc memory.\n");
    return -1;        // Error
  }
  pRead = pWrite + (AllocSize / 2);
  //
  // Fill memory with test pattern
  //
  for (i = 0; i < MAX_NUM_BYTES_TEST_NET; i++) {
    *(pWrite + i + 4) = i % 255;
  }
  //
  // Perform test in a loop
  //
  for (Delay = 0; Delay <= DelayMax; Delay += DelayInc) {
    for (NumBytes = 1; NumBytes <= MAX_NUM_BYTES_TEST_NET; NumBytes += SizeInc) {
      if (Delay || (NumBytes > 1)) {
        _ReportOutf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
      }
      _ReportOutf("  %4d bytes with delay %4d", NumBytes, Delay);
      *(pWrite + 0) = EMU_CMD_TEST_NET;
      *(pWrite + 1) = Delay;
      *(pWrite + 2) = (NumBytes >> 0) & 0xFF;
      *(pWrite + 3) = (NumBytes >> 8) & 0xFF;
      JLINKARM_Lock();
      NumBytesRead = JLINKARM_CommunicateEx(pWrite, (NumBytes + 4), pRead, (NumBytes + 1), 1);
      JLINKARM_Unlock();
      if (NumBytesRead != (NumBytes + 1)) {
        _ReportOutf("ERROR: Communication error (Expected %d bytes, received %d)!\n", (NumBytes + 1), NumBytesRead);
        r = JLINK_ERROR_UNKNOWN;
        goto Done;
      }
      Response = *(pRead + NumBytes);
      if (Response != 0) {
        if (Response == 6) {
          _ReportOutf("ERROR: J-Link is out of memory!\n");
        } else {
          _ReportOutf("ERROR: Failed to transfer data!\n");
        }
        r = JLINK_ERROR_UNKNOWN;
        goto Done;
      }
      if (memcmp(pWrite + 4, pRead, NumBytes) != 0) {
        _ReportOutf("ERROR: Verification of data failed!\n");
        r = JLINK_ERROR_UNKNOWN;
        goto Done;
      }
      if (_kbhit() != 0) {
        _getch();
        _ReportOutf("\nTest canceled by user!\n");
        r = JLINK_ERROR_CANCELED_BY_USER;
        goto Done;    // Canceled by user
      }
    }
  }
  _ReportOutf("\nTest completed successfully!\n");
  r = 0;              // O.K.
  //
  // Free the allocated memory
  //
Done:
  if (pWrite) {
    free(pWrite);
  }
  return r;
}

/*********************************************************************
*
*       _ExecTestNet
*/
static int _ExecTestNet(const char* s) {
  int r = 0;
  
  (void)s;
  if (JLINKARM_EMU_HasCapEx(EMU_CAP_EX_TEST_NET) == 0) {
    _ReportOutf("Connected J-Link does not support EMU_CMD_TEST_NET.\n");
    return JLINK_NOERROR;
  }
  if (r == 0) {
    _ReportOutf("Performing quick test...(press any key to cancel)\n");
    r = _TestNet(47, 5, 5);
    r = _TestNet(255, 500, 1000);
  }
  if (r == 0) {
    _ReportOutf("Performing intensive test...(press any key to cancel)\n");
    r = _TestNet(1, 2, 10);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTestNRWSpeed
*
*  Function description
*    Tests the network speed for read or write
*/
static int _ExecTestNRWSpeed(const char* s, int IsWrite) {
  U32 NumReps;
  U32 NumRepsRem;
  U32 NumBytes;
  U32 NumRepsAtOnce;
  U32 NumRepsAtOnceMax;
  U32 v;
  double Speed;
  int r;
  int t;

  NumReps  = 512;
  NumBytes = 0x10000;
  _ReportOutf("Syntax: TestN%sspeed [<NumBytes> [<NumReps>]]\n", IsWrite ? "W" : "R");
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (!*s) {
    _ReportOutf("Using defaults\n");
  } else {
    if (_ParseDec(&s, &NumBytes)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (*s) {
    if (_ParseDec(&s, &NumReps)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Check parameters
  //
  if (NumBytes > 0x200000) {
    _ReportOutf("\nNumBytes may not exceed %d.\n", 0x200000);
    return JLINK_NOERROR;
  }
  NumRepsAtOnceMax = (512 * 1024) / NumBytes;     // make sure we do not transfer more than 512KB at once
  _ReportOutf("\nTransferring "),
  _PrintNumBytes(NumReps * NumBytes);
  _ReportOutf(" (%d * ", NumReps);
  _PrintNumBytes(NumBytes);
  _ReportOutf(")\n");
  //
  // Transfer data
  //
  NumRepsRem = NumReps;
  r = 0;
  t = SYS_GetTickCount();
  do {
    NumRepsAtOnce = MIN(NumRepsAtOnceMax, NumRepsRem);
    if (IsWrite) {
      r = JLINKARM_EMU_TestNWSpeed(NumRepsAtOnce, NumBytes);
    } else {
      r = JLINKARM_EMU_TestNRSpeed(NumRepsAtOnce, NumBytes);
    }
    if (r < 0) {  // Failed to test speed? => Done
      _ReportOutf("\nFailed to test speed.\n");
      return JLINK_ERROR_UNKNOWN;
    }
    _ReportOutf(".");
    NumRepsRem -= NumRepsAtOnce;
  } while (NumRepsRem);
  t = SYS_GetTickCount() - t;
  if (t) {
    Speed = (double)(NumBytes * NumReps) / (double)t;
    v = (U32)(Speed * 100);
    _ReportOutf("\n%d.%.2d KBytes/sec", v / 100, v % 100);
    Speed = (double)(1000 * NumReps) / (double)t;
    _ReportOutf("\n%d Blocks/sec\n", (int)Speed);
  } else {
    _ReportOutf("WARNING: Measured time is 0ms\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTestNRSpeed
*/
static int _ExecTestNRSpeed(const char* s) {
  return _ExecTestNRWSpeed(s, 0);
}

/*********************************************************************
*
*       _ExecTestNWSpeed
*/
static int _ExecTestNWSpeed(const char* s) {
  return _ExecTestNRWSpeed(s, 1);
}


/*********************************************************************
*
*       _ExecTestWSpeed
*/
static int _ExecTestWSpeed(const char* s) {
  int i;
  int t;
  int r;
  I32 BlockSize;
  U16 Speed;
  void * p;
  void * pRef;
  I32 NumReps;
  U32 Addr = _RAMAddr;
  //
  // Determine block size
  //
  Speed   = JLINKARM_GetSpeed();
  NumReps = 8;
  if (Speed <= 100) {
    BlockSize = 0x400;
    NumReps = 4;
  } else if (Speed < 1000) {
    BlockSize = 0x2000;
  } else if (Speed < 4000) {
    BlockSize = 0x10000;
  } else {
    BlockSize = 0x20000;
  }
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &Addr)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (*s) {
    if (_ParseHex(&s, (U32*)&BlockSize)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (*s) {
    if (_ParseDec(&s, (U32*)&NumReps)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Alloc memory
  //
  _ReportOutf("Speed test: Writing %d * %dKB into memory @ address 0x%.8X ", NumReps, BlockSize >> 10, Addr);
  p = malloc(BlockSize);
  pRef = malloc(BlockSize);
  if ((p == NULL) || (pRef == NULL)) {
    _ReportOutf("Could not alloc memory\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Fill memory with pattern
  //
  for (i = 0; i < BlockSize; i++) {
    *((U8*)p + i) = i % 255;
  }
  //
  // Transfer data
  // We do NOT halt the CPU to be also able to perform speed tests with a running CPU
  // Important for devices where memory accesses are possible while the CPU is running
  //
  t = SYS_GetTickCount();
  for (i =0; i < NumReps; i++) {
    r = JLINKARM_WriteMemEx(Addr, BlockSize, p, 0);
    if (r != BlockSize) {
      _ReportOutf("\nCould not write memory.\n");
      return JLINK_ERROR_UNKNOWN;
    }
    _ReportOutf(".");
  }
  t = SYS_GetTickCount() - t;
  if (t < 1) {
    t = 1;
  }
  //
  // Perform check after write
  //
  JLINKARM_ReadMemEx(Addr, BlockSize, pRef, 0);
  r = _memcmp(p, pRef, BlockSize);
  if (r < 0) {
    _ReportOutf("\n%d KByte written in %dms ! (%d KByte/sec)\n", BlockSize >> 10, t / NumReps, (NumReps * BlockSize) / t);
  } else {
    _ReportOutf("\nWrite test failed. %d bytes written successfully.\n", r);
  }
  free(pRef);
  free(p);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTestRSpeed
*/
static int _ExecTestRSpeed(const char* s) {
  int i;
  int t;
  int r;
  int Result;
  int IsMemCached;
  U32 BlockSize;
  U16 Speed;
  void* p;
  I32 NumReps;
  U32 Addr;
  char ac[64];
  
  Result      = JLINK_NOERROR;
  NumReps     = 8;
  p           = NULL;
  Addr        = _RAMAddr;
  IsMemCached = JLINK_ExecCommand("SetEnableMemCache = 0", NULL, 0); // Disable memcaching. Otherwise, we might measure the time of loading cached memory, which is not what we want.
  Speed       = JLINKARM_GetSpeed();
  //
  // Determine block size
  //
  if (Speed < 1000) {
    BlockSize = 0x2000;
  } else {
    BlockSize = 0x20000;
  }
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &Addr)) {
      Result = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (*s) {
    if (_ParseHex(&s, &BlockSize)) {
      Result = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (*s) {
    if (_ParseDec(&s, (U32*)&NumReps)) {
      Result = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  //
  // Test speed
  //
  _ReportOutf("Speed test: Reading %d * %dKB from address 0x%.8X ", NumReps, BlockSize >> 10, Addr);
  p = SYS_MEM_Alloc(BlockSize);
  if (p == NULL) {
    _ReportOutf("Could not alloc memory\n");
    Result = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  t = SYS_GetTickCount();
  for (i =0; i < NumReps; i++) {
    r = JLINKARM_ReadMemEx(Addr, BlockSize, p, 0);
    if (r != (int)BlockSize) {
      _ReportOutf("\nCould not read memory.\n");
      Result = JLINK_ERROR_UNKNOWN;
      goto Done;
    }
    _ReportOutf(".");
  }
  t = SYS_GetTickCount() - t;
  if (t < 1) {
    t = 1;
  }
  _ReportOutf("\n%d KByte read in %dms ! (%d KByte/sec)\n", BlockSize >> 10, t / NumReps, (NumReps * BlockSize) / t);
Done:
  SYS_MEM_Free(p);
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "SetEnableMemCache = %i", IsMemCached); // Restore memcache settings.
  JLINK_ExecCommand(ac, NULL, 0);
  if (Result == JLINK_ERROR_SYNTAX) {
    _ReportOutf("Syntax: %s\n", SYNTAX_TESTRSPEED);
  }
  return Result;
}

/*********************************************************************
*
*       _ExecTestCSpeed
*/
static int _ExecTestCSpeed(const char* s) {
  int Freq;
  U32 RAMAddr  = _RAMAddr;
  U32 RepCount = 1;
  U32 Data1, Data2, Data3;

  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &RAMAddr)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &RepCount)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Determine CPU clock frequency
  //
  _ReportOutf("Testing CPU clock frequency @ address 0x%.8X...\n", RAMAddr);
  JLINKARM_ReadMemHW (RAMAddr, 4, &Data1);
  Data3 = Data1;
  Data2 = (Data1 + 0x00010001) & 0x000F000F;
  JLINKARM_WriteMemHW(RAMAddr, 4, &Data2);
  JLINKARM_ReadMemHW (RAMAddr, 4, &Data1);
  JLINKARM_WriteMemHW(RAMAddr, 4, &Data3);
  if (Data1 != Data2) {
    _ReportOutf("No RAM available @ address 0x%.1X. Could not determine CPU clock\n", RAMAddr);
    return JLINK_ERROR_UNKNOWN;
  }
  while (RepCount--) {
    Freq = JLINKARM_MeasureCPUSpeed(RAMAddr, 1);
    if (Freq <= 0) {
      _ReportOutf("Could not determine CPU clock\n");
      break;
    } else {
      _ReportOutf("CPU running at %d kHz\n", (U32)Freq / 1000);
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecExportDeviceListXML
*/
static int _ExecExportDeviceListXML(const char* s) {
  JLINK_FUNC_GET_PF_EXP_DEVICE_LIST_XML* pf;
  int r;

  r = JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED;
  pf = (JLINK_FUNC_GET_PF_EXP_DEVICE_LIST_XML*)JLINK_GetpFunc(JLINK_IFUNC_GET_PF_EXP_DEVICE_LIST_XML);
  if (pf) {
    _EatWhite(&s);
    r = pf(s);
  }
  return r;
}

/*********************************************************************
*
*       _ExecExportDeviceList
*/
static int _ExecExportDeviceList(const char* s) {
  char ac[MAX_PATH];
  char acOut[512];
  HANDLE hFile;
  int NumDevices;
  int NumBytesWritten;
  int Len;
  int i;
  int j;
  JLINKARM_DEVICE_INFO DeviceInfo;
  //
  // Parse parameters
  //
  hFile = INVALID_HANDLE_VALUE;
  ac[0] = 0;
  _ParseString(&s, ac, sizeof(ac));
  if (ac[0] == 0) {
    _ReportOutf("Syntax: exportdevicelist <filename>\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Open file for writing
  //
  _ReportOutf("Opening text file for writing... [%s]\n", ac);
  hFile = _OpenFile(ac, FILE_FLAG_WRITE | FILE_FLAG_CREATE | FILE_FLAG_TRUNC);
  if (hFile == INVALID_HANDLE_VALUE) {
    _ReportOutf("Could not open file for writing.\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Output table header
  //
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "\"Manufacturer\", \"Device\", \"Core\", {Flash areas}, {RAM areas}\r\n");
  Len = UTIL_strlen(ac);
  NumBytesWritten = _WriteFile(hFile, ac, Len);
  if (NumBytesWritten != Len) {
    _ReportOutf("Could not write to file.\n");
    goto Done;
  }
  //
  // Export device list from DLL
  //
  NumDevices = JLINKARM_DEVICE_GetInfo(-1, NULL);
  i = 0;
  do {
    DeviceInfo.SizeOfStruct = sizeof(JLINKARM_DEVICE_INFO);
    JLINKARM_DEVICE_GetInfo(i, &DeviceInfo);
    if (DeviceInfo.sName == NULL) {   // Hidden devices are never output by the DLL. Hidden devices are always at the end of the list
      break;
    }
    //
    // Output manufacturer & device name
    //
    JLINKARM_Core2CoreName(DeviceInfo.Core, acOut, sizeof(acOut));
    UTIL_snprintf(ac, UTIL_SIZEOF(ac), "\"%s\", \"%s\", \"%s\", ", DeviceInfo.sManu, DeviceInfo.sName, acOut);
    Len = UTIL_strlen(ac);
    _WriteFile(hFile, ac, Len);
    //
    // Output flash region info
    //
    if (DeviceInfo.aFlashArea[1].Size) {  // More than one region?
      _WriteFile(hFile, "{ ", 2);
    }
    j = 0;
    do {
      UTIL_snprintf(ac, UTIL_SIZEOF(ac), "{0x%.8X, 0x%.8X}", DeviceInfo.aFlashArea[j].Addr, DeviceInfo.aFlashArea[j].Size);
      Len = UTIL_strlen(ac);
      j++;
      _WriteFile(hFile, ac, Len);
      if (DeviceInfo.aFlashArea[j].Size == 0) {   // Reached end of list?
        if (j > 1) {
          _WriteFile(hFile, " }", 2);
        }
        break;
      }
      _WriteFile(hFile, ", ", 2);
    } while(1);
    _WriteFile(hFile, ", ", 2);
    //
    // Output RAM region info
    //
    if (DeviceInfo.aRAMArea[1].Size) {  // More than one region?
      _WriteFile(hFile, "{ ", 2);
    }
    j = 0;
    do {
      UTIL_snprintf(ac, UTIL_SIZEOF(ac), "{0x%.8X, 0x%.8X}", DeviceInfo.aRAMArea[j].Addr, DeviceInfo.aRAMArea[j].Size);
      Len = UTIL_strlen(ac);
      j++;
      _WriteFile(hFile, ac, Len);
      if (DeviceInfo.aRAMArea[j].Size == 0) {   // Reached end of list?
        if (j > 1) {
          _WriteFile(hFile, " }", 1);
        }
        break;
      }
      _WriteFile(hFile, ", ", 2);
    } while(1);
    _WriteFile(hFile, "\r\n", 2);
  } while(++i < NumDevices);
Done:
  if (hFile != INVALID_HANDLE_VALUE) {
    _CloseFile(hFile);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecLoadBin
*/
static int _ExecLoadBin(const char* s) {
  char ac[MAX_PATH];
  U32 Addr;
  int r;
  int PrevState;
  //
  // Parse parameters
  //
  Addr = 0;
  ac[0] = 0;
  _ParseString(&s, ac, sizeof(ac));
  if (_ParseHex(&s, &Addr) || (ac[0] == 0)) {
    _ReportOutf("Syntax: loadbin <filename>, <addr>\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Open file for reading
  //
  if (JLINKARM_IsHalted() == 0) {
    _ReportOutf("Halting CPU for downloading file.\n");
    JLINKARM_Halt();
  }
  _ReportOutf("Downloading file [%s]...\n", ac);
  PrevState = JLINKARM_ExecCommand("SetVerifyRAMDownload = 1", NULL, 0);       // Enable verify of downloads into RAM
  r = JLINK_DownloadFile(ac, Addr);
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "SetVerifyRAMDownload = %d", PrevState);  // Restore previous state of setting
  JLINKARM_ExecCommand(ac, NULL, 0);
  if (r < 0) {
    switch (r) {
    case JLINK_ERR_FLASH_PROG_COMPARE_FAILED:  _ReportOutf("Error while programming flash: Comparing flash contens failed.\n"); break;
    case JLINK_ERR_FLASH_PROG_PROGRAM_FAILED:  _ReportOutf("Error while programming flash: Programming failed.\n");             break;
    case JLINK_ERR_FLASH_PROG_VERIFY_FAILED:   _ReportOutf("Error while programming flash: Verify failed.\n");                  break;
    case JLINK_ERR_OPEN_FILE_FAILED:           _ReportOutf("Failed to open file.\n");                                           break;
    case JLINK_ERR_UNKNOWN_FILE_FORMAT:        _ReportOutf("File is of unknown / supported format.\n");                         break;
    case JLINK_ERR_WRITE_TARGET_MEMORY_FAILED: _ReportOutf("Writing target memory failed.\n");                                  break;
    default:                                   _ReportOutf("Unspecified error %d\n", r);                                        break;
    }
    return JLINK_ERROR_UNKNOWN;
  } else {
    _ReportOutf("O.K.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecLoadFile
*/
static int _ExecLoadFile(const char* s) {
  char ac[MAX_PATH];
  char acExt[32];
  U32 Addr;
  int PrevState;
  int r;
  //
  // Parse parameters
  //
  Addr = 0;
  ac[0] = 0;
  _ParseString(&s, ac, sizeof(ac));
  if (ac[0] == 0) {
    _ReportOutf("Syntax: loadfile <filename> [<addr>]\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
    _EatWhite(&s);
  }
  if (*s) {
    if (_ParseHex(&s, &Addr)) {
      _ReportOutf("Syntax: loadfile <filename> [<addr>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  } else {
    SYS_ExtractExtension(ac, acExt, UTIL_SIZEOF(acExt));
    r = UTIL_stricmp(acExt, ".bin");
    if (r == 0) {
      _ReportOutf("No address passed for .bin file. Assuming address: 0x0\n");
    }
  }
  //
  // Open file for reading
  //
  if (JLINKARM_IsHalted() == 0) {
    JLINKARM_Halt();
  }
  _ReportOutf("Downloading file [%s]...\n", ac);
  PrevState = JLINKARM_ExecCommand("SetVerifyRAMDownload = 1", NULL, 0);       // Enable verify of downloads into RAM
  r = JLINK_DownloadFile(ac, Addr);
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "SetVerifyRAMDownload = %d", PrevState);  // Restore previous state of setting
  JLINKARM_ExecCommand(ac, NULL, 0);
  if (r < 0) {
    switch (r) {
    case JLINK_ERR_FLASH_PROG_COMPARE_FAILED:  _ReportOutf("Error while programming flash: Comparing flash contens failed.\n"); break;
    case JLINK_ERR_FLASH_PROG_PROGRAM_FAILED:  _ReportOutf("Error while programming flash: Programming failed.\n");             break;
    case JLINK_ERR_FLASH_PROG_VERIFY_FAILED:   _ReportOutf("Error while programming flash: Verify failed.\n");                  break;
    case JLINK_ERR_OPEN_FILE_FAILED:           _ReportOutf("Failed to open file.\n");                                           break;
    case JLINK_ERR_UNKNOWN_FILE_FORMAT:        _ReportOutf("File is of unknown / supported format.\n");                         break;
    case JLINK_ERR_WRITE_TARGET_MEMORY_FAILED: _ReportOutf("Writing target memory failed.\n");                                  break;
    default:                                   _ReportOutf("Unspecified error %d\n", r);                                        break;
    }
    return JLINK_ERROR_UNKNOWN;
  } else {
    _ReportOutf("O.K.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecVerifyBin
*/
static int _ExecVerifyBin(const char* s) {
  HANDLE hFile;
  U32 Addr;
  U32 FileSize;
  U8* pBinData;
  U8* pTargetData;
  char ac[MAX_PATH];
  char acErr[512];
  int r;
  int NumBytesRead;
  //
  // Parse parameters: <filename>, <addr>0
  //
  ac[0] = 0;
  acErr[0] = 0;
  _ParseString(&s, ac, sizeof(ac));
  if (_ParseHex(&s, &Addr) || (ac[0] == 0)) {
    _ReportOutf("Syntax: verifybin <filename>, <addr>\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Open file for reading
  //
  _ReportOutf("Loading binary file %s\n", ac);
  hFile = _OpenFile(ac, FILE_FLAG_READ | FILE_FLAG_SHARE_READ);
  if (hFile != INVALID_HANDLE_VALUE) {
    FileSize = _GetFileSize(hFile);
    pBinData  = (U8*) malloc(FileSize);
    if (pBinData) {
      NumBytesRead = _ReadFile(hFile, pBinData, FileSize);
      if (NumBytesRead >= 0) {
        if (NumBytesRead == (int)FileSize) {
          pTargetData = (U8*) malloc(FileSize);
          _ReportOutf("Reading %i byte%s data from target memory @ 0x%.8X.\n", NumBytesRead,(NumBytesRead >= 1) ? "s" : "", Addr);
          if (JLINKARM_IsHalted() == 0) {
            JLINKARM_Halt();
          }
          JLINKARM_ReadMem(Addr, FileSize, pTargetData);
          //
          //  Compare bin file and target data
          //
          r = _memcmp(pTargetData, pBinData, FileSize);
          if (r >= 0) {
            _ReportOutf("Verify failed @ address 0x%.8X.\nExpected %.2X read %.2X\n", Addr + r, *(pBinData + r), *(pTargetData + r));
            _UTIL_CopyString(acErr, "ERROR: Verify failed.\n", _SIZEOF(acErr));     // Verify failed error handler
          } else {                                 // Compared data are identical
            _ReportOutf("Verify successful.\n");
          }
        } else {
          _UTIL_CopyString(acErr, "ERROR: Could not read file.\n", _SIZEOF(acErr));
        }
      } else {
        _UTIL_CopyString(acErr, "ERROR: Could not read file.\n", _SIZEOF(acErr));
      }
      free(pBinData);
    } else {
      _UTIL_CopyString(acErr, "ERROR: Could not malloc file buffer.\n", _SIZEOF(acErr));
    }
    _CloseFile(hFile);
  } else {
    _UTIL_CopyString(acErr, "ERROR: Could not open file.\n", _SIZEOF(acErr));
  }
  if (acErr[0]) {
    _ReportOutf(acErr);
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSaveBin
*
*  Function description
*    Saves target memory into binary file
*/
static int _ExecSaveBin(const char* s) {
  char ac[MAX_PATH];
  HANDLE hFile;
  int NumBytesWritten;
  U32 Addr;
  char* pBuffer;
  U32 NumBytes;
  int r;
  //
  // Parse parameters
  //
  _ParseString(&s, ac, sizeof(ac));
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: savebin <filename>, <addr>, <NumBytes> (hex)\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumBytes)) {
    _ReportOutf("Syntax: savebin <filename>, <addr>, <NumBytes> (hex)\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Open file for writing
  //
  _ReportOutf("Opening binary file for writing... [%s]\n", ac);
  hFile = _OpenFile(ac, FILE_FLAG_WRITE | FILE_FLAG_CREATE | FILE_FLAG_TRUNC);
  if (hFile == INVALID_HANDLE_VALUE) {
    _ReportOutf("Could not open file for writing.\n");
    return JLINK_ERROR_SYNTAX;
  }
  pBuffer  = (char*) malloc(NumBytes);
  if(pBuffer == NULL) {
    _ReportOutf("Could not allocate memory for reading %d bytes at addr 0x%.8X.\n", NumBytes, Addr);
    r = JLINK_ERROR_UNKNOWN;
    goto Cleanup;
  }
  _ReportOutf("Reading %d bytes from addr 0x%.8X into file...", NumBytes, Addr);
  r = JLINKARM_ReadMemEx(Addr, NumBytes, pBuffer, 0);
  if (r < 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Cleanup;
  }
  if ((U32)r != NumBytes) {
    _ReportOutf("Not all bytes could be read from target. 0x%.8X bytes successfully read.\n", r);
  }
  NumBytesWritten = _WriteFile(hFile, pBuffer, r);
  if (NumBytesWritten != r) {
    _ReportOutf("Could not write to file.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Cleanup;
  }
  _ReportOutf("O.K.\n");
  r = JLINK_NOERROR;
Cleanup:
  _CloseFile(hFile);
  if(pBuffer) {
    free(pBuffer);
  }
  return r;
}

/*********************************************************************
*
*       _ExecWriteRaw
*/
static int _ExecWriteRaw(const char* s) {
  I32 NumBits;
  U8 abTDI[1024];
  U8 abTMS[1024];
  U8 abTDO[1024];
  const char * sErr;
  int i;
  int Off;

  if (_ParseDec(&s, (U32*)&NumBits)) {
    _ReportOutf("Expected number of bits\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  sErr = _ParseHexString(&s, abTMS, NumBits);
  if (sErr) {
    _ReportOutf(sErr);
    _ReportOutf("\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  sErr = _ParseHexString(&s, abTDI, NumBits);
  if (sErr) {
    _ReportOutf(sErr);
    _ReportOutf("\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_JTAG_StoreGetRaw(abTDI, abTDO, abTMS, NumBits);
  _ReportOutf(" TDO: ");
  for (i = 0; i < NumBits; i += 8) {
    Off = (NumBits - 1 - i) / 8;
    if ((NumBits - i) <= 4) {
      _ReportOutf("%.1X", abTDO[Off]);
    } else {
      _ReportOutf("%.2X", abTDO[Off]);
    }
  }
  _ReportOutf("\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _CalcParity
*
*  Function description
*    Computes the (even) parity of the 32-bit value passed as parameter
*
*  Return value
*    0:   Number of 1s in Data is even
*    1:   Number of 1s in Data is odd
*
*  Examples
*    0 => 0
*    1 => 1
*    2 => 1
*    3 => 0
*    4 => 1
*    ...
*/
static int _CalcParity(U32 Data) {
  Data = (Data >> 16) ^ Data;           // Reduce 32 bits to 16 bits
  Data = (Data >>  8) ^ Data;           // Reduce 16 bits to 8 bits
  Data = (Data >>  4) ^ Data;           // Reduce 8 bits to 4 bits
  Data = (Data >>  2) ^ Data;           // Reduce 4 bits to 2 bits
  Data = (Data >>  1) ^ Data;           // Reduce 2 bits to 1 bit
  return Data & 1;
}

/*********************************************************************
*
*       _ClrDPError
*/
static void _ClrDPError(void) {
  U8 aIn[6]  = { 0x81, 0xC0, 0x03, 0x00, 0x00, 0x00 };
  U8 aDir[6] = { 0xFF, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF };

/*
  aData[0]  = 1      << 0;   // START bit
  aData[0] |= 0      << 1;   // APnDP
  aData[0] |= 0      << 2;   // Write
  aData[0] |= 0      << 3;   // ADDR[2:3]
  aData[0] |= 0      << 5;   // PARITY
  aData[0] |= 0      << 6;   // STOP
  aData[0] |= 1      << 7;   // PARK

  v |= DP_ABORT_MASK_ORUNERRCLR;            // Clear STICKYORUN flag in Ctrl/Stat register
  v |= DP_ABORT_MASK_WDERRCLR;              // Clear WDATAERR   flag in Ctrl/Stat register
  v |= DP_ABORT_MASK_STKERRCLR;             // Clear STICKYERR  flag in Ctrl/Stat register
  v |= DP_ABORT_MASK_STKCMPCLR;             // Clear STICKYCMP  flag in Ctrl/Stat register

*/

  //
  // Clear DP error flags. Does the same as typing:
  //   SWDWriteDP 0,1E
  //
  JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 48);
  JLINKARM_SWD_SyncBits();
}

/*********************************************************************
*
*       _TIF_SWD_ReadU32
*/
static int _TIF_SWD_ReadU32(U32 Index, U32* pData) {
  int BitPos;
  int Status;
  U32 Data;
  U8 aIn[6];
  U8 aDir[6];
  U8 Parity;
  U8 APnDP;

  APnDP   = (U8)(Index >> 2) & 1;
  Index  &= 3;
  //
  // Calc parity of command
  //
  Parity  = 1;                  // Read
  Parity ^= APnDP;              // APnDP
  Parity ^= (Index >> 0) & 1;   // Addr[0]
  Parity ^= (Index >> 1) & 1;   // Addr[1]
  //
  // Send Command
  //
  aIn[0]  = 1      << 0;   // START bit
  aIn[0] |= APnDP  << 1;   // APnDP
  aIn[0] |= 1      << 2;   // Read
  aIn[0] |= Index  << 3;   // ADDR[2:3]
  aIn[0] |= Parity << 5;   // PARITY
  aIn[0] |= 0      << 6;   // STOP
  aIn[0] |= 1      << 7;   // PARK
  aDir[0] = 0xFF;          // Output
  JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 8);
  //
  // Receive ACK[0:2]
  //
  aIn [0]  = 0;
  aDir[0]  = 0;
  BitPos = JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 3);
  //
  // Read data
  //
  memset(aIn,  0, 5);
  memset(aDir, 0, 4);      // Read 32 bits
  aDir[4] = 0xFC;          // Read 1 bit, 7 clocks
  JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 40);
  Status = JLINKARM_SWD_GetU8(BitPos) & 7;
  Data   = JLINKARM_SWD_GetU32(BitPos + 3);
  //
  // Check result
  //
  if (Status == ACK_OK) {
    Parity  = JLINKARM_SWD_GetU8(BitPos + 35);
    Parity &= 1;
    *pData = Data;
    if (_CalcParity(Data) != Parity) {
      _ReportOutf("ERROR: Wrong parity (Data = 0x%.8X, ReceivedParity = %d)\n", Data, Parity);
      return -1;      // Error;
    }
  }
  return Status;      // O.K.
}

/*********************************************************************
*
*       _ReadUntilOK
*/
static int _ReadUntilOK(U32 Index, U32* pData) {
  int TryCnt = 0;
  int Status;
  //
  // Continue reading until we get "OK", fault or timeout
  //
  while (1) {
    Status = _TIF_SWD_ReadU32(Index, pData);
    if (Status == ACK_OK) {
      break;
    }
    if (Status < 0) {
      return -1;      // Error (Parity)
    }
    _ClrDPError();
    if (Status != ACK_WAIT) {
      _ReportOutf("ERROR: Read from DP/AP register failed!\n");
      return -1;      // Error (Fault)
    }
    if (TryCnt++ == 10) {
      _ReportOutf("ERROR: Timeout while reading from DP/AP register!\n");
      return -1;      // Error (Timeout)
    }
  }
  return 0;           // O.K.
}

/*********************************************************************
*
*       _TIF_SWD_WriteU32
*/
static int _TIF_SWD_WriteU32(U32 Index, U32 Data) {
  int BitPos;
  int Status;
  U8 aIn[6];
  U8 aDir[6];
  U8 Parity;
  U8 APnDP;

  APnDP   = (U8)(Index >> 2) & 1;
  Index  &= 3;
  //
  // Calc parity of command
  //
  Parity  = 0;                  // Write
  Parity ^= APnDP;              // APnDP
  Parity ^= (Index >> 0) & 1;   // Addr[0]
  Parity ^= (Index >> 1) & 1;   // Addr[1]
  //
  // Send Command
  //
  aIn[0]  = 1      << 0;    // START bit
  aIn[0] |= APnDP  << 1;    // APnDP
  aIn[0] |= 0      << 2;    // Write
  aIn[0] |= Index  << 3;    // ADDR[2:3]
  aIn[0] |= Parity << 5;    // PARITY
  aIn[0] |= 0      << 6;    // STOP
  aIn[0] |= 1      << 7;    // PARK
  aDir[0] = 0xFF;           // Output
  JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 8);
  //
  // Receive ACK[0:2]
  //
  aIn [0]  = 0;
  aDir[0]  = 0;
  BitPos = JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 3);
  //
  // TURNAROUND
  //
  aIn [0]  = 0;
  aDir[0]  = 0;
  JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 2);
  //
  // Write data
  //
  Parity = _CalcParity(Data);
  *(U32*)aIn  = Data;
  aIn[4]      = Parity;
  *(U32*)aDir = 0xFFFFFFFF;
  aDir[4]     = 0xFF;
  JLINKARM_SWD_StoreRaw(&aDir[0], &aIn[0], 40);
  //
  // Check result
  //
  Status = JLINKARM_SWD_GetU8(BitPos) & 7;
  return Status;      // O.K.
}

/*********************************************************************
*
*       _JLINK_CORESIGHT_TriggerReadAPDPReg
*
*  Function description
*    Only triggers an AP/DP read by sending the read request and waiting until it has been accepted.
*    But it does not make sure that the actual read data is returned. Depending on the interface,
*    an additional read request may be required to get the actual read data from the first one.
*    This function does *not* perform this additional read request internally.
*    For a function that performs this additional read request internally and always returns the actual read data
*    use JLINKARM_CORESIGHT_ReadAPDPReg()
*
*    JTAG:
*      DP read accesses return the read data from the previous read request. The actual read data is returned on the next read request.
*      AP read accesses return the read data from the previous read request. The actual read data is returned on the next read request.
*
*    SWD:
*      DP read accesses immediately return the read data
*      AP read accesses return the read data from the previous read request. The actual read data is returned on the next read request.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _JLINK_CORESIGHT_TriggerReadAPDPReg(U8 RegIndex, U8 APnDP, U32* pData) {
  JLINK_FUNC_CORESIGHT_TRIGGER_READ_APDP_REG* pf;
  int r;

  if (pData) {     // Make sure that pData is set to some default value, even in case old DLL is used that does not support this function
    *pData = 0xFFFFFFFF;
  }
  r = JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED;
  pf = (JLINK_FUNC_CORESIGHT_TRIGGER_READ_APDP_REG*)JLINK_GetpFunc(JLINK_IFUNC_CORESIGHT_TRIGGER_READ_APDP_REG);
  if (pf) {
    r = pf(RegIndex, APnDP, pData);
  }
  return r;
}

/*********************************************************************
*
*       _TIF_SWD_Select
*/
static void _TIF_SWD_Select(void) {
  U32 Id;
  U32 CtrlStat;
  U32 v;

  JLINKARM_TIF_Select(JLINKARM_TIF_SWD);
  JLINKARM_CORESIGHT_Configure("");      // Output JTAG -> SWD switching sequence
  _JLINK_CORESIGHT_TriggerReadAPDPReg(DP_REG_IDCODE, 0, &Id);
  if ((Id & 0x0F000FFF) == 0x0B000477) {
    _ReportOutf("Found SWD-DP with ID 0x%.8X\n", Id);
  }
  //
  // Clear error flags if any are set
  //
  _JLINK_CORESIGHT_TriggerReadAPDPReg(DP_REG_CTRL_STAT, 0, &CtrlStat);
  v = 0
    | (1 << 1) // STICKYORUN
    | (1 << 4) // STICKYCMP
    | (1 << 5) // STICKYERR
    | (1 << 7) // WDATAERR
    ;
  if (CtrlStat & v) {
    JLINKARM_CORESIGHT_WriteAPDPReg(DP_REG_ABORT, 0, 0x1E);  // Clear DP errors
  }
  CtrlStat = 0
           | DP_CTRLSTAT_MASK_CSYSPWRUPREQ      // System power up
           | DP_CTRLSTAT_MASK_CDBGPWRUPREQ      // Debug power up
           ;
  JLINKARM_CORESIGHT_WriteAPDPReg(DP_REG_CTRL_STAT, 0, CtrlStat);
}

/*********************************************************************
*
*       _ExecSWDReadAP
*/
static int _ExecSWDReadAP(const char* s) {
  U32 Index;
  U32 Data;
  int r;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: SWDReadAP <RegIndex>\n"
           "Value of the AP register is returned on the NEXT transfer.\n"
           "In order to read an AP register, the following sequence is recommended:\n"
           "SWDReadAP <RegIndex>\n"
           "SWDReadDP 3\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = _JLINK_CORESIGHT_TriggerReadAPDPReg((U8)Index, 1, &Data);
  if (r >= 0) {
    _ReportOutf("Read AP register %d = 0x%.8X\n", Index, Data);
  } else {
    _ReportOutf("Read AP register %d = ERROR\n", Index);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWDReadDP
*/
static int _ExecSWDReadDP(const char* s) {
  U32 Index;
  U32 Data;
  int r;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: SWDReadDP <RegIndex>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = _JLINK_CORESIGHT_TriggerReadAPDPReg((U8)Index, 0, &Data);
  if (r >= 0) {
    _ReportOutf("Read DP register %d = 0x%.8X\n", Index, Data);
  } else {
    _ReportOutf("Read DP register %d = ERROR\n", Index);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWDWriteAP
*/
static int _ExecSWDWriteAP(const char* s) {
  U32 Index;
  U32 Data;
  int r;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: SWDWriteAP <RegIndex> <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: SWDWriteAP <RegIndex> <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_CORESIGHT_WriteAPDPReg(Index, 1, Data);
  _ReportOutf("Write AP register %d = 0x%.8X%s\n", Index, Data, (r < 0) ? " ***ERROR" : "");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWDWriteDP
*/
static int _ExecSWDWriteDP(const char* s) {
  U32 Index;
  U32 Data;
  int r;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: SWDWriteDP <RegIndex> <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: SWDWriteDP <RegIndex> <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_CORESIGHT_WriteAPDPReg(Index, 0, Data);
  _ReportOutf("Write DP register %d = 0x%.8X%s\n", Index, Data, (r < 0) ? " ***ERROR" : "");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWDSelect
*/
static int _ExecSWDSelect(const char* s) {
  U32 Id;
  U32 CtrlStat;
  U32 v;
  
  (void)s;
  //
  // Perform operation
  //
  _ReportOutf("Select SWD by sending SWD switching sequence.\n");
  JLINKARM_TIF_Select(JLINKARM_TIF_SWD);
  JLINKARM_CORESIGHT_Configure("");      // Output JTAG -> SWD switching sequence
  _JLINK_CORESIGHT_TriggerReadAPDPReg(DP_REG_IDCODE, 0, &Id);
  if ((Id & 0x0F000FFF) == 0x0B000477) {
    _ReportOutf("Found SWD-DP with ID 0x%.8X\n", Id);
  }
  //
  // Clear error flags if any are set
  //
  _JLINK_CORESIGHT_TriggerReadAPDPReg(DP_REG_CTRL_STAT, 0, &CtrlStat);
  v = 0
    | (1 << 1) // STICKYORUN
    | (1 << 4) // STICKYCMP
    | (1 << 5) // STICKYERR
    | (1 << 7) // WDATAERR
    ;
  if (CtrlStat & v) {
    JLINKARM_CORESIGHT_WriteAPDPReg(DP_REG_ABORT, 0, 0x1E);  // Clear DP errors
  }
  CtrlStat = 0
           | DP_CTRLSTAT_MASK_CSYSPWRUPREQ      // System power up
           | DP_CTRLSTAT_MASK_CDBGPWRUPREQ      // Debug power up
           ;
  JLINKARM_CORESIGHT_WriteAPDPReg(DP_REG_CTRL_STAT, 0, CtrlStat);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteAPDP
*/
static int _ExecWriteAPDP(const char* s, U8 APnDP) {
  U32 Index;
  U32 Data;
  int r;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: Write%s <RegIndex> <Data>\n", APnDP ? "AP" : "DP");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: Write%s <RegIndex> <Data>\n", APnDP ? "AP" : "DP");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Perform operation
  //
  r = JLINKARM_CORESIGHT_WriteAPDPReg(Index, APnDP, Data);
  if (r >= 0) {
    _ReportOutf("Writing %s register %d = 0x%.8X (%d write repetitions needed)\n", APnDP ? "AP" : "DP", Index, Data, r);
  } else {
    _ReportOutf("Writing failed.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteAP
*/
static int _ExecWriteAP(const char* s) {
  return _ExecWriteAPDP(s, 1);
}

/*********************************************************************
*
*       _ExecWriteDP
*/
static int _ExecWriteDP(const char* s) {
  return _ExecWriteAPDP(s, 0);
}

/*********************************************************************
*
*       _ExecReadAPDP
*/
static int _ExecReadAPDP(const char* s, U8 APnDP) {
  U32 Index;
  U32 Data;
  int r;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: Read%s <RegIndex>\n", APnDP ? "AP" : "DP");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Perform operation
  //
  r = JLINKARM_CORESIGHT_ReadAPDPReg(Index, APnDP, &Data);
  if (r >= 0) {
    _ReportOutf("Reading %s register %d = 0x%.8X (%d read repetitions needed)\n", APnDP ? "AP" : "DP", Index, Data, r);
  } else {
    _ReportOutf("Reading failed.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadAP
*/
static int _ExecReadAP(const char* s) {
  return _ExecReadAPDP(s, 1);
}

/*********************************************************************
*
*       _ExecReadDP
*/
static int _ExecReadDP(const char* s) {
  return _ExecReadAPDP(s, 0);
}

/*********************************************************************
*
*       _CompareCmd
*
*  Return value
*    0    Equal
*    1    Not equal
*/
static char _CompareCmd(const char** ps, const char* sCmd) {
  const char *s;
  s = *ps;
  do {
    char c;
    char c1;

    c  = UTIL_toupper(*sCmd++);
    c1 = UTIL_toupper(*s);
    if (c == 0) {
      if ((c1 == '\0') | (c1 == ' ') || (c1 == ',') || (c1 == '=')) {
        *ps = s;
        return 0;       // Command found
      }
      return 1;         // Command not found
    }
    if (c != c1) {
      return 1;         // Command not found
    }
    s++;
  } while (1);
}

/*********************************************************************
*
*       _GetManufacturer
*/
static const char * _GetManufacturer(U32 DeviceID) {
  int i;
  U16 ManufacturerID;

  ManufacturerID = (DeviceID >> 1) & 0x7FF;
  for (i = 0; i < COUNTOF(_aManufacturer); i++) {
    if (_aManufacturer[i].ManufacturerID == ManufacturerID) {
      return _aManufacturer[i].sName;
    }
  }
  return NULL;
}

/*********************************************************************
*
*       _SetIFSpeedPerformConnect
*
*  Function description
*    Sets the interface speed and performs a reconnect
*
*  Parameters
*    Speed: Interface speed to be used
*
*  Return value
*    == 0  O.K.
*     < 0  Error
*/
static int _SetIFSpeedPerformConnect(const U32 Speed) {
  int r;
  int IsConnected;
  //
  // The JLINKARM_SetSpeed function needs to perform a CPU_Identify() in case the speed
  // has been set to auto (necessary in order to auto-detect the maximum speed).
  //
  // We have the following four possible scenarios:
  //   Fixed interface speed:
  //     1. Connection already open --> Perform Connect()
  //     2. Connection not open yet --> Perform Connect()
  //   Auto interface speed:
  //     3. Connection already open --> Perform Connect()
  //     4. Connection not open yet --> Perform SetSpeed() only which is calling a implicit Connect()
  // 
  // This way, we can make sure that the DLL does not perform two connects in case of auto is selected. 
  //
  IsConnected = JLINKARM_IsConnected();
  JLINKARM_SetSpeed(Speed);
  if ((IsConnected == 0) && (Speed == JLINKARM_SPEED_AUTO)) {
    r = JLINKARM_IsConnected();
    r = (r == 1) ? 0 : -1;
  } else {
    r = JLINKARM_Connect();   // Establish target connection
    if (r == JLINK_ERR_VCC_FAILURE) {
      _ReportOutf("Target voltage too low. Please check https://wiki.segger.com/J-Link_cannot_connect_to_the_CPU#Target_connection.\n");
    }
  }
  return r;
}

/*********************************************************************
*
*       _ShowCoreID
*/
static char _ShowCoreID(_SETTINGS* pCommanderSettings) {
  char acTmp[256];
  JTAG_ID_DATA IdData = {0};
  U32 Core = 0;
  U32 DevFamily;
  U32 InterfaceMask;
  U32 SelectedTIF;
  int r;
  //
  // If the emulator supports multiple interfaces, check which ones are supported,
  // since there are some J-Links out there which do not support JTAG (because they have been designed for some other interfaces only)
  // For J-Links which do not support the select interface command, we can assume JTAG is available.
  //
  SelectedTIF = JLINKARM_TIF_JTAG;
  if (JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_SELECT_IF) {
    JLINKARM_TIF_GetAvailable(&InterfaceMask);
  } else {
    InterfaceMask = (1 << JLINKARM_TIF_JTAG);
  }
  //
  // If no interface has been explicitly set, leave auto-connect as it is to avoid problems when starting J-Link Commander in parallel to IAR
  //
  if (pCommanderSettings->TargetIFSet) {                                   // Specific interface pre-selected?
    if ((InterfaceMask & (1 << pCommanderSettings->TargetIF)) == 0) {      // Selected interface not supported?
      _ReportOutf("Selected interface (%s) is not supported by the connected probe.\n", _TIF2Name(pCommanderSettings->TargetIF));
      return 1;
    }
    JLINKARM_TIF_Select(pCommanderSettings->TargetIF);
    SelectedTIF = pCommanderSettings->TargetIF;
    //
    // If interface has been selected explicitly , do NOT perform auto connect sequence of Commander
    //
    _ReportOutf("Connecting to target via %s\n", _TIF2Name(SelectedTIF));
    r = _SetIFSpeedPerformConnect(pCommanderSettings->InitTIFSpeedkHz);
    JLINKARM_GetIdData(&IdData);
    if ((IdData.NumDevices == 0) || r) {
      _ReportOutf("Cannot connect to target.\n");
      return 1;
    }
    if (r == 0) {
      _ShowMemZones();
    }
  } else {
    if ((InterfaceMask & (1 << JLINKARM_TIF_JTAG)) || (InterfaceMask & (1 << JLINKARM_TIF_SWD))) {
      SelectedTIF = JLINKARM_TIF_JTAG;
      _ReportOutf("Connecting to target via %s\n", _TIF2Name(SelectedTIF));
      r = _SetIFSpeedPerformConnect(pCommanderSettings->InitTIFSpeedkHz);
      JLINKARM_GetIdData(&IdData);
      if ((IdData.NumDevices == 0) || r) {
        if((InterfaceMask & (1 << JLINKARM_TIF_SWD))) {    // Check if SWD is supported
          _ReportOutf("No devices found on JTAG chain. Trying to find device on SWD.\n");
          JLINKARM_ClrError();
          JLINKARM_TIF_Select(JLINKARM_TIF_SWD);
          SelectedTIF = JLINKARM_TIF_SWD;
          _ReportOutf("Connecting to target via %s\n", _TIF2Name(SelectedTIF));
          r = _SetIFSpeedPerformConnect(pCommanderSettings->InitTIFSpeedkHz);
          Core = JLINKARM_CORE_GetFound();
          if ((Core == 0) || r) {
            _ReportOutf("No device found on SWD.\n");
            if (InterfaceMask & (1 << JLINKARM_TIF_JTAG)) {  // Do not try to select JTAG if we have a SWD-only J-Link
              JLINKARM_TIF_Select(JLINKARM_TIF_JTAG);
            }
          }
        } else {
          _ReportOutf("No devices found on JTAG chain. SWD is not supported by this emulator.\n");
        }
      }
      if (r == 0) {
        _ShowMemZones();
      }
    }
    if (InterfaceMask & (1 << JLINKARM_TIF_FINE)) {
      if ((IdData.NumDevices == 0) && Core == 0) {   // If no device has been found so far, try to find device on UART interface
        _ReportOutf("Trying to find device on FINE interface.\n");
        JLINKARM_TIF_Select(JLINKARM_TIF_FINE);
        SelectedTIF = JLINKARM_TIF_FINE;
        _ReportOutf("Connecting to target via %s\n", _TIF2Name(SelectedTIF));
        r = _SetIFSpeedPerformConnect(pCommanderSettings->InitTIFSpeedkHz);
        JLINKARM_GetIdData(&IdData);
        if ((IdData.aId[0] == 0) || r) {
          _ReportOutf("No device found on FINE interface.\n");
        }
        if (r == 0) {
          _ShowMemZones();
        }
      }
    }
  }
  //
  // Current versions of the J-Link DLL already output findings for JTAG via Report() function,
  // so they are no longer only available in the J-Link Commander but from all IDEs + the logfile
  //
  if (JLINKARM_HasError() == 0) {
    Core = JLINKARM_CORE_GetFound();
    JLINKARM_Core2CoreName(Core, acTmp, sizeof(acTmp));
    _ReportOutf("%s identified.\n", acTmp);
    DevFamily = (Core >> 24) & 0xFF;
    if ((DevFamily == JLINKARM_DEV_FAMILY_ARM7) || (DevFamily == JLINKARM_DEV_FAMILY_ARM9)) {
      if (JLINKARM_ETM_IsPresent()) {
        _ShowETMConfig();
        _ShowETBConfig();
      }
    }
    return 0;   // O.K.
  }
  return 1;       // Error
}

/*********************************************************************
*
*       _HasEmuEthernet
*
*  Return value
*     0     Emulator has no ethernet interface
*     1     Emulator has ethernet interface
*/
static int _HasEmuEthernet(void) {
  char acFW[256];
  int ProdId;
  int HWVersion;
  int HWVersionMajor;
  int r;

  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_ETHERNET)) {
    return 1;
  }
  r = 0;
  ProdId         = JLINKARM_EMU_GetProductId();
  HWVersion      = JLINKARM_GetHardwareVersion();
  HWVersionMajor = HWVersion / 10000 % 100;

  if (ProdId < 0) {  // On error
    return r;
  }
  switch(ProdId) {
  case JLINK_EMU_PRODUCT_ID_FLASHER_ARM:
    if (HWVersionMajor >= 3) {
      r = 1;
    }
    break;
  case JLINK_EMU_PRODUCT_ID_JLINK_PRO:
  case JLINK_EMU_PRODUCT_ID_FLASHER_PPC:
  case JLINK_EMU_PRODUCT_ID_FLASHER_RX:
    r = 1;
    break;
  default:
    JLINKARM_GetFirmwareString(acFW,256);
    if (memcmp(acFW, "Ember ", 6) == 0) {  // Ember hardware features TCP/IP functionality but does not have a J-Link compliant product ID
      r = 1;
    }
  }
  return r;
}

/*********************************************************************
*
*       _GetEmuIPAddr
*
*  Return value
*    -1     Error
*     0     O.K.: IP addr. is fixed
*     1     O.K.: IP addr. is assigned by DHCP but not yet ready
*     2     O.K.: IP addr. is assigned by DHCP
*/
static int _GetEmuIPAddr(U32* pIPAddr) {
  int r;
  int Result;
  U32 IPByte[4];
  //
  // Early out if the connected probe does not support IP
  //
  Result = -1;
  r = _HasEmuEthernet();
  if (r == 0) {
    _ReportOutf("This hardware does not support ethernet.\n");
    Result = -1;
    goto Done;
  }
  r = JLINKARM_ReadEmuConfigMem((unsigned char*)pIPAddr, 0x20, 4);
  if (r != 0) {    // Testing successfully reading has to be done only once
    if (r == 2) {
      _ReportOutf("This feature is not supported by connected J-Link.\n");
    } else {
      _ReportOutf("Failed to read configuration.\n");
    }
    Result = -1;  // Error
    goto Done;
  } else {
    //
    // Read rest of configuration
    //
    if (*pIPAddr != 0xFFFFFFFF) {  // DHCPc is not active, we have set IP addr. manually
      Result = 0;
    } else {
      JLINKARM_GetHWInfo((1 << HW_INFO_IP_ADDR), pIPAddr);
      if (*pIPAddr == 0) {         // IP addr. is assigned by DHCP but not yet ready
        Result = 1;
      } else {                     // IP addr. has been assigned by DHCP
        IPByte[0]   =  *pIPAddr             >> 24;
        IPByte[1]   = (*pIPAddr & 0xFF0000) >> 16;
        IPByte[2]   = (*pIPAddr & 0x00FF00) >> 8;
        IPByte[3]   =  *pIPAddr & 0x0000FF;
        *pIPAddr    = (IPByte[0] << 24) | (IPByte[1] << 16) | (IPByte[2] <<  8) | (IPByte[3]);
        Result = 2;
      }
    }
  }
Done:
  return Result;
}

/*********************************************************************
*
*       _DisconnectFromTarget
*
*  Function description
*    Closes connection to target without closing connection to J-Link.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _DisconnectFromTarget(_SETTINGS* pCommanderSettings) {
  _ReportOutf("Disconnecting from J-Link...");
  //
  // TBD: Create extra API function that only disconnects from the target
  //
  pCommanderSettings->Status.ConnectedToTarget = 0;
  _ReportOutf("O.K.\n");
  return 0;
}

/*********************************************************************
*
*       _ConnectToTarget
*
*  Function description
*    Establish connection to target device by specifying device, target interface etc.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _ConnectToTarget(_SETTINGS* pCommanderSettings) {
  char                        ac[1024];
  char                        acDevName[256];
  char                        c0;
  char*                       ps;
  const U32*                  paSuppCores;
  int                         NumEntries;
  int                         i;
  int                         r;
  int                         Found;
  U32                         SelectedCore;
  U32                         DefaultTIFSpeed;
  JLINK_FUNC_MRU_GETLIST*     pfGetMRU;
  JLINKARM_DEVICE_SELECT_INFO DeviceSelectInfo;
  JLINKARM_DEVICE_INFO        DevInfo;

  DefaultTIFSpeed    = DEFAULT_TIF_SPEED_KHZ;        // Set default speed for all TIFs without a TIF specific default speed.
  pCommanderSettings = &_CommanderSettings;
  //
  // Ask for device / core name, if not already specified
  //
  if (pCommanderSettings->acDeviceName[0] == 0) {
    //
    // If the DLL provides the possibility to get the MRU device names, we show the latest one
    //
    acDevName[0] = 0;
    pfGetMRU     = (JLINK_FUNC_MRU_GETLIST*)JLINK_GetpFunc(JLINK_IFUNC_MRU_GETLIST);
    if (pfGetMRU) {
      r = pfGetMRU(JLINK_MRU_GROUP_DEVICE, ac, sizeof(ac));
      if (r >= 0) {
        _UTIL_CopyString(acDevName, ac, _SIZEOF(acDevName));
      }
    }
    //
    // No MRU device present, assume "Unspecified" as device
    //
    if (acDevName[0] == 0) {
      _UTIL_CopyString(acDevName, "Unspecified", _SIZEOF(acDevName));
    }
    //
    // Get user input, if any
    //
    _ReportOutf("Please specify device / core. <Default>: %s\n", acDevName);
    _ReportOut("Type '?' for selection dialog\n");
    r = SYS_ConsoleGetString("Device>", ac, sizeof(ac));
    if (r < 0) {
      _CommanderSettings.CloseRequested = 1;
      return 0;
    }
    ps = ac;
    if (*ps) {
      _ParseString((const char**)&ps, acDevName, sizeof(acDevName));
      if (acDevName[0] == '?') {
        DeviceSelectInfo.SizeOfStruct = sizeof(JLINKARM_DEVICE_SELECT_INFO);
        r = JLINKARM_DEVICE_SelectDialog(NULL, 0, &DeviceSelectInfo);
        if (r >= 0) {
          if (DeviceSelectInfo.CoreIndex > 0) {
            pCommanderSettings->CoreIndex = DeviceSelectInfo.CoreIndex;
          }
          memset(&DevInfo, 0, sizeof(DevInfo));
          DevInfo.SizeOfStruct = sizeof(JLINKARM_DEVICE_INFO);
          JLINKARM_DEVICE_GetInfo(r, &DevInfo);
          _UTIL_CopyString(acDevName, DevInfo.sName, _SIZEOF(acDevName));
        }
      }
    }
    //
    // Remember selection in commander settings
    //
    _UTIL_CopyString(pCommanderSettings->acDeviceName, acDevName, _SIZEOF(pCommanderSettings->acDeviceName));
  }
  //
  // Ask for target interface to use, if not already specified
  //
  if (pCommanderSettings->TargetIFSet == 0) {
    _ReportOut("Please specify target interface:\n");
    //
    // Show TIF list, prefiltered by selected core so we do not show interfaces
    // that are not supported by the currently selected device
    // If we cannot retrieve any information about the currently selected device, show all TIFs
    //
    r = JLINKARM_DEVICE_GetIndex(pCommanderSettings->acDeviceName);
    SelectedCore = JLINK_CORE_ANY;
    if (r >= 0) {
      memset(&DevInfo, 0, sizeof(DevInfo));
      DevInfo.SizeOfStruct = sizeof(JLINKARM_DEVICE_INFO);
      JLINKARM_DEVICE_GetInfo(r, &DevInfo);
      SelectedCore = DevInfo.Core;
    }
    //
    // Go through TIF list
    //
    NumEntries = COUNTOF(_aTIFList);
    r = 0;
    for (i = 0; i < NumEntries; i++) {
      Found = 0;
      //
      // Go through core filter for each TIF
      // 0-entry marks end of core filter list
      //
      paSuppCores = _aTIFList[i].paSupportedCores;
      if (paSuppCores) {
        do {
          if (*paSuppCores == 0) {
            break;
          }
          if (*paSuppCores == SelectedCore) {
            Found = 1;
            break;
          }
          paSuppCores++;
        } while (1);
      } else {
        Found = 1;
      }
      if (Found) {
        //
        // Select first supported interface as default
        //
        if (r == 0) {
          pCommanderSettings->TargetIF = _aTIFList[i].TIFVal;
          pCommanderSettings->TargetIFSet = 1;
        }
        _ReportOutf("  %c) %s%s\n", _aTIFList[i].Shortcut, _aTIFList[i].sName, (r == 0) ? " (Default)" : "");
        r++;
      }
    }
    if (r == 0) {
      _ReportOutf("No suggested target interface for core: 0x%X !\n", SelectedCore);
      //
      // Show all available TIF.
      //
      _ReportOutf("Please specify target interface manually:\n", SelectedCore);
      for (i = 0; i < NumEntries; i++) {
        _ReportOutf("  %c) %s%s\n", _aTIFList[i].Shortcut, _aTIFList[i].sName, "");
      }
    }
    //
    // Evaluate selection
    //
    do {
      r = SYS_ConsoleGetString("TIF>", ac, sizeof(ac));
      if (r < 0) {
        _CommanderSettings.CloseRequested = 1;
        return 0;
      }
      ps = ac;
      if (*ps) {
        Found = 0;
        for (i = 0; i < NumEntries; i++) {
          c0 = UTIL_toupper(ac[0]);
          if (c0 == _aTIFList[i].Shortcut) {
            Found = 1;
            break;
          }
        }
        if (Found) {
          pCommanderSettings->TargetIF = _aTIFList[i].TIFVal;
          pCommanderSettings->TargetIFSet = 1;
          break;
        }
      } else {
        //
        // Use default
        //
        break;
      }
    } while (1);
  }
  //
  // Special for JTAG: Ask for position of device in JTAG chain, if not already specified by user
  //
  if ((pCommanderSettings->TargetIF == JLINKARM_TIF_JTAG) || (pCommanderSettings->TargetIF == JLINKARM_TIF_CJTAG) || (pCommanderSettings->TargetIF == JLINKARM_TIF_MC2WJTAG_TDI)) {
    if (pCommanderSettings->JTAGConfSet == 0) {
      //
      // Default: Take first supported core in JTAG chain
      //
      pCommanderSettings->JTAGConfIRPre = -1;
      pCommanderSettings->JTAGConfDRPre = -1;
      _ReportOut("Device position in JTAG chain (IRPre,DRPre) <Default>: -1,-1 => Auto-detect\n");
      r = SYS_ConsoleGetString("JTAGConf>", ac, sizeof(ac));
      if (r < 0) {
        _CommanderSettings.CloseRequested = 1;
        return 0;
      }
      ps = ac;
      if (*ps) {
        if (_ParseDec((const char**)&ps, (U32*)&pCommanderSettings->JTAGConfIRPre)) {
          _ReportOutf("ERROR while parsing value for IRPre. Using default: %d.\n", pCommanderSettings->JTAGConfIRPre);
        }
        if (*ps == ',') {     // Skip comma between JTAG config params
          ps++;
        }
        if (_ParseDec((const char**)&ps, (U32*)&pCommanderSettings->JTAGConfDRPre)) {
          _ReportOutf("ERROR while parsing value for DRPre. Using default: %d.\n", pCommanderSettings->JTAGConfDRPre);
        }
      }
      pCommanderSettings->JTAGConfSet = 1;
    }
    JLINKARM_ConfigJTAG(pCommanderSettings->JTAGConfIRPre, pCommanderSettings->JTAGConfDRPre);
  }
  //
  // Some interfaces should use a different default speed
  //
  if (pCommanderSettings->TargetIF == JLINKARM_TIF_FINE) {
    DefaultTIFSpeed = DEFAULT_TIF_SPEED_KHZ_FINE;
  } else if (pCommanderSettings->TargetIF == JLINKARM_TIF_C2) {
    DefaultTIFSpeed = DEFAULT_TIF_SPEED_KHZ_C2;
  }
  //
  // Ask for interface speed, if not already specified
  //
  if (pCommanderSettings->InitTIFSpeedkHzSet == 0) {
    _ReportOutf("Specify target interface speed [kHz]. <Default>: %d kHz\n", DefaultTIFSpeed);
    pCommanderSettings->InitTIFSpeedkHz    = DefaultTIFSpeed;
    pCommanderSettings->InitTIFSpeedkHzSet = 1;
    r = SYS_ConsoleGetString("Speed>", ac, sizeof(ac));
    if (r < 0) {
      _CommanderSettings.CloseRequested = 1;
      return 0;
    }
    if (ac[0]) {
      _EatWhite((const char**)&ps);
      //
      // Special speeds: "auto" and "adaptive"
      //
      if (_JLinkstricmp(ps, "adaptive") == 0) {
        pCommanderSettings->InitTIFSpeedkHz = JLINKARM_SPEED_ADAPTIVE;
      } else if (_JLinkstricmp(ps, "auto") == 0) {
        pCommanderSettings->InitTIFSpeedkHz = JLINKARM_SPEED_AUTO;
      } else {
        if (_ParseDec((const char**)&ps, &pCommanderSettings->InitTIFSpeedkHz)) {
          _ReportOutf("ERROR while parsing value for speed. Using default: %d kHz.\n", DefaultTIFSpeed);
        }
      }
    }
  }
  //
  // Set settings file
  //
  if (pCommanderSettings->acSettingsFile[0]) {
    char acSettingsFile[_MAX_PATH];

    UTIL_snprintf(acSettingsFile, UTIL_SIZEOF(acSettingsFile), "ProjectFile = %s", pCommanderSettings->acSettingsFile);
    JLINKARM_ExecCommand(acSettingsFile, NULL, 0);
  }
  //
  // Pass device selection to DLL
  //
  if (pCommanderSettings->acDeviceName[0]) {
    UTIL_snprintf(ac, UTIL_SIZEOF(ac), "device=%s", pCommanderSettings->acDeviceName);
    r = JLINKARM_ExecCommand(ac, NULL, 0);
    //
    // No valid device has been selected?
    //
    if (r < 0) {
      _ReportOut("No valid device has been selected.\n");
      _UTIL_CopyString(pCommanderSettings->acDeviceName, "", _SIZEOF(pCommanderSettings->acDeviceName));
      return -1;
    }
    //
    // Set core index if specified
    //
    if (pCommanderSettings->CoreIndex > 0) {
      UTIL_snprintf(ac, UTIL_SIZEOF(ac), "SetCoreIndex=%i", pCommanderSettings->CoreIndex);
      r = JLINKARM_ExecCommand(ac, NULL, 0);
    }
  }
  //
  // Connect to device
  //
  JLINKARM_EnableLog(_Log);
  _ReportOut("\n\n");
  r = _ShowCoreID(pCommanderSettings);
  if (r == 0) {
    pCommanderSettings->Status.ConnectedToTarget = 1;
    return 0;
  }
  return -1;
}

/*********************************************************************
*
*       _DisconnectFromJLink
*
*  Function description
*    Closes connection to J-Link
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _DisconnectFromJLink(_SETTINGS* pCommanderSettings) {
  _ReportOutf("Disconnecting from J-Link...");
  JLINKARM_Close();
  pCommanderSettings->Status.ConnectedToJLink = 0;
  _ReportOutf("O.K.\n");
  return 0;
}

/*********************************************************************
*
*       _ConnectToJLink
*
*  Function description
*    Establish connection to J-Link *without* connecting to the target device.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*
*  Notes
*    (1) No connection to target is established
*/
static int _ConnectToJLink(_SETTINGS* pCommanderSettings) {
  JLINK_FUNC_EMU_GET_CURR_CONN_INFO* pfCurrConnInfo;
  JLINK_EMU_CURR_CONNECT_INFO CurrConnInfo;
  JLINKARM_HW_STATUS Stat;
  const char* sUSBMode;
  const char *s;
  char ac[1024];
  char acScriptFile[_MAX_PATH];
  U32 IPAddr;
  int v;
  int r;
  int VerHW;
  int t;
  //
  // Setup host interface (HIF)
  //
  _ReportOutf("Connecting to J-Link via %s...", (pCommanderSettings->HostIF == JLINKARM_HOSTIF_IP) ? "IP" : "USB");
  r = 0;
  if (pCommanderSettings->HostIF == JLINKARM_HOSTIF_IP) {
    r = JLINKARM_SelectIP(pCommanderSettings->acIPAddr, 0);
    r = (r != 0) ? -1 : 0;                                      // Convert error to standard commander internal error value (< 0)
  } else {
    //
    // Check if we need to connect to a specific J-Link (S/N specified or old USB0-3 enumeration method)
    // S/N 0-3 are interpreted as old USB0-3 connection method
    //
    if (pCommanderSettings->IsSetEmuSN) {
      if (pCommanderSettings->EmuSN > 3) {
        r = JLINKARM_EMU_SelectByUSBSN(pCommanderSettings->EmuSN);
      } else {
        r = JLINKARM_SelectUSB(pCommanderSettings->EmuSN);
        r = (r != 0) ? -1 : 0;                                      // Convert error to standard commander internal error value (< 0)
      }
    }
  }
  //
  // If DLL already reported an error during HIF setup, we are done
  //
  if (r < 0) {
    _ReportOut("FAILED\n");
    return -1;
  }
  //
  // Execute commands required before opening a connection to J-Link
  //
  // Suppress GUI elements if required
  //
  JLINKARM_SetWarnOutHandler(_cbWarnOut);
  if (_SuppressGUI) {
    JLINKARM_ExecCommand("SuppressGUI", NULL, 0);
  }
  //
  // Set J-Link script file.
  // This is required before opening the connection to the J-Link,
  // because it might contain functions, that are supposed to be executed
  // before (e.g. InitEMU()).
  //
  if (pCommanderSettings->acJLinkScriptFile[0]) {
    _UTIL_CopyString(acScriptFile, "scriptfile = ", _SIZEOF(acScriptFile));
    _UTIL_CopyString(acScriptFile + UTIL_strlen(acScriptFile), pCommanderSettings->acJLinkScriptFile, _SIZEOF(acScriptFile) - UTIL_strlen(acScriptFile));
    ac[0] = 0;
    JLINKARM_ExecCommand(acScriptFile, &ac[0], sizeof(ac));
  }
  //
  // Open actual connection to J-Link (not to target yet!)
  //
  t = SYS_GetTickCount();
  s = JLINKARM_OpenEx(_Log, _cbErrorOut);
  t = SYS_GetTickCount() - t;
  if (s) {
    _ReportOutf("FAILED: %s\n", s);
    return -1;
  }
  _ReportOut("O.K.\n");
#if MEASURE_DLL_INIT_TIME
  _ReportOutf("DEBUG: Init DLL (%dms)\n", t);
#else
  (void)t;
#endif
  //
  // Set Progressbar callback if -NoGui was passed
  //
  if(_SuppressGUI) {
    JLINK_SetFlashProgProgressCallback(_cbFlashProgProgress);
  }
  //
  // Show J-Link info (S/N, IP, OEM etc.)
  //
  JLINKARM_GetFirmwareString(ac, sizeof(ac));
  _ReportOutf("Firmware: %s\n", ac);
  VerHW = JLINKARM_GetHardwareVersion();
  _ReportOutf("Hardware version: V%d.%.2d\n", VerHW / 10000 % 100, VerHW / 100 % 100);
  _ReportOutf("S/N: %d\n", JLINKARM_GetSN());
  JLINKARM_GetFeatureString(&ac[0]);
  if (UTIL_strlen(ac)) {
    _ReportOutf("License(s): %s\n", ac);
  }
  JLINKARM_GetOEMString(&ac[0]);
  if (UTIL_strlen(ac)) {
    _ReportOutf("OEM: %s\n", ac);
  }
  pfCurrConnInfo = (JLINK_FUNC_EMU_GET_CURR_CONN_INFO*)JLINK_GetpFunc(JLINK_IFUNC_EMU_GET_CURR_CONN_INFO);
  if (pfCurrConnInfo) {                 // Older DLLs do not provide this function pointer
    r = pfCurrConnInfo(&CurrConnInfo);
    if (r >= 0) {
      if (CurrConnInfo.USBSpeedMode) {   // == 0 indicates that we could not determine the USB mode, J-Link is currently using. (Also the case when connected via IP)
        switch (CurrConnInfo.USBSpeedMode) {
        case 1:  sUSBMode = "Low speed (1.5 MBit/s)";   break;
        case 2:  sUSBMode = "Full speed (12 MBit/s)";   break;
        case 3:  sUSBMode = "High speed (480 MBit/s)";  break;
        case 4:  sUSBMode = "Super speed (5 GBit/s)";   break;
        case 5:  sUSBMode = "Super speed+ (10 GBit/s)"; break;
        default: sUSBMode = NULL;                       break;
        }
        if (sUSBMode) {
          _ReportOutf("USB speed mode: %s\n", sUSBMode);
        }
      }
    }
  }
  if (_HasEmuEthernet()) {
    v = _GetEmuIPAddr(&IPAddr);
    switch(v) {
    case 0:  _ReportOutf("IP-Addr: %d.%d.%d.%d\n", (IPAddr >> 24) & 0xFF, (IPAddr >> 16) & 0xFF, (IPAddr >>  8) & 0xFF, (IPAddr >>  0) & 0xFF);            break;
    case 1:  _ReportOutf("IP-Addr: DHCP (no addr. received yet)\n");                                                                                       break;
    case 2:  _ReportOutf("IP-Addr: %d.%d.%d.%d (DHCP)\n", (IPAddr >> 24) & 0xFF, (IPAddr >> 16) & 0xFF, (IPAddr >>  8) & 0xFF, (IPAddr >>  0) & 0xFF);     break;
    default: _ReportOutf("IP-Addr: INVALID\n");                                                                                                            break;
    }
  }
  if (JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_TRACE) {
    _ReportOutf("Emulator has Trace capability\n");
  } else if (JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_RAWTRACE) {    // Due to compatibility reasons, some probes need to report both caps, therefore else if ()
    _ReportOutf("Emulator has RAWTRACE capability\n");
  }
  //
  // Show target voltage
  //
  JLINKARM_GetHWStatus(&Stat);
  _ShowVTref(Stat.VTarget);
  pCommanderSettings->Status.ConnectedToJLink = 1;
  //
  // Set settings file
  // We have to set the setting file right after JLINKARM_OpenEx() to make sure that
  // it is already available when parsing the JLinkDevices.xml
  //
  if (pCommanderSettings->acSettingsFile[0]) {
    char acSettingsFile[_MAX_PATH];

    UTIL_snprintf(acSettingsFile, UTIL_SIZEOF(acSettingsFile), "ProjectFile = %s", pCommanderSettings->acSettingsFile);
    JLINKARM_ExecCommand(acSettingsFile, NULL, 0);
  }
  return 0;
}

/*********************************************************************
*
*       _GetEmuIPMask
*
*  Return value
*    -1     Error
*     0     O.K.: IP mask is fixed
*     1     O.K.: IP mask is assigned by DHCP but not yet ready
*     2     O.K.: IP mask is assigned by DHCP
*/
static int _GetEmuIPMask(U32* IPMask) {
  int r;
  U32 v;

  if (_HasEmuEthernet()) {
    r = JLINKARM_ReadEmuConfigMem((unsigned char*)&v, 0x20, 4);
    if (r != 0) {    // Testing successfully reading has to be done only once
      if (r == 2) {
        _ReportOutf("This feature is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to read configuration.\n");
      }
      return -1;  // Error
    } else {
      //
      // Read rest of configuration
      //
      if (v != 0xFFFFFFFF) {  // DHCPc is not active, we have set IP addr. manually
        JLINKARM_ReadEmuConfigMem((unsigned char*)IPMask, 0x24, 4);
        return 0;  // Addr. fixed
      } else {
        JLINKARM_GetHWInfo((1 << HW_INFO_IP_MASK), IPMask);
        if (*IPMask == 0) {
          return 1;  // IP mask is assigned by DHCP but not yet ready
        } else {
          return 2;  // IP mask has been assigned by DHCP
        }
      }
    }
  } else {
    _ReportOutf("This hardware does not support ethernet.\n");
    return -1;  // Error
  }
}

/*********************************************************************
*
*       _GetEmuIPGateway
*
*  Return value
*    -1     Error
*     0     O.K.: GW is fixed
*     1     O.K.: GW is assigned by DHCP but not yet ready
*     2     O.K.: GW is assigned by DHCP
*/
static int _GetEmuIPGateway(U32* Gateway) {
  int r;
  U32 v;

  if (_HasEmuEthernet()) {
    r = JLINKARM_ReadEmuConfigMem((unsigned char*)&v, 0x20, 4);
    if (r != 0) {    // Testing successfully reading has to be done only once
      if (r == 2) {
        _ReportOutf("This feature is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to read configuration.\n");
      }
      return -1;  // Error
    } else {
      //
      // Read rest of configuration
      //
      if (v != 0xFFFFFFFF) {  // DHCPc is not active, we have set IP addr. manually
        JLINKARM_ReadEmuConfigMem((unsigned char*)&v, 0x40, 4);
        if (v == 0xFFFFFFFF) {
          *Gateway = 0;
        } else {
          *Gateway = v;
        }
        return 0;  // Addr. fixed
      } else {
        JLINKARM_GetHWInfo((1 << HW_INFO_IP_GW), Gateway);
        if (*Gateway == 0) {
          return 1;  // IP mask is assigned by DHCP but not yet ready
        } else {
          return 2;  // IP mask has been assigned by DHCP
        }
      }
    }
  } else {
    _ReportOutf("This hardware does not support ethernet.\n");
    return -1;  // Error
  }
}

/*********************************************************************
*
*       _GetEmuIPDNS0
*
*  Return value
*    -1     Error
*     0     O.K.: GW is fixed
*     1     O.K.: GW is assigned by DHCP but not yet ready
*     2     O.K.: GW is assigned by DHCP
*/
static int _GetEmuIPDNS0(U32* DNSServer) {
  int r;
  U32 v;

  if (_HasEmuEthernet()) {
    r = JLINKARM_ReadEmuConfigMem((unsigned char*)&v, 0x20, 4);
    if (r != 0) {    // Testing successfully reading has to be done only once
      if (r == 2) {
        _ReportOutf("This feature is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to read configuration.\n");
      }
      return -1;  // Error
    } else {
      //
      // Read rest of configuration
      //
      if (v != 0xFFFFFFFF) {  // DHCPc is not active, we have set IP addr. manually
        JLINKARM_ReadEmuConfigMem((unsigned char*)&v, 0x48, 4);
        if (v == 0xFFFFFFFF) {
          *DNSServer = 0;
        } else {
          *DNSServer = v;
        }
        return 0;  // Addr. fixed
      } else {
        JLINKARM_GetHWInfo((1 << HW_INFO_IP_DNS0), DNSServer);
        if (*DNSServer == 0) {
          return 1;  // IP mask is assigned by DHCP but not yet ready
        } else {
          return 2;  // IP mask has been assigned by DHCP
        }
      }
    }
  } else {
    _ReportOutf("This hardware does not support ethernet.\n");
    return -1;  // Error
  }
}

/*********************************************************************
*
*       _ExecSelectUSB
*/
static int _ExecSelectUSB(const char* s) {
  U32 Port = 0;
  int r;
  unsigned WasConnectedToJLink;
  unsigned WasConnectedToTarget;
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &Port)) {
      _ReportOutf("Syntax: usb <port>, where port is 0..3\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Disconnect from target + J-Link
  // Update commander settings
  // Reconnect to J-Link + target
  //
  WasConnectedToJLink = pCommanderSettings->Status.ConnectedToJLink;
  WasConnectedToTarget = pCommanderSettings->Status.ConnectedToTarget;
  if (WasConnectedToTarget) {
    _DisconnectFromTarget(pCommanderSettings);
  }
  if (WasConnectedToJLink) {
    _DisconnectFromJLink(pCommanderSettings);
  }
  pCommanderSettings->HostIF      = JLINKARM_HOSTIF_USB;
  pCommanderSettings->acIPAddr[0] = 0;
  pCommanderSettings->EmuSN       = Port;
  pCommanderSettings->IsSetEmuSN  = 1;
  r = _ConnectToJLink(pCommanderSettings);
  if (WasConnectedToTarget && (r >= 0)) {
    r = _ConnectToTarget(pCommanderSettings);
  }
  return (r < 0) ? JLINK_ERROR_UNKNOWN : JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSelectIP
*/
static int _ExecSelectIP(const char* s) {
  char acHost[256];
  int r;
  unsigned WasConnectedToJLink;
  unsigned WasConnectedToTarget;
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;
  _ParseString(&s, acHost, sizeof(acHost));
  if (acHost[0]) {
    _ReportOutf("Connecting to %s\n", acHost);
  } else {
    _ReportOutf("Connecting via TCP/IP\n");
  }
  //
  // Disconnect from target + J-Link
  // Update commander settings
  // Reconnect to J-Link + target
  //
  WasConnectedToJLink = pCommanderSettings->Status.ConnectedToJLink;
  WasConnectedToTarget = pCommanderSettings->Status.ConnectedToTarget;
  if (WasConnectedToTarget) {
    _DisconnectFromTarget(pCommanderSettings);
  }
  if (WasConnectedToJLink) {
    _DisconnectFromJLink(pCommanderSettings);
  }
  pCommanderSettings->HostIF = JLINKARM_HOSTIF_IP;
  pCommanderSettings->EmuSN    = 0;
  pCommanderSettings->IsSetEmuSN  = 1;
  _UTIL_CopyString(&_CommanderSettings.acIPAddr[0], acHost, _SIZEOF(_CommanderSettings.acIPAddr));
  r = _ConnectToJLink(pCommanderSettings);
  if (WasConnectedToTarget && (r >= 0)) {
    r = _ConnectToTarget(pCommanderSettings);
  }
  return (r < 0) ? JLINK_ERROR_UNKNOWN : JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSleep
*/
static int _ExecSleep(const char* s) {
  U32 ms;
  if (_ParseDec(&s, &ms)) {
    _ReportOutf("Syntax: Sleep <ms>\n");
  } else {
    _ReportOutf("Sleep(%d)\n", ms);
    _Sleep(ms);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecMACAddressSyntaxError
*/
static void _ExecMACAddressSyntaxError(void) {
  _ReportOutf("Syntax: macaddr = <MAC address>\n");
  _ReportOutf("Sample: macaddr = 00:0C:29:0A:48:1F\n");
}

/*********************************************************************
*
*       _ExecMACAddress
*/
static int _ExecMACAddress(const char* s) {
  U64 MACAddr;
  U32 MACByte[6];
  U16 USBConfig;
  int r;
  int i;

  _EatWhite(&s);
  if (*s) {
    if (*s == '=') {
      s++;
    }
    for (i = 0; i <= 5; i++) {
      if (_ParseHex(&s, &MACByte[i])) {
        _ExecMACAddressSyntaxError();
        return JLINK_ERROR_SYNTAX;
      }
      if (i < 5) {
        if (*s == ':') {
          s++;
        } else {
          _ExecMACAddressSyntaxError();
          return JLINK_ERROR_SYNTAX;
        }
      }
    }
    MACAddr = ((U64)MACByte[5]) << 40
            | ((U64)MACByte[4]  << 32)
            |  (MACByte[3]      << 24)
            |  (MACByte[2]      << 16)
            |  (MACByte[1]      <<  8)
            |   MACByte[0]
            ;
    r = JLINKARM_WriteEmuConfigMem((const unsigned char*)&MACAddr, 0x30, 6);
    if (r != 0) {
      if (r == 2) {
        _ReportOutf("MAC address is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to write MAC address.\n");
        return JLINK_ERROR_UNKNOWN;
      }
    } else {
      _ReportOutf("MAC address successfully changed to '%.2X:%.2X:%.2X:%.2X:%.2X:%.2X'.\n", MACByte[0], MACByte[1], MACByte[2], MACByte[3], MACByte[4], MACByte[5]);
      _ReportOutf("Please unplug the device, then plug it back in.\n");
    }
  } else {
    r = JLINKARM_ReadEmuConfigMem((unsigned char*)&USBConfig, 0x00, 2);
    if (r != 0) {    // Testing successfully reading has to be done only once
      if (r == 2) {
        _ReportOutf("This feature is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to read configuration.\n");
      }
    } else {
      //
      // Read rest of configuration
      //
      JLINKARM_ReadEmuConfigMem((unsigned char*)&MACAddr, 0x30, 6);
      _PrintMACAddr(MACAddr);
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecIPAddressSyntaxError
*/
static void _ExecIPAddressSyntaxError(void) {
  _ReportOutf("Syntax: ipaddr = <ip> [<subnetmask>]\n");
  _ReportOutf("Sample: ipaddr = 192.168.0.10 255.255.255.0\n");
}

/*********************************************************************
*
*       _ExecIPAddress
*/
static int _ExecIPAddress(const char* s) {
  U64  ConfigData;
  U32  IPAddr;
  U32  IPByte[4];
  U32  IPMask;
  U32  MaskByte[4] = {0};
  int  r;
  int  i;

  _EatWhite(&s);
  if (*s) {
    if (*s == '=') {
      s++;
    }
    if (_CompareCmd(&s, "dhcp") == 0) {
      for (i = 0; i <= 3; i++) {
        IPByte[i] = 0xFF;
      }
    } else {
      for (i = 0; i <= 3; i++) {
        if (_ParseDec(&s, &IPByte[i])) {
          _ExecIPAddressSyntaxError();
          return JLINK_ERROR_SYNTAX;
        }
        if (i < 3) {
          if (*s == '.') {
            s++;
          } else {
            _ExecIPAddressSyntaxError();
            return JLINK_ERROR_SYNTAX;
          }
        }
      }
      _EatWhite(&s);
      if (*s) {
        for (i = 0; i <= 3; i++) {
          if (_ParseDec(&s, &MaskByte[i])) {
            _ExecIPAddressSyntaxError();
            return JLINK_ERROR_SYNTAX;
          }
          if (i < 3) {
            if (*s == '.') {
              s++;
            } else {
              _ExecIPAddressSyntaxError();
              return JLINK_ERROR_SYNTAX;
            }
          }
        }
      }
      for (i = 0; i <= 3; i++) {
        if ((IPByte[i] > 255) || (MaskByte[i] > 255)) {
          _ReportOutf("Please input a valid IP address and subnetmask!\n");
          return JLINK_ERROR_SYNTAX;
        }
      }
    }
    IPAddr = (IPByte[0] << 24)
           | (IPByte[1] << 16)
           | (IPByte[2] <<  8)
           | (IPByte[3])
           ;
    IPMask = (MaskByte[0] << 24)
           | (MaskByte[1] << 16)
           | (MaskByte[2] <<  8)
           | (MaskByte[3])
           ;
    ConfigData = IPAddr;
    if (IPMask == 0) {
      IPMask = 0xFFFF0000;
      MaskByte[0] = 255;
      MaskByte[1] = 255;
    }
    ConfigData =  (U64)ConfigData
               | ((U64)IPMask << 32);
    r = JLINKARM_WriteEmuConfigMem((const unsigned char*)&ConfigData, 0x20, 8);
    if (r != 0) {
      if (r == 2) {
        _ReportOutf("IP address and subnetmask is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to write IP address and subnetmask.\n");
        return JLINK_ERROR_UNKNOWN;
      }
    } else {
      if (IPAddr != 0xFFFFFFFF) {
        _ReportOutf("IP address successfully changed to '%d.%d.%d.%d'.\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);
        if (IPMask != 0) {
          _ReportOutf("Subnetmask successfully changed to '%d.%d.%d.%d'.\n", MaskByte[0], MaskByte[1], MaskByte[2], MaskByte[3]);
        }
      } else {
        _ReportOutf("Configuration successfully changed to DHCP.\n");
      }
    }
  } else {
    r = _GetEmuIPAddr(&IPAddr);
    if (r == EMU_IP_ERROR) {  // IP addr could not be read
      return JLINK_NOERROR;
    } else {
      _GetEmuIPMask(&IPMask);
      IPByte[0]   =  IPAddr             >> 24;
      IPByte[1]   = (IPAddr & 0xFF0000) >> 16;
      IPByte[2]   = (IPAddr & 0x00FF00) >> 8;
      IPByte[3]   =  IPAddr & 0x0000FF;
      MaskByte[0] =  IPMask & 0x0000FF;
      MaskByte[1] = (IPMask & 0x00FF00) >> 8;
      MaskByte[2] = (IPMask & 0xFF0000) >> 16;
      MaskByte[3] =  IPMask             >> 24;
      if (r == EMU_IP_DHCP_NO_CONFIG) {
        _ReportOutf("DHCP active but no configuration received\n");
      } else {
        if (r == EMU_IP_FIXED) {
          _ReportOutf("User assigned network configuration\n");
        } else if (r == EMU_IP_DHCP_CONFIGURED) {  // DHCP is active and assigned
          _ReportOutf("DHCP assigned network configuration\n");
        }
        _ReportOutf("IP-Addr:    %d.%d.%d.%d\n"
               "Subnetmask: %d.%d.%d.%d\n", IPByte[0]  , IPByte[1]  , IPByte[2]  , IPByte[3],
                                            MaskByte[3], MaskByte[2], MaskByte[1], MaskByte[0]);
      }
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGWAddressSyntaxError
*/
static void _ExecGWAddressSyntaxError(void) {
  _ReportOutf("Syntax: gwaddr = <ip>\n");
  _ReportOutf("Sample: gwaddr = 192.168.0.1\n");
}

/*********************************************************************
*
*       _ExecGWAddress
*/
static int _ExecGWAddress(const char* s) {
  U32 IPAddr;
  U32 GWAddr;
  U32 IPByte[4];
  int r;
  int i;

  _EatWhite(&s);
  if (*s) {
    if (*s == '=') {
      s++;
    }
    for (i = 0; i <= 3; i++) {
      if (_ParseDec(&s, &IPByte[i])) {
        _ExecGWAddressSyntaxError();
        return JLINK_ERROR_SYNTAX;
      }
      if (i < 3) {
        if (*s == '.') {
          s++;
        } else {
          _ExecGWAddressSyntaxError();
          return JLINK_ERROR_SYNTAX;
        }
      }
    }
    for (i = 0; i <= 3; i++) {
      if (IPByte[i] > 255) {
        _ReportOutf("Please input a valid gateway address!\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
    GWAddr = (IPByte[0] << 24)
           | (IPByte[1] << 16)
           | (IPByte[2] <<  8)
           | (IPByte[3])
           ;
    r = JLINKARM_WriteEmuConfigMem((const unsigned char*)&GWAddr, 0x40, 4);
    if (r != 0) {
      if (r == 2) {
        _ReportOutf("Gateway address is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to write gateway address.\n");
        return JLINK_ERROR_UNKNOWN;
      }
    } else {
      _ReportOutf("Gateway successfully changed to '%d.%d.%d.%d'.\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);
    }
  } else {
    r = _GetEmuIPAddr(&IPAddr);
    if (r == EMU_IP_ERROR) {  // IP addr could not be read
      return JLINK_NOERROR;
    } else {
      _GetEmuIPGateway(&GWAddr);
      IPByte[0]   =  GWAddr             >> 24;
      IPByte[1]   = (GWAddr & 0xFF0000) >> 16;
      IPByte[2]   = (GWAddr & 0x00FF00) >> 8;
      IPByte[3]   =  GWAddr & 0x0000FF;
      if (r == EMU_IP_DHCP_NO_CONFIG) {
        _ReportOutf("DHCP active but no configuration received\n");
      } else {
        if (r == EMU_IP_FIXED) {
          _ReportOutf("User assigned network configuration\n");
        } else if (r == EMU_IP_DHCP_CONFIGURED) {
          _ReportOutf("DHCP assigned network configuration\n");
        }
        _ReportOutf("Gateway address: %d.%d.%d.%d\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);
      }
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecDNSAddressSyntaxError
*/
static void _ExecDNSAddressSyntaxError(void) {
  _ReportOutf("Syntax: dnsaddr = <ip>\n");
  _ReportOutf("Sample: dnsaddr = 192.168.0.1\n");
}

/*********************************************************************
*
*       _ExecDNSAddress
*/
static int _ExecDNSAddress(const char* s) {
  U32 IPAddr;
  U32 DNSAddr;
  U32 IPByte[4];
  int r;
  int i;

  _EatWhite(&s);
  if (*s) {
    if (*s == '=') {
      s++;
    }
    for (i = 0; i <= 3; i++) {
      if (_ParseDec(&s, &IPByte[i])) {
        _ExecDNSAddressSyntaxError();
        return JLINK_ERROR_SYNTAX;
      }
      if (i < 3) {
        if (*s == '.') {
          s++;
        } else {
          _ExecDNSAddressSyntaxError();
          return JLINK_ERROR_SYNTAX;
        }
      }
    }
    for (i = 0; i <= 3; i++) {
      if (IPByte[i] > 255) {
        _ReportOutf("Please input a valid DNS server address!\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
    DNSAddr = (IPByte[0] << 24)
            | (IPByte[1] << 16)
            | (IPByte[2] <<  8)
            | (IPByte[3])
            ;
    r = JLINKARM_WriteEmuConfigMem((const unsigned char*)&DNSAddr, 0x48, 4);
    if (r != 0) {
      if (r == 2) {
        _ReportOutf("DNS server address is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to write DNS server address.\n");
        return JLINK_ERROR_UNKNOWN;
      }
    } else {
      _ReportOutf("DNS server successfully changed to '%d.%d.%d.%d'.\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);
    }
  } else {
    r = _GetEmuIPAddr(&IPAddr);
    if (r == EMU_IP_ERROR) {  // IP addr could not be read
      return JLINK_NOERROR;
    } else {
      _GetEmuIPDNS0(&DNSAddr);
      IPByte[0]   =  DNSAddr             >> 24;
      IPByte[1]   = (DNSAddr & 0xFF0000) >> 16;
      IPByte[2]   = (DNSAddr & 0x00FF00) >> 8;
      IPByte[3]   =  DNSAddr & 0x0000FF;
      if (r == EMU_IP_DHCP_NO_CONFIG) {
        _ReportOutf("DHCP active but no configuration received\n");
      } else {
        if (r == EMU_IP_FIXED) {
          _ReportOutf("User assigned network configuration\n");
        } else if (r == EMU_IP_DHCP_CONFIGURED) {
          _ReportOutf("DHCP assigned network configuration\n");
        }
        _ReportOutf("DNS-Addr: %d.%d.%d.%d\n", IPByte[0], IPByte[1], IPByte[2], IPByte[3]);
      }
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowConfig
*/
static int _ExecShowConfig(const char* s) {
  U16  USBConfig;
  U8   USBAddr;
  U8   EnumType;
  I32  KSPower;
  int  SN;
  U32  FakeSN;
  U32  IPAddr;
  U32  SubnetMask;
  U32  GWAddr;
  U32  DNSAddr;
  U64  MACAddr;
  char acOEM[256];
  char acFW[256];
  int  r;
  int  v;

  (void)s;
  r = JLINKARM_ReadEmuConfigMem((unsigned char*)&USBConfig, 0x00, 2);
  if (r != 0) {    // Testing successfully reading has to be done only once
    if (r == 2) {
      _ReportOutf("This feature is not supported by connected J-Link.\n");
    } else {
      _ReportOutf("Failed to read configuration.\n");
      return JLINK_ERROR_UNKNOWN;
    }
  } else {
    //
    // Read rest of configuration
    //
    JLINKARM_ReadEmuConfigMem((unsigned char*)&KSPower,    0x04, 4);
    JLINKARM_ReadEmuConfigMem((unsigned char*)&FakeSN,     0x08, 4);
    JLINKARM_ReadEmuConfigMem((unsigned char*)&MACAddr,    0x30, 6);
    JLINKARM_GetFirmwareString(acFW,256);
    JLINKARM_GetOEMString(&acOEM[0]);
    SN = JLINKARM_GetSN();
    //
    // Show USB address
    //
    USBAddr = USBConfig & 0xFF;
    if (USBAddr == 0xFF) {
      _ReportOutf("USB-Address: Default\n");
    } else {
      _ReportOutf("USB-Address: %d\n", USBAddr);
    }
    //
    // Show enum. type
    //
    EnumType = USBConfig >> 8;
    if ((EnumType == 0) || (EnumType == 0xFF)) {
      _ReportOutf("Enum. type:  USB-Address is used\n");
    } else if (EnumType == 1) {
      _ReportOutf("Enum. type:  Real-SN is used\n");
      _ReportOutf("Real-SN:     %d\n", SN);
    } else if (EnumType == 2) {
      _ReportOutf("Enum. type:  Fake-SN is used\n");
      _ReportOutf("Fake-SN:     %d\n", FakeSN);
    }
    //
    // Show KS-Power
    //
    if ((KSPower == -1) && (strcmp(&acOEM[0], "IARKS") == 0)) {
      _ReportOutf("KS-Power:    On (Default)\n");
    } else if (KSPower == -1) {
      _ReportOutf("KS-Power:    Off (Default)\n");
    } else if (KSPower == 0) {
      _ReportOutf("KS-Power:    Off\n");
    } else if (KSPower == 1) {
      _ReportOutf("KS-Power:    On\n");
    }
    //
    // Show ethernet configuration
    //
    if (_HasEmuEthernet()) {
      v = _GetEmuIPAddr(&IPAddr);
      _GetEmuIPMask(&SubnetMask);
      if (v == EMU_IP_DHCP_NO_CONFIG) {
        _ReportOutf("No DHCP configuration received.\n");
      } else {
        if (v == EMU_IP_FIXED) {
          _ReportOutf("User assigned network configuration\n");
        } else if (v == EMU_IP_DHCP_CONFIGURED) {
          _ReportOutf("DHCP assigned network configuration\n");
        }
        _ReportOutf("IP-Addr:        %d.%d.%d.%d\n",    IPAddr                  >> 24,
                                                  (IPAddr & 0xFF0000)      >> 16,
                                                  (IPAddr & 0x00FF00)      >> 8,
                                                   IPAddr & 0x0000FF);
      }
      if ((v == EMU_IP_FIXED) || (v == EMU_IP_DHCP_CONFIGURED)) {  // Either addresses are fixed or DHCPc has received configuration
        _ReportOutf("Subnetmask:     %d.%d.%d.%d\n",    SubnetMask              >> 24,
                                                  (SubnetMask & 0xFF0000)  >> 16,
                                                  (SubnetMask & 0x00FF00)  >> 8,
                                                   SubnetMask & 0x0000FF);
      }
      v = _GetEmuIPGateway(&GWAddr);
      if (((v == EMU_IP_FIXED) && (GWAddr != 0xFFFFFFFF))|| (v == EMU_IP_DHCP_CONFIGURED)) {  // Either addresses are fixed and have been set or DHCPc has received configuration
        _ReportOutf("Gateway-Addr:   %d.%d.%d.%d\n",    GWAddr                  >> 24,
                                                  (GWAddr & 0xFF0000)      >> 16,
                                                  (GWAddr & 0x00FF00)      >> 8,
                                                   GWAddr & 0x0000FF);
      }
      v = _GetEmuIPDNS0(&DNSAddr);
      if (((v == EMU_IP_FIXED) && (GWAddr != 0xFFFFFFFF))|| (v == EMU_IP_DHCP_CONFIGURED)) {  // Either addresses are fixed and have been set or DHCPc has received configuration
        _ReportOutf("DNS-Addr:       %d.%d.%d.%d\n",    DNSAddr                 >> 24,
                                                  (DNSAddr & 0xFF0000)     >> 16,
                                                  (DNSAddr & 0x00FF00)     >> 8,
                                                   DNSAddr & 0x0000FF);
      }
      _PrintMACAddr(MACAddr);
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetConfig
*/
static int _ExecSetConfig(const char* s) {
  U32 IRPre, DRPre;
  if (_ParseDec(&s, &IRPre)) {
    _ReportOutf("Syntax: Config <IRPre>, <DRPre>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &DRPre)) {
    _ReportOutf("Syntax: Config <IRPre>, <DRPre>\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_ConfigJTAG(IRPre, DRPre);
  _ShowCoreID(&_CommanderSettings);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetJTAGConfig
*/
static int _ExecSetJTAGConfig(const char* s) {
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;

  if (_ParseDec(&s, (U32*)&pCommanderSettings->JTAGConfIRPre)) {
    _ReportOutf("ERROR while parsing value for IRPre. Using default: %d.\n", pCommanderSettings->JTAGConfIRPre);
    return JLINK_ERROR_SYNTAX;
  }
  if (*s == ',') {     // Skip comma between JTAG config params
    s++;
  }
  if (_ParseDec(&s, (U32*)&pCommanderSettings->JTAGConfDRPre)) {
    _ReportOutf("ERROR while parsing value for DRPre. Using default: %d.\n", pCommanderSettings->JTAGConfDRPre);
    return JLINK_ERROR_SYNTAX;
  }
  pCommanderSettings->JTAGConfSet = 1;
  pCommanderSettings->TargetIFSet = 1;
  return 0;
}

/*********************************************************************
*
*       _ExecMSDConfig
*/
static int _ExecMSDConfig(int Enable) {
  U8 ConfigByte;
  int r;

  r = JLINKARM_ReadEmuConfigMem(&ConfigByte, CONFIG_OFF_HW_FEATURES, 1);
  if (r) {
    _ReportOutf("Failed to read config area of connected probe.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (Enable) {
    ConfigByte |= (1uL << 1);
  } else {
    ConfigByte &= ~(1uL << 1);
  }
  r = JLINKARM_WriteEmuConfigMem(&ConfigByte, CONFIG_OFF_HW_FEATURES, 1);
  if (r) {
    _ReportOutf("Failed to write config area of connected probe.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("Probe configured successfully.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWinUSBConfig
*/
static int _ExecWinUSBConfig(int Enable) {
  U8 ConfigByte;
  int r;

  r = JLINKARM_ReadEmuConfigMem(&ConfigByte, CONFIG_OFF_HW_FEATURES, 1);
  if (r) {
    _ReportOutf("Failed to read config area of connected probe.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Bit3: WinUSB disable (0: Enabled, 1: Disabled). Default disabled
  //
  if (Enable) {
    ConfigByte &= ~(1uL << 3);
  } else {
    ConfigByte |= (1uL << 3);
  }
  r = JLINKARM_WriteEmuConfigMem(&ConfigByte, CONFIG_OFF_HW_FEATURES, 1);
  if (r) {
    _ReportOutf("Failed to write config area of connected probe.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("Probe configured successfully.\n");
  _ReportOutf("The new configuration applies after power cycling the probe.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecMSDDisble
*/
static int _ExecMSDDisble(const char* s) {
  (void)s;
  return _ExecMSDConfig(0);
}

/*********************************************************************
*
*       _ExecMSDEnable
*/
static int _ExecMSDEnable(const char* s) {
  (void)s;
  return _ExecMSDConfig(1);
}

/*********************************************************************
*
*       _ExecWebUSBDisable
*/
static int _ExecWebUSBDisable(const char* s) {
  (void)s;
  _ExecWinUSBConfig(0);
  return 0;
}

/*********************************************************************
*
*       _ExecWebUSBEnable
*/
static int _ExecWebUSBEnable(const char* s) {
  (void)s;
  _ExecWinUSBConfig(1);
  return 0;
}

/*********************************************************************
*
*       _ExecUSBIP
*
*  Function description
*    Enables/disables IP over USB functionality of J-Link, by writing the config area.
*    Syntax: USBIP enable/disable
*
*  Return value
*    JLINK_NOERROR  O.K.
*    Else           Error
*/
static int _ExecUSBIP(const char* s) {
  U8 ConfigByte;
  int r;
  unsigned Enable;
  //
  // Check if J-Link supports this feature at all
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_USB_IP) == 0) {
    _ReportOutf("The connected probe does not support IP over USB functionality.\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if(_CompareCmd(&s, "Enable") == 0) {
    Enable = 1;
  } else if(_CompareCmd(&s, "Disable") == 0) {
    Enable = 0;
  } else {
    _ReportOutf("Syntax: USBIP Enable/Disable\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_ReadEmuConfigMem(&ConfigByte, CONFIG_OFF_HW_FEATURES, 1);
  if (r != 0) {
    _ReportOutf("Failed to read config byte of J-Link\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (Enable) {
    ConfigByte &= (U8)~(1 << 0);
  } else {
    ConfigByte |= (1 << 0);
  }
  r = JLINKARM_WriteEmuConfigMem(&ConfigByte, CONFIG_OFF_HW_FEATURES, 1);
  if (r != 0) {
    _ReportOutf("Failed to write config data\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("The new configuration applies after power cycling the debug probe.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetHWFC
*
*  Function description
*    Enables/disables/Forces VCOM hardware flow control functionality of J-Link, by writing the config area.
*    Syntax: SetHWFC enable/disable/force
*
*  Return value
*    JLINK_NOERROR  O.K.
*    Else           Error
*/
static int _ExecSetHWFC(const char* s) {
  U8 NewConfigByte;
  U8 OldConfigByte;
  int r;
  //
  // Check if emulator supports this feature
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_CDC_EXEC) == 0) {
    _ReportOutf("The connected probe does not support VCOM functionality.\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if(_CompareCmd(&s, "Enable") == 0) {
    NewConfigByte = 1;
  } else if(_CompareCmd(&s, "Disable") == 0) {
    NewConfigByte = 0;
  } else if(_CompareCmd(&s, "Force") == 0) {
    NewConfigByte = 2;
  } else {
    _ReportOutf("Syntax: SetHWFC <State>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_ReadEmuConfigMem(&OldConfigByte, CONFIG_OFF_HWFC, 1);
  if (r != 0) {
    OldConfigByte = -1;          // Force writing the config byte if we were not able to check if it changed.
  }
  //
  // Check if virtual com port config byte has changed
  //
  if (OldConfigByte ^ NewConfigByte) {
    r = JLINKARM_WriteEmuConfigMem(&NewConfigByte, CONFIG_OFF_HWFC, 1);
    if (r != 0) {
      if (r == 2) {
        _ReportOutf("Write config data is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to write config data\n");
        return JLINK_ERROR_UNKNOWN;
      }
    }
    _ReportOutf("New configuration applies immediately.\n");
  } else {
    _ReportOutf("New configuration applies immediately.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecVCOM
*
*  Function description
*    Enables/disables VCOM functionality of J-Link, by writing the config area.
*    Syntax: VCOM enable/disable
*
*  Return value
*    JLINK_NOERROR  O.K.
*    Else           Error
*/
static int _ExecVCOM(const char* s) {
  U8 NewConfigByte;
  U8 OldConfigByte;
  int r;
  //
  // Check if emulator supports CDC
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_CDC_EXEC) == 0) {
    _ReportOutf("The connected probe does not support VCOM functionality.\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if(_CompareCmd(&s, "Enable") == 0) {
    NewConfigByte = 1;

  } else if(_CompareCmd(&s, "Disable") == 0) {
    NewConfigByte = 0;
  } else {
    _ReportOutf("Syntax: VCOM <State>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_ReadEmuConfigMem(&OldConfigByte, VIRTUAL_COM_PORT_OFFSET, 1);
  if (r != 0) {
    OldConfigByte = -1;          // Force writing the config byte if we were not able to check if it changed.
  }
  //
  // Check if virtual com port config byte has changed
  //
  if (OldConfigByte ^ NewConfigByte) {
    r = JLINKARM_WriteEmuConfigMem(&NewConfigByte, VIRTUAL_COM_PORT_OFFSET, 1);
    if (r != 0) {
      if (r == 2) {
        _ReportOutf("Write config data is not supported by connected J-Link.\n");
      } else {
        _ReportOutf("Failed to write config data\n");
        return JLINK_ERROR_UNKNOWN;
      }
    }
    _ReportOutf("The new configuration applies after power cycling the debug probe.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecVTREF
*
*  Function description
*    Prints current VTref or sets a fixed value for it if the connected J-Link model supports this.
*    The fixed value is remembered in the config area of the J-Link
*    Syntax: VTREF [mV]
*
*  Return value
*    JLINK_NOERROR  O.K.
*    Else           Error
*
*  Notes
*    (1) Fixed VTref values higher than 3300mV are not supported to avoid misconfiguration of J-Link that may damage 3.3V / 1.8V targets
*    (2) Given values are rounded to multiple of 100mV
*/
static int _ExecVTREF(const char* s) {
  U32 v;
  U32 HundredmVolts;
  int r;
  const char* sErr;
  //
  // Early out if J-Link does not support this feature
  //
  r = _SupportsFixedVTref();
  if (r == 0) {
    _ReportOutf("The connected probe does not support setting a fixed VTref.\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Parse value for fixed VTref
  //
  _EatWhite(&s);
  if (*s == 0) {
    _ReportOutf("Missing parameter after command\n");
    return JLINK_ERROR_SYNTAX;
  }
  sErr = _ParseDec(&s, &v);
  if (sErr) {
    _ReportOutf("Error while parsing parameter value: %s.\n", sErr);
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Apply value
  //
  v += 50;
  v /= 100;
  v *= 100;                         // Round to multiple of 100mV
  if (v < 1200) {                   // Min. fixed VTref is 1200 mV
    _ReportOutf("Fixed VTref disabled. Using VTref pin of J-Link for target I/O voltage detection.\n");
    v = 0xFF;
    JLINKARM_WriteEmuConfigMem((const U8*)&v, CONFIG_OFF_FIXED_VTREF, 1);  // Stored in V not mV, in hardware
    _ReportOutf("The new configuration applies after power cycling the probe.\n");
    return JLINK_NOERROR;
  }
  if (v > 3300) {                   // Max. fixed VTref is 3300 mV
    _ReportOutf("ERROR: Cannot set VTref to %dmV. Fixed VTref is only supported up to 3300mV.\n", v);
    return JLINK_ERROR_SYNTAX;
  }
  HundredmVolts = v / 100;
  JLINKARM_WriteEmuConfigMem((const U8*)&HundredmVolts, CONFIG_OFF_FIXED_VTREF, 1);  // Stored in 100mV units, in hardware
  _ReportOutf("New fixed VTref = %dmV\n", v);
  _ReportOutf("The new configuration applies after power cycling the probe.\n");
// AG xxxxxxxxxxxxxxxxx TBD: New models support fixed VTref change without power-cycle. They do it through an emu option, so we first check if this is supported by the connected J-Link
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetWiFi
*
*  Function description
*    Sets WiFi options like SSID, PW, ...
*    The options are written to the J-Link config area
*    Syntax: SetWiFi SSID <String_SSID>
*            SetWiFi Pass <String_Password>
*
*  Return value
*    JLINK_NOERROR  O.K.
*    Else           Error
*/
static int _ExecSetWiFi(const char* s) {
  int Len;
  char ac[256];
  char acSSID[32];
  char acPW[32];
  //
  // Check if J-Link supports this feature at all
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_WIFI) == 0) {
    _ReportOutf("The connected probe does not support WiFi functionality.\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Parse & execute SubCmd
  //
  ac[0] = 0;  // Empty string by default
  _ParseString(&s, ac, _SIZEOF(ac));
  if (_JLinkstricmp("SSID", ac) == 0) {
    //
    // Handle set SSID
    //
    ac[0] = 0;
    _ParseString(&s, ac, _SIZEOF(ac));
    Len = UTIL_strlen(ac) + 1;  // Also store terminating zero char
    if (Len > _SIZEOF(acSSID)) {
      _ReportOutf("ERROR: String is too long. (Max. %d characters supported)\n", _SIZEOF(acPW) - 1);
      return JLINK_ERROR_SYNTAX;
    }
    memset(acSSID, 0xFF, _SIZEOF(acSSID));
    memcpy(acSSID, ac, Len);
    JLINKARM_WriteEmuConfigMem(acSSID, CONFIG_OFF_WIFI_SSID, _SIZEOF(acSSID));   // Always write max. len so there are no leftovers in the config area
  } else if (_JLinkstricmp("Pass", ac) == 0) {
    //
    // Handle set Pass
    //
    ac[0] = 0;
    _ParseString(&s, ac, _SIZEOF(ac));
    Len = UTIL_strlen(ac) + 1;   // Also store terminating zero char
    if (Len > _SIZEOF(acPW)) {
      _ReportOutf("ERROR: String is too long. (Max. %d characters supported)\n", _SIZEOF(acPW) - 1);
      return JLINK_ERROR_SYNTAX;
    }
    memset(acPW, 0xFF, _SIZEOF(acPW));
    memcpy(acPW, ac, Len);
    JLINKARM_WriteEmuConfigMem(acPW, CONFIG_OFF_WIFI_PW, _SIZEOF(acPW));
  } else {
    _ReportOutf("ERROR: Unknown sub command: %s\n", ac);
    return JLINK_ERROR_SYNTAX;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecResetConfig
*/
static int _ExecResetConfig(const char* s) {
  int r;
  int ConfigSize;
  U8* pConfigData;
  U8 Dummy;

  (void)s;
  r = JLINKARM_ReadEmuConfigMem(&Dummy, 0, 1);
  if (r != 0) {
    if (r == 2) {
      _ReportOutf("This feature is not supported by connected J-Link.\n");
    } else {
      _ReportOutf("Failed to reset config data. Error code %d\n", r);
    }
    return JLINK_ERROR_UNKNOWN;
  }
  ConfigSize = 0x100;  // Default config size supported by all models
  //
  // New models support a dynamic size of the config area so determine config area size (which is always a multiple of 256 bytes)
  //
  do {
    r = JLINKARM_ReadEmuConfigMem(&Dummy, ConfigSize, 1);
    if (r != 0) {
      break;
    }
    ConfigSize += 256;
  } while (1);
  pConfigData = (U8*)malloc(ConfigSize);
  memset(pConfigData, 0xFF, ConfigSize);
  r = JLINKARM_WriteEmuConfigMem(pConfigData, 0, ConfigSize);
  free(pConfigData);
  if (r != 0) {
    if (r == 2) {
      _ReportOutf("Write config data is not supported by connected J-Link.\n");
    } else {
      _ReportOutf("Failed to reset config data. Error code %d\n", r);
      return JLINK_ERROR_UNKNOWN;
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteConfig
*/
static int _ExecWriteConfig(const char* s) {
  U32 Offset, Data;
  U8  ConfigByte;
  int r;
  if (_ParseHex(&s, &Offset)) {
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Data)) {
    return JLINK_ERROR_SYNTAX;
  }
  ConfigByte = (U8)Data;
  r = JLINKARM_WriteEmuConfigMem(&ConfigByte, Offset, 1);
  if (r != 0) {
    if (r == 2) {
      _ReportOutf("Write config data is not supported by connected J-Link.\n");
    } else {
      _ReportOutf("Failed to write config data\n");
      return JLINK_ERROR_UNKNOWN;
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadConfig
*/
static int _ExecReadConfig(const char* s) {
  U8* pData;
  U8* pBase;
  U32 NumBytes;
  U32 Offset;
  int r;
  int ConfigSize;
  U8 Dummy;

  pBase = NULL;
  pData = pBase;
  //
  // Default
  //
  NumBytes = JLINK_DEFAULT_SIZEOF_CONFIG;
  Offset   = 0;
  //
  // Any parameters given?
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &Offset)) {
      _ReportOutf("Syntax: mem <Addr>, <NumBytes>\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &NumBytes)) {
      _ReportOutf("Syntax: mem <Addr>, <NumBytes>\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  pBase = malloc(NumBytes);
  pData = pBase;
  if (pData == NULL) {
    _ReportOutf("Failed to allocate memory for temporary buffer.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  r = JLINKARM_ReadEmuConfigMem(pData, Offset, NumBytes);
  if (r != 0) {
    if (r == 2) {
      _ReportOutf("Read config data is not supported by connected J-Link.\n");
      return JLINK_NOERROR;
    } else {
      _ReportOutf("Failed to read config data.\n");
      return JLINK_ERROR_UNKNOWN;
    }
  }
  //
  // New models support a dynamic size of the config area so determine config area size (which is always a multiple of 256 bytes)
  //
  ConfigSize = 0x100;  // Default config size supported by all models
  do {
    r = JLINKARM_ReadEmuConfigMem(&Dummy, ConfigSize, 1);
    if (r != 0) {
      break;
    }
    ConfigSize += 256;
  } while (1);
  _ReportOutf("Total size of config area: 0x%X bytes\n\n", ConfigSize);
  while (NumBytes > 0) {
    int NumBytesPerLine;
    _ReportOutf("%.8X = ", Offset);
    NumBytesPerLine = (NumBytes > 16) ? 16 : NumBytes;
    NumBytes -= NumBytesPerLine;
    Offset   += NumBytesPerLine;
    for (; NumBytesPerLine > 0; NumBytesPerLine--) {
      _ReportOutf("%.2X ", *pData++);
    }
    _ReportOutf("\n");
  }
  if (pBase) {
    free(pBase);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteConfigVMSD
*/
static int _ExecWriteConfigVMSD(const char* s) {
  char ac[MAX_PATH];
  HANDLE hFile;
  U8* pData;
  int Result;
  int r;
  U32 FileSize;
  //
  // Parse filename of VMSD config file for J-Link
  //
  pData = NULL;
  Result = JLINK_NOERROR;
  ac[0] = 0;
  _ParseString(&s, ac, sizeof(ac));
  //
  // Read VMSD config file into tmeporary buffer
  //
  hFile = _OpenFile(ac, FILE_FLAG_READ | FILE_FLAG_SHARE_READ);
  if (hFile == INVALID_HANDLE_VALUE) {
    _ReportOutf("Failed to open file %s\n", ac[0] ? ac : "[NULL]");
    Result = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  FileSize = _GetFileSize(hFile);
  if ((FileSize == 0) || (FileSize == 0xFFFFFFFF)) {
    _ReportOutf("Invalid FileSize: %d\n", FileSize);
    Result = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  pData = (U8*)malloc(FileSize + sizeof(FileSize));  // Reserve some extra space to be able to store the filesize/streamsize at the beginning of the buffer
  if (pData == NULL) {
    _ReportOutf("Failed to allocate buffer for reading file.\n");
    Result = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  memcpy(pData, &FileSize, sizeof(FileSize));
  _ReadFile(hFile, pData + sizeof(FileSize), FileSize);
  _CloseFile(hFile);
  //
  // Download VMSD config file into config area of J-Link
  // Offset 0x800: Size of VMSD configuration downloaded
  // Offset 0x804: Start of VMSD configuration file
  //
  _ReportOutf("Downloading VMSD config...");
  r = JLINKARM_WriteEmuConfigMem(pData, 0x800, FileSize + sizeof(FileSize));
  if (r == 0) {
    _ReportOutf("O.K.\n");
  } else {
    if (r == 1) {
      _ReportOutf("ERROR: Configuration area of J-Link too small.\n");
      Result = JLINK_ERROR_UNKNOWN;
    } else {
      _ReportOutf("ERROR: Error while writing VMSD config.\n");
      Result = JLINK_ERROR_UNKNOWN;
    }
  }
  //
  // Maintenance
  //
Done:
  if (pData) {
    free(pData);
  }
  return Result;
}

/*********************************************************************
*
*       _ExecSelectFamily
*/
static int _ExecSelectFamily(const char* s) {
  U32 Family;
  if (_ParseHex(&s, &Family)) {
    _ReportOutf("Parameters: <3:Cortex-M3 / 5:XScale / 7:ARM7 / 9:ARM9 / 11:ARM11>\n");
    return JLINK_ERROR_SYNTAX;
  } else {
    JLINKARM_SelectDeviceFamily(Family);
    _ReportOutf("Device family has been set to %d\n", Family);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSelectCore
*/
static int _ExecSelectCore(const char* s) {
  U32 Core;
  if (_ParseHex(&s, &Core)) {
    _ReportOutf("Syntax: SelectCore <Core>\n");
    return JLINK_ERROR_SYNTAX;
  } else {
    JLINKARM_CORE_Select(Core);
    _ReportOutf("Core has been set to 0x%.8X\n", Core);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSelectInterface
*/
static int _ExecSelectInterface(const char* s) {
  U32 Interface;
  U32 IFMask;
  char ac[MAX_PATH+10];
  int Result;
  unsigned WasConnectedToTarget;
  _SETTINGS* pCommanderSettings;
  //
  // Obsolete method: Customers can pass the interface number to select the interface
  // New method: Customers can pass the interface name (e.g. SWD, JTAG, etc...) to select the interface
  //
  pCommanderSettings = &_CommanderSettings;
  _ParseString(&s, ac, sizeof(ac));
  if (ac[0] == 0) {                                          // No string given? --> Error
    _ReportOutf("Syntax: SelectInterface <interface>\n");
    return JLINK_ERROR_SYNTAX;
  } else {
    Interface = _Name2TIF(ac);
    if ((I32)Interface < 0) {                                // Interface not found? --> Parse interface number
      s = ac;
      if (_ParseHex(&s, &Interface)) {                       // No hex index parsed? --> Error
        _ReportOutf("Syntax: Unknown interface entered: %s\n", ac);
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  //
  // On interface change:
  // Disconnect from target
  // Change interface
  // Update Commander settings
  // Reconnect to target
  //
  WasConnectedToTarget = pCommanderSettings->Status.ConnectedToTarget;
  if (WasConnectedToTarget) {
    _DisconnectFromTarget(pCommanderSettings);
  }
  IFMask = 0;
  JLINKARM_TIF_GetAvailable(&IFMask);
  Result = JLINK_NOERROR;
  if ((IFMask & (1uL << Interface)) == 0) {
    _ReportOutf("%s is not supported by connected J-Link.\n", _TIF2Name(Interface));
    Result = JLINK_ERROR_UNKNOWN;
  } else {
    _ReportOutf("Selecting %s as current target interface.\n", _TIF2Name(Interface));
    pCommanderSettings->TargetIF = Interface;
    pCommanderSettings->TargetIFSet = 1;
  }
  if (Result == JLINK_NOERROR) {
    if (WasConnectedToTarget) {
      Result = _ConnectToTarget(pCommanderSettings);
    }
  }
  return Result;
}

/*********************************************************************
*
*       _ExecTraceClear
*/
static int _ExecTraceClear(const char* s) {
  (void)s;
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_FLUSH, NULL);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTraceSetFormat
*/
static int _ExecTraceSetFormat(const char* s) {
  U32 Size;
  U32 Format;
  if (_ParseHex(&s, &Size)) {
    _ReportOutf("Parameters: <4/8/16>\n");
    return JLINK_ERROR_SYNTAX;
  } else {
    if (Size == 4) {
      Format = JLINKARM_TRACE_FORMAT_4BIT;
    } else if (Size == 8) {
      Format = JLINKARM_TRACE_FORMAT_8BIT;
    } else if (Size == 16) {
      Format = JLINKARM_TRACE_FORMAT_16BIT;
    } else {
      _ReportOutf("Invalid trace format\n");
      return JLINK_ERROR_UNKNOWN;
    }
    JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_SET_FORMAT, &Format);
    _ReportOutf("Trace format has been set to %d bits\n", Size);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTraceStart
*/
static int _ExecTraceStart(const char* s) {
  U32 TFreq;
  
  (void)s;
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_START, &TFreq);
  if (TFreq) {
    _ReportOutf("Trace started, Trace frequency is %dkHz\n", TFreq / 1000);
  } else {
    _ReportOutf("Trace could not be started, no trace clock.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTraceStop
*/
static int _ExecTraceStop(const char* s) {
  U32 TFreq;
  
  (void)s;
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_STOP, &TFreq);
  _ReportOutf("Trace stopped.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTraceSetSize
*/
static int _ExecTraceSetSize(const char* s) {
  U32 Size;
  if (_ParseHex(&s, &Size)) {
    _ReportOutf("Syntax: TSetSize <Size(hex)>\n");
    return JLINK_ERROR_SYNTAX;
  } else {
    JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_SET_CAPACITY, (U32*)&Size);
    _ReportOutf("Trace Buffer has been set to 0x%X Bytes (%dKBytes)\n", Size, Size >> 10);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTraceAddInst
*/
static int _ExecTraceAddInst(const char* s) {
  (void)s;
  // No longer used
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTraceAddBranch
*/
static int _ExecTraceAddBranch(const char* s) {
  (void)s;
  // No longer used
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecResetSetType
*/
static int _ExecResetSetType(const char* s) {
  const char* sName;
  const char* sDesc;
  const char* sErr;
  int NumTypes, i;
  U32 Type;

  sErr = _ParseDec(&s, &Type);
  if (sErr) {
    _ReportOutf("Syntax: RSetType <type>\n\n");
    _ReportOutf("        Types:\n");
    NumTypes = JLINKARM_GetResetTypeDesc(0, 0, 0);
    for (i = 0; i < NumTypes; i++) {
      JLINKARM_GetResetTypeDesc(i, &sName, &sDesc);
      _ReportOutf("        %d = %s (%s)\n", i, sName, sDesc);
    }
    return JLINK_ERROR_SYNTAX;
  } else {
    _ResetType = Type;
    JLINKARM_SetResetType(Type);
    JLINKARM_GetResetTypeDesc(Type, &sName, &sDesc);
    _ReportOutf("Reset type %s: %s\n", sName, sDesc);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetWP
*/
static int _ExecSetWP(const char* s) {
  JLINKARM_DATA_EVENT Event;
  const char* sErr;
  U32 Handle;
  U32 Addr;
  U32 AddrMask;
  U32 Data;
  U32 DataMask;
  U8  Access;
  U8  AccessMask;
  int r;

  Addr       = 0;
  AddrMask   = 0;              // exact match
  Data       = 0;
  DataMask   = 0xffffffff;     // don't care
  Access     = 0;
  AccessMask = JLINKARM_WP_MASK_PRIV;
  //
  // Parse Addr
  //
  sErr = _ParseHex(&s, &Addr);
  if (sErr) {
    goto SyntaxError;
  }
  //
  // Parse optional <accesstype>
  //
  _EatWhiteAndSeparator(&s);
  if        (_CompareCmd(&s, "W") == 0) {
    Access |= JLINKARM_WP_DIR_WR;
  } else if (_CompareCmd(&s, "R") == 0) {
    Access |= JLINKARM_WP_DIR_RD;
  } else if (_CompareCmd(&s, "ANY") == 0) {
    AccessMask |= JLINKARM_WP_MASK_DIR;
  } else {
    AccessMask |= JLINKARM_WP_MASK_DIR;
  }
  //
  // Parse optional <size>
  //
  _EatWhiteAndSeparator(&s);
  if        (_CompareCmd(&s, "S8")  == 0) {
    Access |= JLINKARM_WP_SIZE_8BIT;
  } else if (_CompareCmd(&s, "S16") == 0) {
    Access |= JLINKARM_WP_SIZE_16BIT;
  } else if (_CompareCmd(&s, "S32") == 0) {
    Access |= JLINKARM_WP_SIZE_32BIT;
  } else if (_CompareCmd(&s, "ANY") == 0) {
    AccessMask |= JLINKARM_WP_MASK_SIZE;
  } else {
    AccessMask |= JLINKARM_WP_MASK_SIZE;
  }
  //
  // Parse optional <Data>, <DataMask>
  //
  _EatWhiteAndSeparator(&s);
  if (*s) {
    sErr = _ParseHex(&s, &Data);
    if (sErr) {
      goto SyntaxError;
    }
    DataMask = 0;     // Exact match
    //
    // Parse optional <DataMask>
    //
    _EatWhiteAndSeparator(&s);
    if (*s) {
      sErr = _ParseHex(&s, &DataMask);
      if (sErr) {
        goto SyntaxError;
      }
      //
      // Parse optional <AddrMask>
      //
      _EatWhiteAndSeparator(&s);
      if (*s) {
        sErr = _ParseHex(&s, &AddrMask);
        if (sErr) {
          goto SyntaxError;
        }
      }
    }
  }
  //
  // Set event
  //
  memset(&Event, 0, sizeof(JLINKARM_DATA_EVENT));
  Event.SizeOfStruct = sizeof(JLINKARM_DATA_EVENT);
  Event.Type         = JLINKARM_EVENT_TYPE_DATA_BP;
  Event.Addr         = Addr;
  Event.AddrMask     = AddrMask;
  Event.Data         = Data;
  Event.DataMask     = DataMask;
  Event.Access       = Access;
  Event.AccessMask   = AccessMask;
  r = JLINKARM_SetDataEvent(&Event, &Handle);
  if ((r >= 0) && Handle) {
    _ReportOutf("Watchpoint set @ addr 0x%.8X (Handle = 0x%.8X)", Addr, Handle);
    if ((AccessMask & JLINKARM_WP_MASK_DIR) == 0) {
      if ((Access & JLINKARM_WP_MASK_DIR) == JLINKARM_WP_DIR_WR) {
        _ReportOutf(" Write");
      } else {
        _ReportOutf(" Read");
      }
    }
    if ((AccessMask & JLINKARM_WP_MASK_SIZE) == 0) {
      if        ((Access & JLINKARM_WP_MASK_SIZE) == JLINKARM_WP_SIZE_8BIT) {
        _ReportOutf(" 8-Bit");
      } else if ((Access & JLINKARM_WP_MASK_SIZE) == JLINKARM_WP_SIZE_16BIT) {
        _ReportOutf(" 16-Bit");
      } else if ((Access & JLINKARM_WP_MASK_SIZE) == JLINKARM_WP_SIZE_32BIT) {
        _ReportOutf(" 32-Bit");
      }
    }
    if (DataMask != 0xFFFFFFFF) {
      _ReportOutf(" 0x%.8X", Data);
      if (DataMask != 0) {
        _ReportOutf(" Data mask: 0x%.8X", DataMask);
      }
    }
    if (AddrMask != 0) {
      _ReportOutf(" Addr mask: 0x%.8X", AddrMask);
    }
  } else {
    _ReportOutf("Could not set watchpoint @ addr 0x%.8X\n", Addr);
    return JLINK_ERROR_WARNING;
  }
  _ReportOutf("\n");
  return JLINK_NOERROR;
SyntaxError:
  _ReportOutf("Syntax: <addr> [<accesstype>] [<size>] [<data> [<data mask> [<addr mask>]]]\n\n"
         "        <accesstype> = R | W | ANY\n"
         "        <size>       = S8 | S16 | S32 | ANY\n"
         "        <data>       = Data value that should be read/written\n"
         "                       to trigger watchpoint\n"
         "        <data mask>  = Bits set to 1 are \"do not care\"\n"
         "        <addr mask>  = Bits set to 1 are \"do not care\"\n"
         "Example:\n"
         "Stop if value 0x1000 is written to addr. 0x20000000 (32-bit access):\n"
         "setwp 0x20000000 W S32 0x1000 0 0\n"
        );
  return JLINK_ERROR_SYNTAX;
}

/*********************************************************************
*
*       _ExecClrWP
*/
static int _ExecClrWP(const char* s) {
  const char * sErr;
  I32 WPHandle;
  //
  // Parse handle identifier
  //
  sErr = _ParseHex(&s, (U32*)&WPHandle);
  if (sErr) {
    goto Err;
  }
  if (JLINKARM_ClrWP(WPHandle)) {
    _ReportOutf("Could not clear watchpoint 0x%.8X\n", WPHandle);
    return JLINK_ERROR_WARNING;
  } else {
    _ReportOutf("Watchpoint cleared\n");
  }
  return JLINK_NOERROR;
Err:
  _ReportOutf("Parameters: <handle>\n");
  return JLINK_ERROR_SYNTAX;
}

/*********************************************************************
*
*       _NeedARMThumbSpecForBP
*
*  Function description
*    Determines if for the given device family it is necessary to specify ARM/Thumb when setting a BP
*
*  Return value
*    == 0  No ARM/Thumb spec. needed for BPs
*    == 1  ARM/Thumb spec. needed for BPs
*/
static unsigned _NeedARMThumbSpecForBP(U32 DevFamily) {
  unsigned r;

  switch (DevFamily) {
  case JLINKARM_DEV_FAMILY_ARM7:             r = 1; break;
  case JLINKARM_DEV_FAMILY_ARM9:             r = 1; break;
  case JLINKARM_DEV_FAMILY_ARM11:            r = 1; break;
  case JLINKARM_DEV_FAMILY_CORTEX_A5:        r = 1; break;
  case JLINKARM_DEV_FAMILY_CORTEX_R4:        r = 1; break;
  case JLINKARM_DEV_FAMILY_CORTEX_A8:        r = 1; break;
  case JLINKARM_DEV_FAMILY_CORTEX_AR_ARMV8:  r = 1; break;
  default:                                   r = 0; break;
  }
  return r;
}

/*********************************************************************
*
*       _ExecSetBP
*/
static int _ExecSetBP(const char* s) {
  char c;
  U64 Addr;
  U32 BPType;
  int BPHandle;
  int r;
  U32 DevFamily;

  BPType = JLINKARM_BP_TYPE_ARM;
  DevFamily = JLINKARM_GetDeviceFamily();
  //
  // Parse Addr
  //
  r = UTIL_ParseHex64(&s, &Addr);
  if (r < 0) {
    goto Err;
  }
  //
  // Check breakpoint mode (ARM/THUMB) only for ARM7/9/11 and Cortex-R4
  //
  r = _NeedARMThumbSpecForBP(DevFamily);
  if (r) {
    //
    // Parse <A/T>
    //
    _EatWhite(&s);
    c = UTIL_tolower(*s);
    if (c == 'a') {
      s++;
      BPType = JLINKARM_BP_TYPE_ARM;      // ARM mode
    } else if (c == 't') {
      s++;
      BPType = JLINKARM_BP_TYPE_THUMB;    // THUMB mode
    } else {
      _ReportOutf("Syntax: setbp <Addr> <A/T> [S/H]\n");
      return JLINK_ERROR_SYNTAX;
    }
  } else if (DevFamily == JLINK_DEV_FAMILY_MIPS) {
    //
    // For MIPS, breakpoint size has to be defined
    // W = Word => MIPS32 mode
    // H = Half-word => MIPS16e mode
    //
    // Parse optional <W/H>
    //
    _EatWhite(&s);
    c = UTIL_tolower(*s);
    if (c == 'w') {
      s++;
      BPType = JLINKARM_BP_MODE1;      // MIPS32 mode
    } else if (c == 'h') {
      s++;
      BPType = JLINKARM_BP_MODE2;      // MIPS16e mode
    } else {
      _ReportOutf("Syntax: setbp <Addr> <W/H> [S/H]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Parse optional <S/H>
  //
  _EatWhite(&s);
  c = UTIL_tolower(*s);
  if (c == 's') {
    s++;
    BPType |= JLINKARM_BP_TYPE_SW;      // Software breakpoint
  }
  if (c == 'h') {
    s++;
    BPType |= JLINKARM_BP_TYPE_HW;      // Hardware breakpoint
  }
  BPHandle = JLINK_SetBPEx_64(Addr, BPType);
  if (BPHandle) {
    _ReportOutf("Breakpoint set @ addr 0x%.8llX (Handle = %d)\n", Addr, BPHandle);
  } else {
    _ReportOutf("Could not set breakpoint @ addr 0x%.8llX\n", Addr);
    return JLINK_ERROR_WARNING;
  }
  return JLINK_NOERROR;
Err:
  switch (DevFamily) {
  case JLINKARM_DEV_FAMILY_ARM7:
  case JLINKARM_DEV_FAMILY_ARM9:
  case JLINKARM_DEV_FAMILY_ARM10:
  case JLINKARM_DEV_FAMILY_ARM11:
  case JLINKARM_DEV_FAMILY_CORTEX_R4:
  case JLINKARM_DEV_FAMILY_CORTEX_A5:
  case JLINKARM_DEV_FAMILY_CORTEX_A8:
    _ReportOutf("Parameters: <addr> <A/T> [S/H]\n");
    break;
  case JLINKARM_DEV_FAMILY_CM1:
  case JLINKARM_DEV_FAMILY_CM3:
  case JLINKARM_DEV_FAMILY_CM0:
  case JLINKARM_DEV_FAMILY_RX:
  case JLINKARM_DEV_FAMILY_CM4:
    _ReportOutf("Parameters: <Addr> [S/H]\n");
    break;
  case JLINK_DEV_FAMILY_MIPS:
    _ReportOutf("Parameters: <Addr> <W/H> [S/H]\n");
    break;
  }
  return JLINK_ERROR_SYNTAX;
}

/*********************************************************************
*
*       _Reset
*/
static void _Reset(int Delay) {
  const char* sName;
  const char* sDesc;
  JLINKARM_GetResetTypeDesc(_ResetType, &sName, &sDesc);
  _ReportOutf("Reset delay: %d ms\n", Delay);
  _ReportOutf("Reset type %s: %s\n", sName, sDesc);
  JLINKARM_SetResetDelay(Delay);
  JLINKARM_Reset();
}

/*********************************************************************
*
*       _ExecTraceShowRegions
*/
static int _ExecTraceShowRegions(const char* s) {
  (void)s;
  TRACE_ShowRegions();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetLittleEndian
*/
static int _ExecSetLittleEndian(const char* s) {
  (void)s;
  JLINKARM_SetEndian(ARM_ENDIAN_LITTLE);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetBigEndian
*/
static int _ExecSetBigEndian(const char* s) {
  (void)s;
  JLINKARM_SetEndian(ARM_ENDIAN_BIG);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecStartTrace
*/
static int _ExecStartTrace(const char* s) {
  (void)s;
  JLINKARM_ETM_StartTrace();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGo
*/
static int _ExecGo(const char* s) {
  U32 NumSteps = 0;
  U32 Flags = JLINKARM_GO_OVERSTEP_BP;

  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &NumSteps)) {
      _ReportOutf("Syntax: g [<NumSteps> [<Flags>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (_ParseHex(&s, &Flags)) {
        _ReportOutf("Syntax: g [<NumSteps> [<Flags>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  JLINKARM_GoEx(NumSteps, Flags);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecHalt
*/
static int _ExecHalt(const char* s) {
  (void)s;
  if (!JLINKARM_Halt()) {
    _ShowRegs();
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _GetCurrentPC
*/
static int _GetCurrentPC(U32* pCurrentPC) {
  U32 DevFamily;
  int RegIndexPC;

  DevFamily  = JLINKARM_GetDeviceFamily();
  switch (DevFamily) {
  case JLINKARM_DEV_FAMILY_CF:           RegIndexPC = JLINK_CF_REG_PC;            break;
  case JLINKARM_DEV_FAMILY_CM0:          RegIndexPC = JLINKARM_CM3_REG_R15;       break;
  case JLINKARM_DEV_FAMILY_CM1:          RegIndexPC = JLINKARM_CM3_REG_R15;       break;
  case JLINKARM_DEV_FAMILY_CM3:          RegIndexPC = JLINKARM_CM3_REG_R15;       break;
  case JLINKARM_DEV_FAMILY_CM4:          RegIndexPC = JLINKARM_CM3_REG_R15;       break;
  case JLINKARM_DEV_FAMILY_CORTEX_R4:    RegIndexPC = JLINKARM_CORTEX_R4_REG_R15; break;
  case JLINKARM_DEV_FAMILY_RX:           RegIndexPC = JLINKARM_RX_REG_PC;         break;
  case JLINKARM_DEV_FAMILY_POWER_PC:     RegIndexPC = JLINK_POWER_PC_REG_PC;      break;
  case JLINKARM_DEV_FAMILY_CORTEX_A5:    RegIndexPC = JLINKARM_CORTEX_R4_REG_R15; break;
  case JLINKARM_DEV_FAMILY_CORTEX_A8:    RegIndexPC = JLINKARM_CORTEX_R4_REG_R15; break;
  case JLINK_DEV_FAMILY_MIPS:            RegIndexPC = JLINK_MIPS_REG_PC;          break;
  case JLINKARM_DEV_FAMILY_ARM7:         RegIndexPC = ARM_REG_R15;                break;
  case JLINKARM_DEV_FAMILY_ARM9:         RegIndexPC = ARM_REG_R15;                break;
  case JLINKARM_DEV_FAMILY_ARM11:        RegIndexPC = ARM_REG_R15;                break;
  case JLINK_DEV_FAMILY_EFM8:            RegIndexPC = JLINK_EFM8_PC;              break;
  case JLINK_DEV_FAMILY_RISC_V:          RegIndexPC = JLINK_RISCV_REG_PC;         break;
  case JLINK_DEV_FAMILY_CORTEX_AR_ARMV8: RegIndexPC = JLINK_ARM_V8AR_REG_PC;      break;
  default:                            return -1;
  }
  *pCurrentPC = JLINKARM_ReadReg(RegIndexPC);
  return 0;
}

/*********************************************************************
*
*       _ExecIsHalted
*/
static int _ExecIsHalted(const char* s) {
  U32 CurrentPC;
  int r;

  (void)s;
  if (JLINKARM_IsHalted() == 1) {
    r = _GetCurrentPC(&CurrentPC);
    if (r >= 0) {
      _ReportOutf("CPU is halted (PC = 0x%.8X).\n", CurrentPC);
    } else {
      _ReportOutf("CPU is halted.\n");
    }
  } else {
    _ReportOutf("CPU is not halted.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWaitHalt
*
*  Function description
*    Waits until the CPU is halted or the given timeout exceeded.
*
*  Syntax
*    WaitHalt <TimeoutMs>  Default timeout is 1000 ms
*
*  Return value
*    See JLINK_ERROR_ defines
*/
static int _ExecWaitHalt(const char* s) {
  U32 Timeout;
  int r;

  Timeout = 1000;
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &Timeout)) {
      _ReportOutf("Syntax: wh [<Timeout>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  r = JLINKARM_WaitForHalt(Timeout);
  if (r > 0) {
    _ReportOutf("CPU halted.\n", Timeout);
    _ShowRegs();
  } else if (r == 0) {
    _ReportOutf("CPU not halted. Timeout of %d ms exceeded\n", Timeout);
  } else {
    _ReportOutf("CPU not halted. Error %d while requesting CPU state\n", r);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClock
*/
static int _ExecClock(const char* s) {
  U32 RepCount = 1;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &RepCount)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  while (RepCount--) {
    JLINKARM_Clock();
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecC00
*/
static int _ExecC00(const char* s) {
  U32 RepCount = 1;
  U8 tdi = 0;
  U8 tms = 0;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &RepCount)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  while (RepCount--) {
    JLINKARM_JTAG_StoreRaw(&tdi, &tms, 1);
  }
  JLINKARM_JTAG_SyncBits();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClrTRST
*/
static int _ExecClrTRST(const char* s) {
  (void)s;
  JLINKARM_ClrTRST();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetTRST
*/
static int _ExecSetTRST(const char* s) {
  (void)s;
  JLINKARM_SetTRST();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClrRESET
*/
static int _ExecClrRESET(const char* s) {
  (void)s;
  JLINKARM_ClrRESET();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetRESET
*/
static int _ExecSetRESET(const char* s) {
  (void)s;
  JLINKARM_SetRESET();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClrTMS
*/
static int _ExecClrTMS(const char* s) {
  (void)s;
  JLINKARM_ClrTMS();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetTMS
*/
static int _ExecSetTMS(const char* s) {
  (void)s;
  JLINKARM_SetTMS();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClrTCK
*/
static int _ExecClrTCK(const char* s) {
  int r;

  (void)s;
  r = JLINKARM_ClrTCK();
  if (r < 0) {
    _ReportOutf("Firmware of connected emulator does not support this feature.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetTCK
*/
static int _ExecSetTCK(const char* s) {
  int r;

  (void)s;
  r = JLINKARM_SetTCK();
  if (r < 0) {
    _ReportOutf("Firmware of connected emulator does not support this feature.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClrTDI
*/
static int _ExecClrTDI(const char* s) {
  (void)s;
  JLINKARM_ClrTDI();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetTDI
*/
static int _ExecSetTDI(const char* s) {
  (void)s;
  JLINKARM_SetTDI();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecResetTRST
*/
static int _ExecResetTRST(const char* s) {
  (void)s;
  JLINKARM_ResetTRST();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecResetTAP
*/
static int _ExecResetTAP(const char* s) {
  (void)s;
  JLINKARM_StoreBits(0x1F, 0, 6);
  JLINKARM_JTAG_SyncBytes();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecIdentifySCSRLen
*/
static int _ExecIdentifySCSRLen(const char* s) {
  (void)s;
  _ReportOutf("JTAG scan length: %d\n", JLINKARM_GetScanLen());
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecIdentify
*/
static int _ExecIdentify(const char* s) {
  (void)s;
  _PrintId(JLINKARM_GetId());
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowHWStatus
*/
static int _ExecShowHWStatus(const char* s) {
  (void)s;
  _ShowHWStatus();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowHWInfo
*/
static int _ExecShowHWInfo(const char* s) {
  (void)s;
  _ShowHWInfo();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowFirmwareInfo
*/
static int _ExecShowFirmwareInfo(const char* s) {
  (void)s;
  _ShowFirmwareInfo();
  _ReportOutf("\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowICERegs
*/
static int _ExecShowICERegs(const char* s) {
  (void)s;
  _ShowDebugState();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowETMState
*/
static int _ExecShowETMState(const char* s) {
  (void)s;
  _ShowETMState();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteETMReg
*/
static int _ExecWriteETMReg(const char* s) {
  U32 Addr, Data;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: we <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  s++;
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: we <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ReportOutf("ETMreg[%.8X] = %.8X\n", Addr, Data);
  JLINKARM_ETM_WriteReg(Addr, Data, 0);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadETMReg
*/
static int _ExecReadETMReg(const char* s) {
  U32 Addr, Data;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: re <Addr>\n");
    return JLINK_ERROR_SYNTAX;
  }
  Data = JLINKARM_ETM_ReadReg(Addr);
  _ReportOutf("ETMreg[%.8X] = %.8X\n", Addr, Data);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowETBState
*/
static int _ExecShowETBState(const char* s) {
  (void)s;
  _ShowETBState();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteETBReg
*/
static int _ExecWriteETBReg(const char* s) {
  U32 Addr, Data;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: wb <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  s++;
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: wb <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ReportOutf("ETBreg[%.2X] = %.8X\n", Addr, Data);
  JLINKARM_ETB_WriteReg(Addr, Data, 0);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadETBReg
*/
static int _ExecReadETBReg(const char* s) {
  U32 Addr, Data;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: rb <Addr>\n");
    return JLINK_ERROR_SYNTAX;
  }
  Data = JLINKARM_ETB_ReadReg(Addr);
  _ReportOutf("ETBreg[%.2X] = %.8X\n", Addr, Data);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _DisassembleInst
*
*  Function description
*    Disassembles an instruction at the specified address
*
*  Return value
*    >= 0  InstSize
*     < 0  Error
*/
static int _DisassembleInst(char* pBuffer, U32 Addr, const JLINK_DISASSEMBLY_INFO* pInfo, const char* sAddInfo, U32 BufSize) {
  char acDA[64];
  U8   abData[32];
  U8*  p;
  int  InstSize;
  int  Offset;
  int  NumBytesAtOnce;
  int  r;

  if (pInfo == NULL) {
    InstSize = JLINKARM_DisassembleInst(acDA, sizeof(acDA), Addr);
    if (InstSize < 0) {
      UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s???                ???\n", Addr, sAddInfo ? sAddInfo : "");
      return -1;
    }
  } else {
    InstSize = JLINKARM_DisassembleInstEx(acDA, sizeof(acDA), Addr, pInfo);
    if (InstSize < 0) {
      UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s???                ???\n", Addr, sAddInfo ? sAddInfo : "");
      return -1;
    }
  }
  //
  // For 8051 devices, we need to make sure that we read from the CODE area
  //
  if (JLINKARM_GetDeviceFamily() == JLINK_DEV_FAMILY_EFM8) {
    r = JLINKARM_ReadMemEx(Addr | JLINK_EFM8_START_ADDR_CODE, InstSize, abData, 0);
  } else {
    r = JLINKARM_ReadMemEx(Addr, InstSize, abData, 0);
  }
  if (r != InstSize) {
    UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s???                %s\n", Addr, sAddInfo ? sAddInfo : "", acDA);
    return -1;
  }
  switch (InstSize) {
  case 0:   UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s -                  %s\n",              Addr, sAddInfo ? sAddInfo : "",                                                                   acDA);  break;
  case 1:   UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s %.2X                 %s\n",            Addr, sAddInfo ? sAddInfo : "", abData[0],                                                        acDA);  break;
  case 2:   UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s %.2X %.2X              %s\n",          Addr, sAddInfo ? sAddInfo : "", abData[0], abData[1],                                             acDA);  break;
  case 3:   UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s %.2X %.2X %.2X           %s\n",        Addr, sAddInfo ? sAddInfo : "", abData[0], abData[1], abData[2],                                  acDA);  break;
  case 4:   UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s %.2X %.2X %.2X %.2X        %s\n",      Addr, sAddInfo ? sAddInfo : "", abData[0], abData[1], abData[2], abData[3],                       acDA);  break;
  case 5:   UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s %.2X %.2X %.2X %.2X %.2X     %s\n",    Addr, sAddInfo ? sAddInfo : "", abData[0], abData[1], abData[2], abData[3], abData[4],            acDA);  break;
  default:  UTIL_snprintf(pBuffer, BufSize, "%.8X:  %s %.2X %.2X %.2X %.2X %.2X %.2X  %s\n",  Addr, sAddInfo ? sAddInfo : "", abData[0], abData[1], abData[2], abData[3], abData[4], abData[5], acDA);  break;
  }
  if (InstSize > 6) {
    Offset = 6;
    do {
      NumBytesAtOnce = MIN(InstSize - Offset, 6);
      p = &abData[Offset];
      switch (NumBytesAtOnce) {
      case 1:   UTIL_snprintf(pBuffer, BufSize, "%s           %.2X\n",                          pBuffer, *(p+0));                                         break;
      case 2:   UTIL_snprintf(pBuffer, BufSize, "%s           %.2X %.2X\n",                     pBuffer, *(p+0), *(p+1));                                 break;
      case 3:   UTIL_snprintf(pBuffer, BufSize, "%s           %.2X %.2X %.2X\n",                pBuffer, *(p+0), *(p+1), *(p+2));                         break;
      case 4:   UTIL_snprintf(pBuffer, BufSize, "%s           %.2X %.2X %.2X %.2X\n",           pBuffer, *(p+0), *(p+1), *(p+2), *(p+3));                 break;
      case 5:   UTIL_snprintf(pBuffer, BufSize, "%s           %.2X %.2X %.2X %.2X %.2X\n",      pBuffer, *(p+0), *(p+1), *(p+2), *(p+3), *(p+4));         break;
      default:  UTIL_snprintf(pBuffer, BufSize, "%s           %.2X %.2X %.2X %.2X %.2X %.2X\n", pBuffer, *(p+0), *(p+1), *(p+2), *(p+3), *(p+4), *(p+5)); break;
      }
      Offset += NumBytesAtOnce;
    } while (Offset < InstSize);
  }

  return InstSize;
}

/*********************************************************************
*
*       _ExecStep
*/
static int _ExecStep(const char* s) {
  char ac[256];
  U32 CurrentPC;
  int NumSteps;
  int r;
  int i;
  _REGS_STAT* pRegsStat;

  NumSteps = 1;                      // If user did not speciy the number of steps, we perform a single step
  _EatWhite(&s);
  if (*s == ',') {
    s++;
    _EatWhite(&s);
  }
  if (*s) {
    if (_ParseDec(&s, &NumSteps)) {
      _ReportOutf("Syntax: TestStep [<NumSteps>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  if (JLINKARM_IsHalted() == 0) {
    _cbErrorOut("CPU is not halted");
    return JLINK_ERROR_UNKNOWN;
  }
  pRegsStat = _DiffRegs_Init();
  for (i = 0; i < NumSteps; i++) {
    r = _GetCurrentPC(&CurrentPC);
    if (r >= 0) {
      r = _DisassembleInst(ac, CurrentPC, NULL, NULL, UTIL_SIZEOF(ac));
      if (r >= 0) {
        _ReportOutf(ac);
      }
    }
    JLINKARM_Step();
    _DiffRegs_PrintDiff(pRegsStat);
    _DiffRegs_Update(pRegsStat);
  }
  _DiffRegs_DeInit(pRegsStat);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecStepOver
*
*  Function description
*    Performs a step-over, meaning a branch instruction will not be stepped but a BP would be set afterwards instead and a Go() is issued.
*
*  Notes
*    (1) ARM 7/9 and Cortex-A/R are currently not supported
*        because for these cores we need to know what mode (ARM/Thumb)
*        the core is in after the instruction we want to overstep. (Because we need to specify the mode for the BP to be set)
*        This is almost impossible without having ELF information
*/
static int _ExecStepOver(const char* s) {
  char acDA[64];
  int r;
  int Result;
  int InstSize;
  int i;
  int hBP;
  int t;
  int tDelta;
  unsigned IsSupported;
  U32 CurrentPC;
  U32 DevFamily;
  JLINK_DISASSEMBLY_INFO DAInfo;
  const U32 aSuppDevFamilies[] = { JLINKARM_DEV_FAMILY_CM1, JLINKARM_DEV_FAMILY_CM3, JLINKARM_DEV_FAMILY_CM0, JLINKARM_DEV_FAMILY_CM4, JLINK_DEV_FAMILY_RISC_V };  // See (1)
  //
  // Early outs
  //
  (void)s;
  Result = JLINK_NOERROR;
  r = JLINKARM_IsHalted();
  if (r == 0) {                           // If CPU is running, step over is not possible
    _cbErrorOut("CPU is not halted");
    return JLINK_ERROR_UNKNOWN;
  }
  DevFamily = JLINKARM_GetDeviceFamily();
  IsSupported = 0;
  for (i = 0; i < COUNTOF(aSuppDevFamilies); i++) {
    if (aSuppDevFamilies[i] == DevFamily) {
      IsSupported = 1;
      break;
    }
  }
  if (IsSupported == 0) {
    _ReportOutf("Stepover not supported for this CPU. Performing single step\n");
    InstSize = -1;
    goto Done;
  }
  //
  // Ask disassembly module if we can get some information
  // about the current instruction to set a BP after the current <inst>
  //
  r = _GetCurrentPC(&CurrentPC);
  _ReportOutf("Overstepping inst. @ 0x%.8X\n", CurrentPC);
  DAInfo.SizeOfStruct = sizeof(DAInfo);
  InstSize = JLINKARM_DisassembleInstEx(acDA, sizeof(acDA), CurrentPC, &DAInfo);
  if (InstSize > 0) {
    //
    // Set temporary BP after current <inst> and start core
    //
    hBP = JLINKARM_SetBPEx(CurrentPC + InstSize, JLINKARM_BP_IMP_ANY | JLINKARM_BP_MODE1);
    if (hBP > 0) {
      JLINKARM_GoEx(JLINKARM_GO_MAX_EMUL_INSTS_DEFAULT, JLINKARM_GO_FLAG_OVERSTEP_BP);
      //
      // Wait until BP is hit
      //
      t = SYS_GetTickCount();
      do {
        r = JLINKARM_IsHalted();
        if (r) {
          break;
        }
        //
        // Manually halt CPU if BP is not hit in time
        //
        tDelta = SYS_GetTickCount() - t;
        if (tDelta > _TimeoutStepOver) {
          _ReportOutf("CPU did not halt after %d ms. Manually halting CPU\n", _TimeoutStepOver);
          r = JLINKARM_Halt();
          if (r) {
            _cbErrorOut("Failed to halt CPU");
            Result = JLINK_ERROR_UNKNOWN;
          }
          break;
        }
      } while (1);
      //
      // Clear temporary BP
      //
      JLINKARM_ClrBPEx(hBP);
    } else {
      InstSize = -1;
      _ReportOutf("Cannot set breakpoint. Performing single step\n");
    }
  } else {
    _ReportOutf("Cannot determine instruction size. Performing single step\n");
  }
Done:
  if (InstSize < 0) {
    //
    // In case we could not use the BP, perform a single step
    //
    Result = _ExecStep("");
  }
  return Result;
}

/*********************************************************************
*
*       _ExecSetTimeoutCmd
*
*  Function description
*    Sets the timeout for a specific J-Link Commander command
*    Syntax: SetTimeoutCmd <Cmd> = <TimeoutMs>
*/
static int _ExecSetTimeoutCmd(const char* s) {
  char ac[64];
  U32 Timeout;
  int r;
  int Result;
  //
  // Parse <Cmd>
  //
  Result = JLINK_NOERROR;
  _EatWhite(&s);
  ac[0] = 0;
  UTIL_ParseString(&s, ac, UTIL_SIZEOF(ac));
  //
  // Parse <TimeoutMs>
  //
  UTIL_ParseChar(&s, '=');           // Optional
  r = UTIL_ParseIntEx(&s, &Timeout);
  if (r < 0) {
    _ReportOutf("SetTimeoutCmd: Error while parsing <TimeoutMs>. Syntax: SetTimeoutCmd <Cmd> = <TimeoutMs>\n");
    Result = JLINK_ERROR_SYNTAX;
    goto Done;
  }
  if (UTIL_stricmp(ac, "stepover") == 0) {
    _ReportOutf("O.K., Setting timeout for <Cmd> \"stepover\" to %d ms\n", Timeout);
    _TimeoutStepOver = Timeout;
  } else {
    _ReportOutf("SetTimeoutCmd: Unknown <Cmd>: %s. Syntax: SetTimeoutCmd <Cmd> = <TimeoutMs>\n", ac);
  }
Done:
  return Result;
}

/*********************************************************************
*
*       _ExecDisassemble
*/
static int _ExecDisassemble(const char* s) {
  char ac[256];
  U32 Addr;
  U32 NumInsts     = 10;
  int UseCurrentPC = 1;
  int r;
  JLINK_DISASSEMBLY_INFO DAInfo;

  Addr = 0;  // Avoid false-positive "unintinialized variable" warning
  DAInfo.SizeOfStruct = sizeof(JLINK_DISASSEMBLY_INFO);
  DAInfo.Mode = JLINK_DISASSEMBLY_MODE_CURRENT;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (*s == ',') {
      goto ParseNumInsts;
    }
    if (_JLinkstrnicmp(s, "PC", 2) == 0) {
      s += 2;
      goto ParseNumInsts;
    }
    if (_ParseHex(&s, &Addr)) {
      _ReportOutf("Syntax: disassemble <Addr> [<NumInsts>]\n");
      return JLINK_ERROR_SYNTAX;
    }
    UseCurrentPC = 0;
ParseNumInsts:
    _EatWhite(&s);
    if (*s) {
      if (*s == ',') {
        s++;
      }
      if (_ParseDec(&s, &NumInsts)) {
        _ReportOutf("Syntax: disassemble <Addr> [<NumInsts>]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
    _EatWhite(&s);
    if (*s) {
      if (_JLinkstrnicmp(s, "ARM", 3) == 0) {
        DAInfo.Mode = JLINK_DISASSEMBLY_MODE_ARM;
      } else if (_JLinkstrnicmp(s, "THUMB", 5) == 0) {
        DAInfo.Mode = JLINK_DISASSEMBLY_MODE_THUMB;
      }
    }
  }
  //
  // Get current PC if necessary
  //
  if (UseCurrentPC) {
    if (JLINKARM_IsHalted() == 0) {
      _cbErrorOut("CPU is not halted");
      return JLINK_ERROR_UNKNOWN;
    }
    r = _GetCurrentPC(&Addr);
    if (r < 0) {
      _cbErrorOut("Could not read current PC");
      return JLINK_ERROR_UNKNOWN;
    }
  }
  //
  // Disassemble instructions
  //
  while (NumInsts) {
    r = _DisassembleInst(ac, Addr, &DAInfo, NULL, UTIL_SIZEOF(ac));
    _ReportOutf(ac);
    if (r < 0) {
      break;
    }
    Addr += r;
    NumInsts--;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowRegs
*/
static int _ExecShowRegs(const char* s) {
  (void)s;
  if (JLINKARM_IsHalted() > 0) {
    _ShowRegs();
  } else {
    _ReportOutf("CPU is not halted !\n");
    return JLINK_ERROR_WARNING;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecLog
*/
static int _ExecLog(const char* s) {
  char ac[MAX_PATH+10];
  _ParseString(&s, ac, sizeof(ac));
  JLINKARM_SetLogFile(ac);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _UnlockLM3Sxxx
*
*    The internal flash of the device is erased via a special unlock sequence:
*     1. nRESET is pulled LOW and hold it LOW
*     2. Perform switching sequence JTAG->SWD then SWD->JTAG
*     3. Perform 2. again 4 times.
*     4. Release nRESET
*    JTAG-to-SWD Switching
*    -----------------------
*    1. Send at least 50 TCK/SWCLK cycles with TMS/SWDIO set to 1.
*       This ensures that both JTAG and SWD are in their reset/idle states.
*    2. Send the 16-bit JTAG-to-SWD switch sequence, 0xE79E. (Sending LSB first)
*    3. Send at least 50 TCK/SWCLK cycles with TMS/SWDIO set to 1.
*       This ensures that if SWJ-DP was already in SWD mode, before sending the switch sequence,
*       the SWD goes into the line reset state.
*    SWD-to-JTAG Switching
*    -----------------------
*    1. Send at least 50 TCK/SWCLK cycles with TMS/SWDIO set to 1. This ensures that both JTAG and
*       SWD are in their reset/idle states.
*    2. Send the 16-bit SWD-to-JTAG switch sequence, 0xE73C. (Sending LSB first)
*    3. Send at least 5 TCK/SWCLK cycles with TMS/SWDIO set to 1.
*       This ensures that if SWJ-DP was already in JTAG mode, before sending the switch sequence,
*       the JTAG goes into the Test Logic Reset state.
*/
static int _UnlockLM3Sxxx(const char * s) {
  int i;
  U8 aTDI[7] = {0};
  U8 aTMS[7] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x03};  // 50 clocks with TMS high to go to reset/idle
  U16 SwitchToSWD;
  U16 SwitchToJTAG;
  U32 DisInteraction;

  _ReportOutf("Be sure that nRESET is LOW before continuing.\n"
         "If you are working with an eval board,\n"
         "please press the RESET button and keep it pressed.\n"
        );
  _ParseDec(&s, &DisInteraction);
  if (DisInteraction != 1) {
    _ReportOutf("Press enter to start unlock sequence...");
    getchar();
  }
  _ReportOutf("Unlocking device...");
  SwitchToSWD  = 0xE79E;
  SwitchToJTAG = 0xE73C;
  //
  // Make sure that all JTAG sequences are output before we set RESET low.
  // Since while reset is active, the LM3S devices do not respond with
  // their IRPrint when sending a JTAG command.
  // This would cause a "Bad JTAG communication" error.
  //
  JLINKARM_JTAG_SyncBits();
  JLINKARM_ClrRESET();
  for (i = 0; i < 5; i++) {
    //
    // Switch to SWD
    //
    JLINKARM_JTAG_StoreRaw(&aTDI[0], &aTMS[0], 50);
    JLINKARM_JTAG_StoreRaw(&aTDI[0], (U8 *)&SwitchToSWD, 16);
    JLINKARM_JTAG_StoreRaw(&aTDI[0], &aTMS[0], 50);
    //
    // Switch back to JTAG
    //
    JLINKARM_JTAG_StoreRaw(&aTDI[0], &aTMS[0], 50);
    JLINKARM_JTAG_StoreRaw(&aTDI[0], (U8 *)&SwitchToJTAG, 16);
    JLINKARM_JTAG_StoreRaw(&aTDI[0], &aTMS[0], 50);
    JLINKARM_JTAG_SyncBits();
  }
  JLINKARM_SetRESET();
  _Sleep(400);
  _ReportOutf("O.K.\n");
  _ReportOutf("Please power-cycle target hardware.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _UnlockKinetis
*
*  Function description
*    Freescale Kinetis K40 and K60 devices unlock
*    For information about the MDM-AP, please refer to
*    \\fileserver\techinfo\Company\Freescale\MCU\CortexM4_Kinetis\K40P144M100SF2RM_review.pdf
*/
static int _UnlockKinetis(const char * s) {
  int r;
  int Stat;
  U32 Data;
  int t;

  (void)s;
  Stat = JLINK_ERROR_UNKNOWN;
  //
  // Freescale Kinetis K40 and K60 devices unlock
  // For information about the MDM-AP, please refer to
  // \\fileserver\techinfo\Company\Freescale\MCU\CortexM4_Kinetis\K40P144M100SF2RM_review.pdf
  //
  JLINKARM_TIF_Select(1);
  _TIF_SWD_Select();
  _ReportOutf("Unlocking device...");
  JLINKARM_ClrRESET();
  _Sleep(50);
  _TIF_SWD_WriteU32(DP_REG_ABORT, 0x1E);     // Clear all sticky error flags
  _TIF_SWD_WriteU32(DP_REG_SELECT, 1 << 24); // Select AP[1] which is the MDM-AP
  r = _ReadUntilOK(0 | (1 << 2), &Data);     // Trigger read of AP[1] bank 0, reg 0 which is the MDM-AP status register
  if (r < 0) {
    goto Done;
  }
  r = _ReadUntilOK(0 | (1 << 2), &Data);     // Trigger second read of AP[1] bank 0, reg 0 which is the MDM-AP status register. This time we will get valid data (from the read which was triggered first)
  if (r < 0) {
    goto Done;
  }
  //
  // In case of enabled system security
  // also check if mass erase is allowed
  // If the system security is not active, mass erase disabled set has no effect
  //
  if (Data & (1 << 2)) {
    if ((Data & (1 << 5)) == 0) {
      _ReportOutf("Unlock via debug port is disabled. Unlock failed.\n");
    }
  }
  _TIF_SWD_WriteU32(1 | (1 << 2), 1 << 0);   // Write AP[1] bank 0, reg 1 which is the MDM-AP control register. Bit 0 requests a mass erase of the flash memory. This will also disable the security.
  //
  // Wait until mass erase command has been accepted
  // by checking the "Flash Mass Erase Acknowledge"
  // bit in the status register
  //
  r = _ReadUntilOK(0 | (1 << 2), &Data);     // Trigger the first read outside the loop to make sure that we will always get valid data inside the loop
  if (r < 0) {
    goto Done;
  }
  t = SYS_GetTickCount() + 3000;
  do {
    r = _ReadUntilOK(0 | (1 << 2), &Data);
    if (r < 0) {
      goto Done;
    }
    if (Data & (1 << 0)) {                   // "Flash Mass Erase Acknowledge" bit is set as soon as the mass erase command has been accepted
      break;
    }
    if ((t - SYS_GetTickCount()) <= 0) {
      _ReportOutf("Timeout while unlocking device.\n");
      goto Done;
    }
  } while (1);
  //
  // Wait for mass erase to complete
  // Read AP[1] bank 0, reg 1 (MDM-AP control register)
  // in order to check if mass erase has been completed.
  //
  r = _ReadUntilOK(1 | (1 << 2), &Data);     // Trigger the first read outside the loop to make sure that we will always get valid data inside the loop
  if (r < 0) {
    goto Done;
  }
  t = SYS_GetTickCount() + 3000;
  do {
    r = _ReadUntilOK(1 | (1 << 2), &Data);
    if (r < 0) {
      goto Done;
    }
    if ((Data & (1 << 0)) == 0) {            // Flash mass erase in Progress bit is cleared automatically when the mass operation completes.
      break;
    }
    if ((t - SYS_GetTickCount()) <= 0) {
      _ReportOutf("Timeout while unlocking device.\n");
      goto Done;
    }
  } while (1);
  _ReportOutf("O.K.\n");
  Stat = JLINK_NOERROR;
Done:
  JLINKARM_SetRESET();
  _Sleep(200);
  return Stat;
}

/*********************************************************************
*
*       _UnlockEFM32Gxxx
*
*  Function description
*    For the EFM32Gxxx we use a J-Link PCode for the unlock prcedure.
*    PCode execution has to be supported by the connected emulator
*    in order to use this unlock feature
*/
static int _UnlockEFM32Gxxx(const char * s) {
  PCODE_PROGRAM Program = {
    sizeof(PCODE_PROGRAM),
    {
       //
       // Select interface
       //
       S32_MOVI7(0, 1)                                                                      // Move Interface (TIF_SWD == 1) to register 0
      ,S32_PUSH(0, 1)                                                                           // Push R0 onto the stack
      ,S32_CALLH(4, 1)                                                                          // CALLH(Off = 2) which is _TIF_Sel. 1 Parameter is used
       //
       // Set speed
       //
      ,S32_MOVI32(0, 0x3D0900)                                                              // Move Speed (4000000 Hz = 4 MHz) to register 0
      ,S32_PUSH(0, 1)                                                                           // Push R0 onto the stack
      ,S32_CALLH(8, 1)                                                                          // CALLH(Off = 4) which is _TIF_SetSpeed. 1 Parameter is used
       //
       // Prepare parameters for the unlock sequence, since these registers
       // are not affected by the reset-related operations
       // This saves some time between reset release and start of sending the sequence
       // Register usage:
       // R0 Used to setup speed & interface. Furthermore used for return values of host-functions
       // R1 Reset hold time
       // R2 pDataIn for _TIF_SWD_Transfer
       // R3 pDirection for _TIF_SWD_Transfer
       // R4 pDataOut for _TIF_SWD_Transfer
       // R5 NumBits for _TIF_SWD_Transfer
       // R6 TRY_CNT_MAX
       // R7 decrement for TRY_CNT_MAX
       // R8 Pointer to first byte of IDCode
       // R9 Reference value for IDCode
       // R10 Holds IDCode pointed to by R8
       // R11 pDataIn for remaining unlock sequence
       // R12 pDirection for remaining unlock sequence
       // R13 pDataOut for remaining unlock sequence
       // R14 NumBits for remaining unlock sequence
       //
      ,S32_MOVI32(1, 1000)                                                                                                                 // Initialize counter
      ,S32_MOVI32(2, (sizeof(Program.StackPtr) + sizeof(Program.aCode)))                                                                   // Move pDataIn
      ,S32_MOVI32(3, (sizeof(Program.StackPtr) + sizeof(Program.aCode) + sizeof(Program.aDataIn)))                                         // Move pDirection
      ,S32_MOVI32(4, (sizeof(Program.StackPtr) + sizeof(Program.aCode) + sizeof(Program.aDataIn) + sizeof(Program.aDataDir)))              // Move pDataOut
      ,S32_MOVI32(5, (39 * 8))                                                                                                             // Move NumBits (39 * 8)
      ,S32_MOVI32(6, 100000)                                                                                                               // Wait for erase time: 100ms
      ,S32_MOVI32(7, 10000)                                                                                                                // Reset hold time: 10ms
      ,S32_MOVI32(8, 8 * (sizeof(Program.StackPtr) + sizeof(Program.aCode) + sizeof(Program.aDataIn) + sizeof(Program.aDataDir) + 34) + 3) // Initialize bitpos of first AAP-IDCode byte
      ,S32_MOVI32(9, 0x16E60001)                                                                                                           // Load expected IDCode value into R9
      ,S32_MOVI32(11, (sizeof(Program.StackPtr) + sizeof(Program.aCode) + 39))                                                             // Move pDataIn
      ,S32_MOVI32(12, (sizeof(Program.StackPtr) + sizeof(Program.aCode) + sizeof(Program.aDataIn) + 39))                                   // Move pDirection
      ,S32_MOVI32(13, (sizeof(Program.StackPtr) + sizeof(Program.aCode) + sizeof(Program.aDataIn) + sizeof(Program.aDataDir)))             // Move pDataOut
      ,S32_MOVI32(14, (31 * 8))                                                                                                            // Move NumBits (31 * 8). Remaining 18 bytes of the unlock sequence
      ,S32_PUSH(7, 1)                                                                                                                      // PUSH R7. Used as Para0 by CALLH(10, 1)
       //
       // Perform reset
       //
      ,S32_CALLH(2, 0)                                                                                                                     // CALLH 1 which is _TIF_ClrRESET. No parameters are used
      ,S32_CALLH(10, 1)                                                                                                                    // CALLH(Off = 5) which is _Delay_us. 1 Parameter is used
      ,S32_CALLH(0, 0)                                                                                                                     // CALLH 0 which is _TIF_SetRESET. No parameters are used
       //
       // Send SWD switching sequence with DP-ID request as well as AAP-ID request
       // Repeat sequence until we get a valid AAP-ID
       // The AAP-ID is: 0x16E60001
       //
      ,S32_PUSH(11, 4)                                                                                                                     // R14 is pushed first
      ,S32_PUSH(2, 4)                                                                                                                      // PUSH R2-R5. R5 is pushed first
//Loop
      ,S32_CALLH(6, 0)                                                                                                                     // CALLH 3 which is _TIF_SWD_Transfer. 4 Parameters are used: pDataIn, pDirection, pDataOut, NumBits
      ,S32_LOADWBITPOS(0, 8)
      ,S32_SUB(0, 9)
      ,S32_BZ(3)
       //
       // Check if operation timed out
       //
      ,S32_DBNZ(1, -5)   // Decrement timeout counter
      ,S32_MOVI7(0, 1)                                                                                                                     // Return value: AAP not found
      ,S32_BRK                                                                                                                             // End of application
       //
       // Send the remaining bits of the unlock sequence
       //
      ,S32_ADDI7_NO_FLAG(15,16)
      ,S32_CALLH(6, 4)                                                                                                                     // CALLH 3 which is _TIF_SWD_Transfer. 4 Parameters are used: pDataIn, pDirection, pDataOut, NumBits
       //
       // Send another reset pulse after 100ms (the device should be erased now)
       // to make sure that unsecure has been recongized by the core
       //
      ,S32_PUSH(6, 2)                                                                                                                      // PUSH R6-R7
      ,S32_CALLH(10, 1)                                                                                                                    // CALLH(Off = 5) which is _Delay_us. 1 Parameter is used. Wait 100ms for erase to finish
      ,S32_CALLH(2, 0)                                                                                                                     // CALLH 1 which is _TIF_ClrRESET. No parameters are used
      ,S32_CALLH(10, 1)                                                                                                                    // CALLH(Off = 5) which is _Delay_us. 1 Parameter is used
      ,S32_CALLH(0, 0)                                                                                                                     // CALLH 0 which is _TIF_SetRESET. No parameters are used
       //
       // Program execution finished
       //
      ,S32_MOVI7(0, 0)                                                                                                                     // Return value
      ,S32_BRK                                                                                                                             // End of application
    },
    {
      //
      // Erase sequence
      //
      // First sequence. Output in a loop until we see the right AAP-ID
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0x00, 0x00,                               // Make sure SWD is ready for a start bit
      0xa5, 0x00, 0x00, 0x00, 0x00, 0x00,       // Read DP-ID - init sequence ends after this
      0xa9, 0x00, 0x00, 0x00, 0x00, 0x0a,       // ctrlstat = 0x50000000
      0xb1, 0x00, 0x1E, 0x00, 0x00, 0x00,       // Write DP2: 0x000000F0: Select AP0, Bank 15
      0x9F, 0x00, 0x00, 0x00, 0x00, 0x00,       // Read AP3
      0x9F, 0x00, 0x00, 0x00, 0x00, 0x00,       // Read AP3
      // Second sequence: Erase/Unlock
      0xb1, 0x00, 0x00, 0x00, 0x00, 0x00,       // Write DP2: 0x00000000: Select AP0, Bank 0
      0x8b, 0x00, 0x23, 0x98, 0xf5, 0x39,       // Write AP1: Unlock key
      0xa3, 0x20, 0x00, 0x00, 0x00, 0x20,       // Write AP0: AAP_CMD = 1;   // Set Deverase
      0x00,                                     // Make sure write is clocked through
      0xB7, 0x00, 0x00, 0x00, 0x00, 0x00,       // Read AP2
      0xB7, 0x00, 0x00, 0x00, 0x00, 0x00        // Read AP2
    },
    {
      //
      // Direction sequence
      //
      // First sequence. Output in a loop until we see the right AAP-ID
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff,                               // Make sure SWD is ready for a start bit
      0xff, 0x00 ,0x00 ,0x00, 0x00, 0xf0,       // Read DP-ID
      0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,       // Write DP1: CTRL/STAT
      0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,       // Write DP2: SELAP
      0xff, 0x00 ,0x00 ,0x00, 0x00, 0xf0,       // Read AP3
      0xff, 0x00 ,0x00 ,0x00, 0x00, 0xf0,       // Read AP3
      // Second sequence: Erase/Unlock
      0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,       // Write DP2
      0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,       // Write AP1
      0xff, 0xe0, 0xff, 0xff, 0xff, 0xff,       // Write AP0: AAP_CMD
      0xff,                                     // Make sure write is clocked through
      0xff, 0x00 ,0x00 ,0x00, 0x00, 0xf0,       // Read AP2
      0xff, 0x00 ,0x00 ,0x00, 0x00, 0xf0        // Read AP2
    },
    {0}
  };
  JLINKARM_EMU_PCODE_STATUS_INFO PCodeStat;
  int r;

  (void)s;
  _ReportOutf("Unlocking device...");
  //
  // Check if emulator has PCode execution capability
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_PCODE_EXEC) == 0) {
    _ReportOutf("Unsecure requires PCode execution feature of J-Link.\nThe connected emulator does not support PCode execution.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  r = JLINKARM_PCODE_Exec((U8*)&Program, sizeof(Program) - sizeof(Program.aStack), &PCodeStat);
  if (r != 0) {  // Error occurred before PCode execution
    if (r == JLINK_ERR_EMU_COMM_ERROR) {
      _ReportOutf("Emulator communication error.\n");
    }
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // PCode related error occurred on J-Link during download of PCode (no memory for PCode execution)
  //
  switch (PCodeStat.PCodeStat) {
  case 1:    // BRK - This is the normal way to end a PCODE
    break;
  case 2:
    _ReportOutf("Undefined instruction in PCode. PC = 0x%.8X.\n", PCodeStat.S32_PC - 2);
    return JLINK_ERROR_UNKNOWN;
  case 0xFFFFFFFF:
    _ReportOutf("Emulator has no memory for PCode execution.\n");
    return JLINK_ERROR_UNKNOWN;
  default:
    _ReportOutf("Unknown error (%d) occurred during download of PCode.\n", PCodeStat.PCodeStat);
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Return value of PCode itself. It depends on the PCode what this value means
  //
  if (PCodeStat.S32_R0) {
    switch (PCodeStat.S32_R0) {
    case 1:
      _ReportOutf("Could not find AAP. Device does not seem to be secured.\n");
      break;
    default:
      _ReportOutf("PCode reported unknown error: 0x%.8X", PCodeStat.S32_R0);
    }
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("O.K.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _SendISPCommand
*
*  Function description
*    Sends a command to the ISP-AP and waits until the request is processed.
*    Then it reads the result.
*
**  Parameters
*    pContext Currently not used.
*    Command  Command code to send.
*    pResult  Result
*
*  Return value
*    >= 0  O.K. (Number of repetitions needed before result returned valid data)
*    <  0  Error
*/
static U32 _SendISPCommand(U32 Command, U32* pResult) {
  U32 Data;
  int r;

  r = JLINKARM_CORESIGHT_WriteAPDPReg(1, 1, Command); // Send Command to REQUEST
  if (r < 0) {
    return -1;
  }
  do {
    r = JLINKARM_CORESIGHT_ReadAPDPReg(0, 1, &Data);  // Read CSW
    if (r < 0) {
      return -1;
    }
  } while ((Data & LPC5460_ISP_AP_CSW_REQ_PENDING) == LPC5460_ISP_AP_CSW_REQ_PENDING);  // Wait until request is read
  r = JLINKARM_CORESIGHT_ReadAPDPReg(2, 1, pResult);  // Read RETURN
  if (r < 0) {
    return -1;
  }
  return r;
}

/*********************************************************************
*
*       _UnlockLPC5460x
*
*  Function description
*    LPC5460x devices unlock
*    For information about the ISP-AP, please refer to
*    \\fileserver\Techinfo\Company\NXP\MCU\LPC54xx\UM10912_LPC546xx_UserManual_Rev2.2.pdf Chapter 49 
*/
static int _UnlockLPC5460x(const char * s) {
  U32 Stat;
  U32 Data;
  int r;

  (void)s;
  Stat = JLINK_ERROR_UNKNOWN;
  JLINKARM_TIF_Select(JLINKARM_TIF_SWD);
  _TIF_SWD_Select();
  _ReportOutf("Unlocking device...");
  _TIF_SWD_WriteU32(DP_REG_SELECT, (2 << 24) | (15 << 4)); // Select AP[2] bank 15 which is the ISP-AP
  //
  // Check ISP-AP ID
  //
  r = JLINKARM_CORESIGHT_ReadAPDPReg(3, 1, &Data);
  if (r < 0) {
    goto Fail;
  }
  if (Data == LPC5460_ISP_AP_ID_RESETVALUE) {
    //
    // Switch back to AP[2] bank 0
    //
    _TIF_SWD_WriteU32(DP_REG_SELECT, (2 << 24) | (0 << 4));
    if (r < 0) {
      goto Fail;
    }
    //
    // Resync and Reset
    //
    r = JLINKARM_CORESIGHT_WriteAPDPReg(0, 1, (LPC5460_ISP_AP_CSW_RESYNCH_REQ | LPC5460_ISP_AP_CSW_CHIP_RESET_REQ));
    if (r < 0) {
      goto Fail;
    }
    JLINKARM_CORESIGHT_ReadAPDPReg(0, 1, &Data);
    //
    // Enter command mode
    //
    r = _SendISPCommand(LPC5460_ISP_AP_CMD_ENTER_ISP_AP, &Data);
    if (r < 0) {
      goto Fail;
    }
    //
    // Perform bulk erase
    //
    r = _SendISPCommand(LPC5460_ISP_AP_CMD_BULK_ERASE, &Data);
    if (r < 0) {
      goto Fail;
    }
    if (Data != 0) {
      _ReportOutf("Mass erase failed. If mass erase is disabled, J-Link cannot unlock the device.\n");
      goto Fail;
    }
    //
    // Exit command mode
    //
    r = _SendISPCommand(LPC5460_ISP_AP_CMD_EXIT_ISP_AP, &Data);
    if (r < 0) {
      goto Fail;
    }
  }
  Stat = JLINK_NOERROR;
  _ReportOutf("O.K.\n");
  _ReportOutf("Note: Device needs to be power-cycled, if SWD access was disabled.\n");
  return Stat;
Fail:
  _ReportOutf("failed.\n");
  return Stat;
}

/*********************************************************************
*
*       _ExecUnlock
*
*  Function description
*    This function unlock a device which has
*    been accidentally locked by malfunction of user software.
*/
static int _ExecUnlock(const char * s) {
  char ac[64];
  int r;

  _ParseString(&s, ac, sizeof(ac));
  //
  // Luminary LM3S unlock
  //
  if (_JLinkstricmp(ac, "LM3Sxxx") == 0) {
    r = _UnlockLM3Sxxx(s);
  } else if (_JLinkstricmp(ac, "Kinetis") == 0) {
    r = _UnlockKinetis(s);
  } else if (_JLinkstricmp(ac, "EFM32Gxxx") == 0) {
    r = _UnlockEFM32Gxxx(s);
  } else if (_JLinkstricmp(ac, "LPC5460x") == 0) {
    r = _UnlockLPC5460x(s);
  } else {
    //
    // If device has not been found, print a list of all supported devices.
    //
    _ReportOutf("Syntax: unlock <DeviceName>\n");
    _ReportOutf("---Supported devices---\n"
           "  LM3Sxxx [<Auto>]\n"
           "  Kinetis\n"
           "  EFM32Gxxx\n"
           "  LPC5460x\n"
          );
    r = JLINK_NOERROR;
  }
  return r;
}

/*********************************************************************
*
*       _ExecTestRecover
*
*  Function description
*    Checks if the connected emulator recovers from error scenarios such as
*      - Timeout on receive (Command takes too long to execute)
*      - Wrong command
*/
static int _ExecTestRecover(const char * s) {
#ifdef _DEBUG
  int r;
  U8 aCmd[4];
  U8 aRead[4];
  static char acData[0x1000];
  const char * sErr;

  //Cmd = 0x9F;  // Unknown command
  aCmd[0] = 0x0C; // PCode exec
  aCmd[1] = 0; // SubCmd: GetCaps
  r = JLINKARM_Communicate(&aCmd[0], 2, &aRead[0], 4);
  do {
    JLINKARM_ReadMem(0, 0x1000 , &acData[0]);
  } while (1);
  JLINKARM_Close();
  sErr = JLINKARM_Open();
#endif
  (void)s;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecRunPCode
*
*  Function description
*    Loads a pcode assembler file, assembles it and performs the PCode.
*/
static int _ExecRunPCode(const char * s) {
  JLINKARM_EMU_PCODE_STATUS_INFO PCodeStat;
  char acASMFile[256];
  U8* pTmp;
  U8* pPCode;
  U32 PCodeSize;
  int v;
  int r;
  int Ret;
  HANDLE hFile;

  Ret = JLINK_ERROR_UNKNOWN;
  pTmp      = NULL;
  pPCode    = NULL;
  //
  // Check if emulator has PCode execution capability
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_PCODE_EXEC) == 0) {
    _ReportOutf("The connected emulator does not support PCode execution.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Parse parameters (path to ASM file)
  //
  _ParseString(&s, acASMFile, sizeof(acASMFile));
  if (acASMFile[0] == 0) {
    _ReportOutf("Syntax: runpcode <ASMFile>");
    Ret = JLINK_ERROR_SYNTAX;
    goto Done;
  }
  //
  // Read file contents
  //
  hFile = _OpenFile(acASMFile, FILE_FLAG_READ | FILE_FLAG_SHARE_READ);
  if (hFile == INVALID_HANDLE_VALUE) {
    _ReportOutf("Could not open PCode ASM file.\n");
    goto Done;
  }
  v = _GetFileSize(hFile);
  pTmp = malloc(v);
  _ReadFile(hFile, pTmp, v);
  _CloseFile(hFile);
  //
  // Assemble PCode
  //
  _ReportOutf("Assembling pcode [%s]...", acASMFile);
  r = JLINKARM_PCODE_Assemble(&pPCode, &PCodeSize, pTmp, _cbErrorOut);

  if (r < 0) {
    _ReportOutf("Error while assembling PCode.\n");
    goto Done;
  }
  _ReportOutf("O.K.\n");
  //
  // Output PCode information
  //
  _ReportOutf("Segment [0] size: %d bytes\n", *(U32*)(pPCode +  8));
  //
  // Execute PCode
  //
  r = JLINKARM_PCODE_Exec(pPCode + 16, PCodeSize - 16, &PCodeStat);
  //
  // Analyse PCode status
  switch (PCodeStat.PCodeStat) {
  case 1:  _ReportOutf("O.K. PC = 0x%.8X, R0 = 0x%.8X.\n", PCodeStat.S32_PC - 2, PCodeStat.S32_R0);              Ret = JLINK_NOERROR;       break;
  case 2:  _ReportOutf("Undefined instruction at addr 0x%.8X.\n", PCodeStat.S32_PC - 2);                         Ret = JLINK_ERROR_UNKNOWN; break;
  default: _ReportOutf("Unknown error (error %d) at addr 0x%.8X.\n", PCodeStat.S32_PC - 2, PCodeStat.PCodeStat); Ret = JLINK_ERROR_UNKNOWN; break;
  }
Done:
  if (pTmp) {
    free(pTmp);
  }
  if (pPCode) {
    JLINKARM_FreeMem(pPCode);
  }
  return Ret;
}

/*********************************************************************
*
*       _ExecTestPCode
*
*  Function description
*    Checks PCode performance.
*/
static int _ExecTestPCode(const char * s) {
  typedef struct {
    U32 StackPtr;
    U16 aCode[100];
  } PCODE_TEST_PROG;

  PCODE_TEST_PROG ProgramDBNZ = {
    sizeof(PCODE_TEST_PROG) + 32,  // Reserve 32 bytes stack
    {
      S32_MOVI32(0, 2000000),
      S32_DBNZ(0, -1),
      S32_BRK
    }
  };

  PCODE_TEST_PROG ProgramDEC = {
    sizeof(PCODE_TEST_PROG) + 32,  // Reserve 32 bytes stack
    {
      S32_MOVI32(0, 1000000),
      S32_SUBI4(0, 1),
      S32_BNZ((U8)-2),
      S32_BRK
    }
  };

  PCODE_TEST_PROG ProgramCALLH = {
    sizeof(PCODE_TEST_PROG) + 32,  // Reserve 32 bytes stack
    {
      S32_MOVI32(1, 500000),
      S32_CALLH(18, 0),                                                                         // CALLH(Off = 9) which is _GetTimeMS
      S32_DBNZ(1, -2),
      S32_BRK
    }
  };
  PCODE_TEST_PROG ProgramDelayus = {
    sizeof(PCODE_TEST_PROG),  // Stack area is part of PCode to be downloaded, so at startup, stackpointer should point to the first location after the PCode.
    {
      S32_MOVI32(1, 1000000),  // Counter for HW_Delay_us: Wait 1 second
      S32_PUSH(1, 1),
      //
      // Wait until a systick happens
      //
      S32_CALLH(18, 0),       // _GetTimeMS
      S32_MOV(2, 0, 0),       // Remember systick reference value
      S32_CALLH(18, 0),       // _GetTimeMS
      S32_SUB(0, 2),
      S32_BZ((U8)-3),         // Wait until a systick occurred
      //
      // Wait 1000 ms using HW_Delay_us()
      // to check if routine is implemented correctly
      //
      S32_CALLH(10, 1),       // HW_Delay_us(int Delayus), pop Delayus automatically
      S32_CALLH(18, 0),       // _GetTimeMS => OS_Time is in R0
      S32_ADDI4(2, 1),        // Increment systick reference value since at startup we waited until 1 systick occurred
      S32_SUB(0, 2),
      S32_BRK
    }
  };

  JLINKARM_EMU_PCODE_STATUS_INFO PCodeStat;
  int r;
  int t;
  U32 Caps;
  U32 Version;

  (void)s;
  //
  // Check if emulator has PCode execution capability
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_PCODE_EXEC) == 0) {
    _ReportOutf("The connected emulator does not support PCode execution.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  r = JLINKARM_PCODE_GetCaps(&Caps);
  if (r < 0) {
    return JLINK_ERROR_UNKNOWN;
  }
  r = JLINKARM_PCODE_GetS32Version(&Version);
  if (r < 0) {
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("  Emulator supports PCODE. S32 Version: %d.%d\n", Version / 100, Version % 100);
  if (Caps) {
    if (Caps & (1 << 1)) { _ReportOutf("    GetAPIList\n"); }
    if (Caps & (1 << 2)) { _ReportOutf("    Download\n");   }
    if (Caps & (1 << 3)) { _ReportOutf("    Exec\n");       }
    if (Caps & (1 << 4)) { _ReportOutf("    Step\n");       }
    if (Caps & (1 << 5)) { _ReportOutf("    GetRegs\n");    }
    if (Caps & (1 << 6)) { _ReportOutf("    SetRegs\n");    }
  }
  _ReportOutf("  Checking PCode performance:\n");
  //
  // Check DBNZ Performance
  //
  t = SYS_GetTickCount();
  r = JLINKARM_PCODE_Exec((U8*)&ProgramDBNZ, sizeof(ProgramDBNZ), &PCodeStat);
  t = SYS_GetTickCount() - t;
  if (t) {
    t = 2000000 / t;
    _ReportOutf("    DBNZ : %dk Loops/Sec.\n", t);
  }
  //
  // Check DEC Performance
  //
  t = SYS_GetTickCount();
  r = JLINKARM_PCODE_Exec((U8*)&ProgramDEC, sizeof(ProgramDEC), &PCodeStat);
  t = SYS_GetTickCount() - t;
  if (t) {
    t = 1000000 / t;
    _ReportOutf("    SUB  : %dk Loops/Sec.\n", t);
  }
  //
  // Check CALLH Performance
  //
  t = SYS_GetTickCount();
  r = JLINKARM_PCODE_Exec((U8*)&ProgramCALLH, sizeof(ProgramCALLH), &PCodeStat);
  t = SYS_GetTickCount() - t;
  if (t) {
    t = 500000 / t;
    _ReportOutf("    CALLH: %dk Loops/Sec.\n", t);
  }
  //
  // Check functions implementation
  //
  _ReportOutf("  Checking implementation of J-Link functions:\n");
  //
  // Check implementation of HW_Delay_us()
  //
  _ReportOutf("    HW_Delay_us(1000000 us)...");
  if ((Version / 100) >= 2) {
    r = JLINKARM_PCODE_Exec((U8*)&ProgramDelayus, sizeof(ProgramDelayus), &PCodeStat);
    _ReportOutf("O.K.: %d Systicks (ms) elapsed.\n", PCodeStat.S32_R0);
  } else {
    _ReportOutf("ERROR: Min. PCode V2.0 required. Current version: V%d.%d\n", Version / 100, Version % 100);
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReset
*/
static int _ExecReset(const char* s) {
  (void)s;
  _Reset(0);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecResetEx
*/
static int _ExecResetEx(const char* s) {
  U32 Delay;
  if (_ParseDec(&s, &Delay)) {
    _ReportOutf("Syntax: rx <DelayAfterReset>\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_SetInitRegsOnReset(0);
  _Reset(Delay);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecResetNoHalt
*/
static int _ExecResetNoHalt(const char* s) {
  (void)s;
  JLINKARM_ResetNoHalt();
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSelDevice
*/
static int _ExecSelDevice(const char* s) {
  U32 DevIndex;
  if (_ParseDec(&s, &DevIndex)) {
    _ReportOutf("Syntax: sel <DevIndex>\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_SelDevice((U16)DevIndex);
  _ReportOutf("Selected device with JTAG index %d\n", DevIndex);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetSpeed
*/
static int _ExecSetSpeed(const char* s) {
  U32 Speed;
  int r;
  _SETTINGS* pCommanderSettings;
  JLINKARM_SPEED_INFO SpeedInfo;

  pCommanderSettings = &_CommanderSettings;
  r = JLINK_NOERROR;
  //
  // Parse speed (given in kHz)
  //
  Speed = 0;
  _EatWhite(&s);
  if ((_CompareCmd(&s, "adaptive") == 0) || (_CompareCmd(&s, "a") == 0)) {
    Speed = JLINKARM_SPEED_ADAPTIVE;
  } else if (_CompareCmd(&s, "auto") == 0) {
    Speed = JLINKARM_SPEED_AUTO;
  } else {
    if (_ParseDec(&s, &Speed)) {
      _ReportOutf("Syntax: speed <freq>|auto|adaptive\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Output speed selection
  //
  if (Speed == JLINKARM_SPEED_ADAPTIVE) {
    _ReportOut("Selecting adaptive clocking as target interface speed\n");
  } else if (Speed == JLINKARM_SPEED_AUTO) {
    _ReportOut("Selecting auto as target interface speed\n");
  } else {
    _ReportOutf("Selecting %d kHz as target interface speed\n", Speed);
  }
  //
  // If target connection already established: Pass new speed settings to DLL
  //
  if (pCommanderSettings->Status.ConnectedToTarget) {
    //
    // For adaptive, check if J-Link supports this
    //
    if (Speed == JLINKARM_SPEED_ADAPTIVE) {
      SpeedInfo.SizeOfStruct = sizeof(SpeedInfo);
      JLINKARM_GetSpeedInfo(&SpeedInfo);
      if (SpeedInfo.SupportAdaptive == 0) {
        _ReportOutf("The connected J-Link does not support adaptive clocking.\n");
        return JLINK_ERROR_UNKNOWN;
      }
    }
    JLINKARM_SetSpeed(Speed);
  }
  //
  // Update commander settings
  //
  pCommanderSettings->InitTIFSpeedkHz    = Speed;
  pCommanderSettings->InitTIFSpeedkHzSet = 1;
  return r;
}

/*********************************************************************
*
*       _ExecSetPC
*/
static int _ExecSetPC(const char* s) {
  U64 Addr;
  U32 DevFamily;
  U32 RegIndex;
  int i;
  unsigned Found;

  if (_ParseHex64(&s, &Addr)) {
    _ReportOutf("Syntax: SetPC <addr>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (JLINKARM_IsHalted()) {
    Found = 0;
    DevFamily = JLINKARM_GetDeviceFamily();
    for (i = 0; i < COUNTOF(_aTblDevInfo); i++) {
      if (_aTblDevInfo[i].DevFamily == DevFamily) {
        RegIndex = _aTblDevInfo[i].iRegPC;
        JLINK_WriteRegs_64(&RegIndex, &Addr, NULL, 1);
        Found = 1;
        break;
      }
    }
    if (Found == 0) {
      _ReportOutf("Command is not supported for this CPU\n");
    }
  } else {
    _ReportOutf("CPU is not halted !\n");
    return JLINK_ERROR_WARNING;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecClrBP
*/
static int _ExecClrBP(const char* s) {
  I32 BPHandle;
  if (_ParseDec(&s, (U32*)&BPHandle)) {
    _ReportOutf("Syntax: ClrBP <BP_Handle>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (JLINKARM_ClrBPEx(BPHandle)) {
    _ReportOutf("Could not clear breakpoint.\n");
  } else {
    _ReportOutf("Breakpoint cleared\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecMeasureSCLen
*/
static int _ExecMeasureSCLen(const char* s) {
  U32 SCLen, ScanChain;
  if (_ParseDec(&s, &ScanChain)) {
    _ReportOutf("Syntax: ms <scan chain>\n");
    return JLINK_ERROR_SYNTAX;
  }
  SCLen = JLINKARM_MeasureSCLen(ScanChain);
  _ReportOutf("Length of scan chain [%d] = %d\n", ScanChain, SCLen);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecMeasureRTCKReactTime
*/
static int _ExecMeasureRTCKReactTime(const char* s) {
  U32 RepCount = 1;
  int r;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &RepCount)) {
      return JLINK_ERROR_SYNTAX;
    }
  }
  while (RepCount--) {
    JLINKARM_RTCK_REACT_INFO Info;
    Info.SizeOfStruct = sizeof(Info);
    r = JLINKARM_MeasureRTCKReactTime(&Info);
    if (r == 0) {
      _ReportOutf("Min: %dns, Max: %dns, Average: %dns\n", Info.Min, Info.Max, Info.Average);
    } else if (r == -1) {
      _ReportOutf("RTCK did not react\n");
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadCP15
*/
static int _ExecReadCP15(const char* s) {
  U32 RegIndex, Data;
  int r;
  if (_ParseHex(&s, &RegIndex)) {
    _ReportOutf("Syntax: rcp15 <RegIndex>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_CP15_ReadReg(RegIndex, &Data);
  if (r == 0) {
    _ReportOutf("CP15[%.2X] = 0x%.8X\n", RegIndex, Data);
  } else {
    _ReportOutf("Read failed\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadCP15Ex
*/
static int _ExecReadCP15Ex(const char* s) {
  U32 CRn, CRm, Op1, Op2;
  U32 Data;
  int r;
  if (_ParseDec(&s, &Op1)) {
    _ReportOutf("Syntax: rcp15ex <Op1>, <CRn>, <CRm>, <Op2>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &CRn)) {
    _ReportOutf("Syntax: rcp15ex <Op1>, <CRn>, <CRm>, <Op2>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &CRm)) {
    _ReportOutf("Syntax: rcp15ex <Op1>, <CRn>, <CRm>, <Op2>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &Op2)) {
    _ReportOutf("Syntax: rcp15ex <Op1>, <CRn>, <CRm>, <Op2>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_CP15_ReadEx((U8)CRn, (U8)CRm, (U8)Op1, (U8)Op2, &Data);
  if (r == 0) {
    _ReportOutf("CP15[%d, %d, %d, %d] = 0x%.8X\n", Op1, CRn, CRm, Op2, Data);
  } else {
    _ReportOutf("Read failed\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteCP15
*/
static int _ExecWriteCP15(const char* s) {
  U32 RegIndex, Data;
  if (_ParseHex(&s, &RegIndex)) {
    _ReportOutf("Syntax: wcp15 <RegIndex>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  s++;
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: wcp15 <RegIndex>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ReportOutf("CP15[%.2X] = 0x%.8X\n", RegIndex, Data);
  JLINKARM_CP15_WriteReg(RegIndex, Data);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteCP15Ex
*/
static int _ExecWriteCP15Ex(const char* s) {
  U32 CRn, CRm, Op1, Op2;
  U32 Data;

  if (_ParseDec(&s, &Op1)) {
    _ReportOutf("Syntax: wcp15ex <Op1>, <CRn>, <CRm>, <Op2>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &CRn)) {
    _ReportOutf("Syntax: wcp15ex <Op1>, <CRn>, <CRm>, <Op2>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &CRm)) {
    _ReportOutf("Syntax: wcp15ex <Op1>, <CRn>, <CRm>, <Op2>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseDec(&s, &Op2)) {
    _ReportOutf("Syntax: wcp15ex <Op1>, <CRn>, <CRm>, <Op2>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: wcp15ex <Op1>, <CRn>, <CRm>, <Op2>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_CP15_WriteEx((U8)CRn, (U8)CRm, (U8)Op1, (U8)Op2, Data);
  _ReportOutf("CP15[%d, %d, %d, %d] = 0x%.8X\n", Op1, CRn, CRm, Op2, Data);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadCSR
*
*  Function description
*    Reads a CSR on a RISC-V based target device
*    Syntax:
*      readcsr <RegIndex>
*    Value will be printed in hex
*/
static int _ExecReadCSR(const char* s) {
  JLINK_FUNC_READ_CSR* pf;
  int r;
  U32 RegIndex;
  U32 RegSize;
  U64 v64;
  //
  // Parse parameter <RegIndex>
  //
  RegSize = 0;
  r = UTIL_ParseIntEx(&s, &RegIndex);
  if (r < 0) {
    _ReportOutf("ReadCSR: Error while parsing <RegIndex>. Syntax: ReadCSR <RegIndex>[,<RegSize>]\n");
    r = JLINK_ERROR_SYNTAX;
    goto Done;
  }
  //
  // Parse parameter <RegSize>
  //
  UTIL_EatWhite(&s);
  UTIL_ParseChar(&s, ',');           // ',' is optional
  if (*s) {
    r = UTIL_ParseIntEx(&s, &RegSize);
    if (r < 0) {
      _ReportOutf("ReadCSR: Error while parsing <RegSize>. Syntax: ReadCSR <RegIndex>[,<RegSize>]\n");
      r = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  //
  // Execute access
  //
  r = JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED;
  pf = (JLINK_FUNC_READ_CSR*)JLINK_GetpFunc(JLINK_IFUNC_READ_CSR);
  if (pf) {
    r = pf(RegIndex, &v64, RegSize);
    if (r < 0) {
      _ReportOutf("ReadCSR: Failed to write CSR. Error code: %d\n", r);
    } else {
      if (v64 & 0xFFFFFFFF00000000) {
        _ReportOutf("CSR 0x%.4X: 0x%.8X%.8X\n", RegIndex, (U32)(v64 >> 32), (U32)v64);
      } else {
        _ReportOutf("CSR 0x%.4X: 0x%.8X\n", RegIndex, (U32)v64);
      }
      r = 0;
    }
  } else {
    _ReportOutf("ReadCSR is not implemented for this version of the J-Link software\n");
  }
Done:
  return r;
}

/*********************************************************************
*
*       _ExecWriteCSR
*
*  Function description
*    Writes a CSR on a RISC-V based target device
*    Syntax:
*      writecsr <RegIndex>,<Val>[,<RegSize>]
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _ExecWriteCSR(const char* s) {
  JLINK_FUNC_WRITE_CSR* pf;
  int r;
  U32 RegIndex;
  U32 RegVal;
  U32 RegSize;
  //
  // Parse parameter <RegIndex>
  //
  RegSize = 0;
  r = UTIL_ParseIntEx(&s, &RegIndex);
  if (r < 0) {
    _ReportOutf("WriteCSR: Error while parsing <RegIndex>. Syntax: WriteCSR <RegIndex>,<Value>[,<RegSize>]\n");
    r = JLINK_ERROR_SYNTAX;
    goto Done;
  }
  //
  // Parse parameter <RegVal>
  //
  UTIL_EatWhite(&s);
  UTIL_ParseChar(&s, ',');           // ',' is optional
  r = UTIL_ParseIntEx(&s, &RegVal);
  if (r < 0) {
    _ReportOutf("WriteCSR: Error while parsing <Value>. Syntax: WriteCSR <RegIndex>,<Value>[,<RegSize>]\n");
    r = JLINK_ERROR_SYNTAX;
    goto Done;
  }
  //
  // Parse parameter <RegSize>
  //
  UTIL_EatWhite(&s);
  UTIL_ParseChar(&s, ',');           // ',' is optional
  if (*s) {
    r = UTIL_ParseIntEx(&s, &RegSize);
    if (r < 0) {
      _ReportOutf("WriteCSR: Error while parsing <RegSize>. Syntax: WriteCSR <RegIndex>,<Value>[,<RegSize>]\n");
      r = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  //
  // Execute access
  //
  r = JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED;
  pf = (JLINK_FUNC_WRITE_CSR*)JLINK_GetpFunc(JLINK_IFUNC_WRITE_CSR);
  if (pf) {
    r = pf(RegIndex, RegVal, RegSize);
    if (r < 0) {
      _ReportOutf("WriteCSR: Failed to write CSR. Error code: %d\n", r);
    } else {
      r = 0;
    }
  } else {
    _ReportOutf("WriteCSR is not implemented for this version of the J-Link software\n");
  }
Done:
  return r;
}

/*********************************************************************
*
*       _ExecExecCommand
*/
static int _ExecExecCommand(const char* s) {
  char ac[4000];
  char c;
  int r;
  unsigned iExec;
  unsigned NeedConnect;
  unsigned NumCharsCommandString;
  _SETTINGS* pCommanderSettings;
  //
  // For some command strings, we must not even connect to J-Link
  // As this is exec specific, we determine it in this function and the generic layer assumes that no J-Link connection is necessary
  // On this level, in case we do not find any specifics for the given command, we assume that a J-Link connection is necessary
  //
  pCommanderSettings = &_CommanderSettings;
  ac[0] = 0;
  NeedConnect = 1;
  _EatWhite(&s);
  //
  // Make sure we extract the command string and filter out parameters and so on (new lines or spaces at the end etc.)
  // command strings itself only contain a-z and A-Z
  //
  NumCharsCommandString = 0;
  do {
    if (NumCharsCommandString == ((unsigned)sizeof(ac) - 1)) {
      break;
    }
    c = *(s + NumCharsCommandString);
    if ((c < 'a') || (c > 'z')) {
      if ((c < 'A') || (c > 'Z')) {
        break;
      }
    }
    ac[NumCharsCommandString] = c;
    NumCharsCommandString++;
  } while (1);
  ac[NumCharsCommandString] = 0;  // Zero terminate command string
  for (iExec = 0; iExec < COUNTOF(_aExec); iExec++) {
    r = _JLinkstricmp(_aExec[iExec].sCmdStr, ac);
    if (r == 0) {
      NeedConnect = _aExec[iExec].NeedsJLinkConn;
      break;
    }
  }
  if (NeedConnect) {
    if (pCommanderSettings->Status.ConnectedToJLink == 0) {
      _ReportOut("J-Link connection not established yet but required for command.\n");
      r = _ConnectToJLink(&_CommanderSettings);
      if (r < 0) {
        return JLINK_ERROR_UNKNOWN;
      }
    }
  }
  JLINKARM_ExecCommand(s, &ac[0], sizeof(ac));
  _ReportOutf("%s", ac);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecDevice
*/
static int _ExecDevice(const char* s) {
  int r;
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;
  _EatWhite(&s);
  if (*s == '=') {
    s++;
    _EatWhite(&s);
  }
  //
  // No device given -> Show help
  //
  if (*s == 0) {
    _ReportOutf("Syntax: device <DeviceName>\n"
           "        Type \"device ?\" to show the device selection dialog.\n"
          );
    return JLINK_NOERROR;
  }
  //
  // Update commander settings
  //
  _UTIL_CopyString(pCommanderSettings->acDeviceName, s, _SIZEOF(pCommanderSettings->acDeviceName));
  //
  // In case we were already connected to a device, disconnect from the device and reconnect
  //
  if (pCommanderSettings->Status.ConnectedToTarget) {
    _DisconnectFromTarget(pCommanderSettings);
    r = _ConnectToTarget(pCommanderSettings);
    if (r == 0) {
      _ShowMemZones();
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecPower
*/
static int _ExecPower(const char* s) {
  char ac[4000];
  int r;

  _EatWhite(&s);
  if(_CompareCmd(&s, "On") == 0) {
    r = JLINKARM_ExecCommand("SupplyPower = 1", &ac[0], sizeof(ac));
    if (r == 1) {
      _ReportOutf("The connected debug probe does not support this command.\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if(_CompareCmd(&s, "perm") == 0) {
      JLINKARM_ExecCommand("SupplyPowerDefault = 1", &ac[0], sizeof(ac));
    }
  } else if(_CompareCmd(&s, "Off") == 0) {
    JLINKARM_ExecCommand("SupplyPower = 0", &ac[0], sizeof(ac));
    _EatWhite(&s);
    if(_CompareCmd(&s, "perm") == 0) {
      JLINKARM_ExecCommand("SupplyPowerDefault = 0", &ac[0], sizeof(ac));
    }
  } else {
    _ReportOutf("Syntax: power <State> [perm]\n");
    return JLINK_ERROR_SYNTAX;
  }

  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTestHaltGo
*/
static int _ExecTestHaltGo(const char* s) {
  int i;
  int TickCnt = SYS_GetTickCount();
  
  (void)s;
  _ReportOutf("test halt/go \n");
  if (JLINKARM_IsHalted()) {
    JLINKARM_Go();
  }
  for (i = 0; i < 1000; i++) {
    if (JLINKARM_Halt()) {
      break;
    }
    _ReportOutf(".");
    JLINKARM_Go();
  }
  TickCnt = SYS_GetTickCount() - TickCnt;
  _ReportOutf("\nCompleted. %dms required\n", TickCnt );
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTestSingleStep
*/
static int _ExecTestSingleStep(const char* s) {
  int i;
  int NumSteps;
  int TickCnt = SYS_GetTickCount();

  NumSteps = 1000;                   // If user did not speciy the number of steps, we perform 1000
  _ReportOutf("Test single step");
  _EatWhite(&s);
  if (*s == ',') {
    s++;
    _EatWhite(&s);
  }
  if (*s) {
    if (_ParseDec(&s, &NumSteps)) {
      _ReportOutf("Syntax: TestStep [<NumSteps>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  JLINKARM_Halt();
  for (i = 0; i < NumSteps; i++) {
    JLINKARM_Step();
    _ReportOutf(".");
  }
  TickCnt = SYS_GetTickCount() - TickCnt;
  _ReportOutf("\nCompleted. %dms required\n", TickCnt );
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetRAMAddr
*/
static int _ExecSetRAMAddr(const char* s) {
  char acIn [128];
  char acOut[128];
  int r = JLINK_NOERROR;
  if (_ParseHex(&s, &_RAMAddr)) {
    _ReportOutf("Syntax: ram <RAMAddr>(hex)\n");
    _RAMAddr = DEFAULT_RAMADDR;
    r = JLINK_ERROR_SYNTAX;
  }
  UTIL_snprintf(acIn, UTIL_SIZEOF(acIn), "map ram 0x%.8X - 0x%.8X", _RAMAddr, _RAMAddr + 0x100 - 1);
  JLINKARM_ExecCommand(acIn, acOut, sizeof(acOut));
  return r;
}

/*********************************************************************
*
*       _ExecTestRAM
*/
static int _ExecTestRAM(const char* s) {
  U32 Addr;
  U32 ExitOnError;
  U32 Pattern;
  U8* pDataIn;
  U8* pDataOut;
  int c;
  int i;
  int n;
  I32 NumBytes;
  I32 NumCycles;
  int r;
  int Percent;

  NumCycles   = 2;
  ExitOnError = 0;
  //
  // Parse options
  //
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: TestRAM <StartAddr (hex)>, <NumBytes (hex)>, [<NumCycles>], [<ExitOnError>]\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, (U32*)&NumBytes)) {
    _ReportOutf("Syntax: TestRAM <StartAddr (hex)>, <NumBytes (hex)>, [<NumCycles>], [<ExitOnError>]\n");
    return JLINK_ERROR_SYNTAX;
  }
  n = (NumBytes + NumBytes%4)/4;
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  //
  // Parse NumCycles if necessary
  //
  if (*s) {
    if (_ParseDec(&s, (U32*)&NumCycles)) {
      _ReportOutf("Syntax: TestRAM <StartAddr (hex)>, <NumBytes (hex)>, [<NumCycles>], [<ExitOnError>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  //
  // Parse ExitOnError if necessary
  //
  if (*s) {
    if (_ParseDec(&s, &ExitOnError)) {
      _ReportOutf("Syntax: TestRAM <StartAddr (hex)>, <NumBytes (hex)>, [<NumCycles>], [<ExitOnError>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Halt target for RAM test
  //
  _ReportOutf("Halting target for RAM test...");
  r = JLINKARM_Halt();
  if (r) {
    return JLINK_ERROR_SYNTAX;
  }
  _ReportOutf("O.K.\n");
  //
  // Allocate buffers for RAM test
  //
  pDataIn  = (U8*)malloc(n * 4);                         // Make sure Buffer size is a multiple of 4, because CRC returns 4 bytes
  pDataOut = (U8*)malloc(n * 4);
  Pattern  = 0xAAAAAAAA;                                 // Pattern for CRC generation we start with
  //
  // Start RAM check
  //
  _ReportOutf("Starting RAM check @0x%.8X, 0x%.8X bytes, %d cycles.\n", Addr, NumBytes, NumCycles);
  _ReportOutf("Checking RAM...[000%%]");
  c = 1;
  do {
    Percent = (c * 100) / NumCycles;
    _ReportOutf("\b\b\b\b\b\b");
    _ReportOutf("[%3d%%]", Percent);
    if(c & 1) {
      //
      // Fill buffer with test pattern
      //
      i = 0;
      do {
        Pattern = _CRC_Calc32Fast((U8*)&Pattern, 4, 0);
        memcpy(pDataIn + (i * 4), &Pattern, 4);
        i++;
      } while(i < n);
    } else {
      //
      // Invert test pattern
      //
      i = 0;
      do {
        *(pDataIn + i) = ~(*(pDataIn + i));
        i++;
      } while(i < n);
    }
    //
    // Write & verify test pattern
    //
    r = JLINKARM_WriteMemEx(Addr, NumBytes, pDataIn, 0);
    if (r != NumBytes) {
      _ReportOutf("RAM check Failed.\n");
      r = JLINK_ERROR_UNKNOWN;
      goto Cleanup;
    }
    r = JLINKARM_ReadMemEx(Addr, NumBytes, pDataOut, 0);
    if((r != NumBytes) || memcmp(pDataIn, pDataOut, NumBytes)) {
      _ReportOutf("RAM check Failed.\n");
      r = JLINK_ERROR_UNKNOWN;
      goto Cleanup;
    }
    c++;
  } while(c <= NumCycles);
  _ReportOutf("\b\b\b\b\b\bdone. \n");
  r = JLINK_NOERROR;
Cleanup:
  free(pDataIn);
  free(pDataOut);
  return r;
}


/*********************************************************************
*
*       _ExecWriteICEReg
*/
static int _ExecWriteICEReg(const char* s) {
  U32 Addr, Data;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: wi <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  s++;
  if (_ParseHex(&s, &Data)) {
    _ReportOutf("Syntax: wi <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ReportOutf("Icereg[%.8X] = %.8X\n", Addr, Data);
  JLINKARM_WriteICEReg(Addr, Data, 0);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadICEReg
*/
static int _ExecReadICEReg(const char* s) {
  U32 Addr, Data;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: wi <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  Data = JLINKARM_ReadICEReg(Addr);
  _ReportOutf("Icereg[%.8X] = %.8X\n", Addr, Data);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteJTAGCommand
*/
static int _ExecWriteJTAGCommand(const char* s) {
  U32 Cmd;
  if (_ParseHex(&s, &Cmd)) {
    _ReportOutf("Syntax: WJC <Cmd(hex)>\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_JTAG_StoreInst((U8*)&Cmd, 4);
  JLINKARM_JTAG_SyncBits();
  _ReportOutf("Command 0x%X (%s) successfully written\n", Cmd, _Cmd2String((U8)Cmd));
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteJTAGIR
*/
static int _ExecWriteJTAGIR(const char* s) {
  U32 Cmd;
  U32 IRLen;
  
  IRLen = 4;                 // Default is IRLen == 4;
  if (_ParseHex(&s, &Cmd)) {
    _ReportOutf("Syntax: " SYNTAX_WJTAGIR "\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if(*s != 0) {
    if (_ParseDec(&s, (U32*)&IRLen)) {
      _ReportOutf("Syntax: " SYNTAX_WJTAGIR "\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  JLINKARM_JTAG_StoreInst((U8*)&Cmd, IRLen);
  JLINKARM_JTAG_SyncBits();
  _ReportOutf("Command 0x%X (%s) successfully written\n", Cmd, _Cmd2String((U8)Cmd));
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteJTAGDR
*/
static int _ExecWriteJTAGDR(const char* s) {
  U64 Data;
  I32 NumBits;
  int BitPos;
  U8  ac[8];
  if (_ParseHex64(&s, &Data)) {
    _ReportOutf("Syntax: " SYNTAX_WJTAGDR "\n");
    return JLINK_ERROR_SYNTAX;
  }
  s++;
  if (_ParseDec(&s, (U32*)&NumBits)) {
    _ReportOutf("Syntax: " SYNTAX_WJTAGDR "\n");
    return JLINK_ERROR_SYNTAX;
  }
  ac[0] = (U8) (Data & 255);
  ac[1] = (U8)((Data >> 8) & 255);
  ac[2] = (U8)((Data >> 16) & 255);
  ac[3] = (U8)((Data >> 24) & 255);
  ac[4] = (U8)((Data >> 32) & 255);
  ac[5] = (U8)((Data >> 40) & 255);
  ac[6] = (U8)((Data >> 48) & 255);
  ac[7] = (U8)((Data >> 56) & 255);
  BitPos = JLINKARM_JTAG_StoreData(ac, NumBits);
  JLINKARM_JTAG_SyncBits();

  _Var = JLINKARM_GetU32(BitPos);
  if (NumBits > 32) {
    _Var |= (U64)JLINKARM_GetU32(BitPos + 32) << 32;
  }
  if        (NumBits < 32) {
    _Var &= (1U << NumBits) - 1;
  } else if (NumBits < 64) {
    U64 Mask;
    Mask   = ~(0);
    Mask >>= (64 - NumBits);
    _Var  &= Mask;
  }
  _ReportOutf("returns 0x%llX\n", _Var);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadReg
*/
static int _ExecReadReg(const char* s) {
  const char * sReg;
  char ac[32];
  U32 RegIndex;
  const U32* paAlternateRegIndex;
  U64 Data;
  U32 DevFamily;
  int NumRegs;
  int i;

  if (JLINKARM_IsHalted() == 0) {
    _ReportOutf("Halting CPU in order to read register...\n");
    JLINKARM_Halt();
  }
  //
  // In older versions, ReadReg expected a index as parameter which was passed to JLINKARM_ReadReg()
  // We decided to change so that the function expects a string in order to make it "compatible" to WriteReg.
  // In order to stay backward compatible, we still support the old method (RegIndex as integer).
  //
  if ((_ParseDec(&s, &RegIndex)) == 0) {
    JLINK_ReadRegs_64(&RegIndex, &Data, NULL, 1);
    _ReportOutf("%s = 0x%.8X\n", JLINKARM_GetRegisterName(RegIndex), (U32)(Data >> 0));
  } else {
    _ParseString(&s, ac, sizeof(ac));
    DevFamily = JLINKARM_GetDeviceFamily();
    paAlternateRegIndex = _GetNumRegs(DevFamily, &NumRegs);
    for (i = 0; i < NumRegs; i++) {
      if (paAlternateRegIndex) {
        RegIndex = *(paAlternateRegIndex + i);
      } else {
        RegIndex = i;
      }
      sReg = JLINKARM_GetRegisterName(RegIndex);
      if (_JLinkstricmp(ac, sReg) == 0) {
        goto FoundReg;
      }
    }
    _ReportOutf("Illegal register name.\nSyntax: rreg <RegName>\n\n");
    for (i = 0; i < NumRegs; i++) {
      if (paAlternateRegIndex) {
        RegIndex = *(paAlternateRegIndex + i);
      } else {
        RegIndex = i;
      }
      sReg = JLINKARM_GetRegisterName(RegIndex);
      _ReportOutf("%s\n", sReg);
    }
    return JLINK_ERROR_SYNTAX;
  FoundReg:
    _EatWhite(&s);
    if ((*s == ',') || (*s == '=')) {
      s++;
    }
    JLINK_ReadRegs_64(&RegIndex, &Data, NULL, 1);
    if ((DevFamily == JLINK_DEV_FAMILY_CORTEX_AR_ARMV8) && (_CortexARMV8AR_IsAArch32() == 0)) {
      _ReportOutf("%s = 0x%.8X%.8X\n", JLINKARM_GetRegisterName(RegIndex), (U32)(Data >> 32), (U32)(Data >> 0));
    } else {
      _ReportOutf("%s = 0x%.8X\n", JLINKARM_GetRegisterName(RegIndex), (U32)(Data >> 0));
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteReg
*/
static int _ExecWriteReg(const char* s) {
  const char * sReg;
  char ac[32];
  U32 RegIndex;
  const U32* paAlternateRegIndex;
  U64 Data;
  U32 DevFamily;
  int NumRegs;
  int i;

  if (JLINKARM_IsHalted() == 0) {
    _ReportOutf("Halting CPU in order to write register...\n");
    JLINKARM_Halt();
  }
  _ParseString(&s, ac, sizeof(ac));
  DevFamily = JLINKARM_GetDeviceFamily();
  paAlternateRegIndex = _GetNumRegs(DevFamily, &NumRegs);
  for (i = 0; i < NumRegs; i++) {
    if (paAlternateRegIndex) {
      RegIndex = *(paAlternateRegIndex + i);
    } else {
      RegIndex = i;
    }
    sReg = JLINKARM_GetRegisterName(RegIndex);
    if (_JLinkstricmp(ac, sReg) == 0) {
      goto FoundReg;
    }
  }
  _ReportOutf("Illegal register name.\nSyntax: wreg <RegName>, <Data>\n\n");
  for (i = 0; i < NumRegs; i++) {
    if (paAlternateRegIndex) {
      RegIndex = *(paAlternateRegIndex + i);
    } else {
      RegIndex = i;
    }
    sReg = JLINKARM_GetRegisterName(RegIndex);
    _ReportOutf("%s\n", sReg);
  }
  return JLINK_ERROR_SYNTAX;
FoundReg:
  _EatWhite(&s);
  if ((*s == ',') || (*s == '=')) {
    s++;
  }
  if (_ParseHex64(&s, &Data)) {
    _ReportOutf("Syntax: wreg <RegName>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if ((DevFamily == JLINK_DEV_FAMILY_CORTEX_AR_ARMV8) && (_CortexARMV8AR_IsAArch32() == 0)) {
    _ReportOutf("%s = 0x%.8X%.8X\n", sReg, (U32)(Data >> 32), (U32)(Data >> 0));
    JLINK_WriteRegs_64(&RegIndex, &Data, NULL, 1);
  } else {
    _ReportOutf("%s = 0x%.8X\n", sReg, (U32)(Data >> 0));
    JLINKARM_WriteReg(RegIndex, (U32)Data);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGetDebugInfo
*/
static int _ExecGetDebugInfo(const char* s) {
  U32 Index, Data;
  int r;

  r = JLINK_NOERROR;
  if (_ParseHex(&s, &Index)) {
    _ReportOutf("Syntax: GetDebugInfo <Index>\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_GetDebugInfo(Index, &Data);
  if (r == 0) {
    _ReportOutf("Index 0x%.4X = 0x%.8X\n", Index, Data);
  } else {
    _ReportOutf("No debug information for index 0x%.4X available on this CPU.\n", Index);
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecReadMem
*/
static int _ExecReadMem(const char* s) {
  U32 Addr;
  U32 NumBytes;
  U32 i;
  U32 v;
  U32 NumBytesPerLine;
  U8* pData0;
  U8* pData;
  char* pChar;
  char acZone[32];
  char acLine[(16 * 4) + 1 + 1 + 1 + 1];  // Each byte is represented by 4 characters (LOWNibble, HIGHNibble, <space>, <ASCII>). 1 add. <space> column 0-7. 1 add. <space> column 15. The line is terminated by 2 characters: \n\0
  int ReadResult;
  int r;

  r = JLINK_NOERROR;
  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: mem <Addr>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumBytes)) {
    _ReportOutf("Syntax: mem <Addr>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumBytes > 0x100000) {
    _ReportOutf("NumBytes should be <= 0x100000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = pData = malloc(NumBytes);
  //
  // Read memory zoned, if necessary
  //
  if (acZone[0]) {
    ReadResult = JLINK_ReadMemZonedEx(Addr, NumBytes, pData, 0, acZone);
    if (ReadResult == (int)NumBytes) {
      ReadResult = 0;
    }
    if (ReadResult == -5) {
      _ReportOutf("Unknown zone: \"%s\".\n", acZone);
    }
  } else {
    ReadResult = JLINKARM_ReadMem(Addr, NumBytes, pData);
  }
  if (ReadResult != 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  if (JLINKARM_HasError() != 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  while (NumBytes > 0) {
    //
    // Print binary contents
    //
    _ReportOutf("%.8X = ", Addr);
    NumBytesPerLine = (NumBytes > 16) ? 16 : NumBytes;
    memset(acLine, 0x20, sizeof(acLine));                   // By default, fill everything with <space> chars
    pChar = &acLine[0];
    for (i = 0; i < NumBytesPerLine; i++) {
      if (i == 8) {
        *pChar++ = ' ';
      }
      v        = *(pData + i);
      *pChar++ = UTIL_Nibble2HexChar((v >> 4) & 0xFF);
      *pChar++ = UTIL_Nibble2HexChar((v >> 0) & 0xFF);
      *pChar++ = ' ';
    }
    if (_ASCIIViewDisable == 0) {
      //
      // Print ASCII view
      //
      pChar = &acLine[3 * 16];   // Point to ASCII representation part
      pChar++;                   // Skip add. character after column 7
      *pChar++ = ' ';            // Additional <space> between HEX and ASCII view
      for (i = 0; i < NumBytesPerLine; i++) {
        v = *(pData + i);
        if (UTIL_IsPrintableASCII(v)) {
          *pChar++ = *(pData + i);
        } else {
          *pChar++ = '.';
        }
      }
      *pChar++ = '\n';
      *pChar++ = '\0';
    } else {
      *pChar++ = '\n';
      *pChar++ = '\0';
    }
    _ReportOut(acLine);
    pData    += NumBytesPerLine;
    NumBytes -= NumBytesPerLine;
    Addr     += NumBytesPerLine;
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecReadMem8
*/
static int _ExecReadMem8(const char* s) {
  U32 Addr, NumBytes;
  U8* pData0;
  U8* pData;
  int r;
  char acZone[32];

  r = JLINK_NOERROR;
  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: mem8 <Addr>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumBytes)) {
    _ReportOutf("Syntax: mem8 <Addr>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumBytes > 0x10000) {
    _ReportOutf("NumBytes should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = pData = malloc(NumBytes);
  //
  // Read memory zoned, if necessary
  //
  if (acZone[0]) {
    r = JLINK_ReadMemZonedEx(Addr, NumBytes, pData, 1, acZone);
  } else {
    r = JLINKARM_ReadMemU8(Addr, NumBytes, pData, NULL);
  }
  if (r < 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  if (JLINKARM_HasError() == 0) {
    NumBytes = (U32)r;
    while (NumBytes > 0) {
      int NumBytesPerLine;
      _ReportOutf("%.8X = ", Addr);
      NumBytesPerLine = (NumBytes > 16) ? 16 : NumBytes;
      NumBytes -= NumBytesPerLine;
      Addr     += NumBytesPerLine;
      for (; NumBytesPerLine > 0; NumBytesPerLine--) {
        _ReportOutf("%.2X ", *pData++);
      }
      _ReportOutf("\n");
    }
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecReadMem16
*/
static int _ExecReadMem16(const char* s) {
  U32 Addr, NumItems;
  U16* pData0;
  U16* pData;
  int r;
  char acZone[32];

  r = JLINK_NOERROR;
  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: mem16 <Addr>, <NumItems>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumItems)) {
    _ReportOutf("Syntax: mem16 <Addr>, <NumItems>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumItems > 0x8000) {
    _ReportOutf("NumItems should be <= 0x8000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = (U16*)malloc(NumItems * sizeof(U16));
  pData = pData0;                                 // Remember original pointer
  //
  // Read memory zoned, if necessary
  //
  if (acZone[0]) {
    r = JLINK_ReadMemZonedEx(Addr, NumItems << 1, pData, 2, acZone);
    if (r > 0) {
      r >>= 1;
    }
  } else {
    r = JLINKARM_ReadMemU16(Addr, NumItems, pData, NULL);
  }
  if (r < 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  if (JLINKARM_HasError() == 0) {
    NumItems = (U32)r;
    while (NumItems > 0) {
      int NumItemsPerLine;
      _ReportOutf("%.8X = ", Addr);
      NumItemsPerLine = (NumItems > 8) ? 8 : NumItems;
      NumItems -= NumItemsPerLine;
      Addr     += NumItemsPerLine * 2;
      for (; NumItemsPerLine > 0; NumItemsPerLine--) {
        _ReportOutf("%.4X ", *pData++);
      }
      _ReportOutf("\n");
    }
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecReadMem32
*/
static int _ExecReadMem32(const char* s) {
  U32 Addr, NumItems;
  U32* pData0;
  U32* pData;
  int r;
  char acZone[32];

  r = JLINK_NOERROR;
  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: mem32 <Addr>, <NumItems>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumItems)) {
    _ReportOutf("Syntax: mem32 <Addr>, <NumItems>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumItems > 0x4000) {
    _ReportOutf("NumItems should be <= 0x4000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = pData = malloc(NumItems * sizeof(U32));
  //
  // Read memory zoned, if necessary
  //
  if (acZone[0]) {
    r = JLINK_ReadMemZonedEx(Addr, NumItems << 2, pData, 4, acZone);
    if (r > 0) {
      r >>= 2;
    }
  } else {
    r = JLINKARM_ReadMemU32(Addr, NumItems, pData, NULL);
  }
  if (r < 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  if (JLINKARM_HasError() == 0) {
    NumItems = (U32)r;
    while (NumItems > 0) {
      int NumItemsPerLine;
      _ReportOutf("%.8X = ", Addr);
      NumItemsPerLine = (NumItems > 4) ? 4 : NumItems;
      NumItems -= NumItemsPerLine;
      Addr     += NumItemsPerLine * 4;
      for (; NumItemsPerLine > 0; NumItemsPerLine--) {
        _ReportOutf("%.8X ", *pData++);
      }
      _ReportOutf("\n");
    }
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecReadMem64
*/
static int _ExecReadMem64(const char* s) {
  U32 Addr, NumItems;
  U64* pData0;
  U32* pData;
  int r;
  char acZone[32];

  r = JLINK_NOERROR;
  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: mem64 <Addr>, <NumItems>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumItems)) {
    _ReportOutf("Syntax: mem64 <Addr>, <NumItems>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumItems > 0x4000) {
    _ReportOutf("NumItems should be <= 0x4000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = malloc(NumItems * sizeof(U64));
  //
  // Read memory zoned, if necessary
  //
  if (acZone[0]) {
    r = JLINK_ReadMemZonedEx(Addr, NumItems << 3, pData0, 8, acZone);
    if (r > 0) {
      r >>= 3;
    }
  } else {
    r = JLINKARM_ReadMemU64(Addr, NumItems, pData0, NULL);
  }
  if (r < 0) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  if (JLINKARM_HasError() == 0) {
    pData = (U32*)pData0;
    NumItems = (U32)r;
    while (NumItems > 0) {
      int NumItemsPerLine;
      _ReportOutf("%.8X = ", Addr);
      NumItemsPerLine = (NumItems > 2) ? 2 : NumItems;
      NumItems -= NumItemsPerLine;
      Addr     += NumItemsPerLine * 8;
      for (; NumItemsPerLine > 0; NumItemsPerLine--) {
        _ReportOutf("%.8X%.8X ", *(pData + 1), *pData);
        pData += 2;
      }
      _ReportOutf("\n");
    }
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecReadMemIndirect
*/
static int _ExecReadMemIndirect(const char* s) {
  U32 Addr, NumBytes;
  U8* pData0;
  U8* pData;
  int r;

  r = JLINK_NOERROR;
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: memi <Addr>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumBytes)) {
    _ReportOutf("Syntax: memi <Addr>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumBytes > 0x10000) {
    _ReportOutf("NumBytes should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (JLINKARM_IsHalted() == 0) {
    _ReportOutf("Halting CPU in order to read memory...\n");
    JLINKARM_Halt();
  }
  pData0 = pData = malloc(NumBytes);
  if (JLINKARM_ReadMemIndirect(Addr, NumBytes, pData) != (int)NumBytes) {
    _ReportOutf("Could not read memory.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  if (JLINKARM_HasError() == 0) {
    while (NumBytes > 0) {
      int NumBytesPerLine;
      _ReportOutf("%.8X = ", Addr);
      NumBytesPerLine = (NumBytes > 16) ? 16 : NumBytes;
      NumBytes -= NumBytesPerLine;
      Addr     += NumBytesPerLine;
      for (; NumBytesPerLine > 0; NumBytesPerLine--) {
        _ReportOutf("%.2X ", *pData++);
      }
      _ReportOutf("\n");
    }
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecWriteU8
*/
static int _ExecWriteU8(const char* s) {
  U32 aData[MAX_NUM_WRITE_ITEMS];
  U32 v;
  U32 Addr;
  U32 NumItems, i;
  char acZone[32];
  int r;

  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: w1 <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  NumItems = 0;
  do {
    _EatWhiteAndSeparator(&s);
    if (*s == 0) {  // End of command? => Done
      break;
    }
    if (_ParseHex(&s, &v)) {       // End of command reached?
      if (NumItems == 0) {
        _ReportOutf("Syntax: w1 <Addr>, <Data>\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    if (NumItems >= MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d items\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    aData[NumItems] = v & 0xFF;
    NumItems++;
  } while (1);
  for (i = 0; i < NumItems; i++) {
    _ReportOutf("Writing %.2X -> %.8X\n", aData[i], Addr);
    if (acZone[0]) {  // Any zone specified?
      r = JLINK_WriteMemZonedEx(Addr, 1, &aData[i], 1, acZone);
      if (r != 1) {
        if (r == -5) {
          _ReportOutf("Unknown zone: \"%s\".\n", acZone);
        }
        _ReportOutf("Failed to write memory\n");
        return JLINK_ERROR_UNKNOWN;
      }
    } else {
      r = JLINKARM_WriteU8(Addr, (U8)aData[i]);
      if (r) {
        _ReportOutf("Failed to write memory\n");
        return JLINK_ERROR_UNKNOWN;
      }
    }
    Addr++;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteU16
*/
static int _ExecWriteU16(const char* s) {
  U32 aData[MAX_NUM_WRITE_ITEMS];
  U32 v;
  U32 Addr;
  U32 NumItems, i;
  char acZone[32];
  int r;

  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: w2 <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  NumItems = 0;
  do {
    _EatWhiteAndSeparator(&s);
    if (*s == 0) {  // End of command? => Done
      break;
    }
    if (_ParseHex(&s, &v)) {
      if (NumItems == 0) {
        _ReportOutf("Syntax: w2 <Addr>, <Data>\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    if (NumItems >= MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d items\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    aData[NumItems] = v & 0xFFFF;
    NumItems++;
  } while (1);
  for (i = 0; i < NumItems; i++) {
    _ReportOutf("Writing %.4X -> %.8X\n", aData[i], Addr);
    if (acZone[0]) {  // Any zone specified?
      r = JLINK_WriteZonedU16(Addr, aData[i], acZone);
    } else {
      r = JLINKARM_WriteU16(Addr, (U16)aData[i]);
    }
    if (r) {
      if (acZone[0]) {  // Any zone specified?
        if (r == -5) {
          _ReportOutf("Unknown zone: \"%s\".\n", acZone);
        }
      }
      _ReportOutf("Failed to write memory\n");
      return JLINK_ERROR_UNKNOWN;
    }
    Addr += 2;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteU32
*/
static int _ExecWriteU32(const char* s) {
  U32 aData[MAX_NUM_WRITE_ITEMS];
  U32 Addr;
  U32 v;
  U32 NumItems, i;
  char acZone[32];
  int r;

  _TryParseMemZone(&s, &acZone[0], sizeof(acZone));
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: w4 <Addr>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  NumItems = 0;
  do {
    _EatWhiteAndSeparator(&s);
    if (*s == 0) {  // End of command? => Done
      break;
    }
    if (NumItems >= MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d items\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    if (_ParseHex(&s, &v)) {
      if (NumItems == 0) {
        _ReportOutf("Syntax: w4 <Addr>, <Data>\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    aData[NumItems] = v;
    NumItems++;
  } while (1);
  for (i = 0; i < NumItems; i++) {
    _ReportOutf("Writing %.8X -> %.8X\n", aData[i], Addr);
    if (acZone[0]) {  // Any zone specified?
      r = JLINK_WriteZonedU32(Addr, aData[i], acZone);
    } else {
      r = JLINKARM_WriteU32(Addr, aData[i]);
    }
    if (r) {
      if (acZone[0]) {  // Any zone specified?
        if (r == -5) {
          _ReportOutf("Unknown zone: \"%s\".\n", acZone);
        }
      }
      _ReportOutf("Failed to write memory\n");
      return JLINK_ERROR_UNKNOWN;
    }
    Addr += 4;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteMultiU32
*/
static int _ExecWriteMultiU32(const char* s) {
  JLINK_WRITE_MEM_DESC aDesc[MAX_NUM_WRITE_ITEMS];
  U32 aData[MAX_NUM_WRITE_ITEMS];
  U32 NumItems, i;

  memset(aDesc, 0, sizeof(aDesc));
  NumItems = 0;
  do {
    if (NumItems > MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d items\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    if (_ParseHex(&s, &aDesc[NumItems].Addr)) {
      if (NumItems == 0) {
        _ReportOutf("Syntax: wm4 <Addr> <Data> [<Addr> <Data>...]\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &aData[NumItems])) {
      _ReportOutf("Syntax: wm4 <Addr> <Data> [<Addr> <Data>...]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    aDesc[NumItems].NumBytes  = 4;
    aDesc[NumItems].pData     = (U8*)&aData[NumItems];
    NumItems++;
  } while (1);
  for (i = 0; i < NumItems; i++) {
    _ReportOutf("Writing %.8X -> %.8X\n", aData[i], aDesc[i].Addr);
  }
  JLINKARM_WriteMemMultiple(&aDesc[0], NumItems);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecErase
*/
static int _ExecErase(const char* s) {
  JLINK_FUNC_ERASE* pf;
  I32 d;
  int r;
  U32 SAddr, EAddr;
  U32 EraseChipFlag;
  
  EraseChipFlag = 0;
  r = JLINK_NOERROR;
  if (*s) {
    if (_ParseHex(&s, &SAddr)) {
      _ReportOutf("Syntax: erase <SAddr>, <EAddr>\n");
      return JLINK_ERROR_SYNTAX;
    } else {
      _EatWhite(&s);
      if (*s == ',') {
        s++;
      }
      if (_ParseHex(&s, &EAddr)) {
        _ReportOutf("Syntax: erase <SAddr>, <EAddr>\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  } else {
    SAddr = 0;
    EAddr = 0;
    _ReportOutf("Without any given address range, Erase Chip will be executed\n");
    EraseChipFlag = 1;
  }
  //
  // Get Flash Areas from Device Config
  //
  d = JLINKARM_DEVICE_GetIndex(NULL);
  //
  // Check if everything is O.K.
  //
  if(d == -1) {
    _ReportOutf("Please select a device with \"device <DeviceName>\" before using this command.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if(JLINKARM_IsHalted() == 0) {
    JLINKARM_Halt();
  }
  //
  // For each Flash area,
  // fill an array with erase value (FF)
  // and write it
  //
  if (EraseChipFlag == 1) {
    _ReportOut("Erasing device...\n");
    r = JLINK_EraseChip();
  } else {
    if (SAddr > EAddr) {
      _ReportOutf("Start address mustn't be bigger as end address\n");
    } else {
      r = JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED;
      pf = (JLINK_FUNC_ERASE*)JLINK_GetpFunc(JLINK_IFUNC_ERASE);
      if (pf) {
        _ReportOut("Erasing selected range...\n");
        r = pf(SAddr, EAddr);
      } else {
        _ReportOutf("Erase is not implemented for this version of the J-Link software\n");
      }
    }
  }
  if (r < 0) {
    _ReportOutf("ERROR: Erase returned with error code %d.\n", r);
    return JLINK_ERROR_UNKNOWN;
  } else {
    _ReportOutf("Erasing done.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecConfigDevices
*/
static int _ExecConfigDevices(const char* s) {
  JLINKARM_JTAG_DEVICE_CONF aConf[MAX_NUM_DEVICES];
  U32 NumDevices;

  memset(aConf, 0, sizeof(aConf));
  aConf[0].SizeOfStruct = sizeof(JLINKARM_JTAG_DEVICE_CONF);
  if (_ParseDec(&s, &aConf[0].IRLen)) {
    _ReportOutf("Syntax: ConfigDevices <IRLen_0> [,<IRLen_1>,...,<IRLen_n>]\n");
    return JLINK_ERROR_SYNTAX;
  }
  NumDevices = 1;
  do {
    if (NumDevices >= MAX_NUM_DEVICES) {
      _ReportOutf("This command can only handle up to %d devices\n", MAX_NUM_DEVICES);
      return JLINK_ERROR_UNKNOWN;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    aConf[NumDevices].SizeOfStruct = sizeof(JLINKARM_JTAG_DEVICE_CONF);
    if (_ParseDec(&s, &aConf[NumDevices].IRLen)) {
      break;
    }
    NumDevices++;
  } while (1);

  JLINKARM_JTAG_ConfigDevices(NumDevices, aConf);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecWriteMultiple
*/
static int _ExecWriteMultiple(const char* s) {
  U32 NumWords;
  U8* pData;
  unsigned i;
  if (_ParseDec(&s, &NumWords)) {
    _ReportOutf("Syntax: wm <NumWords>, <NumReps>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ReportOutf("Writing 100 * %d words -> %.8X ...\n", NumWords, _RAMAddr);
  pData = (U8*)malloc(NumWords << 2);
  for (i = 0; i < 100; i++) {
    JLINKARM_WriteMem(_RAMAddr, NumWords << 2, pData);
    if (JLINKARM_HasError()) {
      _ReportOutf("  Write cycles before error: %d\n", i);
      break;
    }
  }
  if (JLINKARM_HasError() == 0) {
    _ReportOutf("  O.K.\n");
  }
  free(pData);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecAND
*/
static int _ExecAND(const char* s) {
  U64 Data;
  if (_ParseHex64(&s, &Data)) {
    return JLINK_ERROR_SYNTAX;
  }
  _Var &= Data;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecOR
*/
static int _ExecOR(const char* s) {
  U64 Data;
  if (_ParseHex64(&s, &Data)) {
    return JLINK_ERROR_SYNTAX;
  }
  _Var |= Data;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecXOR
*/
static int _ExecXOR(const char* s) {
  U64 Data;
  if (_ParseHex64(&s, &Data)) {
    return JLINK_ERROR_SYNTAX;
  }
  _Var ^= Data;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowEmuInfo
*/
static int _ExecShowEmuInfo(const char* s) {
  int NumDevices;
  JLINKARM_EMU_CONNECT_INFO aConnectInfo[50];
  JLINKARM_EMU_CONNECT_INFO * paConnectInfo;
  char NeedDealloc;
  int i;

  (void)s;
  NumDevices = JLINKARM_EMU_GetList(JLINKARM_HOSTIF_USB, &aConnectInfo[0], COUNTOF(aConnectInfo));
  _ReportOutf("%d emulator%s detected.\n", NumDevices, (NumDevices == 1) ? "" : "s");
  //
  // Allocate memory for emulator info buffer if local buffer is not big enough
  //
  NeedDealloc = 0;
  if (NumDevices > COUNTOF(aConnectInfo)) {
    paConnectInfo = malloc(NumDevices * sizeof(JLINKARM_EMU_CONNECT_INFO));
    if (paConnectInfo == NULL) {
      _ReportOutf("Failed to allocate memory for emulator info buffer.\n");
      return -1;
    }
    NeedDealloc = 1;
    JLINKARM_EMU_GetList(JLINKARM_HOSTIF_USB, paConnectInfo, NumDevices);
  } else {
    paConnectInfo = &aConnectInfo[0];
  }
  for (i = 0; i < NumDevices; i++) {
    _ReportOutf("  No.%d: USBAddr=%d, SN=%d\n", i, (paConnectInfo + i)->USBAddr, (paConnectInfo + i)->SerialNumber);
  }
  if (NeedDealloc) {
    free(paConnectInfo);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSelectEmuByUSBSN
*/
static int _ExecSelectEmuByUSBSN(const char* s) {
  U32 SerialNo;
  int r;
  unsigned WasConnectedToJLink;
  unsigned WasConnectedToTarget;
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;
  if (_ParseDec(&s, &SerialNo)) {
    _ReportOutf("Syntax: SelectEmuBySN <SerialNo>\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Disconnect from target + J-Link
  // Update commander settings
  // Reconnect to J-Link + target
  //
  WasConnectedToJLink = pCommanderSettings->Status.ConnectedToJLink;
  WasConnectedToTarget = pCommanderSettings->Status.ConnectedToTarget;
  if (WasConnectedToTarget) {
    _DisconnectFromTarget(pCommanderSettings);
  }
  if (WasConnectedToJLink) {
    _DisconnectFromJLink(pCommanderSettings);
  }
  pCommanderSettings->HostIF      = JLINKARM_HOSTIF_USB;
  pCommanderSettings->acIPAddr[0] = 0;
  pCommanderSettings->EmuSN = SerialNo;
  pCommanderSettings->IsSetEmuSN  = 1;
  r = _ConnectToJLink(pCommanderSettings);
  if (WasConnectedToTarget && (r >= 0)) {
    r = _ConnectToTarget(pCommanderSettings);
  }
  return (r < 0) ? JLINK_ERROR_UNKNOWN : JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecConfigEmu
*/
static int _ExecConfigEmu(const char* s) {
  U32 EnumMode;
  U32 USBAddr;
  U32 FakedSN;
  int r;

  if (JLINKARM_IsOpen() == 0) {
    _ReportOutf("No communication with J-Link !\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (JLINKARM_GetHardwareVersion() < 50000) {
    _ReportOutf("Command \"ConfigEmu\" is only supported by hardware version 5.00 and above.\n");
    return JLINK_NOERROR;
  }
  if (_ParseDec(&s, &EnumMode)) {
    _ReportOutf("Syntax: ConfigEmu <EnumMode> [<USBAddr>|<FakedSN>]\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  switch (EnumMode) {
  case 0:           // Old enumeration method (USBAddr: 0-3, SN: 123456)
    if (*s) {
      if (_ParseDec(&s, &USBAddr)) {
        _ReportOutf("Syntax: ConfigEmu 0 [<USBAddr>]\n");
        return JLINK_ERROR_SYNTAX;
      }
      JLINKARM_WriteEmuConfigMem((U8*)&USBAddr, 0, 1);
    }
    break;
  case 1:           // New enumeration method (use real SN)
    break;
  case 2:           // New enumeration method (use faked SN)
    if (*s) {
      if (_ParseDec(&s, &FakedSN)) {
        _ReportOutf("Syntax: ConfigEmu 2 [<FakedSN>]\n");
        return JLINK_ERROR_SYNTAX;
      }
      JLINKARM_WriteEmuConfigMem((U8*)&FakedSN, 8, 4);
    }
    break;
  default:
    _ReportOutf("ERROR: Unknown enumeration method specified.\n");
    return JLINK_ERROR_SYNTAX;
  }
  r = JLINKARM_WriteEmuConfigMem((U8*)&EnumMode, 1, 1);
  if (r != 0) {
    _ReportOutf("Failed  EMU configuration failed.\n");
    return JLINK_ERROR_UNKNOWN;
  } else {
    _ReportOutf("Please unplug the device, then plug it back in.\n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _HandleBeforeQuit
*
*  Function description
*    Handles all steps that *always* have to be executed before
*    the application quits.
*/
static void _HandleBeforeQuit(void) {
  if (JLINKARM_IsOpen()) {
    JLINKARM_Close();
  }
  SYS_timeEndPeriod(1);
}

/*********************************************************************
*
*       _cbCtrlHandler
*
*  Note
*    Makes sure that the J-Link connection is closed and J-Link DLL is unloaded correctly
*    in case of a Ctrl interrupt.
*/
#ifdef WIN32
static BOOL WINAPI _cbCtrlHandler(DWORD Type) {
  switch (Type) {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
    _CommanderSettings.CloseRequested = 1;
    return TRUE;
  case CTRL_CLOSE_EVENT:
  case CTRL_LOGOFF_EVENT:
  case CTRL_SHUTDOWN_EVENT:
    _CommanderSettings.CloseRequested = 1;
    //
    // We reach this code in case e.g. when the PC is shut down/logged out
    // or the window is closed by pressing the close button ("x").
    // In that case we do not return from SYS_ConsoleGetString() and therefore,
    // We have to close the DLL from here.
    // the application closes directly after returning from this handler,
    // Therefore we make sure that the DLL closes properly before.
    //
    _HandleBeforeQuit();
    return TRUE;
  }
  return FALSE;
}
#else
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
  _CommanderSettings.CloseRequested = 1;  // Mark thread for termination.
}
#endif

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
*       _ExecTerminal
*/
static int _ExecTerminal(const char* s) {
  U32 aData[0x100];
  U32 Data;
  U8 ac[256];
  int NumBytes;
  int iByte;
  int NumItems;
  int iItem;
  char* ps;
  int r;

  (void)s;
  _ExitTerminal = 0;
  //
  // We have two types of terminals:
  // - Binary data
  // - SEGGER DCC terminal protocol
  //
  _ReportOut("Please select terminal protocol:\n");
  _ReportOutf("B) Binary (raw) data (Default)\n");
  _ReportOutf("D) SEGGER DCC terminal\n");
  ac[0] = 'B';
  r = SYS_ConsoleGetString("Protocol>", (char*)ac, sizeof(ac));
  if (r < 0) {
    _CommanderSettings.CloseRequested = 1;
    return 0;
  }
  ps = (char*)ac;
  if (*ps) {
    ac[0] = _MakeLowercase(*ps);
  }
  //
  // Depending on the protocol, the output needs to be parsed different
  //
  if (ac[0] == 'b') {
    if (JLINKARM_EMU_HasCPUCap(JLINKARM_EMU_CPU_CAP_TERMINAL)) {
      U8 aBuffer[0x800];
      int r;
      int i;

      do {
        r = JLINKARM_ReadTerminal(&aBuffer[0], sizeof(aBuffer));
        if (r > 0) {
          for (i = 0; i < r; i++) {
            _ReportOutf("%c", aBuffer[i] & 0xFF);
          }
        }
        if (_kbhit()) {
         _getch();
         _ExitTerminal = 1;
        }
      } while (_ExitTerminal == 0);
    } else {
      _ReportOut("Protocol not supported by connected J-Link.\n");
    }
  } else if (ac[0] == 'd') {
    do {
      NumItems = JLINKARM_ReadDCC(&aData[0], COUNTOF(aData), 20);
      if (NumItems > 0) {
        for (iItem = 0; iItem < NumItems; iItem++) {
          U32 DCCHandlerVer;

          Data = aData[iItem];
          DCCHandlerVer = Data >> 24;
          if ((DCCHandlerVer & 0x93) == 0x93) {           // New DCC handler is running on target
            U32 PacketType;
            //
            // We have to check if the packet we received is a valid terminal packet
            //
            PacketType = (Data >> 16) & 0xFF;
            if (PacketType == 0x80) {                     // 1-byte terminal packet found
              NumBytes = 1;
            } else if (PacketType == 0xA0) {              // 2-byte terminal packet found
              NumBytes = 2;
            } else {                                      // No valid terminal packet found
              NumBytes = 0;
            }
          } else {                                        // Old DCC handler is running on target
            NumBytes = (Data >> 24) + 1;
          }
          for (iByte = 0; iByte < NumBytes; iByte++) {    // Display data
            _ReportOutf("%c", (Data & 0xFF));
            Data >>= 8;
          }
        }
      }
      if (_kbhit()) {
        _getch();
        _ExitTerminal = 1;
      }
    } while (_ExitTerminal == 0);
  } else {
    _ReportOut("ERROR: Illegal protocol selection!\n");
  }
  _ReportOut("\n");
 return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSTraceConfig
*/
static int _ExecSTraceConfig(const char* s) {
  int r;
  r = -1;
  _EatWhite(&s);
  if (*s) {
    r = JLINK_STRACE_Config(s);
  }
  if (r < 0) {
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSTraceControl
*  Function description
*    Makes JLINK_STRACE_Control usable via J-Link Commander
*
*  Parameters
*    s: Paramter string
*
*  Return value
*    >= 0: OK
*     < 0: Error
*
*  Additional information
*    Syntax if CMD is 0 or 2 : STraceControl <Cmd> [,<Addr> ,<Op> [, Range]]
*    Syntax if CMD is 1 :      STraceControl <Cmd>, Handle
*    Syntax if CMD is 3 :      Not implemented yet
*/
static int _ExecSTraceControl(const char* s) {
  int r;
  U32 Cmd;
  JLINK_STRACE_EVENT_INFO EvInfo;
  U32 Addr;
  U32 Op;
  U32 Range;

  memset(&EvInfo, 0, sizeof(EvInfo));
  r = -1;
  Addr  = 0;
  Op    = 0;
  Range = 0;
  if (_ParseHex(&s, &Cmd)) {
    _ReportOutf("Syntax: STraceControl <Cmd> [,<Addr> ,<Op> [, Range]] \n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s) {
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (*s == ',') {
        s++;
      }
      if (_ParseHex(&s, &Addr)) {
        _ReportOutf("Syntax: STraceControl <Cmd> [,<Addr> ,<Op> [, Range]] \n");
        return JLINK_ERROR_SYNTAX;
      }
      _EatWhite(&s);
      if (*s) {
        if (*s == ',') {
          s++;
        }
        if (_ParseHex(&s, &Op)) {
          _ReportOutf("Syntax: STraceControl <Cmd> [,<Addr> ,<Op> [, Range]] \n");
          return JLINK_ERROR_SYNTAX;
        }
        _EatWhite(&s);
        if (*s) {
          if (*s == ',') {
            s++;
          }
          if (_ParseHex(&s, &Range)) {
            _ReportOutf("Syntax: STraceControl <Cmd> [,<Addr> ,<Op> [, Range]] \n");
            return JLINK_ERROR_SYNTAX;
          }
        }
      }
    }
  }
  if (Cmd == 0x1){
    r = JLINK_STRACE_Control(Cmd, &Addr);
  } else {
    if (Cmd != 0x2) {
    EvInfo.SizeofStruct  = sizeof(EvInfo);
    EvInfo.Type          = JLINK_STRACE_EVENT_TYPE_CODE_FETCH;
    EvInfo.Addr          = Addr;
    EvInfo.Op            = Op;
    EvInfo.AddrRangeSize = Range;
    }
    r = JLINK_STRACE_Control(Cmd, &EvInfo);
  } 
  if (r < 0) {
    _ReportOutf("STraceControl: Setting Tracepoint failed. Check syntax. \n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSTraceStart
*/
static int _ExecSTraceStart(const char* s) {
  int r;
  
  (void)s;
  r = JLINK_STRACE_Start();
  if (r < 0) {
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSTraceStop
*/
static int _ExecSTraceStop(const char* s) {
  int r;
  
  (void)s;
  r = JLINK_STRACE_Stop();
  if (r < 0) {
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSTraceRead
*/
static int _ExecSTraceRead(const char* s) {
  char ac[256];
  U32 NumItems = 0x40;
  U32* pData;
  U32 InstAddr;
  U32 i;
  int r = JLINK_NOERROR;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &NumItems)) {
      _ReportOutf("Syntax: STraceRead [<NumItems>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  if (NumItems > 0x10000) {
    _ReportOutf("NumItems should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Read data
  //
  pData = (U32*)malloc(NumItems * 4);
  NumItems = JLINK_STRACE_Read(pData, NumItems);
  if ((int)NumItems < 0) {
    _ReportOutf("Could not read trace data.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  //
  // Display data
  //
  if (JLINKARM_HasError() == 0) {
    _ReportOutf("%d instructions (most recently executed first):\n", NumItems);
    for (i = 0; i < NumItems; i++) {
      InstAddr = *(pData + i);
      if (InstAddr < (U32)0xFFFFFFF0) {                  // STRACE hit an unknown address which is not in our cache? => Do not disassemble
        _DisassembleInst(ac, InstAddr, NULL, NULL, UTIL_SIZEOF(ac));
        _ReportOut(ac);
      } else {
        _ReportOutf("????????:  ??                 \n");
      }
    }
  }
Done:
  free(pData);
  return r;
}

/*********************************************************************
*
*       _ExecSTraceReadEx
*/
static int _ExecSTraceReadEx(const char* s) {
  char ac[256];
  U32 NumItems = 0x400;
  U32* pData;
  JLINK_STRACE_TIMESTAMP_INFO* paTSInfo;
  int NumTimestamps;
  int iTimestamp;
  U32 Flags = 0;
  U32 InstAddr;
  U32 i;
  int r = JLINK_NOERROR;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &NumItems)) {
      _ReportOutf("Syntax: STraceRead [<NumItems>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  if (NumItems > 0x10000) {
    _ReportOutf("NumItems should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Request as many timestamps as PC items are requested
  //
  NumTimestamps = NumItems;
  //
  // Read data
  //
  pData = (U32*)malloc(NumItems * 4);
  paTSInfo  = (JLINK_STRACE_TIMESTAMP_INFO*)malloc(sizeof(JLINK_STRACE_TIMESTAMP_INFO) * NumItems);
  NumItems = JLINK_STRACE_ReadEx(pData, NumItems, paTSInfo, &NumTimestamps, Flags);
  if ((int)NumItems < 0) {
    _ReportOutf("Could not read trace data.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  //
  // Display data
  //
  if (JLINKARM_HasError() == 0) {
    _ReportOutf("%d instructions (most recently executed first):\n", NumItems);
    iTimestamp = 0;
    for (i = 0; i < NumItems; i++) {
      InstAddr = *(pData + i);
      if (InstAddr < (U32)0xFFFFFFF0) {                  // STRACE hit an unknown address which is not in our cache? => Do not disassemble
        _DisassembleInst(ac, InstAddr, NULL, NULL, UTIL_SIZEOF(ac));
        _ReportOut(ac);
      } else {
        _ReportOutf("????????:  ??                 \n");
      }
      //
      // Print Timestamps to corresponding traced instruction
      //
      while (iTimestamp < NumTimestamps) {
        if ((*(paTSInfo + iTimestamp)).Index == i) {
          _ReportOutf("  %d Cycles\n", (*(paTSInfo + iTimestamp)).Time);
          iTimestamp++;
        } else {
          break;
        }
      }
    }
  }
Done:
  free(pData);
  free(paTSInfo);
  return r;
}

/*********************************************************************
*
*       _ExecSTraceGetInstStats
*/
static int _ExecSTraceGetInstStats(const char* s) {
  JLINK_STRACE_INST_STAT* paItem;
  char acTmp[128];
  char ac[256];
  char acFetch[64];
  char acExec [64];
  char acSkip [64];
  U32  Addr;
  U32  NumItems = 0x10;
  U32  NumItemsOut;
  U32  Flags = 0;
  U64  ExecCnt;
  U64  FetchCnt;
  U64  SkipCnt;
  void* p;
  int  r;
  int  i;
  int InstSize;
  U64* paExecCnt;
  U64* paFetchCnt;
  U64* paSkipCnt;
  //
  // Parse parameters
  //
  if (_ParseHex(&s, &Addr)) {
    _ReportOutf("Syntax: STraceGetInstStats <Addr> [<NumItems> [<Flags>]]\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s) {
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &NumItems)) {
      _ReportOutf("Syntax: STraceGetInstStats <Addr> [<NumItems> [<Flags>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s) {
      if (*s == ',') {
        s++;
      }
      if (_ParseHex(&s, &Flags)) {
        _ReportOutf("Syntax: STraceGetInstStats <Addr> [<NumItems> [<Flags>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  if (NumItems > 0x10000) {
    _ReportOutf("NumItems should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (Flags < 3) {
    //
    // Read data
    //
    p = (JLINK_STRACE_INST_STAT*)malloc(NumItems * sizeof(JLINK_STRACE_INST_STAT));
    if (p == NULL) {
      _ReportOutf("Could not alloc memory.\n");
      return JLINK_ERROR_UNKNOWN;
    }
    paItem = (JLINK_STRACE_INST_STAT*)p;
    r = JLINK_STRACE_GetInstStats(paItem, Addr, NumItems, sizeof(JLINK_STRACE_INST_STAT), Flags);
    if (r < 0) {
      _ReportOutf("Could not read instruction statistics.\n");
      r = JLINK_ERROR_UNKNOWN;
      goto Done;
    }
    //
    // Display data
    //
    if (JLINKARM_HasError() == 0) {
      if (NumItems) {
        _ReportOutf("Addr       ExecCnt          Opcode             Disassembly\n");
        _ReportOutf("---------------------------------------------------------------------\n");
        do {
          //
          // Disassemble twice:
          // First to get the instruction size, second to get the actual instruction + append execution count
          // Necessary as we need to know how many "items" of the requested range belong to a instruction
          //
          InstSize = JLINKARM_DisassembleInst(ac, sizeof(ac), Addr);
          if (InstSize < 0) {
            _ReportOutf("Unknown instruction hit at 0x%8X. Further program flow cannot be reconstructed properly.", Addr);
            break;
          }
          ExecCnt = 0;
          for (i = 0; i < InstSize; i += 2) {
            if (NumItems == 0) {   // In case we request less items than the current instruction needs
              break;
            }
            ExecCnt += paItem->ExecCnt;
            paItem++;
            NumItems--;
          }
          _PrintHex64(acTmp, UTIL_SIZEOF(acTmp), ExecCnt);
          _DisassembleInst(ac, Addr, NULL, acTmp, UTIL_SIZEOF(ac));
          _ReportOutf(ac);
          Addr += InstSize;
        } while (NumItems);
      }
    }
  } else {
    //
    // Read data
    //
    NumItems = (NumItems + 1) * 3;
    p = (JLINK_STRACE_INST_STAT*)malloc(NumItems * sizeof(JLINK_STRACE_INST_STAT));
    if (p == NULL) {
      _ReportOutf("Could not alloc memory.\n");
      return JLINK_ERROR_UNKNOWN;
    }
    paItem = (JLINK_STRACE_INST_STAT*)p;
    r = JLINK_STRACE_GetInstStats(paItem, Addr, NumItems, sizeof(JLINK_STRACE_INST_STAT), JLINK_STRACE_CNT_TYPE_FETCHED_EXEC_SKIP);
    if (r < 0) {
      _ReportOutf("Could not read instruction statistics.\n");
      r = JLINK_ERROR_UNKNOWN;
      goto Done;
    }
    //
    // Display data
    //
    if (JLINKARM_HasError() == 0) {
      if (NumItems) {
        _ReportOutf("Addr       FetchCnt          ExecCnt           SkipCnt          Opcode             Disassembly\n");
        _ReportOutf("----------------------------------------------------------------------------------------------------\n");
        //
        // We get the counters in the following format from the DLL:
        // (NumItems - 1) * 8 bytes FetchCnt
        // 8 bytes FetchTotal
        // (NumItems - 1) * 8 bytes ExecCnt
        // 8 bytes ExecTotal
        // (NumItems - 1) * 8 bytes SkipCnt
        // 8 bytes SkipTotal
        //
        NumItems = ((NumItems - 3) / 3);    // substract Total count and then determine count for each category
        NumItemsOut = NumItems;
        paFetchCnt = (U64*)(paItem + 0);
        paExecCnt  = (U64*)(paItem + 1 * (NumItems + 1));
        paSkipCnt  = (U64*)(paItem + 2 * (NumItems + 1));
        do {
          //
          // Disassemble twice:
          // First to get the instruction size, second to get the actual instruction + append execution count
          // Necessary as we need to know how many "items" of the requested range belong to a instruction
          //
          InstSize = JLINKARM_DisassembleInst(ac, sizeof(ac), Addr);
          if (InstSize < 0) {
            _ReportOutf("Unknown instruction hit at 0x%8X. Further program flow cannot be reconstructed properly.", Addr);
            break;
          }
          ExecCnt  = 0;
          FetchCnt = 0;
          SkipCnt  = 0;
          for (i = 0; i < InstSize; i += 2) {
            if (NumItemsOut == 0) {   // In case we request less items than the current instruction needs
              break;
            }
            FetchCnt += *paFetchCnt++;
            ExecCnt  += *paExecCnt++;
            SkipCnt  += *paSkipCnt++;
            NumItemsOut--;
          }
          _PrintHex64(acFetch, UTIL_SIZEOF(acFetch), FetchCnt);
          _PrintHex64(acExec, UTIL_SIZEOF(acExec),   ExecCnt);
          _PrintHex64(acSkip, UTIL_SIZEOF(acSkip),   SkipCnt);
          UTIL_snprintf(acTmp, UTIL_SIZEOF(acTmp), "%s  %s  %s ", acFetch, acExec, acSkip);
          InstSize = _DisassembleInst(ac, Addr, NULL, acTmp, UTIL_SIZEOF(ac));
          _ReportOutf(ac);
          Addr += InstSize;
        } while (NumItemsOut);
        _PrintHex64(acFetch, UTIL_SIZEOF(acFetch), *paFetchCnt);
        _PrintHex64(acExec, UTIL_SIZEOF(acExec),   *paExecCnt);
        _PrintHex64(acSkip, UTIL_SIZEOF(acSkip),   *paSkipCnt);
        _ReportOutf("\n");
        _ReportOutf("Total:     %s  %s  %s\n", acFetch, acExec, acSkip);
      }
    }
  }
  r = JLINK_NOERROR;
Done:
  free(p);
  return r;
}

/*********************************************************************
*
*       _ExecSTraceBIST
*/
static int _ExecSTraceBIST(const char* s) {
  JLINK_RAWTRACE_BIST_ERR_BUF_DESC aBISTErr[16];
  JLINK_FUNC_RAWTRACE_BIST_STARTSTOP* pfStartStop;
  JLINK_FUNC_RAWTRACE_BIST_READ_ERR_STATS* pfGetErrStats;
  int r;
  int i;
  int NumErrors;
  U32 UpTimeSeconds;
  int NumLoops;

  (void)s;
  pfStartStop = (JLINK_FUNC_RAWTRACE_BIST_STARTSTOP*)JLINK_GetpFunc(JLINK_IFUNC_RAWTRACE_BIST_STARTSTOP);
  pfGetErrStats = (JLINK_FUNC_RAWTRACE_BIST_READ_ERR_STATS*)JLINK_GetpFunc(JLINK_IFUNC_RAWTRACE_BIST_READ_ERR_STATS);
  if ((pfStartStop == NULL) || (pfGetErrStats == NULL)) {
    _ReportOutf("ERROR: The J-Link software used does not support trace BIST.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  r = pfStartStop(1);
  if (r == JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED) {
    _ReportOutf("ERROR: The connected trace probe does not support trace BIST.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // BIST error statistics are requested from probe every 100ms to keep USB traffic low
  // Also only check _kbhit() every 100ms as calling this function is relatively expensive under Windows (slows application down. Did not investigate further but have already seen this a few years ago when testing SWO stuff)
  //
  NumErrors     = 0;
  UpTimeSeconds = 0;
  NumLoops      = 0;
  _ReportOut("BIST started. Press any key to cancel.\n");
  do {
    if (NumLoops == 0) {
      _ReportOutf("Test running for %6u seconds. %d errors in total\n", UpTimeSeconds, NumErrors);
      NumLoops = 600;
      UpTimeSeconds += 60;
    }
    r = pfGetErrStats(aBISTErr, COUNTOF(aBISTErr));
    if (r > 0) {
      NumErrors += r;
      for (i = 0; i < r; i++) {
        _ReportOutf("Error @ 0x%.8X: Expected: 0x%.8X, Read: 0x%.8X, ReRead: 0x%.8X\n", aBISTErr[i].FailAddr, aBISTErr[i].DataExpected, aBISTErr[i].DataRead, aBISTErr[i].DataReRead);
      }
    }
    if (_kbhit() != 0) {
      _getch();
      _ReportOutf("\nTest canceled by user!\n");
      break;
    }
    _Sleep(100);
    NumLoops--;
  } while (1);
  pfStartStop(0);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWOSpeed
*/
static int _ExecSWOSpeed(const char* s) {
  JLINKARM_SWO_SPEED_INFO SpeedInfo = {0};
  U32 Interface = 0;
  I32 r;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &Interface)) {
      _ReportOutf("Syntax: SWOSpeed [<Interface>]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Read speed info
  //
  SpeedInfo.SizeofStruct = sizeof(SpeedInfo);
  SpeedInfo.Interface    = Interface;
  r = JLINKARM_SWO_Control(JLINKARM_SWO_CMD_GET_SPEED_INFO, &SpeedInfo);
  if (r < 0) {
    _ReportOutf("Could not read SWO speedinfo.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Show supported SWO speeds
  //
  _ReportOutf("Supported speeds:\n");
  if (SpeedInfo.BaseFreq > 1000) {
    _ReportOutf(" - %d kHz/n, (n>=%d). => %dkHz, %dkHz, %dkHz, ...\n", SpeedInfo.BaseFreq / 1000, SpeedInfo.MinDiv,
                                                                  SpeedInfo.BaseFreq / 1000 / (SpeedInfo.MinDiv + 0),
                                                                  SpeedInfo.BaseFreq / 1000 / (SpeedInfo.MinDiv + 1),
                                                                  SpeedInfo.BaseFreq / 1000 / (SpeedInfo.MinDiv + 2));
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWOStart
*/
static int _ExecSWOStart(const char* s) {
  int CPUSpeed;
  U32 Interface = 0;
  U32 Speed     = 19200;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &Speed)) {
      _ReportOutf("Syntax: SWOStart [<Speed> [<Interface>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (_ParseDec(&s, &Interface)) {
        _ReportOutf("Syntax: SWOStart [<Speed> [<Interface>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  //
  // Start SWO
  //
  CPUSpeed = JLINKARM_MeasureCPUSpeed((U32)-1, 1);
  JLINKARM_SWO_EnableTarget(CPUSpeed, Speed, Interface, 0xFFFFFFFF);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWOStop
*/
static int _ExecSWOStop(const char* s) {
  (void)s;
  JLINKARM_SWO_DisableTarget(0xFFFFFFFF);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWOStat
*/
static int _ExecSWOStat(const char* s) {
  U32 NumBytesInBuffer;
  
  (void)s;
  NumBytesInBuffer = JLINKARM_SWO_Control(JLINKARM_SWO_CMD_GET_NUM_BYTES, NULL);
  _ReportOutf("%d bytes in host buffer\n", NumBytesInBuffer);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWORead
*/
static int _ExecSWORead(const char* s) {
  U32 NumBytesInBuffer;
  U32 NumBytes = 0x100;
  U32 Offset   = 0;
  U8* pData0;
  U8* pData;
  int r = JLINK_NOERROR;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &NumBytes)) {
      _ReportOutf("Syntax: SWORead [<NumBytes> [<Offset>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (_ParseHex(&s, &Offset)) {
        _ReportOutf("Syntax: SWORead [<NumBytes> [<Offset>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  if (NumBytes > 0x10000) {
    _ReportOutf("NumBytes should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Read data
  //
  pData0 = pData = malloc(NumBytes);
  JLINKARM_SWO_Read(pData, Offset, &NumBytes);
  //
  // Display data
  //
  if (JLINKARM_HasError() == 0) {
    NumBytesInBuffer = JLINKARM_SWO_Control(JLINKARM_SWO_CMD_GET_NUM_BYTES, NULL);
    _ReportOutf("%d bytes read (%d bytes in host buffer)\n", NumBytes, NumBytesInBuffer);
    while (NumBytes > 0) {
      int NumBytesPerLine;
      _ReportOutf("%.8X = ", Offset);
      NumBytesPerLine = (NumBytes > 16) ? 16 : NumBytes;
      NumBytes -= NumBytesPerLine;
      Offset   += NumBytesPerLine;
      for (; NumBytesPerLine > 0; NumBytesPerLine--) {
        _ReportOutf("%.2X ", *pData++);
      }
      _ReportOutf("\n");
    }
  }
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _AnalyzeSWOPacket
*
*  Return value:
*    >0  Len of valid packet
*    =0  Not enough data in buffer to analyze packet
*    -1  Unknown packet
*    -2  Ignore packet (in case of "Idle packet")
*/
static int _AnalyzeSWOPacket(U8* pData, U32 NumBytes, char* pInfo, U32 BufSize) {
  int CmdLen, i;
  U8  Cmd, d;
  U32 Data;
  U32 Id;
  //
  // Early out
  //
  Data = 0;  // Avoid false-positive "unintinialized variable" warning
  d    = 0;  // Avoid false-positive "unintinialized variable" warning
  if (NumBytes == 0) {
    return 0;                   // Not enough data
  }
  //
  // Parse command
  //
  Cmd = *pData++;
  //
  // Idle / Sync
  //
  if      (Cmd == 0x00) {
    if (NumBytes >= 6) {
      Data = _Load32LE(pData);
      if ((Data == 0) && (*(pData + 4) == 0x80)) {
        UTIL_snprintf(pInfo, BufSize, "Sync");
        return 6;
      }
    }
    return -2;                  // Ignore packet
  }
  //
  // Sync
  //
  else if (Cmd == 0x80) {
    return -2;                  // Ignore packet, since we can not check for full sync packet
  }
  //
  // Overflow
  //
  else if (Cmd == 0x70) {
    UTIL_snprintf(pInfo, BufSize, "Overflow");
    return 1;
  }
  //
  // Timestamp
  //
  else if ((Cmd & 0x0F) == 0x00) {
    //
    // multi-byte packet
    //
    if (Cmd & (1 << 7)) {
      Cmd = (Cmd >> 4) & 7;
      CmdLen = 1;
      Data   = 0;
      for (i = 0; i < 4; i++) {
        CmdLen++;
        d = *pData++;
        Data |= (d & 0x7F) << (i * 7);
        if ((d & 0x80) == 0) {
          break;
        }
      }
      if (d & 0x80) {
        return -1;              // Unknown packet
      }
      if        (Cmd == 4) {
        UTIL_snprintf(pInfo, BufSize, "Timestamp sync. event (%d)", Data);
      } else if (Cmd == 5) {
        UTIL_snprintf(pInfo, BufSize, "Timestamp delayed event (%d)", Data);
      } else {
        return -1;              // Unknown packet
      }
      return CmdLen;
    }
    //
    // single-byte packet
    //
    else {
      UTIL_snprintf(pInfo, BufSize, "Timestamp sync. ITM/DWT (%d)", (Cmd >> 4) & 7);
      return 1;
    }
  }
  //
  // Reserved
  //
  else if ((Cmd & 0x0F) == 0x04) {
  }
  //
  // Extension
  //
  else if ((Cmd & 0x0B) == 0x08) {
  }
  //
  // Software/Hardware Source
  //
  else if ((Cmd & 0x03) != 0x00) {
    Id     = (Cmd & 0xF8) >> 3;
    CmdLen = (1 << ((Cmd & 3) - 1)) + 1;
    if ((int)NumBytes < CmdLen) {
      return 0;                 // Not enough data
    }
    switch (CmdLen) {
    case 2: Data = _Load8LE (pData);  break;
    case 3: Data = _Load16LE(pData);  break;
    case 5: Data = _Load32LE(pData);  break;
    }
    //
    // Hardware Source (Diagnostics)
    //
    if (Cmd & (1 << 2)) {
      switch (Id) {
      //
      // Event Packet
      //
      case 0:
        if (CmdLen != 2) {
          return -1;            // Unknown packet
        }
        UTIL_snprintf(pInfo, BufSize, "Event counter (");
        if (Data & (1 << 0)) { UTIL_strcat(pInfo, "CPI,", BufSize);   }
        if (Data & (1 << 1)) { UTIL_strcat(pInfo, "Exc,", BufSize);   }
        if (Data & (1 << 2)) { UTIL_strcat(pInfo, "Sleep,", BufSize); }
        if (Data & (1 << 3)) { UTIL_strcat(pInfo, "LSU,", BufSize);   }
        if (Data & (1 << 4)) { UTIL_strcat(pInfo, "Fold,", BufSize);  }
        if (Data & (1 << 5)) { UTIL_strcat(pInfo, "Cyc,", BufSize);   }
        if (Data & 0x3F) {
          *(pInfo + UTIL_strlen(pInfo) - 1) = 0;
        }
        UTIL_strcat(pInfo, ")", BufSize);
        return CmdLen;
      //
      // Exception Trace Packet
      //
      case 1:
        if (CmdLen != 3) {
          return -1;            // Unknown packet
        }
        UTIL_snprintf(pInfo, BufSize, "Exception %d (", (Data & 0x1FF));
        if (((Data >> 12) & 3) == 0) { UTIL_strcat(pInfo, "Invalid", BufSize); }
        if (((Data >> 12) & 3) == 1) { UTIL_strcat(pInfo, "Entry", BufSize);   }
        if (((Data >> 12) & 3) == 2) { UTIL_strcat(pInfo, "Exit", BufSize);    }
        if (((Data >> 12) & 3) == 3) { UTIL_strcat(pInfo, "Return", BufSize);  }
        UTIL_strcat(pInfo, ")", BufSize);
        return CmdLen;
      //
      // PC Sample Packet
      //
      case 2:
        UTIL_snprintf(pInfo, BufSize, "PC = 0x%.8X", Data);
        return CmdLen;
      }
    }
    //
    // Software Source (Application)
    //
    else {
      switch (CmdLen) {
      case 2: UTIL_snprintf(pInfo, BufSize, "SWIT(%d): %.2X",                Id, (Data >> 0) & 0xFF);                                                                break;
      case 3: UTIL_snprintf(pInfo, BufSize, "SWIT(%d): %.2X %.2X",           Id, (Data >> 0) & 0xFF, (Data >> 8) & 0xFF);                                            break;
      case 5: UTIL_snprintf(pInfo, BufSize, "SWIT(%d): %.2X %.2X %.2X %.2X", Id, (Data >> 0) & 0xFF, (Data >> 8) & 0xFF, (Data >> 16) & 0xFF, (Data >> 24) & 0xFF);  break;
      }
      return CmdLen;
    }
  }
  return -1;                    // Unknown packet
}

/*********************************************************************
*
*       _AnalyzeSWOData
*/
static void _AnalyzeSWOData(U8* pData, U32 Offset, U32 NumBytes) {
  char  acInfo[256] = {0};
  int   Sync = 0;
  int   Len, i;
  //
  // Early out
  //
  if (NumBytes == 0) {
    return;
  }
  //
  // Display packets
  //
  _ReportOutf("Offset     Data               Meaning\n");
  _ReportOutf("-----------------------------------------------\n");
  while (1) {
    Len = _AnalyzeSWOPacket(pData, NumBytes, &acInfo[0], UTIL_SIZEOF(acInfo));
    //
    // Handle valid packet
    //
    if (Len > 0) {
      Sync = 1;
      _ReportOutf("%.4X-%.4X  ", Offset, Offset + Len - 1);
      for (i = 0; i < Len; i++) {
        _ReportOutf("%.2X ", *(pData + i));
      }
      for (i = Len; i < 6; i++) {
        _ReportOutf("   ");
      }
      _ReportOutf(" %s\n", acInfo);
    }
    //
    // Handle unknown packet
    //
    else if (Len < 0) {
      if ((Len == -1) && Sync) {
        _ReportOutf("%.4X-....  ", Offset);
        _ReportOutf("%.2X                 ", *pData);
        _ReportOutf("Unknown packet\n");
        break;
      }
      Len = 1;
    }
    //
    // Handle end of data
    //
    else {
      if (Sync == 0) {
        _ReportOutf("No synchronization...\n");
      }
      break;
    }
    //
    // Increment offset
    //
    pData    += Len;
    Offset   += Len;
    NumBytes -= Len;
  }
}

/*********************************************************************
*
*       _ExecSWOShow
*/
static int _ExecSWOShow(const char* s) {
  U32 NumBytesInBuffer;
  U32 NumBytes = 0x100;
  U32 Offset   = 0;
  U8* pData;
  int r = JLINK_NOERROR;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &NumBytes)) {
      _ReportOutf("Syntax: SWOShow [<NumBytes> [<Offset>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (_ParseHex(&s, &Offset)) {
        _ReportOutf("Syntax: SWOShow [<NumBytes> [<Offset>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  if (NumBytes > 0x10000) {
    _ReportOutf("NumBytes should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Read data
  //
  pData = malloc(NumBytes);
  JLINKARM_SWO_Read(pData, Offset, &NumBytes);
  //
  // Display data
  //
  if (JLINKARM_HasError() == 0) {
    NumBytesInBuffer = JLINKARM_SWO_Control(JLINKARM_SWO_CMD_GET_NUM_BYTES, NULL);
    _ReportOutf("%d bytes read (%d bytes in host buffer)\n", NumBytes, NumBytesInBuffer);
    _AnalyzeSWOData(pData, Offset, NumBytes);
  }
  free(pData);
  return r;
}

/*********************************************************************
*
*       _ExtractSWIT
*
*  Return value:
*    >0  Len of valid packet
*    =0  Not enough data in buffer to analyze packet
*    -1  Unknown packet
*    -2  Ignore packet (in case of "Idle packet")
*/
static int _ExtractSWIT(U8* pData, U32 NumBytes, U32 Port) {
  int CmdLen, i;
  U8  Cmd, d;
  U32 Data;
  U32 Id;
  //
  // Early out
  //
  Data = 0;  // Avoid false-positive "unintinialized variable" warning
  d    = 0;  // Avoid false-positive "unintinialized variable" warning
  if (NumBytes == 0) {
    return 0;                   // Not enough data
  }
  //
  // Parse command
  //
  Cmd = *pData++;
  //
  // Idle / Sync
  //
  if (Cmd == 0x00) {
    if (NumBytes >= 6) {
      Data = _Load32LE(pData);
      if ((Data == 0) && (*(pData + 4) == 0x80)) {
        return 6;
      }
    }
    return -2;                  // Ignore packet
  }
  //
  // Sync
  //
  else if (Cmd == 0x80) {
    return -2;                  // Ignore packet, since we can not check for full sync packet
  }
  //
  // Overflow
  //
  else if (Cmd == 0x70) {
    return 1;
  }
  //
  // Timestamp
  //
  else if ((Cmd & 0x0F) == 0x00) {
    //
    // multi-byte packet
    //
    if (Cmd & (1 << 7)) {
      Cmd = (Cmd >> 4) & 7;
      CmdLen = 1;
      Data   = 0;
      for (i = 0; i < 4; i++) {
        CmdLen++;
        d = *pData++;
        Data |= (d & 0x7F) << (i * 7);
        if ((d & 0x80) == 0) {
          break;
        }
      }
      if (d & 0x80) {
        return -1;              // Unknown packet
      }
      if        (Cmd == 4) {
      } else if (Cmd == 5) {
      } else {
        return -1;              // Unknown packet
      }
      return CmdLen;
    }
    //
    // single-byte packet
    //
    else {
      return 1;
    }
  }
  //
  // Reserved
  //
  else if ((Cmd & 0x0F) == 0x04) {
  }
  //
  // Extension
  //
  else if ((Cmd & 0x0B) == 0x08) {
  }
  //
  // Software/Hardware Source
  //
  else if ((Cmd & 0x03) != 0x00) {
    Id     = (Cmd & 0xF8) >> 3;
    CmdLen = (1 << ((Cmd & 3) - 1)) + 1;
    if ((int)NumBytes < CmdLen) {
      return 0;                 // Not enough data
    }
    switch (CmdLen) {
    case 2: Data = _Load8LE (pData);  break;
    case 3: Data = _Load16LE(pData);  break;
    case 5: Data = _Load32LE(pData);  break;
    }
    //
    // Hardware Source (Diagnostics)
    //
    if (Cmd & (1 << 2)) {
      switch (Id) {
      //
      // Event Packet
      //
      case 0:
        if (CmdLen != 2) {
          return -1;            // Unknown packet
        }
        return CmdLen;
      //
      // Exception Trace Packet
      //
      case 1:
        if (CmdLen != 3) {
          return -1;            // Unknown packet
        }
        return CmdLen;
      //
      // PC Sample Packet
      //
      case 2:
        return CmdLen;
      }
    }
    //
    // Software Source (Application)
    //
    else {
      if (Id == Port) {
        switch (CmdLen) {
        case 2:
          _ReportOutf("%c",       (Data >> 0) & 0xFF);
          break;
        case 3:
          _ReportOutf("%c%c",     (Data >> 0) & 0xFF, (Data >> 8) & 0xFF);
          break;
        case 5:
          _ReportOutf("%c%c%c%c", (Data >> 0) & 0xFF, (Data >> 8) & 0xFF, (Data >> 16) & 0xFF, (Data >> 24) & 0xFF);
          break;
        }
      }
      return CmdLen;
    }
  }
  return -1;                    // Unknown packet
}

/*********************************************************************
*
*       _ShowStimulusData
*/
static int _ShowStimulusData(U8* pData, U32 NumBytes, U32 Port) {
  U32 NumBytesRem;
  int Sync = 0;
  int Len;
  //
  // Early out
  //
  if (NumBytes == 0) {
    return 0;
  }
  //
  // Display packets
  //
  NumBytesRem = NumBytes;
  while (1) {
    Len = _ExtractSWIT(pData, NumBytesRem, Port);
    //
    // Handle valid packet
    //
    if (Len > 0) {
      Sync = 1;
    }
    //
    // Handle unknown packet
    //
    else if (Len < 0) {
      if ((Len == -1) && Sync) {
        break;
      }
      Len = 1;
    }
    //
    // Handle end of data
    //
    else {
      break;
    }
    //
    // Increment offset
    //
    pData       += Len;
    NumBytesRem -= Len;
  }
  return NumBytes - NumBytesRem;
}

/*********************************************************************
*
*       _DetermineSWOSpeed
*/
static int _DetermineSWOSpeed(U32 TargetFreq, U32* pTargetDiv, U32* pJLinkFreq, U32* pJLinkDiv) {
  JLINKARM_SWO_SPEED_INFO SpeedInfo = {0};
  U32 JLinkFreq;
  U32 JLinkDiv;
  U32 TargetDiv;
  float Deviation;

  SpeedInfo.SizeofStruct = sizeof(SpeedInfo);
  SpeedInfo.Interface    = 0;
  JLINKARM_SWO_Control(JLINKARM_SWO_CMD_GET_SPEED_INFO, &SpeedInfo);
  //
  // Find speed that matches for both: Emulator and debugger
  //
  JLinkFreq = SpeedInfo.BaseFreq;
  JLinkDiv  = SpeedInfo.MinDiv;
  TargetDiv = TargetFreq / (JLinkFreq / JLinkDiv); // Scale down target frequency to app. J-Link speed
  do {
    Deviation = ((float)JLinkFreq / (float)JLinkDiv) / ((float)TargetFreq / (float)TargetDiv);
    Deviation -= 1;
    Deviation *= 100;
    if ((Deviation <= 3.0) && (Deviation >= -3.0)) {
      break;
    }
    if (Deviation > 0.0) {   // Increment J-Link divider
      JLinkDiv++;
    } else {                 // Increment target divider
      TargetDiv++;
    }
  } while(1);
  if ((JLinkDiv > SpeedInfo.MaxDiv) || (TargetDiv > 0x2000)) {                     // Check if no matching frequency has been found. For the target we have a 13-bit prescaler which can be configured
    return -1;
  }
  *pJLinkFreq = JLinkFreq;
  *pJLinkDiv  = JLinkDiv;
  *pTargetDiv = TargetDiv;
  return 0;
}

/*********************************************************************
*
*       _ExecSWOView
*/
static int _ExecSWOView(const char* s) {
  JLINKARM_SWO_START_INFO StartInfo = {0};
  U32 NumBytes;
  U32 Interface  = 0;
  U32 Port       = 0;
  U32 TargetFreq = 0;
  U32 TargetDiv  = 1;
  U32 JLinkFreq  = 0;
  U32 JLinkDiv   = 1;
  U8* pData;
  int r = JLINK_NOERROR;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &TargetFreq)) {
      _ReportOutf("Syntax: SWOView <TargetFreq>\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Init SWO
  //
  if (TargetFreq == 0) {
    TargetFreq = JLINKARM_MeasureCPUSpeed(-1, 1);
  }
  if (TargetFreq <= 0) {
    _ReportOutf("ERROR: Could not measure target CPU speed.\n");
    _ReportOutf("Please select a device using the \"device\" command or enter\n");
    _ReportOutf("the target CPU speed manually using \"SWOView <TargetFreq>\".\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (_DetermineSWOSpeed(TargetFreq, &TargetDiv, &JLinkFreq, &JLinkDiv) < 0) {
    _ReportOutf("ERROR: Can not determine SWO clock frequency.\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_WriteU32(0xE000EDFC, 0x01000000);      // Debug Exception and Monitor Control Register.
  JLINKARM_WriteU32(0xE0042004, 0x00000027);      //
  JLINKARM_WriteU32(0xE00400F0, 0x00000002);      // Selected Pin Protocol Register, Select NRZ mode
  JLINKARM_WriteU32(0xE0040010, TargetDiv - 1);   // Async Clock Prescaler Register (72/48 = 1,5 MHz)
  JLINKARM_WriteU32(0xE0000FB0, 0xC5ACCE55);      // Lock Access Register
  JLINKARM_WriteU32(0xE0000E80, 0x0001000D);      // Trace Control Register
  JLINKARM_WriteU32(0xE0000E40, 0x0000000F);      // Trace Privilege Register
  JLINKARM_WriteU32(0xE0000E00, 0x00000001);      // Trace Enable Register
  JLINKARM_WriteU32(0xE0001000, 0x400003FE);      // DWT Control Register
  JLINKARM_WriteU32(0xE0040304, 0x00000100);      // Formatter and Flush Control Register
  //
  // Start SWO
  //
  StartInfo.SizeofStruct = sizeof(StartInfo);
  StartInfo.Interface    = Interface;
  StartInfo.Speed        = JLinkFreq / JLinkDiv;
  _ReportOutf("\nReceiving SWO data @ %d kHz.\n", StartInfo.Speed / 1000);
  JLINKARM_SWO_Control(JLINKARM_SWO_CMD_START, &StartInfo);
  //
  // Read and analyze data
  //
  _ReportOutf("Data from stimulus port %d:\n", Port);
  _ReportOutf("-----------------------------------------------\n");
  _ExitTerminal = 0;
  pData = malloc(0x800);
  do {
    NumBytes = 0x800;
    JLINKARM_SWO_Read(pData, 0, &NumBytes);
    if (NumBytes > 0) {
      NumBytes = _ShowStimulusData(pData, NumBytes, Port);
      JLINKARM_SWO_Control(JLINKARM_SWO_CMD_FLUSH, &NumBytes);
    }
    if (_kbhit()) {
      _getch();
      _ExitTerminal = 1;
    }
  } while (_ExitTerminal == 0);
  free(pData);
  //
  // Stop SWO
  //
  JLINKARM_SWO_Control(JLINKARM_SWO_CMD_STOP, NULL);
  return r;
}

/*********************************************************************
*
*       _ExecSWOFlush
*/
static int _ExecSWOFlush(const char* s) {
  U32* pNumBytes = NULL;
  U32 NumBytes;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &NumBytes)) {
      _ReportOutf("Syntax: SWOFlush [<NumBytes>]\n");
      return JLINK_ERROR_SYNTAX;
    }
    pNumBytes = &NumBytes;
  }
  //
  // Flush data
  //
  JLINKARM_SWO_Control(JLINKARM_SWO_CMD_FLUSH, pNumBytes);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSWOStabilityTest
*
*  Function description
*    Performs SWO stability test on Cortex-M target.
*    Generic RAMCode is downloaded which outputs a 4-byte unsigned counter via SWO,
*    We should ONLY receive SWIT packets. NO sync packets or anything else.
*    The SWIT packets will always have 4 bytes payload which describe a counter value that is incremented.
*    We check if we receive the correct counter value.
*
*  Notes
*    Stability test is always performed with the highest SWO speed that is supported by the CPU and J-Link.
*    Device needs to be selected to perform this test
*/
static int _ExecSWOStabilityTest(const char* s) {
  U32 Data;
  int HasError;
  U32 NumErrPackets = 0;
  U32 OldNumErrPackets = 0;
  U32 NumBytesRead;
  U32 NumBytesInBuff;
  U32 Cnt;
  U8 * pData;
  U8 * pAlloc;
  JLINKARM_DEVICE_INFO DeviceInfo;
  int iDevice;
  U32 CPUSpeedHz;
  U32 MaxSWOSpeedHz;
  int r;
  U32 v;
  U8 acSWORAMCode[] = {
                        0x0A, 0x68, // ??main_0: LDR      R2,[R1, #+0]
                        0xD2, 0x07, // LSLS     R2,R2,#+31
                        0xFC, 0xD5, // BPL.N    ??main_0
                        0x08, 0x60, // STR      R0,[R1, #+0]
                        0x40, 0x1C, // ADDS     R0,R0,#+1
                        0xF9, 0xE7  // B.N      ??main_0
                      };
  
  (void)s;
  //
  // Check if connected J-Link has SWO capabilities
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_SWO) == 0) {
    _ReportOutf("Connected J-Link does not support SWO.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Check if device is set and if it is a Cortex-M.
  // If not, we are done.
  //
  iDevice = JLINKARM_DEVICE_GetIndex(NULL);
  if(iDevice == -1) {
    _ReportOutf("Please select a device with \"device <DeviceName>\" before using this command.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  DeviceInfo.SizeOfStruct = sizeof(JLINKARM_DEVICE_INFO);
  JLINKARM_DEVICE_GetInfo(iDevice, &DeviceInfo);
  switch (DeviceInfo.Core >> 24) {
  case JLINKARM_DEV_FAMILY_CM1:
  case JLINKARM_DEV_FAMILY_CM3:
  case JLINKARM_DEV_FAMILY_CM0:
  case JLINKARM_DEV_FAMILY_CM4:
    break;
  default:
    _ReportOutf("This command is only supported for Cortex-M devices.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Make sure that SWD is selected as target interface
  // and remember current speed settings
  //
  _ReportOutf("Reconnecting to device via SWD interface...");
  v = JLINKARM_GetSpeed();
  JLINKARM_TIF_Select(JLINKARM_TIF_SWD);
  JLINKARM_SetSpeed(v);
  _ReportOutf("Connecting to target via %s\n", _TIF2Name(JLINKARM_TIF_SWD));
  r = JLINKARM_Connect();
  if (r != 0) {
    if (r == JLINK_ERR_VCC_FAILURE) {
      _ReportOutf("Target voltage too low. Please check https://wiki.segger.com/J-Link_cannot_connect_to_the_CPU#Target_connection.\n");
    } else {
      r = JLINK_ERROR_UNKNOWN;
    }
    _ReportOutf("Failed to reconnect to device.\n");
    return r;
  }
  //
  // Halt CPU
  //
  if(JLINKARM_IsHalted() == 0) {
    JLINKARM_Halt();
  }
  //
  // Determine max. SWO speed by measuring CPU speed and checking J-Link SWO speed capabilities
  //
  CPUSpeedHz = JLINKARM_MeasureCPUSpeed(-1, 1);
  if ((int)CPUSpeedHz <= 0) {
    _ReportOutf("Failed to measure CPU clock speed.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  r = JLINKARM_SWO_GetCompatibleSpeeds(CPUSpeedHz, 0, &MaxSWOSpeedHz, 1);
  if (r < 1) {
    _ReportOutf("Failed to calculate suitable SWO speed.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf( "Performing SWO test with the following settings:\n"
          "Target: %s\n"
          "CPUSpeed: %d kHz\n"
          "SWOSpeed: %d kHz\n",
          DeviceInfo.sName,
          CPUSpeedHz / 1000,
          MaxSWOSpeedHz / 1000
        );
  //
  // R0 = 0
  // R1 = 0xE0000000 // ITM_STIM_U32
  //
  // int Cnt;
  // Cnt = 0;
  // do {
  //   while ((ITM_STIM_U32 & 1) == 0);
  //   ITM_STIM_U32 = Cnt++;
  // } while (1);
  //
  JLINKARM_WriteReg((ARM_REG)JLINKARM_CM3_REG_R0, 0);
  JLINKARM_WriteReg((ARM_REG)JLINKARM_CM3_REG_R1, 0xE0000000);
  JLINKARM_WriteReg((ARM_REG)JLINKARM_CM3_REG_R15, DeviceInfo.RAMAddr);
  JLINKARM_WriteMem(DeviceInfo.RAMAddr, sizeof(acSWORAMCode), acSWORAMCode);
  JLINKARM_SWO_EnableTarget(CPUSpeedHz, MaxSWOSpeedHz, JLINKARM_SWO_IF_UART, 1); // Enable stimulus port 0
  JLINKARM_GoIntDis();
  //
  // Read and analyze SWO data
  // What we expect:
  // Target sends a 32-bit counter value with full speed.
  // We have no syncs etc enabled so we expect to only get SWIT packets with 1 byte header and 4 bytes payload.
  // Everything else is an error.
  //
  pAlloc = malloc(0x100000);
  NumBytesInBuff = 0;
  pData = pAlloc;
  Cnt = 0;
  HasError = 0;
  _ReportOutf("Counter: %9d, Errors: %9d", Cnt, NumErrPackets);
  do {
    NumBytesRead = 0x100000 - NumBytesInBuff;
    JLINKARM_SWO_Read(pData + NumBytesInBuff, 0, &NumBytesRead);
    if (NumBytesRead > 0) {
      JLINKARM_SWO_Control(JLINKARM_SWO_CMD_FLUSH, &NumBytesRead);
      NumBytesInBuff += NumBytesRead;
    }
    //
    // Analyze buffer content
    //
    do {
      if (NumBytesInBuff < 5) {                   // Too less data to analyze in the buffer?
        if (NumBytesInBuff) {                     // Any incomplete packets in buffer? move them to the start of the buffer
          memcpy(pAlloc, pData, NumBytesInBuff);
        }
        pData = pAlloc;
        break;
      }
      //
      // We expect data from stimulus port 0 which means the header[7:3] must be 0.
      // We expect 32-bit items to be send by the target, so header[1:0] must be 3.
      //
      if (*pData != 0x3) {
        if (HasError == 0) {
          HasError = 1;
          NumErrPackets++;
          Cnt++;
        }
        pData += 1;
        NumBytesInBuff -= 1;
        continue;
      }
      //
      // Extract data and check if counter value matches the expected one.
      //
      Data = _Load32LE(pData + 1);
      if (HasError) {
        HasError = 0;
        Cnt = Data;
      }
      if (Data != Cnt) {
        HasError = 1;
        NumErrPackets++;
      }
      if (((Cnt % 10000) == 0) || (OldNumErrPackets != NumErrPackets)) {
        OldNumErrPackets = NumErrPackets;
        _ReportOutf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\bCounter: %11d, Errors: %11d", Cnt, NumErrPackets);

      }
      Cnt++;
      pData += 5;
      NumBytesInBuff -= 5;
    } while(_ExitTerminal == 0);
    _Sleep(2);  // Give other threads some time
  } while (_ExitTerminal == 0);
  _ReportOutf("\n");
  free(pAlloc);
  JLINKARM_SWO_DisableTarget(1);
  _ExitTerminal = 0;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       static code, HSS
*
**********************************************************************
*/
#define HSS_MAX_NUM_BLOCKS      (8)

static JLINK_HSS_MEM_BLOCK_DESC _HSS_aBlock[HSS_MAX_NUM_BLOCKS + 1];  // Last element will have <NumBytes> set to 0 to indicate end of list
static U32                      _HSS_IsRunning;

/*********************************************************************
*
*       _ExecHSSStart
*/
static int _ExecHSSStart(const char* s) {
  U32 Addr;
  U32 Period;
  int NumBlocks;
  int r;
  int i;
  //
  // Show capabilities
  //
  _HSS_IsRunning = 0;
  //
  // Parse parameters
  //
  Period         = 10000;
  NumBlocks      = 0;
  _HSS_IsRunning = 0;  // Initially, HSS is not running
  _EatWhite(&s);
  _ReportOutf("Syntax: HSSStart <Period_us> Block0 [Block1] [...]\n Blockn: <Addr> <NumBytes> <Flags>\n");
  if (*s == 0) {
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Parse period first
  //
  if (_ParseDec(&s, &Period)) {
    _ReportOutf("Syntax error.\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Parse <Addr>, <NumBytes>, <Flags> parameters
  //
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (*s == 0) {
    _ReportOutf("Syntax error.\n");
    return JLINK_ERROR_SYNTAX;
  }
  do {
    _EatWhite(&s);
    if (*s == 0) {
      break;
    }
    if (NumBlocks >= HSS_MAX_NUM_BLOCKS) {
      _ReportOutf("This command can only handle up to %d blocks\n", HSS_MAX_NUM_BLOCKS);
      return JLINK_ERROR_UNKNOWN;
    }
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &Addr)) {
      _ReportOutf("Syntax error.\n");
      return JLINK_ERROR_SYNTAX;
    }
    _HSS_aBlock[NumBlocks].Addr = Addr;
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &_HSS_aBlock[NumBlocks].NumBytes)) {
      _ReportOutf("Syntax error.\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &_HSS_aBlock[NumBlocks].Flags)) {
      _ReportOutf("Syntax error.\n");
      return JLINK_ERROR_SYNTAX;
    }
    NumBlocks++;
  } while (1);
  _HSS_aBlock[NumBlocks].NumBytes = 0;  // Indicate end of list
  _ReportOutf("Starting HSS with the following settings:\n  %d blocks\n", NumBlocks);
  for(i = 0; i < NumBlocks; i++) {
    _ReportOutf("    Block %d: %d bytes @ 0x%.8X\n", i, _HSS_aBlock[i].NumBytes, _HSS_aBlock[i].Addr);
  }
  _ReportOutf("  %d us period.\n", Period);
  //
  // Start HSS
  //
  r = JLINK_HSS_Start(&_HSS_aBlock[0], NumBlocks, Period, 0);
  if (r < 0) {
    _ReportOutf("Failed to start HSS. Error code: %d\n", r);
    return JLINK_ERROR_UNKNOWN;
  } else {
    _HSS_IsRunning = 1;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecHSSStop
*/
static int _ExecHSSStop(const char* s) {
  (void)s;
  JLINK_HSS_Stop();
  _HSS_IsRunning = 0;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecHSSRead
*/
static int _ExecHSSRead(const char* s) {
  int NumSamplesRead;
  U32 NumSamples;
  U32 AllocSize;
  U32 Tmp;
  U8* pData0;
  U8* pData;
  int r;
  int i;
  int NumBlocks;

  pData0     = NULL;
  pData      = NULL;
  r          = JLINK_NOERROR;
  NumSamples = 10;
  _ReportOutf("Syntax: HSSRead [<NumSamples>]\n");
  //
  // Early out if HSS is not running
  //
  if (_HSS_IsRunning == 0) {
    _ReportOutf("HSS is not running.\n");
    return JLINK_NOERROR;
  }
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (*s) {
    if (_ParseHex(&s, &NumSamples)) {
      _ReportOutf("Syntax error\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Determine NumBlocks of current config, AllocSize starts with 4 for the size of the timestamp value (4 bytes)
  //
  NumBlocks = 0;
  AllocSize = 4;
  do {
    if (_HSS_aBlock[NumBlocks].NumBytes == 0) {
      break;
    }
    AllocSize += _HSS_aBlock[NumBlocks].NumBytes;
    NumBlocks++;
  } while (1);
  _ReportOutf("HSS config: %d blocks.\n", NumBlocks);
  for (i = 0; i < NumBlocks; i++) {
    _ReportOutf("  Block %d, %d bytes.\n", i, _HSS_aBlock[i].NumBytes);
  }
  //
  // Allocate memory for requested number of samples
  //
  pData = malloc(NumSamples * AllocSize);
  if (pData == NULL) {
    _ReportOutf("Failed to allocate memory for HSS buffer.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = pData;
  NumSamplesRead = JLINK_HSS_Read(pData, NumSamples * AllocSize);
  if (NumSamplesRead < 0) {
    _ReportOutf("Error while reading HSS data.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  NumSamplesRead /= AllocSize;
  _ReportOutf("Read %d samples.\n", NumSamplesRead);
  //
  // Show data for each sample
  //
  if (NumSamplesRead) {
    do {
      //
      // Show Timestamp
      //
      Tmp = 4;
      _ReportOutf("Timestamp: ");
      do {
        _ReportOutf("%.2X ", *pData++);
      } while (--Tmp);
      _ReportOutf("\n");
      //
      // Show data for each block
      //
      for (i = 0; i < NumBlocks; i++) {
        _ReportOutf("Block %d (@0x%.8X): ", i, _HSS_aBlock[i].Addr);
        Tmp = _HSS_aBlock[i].NumBytes;
        do {
          _ReportOutf("%.2X ", *pData++);
        } while (--Tmp);
        _ReportOutf("\n");
      }
    } while (--NumSamplesRead);
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       static code, POWERTRACE
*
**********************************************************************
*/
/*********************************************************************
*
*       _ExecPowerTrace
*/
static int _ExecPowerTrace(const char* s) {
  JLINK_POWERTRACE_SETUP Setup;
  JLINK_POWERTRACE_CAPS  Caps;
  JLINK_POWERTRACE_DATA_ITEM * pData;
  JLINK_POWERTRACE_DATA_ITEM * pTemp;
  JLINK_POWERTRACE_CHANNEL_CAPS_IN  ChannelCapsIn;
  JLINK_POWERTRACE_CHANNEL_CAPS_OUT ChannelCapsOut;
  JLINKARM_SWO_START_INFO SWOStartInfo;
  const char* sErr;
  HANDLE hFile;
  U32 ChannelMask;
  int NumItemsRead;
  int NumChannelsActive;
  U32 NumItemsReadTotal;
  U8  ac[512];
  U8 acFile[MAX_PATH];
  int r;
  int i;
  U32 Data;

  hFile = INVALID_HANDLE_VALUE;
  pData = NULL;
  memset(&Setup, 0, sizeof(Setup));
  memset(&Caps, 0, sizeof(Caps));
  memset(&SWOStartInfo, 0, sizeof(SWOStartInfo));
  //
  // Select defaults for powertrace parameters
  //
  _UTIL_CopyString((char*)acFile, FILE_NAME_POWERTRACE_TEST, _SIZEOF(acFile));
  Setup.ChannelMask = 1;  //
  Setup.RefSelect   = POWERTRACE_REFC_NO_REFC;  // No reference counter
  Setup.SampleFreq  = 0;  // Max.
  Setup.EnableCond  = 0;  // Non-stop collecting of data
  //
  // Parse parameters
  //
  //
  // Para <LogFile>
  //
  _EatWhite(&s);
  if (*s) {
    _ParseString(&s, (char*)acFile, sizeof(acFile));
  }
  //
  // Para <ChannelMask>
  //
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    sErr = _ParseHex(&s, &Setup.ChannelMask);
    if (sErr) {
      _ReportOutf("ERROR: %s\n", sErr);
      r = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  //
  // Para <RefCountSel>
  //
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    sErr = _ParseHex(&s, (U32*)&Setup.RefSelect);
    if (sErr) {
      _ReportOutf("ERROR: %s\n", sErr);
      r = JLINK_ERROR_SYNTAX;
      goto Done;
    }
  }
  //
  // Check J-Link caps
  //
  if (JLINKARM_EMU_HasCapEx(JLINKARM_EMU_CAP_EX_POWERTRACE) == 0) {
    _ReportOutf("ERROR: POWERTRACE is not supported by connected J-Link.\n");
    r = JLINK_ERROR_SYNTAX;
    goto Done;
  }
  //
  // Print selected settings
  //
  _ReportOutf("---Selected settings---\n\nLogFile: %s\nChannelMask: 0x%.8X\nSampling frequency: %d %s\nReference counter selection: %d\n\n",
         acFile,
         Setup.ChannelMask,
         Setup.SampleFreq,
         Setup.SampleFreq ? "Hz" : "(Max)",
         Setup.RefSelect
        );
  //
  // Print generic J-Link POWERTRACE capabilities
  //
  Caps.SizeOfStruct = sizeof(JLINK_POWERTRACE_CAPS);
  JLINK_POWERTRACE_Control(JLINK_POWERTRACE_CMD_GET_CAPS, NULL, &Caps);
  _ReportOutf("---J-Link capabilities---\n\n");
  _ReportOutf("Channels supported by connected emulator:\n");
  ChannelMask = Caps.ChannelMask;
  i = 0;
  while (ChannelMask) {
    if (ChannelMask & 1) {
      _ReportOutf(" - %s%d\n", (i < 8) ? "Int" : "Ext", (i < 8) ? i : i - 8);
    }
    ChannelMask >>= 1;
    i++;
  }
  if ((Setup.ChannelMask & Caps.ChannelMask) != Setup.ChannelMask) {
    _ReportOutf("ERROR: Connected J-Link does not support selected channel mask.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  NumChannelsActive = 0;
  ChannelMask = Setup.ChannelMask;
  while (ChannelMask) {
    if (ChannelMask & 1) {
      NumChannelsActive++;
    }
    ChannelMask >>= 1;
  }
  //
  // Check if selected channels & frequency are supported by emulator
  //
  ChannelCapsIn.SizeOfStruct  = sizeof(JLINK_POWERTRACE_CHANNEL_CAPS_IN);
  ChannelCapsIn.ChannelMask = Setup.ChannelMask;
  ChannelCapsOut.SizeOfStruct = sizeof(JLINK_POWERTRACE_CHANNEL_CAPS_OUT);
  JLINK_POWERTRACE_Control(JLINK_POWERTRACE_CMD_GET_CHANNEL_CAPS, &ChannelCapsIn, &ChannelCapsOut);
  if (Setup.SampleFreq > (ChannelCapsOut.BaseFreq / ChannelCapsOut.MinDiv)) {
    _ReportOutf("Selected sampling frequency is not supported for selected setup, by connected J-Link.\n");
  }
  //
  // Setup J-Link for POWERTRACE
  //
  hFile = _OpenFile((char*)acFile, FILE_FLAG_WRITE | FILE_FLAG_CREATE | FILE_FLAG_TRUNC);   // Create logfile for POWERTRACE data
  if (hFile == INVALID_HANDLE_VALUE) {
    _ReportOutf("ERROR: Could not open file for writing.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  //
  // Setup emulator for POWERTRACE and print the setup information
  //
  Setup.SizeOfStruct = sizeof (JLINK_POWERTRACE_SETUP);
  if (Setup.SampleFreq == 0) {                                              // Max. frequency selected?
    Setup.SampleFreq = ChannelCapsOut.BaseFreq / ChannelCapsOut.MinDiv;
  }
  r = JLINK_POWERTRACE_Control(JLINK_POWERTRACE_CMD_SETUP, &Setup, NULL);
  if (r < 0) {
    _ReportOutf("ERROR: Unspecified error occurred while setting up POWERTRACE\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  _ReportOutf(" ChannelMask: 0x%.8X\n Sampling frequency: %d Hz\n RefValue select: %d\n EnableCond: %d\n", Setup.ChannelMask, r, Setup.RefSelect, Setup.EnableCond);
  UTIL_snprintf((char*)ac, UTIL_SIZEOF(ac), "POWERTRACE Test\n--Settings--\nChannelMask: 0x%.8X\nSampling frequency: %d Hz\nRefValue select: %d\nEnableCond: %d\n", Setup.ChannelMask, r, Setup.RefSelect, Setup.EnableCond);
  _WriteFile(hFile, ac, UTIL_strlen((const char*)ac));
  //
  // Select SWD as target interface in order to be able to use SWO
  // in case reference counter type 1 has been selected
  //
  if (Setup.RefSelect == POWERTRACE_REFC_SWO_NUMBYTES_TRANSMITTED) {
    if (JLINKARM_TIF_Select(JLINKARM_TIF_SWD) != 0) {
      _ReportOutf("SWO byte count selected as reference count but SWD/SWO is not supported by connected emulator.\n");
      r = JLINK_ERROR_UNKNOWN;
      goto Done;
    }
    JLINKARM_SetSpeed(4000);
    //
    // Reset target to get it in a defined state
    //
    JLINKARM_Reset();
    JLINKARM_WriteU32(0xE000EDFC, 0x01000000);    // Debug Exception and Monitor Control Register.
    JLINKARM_WriteU32(0xE0042004, 0x00000027);    //
    JLINKARM_WriteU32(0xE00400F0, 0x00000002);    // "Selected PIN Protocol Register": Select which protocol to use for trace output (2: SWO)
    Data = (72000000 / POWERTRACE_SWO_DEFAULT_SPEED) - 1;
    JLINKARM_WriteU32(0xE0040010, Data);          // Async Clock Prescaler Register (72/144 = 500 kHz)
    JLINKARM_WriteU32(0xE0000FB0, 0xC5ACCE55);    // ITM Lock Access Register, 0xC5ACCE55 enables more write access to Control Register 0xE00 :: 0xFFC. Invalid write removes write access
    JLINKARM_WriteU32(0xE0000E80, 0x0001000D);    // ITM Trace Control Register. Configures and controls ITM transfers
    JLINKARM_WriteU32(0xE0000E40, 0x0000000F);    // ITM Trace Privilege Register
    JLINKARM_WriteU32(0xE0000E00, 0x00000001);    // ITM Trace Enable Register. Enable tracing on stimulus ports. One bit per stimulus port.
    JLINKARM_WriteU32(0xE0001000, 0x400003FE);    // DWT Control Register
    JLINKARM_WriteU32(0xE0040304, 0x00000100);    // Formatter and Flush Control Register
    //
    // Start SWO
    //
    SWOStartInfo.SizeofStruct = sizeof(SWOStartInfo);
    SWOStartInfo.Interface    = JLINKARM_SWO_IF_UART;
    SWOStartInfo.Speed        = POWERTRACE_SWO_DEFAULT_SPEED;
    JLINKARM_SWO_Control(JLINKARM_SWO_CMD_START, &SWOStartInfo);
  }
  //
  // Allocate buffer for POWERTRACE data
  //
  pData = (JLINK_POWERTRACE_DATA_ITEM*)malloc(sizeof(JLINK_POWERTRACE_DATA_ITEM) * 500);
  if (pData == NULL) {
    _ReportOutf("ERROR: Could not allocate buffer for POWERTRACE data.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  //
  // Start POWERTRACE
  //
  NumItemsReadTotal = 0;
  _ReportOutf("Press any key to start POWERTRACE (press any key to stop)...");
  _getch();
  _ReportOutf("\n");
  JLINK_POWERTRACE_Control(JLINK_POWERTRACE_CMD_START, NULL, NULL);
  //
  // Let the CPU run
  //
  if (Setup.RefSelect == POWERTRACE_REFC_SWO_NUMBYTES_TRANSMITTED) {
    JLINKARM_Go();
  }
  //
  // Get POWERTRACE data periodically
  //
  while (1) {
    NumItemsRead = JLINK_POWERTRACE_Read(pData, 500);
    if (NumItemsRead < 0) {
      _ReportOutf("Error while reading POWERTRACE data.\n");
      break;
    }
    NumItemsReadTotal += NumItemsRead;
    _ReportOutf("\r%d items read total", NumItemsReadTotal);
    UTIL_snprintf((char*)ac, UTIL_SIZEOF(ac), "%d items read:\n", NumItemsRead);
    _WriteFile(hFile, ac, UTIL_strlen((const char*)ac));
    pTemp = pData;
    while (NumItemsRead) {
      ChannelMask = Setup.ChannelMask;
      i = 0;
      while (ChannelMask) {
        if (ChannelMask & 1) {
          UTIL_snprintf((char*)&ac[0], UTIL_SIZEOF(ac), "Channel %d: Data: %d, RefVal: 0x%.4X\n", i, pTemp->Data, pTemp->RefValue);
          _WriteFile(hFile, ac, UTIL_strlen((const char*)ac));
          pTemp++;
          NumItemsRead--;
        }
        i++;
        ChannelMask >>= 1;
      }
    }
    if (_kbhit() != 0) {
      _getch();
      break;
    }
    _Sleep(20);   // Give other threads some time to perform their action. This also allows our POWERTRACE thread in the DLL to collect some more data
  }
  //
  // Stop POWERTRACE
  //
  r = JLINK_POWERTRACE_Control(JLINK_POWERTRACE_CMD_STOP, NULL, NULL);
  if (r < 0) {
    _ReportOutf("\nFailed to stop POWERTRACE\n");
  } else {
    _ReportOutf("\nPOWERTRACE stopped!\n");
  }
  //
  // Stop SWO
  //
  if (Setup.RefSelect == POWERTRACE_REFC_SWO_NUMBYTES_TRANSMITTED) {
    JLINKARM_SWO_Control(JLINKARM_SWO_CMD_STOP, NULL);
    JLINKARM_SWO_Control(JLINKARM_SWO_CMD_FLUSH, NULL);
  }
  //
  // Close file handle and free POWERTRACE item buffer
  //
  r = JLINK_NOERROR;
Done:
  if (hFile != INVALID_HANDLE_VALUE) {
    _CloseFile(hFile);
  }
  if (pData) {
    free(pData);
  }
  return r;
}

/*********************************************************************
*
*       _ExecRawTrace
*/
static int _ExecRawTrace(const char* s) {
  (void)s;
  _ReportOutf("Command no longer available.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecEMUCOMWrite
*/
static int _ExecEMUCOMWrite(const char* s) {
  U8  abData[MAX_NUM_WRITE_ITEMS];
  U32 Channel;
  U32 NumBytes;
  int NumBytesWritten;
  U32 Data;

  if (_ParseHex(&s, &Channel)) {
    _ReportOutf("Syntax: ECWrite <Channel>, <Data>\n");
    return JLINK_ERROR_SYNTAX;
  }
  NumBytes = 0;
  while (1) {
    if (NumBytes > MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d bytes.\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &Data)) {
      if (NumBytes == 0) {
        _ReportOutf("Syntax: ECWrite <Channel>, <Data>\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    abData[NumBytes] = Data & 0xFF;
    NumBytes++;
  }
  if (JLINKARM_EMU_COM_IsSupported() <= 0) {
    _ReportOutf("EMUCOM is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  NumBytesWritten = JLINKARM_EMU_COM_Write(Channel, NumBytes, &abData[0]);
  if (NumBytesWritten >= 0) {
    _ReportOutf("%d bytes written successfully to channel 0x%.2X.\n", NumBytesWritten, Channel);
    return JLINK_NOERROR;
  } else if (NumBytesWritten == (int)JLINKARM_EMUCOM_ERR_CHANNEL_NOT_SUPPORTED) {
    _ReportOutf("Channel 0x%.2X is not supported by connected emulator.\n", Channel);
  } else {
    _ReportOutf("Failed to write to channel 0x%.2X.\n", Channel);
  }
  return JLINK_ERROR_UNKNOWN;
}

/*********************************************************************
*
*       _ExecEMUCOMRead
*/
static int _ExecEMUCOMRead(const char* s) {
  U32 Channel;
  U32 NumBytes;
  int NumBytesRead;
  U8* pData0;
  U8* pData;
  int Offset = 0;
  int r = JLINK_ERROR_UNKNOWN;

  if (_ParseHex(&s, &Channel)) {
    _ReportOutf("Syntax: ECRead <Channel>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &NumBytes)) {
    _ReportOutf("Syntax: ECRead <Channel>, <NumBytes>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (NumBytes > 0x10000) {
    _ReportOutf("NumBytes should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (JLINKARM_EMU_COM_IsSupported() <= 0) {
    _ReportOutf("EMUCOM is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  pData0 = pData = malloc(NumBytes);
  if (pData0 == NULL) {
    _ReportOutf("Failed to allocate memory.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  NumBytesRead = JLINKARM_EMU_COM_Read(Channel, NumBytes, pData);
  if (NumBytesRead >= 0) {
    _ReportOutf("%d bytes read successfully from channel 0x%.2X:\n", NumBytesRead, Channel);
    while (NumBytesRead > 0) {
      int NumBytesPerLine;
      _ReportOutf("%.8X = ", Offset);
      NumBytesPerLine = (NumBytesRead > 16) ? 16 : NumBytesRead;
      NumBytesRead   -= NumBytesPerLine;
      Offset         += NumBytesPerLine;
      for (; NumBytesPerLine > 0; NumBytesPerLine--) {
        _ReportOutf("%.2X ", *pData++);
      }
      _ReportOutf("\n");
    }
    r = JLINK_NOERROR;
  } else if (NumBytesRead == 0) {
    _ReportOutf("No bytes available on channel 0x%.2X.\n", Channel);
    r = JLINK_NOERROR;
  } else if (NumBytesRead == (int)JLINKARM_EMUCOM_ERR_CHANNEL_NOT_SUPPORTED) {
    _ReportOutf("Channel 0x%.2X is not supported by connected emulator.\n", Channel);
  } else if ((NumBytesRead & 0xFF000000) == JLINKARM_EMUCOM_ERR_BUFFER_TOO_SMALL) {
    _ReportOutf("Buffer too small. %d bytes available for reading from channel 0x%.2X.\n", NumBytesRead & 0xFFFFFF, Channel);
  } else {
    _ReportOutf("Failed to read from channel 0x%.2X.\n", Channel);
  }
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecEMUCOMIsSupported
*/
static int _ExecEMUCOMIsSupported(const char* s) {
  int r;

  (void)s;
  r = JLINKARM_EMU_COM_IsSupported();
  if (r > 0) {
    _ReportOutf("EMUCOM is supported by connected emulator.\n");
  } else if (r == 0) {
    _ReportOutf("EMUCOM is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  } else {
    _ReportOutf("Failed to determine if EMUCOM is supported by emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecFileWrite
*/
static int _ExecFileWrite(const char* s) {
  char acEmuFile[MAX_PATH];
  char acHostFile[MAX_PATH];
  char acErr[512];
  SYS_FILE_HANDLE hFile;
  U32 ReplaceEmuFile;
  U32 Offset;
  U32 NumBytes;
  U32 NumBytesRead;
  U32 NumBytesWritten;
  U32 NumBytesAtOnce;
  U8* pData;
  int r;
  //
  // Parse parameters
  //
  Offset   = 0;
  NumBytes = 0;
  _EatWhite(&s);
  if ((*s == 0) || (*s == '\r') || (*s == '\n') || (*s == ',')) {
    _ReportOutf("Syntax: fwrite <EmuFile> <HostFile> [<Offset> [<NumBytes>]]\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ParseString(&s, acEmuFile, sizeof(acEmuFile));
  _EatWhite(&s);
  _ParseString(&s, acHostFile, sizeof(acHostFile));
  _EatWhite(&s);
  ReplaceEmuFile = 1;
  if (*s) {
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &Offset)) {
      _ReportOutf("Syntax: fwrite <EmuFile> <HostFile> [<Offset> [<NumBytes>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    ReplaceEmuFile = 0;
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (_ParseHex(&s, &NumBytes)) {
        _ReportOutf("Syntax: fwrite <EmuFile> <HostFile> [<Offset> [<NumBytes>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  //
  // Check if file I/O is supported
  //
  if ((JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_FILE_IO) == 0) {
    _ReportOutf("File I/O is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  if (ReplaceEmuFile) {  // If <NumBytes> and <Offset> were not specified by the user, delete the file before writing to it.
    JLINKARM_EMU_FILE_Delete(acEmuFile);
  }
  //
  // Read from source and write to destination file
  //
  acErr[0] = 0;
  NumBytesWritten = 0;
  hFile = SYS_FILE_Open(acHostFile, SYS_FILE_FLAG_READ | SYS_FILE_FLAG_SHARE_READ);
  if (hFile != SYS_INVALID_HANDLE) {
    if (NumBytes == 0) {
      NumBytes = SYS_FILE_GetSize(hFile);
    }
    JLINKARM_EMU_FILE_Write(acEmuFile, NULL, NumBytes, 0);
    pData = (U8*)malloc(MAX_BLOCK_SIZE_FILE_IO);
    if (pData) {
      SYS_FILE_Seek(hFile, Offset, SYS_FILE_SEEK_BEGIN);               // Apply <Offset> to host file (determine read start point)
      while (NumBytes) {
        NumBytesAtOnce = MIN(NumBytes, MAX_BLOCK_SIZE_FILE_IO);        // Do not exceed intermediate buffer size
        NumBytesRead   = SYS_FILE_Read(hFile, pData, NumBytesAtOnce);
        if (NumBytesRead == -1) {
          _UTIL_CopyString(acErr, "ERROR: Could not read from source file.\n", _SIZEOF(acErr));
          break;
        }
        if (NumBytesRead != NumBytesAtOnce) {
          _UTIL_CopyString(acErr, "ERROR: Could not read from source file.\n", _SIZEOF(acErr));
          break;
        }
        r = JLINKARM_EMU_FILE_Write(acEmuFile, pData, Offset, NumBytesAtOnce);
        if (r < 0) {
          _UTIL_CopyString(acErr, "ERROR: Could not write to destination file.\n", _SIZEOF(acErr));
          break;
        }
        NumBytesWritten += r;
        Offset          += r;
        NumBytes        -= r;
        if (r != (int)NumBytesAtOnce) {
          break;
        }
      }
      free(pData);
    } else {
      _UTIL_CopyString(acErr, "ERROR: Could not allocate file buffer.\n", _SIZEOF(acErr));
    }
    SYS_FILE_Close(hFile);
  } else {
    _UTIL_CopyString(acErr, "ERROR: Could not open source file.\n", _SIZEOF(acErr));
  }
  //
  // Display result
  //
  if (*acErr) {
    _ReportOutf(acErr);
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("%d bytes written successfully.\n", NumBytesWritten);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecFileRead
*/
static int _ExecFileRead(const char* s) {
  char acEmuFile[MAX_PATH];
  char acHostFile[MAX_PATH];
  char acErr[512];
  HANDLE hFile;
  U32 Offset   = 0;
  U32 NumBytes = 0;
  U32 NumBytesRead;
  U32 NumBytesWritten;
  U32 NumBytesAtOnce;
  U8* pData;
  int r;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if ((*s == 0) || (*s == '\r') || (*s == '\n') || (*s == ',')) {
    _ReportOutf("Syntax: fread <EmuFile> <HostFile> [<Offset> [<NumBytes>]]\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ParseString(&s, acEmuFile, sizeof(acEmuFile));
  _EatWhite(&s);
  _ParseString(&s, acHostFile, sizeof(acHostFile));
  _EatWhite(&s);
  if (*s) {
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &Offset)) {
      _ReportOutf("Syntax: fread <EmuFile> <HostFile> [<Offset> [<NumBytes>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (*s) {
      if (_ParseHex(&s, &NumBytes)) {
        _ReportOutf("Syntax: fread <EmuFile> <HostFile> [<Offset> [<NumBytes>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  //
  // Check if file I/O is supported
  //
  if ((JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_FILE_IO) == 0) {
    _ReportOutf("File I/O is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Read from source and write to destination file
  //
  if (NumBytes == 0) {
    NumBytes = JLINKARM_EMU_FILE_GetSize(acEmuFile);
    if (NumBytes == 0xFFFFFFFF) {
      _ReportOutf("ERROR: Source file does not exist.\n");
      return JLINK_ERROR_UNKNOWN;
    }
  }
  acErr[0] = 0;
  NumBytesRead = 0;
  hFile = _OpenFile(acHostFile, FILE_FLAG_WRITE | FILE_FLAG_CREATE | FILE_FLAG_TRUNC);
  if (hFile != INVALID_HANDLE_VALUE) {
    pData = (U8*)malloc(MAX_BLOCK_SIZE_FILE_IO);
    if (pData) {
      while (NumBytes) {
        NumBytesAtOnce = MIN(NumBytes, MAX_BLOCK_SIZE_FILE_IO);
        r = JLINKARM_EMU_FILE_Read(acEmuFile, pData, Offset, NumBytesAtOnce);
        if (r < 0) {
          _UTIL_CopyString(acErr, "ERROR: Could not read from source file.\n", _SIZEOF(acErr));
          break;
        }
        NumBytesWritten = _WriteFile(hFile, pData, r);
        if (NumBytesWritten == -1) {
          _UTIL_CopyString(acErr, "ERROR: Could not write to destination file.\n", _SIZEOF(acErr));
          break;
        }
        if (NumBytesWritten != (U32)r) {
          _UTIL_CopyString(acErr, "ERROR: Could not write to destination file.\n", _SIZEOF(acErr));
          break;
        }
        NumBytesRead += r;
        Offset       += r;
        NumBytes     -= r;
        if (r != (int)NumBytesAtOnce) {
          break;
        }
      }
      free(pData);
    } else {
      _UTIL_CopyString(acErr, "ERROR: Could not allocate file buffer.\n", _SIZEOF(acErr));
    }
    _CloseFile(hFile);
  } else {
    _UTIL_CopyString(acErr, "ERROR: Could not open destination file.\n", _SIZEOF(acErr));
  }
  //
  // Display result
  //
  if (*acErr) {
    _ReportOutf(acErr);
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("%d bytes read successfully.\n", NumBytesRead);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecFileShow
*/
static int _ExecFileShow(const char* s) {
  char acFile[MAX_PATH];
  U32 Offset   = 0;
  U32 NumBytes = 0x100;
  int NumBytesRead;
  U8* pData;
  U8* pData0;
  int r = JLINK_NOERROR;
  int ShowText = 0;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if ((*s == 0) || (*s == '\r') || (*s == '\n') || (*s == ',')) {
    _ReportOutf("Syntax: fshow <FileName> [<Offset> [<NumBytes>]]\n");
    return JLINK_ERROR_SYNTAX;
  }
  if ((*s == '-') && (*(s+1) == 'a')) {
    s += 2;
    ShowText = 1;
    _EatWhite(&s);
  }
  _ParseString(&s, acFile, sizeof(acFile));
  _EatWhite(&s);
  if (*s) {
    if (*s == ',') {
      s++;
    }
    if (_ParseHex(&s, &Offset)) {
      _ReportOutf("Syntax: fshow <FileName> [<Offset> [<NumBytes>]]\n");
      return JLINK_ERROR_SYNTAX;
    }
    _EatWhite(&s);
    if (*s) {
      if (*s == ',') {
        s++;
      }
      if (_ParseHex(&s, &NumBytes)) {
        _ReportOutf("Syntax: fshow <FileName> [<Offset> [<NumBytes>]]\n");
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  if (NumBytes > 0x10000) {
    _ReportOutf("NumBytes should be <= 0x10000\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Check if file I/O is supported
  //
  if ((JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_FILE_IO) == 0) {
    _ReportOutf("File I/O is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Read data
  //
  pData = (U8*)malloc(NumBytes);
  pData0 = pData;
  NumBytesRead = JLINKARM_EMU_FILE_Read(acFile, pData, Offset, NumBytes);
  if (NumBytesRead < 0) {
    _ReportOutf("ERROR: Could not read file.\n");
    r = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  //
  // Display data
  //
  if (JLINKARM_HasError() == 0) {
    _ReportOutf("%d bytes read successfully:\n", NumBytesRead);
    if (ShowText) {
      *(pData + NumBytes - 1) = 0;
      _ReportOutf((const char*)pData);
    } else {
      while (NumBytesRead > 0) {
        int NumBytesPerLine;
        _ReportOutf("%.8X = ", Offset);
        NumBytesPerLine = (NumBytesRead > 16) ? 16 : NumBytesRead;
        NumBytesRead -= NumBytesPerLine;
        Offset       += NumBytesPerLine;
        for (; NumBytesPerLine > 0; NumBytesPerLine--) {
          _ReportOutf("%.2X ", *pData++);
        }
        _ReportOutf("\n");
      }
    }
  } else {
    r = JLINK_ERROR_UNKNOWN;
  }
Done:
  free(pData0);
  return r;
}

/*********************************************************************
*
*       _ExecFileDelete
*/
static int _ExecFileDelete(const char* s) {
  char acFile[MAX_PATH];
  int r;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if ((*s == 0) || (*s == '\r') || (*s == '\n') || (*s == ',')) {
    _ReportOutf("Syntax: fdelete <FileName>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ParseString(&s, acFile, sizeof(acFile));
  //
  // Check if file I/O is supported
  //
  if ((JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_FILE_IO) == 0) {
    _ReportOutf("File I/O is not supported by connected emulator.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Delete file
  //
  r = JLINKARM_EMU_FILE_Delete(acFile);
  if (r != 0) {
    _ReportOutf("ERROR: Could not delete file.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("File deleted successfully.\n");
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecFileSize
*/
static int _ExecFileSize(const char* s) {
  char acFile[MAX_PATH];
  int FileSize;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if ((*s == 0) || (*s == '\r') || (*s == '\n') || (*s == ',')) {
    _ReportOutf("Syntax: fsize <FileName>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ParseString(&s, acFile, sizeof(acFile));
  //
  // Check if file I/O is supported
  //
  if ((JLINKARM_GetEmuCaps() & JLINKARM_EMU_CAP_FILE_IO) == 0) {
    _ReportOutf("File I/O is not supported by connected probe.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Read file size
  //
  FileSize = JLINKARM_EMU_FILE_GetSize(acFile);
  if (FileSize < 0) {
    _ReportOutf("ERROR: Could not read file size.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("Size of file: %d bytes\n", FileSize);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecFileList
*/
static int _ExecFileList(const char* s) {
  char acBuffer[0x400];
  char acFile[MAX_PATH];
  int r;
  //
  // Parse parameters
  //
  acFile[0] = 0;
  _EatWhite(&s);
  if (*s) {
    _ParseString(&s, acFile, sizeof(acFile));
  }
  //
  // Read and display directory
  //
  r = JLINKARM_EMU_FILE_GetList(acFile, acBuffer, sizeof(acBuffer));
  if (r < 0) {
    if (r == JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED) {
      _ReportOutf("This File I/O command is not supported by the connected probe.\n");
    } else {
      _ReportOutf("Unknown error (%d) occurred.\n", r);
    }
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Firmware has more bytes to send than our buffer can handle?
  //
  if (r > sizeof(acBuffer)) {
    U8* pTmp;

    pTmp = malloc(r);
    if (pTmp == NULL) {
      _ReportOutf("Failed to allocate temporary buffer (on PC side) for directory tree.\n");
      return JLINK_ERROR_UNKNOWN;
    }
    JLINKARM_EMU_FILE_GetList(acFile, (char*)pTmp, r);    // Very unlikely that it suddenly fails when called again. Just assume that it will work
    _ReportOutf("%s\n", pTmp);
    free(pTmp);
  } else {
    _ReportOutf("%s\n", acBuffer);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecVolInfo
*/
static int _ExecVolInfo(const char* s) {
  JLINK_FUNC_EMU_FILE_GET_VOLUME_INFO* pf;
  JLINK_EMU_FILE_VOLUME_INFO VolInfo;
  int r;
  int Result;
  U64 VolSize;
  U64 VolUsed;

  (void)s;
  Result = JLINK_NOERROR;
  pf = (JLINK_FUNC_EMU_FILE_GET_VOLUME_INFO*)JLINK_GetpFunc(JLINK_IFUNC_EMU_FILE_GET_VOLUME_INFO);
  if (pf) {
    memset(&VolInfo, 0, sizeof(VolInfo));
    VolInfo.SizeofStruct = sizeof(VolInfo);
    r = pf(&VolInfo);
    if (r >= 0) {
      VolSize = VolInfo.VolumeSizeLo + VolInfo.VolumeSizeHi;
      VolUsed = VolInfo.NumBytesUsedLo + VolInfo.NumBytesUsedHi;
      VolSize >>= 10;   // Convert to [KB]
      VolUsed >>= 10;
      _ReportOutf("Volume info: %d KB of %d KB used (%d KB free)\n", (U32)VolUsed, (U32)VolSize, (U32)(VolSize - VolUsed));
    } else {
      if (r == JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED) {
        _ReportOutf("This File I/O command is not supported by the connected probe.\n");
      } else {
        _ReportOutf("Unknown error (%d) occurred.\n", r);
      }
      Result = JLINK_ERROR_UNKNOWN;
    }
  } else {
    _ReportOutf("API call not supported by this DLL version.\n");
    Result = JLINK_ERROR_UNKNOWN;
  }
  return Result;
}

/*********************************************************************
*
*       _ExecFileInfo
*/
static int _ExecFileInfo(const char* s) {
  JLINK_EMU_FILE_INFO FileInfo;
  JLINK_FUNC_EMU_FILE_GET_FILE_INFO* pf;
  char acPath[256];
  int Result;
  int r;

  Result = JLINK_NOERROR;
  _ParseString(&s, acPath, sizeof(acPath));
  pf = (JLINK_FUNC_EMU_FILE_GET_FILE_INFO*)JLINK_GetpFunc(JLINK_IFUNC_EMU_FILE_GET_FILE_INFO);
  if (pf == NULL) {
    _ReportOutf("API call not supported by this DLL version.\n");
    Result = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  r = pf(acPath, &FileInfo);
  if (r >= 0) {
    _ReportOutf("File info:\n");
    _ReportOutf("  Size:          %u\n", FileInfo.FileSize);
    _ReportOutf("  IsSubdir:      %s\n", (FileInfo.Attributes & JLINK_EMU_FILE_ATT_DIR) ? "Yes" : "No" );
    _ReportOutf("  Last modified: %u-%u-%u %u:%u:%u (YY-MM-DD HH:MM:SS)\n", FileInfo.Year, FileInfo.Month, FileInfo.Day, FileInfo.Hour, FileInfo.Minute, FileInfo.Second);
  } else {
    if (r == JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED) {
      _ReportOutf("This File I/O command is not supported by the connected probe.\n");
    } else {
      _ReportOutf("Unknown error (%d) occurred.\n", r);
    }
    Result = JLINK_ERROR_UNKNOWN;
  }
Done:
  return Result;
}

/*********************************************************************
*
*       _ExecFileEntries
*/
static int _ExecFileEntries(const char* s) {
  JLINK_EMU_FILE_INFO* paFileInfo;
  JLINK_EMU_FILE_INFO* pFileInfo;
  JLINK_FUNC_EMU_FILE_GET_DIR_ENTRIES* pf;
  U32 MaxNumInfo;
  U32 NumInfo;
  U32 i;
  char acPath[256];
  int Result;
  int r;

  Result = JLINK_NOERROR;
  MaxNumInfo = 32;
  paFileInfo = (JLINK_EMU_FILE_INFO*)SYS_MEM_Alloc(MaxNumInfo * sizeof(JLINK_EMU_FILE_INFO));
  _ParseString(&s, acPath, sizeof(acPath));
  pf = (JLINK_FUNC_EMU_FILE_GET_DIR_ENTRIES*)JLINK_GetpFunc(JLINK_IFUNC_EMU_FILE_GET_DIR_ENTRIES);
  if (pf == NULL) {
    _ReportOutf("API call not supported by this DLL version.\n");
    Result = JLINK_ERROR_UNKNOWN;
    goto Done;
  }
  r = pf(acPath, paFileInfo, MaxNumInfo);
  if (r >= 0) {
    NumInfo = (U32)r;
    _ReportOutf("Number of entries found: %u\n", NumInfo);
    if ((U32)r > MaxNumInfo) {
      NumInfo = MaxNumInfo;
      _ReportOutf("Showing first %u ones\n", NumInfo);
    }
    for (i = 0; i < NumInfo; i++) {
      pFileInfo = (paFileInfo + i);
      _ReportOutf("  Entry:         %s\n", pFileInfo->acFile);
      _ReportOutf("  Size:          %u\n", pFileInfo->FileSize);
      _ReportOutf("  IsSubdir:      %s\n", (pFileInfo->Attributes & JLINK_EMU_FILE_ATT_DIR) ? "Yes" : "No" );
      _ReportOutf("  Last modified: %u-%u-%u %u:%u:%u (YY-MM-DD HH:MM:SS)\n", pFileInfo->Year, pFileInfo->Month, pFileInfo->Day, pFileInfo->Hour, pFileInfo->Minute, pFileInfo->Second);
    }
  } else {
    if (r == JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED) {
      _ReportOutf("This File I/O command is not supported by the connected probe.\n");
    } else {
      _ReportOutf("Unknown error (%d) occurred.\n", r);
    }
    Result = JLINK_ERROR_UNKNOWN;
  }
Done:
  SYS_MEM_Free(paFileInfo);
  return Result;
}

/*********************************************************************
*
*       _ExecFileSecureArea
*/
static int _ExecFileSecureArea(const char* s) {
  char acCommand[64];
  char acErr[128];
  int r;

  _ReportOutf("Syntax: SecureArea <Operation>\n<Operation>: Create / Remove\n");
  //
  // Parse parameters
  //
  acCommand[0] = 0;
  _EatWhite(&s);
  if (*s) {
    _ParseString(&s, acCommand, sizeof(acCommand));
  }
  //
  // Perform operation based on parameters parsed
  //
  if (_JLinkstricmp("Create", acCommand) == 0) {
    _ReportOutf("Creating secure area...");
    r = JLINKARM_ExecCommand("SetEmuSecureArea 0x4000000", acErr, sizeof(acErr));
    if (r < 0) {
      _ReportOutf("Failed with error code %d\n", r);
      return JLINK_ERROR_UNKNOWN;
    } else {
      _ReportOutf("O.K.\n");
    }
  } else if (_JLinkstricmp("Remove", acCommand) == 0) {
    _ReportOutf("Removing secure area...");
    r = JLINKARM_ExecCommand("SetEmuSecureArea 0", acErr, sizeof(acErr));
    if (r < 0) {
      _ReportOutf("Failed with error code %d\n", r);
      return JLINK_ERROR_UNKNOWN;
    } else {
      _ReportOutf("O.K.\n");
    }
  } else {
    _ReportOutf("Invalid value for <Operation>\n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecShowEmuStatus
*/
static int _ExecShowEmuStatus(const char* s) {
  U32 NumBytes;
  
  (void)s;
  NumBytes = JLINKARM_EMU_GetMaxMemBlock();
  _ReportOutf("Emulator has currently %d bytes of free memory.\n", NumBytes);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecIndicator
*/
static int _ExecIndicator(const char* s) {
  JLINKARM_INDICATOR_CTRL Ctrl;
  U32 Id            = 0;
  U32 Override      = 0;
  U32 InitialOnTime = 0;
  U32 OnTime        = 0;
  U32 OffTime       = 0;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (_ParseDec(&s, &Id)) {
    _ReportOutf("Syntax: indi <Id> <Override> <InitialOnTime> <OnTime> <OffTime>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &Override)) {
      _ReportOutf("Syntax: indi <Id> <Override> <InitialOnTime> <OnTime> <OffTime>\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &InitialOnTime)) {
      _ReportOutf("Syntax: indi <Id> <Override> <InitialOnTime> <OnTime> <OffTime>\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &OnTime)) {
      _ReportOutf("Syntax: indi <Id> <Override> <InitialOnTime> <OnTime> <OffTime>\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  _EatWhite(&s);
  if (*s) {
    if (_ParseDec(&s, &OffTime)) {
      _ReportOutf("Syntax: indi <Id> <Override> <InitialOnTime> <OnTime> <OffTime>\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  //
  // Update indicator
  //
  Ctrl.IndicatorId    = (U16)Id;
  Ctrl.Override       = (U16)Override;
  Ctrl.InitialOnTime  = (U16)InitialOnTime;
  Ctrl.OnTime         = (U16)OnTime;
  Ctrl.OffTime        = (U16)OffTime;
  JLINKARM_INDICATORS_SetState(1, &Ctrl);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecVCatch
*/
static int _ExecVCatch(const char* s) {
  U32 Value = 0;
  //
  // Parse parameters
  //
  _EatWhite(&s);
  if (_ParseHex(&s, &Value)) {
    _ReportOutf("Syntax: VCatch <Value>\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Write vector catch
  //
  JLINKARM_WriteVectorCatch(Value);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecASCIIView
*/
static int _ExecASCIIView(const char* s) {
  U32 v;
  const char* sErr;

  _EatWhite(&s);
  UTIL_ParseChar(&s, '=');  // Optional
  _EatWhite(&s);
  v = 0;
  sErr = _ParseDec(&s, &v);
  if (sErr) {
    _ReportOutf("Error: %s\n", sErr);
    _ReportOutf("Syntax: ASCIIView = <0/1>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _ASCIIViewDisable = v ? 0 : 1;  // Negated logic for variable
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecUpdateBTL
*/
static int _ExecUpdateBTL(const char* s) {
  JLINK_FUNC_UPDATE_BTL* pf;
  int r;

  (void)s;
  r = JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED;
  pf = (JLINK_FUNC_UPDATE_BTL*)JLINK_GetpFunc(JLINK_IFUNC_UPDATE_BTL);
  if (pf) {
    r = pf();
  }
  switch (r) {
  case 0:                                     _ReportOutf("O.K.\n"); break;
  case 1:                                     _ReportOutf("O.K., BTL already up-to-date\n"); break;
  case JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED:   _ReportOutf("ERROR, not supported by connected J-Link\n"); break;
  default:                                    _ReportOutf("ERROR, unknown error\n"); break;
  }
  if (r >= 0) {
    r = JLINK_NOERROR;
  }
  return r;
}

/*********************************************************************
*
*       _ExecSetDebugUnitBlockMask
*/
static int _ExecSetDebugUnitBlockMask(const char* s) {
  U32 Mask;
  I32 Type;

  if (_ParseHex(&s, (U32*)&Type)) {
    _ReportOutf("Syntax: sdubm <Type> <Mask>\n");
    return JLINK_ERROR_SYNTAX;
  }
  _EatWhite(&s);
  if (*s == ',') {
    s++;
  }
  if (_ParseHex(&s, &Mask)) {
    _ReportOutf("Syntax: sdubm <Type> <Mask>\n");
    return JLINK_ERROR_SYNTAX;
  }
  JLINKARM_SetDebugUnitBlockMask(Type, Mask);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ShowMOE
*/
static void _ShowMOE(void) {
  JLINKARM_MOE_INFO Info;
  JLINKARM_WP_INFO WPInfo;
  int NumWPs;
  int i;
  int r;

  if (JLINKARM_IsHalted() <= 0) {
    _ReportOutf("CPU is not halted.\n");
    return;
  }
  r = JLINKARM_GetMOEs(&Info, 1);
  if (r > 0) {
    if (Info.HaltReason == JLINKARM_HALT_REASON_DBGRQ) {
      _ReportOutf("CPU halted because DBGRQ was asserted.\n");
    } else if (Info.HaltReason == JLINKARM_HALT_REASON_VECTOR_CATCH) {
      _ReportOutf("CPU halted due to vector catch.\n");
    } else if (Info.HaltReason == JLINKARM_HALT_REASON_DATA_BREAKPOINT) {
      if (Info.Index >= 0) {
        _ReportOutf("CPU halted due to data breakpoint unit %d match.\n", Info.Index);
        WPInfo.SizeOfStruct = sizeof(JLINKARM_WP_INFO);
        NumWPs = JLINKARM_GetWPInfoEx(-1, &WPInfo);
        for (i = 0; i < NumWPs; i++) {
          JLINKARM_GetWPInfoEx(i, &WPInfo);
          if (WPInfo.UnitMask && (1 << Info.Index)) {
            _ReportOutf("Unit %d was used for WP with handlle 0x%.4X.\n", Info.Index, WPInfo.Handle);
            return;
          }
        }
      } else {
        _ReportOutf("CPU halted due to data breakpoint match.\n");
      }
    } else if (Info.HaltReason == JLINKARM_HALT_REASON_CODE_BREAKPOINT) {
      if (Info.Index >= 0) {
        _ReportOutf("CPU halted due to code breakpoint unit %d match.\n", Info.Index);
      } else {
        _ReportOutf("CPU halted due to code breakpoint match.\n");
      }
    } else {
      _ReportOutf("CPU halted for unknown reason.");
    }
  }
}

/*********************************************************************
*
*       _ExecShowMOE
*/
static int _ExecShowMOE(const char* s) {
  char ac[256];

  _EatWhite(&s);
  if (_CompareCmd(&s, "On") == 0) {
    JLINKARM_ExecCommand("EnableMOEHandling", &ac[0], sizeof(ac));
  } else if (_CompareCmd(&s, "Off") == 0) {
    JLINKARM_ExecCommand("DisableMOEHandling", &ac[0], sizeof(ac));
  } else {
    _ShowMOE();
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGetCounters
*/
static int _ExecGetCounters(const char* s) {
  U32 aCnt[4];

  (void)s;
  JLINKARM_EMU_GetCounters(15, &aCnt[0]);
  _ReportOutf("aCnt[0] = 0x%.8X\n", aCnt[0]);
  _ReportOutf("aCnt[1] = 0x%.8X\n", aCnt[1]);
  _ReportOutf("aCnt[2] = 0x%.8X\n", aCnt[2]);
  _ReportOutf("aCnt[3] = 0x%.8X\n", aCnt[3]);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTest
*/
static int _ExecTest(const char* s) {
#ifdef _DEBUG
  U32 Count = 0;

  while (Count < 1) {
    if (!JLINKARM_IsOpen()) {
      JLINKARM_Open();
    }
    if (JLINKARM_IsOpen()) {
      JLINKARM_Close();
    }
    Count++;
  }
#endif
  (void)s;
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _uA2mAPrec1
*/
static void _uA2mAPrec1(int uA, char* pBuf, U32 BufSize) {
  int mA;
  int IsNeg;

  IsNeg = (uA < 0) ? 1 : 0;
  if (uA < 0) {
    IsNeg = 1;
    uA *= -1;     // Convert to positive value
  } else {
    IsNeg = 0;
  }
  mA = uA / 1000;
  uA = uA % 1000;
  uA = (uA + 50) / 100;
  UTIL_snprintf(pBuf, BufSize, "%s%d.%d", IsNeg ? "-" : "", mA, uA);
}

/*********************************************************************
*
*       _ExecCalibrate
*/
static int _ExecCalibrate(const char* s) {
  int IAvg;   // uA
  int IMin;   // uA
  int IMax;   // uA
  int IDiff;  // uA
  int IOffsNew;  // uA
  int IOffsOld;  // uA
  int ICur;   // uA
  int Tmp;
  char aacFloat[4][16];
  char ac[256];

  (void)s;
  _ReportOutf("Please disconnect emulator from target.\n");
  _ReportOutf("Press any key to start the calibration...\n");
  _getch();
  _ReportOutf("Calibrating...");
  JLINKARM_ExecCommand("SupplyPower = 1", &ac[0], sizeof(ac));
//  _Sleep(1000);
  //
  // Measure the actual current
  //
  _MeasureITarget(&IAvg, &IMin, &IMax);
  IDiff = abs(IMax - IMin);
  //
  // Fail if the current fluctuates too much
  //
  if (IDiff > (2 * ITARGET_MAX_FLUCTUATION)) {
    UTIL_snprintf(ac, UTIL_SIZEOF(ac), "\nERROR: Current fluctuates too much Min=%duA Max=%duA Diff=%duA!\n", IMin, IMax, IDiff);
    _ReportOut(ac);
    goto OnError;
  }
  JLINKARM_ReadEmuConfigMem((U8*)&IOffsOld, ITARGET_OFFSET_ADDR, sizeof(IOffsOld));
  //
  // MSB is set if the target was *NOT* calibrated
  //
  if (IOffsOld & ITARGET_OFFSET_FLAG) {
    IOffsNew = 0;
  } else {
    _ReportOutf("Emulator already calibrated.\n");
    //
    // Extend the sign
    //
    IOffsNew = IOffsOld;
    IOffsNew <<= 1;
    IOffsNew >>= 1;
  }
  IOffsNew += IAvg;
  Tmp = IOffsNew & (~ITARGET_OFFSET_FLAG);  // Upper bit cleared means for J-Link: Offset is valid
  if (Tmp != IOffsOld) {
    _ReportOutf("Emulator calibrated.\n");
    //
    // Clear MSB to indicate that the offset is valid
    //
    JLINKARM_WriteEmuConfigMem((const U8*)&Tmp, ITARGET_OFFSET_ADDR, sizeof(Tmp));
//    _Sleep(1000);
  } else {
    _ReportOutf("No re-calibration necessary.\n");
  }
  //
  // Measure again the power to check if the calibration was OK
  //
  _MeasureITarget(&ICur, NULL, NULL);
  if (abs(ICur) > ITARGET_MAX_FLUCTUATION) {
    UTIL_snprintf(ac, UTIL_SIZEOF(ac), "\nERROR: Current deviates too much from zero Min=%duA Max=%duA Cur=%duA!\n", IMin, IMax, ICur);
    _ReportOut(ac);
    goto OnError;
  }
  //
  // Sign extend IOffs again to be able to perform a correct output of the variable
  //
  _uA2mAPrec1(IMin, &aacFloat[0][0], UTIL_SIZEOF(aacFloat[0]));
  _uA2mAPrec1(IMax, &aacFloat[1][0], UTIL_SIZEOF(aacFloat[1]));
  _uA2mAPrec1(IAvg, &aacFloat[2][0], UTIL_SIZEOF(aacFloat[2]));
  _uA2mAPrec1(IOffsNew, &aacFloat[3][0], UTIL_SIZEOF(aacFloat[3]));
  UTIL_snprintf(ac, UTIL_SIZEOF(ac), "Min=%smA Max=%smA Avg=%smA Offs=%smA...DONE\n", &aacFloat[0][0], &aacFloat[1][0], &aacFloat[2][0], &aacFloat[3][0]);
  _ReportOut(ac);
OnError:
  JLINKARM_ExecCommand("SupplyPower = 0", &ac[0], sizeof(ac));
  return JLINK_NOERROR;
}


/*********************************************************************
*
*       _ShowAndSelectEmu
*
*  Function description
*    Searches for J-Links which are connected to the host PC
*    and prints a list of all emulators which have been found.
*    Depending on the value of "SelectEmu" the user can connect
*    to one of the emulators or just the list is shown.
*
*  Return value
*    0  O.K.
*   -1  Generic error
*   -2  Syntax error
*/
static int _ShowAndSelectEmu(const char * sIn, char SelectEmu) {
  int r;
  int i;
  char NeedDealloc;
  char ac[128];
  const char * s;
  U32 Index;
  U32 Interface;
  JLINKARM_EMU_CONNECT_INFO * paConnectInfo;
  JLINKARM_EMU_CONNECT_INFO aConnectInfo[50];
  unsigned WasConnectedToJLink;
  unsigned WasConnectedToTarget;
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;
  Interface = 0;
  do {
    _ParseString(&sIn, ac, sizeof(ac));
    if (ac[0] == 0) {
      if (Interface == 0) {
        Interface = JLINKARM_HOSTIF_USB;   // If no interfaces are selected, select USB only.
      }
      break;
    } else if (_JLinkstricmp(ac, "usb") == 0) {
      Interface |= JLINKARM_HOSTIF_USB;
    } else if (_JLinkstricmp(ac, "ip") == 0) {
      Interface |= JLINKARM_HOSTIF_IP;
    } else {
      _ReportOutf("Unknown interface.\n");
      Interface = 0;
      break;
    }
  } while (1);
  if (Interface == 0) {
    return -2;  // Syntax error
  }
  //
  // Request emulator list
  //
  r = JLINKARM_EMU_GetList(Interface, &aConnectInfo[0], COUNTOF(aConnectInfo));
  //
  // Allocate memory for emulator info buffer if local buffer is not big enough
  //
  NeedDealloc = 0;
  if (r > COUNTOF(aConnectInfo)) {
    paConnectInfo = (JLINKARM_EMU_CONNECT_INFO*)malloc(r * sizeof(JLINKARM_EMU_CONNECT_INFO));
    if (paConnectInfo == NULL) {
      _ReportOutf("Failed to allocate memory for emulator info buffer.\n");
      return -1;
    }
    NeedDealloc = 1;
    JLINKARM_EMU_GetList(Interface, paConnectInfo, r);
  } else {
    paConnectInfo = &aConnectInfo[0];
  }
  for (i = 0; i < r; i++) {
    if ((paConnectInfo + i)->Connection == JLINKARM_HOSTIF_USB) {
      _ReportOutf("J-Link[%d]: Connection: USB, Serial number: %d, ProductName: %s\n", i, (paConnectInfo + i)->SerialNumber, (paConnectInfo + i)->acProduct);
    } else {
      _ReportOutf("J-Link[%d]: Connection: IP,  Serial number: %d, IP: %d.%d.%d.%d, ProductName: %s\n", i, (paConnectInfo + i)->SerialNumber, (paConnectInfo + i)->aIPAddr[0], (paConnectInfo + i)->aIPAddr[1], (paConnectInfo + i)->aIPAddr[2], (paConnectInfo + i)->aIPAddr[3], (paConnectInfo + i)->acProduct);
    }
  }
  if (SelectEmu) {
    //
    // Select one emulator from the list in order to connect to it
    //
    r = SYS_ConsoleGetString("Select emulator index: ", ac, sizeof(ac));
    if (r < 0) {
      _CommanderSettings.CloseRequested = 1;
      return 0;
    }
    s = ac;
    s = _ParseDec(&s, &Index);
    if (s == NULL) {
      //
      // Disconnect from target + J-Link
      // Update commander settings
      // Reconnect to J-Link + target
      //
      WasConnectedToJLink = pCommanderSettings->Status.ConnectedToJLink;
      WasConnectedToTarget = pCommanderSettings->Status.ConnectedToTarget;
      if (WasConnectedToTarget) {
        _DisconnectFromTarget(pCommanderSettings);
      }
      if (WasConnectedToJLink) {
        _DisconnectFromJLink(pCommanderSettings);
      }
      if ((paConnectInfo + Index)->Connection == JLINKARM_HOSTIF_USB) {
        //
        // USB selected?
        //
        pCommanderSettings->HostIF      = JLINKARM_HOSTIF_USB;
        pCommanderSettings->acIPAddr[0] = 0;
        if ((paConnectInfo + Index)->SerialNumber == 123456) {     // Older J-Links enumerate with this serial number
          pCommanderSettings->EmuSN = (paConnectInfo + Index)->USBAddr;
        } else {
          pCommanderSettings->EmuSN = (paConnectInfo + Index)->SerialNumber;
        }
        pCommanderSettings->IsSetEmuSN  = 1;
      } else {
        //
        // IP selected?
        //
        UTIL_snprintf(ac, UTIL_SIZEOF(ac), "%d.%d.%d.%d", (paConnectInfo + Index)->aIPAddr[0], (paConnectInfo + Index)->aIPAddr[1], (paConnectInfo + Index)->aIPAddr[2], (paConnectInfo + Index)->aIPAddr[3]);
        pCommanderSettings->HostIF = JLINKARM_HOSTIF_IP;
        pCommanderSettings->EmuSN    = 0;
        pCommanderSettings->IsSetEmuSN  = 1;
        _UTIL_CopyString(&pCommanderSettings->acIPAddr[0], ac, _SIZEOF(pCommanderSettings->acIPAddr));
      }
      r = _ConnectToJLink(pCommanderSettings);
      if (WasConnectedToTarget && (r >= 0)) {
        r = _ConnectToTarget(pCommanderSettings);
      }
    } else {
      _ReportOutf("%s\n", s);
    }
  }
  if (NeedDealloc) {
    free(paConnectInfo);
  }
  return 0;
}

/*********************************************************************
*
*       _ExecShowEmuList
*
*  Function description
*    Searches for J-Links which are connected to the host PC
*    and gives the user a list of all J-Links which have been found,
*    from which he can select the one he wants to talk to.
*
*/
static int _ExecShowEmuList(const char* sIn) {
  int r;

  r = _ShowAndSelectEmu(sIn, 0);
  if (r < 0) {
    if (r == -1) {
      return JLINK_ERROR_UNKNOWN;
    } else if (r == -2) {
      _ReportOutf("Syntax: ShowEmuList [<Interface0> <Interface1> ...]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSelectEmuFromList
*
*  Function description
*    Searches for J-Links which are connected to the host PC
*    and gives the user a list of all J-Links which have been found,
*    from which he can select the one he wants to talk to.
*
*/
static int _ExecSelectEmuFromList(const char* sIn) {
  int r;

  r = _ShowAndSelectEmu(sIn, 1);
  if (r < 0) {
    if (r == -1) {
      return JLINK_ERROR_UNKNOWN;
    } else if (r == -2) {
      _ReportOutf("Syntax: selemu [<Interface0> <Interface1> ...]\n");
      return JLINK_ERROR_SYNTAX;
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecCDCSetHookFuncs
*
*  Function description
*    Set custom handlers for CDC functions on the J-Link.
*    We get an image from the PC which contains a signature as well
*    as the hook functions for the CDC Tx & Rx functionality
*/
static int _ExecCDCSetHookFuncs(const char * sIn) {
  char ac[MAX_PATH];
  HANDLE hFile;
  U32 FileSize;
  char* pBuffer;
  int r;

  ac[0] = 0;
  _ParseString(&sIn, ac, sizeof(ac));
  hFile = _OpenFile(ac, FILE_FLAG_READ | FILE_FLAG_SHARE_READ);
  if (hFile == INVALID_HANDLE_VALUE) {
    _ReportOutf("Could not open file for reading.\n");
    return JLINK_ERROR_SYNTAX;
  }
  //
  // Read in image
  //
  FileSize = _GetFileSize(hFile);
  pBuffer  = (char*) malloc(FileSize);
  if (pBuffer == NULL) {
    _ReportOutf("Could not allocate memory for image file.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  _ReportOutf("Reading image... [%s]\n", ac);
  _ReadFile(hFile, pBuffer, FileSize);
  _CloseFile(hFile);
  _ReportOutf("O.K.\n");
  //
  // Transfer image to J-Link
  //
  _ReportOutf("Downloading image to J-Link...");
  r = JLINKARM_CDC_SetHookFuncs((const U8*)pBuffer, FileSize);
  if (r >= 0) {
    _ReportOutf("O.K.\n");
  } else {
    switch(r) {
    case -2: _ReportOutf("Not enough RAM left on J-Link for image download.\n");                            break;
    case -3: _ReportOutf("Invalid data signature found in image.\n");                                       break;
    case JLINK_ERR_EMU_FEATURE_NOT_SUPPORTED: _ReportOutf("Feature not supported by connected J-Link.\n");  break;
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecSetBMI
*
*  Function description
*    Set BMI mode of Infineon XMC1000 device.
*
*  Syntax
*    SetBMI <Mode>
*    Valid values for <Mode>:
*      0 ASC Bootstrap Load Mode (ASC_BSL)
*      1 User Mode (Productive)
*      2 User Mode (Debug) SWD0
*      3 User Mode (Debug) SWD1
*      4 User Mode (Debug) SPD0
*      5 User Mode (Debug) SPD1
*      6 User Mode (HAR) SWD0
*      7 User Mode (HAR) SWD1
*      8 User Mode (HAR) SPD0
*      9 User Mode (HAR) SPD1
*/
static int _ExecSetBMI(const char * sIn) {
  int r;
  U32 BMIMode;
  //
  // Parse parameter(s) and perform plausibility checks.
  //
  r = 0;
  if (*sIn == 0) {
    r = -1;
    goto Done;
  }
  if (_ParseDec(&sIn, &BMIMode)) {
    r = -1;
    goto Done;
  }
  //
  // Set BMI mode
  //
  _ReportOutf("Setting BMI mode %d...", BMIMode);
  r = JLINKARM_BMI_Set(BMIMode);
  if (r != 0) {
    _ReportOutf("Failed. ErrorCode: %d\n", r);
  } else {
    _ReportOutf("O.K.\n");
  }
Done:
  if (r < 0) {
    _ReportOutf( "Syntax: SetBMI <Mode>\n"
            "Valid values for <Mode>:\n"
            "  0 ASC Bootstrap Load Mode (ASC_BSL)\n"
            "  1 User Mode (Productive)\n"
            "  2 User Mode (Debug) SWD0\n"
            "  3 User Mode (Debug) SWD1\n"
            "  4 User Mode (Debug) SPD0\n"
            "  5 User Mode (Debug) SPD1\n"
            "  6 User Mode (HAR) SWD0\n"
            "  7 User Mode (HAR) SWD1\n"
            "  8 User Mode (HAR) SPD0\n"
            "  9 User Mode (HAR) SPD1\n"
          );
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGetBMI
*
*  Function description
*    Get BMI mode of Infineon XMC1000 device.
*
*  Syntax
*    GetBMI
*    Valid modes:
*      0 ASC Bootstrap Load Mode (ASC_BSL)
*      1 User Mode (Productive)
*      2 User Mode (Debug) SWD0
*      3 User Mode (Debug) SWD1
*      4 User Mode (Debug) SPD0
*      5 User Mode (Debug) SPD1
*      6 User Mode (HAR) SWD0
*      7 User Mode (HAR) SWD1
*      8 User Mode (HAR) SPD0
*      9 User Mode (HAR) SPD1
*/
static int _ExecGetBMI(const char * sIn) {
  int r;
  U32 BMIMode;
  
  (void)sIn;
  //
  // Get BMI mode
  //
  _ReportOutf("Current BMI mode: ");
  r = JLINKARM_BMI_Get(&BMIMode);
  if (r != 0) {
    _ReportOutf("Error, ErrorCode: %d.\n", r);
  } else {
    _ReportOutf("%d.\n", BMIMode);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGPIOGetProps
*/
static int _ExecGPIOGetProps(const char* s) {
  JLINK_EMU_GPIO_DESC aDesc[32];
  int i, NumDesc;
  int r;
  
  (void)s;
  //
  // Get BMI mode
  //
  r = JLINK_EMU_GPIO_GetProps(aDesc, COUNTOF(aDesc));
  if (r <= 0) {
    _ReportOutf("Connected emulator does not support GPIO commands.\n");
    return JLINK_NOERROR;
  }
  NumDesc = MIN(r, COUNTOF(aDesc));
  _ReportOutf("%d GPIOs are supported:\n", r);
  for (i = 0; i < NumDesc; i++) {
    _ReportOutf("%d: %s\n", i, aDesc[i].acName);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGPIOSetState
*/
static int _ExecGPIOSetState(const char* s) {
  U8  aPort  [MAX_NUM_WRITE_ITEMS];
  U8  aState [MAX_NUM_WRITE_ITEMS];
  U8  aResult[MAX_NUM_WRITE_ITEMS];
  U32 NumPorts, i;
  U32 v;

  NumPorts = 0;
  do {
    if (NumPorts > MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d items\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseDec(&s, &v)) {
      if (NumPorts == 0) {
        _ReportOutf("Syntax: GPIOSetState <Port0> <State0> [<Port1> <State1>...]\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    aPort[NumPorts] = v;
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseDec(&s, &v)) {
      _ReportOutf("Syntax: GPIOSetState <Port0> <State0> [<Port1> <State1>...]\n");
      return JLINK_ERROR_SYNTAX;
    }
    aState[NumPorts] = v;
    NumPorts++;
  } while (1);
  JLINK_EMU_GPIO_SetState(aPort, aState, aResult, NumPorts);
  for (i = 0; i < NumPorts; i++) {
    _ReportOutf("%d -> Port %d (State: %d)\n", aState[i], aPort[i], aResult[i]);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecGPIOGetState
*/
static int _ExecGPIOGetState(const char* s) {
  U8  aPort  [MAX_NUM_WRITE_ITEMS];
  U8  aResult[MAX_NUM_WRITE_ITEMS];
  U32 NumPorts, i;
  U32 v;

  NumPorts = 0;
  do {
    if (NumPorts > MAX_NUM_WRITE_ITEMS) {
      _ReportOutf("This command can only handle up to %d items\n", MAX_NUM_WRITE_ITEMS);
      return JLINK_ERROR_UNKNOWN;
    }
    _EatWhite(&s);
    if (*s == ',') {
      s++;
    }
    if (_ParseDec(&s, &v)) {
      if (NumPorts == 0) {
        _ReportOutf("Syntax: GPIOGetState <Port0> [<Port1>...]\n");
        return JLINK_ERROR_SYNTAX;
      }
      break;
    }
    aPort[NumPorts] = v;
    NumPorts++;
  } while (1);
  JLINK_EMU_GPIO_GetState(aPort, aResult, NumPorts);
  for (i = 0; i < NumPorts; i++) {
    _ReportOutf("Port %d (Read: %d)\n", aPort[i], aResult[i]);
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecLicenseAdd
*
*  Return value
*    1:   O.K.,  license already exist
*    0:   O.K.,  license was added successfully
*   -1:   Error, unspecified error occurred
*   -2:   Error, could not read or write config area
*   -3:   Error, not enough space for new license
*   -4:   Error, no license given
*/
static int _ExecLicenseAdd(const char* s) {
  int r;

  _EatWhite(&s);
  if (*s == 0) {
    _ReportOutf("Syntax: license add <LicName>\n");
    return -1;
  }
  r = JLINK_EMU_AddLicense(s);
  //
  // Check result
  //
  switch (r) {
  case  0:  _ReportOutf("License \"%s\" added successfully.\n", s);         break;
  case  1:  _ReportOutf("License \"%s\" already exists.\n", s);             break;
  case -1:  _ReportOutf("Failed to add license \"%s\".\n", s);              break;
  case -2:  _ReportOutf("Failed to add License \"%s\".\n", s);              break;
  case -3:  _ReportOutf("Not enough space to add license \"%s\".\n", s);    break;
  case -4:  _ReportOutf("No license string has been commited\"%s\".\n", s); break;
  default:  _ReportOutf("Unknown error (Error code: %d)\n", r);             break;
  }
  return r;
}

/*********************************************************************
*
*       _ExecLicenseErase
*
*  Return value
*    0:   O.K.
*   -1:   Error
*/
static int _ExecLicenseErase(const char* s) {
  int r;

  (void)s;
  r = JLINK_EMU_EraseLicenses();
  if (r < 0) {
    _ReportOutf("Failed to erase licenses.\n");
    return -1;                                    // Error, could not write config area
  }
  _ReportOutf("All licenses erased successfully.\n");
  return JLINK_NOERROR;                           // OK, licenses erased successfully
}

/*********************************************************************
*
*       _ExecLicenseShow
*/
static int _ExecLicenseShow(const char* s) {
  U8  ac[0x50 + 0x400];  // Enough space to support old J-Links (80 bytes in config for Installed lics) and new ones (additional 1024 bytes available for storage)
  int r;
  //
  // Show built-in licenses
  //
  (void)s;
  r = JLINK_GetAvailableLicense((char*)&ac[0], sizeof(ac));
  if (r >= 0) {
    _ReportOutf("Built-in licenses: %s\n", ac);
  } else {
    _ReportOutf("Could not determine available licenses.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  //
  // Show installable licenses
  //
  r = JLINK_EMU_GetLicenses((char*)ac, sizeof(ac));
  switch (r) {
  case 0:
    _ReportOutf("No installable licenses.\n");
    return JLINK_NOERROR;
  case -1:
    _ReportOutf("Failed to get licenses.\n");
    return JLINK_ERROR_UNKNOWN;
  case -2:
    _ReportOutf("Read config data is not supported by connected J-Link.\n");
    return JLINK_NOERROR;
  case sizeof(ac):
    r--;
    break;
  default:
    break;
  }
  ac[r] = 0;
  r--;
  if (ac[r] == ',') {
    ac[r] = 0;
  }
  _ReportOutf("Installable licenses: %s\n", ac);
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecRTTRead
*/
static int _ExecRTTRead(const char* s) {
  char ac[256];
  int r;

  (void)s;
  r = JLINK_RTTERMINAL_Read(0, ac, sizeof(ac) - 1);
  if (r > 0) {
    ac[r] = 0;
    _ReportOutf("RTT: %s\n", ac);
  } else if (r == 0) {
    _ReportOutf("No more data in real time terminal.\n");
  } else {
    _ReportOutf("Could not read real time terminal.\n");
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecExitOnError
*/
static int _ExecExitOnError(const char* sIn) {
  U32 r;
  _ParseDec(&sIn, &r);
  switch (r) {
  case 0:
    _ExitOnErrorLvl = 0;
    _ReportOutf("J-Link Commander will no longer exit on Error\n");
    break;
  case 1:
    _ReportOutf("J-Link Commander will now exit on Error\n");
    _ExitOnErrorLvl = JLINK_ERROR_SYNTAX;
    break;
  default:
  _ReportOutf("Syntax: ExitonError <1|0>\n");
  return JLINK_ERROR_SYNTAX;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecTestPINOverride
*/
static int _ExecTestPINOverride(const char* s) {
  JLINK_FUNC_PIN_OVERRIDE* pfPinOverride;
  U32 aMode[8];
  U32 aState[8];

  (void)s;
  pfPinOverride = (JLINK_FUNC_PIN_OVERRIDE*)JLINK_GetpFunc(JLINK_IFUNC_PIN_OVERRIDE);
  if (pfPinOverride) {
    aMode[0] = 1;
    aMode[1] = 1;
    aMode[2] = 1;
    aMode[3] = 1;
    aMode[4] = 1;
    aMode[5] = 1;
    aMode[6] = 1;
    aMode[7] = 5;
    pfPinOverride(&aMode[0], &aState[0]);
  }
  return JLINK_NOERROR;
}

#define JLINK_IFUNC_PIN_GET_CAPS    3
typedef int STDCALL JLINK_FUNC_PIN_GET_CAPS(U32* paCaps);                                                // JLINK_IFUNC_PIN_GET_CAPS

/*********************************************************************
*
*       _ExecGetPinCaps
*/
static int _ExecGetPinCaps(const char* s) {
  JLINK_FUNC_PIN_GET_CAPS* pfPINGetCaps;
  U32 aCaps[8];
  int i;

  (void)s;
  pfPINGetCaps = (JLINK_FUNC_PIN_GET_CAPS*)JLINK_GetpFunc(JLINK_IFUNC_PIN_GET_CAPS);
  if (pfPINGetCaps) {
    pfPINGetCaps(&aCaps[0]);
    for (i = 0; i < 8; i++) {
      _ReportOutf("Pin %d: 0x%x \n", i, aCaps[i]);
    }
  } else {
    _ReportOutf("Function not defined. \n");
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecCDCWrite
*/
static int _ExecCDCWrite(const char* s) {
  int r;
  int len;

  r = 0;
  _EatWhite(&s); //Write start at first char != whitespace
  len = UTIL_strlen(s);
  if (len) {
    r = JLINKARM_CDC_Write((const U8*)s, len);
  } else {
    _ReportOutf("Syntax: CDCWrite <StringToWrite>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (r != len) {
    _ReportOutf("Error while writing data. Error: %d.\n", r);
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecCDCRead
*/
static int _ExecCDCRead(const char* s) {
  char acIn[256];
  int r;

  (void)s;
  r = JLINKARM_CDC_Read((U8*)acIn, sizeof(acIn) - 1);
  if ((r >= 0) && (r < sizeof(acIn))) {
    acIn[r] = 0;
    _ReportOutf("%s\n", acIn);
  } else {
    _ReportOutf("Error while reading data. Error: %d.\n", r);
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecCDCSetBaudrate
*/
static int _ExecCDCSetBaudrate(const char* s) {
  int r;
  U32 Baudrate;
  const char* ps;

  r  = 0;
  ps = _ParseDec(&s, &Baudrate); //Write start at first char != whitespace
  if (ps == NULL) {
    r = JLINKARM_CDC_SetBaudrate(Baudrate);
  } else {
    _ReportOutf("Syntax: CDCBaudRate <Baudrate>\n");
    return JLINK_ERROR_SYNTAX;
  }
  if (r < 0) {
    _ReportOutf("Could not set Baudrate. Error: %d.\n", r);
    return JLINK_ERROR_UNKNOWN;
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecMRU
*/
static int _ExecMRU(const char* s) {
  char ac[512];
  char* p;
  int r;
  JLINK_FUNC_MRU_GETLIST* pFunc;

  (void)s;
  pFunc = (JLINK_FUNC_MRU_GETLIST*)JLINK_GetpFunc(JLINK_IFUNC_MRU_GETLIST);
  if (pFunc) {
    r = pFunc(JLINK_MRU_GROUP_DEVICE, ac, sizeof(ac));
    if (r >= 0) {
      _ReportOutf("Most recently used devices:\n");
      p = ac;
      r = 0;
      while ((*p != 0) && (r < 10)) {
        _ReportOutf("#%d: %s\n", r, p);
        p += UTIL_strlen(p) + 1;
        r++;
      }
    }
  }
  return JLINK_NOERROR;
}

/*********************************************************************
*
*       _ExecQuitCommander
*/
static int _ExecQuitCommander(const char* s) {
  _SETTINGS* pCommanderSettings;

  (void)s;
  pCommanderSettings = &_CommanderSettings;
  pCommanderSettings->CloseRequested = 1;
  return 0;
}

/*********************************************************************
*
*       _ExecConnectToTarget
*
*  Function description
*    Establish connection to target device by specifying device, target interface etc.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _ExecConnectToTarget(const char* s) {
  _SETTINGS* pCommanderSettings;

  pCommanderSettings = &_CommanderSettings;
  (void)s;
  return _ConnectToTarget(pCommanderSettings);
}

/*********************************************************************
*
*       Command list public
*/
static const _COMMAND_INFO _aCmd[] = {
//  pfCmd                      sLongName              sShortName            sInfo                    sSyntax                  NeedsJLinkConnection   NeedsTargetConnection
//
// Basic commands
//
  { _ExecShowCommands,         "?",                   "?",                  INFO_GETINFO,            SYNTAX_GETINFO,          0,                     0,    }
 ,{ _ExecQuitCommander,        "Exit",                "Exit",               INFO_EXIT,               SYNTAX_EXIT,             0,                     0,    }
 ,{ _ExecExitOnError,          "ExitOnError",         "EoE",                INFO_EXITONERROR,        SYNTAX_EXITONERROR,      0,                     0,    }
 ,{ _ExecSleep,                "Sleep",               "Sleep",              INFO_SLEEP,              SYNTAX_SLEEP,            0,                     0,    }
 ,{ _ExecLog,                  "Log",                 "Log",                INFO_LOG,                SYNTAX_LOG,              1,                     0,    }
 ,{ _ExecExportDeviceList,     "ExpDevList",          "ExpDevList",         INFO_EXPDEVLIST,         SYNTAX_EXPDEVLIST,       0,                     0,    } // Does not do anything with a J-Link, so we do not need a J-Link or target connection
 ,{ _ExecExportDeviceListXML,  "ExpDevListXML",       "ExpDevListXML",      INFO_EXPDEVLISTXML,      SYNTAX_EXPDEVLISTXML,    0,                     0,    } // Does not do anything with a J-Link, so we do not need a J-Link or target connection
//
// Configuration - J-Link
//
 ,{ _ExecSelectUSB,            "USB",                 "USB",                INFO_USB,                SYNTAX_USB,              0,                     0,    }
 ,{ _ExecSelectIP,             "IP",                  "IP",                 INFO_IP,                 SYNTAX_IP,               0,                     0,    }
 ,{ _ExecSelectEmuFromList,    "SelectProbe",         "SelPrb",             INFO_SELECTPROBE,        SYNTAX_SELECTPROBE,      0,                     0,    }
 ,{ _ExecShowEmuList,          "ShowEmuList",         "ShowEmuList",        INFO_SHOWEMULIST,        SYNTAX_SHOWEMULIST,      0,                     0,    }
 ,{ _ExecPower,                "Power",               "Power",              INFO_POWER,              SYNTAX_POWER,            1,                     0,    }
 ,{ _ExecVTREF,                "VTREF",               "VTREF",              INFO_VTREF,              SYNTAX_VTREF,            1,                     0,    }
 ,{ _ExecVCOM,                 "VCOM",                "VCOM",               INFO_VCOM,               SYNTAX_VCOM,             1,                     0,    }
 ,{ _ExecShowFirmwareInfo,     "ShowFWInfo",          "F",                  INFO_SHOWFWINFO,         SYNTAX_SHOWFWINFO,       1,                     0,    }
 ,{ _ExecShowHWStatus,         "ShowHWStatus",        "St",                 INFO_SHOWHWSTATUS,       SYNTAX_SHOWHWSTATUS,     1,                     0,    }
 ,{ _ExecIPAddress,            "IPAddr",              "IPAddr",             INFO_IPADDR,             SYNTAX_IPADDR,           1,                     0,    }
 ,{ _ExecGWAddress,            "GWAddr",              "GWAddr",             INFO_GWADDR,             SYNTAX_GWADDR,           1,                     0,    }
 ,{ _ExecDNSAddress,           "DNSAddr",             "DNSAddr",            INFO_DNSADDR,            SYNTAX_DNSADDR,          1,                     0,    }
 ,{ _ExecShowConfig,           "ShowConf",            "Conf",               INFO_SHOWCONF,           SYNTAX_SHOWCONF,         1,                     0,    }
 ,{ _ExecCalibrate,            "Calibrate",           "Calib",              INFO_CALIBRATE,          SYNTAX_CALIBRATE,        1,                     0,    }
//
// Configuration - Target (CPU)
//
 ,{ _ExecConnectToTarget,      "Connect",             "Con",                INFO_CONNECT,            SYNTAX_CONNECT,          1,                     0,    }
 ,{ _ExecDevice,               "Device",              "Device",             INFO_DEVICE,             SYNTAX_DEVICE,           1,                     0,    }
 ,{ _ExecSelectInterface,      "SelectInterface",     "SI",                 INFO_SELECTINTERFACE,    SYNTAX_SELECTINTERFACE,  1,                     0,    }
 ,{ _ExecSetSpeed,             "Speed",               "Speed",              INFO_SPEED,              SYNTAX_SPEED,            1,                     0,    } 
 ,{ _ExecSetLittleEndian,      "LE",                  "LE",                 INFO_LE,                 SYNTAX_LE,               1,                     0,    }
 ,{ _ExecSetBigEndian,         "BE",                  "BE",                 INFO_BE,                 SYNTAX_BE,               1,                     0,    }
//
// Debugging
//
 ,{ _ExecHalt,                 "Halt",                "H",                  INFO_HALT,               SYNTAX_HALT,             1,                     1,    }
 ,{ _ExecIsHalted,             "IsHalted",            "IH",                 INFO_ISHALTED,           SYNTAX_ISHALTED,         1,                     1,    }
 ,{ _ExecWaitHalt,             "WaitHalt",            "WH",                 INFO_WAITHALT,           SYNTAX_WAITHALT,         1,                     1,    }
 ,{ _ExecGo,                   "Go",                  "G",                  INFO_GO,                 SYNTAX_GO,               1,                     1,    }
 ,{ _ExecReset,                "Reset",               "R",                  INFO_RESET,              SYNTAX_RESET,            1,                     1,    }
 ,{ _ExecResetEx,              "ResetX",              "RX",                 INFO_RESETX,             SYNTAX_RESETX,           1,                     1,    }
 ,{ _ExecResetSetType,         "RSetType",            "Rst",                INFO_RSETTYPE,           SYNTAX_RSETTYPE,         1,                     1,    }
 ,{ _ExecStep,                 "Step",                "S",                  INFO_STEP,               SYNTAX_STEP,             1,                     1,    }
 ,{ _ExecIdentifySCSRLen,      "IS",                  "IS",                 INFO_IS,                 SYNTAX_IS,               1,                     1,    }
 ,{ _ExecMeasureSCLen,         "MS",                  "MS",                 INFO_MS,                 SYNTAX_MS,               1,                     1,    }
 ,{ _ExecShowRegs,             "Regs",                "Regs",               INFO_REGS,               SYNTAX_REGS,             1,                     1,    }
 ,{ _ExecReadReg,              "RReg",                "RReg",               INFO_RREG,               SYNTAX_RREG,             1,                     1,    }
 ,{ _ExecWriteReg,             "WReg",                "WReg",               INFO_WREG,               SYNTAX_WREG,             1,                     1,    }
 ,{ _ExecShowMOE,              "MoE",                 "MoE",                INFO_MOE,                SYNTAX_MOE,              1,                     1,    }
 ,{ _ExecSetBP,                "SetBP",               "SetBP",              INFO_SETBP,              SYNTAX_SETBP,            1,                     1,    }
 ,{ _ExecClrBP,                "ClearBP",             "ClrBP",              INFO_CLEARBP,            SYNTAX_CLEARBP,          1,                     1,    }
 ,{ _ExecSetWP,                "SetWP",               "SetWP",              INFO_SETWP,              SYNTAX_SETWP,            1,                     1,    }
 ,{ _ExecClrWP,                "ClearWP",             "ClrWP",              INFO_CLEARWP,            SYNTAX_CLEARWP,          1,                     1,    }
 ,{ _ExecVCatch,               "VCatch",              "VC",                 INFO_VCATCH,             SYNTAX_VCATCH,           1,                     1,    }
 ,{ _ExecSetPC,                "SetPC",               "SetPC",              INFO_SETPC,              SYNTAX_SETPC,            1,                     1,    }
 ,{ _ExecReadAP,               "ReadAP",              "ReadAP",             INFO_READAP,             SYNTAX_READAP,           1,                     0,    }
 ,{ _ExecWriteAP,              "WriteAP",             "WriteAP",            INFO_WRITEAP,            SYNTAX_WRITEAP,          1,                     0,    }
 ,{ _ExecReadDP,               "ReadDP",              "ReadDP",             INFO_READDP,             SYNTAX_READDP,           1,                     0,    }
 ,{ _ExecWriteDP,              "WriteDP",             "WriteDP",            INFO_WRITEDP,            SYNTAX_WRITEDP,          1,                     0,    }
 ,{ _ExecReadCP15Ex,           "RCP15Ex",             "RCE",                INFO_RCP15EX,            SYNTAX_RCP15EX,          1,                     1,    }
 ,{ _ExecWriteCP15Ex,          "WCP15Ex",             "WCE",                INFO_WCP15EX,            SYNTAX_WCP15EX,          1,                     1,    }
 ,{ _ExecTerminal,             "Term",                "Term",               INFO_TERM,               SYNTAX_TERM,             1,                     1,    }
// ---- Memory operation ----
 ,{ _ExecReadMem,              "Mem",                 "Mem",                INFO_MEM,                SYNTAX_MEM,              1,                     1,    }
 ,{ _ExecReadMem8,             "Mem8",                "Mem8",               INFO_MEM8,               SYNTAX_MEM8,             1,                     1,    }
 ,{ _ExecReadMem16,            "Mem16",               "Mem16",              INFO_MEM16,              SYNTAX_MEM16,            1,                     1,    }
 ,{ _ExecReadMem32,            "Mem32",               "Mem32",              INFO_MEM32,              SYNTAX_MEM32,            1,                     1,    }
 ,{ _ExecWriteU8,              "Write1",              "W1",                 INFO_WRITE1,             SYNTAX_WRITE1,           1,                     1,    }
 ,{ _ExecWriteU16,             "Write2",              "W2",                 INFO_WRITE2,             SYNTAX_WRITE2,           1,                     1,    }
 ,{ _ExecWriteU32,             "Write4",              "W4",                 INFO_WRITE4,             SYNTAX_WRITE4,           1,                     1,    } 
// ---- JTAG related commands ----
 ,{ _ExecSetJTAGConfig,        "JTAGConf",            "JTAGConf",           INFO_JTAGCONF,           SYNTAX_JTAGCONF,         1,                     0,    }
 ,{ _ExecIdentify,             "JTAGId",              "I",                  INFO_JTAGID,             SYNTAX_JTAGID,           1,                     0,    }
 ,{ _ExecWriteJTAGIR,          "WJTAGIR",             "WJIR",               INFO_WJTAGIR,            SYNTAX_WJTAGIR,          1,                     0,    }
 ,{ _ExecWriteJTAGDR,          "WJTAGDR",             "WJDR",               INFO_WJTAGDR,            SYNTAX_WJTAGDR,          1,                     0,    }
 ,{ _ExecWriteRaw,             "WJTAGRaw",            "WJR",                INFO_WJTAGRAW,           SYNTAX_WJTAGRAW,         1,                     0,    }
 ,{ _ExecResetTAP,             "ResetTAP",            "RTAP",               INFO_RESETTAP,           SYNTAX_RESETTAP,         1,                     0,    }
 ,{ _ExecResetTRST,            "ResetTRST",           "RT",                 INFO_RT,                 SYNTAX_RT,               1,                     0,    }
// ---- ICE -------------
 ,{ _ExecShowICERegs,          "ICE",                 "ICE",                INFO_ICE,                SYNTAX_ICE,              1,                     1,    }
 ,{ _ExecReadICEReg,           "ReadICE",             "RI",                 INFO_RI,                 SYNTAX_RI,               1,                     1,    }
 ,{ _ExecWriteICEReg,          "WriteICE",            "WI",                 INFO_WI,                 SYNTAX_WI,               1,                     1,    }
//
// TRACE
//
 ,{ _ExecTraceClear,           "TClear",              "TC",                 INFO_TCLEAR,             SYNTAX_TCLEAR,           1,                     1,    }
 ,{ _ExecTraceSetSize,         "TSetSize",            "TSS",                INFO_TSETSIZE,           SYNTAX_TSETSIZE,         1,                     1,    }
 ,{ _ExecTraceSetFormat,       "TSetFormat",          "TSF",                INFO_TSETFORMAT,         SYNTAX_TSETFORMAT,       1,                     1,    }
 ,{ _ExecTraceShowRegions,     "TShowRegions",        "TSR",                INFO_TSR,                SYNTAX_TSR,              1,                     1,    }
 ,{ _ExecTraceStart,           "TStart",              "TStart",             INFO_TSTART,             SYNTAX_TSTART,           1,                     1,    }
 ,{ _ExecTraceStop,            "TStop",               "TStop",              INFO_TSTOP,              SYNTAX_TSTOP,            1,                     1,    }
//
// SWO
//
 ,{ _ExecSWOSpeed,             "SWOSpeed",            "SWOSpeed",           INFO_SWOSPEED,           SYNTAX_SWOSPEED,         1,                     1,    }
 ,{ _ExecSWOStart,             "SWOStart",            "SWOStart",           INFO_SWOSTART,           SYNTAX_SWOSTART,         1,                     1,    }
 ,{ _ExecSWOStop,              "SWOStop",             "SWOStop",            INFO_SWOSTOP,            SYNTAX_SWOSTOP,          1,                     1,    }
 ,{ _ExecSWOStat,              "SWOStat",             "SWOStat",            INFO_SWOSTAT,            SYNTAX_SWOSTAT,          1,                     1,    }
 ,{ _ExecSWORead,              "SWORead",             "SWORead",            INFO_SWOREAD,            SYNTAX_SWOREAD,          1,                     1,    }
 ,{ _ExecSWOShow,              "SWOShow",             "SWOShow",            INFO_SWOSHOW,            SYNTAX_SWOSHOW,          1,                     1,    }
 ,{ _ExecSWOFlush,             "SWOFlush",            "SWOFlush",           INFO_SWOFLUSH,           SYNTAX_SWOFLUSH,         1,                     1,    }
 ,{ _ExecSWOView,              "SWOView",             "SWOView",            INFO_SWOVIEW,            SYNTAX_SWOVIEW,          1,                     1,    }
//
// Flash programming
//
 ,{ _ExecErase,                "Erase",               "Erase",              INFO_ERASE,              SYNTAX_ERASE,            1,                     1,    }
 ,{ _ExecLoadFile,             "LoadFile",            "LoadFile",           INFO_LOADFILE,           SYNTAX_LOADFILE,         1,                     1,    }
 ,{ _ExecSaveBin,              "SaveBin",             "SaveBin",            INFO_SAVEBIN,            SYNTAX_SAVEBIN,          1,                     1,    }
 ,{ _ExecVerifyBin,            "VerifyBin",           "VerifyBin",          INFO_VERIFYBIN,          SYNTAX_VERIFYBIN,        1,                     1,    }
// 
// Flasher File I/O Commands
//
 ,{ _ExecFileWrite,            "FWrite",              "FWr",                INFO_FWRITE,             SYNTAX_FWRITE,           1,                     0,    }
 ,{ _ExecFileRead,             "FRead",               "FRd",                INFO_FREAD,              SYNTAX_FREAD,            1,                     0,    }
 ,{ _ExecFileShow,             "FShow",               "FShow",              INFO_FSHOW,              SYNTAX_FSHOW,            1,                     0,    }
 ,{ _ExecFileDelete,           "FDelete",             "FDel",               INFO_FDELETE,            SYNTAX_FDELETE,          1,                     0,    }
 ,{ _ExecFileSize,             "FSize",               "FSz",                INFO_FSIZE,              SYNTAX_FSIZE,            1,                     0,    }
 ,{ _ExecFileList,             "FList",               "FList",              INFO_FLIST,              SYNTAX_FLIST,            1,                     0,    }
 ,{ _ExecFileSecureArea,       "SecureArea",          "SecureArea",         INFO_SECUREAREA,         SYNTAX_SECUREAREA,       1,                     0,    }
//
// Measurement and test commands
//
 ,{ _ExecPowerTrace,           "PowerTrace",          "PowerTrace",         INFO_POWERTRACE,         SYNTAX_POWERTRACE,       1,                     1,    }
 ,{ _ExecTestWSpeed,           "TestWSpeed",          "TestW",              INFO_TESTWSPEED,         SYNTAX_TESTWSPEED,       1,                     1,    }
 ,{ _ExecTestRSpeed,           "TestRSpeed",          "TestR",              INFO_TESTRSPEED,         SYNTAX_TESTRSPEED,       1,                     1,    }
 ,{ _ExecTestCSpeed,           "TestCSpeed",          "TestC",              INFO_TESTCSPEED,         SYNTAX_TESTCSPEED,       1,                     1,    }
 ,{ _ExecTestNWSpeed,          "TestNWSpeed",         "TestNW",             INFO_TESTNWSPEED,        SYNTAX_TESTNWSPEED,      1,                     0,    }
 ,{ _ExecTestNRSpeed,          "TestNRSpeed",         "TestNR",             INFO_TESTNRSPEED,        SYNTAX_TESTNRSPEED,      1,                     0,    }
 ,{ _ExecMeasureRTCKReactTime, "MR",                  "MR",                 INFO_MR,                 SYNTAX_MR,               1,                     1,    }
//
// J-Link Pin control
//
 ,{ _ExecClock,                "Clock",               "C",                  INFO_C,                  SYNTAX_C,                1,                     0,    }
 ,{ _ExecC00,                  "Clock00",             "C00",                INFO_C00,                SYNTAX_C00,              1,                     0,    }
 ,{ _ExecClrTCK,               "ClrTCK",              "TCK0",               INFO_CLRTCK,             SYNTAX_CLRTCK,           1,                     0,    }
 ,{ _ExecSetTCK,               "SetTCK",              "TCK1",               INFO_SETTCK,             SYNTAX_SETTCK,           1,                     0,    }
 ,{ _ExecClrTDI,               "ClrTDI",              "TDI0",               INFO_CLRTDI,             SYNTAX_CLRTDI,           1,                     0,    }
 ,{ _ExecSetTDI,               "SetTDI",              "TDI1",               INFO_SETTDI,             SYNTAX_SETTDI,           1,                     0,    }
 ,{ _ExecClrTMS,               "ClrTMS",              "TMS0",               INFO_CLRTMS,             SYNTAX_CLRTMS,           1,                     0,    }
 ,{ _ExecSetTMS,               "SetTMS",              "TMS1",               INFO_SETTMS,             SYNTAX_SETTMS,           1,                     0,    }
 ,{ _ExecClrTRST,              "ClrTRST",             "TRST0",              INFO_CLRTRST,            SYNTAX_CLRTRST,          1,                     0,    }
 ,{ _ExecSetTRST,              "SetTRST",             "TRST1",              INFO_SETTRST,            SYNTAX_SETTRST,          1,                     0,    }
 ,{ _ExecClrRESET,             "ClrRESET",            "R0",                 INFO_CLRRESET,           SYNTAX_CLRRESET,         1,                     0,    }
 ,{ _ExecSetRESET,             "SetRESET",            "R1",                 INFO_SETRESET,           SYNTAX_SETRESET,         1,                     0,    } 
};

/*********************************************************************
*
*       Command list hidden
*
*   These commands are hidden either because they are for internal use only,
*   or because they are depricated/obsolete.
*/
static const _COMMAND_INFO _aCmdHidden[] = {
//  pfCmd                      sLongName              sShortName                        NeedsJLinkConnection   NeedsTargetConnection
  { _ExecQuitCommander,        "quit",                "q",                  NULL, NULL, 0,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecQuitCommander,        "qc",                  "qc",                 NULL, NULL, 0,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecStepOver,             "stepover",            "so",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecSetTimeoutCmd,        "SetTimeoutCmd",       "SetTimeoutCmd",      NULL, NULL, 0,                     0,    }
 ,{ _ExecReadMem64,            "mem64",               "mem64",              NULL, NULL, 1,                     1,    }
 ,{ _ExecWriteMultiU32,        "wm4",                 "wm4",                NULL, NULL, 1,                     1,    }
 ,{ _ExecResetNoHalt,          "rnh",                 "rnh",                NULL, NULL, 1,                     1,    }
 ,{ _ExecGetDebugInfo,         "GetDebugInfo",        "gdi",                NULL, NULL, 1,                     1,    }
 ,{ _ExecSetDebugUnitBlockMask,"sdubm",               "sdubm",              NULL, NULL, 1,                     1,    }
 ,{ _ExecUpdateBTL,            "UpdateBTL",           "UpdateBTL",          NULL, NULL, 1,                     0     }
 ,{ _ExecASCIIView,            "ASCIIView",           "ASCIIView",          NULL, NULL, 0,                     0     }
// ---- ETM -------------
 ,{ _ExecShowETMState,         "etm",                 "etm",                NULL, NULL, 1,                     1,    }
 ,{ _ExecReadETMReg,           "re",                  "re",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecWriteETMReg,          "we",                  "we",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecStartTrace,           "es",                  "es",                 NULL, NULL, 1,                     1,    }
// ---- ETB ------------- Legacy funcionality only working with ARM7/9 targets
 ,{ _ExecShowETBState,         "etb",                 "etb",                NULL, NULL, 1,                     1,    }
 ,{ _ExecReadETBReg,           "rb",                  "rb",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecWriteETBReg,          "wb",                  "wb",                 NULL, NULL, 1,                     1,    }
// ---- TRACE -----------
 ,{ _ExecTraceAddBranch,       "TAddBranch",          "tab",                NULL, NULL, 1,                     1,    }
 ,{ _ExecTraceAddInst,         "TAddInst",            "tai",                NULL, NULL, 1,                     1,    }
// ---- STRACE ----------
 ,{ _ExecSTraceConfig,         "STraceConfig",        "STConfig",           NULL, NULL, 1,                     1,    }
 ,{ _ExecSTraceStart,          "STraceStart",         "STStart",            NULL, NULL, 1,                     1,    }
 ,{ _ExecSTraceStop,           "STraceStop",          "STStop",             NULL, NULL, 1,                     1,    }
 ,{ _ExecSTraceRead,           "STraceRead",          "STRead",             NULL, NULL, 1,                     1,    }
 ,{ _ExecSTraceReadEx,         "STraceReadEx",        "STReadEx",           NULL, NULL, 1,                     1,    }
 ,{ _ExecSTraceGetInstStats,   "STraceGetInstStats",  "STGetIS",            NULL, NULL, 1,                     1,    }
 ,{ _ExecSTraceBIST,           "STraceBIST",          "STBIST",             NULL, NULL, 1,                     0,    }
 ,{ _ExecSTraceControl,        "STraceControl",       "STCtrl",             NULL, NULL, 1,                     1,    }
// ---- SWO -------------
 ,{ _ExecSWOStabilityTest,     "SWOStability",        "SWOStability",       NULL, NULL, 1,                     1,    }
// ---- HSS -------------
 ,{ _ExecHSSStart,             "HSSStart",            "HSSStart",           NULL, NULL, 1,                     1,    }
 ,{ _ExecHSSStop,              "HSSStop",             "HSSStop",            NULL, NULL, 1,                     1,    }
 ,{ _ExecHSSRead,              "HSSRead",             "HSSRead",            NULL, NULL, 1,                     1,    }
// ---- EMUCOM ----------
 ,{ _ExecEMUCOMWrite,          "ECWrite",             "ecw",                NULL, NULL, 1,                     0,    }
 ,{ _ExecEMUCOMRead,           "ECRead",              "ecr",                NULL, NULL, 1,                     0,    }
 ,{ _ExecEMUCOMIsSupported,    "ECIsSupported",       "ecis",               NULL, NULL, 1,                     0,    }
// ---- File I/O --------
 ,{ _ExecFileInfo,             "finfo",               "finfo",              NULL, NULL, 1,                     0,    } // Mainly for internal API testing, not for public use yet
 ,{ _ExecFileEntries,          "fentries",            "fentries",           NULL, NULL, 1,                     0,    } // Mainly for internal API testing, not for public use yet
// ---- Test ------------
 ,{ _ExecTestPCode,            "testpcode",           "testp",              NULL, NULL, 1,                     0,    }
 ,{ _ExecRunPCode,             "runpcode",            "runp",               NULL, NULL, 1,                     1,    }
 ,{ _ExecTestRecover,          "testRecover",         "testrecover",        NULL, NULL, 1,                     0,    }
// ---- GPIO ------------
 ,{ _ExecGPIOGetProps,         "GPIOGetProps",        "GPIOProps",          NULL, NULL, 1,                     0,    }
 ,{ _ExecGPIOSetState,         "GPIOSetState",        "GPIOSet",            NULL, NULL, 1,                     0,    }
 ,{ _ExecGPIOGetState,         "GPIOGetState",        "GPIOGet",            NULL, NULL, 1,                     0,    }
// ---- JTAG ------------
 ,{ _ExecSetConfig,            "Config",              "Config",             NULL, NULL, 1,                     1,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecWriteJTAGCommand,     "WJTAGCom",            "WJC",                NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecWriteJTAGDR,          "WJTAGData",           "WJD",                NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
// ---- JTAG-Hardware ---
 ,{ _ExecClrTDI,               "ClrTDI",              "0",                  NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSetTDI,               "SetTDI",              "1",                  NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecClrTMS,               "ClrTMS",              "t0",                 NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSetTMS,               "SetTMS",              "t1",                 NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
// ---- Configuration ---
 ,{ _ExecMSDDisble,            "MSDDisable",          "MSDDisable",         NULL, NULL, 1,                     0,    }
 ,{ _ExecMSDEnable,            "MSDEnable",           "MSDEnable",          NULL, NULL, 1,                     0,    }
 ,{ _ExecWebUSBDisable,        "WebUSBDisable",       "WebUSBDisable",      NULL, NULL, 1,                     0,    }
 ,{ _ExecWebUSBEnable,         "WebUSBEnable",        "WebUSBEnable",       NULL, NULL, 1,                     0,    }
 ,{ _ExecWriteConfigVMSD,      "VMSDDownloadConfig",  "VMSDDownloadConfig", NULL, NULL, 1,                     0,    }
 ,{ _ExecCDCSetHookFuncs,      "CDCSetHooks",         "CDCSetHooks",        NULL, NULL, 1,                     0,    }
 ,{ _ExecGetCounters,          "GetCounters",         "gc",                 NULL, NULL, 1,                     0,    }
 ,{ _ExecSelectInterface,      "SelectInterface",     "if",                 NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSetHWFC,              "SetHWFC",             "SetHWFC",            NULL, NULL, 1,                     0,    }
 ,{ _ExecUSBIP,                "USBIP",               "USBIP",              NULL, NULL, 1,                     0,    }
 ,{ _ExecSetWiFi,              "SetWiFi",             "SetWiFi",            NULL, NULL, 1,                     0,    }
 ,{ _ExecResetConfig,          "resetconf",           "resetconf",          NULL, NULL, 1,                     0,    }
 ,{ _ExecMACAddress,           "macaddr",             "macaddr",            NULL, NULL, 1,                     0,    }
 ,{ _ExecDisassemble,          "disassemble",         "da",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecRawTrace,             "rawtrace",            "rtr",                NULL, NULL, 1,                     1,    }
 ,{ _ExecTestNet,              "testnet",             "tn",                 NULL, NULL, 1,                     0,    }
 ,{ _ExecShowEmuInfo,          "ShowEmus",            "emu",                NULL, NULL, 0,                     0,    }
 ,{ _ExecShowEmuStatus,        "EmuStatus",           "est",                NULL, NULL, 1,                     0,    }
 ,{ _ExecConfigEmu,            "ConfigEmu",           "ce",                 NULL, NULL, 1,                     0,    }
 ,{ _ExecSelectEmuByUSBSN,     "SelectEmuBySN",       "sesn",               NULL, NULL, 0,                     0,    }
 ,{ _ExecSelectFamily,         "SelectFamily",        "sf",                 NULL, NULL, 1,                     0,    }
 ,{ _ExecSelectCore,           "SelectCore",          "sc",                 NULL, NULL, 1,                     0,    }
 ,{ _ExecConfigDevices,        "ConfigDevices",       "cd",                 NULL, NULL, 1,                     0,    }
 ,{ _ExecReadMemIndirect,      "memi",                "memi",               NULL, NULL, 1,                     1,    }
 ,{ _ExecTest,                 "test",                "test",               NULL, NULL, 1,                     0,    }
 ,{ _ExecIndicator,            "indi",                "indi",               NULL, NULL, 1,                     0,    }
 ,{ _ExecSelDevice,            "sel",                 "sel",                NULL, NULL, 1,                     0,    }
 ,{ _ExecReadCP15,             "rcp15",               "rcp15",              NULL, NULL, 1,                     1,    }
 ,{ _ExecWriteCP15,            "wcp15",               "wcp15",              NULL, NULL, 1,                     1,    }
 ,{ _ExecReadCSR,              "readcsr",             "rcsr",               NULL, NULL, 1,                     1,    }
 ,{ _ExecWriteCSR,             "writecsr",            "wcsr",               NULL, NULL, 1,                     1,    }
 ,{ _ExecExecCommand,          "Exec",                "Exec",               NULL, NULL, 0,                     0,    } // If J-Link connection is needed depends on exec, so it is determined inside the function
 ,{ _ExecSetRAMAddr,           "ram",                 "ram",                NULL, NULL, 1,                     0,    }
 ,{ _ExecTestRAM,              "testram",             "testram",            NULL, NULL, 1,                     1,    }
 ,{ _ExecAND,                  "and",                 "and",                NULL, NULL, 1,                     1,    }
 ,{ _ExecOR,                   "or",                  "or",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecXOR,                  "xor",                 "xor",                NULL, NULL, 1,                     1,    }
 ,{ _ExecSetBMI,               "SetBMI",              "SetBMI",             NULL, NULL, 1,                     0,    }
 ,{ _ExecGetBMI,               "GetBMI",              "GetBMI",             NULL, NULL, 1,                     0,    }
 ,{ _ExecLicenseAdd,           "license add",         "license add",        NULL, NULL, 1,                     0,    }
 ,{ _ExecLicenseShow,          "license show",        "license show",       NULL, NULL, 1,                     0,    }
 ,{ _ExecLicenseErase,         "license erase",       "license erase",      NULL, NULL, 1,                     0,    }
 ,{ _ExecRTTRead,              "rttread",             "rttread",            NULL, NULL, 1,                     1,    }
 ,{ _ExecTestPINOverride,      "TestPinOverride",     "testpio",            NULL, NULL, 1,                     0,    }
 ,{ _ExecGetPinCaps,           "GetPinCaps",          "pincaps",            NULL, NULL, 1,                     0,    }
 ,{ _ExecCDCWrite,             "CDCWrite",            "cdcW",               NULL, NULL, 1,                     0,    }
 ,{ _ExecCDCRead,              "CDCRead",             "cdcR",               NULL, NULL, 1,                     0,    }
 ,{ _ExecCDCSetBaudrate,       "CDCBaudRate",         "cdcB",               NULL, NULL, 1,                     0,    }
 ,{ _ExecMRU,                  "MRU",                 "MRU",                NULL, NULL, 0,                     0,    }
 ,{ _ExecShowHWInfo,           "hwinfo",              "hwinfo",             NULL, NULL, 1,                     0,    }
 ,{ _ExecWriteMultiple,        "wm",                  "wm",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecLoadBin,              "loadbin",             "loadbin",            NULL, NULL, 1,                     1,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecUnlock,               "unlock",              "unlock",             NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSWDReadAP,            "SWDReadAP",           "srap",               NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSWDWriteAP,           "SWDWriteAP",          "swap",               NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSWDReadDP,            "SWDReadDP",           "srdp",               NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSWDWriteDP,           "SWDWriteDP",          "swdp",               NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecSWDSelect,            "SWDSelect",           "SWDSelect",          NULL, NULL, 1,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
 ,{ _ExecTestHaltGo,           "TestHaltGo",          "thg",                NULL, NULL, 1,                     1,    }
 ,{ _ExecTestSingleStep,       "TestStep",            "ts",                 NULL, NULL, 1,                     1,    }
 ,{ _ExecWriteConfig,          "wconf",               "wconf",              NULL, NULL, 1,                     0,    }
 ,{ _ExecReadConfig,           "rconf",               "rconf",              NULL, NULL, 1,                     0,    }
 ,{ _ExecSelectEmuFromList,    "SelectEmulator",      "selemu",             NULL, NULL, 0,                     0,    } // Obsolete. We keep it here for backward compatibility reasons.
};

/*********************************************************************
*
*       _GetPadString
*
*  Function description
*    Returns padding string pointer from passed pad string.
*    Expects sPed to already be initialized.
*
*  Return value
*    Pointer to peddings string.
*/
static const char* _GetPadString(const char* sToPed, const char* sPed) {
  int iPad;

  iPad = UTIL_strlen(sPed);
  iPad = iPad - MAX(0, iPad - UTIL_strlen(sToPed));
  return sPed + iPad;
}

/*********************************************************************
*
*       _FindCommand
*
*  Function description
*    Returns pointer to Command info if specific command was found.
*
*  Return value
*    Pointer to _COMMAND_INFO  Command was found
*    NULL                      Command was not found
*/
static const _COMMAND_INFO* _FindCommand(const char** s) {
  const _COMMAND_INFO* pCmd;
  int i;

  pCmd = NULL;
  _EatWhite(s);
  //
  // Search for command in "public" command table.
  //
  for (i = 0; i < COUNTOF(_aCmd); i++) {
    if ((_CompareCmd(s, _aCmd[i].sLongName) == 0) || (_CompareCmd(s, _aCmd[i].sShortName) == 0)) {                // Check short and long version of command
      pCmd = &_aCmd[i];
    }
  }
  //
  // Search for command in "hidden" command table.
  //
  if (pCmd == 0) { // Command not found in "public" array? => Look through "hidden" array:
    for (i = 0; i < COUNTOF(_aCmdHidden); i++) {
      if ((_CompareCmd(s, _aCmdHidden[i].sLongName) == 0) || (_CompareCmd(s, _aCmdHidden[i].sShortName) == 0)) {  // Check short and long version of command
        pCmd = &_aCmdHidden[i];
      }
    }
  }
  return pCmd;
}

/*********************************************************************
*
*       _PrintCommandInfo
*
*  Function description
*    Prints command info of a command.
*/
static void _PrintCommandInfo(const _COMMAND_INFO* pCmd) {
  const char* ps;
  
  _ReportOutf("----------------------\n");
  ps = pCmd->sSyntax ? pCmd->sSyntax : pCmd->sLongName;
  _ReportOutf("Command long name: %s\n", pCmd->sLongName);
  _ReportOutf("Command short name: %s\nSyntax: %s\n", pCmd->sShortName, ps);
  if (pCmd->sInfo) {
    _ReportOutf("Function: %s\n", pCmd->sInfo);
  }
  _ReportOutf("Requires connection to J-Link: %s\n", (pCmd->NeedsJLinkConnection == 1)  ? "yes" : "no");
  _ReportOutf("Requires connection to target: %s\n", (pCmd->NeedsTargetConnection == 1) ? "yes" : "no");
  _ReportOutf("----------------------\n");
  return;
}

/*********************************************************************
*
*       _ExecShowCommands
*
*  Function description
*    Show list of available commands.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*/
static int _ExecShowCommands(const char* s) {
  int i;
  char acPadName[NUM_PAD_CHARS_NAME + 1];
  char acPadSyntax[NUM_PAD_CHARS_SYNTAX + 1];
  char ac[64];
  const char* ps;
  const _COMMAND_INFO* pCmd;

  _EatWhite(&s);
  if (*s != 0) { // Specific command was passed with '?' ? => Search for command and print info.
    pCmd = _FindCommand(&s);
    if (pCmd) {
      _PrintCommandInfo(pCmd);
    } else {
      _ReportOutf("Unknown command. '?' for help.\n");
    }
    return 0;
  }
  //
  // Init pad strings
  //
  memset(acPadName, ' ', NUM_PAD_CHARS_NAME);
  acPadName[NUM_PAD_CHARS_NAME] = 0;
  memset(acPadSyntax, ' ', NUM_PAD_CHARS_SYNTAX);
  acPadSyntax[NUM_PAD_CHARS_SYNTAX] = 0;
  //
  // Print "table header"
  //
  _ReportOutf("\nAvailable commands are:\n");
  UTIL_CopyString(ac, "Command name", UTIL_SIZEOF(ac));
  _ReportOutf("%s%s  ", ac, _GetPadString(ac, acPadName));
  UTIL_CopyString(ac, "Command syntax", UTIL_SIZEOF(ac));
  _ReportOutf("%s%s  Command function\n", ac, _GetPadString(ac, acPadSyntax));
  _ReportOutf("----------------------\n");
  //
  // Print commands
  //
  for (i = 0; i < COUNTOF(_aCmd); i++) {
    ps = _aCmd[i].sSyntax ? _aCmd[i].sSyntax : _aCmd[i].sLongName;   // No special syntax specified? => Use sLongName as syntax.
    _ReportOutf("%s%s  ", _aCmd[i].sLongName, _GetPadString(_aCmd[i].sLongName, acPadName));
    _ReportOutf("%s%s  %s\n", ps, _GetPadString(ps, acPadSyntax), _aCmd[i].sInfo);
  }
  _ReportOutf("----------------------\n");
  return 0;
}

/*********************************************************************
*
*       _ExecCommand
*
*
*
*  Return value
*  >= 0: Continue execution of command line
*  <  0: Error occured. Upper level handles quit or continuation
*/

static int _ExecCommand(_SETTINGS* pCommanderSettings, const char* s) {
  int r;
  const _COMMAND_INFO* pCmd;
  
  pCmd = NULL;
  _EatWhite(&s);
  //
  // Go through command tables and find command
  //
  pCmd = _FindCommand(&s);
  if (pCmd == NULL) {  // Command not found? => Early out.
    _ReportOutf("Unknown command. '?' for help.\n");
    return 0;
  }
  //
  // Make sure that J-Link connection is established, if required by command
  // In script mode we do not automatically connect to J-Link to allow the user to specify connection settings via command file
  //
  if (pCmd->NeedsJLinkConnection && (pCommanderSettings->Status.ConnectedToJLink == 0)) {
    _ReportOut("J-Link connection not established yet but required for command.\n");
    r = _ConnectToJLink(&_CommanderSettings);
    if (r < 0) {
      return r;
    }
  }
  //
  // Make sure that target connection is established, if required by command
  //
  if (pCmd->NeedsTargetConnection && (pCommanderSettings->Status.ConnectedToTarget == 0)) {
    _ReportOut("Target connection not established yet but required for command.\n");
    r = _ConnectToTarget(&_CommanderSettings);
    if (pCommanderSettings->CloseRequested) {
      return 0;
    }
    if (r < 0) {
      return r;
    }
  }
  //
  // Perform command
  //
  r = pCmd->pfCmd(s);
  _PRINT_RETVAL(("Command returned: %d\n", r));
  return (r >= 0) ? 0 : r;
}

/*********************************************************************
*
*       _ParseCommandFile
*
*  Function description
*    Parses the command file passed to J-Link Commander and evaluates it line by line.
*    Command files contain 1 J-Link Commander command per line.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error happened
*/
static int _ParseCommandFile(_SETTINGS* pCommanderSettings, const char* pBuffer) {
  const char* s;
  char ac[512];
  int r;

  while (_GetLineAndEatComments(&pBuffer, ac, sizeof(ac))) {
    s = ac;
    _ReportOutf("J-Link>%s\n", s);
    if        (_CompareCmd(&s, ":Loop") == 0) {
      _pLoop = pBuffer;
    } else if (_CompareCmd(&s, "BEQ") == 0) {
      if (_pLoop && (_Var == 0)) {
        pBuffer = _pLoop;
      }
    } else if (_CompareCmd(&s, "BNE") == 0) {
      if (_pLoop && (_Var != 0)) {
        pBuffer = _pLoop;
      }
    } else {
      r = _ExecCommand(pCommanderSettings, ac);
      //
      // If executed command set requested to close, the command returns O.K. but the upper layer have to handle the close request
      //
      if (pCommanderSettings->CloseRequested) {
        return 0;
      }
      if ((_ExitOnErrorLvl) && (r < 0)) {
        return JLINK_ERROR_SYNTAX;
      }
    }
  }
  return 0;
}

/*********************************************************************
*
*       _InteractiveMode
*
*  Function description
*    Controls interactive mode of J-Link where user needs to enter commands.
*
*  Return value
*    >= 0  O.K.
*     < 0  Error
*
*  Notes
*    (1) Interactive mode may be cancelled by a "q" command or if exit on error level has been activated and an error occurred
*/
static int _InteractiveMode(void) {
  char ac[1024];
  char * s;
  int r;
  _SETTINGS* pCommanderSettings;
  //
  // Open connection to J-Link and show some information about J-Link + DLL
  //
  pCommanderSettings = &_CommanderSettings;
  r = 0;
  if (pCommanderSettings->Status.ConnectedToJLink == 0) {
    r = _ConnectToJLink(pCommanderSettings);
  }
  //
  // User specified to automatically establish a target connection on start of commander?
  //
  if (r >= 0) {
    r = 0;
    if (pCommanderSettings->DoAutoConnect) {
      if (pCommanderSettings->Status.ConnectedToTarget == 0) {
        r = _ConnectToTarget(pCommanderSettings);
        if (_CommanderSettings.CloseRequested) {
          return 0;
        }
      }
    } else {
      _ReportOut("\n\nType \"connect\" to establish a target connection, '?' for help\n");
    }
  } else {                       // Error occurred? => Check if ExitOnError is active.
    if (r >= _ExitOnErrorLvl) {  // Exit on error is set? => Quit.
      return r;
    }
  }
  //
  // Wait for user input
  //
  do {
    r = SYS_ConsoleGetString("J-Link>", ac, sizeof(ac));
    if (r < 0) {
      _CommanderSettings.CloseRequested = 1;
      break;
    }
    s = ac;
    r = _ExecCommand(pCommanderSettings, s);
    if ((r < 0) && (r >= _ExitOnErrorLvl)) {      // Error occured and exit on error activated? => Quit
      break;
    }
    if (pCommanderSettings->CloseRequested) {      // Quit command received? => Quit
      break;
    }
  } while (1);
  return r;
}

/*********************************************************************
*
*       _ScriptMode
*
*  Function description
*    Controls script mode where user passes a command file to J-Link Commander and automates operations.
*
*  Return value
*    >= 0: O.K.
*    == 0: O.K., enter interactive mode
*    == 1: O.K., script closed connection with "qc" or "q" command
*     < 0: Error happened
*
*  Notes
*    (1) Interactive mode may be cancelled by a "q" command or if exit on error level has been activated and an error occurred
*/
static int _ScriptMode(_SETTINGS* pCommanderSettings) {
  char* pBuffer;
  HANDLE hFile;
  U32 NumBytes;
  int NumBytesRead;
  int r;
  //
  // Load command file
  //
  r = 0;
  pBuffer = NULL;
  hFile = _OpenFile(pCommanderSettings->acCommanderScriptFile, FILE_FLAG_READ | FILE_FLAG_SHARE_READ);
  if (hFile != INVALID_HANDLE_VALUE) {
    NumBytes = _GetFileSize(hFile);
    pBuffer = malloc(NumBytes + 1);
    if (pBuffer) {
      NumBytesRead = _ReadFile(hFile, pBuffer, NumBytes);
    } else {
      NumBytesRead = 0;
    }
    if (NumBytesRead == (int)NumBytes) {
      pBuffer[NumBytes] = 0;
      _ReportOut("\nJ-Link Command File read successfully.\n");
    } else {
      _ReportOut("Failed to read J-Link Command File\n");
      r = -1;
    }
    _CloseFile(hFile);
  } else {
    _ReportOutf("Could not open J-Link Command File '%s'\n", pCommanderSettings->acCommanderScriptFile);
    r = -1;
  }
  if (r < 0) {
    goto Done;
  }
  //
  // Process command file
  //
  _ReportOutf("Processing script file...\n");
  r = _ParseCommandFile(pCommanderSettings, pBuffer);
  _ReportOutf("\nScript processing completed.\n\n");
  //
  // Do cleanup
  //
Done:
  if (pBuffer) {
    free(pBuffer);
  }
  return r;
}

/*********************************************************************
*
*       _HandleCmdLineHelp
*
*  Function description
*    Parses specific command line option "?" which is used
*    to print all available command line options.
*
*  Return Value
*      0  O.K.
*    < 0  Error (Syntax error etc.)
*/
static int _HandleCmdLineHelp(_SETTINGS* pCommanderSettings, const char* s) {
  (void)pCommanderSettings;
  (void)s;
  _ReportOutf("For all available command line options please refer to:\nhttps://wiki.segger.com/J-Link_Commander#Batch_processing\n\n");
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineSI
*
*  Function description
*    Parses specific command line option -si which is used
*    to select the target interface J-Link shall use to connect to the target.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineSI(_SETTINGS* pCommanderSettings, const char* s) {
  int r;

  r = _Name2TIF(s);
  if (r < 0) {
    _ReportOutf("ERROR while parsing value for -si.\n");
    return JLINK_ERROR_SYNTAX;
  }
  pCommanderSettings->TargetIF = r;
  pCommanderSettings->TargetIFSet = 1;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineJTAGConf
*
*  Function description
*    Parses command line option -JTAGConf
*    which is used to setup the JTAG device selection inside a JTAG chain
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineJTAGConf(_SETTINGS* pCommanderSettings, const char* s) {
  if (_ParseDec(&s, (U32*)&pCommanderSettings->JTAGConfIRPre)) {
    _ReportOutf("ERROR while parsing value for IRPre. Using default: %d.\n", pCommanderSettings->JTAGConfIRPre);
    return JLINK_ERROR_SYNTAX;
  }
  if (*s == ',') {     // Skip comma between JTAG config params
    s++;
  }
  if (_ParseDec(&s, (U32*)&pCommanderSettings->JTAGConfDRPre)) {
    _ReportOutf("ERROR while parsing value for DRPre. Using default: %d.\n", pCommanderSettings->JTAGConfDRPre);
    return JLINK_ERROR_SYNTAX;
  }
  pCommanderSettings->JTAGConfSet = 1;
  pCommanderSettings->TargetIFSet = 1;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineCommanderScript
*
*  Function description
*    Parses specific command line option -CommandFile which is used
*    to pass a J-Link commander script that is executed by J-Link Commander.
*    Do NOT confuse with J-Link script files.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineCommanderScript(_SETTINGS* pCommanderSettings, const char* s) {
  _UTIL_CopyString(pCommanderSettings->acCommanderScriptFile, s, _SIZEOF(pCommanderSettings->acCommanderScriptFile));
  pCommanderSettings->ScriptModeActive = 1;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineUSB
*
*  Function description
*    Parses specific command line option -usb which is used
*    to select the USB address to use to connec to the emulator.
*    Deprecated, use -SelectEmuBySN instead.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineUSB(_SETTINGS* pCommanderSettings, const char* s) {
  U32 v;

  if (_ParseDec(&s, &v)) {
    _ReportOutf("ERROR while parsing value for usb.\n");
    return -1;
  }
  pCommanderSettings->HostIF = JLINKARM_HOSTIF_USB;
  pCommanderSettings->EmuSN = v;    // Will be 0-3 in this case
  pCommanderSettings->IsSetEmuSN  = 1;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineDevice
*
*  Function description
*    Parses specific command line option -device which is used
*    to select the device the emulator shall connect to.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineDevice(_SETTINGS* pCommanderSettings, const char* s) {
  JLINK_FUNC_MRU_GETLIST* pfGetMRU;
  char                    ac[1024];
  int                     r;
  
  _UTIL_CopyString(ac, s, _SIZEOF(ac));
  //
  // Handle "default" device
  // User specified the device as "default" device (last device used and saved by the DLL)? => Get default device
  //
  if (_JLinkstricmp("default", ac) == 0) {
    pfGetMRU     = (JLINK_FUNC_MRU_GETLIST*)JLINK_GetpFunc(JLINK_IFUNC_MRU_GETLIST);
    if (pfGetMRU) {
      r = pfGetMRU(JLINK_MRU_GROUP_DEVICE, ac, sizeof(ac));
      if (r < 0) {
        ac[0] = 0;
      }
    }
    //
    // No MRU device present, assume "Unspecified" as device
    //
    if (ac[0] == 0) { // We did not find a default device? => Use "Unspecified" as device name.
      _UTIL_CopyString(ac, "Unspecified", _SIZEOF(ac));
    }
  }
  _UTIL_CopyString(pCommanderSettings->acDeviceName, ac, _SIZEOF(pCommanderSettings->acDeviceName));
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineLog
*
*  Function description
*    Parses specific command line option -log which is used
*    to enable the J-Link log file
*
*  Notes
*    In general, the command line parameters are parsed, stored in the
*    _SETTINGS context and evaluated later during runtime. However, the
*    "-log" command is an exception as we want to make sure that all
*    following comamnds are logged
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineLog(_SETTINGS* pCommanderSettings, const char* s) {
  (void)pCommanderSettings;
  JLINKARM_SetLogFile(s);
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineJLinkScriptFile
*
*  Function description
*    Parses specific command line option -JLinkScriptFile which is used
*    to select a J-Link script file to be used by the DLL for connect etc.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineJLinkScriptFile(_SETTINGS* pCommanderSettings, const char* s) {
  _UTIL_CopyString(pCommanderSettings->acJLinkScriptFile, s, _SIZEOF(pCommanderSettings->acJLinkScriptFile));
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineJLinkSettingsFile
*
*  Function description
*    Parses specific command line option -settingsfile which is used
*    to select a settings file to be used by the DLL.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineJLinkSettingsFile(_SETTINGS* pCommanderSettings, const char* s) {
  _UTIL_CopyString(pCommanderSettings->acSettingsFile, s, _SIZEOF(pCommanderSettings->acSettingsFile));
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineSelectEmuBySN
*
*  Function description
*    Parses specific command line option -SelectEmuBySN which is used
*    to connect to an emulator via its serial number.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineSelectEmuBySN(_SETTINGS* pCommanderSettings, const char* s) {
  U32 v;

  if (_ParseDec(&s, &v)) {
    _ReportOutf("ERROR while parsing value for -SelectEmuBySN.\n");
    return -1;
  }
  pCommanderSettings->EmuSN = v;
  pCommanderSettings->IsSetEmuSN  = 1;
  pCommanderSettings->HostIF = JLINKARM_HOSTIF_USB;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineIP
*
*  Function description
*    Parses specific command line option -ip which is used
*    to connect to an emulator connected via IP.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineIP(_SETTINGS* pCommanderSettings, const char* s) {
  _UTIL_CopyString(pCommanderSettings->acIPAddr, s, _SIZEOF(pCommanderSettings->acIPAddr));
  pCommanderSettings->HostIF = JLINKARM_HOSTIF_IP;
  return 0;
}
/*********************************************************************
*
*       _HandleCmdLineExitOnError
*
*  Function description
*    Sets _ExitOnErrorLvl to JLINK_ERROR_ALL, so JLinkCommander will break on all return values < 0.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineExitOnError(_SETTINGS* pCommanderSettings, const char* s) {
  (void)pCommanderSettings;
  (void)s;
  _ReportOutf("J-Link Commander will now exit on Error\n");
  _ExitOnErrorLvl = JLINK_ERROR_SYNTAX;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineAutoConnect
*
*  Function description
*    Parses specific command line option -AutoConnect which is used
*    to set automatically start the connect sequence for connecting to the target.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineAutoConnect(_SETTINGS* pCommanderSettings, const char* s) {
  U32 v;

  if (_ParseDec(&s, &v)) {
    _ReportOutf("ERROR while parsing value for -AutoConnect.\n");
    return -1;
  }
  pCommanderSettings->DoAutoConnect = v;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineSpeed
*
*  Function description
*    Parses specific command line option -speed which is used
*    to set the default target interface speed used by the Commander.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineSpeed(_SETTINGS* pCommanderSettings, const char* s) {
  U32 v;

  if (_JLinkstricmp(s, "adaptive") == 0) {
    v = JLINKARM_SPEED_ADAPTIVE;
  } else if (_JLinkstricmp(s, "auto") == 0) {
    v = JLINKARM_SPEED_AUTO;
  } else {
    if (_ParseDec(&s, &v)) {
      _ReportOutf("ERROR while parsing value for -speed.\n");
      return -1;
    }
  }
  pCommanderSettings->InitTIFSpeedkHz = v;
  pCommanderSettings->InitTIFSpeedkHzSet = 1;
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineRTTTelnetPort
*
*  Function description
*    Parses specific command line option -RTTTelnetPort which is used
*    to set the RTT Telnet Port of the J-Link DLL server.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineRTTTelnetPort(_SETTINGS* pCommanderSettings, const char* s) {
  U32 Port;
  const char* sErr;
  char ac[4000];
  char acCommand[128];

  (void)pCommanderSettings;
  sErr = _ParseDec(&s, &Port);
  if (sErr) {
    return -1;
  }
  ac[0] = 0;
  UTIL_snprintf(acCommand, UTIL_SIZEOF(acCommand), "SetRTTTelnetPort %d", Port);
  JLINKARM_ExecCommand(acCommand, &ac[0], sizeof(ac));
  _ReportOutf(ac);
  return 0;
}

/*********************************************************************
*
*       _HandleCmdLineNoGui
*
*  Function description
*    Prevents the DLL to use GUI (except for License dialogs and similar).
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _HandleCmdLineNoGui(_SETTINGS* pCommanderSettings, const char* s) {
  U32 v;
  
  (void)pCommanderSettings;
  if (_ParseDec(&s, &v)) {
    _ReportOutf("ERROR while parsing value for -NoGui.\n");
    return -1;
  }
  _SuppressGUI = v;
  return 0;
}

static const CMDLINE_OPTION _aCmdLineOption[] = {
  //
  // Batch mode specific
  //
  { "-AutoConnect",     _HandleCmdLineAutoConnect       },
  { "-CommandFile",     _HandleCmdLineCommanderScript   },
  { "-ExitOnError",     _HandleCmdLineExitOnError       },
  { "-NoGui",           _HandleCmdLineNoGui             },
  //
  // J-Link connection specific
  //
  { "-IP",              _HandleCmdLineIP                },
  { "-USB",             _HandleCmdLineUSB               },
  //
  // Device specific
  //
  { "-Device",          _HandleCmdLineDevice            },
  { "-If",              _HandleCmdLineSI                },
  { "-JLinkScriptFile", _HandleCmdLineJLinkScriptFile   },
  { "-JTAGConf",        _HandleCmdLineJTAGConf          },
  { "-Speed",           _HandleCmdLineSpeed             },
  //
  // Misc
  //
  { "-Log",             _HandleCmdLineLog               },
  { "-RTTTelnetPort",   _HandleCmdLineRTTTelnetPort     },
  //
  // Obsolete command line arguments.
  // We keep them here for backward compatibility.
  // We do no longer mention them in the manual.
  //  
  { "-CommanderScript", _HandleCmdLineCommanderScript   },
  { "-SelectEmuBySN",   _HandleCmdLineSelectEmuBySN     },
  { "-SettingsFile",    _HandleCmdLineJLinkSettingsFile },
  { "-Si",              _HandleCmdLineSI                },
  { "IP",               _HandleCmdLineIP                },
  { "SelectEmuBySN",    _HandleCmdLineSelectEmuBySN     },
  { "USB",              _HandleCmdLineUSB               },
  { NULL, NULL }
};

/*********************************************************************
*
*       _ParseCmdLine
*
*  Function description
*    Parse all command line parameters and setup the commander options accordingly.
*
*  Return Value
*      0: O.K.
*    < 0: Error (Syntax error etc.)
*/
static int _ParseCmdLine(_SETTINGS* pCommanderSettings, int argc, char* argv[]) {
  HANDLE hFile;
  const char* s;
  int i;
  int j;
  int r;
  //
  // Parse option by option
  //
  i = 1;  // First command line parameter is always the application name
  while (i < argc) {
    //
    // Check if command is in command list
    //
    s = argv[i++];
    j = 0;
    do {
      if (_aCmdLineOption[j].sName == NULL) {  // End of list reached?
        //
        // Check if a J-Link Commander script file has been passed
        // In older versions of the commander it was allowed to pass a commander script file without specifying a specific command line option
        // We check if the current option is a valid commander file and if it is not, we just output that it is an unknown option
        //
        hFile = _OpenFile(s, FILE_FLAG_READ | FILE_FLAG_SHARE_READ);
        if (hFile == INVALID_HANDLE_VALUE) {
          _ReportOutf("Unknown command line option %s.\n", s);
          return -1;
        }
        _CloseFile(hFile);
        _HandleCmdLineCommanderScript(pCommanderSettings, s);
        break;
      }
      if (_JLinkstricmp(s, "?") == 0) { // "?" was called? => Special handling, as we do not expect a parameter.
        _HandleCmdLineHelp(NULL, NULL);
        break;
      }
      if (_JLinkstricmp(s, _aCmdLineOption[j].sName) == 0) {  // Cmd found?
        //
        // Execute command
        //
        if (i >= argc) {
          _ReportOutf("Missing command line parameter after command %s.\n", _aCmdLineOption[j].sName);
          return -1;
        }
        r = _aCmdLineOption[j].pfCmd(pCommanderSettings, argv[i++]);
        if (r < 0) {
          return -1;
        }
        break;
      }
      j++;
    } while(1);
  }
  return 0;
}

/*********************************************************************
*
*       main
*/
int main(int argc, char* argv[], char* envp[]) {
  char acRev[8] = {0};
  char ac[512];
  int Revision;
  int r = 0;
  int RevDLL;
  int VerDLL;

  (void)envp;
  SYS_timeBeginPeriod(1);         // Initialize system timers for usage
  _RegisterSysCloseReqHandlers(); // Make sure the application shuts down "gracefully" on close.
  //
  // Init host-interface related settings of commander with good defaults
  //
  memset(&_CommanderSettings, 0, sizeof(_CommanderSettings));
  _CommanderSettings.HostIF          = JLINKARM_HOSTIF_USB;       // Default interface is USB
  //
  // Init target-interface related settings of commander with good defaults
  //
  _CommanderSettings.TargetIF        = JLINKARM_TIF_JTAG;
  _CommanderSettings.InitTIFSpeedkHz = DEFAULT_FAST_SPEED;        // Default connect speed is 100 kHz
  //
  // Output commander version info
  //
  Revision = APP_VERSION % 100;
  if (Revision > 26) {
    Revision -= 26;
    acRev[0] = 'z';
    acRev[1] = '0' + (Revision / 10);
    acRev[2] = '0' + (Revision % 10);
    acRev[3] = 0;
  } else if (Revision > 0) {
    acRev[0] = 'a' + Revision - 1;
    acRev[1] = 0;
  } else {
    acRev[0] = 0;
  }
  _ReportOutf("SEGGER J-Link Commander V%d.%.2d%s (Compiled %s %s)\n", APP_VERSION / 10000, (APP_VERSION / 100) % 100, acRev, __DATE__, __TIME__);
  //
  // Show DLL info
  //
  VerDLL   = JLINKARM_GetDLLVersion();
  RevDLL   = VerDLL % 100;
  if (RevDLL > 26) {
    RevDLL -= 26;
    ac[0] = 'z';
    ac[1] = '0' + (RevDLL / 10);
    ac[2] = '0' + (RevDLL % 10);
    ac[3] = 0;
  } else if (RevDLL > 0) {
    ac[0] = 'a' + RevDLL - 1;
    ac[1] = 0;
  } else {
    ac[0] = 0;
  }
  _ReportOutf("DLL version V%d.%.2d%s, compiled %s\n\n", VerDLL / 10000, VerDLL / 100 % 100, ac, JLINKARM_GetCompileDateTime());
  //
  // Parse command line options, if any
  //
  r = _ParseCmdLine(&_CommanderSettings, argc, argv);
  if (r < 0) {                                         // Command line error? => Quit
    r = -1;
    _Sleep(3000);
    goto Done;
  }
  //
  // Decide if we need to enter interactive or command-file based mode
  //
  if (_CommanderSettings.ScriptModeActive) {
    r = _ScriptMode(&_CommanderSettings);
    if (r == 0) {
      if (_CommanderSettings.CloseRequested) {
        goto Done;
      }
      r = _InteractiveMode();
    }
  } else {
    r = _InteractiveMode();
  }
Done:
  _HandleBeforeQuit();
  r = (r < 0) ? 1 : 0;        // Map to Windows / Linux etc. exit code handling => 0 == O.K., else == error
  return r;
}

/*************************** end of file ****************************/
