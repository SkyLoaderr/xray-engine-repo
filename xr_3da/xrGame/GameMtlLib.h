//---------------------------------------------------------------------------
#ifndef GameMtlLibH
#define GameMtlLibH
//---------------------------------------------------------------------------
#pragma once

#define GAMEMTL_CURRENT_VERSION			0x0001
//----------------------------------------------------

#define GAMEMTLS_CHUNK_VERSION        	0x1000
#define GAMEMTLS_CHUNK_AUTOINC	        0x1001
#define GAMEMTLS_CHUNK_MTLS	        	0x1002
#define GAMEMTLS_CHUNK_MTLS_PAIR       	0x1003
//----------------------------------------------------
#define GAMEMTL_CHUNK_MAIN	        	0x1000
#define GAMEMTL_CHUNK_FLAGS	        	0x1001
#define GAMEMTL_CHUNK_PHYSICS        	0x1002
#define GAMEMTL_CHUNK_FACTORS        	0x1003
//----------------------------------------------------
#define GAMEMTLPAIR_CHUNK_PAIR   		0x1000
#define GAMEMTLPAIR_CHUNK_FLOTATION   	0x1001
#define GAMEMTLPAIR_CHUNK_BREAKING   	0x1002
#define GAMEMTLPAIR_CHUNK_STEP   		0x1003
#define GAMEMTLPAIR_CHUNK_COLLIDE   	0x1004
#define GAMEMTLPAIR_CHUNK_HIT		  	0x1005
//----------------------------------------------------

#define GAMEMTL_SUBITEM_COUNT			4

#ifdef _EDITOR
 #define SoundSVec4 	AnsiString
 #define PSSVec4 		AnsiString
 #define ShaderSVec4 	AnsiString
#else
 class CSound;
 class CPS;

 DEFINE_SVECTOR(CSound*,GAMEMTL_SUBITEM_COUNT,SoundSVec4,SoundS4It);
 DEFINE_SVECTOR(CPS*,GAMEMTL_SUBITEM_COUNT,PSSVec4,PSS4It);
 DEFINE_SVECTOR(Shader*,GAMEMTL_SUBITEM_COUNT,ShaderSVec4,ShaderS4It);
#endif

struct SGameMtl{
	friend class CGameMtlLibrary;
protected:
	int 				ID; 	// auto number
public:
	enum{
    	flBreakable	= 1<<0,
        flShootable = 1<<1,
        flBounceable= 1<<2,
        flWheeltrace= 1<<3,
        flBloodmark	= 1<<4
    };
public:
	string64			name;
    
    Flags32				Flags;
    // physics part
    float				fPHFriction;            // ?
    float				fPHDumping;             // ?
    float				fPHSpring;              // ?
    float				fPHBounceStartVelocity;	// ?
	float				fPHBouncing;            // ?
	// shoot&bounce&visibility
    float				fShootFactor;			// 0.f - 1.f
    float				fBounceDamageFactor;	// 0.f - 100.f
    float				fVisTransparencyFactor;	// 0.f - 1.f
    float				fSndOcclusionFactor;	// 0.f - 1.f
public:
	SGameMtl			()	
    {
		ZeroMemory		(this,sizeof(*this));
		strcpy			(name,"unknown");
        ID				= -1;
	}               	
    void 				Load			(CStream& fs);
    void 				Save			(CFS_Base& fs);
    IC int				GetID			(){return ID;}
#ifdef _EDITOR
    void 				FillProp		(PropValueVec& values);
#endif
};
DEFINE_VECTOR(SGameMtl*,GameMtlVec,GameMtlIt);

struct SGameMtlPair{
	friend class CGameMtlLibrary;
private:
	int					mtl0;
	int					mtl1;
protected:
	int 				ID; 	// auto number
    int					ID_parent;
public:
	enum{
        flFlotation		= (1<<0),
        flBreakingSounds= (1<<1),
        flStepSounds	= (1<<2),
        flCollideSounds	= (1<<3),
        flHitSounds		= (1<<4),
        flHitParticles	= (1<<5),
        flHitMarks		= (1<<6)
    };
    Flags32				OwnProps;
//	properties
    float		    	fFlotation;
    SoundSVec4			BreakingSounds;
    SoundSVec4			StepSounds;
    SoundSVec4			CollideSounds;
    SoundSVec4			HitSounds;
    PSSVec4				HitParticles;
    ShaderSVec4			HitMarks;
#ifdef _EDITOR
    PropValue*	    	propFlotation;
    PropValue*			propBreakingSounds;
    PropValue*			propStepSounds;
    PropValue*			propCollideSounds;
    PropValue*			propHitSounds;
    PropValue*			propHitParticles;
    PropValue*			propHitMarks;
    void __fastcall 	OnFlagChange	(PropValue* sender);
#endif
public:
	SGameMtlPair		()
    {
		ZeroMemory		(this,sizeof(*this));
    	mtl0			= -1;
    	mtl1			= -1;
        ID				= -1;
        ID_parent		= -1;
        OwnProps.one	();
	}
    IC int				GetMtl0			(){return mtl0;}
    IC int				GetMtl1			(){return mtl1;}
    IC int				GetID			(){return ID;}
    IC void				SetPair			(int m0, int m1){if (m0<m1){mtl0=m0; mtl1=m1;} else { mtl0=m1; mtl1=m0;}}
	IC bool				IsPair			(int m0, int m1){return !!(((mtl0==m0)&&(mtl1==m1))||((mtl0==m1)&&(mtl1==m0)));}
    void				Save			(CFS_Base& fs);
    void				Load			(CStream& fs);
#ifdef _EDITOR
	void 				FillProp		(PropValueVec& values);
    void				TransferFromParent(SGameMtlPair* parent);
#endif
};

