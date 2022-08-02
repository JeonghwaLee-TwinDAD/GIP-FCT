<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="21008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="FCTAPITests" Type="Folder">
			<Item Name="CANBusTestRx.vi" Type="VI" URL="../FCTAPITests/CANBusTestRx.vi"/>
			<Item Name="CANBusTestTx.vi" Type="VI" URL="../FCTAPITests/CANBusTestTx.vi"/>
			<Item Name="EEPROMTest.vi" Type="VI" URL="../FCTAPITests/EEPROMTest.vi"/>
			<Item Name="eMMcTest.vi" Type="VI" URL="../FCTAPITests/eMMcTest.vi"/>
			<Item Name="EthernetTest.vi" Type="VI" URL="../FCTAPITests/EthernetTest.vi"/>
			<Item Name="MACIDTest.vi" Type="VI" URL="../FCTAPITests/MACIDTest.vi"/>
			<Item Name="ReadIdentifierTest.vi" Type="VI" URL="../FCTAPITests/ReadIdentifierTest.vi"/>
			<Item Name="RS485TestRx.vi" Type="VI" URL="../FCTAPITests/RS485TestRx.vi"/>
			<Item Name="RS485TestTx.vi" Type="VI" URL="../FCTAPITests/RS485TestTx.vi"/>
			<Item Name="UARTTestRx.vi" Type="VI" URL="../FCTAPITests/UARTTestRx.vi"/>
			<Item Name="UARTTestTx.vi" Type="VI" URL="../FCTAPITests/UARTTestTx.vi"/>
			<Item Name="WriteIdentifierTest.vi" Type="VI" URL="../FCTAPITests/WriteIdentifierTest.vi"/>
		</Item>
		<Item Name="subVI" Type="Folder">
			<Item Name="CommonResult.vi" Type="VI" URL="../subVI/CommonResult.vi"/>
			<Item Name="ErrorCodes.vi" Type="VI" URL="../subVI/ErrorCodes.vi"/>
			<Item Name="GetDescription.vi" Type="VI" URL="../subVI/GetDescription.vi"/>
			<Item Name="GetErrorDescription.vi" Type="VI" URL="../subVI/GetErrorDescription.vi"/>
			<Item Name="GetStatus.vi" Type="VI" URL="../subVI/GetStatus.vi"/>
			<Item Name="GetStatusOnly.vi" Type="VI" URL="../subVI/GetStatusOnly.vi"/>
			<Item Name="IdentifierResult.vi" Type="VI" URL="../subVI/IdentifierResult.vi"/>
			<Item Name="KeyValueMap.vi" Type="VI" URL="../subVI/KeyValueMap.vi"/>
			<Item Name="LoopBackResultRx.vi" Type="VI" URL="../subVI/LoopBackResultRx.vi"/>
			<Item Name="LoopBackResultTx.vi" Type="VI" URL="../subVI/LoopBackResultTx.vi"/>
			<Item Name="ParseIdentifierResponse.vi" Type="VI" URL="../subVI/ParseIdentifierResponse.vi"/>
			<Item Name="ParsePN_SN.vi" Type="VI" URL="../subVI/ParsePN_SN.vi"/>
			<Item Name="ParseResponseSDIO.vi" Type="VI" URL="../subVI/ParseResponseSDIO.vi"/>
			<Item Name="POST.vi" Type="VI" URL="../subVI/POST.vi"/>
			<Item Name="ReadTSVariables.vi" Type="VI" URL="../subVI/ReadTSVariables.vi"/>
			<Item Name="Response.vi" Type="VI" URL="../subVI/Response.vi"/>
			<Item Name="SimpleError.vi" Type="VI" URL="../subVI/SimpleError.vi"/>
			<Item Name="StatusOnlyResult.vi" Type="VI" URL="../subVI/StatusOnlyResult.vi"/>
		</Item>
		<Item Name="JSONRPC_Test.vi" Type="VI" URL="../JSONRPC_Test.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Array of VData to VCluster__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Array of VData to VCluster__ogtk.vi"/>
				<Item Name="Array Size(s)__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Array Size(s)__ogtk.vi"/>
				<Item Name="Array to Array of VData__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Array to Array of VData__ogtk.vi"/>
				<Item Name="Array to VCluster__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Array to VCluster__ogtk.vi"/>
				<Item Name="Base64 Support.lvlib" Type="Library" URL="/&lt;vilib&gt;/JDP Science/JSONtext/Base64/Base64 Support.lvlib"/>
				<Item Name="Build Error Cluster__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/error/error.llb/Build Error Cluster__ogtk.vi"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="General Error Handler Core CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler Core CORE.vi"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="Get Data Name from TD__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Get Data Name from TD__ogtk.vi"/>
				<Item Name="Get Data Name__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Get Data Name__ogtk.vi"/>
				<Item Name="Get Header from TD__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Get Header from TD__ogtk.vi"/>
				<Item Name="Get Last PString__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Get Last PString__ogtk.vi"/>
				<Item Name="Get LV Class Name.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/LVClass/Get LV Class Name.vi"/>
				<Item Name="Get PString__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Get PString__ogtk.vi"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Get Variant Attributes__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Get Variant Attributes__ogtk.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="imagedata.ctl" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/imagedata.ctl"/>
				<Item Name="JDP Timestamp.lvlib" Type="Library" URL="/&lt;vilib&gt;/JDP Science/JDP Science Common Utilities/Timestamp/JDP Timestamp.lvlib"/>
				<Item Name="JDP Utility.lvlib" Type="Library" URL="/&lt;vilib&gt;/JDP Science/JDP Science Common Utilities/JDP Utility.lvlib"/>
				<Item Name="JSON Value.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/JSON Value/JSON Value.lvclass"/>
				<Item Name="JSONRPC Client.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/NI/NI JSONRPC Server Framework/Classes/Client/JSONRPC Client.lvclass"/>
				<Item Name="JSONtext LVClass Serializer.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/JDP Science/JSONtext/LVClass Serializer/JSONtext LVClass Serializer.lvclass"/>
				<Item Name="JSONtext.lvlib" Type="Library" URL="/&lt;vilib&gt;/JDP Science/JSONtext/JSONtext.lvlib"/>
				<Item Name="LabVIEWHTTPClient.lvlib" Type="Library" URL="/&lt;vilib&gt;/httpClient/LabVIEWHTTPClient.lvlib"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="LVFixedPointRepBitsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/fxp/LVFixedPointRepBitsTypeDef.ctl"/>
				<Item Name="LVFixedPointRepRangeTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/fxp/LVFixedPointRepRangeTypeDef.ctl"/>
				<Item Name="LVNumericRepresentation.ctl" Type="VI" URL="/&lt;vilib&gt;/numeric/LVNumericRepresentation.ctl"/>
				<Item Name="lvpalettesupport.dll" Type="Document" URL="/&lt;vilib&gt;/Addons/TestStand/lvpalettesupport.dll"/>
				<Item Name="LVRectTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVRectTypeDef.ctl"/>
				<Item Name="NI_Data Type.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/Data Type/NI_Data Type.lvlib"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="Path To Command Line String.vi" Type="VI" URL="/&lt;vilib&gt;/AdvancedString/Path To Command Line String.vi"/>
				<Item Name="PathToUNIXPathString.vi" Type="VI" URL="/&lt;vilib&gt;/Platform/CFURL.llb/PathToUNIXPathString.vi"/>
				<Item Name="Picture to Pixmap.vi" Type="VI" URL="/&lt;vilib&gt;/picture/pictutil.llb/Picture to Pixmap.vi"/>
				<Item Name="Reshape Array to 1D VArray__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Reshape Array to 1D VArray__ogtk.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Set Data Name__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Set Data Name__ogtk.vi"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="TestStand - Get Property Value (Boolean Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Boolean Array).vi"/>
				<Item Name="TestStand - Get Property Value (Boolean).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Boolean).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Signed 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Signed 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number {Unsigned 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number {Unsigned 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Number).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Number).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Signed 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Signed 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 8-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 8-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 16-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 16-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 32-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 32-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array {Unsigned 64-bit Integer}).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array {Unsigned 64-bit Integer}).vi"/>
				<Item Name="TestStand - Get Property Value (Numeric Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Numeric Array).vi"/>
				<Item Name="TestStand - Get Property Value (Object).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Object).vi"/>
				<Item Name="TestStand - Get Property Value (Reference).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (Reference).vi"/>
				<Item Name="TestStand - Get Property Value (String Array).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (String Array).vi"/>
				<Item Name="TestStand - Get Property Value (String).vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value (String).vi"/>
				<Item Name="TestStand - Get Property Value.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Get Property Value.vi"/>
				<Item Name="TestStand - Validate Evaluation Types.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand - Validate Evaluation Types.vi"/>
				<Item Name="TestStand API Numeric Constants.ctl" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand API Numeric Constants.ctl"/>
				<Item Name="TestStand API Numeric Constants.vi" Type="VI" URL="/&lt;vilib&gt;/addons/TestStand/_TSUtility.llb/TestStand API Numeric Constants.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="Type Descriptor Enumeration__ogtk.ctl" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Type Descriptor Enumeration__ogtk.ctl"/>
				<Item Name="Type Descriptor Header__ogtk.ctl" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Type Descriptor Header__ogtk.ctl"/>
				<Item Name="Type Descriptor__ogtk.ctl" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Type Descriptor__ogtk.ctl"/>
				<Item Name="Variant to Header Info__ogtk.vi" Type="VI" URL="/&lt;vilib&gt;/NI/JSON Support for LabVIEW/includes/lvdata/lvdata.llb/Variant to Header Info__ogtk.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
