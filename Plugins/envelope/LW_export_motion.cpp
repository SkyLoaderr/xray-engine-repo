#include "stdafx.h"
#include "FileSystem.h"
#include "FS.h"
#include <lwrender.h>
#include <lwhost.h>
#include "Envelope.h"
#include "Bone.h"
#include "Motion.h"
#include "scenscan\objectdb.h"

extern "C"	LWItemInfo		*g_iteminfo;
extern "C"	LWChannelInfo	*g_chinfo;
extern "C"	LWEnvelopeFuncs	*g_envf;
extern "C"	LWSceneInfo		*g_lwsi;
extern "C"	LWInterfaceInfo	*g_intinfo;
extern "C"	LWMessageFuncs	*g_msg;
extern "C"	LWBoneInfo		*g_boneinfo;
extern "C"	LWObjectFuncs	*g_objfunc;
extern "C"	LWObjectInfo	*g_objinfo;

static COMotion* m_Motion;

extern "C" {
//-----------------------------------------------------------------------------------------
void __cdecl SaveObjectMotion(GlobalFunc *global){
	// get bone ID
	LWItemID		object;
	bool bErr		= false;
	
	char buf[1024];	buf[0] = 0;
	char name[64];

	FS.GetSaveName	(&FS.m_OMotion,buf);

	if (buf[0]){
		object		= g_iteminfo->first( LWI_OBJECT, NULL );
		while ( object ) {
			if(g_intinfo->itemFlags(object)&LWITEMF_SELECTED){
				int obj_cnt = 0;
				_splitpath( buf, 0, 0, name, 0 );
				m_Motion	= new COMotion();
				m_Motion->SetName(name);
				m_Motion->ParseObjectMotion(object);
				m_Motion->SetParam(g_intinfo->previewStart, g_intinfo->previewEnd, g_lwsi->framesPerSecond);
				m_Motion->SaveMotion(buf);
				g_msg->info	("Export successful.",buf);
				_DELETE(m_Motion);
				break;
			}
			object = g_iteminfo->next( object );
		}
	}
}
};