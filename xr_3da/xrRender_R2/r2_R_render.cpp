#include "stdafx.h"
#include "..\fbasicvisual.h"
#include "..\customhud.h"

using namespace SceneGraph;

extern float		r_ssaDISCARD;
extern float		r_ssaDONTSORT;
extern float		r_ssaLOD_A;
extern float		r_ssaLOD_B;
extern float		r_ssaHZBvsTEX;

extern float		g_fLOD, g_fFarSq;

IC float calcLOD	(float fDistSq, float R)
{
	float dist	= g_fFarSq - fDistSq + R*R;
	float lod	= g_fLOD*dist/g_fFarSq;
	clamp		(lod,0.001f,0.999f);
	return		lod;
}

// NORMAL
void __fastcall normal_L2(FixedMAP<float,IVisual*>::TNode *N)
{
	IVisual *V = N->val;
	V->Render(calcLOD(N->key,V->vis.sphere.R));
}

void __fastcall mapNormal_Render	(mapNormalItems& N)
{
	// *** DIRECT ***
	{
		// DIRECT:SORTED
		N.sorted.traverseLR		(normal_L2);
		N.sorted.clear			();

		// DIRECT:UNSORTED
		vector<IVisual*>&	L			= N.unsorted;
		IVisual **I=&*L.begin(), **E = &*L.end();
		for (; I!=E; I++)
		{
			IVisual *V = *I;
			V->Render	(0);	// zero lod 'cause it is too small onscreen
		}
		L.clear	();
	}
}

// MATRIX
void __fastcall matrix_L2	(mapMatrixItem::TNode *N)
{
	IVisual *V				= N->val.pVisual;
	RCache.set_xform_world	(N->val.Matrix);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

void __fastcall matrix_L1	(mapMatrix_Node *N)
{
	RCache.set_Element	(N->key);
	N->val.traverseLR			(matrix_L2);
	N->val.clear				();
}

// ALPHA
void __fastcall sorted_L1	(mapSorted_Node *N)
{
	IVisual *V = N->val.pVisual;
	RCache.set_Shader		(V->hShader);
	RCache.set_xform_world	(N->val.Matrix);
	V->Render				(calcLOD(N->key,V->vis.sphere.R));
}

IC	bool	cmp_vs				(mapNormalVS::TNode* N1, mapNormalVS::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_ps				(mapNormalPS::TNode* N1, mapNormalPS::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_cs				(mapNormalCS::TNode* N1, mapNormalCS::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_states			(mapNormalStates::TNode* N1, mapNormalStates::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_vb				(mapNormalVB::TNode* N1, mapNormalVB::TNode* N2)
{	return (N1->val.ssa > N2->val.ssa);		}

IC	bool	cmp_textures_lex2	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	else						return false;
}
IC	bool	cmp_textures_lex3	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	if ((*t1)[0] < (*t2)[0])	return true;
	if ((*t1)[0] > (*t2)[0])	return false;
	if ((*t1)[1] < (*t2)[1])	return true;
	if ((*t1)[1] > (*t2)[1])	return false;
	if ((*t1)[2] < (*t2)[2])	return true;
	else						return false;
}
IC	bool	cmp_textures_lexN	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	STextureList*	t1			= N1->key;
	STextureList*	t2			= N2->key;
	return lexicographical_compare(t1->begin(),t1->end(),t2->begin(),t2->end());
}
IC	bool	cmp_textures_ssa	(mapNormalTextures::TNode* N1, mapNormalTextures::TNode* N2)
{	
	return (N1->val.ssa > N2->val.ssa);		
}

void		sort_tlist			
(
 vector<mapNormalTextures::TNode*>& lst, 
 vector<mapNormalTextures::TNode*>& temp, 
 mapNormalTextures& textures, 
 BOOL	bSSA
 )
{
	int amount			= textures.begin()->key->size();
	if (bSSA)	
	{
		if (amount<=1)
		{
			// Just sort by SSA
			textures.getANY_P			(lst);
			std::sort					(lst.begin(), lst.end(), cmp_textures_ssa);
		} 
		else 
		{
			// Split into 2 parts
			mapNormalTextures::TNode* _it	= textures.begin	();
			mapNormalTextures::TNode* _end	= textures.end		();
			for (; _it!=_end; _it++)	{
				if (_it->val.ssa > r_ssaHZBvsTEX)	lst.push_back	(_it);
				else								temp.push_back	(_it);
			}

			// 1st - part - SSA, 2nd - lexicographically
			std::sort					(lst.begin(),	lst.end(),	cmp_textures_ssa);
			if (2==amount)				std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex2);
			else if (3==amount)			std::sort	(temp.begin(),	temp.end(),	cmp_textures_lex3);
			else						std::sort	(temp.begin(),	temp.end(),	cmp_textures_lexN);

			// merge lists
			lst.insert					(lst.end(),temp.begin(),temp.end());
		}
	}
	else 
	{
		textures.getANY_P			(lst);
		if (2==amount)				std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex2);
		else if (3==amount)			std::sort	(lst.begin(),	lst.end(),	cmp_textures_lex3);
		else						std::sort	(lst.begin(),	lst.end(),	cmp_textures_lexN);
	}
}

