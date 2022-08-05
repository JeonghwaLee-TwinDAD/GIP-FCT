/*********************************************************************
*              SEGGER MICROCONTROLLER SYSTEME GmbH                   *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*          (c) 2006 SEGGER Microcontroller Systeme GmbH              *
*                                                                    *
*       Internet: www.segger.com Support: support@segger.com         *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File    : ReadId.c
Purpose : JTAG sample program
---------------------------END-OF-HEADER------------------------------
*/
#include <stdio.h>
#include "JLinkARMDLL.h"

/*********************************************************************
*
*       _cbErrorOut
*/
static void _cbErrorOut(const char *s) {
  printf("\n ERROR: %s\n", s);
}

/*********************************************************************
*
*       _ReadId
*/
static void _ReadId(void) {
  U8 TMS     = 0x1f;
  U8 aTDI[4] = {0};
  int BitPos;
  U32 Id;
  //
  // Reset Tap controller and go idle
  //  
  JLINKARM_JTAG_StoreRaw(&aTDI[0], &TMS, 6);
  //
  // Shifting 32 bit in data scan chain
  //
  BitPos = JLINKARM_JTAG_StoreData(&aTDI[0], 32);
  //
  // Transfer output buffers to JTAG device and fill input buffer
  //
  JLINKARM_JTAG_SyncBits();
  Id = JLINKARM_JTAG_GetU32(BitPos);
  printf("JTAG-Id = 0x%.8X\n", Id);
}

/*********************************************************************
*
*       main
*/
int main(void) {
	const char* sErr;

  printf("SEGGER JTAG Sample\n\n");

  sErr = JLINKARM_OpenEx(NULL, _cbErrorOut);

  if (sErr == 0) {
    JLINKARM_JTAG_GetDeviceId(0);
    _ReadId();
  }
  JLINKARM_Close();
  return 0;
}