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
GLOBAL_DISABLE_WARNINGS()   // Avoid that 3rd party headers throw warnings
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
  #include <conio.h>
  #include <windows.h>
#endif
#include <string.h>
GLOBAL_ENABLE_WARNINGS()

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/

#define PIPESTAT_IE 0     // Instruction executed
#define PIPESTAT_ID 1     // Instruction executed with data
#define PIPESTAT_IN 2     // Instruction NOT executed
#define PIPESTAT_WT 3     // Wait
#define PIPESTAT_BE 4     // Branch executed
#define PIPESTAT_BD 5     // Branch with data
#define PIPESTAT_TR 6     // Trigger
#define PIPESTAT_TD 7     // Trace disable

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
  U32 NumAddrBits;
  U32 ReasonCode;
} BRANCH_INFO;


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/


/*********************************************************************
*
*       _PipeStat2String
*/
static const char * _PipeStat2String(U32 PipeStat) {
  switch (PipeStat) {
  case 0: return "IE";
  case 1: return "ID";
  case 2: return "IN";
  case 3: return "WT";
  case 4: return "BE";
  case 5: return "BD";
  case 6: return "TR";
  case 7: return "TD";
  }
  return "Ill. pipestat";
}

/*********************************************************************
*
*       _CondCode2String
*/
static const char * _CondCode2String(U32 cc) {
  switch (cc) {
  case 0: return "EQ";
  case 1: return "NE";
  case 2: return "CS";
  case 3: return "CC";
  case 4: return "MI";
  case 5: return "PL";
  case 6: return "VS";
  case 7: return "VC";
  case 8: return "HI";
  case 9: return "LS";
  case 10: return "GE";
  case 11: return "LT";
  case 12: return "GT";
  case 13: return "LE";
  case 14: return "";
  }
  return " *** Error-- Ill. Condition code";
}


/*********************************************************************
*
*       _GetNumRegions
*/
static U32 _GetNumRegions(void) {
  U32 NumRegions;
  JLINKARM_TRACE_Control (JLINKARM_TRACE_CMD_GET_NUM_REGIONS, &NumRegions);
  return NumRegions;
}

/*********************************************************************
*
*       _GetNumSamples
*/
static U32 _GetNumSamples(void) {
  U32 NumSamples;
  JLINKARM_TRACE_Control (JLINKARM_TRACE_CMD_GET_NUM_SAMPLES, &NumSamples);
  return NumSamples;
}

/*********************************************************************
*
*       _GetRegionPropsEx
*/
static void _GetRegionPropsEx(U32 RegionIndex, JLINKARM_TRACE_REGION_PROPS_EX* pPropsEx) {
  pPropsEx->SizeofStruct  = sizeof(JLINKARM_TRACE_REGION_PROPS_EX);
  pPropsEx->RegionIndex   = RegionIndex;
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_GET_REGION_PROPS_EX, (U32*)pPropsEx);
}

/*********************************************************************
*
*       _CheckInstruction
*/
static void _CheckInstruction(JLINKARM_TRACE_DATA * pItem, U32 * pAddr) {
  U32 Inst;
  U32 Addr;
  U32 BranchAddr;
  char IsBranch;
  I32 Off;
  static I32 _BranchOff;

  BranchAddr = 0;  // Avoid false-positive "unintinialized variable" warning
  Addr = *pAddr;
  Inst = 0;
  IsBranch = 0;
  //
  // Read instruction
  //
  if (Addr & 1) {
    JLINKARM_ReadCodeMem(Addr & ~1, 2, &Inst);
    printf(" %.4X", Inst);
  }  else {
    JLINKARM_ReadCodeMem(Addr & ~1, 4, &Inst);
    printf(" %.8X", Inst);
  }

  //
  // Handle executed branches
  //
  if ((pItem->PipeStat == PIPESTAT_IE) || (pItem->PipeStat == PIPESTAT_ID)) {
    if (Addr & 1) {
      if ((Inst & 0xF000) == 0xD000) {      // Conditional branch
        IsBranch = 1;
        Off = 2 * (Inst & 0xFF);
        Off = (Off << 23) >> 23;       // Sign extend
        BranchAddr = Addr + 4 + Off;
        printf(" B%s ", _CondCode2String((Inst >> 8) & 15));
      } else if ((Inst & 0xF800) == 0xE000) {      // Unconditional branch (A7-20)
        IsBranch = 1;
        Off = 2 * (Inst & 0xFF);
        Off = (Off << 23) >> 23;       // Sign extend
        BranchAddr = Addr + 4 + Off;
        printf(" B ");
      } else if ((Inst & 0xF800) == 0xF000) {      // First part of double-instruction branch (A7-26)
        Off = Inst & 0x7FF;
        Off <<= 21;             // Sign extend
        Off >>= 9;              // Shift to final position
        _BranchOff = Off + 2;
      } else if ((Inst & 0xF800) == 0xF800) {      // BL
        IsBranch = 1;
        BranchAddr = Addr + _BranchOff + ((Inst & 0x7FF) << 1);
        printf(" BL ");
      } else if ((Inst & 0xF800) == 0xE800) {      // BLX
        IsBranch = 1;
        BranchAddr = Addr + _BranchOff + ((Inst & 0x7FF) << 1);
        BranchAddr &= ~1;                          // Next inst is in arm mode
        if (BranchAddr & 0x2) {
          BranchAddr &= ~2;
          printf(" *** Error: BLX can only jump to 4-byte aligned addresses.");
        }
        printf(" BLX");
      }
    }
  }
  //
  // Update Addr
  //
  if (IsBranch) {
    *pAddr = BranchAddr;
    printf(" -> 0x%.8X", BranchAddr & ~1);
  } else {
    *pAddr += (Addr & 1) ? 2 : 4;
  }
}


