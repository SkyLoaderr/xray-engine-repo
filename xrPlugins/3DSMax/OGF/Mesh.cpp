// Mesh.cpp: implementation of the CMesh class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Mesh.h"
#include "dc_h.inl"
#include "ogfexp.h"

#define P_DIFF 0.0001f			// position/normal difference
#define T_DIFF (1.0f/256.0f)	// texture coords difference
#define N_DIFF 0.0001f			// smooth: v-pos difference

extern void		SetCheck	(HWND,DWORD,bool);
extern BOOL		GetCheck	(HWND,DWORD);
extern DWORD	GetTB		(HWND h, DWORD c);
extern void		GetEditText	(HWND h, DWORD c, char *text, int max);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMesh::CMesh(HWND _hw, bool _bSF)
{
	hw = _hw;
	bSF = _bSF;
}

CMesh::~CMesh()
{

}

void CMesh::Information()
{
	DMsg("*** Information ***");
	DMsg("   %d vertices",	vert.size());
	DMsg("   %d faces",		faces.size());
	DMsg("   %d matid",		matid.size());
	DMsg("   %d matid_list",matid_list.size());
	DMsg("   %d t-verts",	uvw.size());
	DMsg("   %d t-faces",	uvfaces.size());
}

bool CMesh::GetVertices(Mesh &m)
{
	m.DeleteIsoVerts();
	int iNumVert = m.getNumVerts();
	for (int i=0; i<iNumVert; i++) {
		Point3 &p = m.getVert(i);
		Fvector v;
		v.set(p.x,p.z,p.y);
		vert.push_back(v);
	}
	return true;
}

bool CMesh::GetVColors(Mesh &m)
{
	int iNumVert = m.numCVerts;
	if (iNumVert==0) {
		if (GetCheck(hw,IDC_VC)) {
			SetCheck(hw,IDC_VC,false);
			MessageBox(NULL,
				"Model (object) doesn't have per-vertex colors!\nCheckbox cleared.",
				"Error",MB_OK);
		} else {
			DMsg("Per-vertex colors not found.");
		}
		return false;
	} else {
		DMsg("Model contains per-vertex colors.");
	}
	for (int i=0; i<iNumVert; i++) {
		Point3 *p = &(m.vertCol[i]);
		Fvector v;
		v.set(p->x,p->y,p->z); // rgb
		colors.push_back(v);
	}
	int iNumFaces = m.getNumFaces();
	for (i=0; i<iNumFaces; i++) {
		TVFace* f = &(m.vcFace[i]);
		_face v;
		v.v[0] = f->t[2];
		v.v[1] = f->t[1];
		v.v[2] = f->t[0];
		clrfaces.push_back(v);
	}
	return true;
}

bool CMesh::GetFaces(Mesh &m)
{
	if (m.RemoveIllegalFaces()) {
		DMsg("   Illegal faces removed.");
	}
	if (m.RemoveDegenerateFaces()) {
		DMsg("   Degenerated faces removed.");
	}
	int iNumFaces = m.getNumFaces();
	for (int i=0; i<iNumFaces; i++) {
		Face* f = &(m.faces[i]);
		_face v;
		v.v[0] = f->v[2];
		v.v[1] = f->v[1];
		v.v[2] = f->v[0];
		faces.push_back(v);
		matid.push_back(f->getMatID()%mat.size());
	}
	matid_list.clear();
	for (i=0; i<matid.size(); i++)
	{
		bool bFound = false;
		for (int j=0; j<matid_list.size(); j++)
			if (matid_list[j]==matid[i]) bFound = true;
		if (!bFound) matid_list.push_back(matid[i]);
	}
	for (i=0; i<matid_list.size(); i++)
		DMsg("MATID: %d",matid_list[i]);
	return true;
}

bool CMesh::GetNormals(void)
{
	int i;

	if (!bSF) {
		// if not ShadowForm
		DMsg("Smoothing...");
		norm.resize(vert.size());
		for(i=0; i<norm.size(); i++)
			norm[i].set(0,0,0);
		
		for(i=0; i<faces.size(); i++ )
		{
			DWORD a = faces[i].v[0];
			DWORD b = faces[i].v[1];
			DWORD c = faces[i].v[2];
			
			Fvector n,v1,v2,v3,t1,t2;
			
			v1.set(vert[a]);
			v2.set(vert[b]);
			v3.set(vert[c]);
			
			t1.sub(v2,v1);
			t2.sub(v3,v2);
			n.crossproduct(t1,t2);
			n.normalize_safe();
			
			for (int j=0; j<vert.size(); j++) {
				if (vert[j].equal(vert[a],N_DIFF)) norm[j].add(n);
				if (vert[j].equal(vert[b],N_DIFF)) norm[j].add(n);
				if (vert[j].equal(vert[c],N_DIFF)) norm[j].add(n);
			}
		}
		
		for(i=0; i<norm.size(); i++)
			norm[i].normalize_safe();
	}

	return true;
}

