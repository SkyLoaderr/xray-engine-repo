#include "stdafx.h"
#include "igame_level.h"

#include "xr_object.h"
#include "xr_area.h"
#include "render.h"
#include "xrLevel.h"
#include "fbasicvisual.h"

#include "x_ray.h"
#include "GameFont.h"

void CObject::cName_set			(shared_str N)
{ 
	NameObject	=	N; 
}
void CObject::cNameSect_set		(shared_str N)
{ 
	NameSection	=	N; 
}
void CObject::cNameVisual_set	(shared_str N)
{ 
	// check if equal
	if (*N && *NameVisual)
		if (N==NameVisual)	return;

	// replace model
	if (*N && N[0]) 
	{
		::Render->model_Delete	(renderable.visual);
		NameVisual			= N;
		renderable.visual	= Render->model_Create	(*N);
	} else {
		::Render->model_Delete	(renderable.visual,TRUE);
		NameVisual			= 0;
	}
}

// flagging
void CObject::processing_activate	()
{
	VERIFY3	(255!= Props.bActiveCounter, "Invalid sequence of processing enable/disable calls: overflow",*cName());
	Props.bActiveCounter			++;
}
void CObject::processing_deactivate	()
{
	VERIFY3	(0	!= Props.bActiveCounter, "Invalid sequence of processing enable/disable calls: underflow",*cName());
	Props.bActiveCounter			--;
}

void CObject::setEnabled			(BOOL _enabled)
{
	if (_enabled)
	{
		Props.bEnabled							=	1;	
		if (collidable.model)	spatial.type	|=	STYPE_COLLIDEABLE;
	}
	else
	{
		Props.bEnabled							=	0;
		spatial.type							&=	~STYPE_COLLIDEABLE;
	}
}
void CObject::setVisible			(BOOL _visible)
{
	if (_visible)				// Parent should control object visibility itself (??????)
	{
		Props.bVisible							= 1;
		if (renderable.visual)	spatial.type	|=	STYPE_RENDERABLE;
	}
	else
	{
		Props.bVisible							= 0;
		spatial.type							&=	~STYPE_RENDERABLE;
	}
}

void	CObject::Center					(Fvector& C)	const	{ VERIFY2(renderable.visual,*cName()); renderable.xform.transform_tiny(C,renderable.visual->vis.sphere.P);	}
float	CObject::Radius					()				const	{ VERIFY2(renderable.visual,*cName()); return renderable.visual->vis.sphere.R;								}
const	Fbox&	CObject::BoundingBox	()				const	{ VERIFY2(renderable.visual,*cName()); return renderable.visual->vis.box;									}

//----------------------------------------------------------------------
// Class	: CXR_Object
// Purpose	:
//----------------------------------------------------------------------
CObject::CObject		( )
{
	// Transform
	Props.storage				= 0;

	Parent						= NULL;

	NameObject					= NULL;
	NameSection					= NULL;
	NameVisual					= NULL;

#ifdef DEBUG
	dbg_update_shedule			= u32(-1)/2;
	dbg_update_cl				= u32(-1)/2;
#endif
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
	VERIFY						(_valid(renderable.xform));

	if (0==collidable.model) 
	{
		if (pSettings->line_exist(cNameSect(),"cform")) {
			LPCSTR cf			= pSettings->r_string	(*cNameSect(), "cform");

			R_ASSERT3			(*NameVisual, "Model isn't assigned for object, but cform requisted",*cName());

			if (xr_strcmp(cf,"skeleton")==0) collidable.model	= xr_new<CCF_Skeleton>	(this);
			else {
				if (xr_strcmp(cf,"rigid")==0)collidable.model	= xr_new<CCF_Rigid>		(this);
				else{
					collidable.model						= xr_new<CCF_Polygonal> (this);
					((CCF_Polygonal*)(collidable.model))->LoadModel(pSettings, *cNameSect());
				}
			}
		}
	}
	spatial_register			();
	shedule_register			();

	// reinitialize flags
	Props.bActiveCounter		= 0;	
	processing_activate			();
	setDestroy					(false);

	return TRUE;
}

void CObject::net_Destroy		()
{
	shedule_unregister			();
	spatial_unregister			();
	setDestroy					(true);
}

// Updates
void CObject::UpdateCL			()
{
	// consistency check
#ifdef DEBUG
	VERIFY2								(_valid(renderable.xform),*cName());

	if (Device.dwFrame==dbg_update_cl)	Debug.fatal	("'UpdateCL' called twice per frame for %s",*cName());
	dbg_update_cl	= Device.dwFrame;

	if (Parent && spatial.node_ptr)		Debug.fatal	("Object %s has parent but is still registered inside spatial DB",*cName());
#endif
}

void CObject::shedule_Update	( u32 T )
{
	// consistency check
	// Msg							("-SUB-:[%x][%s] CObject::shedule_Update",dynamic_cast<void*>(this),*cName());
	ISheduled::shedule_Update	(T);

	const	float eps_R	= 0.01f;
	const	float eps_P	= 0.005f;

	//
	BOOL	bUpdate=FALSE;
	if (PositionStack.empty())
	{
		// Empty
		bUpdate							= TRUE;
		PositionStack.push_back			(SavedPosition());
		PositionStack.back().dwTime		= Device.dwTimeGlobal;
		PositionStack.back().vPosition	= Position();
	} else {
		if (PositionStack.back().vPosition.similar(Position(),eps_P))
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

	if (bUpdate)		{	
		spatial_move	();
	} else {
		if (spatial.node_ptr)	
		{	// Object registered!
			if (!fsimilar(Radius(),spatial.radius,eps_R))	spatial_move();
			else			{
				Fvector			C;
				Center			(C);
				if (!C.similar(spatial.center,eps_P))	spatial_move();
			}
			// else nothing to do :_)
		}
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

CObject::SavedPosition CObject::ps_Element(u32 ID) const
{
	VERIFY(ID<ps_Size());
	return PositionStack[ID];
}

void CObject::renderable_Render	()
{
}

CObject* CObject::H_SetParent	(CObject* new_parent)
{
	if (new_parent==Parent)	return new_parent;

	CObject* old_parent	= Parent; 
	
	VERIFY2((new_parent==0)||(old_parent==0),"Before set parent - execute H_SetParent(0)");

	// if (Parent) Parent->H_ChildRemove	(this);
	if (0==old_parent)	OnH_B_Chield		();	// before attach
	else				OnH_B_Independent	(); // before detach
	if (new_parent)		spatial_unregister	();
	else				spatial_register	();
	Parent				= new_parent;
	if (0==old_parent)	OnH_A_Chield		();	// after attach
	else				OnH_A_Independent	(); // after detach
	// if (Parent)	Parent->H_ChildAdd		(this);

	return		old_parent;
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
