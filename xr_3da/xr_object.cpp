#include "stdafx.h"
#include "xr_object.h"
#include "xr_creator.h"
#include "xr_area.h"
#include "render.h"
#include "xr_tokens.h"
#include "portal.h"
#include "xr_ini.h"
#include "xrLevel.h"
#include "fbasicvisual.h"
#include "lighttrack.h"

#include "x_ray.h"
#include "GameFont.h"

void CObject::StatusBegin	()
{
	Fvector		T;
	Fvector4	S;
	
	T.set	(vPosition); T.y+=(Radius()*2);
	Device.mFullTransform.transform	(S,T);
	
	pApp->pFont->Size	(0.07f/sqrtf(_abs(S.w)));
	pApp->pFont->Color	(D3DCOLOR_RGBA(0,255,0,(S.z<=0)?0:255));
	pApp->pFont->OutSet	(S.x,-S.y);
}

void CObject::cName_set			(LPCSTR N)
{ 
	_FREE					(NameObject);
	NameObject=xr_strdup	(N); 
}
void CObject::cNameSect_set		(LPCSTR N)
{ 
	_FREE					(NameSection);
	NameSection=xr_strdup	(N); 
}
void CObject::cNameVisual_set	(LPCSTR N)
{ 
	_FREE					(NameVisual);
	NameVisual=xr_strdup	(N); 
}
void CObject::setEnabled		(BOOL _enabled)
{
	FLAGS.bEnabled = _enabled?1:0;	
	if (cfModel) cfModel->Enable(_enabled); 
}
void	CObject::svCenter			(Fvector& C)	const	{ VERIFY(pVisual); svTransform.transform_tiny(C,pVisual->bv_Position);	}
void	CObject::clCenter			(Fvector& C)	const	{ VERIFY(pVisual); clTransform.transform_tiny(C,pVisual->bv_Position);	}
float	CObject::Radius				()				const	{ VERIFY(pVisual); return pVisual->bv_Radius;							}

//----------------------------------------------------------------------
// Class	: CXR_Object
// Purpose	:
//----------------------------------------------------------------------
CObject::CObject		( )
{
	// Transform
	vPosition.set				(0,0,0);
	vScale.set					(1,1,1);
	mRotate.identity			();
	svTransform.identity		();
	clTransform.identity		();

	cfModel						= NULL;
	pVisual						= NULL;

	FLAGS.storage				= 0;

	pSector						= 0;
	SectorMode					= EPM_AUTO;

	Parent						= 0;

	NameObject					= 0;
	NameSection					= 0;
	NameVisual					= NULL;

	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);
}

CObject::~CObject( )
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	OnDeviceDestroy				();
	_DELETE						( cfModel		);
	_FREE						( NameObject	);
	_FREE						( NameSection	);
	_FREE						( NameVisual	);
}

void CObject::UpdateTransform( )
{
	Fmatrix	mScale,mTranslate;

	mScale.scale			(vScale);
	mTranslate.translate	(vPosition);
	svTransform.mul_43		(mTranslate,mRotate);
	svTransform.mulB_43		(mScale);
}

void CObject::Load				(LPCSTR section )
{
	// Name
	R_ASSERT					(section);
	cName_set					(section);
	cNameSect_set				(section);
	
	// Geometry and transform
	vPosition.set				(0,0,0);
	mRotate.identity			();
	UpdateTransform				();

	// Actual loading
	R_ASSERT					( pCreator );

	// Visual
	if (pSettings->LineExists(section,"visual")) 
		pVisualName				= xr_strdup(pSettings->ReadSTRING(section,"visual"));
	
	// Collision model
	cfModel						= NULL;
	if (pSettings->LineExists(section,"cform")) {
		LPCSTR cf				= pSettings->ReadSTRING(section, "cform");
		
		if (strcmp(cf,"skeleton")==0) cfModel	= new CCF_Skeleton(this);
		else {
			cfModel					= new CCF_Polygonal(this);
			((CCF_Polygonal*)(cfModel))->LoadModel(pSettings, section);
		}
		pCreator->ObjectSpace.Object_Register	(this);
		cfModel->OnMove();
	}

	setVisible					(true);
}