bool CMesh::GetMapping(Mesh &m)
{
	int tvCnt = m.getNumTVerts();
	for (int i=0; i<tvCnt; i++) {
		UVVert& t = m.getTVert(i);
		Fvector v;
		v.set(t.x,t.y,t.z);
		uvw.push_back(v);
	}	
	int iNumFaces = m.getNumFaces();
	for (i=0; i<iNumFaces; i++) {
		TVFace* f = &(m.tvFace[i]);
		_face v;
		v.v[0] = f->t[2];
		v.v[1] = f->t[1];
		v.v[2] = f->t[0];
		uvfaces.push_back(v);
	}
	return true;
}

bool CMesh::GetBBox(void)
{
	bbmin.set(FLT_MAX,FLT_MAX,FLT_MAX);
	bbmax.set(FLT_MIN,FLT_MIN,FLT_MIN);

	for (int i=0; i<V.size(); i++) {
		bbmin.minv(V[i].p);
		bbmax.maxv(V[i].p);
	}

	return true;
}

bool GetTName(StdMat2 *_Mstd, DWORD _Tid, char *name) {

	if( !_Mstd->MapEnabled( _Tid ) )
		return false;

	Texmap *map = 0;
	if( 0==(map = _Mstd->GetSubTexmap(_Tid)) )
		return false;

	if( map->ClassID() != Class_ID(BMTEX_CLASS_ID,0) )
		return false;

	BitmapTex *bmap = (BitmapTex*)map;
	_splitpath( bmap->GetMapName(), 0, 0, name, 0 );
	_splitpath( bmap->GetMapName(), 0, 0, 0, name + strlen(name) );

	return true;
}

void CMesh::AddSingleMaterial(StdMat2 *_Mstd)
{
	if( _Mstd->MapEnabled( ID_DI ) ){	// diffuse texture
		char fn[MAX_PATH];
		if (GetTName(_Mstd,ID_DI,fn)) {
			DMsg( "   diffuse texture: %s",fn);
			tex.push_back(strdup(fn));
		}
	}
	
	// colors
	Color am  = _Mstd->GetAmbient		(0);
	Color di  = _Mstd->GetDiffuse		(0);
	Color sp  = _Mstd->GetSpecular		(0);
	Color em  = _Mstd->GetSelfIllumColor(0);
	float spow= _Mstd->GetShinStr		(0);
	Fmaterial		dm;
	dm.ambient.set	(am.r,am.g,am.b,1);
	dm.diffuse.set	(di.r,di.g,di.b,1);
	dm.specular.set	(sp.r,sp.g,sp.b,1);
	dm.emissive.set	(em.r,em.g,em.b,1);
	dm.power		= spow;
	mat.push_back(dm);
}

bool CMesh::GetMaterials(Mtl *m)
{
	if (m==NULL) return false;
	DMsg("   Material '%s'.",m->GetName());
	if( m->ClassID() == Class_ID(MULTI_CLASS_ID,0) ){
		DMsg( "   multi material");
		MultiMtl *_M = (MultiMtl*)m;
		
		for( int i=0; i<_M->NumSubMtls(); i++) {
			Mtl* mtl = _M->GetSubMtl(i);
			if( mtl ){
				if( mtl->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
					DMsg( "      sub #%d -> '%s'", i, mtl->GetName() );
					AddSingleMaterial( (StdMat2*)mtl );
				}
				else {
					DMsg( "   Invalid material hierarchy!" );
					return false;
				}
			} 
			else {
				DMsg( "   Zero material Ptr!" );
				return false;
			}
		}
	}
	else if( m->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
		DMsg( "   std material -> clear matid_list");
		for (int i=0; i<matid.size(); i++)	matid[i]=0;
		matid_list.clear(); matid_list.push_back(0);

		AddSingleMaterial((StdMat2 *)m);
	}
	else {
		DMsg( "   Unknown material class.");
		return false;
	}
	return true;
}

