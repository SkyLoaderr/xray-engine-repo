#include "stdafx.h"
#include "ispatial.h"
#include "irenderable.h"

IRenderable::IRenderable()
{
	renderable.xform.identity			();
	renderable.visual					= NULL;
	renderable.ROS						= Render->ros_create(this);
	ISpatial*		self				= dynamic_cast<ISpatial*> (this);
	if (self)		self->spatial.type	|= STYPE_RENDERABLE;
}
IRenderable::~IRenderable()
{
	Render->model_Delete				(renderable.visual);
	Render->ros_destroy					(renderable.ROS);
	renderable.visual					= NULL;
	renderable.ROS						= NULL;
}
