#include "stdafx.h"
#include "r2.h"
#include "..\fbasicvisual.h"
#include "..\xr_object.h"
#include "..\CustomHUD.h"

CRender										RImplementation;

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(CObject* parent)				{ return 0;								}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)	{ xr_delete(p);							}
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
	PS::SDef*	source		= PSystems.FindPS	(name);
	VERIFY					(source);
	return Models.CreatePS	(source,E);
}
IVisual*				CRender::model_CreatePG			(LPCSTR name)	
{ 
	PS::CPGDef*	source		= PSystems.FindPG	(name);
	VERIFY					(source);
	return Models.CreatePG	(source);
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

IRender_Light*			CRender::light_create			()					{ return Lights.Create();								}
void					CRender::light_destroy			(IRender_Light* &L)	{ if (L) { Lights.Destroy((light*)L); L=0; }			}
void					CRender::L_select				(Fvector &pos, float fRadius, vector<xrLIGHT*>& dest)
{	
	//Msg		("! NotImplemented: CRender::L_select");
}

void					CRender::flush					()					{ flush_Models();									}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);							}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);							}

void					CRender::add_Visual				(IVisual*		V )	{ add_leafs_Dynamic(V);								}
void					CRender::add_Geometry			(IVisual*		V )	{ add_Static(V,View->getMask());					}
void					CRender::add_Lights				(vector<WORD> &	V )	
{ 
	Lights.add_sector_lights(V);						
}
void					CRender::add_Glows				(vector<WORD> &	V )	
{
	//Msg		("! NotImplemented: CRender::add_Glows");
}
void					CRender::add_Patch				(Shader* S, const Fvector& P1, float s, float a, BOOL bNearer)
{
	//Msg		("! NotImplemented: CRender::add_Patch");
}
void		CRender::add_Wallmark		(Shader* S, const Fvector& P, float s, CDB::TRI* T)
{
	//Msg		("! NotImplemented: CRender::add_Wallmark");
}
void		CRender::set_Object			(CObject*		O )	
{ 
	val_pObject				= O;
}
void		CRender::rmNear				()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void		CRender::rmFar				()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void		CRender::rmNormal			()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender()
{
	b_nv3x	= (strstr(Core.Params,"-nv3x"))?TRUE:FALSE;
}

CRender::~CRender()
{
}

void CRender::flush_Models	()
{
	/*
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();
	*/
}

// Device events
void CRender::OnDeviceCreate	()
{
	REQ_CREATE					();
	Target.OnDeviceCreate		();
	LR_Direct.Create			();

	PSystems.OnCreate			();
	PSystems.OnDeviceCreate		();
	level_Load					();

	rmNormal					();
	marker						= 0;
}

void CRender::OnDeviceDestroy	()
{
	level_Unload				();
	PSystems.OnDeviceDestroy	();
	PSystems.OnDestroy			();

	LR_Direct.Destroy			();
	Target.OnDeviceDestroy		();
}

#include "..\fhierrarhyvisual.h"
#include "..\bodyinstance.h"
#include "..\fmesh.h"

void CRender::RenderBox			(IRender_Sector* _S, Fbox& BB, int sh)
{
	//Msg		("! NotImplemented: CRender::RenderBox");
	/*
	CSector*	S			= (CSector*)_S;
	lstVisuals.clear		();
	lstVisuals.push_back	(S->Root());
	
	for (u32 test=0; test<lstVisuals.size(); test++)
	{
		IVisual*	V		= 	lstVisuals[test];
		
		// Visual is 100% visible - simply add it
		vector<IVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals
		
		switch (V->Type) {
		case MT_HIERRARHY:
			{
				// Add all children
				FHierrarhyVisual* pV = (FHierrarhyVisual*)V;
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					IVisual* T			= *I;
					if (BB.intersect(T->vis.box))	lstVisuals.push_back(T);
				}
			}
			break;
		case MT_SKELETON:
			{
				// Add all children
				CKinematics * pV = (CKinematics*)V;
				pV->Calculate			();
				I = pV->children.begin	();
				E = pV->children.end		();
				for (; I!=E; I++)		{
					IVisual* T			= *I;
					if (BB.intersect(T->vis.box))	lstVisuals.push_back(T);
				}
			}
			break;
		default:
			{
				// Renderable visual
				ShaderElement* E	= V->hShader->E[sh];
				for (u32 pass=0; pass<E->Passes.size(); pass++)
				{
					RCache.set_Element			(E,pass);
					V->Render					(-1.f);
				}
			}
			break;
		}
	}
	*/
}
