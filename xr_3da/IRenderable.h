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
	IRenderable();
	virtual ~IRenderable();
	virtual	void						renderable_Render	()	= 0;
};
