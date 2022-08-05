#/*********************************************************************
#*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
#*        Solutions for real time microcontroller applications        *
#**********************************************************************
#*                                                                    *
#*        (C) 2004-2019    SEGGER Microcontroller Systeme GmbH        *
#*                                                                    *
#*      Internet: www.segger.com    Support:  support@segger.com      *
#*                                                                    *
#**********************************************************************
#----------------------------------------------------------------------
#File        : jlinksdk_StartupSequence.py
#Purpose     : Implementation of the jlink startup sequence using the jlinksdk python package
#---------------------------END-OF-HEADER------------------------------
#*/
import jlinksdk

#/*********************************************************************
#*
#*       "Defines"
#*
#**********************************************************************
#*/

#**********************************************************************
#*
#*       Execution starts here
#*
#**********************************************************************
#*/
#
# Select and configure host interface (USB is default)
#
jlink = jlinksdk.JLink()
print("DLL Version: %s" % jlink.GetDllVersionString())
v = input("Enter the host interface.\n  0> USB (Default)\n  1> IP\nHostIF> ")
if (v == '1'):  # User entered '1'? => Select IP as HIF
  HostIF = jlinksdk.HOST_IF.TCPIP
else:           # Use USB as default
  HostIF = jlinksdk.HOST_IF.USB
#
# Get serial number
#
SN = input("Enter J-Link serial number\nSN> ")
if (SN == ""):                                                 # No serial number selected? --> Use 0 as default
  SN = 0
SN = int(SN)
#
# Open connection to J-Link
#
jlink = jlinksdk.JLink()
jlink.Open(HostIF = HostIF, SN = SN)
#
# Select device or core
#
sDevice = input("Please specify device / core. <Default>: unspecified\nDevice> ")
#
# Select and configure target interface
#
TIF = input("Please specify target interface:\n  0> JTAG\n  1> SWD (Default)\nTIF> ")
if (TIF == '0'):
  TIF = jlinksdk.TIF.JTAG
else:
  TIF = jlinksdk.TIF.SWD
#
# Select target interface speed which should be used by J-Link for target communication
#
Speed = input("Specify target interface speed [kHz]. <Default>: 4000 kHz\nSpeed> ")
if (Speed == ""):                                          # No speed selected? --> Use 4000 kHz as default
  Speed = 4000
else:
  Speed = int(Speed)
#
# Connect to target CPU
#
jlink.Connect(sDevice = sDevice, TIF = TIF, TIFSpeed = Speed)
#
# From now on the target is identified and we can start working with it.
# Insert your code here...
# Everything is done. Close connection to J-Link.
#
input("To close J-Link press <Enter>.")
jlink.Close()