// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 11:44:58 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "fstaticrender.h"
#include "..\fhierrarhyvisual.h"
#include "..\bodyinstance.h"
#include "..\fmesh.h"
#include "..\fcached.h"
#include "..\flod.h"

extern	Shader*			shDEBUG;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene graph actual insertion and sorting ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
float ssaDISCARD		= 4.f;
float ssaDONTSORT		= 32.f;
float ssaLOD_A			= 64.f;
float ssaLOD_B			= 48.f;
float ssaHZBvsTEX		= 128.f;

float r_ssaDISCARD;
float r_ssaDONTSORT;
float r_ssaLOD_A;
float r_ssaLOD_B;

float r_ssaHZBvsTEX;

IC	float	CalcSSA(float& distSQ, Fvector& C, IVisual* V)
{
	float R	= V->vis.sphere.R;
	distSQ	= Device.vCameraPosition.distance_to_sqr(C);
	return	R*R/distSQ;
}

void CRender::InsertSG_Dynamic	(IVisual *pVisual, Fvector& Center)
{
	float distSQ;	if (CalcSSA(distSQ,Center,pVisual)<=r_ssaDISCARD)	return;

	// Select List and add to it
	ShaderElement*		sh		= L_Projector.shadowing()?pVisual->hShader->lod0:pVisual->hShader->lod1;
	if (val_bHUD)	{
		SceneGraph::mapHUD_Node* N			= mapHUD.insertInAnyWay(distSQ);
		N->val.pObject			= val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *val_pTransform;
		N->val.vCenter.set		(Center);
	} else if (sh->Flags.bStrictB2F) {
		SceneGraph::mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
		N->val.pObject			= val_pObject;
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *val_pTransform;
		N->val.vCenter.set		(Center);
		L_Shadows.add_element	(N);
	} else {
		SceneGraph::mapMatrix_Node* N		= mapMatrix.insert		(sh		);
		SceneGraph::mapMatrixItem::TNode* C	= N->val.insertInAnyWay	(distSQ	);
		C->val.pObject			= val_pObject;
		C->val.pVisual			= pVisual;
		C->val.Matrix			= *val_pTransform;
		C->val.vCenter.set		(Center);
		L_Shadows.add_element	(C);
	}
}

