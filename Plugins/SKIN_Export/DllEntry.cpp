//-----------------------------------------------------------------------------
// File: DllEntry.cpp
//
// Desc: Contains the Dll Entry stuff
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "xrayskinexp.h"

extern ClassDesc* GetXRaySkinExpDesc();

HINSTANCE g_hInstance;
int controlsInit = FALSE;

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved)
{
    g_hInstance = hinstDLL;                // Hang on to this DLL's instance handle.

    if (!controlsInit) {
        controlsInit = TRUE;
        InitCustomControls(g_hInstance);    // Initialize MAX's custom controls
        InitCommonControls();				// Initialize Win95 controls
    }
            
    return (TRUE);
}

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}

// This function returns the number of plug-in classes this DLL
//TODO: Must change this number when adding a new class
__declspec( dllexport ) int LibNumberClasses()
{
    return 1;
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
    switch(i) {
        case 0: return GetXRaySkinExpDesc();
        default: return 0;
    }
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

TCHAR *GetString(int id)
{
    static TCHAR buf[256];

    if (g_hInstance)
        return LoadString(g_hInstance, id, buf, sizeof(buf)) ? buf : NULL;
    return NULL;
}

