Attribute VB_Name = "JLinkStartupSequence"
Option Explicit

Public Declare Sub JLINK_Close Lib "JLinkARM" ()
Public Declare Function JLINK_GetId Lib "JLinkARM" () As Long
Public Declare Function JLink_OpenExDLL Lib "JLinkARM" Alias "JLINK_OpenEx" (ByVal pfLog As Long, ByVal pfErrorOut As Long) As String
Public Declare Function JLINK_ReadMemU32 Lib "JLinkARM" (ByVal Addr As Long, ByVal NumItems As Long, pData32 As Long, pStatus As Byte) As Byte
Public Declare Sub JLINK_SetSpeed Lib "JLinkARM" (ByVal Speed As Long)
Public Declare Function JLINK_SelectIP Lib "JLinkARM" (ByVal sHost As String, ByVal Port As Long) As Byte
Public Declare Function JLINK_SelectUSB Lib "JLinkARM" (ByVal Port As Long) As Byte
Public Declare Function JLINK_EMU_SelectByUSBSN Lib "JLinkARM" (ByVal SerialNo As Long) As Long
Public Declare Function JLINK_GetSN Lib "JLinkARM" () As Long
Public Declare Function JLINK_GetDLLVersion Lib "JLinkARM" () As Long
Public Declare Function JLINK_GetHardwareVersion Lib "JLinkARM" () As Long
Public Declare Function JLINK_TIF_Select Lib "JLinkARM" (ByVal Interface As Long) As Long
Public Declare Function JLINK_Connect Lib "JLinkARM" () As Long
Public Declare Function JLINK_ExecCommand Lib "JLinkARM" (ByVal sIn As String, ByVal pOut As Long, ByVal BufSize As Long) As Long
Public Declare Function JLINK_Halt Lib "JLinkARM" () As Byte
Public Declare Sub JLINK_Go Lib "JLinkARM" ()
Public Declare Sub JLINK_ConfigJTAG Lib "JLinkARM" (IRPRe As Long, DRPre As Long)
Public Declare Function JLINK_IsOpen Lib "JLinkARM" () As Byte

Private Declare Function lstrcpy Lib "kernel32" Alias "lstrcpyA" (ByVal lpString1 As Any, _
    ByVal lpString2 As Any) As Long

Private Function CopyString(ByVal Addr As Long) As String
  Dim ret As String
  
  If Addr > 0 Then
    ret = Space(1024)
    lstrcpy ret, Addr
    ret = Trim(ret)
    If ret <> "" Then CopyString = Left(ret, Len(ret) - 1)
  End If
End Function

Public Function JLink_OpenEx(ByVal pfLog As Long, ByVal pfErrorOut As Long) As String
  JLink_OpenEx = CopyString(Val(JLink_OpenExDLL(pfLog, pfErrorOut)))
End Function

Public Sub ErrorOut(ByVal pString As Long)
  Dim sErr As String
  sErr = CopyString(pString)
  MsgBox sErr, , "Error"
End Sub

