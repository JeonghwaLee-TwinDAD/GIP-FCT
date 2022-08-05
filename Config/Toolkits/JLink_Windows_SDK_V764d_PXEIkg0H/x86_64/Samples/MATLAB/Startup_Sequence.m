% *********************************************************************
% *       Example based on MATLAB R2016b 64 bit
% *    
% *
% *       import and DLL include
% *
% **********************************************************************
disp(pwd)                                                                 % Displays current working directory (for Debugging)
if (libisloaded('JLink_x64'))
   unloadlibrary JLink_x64                                                % Unload used libraries
end
if (not(libisloaded('JLink_x64')))
loadlibrary('..\..\JLink_x64.dll','..\..\.\Inc\JLinkARMDLL.h')            % Loads librarys directly from SDK mainfolder
end
clc                                                                       % Clear Terminal from warnings
% *********************************************************************
% *
% *       MATLAB does not support Functionpointers
% *       therefore certain SDK Functions cannot be used     
% *
% **********************************************************************
% *********************************************************************
% *
% *        "main" InitDebug Session
% *
% **********************************************************************
% 
%  Select and configure host interface (USB is default)
% 
r = ConsoleGetString('Host interface. 0: USB, 1: IP.          Default: USB\n>');
if (isempty(r))                                                           % No host interface selected? --> Use USB as default
  r = '0';
end                                                         
% 
%  Get serial number if USB selected or IP address if Ethernet selected as host interface
% 
if (r == '0')                                                             % USB selected?
  r = ConsoleGetString('Enter J-Link serial number\n>');
  if (isempty(r))                                                         % No serial number selected? --> Use 0 as default
    r = '0';
  end
  calllib('JLink_x64', 'JLINKARM_EMU_SelectByUSBSN', str2num(r))
elseif (r == '1')                                                         % Ethernet selected?
  r = ConsoleGetString('J-Link IP or host name\n>');
  if (isempty(r))
    r = '0';
  else
    calllib('JLink_x64', 'JLINKARM_SelectIP', r, 0)                       % Select J-Link over inserted IP
  end
end
% 
%  Open connection to J-Link 
% 
r = calllib('JLink_x64', 'JLINKARM_Open');
if (r)                                                                    % Error occurred? --> Print error
  disp(r)
  input('J-Link stopped, press Enter to exit')
  calllib('JLink_x64', 'JLINKARM_Close')                                  % Close J-Link connection
  return                                                                  % Terminate Programm with error message
end
% 
%  Select settings file
%  Used by the control panel to store its settings and can be used by the user to
%  enable features like flash breakpoints in external CFI flash, if not selectable
%  by the debugger. There should be different settings files for different debug
%  configurations, for example different settings files for LEDSample_DebugFlash
%  and LEDSample_DebugRAM. If this file does not exist, the DLL will create one
%  with default settings. If the file is already present, it will be used to load
%  the control panel settings
%
FileName = 'ProjectFile = Settings.jlink';                                % Create settingsfile
calllib('JLink_x64', 'JLINKARM_ExecCommand', libpointer('cstring', FileName), [], 0)      
% 
% Select device or core
% 
r = ConsoleGetString('Enter the device name and confirm with ENTER     Default: unspecified:\n>');
if (isempty(r))
  r = 'unspecified';
end
DeviceString = ['device = ', r];                                          % Set device
calllib('JLink_x64', 'JLINKARM_ExecCommand', libpointer('cstring', DeviceString), [], 0)  
% 
% Select and configure target Interface
% 
r = ConsoleGetString('Target interface. 0: JTAG, 1: SWD.      Default: SWD\n>');
if (isempty(r))                                                           % No target interface selected? --> Use SWD as default
  r = '1';
end
calllib('JLink_x64', 'JLINKARM_TIF_Select', str2num(r))
%
% Select target interface speed which should be used by J-Link for target communication
%
r = ConsoleGetString('Interface speed [kHz].             Default: 4000 kHz\n>');
if (isempty(r))                                                           % No speed selected? --> Use 4000 kHz as default
  r = '4000';
end
calllib('JLink_x64', 'JLINKARM_SetSpeed', str2num(r))
%
% Connect to target CPU
%
r = calllib('JLink_x64', 'JLINKARM_Connect');
if (r < 0)                                                                % Check if error returned
  fprintf('Connect failed. Error: %d\n', r);
else
  disp('Connected to target successfully...\n')
end
% 
%  From now on the target is identified and we can start working with it.
%  Insert your code here...
%  Everything is done. Close connection to J-Link.
% 
input('To close J-Link press enter')
calllib('JLink_x64', 'JLINKARM_Close')
unloadlibrary JLink_x64                                                   % Unload used libraries
% *********************************************************************
% *
% *      local functions
% *
% **********************************************************************
% *********************************************************************
% *
% *        ConsoleGetString
% *
% *      function description
% *        get user console input and return it
% *        needs stringtext as argument e.g. "Insert Info and press ENTER"
% **********************************************************************
function UserIn = ConsoleGetString(request)
  UserIn = input(request, 's');
end 