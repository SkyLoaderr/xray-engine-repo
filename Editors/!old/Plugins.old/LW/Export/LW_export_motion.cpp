#include "stdafx.h"
#include "FileSystem.h"
#include "FS.h"
#include <lwrender.h>
#include <lwhost.h>
#include "Envelope.h"
#include "Bone.h"
#include "Motion.h"
#include "scenscan\objectdb.h"
#include <lwdisplay.h>

extern "C"	LWItemInfo		*g_iteminfo;
extern "C"	LWChannelInfo	*g_chinfo;
extern "C"	LWEnvelopeFuncs	*g_envf;
extern "C"	LWSceneInfo		*g_lwsi;
extern "C"	LWInterfaceInfo	*g_intinfo;
extern "C"	LWMessageFuncs	*g_msg;
extern "C"	LWBoneInfo		*g_boneinfo;
extern "C"	LWObjectFuncs	*g_objfunc;
extern "C"	LWObjectInfo	*g_objinfo;
extern "C"	HostDisplayInfo *g_hdi;

static COMotion* m_Motion;

extern "C" {
//-----------------------------------------------------------------------------------------
void __cdecl SaveObjectMotion(GlobalFunc *global)
{
	Core._initialize("XRayPlugin",ELogCallback,FALSE);
	FS._initialize	(CLocatorAPI::flScanAppRoot);
	// get bone ID
	LWItemID		object;
	bool bErr		= false;
	
	string1024 buf="";
	string64 name;

	EFS.GetSaveName("$omotion$",buf,sizeof(buf));

	if (buf[0]){
		object		= g_iteminfo->first( LWI_OBJECT, NULL );
		while ( object ) {
			if(g_intinfo->itemFlags(object)&LWITEMF_SELECTED){
				int obj_cnt = 0;
				_splitpath( buf, 0, 0, name, 0 );
				m_Motion	= xr_new<COMotion>();
				m_Motion->SetName(name);
				m_Motion->ParseObjectMotion(object);
				m_Motion->SetParam(g_intinfo->previewStart, g_intinfo->previewEnd, (float)g_lwsi->framesPerSecond);
				m_Motion->SaveMotion(buf);
				g_msg->info	("Export successful.",buf);
				xr_delete(m_Motion);
				break;
			}
			object = g_iteminfo->next( object );
		}
	}
}
};