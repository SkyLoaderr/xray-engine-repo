#include "stdafx.h"
#include "xrHemisphere.h"
#include "xrThread.h"
#include "xrSyncronize.h"

#define	GI_THREADS		4
const	u32				gi_num_photons		= 1024;
const	float			gi_reflect			= .9f;
const	float			gi_clip				= 0.001f;
//////////////////////////////////////////////////////////////////////////
xr_vector<R_Light>*		task;
xrCriticalSection		task_cs;
u32						task_it;

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
				task_it			++;
				thProgress		= float(task_it)/float(task->size());
			}
			task_cs.Leave		();

			// analyze
			CRandom				random;
			random.seed			(0x12071980);

			for (int it=0; it<gi_num_photons; it++)	{
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
				dst.energy				/=  float	(gi_num_photons);

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
	task					= &(pBuild->L_static.rgb);
	task_it					= 0;
	for (int t=0; t<GI_THREADS; t++)	gi.start(xr_new<CGI>(t));
	gi.wait					();
}
