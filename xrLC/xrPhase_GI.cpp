#include "stdafx.h"
#include "xrHemisphere.h"
#include "xrThread.h"
#include "xrSyncronize.h"

#define	GI_THREADS		4
const	u32				gi_num_photons		= 1024;
const	float			gi_optimal_range	= 10.f;
const	float			gi_reflect			= .9f;
const	float			gi_clip				= 0.01f;
//////////////////////////////////////////////////////////////////////////
xr_vector<R_Light>*		task;
xrCriticalSection		task_cs;
u32						task_it;

//////////////////////////////////////////////////////////////////////////
Fvector		GetPixel_3x3		(CDB::RESULT& rpinf)
{
	Fvector B,P,R={0,0,0};

	// Access to texture
	CDB::TRI& clT										= RCAST_Model->get_tris()[rpinf.id];
	base_Face* F										= (base_Face*) clT.dummy;
	if (0==F)											return R;
	Shader_xrLC&	SH									= F->Shader();
	if (!SH.flags.bLIGHT_CastShadow)					return R;
	if (!F->flags.bOpaque)								return R;	// don't use transparency

	b_material& M	= pBuild->materials			[F->dwMaterial];
	b_texture&	T	= pBuild->textures			[M.surfidx];
	if (0==T.pSurface)									return R;

	// barycentric coords
	// note: W,U,V order
	B.set	(1.0f - rpinf.u - rpinf.v,rpinf.u,rpinf.v);

	// calc UV
	Fvector2*	cuv = F->getTC0					();
	Fvector2	uv;
	uv.x = cuv[0].x*B.x + cuv[1].x*B.y + cuv[2].x*B.z;
	uv.y = cuv[0].y*B.x + cuv[1].y*B.y + cuv[2].y*B.z;

	for (int _y=-1; _y<=1; _y++)	
	{
		for (int _x=-1; _x<=1; _x++)	
		{
			int U = iFloor(uv.x*float(T.dwWidth) + .5f) + _x;
			int V = iFloor(uv.y*float(T.dwHeight)+ .5f) + _y;
			U %= T.dwWidth;		if (U<0) U+=T.dwWidth;
			V %= T.dwHeight;	if (V<0) V+=T.dwHeight;
			u32 pixel		= T.pSurface[V*T.dwWidth+U];
			P.set(color_get_R(pixel),color_get_G(pixel),color_get_B(pixel));
			R.mad(P,1/255.f);
		}
	}
	R.div	(9.f);
	return	R;
}

//////////////////////////////////////////////////////////////////////////
class CGI		: public CThread
{
public:
	CGI			(u32 ID)	: CThread(ID)	{	thMessages	= FALSE; }

	virtual void	Execute	()
	{
		CDB::COLLIDER		xrc;
		xrc.ray_options		(CDB::OPT_CULL|CDB::OPT_ONLYNEAREST);
		CDB::MODEL*	model	= RCAST_Model;
		CDB::TRI*	tris	= RCAST_Model->get_tris();
		Fvector*	verts	= RCAST_Model->get_verts();

		// full iteration
		for (;;)	
		{
			// get task
			R_Light				src,dst;
			task_cs.Enter		();
			if (task_it>=task->size())	{
				task_cs.Leave	();
				return;
			} else {
				src				= task->at(task_it);
				dst				= src;
				dst.type		= LT_SECONDARY;
				dst.level		++;
				task_it			++;
				thProgress		= float(task_it)/float(task->size());
			}
			task_cs.Leave		();

			// analyze
			CRandom				random;
			random.seed			(0x12071980);
			float	factor		= (src.range / gi_optimal_range);	// smaller lights get smaller amount of photons
			if (LT_SECONDARY == src.type)	factor *= .5f;			// secondary lights get half the photons
					factor		*= (src.energy / 2.f);				// 2.f is optimal energy = baseline
			int		count		= iCeil( factor * float(gi_num_photons) );
			for (int it=0; it<count; it++)	{
				Fvector	dir,idir;		float	s=1.f;
				switch	(src.type)		{
					case LT_POINT		:	dir.random_dir(random).normalize();				break;
					case LT_SECONDARY	:	
						dir.random_dir	(src.direction,PI_DIV_2,random);					//. or PI ?
						s				= src.direction.dotproduct(dir.normalize());
						break;
					default:			continue;											// continue loop
				}
				xrc.ray_query		(model,src.position,dir,src.range);
				if					(!xrc.r_count()) continue;
				CDB::RESULT *R		= xrc.r_begin	();
				CDB::TRI&	T		= tris[R->id];
				Fvector		Tv[3]	= { verts[T.verts[0]],verts[T.verts[1]],verts[T.verts[2]] };
				Fvector		TN;		TN.mknormal		(Tv[0],Tv[1],Tv[2]);
				float		dot		= TN.dotproduct	(idir.invert(dir));

				dst.position.mad		(src.position,dir,R->range);
				dst.direction.reflect	(dir,TN);
				dst.energy				= src.energy * dot * gi_reflect * (1-R->range/src.range);
				if (dst.energy < gi_clip)	continue;

				// color bleeding
				dst.diffuse.mul			(src.diffuse,GetPixel_3x3(*R));
				dst.diffuse.mul			(dst.energy);
				{
					float			_e		=	(dst.diffuse.x+dst.diffuse.y+dst.diffuse.z)/3.f;
					Fvector			_c		=	{dst.diffuse.x,dst.diffuse.y,dst.diffuse.z};
					if (_abs(_e)>EPS_S)		_c.div	(_e);
					else					{ _c.set(0,0,0); _e=0; }
					dst.diffuse				= _c;
					dst.energy				= _e;
				}
				if (dst.energy < gi_clip)	continue;

				dst.range				=	src.range * _sqrt(dst.energy / src.energy); // scale range in proportion with energy
				dst.energy				/=  float	(count);

				// submit answer
				task_cs.Enter		();
				task->push_back		(dst);
				task_cs.Leave		();
			}
		}
	}
};

// test_radios
void	CBuild::xrPhase_Radiosity	()
{
	CThreadManager			gi;
	Status					("Working...");
	task					= &(pBuild->L_static.rgb);
	task_it					= 0;
	u32	setup_old			= task->size	();
	for (int t=0; t<GI_THREADS; t++)	{
		Sleep	(111);
		gi.start(xr_new<CGI>(t));
	}
	gi.wait					();
	u32 setup_new			= task->size	();
	clMsg					("old setup [%d], new setup[%d]",setup_old,setup_new);
}
