using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using JLinkSDK;

namespace JLink_Find_Emulators
{
  public partial class FindEmulatorsGUI : Form
  {

    /*********************************************************************
    *
    *       FindDevicesGUI()
    *
    *  Function description
    *    Constructor for the GUI class. 
    */
    public FindEmulatorsGUI()
    {
      InitializeComponent();
    }

    /*********************************************************************
    *
    *       SearchDevices()
    *
    *  Function description
    *    This function is triggered, when the user presses the Search 
     *    button. This function reads the connected devices with the 
     *    SDK GetEmuList function to retrieve all data. Afterwards, 
     *    the data is feed into a data table, which is linked to a 
     *    DataGridView. 
    */
    private unsafe void SearchDevices(object sender, EventArgs e)
    {
      int NumDevices;
      int i;
      int j;
      const int MaxDevices = 255;
      JLINKARM_EMU_CONNECT_INFO[] CInfo = new JLINKARM_EMU_CONNECT_INFO[MaxDevices];
      DataTable DataTbl;      

      //
      // Retrieve data from JLink DLL and get number of connected devices. 
      //
      NumDevices = JLink.GetEmuList(MaxDevices, CInfo, MaxDevices);      
      GridDevices.ReadOnly = true;
      GridDevices.AllowUserToAddRows = false;
      GridDevices.AllowUserToDeleteRows = false;
      DataTbl = new DataTable();
      DataTbl.Columns.Add("Serial No", typeof(string));
      DataTbl.Columns.Add("Product",   typeof(string));
      DataTbl.Columns.Add("Nickname",  typeof(string));
      for (i = 0; i < NumDevices; ++i)
      {     
        DataTbl.Rows.Add(new Object[] {
          CInfo[i].SerialNumber.ToString(),           
          CInfo[i].acProduct, 
          CInfo[i].acNickName, 
        });        
      }
      GridDevices.DataSource = DataTbl;
      for (i = 0; i < DataTbl.Columns.Count; ++i)
      {
        GridDevices.Columns[i].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
      }      
    }

  }
}
