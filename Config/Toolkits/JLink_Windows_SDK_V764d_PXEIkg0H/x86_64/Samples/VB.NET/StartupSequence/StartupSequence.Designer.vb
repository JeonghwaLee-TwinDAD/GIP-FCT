<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class DlgStartupSequence
  Inherits System.Windows.Forms.Form

  'Form overrides dispose to clean up the component list.
  <System.Diagnostics.DebuggerNonUserCode()> _
  Protected Overrides Sub Dispose(ByVal disposing As Boolean)
    Try
      If disposing AndAlso components IsNot Nothing Then
        components.Dispose()
      End If
    Finally
      MyBase.Dispose(disposing)
    End Try
  End Sub

  'Required by the Windows Form Designer
  Private components As System.ComponentModel.IContainer

  'NOTE: The following procedure is required by the Windows Form Designer
  'It can be modified using the Windows Form Designer.  
  'Do not modify it using the code editor.
  <System.Diagnostics.DebuggerStepThrough()> _
  Private Sub InitializeComponent()
    Me.BtnConnect = New System.Windows.Forms.Button()
    Me.TxtSN = New System.Windows.Forms.TextBox()
    Me.RdoIFUSB = New System.Windows.Forms.RadioButton()
    Me.RdoIFIP = New System.Windows.Forms.RadioButton()
    Me.TxtIP = New System.Windows.Forms.TextBox()
    Me.GroupBox1 = New System.Windows.Forms.GroupBox()
    Me.GroupBox2 = New System.Windows.Forms.GroupBox()
    Me.TxtDevice = New System.Windows.Forms.TextBox()
    Me.GroupBox3 = New System.Windows.Forms.GroupBox()
    Me.CmbTIF = New System.Windows.Forms.ComboBox()
    Me.Label1 = New System.Windows.Forms.Label()
    Me.NumTIFSpeed = New System.Windows.Forms.NumericUpDown()
    Me.Label2 = New System.Windows.Forms.Label()
    Me.GroupBox4 = New System.Windows.Forms.GroupBox()
    Me.TxtSettingsFile = New System.Windows.Forms.TextBox()
    Me.GroupBox5 = New System.Windows.Forms.GroupBox()
    Me.TxtLog = New System.Windows.Forms.TextBox()
    Me.GroupBox1.SuspendLayout()
    Me.GroupBox2.SuspendLayout()
    Me.GroupBox3.SuspendLayout()
    CType(Me.NumTIFSpeed, System.ComponentModel.ISupportInitialize).BeginInit()
    Me.GroupBox4.SuspendLayout()
    Me.GroupBox5.SuspendLayout()
    Me.SuspendLayout()
    '
    'BtnConnect
    '
    Me.BtnConnect.Location = New System.Drawing.Point(205, 274)
    Me.BtnConnect.Name = "BtnConnect"
    Me.BtnConnect.Size = New System.Drawing.Size(75, 23)
    Me.BtnConnect.TabIndex = 0
    Me.BtnConnect.Text = "Connect"
    Me.BtnConnect.UseVisualStyleBackColor = True
    '
    'TxtSN
    '
    Me.TxtSN.Location = New System.Drawing.Point(162, 16)
    Me.TxtSN.Name = "TxtSN"
    Me.TxtSN.Size = New System.Drawing.Size(100, 20)
    Me.TxtSN.TabIndex = 1
    Me.TxtSN.Text = "0"
    '
    'RdoIFUSB
    '
    Me.RdoIFUSB.AutoSize = True
    Me.RdoIFUSB.Checked = True
    Me.RdoIFUSB.Location = New System.Drawing.Point(6, 19)
    Me.RdoIFUSB.Name = "RdoIFUSB"
    Me.RdoIFUSB.Size = New System.Drawing.Size(117, 17)
    Me.RdoIFUSB.TabIndex = 2
    Me.RdoIFUSB.TabStop = True
    Me.RdoIFUSB.Text = "USB Serial number:"
    Me.RdoIFUSB.UseVisualStyleBackColor = True
    '
    'RdoIFIP
    '
    Me.RdoIFIP.AutoSize = True
    Me.RdoIFIP.Location = New System.Drawing.Point(6, 42)
    Me.RdoIFIP.Name = "RdoIFIP"
    Me.RdoIFIP.Size = New System.Drawing.Size(134, 17)
    Me.RdoIFIP.TabIndex = 3
    Me.RdoIFIP.TabStop = True
    Me.RdoIFIP.Text = "Ethernet IP/Hostname:"
    Me.RdoIFIP.UseVisualStyleBackColor = True
    '
    'TxtIP
    '
    Me.TxtIP.Location = New System.Drawing.Point(162, 39)
    Me.TxtIP.Name = "TxtIP"
    Me.TxtIP.Size = New System.Drawing.Size(100, 20)
    Me.TxtIP.TabIndex = 4
    '
    'GroupBox1
    '
    Me.GroupBox1.Controls.Add(Me.RdoIFUSB)
    Me.GroupBox1.Controls.Add(Me.TxtIP)
    Me.GroupBox1.Controls.Add(Me.TxtSN)
    Me.GroupBox1.Controls.Add(Me.RdoIFIP)
    Me.GroupBox1.Location = New System.Drawing.Point(12, 12)
    Me.GroupBox1.Name = "GroupBox1"
    Me.GroupBox1.Size = New System.Drawing.Size(268, 69)
    Me.GroupBox1.TabIndex = 5
    Me.GroupBox1.TabStop = False
    Me.GroupBox1.Text = "J-Link Connection"
    '
    'GroupBox2
    '
    Me.GroupBox2.Controls.Add(Me.TxtDevice)
    Me.GroupBox2.Location = New System.Drawing.Point(12, 87)
    Me.GroupBox2.Name = "GroupBox2"
    Me.GroupBox2.Size = New System.Drawing.Size(268, 48)
    Me.GroupBox2.TabIndex = 6
    Me.GroupBox2.TabStop = False
    Me.GroupBox2.Text = "Target Device"
    '
    'TxtDevice
    '
    Me.TxtDevice.Location = New System.Drawing.Point(6, 19)
    Me.TxtDevice.Name = "TxtDevice"
    Me.TxtDevice.Size = New System.Drawing.Size(256, 20)
    Me.TxtDevice.TabIndex = 0
    Me.TxtDevice.Text = "STM32F407IE"
    '
    'GroupBox3
    '
    Me.GroupBox3.Controls.Add(Me.Label2)
    Me.GroupBox3.Controls.Add(Me.NumTIFSpeed)
    Me.GroupBox3.Controls.Add(Me.Label1)
    Me.GroupBox3.Controls.Add(Me.CmbTIF)
    Me.GroupBox3.Location = New System.Drawing.Point(12, 141)
    Me.GroupBox3.Name = "GroupBox3"
    Me.GroupBox3.Size = New System.Drawing.Size(268, 73)
    Me.GroupBox3.TabIndex = 7
    Me.GroupBox3.TabStop = False
    Me.GroupBox3.Text = "Target Connection"
    '
    'CmbTIF
    '
    Me.CmbTIF.FormattingEnabled = True
    Me.CmbTIF.Items.AddRange(New Object() {"JTAG", "SWD"})
    Me.CmbTIF.Location = New System.Drawing.Point(141, 19)
    Me.CmbTIF.Name = "CmbTIF"
    Me.CmbTIF.Size = New System.Drawing.Size(121, 21)
    Me.CmbTIF.TabIndex = 1
    Me.CmbTIF.Text = "SWD"
    '
    'Label1
    '
    Me.Label1.AutoSize = True
    Me.Label1.Location = New System.Drawing.Point(6, 22)
    Me.Label1.Name = "Label1"
    Me.Label1.Size = New System.Drawing.Size(85, 13)
    Me.Label1.TabIndex = 2
    Me.Label1.Text = "Target interface:"
    '
    'NumTIFSpeed
    '
    Me.NumTIFSpeed.Increment = New Decimal(New Integer() {100, 0, 0, 0})
    Me.NumTIFSpeed.Location = New System.Drawing.Point(141, 46)
    Me.NumTIFSpeed.Maximum = New Decimal(New Integer() {50000, 0, 0, 0})
    Me.NumTIFSpeed.Name = "NumTIFSpeed"
    Me.NumTIFSpeed.Size = New System.Drawing.Size(120, 20)
    Me.NumTIFSpeed.TabIndex = 3
    Me.NumTIFSpeed.Value = New Decimal(New Integer() {4000, 0, 0, 0})
    '
    'Label2
    '
    Me.Label2.AutoSize = True
    Me.Label2.Location = New System.Drawing.Point(7, 48)
    Me.Label2.Name = "Label2"
    Me.Label2.Size = New System.Drawing.Size(112, 13)
    Me.Label2.TabIndex = 4
    Me.Label2.Text = "Interface speed [kHz]:"
    '
    'GroupBox4
    '
    Me.GroupBox4.Controls.Add(Me.TxtSettingsFile)
    Me.GroupBox4.Location = New System.Drawing.Point(12, 220)
    Me.GroupBox4.Name = "GroupBox4"
    Me.GroupBox4.Size = New System.Drawing.Size(268, 48)
    Me.GroupBox4.TabIndex = 8
    Me.GroupBox4.TabStop = False
    Me.GroupBox4.Text = "J-Link Settings File"
    '
    'TxtSettingsFile
    '
    Me.TxtSettingsFile.Location = New System.Drawing.Point(6, 19)
    Me.TxtSettingsFile.Name = "TxtSettingsFile"
    Me.TxtSettingsFile.Size = New System.Drawing.Size(255, 20)
    Me.TxtSettingsFile.TabIndex = 0
    Me.TxtSettingsFile.Text = "Settings.jlink"
    '
    'GroupBox5
    '
    Me.GroupBox5.Controls.Add(Me.TxtLog)
    Me.GroupBox5.Location = New System.Drawing.Point(12, 303)
    Me.GroupBox5.Name = "GroupBox5"
    Me.GroupBox5.Size = New System.Drawing.Size(268, 120)
    Me.GroupBox5.TabIndex = 9
    Me.GroupBox5.TabStop = False
    Me.GroupBox5.Text = "Log"
    '
    'TxtLog
    '
    Me.TxtLog.BackColor = System.Drawing.SystemColors.ControlLightLight
    Me.TxtLog.Location = New System.Drawing.Point(6, 19)
    Me.TxtLog.Multiline = True
    Me.TxtLog.Name = "TxtLog"
    Me.TxtLog.ReadOnly = True
    Me.TxtLog.ScrollBars = System.Windows.Forms.ScrollBars.Both
    Me.TxtLog.Size = New System.Drawing.Size(255, 95)
    Me.TxtLog.TabIndex = 0
    '
    'DlgStartupSequence
    '
    Me.AutoScaleDimensions = New System.Drawing.SizeF(6.0!, 13.0!)
    Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
    Me.ClientSize = New System.Drawing.Size(292, 433)
    Me.Controls.Add(Me.GroupBox5)
    Me.Controls.Add(Me.GroupBox4)
    Me.Controls.Add(Me.GroupBox3)
    Me.Controls.Add(Me.GroupBox2)
    Me.Controls.Add(Me.GroupBox1)
    Me.Controls.Add(Me.BtnConnect)
    Me.MaximumSize = New System.Drawing.Size(300, 460)
    Me.MinimumSize = New System.Drawing.Size(300, 460)
    Me.Name = "DlgStartupSequence"
    Me.Text = "J-Link Startup Sequence"
    Me.GroupBox1.ResumeLayout(False)
    Me.GroupBox1.PerformLayout()
    Me.GroupBox2.ResumeLayout(False)
    Me.GroupBox2.PerformLayout()
    Me.GroupBox3.ResumeLayout(False)
    Me.GroupBox3.PerformLayout()
    CType(Me.NumTIFSpeed, System.ComponentModel.ISupportInitialize).EndInit()
    Me.GroupBox4.ResumeLayout(False)
    Me.GroupBox4.PerformLayout()
    Me.GroupBox5.ResumeLayout(False)
    Me.GroupBox5.PerformLayout()
    Me.ResumeLayout(False)

  End Sub
  Friend WithEvents BtnConnect As System.Windows.Forms.Button
  Friend WithEvents TxtSN As System.Windows.Forms.TextBox
  Friend WithEvents RdoIFUSB As System.Windows.Forms.RadioButton
  Friend WithEvents RdoIFIP As System.Windows.Forms.RadioButton
  Friend WithEvents TxtIP As System.Windows.Forms.TextBox
  Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
  Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
  Friend WithEvents TxtDevice As System.Windows.Forms.TextBox
  Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
  Friend WithEvents Label1 As System.Windows.Forms.Label
  Friend WithEvents CmbTIF As System.Windows.Forms.ComboBox
  Friend WithEvents Label2 As System.Windows.Forms.Label
  Friend WithEvents NumTIFSpeed As System.Windows.Forms.NumericUpDown
  Friend WithEvents GroupBox4 As System.Windows.Forms.GroupBox
  Friend WithEvents TxtSettingsFile As System.Windows.Forms.TextBox
  Friend WithEvents GroupBox5 As System.Windows.Forms.GroupBox
  Friend WithEvents TxtLog As System.Windows.Forms.TextBox

End Class
