// FStaticRender.cpp: implementation of the FStaticRender class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_creator.h"
#include "xr_ioconsole.h"
#include "FStaticRender.h"
#include "flightscontroller.h"
#include "fhierrarhyvisual.h"
#include "xr_percentage.h"
#include "xr_effects.h"
#include "xr_interface.h"
#include "environment.h"
#include "bodyinstance.h"

_FpsController	QualityControl;
Fmatrix			*pCachedWM;

// Textures
// static Fmatrix		matTrans;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
FStaticRender::FStaticRender()
{
	// Device
	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);

	// Mobile
	mobileVisuals.clear	();
	mobileMatrix.clear	();
	mobilePosition.clear();

	// Q-Control
	QualityControl.fScaleGLOD=0.0f;

	// Visibility
	pVisibility=new FStaticVisibility();

	// Visuals
	pVisuals = new FVisualContainer();

	// Shadows
	if (psDeviceFlags & rsShadows)	pShadows = new CShadowController;
	else							pShadows = NULL;
}

void FStaticRender::Load(char *folder)
{
	pLoadFolder = strdup(folder);

	// Visibility
	pVisibility->Load(folder);
	pCreator->xrPercentage->Set(15);

	// Visuals
	pVisuals->Load();
	pCreator->xrPercentage->Set(30);

	// Load lights
	Device.Lights.Load(folder);
	pCreator->xrPercentage->Set(33);

	// Lighting
	pVisuals->PerformLighting();

	// Optimizations
	pVisuals->PerformOptimize();

	// Logging
	Log("* Rendering subsystem memory usage (Kb): ", DWORD(_msize(this))/1024);
}

_E(FStaticRender::~FStaticRender())
	Device.seqDevDestroy.Remove(this);
	Device.seqDevCreate.Remove(this);
	Device.Lights.Unload();
	_DELETE(pShadows);
	_DELETE(pVisibility);
	_DELETE(pVisuals);
	_FREE(pLoadFolder);
_end;

_E(void FStaticRender::InitSorter())
	// Build lists
	dwTexCount = Device.Texture.GetTextureCount();
	Msg("Initializing internal caching scheme for %d textures...",dwTexCount);
	VERIFY(dwTexCount<MAX_TEXTURES);
	for (int Q=0; Q<2; Q++) {
		for (DWORD K=0; K<dwTexCount; K++) {
			list[Q][K].Count=0;
		}
	}
_end;

void FStaticRender::AddDynamic(FBasicVisual *v, Fmatrix *m, Fvector *p)
{
	// Check counts
	VERIFY(mobileMatrix.size()  == mobileVisuals.size());
	VERIFY(mobileMatrix.size()  == mobilePosition.size());
	VERIFY(mobileVisuals.size() == mobilePosition.size());

	v->EnableMode(vMATRIX);
	mobileVisuals.push_back(v);
	mobileMatrix.push_back(m);
	mobilePosition.push_back(p);
}

_E(FBasicVisual* FStaticRender::RegisterDynamic(char *name, Fmatrix *m, Fvector *p, bool bDuplicate))
	// Add to container
	FBasicVisual *v = pVisuals->Get(name);
	if (bDuplicate) {
		FBasicVisual *p = v->CreateInstance();
		p->Copy(v);
		v = p;
	}
	AddDynamic(v,m,p);
	return v;
_end;

_E(void FStaticRender::UnregisterDynamic(Fmatrix *m))
//	VERIFY(mobileMatrix.Contains(m));
	int idx = -1;
	for (int i=0; i<mobileMatrix.size(); i++)
		if (mobileMatrix[i]==m) { idx = i; break; }

	if (idx<0) THROW;
	mobileVisuals.erase(mobileVisuals.begin()+idx);
	mobileMatrix.erase(mobileMatrix.begin()+idx);
	mobilePosition.erase(mobilePosition.begin() + idx);
_end;

_E(void FStaticRender::Prepare())
	float	diff;

	// ******************** Calculate FPS'es & LOD's