DEFINE_VECTOR(SGameMtlPair*,GameMtlPairVec,GameMtlPairIt);

class CGameMtlLibrary{
	int					material_index;
	int					material_pair_index;

	GameMtlVec			materials;
    GameMtlPairVec		material_pairs;

    // game part
    u32					material_count;
    GameMtlPairVec		material_pairs_rt;
public:
	CGameMtlLibrary		()
	{
	    material_index 		= 0;
	    material_pair_index = 0;
        material_count	    = 0;
    }
	~CGameMtlLibrary	()
	{
		for (GameMtlIt m_it=materials.begin(); m_it!=materials.end(); m_it++)
			_DELETE	(*m_it);
		for (GameMtlPairIt p_it=material_pairs.begin(); p_it!=material_pairs.end(); p_it++)
			_DELETE	(*p_it);
    }
    // material routine
    IC GameMtlIt 		GetMaterialIt	(LPCSTR name)
    {
        for (GameMtlIt it=materials.begin(); it!=materials.end(); it++)
            if (0==strcmpi((*it)->name,name)) return it;
        return materials.end();
    }
    IC GameMtlIt 		GetMaterialItByID(int id)
    {
        for (GameMtlIt it=materials.begin(); it!=materials.end(); it++)
            if ((*it)->ID==id) return it;
        return materials.end();
    }
#ifdef _EDITOR
	// editor
	SGameMtl*			AppendMaterial	(SGameMtl* parent);
	void				RemoveMaterial	(LPCSTR name);
	SGameMtl*			GetMaterialByID	(int ID);
	SGameMtl*			GetMaterial		(LPCSTR name);
#else
	// game
	IC u32				GetMaterialIdx	(int ID){GameMtlIt it=GetMaterialItByID(ID);R_ASSERT(it!=materials.end()); return it-materials.begin();}
	IC u32				GetMaterialIdx	(LPCSTR name){GameMtlIt it=GetMaterialIt(name);R_ASSERT(it!=materials.end()); return it-materials.begin();}
	IC SGameMtl*		GetMaterial		(u32 idx){R_ASSERT(idx<materials.size()); return materials[idx];}
#endif

	IC GameMtlIt		FirstMaterial	(){return materials.begin();}
	IC GameMtlIt		LastMaterial	(){return materials.end();}

// material pair routine
#ifdef _EDITOR
	LPCSTR				MtlPairToName		(int mtl0, int mtl1);
	void				NameToMtlPair		(LPCSTR name, int& mtl0, int& mtl1);
	void				MtlNameToMtlPair	(LPCSTR name, int& mtl0, int& mtl1);
	SGameMtlPair*		AppendMaterialPair	(int m0, int m1, SGameMtlPair* parent=0);
	void				RemoveMaterialPair	(LPCSTR name);
	void				RemoveMaterialPair	(GameMtlPairIt rem_it);
	void				RemoveMaterialPair	(int mtl);
	void				RemoveMaterialPair	(int mtl0, int mtl1);
	GameMtlPairIt		GetMaterialPairIt	(int id);
	SGameMtlPair*		GetMaterialPair		(int id);
	GameMtlPairIt		GetMaterialPairIt	(int mtl0, int mtl1);
	SGameMtlPair*		GetMaterialPair		(int mtl0, int mtl1);
	SGameMtlPair*		GetMaterialPair		(LPCSTR name);
    int					GetParents			(SGameMtlPair* obj, GameMtlPairVec& lst);
#endif
                                                              
	// game
	IC SGameMtlPair*	GetMaterialPair		(u32 idx0, u32 idx1){R_ASSERT((idx0<material_count)&&(idx1<material_count)); return material_pairs_rt[idx1*material_count+idx0];}

	IC GameMtlPairIt	FirstMaterialPair	(){return material_pairs.begin();}
	IC GameMtlPairIt	LastMaterialPair	(){return material_pairs.end();}
    // IO routines
	void				Load				(LPCSTR name);
	void				Save				(LPCSTR name);
};

extern CGameMtlLibrary GMLib;
#endif

