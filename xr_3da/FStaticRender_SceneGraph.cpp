#include "stdafx.h"
#include "fstaticrender.h"
#include "fhierrarhyvisual.h"
#include "bodyinstance.h"
#include "xrShader.h"
#include "fmesh.h"
#include "fcached.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
// Scene graph actual insertion and sorting ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
float ssaLIMIT		= 5.f;
float ssaDONTSORT	= 25.f*25.f;

extern float g_fSCREEN;
IC	float	CalcSSA(float& distSQ, Fvector& C, FBasicVisual* V)
{
	float R	= V->bv_Radius;
	distSQ	= Device.vCameraPosition.distance_to_sqr(C);
	return	g_fSCREEN*R*R/distSQ;
}

void CRender::InsertSG_Dynamic(FBasicVisual *pVisual, Fvector& Center)
{
	float distSQ;	if (CalcSSA(distSQ,Center,pVisual)<=ssaLIMIT)	return;

	// Select List and add to it
	if (pVisual->hShader->shader->Flags.bStrictB2F) {
		SceneGraph::mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
		N->val.pVisual			= pVisual;
		N->val.Matrix			= *pTransform;
		N->val.vCenter.set		(Center);
		N->val.iLighting		= iLightLevel;
	} else {
		SceneGraph::mapMatrix_Node* N		= mapMatrix.insert(pVisual->hShader);
		SceneGraph::mapMatrixItem::TNode* C	= N->val.insertInAnyWay(distSQ);
		C->val.pVisual			= pVisual;
		C->val.Matrix			= *pTransform;
		C->val.vCenter.set		(Center);
		C->val.iLighting		= iLightLevel;
	}
}

void CRender::InsertSG_Static(FBasicVisual *pVisual)
{
	if (pVisual->dwFrame!= Device.dwFrame) 
	{
		pVisual->dwFrame = Device.dwFrame;
		
		float distSQ;
		float SSA    = CalcSSA(distSQ,pVisual->bv_Position,pVisual);

		if (SSA<=ssaLIMIT)	return;

		// Select List and add to it
		Shader*		sh	= pVisual->hShader;
		CXRShader*	shr = sh->shader;
		if (shr->Flags.bStrictB2F) {
			SceneGraph::mapSorted_Node* N		= mapSorted.insertInAnyWay(distSQ);
			N->val.pVisual			= pVisual;
			N->val.Matrix			= precalc_identity;
			N->val.vCenter.set		(pVisual->bv_Position);
		} else {
			SceneGraph::mapNormal_Node*	N		= mapNormal[shr->Flags.iPriority].insert(sh);
			if (!N->val.ssa_valid)	{
				N->val.ssa_valid	= TRUE;
				N->val.ssa			= SSA;
			} else if (SSA<N->val.ssa)	N->val.ssa=SSA;

			if (SSA<ssaDONTSORT)		N->val.direct.unsorted.push_back	(pVisual);
			else						N->val.direct.sorted.insertInAnyWay	(distSQ,pVisual);
		}
	}
}

void CRender::InsertSG_Cached(FCached *pVisual)
{
	if (pVisual->dwFrame!= Device.dwFrame) {
		pVisual->dwFrame = Device.dwFrame;
		
		float distSQ;
		float SSA    = CalcSSA(distSQ,pVisual->bv_Position,pVisual);

		if (SSA<=ssaLIMIT)	return;

		// Select List and add to it
		Shader*		sh	= pVisual->hShader;
		CXRShader*	shr = sh->shader;
		SceneGraph::mapNormal_Node*		N	= mapNormal[shr->Flags.iPriority].insert(sh);
		if (!N->val.ssa_valid)	{
			N->val.ssa_valid	= TRUE;
			N->val.ssa			= SSA;
		} else if (SSA<N->val.ssa)	N->val.ssa=SSA;

		if (SSA<ssaDONTSORT)			N->val.cached.unsorted.push_back	(pVisual);
		else							N->val.cached.sorted.insertInAnyWay	(distSQ,pVisual);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
void CRender::add_leafs_Dynamic(FBasicVisual *pVisual)
{
	// Visual is 100% visible - simply add it
	vector<FBasicVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all chields, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->chields.begin	();
			E = pV->chields.end		();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	case MT_SKELETON:
		{
			// Add all chields, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->chields.begin	();
			E = pV->chields.end		();
			for (; I!=E; I++)	add_leafs_Dynamic	(*I);
		}
		return;
	default:
		{
			// General type of visual
			// Calculate distance to it's center
			Fvector		Tpos;
			pTransform->transform_tiny(Tpos, pVisual->bv_Position);
			InsertSG_Dynamic(pVisual,Tpos);
		}
		return;
	}
}

void CRender::add_leafs_Static(FBasicVisual *pVisual)
{
	// Visual is 100% visible - simply add it
	vector<FBasicVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all chields, doesn't perform any tests
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->chields.begin	();
			E = pV->chields.end		();
			for (; I!=E; I++)	add_leafs_Static (*I);
		}
		return;
	case MT_SKELETON:
		{
			// Add all chields, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->chields.begin	();
			E = pV->chields.end		();
			for (; I!=E; I++)	add_leafs_Static	(*I);
		}
		return;
	case MT_CACHED:
		{
			// Cached visual
			InsertSG_Cached((FCached*)pVisual);
		}
		return;
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
BOOL CRender::add_Dynamic(FBasicVisual *pVisual, CVisiCache C)
{
	// Check frustum visibility and calculate distance to visual's center
	Fvector		Tpos;	// transformed position
	EFC_Visible	VIS;

	pTransform->transform_tiny(Tpos, pVisual->bv_Position);
	VIS = View->visibleSphere(C,Tpos,pVisual->bv_Radius);
	if (fcvNone==VIS) return FALSE;

	// If we get here visual is visible or partially visible
	vector<FBasicVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	int count = 0;
	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all chields
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->chields.begin	();
			E = pV->chields.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,C);
			} else {
				for (; I!=E; I++)	add_leafs_Dynamic	(*I);
			}
		}
		break;
	case MT_SKELETON:
		{
			// Add all chields, doesn't perform any tests
			CKinematics * pV		= (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->chields.begin	();
			E = pV->chields.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Dynamic			(*I,C);
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

void CRender::add_Static(FBasicVisual *pVisual, CVisiCache C)
{
	// Check frustum visibility and calculate distance to visual's center
	EFC_Visible	VIS;
	VIS = View->visibleVisual(C,pVisual);
	if (fcvNone==VIS) { return; }
	
	// If we get here visual is visible or partially visible
	vector<FBasicVisual*>::iterator I,E;	// it may be usefull for 'hierrarhy' visuals

	switch (pVisual->Type) {
	case MT_HIERRARHY:
		{
			// Add all chields
			FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
			I = pV->chields.begin	();
			E = pV->chields.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,C);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_SKELETON:
		{
			// Add all chields, doesn't perform any tests
			CKinematics * pV = (CKinematics*)pVisual;
			pV->Calculate			();
			I = pV->chields.begin	();
			E = pV->chields.end		();
			if (fcvPartial==VIS) {
				for (; I!=E; I++)	add_Static			(*I,C);
			} else {
				for (; I!=E; I++)	add_leafs_Static	(*I);
			}
		}
		break;
	case MT_CACHED:
		{
			// Cached visual
			InsertSG_Cached((FCached*)pVisual);
		}
		return;
	default:
		{
			// General type of visual
			InsertSG_Static(pVisual);
		}
		break;
	}
}
