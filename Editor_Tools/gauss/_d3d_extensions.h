#ifndef _D3D_EXT_internal
#define _D3D_EXT_internal

#ifndef NO_XR_LIGHT
typedef struct _light {
public:
    DWORD           type;             /* Type of light source */
    Fcolor          diffuse;          /* Diffuse color of light */
    Fcolor          specular;         /* Specular color of light */
    Fcolor          ambient;          /* Ambient color of light */
    Fvector         position;         /* Position in world space */
    Fvector         direction;        /* Direction in world space */
    float		    range;            /* Cutoff range */
    float	        falloff;          /* Falloff */
    float	        attenuation0;     /* Constant attenuation */
    float	        attenuation1;     /* Linear attenuation */
    float	        attenuation2;     /* Quadratic attenuation */
    float	        theta;            /* Inner angle of spotlight cone */
    float	        phi;              /* Outer angle of spotlight cone */
	
	IC	D3DLIGHT8	*d3d() { return (D3DLIGHT8 *)this; };
	IC	void	set(DWORD ltType, float x, float y, float z) {
		VERIFY(sizeof(_light)==sizeof(D3DLIGHT8));
		ZeroMemory( this, sizeof(_light) );
		type=ltType;
		diffuse.set(1.0f, 1.0f, 1.0f, 1.0f);
		specular.set(diffuse);
		position.set(x,y,z);
		direction.set(x,y,z);
		direction.normalize();
		range= sqrtf(flt_max);
	}
} Flight;
#endif

#ifndef NO_XR_MATERIAL
typedef struct _material {
public:
    Fcolor			diffuse;        /* Diffuse color RGBA */
    Fcolor			ambient;        /* Ambient color RGB */
    Fcolor		    specular;       /* Specular 'shininess' */
    Fcolor			emissive;       /* Emissive color RGB */
    float			power;          /* Sharpness if specular highlight */
	
	IC	D3DMATERIAL8	*d3d(void) { return (D3DMATERIAL8 *)this; };
	IC	void	set(_material& m)
	{
		CopyMemory(this,&m,sizeof(m));
	}
	IC	void	set(float r, float g, float b)
	{
		VERIFY(sizeof(_material)==sizeof(D3DMATERIAL8));
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = 1.0f;
		power	  = 0;
	}
	IC	void	set(float r, float g, float b, float a)
	{
		VERIFY(sizeof(_material)==sizeof(D3DMATERIAL8));
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = r;
		diffuse.g = ambient.g = g;
		diffuse.b = ambient.b = b;
		diffuse.a = ambient.a = a;
		power	  = 0;
	}
	IC	void	set(Fcolor &c)
	{
		VERIFY(sizeof(_material)==sizeof(D3DMATERIAL8));
		ZeroMemory( this, sizeof(_material) );
		diffuse.r = ambient.r = c.r;
		diffuse.g = ambient.g = c.g;
		diffuse.b = ambient.b = c.b;
		diffuse.a = ambient.a = c.a;
		power	  = 0;
	}
} Fmaterial;
#endif

#ifndef NO_XR_3DBUFFER
typedef struct _3dbuffer{
public:
    DWORD			dwSize;
    Fvector			vPosition;
    Fvector			vVelocity;
    DWORD			dwInsideConeAngle;
    DWORD			dwOutsideConeAngle;
    Fvector			vConeOrientation;
    int				lConeOutsideVolume;
    float			flMinDistance;
    float			flMaxDistance;
    DWORD			dwMode;
	
	IC	DS3DBUFFER *d3d(void) { return (DS3DBUFFER *)this; };
	IC	void	Init( )
	{
		dwSize				= sizeof(DS3DBUFFER);
		vPosition.set		( 0.0f, 0.0f, 0.0f );
		vVelocity.set		( 0.0f, 0.0f, 0.0f );
		vConeOrientation.set( 0.0f, 0.0f, 1.0f );
		flMinDistance		= DS3D_DEFAULTMINDISTANCE;
		flMaxDistance		= DS3D_DEFAULTMAXDISTANCE;
		dwInsideConeAngle	= DS3D_DEFAULTCONEANGLE;
		dwOutsideConeAngle	= DS3D_DEFAULTCONEANGLE;
		lConeOutsideVolume	= DS3D_DEFAULTCONEOUTSIDEVOLUME;
		dwMode				= DS3DMODE_NORMAL;
	}
	IC	void	set( _3dbuffer b )
	{
		dwSize				= b.dwSize;
		vPosition.set		( b.vPosition );
		vVelocity.set		( b.vVelocity);
		vConeOrientation.set( b.vConeOrientation );
		flMinDistance		= b.flMinDistance;
		flMaxDistance		= b.flMaxDistance;
		dwInsideConeAngle	= b.dwInsideConeAngle;
		dwOutsideConeAngle	= b.dwOutsideConeAngle;
		lConeOutsideVolume	= b.lConeOutsideVolume;
		dwMode				= b.dwMode;
	}
}F3dbuffer;
#endif

#ifndef NO_XR_3DLISTENER
typedef struct _3dlistener{
public:
    DWORD			dwSize;
    Fvector			vPosition;
    Fvector			vVelocity;
    Fvector			vOrientFront;
    Fvector			vOrientTop;
    float			flDistanceFactor;
    float			flRolloffFactor;
    float			flDopplerFactor;
	
	IC	DS3DLISTENER *d3d(void) { return (DS3DLISTENER *)this; };
	IC	void	Init( )
	{
		dwSize				= sizeof(DS3DLISTENER);
		vPosition.set		( 0.0f, 0.0f, 0.0f );
		vVelocity.set		( 0.0f, 0.0f, 0.0f );
		vOrientFront.set	( 0.0f, 0.0f, 1.0f );
		vOrientTop.set		( 0.0f, 1.0f, 0.0f );
		flDistanceFactor	= 1.0f;
		flRolloffFactor		= DS3D_DEFAULTROLLOFFFACTOR;
		flDopplerFactor		= DS3D_DEFAULTDOPPLERFACTOR;
	}
}F3dlistener;
#endif

#endif
