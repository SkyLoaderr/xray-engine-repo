// xrSKIN_BUILD.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "fs.h"
#include "fmesh.h"
#include "progmesh.h"
#include "xrSKIN_BUILD.h"
#include "Exporter.h"
#include "ftimer.h"
#include "MgcCont3DMinBox.h"

#pragma comment(lib, "x:\\xrProgressive.lib")

string convert_space(string input)
{
	string result = "";
	for (int i=0; i<input.size(); i++)
		result += (input[i]==' ')?'_':input[i];
	char * res_ptr = (char *)result.c_str();
	_strlwr(res_ptr);
	return result;
}

/*
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{	return DefWindowProc( hWnd, msg, wParam, lParam );	}
*/
struct st_SPLIT;
DEF_VECTOR	(vSPLIT,st_SPLIT);
vSPLIT		splits;

vVERT		reg_verts;
vFACE		reg_faces;

vVERT		verts;
vFACE		faces;
vMAT		mat;

void RegisterVERT(st_VERT* v){reg_verts.push_back(v);}
void RegisterFACE(st_FACE* f){reg_faces.push_back(f);}


BOOL	g_bProgressive	= TRUE;

struct st_SPLIT
{
	vVERT			Vlist;
	vFACE			Flist;

	std::vector<WORD>	Indices;		// valid only after call to MakeValidToRender
	
	DWORD			mtl;
	DWORD			dwBonesPerVert;
	Fbox			bb;

	// Progressive
	DWORD			I_Current;
	DWORD			V_Minimal;
	std::vector<Vsplit>	pmap_vsplit;
	std::vector<WORD>	pmap_faces;

	/*
	void	SelectByBoneCount(DWORD num, vFACE &L)
	{
		dwBonesPerVert = num;
		for (vFACE_it fI=L.begin(); fI!=L.end(); fI++){
			FACE*	F = *fI;
			DWORD CNT = F->GetMaxBoneCnt();
			if (CNT==num){
				Flist.push_back(F);
			}
		}
	}
	*/
	st_VERT*	AddVert(st_VERT* pTestV)
	{
		for (vVERT_it vI=Vlist.begin(); vI!=Vlist.end(); vI++)
			if ((*vI)->similar(*pTestV)) return *vI;

		st_VERT* V = new st_VERT;
		*V		= *pTestV;
		Vlist.push_back(V);
		return V;
	}
	void	MakeValidToRender()
	{
		/*
		// construct bones list
		for(vFACE_it FI=Flist.begin(); FI!=Flist.end(); FI++)
		{
			FACE*	F = *FI;
			for (int i=0; i<3; i++) {
				Blist.insert(F->v[i]->bone[0]);
				Blist.insert(F->v[i]->bone[1]);
			}
		}
		Blist.erase(DWORD(BONE_NONE));
		*/

		// correct vertices and copy them to local list
		for(vFACE_it FI=Flist.begin(); FI!=Flist.end(); FI++){
			st_FACE*	F = *FI;
			for (int i=0; i<3; i++)
				F->v[i] = AddVert(F->v[i]);
		}

		// Progressive
		I_Current=V_Minimal=0;
		for (vFACE_it F=Flist.begin(); F!=Flist.end(); F++)
		{
			st_FACE* pF = *F;
			for (int i=0; i<3; i++) {
				vVERT_it	found	= std::find(Vlist.begin(),Vlist.end(),pF->v[i]);
				R_ASSERT	(found!=Vlist.end());
				DWORD		ID		= found - Vlist.begin();
				Indices.push_back	(WORD(ID));
			}
		}

	}
	void	MakeProgressive()
	{
		if (Flist.size()>1) {
			// Options
			PM_Options(1,1,3);
			
			// Transfer vertices
			for (vVERT_it V=Vlist.begin(); V!=Vlist.end(); V++)
			{
				st_VERT		&iV = **V;
				PM_CreateVertex(iV.P.x,iV.P.y,iV.P.z,V - Vlist.begin(),(P_UV*)(&iV.u));
			}
			
			// Convert
			PM_Result R;
			I_Current = PM_Convert(Indices.begin(),Indices.size(), &R);
			if (I_Current) {
				// Permute vertices
				vVERT temp_list = Vlist;
				
				// Perform permutation
				for(int i=0; i<temp_list.size(); i++)
					Vlist[R.permutePTR[i]]=temp_list[i];
				
				// Copy results
				pmap_vsplit.resize(R.splitSIZE);
				CopyMemory(pmap_vsplit.begin(),R.splitPTR,R.splitSIZE*sizeof(Vsplit));
				
				pmap_faces.resize(R.facefixSIZE);
				CopyMemory(pmap_faces.begin(),R.facefixPTR,R.facefixSIZE*sizeof(WORD));
				
				V_Minimal = R.minVertices;
			}
		}
	}