/*********************************************************************
*
*       _HandleBranch
*/
static void _HandleBranch(const JLINKARM_TRACE_DATA * pItem, int NumItems, U32 * pLastBranchAddr, U32 * pAddr, char *pAddrIsvalid
                        ,BRANCH_INFO * pBranchInfo, U32 TraceFormat)
{
  U32 Addr;
  int apo;
  U32 ByteCnt;
  U32 Mask;
  U8 Packet;

  pBranchInfo->NumAddrBits = 0;
  if ((NumItems) < 3) {
    printf("\n******* Error: not enough packets for APO ******");
    *pAddrIsvalid = 0;
    return;
  }
  apo  = (pItem + 1)->PipeStat & 3;
  apo |= ((pItem + 2)->PipeStat & 3) << 2;
  //
  // Search sync
  //
  while (NumItems) {
    if (pItem->Sync) {
      if (apo-- == 0) {
        goto LoadAddr;
      }
    }
    pItem++;
    NumItems--;
  }
  printf("\n******* Error: Sync point not in buffer ******");
  *pAddrIsvalid = 0;
  return;
  //
  // Load addr
  //
LoadAddr:
  ByteCnt = 0;
  Addr = 0;
  if (TraceFormat & JLINKARM_TRACE_FORMAT_4BIT) {
    while (NumItems > 1) {
      Packet = pItem->Packet + ((pItem + 1)->Packet << 4);
      pItem    += 2;
      NumItems -= 2;
      Addr |= (Packet & 127) << (7 * ByteCnt);
      ByteCnt++;
      if ((Packet & 0x80) == 0) {     // Last data packet ?
        goto HandleLastPacket;
      }
    }
  } else if (TraceFormat & JLINKARM_TRACE_FORMAT_8BIT) {
    while (NumItems > 0) {
      Packet = (U8)pItem->Packet;
      pItem    += 1;
      NumItems -= 1;
      Addr |= (Packet & 127) << (7 * ByteCnt);
      ByteCnt++;
      if ((Packet & 0x80) == 0) {     // Last data packet ?
        goto HandleLastPacket;
      }
    }
  } else {
    printf("\n******* Error: Unsupported trace format ******");
    return;
  }
  printf("\n******* Error: not enough packets for Addr. ******");
  *pAddrIsvalid = 0;
  return;

HandleLastPacket:
  if (ByteCnt == 5) {   // Full addr
    *pAddr = Addr;
    *pAddrIsvalid = 1;
    pBranchInfo->NumAddrBits = 32;
    pBranchInfo->ReasonCode  = (Packet >> 4) & 7;
    *pLastBranchAddr = Addr;
  } else {
    Mask = (1 << (7 * ByteCnt)) - 1;
    *pAddr = (*pLastBranchAddr & ~Mask) | Addr;
    pBranchInfo->NumAddrBits = 7 * ByteCnt;
  }
}


