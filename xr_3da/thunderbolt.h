// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////

#ifndef ThunderboltH
#define ThunderboltH
#pragma once

//refs
class ENGINE_API IRender_DetailModel;

struct CThunderboltDesc{
	// geom
	IRender_DetailModel*		l_model;
	IRender_DetailModel*		g_model;
    // sound
    ref_sound					snd;
    // gradient
	struct SFlare
	{
    	float					fOpacity;
	    float					fRadius;
        ref_str					texture;
        ref_str					shader;
        ref_shader				hShader;
    	SFlare()				{ fOpacity = fRadius = 0; }
	};
    SFlare						m_Gradient;
    ref_str						name;
public:
								CThunderboltDesc	(CInifile* pIni, LPCSTR sect);
							    ~CThunderboltDesc	();
};
//
class ENGINE_API CEffect_Thunderbolt
{
	DEFINE_VECTOR(CThunderboltDesc*,DescVec,DescIt);
	DescVec			  			palette;
    CThunderboltDesc*			current;

    Fmatrix				  		current_xform;
	ref_geom			  		hGeom_model;
    // states
	enum EState
	{
        stIdle,
		stWorking
	};
	EState						state;

	ref_geom			  		hGeom_gradient;

	Fvector						vecX, vecY;
    Fvector						light_dir;

    float						life_time;
    float						current_time;
    float						next_lightning_time;
//    float						next_bolt_time;
	BOOL						bEnabled;
private:
	BOOL						RayPick				(const Fvector& s, const Fvector& d, float& range);
    void						Bolt				(float life_time);
public:                     
								CEffect_Thunderbolt	();
								~CEffect_Thunderbolt();

	void						OnFrame				(BOOL enabled, float period, float duration);
	void						Render				();
};

#endif //ThunderboltH
