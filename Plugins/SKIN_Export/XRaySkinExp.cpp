//-----------------------------------------------------------------------------
// File: XRaySkinExp.cpp
//
// Desc: Export interface for max
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "XRaySkinExp.h"

HRESULT ExportSkinFile( const TCHAR *szFilename, ExpInterface *pExportInterface, Interface *pInterface, 
						BOOL bSuppressPrompts, BOOL bSaveSelection, HWND hwndParent);


static XRaySkinExpClassDesc XRaySkinExpDesc;
ClassDesc* GetXRaySkinExpDesc() {return &XRaySkinExpDesc;}

void XRaySkinExpClassDesc::ResetClassParams (BOOL fileReset) 
{
    // nothing to reset when Max is reset.
}

//--- XRaySkinExp -------------------------------------------------------
XRaySkinExp::XRaySkinExp(){
}

XRaySkinExp::~XRaySkinExp(){
}

int XRaySkinExp::ExtCount()
{
    //TODO: Returns the number of file name extensions supported by the plug-in.
    return 1;
}

const TCHAR *XRaySkinExp::Ext(int n)
{        
    //TODO: Return the 'i-th' file name extension (i.e. "3DS").
    return _T("ogf");
}

const TCHAR *XRaySkinExp::LongDesc()
{
    //TODO: Return long ASCII description (i.e. "Targa 2.0 Image File")
    return _T("XRay Skin File");
}
    
const TCHAR *XRaySkinExp::ShortDesc() 
{            
    //TODO: Return short ASCII description (i.e. "Targa")
    return _T("XRay Skin File");
}

const TCHAR *XRaySkinExp::AuthorName()
{            
    //TODO: Return ASCII Author name
    return _T("");
}

const TCHAR *XRaySkinExp::CopyrightMessage() 
{    
    // Return ASCII Copyright message
    return _T("Copyright 2001, GSC Game World");
}

const TCHAR *XRaySkinExp::OtherMessage1() 
{        
    //TODO: Return Other message #1 if any
    return _T("");
}

const TCHAR *XRaySkinExp::OtherMessage2() 
{        
    //TODO: Return other message #2 in any
    return _T("");
}

unsigned int XRaySkinExp::Version()
{                
    //TODO: Return Version number * 100 (i.e. v3.01 = 301)
    return 110;
}

void XRaySkinExp::ShowAbout(HWND hWnd)
{            
    // Optional
}

BOOL XRaySkinExp::SupportsOptions(int ext, DWORD options)
{
    return ( options == SCENE_EXPORT_SELECTED );
}

int XRaySkinExp::DoExport(const TCHAR *szFilename,ExpInterface *ei,
                        Interface *i, BOOL suppressPrompts, DWORD options) 
{
	CreateLog	();
	InitMath	();

    HRESULT hr;
    BOOL bSaveSelection = (options & SCENE_EXPORT_SELECTED);

    // return failure if no filename provided
    if (szFilename == NULL)
        return FALSE;

    // actually export the file
    hr = ExportSkinFile(szFilename, ei, i, suppressPrompts, bSaveSelection, GetActiveWindow());
    if (FAILED(hr))
        return FALSE;
    else
        return TRUE;
}
    

