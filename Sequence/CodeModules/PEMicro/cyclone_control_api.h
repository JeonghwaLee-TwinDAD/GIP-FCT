#ifndef CYCLONE_CONTROL_API_H
#define CYCLONE_CONTROL_API_H

/* This header file was modified from the original
   for compatibility with LabView */

/* CATEGORY NAMES AND THEIR PROPERTIES
A list of properties supported by the Cyclone can
be retrieved with the getPropertyList() routine */

// DLL Loading / Unloading
extern "C" void            _cdecl  enumerateAllPorts(void);
extern "C" void            _cdecl  disconnectFromAllCyclones(void);
extern "C" char *          _cdecl  version(void);
extern "C" unsigned long   _cdecl  queryNumberOfAutodetectedCyclones(void);
extern "C" char *          _cdecl  queryInformationOfAutodetectedCyclone(signed long autodetectIndex, signed long informationType);

// Cyclone Connecting / Disconnecting Calls
extern "C" unsigned long   _cdecl  connectToCyclone(char *nameIpOrPortIdentifier);
extern "C" void            _cdecl  setLocalMachineIpNumber(char *ipNumber);

// Controlling Cyclone Programming
extern "C" bool            _cdecl  clearOverlayProgramData(unsigned long cycloneHandle);
extern "C" bool            _cdecl  specifyOverlayProgramData(unsigned long cycloneHandle, unsigned long targetAddress, unsigned long dataLength, void *pointer);
extern "C" bool            _cdecl  startImageExecution(unsigned long cycloneHandle, unsigned long imageId);
extern "C" bool            _cdecl  startDynamicDataProgram(unsigned long cycloneHandle, unsigned long targetAddress, unsigned long dataLength, void *pointer);
extern "C" unsigned long   _cdecl  checkCycloneExecutionStatus(unsigned long cycloneHandle);
extern "C" bool            _cdecl  dynamicReadBytes(unsigned long cycloneHandle, unsigned long targetAddress, unsigned long dataLength, void *pointer);
extern "C" unsigned long   _cdecl  getNumberOfErrors(unsigned long cycloneHandle);
extern "C" signed long     _cdecl  getErrorCode(unsigned long cycloneHandle, unsigned long errorNum);
extern "C" unsigned long   _cdecl  getLastErrorAddr(unsigned long cycloneHandle);
extern "C" char *          _cdecl  getDescriptionOfErrorCode(unsigned long cycloneHandle, signed long errorCode);
extern "C" bool            _cdecl  resetCyclone(unsigned long cycloneHandle, unsigned long resetDelayInMs);

// Configuration / Image Maintenance Calls
extern "C" char *          _cdecl  getImageDescription(unsigned long cycloneHandle, unsigned long imageID);
extern "C" bool            _cdecl  compareImageInCycloneWithFile(unsigned long cycloneHandle, char *aFile, unsigned long imageId);
extern "C" bool            _cdecl  formatCycloneMemorySpace(unsigned long cycloneHandle, unsigned long selectedMediaType);
extern "C" bool            _cdecl  eraseCycloneImage(unsigned long cycloneHandle, unsigned long imageId);
extern "C" unsigned long   _cdecl  addCycloneImage(unsigned long cycloneHandle, unsigned long selectedMediaType, bool replaceImageOfSameDescription, char *aFile);
extern "C" unsigned long   _cdecl  countCycloneImages(unsigned long cycloneHandle);
extern "C" char *          _cdecl  getPropertyValue(unsigned long cycloneHandle, unsigned long resourceOrImageId, char *categoryName, char *propertyName);
extern "C" bool            _cdecl  setPropertyValue(unsigned long cycloneHandle, unsigned long resourceOrImageId, char *categoryName, char *propertyName, char *newValue);
extern "C" char *          _cdecl  getPropertyList(unsigned long cycloneHandle, unsigned long resourceOrImageId, char *categoryName);

extern "C" char *          _cdecl  getFirmwareVersion(unsigned long cycloneHandle);
extern "C" bool            _cdecl  cycloneSpecialFeatures(unsigned long featureNum, bool setFeature, unsigned long paramValue1, unsigned long paramValue2, unsigned long paramValue3, void *paramReference1, void *paramReference2);

#endif