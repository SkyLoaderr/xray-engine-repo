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

void CObject::StatusBegin()
{
	Fvector		T;
	Fvector4	S;

	T.set	(vPosition); T.y+=(Radius()*2);
	Device.mFullTransform.transform	(S,T);

	pApp->pFont->Size	(0.07f/sqrtf(fabsf(S.w)));
	pApp->pFont->Color	(D3DCOLOR_RGBA(0,255,0,(S.z<=0)?0:255));
	pApp->pFont->OutSet	(S.x,-S.y);
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
	SectorMode					= EPM_AUTO_AI;
	vPositionPrevious.set		(flt_min,flt_min,flt_min);

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

	if (sh_Shader)				Device.Shader.Delete		(sh_Shader);
	if (pVisual)				Render.Models.Delete		(pVisual);
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
	pVisual						= Render.Models.Create(pVisualName);
	bVisible					= true;

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
	
	// shadow
	sh_Shader					= Device.Shader.Create	("shadow","shadow",false);
	sh_Size						= Radius()/2;
}

BOOL CObject::Spawn(BOOL bLocal, int server_id, Fvector4& o_pos)
{
	// XForm
	Fvector4&			P = o_pos;
	vPosition.set		(P.x,P.y,P.z);
	mRotate.setXYZ		(0,P.w,0);
	UpdateTransform		();

	// Net params
	net_Local			= bLocal;
	net_ID				= server_id;
	net_Ready			= TRUE;

	// AI-DB connectivity
	Fvector				nPos = vPosition;
	nPos.y				+= .1f;
	int node			= pCreator->AI.q_LoadSearch(nPos);
	if (node<0)			{
		Msg				("! ERROR: AI node not found. (%f,%f,%f)",nPos.x,nPos.y,nPos.z);
		R_ASSERT		(node>=0);
	}
	AI_NodeID			= DWORD(node);
	AI_Node				= pCreator->AI.Node(AI_NodeID);
	AI_Lighting			= (AI_Node?float(AI_Node->light):255);

	// Sector detection
	DetectSector		();
	return TRUE;
}

void CObject::OnDeviceDestroy	()
{
	Render.Models.Delete		(pVisual);
}
void CObject::OnDeviceCreate	()
{
	pVisual						= Render.Models.Create(pVisualName);
}

// Updates
void CObject::UpdateCL	()
{
	clTransform.set(svTransform);
}

void CObject::Update	( DWORD T )
{
	if (!vPositionPrevious.similar(vPosition,0.005f))
	{
		vPositionPrevious.set(vPosition);

		// cfmodel
		if (cfModel)	cfModel->OnMove();

		// ai space
		{
			Fvector		Pos;
			pVisual->bv_BBox.getcenter(Pos);
			Pos.add		(vPosition);
			AI_NodeID	= pCreator->AI.q_Node	(AI_NodeID,vPosition);
			AI_Node		= pCreator->AI.Node		(AI_NodeID);
		}

		// sector
		switch (SectorMode) {
		case EPM_AUTO_AI:
			if (0==AI_Node) {
				DetectSector();	// undefined sector
			} else {
				if (AI_Node->sector == 255)	DetectSector();	// undefined sector
				else	{
					CSector*	P = Render.getSector(AI_Node->sector);
					if	(P && P!=pSector) {
						if (pSector)	pSector->objectRemove	(this);
						pSector = P;
						if (pSector)	pSector->objectAdd		(this);
					}
				}
			}
			break;
		case EPM_AUTO_POLYGONAL:
			DetectSector();
			break;
		};
	}
}

void CObject::OnMoveVisible()
{
}

void CObject::DetectSector()
{
	// Detect sector
	CSector*	P = 0;
	
	VERIFY		(pVisual);
	Fvector		Pos;
	pVisual->bv_BBox.getcenter(Pos);
	Pos.add		(vPosition);
	P			= Render.detectSector(vPosition);
	
	// Update
	if (P && P!=pSector) {
		if (pSector)	pSector->objectRemove	(this);
		pSector = P;
		if (pSector)	pSector->objectAdd		(this);
	}
}
