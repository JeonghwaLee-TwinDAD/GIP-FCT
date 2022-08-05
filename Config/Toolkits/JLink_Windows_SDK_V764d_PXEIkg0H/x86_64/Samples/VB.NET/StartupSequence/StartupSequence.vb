Public Class DlgStartupSequence
  '
  ' Declare J-Link DLL functions.
  ' Depending on the architecture that was built for, we need to load a different DLL.
  '
#If ARCH = "x64" Then
  Public Declare Function JLink_OpenExDLL Lib "JLink_x64" Alias "JLINK_OpenEx" (ByVal pfLog As cbOut, ByVal pfErrorOut As cbOut) As String
  Public Declare Function JLINK_SelectIP Lib "JLink_x64" (ByVal sHost As String, ByVal Port As Int32) As Byte
  Public Declare Function JLINK_EMU_SelectByUSBSN Lib "JLink_x64" (ByVal SerialNo As Int32) As Int32
  Public Declare Sub JLINK_Close Lib "JLink_x64" ()
  Public Declare Function JLINK_ExecCommand Lib "JLink_x64" (ByVal sIn As String, ByVal sError() As Char, ByVal BufferSize As Int32) As Int32
  Public Declare Function JLINK_TIF_Select Lib "JLink_x64" (ByVal TIF As Int32) As Int32
  Public Declare Sub JLINK_SetSpeed Lib "JLink_x64" (ByVal Speed As UInt32)
  Public Declare Function JLINK_Connect Lib "JLink_x64" () As Int32
#Else
  Public Declare Function JLink_OpenExDLL Lib "JLinkARM" Alias "JLINK_OpenEx" (ByVal pfLog As cbOut, ByVal pfErrorOut As cbOut) As String
  Public Declare Function JLINK_SelectIP Lib "JLinkARM" (ByVal sHost As String, ByVal Port As Int32) As Byte
  Public Declare Function JLINK_EMU_SelectByUSBSN Lib "JLinkARM" (ByVal SerialNo As Int32) As Int32
  Public Declare Sub JLINK_Close Lib "JLinkARM" ()
  Public Declare Function JLINK_ExecCommand Lib "JLinkARM" (ByVal sIn As String, ByVal sError() As Char, ByVal BufferSize As Int32) As Int32
  Public Declare Function JLINK_TIF_Select Lib "JLinkARM" (ByVal TIF As Int32) As Int32
  Public Declare Sub JLINK_SetSpeed Lib "JLinkARM" (ByVal Speed As UInt32)
  Public Declare Function JLINK_Connect Lib "JLinkARM" () As Int32
#End If

  Delegate Sub cbOut(ByVal pString As String)

  Private Sub ErrorOut(ByVal pString As String)
    TxtLog.AppendText("J-Link DLL ERROR:" + pString & Environment.NewLine)
  End Sub
  Private Sub LogOut(ByVal pString As String)
    TxtLog.AppendText("J-Link DLL Log: " + pString & Environment.NewLine)
  End Sub

  Private Sub Log(ByVal sLog As String)
    TxtLog.AppendText(sLog)
  End Sub

  Private Sub Button1_Click(sender As System.Object, e As System.EventArgs) Handles BtnConnect.Click
    Dim Result As Int32
    Dim sError(256) As Char

    If RdoIFIP.Checked = True Then
      Log("IP Selected." & Environment.NewLine)
      If Not TxtIP.Text = "" Then
        Result = JLINK_SelectIP(TxtIP.Text, 0)
        If Result < 0 Then
          Log("Failed to select J-Link via IP." & Environment.NewLine)
          GoTo Done
        End If
      End If
    ElseIf RdoIFUSB.Checked = True Then
      Log("USB Selected." & Environment.NewLine)
      If Not TxtSN.Text = "0" Then
        Result = JLINK_EMU_SelectByUSBSN(Convert.ToInt32(TxtSN.Text))
        If Result < 0 Then
          Log("Failed to select J-Link via USB serial number." & Environment.NewLine)
          GoTo Done
        End If
      End If
    Else
      Log("Failed to get J-Link connection." & Environment.NewLine)
      GoTo Done
    End If

    sError = JLink_OpenExDLL(AddressOf LogOut, AddressOf ErrorOut)
    If (sError.Length) Then
      Log(sError & Environment.NewLine)
      GoTo Done
    End If
    Log("Connected to J-Link." & Environment.NewLine)

    JLINK_ExecCommand("ProjectFile = " + TxtSettingsFile.Text, sError, 256)
    JLINK_ExecCommand("Device = " + TxtDevice.Text, sError, 256)

    If CmbTIF.Text = "JTAG" Then
      JLINK_TIF_Select(0)
    Else
      JLINK_TIF_Select(1)
    End If

    JLINK_SetSpeed(NumTIFSpeed.Value)

    Result = JLINK_Connect()
    If Result < 0 Then
      Log("Could not connect to target." & Environment.NewLine)
      GoTo Close
    End If
    Log("Connected to target." & Environment.NewLine)
    Log("Now your work starts." & Environment.NewLine)

Close:
    JLINK_Close()
    Log("Disconnected from target and J-Link." & Environment.NewLine)
Done:
  End Sub

End Class
