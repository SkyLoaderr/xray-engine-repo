#ifndef xr_efflensflareH
#define xr_efflensflareH

class ENGINE_API CInifile;

class ENGINE_API CLensFlareDescriptor
{
public:
	struct SFlare
	{
    	float			fOpacity;
	    float			fRadius;
    	float			fPosition;
        string128		texture;
        ref_shader		hShader;
    	SFlare()		{ fOpacity = fRadius = fPosition = 0; texture[0]=0; }
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
    SFlare				m_Source;
    
	// gradient
    SFlare				m_Gradient;

	void				SetGradient		(float fMaxRadius, float fOpacity, const char* tex_name);
    void				SetSource		(float fRadius, const char* tex_name);
    void				AddFlare		(float fRadius, float fOpacity, float fPosition, const char* tex_name);
    ref_shader			CreateSourceShader	(const char* tex_name);
    ref_shader			CreateFlareShader	(const char* tex_name);

	string64			section;
public:
    					CLensFlareDescriptor(){m_Flags.zero();section[0]=0;}
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
	CLensFlareDescriptor* first_desc;
	CLensFlareDescriptor* second_desc;
    float				factor;
public:
	void				lerp			(int a, int b, float f);
public:
						CLensFlare		();
	virtual				~CLensFlare		();

    void __fastcall		Render			( BOOL bSun, BOOL bFlares, BOOL bGradient );
	void 				OnDeviceCreate	();
	void 				OnDeviceDestroy	();

    int					AppendDef		(CInifile* pIni, LPCSTR sect);
};

#endif // xr_efflensflareH
