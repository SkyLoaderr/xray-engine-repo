//----------------------------------------------------
#ifndef BoneH
#define BoneH

#ifdef _LW_EXPORT
#include <lwrender.h>
#include <lwhost.h>
#endif

#pragma pack( push,1 )
enum EJointType
{
    jtRigid,
	jtCloth,
	jtJoint,
    jtWheel,			// SJointLimit[0] = axis X(1,0,0) = steer; SJointLimit[2] = axis Z(0,0,1) = wheel; 
    jtForceU32 = u32(-1)
};

struct SJointLimit
{
	Fvector2		limit;
    float 			spring_factor;
    float 			damping_factor;
    SJointLimit		(){Reset();}
    void 			Reset()
    {
    	limit.set		(0.f,0.f);
        spring_factor 	= 1.f;
        damping_factor  = 1.f;
    }
};

struct SBoneShape
{
    enum EShapeType
	{
    	stNone,
        stBox,
        stSphere,
        stCylinder,
        stForceU32 = u32(-1)
    };

	EShapeType		type;
  	Fobb			box;      	// 15*4
    Fsphere			sphere;		// 4*4
    Fcylinder		cylinder;	// 8*4
    SBoneShape		(){Reset();}
    void			Reset()
    {
    	type		= stNone;
        box.invalidate();
        sphere.identity();
        cylinder.invalidate();
    }
};

struct SJointIKData
{
    // IK
    EJointType		type;
    SJointLimit		limits	[3];// by [axis XYZ on joint] and[Z-wheel,X-steer on wheel]
    float			spring_factor;
    float			damping_factor;
    SJointIKData	(){ Reset();}
    void			Reset	()
    {
    	limits[0].Reset	();
    	limits[1].Reset	();
    	limits[2].Reset	();
        type			= jtRigid;
        spring_factor	= 1.f;
        damping_factor	= 1.f;
    }
    void			clamp_by_limits(const Fvector& basis_hpb, Fvector& dest_hpb);
};
#pragma pack( pop )

class CBone
{
	string64		name;
	string64		parent;
	string64		wmap;
	Fvector			rest_offset;
	Fvector			rest_rotate;    // HPB format
	float			rest_length;

	Fvector			mot_offset;
	Fvector			mot_rotate;		// HPB format
	float			mot_length;

    Fmatrix			last_transform;
    Fmatrix			last_i_transform;
	int				parent_idx;

public:
    // editor part
    Flags8			flags;    
	enum{
    	flSelected	= (1<<0),
    };
    SJointIKData	IK_data;
    string64		game_mtl;
    SBoneShape		shape;

    float			mass;
    Fvector			center_of_mass;
public:
					CBone			();
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
    IC BOOL			IsRoot			(){return parent_idx==-1;}

	void			Update			(const Fvector& T, const Fvector& R){mot_offset.set(T); mot_rotate.set(R); mot_length=rest_length;}
    void			Reset			(){mot_offset.set(rest_offset); mot_rotate.set(rest_rotate); mot_length=rest_length;}

	void			Save			(IWriter& F);
	void			Load_0			(IReader& F);
	void			Load_1			(IReader& F);

#ifdef _LW_EXPORT
	void			ParseBone		(LWItemID bone);
#endif

	void			SaveData		(IWriter& F);
	void			LoadData		(IReader& F);
    void			ResetData		();
    void			CopyData		(CBone* bone);
    
#ifdef _EDITOR
	Fvector&		get_rest_offset	(){return rest_offset;}
	Fvector&		get_rest_rotate	(){return rest_rotate;}
	float&			get_rest_length	(){return rest_length;}

	void			ShapeScale		(const Fvector& amount);
	void			ShapeRotate		(const Fvector& amount);
	void			ShapeMove		(const Fvector& amount);

	void			BoneRotate		(const Fvector& amount);

	bool 			Pick			(float& dist, const Fvector& S, const Fvector& D, const Fmatrix& parent);

    void			Select			(BOOL flag){flags.set(flSelected,flag);}
    bool			Selected		(){return flags.is(flSelected);}
#endif
};
DEFINE_VECTOR		(CBone*,BoneVec,BoneIt);

#endif
