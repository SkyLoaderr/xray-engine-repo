#include "stdafx.h"
#include "build.h"

#define VPUSH(a) a.x,a.y,a.z
void check(Vertex* vE1, Vertex* vE2, Vertex* vTEST)
{
	Fvector	E1;	E1.sub(vTEST->P,vE1->P);
	Fvector	E2;	E2.sub(vTEST->P,vE2->P);
	float	A	= E1.magnitude();
	float	B	= E2.magnitude();
	float	C	= vE1->P.distance_to(vE2->P);
	float	X	= (A*A - B*B + C*C)/(2*C);
	float	h2	= A*A - X*X;
	if (h2<0)	return;
	if (sqrtf(h2)<0.005f)	{
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
					if (v1==vA)			check(vA,vB,v2);
					else if (v2==vA)	check(vA,vB,v1);
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
					if (v1==vB)			check(vA,vB,v2);
					else if (v2==vB)	check(vA,vB,v1);
				}
			}
		}
		Progress(float(I)/float(g_faces.size()));
	}
}
