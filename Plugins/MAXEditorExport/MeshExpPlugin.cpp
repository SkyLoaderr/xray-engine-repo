// file: MeshExpPlugin.cpp

#include "stdafx.h"
#pragma hdrstop

#include "MeshExpUtility.h"


//-------------------------------------------------------------------
// Class Descriptor

class MeshExpUtilityClassDesc : public ClassDesc {
	public:
	int 			IsPublic()					{ return TRUE; }
	void *			Create( BOOL loading )		{ return &U; }
	const TCHAR *	ClassName()					{ return "XRay Export"; }
	SClass_ID		SuperClassID()				{ return UTILITY_CLASS_ID; }
	Class_ID 		ClassID()					{ return EXP_UTILITY_CLASSID; }
	const TCHAR* 	Category()					{ return "XRay Export";  }
	void ResetClassParams (BOOL fileReset);
};

MeshExpUtility U;
MeshExpUtilityClassDesc MeshExpUtilityClassDescCD;

// Reset all the utility values on File/Reset
void MeshExpUtilityClassDesc::ResetClassParams (BOOL fileReset) 
{
}


//-------------------------------------------------------------------
// DLL interface


HINSTANCE hInstance;
int controlsInit = FALSE;


BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID lpvReserved) {

	hInstance = hinstDLL;
	
	if ( !controlsInit ) {
		controlsInit = TRUE;
		InitCustomControls(hInstance);
		InitCommonControls();
		InitMath();
		NConsole.print("X-Ray Object Export (ver. %d.%04d)",EXPORTER_VERSION,EXPORTER_BUILD);
		NConsole.print( "-------------------------------------------------------" );
	}

	switch(fdwReason) {

		case DLL_PROCESS_ATTACH:
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;

		case DLL_PROCESS_DETACH:
			break;
		}
	return(TRUE);
}



__declspec( dllexport ) const TCHAR *
LibDescription() { return "XRay Mesh Export utility"; }


__declspec( dllexport ) int LibNumberClasses() {
	return 1;
}


__declspec( dllexport ) ClassDesc* LibClassDesc(int i) {
	switch(i) {
		case 0: return &MeshExpUtilityClassDescCD;
		default: return 0;
	}
}


__declspec( dllexport ) ULONG LibVersion() {
	return VERSION_3DSMAX; }


