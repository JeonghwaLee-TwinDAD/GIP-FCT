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
File    : TestHW.c
Purpose : J-Link hardware test program
---------------------------END-OF-HEADER------------------------------
*/

#include <stdio.h>
#include <string.h>
#ifdef _MSC_VER
  #include <conio.h>
#endif
#include "JLinkARMDLL.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define RAMADDR (0x40000000)

/*********************************************************************
*
*       _cbErrorOut
*/
static void _cbErrorOut(const char *s) {
  printf("\n ERROR: %s\n", s);
}

/*********************************************************************
*
*       _CheckTargetPower
*/
static char _CheckTargetPower(void) {
  JLINKARM_HW_STATUS Stat;
  printf("Checking target power... ");
  if (JLINKARM_GetHWStatus(&Stat) != 0) {
    return 1;   // Error
  }
  if ((Stat.VTarget < 2500) || (Stat.VTarget > 3500)) {
    printf("FAILED!\n");
    return 1;   // Error
  }
  printf("O.K.\n");
  return 0;     // O.K.
}

/*********************************************************************
*
*       _CheckCoreId
*/
static char _CheckCoreId(void) {
  U32 Id;
  printf("Checking core Id... ");
  Id = JLINKARM_GetId();
  if (Id != 0x4F1F0F0F) {
    printf("FAILED!\n");
    return 1;   // Error
  }
  printf("O.K.\n");
  return 0;     // O.K.
}

/*********************************************************************
*
*       _CheckETM
*/
static char _CheckETM(void) {
  printf("Checking ETM... ");
  if (JLINKARM_ETM_IsPresent() != 0x12) {
    printf("FAILED!\n");
    return 1;   // Error
  }
  printf("O.K.\n");
  return 0;     // O.K.
}

/*********************************************************************
*
*       _ExecReset
*/
static char _ExecReset(void) {
  printf("Executing reset... ");
  JLINKARM_SetSpeed(5);
  JLINKARM_SetResetType(JLINKARM_RESET_TYPE_NORMAL);
  JLINKARM_SetResetDelay(0);
  JLINKARM_ResetPullsRESET(1);
  JLINKARM_Reset();
  if (JLINKARM_HasError()) {
    printf("FAILED!\n");
    return 1;   // Error
  }
  JLINKARM_SetSpeed(JLINKARM_SPEED_ADAPTIVE);
  printf("O.K.\n");
  return 0;     // O.K.
}

/*********************************************************************
*
*       _CheckMemory
*/
static char _CheckMemory(void) {
  char acWr[256];
  char acRd[256];
  int i;
  printf("Checking memory... ");
  for (i = 0; i < sizeof(acWr); i++) {
    acWr[i] = i % 255;
  }
  JLINKARM_WriteMem(RAMADDR, sizeof(acWr), acWr);
  memset(acRd, 0, sizeof(acRd));
  JLINKARM_ReadMem (RAMADDR, sizeof(acRd), acRd);
  if (memcmp(acRd, acWr, sizeof(acRd))) {
    printf("FAILED!\n");
    return 1;   // Error
  }
  printf("O.K.\n");
  return 0;     // O.K.
}

/*********************************************************************
*
*       main
*/
int main(void) {
  const char* sErr;
  char r = 0;

  printf("SEGGER J-Link Hardware Test\n\n");
  printf("Please connect J-Link to IAR LPC-P213X board\nand press any key to test J-Link.\n\n");
#ifdef _MSC_VER
  getch();
#endif
  
  sErr = JLINKARM_OpenEx(NULL, _cbErrorOut);
  if (sErr) {
    printf("%s\n", sErr);
    r = 1;
  }
  if (r == 0) {
    r = _CheckTargetPower();
  }
  if (r == 0) {
    r = _ExecReset();
  }
  if (r == 0) {
    r = _CheckCoreId();
  }
  if (r == 0) {
    r = _CheckETM();
  }
  if (r == 0) {
    r = _CheckMemory();
  }
  JLINKARM_Close();
  if (r) {
    printf("\nJ-Link test failed !\n");
  } else {
    printf("\nTested J-Link is O.K. !\n");
  }
  return 0;
}

/*************************** end of file ****************************/
