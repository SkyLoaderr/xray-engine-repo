#pragma once

#include "render.h"

//////////////////////////////////////////////////////////////////////////
// definition ("Renderable")
class	ENGINE_API	IRenderable			{
public:
	struct 
	{
		Fmatrix							xform;
		IRender_Visual*					visual;
		IRender_ObjectSpecific*			ROS;
	}	renderable;
public:
	IRenderable()
	{
		renderable.xform.identity		();
		renderable.visual				= NULL;
		renderable.ROS					= Render->ros_create(this);
	}
	virtual ~IRenderable()
	{
		Render->model_Delete			(renderable.visual);
		Render->ros_destroy				(renderable.ROS);
	}
	virtual	void						renderable_Render	()	= 0;
};
