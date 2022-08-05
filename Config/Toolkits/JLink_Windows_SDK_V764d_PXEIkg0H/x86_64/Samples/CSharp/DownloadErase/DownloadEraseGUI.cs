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

    /*********************************************************************
    *
    *       Connect()
    *
    *  Function description
    *    Connects to J-Link and the target with the parameters set by user.
    */
    private int Connect() {
      int Result;
      byte[] acOut;
      byte[] acIn;
      string sError;
      //
      // Init locals
      //
      acOut = new byte[256];
      acIn  = new byte[256];
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
            return -1;
          }
        }
      } else if (RdoIFIP.Checked == true) {  // Ethernet?
        Log(" IP...");
        if (TxtIP.Text != "") {
          Result = JLink.SelectIP(TxtIP.Text, 0);
          if (Result < 0) {
            LogLine("Failed");
            return -1;
          }
        }
      } else { // None?
        Log(" <unknown>...");
        LogLine("Failed");
        return -1;
      }
      LogLine("O.K.");
      //
      // Open connection to J-Link
      //
      Log("Connecting to J-Link...");
      sError = JLink.OpenEx(null, pJLINK_LOG_callback_pfErrorOut);
      if (sError != null) {
        Log(sError);
        return -1;
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
        JLink.Close();
        return -1;
      }
      LogLine("O.K.");
      return 0;
    }

    private void button1_Click(object sender, EventArgs e) {
      int Result;
      U32 Addr;
      //
      // Connect to device
      //
      Result = Connect();
      if (Result < 0) {
        return;
      }
      Addr = Convert.ToUInt32(TxtAddr.Text, 16);          // Get Addr
      Result = JLink.DownloadFile(TxtDataFile.Text, Addr);
      if (Result < 0) {
        LogLine("Failed");
      } else {
        LogLine("O.K.");
      }
      //
      // Close connection
      //
      JLink.Close();
      LogLine("Disconnected from target and J-Link.");
      return;
    }

    private void button2_Click(object sender, EventArgs e) {
      int Result;
      Result = Connect();
      if (Result < 0) {
        return;
      }
      Log("Erasing Chip...");
      Result = JLink.EraseChip();
      if (Result < 0) {
        LogLine("Failed");
      } else {
        LogLine("O.K.");
      }
      //
      // Close connection
      //
      JLink.Close();
      LogLine("Disconnected from target and J-Link.");
      return;
    }
  }
}
