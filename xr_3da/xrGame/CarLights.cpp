#include "stdafx.h"
#include "CarLights.h"


SCarLight::SCarLight()
{
	light_render=NULL;
	glow_render=NULL;
	bone_id		=BI_NONE;
}

SCarLight::~SCarLight()
{

	if(light_render) ::Render->light_destroy	(light_render);
	if(glow_render)  ::Render->glow_destroy	(glow_render);
	light_render=NULL;
	glow_render=NULL;
	bone_id		=BI_NONE;
}

void SCarLight::Init()
{

}

void SCarLight::ParseDefinitions( )
{
/*
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	//	lanim					= 0;
	//	time2hide				= 0;

	// set bone id
	Fcolor					clr;
	clr.set					(torch->color);
	clr.mul_rgb				(torch->spot_brightness);
	fBrightness				= torch->spot_brightness;
	light_render->set_range	(torch->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(torch->spot_cone_angle);
	light_render->set_texture(torch->spot_texture[0]?torch->spot_texture:0);

	glow_render->set_texture(torch->glow_texture[0]?torch->glow_texture:0);
	glow_render->set_color	(clr);
	glow_render->set_radius	(torch->glow_radius);

	R_ASSERT				(Visual());
	lanim					= LALib.FindItem(torch->animator);
	*/
}