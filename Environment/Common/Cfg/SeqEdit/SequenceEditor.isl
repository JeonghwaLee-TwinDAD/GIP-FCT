<?xml version="1.0" encoding="utf-8"?>
<styleLibrary>
  <annotation>
    <lastModified>2020-08-28T02:51:11</lastModified>
  </annotation>
  <styleSets defaultStyleSet="Default">
    <styleSet name="Default">
      <componentStyles>
        <componentStyle name="UltraButton" useOsThemes="True" />
        <componentStyle name="UltraCombo" buttonStyle="Borderless" useFlatMode="True" />
        <componentStyle name="UltraDockManager" useOsThemes="False">
          <properties>
            <property name="GroupPaneTabStyle" colorCategory="{Default}">PropertyPageFlat</property>
            <property name="UnpinnedTabStyle" colorCategory="{Default}">Excel</property>
            <property name="WindowStyle" colorCategory="{Default}">Windows</property>
          </properties>
        </componentStyle>
        <componentStyle name="UltraExplorerBar" useOsThemes="False" useFlatMode="True" />
        <componentStyle name="UltraPanel" useOsThemes="False" useFlatMode="True" />
        <componentStyle name="UltraSplitter" useFlatMode="True">
          <properties>
            <property name="UseHotTracking" colorCategory="{Default}">True</property>
          </properties>
        </componentStyle>
        <componentStyle name="UltraTabbedMdiManager" buttonStyle="FlatBorderless" useOsThemes="False" useFlatMode="True">
          <properties>
            <property name="HotTrack" colorCategory="{Default}">True</property>
            <property name="TabStyle" colorCategory="{Default}">Flat</property>
            <property name="ViewStyle" colorCategory="{Default}">VisualStudio2005</property>
          </properties>
        </componentStyle>
        <componentStyle name="UltraTabControl" useOsThemes="False" useFlatMode="True">
          <properties>
            <property name="Style" colorCategory="{Default}">VisualStudio</property>
            <property name="ViewStyle" colorCategory="{Default}">Standard</property>
          </properties>
        </componentStyle>
        <componentStyle name="UltraTabStripControl" useOsThemes="False" useFlatMode="True">
          <properties>
            <property name="Style" colorCategory="{Default}">Office2007Ribbon</property>
          </properties>
        </componentStyle>
        <componentStyle name="UltraTilePanel" useFlatMode="True">
          <properties>
            <property name="ShowShadows" colorCategory="{Default}">False</property>
          </properties>
        </componentStyle>
        <componentStyle name="UltraToolbarsManager" resolutionOrder="ControlThenApplication" useFlatMode="True">
          <properties>
            <property name="Style" colorCategory="{Default}">Office2013</property>
          </properties>
        </componentStyle>
      </componentStyles>
      <styles>
        <style role="Base">
          <states>
            <state name="Normal" backColor="White" backGradientStyle="None" themedElementAlpha="Transparent" backHatchStyle="None" />
            <state name="Selected" backColor="229, 246, 245" foreColor="Black" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="Button" buttonStyle="Flat" />
        <style role="DockAreaSplitterBarHorizontal">
          <states>
            <state name="Normal" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="0, 3, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAhgAAAAKJUE5HDQoaCgAAAA1JSERSAAAAeQAAAAUIBgAAAGEaQ/MAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAKElEQVRIS2P4PwqGPRiN5BEAGC5fvfZ/FA9vPJqTRwAYjeRhD/7/BwBLai8NUOQwaQAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="DockAreaSplitterBarVertical">
          <states>
            <state name="Normal" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="3, 0, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAkgAAAAKJUE5HDQoaCgAAAA1JSERSAAAABQAAAKAIBgAAAMS+prgAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAANElEQVRIS+3IwQkAIBDEQPtvT7SdNbDXhJB5hayMfe5U4oSznHCWE85ywllOOMsJZ308kweg7yTVrg5PpgAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="DockManagerPane">
          <states>
            <state name="Normal" backColor="White" borderColor="Transparent" backGradientStyle="None" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="DockPaneCaption">
          <states>
            <state name="Normal" borderColor="Transparent" fontName="Segoe UI" imageBackgroundStyle="Stretched" fontSize="14" borderColor3DBase="Transparent" imageBackgroundStretchMargins="0, 3, 0, 0" borderColor2="Transparent" />
            <state name="Active" backColor="White" borderColor="Transparent" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="DockPaneCaptionButton">
          <states>
            <state name="Normal" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
            <state name="HotTracked" borderColor="3, 181, 133" borderColor2="3, 181, 133" />
          </states>
        </style>
        <style role="DockPanePinButton">
          <states>
            <state name="Normal">
              <image>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAswAAAAKJUE5HDQoaCgAAAA1JSERSAAAAEAAAABAIBgAAAB/z/2EAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7DAAAOwwHHb6hkAAAAVUlEQVQ4T+2MQQoAIAgEe4X9/6VbSUXZBkmHLi0M6LAaAFxBpQcqPVApErHDdqelyxx2XPzStULlf0CPG7Y7LV0OKUd11CxdK1QOefPAA5UeqDwHIQEIS2DFSOaaVAAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</image>
            </state>
          </states>
        </style>
        <style role="DropDownButton" buttonStyle="Flat">
          <states>
            <state name="Normal" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
            <state name="HotTracked" backColor="229, 246, 245" borderColor="171, 171, 171" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="ExplorerBarControlArea">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor3DBase="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="ExplorerBarGroupHeader">
          <states>
            <state name="Normal" fontBold="False" fontSize="10" />
          </states>
        </style>
        <style role="GroupPaneSplitterBarHorizontal">
          <states>
            <state name="Normal" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="0, 3, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAhgAAAAKJUE5HDQoaCgAAAA1JSERSAAAAeQAAAAUIBgAAAGEaQ/MAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAKElEQVRIS2P4PwqGPRiN5BEAGC5fvfZ/FA9vPJqTRwAYjeRhD/7/BwBLai8NUOQwaQAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="GroupPaneSplitterBarVertical">
          <states>
            <state name="Normal" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="3, 0, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAkgAAAAKJUE5HDQoaCgAAAA1JSERSAAAABQAAAKAIBgAAAMS+prgAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAANElEQVRIS+3IwQkAIBDEQPtvT7SdNbDXhJB5hayMfe5U4oSznHCWE85ywllOOMsJZ308kweg7yTVrg5PpgAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="Header">
          <states>
            <state name="Normal" borderColor="Transparent" imageBackgroundStyle="Stretched" borderColor3DBase="Transparent" imageBackgroundStretchMargins="0, 2, 0, 2" borderColor2="Transparent">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAzAAAAAKJUE5HDQoaCgAAAA1JSERSAAAAxgAAAB4IBgAAAHf2YR8AAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAbklEQVR4Xu3cQQ0AMAwDsfFnV0Tde9pBsKVAuG/OzKyZvTsLfIQBQRgQhAFBGBCEAUEYEIQBQRgQhAFBGBCEAUEYEIQBQRgQhAFBGBCEAUEYEIQBQRgQhAFBGBCEAUEYEIQBQRgQhAHBqbPZt9kLzzSjDo5oC5kAAAAASUVORK5CYIILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="ListViewControlArea">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor3DBase="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="MdiSplitterBarHorizontal">
          <states>
            <state name="Normal" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="0, 3, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAgQAAAAKJUE5HDQoaCgAAAA1JSERSAAAAeQAAAAQIBgAAAKpGkFYAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAI0lEQVRIS2P4PwqGPWBYt2Hz/1E8vPFoTh4BYDSShz34/x8AAH8ZTjH4j9QAAAAASUVORK5CYIILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="MdiSplitterBarVertical">
          <states>
            <state name="Normal" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="3, 0, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAjAAAAAKJUE5HDQoaCgAAAA1JSERSAAAABAAAAKAIBgAAACt8zYYAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAALklEQVRIS+3IwQkAIBAEMfsvRgQLPD1hS/CX+Qxk1G2u3XsBABIAIAEAEvgOVQfKpGMIOu87iwAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="MenuItem">
          <states>
            <state name="HotTracked" backColor="229, 246, 245" borderColor="Transparent" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="NotificationBadge">
          <states>
            <state name="Normal" backColor="Transparent" borderColor="Transparent" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="ScrollBarArrow">
          <states>
            <state name="Normal" backColor="240, 240, 240" backGradientStyle="None" backHatchStyle="None" />
            <state name="HotTracked" backColor="218, 218, 218" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="ScrollBarThumb">
          <states>
            <state name="Normal" backColor="205, 205, 205" backGradientStyle="None" backHatchStyle="None" />
            <state name="HotTracked" backColor="166, 166, 166" backGradientStyle="None" backHatchStyle="None" />
            <state name="Pressed" backColor="96, 96, 96" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="ScrollBarTrackSection">
          <states>
            <state name="Normal" backColor="240, 240, 240" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="TabButton">
          <states>
            <state name="Normal" backColor="Transparent" borderColor="Transparent" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" borderColor2="Transparent" />
            <state name="HotTracked" backColor="Transparent" borderColor="Transparent" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="TabClientAreaHorizontal">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor3DBase="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="TabCloseButton">
          <states>
            <state name="HotTracked" borderColor="3, 181, 133" borderColor3DBase="3, 181, 133" borderColor2="3, 181, 133" />
          </states>
        </style>
        <style role="TabItem">
          <states>
            <state name="Normal" backColor="Transparent" borderColor="Transparent" fontBold="False" backGradientStyle="None" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="TabItemAreaHorizontalBottom">
          <states>
            <state name="Normal" borderColor="Transparent" imageBackgroundStyle="Stretched" borderColor3DBase="Transparent" imageBackgroundStretchMargins="0, 3, 0, 0" borderColor2="Transparent">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAzAAAAAKJUE5HDQoaCgAAAA1JSERSAAAAyAAAAB4IBgAAAGk/UawAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAbklEQVR4Xu3VAQ0AIRADQfy74xF0GHhWwUxSCZuuAZ7W+faY2f88CASBQBAIBIFAEAgEgUAQCASBQBAIBIFAEAgEgUAQCASBQBAIBIFAEAgEgUAQCASBQBAIBIFAEAgEgUAQCASBQBAIBIHA08wFDX7+LVNpWlEAAAAASUVORK5CYIILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="TabItemAreaHorizontalTop">
          <states>
            <state name="Normal" borderColor="Transparent" imageBackgroundStyle="Stretched" borderColor3DBase="Transparent" imageBackgroundStretchMargins="0, 0, 0, 3" borderColor2="Transparent">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAzAAAAAKJUE5HDQoaCgAAAA1JSERSAAAAyAAAAB4IBgAAAGk/UawAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAbklEQVR4Xu3VAQ0AIRADQfy74xF0GHhWwUxSCZuuAZ4EAkEgEAQCQSAQBAJBIBAEAkEgEAQCQSAQBAJBIBAEAkEgEAQCQSAQBAJBIBAEAkEgEAQCQSAQBAJBIBAEAkEgEAQCYZ1vj5n9z4PA08wFJFb+LSBIiMUAAAAASUVORK5CYIILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="TabItemHorizontalBottom">
          <states>
            <state name="Selected" backColor="White" imageBackgroundStyle="Stretched" backGradientStyle="None" backHatchStyle="None" imageBackgroundStretchMargins="0, 3, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAvAAAAAKJUE5HDQoaCgAAAA1JSERSAAAAyAAAABcIBgAAAE4wAGQAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAXklEQVR4Xu3ToRHAQAwDwS8/JaU7PwiNr4LdGRHxO+d9xsyW/Z5m9m2AlUAgCASCQCAIBIJAIAgEgkAgCASCQCAIBIJAIAgEgkAgCASCQCAIBIJAIAgEgkAgCARWMxfz+uTNQ/EU9gAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
            <state name="Active" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="0, 3, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAvgAAAAKJUE5HDQoaCgAAAA1JSERSAAAAyAAAABcIBgAAAE4wAGQAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAYElEQVR4Xu3ToQGAMADAsJ3PSTzECcMOgd16QURMfce4rwUcbCPwe965gD2DQDAIBINAMAgEg0AwCASDQDAIBINAMAgEg0AwCASDQDAIBINAMAgEg0AwCASDQDAIHM31Acw5AmcgWt7RAAAAAElFTkSuQmCCCwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="TabItemHorizontalTop">
          <states>
            <state name="Selected" backColor="White" imageBackgroundStyle="Stretched" backGradientStyle="None" backHatchStyle="None" imageBackgroundStretchMargins="0, 0, 0, 3">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAvAAAAAKJUE5HDQoaCgAAAA1JSERSAAAAyAAAABcIBgAAAE4wAGQAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAXklEQVR4Xu3TQQ3AQBDDwIV/kMpuWwA9I5iR8snfs8CVQCAIBIJAIAgEgkAgCASCQCAIBIJAIAgEgkAgCASCQCAIBIJAIAgEgkAgCASCQCAIBMLMc9bMLvs9zezb2RdVKOTNsoqhjQAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
            <state name="Active" imageBackgroundStyle="Stretched" imageBackgroundStretchMargins="0, 0, 0, 3">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAvgAAAAKJUE5HDQoaCgAAAA1JSERSAAAAyAAAABcIBgAAAE4wAGQAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAYElEQVR4Xu3TMQGAMADAsMmfJAwhAd4NAaMKcuTp33G/zwLODALBIBAMAsEgEAwCwSAQDALBIBAMAsEgEAwCwSAQDALBIBAMAsEgEAwCwSAQDALBIBDGuOYCfhwj8JlrAxgUAmer0vwuAAAAAElFTkSuQmCCCwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="TabListButton">
          <states>
            <state name="HotTracked" borderColor="3, 181, 133" borderColor3DBase="3, 181, 133" borderColor2="3, 181, 133" />
          </states>
        </style>
        <style role="TileHeader">
          <states>
            <state name="Normal" backColor="Transparent" borderColor="Transparent" backGradientStyle="None" backHatchStyle="None" borderColor2="Transparent" />
          </states>
        </style>
        <style role="ToolbarBase">
          <states>
            <state name="Normal" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="ToolbarDockArea">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor3DBase="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="ToolbarDockAreaTop">
          <states>
            <state name="Normal" backColor="Transparent" borderColor="Transparent" imageBackgroundStyle="Stretched" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" imageBackgroundStretchMargins="0, 0, 0, 3" borderColor2="Transparent">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAA2wAAAAKJUE5HDQoaCgAAAA1JSERSAAABTgAAAB0IBgAAAMU+hYQAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAfUlEQVR4Xu3UQREAIAzAMPwrHSqGBfpP7mqhZwFIjBMgMk6AyDgBIuMEiIwTIDJOgMg4ASLjBIiMEyAyToDIOAEi4wSIjBMgMk6AyDgBIuMEiIwTIDJOgMg4ASLjBIiMEyAyToDIOAEi4wSIjBMgMk6A6NyZlST9Z5ySlJp9/qsYA94XyhIAAAAASUVORK5CYIILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
          </states>
        </style>
        <style role="ToolbarHorizontal">
          <states>
            <state name="Normal" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="ToolbarItem">
          <states>
            <state name="Normal" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
            <state name="HotTracked" backColor="229, 246, 245" borderColor="Transparent" backGradientStyle="None" borderColor3DBase="Transparent" backHatchStyle="None" borderColor2="Transparent" />
            <state name="Pressed" backColor="229, 246, 245" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="TreeControlArea" borderStyle="None">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor3DBase="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="UltraComboEditor" borderStyle="Solid" buttonStyle="FlatBorderless" />
        <style role="UltraPanel">
          <states>
            <state name="Normal" backColor="White" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="UltraSplitter">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="UltraSplitterHorizontal">
          <states>
            <state name="Normal" backColor="Transparent" imageBackgroundStyle="Stretched" backGradientStyle="None" backHatchStyle="None" imageBackgroundStretchMargins="0, 3, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAgQAAAAKJUE5HDQoaCgAAAA1JSERSAAAAeQAAAAQIBgAAAKpGkFYAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAAI0lEQVRIS2P4PwqGPWBYt2Hz/1E8vPFoTh4BYDSShz34/x8AAH8ZTjH4j9QAAAAASUVORK5CYIILAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
            <state name="HotTracked" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="UltraSplitterVertical">
          <states>
            <state name="Normal" backColor="Transparent" imageBackgroundStyle="Stretched" backGradientStyle="None" backHatchStyle="None" imageBackgroundStretchMargins="3, 0, 0, 0">
              <imageBackground>AAEAAAD/////AQAAAAAAAAAMAgAAAFFTeXN0ZW0uRHJhd2luZywgVmVyc2lvbj00LjAuMC4wLCBDdWx0dXJlPW5ldXRyYWwsIFB1YmxpY0tleVRva2VuPWIwM2Y1ZjdmMTFkNTBhM2EFAQAAABVTeXN0ZW0uRHJhd2luZy5CaXRtYXABAAAABERhdGEHAgIAAAAJAwAAAA8DAAAAjAAAAAKJUE5HDQoaCgAAAA1JSERSAAAABAAAAKAIBgAAACt8zYYAAAAEZ0FNQQAAsY8L/GEFAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAALklEQVRIS+3IwQkAIBAEMfsvRgQLPD1hS/CX+Qxk1G2u3XsBABIAIAEAEvgOVQfKpGMIOu87iwAAAABJRU5ErkJgggsAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA=</imageBackground>
            </state>
            <state name="HotTracked" backColor="Transparent" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="UltraTabControlBase">
          <states>
            <state name="Normal" borderColor="Transparent" borderColor3DBase="Transparent" borderColor2="Transparent" />
          </states>
        </style>
        <style role="UltraTile">
          <states>
            <state name="Normal" backColor="White" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
        <style role="UltraTilePanel">
          <states>
            <state name="Normal" backColor="White" backGradientStyle="None" backHatchStyle="None" />
          </states>
        </style>
      </styles>
      <targetComponents>
        <targetComponent name="UltraGroupBox" />
        <targetComponent name="UltraScrollBar" />
        <targetComponent name="UltraSplitter" />
        <targetComponent name="UltraTabbedMdiManager" />
        <targetComponent name="UltraTabControl" />
        <targetComponent name="UltraTabStripControl" />
        <targetComponent name="UltraToolbarsManager" />
        <targetComponent name="UltraTree" />
        <targetComponent name="UltraDockManager" />
        <targetComponent name="UltraExplorerBar" />
        <targetComponent name="UltraTilePanel" />
        <targetComponent name="UltraTile" />
        <targetComponent name="UltraStatusBar" />
        <targetComponent name="UltraPanel" />
        <targetComponent name="UltraListView" />
        <targetComponent name="UltraFormManager" />
        <targetComponent name="UltraButton" />
        <targetComponent name="UltraCombo" />
        <targetComponent name="UltraComboEditor" />
      </targetComponents>
    </styleSet>
  </styleSets>
</styleLibrary>