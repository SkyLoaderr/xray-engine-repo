#ifndef xr_efflensflareH
#define xr_efflensflareH

class ENGINE_API CInifile;

class ENGINE_API CLensFlare
{
protected:
	BOOL			bInit;
	float			fBlend;
	u32				dwFrame;

	Fvector			vSunDir;
	Fvector			vecLight;
	Fvector			vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL			bRender;

	// variable
    Fcolor			LightColor;
	float			fGradientValue;

	ref_geom		hGeom;

	void			SetGradient	(float fMaxRadius, float fOpacity, const char* tex_name);
    void			SetSource	(float fRadius, const char* tex_name);
    void			AddFlare	(float fRadius, float fOpacity, float fPosition, const char* tex_name);
public:
	struct SFlare
	{
    	float		fOpacity;
	    float		fRadius;
    	float		fPosition;
		ref_shader	hShader;
        string128	texture;
    	SFlare()	{	fOpacity = fRadius = fPosition = 0; texture[0]=0; }
	};
    DEFINE_VECTOR	(SFlare,FlareVec,FlareIt);
    FlareVec		m_Flares;

	enum {
    	flFlare 	= (1<<0),
    	flSource	= (1<<1),
    	flGradient 	= (1<<2)
    };
	Flags32			m_Flags;
    
	// source
    SFlare			m_Source;
    
	// gradient
    SFlare			m_Gradient;
    ref_shader		CreateSourceShader	(const char* tex_name);
    ref_shader		CreateFlareShader	(const char* tex_name);

	void			OnFrame		();
public:
					CLensFlare	();
	virtual			~CLensFlare	();

	void			Load		( CInifile* pIni, LPCSTR section );
    void __fastcall	Render		( BOOL bSun, BOOL bFlares, BOOL bGradient );
	void			Update		( Fvector& sun_dir, Fcolor& color );
	void 			DDUnload	();
	void 			DDLoad		();
};

#endif // xr_efflensflareH
