#pragma once

struct UVtri : public _TCF 
{
	Face*	owner;
};

#define LT_DIRECT		0
#define LT_POINT		1
#define LT_SECONDARY	2

struct R_Light
{
    DWORD           type;				// Type of light source		
    Fcolor          diffuse;			// Diffuse color of light	
    Fvector         position;			// Position in world space	
    Fvector         direction;			// Direction in world space	
    float		    range;				// Cutoff range
	float			range2;				// ^2
    float	        attenuation0;		// Constant attenuation		
    float	        attenuation1;		// Linear attenuation		
    float	        attenuation2;		// Quadratic attenuation	
	float			energy;				// For radiosity ONLY
	
	Fvector			tri[3];

	R_Light()		{
		tri[0].set	(0,0,0);
		tri[1].set	(0,0,EPS_S);
		tri[2].set	(EPS_S,0,0);
	}
};

class CDeflector
{
public:
	vector<UVtri>	tris;
	Fvector			N;
	b_texture		lm;
	Fvector*		lm_rad;

	Fvector			Center;
	float			Radius;
	int				iArea;

	vector<R_Light>	LightsSelected;
public:
	CDeflector		();
	~CDeflector		();

	VOID	OA_SetNormal(Fvector &_N )	{ N.set(_N); N.normalize(); }
	BOOL	OA_Place	(Face *owner);
	VOID	OA_Export	();
	VOID	Capture		(CDeflector *D, int b_u, int b_v, int s_u, int s_v, BOOL bRotate);

	VOID	GetRect		(UVpoint &min, UVpoint &max);

	DWORD	GetFaceCount() { return tris.size();	};

	VOID	Prepare		();
	VOID	Light		(float P_Base);
	VOID	L_Direct	(float P_Base);
	VOID	L_Radiosity	(float P_Base);
	VOID	Save		();

	WORD	GetBaseMaterial() { return tris.front().owner->dwMaterial;	}
};

typedef vector<UVtri>::iterator UVIt;
typedef vector<UVtri*>	vecPT;
typedef vecPT::iterator	vecPTIT;

const int	i_size	= 2;
const float f_size	= 4.f/512.f;
const int	h_size	= 512/i_size;

IC int	slot(float t)
{	return iFloor(t/f_size);	}

IC float Lrnd()
{
	return g_params.m_lm_dither*2.f*float(rand())/float(RAND_MAX) + 
		(1.f - g_params.m_lm_dither);
}

extern vecPT	HASH[h_size][h_size];

extern void		Jitter_Select(UVpoint* &Jitter, DWORD& Jcount);

#define BORDER 2