/*********************************************************************
*
*       _GetRegionProps
*/
static void _ShowRegionDetails(U32 RegionIndex) {
  U32 u;
  JLINKARM_TRACE_DATA * pData;
  U32 PipePos;
  char AddrIsValid;
  U32 Addr;
  U32 LastBranchAddr;
  U32 TraceFormat;
  JLINKARM_TRACE_REGION_PROPS_EX PropsEx;

  AddrIsValid = 0;
  Addr = 0;  // Avoid false-positive "unintinialized variable" warning
  _GetRegionPropsEx(RegionIndex, &PropsEx);
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_GET_FORMAT, &TraceFormat);
  pData = malloc(PropsEx.NumSamples * sizeof(JLINKARM_TRACE_DATA));
  PipePos = 0;
  if (pData) {
    U32 NumItems;
    NumItems = PropsEx.NumSamples;
    JLINKARM_TRACE_Read(pData, PropsEx.Off, &NumItems);
    for (u = 0; u < NumItems; u++) {
      const char *sPipeStat;
      JLINKARM_TRACE_DATA *pItem;
      pItem = pData + u;

      printf ("%.4d - RCnt%.2d - R%.2d-%.4d - %s", u + PropsEx.Off, PropsEx.RegionCnt, RegionIndex, u, pItem->Sync ? "S" : " ");
      printf ("%d %X", pItem->PipeStat, pItem->Packet);
      //
      // Analyze trace
      //
      if (pItem->PipeStat == PIPESTAT_BE) {
        BRANCH_INFO BranchInfo;
        printf(" BE");
        if (AddrIsValid) {
          printf(" 0x%.8X", Addr & ~1);
          printf((Addr & 1) ? "-T" : "-A");
        } else {
          printf(" 0xxxxxxxxx-?");
        }
        _HandleBranch(pItem, NumItems - u, &LastBranchAddr, &Addr, &AddrIsValid, &BranchInfo, TraceFormat);
        if (AddrIsValid) {
          printf(" -> 0x%.8X", Addr & ~1);
          printf((Addr & 1) ? "-T" : "-A");
        } else {
          printf(" Addr = ???");
        }
        if (BranchInfo.NumAddrBits == 32) {
          printf(" R %d", BranchInfo.ReasonCode);
          switch(BranchInfo.ReasonCode) {
          case 0: printf(", PC change"); break;    // Periodic sync point ETM V1.1 and eralier
          case 1: printf(", Trace enable"); break;    // Periodic sync point ETM V1.1 and eralier
          case 2: printf(", Trace restart"); break;    // Periodic sync point ETM V1.1 and eralier
          case 3: printf(", Exit from debug"); break;    // Periodic sync point ETM V1.1 and eralier
          case 4: printf(", Per. Sync"); break;    // Periodic sync point ETM V1.1 and eralier
          }
        } else {
          printf(" %d Bits", BranchInfo.NumAddrBits);
        }
        PipePos = u + 3;
      }
      //
      // Show pipestat and addr.
      //
      if (PipePos == u) {
        sPipeStat = _PipeStat2String(pItem->PipeStat);
        printf(" %s", sPipeStat);
        PipePos++;
        switch (pItem->PipeStat) {
        case PIPESTAT_IE:
        case PIPESTAT_ID:
        case PIPESTAT_IN:
          if (AddrIsValid) {
            printf(" 0x%.8X", Addr & ~1);
            printf((Addr & 1) ? "-T" : "-A");
            _CheckInstruction(pItem, &Addr);
          } else {
            printf(" 0xxxxxxxxx-? (Waiting for sync point)");
          }
          break;
        case PIPESTAT_WT:
        case PIPESTAT_BE:
        case PIPESTAT_BD:
        case PIPESTAT_TR:
        case PIPESTAT_TD:
          ;
        }
      }
      printf("\n");
    }
  }
  free(pData);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/


/*********************************************************************
*
*       TRACE_ShowStatus
*
* Function description
*   Shows the status of the trace unit
*/
void TRACE_ShowStatus(void) {
  U32 ConfSize;
  U32 MinSize;
  U32 MaxSize;
  U32 TraceFormat;

  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_GET_MAX_CAPACITY, &MaxSize);
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_GET_MIN_CAPACITY, &MinSize);
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_GET_CONF_CAPACITY, &ConfSize);
  printf("Trace buffer size: %dKB (%dKB Min, %dKB Max)\n", ConfSize >> 10, MinSize >> 10, MaxSize >> 10);
  JLINKARM_TRACE_Control(JLINKARM_TRACE_CMD_GET_FORMAT, &TraceFormat);
  if (TraceFormat) {
    printf("Trace format: %s%s%s%s\n", (TraceFormat & JLINKARM_TRACE_FORMAT_4BIT) ? "4-bit" : ""
           , (TraceFormat & JLINKARM_TRACE_FORMAT_8BIT)  ? "8-bit"  : ""
           , (TraceFormat & JLINKARM_TRACE_FORMAT_16BIT) ? "16-bit" : ""
           , (TraceFormat & JLINKARM_TRACE_FORMAT_DOUBLE_EDGE) ? "Halfrate" : " "
           );
  } else {
    printf("Trace format not selected.\n");
  }
}


/*********************************************************************
*
*       TRACE_ShowRegions
*
* Function description
*   Shows the number and contents of the trace regions
*/
void TRACE_ShowRegions(void) {
  U32 u;
  U32 NumRegions;
  U32 NumSamples;
  JLINKARM_TRACE_REGION_PROPS_EX PropsEx;

  TRACE_ShowStatus();
  //
  // Print Number of regions & samples
  //
  NumRegions = _GetNumRegions();
  NumSamples = _GetNumSamples();
  printf("%d Samples in %d region(s)\n", NumSamples, NumRegions);
  //
  // Print region info
  //
  for (u = 0; u < NumRegions; u++) {
    _GetRegionPropsEx(u, &PropsEx);
    printf("Region %d: %d samples @%d\n", u, PropsEx.NumSamples, PropsEx.Off);
  }
  //
  // Print region details
  //
  for (u = 0; u < NumRegions; u++) {
    _ShowRegionDetails(NumRegions - 1 - u);
  }
}



/*************************** end of file ****************************/
