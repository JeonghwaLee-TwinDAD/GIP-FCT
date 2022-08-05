namespace JLink_Find_Emulators
{
  partial class FindEmulatorsGUI
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
      this.button2 = new System.Windows.Forms.Button();
      this.GridDevices = new System.Windows.Forms.DataGridView();
      ((System.ComponentModel.ISupportInitialize)(this.GridDevices)).BeginInit();
      this.SuspendLayout();
      // 
      // button2
      // 
      this.button2.Location = new System.Drawing.Point(446, 442);
      this.button2.Margin = new System.Windows.Forms.Padding(4);
      this.button2.Name = "button2";
      this.button2.Size = new System.Drawing.Size(79, 30);
      this.button2.TabIndex = 1;
      this.button2.Text = "Search";
      this.button2.UseVisualStyleBackColor = true;
      this.button2.Click += new System.EventHandler(this.SearchDevices);
      // 
      // GridDevices
      // 
      this.GridDevices.AllowUserToAddRows = false;
      this.GridDevices.AllowUserToDeleteRows = false;
      this.GridDevices.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
      this.GridDevices.Location = new System.Drawing.Point(12, 12);
      this.GridDevices.Name = "GridDevices";
      this.GridDevices.ReadOnly = true;
      this.GridDevices.RowTemplate.Height = 24;
      this.GridDevices.Size = new System.Drawing.Size(513, 423);
      this.GridDevices.TabIndex = 2;
      // 
      // FindEmulatorsGUI
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(536, 480);
      this.Controls.Add(this.GridDevices);
      this.Controls.Add(this.button2);
      this.Name = "FindEmulatorsGUI";
      this.Text = "J-Link Find Emulators";
      ((System.ComponentModel.ISupportInitialize)(this.GridDevices)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.Button button2;
    private System.Windows.Forms.DataGridView GridDevices;
  }
}

