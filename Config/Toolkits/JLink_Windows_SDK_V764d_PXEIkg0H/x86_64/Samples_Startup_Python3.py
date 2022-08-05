'''*********************************************************************
*       Example based on Python 3.5.1, NV
*    
*
*       import and DLL include
*
**********************************************************************
'''
import os 
import ctypes
import fnmatch
import sys
import time
import platform
import struct
#
# Load DLL
#
ActiveOS = platform.system()
PyArch   = (struct.calcsize("P") * 8)   # Determine if 32 or 64 bit python is running
sDLLPath = (os.path.dirname(__file__))  # Look for DLL relative to this script's directory
if ActiveOS == 'Windows':
  if (PyArch == 32):
    sDLLPath += "/JLinkARM.dll"
  else:  
    sDLLPath += "/JLink_x64.dll"
elif ActiveOS == 'Linux':
  sDLLPath += "/libjlinkarm.so"
elif ActiveOS == 'Darwin':
  sDLLPath += "/libjlinkarm.dylib"

dll = ctypes.cdll.LoadLibrary(sDLLPath)

'''*********************************************************************
*
*      local functions
*
**********************************************************************
'''
dll.JLINKARM_Open.restype = ctypes.c_char_p           # Reset function return from int to char pointer for Python compiler
'''*********************************************************************
*
*        _ConsoleGetString
*
*      function description
*        get user console input and return it
*        needs stringtext as argument e.g. "Insert Info and press ENTER"
**********************************************************************
'''
def _ConsoleGetString(request):
    Input = input(request)
    return Input

'''*********************************************************************
*
*        "main" InitDebug Session
*
**********************************************************************
'''
#
# Select and configure host interface (USB is default)
#
v = _ConsoleGetString("Enter the host interface.\n  0> USB (Default)\n  1> IP\nHostIF> ")
if (v == '1'):  # User entered '1'? => Select IP as HIF
  v = 1
else:           # Use USB as default
  v = 0
#
# Get serial number if USB selected or IP address if Ethernet selected as host interface
#
if (v == 0):                                                     # USB selected?
  v = _ConsoleGetString("Enter J-Link serial number\nSN> ")
  if (v == ""):                                                  # No serial number selected? --> Use 0 as default
    v = 0
  dll.JLINKARM_EMU_SelectByUSBSN(int(v))
else:                                                            # IP selected?
  v = _ConsoleGetString("Enter J-Link IP or host name\nHost> ")
  if (v == ""):
    v = 0
  else:
    temp = v.encode('utf-8')
    dll.JLINKARM_SelectIP(ctypes.c_char_p(temp),0)     # Select J-Link over inserted IP
#
# Open connection to J-Link
#
r = dll.JLINKARM_Open()                  
if (r):                                                # Error occurred? --> Print error
  print(r.decode("utf-8"))
  input("J-Link stopped, press <Enter> to exit...")
  dll.JLINKARM_Close()                                 # Close J-Link connection
  sys.exit()                                           # Terminate Program
#
# Select device or core
#
if platform.system() == 'Windows':                     # Script runs on Windows? Print selection dialog option
  sDlg = "Type '?' for selection dialog\n"
else:
  sDlg = ""
v = _ConsoleGetString("Please specify device / core. <Default>: unspecified\n%sDevice> " % sDlg)
if (v == ""):                                          # No device selected? --> Use "unspecified"
  v = "unspecified"
temp = "device = " + str(v)                            # Create commandstring for J-Link
acCmd = temp.encode('utf-8')                           # Convert to utf for c interpreter  
dll.JLINKARM_ExecCommand(ctypes.c_char_p(acCmd), 0, 0)
#
# Select and configure target interface
#
v = _ConsoleGetString("Please specify target interface:\n  0> JTAG\n  1> SWD (Default)\nTIF> ")
if (v == ""):                                          # No target interface selected? --> Use SWD as default
  v = 1
dll.JLINKARM_TIF_Select(int(v))
#
# Select target interface speed which should be used by J-Link for target communication
#
v = _ConsoleGetString("Specify target interface speed [kHz]. <Default>: 4000 kHz\nSpeed> ")
if (v == ""):                                          # No speed selected? --> Use 4000 kHz as default
  v = 4000
dll.JLINKARM_SetSpeed(int(v))
#
# Connect to target CPU
#
r = dll.JLINKARM_Connect()
if (r < 0):
  print("Connect failed. Error:", r)                 # Print errorcode if aviable
else:
  print("Connected to target successfully.\n")
#
# From now on the target is identified and we can start working with it.
# Insert your code here...
# Everything is done. Close connection to J-Link.
#
input("To close J-Link press <Enter>.")
dll.JLINKARM_Close()