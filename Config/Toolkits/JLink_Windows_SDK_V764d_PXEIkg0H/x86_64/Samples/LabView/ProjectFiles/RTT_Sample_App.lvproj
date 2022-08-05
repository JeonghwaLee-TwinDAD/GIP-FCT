<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="11008008">
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
		<Item Name="RTT_Sample_App.vi" Type="VI" URL="../RTT_Sample_App.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="subTimeDelay.vi" Type="VI" URL="/&lt;vilib&gt;/express/express execution control/TimeDelayBlock.llb/subTimeDelay.vi"/>
			</Item>
			<Item Name="JLinkARM.lvlib" Type="Library" URL="../../Lib/JLinkARM.lvlib"/>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="RTT Sample Application" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{B50BE122-5F77-4244-8452-DAD856DFAB18}</Property>
				<Property Name="App_INI_GUID" Type="Str">{47F263A6-5A55-4BAA-A780-50F5DC4BF0AA}</Property>
				<Property Name="App_winsec.description" Type="Str">http://www.SEGGER.com</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{3D366ED5-8D37-4903-AA65-E2EDF412B86B}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">RTT Sample Application</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">..</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{A80C6D84-C42D-48D7-B607-3C0B61DD16ED}</Property>
				<Property Name="Destination[0].destName" Type="Str">RTT_Sample_App_LabView2011.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../RTT_Sample_App_LabView2011.exe</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../data</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{8AD51F34-99BD-4C48-8B5E-EB52C0209394}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/RTT_Sample_App.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
				<Property Name="TgtF_companyName" Type="Str">SEGGER</Property>
				<Property Name="TgtF_fileDescription" Type="Str">RTT Sample Application</Property>
				<Property Name="TgtF_fileVersion.major" Type="Int">1</Property>
				<Property Name="TgtF_internalName" Type="Str">RTT Sample Application</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright © 2014 SEGGER</Property>
				<Property Name="TgtF_productName" Type="Str">RTT Sample Application</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{6C9B65C1-E4B4-4F6E-93AD-CF71DCFF24AB}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">RTT_Sample_App_LabView2011.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