void CRender::Render	()
{
	Device.Statistic.RenderDUMP.Begin();

	// Target.set_gray					(.5f+sinf(Device.fTimeGlobal)/2.f);
	Target.phase_scene				();

	// HUD render
	{
		ENGINE_API extern float		psHUD_FOV;
		
		// Change projection
		Fmatrix Pold				= Device.mProject;
		Fmatrix FTold				= Device.mFullTransform;
		Device.mProject.build_projection(
			deg2rad(psHUD_FOV*Device.fFOV*Device.fASPECT), 
			Device.fASPECT, VIEWPORT_NEAR, 
			pCreator->Environment.Current.Far);
		Device.mFullTransform.mul	(Device.mProject, Device.mView);
		RCache.set_xform_project	(Device.mProject);

		// Rendering
		rmNear						();
		mapHUD.traverseLR			(sorted_L1);
		mapHUD.clear				();
		rmNormal					();

		// Restore projection
		Device.mProject				= Pold;
		Device.mFullTransform		= FTold;
		RCache.set_xform_project	(Device.mProject);
	}

	// NORMAL			*** mostly the main level
	// Perform sorting based on ScreenSpaceArea
	// Sorting by SSA and changes minimizations
	RCache.set_xform_world			(Fidentity);

	mapNormalVS&	vs			= mapNormal;
	vs.getANY_P					(lstVS);
	std::sort					(lstVS.begin(), lstVS.end(), cmp_vs);
	for (u32 vs_id=0; vs_id<lstVS.size(); vs_id++)
	{
		mapNormalVS::TNode*	Nvs			= lstVS[vs_id];
		RCache.set_VS					(Nvs->key);	

		mapNormalPS&		ps			= Nvs->val;		ps.ssa	= 0;
		ps.getANY_P						(lstPS);
		std::sort						(lstPS.begin(), lstPS.end(), cmp_ps);
		for (u32 ps_id=0; ps_id<lstPS.size(); ps_id++)
		{
			mapNormalPS::TNode*	Nps			= lstPS[ps_id];
			RCache.set_PS					(Nps->key);	

			mapNormalCS&		cs			= Nps->val;		cs.ssa	= 0;
			cs.getANY_P						(lstCS);
			std::sort						(lstCS.begin(), lstCS.end(), cmp_cs);
			for (u32 cs_id=0; cs_id<lstCS.size(); cs_id++)
			{
				mapNormalCS::TNode*	Ncs			= lstCS[cs_id];
				RCache.set_xform_world			(Fidentity);
				RCache.set_Constants			(Ncs->key);

				mapNormalStates&	states		= Ncs->val;		states.ssa	= 0;
				states.getANY_P					(lstStates);
				std::sort						(lstStates.begin(), lstStates.end(), cmp_states);
				for (u32 state_id=0; state_id<lstStates.size(); state_id++)
				{
					mapNormalStates::TNode*	Nstate		= lstStates[state_id];
					RCache.set_States					(Nstate->key);

					mapNormalTextures&		tex			= Nstate->val;	tex.ssa =	0;
					sort_tlist							(lstTextures,lstTexturesTemp,tex,true);
					for (u32 tex_id=0; tex_id<lstTextures.size(); tex_id++)
					{
						mapNormalTextures::TNode*	Ntex		= lstTextures[tex_id];
						RCache.set_Textures						(Ntex->key);

						mapNormalVB&				vb			= Ntex->val;	vb.ssa	=	0;
						vb.getANY_P								(lstVB);
						std::sort								(lstVB.begin(), lstVB.end(), cmp_vb);
						for (u32 vb_id=0; vb_id<lstVB.size(); vb_id++)
						{
							mapNormalVB::TNode*			Nvb		= lstVB[vb_id];
							// no need to setup that shit - visual defined

							mapNormalItems&				items	= Nvb->val;		items.ssa	= 0;
							mapNormal_Render						(items);
						}
						lstVB.clear				();
						vb.clear				();
					}
					lstTextures.clear		();
					lstTexturesTemp.clear	();
					tex.clear				();
				}
				lstStates.clear			();
				states.clear			();
			}
			lstCS.clear				();
			cs.clear				();
		}
		lstPS.clear				();
		ps.clear				();
	}
	lstVS.clear				();
	vs.clear				();

	//-----------------------------------
	RCache.set_xform_world	(Fidentity);
	Details.Render			(Device.vCameraPosition);


	// NORMAL-matrix		*** actors and dyn. objects
	mapMatrix.traverseANY	(matrix_L1);
	mapMatrix.clear			();

	// Sorted (back to front)
	mapSorted.traverseRL	(sorted_L1);
	mapSorted.clear			();

	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Affected			();
	Device.Statistic.RenderDUMP_HUD.End		();

	// Direct-compute
	LR_Direct.compute_xf_1					();

	// pCreator->Environment.RenderFirst	();
	// pCreator->Environment.RenderLast		();
	
	// Postprocess
	Target.phase_combine					();
	
	// HUD
	Device.Statistic.RenderDUMP_HUD.Begin	();
	pCreator->pHUD->Render_Direct			();
	Device.Statistic.RenderDUMP_HUD.End		();

	Device.Statistic.RenderDUMP.End();
}
