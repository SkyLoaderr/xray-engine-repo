#ifndef __CLENSFLARE_HPP
#define __CLENSFLARE_HPP

class CInifile;

class CFlare{
public:
    float			fOpacity;
    float			fRadius;
    float			fPosition;
	Shader*			hShader;
};

class CLensFlare:
	public pureDeviceCreate, 
	public pureDeviceDestroy  
{
	BOOL			bInit;
	BOOL			bSourcePresent;
	BOOL			bFlaresPresent;
	BOOL			bGradientPresent;

	float			fBlend;

	Fvector			vSunDir;
	Fvector			vecLight;
	Fvector			vecX, vecY, vecDir, vecAxis, vecCenter;
	BOOL			bRender;
protected:

	vector<CFlare>	Flares;		// 0-Source if exists

// variable
    Fcolor			LightColor;
	float			fGradientDensity;

	CPrimitive		P;

    void			SetSource	(float fRadius, const char* tex_name);
    void			AddFlare	(float fRadius, float fOpacity, float fPosition, const char* tex_name);
public:
					CLensFlare	();
					~CLensFlare	( );

	virtual void	OnDeviceDestroy();
	virtual void	OnDeviceCreate();

	void			OnMove		( );
	void			Load		( CInifile* pIni, LPSTR section );
    void __fastcall	Render		( BOOL bSun, BOOL bFlares );
	void			Update		( Fvector& sun_dir, float view_dist, Fcolor& color );
};

#endif // __CLENSFLARE_HPP
