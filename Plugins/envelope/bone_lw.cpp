#include "stdafx.h"
#include "bone.h"
#include "envelope.h"

#ifdef LWO_EXPORTS
extern "C"	LWItemInfo		*g_iteminfo;
extern "C"	LWChannelInfo	*g_chinfo;
extern "C"	LWBoneInfo		*g_boneinfo;
extern "C"	LWEnvelopeFuncs	*g_envf;

void CBone::ParseBone(LWItemID bone){
	LWItemID P = g_iteminfo->parent(bone);
	if (g_iteminfo->type(P)==LWI_BONE) SetParent(g_iteminfo->name(P));

	LWDVector vec;
	g_boneinfo->restParam(bone,LWIP_POSITION,vec);
	rest_offset.set(vec[0],vec[1],vec[2]);
	g_boneinfo->restParam(bone,LWIP_ROTATION,vec);
	rest_rotate.set(vec[0],vec[1],vec[2]);

	rest_length = g_boneinfo->restLength(bone);

	SetWMap(g_boneinfo->weightMap(bone));
}
#endif
