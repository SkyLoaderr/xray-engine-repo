#ifndef CL_DEFS_H
#define CL_DEFS_H
#pragma once

namespace RAPID {
#pragma pack(push,4)
	struct ENGINE_API tri {
		Fvector*	verts	[3];	// 3*4 = 12b
		tri*		adj		[3];	// 3*4 = 12b	(24b)
		WORD		material;		// 2b			(26b)
		WORD		sector;			// 2b			(28b)
		DWORD		dummy;			// 4b			(32b)
		
		IC Fvector&	V(int id)		{ return *(verts[id]); }
		
		IC DWORD*	IDverts()	{ return (DWORD*)	verts;	}
		IC DWORD*	IDadj()		{ return (DWORD*)	adj;	}
		IC void		convert_I2P(Fvector* pBaseV, tri* pBaseTri)	
		{
			DWORD*	pVertsID= (DWORD*)	verts;	// as indexed form
			verts[0] = pBaseV+pVertsID[0];
			verts[1] = pBaseV+pVertsID[1];
			verts[2] = pBaseV+pVertsID[2];
			DWORD*	pAdjID	= (DWORD*)	adj;	// as indexed form (take care about open-edges)
			adj	 [0] = (0xffffffff==pAdjID[0])?0:pBaseTri+pAdjID[0];
			adj	 [1] = (0xffffffff==pAdjID[1])?0:pBaseTri+pAdjID[1];
			adj	 [2] = (0xffffffff==pAdjID[2])?0:pBaseTri+pAdjID[2];
		}
		IC void		convert_P2I(Fvector* pBaseV, tri* pBaseTri)	
		{
			DWORD*	pVertsID= (DWORD*)	verts;	// as indexed form
			pVertsID[0] = verts[0]-pBaseV;
			pVertsID[1] = verts[1]-pBaseV;
			pVertsID[2] = verts[2]-pBaseV;
			DWORD*	pAdjID	= (DWORD*)	adj;	// as indexed form (take care about open-edges)
			pAdjID	[0]	= (adj[0])?adj[0]-pBaseTri:0xffffffff;
			pAdjID	[1]	= (adj[1])?adj[1]-pBaseTri:0xffffffff;
			pAdjID	[2]	= (adj[2])?adj[2]-pBaseTri:0xffffffff;
		}
	};
	
	class ENGINE_API box
	{
	public:
		// placement in parent's space
		// box to parent space: x_m = pR*x_b + pT
		// parent to box space: x_b = pR.T()*(x_m - pT)
		Fmatrix33	pR;
		Fvector		pT;
		
		// dimensions
		Fvector		d;			// this is "radius", that is,
								// half the measure of a side length
		
		box			*P;			// points to but does not "own".
		box			*N;
		
		int			*tri_index;	// points to an array of integers, m->tri_index, which refer to m->trp
		int			num_tris;
		
		int			leaf() const { return (!P && !N); }
		float		size() const { return d.x; }
		
		int			split_recurse(int *t, int n, int level);
		int			split_recurse(int *t);               // specialized for leaf nodes
	};
	
	class ENGINE_API ray
	{
	public:
		
		// placement in parent's space
		// box to parent space: x_m = pR*x_b + pT
		// parent to box space: x_b = pR.T()*(x_m - pT)
		Fmatrix33	pR;
		Fvector		pT;
		// dimensions
		Fvector		d;      // this is "radius", that is,
							// half the measure of a side length
		Fvector		C;
		Fvector		D;
		float		range;
		
		int set_ray(const Fvector& C, const Fvector& D, float range);
	};

	class ENGINE_API bbox
	{
	public:
		Fmatrix33	pR;
		Fvector		pT;
		Fvector		d;
		void set_bbox(const Fmatrix& P, const Fvector& box_center, const Fvector& box_radius );
	};
};
#pragma pack(pop)

#endif
