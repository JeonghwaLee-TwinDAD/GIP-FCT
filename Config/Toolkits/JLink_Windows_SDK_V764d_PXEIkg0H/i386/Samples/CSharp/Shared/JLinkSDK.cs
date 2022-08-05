using System;
using System.Diagnostics;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

using U8  = System.Byte;   // unsigned char
using U16 = System.UInt16; // unsigned short
using U32 = System.UInt32; // unsigned int
using U64 = System.UInt64; // unsigned long long

using I8  = System.SByte; // signed char
using I16 = System.Int16; // signed short
using I32 = System.Int32; // signed int
using I64 = System.Int64; // signed long long

namespace JLinkSDK {
  [StructLayout(LayoutKind.Sequential)]
  public unsafe struct INTERNAL_JLINKARM_EMU_CONNECT_INFO {  // C# equivalent of C definition for JLINKARM_EMU_CONNECT_INFO, only used inside this module
    public U32 SerialNumber;
    public U32 Connection;
    public U32 USBAddr;
    public fixed U8 aIPAddr[16];
    public int Time;
    public U64 Time_us;
    public U32 HWVersion;
    public fixed U8 abMACAddr[6];
    public fixed byte acProduct[32];
    public fixed byte acNickName[32];
    public fixed byte acFWString[112];
    public byte IsDHCPAssignedIP;
    public byte IsDHCPAssignedIPIsValid;
    public byte NumIPConnections;
    public byte NumIPConnectionsIsValid;
    public fixed U8 aPadding[34];
  };
  public struct JLINKARM_EMU_CONNECT_INFO {  // Copy of INTERNAL_JLINKARM_EMU_CONNECT_INFO, except that it does not use any fixed-size arrays for convenience
    public U32 SerialNumber;
    public U32 Connection;
    public U32 USBAddr;
    public U8[] aIPAddr;
    public int Time;
    public U64 Time_us;
    public U32 HWVersion;
    public U8[] abMACAddr;
    public string acProduct;
    public string acNickName;
    public string acFWString;
    public byte IsDHCPAssignedIP;
    public byte IsDHCPAssignedIPIsValid;
    public byte NumIPConnections;
    public byte NumIPConnectionsIsValid;
  };

  public unsafe struct JLINK_FLASH_AREA_INFO {
    public U32 Addr;
    public U32 Size;
  };

  public struct JLINK_RAM_AREA_INFO {
    public U32 Addr;
    public U32 Size;
  };

  public unsafe struct JLINK_PCODE_INFO {
    public byte*             pPCode;          // Pointer to PCode.
    public U32               NumBytes;        // Length of PCode in bytes.
  };

  public unsafe struct JLINK_FLASH_BANK_INFO_EXT {
    public void*               pRAMCodeTurbo_LE;
    public void*               pRAMCodeTurbo_BE;
    public void*               pRAMCode_LE;
    public void*               pRAMCode_BE;
    public U32                 SizeRAMCodeTurbo_LE;
    public U32                 SizeRAMCodeTurbo_BE;
    public U32                 SizeRAMCode_LE;
    public U32                 SizeRAMCode_BE;
    public void*               pPCode;
    public U32                 SizePCode;
  };

  public unsafe struct JLINK_FLASH_BANK_INFO {
    public byte*             sBankName;
    public byte*             sAlgoFile;
    public U32               AlgoType;        // Really of type MAIN_FLASH_ALGO_TYPE but to avoid mutual inclusion, we choose U32 here for now...
    public U32               BaseAddr;
    public void*             paBlockInfo;     // For some algos, e.g. for SPIFI, this is just a default block info that may vary from target to target, so we need to request the actual one from the target via the RAMCode, at runtime
    public void*             pAlgoInfo;
  };

