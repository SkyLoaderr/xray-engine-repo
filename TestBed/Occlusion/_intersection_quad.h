#ifndef _INT_QUAD
#define _INT_QUAD

class FIntersectionQuad {
	Fplane		quad_plane;			// plane
	Fvector		p1,p2,p3,p4;		// corners
	Fvector		n1,n2,n3,n4;		// normals to sides
public:
	void		build(Fvector& v1, Fvector& v2, Fvector& v4) {
		// ~~~~~~~~~~~~~~~~~~~~~~~~ corners
		p1.set(v1);
		p2.set(v2);
		p4.set(v4);
		Fvector c;			// calc point 3
		c.add(p2,p4);
		c.div(2);
		c.sub(p1);
		p3.direct(p1, c, 2.f);
		// ~~~~~~~~~~~~~~~~~~~~~~~~ plane
		quad_plane.build(v1,v2,v4);
		// ~~~~~~~~~~~~~~~~~~~~~~~~ normals
		Fvector t;			// direction: CW
		// 1
		t.sub			(p2,p1);
		n1.crossproduct	(quad_plane.n,t);
		n1.normalize	();
		// 2
		t.sub			(p3,p2);
		n2.crossproduct	(quad_plane.n,t);
		n2.normalize	();
		// 3
		t.sub			(p4,p3);
		n3.crossproduct	(quad_plane.n,t);
		n3.normalize	();
		// 4
		t.sub			(p1,p4);
		n4.crossproduct	(quad_plane.n,t);
		n4.normalize	();
	}
	IC BOOL intersect(Fvector& a, Fvector &b)
	{
		Fvector ipoint;
		if (!quad_plane.intersect(a,b,ipoint)) return false;
		Fvector d;
		// 1
		d.sub(ipoint, p1);
		if (d.dotproduct(n1)<0) return false;

		// 2
		d.sub(ipoint, p2);
		if (d.dotproduct(n2)<0) return false;

		// 3
		d.sub(ipoint, p3);
		if (d.dotproduct(n3)<0) return false;

		// 4
		d.sub(ipoint, p4);
		if (d.dotproduct(n4)<0) return false;

		// OK!
		return true;
	}
	void	Render(void);
};

#endif
