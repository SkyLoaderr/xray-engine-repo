#ifndef xrEPropsH
#define xrEPropsH

#ifdef XR_EPROPS_EXPORTS
	#define XR_EPROPS_API __declspec(dllexport)
#else
	#define XR_EPROPS_API __declspec(dllimport)
#endif

#include "FolderLib.h"                 
#include "PropertiesListTypes.h"
#include "PropertiesListHelper.h"
#include "ItemListTypes.h"
#include "ItemListHelper.h"

//---------------------------------------------------------------------------
void XR_EPROPS_API CheckWindowPos(TForm* form);
//---------------------------------------------------------------------------

#endif
 