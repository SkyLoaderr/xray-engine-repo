#include "stdafx.h"
#include "build.h"

#define VPUSH(a) a.x,a.y,a.z

IC float	SqrDistance2Segment(const Fvector& P, const Fvector& A, const Fvector& B)
{
	// Determine t (the length of the vector from ‘a’ to ‘p’)
	Fvector c; c.sub(P,A);
	Fvector V; V.sub(B,A); 
	
	float d = V.magnitude	();
	
	V.div	(d); 
	float t = V.dotproduct	(c);
	
	// Check to see if ‘t’ is beyond the extents of the line segment
	if (t <= 0.0f)	return P.distance_to_sqr(A);
	if (t >= d)		return P.distance_to_sqr(B);
	
	// Return the point between ‘a’ and ‘b’
	// set length of V to t. V is normalized so this is easy
	Fvector	R;	R.direct(A,V,t);
	return P.distance_to_sqr(R);
}

struct record
{
	Vertex	*E1,*E2;
	Vertex	*T;
};

vector<record>	vecJunctions;

void check(Vertex* vE1, Vertex* vE2, Vertex* vTEST)
{
	if (_sqrt(SqrDistance2Segment(vTEST->P,vE1->P,vE2->P))<0.005f)	
	{
		// sort verts
		if (vE1>vE2)	swap(vE1,vE2);

		// check for duplicated errors
		for (DWORD i=0; i<vecJunctions.size(); i++)
		{
			record&	rec = vecJunctions[i];
			if (rec.T==vTEST) return;
		}

		// register
		record	rec;
		rec.E1	= vE1;
		rec.E2	= vE2;
		rec.T	= vTEST;
		vecJunctions.push_back	(rec);

		// display
		Msg	("ERROR. edge [%3.1f,%3.1f,%3.1f]-[%3.1f,%3.1f,%3.1f], vertex [%3.1f,%3.1f,%3.1f]",
			VPUSH(vE1->P),VPUSH(vE2->P),VPUSH(vTEST->P)
			);
	}
}

void CBuild::CorrectTJunctions()
{
	Status("Processing...");
	for (DWORD I=0; I<g_faces.size(); I++)
	{
		Face* F = g_faces[I];

		// Iterate on edges
		for (DWORD e=0; e<3; e++)
		{
			Vertex			*vA,*vB;
			F->EdgeVerts	(e,&vA,&vB);

			// Iterate on 'vA'-adjacent faces
			for (DWORD f1=0; f1!=vA->adjacent.size(); f1++)
			{
				Face*	F1	= vA->adjacent[f1];

				// Iterate on it's edges
				for (DWORD e1=0; e1<3; e1++)
				{
					Vertex			*v1,*v2;
					F1->EdgeVerts	(e1,&v1,&v2);
					if (v1==vA && v2!=vB)		check(vA,vB,v2);
					else if (v2==vA && v1!=vB)	check(vA,vB,v1);
				}
			}
			// Iterate on 'vB'-adjacent faces
			for (DWORD f2=0; f2!=vB->adjacent.size(); f2++)
			{
				Face*	F2	= vB->adjacent[f2];

				// Iterate on it's edges
				for (DWORD e1=0; e1<3; e1++)
				{
					Vertex			*v1,*v2;
					F2->EdgeVerts	(e1,&v1,&v2);
					if (v1==vB && v2!=vA)		check(vA,vB,v2);
					else if (v2==vB && v1!=vA)	check(vA,vB,v1);
				}
			}
		}
		Progress(float(I)/float(g_faces.size()));
	}
	Msg("*** %d errors found.",vecJunctions.size());
}
