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
import sys
import time
import jlinksdk

#/*********************************************************************
#*
#*       "Defines"
#*
#**********************************************************************
#*/
_MAX_RTT_DATA = 0x3000                # 12kiB maximum RTT data
_TIMEOUT      = 10                    # Stop if we do not find the RTT Control block within 10 sec.

#/*********************************************************************
#*
#*       _Error()
#*
#*  Function description
#*    Called when an error occurs while using RTT.
#*    Makes sure we do stop RTT and exit on Error.
#*/
def _Error(sErr):
  print("ERROR: " + sErr)
  jlink.RTTERMINAL_Stop()
  sys.exit(-1)

#/*********************************************************************
#*
#*       _RTTHandleStart()
#*
#*  Function description
#*    Starts RTT.
#*/
def _RTTHandleStart(jlink):
  r = jlink.RTTERMINAL_Start()
  if (r < 0):
    _Error("Could not start RTT")
  #
  # Wait for the control block to be detected...
  #
  t = time.time() + _TIMEOUT
  while(1):
    NumBuf = jlink.RTTERMINAL_GetNumBuf(jlinksdk.RTTERMINAL_BUFFER_DIR.UP)
    if ((NumBuf < 0) and (NumBuf != -2)):
      _Error("Could not get number of buffers")
    if (NumBuf >= 0):
      break
    if (t - time.time() < 0):
      _Error("Could not find RTT Control Block")
    time.sleep(0.1)
  #
  # Print the description of the buffer
  #
  r, sBufName, BufSize, Flags = jlink.RTTERMINAL_GetDesc(0, jlinksdk.RTTERMINAL_BUFFER_DIR.UP)
  if (r < 0):
    _Error("Could not get buffer description")
  print("Buffer Name:  %s" % sBufName)
  print("Buffer Size:  %d" % BufSize)
  print("Buffer Flags: %d" % Flags)
  print("NumBuffers:   %d" % NumBuf)

#/*********************************************************************
#*
#*       _RTTHandleReadPrint()
#*
#*  Function description
#*    Start reading data until we do not receive data anymore for 0.5 sec. or
#*    until _MAX_RTT_DATA is reached
#*/
def _RTTHandleReadPrint(jlink):
  sData = ""
  NumBytesReadTotal = 0
  NumBytesRead      = 1    # Make sure we enter while loop
  while (NumBytesRead > 0):
    NumBytesRead, s    = jlink.RTTERMINAL_Read(0, 0x100)
    if (NumBytesRead < 0):
      _Error("Failed to read RTT data from target")
    sData             += s[0:NumBytesRead].decode("utf-8")
    NumBytesReadTotal += NumBytesRead
    #
    # Show kb read
    #
    sys.stdout.write("\r" + '{:15}'.format("Data read:") + "%8.4fkB" % (NumBytesReadTotal / 1000))
    sys.stdout.flush()
    if (NumBytesReadTotal >= _MAX_RTT_DATA): # Maximum reached? => We are done.
      sys.stdout.write("\nMaximum of %d bytes reached" % _MAX_RTT_DATA)
      break
    time.sleep(0.5)        # Sleep 500 ms to give target some time to write more data
  #
  # Print data if any
  #
  print("")    # Line break
  if (NumBytesReadTotal >= 0):
    print("--- Data ---")
    print(sData)

#/*********************************************************************
#*
#*       _RTTHandleWriteData()
#*
#*  Function description
#*    Write a string to the target via RTT
#*/
def _RTTHandleWriteData(jlink):
  x = input("String to send to target: ")
  if (x == ""):
    x = '\n'
  x = x.encode("utf-8")  # Convert string to an utf-8 byte object.
  NumBytesWritten = jlink.RTTERMINAL_Write(0, x, len(x))
  if (NumBytesWritten < 0):
    _Error("Failed to write RTT data to target")
  print("NumBytesWritten = %d" % NumBytesWritten)

#/*********************************************************************
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
# Start working with RTT
#
UseRTT = input("Start RTT (y/n)? ")
if (UseRTT.lower() == "y"):
  _RTTHandleStart(jlink)
  #
  # RTT read
  #
  DoRead = input("Read RTT data (y/n)? ")
  if (DoRead.lower() == "y"):
    _RTTHandleReadPrint(jlink)
  #
  # RTT write
  #
  DoWrite = input("Write RTT data (y/n)? ")
  if (DoWrite.lower() == "y"):
    _RTTHandleWriteData(jlink)
  #
  # Stop RTT
  #
  r = jlink.RTTERMINAL_Stop()
input("To close J-Link press <Enter>.")
jlink.Close()