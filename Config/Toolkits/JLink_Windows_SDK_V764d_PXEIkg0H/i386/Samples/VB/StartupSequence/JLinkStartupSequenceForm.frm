VERSION 5.00
Begin VB.Form JLinkStartupSequenceForm 
   BorderStyle     =   1  'Fest Einfach
   Caption         =   "J-Link Startup"
   ClientHeight    =   8070
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5670
   FillColor       =   &H00C0FFFF&
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   8070
   ScaleWidth      =   5670
   StartUpPosition =   3  'Windows-Standard
   Begin VB.ComboBox JLinkHIFCombo 
      Height          =   315
      Left            =   3120
      Style           =   2  'Dropdown-Liste
      TabIndex        =   21
      Top             =   120
      Width           =   2415
   End
   Begin VB.CommandButton HaltGoBtn 
      Caption         =   "Halt / Go"
      Enabled         =   0   'False
      Height          =   495
      Left            =   120
      TabIndex        =   18
      Top             =   5760
      Width           =   2775
   End
   Begin VB.CommandButton TargetConnectBtn 
      Caption         =   "Connect"
      Height          =   495
      Left            =   120
      TabIndex        =   17
      Top             =   5040
      Width           =   2775
   End
   Begin VB.ComboBox TIFSpeedCombo 
      Height          =   315
      Left            =   3120
      Style           =   2  'Dropdown-Liste
      TabIndex        =   16
      Top             =   4560
      Width           =   2415
   End
   Begin VB.ComboBox TIFCombo 
      Height          =   315
      Left            =   3120
      Style           =   2  'Dropdown-Liste
      TabIndex        =   14
      Top             =   4080
      Width           =   2415
   End
   Begin VB.TextBox DeviceNameTxt 
      Height          =   285
      Left            =   3120
      TabIndex        =   12
      Top             =   3600
      Width           =   2415
   End
   Begin VB.Frame Frame1 
      Caption         =   "Info"
      Height          =   1335
      Left            =   240
      TabIndex        =   6
      Top             =   6600
      Width           =   5175
      Begin VB.Label DLLVerLbl 
         Caption         =   "DLL Version:"
         Height          =   255
         Left            =   240
         TabIndex        =   10
         Top             =   480
         Width           =   4695
      End
      Begin VB.Label JLinkHWLbl 
         Caption         =   "Hardware:"
         Height          =   255
         Left            =   240
         TabIndex        =   9
         Top             =   720
         Width           =   4695
      End
      Begin VB.Label JLinkSNLbl 
         Caption         =   "S/N:"
         Height          =   255
         Left            =   240
         TabIndex        =   8
         Top             =   960
         Width           =   4695
      End
      Begin VB.Label JLinkConnStatLbl 
         Caption         =   "Status: J-Link disconnected"
         Height          =   255
         Left            =   240
         TabIndex        =   7
         Top             =   240
         Width           =   4695
      End
   End
   Begin VB.TextBox PortNoIPTxt 
      Height          =   285
      Left            =   3120
      TabIndex        =   2
      Text            =   "19020"
      Top             =   2520
      Width           =   2415
   End
   Begin VB.TextBox HostNameTxt 
      Height          =   285
      Left            =   3120
      TabIndex        =   1
      Text            =   "localhost"
      Top             =   2040
      Width           =   2415
   End
   Begin VB.TextBox JLinkSNTxt 
      Height          =   285
      Left            =   3120
      TabIndex        =   0
      Text            =   "0"
      Top             =   1080
      Width           =   2415
   End
   Begin VB.Label Label4 
      Alignment       =   2  'Zentriert
      Caption         =   "USB Settings"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   26
      Top             =   600
      Width           =   5415
   End
   Begin VB.Label TargetStateLbl 
      Alignment       =   2  'Zentriert
      Caption         =   "N/A"
      Height          =   255
      Left            =   3840
      TabIndex        =   25
      Top             =   5880
      Width           =   1575
   End
   Begin VB.Label Label13 
      Caption         =   "J-Link Connection Method"
      Height          =   255
      Left            =   120
      TabIndex        =   24
      Top             =   120
      Width           =   2775
   End
   Begin VB.Label Label12 
      Alignment       =   2  'Zentriert
      Caption         =   "Target settings"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   23
      Top             =   3120
      Width           =   5415
   End
   Begin VB.Label Label11 
      Alignment       =   2  'Zentriert
      Caption         =   "IP Settings"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   22
      Top             =   1560
      Width           =   5415
   End
   Begin VB.Label TargetHaltLED 
      BackColor       =   &H00C0C0C0&
      Height          =   495
      Left            =   3120
      TabIndex        =   20
      Top             =   5760
      Width           =   495
   End
   Begin VB.Label TargetConnLED 
      BackColor       =   &H00C0C0C0&
      Height          =   495
      Left            =   3120
      TabIndex        =   19
      Top             =   5040
      Width           =   495
   End
   Begin VB.Label Label10 
      Caption         =   "Target interface speed in kHz"
      Height          =   255
      Left            =   120
      TabIndex        =   15
      Top             =   4560
      Width           =   2775
   End
   Begin VB.Label Label9 
      Caption         =   "Target interface"
      Height          =   255
      Left            =   120
      TabIndex        =   13
      Top             =   4080
      Width           =   2775
   End
   Begin VB.Label Label8 
      Caption         =   "Target device name"
      Height          =   255
      Left            =   120
      TabIndex        =   11
      Top             =   3600
      Width           =   2775
   End
   Begin VB.Label Label3 
      Caption         =   "PortNo."
      Height          =   255
      Left            =   120
      TabIndex        =   5
      Top             =   2520
      Width           =   2775
   End
   Begin VB.Label Label2 
      Caption         =   "Hostname/IP"
      Height          =   255
      Left            =   120
      TabIndex        =   4
      Top             =   2040
      Width           =   2775
   End
   Begin VB.Label Label1 
      Caption         =   "JLink S/N"
      Height          =   255
      Left            =   120
      TabIndex        =   3
      Top             =   1080
      Width           =   2775
   End
