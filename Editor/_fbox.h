#ifndef __FBOX
#define __FBOX

typedef struct _fbox {
public:
	union{
		struct{
			_vector	min;
			_vector max;
		};
		struct{
			float x1, y1, z1;
			float x2, y2, z2;
		};
	};

	__forceinline	void	set(_vector &_min, _vector &_max)	{ min.set(_min);	max.set(_max);	};
	__forceinline   void	set(_fbox &b)						{ min.set(b.min);	max.set(b.max);	};
	__forceinline   void	null( )								{ min.set(0,0,0);	max.set(0,0,0);	};

	__forceinline   void	shrink(float s)						{ min.add(s); max.sub(s); };
	__forceinline   void	grow(float s)						{ min.sub(s); max.add(s); };

	__forceinline   void	add		(_vector &p)				{ min.add(p); max.add(p); };
	__forceinline   void	offset	(_vector &p)				{ min.add(p); max.add(p); };
	__forceinline   void	add(_fbox &b, _vector &p)			{ min.add(b.min, p); max.add(b.max, p);	};

	__forceinline   BOOL	contains(float x, float y, float z)	{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2) && (z>=z1) && (z<=z2); };
	__forceinline   BOOL	contains(_vector &p)				{ return contains(p.x,p.y,p.z);	};
	__forceinline   BOOL	contains(_fbox &b)					{ return contains(b.min) && contains(b.max); };

	__forceinline   BOOL	similar(_fbox &b)					{ return min.similar(b.min) && max.similar(b.max); };

	__forceinline	void	invalidate	()						{ min.set(100000.f,100000.f,100000.f); max.set(min); max.mul(-1); }
	__forceinline	void	modify		(_vector &p)			{ min.min(p); max.max(p);	}
	__forceinline	void	merge		(_fbox &b)				{ modify(b.min); modify(b.max); };
	__forceinline	void	transform	(_matrix &m)			{ min.transform_tiny(m); max.transform_tiny(m);	}

	__forceinline	void	getsize		(_vector& R )			{ R.sub( max, min ); };

	__forceinline	void	getcenter	(_vector& R )			{
		R.x = (min.x + max.x) * 0.5f;
		R.y = (min.y + max.y) * 0.5f;
		R.z = (min.z + max.z) * 0.5f;
	};
	__forceinline	void	getsphere	(_vector &C, float &R)	{
		getcenter(C);
		R = C.distance_to(max);
	};

	// Detects if this box intersect other
	__forceinline	BOOL	intersect(_fbox& box )
	{
		if( max.x < box.min.x )	return FALSE;
		if( max.y < box.min.y )	return FALSE;
		if( max.z < box.min.z )	return FALSE;
		if( min.x > box.max.x )	return FALSE;
		if( min.y > box.max.y )	return FALSE;
		if( min.z > box.max.z )	return FALSE;
		return TRUE;
	};

	// Make's this box valid AABB
    __forceinline void sort(){
    	float tmp;
		if( min.x > max.x ) { tmp = min.x; min.x = max.x; max.x = tmp; }
		if( min.y > max.y ) { tmp = min.y; min.y = max.y; max.y = tmp; }
		if( min.z > max.z ) { tmp = min.z; min.z = max.z; max.z = tmp; } 
	};

	// Does the vector intersects box
	__forceinline BOOL Pick( _vector& start, _vector& dir ){
		float	alpha,xt,yt,zt;
		_vector rvmin,rvmax;

		rvmin.sub( min, start );
		rvmax.sub( max, start );

		if( !is_zero(dir.x) ){
			alpha = rvmin.x / dir.x;
			yt = alpha * dir.y;
			if( yt >= rvmin.y && yt <= rvmax.y ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
			alpha = rvmax.x / dir.x;
			yt = alpha * dir.y;
			if( yt >= rvmin.y && yt <= rvmax.y ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
		}

		if( !is_zero(dir.y) ){
			alpha = rvmin.y / dir.y;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
			alpha = rvmax.y / dir.y;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				zt = alpha * dir.z;
				if( zt >= rvmin.z && zt <= rvmax.z )
					return true;
			}
		}

		if( !is_zero(dir.z) ){
			alpha = rvmin.z / dir.z;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				yt = alpha * dir.y;
				if( yt >= rvmin.y && yt <= rvmax.y )
					return true;
			}
			alpha = rvmax.z / dir.z;
			xt = alpha * dir.x;
			if( xt >= rvmin.x && xt <= rvmax.x ){
				yt = alpha * dir.y;
				if( yt >= rvmin.y && yt <= rvmax.y )
					return true;
			}
		}
		return false;
	};
} Fbox;

#endif
