#ifndef	CAR_LIGHTS_H
#define CAR_LIGHTS_H
#pragma once
struct SCarLight
{
	IRender_Light*			light_render;
	IRender_Glow*			glow_render;
	u16						bone_id;
			SCarLight		();
			~SCarLight		();
	void	Switch			();
	bool	isOn			();
	void	Init			();
	void	ParseDefinitions();

};

DEFINE_VECTOR(SCarLight,LIGHTS_VECTOR,LIGHTS_I)
class CCarLights
{
public:
	void ParseDefinitions();
	void Init();
	void Update();

protected:
	LIGHTS_VECTOR	m_lights;
private:
};






#endif