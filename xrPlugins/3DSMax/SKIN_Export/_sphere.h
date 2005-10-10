#ifndef _F_SPHERE_H_
#define _F_SPHERE_H_

template <class T>
struct _sphere {
	_vector3<T>	P;
	T			R;
public:
	IC void		set(const _vector3<T> &_P, T _R)	{ P.set(_P); R = _R; }
	IC void		set(const _sphere<T> &S)			{ P.set(S.P); R=S.R; }
	IC void		identity()							{ P.set(0,0,0); R=1; }

	enum ERP_Result{
		rpNone			= 0,
		rpOriginInside	= 1,
		rpOriginOutside	= 2,
		fcv_forcedword = u32(-1)
	};
	// Ray-sphere intersection
	IC ERP_Result intersect(const _vector3<T>& S, const _vector3<T>& D, T& range) const	
    {
		_vector3<T> Q;	Q.sub(P,S);
	
		T R2	= R*R;
		T c2	= Q.square_magnitude	();
		T v		= Q.dotproduct			(D);
		T d		= R2 - (c2 - v*v);

		if		(d > 0.f)
		{
			T _range	= v - _sqrt(d);
			if (_range<range)	{
				range = _range;
				return (c2<R2)?rpOriginInside:rpOriginOutside;
			}
		}
		return rpNone;
	}
	IC BOOL		intersect(const _vector3<T>& S, const _vector3<T>& D) const	
	{
		_vector3<T> Q;	Q.sub(P,S);
	
		T c = Q.magnitude	();
		T v = Q.dotproduct	(D);
		T d = R*R - (c*c - v*v);
		return (d > 0);
	}
	IC BOOL		intersect(const _sphere<T>& S) const
	{	
		T SumR = R+S.R;
		return P.distance_to_sqr(S.P) < SumR*SumR;
	}
	IC BOOL		contains(const _vector3<T>& PT) const 
	{
		return P.distance_to_sqr(PT) <= (R*R+EPS_S);
	}
	
	// returns true if this wholly contains the argument sphere
	IC BOOL		contains(const _sphere<T>& S) const	
	{
		// can't contain a sphere that's bigger than me !
		const T RDiff		= R - S.R;
		if ( RDiff < 0 )	return false;

		return ( P.distance_to_sqr(S.P) <= RDiff*RDiff );
	}

	// return's volume of sphere
	IC T		volume	() const
	{
		return T( PI_MUL_4 / 3 ) * (R*R*R);
	}
};

typedef _sphere<float>	Fsphere;
typedef _sphere<double> Dsphere;

template <class T>
BOOL	_valid			(const _sphere<T>& s)		{ return _valid(s.P) && _valid(s.R);	}

void	XRCORE_API		Fsphere_compute		(Fsphere& dest, const Fvector *verts, int count);

#endif