#ifndef _D3D_EXT_internal
#define _D3D_EXT_internal

#ifndef NO_XR_LIGHT
typedef struct _light {
public:
    u32           type;             /* Type of light source */
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
	IC	void	set(u32 ltType, float x, float y, float z) {
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
    IC	void		mul(float brightness){
	    diffuse.mul_rgb		(brightness);
    	ambient.mul_rgb		(brightness);
	    specular.mul_rgb	(brightness);
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

#endif
