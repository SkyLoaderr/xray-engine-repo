#pragma once

#include "communicate.h"
#include "xruvpoint.h"
#include "progmesh.h"
#include "xrSpherical.h"

struct OGF_Texture
{
	shared_str			name;
	b_texture*			pSurface;
};
typedef svector<OGF_Texture,3>	vecOGF_T;
typedef vecOGF_T::iterator		itOGF_T;

struct OGF;
struct OGF_Vertex
{
	Fvector				P;
	Fvector				N;		// normal
	base_basis			T;		// tangent
	base_basis			B;		// binormal
	base_color			Color;
	svector<Fvector2,2>	UV;

	BOOL				similar	(OGF* p, OGF_Vertex&	other);
};
typedef xr_vector<OGF_Vertex>	vecOGF_V;
typedef vecOGF_V::iterator		itOGF_V;

#pragma pack(push,1)
struct OGF_Face
{
	u16			v[3];

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
typedef xr_vector<OGF_Face>		vecOGF_F;
typedef vecOGF_F::iterator		itOGF_F;
#pragma pack(pop)

struct OGF;

struct OGF_Base
{
	int					iLevel;
	u16					Sector;
	BOOL				bConnected;

	Fbox				bbox;
	Fvector				C;
	float				R;

	OGF_Base(int _Level) {
		bbox.invalidate	();
		iLevel			= _Level;
		bConnected		= FALSE;
		Sector			= 0xffff;
	}

	IC BOOL				IsNode()	{ return iLevel; }

	virtual void		PreSave		()							{};
	virtual void		Save		(IWriter &fs);
	virtual void		GetGeometry	(xr_vector<Fvector> &RES)	= 0;
	void				CalcBounds	(); 
};
extern xr_vector<OGF_Base *>		g_tree;

struct OGF : public OGF_Base
{
	u32					material;
	vecOGF_T			textures;
	vecOGF_V			vertices, vertices_saved;
	vecOGF_F			faces,    faces_saved;

	// Progressive
	xr_vector<Vsplit>	pmap_vsplit;
	xr_vector<u16>		pmap_faces;
	u32					dwMinVerts;
	int					I_Current;

	// for build only
	u32					dwRelevantUV;
	u32					dwRelevantUVMASK;

	u32					vb_id;
	u32					vb_start;
	u32					ib_id;
	u32					ib_start;

	OGF() : OGF_Base(0) {
		dwRelevantUV		= 0;
		dwRelevantUVMASK	= 0;
		I_Current			= -1;
	};
	u16					_BuildVertex	(OGF_Vertex& V1);
	void				_BuildFace		(OGF_Vertex& V1, OGF_Vertex& V2, OGF_Vertex& V3);

	void				Optimize		();
	void				CreateOccluder	();
	void				MakeProgressive	();
	void				Stripify		();
	void				PerturbPMAP		();
	void				BuildVSPLIT		();
	void				DumpFaces		();

	virtual void		PreSave			();
	virtual void		Save			(IWriter &fs);

	void				Save_Cached		(IWriter &fs, ogf_header& H, BOOL bColors);
	void				Save_Normal_PM	(IWriter &fs, ogf_header& H, BOOL bColors);
	void				Save_Progressive(IWriter &fs, ogf_header& H, BOOL bColors);

	virtual void		GetGeometry		(xr_vector<Fvector> &R)
	{
		for (xr_vector<OGF_Vertex>::iterator I=vertices.begin(); I!=vertices.end(); I++)
			R.push_back(I->P);
	}
};

struct OGF_Reference : public OGF_Base
{
	OGF*				model;

	u32					material;
	vecOGF_T			textures;

	u32					vb_id;
	u32					vb_start;
	u32					ib_id;
	u32					ib_start;

	Fmatrix				xform;
	base_color_c		c_scale;
	base_color_c		c_bias;

	OGF_Reference() : OGF_Base(0) 
					{
						model	= 0;
					}

	virtual void		Save		(IWriter &fs);
	virtual void		GetGeometry	(xr_vector<Fvector> &R)
	{
		Fvector			P;
		for (xr_vector<OGF_Vertex>::iterator I=model->vertices.begin(); I!=model->vertices.end(); I++)
		{
			xform.transform_tiny	(P,I->P);
			R.push_back				(P);
		}
	}
};

struct OGF_Node : public OGF_Base
{
	xr_vector<u32>		chields;

	OGF_Node(int _L, u16 _Sector) : OGF_Base(_L) { Sector=_Sector; }

	void				AddChield	(u32 ID)
	{
		chields.push_back	(ID);
		OGF_Base*			P = g_tree[ID];
		R_ASSERT			(P->Sector == Sector);
		bbox.merge			(P->bbox);
		P->bConnected		= TRUE;
	}
	virtual void		Save		(IWriter &fs);
	virtual void		GetGeometry	(xr_vector<Fvector> &R)
	{
		for (xr_vector<u32>::iterator I=chields.begin(); I!=chields.end(); I++)
			g_tree[*I]->GetGeometry(R);
	}
};

struct	OGF_LOD		: public OGF_Node
{
	OGF_LOD(int _L, u16 _Sector) : OGF_Node(_L,_Sector) {};

	struct _vertex
	{
		Fvector			v;
		Fvector2		t;
		u32				c_rgb_hemi;	// rgb,hemi
		u8				c_sun;
	};
	struct _face
	{
		_vertex			v			[4];
	};

	_face				lod_faces	[8];
	u32					lod_Material;

	virtual void		Save		(IWriter &fs);
};

void set_status(char* N, int id, int f, int v);
