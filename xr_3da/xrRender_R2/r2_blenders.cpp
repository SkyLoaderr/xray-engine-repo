#include "stdafx.h"

#include "blender_fat_flat.h"

CBlender*	CRender::blender_create	(CLASS_ID cls)
{	
	switch (cls)
	{
	case B_DEFAULT:			return xr_new<CBlender_fat_flat>		();		
	case B_DEFAULT_AREF:	return xr_new<CBlender_default_aref>	();	
	case B_VERT:			return xr_new<CBlender_Vertex>			();		
	case B_VERT_AREF:		return xr_new<CBlender_Vertex_aref>		();	
	case B_SCREEN_SET:		return xr_new<CBlender_Screen_SET>		();	
	case B_SCREEN_GRAY:		return xr_new<CBlender_Screen_GRAY>		();	
	case B_EDITOR_WIRE:		return xr_new<CBlender_Editor_Wire>		();	
	case B_EDITOR_SEL:		return xr_new<CBlender_Editor_Selection>();
	case B_LIGHT:			return xr_new<CBlender_LIGHT>			();		
	case B_LmBmmD:			return xr_new<CBlender_LmBmmD>			();		
	case B_LaEmB:			return xr_new<CBlender_LaEmB>			();		
	case B_LmEbB:			return xr_new<CBlender_LmEbB>			();		
	case B_B:				return xr_new<CBlender_B>				();			
	case B_BmmD:			return xr_new<CBlender_BmmD>			();			
	case B_SHADOW_TEX:		return xr_new<CBlender_ShTex>			();		
	case B_SHADOW_WORLD:	return xr_new<CBlender_ShWorld>			();		
	case B_BLUR:			return xr_new<CBlender_Blur>			();			
	case B_MODEL:			return xr_new<CBlender_Model>			();		
	case B_MODEL_EbB:		return xr_new<CBlender_Model_EbB>		();	
	case B_DETAIL:			return xr_new<CBlender_Detail_Still>	();	
	case B_TREE:			return xr_new<CBlender_Tree>			();	
	}
	return 0;
}

void		CRender::blender_destroy(CBlender* &B)
{
	xr_delete(B);
}