//	if (Device.Statistic.fFPS>QualityControl.fMaxFPS) {
//		Sleep(DWORD(float(0.5f+(1.0f/QualityControl.fMaxFPS-1.0f/Device.Statistic.fFPS)*1000.0f)) );
//	}

	diff=Device.Statistic.fFPS-QualityControl.fMinFPS;

	QualityControl.fScaleGLOD     += diff*0.0001f;
	if (QualityControl.fScaleGLOD<0.0f) QualityControl.fScaleGLOD=0.0f;
	if (QualityControl.fScaleGLOD>1.0f) QualityControl.fScaleGLOD=1.0f;

	// ******************** Geometry detail
	fGLOD = QualityControl.fGeometryLOD*2.f*QualityControl.fScaleGLOD/pCreator->pEnvironment->fViewportFar;

	// ******************** Update render states if needed

	// *
//	dwPolyCount=0;

	// * Textures
//	CHK_DX(HW.pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2));
//	CHK_DX(HW.pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR));
//	CHK_DX(HW.pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION));
//	CHK_DX(HW.pDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACENORMAL));
//	matTrans._31 += Device.fTimeDelta/4.f;
//	matTrans._32 += Device.fTimeDelta/4.f;
//	CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_TEXTURE0, matTrans.d3d()));
//	pCreator->xrEffectManager->xrGamma->Dark(10);
_end;

_E(void FStaticRender::Calculate())
	WORD *	Indices;
	float	dist;
	int		i;

	dwALCount		= 0; // ensure no transparent surfaces left
	// ***********************************************
	// ******************************** Static objects
	// ***********************************************
	// first
	int Count=pVisibility->GetSlot(Device.vCameraPosition, &Indices);
	Device.VisTester.Init();
	for (i=0; i<Count; i++)
	{
		FBasicVisual *test = pVisuals->Get(Indices[i]);
		if (test->bVisible)
			Device.VisTester.Add(test,test->vPosition,test->fRadius);
	}
	Device.VisTester.Calculate();
	// second
	for (i=0; i<Device.VisTester.Count; i++)
	{
		FBasicVisual *test = (FBasicVisual *) Device.VisTester.Object[i];
		if (Device.VisTester.Test(i,dist)) {
			AddToRender(test,NULL,&(test->vPosition),dist);
		}
	}

	// ***********************************************
	// ******************************** Mobile objects
	// ***********************************************
	// first
	// note: fullfill vis-test array - even with not visible objects
	Device.VisTester.Init();
	for (i=0; i<mobileVisuals.size(); i++)
	{
		FBasicVisual *test = mobileVisuals[i];
		Device.VisTester.Add(test,*(mobilePosition[i]),test->fRadius);
	}
	Device.VisTester.Calculate();
	// second
	for (i=0; i<mobileVisuals.size(); i++)
	{
		FBasicVisual *test = (FBasicVisual *)Device.VisTester.Object[i];
		if (test->bVisible && Device.VisTester.Test(i,dist)) {
			AddToRender(test,mobileMatrix[i],mobilePosition[i],dist);
		}
	}
_end;

_E(void FStaticRender::AddToRender(FBasicVisual *pVisual, Fmatrix *pMatrix, Fvector *pCenter, float dist )) {
	if (pVisual->Type==MT_HIERRARHY) {
		FHierrarhyVisual* pV = (FHierrarhyVisual*)pVisual;
		for (DWORD i=0; i<pV->chields.size(); i++)
			AddToRender(pV->chields[i],pMatrix,pCenter,dist);
	} if (pVisual->Type==MT_SKELETON) {
		CBodyInstance * pV = (CBodyInstance*)pVisual;
		pV->GetGeometry();
		for (DWORD i=0; i<pV->chields.size(); i++)
			AddToRender(pV->chields[i],pMatrix,pCenter,dist);
	} else {
		if (pVisual->dwRMode & vAlpha) {
			list_alpha[dwALCount].fDist = dist;
			list_alpha[dwALCount].pV	= pVisual;
			list_alpha[dwALCount].pM	= (pMatrix?pMatrix:&precalc_identity);
			list_alpha[dwALCount].pC	= pCenter;
			dwALCount++;
			VERIFY(dwALCount<MAX_ALPHAVISUALS);
		} else {
			_Texture *T;
			if (pMatrix)	T = &(list[1][pVisual->iTexture]);
			else			T = &(list[0][pVisual->iTexture]);
			T->fLod		[T->Count] = dist;
			T->pVisual	[T->Count] = pVisual; //->dwIndexInContainer;
			T->pMatrix	[T->Count] = pMatrix;
			T->pCenter	[T->Count] = pCenter;
			T->Count++;
			VERIFY(T->Count<MAX_VISUALS);
			if (pVisual->pShadowLink) {
				VERIFY(pShadows);
				pShadows->Add( (FShadowForm *) pVisual->pShadowLink, pMatrix );
			}
		}
	}
} _end;