End
Attribute VB_Name = "JLinkStartupSequenceForm"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Public IsConnectedToTarget As Long
Public IsHalted As Long
Private Const JLINKARM_TIF_JTAG = 0
Private Const JLINKARM_TIF_SWD = 1
Private Const JLINKARM_TIF_FINE = 3
Private Const JLINKARM_TIF_CJTAG = 7
Private Const JLINKARM_TIF_ICSP = 4
Private COL_GREEN As Long
Private COL_GRAY As Long
Private COL_ORANGE As Long

Function RunningInVB() As Boolean 'Returns whether we are running in vb(true), or compiled (false)
  Static counter As Variant
    
  If IsEmpty(counter) Then
    counter = 1
    Debug.Assert RunningInVB() Or True
    counter = counter - 1
  ElseIf counter = 1 Then
    counter = 0
  End If
  RunningInVB = counter
End Function

Private Sub Form_Load()
  Dim s As String
  '
  ' This function is called as soon as the form for the GUI is loaded.
  '
  If RunningInVB = True Then    ' Are we running inside a debug session?
    '
    ' When debugging, the Visual Basic 6.0 IDE behaves differently compared to modern IDEs:
    '   There is no actual .exe file that is debugged.
    '   A virtual process is created from the VB6.exe.
    '   This causes the IDE to use the directory that the VB6.exe was invoked from (e.g. the directory of the project file)
    '   as its working directory.
    '   The JLinkARM.dll directory is expected inside the working directory of the application.
    '   Because of this, we change the working directory to the directory of the J-Link SDK (with the JLinkARM.dll inside)
    '   when running inside the IDE.
    '
    s = App.Path & "\..\..\"
    ChDrive App.Path
    ChDir s
  End If
  '
  ' Populate combo boxes
  ' J-Link HIF
  '
  JLinkHIFCombo.Clear
  JLinkHIFCombo.AddItem ("USB")
  JLinkHIFCombo.AddItem ("IP")
  JLinkHIFCombo.Text = "USB"
  '
  ' Target Interface
  '
  TIFCombo.Clear
  TIFCombo.AddItem ("JTAG")
  TIFCombo.AddItem ("cJTAG")
  TIFCombo.AddItem ("SWD")
  TIFCombo.AddItem ("FINE")
  TIFCombo.AddItem ("ICSP")
  TIFCombo.Text = "JTAG"
  '
  ' TIF Speed
  '
  TIFSpeedCombo.Clear
  TIFSpeedCombo.AddItem ("5")
  TIFSpeedCombo.AddItem ("10")
  TIFSpeedCombo.AddItem ("20")
  TIFSpeedCombo.AddItem ("30")
  TIFSpeedCombo.AddItem ("50")
  TIFSpeedCombo.AddItem ("100")
  TIFSpeedCombo.AddItem ("200")
  TIFSpeedCombo.AddItem ("300")
  TIFSpeedCombo.AddItem ("400")
  TIFSpeedCombo.AddItem ("500")
  TIFSpeedCombo.AddItem ("600")
  TIFSpeedCombo.AddItem ("750")
  TIFSpeedCombo.AddItem ("900")
  TIFSpeedCombo.AddItem ("1000")
  TIFSpeedCombo.AddItem ("1334")
  TIFSpeedCombo.AddItem ("1600")
  TIFSpeedCombo.AddItem ("2000")
  TIFSpeedCombo.AddItem ("2667")
  TIFSpeedCombo.AddItem ("3200")
  TIFSpeedCombo.AddItem ("4000")
  TIFSpeedCombo.AddItem ("4800")
  TIFSpeedCombo.AddItem ("5334")
  TIFSpeedCombo.AddItem ("6000")
  TIFSpeedCombo.AddItem ("8000")
  TIFSpeedCombo.AddItem ("9600")
  TIFSpeedCombo.AddItem ("12000")
  TIFSpeedCombo.AddItem ("15000")
  TIFSpeedCombo.AddItem ("20000")
  TIFSpeedCombo.AddItem ("25000")
  TIFSpeedCombo.AddItem ("30000")
  TIFSpeedCombo.AddItem ("40000")
  TIFSpeedCombo.AddItem ("50000")
  TIFSpeedCombo.Text = "4000"
  '
  ' Init "static" values
  '
  COL_GREEN = RGB(&H33, &HCC, 0)
  COL_GRAY = RGB(&HAA, &HAA, &HAA)
  COL_ORANGE = RGB(&HFF, &HC6, &H0)
  IsHalted = 0
  IsConnectedToTarget = 0
  TargetConnLED.BackColor = COL_ORANGE
