//----------------------------------------------------
#ifndef _Bone_H_
#define _Bone_H_

#ifdef _LW_EXPORT
#include <lwrender.h>
#include <lwhost.h>
#endif
// refs
class CFS_Base;
class CStream;

class CBone{
	string64		name;
	string64		parent;
	string64		wmap;
	Fvector			rest_offset;
	Fvector			rest_rotate;
	float			rest_length;

	Fvector			mot_offset;
	Fvector			mot_rotate;
	float			mot_length;
    
    Fmatrix			last_transform;
    Fmatrix			last_i_transform;
	int				parent_idx;
public:
					CBone			(){name[0]=0;parent[0]=0;wmap[0]=0;rest_length=0;}
	virtual			~CBone			();

	void			SetName			(const char* p){if(p) strcpy(name,p); strlwr(name); }
	void			SetParent		(const char* p){if(p) strcpy(parent,p); strlwr(parent); }
	void			SetWMap			(const char* p){if(p) strcpy(wmap,p);}
	void			SetRestParams	(float length, const Fvector& offset, const Fvector& rotate){rest_offset.set(offset);rest_rotate.set(rotate);rest_length=length;};

	const char*		Name			(){return name;}
	const char*		Parent			(){return parent;}
	const char*		WMap			(){return wmap;}
    const Fvector&  Offset			(){return mot_offset;}
    const Fvector&  Rotate			(){return mot_rotate;}
    float			Length			(){return mot_length;}
    IC Fmatrix&		LTransform		(){return last_transform;}
    IC Fmatrix&		LITransform		(){return last_i_transform;}
    IC int&			ParentIndex		(){return parent_idx;}

	void			Update			(const Fvector& T, const Fvector& R){mot_offset.set(T); mot_rotate.set(R); mot_length=rest_length;}
    void			Reset			(){mot_offset.set(rest_offset); mot_rotate.set(rest_rotate); mot_length=rest_length;}
    
	void			Save			(CFS_Base& F);
	void			Load			(CStream& F);

#ifdef _LW_EXPORT
	void			ParseBone		(LWItemID bone);
#endif
};
#endif
