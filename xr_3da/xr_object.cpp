#include "stdafx.h"
#include "igame_level.h"

#include "xr_object.h"
#include "xr_area.h"
#include "render.h"
#include "xrLevel.h"
#include "fbasicvisual.h"

#include "x_ray.h"
#include "GameFont.h"

void CObject::StatusBegin	()
{
	Fvector		T;
	Fvector4	S;

	Center							(T);
	Device.mFullTransform.transform	(S,T);
}

void CObject::cName_set			(LPCSTR N)
{ 
	xr_free							(NameObject);
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
	::Render->model_Delete	(renderable.visual);
	if (N) 
	{
		NameVisual=xr_strdup	(N); 
		if (NameVisual[0]) renderable.visual	= Render->model_Create	(NameVisual);
	}
}

// flagging
void CObject::setEnabled		(BOOL _enabled)
{
	if (_enabled)
	{
		FLAGS.bEnabled							=	1;	
		if (collidable.model)	spatial.type	|=	STYPE_COLLIDEABLE;
	}
	else
	{
		FLAGS.bEnabled							=	0;
		spatial.type							&=	~STYPE_COLLIDEABLE;
	}
}

void CObject::setVisible		(BOOL _visible)
{
	if (_visible)				// Parent should control object visibility itself (??????)
	{
		FLAGS.bVisible							= 1;
		if (renderable.visual)	spatial.type	|=	STYPE_RENDERABLE;
	}
	else
	{
		FLAGS.bVisible							= 0;
		spatial.type							&=	~STYPE_RENDERABLE;
	}
}

void	CObject::Center					(Fvector& C)	const	{ VERIFY(renderable.visual); renderable.xform.transform_tiny(C,renderable.visual->vis.sphere.P);	}
float	CObject::Radius					()				const	{ VERIFY(renderable.visual); return renderable.visual->vis.sphere.R;								}
const	Fbox&	CObject::BoundingBox	()				const	{ VERIFY(renderable.visual); return renderable.visual->vis.box;								}

//----------------------------------------------------------------------
// Class	: CXR_Object
// Purpose	:
//----------------------------------------------------------------------
CObject::CObject		( )
{
	// Transform
	FLAGS.storage				= 0;

	Parent						= NULL;

	NameObject					= NULL;
	NameSection					= NULL;
	NameVisual					= NULL;
}

CObject::~CObject				( )
{
	cNameVisual_set				( 0 );
	cName_set					( 0 );
	cNameSect_set				( 0 );
}

void CObject::Load				(LPCSTR section )
{
	// Name
	R_ASSERT					(section);
	cName_set					(section);
	cNameSect_set				(section);
	
	// Visual and light-track
	if (pSettings->line_exist(section,"visual")) 
		cNameVisual_set	(pSettings->r_string(section,"visual"));
	setVisible					(false);
}

BOOL CObject::net_Spawn			(LPVOID data)
{
	if (0==collidable.model) 
	{
		if (pSettings->line_exist(cNameSect(),"cform")) {
			LPCSTR cf			= pSettings->r_string	(cNameSect(), "cform");

			if (strcmp(cf,"skeleton")==0) collidable.model	= xr_new<CCF_Skeleton>	(this);
			else {
				if (strcmp(cf,"rigid")==0)collidable.model	= xr_new<CCF_Rigid>		(this);
				else{
					collidable.model						= xr_new<CCF_Polygonal> (this);
					((CCF_Polygonal*)(collidable.model))->LoadModel(pSettings, cNameSect());
				}
			}
		}
	}
	spatial_register			();
	shedule_register			();

	return TRUE;
}

void CObject::net_Destroy		()
{
	shedule_unregister			();
	spatial_unregister			();
	FLAGS.bDestroy				= 1;
}

// Updates
void CObject::UpdateCL			()
{
}

void CObject::shedule_Update	( u32 T )
{
	BOOL	bUpdate=FALSE;
	if (PositionStack.empty())
	{
		// Empty
		bUpdate							= TRUE;
		PositionStack.push_back			(SavedPosition());
		PositionStack.back().dwTime		= Device.dwTimeGlobal;
		PositionStack.back().vPosition	= Position();
	} else {
		if (PositionStack.back().vPosition.similar(Position(),0.005f))
		{
			// Just update time
			PositionStack.back().dwTime		= Device.dwTimeGlobal;
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
			PositionStack.back().vPosition	= Position();
		}
	}

	if (bUpdate)
	{	
		spatial_move	();
	}
}

void	CObject::spatial_register()
{
	Center						(spatial.center);
	spatial.radius				= Radius();
	ISpatial::spatial_register	();
}

void	CObject::spatial_unregister()
{
	ISpatial::spatial_unregister();
}

void	CObject::spatial_move()
{
	Center						(spatial.center);
	spatial.radius				= Radius();
	ISpatial::spatial_move		();
}

CObject::SavedPosition CObject::ps_Element(u32 ID)
{
	VERIFY(ID<ps_Size());
	return PositionStack[ID];
}

void CObject::renderable_Render	()
{
}

CObject* CObject::H_SetParent	(CObject* O)
{
	if (O==Parent)	return O;

	CObject* S	= Parent; 

	if (0==S)	OnH_B_Chield		();
	else		OnH_B_Independent	();
	if (O)		spatial_unregister	();
	else		spatial_register	();
	Parent		= O;
	if (0==S)	OnH_A_Chield		();
	else		OnH_A_Independent	();

	return		S;
}

void CObject::OnH_A_Chield		()
{
}
void CObject::OnH_B_Chield		()
{
	setVisible	(false);
}
void CObject::OnH_A_Independent	()
{
	setVisible	(true);
}
void CObject::OnH_B_Independent	()
{
}