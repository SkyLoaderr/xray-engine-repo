// Blender.cpp: implementation of the CBlender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "Blender.h"

void CBlender_DESC::Setup	(LPCSTR N)
{
	// Name
	VERIFY(strlen(N)<128);
	VERIFY(0==strchr(N,'.'));
	strcpy(cName,N);
	strlwr(cName);
	
	// Computer
	const u32 comp = MAX_COMPUTERNAME_LENGTH + 1;
	char	buf	[comp];
	DWORD	sz = comp;
	GetComputerName(buf,&sz);
	if (sz > 31) sz=31;
	buf[sz] = 0;
	strcpy(cComputer,buf);
	
	// Time
	_tzset(); time( (long*)&cTime );
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlender::CBlender()
{
	oPriority.min	= 0;
	oPriority.max	= 3;
	oPriority.value	= 1;
	strcpy			(oT_Name,	"$base0");
	strcpy			(oT_xform,	"$null");
}

CBlender::~CBlender()
{

}

void	CBlender::Save(IWriter& FS )
{
	FS.w			(&description,sizeof(description));
	xrPWRITE_MARKER (FS,"General");
	xrPWRITE_PROP	(FS,"Priority",			xrPID_INTEGER,	oPriority);
	xrPWRITE_PROP	(FS,"Strict sorting",	xrPID_BOOL,		oStrictSorting);
	xrPWRITE_MARKER	(FS,"Base Texture");
	xrPWRITE_PROP	(FS,"Name",				xrPID_TEXTURE,	oT_Name);
	xrPWRITE_PROP	(FS,"Transform",		xrPID_MATRIX,	oT_xform);
}

void	CBlender::Load(	IReader& FS, WORD version )
{
	// Read desc and doesn't change version
	WORD	V		= description.version;
	FS.r			(&description,sizeof(description));
	description.version	= V;

	// Properties
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_INTEGER,	oPriority);
	xrPREAD_PROP	(FS,xrPID_BOOL,		oStrictSorting);
	xrPREAD_MARKER	(FS);
	xrPREAD_PROP	(FS,xrPID_TEXTURE,	oT_Name);
	xrPREAD_PROP	(FS,xrPID_MATRIX,	oT_xform);
}

void	CBlender::Compile(CBlender_Compile& C)
{
	if (C.bEditor)	C.SetParams	(oPriority.value,oStrictSorting.value?true:false,true,	false);
	else			C.SetParams	(oPriority.value,oStrictSorting.value?true:false,false,	false);
}

//////////////////////////////////////////////////////////////////////
#include "blender_clsid.h"
#include "blenderdefault.h"
#include "blender_default_aref.h"
#include "blender_vertex.h"
#include "blender_vertex_aref.h"
#include "blender_screen_set.h"
#include "blender_screen_gray.h"
#include "blender_editor_wire.h"
#include "blender_editor_selection.h"
#include "blender_light.h"
#include "blender_LaEmB.h"
#include "blender_Lm(EbB).h"
#include "blender_LmBmmD.h"
#include "blender_BmmD.h"
#include "blender_B.h"
#include "blender_shadow_texture.h"
#include "blender_shadow_world.h"
#include "blender_blur.h"
#include "blender_model.h"
#include "blender_model_ebb.h"
#include "blender_detail_still.h"
#include "blender_tree.h"

CBlender*	CBlender::Create	(CLASS_ID cls)
{	
	switch (cls)
	{
	case B_DEFAULT:			return xr_new<CBlender_default>();		
	case B_DEFAULT_AREF:	return xr_new<CBlender_default_aref>();	
	case B_VERT:			return xr_new<CBlender_Vertex>();		
	case B_VERT_AREF:		return xr_new<CBlender_Vertex_aref>();	
	case B_SCREEN_SET:		return xr_new<CBlender_Screen_SET>();	
	case B_SCREEN_GRAY:		return xr_new<CBlender_Screen_GRAY>();	
	case B_EDITOR_WIRE:		return xr_new<CBlender_Editor_Wire>();	
	case B_EDITOR_SEL:		return xr_new<CBlender_Editor_Selection>();
	case B_LIGHT:			return xr_new<CBlender_LIGHT>();		
	case B_LmBmmD:			return xr_new<CBlender_LmBmmD>();		
	case B_LaEmB:			return xr_new<CBlender_LaEmB>();		
	case B_LmEbB:			return xr_new<CBlender_LmEbB>();		
	case B_B:				return xr_new<CBlender_B>();			
	case B_BmmD:			return xr_new<CBlender_BmmD>();			
	case B_SHADOW_TEX:		return xr_new<CBlender_ShTex>();		
	case B_SHADOW_WORLD:	return xr_new<CBlender_ShWorld>();		
	case B_BLUR:			return xr_new<CBlender_Blur>();			
	case B_MODEL:			return xr_new<CBlender_Model>();		
	case B_MODEL_EbB:		return xr_new<CBlender_Model_EbB>();	
	case B_DETAIL:			return xr_new<CBlender_Detail_Still>();	
	case B_TREE:			return xr_new<CBlender_Tree>();	
	}
    return 0;
}

IC bool		p_sort			(CBlender* A, CBlender* B)
{
	return stricmp(A->getComment(),B->getComment())<0;
}

void		CBlender::CreatePalette(vector<CBlender*> &palette)
{
	R_ASSERT(palette.empty());
	palette.push_back(Create(B_DEFAULT));
	palette.push_back(Create(B_DEFAULT_AREF));
	palette.push_back(Create(B_VERT));
	palette.push_back(Create(B_VERT_AREF));
	palette.push_back(Create(B_SCREEN_SET));
	palette.push_back(Create(B_SCREEN_GRAY));
	palette.push_back(Create(B_EDITOR_WIRE));
	palette.push_back(Create(B_EDITOR_SEL));
	palette.push_back(Create(B_LIGHT));
	palette.push_back(Create(B_LmBmmD));
	palette.push_back(Create(B_LaEmB));
	palette.push_back(Create(B_LmEbB));
	palette.push_back(Create(B_BmmD));
	palette.push_back(Create(B_B));
	palette.push_back(Create(B_SHADOW_TEX));
	palette.push_back(Create(B_SHADOW_WORLD));
	palette.push_back(Create(B_BLUR));
	palette.push_back(Create(B_MODEL));
	palette.push_back(Create(B_MODEL_EbB));
	palette.push_back(Create(B_DETAIL));
	palette.push_back(Create(B_TREE));

	std::sort		(palette.begin(),palette.end(),p_sort);
}
