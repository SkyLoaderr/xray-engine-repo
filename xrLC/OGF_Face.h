#pragma once

#include "communicate.h"
#include "xruvpoint.h"
#include "progmesh.h"
#include "xrSpherical.h"

struct OGF_Texture
{
	sh_name			name;

	// for build only
	b_texture*		pSurface;
};
typedef vector<OGF_Texture>		vecOGF_T;
typedef vecOGF_T::iterator		itOGF_T;

typedef vector<WORD>			vecUnroll;
typedef vecUnroll::iterator		vecUnrollIt;

struct OGF;
struct OGF_Vertex
{
	Fvector		P;
	Fvector		N;
	DWORD		Color;
	svector<UVpoint,8>	UV;

	BOOL		similar(OGF* p, OGF_Vertex&	other);
};
typedef vector<OGF_Vertex>		vecOGF_V;
typedef vecOGF_V::iterator		itOGF_V;

#pragma pack(push,1)
struct OGF_Face
{
	WORD				v[3];

	IC void			safe_replace(int what, int to)
	{
		if (v[0]==what) v[0]=to; else if (v[0]>what) v[0]-=1;
		if (v[1]==what) v[1]=to; else if (v[1]>what) v[1]-=1;
		if (v[2]==what) v[2]=to; else if (v[2]>what) v[2]-=1;
	}
	IC bool			Degenerate()
	{	return ((v[0]==v[1]) || (v[0]==v[2]) || (v[1]==v[2])); }
	IC bool			Equal(OGF_Face& F)
	{
		// Test for 6 variations
		if ((v[0]==F.v[0]) && (v[1]==F.v[1]) && (v[2]==F.v[2])) return true;
		if ((v[0]==F.v[0]) && (v[2]==F.v[1]) && (v[1]==F.v[2])) return true;
		if ((v[2]==F.v[0]) && (v[0]==F.v[1]) && (v[1]==F.v[2])) return true;
		if ((v[2]==F.v[0]) && (v[1]==F.v[1]) && (v[0]==F.v[2])) return true;
		if ((v[1]==F.v[0]) && (v[0]==F.v[1]) && (v[2]==F.v[2])) return true;
		if ((v[1]==F.v[0]) && (v[2]==F.v[1]) && (v[0]==F.v[2])) return true;
		return false;
	}
};
typedef vector<OGF_Face>		vecOGF_F;
typedef vecOGF_F::iterator		itOGF_F;
#pragma pack(pop)

struct OGF;

struct OGF_Base
{
	int					iLevel;
	BOOL				isPatch;
	WORD				Sector;
	BOOL				bConnected;

	Fbox				bbox;
	Fvector				C;
	float				R;

	OGF_Base(int _Level) {
		bbox.invalidate	();
		iLevel			= _Level;
		bConnected		= FALSE;
		isPatch			= FALSE;
		Sector			= 0xffff;
	}

	IC BOOL				IsNode()	{ return iLevel; }

	virtual void		Save		(CFS_Base &fs) = 0;
	virtual void		GetGeometry	(vector<Fvector> &RES) = 0;
	void				CalcBounds	() 
	{
		// get geometry
		vector<Fvector>				V;
		vector<Fvector>::iterator	I;
		V.reserve					(1024);
		GetGeometry					(V);
		FPU::m64					();

		// calc first variation
		Fsphere	S1;
		S1.compute					(V.begin(),V.size());

		// calc ordinary algorithm
		Fsphere	S2;
		bbox.invalidate				();
		for (I=V.begin(); I!=V.end(); I++)	bbox.modify(*I);
		bbox.getsphere				(S2.P,S2.R);
		S2.R = -1;
		for (I=V.begin(); I!=V.end(); I++)	{
			float d = S2.P.distance_to(*I);
			if (d>S2.R) S2.R=d;
		}

		// select best one
		if (S1.R<S2.R)			
		{
			C.set	(S1.P);
			R	=	S1.R;
		} else {
			C.set	(S2.P);
			R	=	S2.R;
		}
	}
};
extern vector<OGF_Base *>		g_tree;

struct OGF_Patch : public OGF_Base
{
	DWORD				treeID;
	vector<DetailPatch>	data;

	OGF_Patch(vector<DetailPatch>& source) : OGF_Base(0)
	{
		data = source;
		isPatch = TRUE;
	}

	virtual void		Save(CFS_Base &fs);
	virtual void		GetGeometry(vector<Fvector> &RES)
	{
		vector<DetailPatch>::iterator I;
		for (I=data.begin(); I!=data.end(); I++)
			RES.push_back(I->P);
	}
};

struct OGF : public OGF_Base
{
	DWORD				treeID;

	SH_ShaderDef*		shader;
	vecOGF_T			textures;
	vecOGF_V			vertices, vertices_saved;
	vecOGF_F			faces,    faces_saved;

	// Progressive
	vector<Vsplit>		pmap_vsplit;
	vector<WORD>		pmap_faces;
	DWORD				dwMinVerts;
	int					I_Current;

	// for build only
	DWORD				dwRelevantUV;
	DWORD				dwRelevantUVMASK;

	OGF() : OGF_Base(0) {
		dwRelevantUV		= 0;
		dwRelevantUVMASK	= 0;
		I_Current			= -1;
	};
	WORD				_BuildVertex	(OGF_Vertex& V1);
	void				_BuildFace		(OGF_Vertex& V1, OGF_Vertex& V2, OGF_Vertex& V3)
	{
		OGF_Face F;
		DWORD	VertCount = vertices.size();
		F.v[0]	= _BuildVertex(V1);
		F.v[1]	= _BuildVertex(V2);
		F.v[2]	= _BuildVertex(V3);
		if (!F.Degenerate()) {
			for (itOGF_F I=faces.begin(); I!=faces.end(); I++)
				if (I->Equal(F)) return;
			faces.push_back(F);
		} else {
			if (vertices.size()>VertCount) 
				vertices.erase(vertices.begin()+VertCount,vertices.end());
		}
	}
	void				Optimize		();
	void				CreateOccluder	();
	void				MakeProgressive	();
	void				Stripify		();
	void				PerturbPMAP		();
	void				BuildVSPLIT		();
	void				DumpFaces		();

	virtual void		Save			(CFS_Base &fs);

	void				Save_Cached		(CFS_Base &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bLighting);
	void				Save_Normal_PM	(CFS_Base &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bLighting);
	void				Save_Progressive(CFS_Base &fs, ogf_header& H, DWORD FVF, BOOL bColors, BOOL bLighting);

	virtual void		GetGeometry		(vector<Fvector> &R)
	{
		for (OGF_Vertex* I=vertices.begin(); I!=vertices.end(); I++)
			R.push_back(I->P);
	}
};
struct OGF_Node : public OGF_Base
{
	vector<DWORD>		chields;

	OGF_Node(int _L, WORD _Sector) : OGF_Base(_L) { Sector=_Sector; }

	void				AddChield(DWORD ID)
	{
		chields.push_back	(ID);
		OGF_Base*			P = g_tree[ID];
		R_ASSERT			(P->Sector == Sector);
		bbox.merge			(P->bbox);
		P->bConnected		= TRUE;
	}
	virtual void		Save		(CFS_Base &fs);
	virtual void		GetGeometry	(vector<Fvector> &R)
	{
		for (vector<DWORD>::iterator I=chields.begin(); I!=chields.end(); I++)
			g_tree[*I]->GetGeometry(R);
	}
};

void set_status(char* N, int id, int f, int v);
