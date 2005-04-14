// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ThunderboltH
#define ThunderboltH
#pragma once

//refs
class ENGINE_API IRender_DetailModel;
class ENGINE_API CLAItem;

struct CThunderboltDesc
{
	// geom
	IRender_DetailModel*		l_model;
    // sound
    ref_sound					snd;
    // gradient
	struct SFlare
	{
    	float					fOpacity;
	    Fvector2				fRadius;
        shared_str				texture;
        shared_str				shader;
        ref_shader				hShader;
    	SFlare()				{ fOpacity = 0; fRadius.set(0.f,0.f);}
	};
    SFlare						m_GradientTop;
    SFlare						m_GradientCenter;
    shared_str					name;
	CLAItem*					color_anim;
public:
								CThunderboltDesc	(CInifile* pIni, LPCSTR sect);
							    ~CThunderboltDesc	();
};

//
class ENGINE_API CEffect_Thunderbolt
{
public:
private:
	DEFINE_VECTOR(CThunderboltDesc*,DescVec,DescIt);
	DescVec			  			palette;
    CThunderboltDesc*			current;

    Fmatrix				  		current_xform;
	Fvector3					current_direction;

	ref_geom			  		hGeom_model;
    // states
	enum EState
	{
        stIdle,
		stWorking
	};
	EState						state;

	ref_geom			  		hGeom_gradient;

    Fvector						lightning_center;
    float						lightning_size;
    float						lightning_phase;

    float						life_time;
    float						current_time;
    float						next_lightning_time;
	BOOL						bEnabled;

    // params
    Fvector2					p_var_alt;
    float						p_var_long;
    float						p_min_dist;
    float						p_tilt;
    float						p_second_prop;
	float						p_sky_color;
	float						p_sun_color;
private:
	BOOL						RayPick				(const Fvector& s, const Fvector& d, float& range);
    void						Bolt				(float period, float life_time);
public:                     
								CEffect_Thunderbolt	();
								~CEffect_Thunderbolt();

	void						OnFrame				(BOOL enabled, float period, float duration);
	void						Render				();
};

#endif //ThunderboltH