End Sub

Private Sub HaltGoBtn_Click()
  Dim r As Byte
  If IsHalted = 0 Then
    r = JLINK_Halt
    If r <> 0 Then  ' Failed to halt target? => Error.
      MsgBox "Failed halt target", , "Error"
    Else
      IsHalted = 1
    End If
  Else
    JLINK_Go
    IsHalted = 0
  End If
  UpdateHaltGUI
End Sub

Private Sub UpdateHaltGUI()
  Dim Col As Long
  Dim s As String
  
  If IsConnectedToTarget Then
    If IsHalted Then           ' Target halted?
      Col = COL_ORANGE
      s = "Target halted"
    Else                       ' Target running?
      Col = COL_GREEN
      s = "Target running"
    End If
  Else  ' J-Link not connected to target?
    Col = COL_GRAY
  End If
  TargetHaltLED.BackColor = Col
  TargetStateLbl.Caption = s
End Sub

Private Sub TargetConnectBtn_Click()
  Dim sErr       As String
  Dim sDevice    As String
  Dim sCmd       As String
  Dim TIF        As Long
  Dim TIFSpeed   As Long
  Dim pfLog      As Long
  Dim r          As Long
  Dim b          As Byte
  
  JLINK_Close
  If IsConnectedToTarget = 1 Then  ' Disconnect?
    TargetConnLED.BackColor = COL_ORANGE
    TargetHaltLED.BackColor = COL_GRAY
    JLinkConnStatLbl.Caption = "Status: J-Link disconnected."
    TargetConnectBtn.Caption = "Connect"
    HaltGoBtn.Enabled = False
    IsHalted = 0
    IsConnectedToTarget = 0
  Else                              ' Connect?
    '
    ' Connect to J-Link
    ' First, select J-Link.
    ' After that, call OpenEx()
    '
    Select Case JLinkHIFCombo.Text  ' Select J-Link
      Case "USB"                    ' via USB?
        b = JLINK_SelectUSB(CLng(JLinkSNTxt.Text))
      Case "IP"                     ' via IP?
        If HostNameTxt.Text <> "" Then
          b = JLINK_SelectIP(HostNameTxt.Text, CLng(PortNoIPTxt.Text))
        Else
          b = JLINK_SelectIP("", CLng(PortNoIPTxt.Text))
        End If
      Case Else                     ' Neither USB or IP? => Error.
        MsgBox ("Invalid J-Link connection method.")
        Exit Sub
    End Select
    sErr = JLink_OpenEx(0&, AddressOf ErrorOut)
    b = JLINK_IsOpen
    If sErr = vbNullString And b = 1 Then        ' Connected to J-Link?
      JLinkConnStatLbl.Caption = "Status: J-Link connection successful"
      GetInfo
    Else                               ' Failed to connect to J-Link? => Error.
      DLLVerLbl.Caption = "DLL Version:"
      JLinkHWLbl.Caption = "Hardware:"
      JLinkSNLbl.Caption = "S/N:"
      JLinkConnStatLbl.Caption = "Status: J-Link connection unsuccessful"
      If sErr <> vbNullString Then
        ErrorOut (sErr)
      End If
      Exit Sub
    End If
    '
    ' Connect to target
    '
    sDevice = DeviceNameTxt.Text
    If sDevice = "" Then  ' Empty device name? => Error; Early-out.
      MsgBox ("Please enter a device name.")
      Exit Sub
    End If
    TIFSpeed = Val(TIFSpeedCombo.Text)
    Select Case TIFCombo.Text
     Case "JTAG"
       TIF = JLINKARM_TIF_JTAG
     Case "cJTAG"
       TIF = JLINKARM_TIF_CJTAG
     Case "SWD"
       TIF = JLINKARM_TIF_SWD
     Case "FINE"
       TIF = JLINKARM_TIF_FINE
     Case "ICSP"
       TIF = JLINKARM_TIF_ICSP
     Case Else                  ' Unknown TIF? => Use default.
       TIF = JLINKARM_TIF_JTAG
    End Select
    '
    ' Pass info to DLL and connect to target
    '
    sCmd = "device = " + sDevice
    r = JLINK_ExecCommand(sCmd, 0, 0)
    r = JLINK_TIF_Select(TIF)
    If TIF = JLINKARM_TIF_JTAG Then  ' JTAG selected as TIF? => Use default JTAG config
      JLINK_ConfigJTAG -1, -1
    End If
    JLINK_SetSpeed (TIFSpeed)
    r = JLINK_Connect
    If r <> 0 Then  ' Connect failed? => Error.
      MsgBox "Failed to connect to target", , "Error"
      Exit Sub
    End If
    '
    ' Update GUI
    '
    TargetConnLED.BackColor = COL_GREEN
    TargetHaltLED.BackColor = COL_GREEN
    TargetConnectBtn.Caption = "Disconnect"
    HaltGoBtn.Enabled = True
    IsConnectedToTarget = 1
  End If
  UpdateHaltGUI
End Sub

Private Sub GetInfo()
  Dim Ver As Long
  Dim Sn As Long

  Ver = JLINK_GetDLLVersion
  DLLVerLbl.Caption = "DLL Version: " & CInt(Ver / 10000) & "." & (Ver / 100) Mod 100 & "." & (Ver Mod 100)
  Ver = JLINK_GetHardwareVersion
  JLinkHWLbl.Caption = "Hardware: V" & (Ver / 10000) Mod 100 & "." & (Ver / 100) Mod 100
  Sn = JLINK_GetSN
  JLinkSNLbl.Caption = "S/N: " & Sn
End Sub
