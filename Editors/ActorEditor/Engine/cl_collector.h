// cl_collector.h: interface for the Collector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CL_COLLECTOR_H__F107ABA0_E877_4C5A_8EFF_DAB3A86ABDB6__INCLUDED_)
#define AFX_CL_COLLECTOR_H__F107ABA0_E877_4C5A_8EFF_DAB3A86ABDB6__INCLUDED_
#pragma once

namespace CDB {
	const DWORD edge_open = 0xffffffff;

	class ENGINE_API Collector  
	{
		vector<Fvector>	verts;
		vector<TRI>		faces;

		IC DWORD		VPack(Fvector& V, float eps)
		{
			vector<Fvector>::iterator I,E;
			I=verts.begin(); E=verts.end();
			for (;I!=E;I++) if (I->similar(V,eps)) return (I-verts.begin());
			verts.push_back(V);
			return verts.size()-1;
		}
	public:
		void			add_face(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			DWORD e01, DWORD e12, DWORD e20,		// edges
			WORD material, WORD sector, DWORD dummy	// misc
			)
		{
			TRI T;
			T.IDverts()	[0] = verts.size();
			T.IDverts()	[1] = verts.size()+1;
			T.IDverts()	[2] = verts.size()+2;
			T.IDadj()	[0]	= e01;
			T.IDadj()	[1]	= e12;
			T.IDadj()	[2]	= e20;
			T.material		= material;
			T.sector		= sector;
			T.dummy			= dummy;

			verts.push_back(v0);
			verts.push_back(v1);
			verts.push_back(v2);
			faces.push_back(T);
		}
		void			add_face_packed(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			DWORD e01, DWORD e12, DWORD e20,		// edges
			WORD material, WORD sector, DWORD dummy,// misc
			float eps = EPS
			)
		{
			TRI T;
			T.IDverts()	[0] = VPack(v0,eps);
			T.IDverts()	[1] = VPack(v1,eps);
			T.IDverts()	[2] = VPack(v2,eps);
			T.IDadj()	[0]	= e01;
			T.IDadj()	[1]	= e12;
			T.IDadj()	[2]	= e20;
			T.material		= material;
			T.sector		= sector;
			T.dummy			= dummy;
			faces.push_back(T);
		}
		void			calc_adjacency	();
		
		Fvector*		getV()	{ return &*verts.begin();	}
		u32				getVS()	{ return verts.size();	}
		TRI*			getT()	{ return &*faces.begin();	}
		u32				getTS() { return faces.size();	}
	};

	const int clpMX = 28, clpMY=16, clpMZ=28;

	class ENGINE_API CollectorPacked
	{
		typedef vector<DWORD>		DWORDList;
		typedef DWORDList::iterator	DWORDIt;

		vector<Fvector>	verts;
		vector<TRI>		faces;

		Fvector			VMmin, VMscale;
		DWORDList		VM[clpMX+1][clpMY+1][clpMZ+1];
		Fvector			VMeps;

		DWORD			VPack(Fvector& V);
	public:
		CollectorPacked	(const Fbox &bb, int apx_vertices=5000, int apx_faces=5000);
		
		void			add_face(
			Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
			DWORD e01, DWORD e12, DWORD e20,		// edges
			WORD material, WORD sector, DWORD dummy	// misc
			)
		{
			TRI T;
			T.IDverts()	[0] = VPack(v0);
			T.IDverts()	[1] = VPack(v1);
			T.IDverts()	[2] = VPack(v2);
			T.IDadj()	[0]	= e01;
			T.IDadj()	[1]	= e12;
			T.IDadj()	[2]	= e20;
			T.material		= material;
			T.sector		= sector;
			T.dummy			= dummy;
			faces.push_back(T);
		}
		vector<Fvector>& getV_Vec()	{ return verts;	}
		Fvector*		getV()	{ return &*verts.begin();	}
		u32				getVS()	{ return verts.size();	}
		TRI*			getT()	{ return &*faces.begin();	}
		u32				getTS() { return faces.size();	}
	};
};

#endif // !defined(AFX_CL_COLLECTOR_H__F107ABA0_E877_4C5A_8EFF_DAB3A86ABDB6__INCLUDED_)
