#/*********************************************************************
#*               (c) SEGGER Microcontroller GmbH                      *
#*                        The Embedded Experts                        *
#*                           www.segger.com                           *
#**********************************************************************
#
#-------------------------- END-OF-HEADER -----------------------------
#
#File    : SDK_STraceSample.py
#Purpose : Implementation of the jlink STrace functionality.
#Literature:
#  [1]  Python 3 Online Documentation https://docs.python.org/3/library/index.html
#  [2]  J-Link SDK Manual (UM08002)
#  [3]  STraceSample_Readme.txt
#  [4]  https://wiki.segger.com/Tracing_on_ST_STM32F407_(SEGGER_Cortex-M_Trace_Reference_Board)
#
#Notes:
#  Requirements:
#    1) The STRACE sample script expects the Start_STM32F407_Trace_Example.hex file in the same directory as itself.
#    2) A Probe with trace capabilities is required (J-Trace Pro V2 or higher) for the sample to work.
#    3) The script is designed to be used with SEGGER's Cortex-M Trace Reference Board V1.2 (STM32F407VE).
#  
#  Instructions:
#    1) Make sure that all requirements above are fulfilled.
#    2) Connect the J-Trace Pro to your PC.
#    3) Power the Cortex-M Trace Reference Board V1.2 via the Mini-USB port.
#    4) Connect the J-Trace with the Cortex-M Trace Reference Board V1.2 (STM32F407VE), using the trace ribbon cable shipped with the J-Trace PRO.
#    5) Start the sample script.
#Additional information:
#  -
#*/

import os
import sys
import time
import jlinksdk

#/*********************************************************************
#*
#*       "Defines"
#*
#**********************************************************************
#*/
_SAMPLE_APP_PATH    = "./Start_STM32F407_Trace_Example.hex"  # Trace example hex file.
_DUMP_FILE_PATH     = "./MyDump.txt"                         # Dump file path
_DEFAULT_TRACE_TIME = 10                                     # 10 sec.

#/*********************************************************************
#*
#*       Global data
#*
#**********************************************************************
#*/
jlink = None

#/*********************************************************************
#*
#*       "Static" functions
#*
#**********************************************************************
#*/

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
  jlink.STRACE_Stop()
  jlink.Close()
  print("Shutting down...")
  time.sleep(3)       # Give user some time to read the error message.
  sys.exit(-1)

#/*********************************************************************
#*
#*       _LogToFile()
#*
#*  Function description
#*    Formats trace data and loggs it to file.
#*/
def _LogData(sData):
  print(sData)
  f = open(_DUMP_FILE_PATH, "w+")        # Open dump file in write
  f.write(sData)
  f.close()
  print("Data \"logged to %s\"." % _DUMP_FILE_PATH)
  
#/*********************************************************************
#*
#*       _AnalyzeData()
#*
#*  Function description
#*    Analyzes the STrace Data and returns the Analyzed data as a string.
#*    This function expects the target to be halted.
#*
#*  Prameters
#*    Addr   - Address of the first Item to be analyzed
#*    aItems - Array of STrace data
#*
#*  Return value
#*    sData  - Analyzed data.
#*/
def _AnalyzeData(Addr, aItems):
  sData          = ""
  InstBytesLeft  = 0
  sData += "Addr         FetchCnt   ExecCnt    SkipCnt     Disassembly\n"
  sData += "------------------------------------------------------------------\n"
  #
  # Go through all Items and print the data per instruction,
  # as some instructions might be longer then 2 bytes.
  #
  for Item in aItems:
    #
    # Get disassembly and instruction size.
    #
    if (InstBytesLeft == 0):  # Only get instruction info, if we have finished handling the last instruction
      InstSize, sDis = jlink.DisassembleInst(Addr)
      InstBytesLeft  = InstSize
      ExecCnt        = 0
      FetchCnt       = 0
      SkipCnt        = 0
    if (InstSize < 0):        # Function returned an error? => We are done.
      sData = "Unknown instruction hit at 0x%8X. Further program flow cannot be reconstructed properly." % Addr
      _Error(sData)
      break
    FetchCnt      += Item.FetchCnt
    ExecCnt       += Item.ExecCnt
    SkipCnt       += Item.SkipCnt
    InstBytesLeft -= 2
    if (InstBytesLeft == 0):  # Instruction complete? => append data.
      sData += "0x%.8X:  " % Addr
      sData += "0x%8.8X 0x%8.8X 0x%8.8X  " % (FetchCnt, ExecCnt, SkipCnt)
      sData += sDis
      sData += "\n"
      Addr += InstSize
  #
  # Finally append "Total" counters.
  #
  sData += "Total:       "
  sData += "0x%8.8X " % aItems[-1].FetchCnt
  sData += "0x%8.8X " % aItems[-1].ExecCnt
  sData += "0x%8.8X"  % aItems[-1].SkipCnt
  sData += "\n"
  return sData

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
print("DLLVersion: " +  jlink.GetDllVersionString())
v = input("Enter the host interface.\n  0> USB (Default)\n  1> IP\nHostIF> ")
if (v == '1'):  # User entered '1'? => Select IP as HIF
  HostIF = jlinksdk.HOST_IF.TCPIP