  [StructLayout(LayoutKind.Sequential)]
  public struct INTERNAL_JLINKARM_DEVICE_INFO {
    public U32                         SizeOfStruct;
    public string                      sName;
    public U32                         CoreId;
    public U32                         FlashAddr;
    public U32                         RAMAddr;
    public char                        EndianMode;
    public U32                         FlashSize;
    public U32                         RAMSize;
    public string                      sManu;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public JLINK_FLASH_AREA_INFO[]     aFlashArea;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 32)]
    public JLINK_RAM_AREA_INFO[]       aRAMArea;
    public U32                         Core;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public JLINK_FLASH_BANK_INFO[]     aFlashBank;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 10)]
    public JLINK_PCODE_INFO[]          aPCodes;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
    public JLINK_FLASH_BANK_INFO_EXT[] aFlashBankExt;
    public string                      sNote;
    public string                      sDeviceFamily;
  };

  public struct JLINKARM_DEVICE_INFO {    // Copy of INTENRAL_JLINKARM_DEVICE_INFO, except that it does not use any fixed-size arrays for convenience
    public string                      sName;
    public U32                         CoreId;
    public U32                         FlashAddr;
    public U32                         RAMAddr;
    public char                        EndianMode;
    public U32                         FlashSize;
    public U32                         RAMSize;
    public string                      sManu;
    public JLINK_FLASH_AREA_INFO[]     aFlashArea;
    public JLINK_RAM_AREA_INFO[]       aRAMArea;
    public U32                         Core;
    public JLINK_FLASH_BANK_INFO[]     aFlashBank;
    public JLINK_PCODE_INFO[]          aPCodes;
    public JLINK_FLASH_BANK_INFO_EXT[] aFlashBankExt;
    public string                      sNote;
    public string                      sDeviceFamily;
  };

  [StructLayout(LayoutKind.Sequential)]
  public struct INTERNAL_JTAG_ID_DATA {  // C# equivalent of C definition for JTAG_ID_DATA, only used inside this module
    public int NumDevices;
    public U16 ScanLen;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public U32[] aId;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public U8[] aScanLen;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public U8[] aIrRead;
    [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
    public U8[] aScanRead;
  }

  public struct JTAG_ID_DATA {  // Copy of INTERNAL_JTAG_ID_DATA, except that it does not use any fixed-size arrays for convenience
    public int NumDevices;
    public U16 ScanLen;
    public U32[] aId;
    public U8[] aScanLen;
    public U8[] aIrRead;
    public U8[] aScanRead;
  }  

  public class JLink {
    public const int HOSTIF_USB = (1 << 0);  // C# equivalent of C define for JLINKARM_HOSTIF_USB in JLinkARM_Const.h
    public const int HOSTIF_IP  = (1 << 1);  // C# equivalent of C define for JLINKARM_HOSTIF_IP  in JLinkARM_Const.h
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate void JLINK_LOG(string s);
    //
    // Determine J-Link DLL name depending on the architecture this exe was built for
    //
#if IS_ARCH_x86
    private const string sDLL = "JLinkARM.dll";
#else
    private const string sDLL = "JLink_x64.dll";
#endif
    //
    // NEVER call J-Link API functions directly
    // Always use the static wrapper functions that start with _
    //
    [DllImport(sDLL, EntryPoint = "JLINK_Open",              CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_Open              ();
    [DllImport(sDLL, EntryPoint = "JLINK_OpenEx",            CallingConvention = CallingConvention.StdCall)] unsafe private static extern string JLINK_OpenEx            (JLINK_LOG pfLog, JLINK_LOG pfErrorOut);
    [DllImport(sDLL, EntryPoint = "JLINK_IsOpen",            CallingConvention = CallingConvention.StdCall)] unsafe private static extern U8     JLINK_IsOpen            ();
    [DllImport(sDLL, EntryPoint = "JLINK_SelectIP",          CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_SelectIP          (string sHost, int Port);
    [DllImport(sDLL, EntryPoint = "JLINK_EMU_SelectByUSBSN", CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EMU_SelectByUSBSN (int SerialNo);
    [DllImport(sDLL, EntryPoint = "JLINK_ExecCommand",       CallingConvention = CallingConvention.StdCall)] unsafe private extern static int    JLINK_ExecCommand       (byte* pIn, byte* pOut, int BufferSize);
    [DllImport(sDLL, EntryPoint = "JLINK_TIF_Select",        CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_TIF_Select        (int Interface);
    [DllImport(sDLL, EntryPoint = "JLINK_SetSpeed",          CallingConvention = CallingConvention.StdCall)] unsafe private static extern void   JLINK_SetSpeed          (int Speed);
    [DllImport(sDLL, EntryPoint = "JLINK_Connect",           CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_Connect           ();
    [DllImport(sDLL, EntryPoint = "JLINK_Close",             CallingConvention = CallingConvention.StdCall)] unsafe private static extern void   JLINK_Close             ();
    [DllImport(sDLL, EntryPoint = "JLINK_IsConnected",       CallingConvention = CallingConvention.StdCall)] unsafe private static extern U8     JLINK_IsConnected       ();
    [DllImport(sDLL, EntryPoint = "JLINK_Reset",             CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_Reset             ();
    [DllImport(sDLL, EntryPoint = "JLINK_Go",                CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_Go                ();
    [DllImport(sDLL, EntryPoint = "JLINK_ReadMemEx",         CallingConvention = CallingConvention.StdCall)] unsafe private extern static int    JLINK_ReadMemEx         (U32 Addr, U32 NumBytes, void* pData, U32 AccessWidth);
    [DllImport(sDLL, EntryPoint = "JLINK_WriteMemEx",        CallingConvention = CallingConvention.StdCall)] unsafe private extern static int    JLINK_WriteMemEx        (U32 Addr, U32 NumBytes, void* pData, U32 AccessWidth);
    [DllImport(sDLL, EntryPoint = "JLINK_EraseChip",         CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EraseChip         ();
    [DllImport(sDLL, EntryPoint = "JLINK_DownloadFile",      CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_DownloadFile      (string sFileName, UInt32 Addr);
    [DllImport(sDLL, EntryPoint = "JLINK_EMU_GetList",       CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EMU_GetList       (int HostIFs, INTERNAL_JLINKARM_EMU_CONNECT_INFO * paConnectInfo, int MaxInfos);
    [DllImport(sDLL, EntryPoint = "JLINK_DEVICE_GetInfo",    CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_DEVICE_GetInfo    (int DeviceIndex, byte* p);
    [DllImport(sDLL, EntryPoint = "JLINK_Halt",              CallingConvention = CallingConvention.StdCall)] unsafe private static extern U8     JLINK_Halt              ();
    [DllImport(sDLL, EntryPoint = "JLINK_ConfigJTAG",        CallingConvention = CallingConvention.StdCall)] unsafe private static extern void   JLINK_ConfigJTAG        (U16 DeviceIndex);
    [DllImport(sDLL, EntryPoint = "JLINK_EMU_FILE_Delete",   CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EMU_FILE_Delete   (string sFile);
    [DllImport(sDLL, EntryPoint = "JLINK_EMU_FILE_Read",     CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EMU_FILE_Read     (string sFile, byte* pData, U32 Offset, U32 NumBytes);
    [DllImport(sDLL, EntryPoint = "JLINK_EMU_FILE_Write",    CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EMU_FILE_Write    (string sFile, byte* pData, U32 Offset, U32 NumBytes);
    [DllImport(sDLL, EntryPoint = "JLINK_EMU_FILE_GetSize",  CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_EMU_FILE_GetSize  (string sFile);    
    [DllImport(sDLL, EntryPoint = "JLINK_GetDeviceFamily",   CallingConvention = CallingConvention.StdCall)] unsafe private static extern int    JLINK_GetDeviceFamily   ();    
    [DllImport(sDLL, EntryPoint = "JLINK_GetId",             CallingConvention = CallingConvention.StdCall)] unsafe private static extern U32    JLINK_GetId             ();
    [DllImport(sDLL, EntryPoint = "JLINK_GetIdData",         CallingConvention = CallingConvention.StdCall)] unsafe private static extern void   JLINK_GetIdData         (byte* pIDData);

    /*********************************************************************
    *
    *       Open()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int Open() {
      int r;

      r = JLINK_Open();
      return r;
    }

    /*********************************************************************
    *
    *       OpenEx()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static string OpenEx(JLINK_LOG pfLog, JLINK_LOG pfErrorOut) {
      string sError;
      //
      // JLINK_LOG function pointers are safe from garbage collection, as delegates
      // are guaranteed to be alive during the call.
      //
      sError = JLINK_OpenEx(pfLog, pfErrorOut);
      return sError;
    }

    /*********************************************************************
    *
    *       Bytes2Str()
    *
    *  Function description
    *    Convenience function for converting a byte array to a string
    */
    public unsafe static string Bytes2Str(byte* p) {
      byte[] aTmp;
      int i;
      string s;

      aTmp = new byte[128];
      i    = 0;
      while (*(p + i) != '\0') {
        aTmp[i] = *(p + i);
        i++;
      }
      aTmp[i] = 0;
      s = System.Text.Encoding.ASCII.GetString(aTmp, 0, i);
      return s;
    }

    /*********************************************************************
    *
    *       GetDeviceInfo()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int GetDeviceInfo(int DeviceIndex, ref JLINKARM_DEVICE_INFO DeviceInfo) {
      byte[] abTmp;
      INTERNAL_JLINKARM_DEVICE_INFO IntInfo;
      int r;
      
      r = System.Runtime.InteropServices.Marshal.SizeOf(typeof(INTERNAL_JLINKARM_DEVICE_INFO));
      abTmp = BitConverter.GetBytes(r);  // First 4 bytes are <SizeOfStruct> member
      Array.Resize(ref abTmp, r);
      //
      // Fixed blocks are necessary to inhibit that garbage collector of C#
      // relocates memory blocks, which would make our pointers etc. invalid while in the J-Link DLL
      // This could cause random crashes that are ultra-hard to locate
      //
      fixed (byte* p = abTmp) {
        r = JLINK_DEVICE_GetInfo(DeviceIndex, p);
        IntInfo = (INTERNAL_JLINKARM_DEVICE_INFO)Marshal.PtrToStructure((IntPtr)p, typeof(INTERNAL_JLINKARM_DEVICE_INFO));  // Map byte array to struct
      }
      DeviceInfo.CoreId        = IntInfo.CoreId;
      DeviceInfo.FlashAddr     = IntInfo.FlashAddr;
      DeviceInfo.RAMAddr       = IntInfo.RAMAddr;
      DeviceInfo.EndianMode    = IntInfo.EndianMode;
      DeviceInfo.FlashSize     = IntInfo.FlashSize;
      DeviceInfo.RAMSize       = IntInfo.RAMSize;
      DeviceInfo.Core          = IntInfo.Core;
      DeviceInfo.sName         = IntInfo.sName;
      DeviceInfo.sManu         = IntInfo.sManu;
      DeviceInfo.sNote         = IntInfo.sNote;
      DeviceInfo.sDeviceFamily = IntInfo.sDeviceFamily;
      DeviceInfo.aFlashArea    = IntInfo.aFlashArea;
      DeviceInfo.aRAMArea      = IntInfo.aRAMArea;
      DeviceInfo.aFlashBank    = IntInfo.aFlashBank;
      DeviceInfo.aPCodes       = IntInfo.aPCodes;
      DeviceInfo.aFlashBankExt = IntInfo.aFlashBankExt;
      return r;
    }

    /*********************************************************************
    *
    *       GetEmuList()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int GetEmuList(int HostIFs, JLINKARM_EMU_CONNECT_INFO[] aConnectInfo, int MaxInfos) {
      U32 i;
      U32 j;
      int r;
      INTERNAL_JLINKARM_EMU_CONNECT_INFO[] aIntInfo;

      aIntInfo = new INTERNAL_JLINKARM_EMU_CONNECT_INFO[MaxInfos];
      //
      // Fixed blocks are necessary to inhibit that garbage collector of C#
      // relocates memory blocks, which would make our pointers etc. invalid while in the J-Link DLL
      // This could cause random crashes that are ultra-hard to locate
      //
      fixed (INTERNAL_JLINKARM_EMU_CONNECT_INFO* paConnectInfo = aIntInfo) {
        r = JLINK_EMU_GetList(HostIFs, paConnectInfo, MaxInfos);
      }
      //
      // Convert internal info to public info
      //
      MaxInfos = (MaxInfos < r) ? MaxInfos : r;
      for (i = 0; i < MaxInfos; i++) {
        aConnectInfo[i].Connection              = aIntInfo[i].Connection;
        aConnectInfo[i].HWVersion               = aIntInfo[i].HWVersion;
        aConnectInfo[i].IsDHCPAssignedIP        = aIntInfo[i].IsDHCPAssignedIP;
        aConnectInfo[i].IsDHCPAssignedIPIsValid = aIntInfo[i].IsDHCPAssignedIPIsValid;
        aConnectInfo[i].NumIPConnections        = aIntInfo[i].NumIPConnections;
        aConnectInfo[i].NumIPConnectionsIsValid = aIntInfo[i].NumIPConnectionsIsValid;
        aConnectInfo[i].SerialNumber            = aIntInfo[i].SerialNumber;
        aConnectInfo[i].Time                    = aIntInfo[i].Time;
        aConnectInfo[i].Time_us                 = aIntInfo[i].Time_us;
        aConnectInfo[i].USBAddr                 = aIntInfo[i].USBAddr;
        aConnectInfo[i].aIPAddr                 = new byte[16];
        aConnectInfo[i].abMACAddr               = new byte[6];
        fixed (byte* p = aIntInfo[i].aIPAddr) {
          for (j = 0; j < 16; j++) {
            aConnectInfo[i].aIPAddr[j] = *(p + j);
          }
        }
        fixed (byte* p = aIntInfo[i].abMACAddr) {
          for (j = 0; j < 6; j++) {
            aConnectInfo[i].abMACAddr[j] = *(p + j);
          }
        }
        fixed (byte* p = aIntInfo[i].acProduct) {
          aConnectInfo[i].acProduct = Bytes2Str(p);
        }
        fixed (byte* p = aIntInfo[i].acNickName) {
          aConnectInfo[i].acNickName = Bytes2Str(p);
        }
        fixed (byte* p = aIntInfo[i].acFWString) {
          aConnectInfo[i].acFWString = Bytes2Str(p);
        }
      }
      return r;
    }

    /*********************************************************************
    *
    *       IsConnected()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static U8 IsOpen() {
      U8 r;

      r = JLINK_IsOpen();
      return r;
    }

    /*********************************************************************
    *
    *       IsConnected()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static U8 IsConnected() {
      U8 r;

      r = JLINK_IsConnected();
      return r;
    }

    /*********************************************************************
    *
    *       Reset()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int Reset() {
      int r;

      r = JLINK_Reset();
      return r;
    }

    /*********************************************************************
    *
    *       Go()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int Go() {
      int r;

      r = JLINK_Go();
      return r;
    }

    /*********************************************************************
    *
    *       SelectIP()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int SelectIP(string sHost, int Port) {
      int r;

      r = JLINK_SelectIP(sHost, Port);
      return r;
    }

    /*********************************************************************
    *
    *       EMU_SelectByUSBSN()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int EMU_SelectByUSBSN(int SerialNo) {
      int r;

      r = JLINK_EMU_SelectByUSBSN(SerialNo);
      return r;
    }

    /*********************************************************************
    *
    *       ExecCommand()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int ExecCommand(byte[] sIn, byte[] sOut, int BufferSize) {
      int r;
      //
      // Fixed blocks are necessary to inhibit that garbage collector of C#
      // relocates memory blocks, which would make our pointers etc. invalid while in the J-Link DLL
      // This could cause random crashes that are ultra-hard to locate
      //
      r = 0;
      fixed (byte* pIn = sIn) {
        fixed (byte* pOut = sOut) {
          r = JLINK_ExecCommand(pIn, pOut, BufferSize);
        }
      }
      return r;
    }

    /*********************************************************************
    *
    *       TIF_Select()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int TIF_Select(int Interface) {
      int r;

      r = JLINK_TIF_Select(Interface);
      return r;
    }

    /*********************************************************************
    *
    *       SetSpeed()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static void SetSpeed(int Speed) {
      JLINK_SetSpeed(Speed);
    }

    /*********************************************************************
    *
    *       Connect()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int Connect() {
      int r;

      r = JLINK_Connect();
      return r;
    }

    /*********************************************************************
    *
    *       Close()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static void Close() {
      JLINK_Close();
    }

    /*********************************************************************
    *
    *       ReadMemEx() - overloaded for U8
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int ReadMemEx(U32 Addr, U32 NumBytes, U8[] pData) {
      int r;

      fixed (U8* pD = pData) {
        r = JLINK_ReadMemEx(Addr, NumBytes, (void*)pD, 4);
      }
      return r;
    }

    /*********************************************************************
    *
    *       ReadMemEx() - overloaded for U16
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int ReadMemEx(U32 Addr, U32 NumBytes, U16[] pData) {
      int r;

      fixed (U16* pD = pData) {
        r = JLINK_ReadMemEx(Addr, NumBytes, (void*)pD, 4);
      }
      return r;
    }

    /*********************************************************************
    *
    *       ReadMemEx() - overloaded for U32
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int ReadMemEx(U32 Addr, U32 NumBytes, U32[] pData) {
      int r;

      fixed (U32* pD = pData) {
        r = JLINK_ReadMemEx(Addr, NumBytes, (void*)pD, 4);
      }
      return r;
    }

    /*********************************************************************
    *
    *       WriteMemEx() - overloaded for U8
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int WriteMemEx(U32 Addr, U32 NumBytes, U8[] pData) {
      int r;

      fixed (U8* pD = pData) {
        r = JLINK_WriteMemEx(Addr, NumBytes, (void*)pD, 1);
      }
      return r;
    }

    /*********************************************************************
    *
    *       WriteMemEx() - overloaded for U16
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int WriteMemEx(U32 Addr, U32 NumBytes, U16[] pData) {
      int r;

      fixed (U16* pD = pData) {
        r = JLINK_WriteMemEx(Addr, NumBytes, (void*)pD, 1);
      }
      return r;
    }

    /*********************************************************************
    *
    *       WriteMemEx() - overloaded for U32
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int WriteMemEx(U32 Addr, U32 NumBytes, U32[] pData) {
      int r;

      fixed (U32* pD = pData) {
        r = JLINK_WriteMemEx(Addr, NumBytes, (void*)pD, 1);
      }
      return r;
    }

    /*********************************************************************
    *
    *       EraseChip()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int EraseChip() {
      int r;
      r = JLINK_EraseChip();
      return r;
    }

    /*********************************************************************
    *
    *       DownloadFile()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int DownloadFile(string sFileName, UInt32 Addr) {
      int r;
      //
      // Strings that are not changed by the DLL can be passed as such, without fixation needed,
      // as they are call by value.
      //
      r = 0;
      r = JLINK_DownloadFile(sFileName, Addr);
      return r;
    }

    
    /*********************************************************************
    *
    *       Halt()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static U8 Halt() {
      U8 r;

      r = JLINK_Halt();
      return r;
    }

    /*********************************************************************
    *
    *       ConfigJTAG()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static void ConfigJTAG(U16 DeviceIndex) {
      JLINK_ConfigJTAG(DeviceIndex);
    }

    /*********************************************************************
    *
    *       EMU_FILE_Delete()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int EMU_FILE_Delete(string sFile) {
      int r;

      r = JLINK_EMU_FILE_Delete(sFile);
      return r;
    }

    /*********************************************************************
    *
    *       EMU_FILE_Read()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int EMU_FILE_Read(string sFile, byte[] pData, U32 Offset, U32 NumBytes) {
      int r;

      r = 0;
      fixed (byte* p = pData) {
        r = JLINK_EMU_FILE_Read(sFile, p, Offset, NumBytes);
      }
      return r;
    }

    /*********************************************************************
    *
    *       EMU_FILE_Write()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int EMU_FILE_Write(string sFile, byte[] pData, U32 Offset, U32 NumBytes) {
      int r;

      r = 0;
      fixed (byte* p = pData) {
        r = JLINK_EMU_FILE_Write(sFile, p, Offset, NumBytes);
      }
      return r;
    }

    /*********************************************************************
    *
    *       EMU_FILE_GetSize()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int EMU_FILE_GetSize(string sFile) {
      int r;

      r = JLINK_EMU_FILE_GetSize(sFile);
      return r;
    }

    /*********************************************************************
    *
    *       GetDeviceFamily()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static int GetDeviceFamily() {
      int r;

      r = JLINK_GetDeviceFamily();
      return r;
    }

    /*********************************************************************
    *
    *       GetId()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static U32 GetId() {
      U32 r;

      r = JLINK_GetId();
      return r;
    }

    /*********************************************************************
    *
    *       GetIdData()
    *
    *  Function description
    *    Wrapper function for DLL call
    */
    public unsafe static void GetIdData(ref JTAG_ID_DATA IDData) {
      byte[] abTmp;
      INTERNAL_JTAG_ID_DATA IntData;
      U32 j;
      int r;
      
      r = System.Runtime.InteropServices.Marshal.SizeOf(typeof(INTERNAL_JTAG_ID_DATA));
      abTmp = new byte[r];
      //
      // Fixed blocks are necessary to inhibit that garbage collector of C#
      // relocates memory blocks, which would make our pointers etc. invalid while in the J-Link DLL
      // This could cause random crashes that are ultra-hard to locate
      //
      fixed (byte* p = abTmp) {
        JLINK_GetIdData(p);
        IntData = (INTERNAL_JTAG_ID_DATA)Marshal.PtrToStructure((IntPtr)p, typeof(INTERNAL_JTAG_ID_DATA));  // Map byte array to struct
      }
      IDData.NumDevices  = IntData.NumDevices;
      IDData.ScanLen     = IntData.ScanLen;
      IDData.aId         = new U32[3];
      IDData.aIrRead     = new byte[3];
      IDData.aScanLen    = new byte[3];
      IDData.aScanRead   = new byte[3];
      fixed (U32* p = IntData.aId) {
        for (j = 0; j < 3; j++) {
          IDData.aId[j] = *(p + j);
        }
      }
      fixed (byte* p = IntData.aIrRead) {
        for (j = 0; j < 3; j++) {
          IDData.aIrRead[j] = *(p + j);
        }
      }
      fixed (byte* p = IntData.aScanLen) {
        for (j = 0; j < 3; j++) {
          IDData.aScanLen[j] = *(p + j);
        }
      }
      fixed (byte* p = IntData.aScanRead) {
        for (j = 0; j < 3; j++) {
          IDData.aScanRead[j] = *(p + j);
        }
      }
    }
  }
}
