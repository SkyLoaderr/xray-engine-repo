#pragma once

class ENGINE_API CObject;
namespace Collide 
{
	struct			tri {
		Fvector	e10; float e10s;
		Fvector	e21; float e21s;
		Fvector	e02; float e02s;
		Fvector p[3];
		Fvector N;
		float	d;
	};
	struct			elipsoid {
		Fmatrix	mL2W;		// convertion from sphere(000,1) to real space
		Fmatrix	mW2L;		// convertion from real space to sphere(000,1)
	};
	struct			ray_cache
	{
		Fvector				verts[3];
		ray_cache() 
		{
			verts[0].set(0,0,0);
			verts[1].set(0,0,0);
			verts[2].set(0,0,0);
		}
	};
	struct			rq_result 
	{
		CObject*	O;				// if NULL - static
		float		range;			// range to intersection
		int			element;		// номер кости/номер треугольника
	};
	class			ray_query
	{
	protected:
		Fvector		start;
		Fvector		dir;
		float		range;
		u32			flags;
	protected:
		DEFINE_VECTOR(rq_result,rqVec,rqIt);
		rqVec		results;
	public:
		ray_query	(const Fvector& _start, const Fvector& _dir, float _range, u32 _flags)
		{
			start	=_start;
			dir		= _dir;
			range	= _range;
			flags	= _flags;
		}
		IC const Fvector&	_start			()	{return start;}
		IC const Fvector&	_dir			()	{return dir;	}
		IC float			_range			()	{return range;}
		IC u32				_flags			()	{return flags;}
		IC BOOL				append_result	(CObject* _who, float _range, int _element)
		{
			if ((flags&CDB::OPT_ONLYNEAREST)&&!results.empty()){
				rq_result& R		= results.back();
				if (_range<R.range){
					R.O				=_who;
					R.range			=_range;
					R.element		=_element;
				}else return FALSE;
			}
			results.push_back		(rq_result());
			rq_result& rq			= results.back();
			rq.range	=_range;
			rq.element	=_element;
			rq.O		=_who;
			return TRUE;
		}
		IC int				r_count			()	{return results.size();}
		IC rq_result*		r_begin			()	{return &*results.begin();}
	};
};