	void Save(char *name)
	{
		CFS_File fs(name);

		// Header
		fs.open_chunk		(OGF_HEADER);
		ogf_header			H;
		H.format_version	= xrOGF_FormatVersion;
		H.type				= I_Current?MT_SKELETON_PART:MT_SKELETON_PART_STRIPPED;
		H.flags				= 0;
		fs.write			(&H,sizeof(H));
		fs.close_chunk		();
		
		// Texture
		// CLASS1:[script1]name1,CLASS2:[script2]name2,CLASS3:[script3]name3,...
		fs.open_chunk(OGF_TEXTURE);
		string Tname = mat[mtl];
		fs.write(Tname.c_str(),Tname.length()+1);
		Tname = string("model");
		fs.write(Tname.c_str(),Tname.length()+1);
		fs.close_chunk();
		
		// Vertices
		bb.invalidate();
		fs.open_chunk(OGF_VERTICES);
		fs.Wdword(0x12071980);
		fs.Wdword(Vlist.size());
		for (vVERT_it V=Vlist.begin(); V!=Vlist.end(); V++){
			st_VERT* pV = *V;
			bb.modify(pV->P);
			fs.write(&(pV->O),sizeof(float)*3);		// position (offset)
			fs.write(&(pV->N),sizeof(float)*3);		// normal
			fs.Wfloat(pV->u); fs.Wfloat(pV->v);		// tu,tv
			fs.Wdword(pV->bone);
		}
		fs.close_chunk();
		
		// Faces
		fs.open_chunk(OGF_INDICES);
		fs.Wdword(Indices.size());
		fs.write(Indices.begin(),Indices.size()*sizeof(WORD));
		fs.close_chunk();
		
		// PMap
		if (I_Current) {
			fs.open_chunk(OGF_P_MAP);
			{
				fs.open_chunk(0x1);
				fs.Wdword(V_Minimal);
				fs.Wdword(I_Current);
				fs.close_chunk();
			}
			{
				fs.open_chunk(0x2);
				fs.write(pmap_vsplit.begin(),pmap_vsplit.size()*sizeof(Vsplit));
				fs.close_chunk();
			}
			{
				fs.open_chunk(0x3);
				fs.Wdword(pmap_faces.size());
				fs.write(pmap_faces.begin(),pmap_faces.size()*sizeof(WORD));
				fs.close_chunk();
			}
			fs.close_chunk();
		}
		
		// BBox (already computed)
		fs.open_chunk(OGF_BBOX);
		fs.write(&bb,sizeof(Fvector)*2);
		fs.close_chunk();
	}
};
 
// ================================================== dummyFn()
// used by 3DS progress bar
static DWORD WINAPI dummyFn(LPVOID arg)
{
    return(0);
}
string GetBoneParent(CExporter *E, string &name)
{
	for (std::vector<INode*>::iterator I=E->g_all_bones.begin(); I!=E->g_all_bones.end(); I++){
		INode*	N = *I;
		if (stricmp(convert_space(N->GetName()).c_str(),name.c_str())==0) {
			// bone found
			INode* P = N->GetParentNode();
			if (P)	{
				if (P->IsRootNode())	return "";
				else					return convert_space(string(P->GetName()));
			}	else	return "";
		}
	}
	return "";
}

