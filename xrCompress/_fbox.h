#ifndef __FBOX
#define __FBOX

typedef struct _fbox {
public:
	union{
		struct{
			Fvector	min;
			Fvector max;
		};
		struct{
			float x1, y1, z1;
			float x2, y2, z2;
		};
	};

	IC	BOOL	is_valid(){return (x2>=x1)&&(y2>=y1)&&(z2>=z1);}

	IC 	void	set(const Fvector &_min, const Fvector &_max){ min.set(_min);	max.set(_max);	};
	IC	void	set(float x1, float y1, float z1, float x2, float y2, float z2){ min.set(x1,y1,z1);	max.set(x2,y2,z2);};
	IC	void	set(const _fbox &b)					{ min.set(b.min);	max.set(b.max);	};
    IC 	void	setb(const Fvector& center, const Fvector& dim){min.sub(center,dim);max.add(center,dim);}
	IC	void	null( )								{ min.set(0,0,0);	max.set(0,0,0);	};
	IC	void	identity( )							{ min.set(-0.5f,-0.5f,-0.5f);	max.set(0.5f,0.5f,0.5f);};
	
	IC	void	shrink(float s)						{ min.add(s); max.sub(s); };
	IC	void	shrink(const Fvector& s)			{ min.add(s); max.sub(s); };
	IC	void	grow(float s)						{ min.sub(s); max.add(s); };
	IC	void	grow(const Fvector& s)				{ min.sub(s); max.add(s); };
	
	IC	void	add		(const Fvector &p)			{ min.add(p); max.add(p); };
	IC	void	sub		(const Fvector &p)			{ min.sub(p); max.sub(p); };
	IC	void	offset	(const Fvector &p)			{ min.add(p); max.add(p); };
	IC	void	add(const _fbox &b, const Fvector &p){ min.add(b.min, p); max.add(b.max, p);	};
	
	IC	BOOL	contains(float x, float y, float z)	{ return (x>=x1) && (x<=x2) && (y>=y1) && (y<=y2) && (z>=z1) && (z<=z2); };
	IC	BOOL	contains(const Fvector &p)			{ return contains(p.x,p.y,p.z);	};
	IC	BOOL	contains(const _fbox &b)			{ return contains(b.min) && contains(b.max); };
	
	IC	BOOL	similar(const _fbox &b)				{ return min.similar(b.min) && max.similar(b.max); };
	
	IC	void	invalidate	()						{ min.set(flt_max,flt_max,flt_max); max.set(flt_min,flt_min,flt_min); }
	IC	void	modify		(const Fvector &p)		{ min.min(p); max.max(p);	}
	IC	void	merge		(const _fbox &b)		{ modify(b.min); modify(b.max); };
	IC	void	merge		(const _fbox &b1, const _fbox &b2) { invalidate(); merge(b1); merge(b2); }
	IC	void	xform		(const _fbox &B, const Fmatrix &m)
	{
		// The three edges transformed: you can efficiently transform an X-only vector
		// by just getting the "X" column of the matrix
		Fvector vx,vy,vz;
		vx.mul				(m.i, B.max.x-B.min.x);	
		vy.mul				(m.j, B.max.y-B.min.y);	
		vz.mul				(m.k, B.max.z-B.min.z);	
		
		// Transform the min point
		m.transform_tiny	(min,B.min);
		max.set				(min);
		
		// Take the transformed min & axes and find _new_ extents
		// Using CPU code in the right place is faster...
		if(negative(vx.x))	min.x += vx.x; else max.x += vx.x;
		if(negative(vx.y))	min.y += vx.y; else max.y += vx.y;
		if(negative(vx.z))	min.z += vx.z; else max.z += vx.z;
		if(negative(vy.x))	min.x += vy.x; else max.x += vy.x;
		if(negative(vy.y))	min.y += vy.y; else max.y += vy.y;
		if(negative(vy.z))	min.z += vy.z; else max.z += vy.z;
		if(negative(vz.x))	min.x += vz.x; else max.x += vz.x;
		if(negative(vz.y))	min.y += vz.y; else max.y += vz.y;
		if(negative(vz.z))	min.z += vz.z; else max.z += vz.z;
	}
	IC	void	xform		(const Fmatrix &m)
    {
		_fbox b;
        b.set(*this);
        xform(b,m);
    }

	IC	void	getsize		(Fvector& R )	const 	{ R.sub( max, min ); };
	IC	void	getradius	(Fvector& R )	const 	{ getsize(R); R.mul(0.5f); };
	IC	float	getradius	( )				const 	{ Fvector R; getradius(R); return R.magnitude();	};
	IC	float	getvolume	()				const	{ Fvector sz; getsize(sz); return sz.x*sz.y*sz.z;	};
	IC	void	getcenter	(Fvector& C )	const 	{
		C.x = (min.x + max.x) * 0.5f;
		C.y = (min.y + max.y) * 0.5f;
		C.z = (min.z + max.z) * 0.5f;
	};
	IC	void	get_CD		(Fvector& bc, Fvector& bd)	// center + dimensions
	{
		bd.sub	(max,min);
		bd.mul	(.5f);
		bc.add	(min,bd);
	}
	IC	void	getsphere	(Fvector &C, float &R) const {
		getcenter			(C);
		R = C.distance_to	(max);
	};
	
	// Detects if this box intersect other
	IC	BOOL	intersect(const _fbox& box )
	{
		if( max.x < box.min.x )	return FALSE;
		if( max.y < box.min.y )	return FALSE;
		if( max.z < box.min.z )	return FALSE;
		if( min.x > box.max.x )	return FALSE;
		if( min.y > box.max.y )	return FALSE;
		if( min.z > box.max.z )	return FALSE;
		return TRUE;
	};

	// Does the vector intersects box
	IC BOOL Pick( const Fvector& start, const Fvector& dir )
	{
		float	alpha,xt,yt,zt;
		Fvector rvmin,rvmax;

		rvmin.sub( min, start );
		rvmax.sub( max, start );

		if( !fis_zero(dir.x) ){
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

		if( !fis_zero(dir.y) ){
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

		if( !fis_zero(dir.z) ){
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

	IC u32& IR(float &x) { return (u32&)x; }
	IC BOOL Pick2(const Fvector& origin, const Fvector& dir, Fvector& coord)
	{
		BOOL Inside = TRUE;
		Fvector		MaxT;
		MaxT.x=MaxT.y=MaxT.z=-1.0f;
		
		// Find candidate planes.
		{
			if(origin[0] < min[0]) {
				coord[0]	= min[0];
				Inside		= FALSE;
				if(IR(dir[0]))	MaxT[0] = (min[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
			} else if(origin[0] > max[0]) {
				coord[0]	= max[0];
				Inside		= FALSE;
				if(IR(dir[0]))	MaxT[0] = (max[0] - origin[0]) / dir[0]; // Calculate T distances to candidate planes
			}
		}
		{
			if(origin[1] < min[1]) {
				coord[1]	= min[1];
				Inside		= FALSE;
				if(IR(dir[1]))	MaxT[1] = (min[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
			} else if(origin[1] > max[1]) {
				coord[1]	= max[1];
				Inside		= FALSE;
				if(IR(dir[1]))	MaxT[1] = (max[1] - origin[1]) / dir[1]; // Calculate T distances to candidate planes
			}
		}
		{
			if(origin[2] < min[2]) {
				coord[2]	= min[2];
				Inside		= FALSE;
				if(IR(dir[2]))	MaxT[2] = (min[2] - origin[2]) / dir[2]; // Calculate T distances to candidate planes
			} else if(origin[2] > max[2]) {
				coord[2]	= max[2];
				Inside		= FALSE;
				if(IR(dir[2]))	MaxT[2] = (max[2] - origin[2]) / dir[2]; // Calculate T distances to candidate planes
			}
		}
		
		// Ray origin inside bounding box
		if(Inside)
		{
			coord = origin;
			return true;
		}
		
		// Get largest of the maxT's for final choice of intersection
		u32 WhichPlane = 0;
		if(MaxT[1] > MaxT[0])			WhichPlane = 1;
		if(MaxT[2] > MaxT[WhichPlane])	WhichPlane = 2;
		
		// Check final candidate actually inside box
		if(IR(MaxT[WhichPlane])&0x80000000) return false;
		
		if  (0==WhichPlane)
		{
				// 1 & 2
				coord[1] = origin[1] + MaxT[0] * dir[1];
				if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
				coord[2] = origin[2] + MaxT[0] * dir[2];
				if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
				return true;
		}
		if (1==WhichPlane)
		{
				// 0 & 2
				coord[0] = origin[0] + MaxT[1] * dir[0];
				if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
				coord[2] = origin[2] + MaxT[1] * dir[2];
				if((coord[2] < min[2]) || (coord[2] > max[2]))	return false;
				return true;
		}
		if (2==WhichPlane)
		{
				// 0 & 1
				coord[0] = origin[0] + MaxT[2] * dir[0];
				if((coord[0] < min[0]) || (coord[0] > max[0]))	return false;
				coord[1] = origin[1] + MaxT[2] * dir[1];
				if((coord[1] < min[1]) || (coord[1] > max[1]))	return false;
				return true;
		}
		return false;
	}
	
	IC void getpoint( int index,  Fvector& result ) const 
	{
		switch( index ){
		case 0: result.set( min.x, min.y, min.z ); break;
		case 1: result.set( min.x, min.y, max.z ); break;
		case 2: result.set( max.x, min.y, max.z ); break;
		case 3: result.set( max.x, min.y, min.z ); break;
		case 4: result.set( min.x, max.y, min.z ); break;
		case 5: result.set( min.x, max.y, max.z ); break;
		case 6: result.set( max.x, max.y, max.z ); break;
		case 7: result.set( max.x, max.y, min.z ); break;
		default: result.set( 0, 0, 0 ); break; }
	};
	IC void getpoints(Fvector* result){
		result[0].set( min.x, min.y, min.z );
		result[1].set( min.x, min.y, max.z );
		result[2].set( max.x, min.y, max.z );
		result[3].set( max.x, min.y, min.z );
		result[4].set( min.x, max.y, min.z );
		result[5].set( min.x, max.y, max.z );
		result[6].set( max.x, max.y, max.z );
		result[7].set( max.x, max.y, min.z );
	};

	IC void modify(const _fbox& src, const Fmatrix& M){
		Fvector pt;
		for(int i=0; i<8; i++){
			src.getpoint(i,pt);
			M.transform_tiny(pt);
			modify(pt);
		}
	}
} Fbox;

#endif
