<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="21008000">
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">false</Property>
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Configuration" Type="Folder">
			<Item Name="Controls" Type="Folder">
				<Item Name="DAQ_Data.ctl" Type="VI" URL="../../Sequence/CodeModules/Load INI/Controls/DAQ_Data.ctl"/>
				<Item Name="DMM_Data.ctl" Type="VI" URL="../../Sequence/CodeModules/Load INI/Controls/DMM_Data.ctl"/>
				<Item Name="Instruments_Data.ctl" Type="VI" URL="../../Sequence/CodeModules/Load INI/Controls/Instruments_Data.ctl"/>
				<Item Name="MUX_Data.ctl" Type="VI" URL="../../Sequence/CodeModules/Load INI/Controls/MUX_Data.ctl"/>
			</Item>
			<Item Name="SubVIs" Type="Folder">
				<Item Name="Load Hardware Config.vi" Type="VI" URL="../../Sequence/CodeModules/Load INI/Load Hardware Config.vi"/>
				<Item Name="SetGetFGV.vi" Type="VI" URL="../../Sequence/CodeModules/Load INI/SetGetFGV.vi"/>
			</Item>
		</Item>
		<Item Name="DUT PrePost" Type="Folder">
			<Item Name="Data" Type="Folder">
				<Item Name="Data.ctl" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/Data/Data.ctl"/>
				<Item Name="InitialStates.ctl" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/Data/InitialStates.ctl"/>
			</Item>
			<Item Name="FPIcons" Type="Folder">
				<Item Name="Barcode_Background_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/Barcode_Background_1x.png"/>
				<Item Name="E Stop_Dark_Yellow_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/E Stop_Dark_Yellow_1x.png"/>
				<Item Name="E Stop_Light_Yellow_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/E Stop_Light_Yellow_1x.png"/>
				<Item Name="Error_Dark_Touch_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/Error_Dark_Touch_1x.png"/>
				<Item Name="Fail_Dark_Touch_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/Fail_Dark_Touch_1x.png"/>
				<Item Name="Login_Background_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/Login_Background_1x.png"/>
				<Item Name="Pass_Dark_Touch_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/Pass_Dark_Touch_1x.png"/>
				<Item Name="Terminated_Dark_Touch_1x.png" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/FPIcons/Terminated_Dark_Touch_1x.png"/>
			</Item>
			<Item Name="LogIn" Type="Folder">
				<Item Name="DoGetNewUser.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/DoGetNewUser.vi"/>
				<Item Name="DoLoginUser.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/DoLoginUser.vi"/>
				<Item Name="DoLogoutUser.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/DoLogoutUser.vi"/>
				<Item Name="GetAllUsers.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/GetAllUsers.vi"/>
				<Item Name="LogInDialog.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/LogInDialog.vi"/>
				<Item Name="LoginUser.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/LoginUser.vi"/>
				<Item Name="SetLogInOutVal.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn/SetLogInOutVal.vi"/>
			</Item>
			<Item Name="SubVIs" Type="Folder">
				<Item Name="FindRowIndex.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/FindRowIndex.vi"/>
				<Item Name="GetVariablesValues.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/GetVariablesValues.vi"/>
				<Item Name="LoadINIFile.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/LoadINIFile.vi"/>
				<Item Name="ScreenCoordinates.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/ScreenCoordinates.vi"/>
				<Item Name="Set Calling VI Wnd Topmost &amp; Active.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/Set Calling VI Wnd Topmost &amp; Active.vi"/>
				<Item Name="Set VI FP Topmost.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/Set VI FP Topmost.vi"/>
				<Item Name="SetModelAndSuffix.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/SetModelAndSuffix.vi"/>
				<Item Name="WriteToFile.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/SubVIs/WriteToFile.vi"/>
			</Item>
			<Item Name="CommonDialogSingleBttn.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/CommonDialogSingleBttn.vi"/>
			<Item Name="DUTResults.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/DUTResults.vi"/>
			<Item Name="DUTResultsDialog.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/DUTResultsDialog.vi"/>
			<Item Name="DUTResultsDialogSequential.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/DUTResultsDialogSequential.vi"/>
			<Item Name="DUTResultsSequential.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/DUTResultsSequential.vi"/>
			<Item Name="LogIn.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/LogIn.vi"/>
			<Item Name="readme.md" Type="Document" URL="../../Sequence/CodeModules/DUT PrePost/readme.md"/>
			<Item Name="UUTInfomation.vi" Type="VI" URL="../../Sequence/CodeModules/DUT PrePost/UUTInfomation.vi"/>
		</Item>
		<Item Name="MIO Wrappers.lvlib" Type="Library" URL="../../Sequence/CodeModules/Wrappers/MIO Wrappers.lvlib"/>
		<Item Name="CAN GIP.lvlib" Type="Library" URL="../../Sequence/CodeModules/CANbus/CAN GIP.lvlib"/>
		<Item Name="OPCUA.lvlib" Type="Library" URL="../../Sequence/CodeModules/OPCUA/OPCUA.lvlib"/>
		<Item Name="Display.lvlib" Type="Library" URL="../../Sequence/CodeModules/Test Monitoring/Display.lvlib"/>
		<Item Name="Database Config.ini" Type="Document" URL="../../Config/Database Config.ini"/>
		<Item Name="Hardware Config.ini" Type="Document" URL="../../Config/Hardware Config.ini"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="Delimited String to 1D String Array.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/Delimited String to 1D String Array.vi"/>
				<Item Name="NI_LVConfig.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/config.llb/NI_LVConfig.lvlib"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="8.6CompatibleGlobalVar.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/config.llb/8.6CompatibleGlobalVar.vi"/>
				<Item Name="TestStand - Get Termination Monitor Status.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Termination Monitor Status.vi"/>
				<Item Name="TestStand - Status Monitor.ctl" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Status Monitor.ctl"/>
				<Item Name="TestStand - Initialize Termination Monitor.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Initialize Termination Monitor.vi"/>
				<Item Name="TestStand - Close Termination Monitor.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Close Termination Monitor.vi"/>
				<Item Name="TestStand - Get Property Value.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value.vi"/>
				<Item Name="TestStand - Get Property Value (Boolean Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Boolean Array).vi"/>
				<Item Name="TestStand - Get Property Value (Boolean).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Boolean).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array).vi"/>
				<Item Name="TestStand - Get Property Value (Object).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Object).vi"/>
				<Item Name="TestStand - Get Property Value (String Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (String Array).vi"/>
				<Item Name="TestStand - Get Property Value (String).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (String).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 64-bit Integer}).vi"/>
				<Item Name="TestStand API Numeric Constants.ctl" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand API Numeric Constants.ctl"/>
				<Item Name="TestStand API Numeric Constants.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand API Numeric Constants.vi"/>
				<Item Name="TestStand - Validate Evaluation Types.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Validate Evaluation Types.vi"/>
				<Item Name="lvpalettesupport.dll" Type="Document" URL="/&lt;vilib&gt;/Addons/TestStand/lvpalettesupport.dll"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Reference).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Reference).vi"/>
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
				<Item Name="Open URL in Default Browser.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser.vi"/>
				<Item Name="Open URL in Default Browser (string).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (string).vi"/>
				<Item Name="Open URL in Default Browser core.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser core.vi"/>
				<Item Name="Open URL in Default Browser (path).vi" Type="VI" URL="/&lt;vilib&gt;/Platform/browser.llb/Open URL in Default Browser (path).vi"/>
				<Item Name="Path to URL.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Path to URL.vi"/>
				<Item Name="Path to URL inner.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Path to URL inner.vi"/>
				<Item Name="Escape Characters for HTTP.vi" Type="VI" URL="/&lt;vilib&gt;/printing/PathToURL.llb/Escape Characters for HTTP.vi"/>
				<Item Name="Read PNG File.vi" Type="VI" URL="/&lt;vilib&gt;/picture/png.llb/Read PNG File.vi"/>
				<Item Name="imagedata.ctl" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/imagedata.ctl"/>
				<Item Name="Check Path.vi" Type="VI" URL="/&lt;vilib&gt;/picture/jpeg.llb/Check Path.vi"/>
				<Item Name="Directory of Top Level VI.vi" Type="VI" URL="/&lt;vilib&gt;/picture/jpeg.llb/Directory of Top Level VI.vi"/>
				<Item Name="Create Mask By Alpha.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Create Mask By Alpha.vi"/>
				<Item Name="Bit-array To Byte-array.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Bit-array To Byte-array.vi"/>
				<Item Name="TestStand - Start Modal Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Start Modal Dialog.vi"/>
				<Item Name="TestStand - Start Modal Dialog (Sequence Context).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Start Modal Dialog (Sequence Context).vi"/>
				<Item Name="TestStand - Start Modal Dialog (Private).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Start Modal Dialog (Private).vi"/>
				<Item Name="TestStand - Start Modal Dialog (Engine).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Start Modal Dialog (Engine).vi"/>
				<Item Name="TestStand - Set Property Value.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value.vi"/>
				<Item Name="TestStand - Set Property Value (Boolean Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Boolean Array).vi"/>
				<Item Name="TestStand - Set Property Value (Boolean).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Boolean).vi"/>
				<Item Name="TestStand - Set Property Value (Number).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number).vi"/>
				<Item Name="TestStand - Set Property Value (Object).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Object).vi"/>
				<Item Name="TestStand - Set Property Value (String Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (String Array).vi"/>
				<Item Name="TestStand - Set Property Value (String).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (String).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Signed 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Signed 64-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Unsigned 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Unsigned 64-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Signed 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Signed 64-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Unsigned 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Unsigned 64-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Signed 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Signed 8-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Unsigned 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Unsigned 8-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Signed 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Signed 16-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Unsigned 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Unsigned 16-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Signed 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Signed 32-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Numeric Array {Unsigned 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Numeric Array {Unsigned 32-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Signed 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Signed 8-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Unsigned 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Unsigned 8-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Signed 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Signed 16-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Unsigned 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Unsigned 16-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Signed 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Signed 32-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Number {Unsigned 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Number {Unsigned 32-bit Integer}).vi"/>
				<Item Name="TestStand - Set Property Value (Reference).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Set Property Value (Reference).vi"/>
				<Item Name="TestStand - End Modal Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - End Modal Dialog.vi"/>
				<Item Name="TestStand - End Modal Dialog (Engine).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - End Modal Dialog (Engine).vi"/>
				<Item Name="TestStand - End Modal Dialog (Private).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - End Modal Dialog (Private).vi"/>
				<Item Name="TestStand - End Modal Dialog (Sequence Context).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - End Modal Dialog (Sequence Context).vi"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="General Error Handler Core CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler Core CORE.vi"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="LVRectTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVRectTypeDef.ctl"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="Stall Data Flow.vim" Type="VI" URL="/&lt;vilib&gt;/Utility/Stall Data Flow.vim"/>
				<Item Name="DAQmx Write.vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write.vi"/>
				<Item Name="DAQmx Write (Analog 1D DBL 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 1D DBL 1Chan NSamp).vi"/>
				<Item Name="DAQmx Fill In Error Info.vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/miscellaneous.llb/DAQmx Fill In Error Info.vi"/>
				<Item Name="DAQmx Write (Analog 1D DBL NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 1D DBL NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Analog 1D Wfm NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 1D Wfm NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Analog 2D DBL NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 2D DBL NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Analog DBL 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog DBL 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Analog Wfm 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog Wfm 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Analog Wfm 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog Wfm 1Chan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital 2D U32 NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 2D U32 NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital 2D U8 NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 2D U8 NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital 1D Bool 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D Bool 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital 1D U32 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D U32 1Chan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital 1D U8 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D U8 1Chan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital Bool 1Line 1Point).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital Bool 1Line 1Point).vi"/>
				<Item Name="DAQmx Write (Digital Wfm 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital Wfm 1Chan NSamp).vi"/>
				<Item Name="DWDT Uncompress Digital.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/DWDTOps.llb/DWDT Uncompress Digital.vi"/>
				<Item Name="DTbl Uncompress Digital.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/DTblOps.llb/DTbl Uncompress Digital.vi"/>
				<Item Name="DTbl Digital Size.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/DTblOps.llb/DTbl Digital Size.vi"/>
				<Item Name="DAQmx Write (Raw 1D I16).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Raw 1D I16).vi"/>
				<Item Name="DAQmx Write (Raw 1D I32).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Raw 1D I32).vi"/>
				<Item Name="DAQmx Write (Raw 1D I8).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Raw 1D I8).vi"/>
				<Item Name="DAQmx Write (Raw 1D U16).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Raw 1D U16).vi"/>
				<Item Name="DAQmx Write (Raw 1D U32).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Raw 1D U32).vi"/>
				<Item Name="DAQmx Write (Raw 1D U8).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Raw 1D U8).vi"/>
				<Item Name="DAQmx Write (Digital 1D Wfm NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D Wfm NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital Wfm 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital Wfm 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Analog 1D Wfm NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 1D Wfm NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital 1D Wfm NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D Wfm NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital U8 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital U8 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital U32 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital U32 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital 1D U32 NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D U32 NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital 1D U8 NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D U8 NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital 2D Bool NChan 1Samp NLine).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 2D Bool NChan 1Samp NLine).vi"/>
				<Item Name="DAQmx Write (Digital 1D Bool NChan 1Samp 1Line).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D Bool NChan 1Samp 1Line).vi"/>
				<Item Name="DAQmx Write (Analog 2D I16 NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 2D I16 NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Analog 2D U16 NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 2D U16 NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Counter Frequency 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter Frequency 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Counter Ticks 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter Ticks 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Counter Time 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter Time 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Counter 1D Frequency NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter 1D Frequency NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Counter 1D Time NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter 1D Time NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Counter 1DTicks NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter 1DTicks NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Analog 2D I32 NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Analog 2D I32 NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital U16 1Chan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital U16 1Chan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital 1D U16 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D U16 1Chan NSamp).vi"/>
				<Item Name="DAQmx Write (Digital 1D U16 NChan 1Samp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 1D U16 NChan 1Samp).vi"/>
				<Item Name="DAQmx Write (Digital 2D U16 NChan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Digital 2D U16 NChan NSamp).vi"/>
				<Item Name="DAQmx Write (Counter 1D Frequency 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter 1D Frequency 1Chan NSamp).vi"/>
				<Item Name="DAQmx Write (Counter 1D Ticks 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter 1D Ticks 1Chan NSamp).vi"/>
				<Item Name="DAQmx Write (Counter 1D Time 1Chan NSamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/write.llb/DAQmx Write (Counter 1D Time 1Chan NSamp).vi"/>
				<Item Name="DAQmx Create Virtual Channel.vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Virtual Channel.vi"/>
				<Item Name="DAQmx Create Channel (AI-Voltage-Basic).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Voltage-Basic).vi"/>
				<Item Name="DAQmx Create Channel (AI-Voltage-Custom with Excitation).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Voltage-Custom with Excitation).vi"/>
				<Item Name="DAQmx Create Channel (AI-Resistance).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Resistance).vi"/>
				<Item Name="DAQmx Create Channel (AI-Temperature-Thermocouple).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Temperature-Thermocouple).vi"/>
				<Item Name="DAQmx Create Channel (AI-Temperature-RTD).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Temperature-RTD).vi"/>
				<Item Name="DAQmx Create Channel (AI-Temperature-Thermistor-Iex).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Temperature-Thermistor-Iex).vi"/>
				<Item Name="DAQmx Create Channel (AI-Temperature-Thermistor-Vex).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Temperature-Thermistor-Vex).vi"/>
				<Item Name="DAQmx Create Channel (AO-Voltage-Basic).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AO-Voltage-Basic).vi"/>
				<Item Name="DAQmx Create Channel (AO-FuncGen).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AO-FuncGen).vi"/>
				<Item Name="DAQmx Create Channel (DI-Digital Input).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (DI-Digital Input).vi"/>
				<Item Name="DAQmx Create Channel (DO-Digital Output).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (DO-Digital Output).vi"/>
				<Item Name="DAQmx Create Channel (CI-Frequency).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Frequency).vi"/>
				<Item Name="DAQmx Create Channel (CI-Period).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Period).vi"/>
				<Item Name="DAQmx Create Channel (CI-Count Edges).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Count Edges).vi"/>
				<Item Name="DAQmx Create Channel (CI-Pulse Width).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Pulse Width).vi"/>
				<Item Name="DAQmx Create Channel (CI-Semi Period).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Semi Period).vi"/>
				<Item Name="DAQmx Create Channel (AI-Current-Basic).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Current-Basic).vi"/>
				<Item Name="DAQmx Create Channel (AI-Strain-Strain Gage).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Strain-Strain Gage).vi"/>
				<Item Name="DAQmx Create Channel (AI-Temperature-Built-in Sensor).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Temperature-Built-in Sensor).vi"/>
				<Item Name="DAQmx Create Channel (AI-Frequency-Voltage).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Frequency-Voltage).vi"/>
				<Item Name="DAQmx Create Channel (CO-Pulse Generation-Frequency).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CO-Pulse Generation-Frequency).vi"/>
				<Item Name="DAQmx Create Channel (CO-Pulse Generation-Time).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CO-Pulse Generation-Time).vi"/>
				<Item Name="DAQmx Create Channel (CO-Pulse Generation-Ticks).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CO-Pulse Generation-Ticks).vi"/>
				<Item Name="DAQmx Create Channel (AI-Position-LVDT).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Position-LVDT).vi"/>
				<Item Name="DAQmx Create Channel (AI-Position-RVDT).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Position-RVDT).vi"/>
				<Item Name="DAQmx Create Channel (CI-Two Edge Separation).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Two Edge Separation).vi"/>
				<Item Name="DAQmx Create Channel (AI-Acceleration-Accelerometer).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Acceleration-Accelerometer).vi"/>
				<Item Name="DAQmx Create Channel (CI-Position-Angular Encoder).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Position-Angular Encoder).vi"/>
				<Item Name="DAQmx Create Channel (CI-Position-Linear Encoder).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Position-Linear Encoder).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Acceleration-Accelerometer).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Acceleration-Accelerometer).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Current-Basic).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Current-Basic).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Position-LVDT).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Position-LVDT).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Position-RVDT).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Position-RVDT).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Resistance).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Resistance).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Strain-Strain Gage).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Strain-Strain Gage).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Temperature-RTD).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Temperature-RTD).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Temperature-Thermistor-Iex).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Temperature-Thermistor-Iex).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Temperature-Thermistor-Vex).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Temperature-Thermistor-Vex).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Voltage-Basic).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Voltage-Basic).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Voltage-Custom with Excitation).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Voltage-Custom with Excitation).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Temperature-Thermocouple).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Temperature-Thermocouple).vi"/>
				<Item Name="DAQmx Create Channel (AI-Sound Pressure-Microphone).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Sound Pressure-Microphone).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Sound Pressure-Microphone).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Sound Pressure-Microphone).vi"/>
				<Item Name="DAQmx Create Channel (CI-GPS Timestamp).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-GPS Timestamp).vi"/>
				<Item Name="DAQmx Create Channel (AO-Current-Basic).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AO-Current-Basic).vi"/>
				<Item Name="DAQmx Create Channel (AI-Voltage-RMS).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Voltage-RMS).vi"/>
				<Item Name="DAQmx Create Channel (AI-Current-RMS).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Current-RMS).vi"/>
				<Item Name="DAQmx Create Channel (AI-Position-EddyCurrentProxProbe).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Position-EddyCurrentProxProbe).vi"/>
				<Item Name="DAQmx Create Channel (CI-Pulse Freq).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Pulse Freq).vi"/>
				<Item Name="DAQmx Create Channel (CI-Pulse Time).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Pulse Time).vi"/>
				<Item Name="DAQmx Create Channel (CI-Pulse Ticks).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Pulse Ticks).vi"/>
				<Item Name="DAQmx Create Channel (AI-Bridge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Bridge).vi"/>
				<Item Name="DAQmx Create Channel (AI-Force-Bridge-Polynomial).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Force-Bridge-Polynomial).vi"/>
				<Item Name="DAQmx Create Channel (AI-Force-Bridge-Table).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Force-Bridge-Table).vi"/>
				<Item Name="DAQmx Create Channel (AI-Force-Bridge-Two-Point-Linear).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Force-Bridge-Two-Point-Linear).vi"/>
				<Item Name="DAQmx Create Channel (AI-Pressure-Bridge-Two-Point-Linear).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Pressure-Bridge-Two-Point-Linear).vi"/>
				<Item Name="DAQmx Create Channel (AI-Pressure-Bridge-Table).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Pressure-Bridge-Table).vi"/>
				<Item Name="DAQmx Create Channel (AI-Pressure-Bridge-Polynomial).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Pressure-Bridge-Polynomial).vi"/>
				<Item Name="DAQmx Create Channel (AI-Torque-Bridge-Two-Point-Linear).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Torque-Bridge-Two-Point-Linear).vi"/>
				<Item Name="DAQmx Create Channel (AI-Torque-Bridge-Table).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Torque-Bridge-Table).vi"/>
				<Item Name="DAQmx Create Channel (AI-Torque-Bridge-Polynomial).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Torque-Bridge-Polynomial).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Force-Bridge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Force-Bridge).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Pressure-Bridge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Pressure-Bridge).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Torque-Bridge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Torque-Bridge).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Force-IEPE Sensor).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Force-IEPE Sensor).vi"/>
				<Item Name="DAQmx Create Channel (AI-Force-IEPE Sensor).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Force-IEPE Sensor).vi"/>
				<Item Name="DAQmx Create Channel (TEDS-AI-Bridge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (TEDS-AI-Bridge).vi"/>
				<Item Name="DAQmx Create Channel (AI-Velocity-IEPE Sensor).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Velocity-IEPE Sensor).vi"/>
				<Item Name="DAQmx Create Channel (AI-Strain-Rosette Strain Gage).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Strain-Rosette Strain Gage).vi"/>
				<Item Name="DAQmx Create Channel (CI-Duty Cycle).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Duty Cycle).vi"/>
				<Item Name="DAQmx Create Channel (CI-Velocity-Angular).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Velocity-Angular).vi"/>
				<Item Name="DAQmx Create Channel (CI-Velocity-Linear).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (CI-Velocity-Linear).vi"/>
				<Item Name="DAQmx Create Channel (AI-Acceleration-4 Wire DC Voltage).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Acceleration-4 Wire DC Voltage).vi"/>
				<Item Name="DAQmx Create Channel (AI-Acceleration-Charge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Acceleration-Charge).vi"/>
				<Item Name="DAQmx Create Channel (AI-Charge).vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/create/channels.llb/DAQmx Create Channel (AI-Charge).vi"/>
				<Item Name="DAQmx Start Task.vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/configure/task.llb/DAQmx Start Task.vi"/>
				<Item Name="DAQmx Clear Task.vi" Type="VI" URL="/&lt;vilib&gt;/DAQmx/configure/task.llb/DAQmx Clear Task.vi"/>
				<Item Name="XNET Fill In Error Info.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Fill In Error Info.vi"/>
				<Item Name="XNET Create Session.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session.vi"/>
				<Item Name="XNET Create Session (Signal Input Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Signal Input Single-point).vi"/>
				<Item Name="XNET Mode.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Mode.ctl"/>
				<Item Name="_XNET Create Session.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/_XNET Create Session.vi"/>
				<Item Name="_XNET Convert List From Array To Comma.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/_XNET Convert List From Array To Comma.vi"/>
				<Item Name="_XNET Split Database Cluster.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/_XNET Split Database Cluster.vi"/>
				<Item Name="XNET Create Session (Signal Input Waveform).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Signal Input Waveform).vi"/>
				<Item Name="XNET Create Session (Signal Input XY).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Signal Input XY).vi"/>
				<Item Name="XNET Create Session (Signal Output Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Signal Output Single-point).vi"/>
				<Item Name="XNET Create Session (Signal Output Waveform).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Signal Output Waveform).vi"/>
				<Item Name="XNET Create Session (Signal Output XY).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Signal Output XY).vi"/>
				<Item Name="XNET Create Session (Frame Input Stream).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Frame Input Stream).vi"/>
				<Item Name="XNET Create Session (Frame Input Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Frame Input Single-point).vi"/>
				<Item Name="XNET Create Session (Frame Output Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Frame Output Single-point).vi"/>
				<Item Name="XNET Create Session (Frame Input Queued).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Frame Input Queued).vi"/>
				<Item Name="XNET Create Session (Frame Output Stream).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Frame Output Stream).vi"/>
				<Item Name="XNET Create Session (Frame Output Queued).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Frame Output Queued).vi"/>
				<Item Name="XNET Create Session (Generic).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Generic).vi"/>
				<Item Name="XNET Create Session (Conversion).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (Conversion).vi"/>
				<Item Name="XNET Create Session (PDU Input Queued).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (PDU Input Queued).vi"/>
				<Item Name="XNET Create Session (PDU Input Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (PDU Input Single-point).vi"/>
				<Item Name="XNET Create Session (PDU Output Queued).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (PDU Output Queued).vi"/>
				<Item Name="XNET Create Session (PDU Output Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Create Session (PDU Output Single-point).vi"/>
				<Item Name="XNET Start.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Start.vi"/>
				<Item Name="XNET Control Scope.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Control Scope.ctl"/>
				<Item Name="XNET Read.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read.vi"/>
				<Item Name="XNET Read (Signal Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Signal Single-point).vi"/>
				<Item Name="XNET Read (Signal Waveform) .vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Signal Waveform) .vi"/>
				<Item Name="XNET Read (Signal XY).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Signal XY).vi"/>
				<Item Name="XNET Read (Frame CAN).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Frame CAN).vi"/>
				<Item Name="XNET Frame CAN.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame CAN.ctl"/>
				<Item Name="XNET Frame Type CAN.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame Type CAN.ctl"/>
				<Item Name="XNET Read (Frame FlexRay).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Frame FlexRay).vi"/>
				<Item Name="XNET Frame FlexRay.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame FlexRay.ctl"/>
				<Item Name="XNET Frame Type FlexRay.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame Type FlexRay.ctl"/>
				<Item Name="XNET Read (Frame Raw).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Frame Raw).vi"/>
				<Item Name="XNET Read (State CAN Comm).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State CAN Comm).vi"/>
				<Item Name="XNET CAN Comm.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET CAN Comm.ctl"/>
				<Item Name="XNET CAN Comm State.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET CAN Comm State.ctl"/>
				<Item Name="XNET CAN Last Err.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET CAN Last Err.ctl"/>
				<Item Name="XNET Read (State FlexRay Comm).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State FlexRay Comm).vi"/>
				<Item Name="XNET FlexRay Comm.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET FlexRay Comm.ctl"/>
				<Item Name="XNET FlexRay POC State.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET FlexRay POC State.ctl"/>
				<Item Name="XNET Read (State Time Comm).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State Time Comm).vi"/>
				<Item Name="XNET Read (State Time Current).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State Time Current).vi"/>
				<Item Name="XNET Read (State Time Start).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State Time Start).vi"/>
				<Item Name="XNET Read (State FlexRay Stats).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State FlexRay Stats).vi"/>
				<Item Name="XNET FlexRay Stats.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET FlexRay Stats.ctl"/>
				<Item Name="XNET Read (State FlexRay Cycle Macrotick).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State FlexRay Cycle Macrotick).vi"/>
				<Item Name="XNET Read (Frame LIN).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Frame LIN).vi"/>
				<Item Name="XNET Frame LIN.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame LIN.ctl"/>
				<Item Name="XNET Frame Type LIN.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame Type LIN.ctl"/>
				<Item Name="XNET Read (State LIN Comm).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State LIN Comm).vi"/>
				<Item Name="XNET LIN Comm.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET LIN Comm.ctl"/>
				<Item Name="XNET LIN Comm State.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET LIN Comm State.ctl"/>
				<Item Name="XNET LIN Last Err.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET LIN Last Err.ctl"/>
				<Item Name="XNET Read (State Session Info).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State Session Info).vi"/>
				<Item Name="XNET Session Info State.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Session Info State.ctl"/>
				<Item Name="XNET Read (State J1939 Comm).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State J1939 Comm).vi"/>
				<Item Name="XNET J1939 Comm.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET J1939 Comm.ctl"/>
				<Item Name="XNET Read (Frame Ethernet).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (Frame Ethernet).vi"/>
				<Item Name="XNET Frame Ethernet.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame Ethernet.ctl"/>
				<Item Name="XNET Frame Type Ethernet.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Frame Type Ethernet.ctl"/>
				<Item Name="XNET Read (State Time Trigger).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Read (State Time Trigger).vi"/>
				<Item Name="XNET Write.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write.vi"/>
				<Item Name="XNET Write (Signal Single-point).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Signal Single-point).vi"/>
				<Item Name="XNET Write (Signal Waveform) .vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Signal Waveform) .vi"/>
				<Item Name="XNET Write (Signal XY).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Signal XY).vi"/>
				<Item Name="XNET Write (Frame CAN).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Frame CAN).vi"/>
				<Item Name="XNET Write (Frame FlexRay).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Frame FlexRay).vi"/>
				<Item Name="XNET Write (Frame Raw).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Frame Raw).vi"/>
				<Item Name="XNET Write (Frame LIN).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Frame LIN).vi"/>
				<Item Name="XNET Write (State LIN Schedule Change).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (State LIN Schedule Change).vi"/>
				<Item Name="XNET Write (State FlexRay Symbol).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (State FlexRay Symbol).vi"/>
				<Item Name="XNET Write (State LIN Diagnostic Schedule Change).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (State LIN Diagnostic Schedule Change).vi"/>
				<Item Name="XNET LIN Diag Schedule Type.ctl" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET LIN Diag Schedule Type.ctl"/>
				<Item Name="XNET Write (Frame Ethernet).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (Frame Ethernet).vi"/>
				<Item Name="XNET Write (State Ethernet Sleep).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (State Ethernet Sleep).vi"/>
				<Item Name="XNET Write (State Ethernet Wake).vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Write (State Ethernet Wake).vi"/>
				<Item Name="XNET Clear.vi" Type="VI" URL="/&lt;vilib&gt;/xnet/xnet.llb/XNET Clear.vi"/>
				<Item Name="JSONtext.lvlib" Type="Library" URL="/&lt;vilib&gt;/JDP Science/JSONtext/JSONtext.lvlib"/>
				<Item Name="JDP Utility.lvlib" Type="Library" URL="/&lt;vilib&gt;/JDP Science/JDP Science Common Utilities/JDP Utility.lvlib"/>
				<Item Name="Subscription ID.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Subscription ID.ctl"/>
				<Item Name="OPC UA Client Refnum.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/OPC UA Client Refnum.ctl"/>
				<Item Name="Subscription Data Change.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Subscription Data Change.ctl"/>
				<Item Name="NodeId.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/NodeId.ctl"/>
				<Item Name="Data Type.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Data Type.ctl"/>
				<Item Name="OPC UA StatusCode.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/OPC UA StatusCode.ctl"/>
				<Item Name="NI OPC UA Client.lvlib" Type="Library" URL="/&lt;vilib&gt;/OPCUA/NI OPC UA Client.lvlib"/>
				<Item Name="Create NI GUID.vi" Type="VI" URL="/&lt;vilib&gt;/string/Create NI GUID.vi"/>
				<Item Name="Write Status.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Write Status.ctl"/>
				<Item Name="Write Variant Request.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Write Variant Request.ctl"/>
				<Item Name="Time Out.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Time Out.ctl"/>
				<Item Name="ni_opcua.dll" Type="Document" URL="/&lt;vilib&gt;/OPCUA/ni_opcua.dll"/>
				<Item Name="Get Cert AbsPath.vi" Type="VI" URL="/&lt;vilib&gt;/OPCUA/server/subVI/Get Cert AbsPath.vi"/>
				<Item Name="NI OPC UA Utility.lvlib" Type="Library" URL="/&lt;vilib&gt;/OPCUA/NI OPC UA Utility.lvlib"/>
				<Item Name="Is Value Changed.vim" Type="VI" URL="/&lt;vilib&gt;/Utility/Is Value Changed.vim"/>
				<Item Name="NodeIds.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/NodeIds.ctl"/>
				<Item Name="Write Variant Result.ctl" Type="VI" URL="/&lt;vilib&gt;/OPCUA/controls/Write Variant Result.ctl"/>
			</Item>
			<Item Name="instr.lib" Type="Folder">
				<Item Name="Agilent N6700 Series.lvlib" Type="Library" URL="/&lt;instrlib&gt;/Agilent N6700 Series/Agilent N6700 Series.lvlib"/>
				<Item Name="APS-7000 Series.lvlib" Type="Library" URL="/&lt;instrlib&gt;/APS-7000 Series/APS-7000 Series.lvlib"/>
				<Item Name="APS GettingStarted.vi" Type="VI" URL="/&lt;instrlib&gt;/APS-7000 Series/Examples/APS GettingStarted.vi"/>
				<Item Name="niDMM Initialize.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Initialize.vi"/>
				<Item Name="niSwitch Initialize With Topology.vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Initialize With Topology.vi"/>
				<Item Name="niSwitch Topologies.ctl" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Topologies.ctl"/>
				<Item Name="niDMM Config Measurement.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Config Measurement.vi"/>
				<Item Name="niDMM Configure Measurement Digits.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Configure Measurement Digits.vi"/>
				<Item Name="niDMM Resolution in Digits.ctl" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Resolution in Digits.ctl"/>
				<Item Name="niDMM Function.ctl" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Function.ctl"/>
				<Item Name="niDMM Function To IVI Constant.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Function To IVI Constant.vi"/>
				<Item Name="niDMM IVI Error Converter.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM IVI Error Converter.vi"/>
				<Item Name="niDMM Configure Measurement Absolute.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Configure Measurement Absolute.vi"/>
				<Item Name="niDMM Configure Current Source.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Configure Current Source.vi"/>
				<Item Name="niDMM Current Source.ctl" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Current Source.ctl"/>
				<Item Name="niSwitch Connect Channels.vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Connect Channels.vi"/>
				<Item Name="niSwitch Connect Channels (Single).vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Connect Channels (Single).vi"/>
				<Item Name="niSwitch IVI Error Converter.vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch IVI Error Converter.vi"/>
				<Item Name="niSwitch Connect Channels (Multiple).vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Connect Channels (Multiple).vi"/>
				<Item Name="niSwitch Wait For Debounce.vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Wait For Debounce.vi"/>
				<Item Name="niSwitch Disconnect All Channels.vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Disconnect All Channels.vi"/>
				<Item Name="niDMM Close.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Close.vi"/>
				<Item Name="niSwitch Close.vi" Type="VI" URL="/&lt;instrlib&gt;/niSwitch/niSwitch.llb/niSwitch Close.vi"/>
				<Item Name="niDMM Read.vi" Type="VI" URL="/&lt;instrlib&gt;/niDMM/nidmm.llb/niDMM Read.vi"/>
			</Item>
			<Item Name="user32.dll" Type="Document" URL="user32.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="nilvaiu.dll" Type="Document" URL="nilvaiu.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="nidmm_64.dll" Type="Document" URL="nidmm_64.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="niswitch_64.dll" Type="Document" URL="niswitch_64.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="nixlvapi.dll" Type="Document" URL="nixlvapi.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="user32.dll" Type="Document" URL="../../../../Windows/System32/user32.dll"/>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
