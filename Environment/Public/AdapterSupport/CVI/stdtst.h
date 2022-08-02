/*----------------------------------------------------------------------------*/
/*    Copyright (c) National Instruments 1998.  All Rights Reserved.     	  */
/*----------------------------------------------------------------------------*/

#ifndef STDTST_H
#define STDTST_H

	/********************************************************/
	/*                                                      */
	/*  The prototype for test functions called by the 		*/
	/*  C/CVI Std Prototype Adapter is:                   	*/
	/*                                                      */
	/*  void funcName (tTestData *data, tTestError *error)	*/
	/*                                                     	*/
	/* 	The structure definitions for the tTestData and		*/
	/*	tTestError types are at the end of this file 		*/
	/*                                                      */
	/********************************************************/

#ifdef _CVI_
#include <cvidef.h>
#endif

#include <ctype.h>	// must be included before stddef.h because of bug with definition of wchar_t in CVI 6.0
#include <stddef.h>

#if (!defined(_NI_mswin16_)) && (defined(_NI_mswin32_)||defined(__WIN32__)||defined(__NT__)||defined(_WIN32)||defined(WIN32)) 
	#define TX_TEST __cdecl
	#define TX_CDECL __cdecl
#else
	#define TX_TEST
	#define TX_CDECL
#endif

	/* function pointer typedefs */
typedef void (TX_CDECL * tFreePtr) (void  *Mem_Block_Pointer);	/* free()   */
typedef void *(TX_CDECL * tMallocPtr) (size_t Mem_Block_Size);	/* malloc() */

/* Returns 0 if unable to allocate memory; returns a non-zero value on success */
typedef int (TX_CDECL * tReplaceStringPtr) (char **destString, char *srcString); 	/* ReplaceString() */

struct 		IDispatch;
typedef intptr_t CAObjHandle;

typedef int Boolean;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

enum { FAIL, PASS };


enum	{ /* These values are no longer used but are included for backwards compatibility */
		IN_NONE, IN_META_PRETEST, IN_SEQ_PRETEST, IN_PRETEST, IN_TEST, 
		IN_POSTTEST, IN_SEQ_POSTTEST, IN_META_POSTTEST
		};

typedef int tErrLoc;	/* No longer used but included for backwards compatibility */

#define CURRENT_TESTDATA_STRUCT_VERSION		2

#pragma pack(8)

typedef struct ClassData_Rec
	{
    int                 result;         	/* [out] 	Set to PASS or FAIL to indicate whether the 		*/ 
    										/* 			test passed.  Set it only in tests that 			*/
    										/*			determine their own pass/fail status.				*/
    double              measurement;    	/* [out] 	Numeric measurement taken by test function	     	*/ 
    										/*		    and placed into the <step>.Result.Numeric property, */
    										/*          if it exists.										*/
    char *              inBuffer;       	/* [in]	 	For passing parameters into test.        			*/  
    char *              outBuffer;      	/* [out] 	For output messages to display in the report.      	*/  
    char * const        modPath;        	/* [in]		Path of module containing the test function.		*/  
    char * const        modFile;        	/* [in]		Base file of module containing the test function.	*/  
    void *              hook;           	/* 			No longer used.           							*/  
    int                 hookSize;       	/* 			No longer used.										*/
    tMallocPtr const    mallocFuncPtr;  	/* [in]		Function to call to allocate memory for				*/
                                        	/*   			inBuffer, outBuffer, errorMessage.     			*/
    tFreePtr const      freeFuncPtr;    	/* [in]		Function to call to free memory for   	 			*/
                                        	/*   			inBuffer, outBuffer, errorMessage.     			*/
	struct IDispatch *  seqContextDisp;		/* [in]		NULL if sequence context is not passed in.			*/
	CAObjHandle			seqContextCVI;		/* [in]		0 if sequence context is not passed in.				*/
	char *              stringMeasurement;	/* [out]    String obtained by test function and placed into 	*/
    										/*		    the <step>.Result.String property, if it exists.    */
	int					structVersion;  	/* [in]     To enable tests to detect new versions of this		*/
											/*          structure.                                          */
    tReplaceStringPtr const replaceStringFuncPtr;	/* [in]	Function to call to update string fields in			*/
													/*		tTestData or tTestError structures.					*/
	} tTestData;


typedef struct ClassError_Rec
	{
	Boolean		errorFlag;		/* The test function must set this to TRUE if an error occurs.  */
	tErrLoc		errorLocation;	/* No longer used, but included for backwards compatibility.	*/
	int			errorCode;		/* The test function may set this to a non-zero value if an     */
								/* error occurs.												*/
	char *		errorMessage;	/* The test function may set this to a descriptive string if    */
								/* an error occurs.												*/
	} tTestError;

#pragma pack()

#endif /* STDTST_H */
