#ifndef xr_efflensflareH
#define xr_efflensflareH

#ifndef _EDITOR
#	include "xr_collide_defs.h"
#endif

class ENGINE_API CInifile;

class ENGINE_API CLensFlareDescriptor
{
public:
	struct SFlare
	{
    	float			fOpacity;
	    float			fRadius;
    	float			fPosition;
        shared_str			texture;
        shared_str			shader;
        ref_shader		hShader;
    	SFlare()		{ fOpacity = fRadius = fPosition = 0; }
	};
    struct SSource: public SFlare
    {
    	BOOL			ignore_color;
    };
    DEFINE_VECTOR		(SFlare,FlareVec,FlareIt);
    FlareVec			m_Flares;

	enum {
    	flFlare 		= (1<<0),
    	flSource		= (1<<1),
    	flGradient 		= (1<<2)
    };
	Flags32				m_Flags;
    
	// source
    SSource				m_Source;
    
	// gradient
    SFlare				m_Gradient;

    float				m_StateBlendUpSpeed;
    float				m_StateBlendDnSpeed;
    
	void				SetGradient		(float fMaxRadius, float fOpacity, LPCSTR tex_name, LPCSTR sh_name);
    void				SetSource		(float fRadius, BOOL ign_color, LPCSTR tex_name, LPCSTR sh_name);
    void				AddFlare		(float fRadius, float fOpacity, float fPosition, LPCSTR tex_name, LPCSTR sh_name);
    ref_shader			CreateShader	(LPCSTR tex_name, LPCSTR sh_name);

	shared_str				section;
public:
    					CLensFlareDescriptor(){m_Flags.zero();section=0;m_StateBlendUpSpeed=m_StateBlendDnSpeed=0.1f;}
    void				load				(CInifile* pIni, LPCSTR section);
	void 				OnDeviceCreate	();
	void 				OnDeviceDestroy	();
};
DEFINE_VECTOR(CLensFlareDescriptor,LensFlareDescVec,LensFlareDescIt); 


class ENGINE_API CLensFlare
{
protected:
	float				fBlend;
	u32					dwFrame;

	Fvector				vSunDir;
	Fvector				vecLight;
	Fvector				vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL				bRender;

	// variable
    Fcolor				LightColor;
	float				fGradientValue;

	ref_geom			hGeom;

    LensFlareDescVec	m_Palette;
	CLensFlareDescriptor* m_Current;

    enum LFState{
        lfsNone,
        lfsIdle,
    	lfsHide,
        lfsShow,
    };
    LFState				m_State;
    float				m_StateBlend;

public:
#ifndef _EDITOR
	collide::ray_cache	m_ray_cache;
#endif
public:
						CLensFlare		();
	virtual				~CLensFlare		();

	void				OnFrame			(int id);
    void __fastcall		Render			(BOOL bSun, BOOL bFlares, BOOL bGradient);
	void 				OnDeviceCreate	();         
	void 				OnDeviceDestroy	();

    int					AppendDef		(CInifile* pIni, LPCSTR sect);
};

#endif // xr_efflensflareH
