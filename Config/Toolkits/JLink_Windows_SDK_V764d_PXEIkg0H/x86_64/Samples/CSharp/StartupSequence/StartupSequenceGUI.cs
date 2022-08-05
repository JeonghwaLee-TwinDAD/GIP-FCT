using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using JLinkSDK;

using U8  = System.Byte;   // unsigned char
using U16 = System.UInt16; // unsigned short
using U32 = System.UInt32; // unsigned int
using U64 = System.UInt64; // unsigned long long

using I8  = System.SByte; // signed char
using I16 = System.Int16; // signed short
using I32 = System.Int32; // signed int
using I64 = System.Int64; // signed long long

namespace WindowsFormsApplication1 {
  public partial class StartupSequenceGUI : Form {
    /*********************************************************************
    *
    *       Log()
    *
    *  Function description
    *    Outputs a string to the log window
    */
    private void Log(String Text) {
      TxtLog.Text += Text;
      TxtLog.SelectionStart = TxtLog.Text.Length;
      TxtLog.ScrollToCaret();
    }

    /*********************************************************************
    *
    *       LogLine()
    *
    *  Function description
    *    Outputs a string to the log window and appends a \n
    */
    private void LogLine(String Text) {
      TxtLog.Text += Text + '\n';
      TxtLog.SelectionStart = TxtLog.Text.Length;
      TxtLog.ScrollToCaret();
    }

    /*********************************************************************
    *
    *       JLINK_LOG_callback_pfErrorOut()
    *
    *  Function description
    *    Callback function passed to the J-Link DLL to manage error out.
    */
    private static void JLINK_LOG_callback_pfErrorOut(string s) {
    }

    private static JLink.JLINK_LOG pJLINK_LOG_callback_pfErrorOut = JLINK_LOG_callback_pfErrorOut;

    /*********************************************************************
    *
    *       StartupSequenceGUI()
    *
    */
    public StartupSequenceGUI() {
      InitializeComponent();
    }

