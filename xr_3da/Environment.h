// Environment.h: interface for the CEnvironment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENVIRONMENT_H__155A6D9E_2ECC_4DCC_80B2_27CD3F070B41__INCLUDED_)
#define AFX_ENVIRONMENT_H__155A6D9E_2ECC_4DCC_80B2_27CD3F070B41__INCLUDED_

class ENGINE_API CSun;
class ENGINE_API CVisual;
class ENGINE_API CInifile;
class ENGINE_API CSoundStream;

enum {
	effSunGlare		= (1ul<<0ul),
	effGlows		= (1ul<<1ul)
};

struct ENGINE_API	SEnvDef 
{
	Fcolor			Sky;
	Fcolor			Ambient;
	Fcolor			Fog;
	float			Fogness;
	float			Far;
};

class ENGINE_API CEnvironment  : public pureDeviceCreate, public pureDeviceDestroy
{
public:
	vector<CSun*>			Suns;
	vector<CSoundStream*>	Music;
	int						Music_Active;
	int						Music_Fade;
public:
	// Environments
	SEnvDef					Current;
	int						CurrentID;
	float					CurrentSpeed;
	svector<SEnvDef,32>		Palette;

	// Environment cache
	u32					c_Ambient;	
	u32					c_Fog;		
	float					c_Fogness;	
	float					c_Far;
	IC void					c_Invalidate() {
		c_Ambient	= 0xAAAAAAAA;
		c_Fog		= 0xAAAAAAAA;
		c_Fogness	= 999;
		c_Far		= .9f;
	}

	// Skydome
	CVisual*				pSkydome;
public:
					CEnvironment		();
					~CEnvironment		();


	void			Music_Play			(int id);
	void			set_EnvMode			(int id, float s) { CurrentID=id; CurrentSpeed=s; }
	void			SetGradient			(float b);
	void			Load				(CInifile *pIni, char* section);
	void			Load_Music			(CInifile* INI);

	void			OnMove				();

	void			RenderFirst			();
	void			RenderLast			();
	virtual void	OnDeviceCreate		();
	virtual void	OnDeviceDestroy		();
};

ENGINE_API extern u32		psEnvFlags;
ENGINE_API extern float		psGravity;

#endif // !defined(AFX_ENVIRONMENT_H__155A6D9E_2ECC_4DCC_80B2_27CD3F070B41__INCLUDED_)
