// Mesh.h: interface for the CMesh class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESH_H__34D4D5C3_7D07_4191_B0E3_1FE85B969DDB__INCLUDED_)
#define AFX_MESH_H__34D4D5C3_7D07_4191_B0E3_1FE85B969DDB__INCLUDED_
#pragma once

struct _face {
	WORD v[3];
public:
	bool	Contains(int idx) {
		if (v[0]==idx) return true;
		if (v[1]==idx) return true;
		if (v[2]==idx) return true;
		return false;
	};
	void	Replace(int idx, int id2) {
		assert(idx>id2);
		// replace
		if (v[0]==idx)	v[0]=id2;
		if (v[1]==idx)	v[1]=id2;
		if (v[2]==idx)	v[2]=id2;
		// because idx deleted - reindex
		if (v[0]>idx)	v[0]-=1;
		if (v[1]>idx)	v[1]-=1;
		if (v[2]>idx)	v[2]-=1;
	};
	bool	isDegenerate(void)
	{
		if ((v[0]==v[1]) || (v[0]==v[2]) || (v[1]==v[2])) return true;
		else return false;
	};
	bool	isInvalid(int idx)
	{
		if (v[0]>idx)	return true;
		if (v[1]>idx)	return true;
		if (v[2]>idx)	return true;
		return false;
	}
};

struct Lvertex {
public:
	Fvector p;
	Fvector n;
	Fvector c;
	float tu,tv;
};

class CMesh  
{
	HWND	hw;
	bool	bSF;

	std::vector <Fvector>   colors;
	std::vector <_face>		clrfaces;

	std::vector <Fvector>	uvw;
	std::vector <_face>		uvfaces;

	std::vector <DWORD>		matid;
	std::vector <Fvector>	vert;
	std::vector <Fvector>	norm;
	std::vector <_face>		faces;

	std::vector <DWORD>		matid_list;	//	)
	std::vector <Fmaterial> mat;		//	)
	std::vector <char *>	tex;		//	)

	std::vector <Lvertex>	V;

	Fvector					bbmin;
	Fvector					bbmax;
private:
	void AddSingleMaterial(StdMat2 *m);

public:
	bool GetMapping		(Mesh &m);
	bool GetNormals		();
	bool GetFaces		(Mesh &m);
	bool GetVertices	(Mesh &m);
	bool GetVColors		(Mesh &m);
	bool GetBBox		(void);
	bool GetMaterials	(Mtl *m);

	bool ConvertMapping	(void);
	bool Optimize		(void);
	bool SaveOne		(int hf, char *sf_link);
	bool SaveCF			(int hf);

	void Information(void);

	CMesh(HWND _hw, bool _bSF);
	virtual ~CMesh();

};

#endif // !defined(AFX_MESH_H__34D4D5C3_7D07_4191_B0E3_1FE85B969DDB__INCLUDED_)