    private void button1_Click(object sender, EventArgs e) {
      JLINKARM_EMU_CONNECT_INFO[] aConnectInfo;
      int r;
      int i;
      int Result;
      byte[] acOut;
      byte[] acIn;
      string sError;
      string sTmp;
      U32[] acData;
      U8[] acTmp;
      U32 NumItems;
      U32 Addr;
      //
      // Init locals
      //
      acOut  = new byte[256];
      acIn   = new byte[256];
      acData = new U32[16];
      acTmp  = new U8[100];
      //
      // Select HIF
      //
      Log("Selecting host interface");
      if (RdoIFUSB.Checked == true) { // USB?
        Log(" USB...");
        if (TxtUSB.Text != "0") {
          Result = JLink.EMU_SelectByUSBSN(Convert.ToInt32(TxtUSB.Text));
          if (Result < 0) {
            LogLine("Failed");
            return;
          }
        }             
      } else if (RdoIFIP.Checked == true) {  // Ethernet?
        Log(" IP...");
        if (TxtIP.Text != "") {
          Result = JLink.SelectIP(TxtIP.Text, 0);
          if (Result < 0) {
            LogLine("Failed");
            return;
          }
        }
      }
      else { // None?
        Log(" <unknown>...");
        LogLine("Failed");
        return;
      }
      LogLine("O.K.");
      //
      // Print information about connected probes/ programmers
      //
      LogLine("Listing information about connected probes/ programmers...");
      aConnectInfo = new JLINKARM_EMU_CONNECT_INFO[16];
      r = JLink.GetEmuList(JLink.HOSTIF_USB | JLink.HOSTIF_IP, aConnectInfo, 16);  // Get information about up to 16 probes/ programmers connected via USB or TCP/IP
      if (r < 0) {
        LogLine("Failed to retrieve information about connected probes/ programmers...");
      } else {
        LogLine("Found " + r + " connected probes/ programmers!");
        r = (r < 16) ? r : 16;   // Do not report about more probes/ programmers than we were able to store
        for (i = 0; i < r; i++) {
          if (aConnectInfo[i].Connection == JLink.HOSTIF_USB) {
            sTmp = "USB";
          } else {
            sTmp = "TCP/IP";
          }
          LogLine("#" + (i + 1) + " | S/N = " + aConnectInfo[i].SerialNumber + " | Product = " + aConnectInfo[i].acProduct + " | Connected via " + sTmp);
        }
      }
      LogLine("O.K.");
      //
      // Open connection to J-Link
      //
      Log("Connecting to J-Link...");
      sError = JLink.OpenEx(null, pJLINK_LOG_callback_pfErrorOut);
      if (sError != null) {
        Log(sError);
        return;
      }
      LogLine("O.K.");
      //
      // Setup settings file
      //
      acIn = Array.ConvertAll((Encoding.ASCII.GetBytes("ProjectFile = " + TxtPrjFile.Text)), q => Convert.ToByte(q));
      JLink.ExecCommand(acIn, acOut, 256);
      //
      // Select device
      //
      acIn = Array.ConvertAll((Encoding.ASCII.GetBytes("Device = " + TxtDevice.Text)), q => Convert.ToByte(q));
      JLink.ExecCommand(acIn, acOut, 256);
      //
      // Select TIF (JTAG / SWD / ...) + TIF speed
      //
      if (CmbTIF.Text == "JTAG") {
        JLink.TIF_Select(0);
      } else {
        JLink.TIF_Select(1);
      }
      JLink.SetSpeed(Convert.ToInt32(NumTIFSpeed.Value));
      //
      // Connect to target
      //
      Result = JLink.Connect();
      Log("Connecting to the target...");
      if (Result < 0) {
        LogLine("Failed");
        goto Close;
      }
      LogLine("O.K.");
      //
      // Read 16 words from the target
      //
      NumItems = 16;                                  // Number of items to read
      Addr = Convert.ToUInt32(TxtAddr.Text, 16);      // Get Addr
      Result = JLink.ReadMemEx(Addr, NumItems * 4, acData);
      Log("Trying to read " + NumItems + " words...");
      if (Result < 0) {
        LogLine("Failed");
        goto Close;
      } else {
        LogLine("O.K.");
        //
        // Output bytes
        //
        for (i = 0; i < NumItems; i += 4) {
          TxtReadMem.Text += String.Format("{0:X8}", acData[i + 0]) + " ";
          TxtReadMem.Text += String.Format("{0:X8}", acData[i + 1]) + " ";
          TxtReadMem.Text += String.Format("{0:X8}", acData[i + 2]) + " ";
          TxtReadMem.Text += String.Format("{0:X8}", acData[i + 3]) + "\n";
        }
      }
      //
      // Close connection
      //
Close:
      JLink.Close();
      LogLine("Disconnected from target and J-Link.");
    }

    /*********************************************************************
    *
    *       button2_Click()
    *
    *  Function description
    *    Callback function which is called when the "..." button for selecting a device is clicked.
    *    This function opens a device selection dialog and updates the device text accordingly.
    */
    private void button2_Click(object sender, EventArgs e) {
      JLINKARM_DEVICE_INFO   DevInfoDummy;
      JLINKARM_DEVICE_INFO[] aDevInfo;
      U32 NumItems;
      U32 i;
      int r;

      DevInfoDummy = new JLINKARM_DEVICE_INFO();
      r = JLink.GetDeviceInfo(-1, ref DevInfoDummy);
      if (r < 1) {
        LogLine("Failed to get list of devices...");
        return;
      }
      NumItems = (U32)r;
      aDevInfo = new JLINKARM_DEVICE_INFO[NumItems];
      for (i = 0; i < NumItems; i++) {
        JLink.GetDeviceInfo((int)i, ref aDevInfo[i]);
      }
      r = DeviceSelection.ShowDialog(aDevInfo);
      if (r >= 0) {  // User selected a device?
        TxtDevice.Text = aDevInfo[r].sName;
      }
    }
  }

  /*********************************************************************
  *
  *       DeviceSelection()
  *
  *  Class description
  *    Class which defines behavior + GUI of device selection dialog
  */
  public static class DeviceSelection {
    static Form         MyForm;
    static DataGridView TblView;
    static DataTable    DataTbl;  
    static TextBox      TxtFilter;
    static int          SelectedIndex;

    /*********************************************************************
    *
    *       _cbOnTextChange()
    *
    *  Function description
    *    Callback function which is called when the text inside the device name filter textbox changes.
    *    This function updates the table view filter accordingly.
    */
    private static void _cbOnTextChange(object Sender, EventArgs e) {
      string sFilter;

      sFilter = string.Format("Device LIKE '%" + TxtFilter.Text + "%'");  // Filter by looking for entries inside "Device" column in DataTable
      (TblView.DataSource as DataTable).DefaultView.RowFilter = sFilter;
    }

