namespace WindowsFormsApplication1
{
    partial class StartupSequenceGUI
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.TxtIP = new System.Windows.Forms.TextBox();
      this.TxtUSB = new System.Windows.Forms.TextBox();
      this.RdoIFIP = new System.Windows.Forms.RadioButton();
      this.RdoIFUSB = new System.Windows.Forms.RadioButton();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.TxtDevice = new System.Windows.Forms.TextBox();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.TxtPrjFile = new System.Windows.Forms.TextBox();
      this.groupBox5 = new System.Windows.Forms.GroupBox();
      this.NumTIFSpeed = new System.Windows.Forms.NumericUpDown();
      this.CmbTIF = new System.Windows.Forms.ComboBox();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.groupBox4 = new System.Windows.Forms.GroupBox();
      this.TxtLog = new System.Windows.Forms.RichTextBox();
      this.button1 = new System.Windows.Forms.Button();
      this.groupBox6 = new System.Windows.Forms.GroupBox();
      this.TxtDataFile = new System.Windows.Forms.TextBox();
      this.label4 = new System.Windows.Forms.Label();
      this.label3 = new System.Windows.Forms.Label();
      this.TxtAddr = new System.Windows.Forms.TextBox();
      this.button2 = new System.Windows.Forms.Button();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.groupBox3.SuspendLayout();
      this.groupBox5.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.NumTIFSpeed)).BeginInit();
      this.groupBox4.SuspendLayout();
      this.groupBox6.SuspendLayout();
      this.SuspendLayout();
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.TxtIP);
      this.groupBox1.Controls.Add(this.TxtUSB);
      this.groupBox1.Controls.Add(this.RdoIFIP);
      this.groupBox1.Controls.Add(this.RdoIFUSB);
      this.groupBox1.Location = new System.Drawing.Point(12, 12);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(258, 72);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "J-Link Connection";
      // 
      // TxtIP
      // 
      this.TxtIP.Location = new System.Drawing.Point(150, 43);
      this.TxtIP.Name = "TxtIP";
      this.TxtIP.Size = new System.Drawing.Size(102, 20);
      this.TxtIP.TabIndex = 3;
      // 
      // TxtUSB
      // 
      this.TxtUSB.Location = new System.Drawing.Point(150, 14);
      this.TxtUSB.Name = "TxtUSB";
      this.TxtUSB.Size = new System.Drawing.Size(102, 20);
      this.TxtUSB.TabIndex = 2;
      this.TxtUSB.Text = "0";
      // 
      // RdoIFIP
      // 
      this.RdoIFIP.AutoSize = true;
      this.RdoIFIP.Location = new System.Drawing.Point(7, 43);
      this.RdoIFIP.Name = "RdoIFIP";
      this.RdoIFIP.Size = new System.Drawing.Size(137, 17);
      this.RdoIFIP.TabIndex = 1;
      this.RdoIFIP.Text = "Ethernet IP / Hostname";
      this.RdoIFIP.UseVisualStyleBackColor = true;
      // 
      // RdoIFUSB
      // 
      this.RdoIFUSB.AutoSize = true;
      this.RdoIFUSB.Checked = true;
      this.RdoIFUSB.Location = new System.Drawing.Point(7, 20);
      this.RdoIFUSB.Name = "RdoIFUSB";
      this.RdoIFUSB.Size = new System.Drawing.Size(114, 17);
      this.RdoIFUSB.TabIndex = 0;
      this.RdoIFUSB.TabStop = true;
      this.RdoIFUSB.Text = "USB Serial number";
      this.RdoIFUSB.UseVisualStyleBackColor = true;
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.TxtDevice);
      this.groupBox2.Location = new System.Drawing.Point(12, 90);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(258, 47);
      this.groupBox2.TabIndex = 1;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Target Device";
      // 
      // TxtDevice
      // 
      this.TxtDevice.Location = new System.Drawing.Point(7, 19);
      this.TxtDevice.Name = "TxtDevice";
      this.TxtDevice.Size = new System.Drawing.Size(245, 20);
      this.TxtDevice.TabIndex = 0;
      this.TxtDevice.Text = "STM32F407VE";
      // 
      // groupBox3
      // 
      this.groupBox3.Controls.Add(this.TxtPrjFile);
      this.groupBox3.Location = new System.Drawing.Point(12, 239);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(258, 48);
      this.groupBox3.TabIndex = 2;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "J-Link Settings File";
      // 
      // TxtPrjFile
      // 
      this.TxtPrjFile.Location = new System.Drawing.Point(7, 19);
      this.TxtPrjFile.Name = "TxtPrjFile";
      this.TxtPrjFile.Size = new System.Drawing.Size(245, 20);
      this.TxtPrjFile.TabIndex = 1;
      // 
      // groupBox5
      // 
      this.groupBox5.Controls.Add(this.NumTIFSpeed);
      this.groupBox5.Controls.Add(this.CmbTIF);
      this.groupBox5.Controls.Add(this.label2);
      this.groupBox5.Controls.Add(this.label1);
      this.groupBox5.Location = new System.Drawing.Point(12, 143);
      this.groupBox5.Name = "groupBox5";
      this.groupBox5.Size = new System.Drawing.Size(258, 90);
      this.groupBox5.TabIndex = 2;
      this.groupBox5.TabStop = false;
      this.groupBox5.Text = "Target Connection";
      // 
      // NumTIFSpeed
      // 
      this.NumTIFSpeed.Increment = new decimal(new int[] {
            100,
            0,
            0,
            0});
      this.NumTIFSpeed.Location = new System.Drawing.Point(129, 61);
      this.NumTIFSpeed.Maximum = new decimal(new int[] {
            15000,
            0,
            0,
            0});
      this.NumTIFSpeed.Minimum = new decimal(new int[] {
            100,
            0,
            0,
            0});
      this.NumTIFSpeed.Name = "NumTIFSpeed";
      this.NumTIFSpeed.Size = new System.Drawing.Size(120, 20);
      this.NumTIFSpeed.TabIndex = 9;
      this.NumTIFSpeed.Value = new decimal(new int[] {
            4000,
            0,
            0,
            0});
      // 
      // CmbTIF
      // 
      this.CmbTIF.FormattingEnabled = true;
      this.CmbTIF.Items.AddRange(new object[] {
            "SWD",
            "JTAG"});
      this.CmbTIF.Location = new System.Drawing.Point(129, 29);
      this.CmbTIF.Name = "CmbTIF";
      this.CmbTIF.Size = new System.Drawing.Size(120, 21);
      this.CmbTIF.TabIndex = 8;
      this.CmbTIF.Text = "SWD";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(7, 61);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(114, 13);
      this.label2.TabIndex = 7;
      this.label2.Text = "Interface Speed [kHz]:";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(7, 32);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(86, 13);
      this.label1.TabIndex = 6;
      this.label1.Text = "Target Interface:";
      // 
      // groupBox4
      // 
      this.groupBox4.Controls.Add(this.TxtLog);
      this.groupBox4.Location = new System.Drawing.Point(12, 428);
      this.groupBox4.Name = "groupBox4";
      this.groupBox4.Size = new System.Drawing.Size(258, 100);
      this.groupBox4.TabIndex = 3;
      this.groupBox4.TabStop = false;
      this.groupBox4.Text = "Log";
      // 
      // TxtLog
      // 
      this.TxtLog.Location = new System.Drawing.Point(7, 19);
      this.TxtLog.Name = "TxtLog";
      this.TxtLog.Size = new System.Drawing.Size(245, 71);
      this.TxtLog.TabIndex = 3;
      this.TxtLog.Text = "";
      // 
      // button1
      // 
      this.button1.Location = new System.Drawing.Point(12, 402);
      this.button1.Name = "button1";
      this.button1.Size = new System.Drawing.Size(121, 23);
      this.button1.TabIndex = 4;
      this.button1.Text = "Program";
      this.button1.UseVisualStyleBackColor = true;
      this.button1.Click += new System.EventHandler(this.button1_Click);
      // 
      // groupBox6
      // 
      this.groupBox6.Controls.Add(this.TxtDataFile);
      this.groupBox6.Controls.Add(this.label4);
      this.groupBox6.Controls.Add(this.label3);
      this.groupBox6.Controls.Add(this.TxtAddr);
      this.groupBox6.Location = new System.Drawing.Point(12, 293);
      this.groupBox6.Name = "groupBox6";
      this.groupBox6.Size = new System.Drawing.Size(258, 103);
      this.groupBox6.TabIndex = 5;
      this.groupBox6.TabStop = false;
      this.groupBox6.Text = "Device Programming";
      // 
      // TxtDataFile
      // 
      this.TxtDataFile.Location = new System.Drawing.Point(7, 68);
      this.TxtDataFile.Name = "TxtDataFile";
      this.TxtDataFile.Size = new System.Drawing.Size(242, 20);
      this.TxtDataFile.TabIndex = 9;
      this.TxtDataFile.Text = "";
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(14, 49);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(49, 13);
      this.label4.TabIndex = 8;
      this.label4.Text = "Data file:";
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(11, 22);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(74, 13);
      this.label3.TabIndex = 7;
      this.label3.Text = "Address (hex):";
      // 
      // TxtAddr
      // 
      this.TxtAddr.Location = new System.Drawing.Point(103, 19);
      this.TxtAddr.Name = "TxtAddr";
      this.TxtAddr.Size = new System.Drawing.Size(147, 20);
      this.TxtAddr.TabIndex = 1;
      this.TxtAddr.Text = "20000000";
      // 
      // button2
      // 
      this.button2.Location = new System.Drawing.Point(162, 402);
      this.button2.Name = "button2";
      this.button2.Size = new System.Drawing.Size(108, 23);
      this.button2.TabIndex = 6;
      this.button2.Text = "Erase";
      this.button2.UseVisualStyleBackColor = true;
      this.button2.Click += new System.EventHandler(this.button2_Click);
      // 
      // StartupSequenceGUI
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(286, 540);
      this.Controls.Add(this.button2);
      this.Controls.Add(this.groupBox6);
      this.Controls.Add(this.button1);
      this.Controls.Add(this.groupBox5);
      this.Controls.Add(this.groupBox4);
      this.Controls.Add(this.groupBox3);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Name = "StartupSequenceGUI";
      this.Text = "J-Link Download & Erase";
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      this.groupBox3.ResumeLayout(false);
      this.groupBox3.PerformLayout();
      this.groupBox5.ResumeLayout(false);
      this.groupBox5.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.NumTIFSpeed)).EndInit();
      this.groupBox4.ResumeLayout(false);
      this.groupBox6.ResumeLayout(false);
      this.groupBox6.PerformLayout();
      this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton RdoIFIP;
        private System.Windows.Forms.RadioButton RdoIFUSB;
        private System.Windows.Forms.TextBox TxtDevice;
        private System.Windows.Forms.TextBox TxtIP;
        private System.Windows.Forms.TextBox TxtUSB;
        private System.Windows.Forms.TextBox TxtPrjFile;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.RichTextBox TxtLog;
        private System.Windows.Forms.ComboBox CmbTIF;
        private System.Windows.Forms.NumericUpDown NumTIFSpeed;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox TxtAddr;
        private System.Windows.Forms.TextBox TxtDataFile;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button2;
    }
}

