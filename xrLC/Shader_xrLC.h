#ifndef SHADER_XRLC_H
#define SHADER_XRLC_H
#pragma once

struct Shader_xrLC
{
public:
	enum {
		flCollision			= 1<<0,
		flRendering			= 1<<1,
		flOptimizeUV		= 1<<2,
		flLIGHT_Vertex		= 1<<3,
		flLIGHT_CastShadow	= 1<<4,
		flLIGHT_Sharp		= 1<<5,
	};
public:
	ref_str		Name;
    Flags32		m_Flags;
	float		vert_translucency;
	float		vert_ambient;
	float		lm_density;

	Shader_xrLC()	
    {
		Name					= "unknown";
        m_Flags.assign			(flCollision|flRendering|flOptimizeUV|flLIGHT_CastShadow|flLIGHT_Sharp);
		vert_translucency		= .5f;
		vert_ambient			= .0f;
		lm_density				= 1.f;
	}
    void		Load	(IReader& F)
    {
    	string128 tmp;
        F.r						(tmp,sizeof(tmp)); Name = tmp;
        m_Flags.assign			(F.r_u32());
		vert_translucency		= F.r_float();
		vert_ambient			= F.r_float();
		lm_density				= F.r_float();
    }
    void		Save	(IWriter& F)
    {
        F.w_stringZ				(Name);
        F.w_u32					(m_Flags.flags);
		F.w_float				(vert_translucency);
		F.w_float				(vert_ambient);
		F.w_float				(lm_density);
    }
};

struct Shader_xrLC_OLD
{
	string128	Name;
    Flags32		m_Flags;
	float		vert_translucency;
	float		vert_ambient;
	float		lm_density;
};

DEFINE_VECTOR(Shader_xrLC,Shader_xrLCVec,Shader_xrLCIt);
class Shader_xrLC_LIB
{
	Shader_xrLCVec			library;
public:
	void					Load	(LPCSTR name)
	{
		IReader* fs			= FS.r_open(name);
		int count			= fs->length()/sizeof(Shader_xrLC_OLD);
//		int count			= fs->r_u32();
		R_ASSERT			(int(fs->length()) == int(count*sizeof(Shader_xrLC_OLD)));
		library.resize		(count);
        for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++) 
        	it->Load		(*fs);
        FS.r_close			(fs);
	}
	void					Save	(LPCSTR name)
	{
		IWriter* F			= FS.w_open(name);
        F->w_u32			(library.size());
        for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++) 
        	it->Save		(*F);
        FS.w_close			(F);
	}
	void					Unload	()
	{
		library.clear		();
	}
	u32						GetID	(ref_str name)
	{
		for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++)
			if (name==it->Name) return u32(it-library.begin());
		return u32(-1);
	}
	Shader_xrLC*			Get		(ref_str name)
	{
		for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++)
			if (name==it->Name) return &(*it);
		return NULL;
	}
	Shader_xrLC*			Get		(int id)
	{
		return &library[id];
	}
	Shader_xrLC*			Append	(Shader_xrLC* parent=0)
	{
		if (parent)			library.push_back(Shader_xrLC(*parent));
        else				library.push_back(Shader_xrLC());
		return &library.back();
	}
	void					Remove	(ref_str name)
	{
		for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++)
			if (name==it->Name){
            	library.erase(it);
                break;
            }
	}
	void					Remove	(int id)
	{
		library.erase(library.begin()+id);
	}
	Shader_xrLCVec&			Library	(){return library;}
};
#endif