    /*********************************************************************
    *
    *       _SelectDevice()
    *
    *  Function description
    *    This function selects the device of the currently selected row and closes the dialog.
    */
    private static void _SelectDevice() {
      DataRowView DataEntry;
      DataGridViewRow SelectedRow;

      if (TblView.SelectedRows.Count != 1) {  // Sanity-check: More or less than 1 row selected? => Early-out
        SelectedIndex = -1;
      } else {
        SelectedRow   = TblView.SelectedRows[0];
        DataEntry     = (DataRowView)SelectedRow.DataBoundItem;
        SelectedIndex = (int)DataEntry.Row.ItemArray[0];
      }
      MyForm.Close();
    }

    /*********************************************************************
    *
    *       _cbOnBtnOK()
    *
    *  Function description
    *    Callback function which is called when the "OK" button is pressed.
    */
    private static void _cbOnBtnOK(object Sender, EventArgs e) {
      _SelectDevice();
    }

    /*********************************************************************
    *
    *       _cbOnCellDblClick()
    *
    *  Function description
    *    Callback function which is called when a cell in the device table is double-clicked.
    */
    private static void _cbOnCellDblClick(object Sender, EventArgs e) {
      _SelectDevice();
    }

    /*********************************************************************
    *
    *       ShowDialog()
    *
    *  Function description
    *    This function is the "entry point" for the device selection dialog and initializes its GUI accordingly.
    */
    public static int ShowDialog(JLINKARM_DEVICE_INFO[] aInfo) {
      U32 i;
      U32 NumItems;
      Button BtnOk;
      FlowLayoutPanel Layout;
      //
      // Device selection dialog layout:
      // +------------------+
      // | <TxtBoxFilter>   |
      // +------------------+
      // | <TblViewDevices> |
      // +------------------+
      // | <BtnOK>          |
      // +------------------+
      //
      MyForm = new Form();
      //
      // Create <TxtBoxFilter>
      //
      TxtFilter = new TextBox();
      TxtFilter.TextChanged += new System.EventHandler(_cbOnTextChange);  // Register callback for when user enters device name filter
      //
      // Create + initialize data table for <TblViewDevices>
      //
      DataTbl = new DataTable();
      DataTbl.Columns.Add("DeviceIndex",  typeof(int));      // Hidden column for being able to associate table row with the corresponding list entry later on
      DataTbl.Columns.Add("Manufacturer", typeof(string));
      DataTbl.Columns.Add("Device",       typeof(string));
      DataTbl.Columns[0].ColumnMapping = MappingType.Hidden;
      NumItems = (uint)aInfo.Length;
      for (i = 0; i < NumItems; i++) {
        DataTbl.Rows.Add(new Object[] {i, aInfo[i].sManu, aInfo[i].sName});
      }
      //
      // Create <TblViewDevices>
      //
      TblView = new DataGridView();
      TblView.ReadOnly = true;
      TblView.RowHeadersVisible = false;
      TblView.SelectionMode = DataGridViewSelectionMode.FullRowSelect;
      TblView.MultiSelect = false;
      TblView.DataSource = DataTbl;
      TblView.Width  = 250;
      TblView.Height = 200;
      TblView.CellDoubleClick += new System.Windows.Forms.DataGridViewCellEventHandler(_cbOnCellDblClick);  // Register callback for when user enters double-clicks a table cell
      //
      // Create <BtnOK>
      //
      BtnOk        = new Button();
      BtnOk.Text   = "OK";
      BtnOk.Click += new System.EventHandler(_cbOnBtnOK);  // Register callback for when user enters presses "OK"
      //
      // Create panel for desired arrangement of GUI elements
      //
      Layout = new FlowLayoutPanel();
      Layout.Width  = 500;
      Layout.Height = 500;
      Layout.FlowDirection = FlowDirection.TopDown;
      Layout.Controls.Add(TxtFilter);
      Layout.Controls.Add(TblView);
      Layout.Controls.Add(BtnOk);
      //
      // Finally, fill form and show dialog
      //
      MyForm.Controls.Add(Layout);
      MyForm.Text = "Device Selection";
      SelectedIndex = -1;
      MyForm.ShowDialog();
      return SelectedIndex;
    }
  }
}
