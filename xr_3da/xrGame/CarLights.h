#ifndef	CAR_LIGHTS_H
#define CAR_LIGHTS_H
#pragma once

class CCarLights;
class CCar;
struct SCarLight
{
	IRender_Light*			light_render;
	IRender_Glow*			glow_render;
	u16						bone_id;
	CCarLights*				m_holder;				
			SCarLight		();
			~SCarLight		();
	void	Switch			();
	bool	isOn			();
	void	Init			(CCarLights* holder);
	void	Update			();
	void	ParseDefinitions(LPCSTR section);
	

};

DEFINE_VECTOR(SCarLight*,LIGHTS_VECTOR,LIGHTS_I)
class CCarLights
{
public:
	void	ParseDefinitions	()							;
	void	Init				(CCar* pcar)				;
	void	Update				()							;
	CCar*	PCar				()							{return m_pcar;}
	void	SwitchHeadLights	()							;
			CCarLights			()							;
			~CCarLights			()							;
protected:
	LIGHTS_VECTOR	m_lights								;
	CCar*			m_pcar									;
private:
};






#endif