void CRender::InsertSG_Static(IVisual *pVisual)
{
	if (pVisual->vis.frame != Device.dwFrame) 
	{
		pVisual->vis.frame = Device.dwFrame;
		
		float distSQ;
		float SSA    = CalcSSA		(distSQ,pVisual->vis.sphere.P,pVisual);

		if (SSA<=r_ssaDISCARD)		return;

		// Select List and add to it
		ShaderElement*		sh		= ((_sqrt(distSQ)-pVisual->vis.sphere.R)<20)?pVisual->hShader->lod0:pVisual->hShader->lod1;
		if (sh->Flags.bStrictB2F) {
			SceneGraph::mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
			N->val.pVisual			= pVisual;
			N->val.Matrix			= Fidentity;
			N->val.vCenter.set		(pVisual->vis.sphere.P);
		} else {
			for (u32 pass_id=0; pass_id<sh->Passes.size(); pass_id++)
			{
				SPass&									pass	= *(sh->Passes[pass_id]);
				SceneGraph::mapNormalCodes&				codes	= mapNormal	[sh->Flags.iPriority][pass_id];
				SceneGraph::mapNormalCodes::TNode*		Ncode	= codes.insert		(pass.state);
				SceneGraph::mapNormalTextures::TNode*	Ntex	= Ncode->val.insert	(pass.T);
				SceneGraph::mapNormalVS::TNode*			Nvs		= Ntex->val.insert	(pass.vs);
				SceneGraph::mapNormalVB::TNode*			Nvb		= Nvs->val.insert	(pVisual->hGeom->vb);
				SceneGraph::mapNormalMatrices::TNode*	Nmat	= Nvb->val.insert	(pass.M);
				SceneGraph::mapNormalConstants::TNode*	Nconst	= Nmat->val.insert	(pass.C);
				SceneGraph::mapNormalItems&				item	= Nconst->val;
				if (pass_id)	{
					// No need to sort - ZB already setted up
					item.unsorted.push_back	(pVisual);
				} else {
					// Need to sort for HZB efficient use
					if (SSA>Nconst->val.ssa) {
						Nconst->val.ssa = SSA;
						if (SSA>Nmat->val.ssa) {
							Nmat->val.ssa = SSA;
							if (SSA>Nvs->val.ssa) {
								Nvs->val.ssa = SSA;
								if (SSA>Nvb->val.ssa) {
									Nvb->val.ssa = SSA;
									if (SSA>Ntex->val.ssa)	{
										Ntex->val.ssa = SSA; 
										if (SSA>Ncode->val.ssa) Ncode->val.ssa = SSA;
									}
								}
							}
						}
					}
					
					if (SSA<r_ssaDONTSORT)		item.unsorted.push_back		(pVisual);
					else						item.sorted.insertInAnyWay	(distSQ,pVisual);
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CRender::add_leafs_Dynamic(IVisual *pVisual)
{
	// Visual is 100% visible - simply add it
	vector<IVisual*>::iterator I,E;	// it may be useful for 'hierrarhy' visual

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	case MT_SKELETON:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end		();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	default:
		{
			// General type of visual
			// Calculate distance to it's center
			Fvector		Tpos;
			val_pTransform->transform_tiny(Tpos, pVisual->vis.sphere.P);
			InsertSG_Dynamic(pVisual,Tpos);
		}
		return;
	}
}

void CRender::add_leafs_Static(IVisual *pVisual)
{
	if (!HOM.visible(pVisual->vis))		return;

	// Visual is 100% visible - simply add it
	vector<IVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all children, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Static (*I);
		}
		return;
	case MT_SKELETON:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end	();
			for (; I!=E; I++)	add_leafs_Static	(*I);
		}
		return;
	case MT_LOD:
		{
			FLOD		* pV	= (FLOD*) pVisual;
			float		D;
			float		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			if (ssa<r_ssaLOD_A)
			{
				SceneGraph::mapLOD_Node*	N	= mapLOD.insertInAnyWay(D);
				N->val.ssa						= ssa;
				N->val.pVisual					= pVisual;
			}
			if (ssa>r_ssaLOD_B)
			{
				// Add all children, doesn't perform any tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static (*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Static(pVisual);
		}
		return;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CRender::add_Dynamic(IVisual *pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	Fvector		Tpos;	// transformed position
	EFC_Visible	VIS;

	val_pTransform->transform_tiny	(Tpos, pVisual->vis.sphere.P);
	VIS = View->testSphere			(Tpos, pVisual->vis.sphere.R,planes);
	if (fcvNone==VIS) return FALSE;

	// If we get here visual is visible or partially visible
	vector<IVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_SKELETON:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Dynamic(pVisual,Tpos);
		}
		break;
	}
	return TRUE;
}

void CRender::add_Static(IVisual *pVisual, u32 planes)
{
	// Check frustum visibility and calculate distance to visual's center
	EFC_Visible	VIS;
//	VIS = View->testSphere(pVisual->bv_Position,pVisual->bv_Radius,planes);
//	VIS = (View->testSphere_dirty(pVisual->bv_Position,pVisual->bv_Radius))?fcvFully:fcvNone;
//	VIS = View->testAABB(pVisual->bv_BBox.min,pVisual->bv_BBox.max,planes);
	vis_data&	vis			= pVisual->vis;
	VIS = View->testSAABB	(vis.sphere.P,vis.sphere.R,vis.box.min,vis.box.max,planes);
	if (fcvNone==VIS){
		return;
	}
	if (!HOM.visible(vis))	return;
	
	// If we get here visual is visible or partially visible
	vector<IVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all children
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->children.begin	();
			E = pV->children.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_SKELETON:
		{
			// Add all children, doesn't perform any tests
			CKinematics * pV	= (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->children.begin	();
			E = pV->children.end	();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,planes);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_LOD:
		{
			FLOD		* pV	= (FLOD*) pVisual;
			float		D;
			float		ssa		= CalcSSA	(D,pV->vis.sphere.P,pV);
			if (ssa<r_ssaLOD_A)	
			{
				SceneGraph::mapLOD_Node*	N	= mapLOD.insertInAnyWay(D);
				N->val.ssa						= ssa;
				N->val.pVisual					= pVisual;
			}
			if (ssa>r_ssaLOD_B)
			{
				// Add all children, perform tests
				I = pV->children.begin	();
				E = pV->children.end	();
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	default:
		{
			// General type of visual
			InsertSG_Static(pVisual);
		}
		break;
	}
}
