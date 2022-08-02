#include "tsapicvi.h"
#include "login.h"
#include "toolbox.h"    /* for the errchk macro */
#include "tsutil.h"
#include <cvirte.h>    /* Initialize CVI libraries */


/********************************************************************/
/* TYPES                                                            */
/********************************************************************/
typedef CAObjHandle             EngineHandle;
typedef CAObjHandle             UserHandle;
typedef CAObjHandle             SeqContextHandle;
typedef struct {
    const char *lastUserName;
    VBOOL isInitialLogin;
} LoginParamsType;

typedef HRESULT (FunType)(EngineHandle engine, void *data);


/********************************************************************/
/* STATIC FUNCTION DECLARATIONS                                     */
/********************************************************************/
static FunType LoginUserFun;
static FunType LogoutUserFun;

static HRESULT DoGetNewUser(EngineHandle engine, const char *lastUserName, 
                            VBOOL isInitialLogin,
                            UserHandle *userPtr, VBOOL *isValidUser);
static HRESULT DoLogoutUser(EngineHandle engine);
static HRESULT DoLoginUser(EngineHandle engine, UserHandle user);


static void CallFunAndReportError(FunType *funPtr, SeqContextHandle seqContext, void *data);
static void ReportError(SeqContextHandle seqContext,
                        HRESULT errorCode, 
                        ERRORINFO *errInfoPtr);

/********************************************************************/
/* STATIC VARIABLES                                                 */
/********************************************************************/
static ERRORINFO gErrorInfo;


/*--------------------------------------------------------------------------*/
/* This function is called by the LoginLogout sequence to login a new user */
void LoginUser(SeqContextHandle seqContext, const char *lastUserName, 
               long isInitialLogin)
{
    LoginParamsType loginParams;
    loginParams.lastUserName = lastUserName;
    loginParams.isInitialLogin = (isInitialLogin == 0 ? VFALSE : VTRUE);

    CallFunAndReportError(&LoginUserFun, 
                          seqContext,
                          (void *)&loginParams);
}

/*--------------------------------------------------------------------------*/
/* This function is called by the LoginLogout sequence to logout the user */
void LogoutUser(SeqContextHandle seqContext)
{
    CallFunAndReportError(&LogoutUserFun, 
                          seqContext,
                          NULL);
}

/*--------------------------------------------------------------------------*/
static HRESULT LoginUserFun(EngineHandle engine, void *data)
{
    HRESULT error = 0;
    LoginParamsType *loginParams = (LoginParamsType *)data;
    const char *lastUserName = loginParams->lastUserName;
    VBOOL isInitialLogin = loginParams->isInitialLogin;
    UserHandle user = 0;
    VBOOL isValidUser;

    errChk(DoGetNewUser(engine, lastUserName, isInitialLogin, &user, &isValidUser));
    if (isValidUser) {
        errChk(DoLogoutUser(engine));
        errChk(DoLoginUser(engine, user));
    }

Error:
    if (user != 0) {
        CA_DiscardObjHandle(user);
    }
    return error;
}

/*--------------------------------------------------------------------------*/
static HRESULT LogoutUserFun(EngineHandle engine, void *data)
{
    return DoLogoutUser(engine);
}


/*--------------------------------------------------------------------------*/
static HRESULT DoGetNewUser(EngineHandle engine, const char *lastUserName,
                            VBOOL isInitialLogin,
                            UserHandle *userPtr, VBOOL *isValidUser)
{
    HRESULT error = 0;
    VBOOL autoLoginSystemUser = VFALSE;
    VBOOL displayLoginDialog = VTRUE;

    /* For the first login, check the "auto-login-system-user" option.
     * If it is enabled and there is a TestStand user that matches the
     * current system user, then log him in automatically without 
     * requiring a password.
     */
    if (isInitialLogin)
    {
        errChk(TS_EngineGetAutoLoginSystemUser(engine, &gErrorInfo, &autoLoginSystemUser));

        if (autoLoginSystemUser)
        {
            char sysUserNameBuf[256];
			DWORD bufSize = (DWORD)sizeof(sysUserNameBuf); // Cast OK because sizeof(sysUserNameBuf) is 256.

            /* Get system user name */
            if (GetUserName(sysUserNameBuf, &bufSize))
            {
                errChk( TS_EngineGetUser(engine,&gErrorInfo,sysUserNameBuf,userPtr));

                if (*userPtr != 0) /* got a valid user */
                {
                    displayLoginDialog = VFALSE;
                    *isValidUser = VTRUE;
                }
            }
        }
    }

    if (displayLoginDialog)
    {
        errChk(TS_EngineDisplayLoginDialog(engine, 
                                           &gErrorInfo, 
                                           "",                  /* dialog title */
                                           (char *)lastUserName,    /* login name */
                                           "",                  /* password */
                                           VTRUE, /* modal to main wnd */
                                           userPtr,
                                           isValidUser));
    }

Error:
    return error;
}

/*--------------------------------------------------------------------------*/
static HRESULT DoLogoutUser(EngineHandle engine)
{
    return TS_EngineSetCurrentUser (engine, &gErrorInfo, 0); /* 0 == no user */
}

/*--------------------------------------------------------------------------*/
static HRESULT DoLoginUser(EngineHandle engine, UserHandle user)
{
    return TS_EngineSetCurrentUser (engine, &gErrorInfo, user);
}

/*--------------------------------------------------------------------------*/
static void CallFunAndReportError(FunType *funPtr, SeqContextHandle seqContext, void *data)
{
    HRESULT error = 0;
    EngineHandle engine = 0;

    errChk(TS_SeqContextGetEngine (seqContext, &gErrorInfo, &engine));

    errChk((*funPtr)(engine, data));

Error:
    if (engine != 0) {
        CA_DiscardObjHandle(engine);
    }

    if (error < 0) {
        ReportError(seqContext, 
                    error,
                    &gErrorInfo);
    }

    return;
}


/*--------------------------------------------------------------------------*/
static void ReportError(SeqContextHandle seqContext,
                        HRESULT errorCode, 
                        ERRORINFO *errInfoPtr)
{
    ERRORINFO localErrInfo;
    HRESULT error = 0;
    char buffer[1024];
    char *errorMessage = NULL;

    if (errInfoPtr != NULL 
        && errInfoPtr->description != NULL
        && errInfoPtr->description[0] != '\0') 
    {
        errorMessage = errInfoPtr->description;
    } 
    else 
    {
        CA_GetAutomationErrorString(errorCode, buffer, sizeof(buffer));
        errorMessage = buffer;
    }


    /* This function is in steputil.c */
    TS_SetStepError(seqContext, errorCode, errorMessage);
}


/*--------------------------------------------------------------------------*/
int __stdcall DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            if (InitCVIRTE (hinstDLL, 0, 0) == 0)    /* Initialize CVI libraries */
                return 0;     /* out of memory */
            break;
        case DLL_PROCESS_DETACH:
            CloseCVIRTE ();    /* Initialize CVI libraries */
            break;
    }
    
    return 1;
}

/*--------------------------------------------------------------------------*/
int __stdcall DllEntryPoint (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    /* Included for compatibility with Borland */

    return DllMain (hinstDLL, fdwReason, lpvReserved);
}