__forceinline void SetTransform(Fmatrix *pM)
{
	VERIFY(pM);
	if (pCachedWM!=pM) {
		CHK_DX(HW.pDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,(D3DMATRIX*)pM));
		pCachedWM = pM;
	}
}
_E(void FStaticRender::Render())
{
	DWORD		i;
	float		fVFar	= pCreator->pEnvironment->fViewportFar;

	// Build lights visibility & perform basic initialization
	Device.Lights.BuildSelection	();
	Device.Lights.BeginStatic		();
	pCachedWM						= NULL;
	SetTransform					(&precalc_identity);
	Fmaterial						mmx;
	mmx.Set							(1,1,1);
	CHK_DX(HW.pDevice->SetMaterial	(mmx.d3d()));

	// NORMAL - mostly the main level 
	for (i=0; i<dwTexCount; i++) {
		DWORD cnt=list[0][i].Count;
		if (cnt>0) {
			Device.Texture.Set(i);
			for (DWORD j=0; j<cnt; j++) {
				_Texture *T = &(list[0][i]);
				FBasicVisual *V = T->pVisual[j];
				Device.Lights.Select(V->vPosition,V->fRadius);
				float lod = fGLOD*(fVFar - T->fLod[j]);
				if (lod>1) lod=1; else if (lod<0) lod=0;
				V->Render(lod);
			}
			list[0][i].Count=0;
		}
	}
	// NORMAL-matrix - actors and dyn. objects 
	for (i=0; i<dwTexCount; i++) {
		DWORD cnt=list[1][i].Count;
		if (cnt>0) {
			Device.Texture.Set(i);
			for (DWORD j=0; j<cnt; j++) {
				_Texture *T = &(list[1][i]);
				FBasicVisual *V = T->pVisual[j];
				SetTransform(T->pMatrix[j]);		// try to cache out matrices
				Device.Lights.SelectDynamic(*(T->pCenter[j]),V->fRadius);
				float lod = fGLOD*(fVFar - T->fLod[j]);
				if (lod>1) lod=1; else if (lod<0) lod=0;
				V->Render						(lod);
			}
			list[1][i].Count=0;
		}
	}

	// render shadows
	if (pShadows) pShadows->Render();

	// Normal effects
	pCreator->xrEffectManager->RenderNormal();

	// Alpha effects
	pCreator->xrEffectManager->RenderAlphaFirst();

	// Reset transform
	pCachedWM = NULL;	SetTransform(&precalc_identity);

	// ************************************  ALPHA (back to front)
	// sort in distance decreasing order
	std::sort(list_alpha,list_alpha+dwALCount);
	CHK_DX(HW.pDevice->SetMaterial	(mmx.d3d()));	// clear to default material
	for (i=0; i<dwALCount; i++) {
		FBasicVisual *V = list_alpha[i].pV;
		Device.Texture.Set(V->iTexture);
		SetTransform(list_alpha[i].pM);		// try to cache out matrices
		Device.Lights.SelectDynamic(*list_alpha[i].pC,V->fRadius);
		float lod = fGLOD*(fVFar - list_alpha[i].fDist);
		if (lod>1) lod=1; else if (lod<0) lod=0;
		V->Render(lod);
	}

	// Alpha effects last
	pCreator->xrEffectManager->RenderAlphaLast();

	// Lights
	Device.Lights.UnselectAll();
} _end;

_E(void FStaticRender::OnDeviceDestroy())
	_DELETE(pVisuals);
	_DELETE(pShadows);
	Device.Lights.Unload();
_end;

_E(void FStaticRender::OnDeviceCreate())
	pVisuals = new		FVisualContainer();
	pVisuals->Load		();
	Device.Lights.Load	(pLoadFolder);

	// Shadows
	if (psDeviceFlags & rsShadows)	pShadows = new CShadowController;
	else							pShadows = NULL;
_end;
