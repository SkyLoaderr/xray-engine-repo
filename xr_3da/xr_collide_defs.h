#pragma once

namespace Collide {
	struct tri {
		Fvector	e10; float e10s;
		Fvector	e21; float e21s;
		Fvector	e02; float e02s;
		Fvector p[3];
		Fvector N;
		float	d;
	};
	struct elipsoid {
		Fmatrix	mL2W;		// convertion from sphere(000,1) to real space
		Fmatrix	mW2L;		// convertion from real space to sphere(000,1)
	};
	struct ray_query 
	{
		CObject*			O;				// if NULL - static
		float				range;			// range to intersection
		int					element;		// номер кости/номер треугольника
	};
	struct ray_cache
	{
		Fvector				verts[3];
		ray_cache() 
		{
			verts[0].set(0,0,0);
			verts[1].set(0,0,0);
			verts[2].set(0,0,0);
		}
	};
};