BOOL CObject::net_Spawn			(LPVOID data)
{
	return FALSE;
}
void CObject::net_Destroy		()
{
	FLAGS.bDestroy				= 1;
}

void CObject::OnDeviceDestroy	()
{
	if (pVisual)								Render->model_Delete	(pVisual);
	_DELETE										(pLights);
	Sector_Move									(0);
}

void CObject::OnDeviceCreate	()
{
	// visual and shadow
	REQ_CREATE					();
	pVisual						= Render->model_Create	(pVisualName);
	pLights						= new CLightTrack;
	Sector_Detect				();
}

// Updates
void CObject::UpdateCL	()
{
	clTransform.set(svTransform);
}

void CObject::Update	( DWORD T )
{
	BOOL	bUpdate=FALSE;
	if (PositionStack.empty())
	{
		// Empty
		bUpdate							= TRUE;
		PositionStack.push_back			(SavedPosition());
		PositionStack.back().dwTime		= Device.dwTimeGlobal;
		PositionStack.back().vPosition	= vPosition;
	} else {
		if (PositionStack.back().vPosition.similar(vPosition,0.005f))
		{
			// Just update time
			PositionStack.back().dwTime	= Device.dwTimeGlobal;
		} else {
			// Register new record
			bUpdate							= TRUE;
			if (PositionStack.size()<4)		{
				PositionStack.push_back			(SavedPosition());
			} else {
				PositionStack[0]				= PositionStack[1];
				PositionStack[1]				= PositionStack[2];
				PositionStack[2]				= PositionStack[3];
			}
			PositionStack.back().dwTime		= Device.dwTimeGlobal;
			PositionStack.back().vPosition	= vPosition;
		}
	}

	if (bUpdate)
	{
		// sector
		if (SectorMode!=EPM_AT_LOAD)		Sector_Detect	();

		// cfmodel 
		if (cfModel && (0==H_Parent()))		cfModel->OnMove	();
	}
}

CObject::SavedPosition CObject::ps_Element(DWORD ID)
{
	VERIFY(ID<ps_Size());
	return PositionStack[ID];
}

void CObject::OnVisible	()
{
}

void CObject::Sector_Detect	()
{
	// Detect sector
	CSector*	P = 0;
	
	if (pVisual)	{
		Fvector		Pos;
		pVisual->bv_BBox.getcenter(Pos);
		Pos.add		(vPosition);
		Pos.y		=	_max(Pos.y,vPosition.y);
		Pos.y		+=	EPS_L;
		P			=	Render->detectSector(Pos);
		if (P)		Sector_Move	(P);
	}
}

void CObject::Sector_Move	(CSector* P)
{
	// Update
	if (P!=pSector) {
		if (pSector)	pSector->objectRemove	(this);
		pSector			= P;
		if (P)			pSector->objectAdd		(this);
	}
}

void CObject::OnActivate	()
{
	setActive	(TRUE);
}

void CObject::OnDeactivate	()
{
	setActive	(FALSE);
}

CObject* CObject::H_SetParent	(CObject* O)
{
	if (O==Parent)	return O;

	CObject* S	= Parent; 

	if (0==S)	OnH_B_Chield		();
	else		OnH_B_Independent	();
	Parent		= O; 
	if (0==S)	OnH_A_Chield		();
	else		OnH_A_Independent	();

	return		S;
}

void CObject::OnH_A_Chield		()
{
	// Become chield
	pCreator->ObjectSpace.Object_Unregister	(this);
	Sector_Move								(0);
}
void CObject::OnH_B_Chield		()
{
}

void CObject::OnH_A_Independent	()
{
	// Become independent
	pCreator->ObjectSpace.Object_Register	(this);
	Sector_Detect							();
}
void CObject::OnH_B_Independent	()
{
}