#include "stdafx.h"
#include "searchlight.h"

#include "../LightAnimLibrary.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSearchlight::CSearchlight()
{
	light_render			= ::Render->light_create();
	light_render->set_type	(IRender_Light::SPOT);
	light_render->set_shadow(true);
	glow_render				= ::Render->glow_create();
	lanim					= 0;

}

CSearchlight::~CSearchlight()
{
	::Render->light_destroy	(light_render);
	::Render->glow_destroy	(glow_render);
}

void CSearchlight::Load(LPCSTR section)
{
	inherited::Load(section);
}

BOOL CSearchlight::net_Spawn(LPVOID DC)
{
	CSE_Abstract				*e		= (CSE_Abstract*)(DC);
	CSE_ALifeObjectSearchlight	*slight	= dynamic_cast<CSE_ALifeObjectSearchlight*>(e);
	R_ASSERT				(slight);
	
	cNameVisual_set			(slight->get_visual());
	inherited::net_Spawn	(DC);
	
	R_ASSERT				(Visual() && PKinematics(Visual()));
	
	Fcolor					clr;
	clr.set					(slight->color);			clr.a = 1.f;
	clr.mul_rgb				(slight->spot_brightness);
	fBrightness				= slight->spot_brightness;
	light_render->set_range	(slight->spot_range);
	light_render->set_color	(clr);
	light_render->set_cone	(slight->spot_cone_angle);
	light_render->set_texture(slight->spot_texture[0]?slight->spot_texture:0);
	light_render->set_active(true);

	glow_render->set_texture(slight->glow_texture[0]?slight->glow_texture:0);
	glow_render->set_color	(clr);
	glow_render->set_radius	(slight->glow_radius);
	glow_render->set_active (true);

	lanim					= LALib.FindItem(slight->animator);

	setVisible(true);
	setEnabled(true);

	return TRUE;
}

void CSearchlight::shedule_Update	(u32 dt)
{
	inherited::shedule_Update(dt);
}

void CSearchlight::UpdateCL	()
{
	inherited::UpdateCL();

	// update light source
	if (light_render->get_active()){
		// calc color animator
		if (lanim){
			int frame;
			// возвращает в формате BGR
			u32 clr			= lanim->Calculate(Device.fTimeGlobal,frame); 

			Fcolor			fclr;
			fclr.set		((float)color_get_B(clr),(float)color_get_G(clr),(float)color_get_R(clr),1.f);
			fclr.mul_rgb	(fBrightness/255.f);
			light_render->set_color(fclr);
			glow_render->set_color(fclr);
		}
	}

}

void CSearchlight::renderable_Render()
{
	inherited::renderable_Render	();

	if(getVisible()) {
		::Render->set_Transform		(&XFORM());
		::Render->add_Visual		(Visual());
	}
}

BOOL CSearchlight::UsedAI_Locations()
{
	return					(FALSE);
}
