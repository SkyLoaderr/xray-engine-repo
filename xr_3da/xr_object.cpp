#include "stdafx.h"
#include "xr_object.h"
#include "xr_creator.h"
#include "xr_area.h"
#include "render.h"
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
}

void CObject::cName_set			(LPCSTR N)
{ 
	xr_free					(NameObject);
	if (N)	NameObject=xr_strdup	(N); 
}
void CObject::cNameSect_set		(LPCSTR N)
{ 
	xr_free					(NameSection);
	if (N) NameSection=xr_strdup	(N); 
}
void CObject::cNameVisual_set	(LPCSTR N)
{ 
	// check if equal
	if (N && NameVisual)
		if (0==stricmp(N,NameVisual))	return;

	// replace model
	xr_free					(NameVisual);
	::Render->model_Delete	(pVisual);
	if (N) 
	{
		NameVisual=xr_strdup	(N); 
		if (NameVisual[0]) pVisual	= Render->model_Create	(NameVisual);
	}
}
void CObject::setEnabled		(BOOL _enabled)
{
	FLAGS.bEnabled = _enabled?1:0;	
	if (cfModel) cfModel->Enable(_enabled); 
}
void	CObject::svCenter			(Fvector& C)	const	{ VERIFY(pVisual); svTransform.transform_tiny(C,pVisual->vis.sphere.P);	}
void	CObject::clCenter			(Fvector& C)	const	{ VERIFY(pVisual); clTransform.transform_tiny(C,pVisual->vis.sphere.P);	}
float	CObject::Radius				()				const	{ VERIFY(pVisual); return pVisual->vis.sphere.R;						}

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
	pLights						= NULL;

	FLAGS.storage				= 0;

	pSector						= 0;

	Parent						= 0;

	NameObject					= NULL;
	NameSection					= NULL;
	NameVisual					= NULL;
}

CObject::~CObject( )
{
	Sector_Move					( 0 );
	xr_delete					( pLights		);
	xr_delete					( cfModel		);

	cNameVisual_set				( 0 );
	cName_set					( 0 );
	cNameSect_set				( 0 );
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

	// Visual and light-track
	if (pSettings->line_exist(section,"visual")) 
		cNameVisual_set	(pSettings->r_string(section,"visual"));
	pLights						= xr_new<CLightTrack> ();
	setVisible					(false);
}

BOOL CObject::net_Spawn			(LPVOID data)
{
	Sector_Detect				();

	if (0==cfModel) 
	{
		if (pSettings->line_exist(cNameSect(),"cform")) {
			LPCSTR cf				= pSettings->r_string(cNameSect(), "cform");

			if (strcmp(cf,"skeleton")==0) cfModel	= xr_new<CCF_Skeleton> (this);
			else {
				cfModel					= xr_new<CCF_Polygonal> (this);
				((CCF_Polygonal*)(cfModel))->LoadModel(pSettings, cNameSect());
			}
			pCreator->ObjectSpace.Object_Register	(this);
			cfModel->OnMove();
		}
	}

	return TRUE;
}

void CObject::net_Destroy		()
{
	FLAGS.bDestroy				= 1;
}

// Updates
void CObject::UpdateCL	()
{
	clTransform.set(svTransform);
}

void CObject::Update	( u32 T )
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
			// Register _new_ record
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
		Sector_Detect	();

		// cfmodel 
		if (cfModel && (0==H_Parent()))		cfModel->OnMove	();
	}
}

CObject::SavedPosition CObject::ps_Element(u32 ID)
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
	IRender_Sector*	P = 0;
	
	if (pVisual)	{
		Fvector		Pos;
		pVisual->vis.box.getcenter(Pos);
		Pos.add		(vPosition);
		Pos.y		=	_max(Pos.y,vPosition.y);
		Pos.y		+=	EPS_L;
		P			=	Render->detectSector(Pos);
		if (P)		Sector_Move	(P);
	}
}

void CObject::Sector_Move	(IRender_Sector* P)
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