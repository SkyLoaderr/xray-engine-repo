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
		flLIGHT_Sharp		= 1<<5
	};
	struct Flags {
		DWORD bCollision			: 1;
		DWORD bRendering			: 1;
		DWORD bOptimizeUV			: 1;
		DWORD bLIGHT_Vertex			: 1;
		DWORD bLIGHT_CastShadow		: 1;
		DWORD bLIGHT_Sharp			: 1;
	};
public:
	char		Name		[128];
	Flags		flags;
	float		vert_translucency;
	float		vert_ambient;
	float		lm_density;

	Shader_xrLC()	{
		ZeroMemory	(this,sizeof(*this));
		strcpy		(Name,"unknown");
		flags.bCollision		= TRUE;
		flags.bRendering		= TRUE;
		flags.bOptimizeUV		= TRUE;
		flags.bLIGHT_Vertex		= FALSE;
		flags.bLIGHT_CastShadow = TRUE;
		flags.bLIGHT_Sharp		= TRUE;
		vert_translucency		= .5f;
		vert_ambient			= .0f;
		lm_density				= 1.f;
	}
};

#define						SHADER_XRLC_HEADER "shXRLC  "
DEFINE_VECTOR(Shader_xrLC,Shader_xrLCVec,Shader_xrLCIt);
class Shader_xrLC_LIB
{
	Shader_xrLCVec			library;
public:
	void					Load	(LPCSTR name)
	{
		CCompressedStream	fs(name,SHADER_XRLC_HEADER);
		int count			= fs.Length()/sizeof(Shader_xrLC);
		R_ASSERT			(int(fs.Length()) == int(count*sizeof(Shader_xrLC)));
		library.resize		(count);
		fs.Read				(library.begin(),fs.Length());
	}
	void					Save	(LPCSTR name)
	{
		CFS_Memory			fs;
		fs.write			(library.begin(),library.size()*sizeof(Shader_xrLC));
		fs.SaveTo			(name,SHADER_XRLC_HEADER);
	}
	void					Unload	()
	{
		library.clear		();
	}
	int						GetID	(LPCSTR name)
	{
		for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++)
			if (0==stricmp(name,it->Name)) return it-library.begin();
		return -1;
	}
	Shader_xrLC*			Get		(LPCSTR name)
	{
		for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++)
			if (0==stricmp(name,it->Name)) return &(*it);
		return NULL;
	}
	Shader_xrLC*			Get		(int id)
	{
		return &library[id];
	}
	Shader_xrLC*			Append	(Shader_xrLC* parent=0)
	{
		library.push_back(parent?Shader_xrLC(*parent):Shader_xrLC());
		return &library.back();
	}
	void					Remove	(LPCSTR name)
	{
		for (Shader_xrLCIt it=library.begin(); it!=library.end(); it++)
			if (0==stricmp(name,it->Name)){
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