else:           # Use USB as default
  HostIF = jlinksdk.HOST_IF.USB
#
# Get serial number (0 is default)
#
SN = input("Enter J-Link serial number\nSN> ")
if (SN == ""):
  SN = 0
SN = int(SN)
#
# Open connection to J-Link
#
jlink.Open(HostIF = HostIF, SN = SN)
#
# Check if the Probe has Trace capabilities (currently only J-Trace PRO).
#
Caps  = jlink.GetEmuCaps()
Caps &= jlinksdk.EMU_CAP.RAWTRACE
if (Caps == 0):
  _Error("The selected Probe does not support STRACE.\nA J-Trace PRO V2 or higher is required for this sample.")
#
# Select and configure target interface and device
#
sDevice = "STM32F407VE"
TIF     = jlinksdk.TIF.SWD
Speed   = 4000
jlink.Connect(sDevice = sDevice, TIF = TIF, TIFSpeed = Speed)
#
# Load sample application
#
if (not os.path.isfile(_SAMPLE_APP_PATH)):
  _Error("Sample file is missing. Shutting down.")
try:
  jlink.DownloadFile(_SAMPLE_APP_PATH, jlinksdk.NULL) # hex files already contain the address.
except(r):
  _Error("Failed downloading sample application:\n" + r.decode("utf-8"))
print("Download was successfull")
#
# Start STRACE and log data to a file.
#
jlink.Reset()            # Implicitly halts CPU.
jlink.STRACE_Start()     # Start STrace
Address  = 0x0800017C    # This Address is picked for the specific application being traced here. Change if needed.
NumItems = 0x30          # This is the number of Items to get information about.This might not be the number of instructions, as instructions might be > 2 bytes.
#
# Determine how long the user wants to gather data.
# We make sure that the user does not crash the script by passing a non integer value.
#
Input = input("How long should the data be read? Time in seconds> ")
if (Input == ""):
  t = _DEFAULT_TRACE_TIME
  print ("Using default: %d sec." % t)
else:
  try:
    t = int(Input)
  except:
    t = _DEFAULT_TRACE_TIME
    print("\"%s\" is not an integer. Using default: %d sec." % (Input, t))
t0 = time.time() + t
#
# We read trace data for "t" seconds and then stop.
#
jlink.Go()    # CPU was halted by "Reset()" so we need to start it again.
while(1):
  #
  # Retrieve trace data from the DLL
  #
  try:
    r, aItems = jlink.STRACE_GetInstStats(Address, NumItems, jlinksdk.STRACE_CNT_TYPE.FETCHED_EXEC_SKIP) # Request data
  except Exception as e:
    _Error(e.value)
  #
  # Update "Total" counters
  #
  sData  = "Total (FetchCnt/ExecCnt/SkipCnt): "
  sData += "0x%8.8X " % aItems[-1].FetchCnt
  sData += "0x%8.8X " % aItems[-1].ExecCnt
  sData += "0x%8.8X"  % aItems[-1].SkipCnt
  sys.stdout.write("\r" + sData)
  sys.stdout.flush()
  if (time.time() > t0):         # Done? => Analyze data, print it and log it to file, then ask user if they want to continue
    print("")                    # Linebreak
    jlink.Halt()                 # _AnalyzeData() requires the CPU to be halted, to aquire the disassembly data.
    sData = _AnalyzeData(Address, aItems)
    _LogData(sData)              # Log data to file and console.
    DoAgain = input("Read trace data again (y/n)? ")
    if (DoAgain.lower() != "y"): # User does not want to continue? => We are done.
      break
    t = input("How long should the data be read? Time in seconds> ")
    jlink.Go()                   # We want to continue tracing? => Start CPU again.
    t0 = time.time() + int(t)
jlink.STRACE_Stop()
jlink.Close()