bool CMesh::ConvertMapping(void)
{
	DMsg("Converting mapping...");

	// build vertices
	V.clear();
	Lvertex v;
	std::vector<_face> nf;

	bool	bVC = !!GetCheck(hw,IDC_VC);
	for (int i=0; i<faces.size(); i++) {
		_face t;
		for (int j=0; j<3; j++) {
			v.p.set	(vert[faces[i].v[j]]);		// position
			if (!bSF)	v.n.set	(norm[faces[i].v[j]]);		// normal
			else		v.n.set	(0,0,0);
			if (bVC) 	v.c.set	(colors[clrfaces[i].v[j]]);	// color
			else 		v.c.set(0,0,0);
			if	(!bSF) {
				v.tu =  uvw	[uvfaces[i].v[j]].x;	// --
				v.tv =  -uvw[uvfaces[i].v[j]].y;	// texture coords
			} else {
				v.tu = v.tv = 0;
			}
			V.push_back(v);
			t.v[j]=V.size()-1;
		}
		nf.push_back(t);
	}
	faces.clear();
	faces.insert(faces.end(),nf.begin(),nf.end());
	DMsg("   %d vertices after expansion",V.size());
	return true;
}

// Optimization
bool IsEqual(Lvertex &v1, Lvertex &v2)
{
	if (
		v1.p.equal(v2.p)			&&
		v1.n.equal(v2.n)			&&
		(fabsf(v1.tu-v2.tu)<T_DIFF)	&&
		(fabsf(v1.tv-v2.tv)<T_DIFF)
		)	return true;
	else	return false;
}

bool CMesh::Optimize()
{
	DMsg("Optimizing...");

	// exclude unused vertices
	for (int i=0; i<V.size(); i++)
	{
		Fvector clr;
		int		ecnt=0;
		clr.set(V[i].c);	// first color
		for (int j=i+1; j<V.size(); j++)
		{
			if (IsEqual(V[i],V[j])) {
				// correct color
				clr.add(V[j].c); ecnt++;
				// exclude vertex 'j'
				V.erase(V.begin()+j);
				// exclude all references to it
				for (int k=0; k<faces.size(); k++)
					faces[k].Replace(j,i);
				j--;
			}
		}
		// merge colors
		clr.div(float(ecnt));
		V[i].c.set(clr);
	}
	DMsg("   %d vertices after similarity pass",V.size());

	// exclude dummy faces (2 or more equal vertices)
	for (i=0; i<faces.size(); i++)
	{
		if (faces[i].isDegenerate() || faces[i].isInvalid(V.size()-1) ) {
			faces.erase(faces.begin()+i);
			i--;
		}
	}
	DMsg("   %d faces left after 'degenerate' check",faces.size());

	return true;
}

//----------------------------
#include "fmesh.h"
void write_chunk_info( int hfile, OGF_Chuncks chnk, int sz )
{
	_write( hfile, &chnk, sizeof(chnk) );
	_write( hfile, &sz, sizeof(sz) );
}

