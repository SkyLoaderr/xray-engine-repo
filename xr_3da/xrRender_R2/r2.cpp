#include "stdafx.h"
#include "r2.h"
#include "..\fbasicvisual.h"
#include "..\fcached.h"
#include "..\xr_object.h"
#include "..\CustomHUD.h"
#include "..\lighttrack.h"

CRender										RImplementation;

// Implementation
IVisual*				CRender::model_Create			(LPCSTR name)					{ return Models.Create(name);			}
IVisual*				CRender::model_Create			(LPCSTR name, IReader* data)	{ return Models.Create(name,data);		}
IVisual*				CRender::model_Duplicate		(IVisual* V)					{ return Models.Instance_Duplicate(V);	}
void					CRender::model_Delete			(IVisual* &V)					{ Models.Delete(V);						}
IRender_DetailModel*	CRender::model_CreateDM			(IReader*	F)
{
	CDetail*	D		= xr_new<CDetail> ();
	D->Load				(F);
	return D;
}
void					CRender::model_Delete			(IRender_DetailModel* & F)
{
	if (F)
	{
		CDetail*	D	= (CDetail*)F;
		D->Unload		();
		xr_delete		(D);
		F				= NULL;
	}
}
IVisual*				CRender::model_CreatePS			(LPCSTR name, PS::SEmitter* E)	
{ 
	PS::SDef_RT*	source	= PSystems.FindPS	(name);
	VERIFY					(source);
	return Models.CreatePS	(source,E);
}

int						CRender::getVisualsCount		()					{ return Visuals.size();								}
IRender_Portal*			CRender::getPortal				(int id)			{ VERIFY(id<int(Portals.size()));	return Portals[id];	}
IRender_Sector*			CRender::getSector				(int id)			{ VERIFY(id<int(Sectors.size()));	return Sectors[id];	}
IRender_Sector*			CRender::getSectorActive		()					{ return pLastSector;									}
IVisual*				CRender::getVisual				(int id)			{ VERIFY(id<int(Visuals.size()));	return Visuals[id];	}
D3DVERTEXELEMENT9*		CRender::getVB_Format			(int id)			{ VERIFY(id<int(DCL.size()));		return DCL[id].begin();	}
IDirect3DVertexBuffer9*	CRender::getVB					(int id)			{ VERIFY(id<int(VB.size()));		return VB[id];		}
IDirect3DIndexBuffer9*	CRender::getIB					(int id)			{ VERIFY(id<int(IB.size()));		return IB[id];		}
IRender_Target*			CRender::getTarget				()					{ return &Target;										}

IRender_Light*			CRender::light_create			()					{ return L_Dynamic.Create();							}
void					CRender::light_destroy			(IRender_Light* &L)	{ if (L) { L_Dynamic.Destroy((CLightPPA*)L); L=0; }		}
void					CRender::L_select				(Fvector &pos, float fRadius, vector<xrLIGHT*>& dest)
{	
	Msg		("! NotImplemented: CRender::L_select");
}

void					CRender::flush					()					{ flush_Models();									}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);							}

void					CRender::add_Visual				(IVisual*		V )	{ add_leafs_Dynamic(V);								}
void					CRender::add_Geometry			(IVisual*		V )	{ add_Static(V,View->getMask());					}
void					CRender::add_Lights				(vector<WORD> &	V )	{ L_DB.add_sector_lights(V);						}
void					CRender::add_Glows				(vector<WORD> &	V )	
{ 
	Msg		("! NotImplemented: CRender::add_Glows");
}
void					CRender::add_Patch				(Shader* S, const Fvector& P1, float s, float a, BOOL bNearer)
{
	Msg		("! NotImplemented: CRender::add_Patch");
}
void		CRender::add_Wallmark		(Shader* S, const Fvector& P, float s, CDB::TRI* T)
{
	Msg		("! NotImplemented: CRender::add_Wallmark");
}
void		CRender::set_Object			(CObject*		O )	
{ 
	val_pObject				= O;
}
