#include "stdafx.h"
#include "xr_object.h"
#include "xr_creator.h"
#include "xr_area.h"
#include "fstaticrender.h"
#include "xr_tokens.h"
#include "portal.h"
#include "xr_ini.h"
#include "xrLevel.h"

#include "x_ray.h"
#include "XR_SmallFont.h"

void CObject::StatusBegin	()
{
	Fvector		T;
	Fvector4	S;
	
	T.set	(vPosition); T.y+=(Radius()*2);
	Device.mFullTransform.transform	(S,T);
	
	pApp->pFont->Size	(0.07f/sqrtf(fabsf(S.w)));
	pApp->pFont->Color	(D3DCOLOR_RGBA(0,255,0,(S.z<=0)?0:255));
	pApp->pFont->OutSet	(S.x,-S.y);
}

void CObject::cNameSET		(LPCSTR N)
{ 
	_FREE(ObjectName); 
	ObjectName=strdup(N); 
}

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
	sh_Shader					= NULL;
	pVisualName					= NULL;

	bEnabled					= true;
	bVisible					= true;

	pSector						= 0;
	SectorMode					= EPM_AUTO;

	ObjectName					= 0;
	dwMinUpdate					= 20;
	dwMaxUpdate					= 500;

	net_Ready					= FALSE;
	
	Device.seqDevDestroy.Add	(this);
	Device.seqDevCreate.Add		(this);
}

CObject::~CObject( )
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);

	OnDeviceDestroy				();
	_DELETE	( cfModel );
	_FREE	( ObjectName	);
	_FREE	( pVisualName	);
}

void CObject::UpdateTransform( )
{
	Fmatrix	mScale,mTranslate;

	mScale.scale			(vScale);
	mTranslate.translate	(vPosition);
	svTransform.mul_43		(mTranslate,mRotate);
	svTransform.mul_43		(mScale);
}

void CObject::Load				( CInifile* ini, const char *section )
{
	// Name
	R_ASSERT					(section);
	_FREE						(ObjectName);
	ObjectName					= strdup(section);

	// Geometry and transform
	Fvector dir,norm;
	vPosition					= ini->ReadVECTOR(section,"position");
	dir							= ini->ReadVECTOR(section,"direction");
	norm						= ini->ReadVECTOR(section,"normal");
	mRotate.rotation			(dir,norm);
	UpdateTransform				();

	// Actual loading
	R_ASSERT					( pCreator );

	// Visual
	pVisualName					= strdup(ini->ReadSTRING(section,"visual"));
	OnDeviceCreate				();
	
	// Collision model
	cfModel						= NULL;
	if (ini->LineExists(section,"cform")) {
		LPCSTR cf				= ini->ReadSTRING(section, "cform");
		
		if (strcmp(cf,"skeleton")==0) cfModel	= new CCF_Skeleton(this);
		else {
			cfModel					= new CCF_Polygonal(this);
			((CCF_Polygonal*)(cfModel))->LoadModel(ini, section);
		}
		pCreator->ObjectSpace.Object_Register(this);
		cfModel->OnMove();
	}

	bVisible					= true;
}

BOOL CObject::Spawn		(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags)
{
	// XForm
	vPosition.set		(o_pos);
	mRotate.setXYZ		(o_angle.x,o_angle.y,o_angle.z);
	UpdateTransform		();

	// Net params
	net_Local			= bLocal;
	net_ID				= server_id;
	net_Ready			= TRUE;
	pCreator->Objects.net_Register	(this);

	// Sector detection
	Sector_Detect		();
	return TRUE;
}

void CObject::OnDeviceDestroy	()
{
	if (pVisual)				Render.Models.Delete	(pVisual);
	if (sh_Shader)				Device.Shader.Delete	(sh_Shader);
}

void CObject::OnDeviceCreate	()
{
	// visual and shadow
	REQ_CREATE					();
	pVisual						= Render.Models.Create	(pVisualName);
	sh_Shader					= Device.Shader.Create	("effects\\shadow","fx\\shadow",false);
	sh_Size						= Radius()/2;
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
		bUpdate							= TRUE;
		PositionStack.push_back			(SavedPosition());
		PositionStack.back().dwTime		= Device.dwTimeGlobal;
		PositionStack.back().vPosition	= vPosition;
	} else {
		if (PositionStack.back().vPosition.similar(vPosition,0.005f))
		{
			PositionStack.back().dwTime	= Device.dwTimeGlobal;
		} else {
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
		if (SectorMode!=EPM_AT_LOAD)	Sector_Detect	();
	}
	// cfmodel
	if (cfModel)	cfModel->OnMove();
}

CObject::SavedPosition CObject::ps_Element(DWORD ID)
{
	VERIFY(ID<ps_Size());
	return PositionStack[ID];
}

void CObject::OnVisible	()
{
	::Render.set_Transform		(&clTransform);
	::Render.set_LightLevel		(255);
	::Render.add_leafs_Dynamic	(Visual());
}

void CObject::Sector_Detect	()
{
	// Detect sector
	CSector*	P = 0;
	
	if (pVisual)	{
		Fvector		Pos;
		pVisual->bv_BBox.getcenter(Pos);
		Pos.add		(vPosition);
		P			= Render.detectSector(vPosition);
		Sector_Move	(P);
	}
}

void CObject::Sector_Move	(CSector* P)
{
	// Update
	if (P && P!=pSector) {
		if (pSector)	pSector->objectRemove	(this);
		pSector = P;
		if (pSector)	pSector->objectAdd		(this);
	}
}

void CObject::OnActivate	()
{
	bActive = TRUE;
}

void CObject::OnDeactivate	()
{
	bActive = FALSE;
}