bool CMesh::SaveOne(int hfile, char *sf_link)
{
	if (matid_list.size()>1) return false;

// write header
	ogf_header ohdr;
	write_chunk_info( hfile, OGF_HEADER, sizeof(ogf_header));
	ohdr.flags = vZTest|vZWrite|
		(GetCheck(hw, IDC_SPECULAR)?vSpecular:0) |
		(GetCheck(hw, IDC_ALPHA)?vAlpha:0) |
		(GetCheck(hw, IDC_LIGHT)?vLight:0)
		;
	ohdr.format_version = 2;
	if (bSF) {
		ohdr.type = MT_SHADOW_FORM;
	} else {
		if (IsDlgButtonChecked(hw,IDC_R_Standart)==BST_CHECKED) {
			ohdr.type = MT_NORMAL;
		} else {
			ohdr.type = MT_SHOTMARK;
		}
	}
	_write( hfile, &ohdr, sizeof(ohdr) );

// write material
	write_chunk_info( hfile, OGF_MATERIAL, sizeof(Fmaterial));
	if (GetCheck(hw,IDC_MATERIAL)) {
		_write( hfile, &(mat[0]), sizeof(Fmaterial) );
	} else {
		Fmaterial mmx;
		mmx.Set(1,1,1);

		if (GetCheck(hw, IDC_SPECULAR)) {
			mmx.specular.set(1,1,1,1);
			mmx.specular.mul(float(GetTB(hw, IDC_BRIGHTNESS))/80.f);
			mmx.power = float(GetTB(hw, IDC_POWER));
		}

		_write( hfile, &mmx, sizeof(Fmaterial) );
	}

// write texture
	char tex_name[256];
	char sh_name[256];
	strcpy( tex_name, tex[0] );
	GetEditText(hw, IDC_SHADER, sh_name, 256);

	write_chunk_info( hfile, OGF_TEXTURE1, strlen(tex_name)+1+strlen(sh_name)+1);
	_write( hfile, tex_name, strlen(tex_name)+1 );
	_write( hfile, sh_name, strlen(sh_name)+1 );

	DWORD dwVertType;
	if (bSF) {
		write_chunk_info( hfile, OGF_VERTICES, V.size()*sizeof(FCvertex)+4+4);
		dwVertType = 0x002 | 0x040;	// p+c
	} else {
		if (GetCheck(hw,IDC_VC)) {
			write_chunk_info( hfile, OGF_VERTICES, V.size()*sizeof(FLITvertex)+4+4);
			dwVertType = 0x002 | 0x010 | 0x040 | 0x100;	// p+n+c+t1
		} else {
			write_chunk_info( hfile, OGF_VERTICES, V.size()*sizeof(Fvertex)+4+4);
			dwVertType = 0x002 | 0x010 | 0x100;			// p+n+t1
		}
	}
	DWORD dwVertCount = V.size();
	_write( hfile, &dwVertType,  4);
	_write( hfile, &dwVertCount, 4);
	if (bSF) {
		// Shadow form vertices
		for (int k=0; k<V.size(); k++)
		{
			FCvertex v;
			v.p.set(V[k].p);
			v.c=0x5f000000;
			_write( hfile, &v, sizeof(v));
		}
	} else {
		if (GetCheck(hw,IDC_VC)) {
			// Lit vertices
			for (int k=0; k<V.size(); k++)
			{
				FLITvertex v;
				v.p.set(V[k].p);
				v.n.set(V[k].n);
				v.color.setf(V[k].c.x,V[k].c.y,V[k].c.z,1.0f);
				v.tu = V[k].tu;
				v.tv = V[k].tv;
				_write( hfile, &v, sizeof(v));
			}
		} else {
			// Unlit vertices
			for (int k=0; k<V.size(); k++)
			{
				Fvertex v;
				v.p.set(V[k].p);
				v.n.set(V[k].n);
				v.tu = V[k].tu;
				v.tv = V[k].tv;
				_write( hfile, &v, sizeof(v));
			}
		}
	}

// write bbox
	write_chunk_info( hfile, OGF_BBOX, 2*sizeof(Fvector));
	_write( hfile, &bbmin, sizeof(Fvector) );
	_write( hfile, &bbmax, sizeof(Fvector) );

// write indices
	DWORD dwIdxCnt = faces.size()*3;
	write_chunk_info( hfile, OGF_INDICES, dwIdxCnt*sizeof(WORD)+4);
	_write(hfile, &dwIdxCnt, 4);
	for(int i=0; i<faces.size(); i++) {
		WORD a = faces[i].v[0];
		WORD b = faces[i].v[1];
		WORD c = faces[i].v[2];
		_write( hfile, &a, sizeof(WORD) );
		_write( hfile, &b, sizeof(WORD) );
		_write( hfile, &c, sizeof(WORD) );
	}

// write SF-link (if any)
	if (sf_link[0]) {
		char fnSF	[_MAX_PATH	];
		char drive	[_MAX_DRIVE	];
		char dir	[_MAX_DIR	];
		char fname	[_MAX_FNAME	];
		char ext	[_MAX_EXT	];
		_splitpath( sf_link, drive, dir, fname, ext );
		strcpy(fnSF,fname);
		strcat(fnSF,ext);
		write_chunk_info( hfile, OGF_SF_LINK, strlen(fnSF)+1);
		_write( hfile, fnSF, strlen(fnSF)+1 );
	}

	return true;
}

bool CMesh::SaveCF(int hfile)
{
	if (matid_list.size()>1) return false;

// write header
	DWORD dwCount = faces.size();
	_write(hfile, &dwCount, 4);
	_vector oc; float r;
	oc.sub(bbmax,bbmin);
	oc.div(2);
	r = oc.magnitude();
	oc.add(bbmin);
	_write( hfile, &oc,		sizeof(oc));
	_write( hfile, &r,		sizeof(r));
	_write( hfile, &bbmin,	sizeof(Fvector) );
	_write( hfile, &bbmax,	sizeof(Fvector) );

// write indices
	for(int i=0; i<faces.size(); i++) {
		WORD a = faces[i].v[0];
		WORD b = faces[i].v[1];
		WORD c = faces[i].v[2];

		// vertices
		_write( hfile, &V[a].p,	sizeof(Fvector) );
		_write( hfile, &V[b].p,	sizeof(Fvector) );
		_write( hfile, &V[c].p,	sizeof(Fvector) );
		// link
		int L = -1;
		_write( hfile, &L, 4);
	}
	return true;
}