void ComputeOBB	(Fobb &B, std::vector<Fvector> &V)
{
	if (V.size()<3) {
		B.invalidate();
		return;
	}
	MgcBox3	BOX			= MgcMinBox(V.size(), (const MgcVector3*) V.begin());
	MgcVector3& R		= BOX.Axis(0);
	MgcVector3& N		= BOX.Axis(1);
	MgcVector3& D		= BOX.Axis(2);
	MgcVector3&	T		= BOX.Center();
	MgcReal*	S		= BOX.Extents();
	B.m_rotate.i.set	(R.x,R.y,R.z);
	B.m_rotate.j.set	(N.x,N.y,N.z);
	B.m_rotate.k.set	(D.x,D.y,D.z);
	B.m_translate.set	(T.x,T.y,T.z);
	B.m_halfsize.set	(S[0],S[1],S[2]);
}

void PARSE		(CExporter* E, Interface *pInterface, char* fnTOSAVE){
	std::vector< std::vector<Fvector> >	bone_points;
	CTimer						T;


    pInterface->ProgressStart(_T("Converting..."),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(50);

	// Optimizing bones usage


	// Calculate normals
	Msg("Computing normals..."); T.Start();
	for(int i=0; i<faces.size(); i++ ){
		Fvector& p1	= faces[i]->v[0]->P;
		Fvector& p2	= faces[i]->v[1]->P;
		Fvector& p3	= faces[i]->v[2]->P;
		DWORD& g1	= faces[i]->v[0]->sm_group;
		DWORD& g2	= faces[i]->v[1]->sm_group;
		DWORD& g3	= faces[i]->v[2]->sm_group;
		
		Fvector n,t1,t2;
		t1.sub(p2,p1);
		t2.sub(p3,p2);
		n.crossproduct(t1,t2);
		n.normalize_safe();
		
		for (vVERT_it vvI=verts.begin(); vvI!=verts.end(); vvI++){
			st_VERT*	 V = *vvI;
			Fvector& P = V->P;
			DWORD&	 g = V->sm_group;
			if (P.similar(p1)&&((g&g1)||!g)) V->N.add(n);
			if (P.similar(p2)&&((g&g2)||!g)) V->N.add(n);
			if (P.similar(p3)&&((g&g3)||!g)) V->N.add(n);
		}
	}
	T.Dump();

	// Normalize influence & normals
	Msg("Transforming normals..."); T.Start();
	bone_points.resize(E->g_bones.size());
	for (vVERT_it vI=verts.begin(); vI!=verts.end(); vI++){
		st_VERT* V = *vI;
		V->N.normalize_safe();
		Point3	N  = Point3(V->N.x,V->N.z,V->N.y);
		Matrix3 X = E->g_bones[V->bone]->matOffset;
		X.NoTrans	();
		N = N*X;
		V->N.set			(N.x,N.z,N.y);
		V->N.normalize_safe	();
		
		bone_points[V->bone].push_back(V->O);
	}
	T.Dump();

	// Collapse materials
	Msg("Collapsing materials..."); T.Start();
	for (i=0; i<mat.size(); i++){
		for (int j=i+1; j<mat.size(); j++){
			if (mat[i]==mat[j]){
				// replace material J by I
				for (vFACE_it fI=faces.begin(); fI!=faces.end(); fI++){
					st_FACE*	F = *fI;
					if (F->m == j) F->m=i;
					else if (F->m > j) F->m--;
				}

				mat.erase(mat.begin()+j);
				j--;
			}
		}
	}
	T.Dump();
	for (int mt=0; mt<mat.size(); mt++)
		Log("*material*",mat[mt].c_str());

	// Collapse vertices
	Msg("...and vertices..."); T.Start();
	for (i=0; i<verts.size(); i++){
		st_VERT* from = verts[i];
		for (int j=i+1; j<verts.size(); j++){
			if (from->similar(*(verts[j]))){
				// replace vertex J by I
				st_VERT* to = verts[j];
				for (vFACE_it fI=faces.begin(); fI!=faces.end(); fI++){
					(*fI)->ReplaceVert(from,to);
				}
				verts.erase(verts.begin()+j);
				j--;
			}
		}
	}
	T.Dump();

	pInterface->ProgressStart(_T("Subdividing..."),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(80);

	// materials
	Msg("Converting..."); T.Start();
	splits.resize(mat.size());
	for (i=0; i<splits.size(); i++) splits[i].mtl=i;

	for (vFACE_it fI=faces.begin(); fI!=faces.end(); fI++)
	{
		st_FACE*	F	= *fI;
		DWORD	mtl	= F->m;
		splits[mtl].Flist.push_back(F);
	}
	T.Dump();

	Msg("VB+IB+PMesh..."); T.Start();
	for (i=0; i<splits.size(); i++){
		splits[i].MakeValidToRender();
		splits[i].MakeProgressive();
	}
	T.Dump();

	Msg("Saving geometry..."); T.Start();
	string	root_name,base_name;
	{
		char drive	[_MAX_DRIVE];
		char dir	[_MAX_DIR];
		char fname	[_MAX_FNAME];
		char ext	[_MAX_EXT];
		_splitpath( fnTOSAVE, drive, dir, fname, ext );
		base_name	= fname;
		root_name	= string(drive)+string(dir)+base_name;
	}
	Fbox	rootBB; rootBB.invalidate();
	CFS_File fs((root_name+".ogf").c_str());

	// Header
	fs.open_chunk(OGF_HEADER);
	ogf_header H;
	H.format_version	= xrOGF_FormatVersion;
	H.type				= MT_SKELETON;
	H.flags				= 0;
	fs.write(&H,sizeof(H));
	fs.close_chunk();

	// OGF_CHIELDS
	fs.open_chunk	(OGF_CHIELDS);
	fs.Wdword		(splits.size());
	for (i=0; i<splits.size(); i++)
	{
		char N[MAX_PATH];
		sprintf(N,"%s%d.ogf",root_name.c_str(),i);
		splits[i].Save(N);

		rootBB.merge(splits[i].bb);
		sprintf(N,"%s%d.ogf",base_name.c_str(),i);
		fs.WstringZ(N);
	}
	fs.close_chunk();
	T.Dump();

    pInterface->ProgressStart(_T("Done."),TRUE,dummyFn,NULL);
    pInterface->ProgressUpdate(100);

	Msg("Computing and saving hierrarhy..."); T.Start();
	// BBox (already computed)
	fs.open_chunk(OGF_BBOX);
	fs.write(&rootBB,sizeof(Fvector)*2);
	fs.close_chunk();

	// BoneNames
	fs.open_chunk(OGF_BONE_NAMES);

	// Exclude duplicates
	std::sort(E->g_all_bones.begin(),E->g_all_bones.end());
	std::vector<INode*>::iterator newend = std::unique(E->g_all_bones.begin(),E->g_all_bones.end());
	E->g_all_bones.erase(newend,E->g_all_bones.end());

	fs.Wdword(E->g_bones.size());
	for (int B=0; B!=E->g_bones.size(); B++){
		CBoneDef*	pBone = E->g_bones[B];
		fs.WstringZ	(pBone->name.c_str());
		fs.WstringZ	(GetBoneParent(E,pBone->name).c_str());

		Fobb	obb;
		ComputeOBB	(obb,bone_points[B]);
		fs.write	(&obb,sizeof(obb));
	}
	fs.close_chunk();

	// Motions
	fs.open_chunk(OGF_MOTIONS);
	fs.Wdword(0);
	fs.close_chunk();

	T.Dump();
};

int startup(CExporter* E, Interface *pInterface, char* fnTOSAVE )
{
	PARSE(E, pInterface,fnTOSAVE);
    pInterface->ProgressEnd();
	return 0;
}

void finalize(){
	for (vVERT_it v_it=reg_verts.begin(); v_it!=reg_verts.end(); v_it++) { _DELETE(*v_it); }
	for (vFACE_it f_it=reg_faces.begin(); f_it!=reg_faces.end(); f_it++) { _DELETE(*f_it); }
	mat.clear();
	verts.clear();
	faces.clear();
	splits.clear();
}
