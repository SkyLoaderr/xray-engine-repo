#include "stdafx.h"
#include <lwrender.h>
#include <lwhost.h>
#include "scenscan\objectdb.h"
#include "exportobject\editobject.h"
#include "FileSystem.h"
#include "FS.h"

extern "C" LWItemInfo		*g_iteminfo;
extern "C" LWMessageFuncs	*g_msg;
extern "C" LWInterfaceInfo	*g_intinfo;

static BoneVec* m_LWBones=0;

static void AppendBone(LWItemID bone){
	m_LWBones->push_back(new CBone());
	CBone* B = m_LWBones->back();
	B->SetName(g_iteminfo->name(bone));
	B->ParseBone(bone);
}

static void RecurseBone(LWItemID parent){
	LWItemID bone = g_iteminfo->firstChild(parent);
	while (bone!=LWITEM_NULL){
		if (g_iteminfo->type(bone)==LWI_BONE){
			AppendBone(bone);
			RecurseBone(bone);
		}
		bone = g_iteminfo->nextChild(parent,bone);
	}
}

static bool ParseObjectBones(LWItemID object, int& obj_cnt){
	LWItemID	bone, parent;
	bone		= g_iteminfo->first( LWI_BONE, object );

	if (!bone){
		g_msg->error("Can't find bone.",0);
		return false;
	}
	
	while (true){
		parent = g_iteminfo->parent(bone);
		if (!parent){
			g_msg->error("Can't find root bone.",0);
			return false;
		}
		if (g_iteminfo->type(parent)!=LWI_BONE) break;
		else									bone = parent;
	}

	if (bone){
		if (obj_cnt>0){
			g_msg->error("Can't support multiple objects.",0);
			return false;
		}
		AppendBone(bone);
		RecurseBone	(bone);

		obj_cnt++;
	}
	return true;
}

extern "C" {
//-----------------------------------------------------------------------------------------
void __cdecl SaveObject(GlobalFunc *global){
	// get bone ID
	LWItemID		object;
	bool bErr		= true;

	char buf[1024];	buf[0] = 0;
	if (FS.GetSaveName(&FS.m_Import,buf)){
		object		= g_iteminfo->first( LWI_OBJECT, NULL );
		int obj_cnt = 0;

		while (object){
			if(g_intinfo->itemFlags(object)&LWITEMF_SELECTED){
				ObjectDB	*odb = getObjectDB(object,global);
				if (!odb){ 
					object = g_iteminfo->next( object );
					bErr = true;
					continue;
				}
				
				bErr = false;
				
				char name[1024];
				_splitpath( buf, 0, 0, name, 0 );
				
				CEditObject* obj = new CEditObject(name,true);
				
				// parse bone if exist
				bool bBoneExists=false;
				if (g_iteminfo->first( LWI_BONE, object )){
					m_LWBones = &obj->GetBones();
					bBoneExists = true;
					if (!ParseObjectBones(object,obj_cnt)) bErr = true;
				}
				if (!bErr){
					if (obj->Import_LWO(odb)){ 
						obj->SaveObject(buf);
						g_msg->info	("Export successful.",buf);
					}else bErr = true;
				}
				
				if (bErr) g_msg->error("Export failed.",0);
				bErr = false;
				
				_DELETE(obj);
				m_LWBones = 0;
				freeObjectDB(odb);
				break;
			}
			object = g_iteminfo->next( object );
		}
	}
	if (bErr) g_msg->error("Export failed.",0);
}
//-----------------------------------------------------------------------------------------
};