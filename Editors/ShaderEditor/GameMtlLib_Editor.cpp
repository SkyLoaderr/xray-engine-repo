//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "GameMtlLib.h"
#include "xr_trims.h"
#include "PropertiesListTypes.h"
#include "FolderLib.h"
#include "LeftBar.h"

//------------------------------------------------------------------------------
// material routines
//------------------------------------------------------------------------------
void SGameMtl::FillProp		(PropItemVec& items, TElTreeItem* owner)
{
    PHelper.CreateRName_TI	(items,	"Name",					&m_Name, owner);
    PHelper.CreateRText		(items,	"Desc",					&m_Desc);
	// flags
	PHelper.CreateFlag32	(items,	"Flags\\Dynamic",		&Flags,	flDynamic);
	PHelper.CreateFlag32	(items,	"Flags\\Passable",		&Flags,	flPassable);
    PHelper.CreateFlag32	(items,	"Flags\\Breakable",		&Flags,	flBreakable);
//.    PHelper.CreateFlag32	(items,	"Flags\\Shootable",		&Flags,	flShootable);
    PHelper.CreateFlag32	(items,	"Flags\\Bounceable",	&Flags,	flBounceable);
    PHelper.CreateFlag32	(items,	"Flags\\Skidmark",		&Flags,	flSkidmark);
    PHelper.CreateFlag32	(items,	"Flags\\Bloodmark",		&Flags,	flBloodmark);
    PHelper.CreateFlag32	(items,	"Flags\\Climbable",		&Flags,	flClimbable);
//.    PHelper.CreateFlag32	(items, "Flags\\Walk On",		&Flags, flWalkOn);
    // physics part
    PHelper.CreateFloat		(items,	"Physics\\Friction",		&fPHFriction,			0.f, 	100.f, 	0.001f, 3); 
    PHelper.CreateFloat		(items,	"Physics\\Damping",			&fPHDamping,			0.001f,	100.f, 	0.001f, 3); 
    PHelper.CreateFloat		(items,	"Physics\\Spring",			&fPHSpring,				0.001f,	100.f, 	0.001f, 3); 
    PHelper.CreateFloat		(items,	"Physics\\Bounce start vel",&fPHBounceStartVelocity,0.f,	100.f, 	0.01f, 	2); 
    PHelper.CreateFloat		(items,	"Physics\\Bouncing",		&fPHBouncing,			0.f,	1.f, 	0.001f, 3); 
    // factors
    PHelper.CreateFloat		(items,	"Factors\\Shoot",			&fShootFactor);
    PHelper.CreateFloat		(items,	"Factors\\Damage",			&fBounceDamageFactor,   0.f,100.f,0.1f,1);
    PHelper.CreateFloat		(items,	"Factors\\Transparency",	&fVisTransparencyFactor);
    PHelper.CreateFloat		(items,	"Factors\\Sound occlusion",	&fSndOcclusionFactor);
    PHelper.CreateFloat		(items,	"Factors\\Flotation",		&fFlotationFactor);
}
SGameMtl* CGameMtlLibrary::AppendMaterial(SGameMtl* parent)
{
    SGameMtl* M	= xr_new<SGameMtl>();
    if (parent)	*M=*parent;
    M->ID		= material_index++;
    materials.push_back			(M);
    return 		M;
}
void CGameMtlLibrary::RemoveMaterial(LPCSTR name)
{
    // find material
    GameMtlIt 	rem_it=GetMaterialIt(name);
    R_ASSERT	(rem_it!=materials.end());
    // remove dependent pairs
    RemoveMaterialPair((*rem_it)->GetID());
    // destroy material
    xr_delete		(*rem_it);
    materials.erase	(rem_it);
}
SGameMtl* CGameMtlLibrary::GetMaterialByID	(int id)
{
	GameMtlIt it = GetMaterialItByID(id);
    return it!=materials.end()?*it:0;
}

//------------------------------------------------------------------------------
// material pair routines
//------------------------------------------------------------------------------
void __fastcall SGameMtlPair::OnFlagChange(PropValue* sender)
{
	bool bChecked = sender->Owner()->m_Flags.is(PropItem::flCBChecked);
    u32 mask=0;
    if (sender==propBreakingSounds)			mask = flBreakingSounds;
    else if (sender==propStepSounds)		mask = flStepSounds;
    else if (sender==propCollideSounds)		mask = flCollideSounds;
    else if (sender==propCollideParticles)	mask = flCollideParticles;
    else if (sender==propCollideMarks)		mask = flCollideMarks;
    else THROW;

    OwnProps.set		(mask,bChecked);
    sender->Owner()->m_Flags.set	(PropItem::flDisabled,!bChecked);
}

IC u32 SetMask(u32 mask, Flags32 flags, u32 flag )
{
    return mask?(mask|(flags.is(flag)?PropItem::flCBChecked:PropItem::flDisabled)):0;
}

IC SGameMtlPair* GetParentValueObj(SGameMtlPair* obj, GameMtlPairVec& parents, u32 flag)
{
	int k 				= 0;
    do{
        if (obj->OwnProps.is(flag)){ break; }
        obj				= (k<(int)parents.size())?parents[k++]:0;
    }while(obj);
    return obj;
}

void SGameMtlPair::FillProp(PropItemVec& items)
{
	TOnChange OnChange	= 0;
    u32 show_CB			= 0;
    GameMtlPairVec parents;
    if (ID_parent!=-1){ 
       	OnChange 		= OnFlagChange;
        show_CB		    = PropItem::flShowCB;
		int cnt 		= m_Owner->GetParents(this,parents); R_ASSERT(cnt);
        PHelper.CreateCaption(items,"Parent", m_Owner->MtlPairToName((*parents.begin())->GetMtl0(),(*parents.begin())->GetMtl1()));
    }
    
	propBreakingSounds	= PHelper.CreateASoundSrc	(items,	"Braking Sounds",	&BreakingSounds);
	propStepSounds		= PHelper.CreateASoundSrc	(items,	"Step Sounds",		&StepSounds);
	propCollideSounds	= PHelper.CreateASoundSrc	(items,	"Collide Sounds",	&CollideSounds);
	propCollideParticles= PHelper.CreateALibPS		(items,	"Collide Particles",&CollideParticles);
	propCollideMarks	= PHelper.CreateATexture	(items,	"Collide Marks",	&CollideMarks);

    propBreakingSounds->Owner()->m_Flags.set	(SetMask(show_CB,OwnProps,flBreakingSounds));
    propStepSounds->Owner()->m_Flags.set		(SetMask(show_CB,OwnProps,flStepSounds));
    propCollideSounds->Owner()->m_Flags.set		(SetMask(show_CB,OwnProps,flCollideSounds));
    propCollideParticles->Owner()->m_Flags.set	(SetMask(show_CB,OwnProps,flCollideParticles));
    propCollideMarks->Owner()->m_Flags.set		(SetMask(show_CB,OwnProps,flCollideMarks));

    propBreakingSounds->OnChangeEvent			= OnChange;
    propStepSounds->OnChangeEvent				= OnChange;
    propCollideSounds->OnChangeEvent			= OnChange;
    propCollideParticles->OnChangeEvent			= OnChange;
    propCollideMarks->OnChangeEvent				= OnChange;

    propBreakingSounds->Owner()->subitem		= GAMEMTL_SUBITEM_COUNT;
    propStepSounds->Owner()->subitem			= GAMEMTL_SUBITEM_COUNT;
    propCollideSounds->Owner()->subitem			= GAMEMTL_SUBITEM_COUNT;
    propCollideParticles->Owner()->subitem		= GAMEMTL_SUBITEM_COUNT;
    propCollideMarks->Owner()->subitem			= GAMEMTL_SUBITEM_COUNT;
    
    if (show_CB){
		SGameMtlPair* O; 
    	if (0!=(O=GetParentValueObj(this,parents,flBreakingSounds)))	BreakingSounds	= O->BreakingSounds;
    	if (0!=(O=GetParentValueObj(this,parents,flStepSounds))) 		StepSounds		= O->StepSounds;
    	if (0!=(O=GetParentValueObj(this,parents,flCollideSounds))) 	CollideSounds	= O->CollideSounds;
    	if (0!=(O=GetParentValueObj(this,parents,flCollideParticles))) 	CollideParticles= O->CollideParticles;
    	if (0!=(O=GetParentValueObj(this,parents,flCollideMarks))) 		CollideMarks	= O->CollideMarks;
    }
}

void SGameMtlPair::TransferFromParent(SGameMtlPair* parent)
{
    R_ASSERT(parent);
    if (!OwnProps.is(flStepSounds))			BreakingSounds  = parent->BreakingSounds;
    if (!OwnProps.is(flBreakingSounds))		StepSounds		= parent->StepSounds;
    if (!OwnProps.is(flCollideSounds))		CollideSounds	= parent->CollideSounds;
    if (!OwnProps.is(flCollideParticles))	CollideParticles= parent->CollideParticles;
    if (!OwnProps.is(flCollideMarks))		CollideMarks	= parent->CollideMarks;
}

//------------------------------------------------------------------------------
// material library routines
//------------------------------------------------------------------------------
LPCSTR CGameMtlLibrary::MtlPairToName		(int mtl0, int mtl1)
{
    static string128 buf;
    SGameMtl* M0	= GetMaterialByID(mtl0);	R_ASSERT(M0);
    SGameMtl* M1	= GetMaterialByID(mtl1);	R_ASSERT(M1);
    sprintf			(buf,"%s # %s",*M0->m_Name,*M1->m_Name);
    _ChangeSymbol	(buf,'\\','/');
    return buf;
}
void CGameMtlLibrary::NameToMtlPair			(LPCSTR name, int& mtl0, int& mtl1)
{
    string64 buf0, buf1;
    sscanf(name,"%s # %s",buf0,buf1);
    _ChangeSymbol	(buf0,'/','\\');
    _ChangeSymbol	(buf1,'/','\\');
    SGameMtl* M0	= GetMaterial(buf0);	R_ASSERT(M0);	mtl0=M0->GetID();
    SGameMtl* M1	= GetMaterial(buf1);	R_ASSERT(M1);   mtl1=M1->GetID();
}
void CGameMtlLibrary::MtlNameToMtlPair		(LPCSTR name, int& mtl0, int& mtl1)
{
    string128 buf;
    SGameMtl* M0 	= GetMaterial(_GetItem(name,0,buf,','));	R_ASSERT(M0); 	mtl0=M0->GetID();
    SGameMtl* M1 	= GetMaterial(_GetItem(name,1,buf,','));	R_ASSERT(M1);	mtl1=M1->GetID();
}

int CGameMtlLibrary::GetParents(SGameMtlPair* obj, GameMtlPairVec& lst)
{
    do{
        GameMtlPairIt it = GetMaterialPairIt(obj->ID_parent); R_ASSERT(it!=LastMaterialPair());
   		obj = *it;
	    lst.push_back(obj);
    }while(obj&&(obj->ID_parent!=-1));
    return lst.size();
}

SGameMtlPair* CGameMtlLibrary::AppendMaterialPair(int m0, int m1, SGameMtlPair* parent)
{
    SGameMtlPair*	S = GetMaterialPair(m0,m1);
    if (!S){
        SGameMtlPair* M	= xr_new<SGameMtlPair>(this);
        if (parent){
            M->ID_parent = parent->ID;
            M->OwnProps.zero();
        }
        M->ID 		= material_pair_index++;
        M->SetPair	(m0,m1);
        material_pairs.push_back			(M);
        return 		M;
    }else{
        return 		S;
     }
}
void CGameMtlLibrary::RemoveMaterialPair(LPCSTR name)
{
    int mtl0,mtl1;
    NameToMtlPair	(name,mtl0,mtl1);
    RemoveMaterialPair(mtl0, mtl1);
}
void CGameMtlLibrary::RemoveMaterialPair(GameMtlPairIt rem_it)
{
    // delete parent dependent
    for (GameMtlPairIt it=material_pairs.begin(); it!=material_pairs.end(); it++)
        if ((*it)->ID_parent==(*rem_it)->ID){ 
            // transfer parented props to child
            (*it)->TransferFromParent(*rem_it);
            // reset parenting
            (*it)->ID_parent=-1;
        }
    // erase from list and remove physically
    xr_delete			(*rem_it);
    material_pairs.erase	(rem_it);
}
void CGameMtlLibrary::RemoveMaterialPair(int mtl)
{
    for (int i=0; i<(int)material_pairs.size(); i++){
        GameMtlPairIt it = material_pairs.begin()+i;
        if (((*it)->mtl0==mtl)||((*it)->mtl1==mtl)){
            RemoveMaterialPair(it);
            i--;
        }
    }
}
void CGameMtlLibrary::RemoveMaterialPair(int mtl0, int mtl1)
{
    GameMtlPairIt 	rem_it=GetMaterialPairIt(mtl0,mtl1);
    if (rem_it==material_pairs.end()) return;
    RemoveMaterialPair	(rem_it);
}
GameMtlPairIt CGameMtlLibrary::GetMaterialPairIt(int id)
{
    for (GameMtlPairIt it=material_pairs.begin(); it!=material_pairs.end(); it++)
        if ((*it)->ID==id) return it;
    return material_pairs.end();
}
SGameMtlPair* CGameMtlLibrary::GetMaterialPair(int id)
{
    GameMtlPairIt it=GetMaterialPairIt(id);
    return it!=material_pairs.end()?*it:0;
}
GameMtlPairIt CGameMtlLibrary::GetMaterialPairIt	(int mtl0, int mtl1)
{
    for (GameMtlPairIt it=material_pairs.begin(); it!=material_pairs.end(); it++)
        if ((*it)->IsPair(mtl0,mtl1)) return it;
    return material_pairs.end();
}
SGameMtlPair* CGameMtlLibrary::GetMaterialPair(int mtl0, int mtl1)
{
    GameMtlPairIt it=GetMaterialPairIt(mtl0, mtl1);
    return it!=material_pairs.end()?*it:0;
}
SGameMtlPair* CGameMtlLibrary::GetMaterialPair(LPCSTR name)
{
    if (name&&name[0]){
        int mtl0, mtl1;
        NameToMtlPair	(name,mtl0,mtl1);
        GameMtlPairIt it=GetMaterialPairIt(mtl0, mtl1);
        return it!=material_pairs.end()?*it:0;
    }
    return 0;
}

//------------------------------------------------------------------------------
// IO - routines
//------------------------------------------------------------------------------
void SGameMtl::Save(IWriter& fs)
{
	Flags.set				(flSlowDown,	fis_zero(fFlotationFactor-1.f,EPS_L));
	Flags.set				(flShootable,	fis_zero(fShootFactor,EPS_L));
	Flags.set				(flTransparent,	fis_zero(fVisTransparencyFactor,EPS_L));

	fs.open_chunk			(GAMEMTL_CHUNK_MAIN);
	fs.w_u32				(ID);
	fs.w_stringZ			(*m_Name);
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_DESC);
    fs.w_stringZ			(*m_Desc?*m_Desc:"");
    fs.close_chunk			();
    
	fs.open_chunk			(GAMEMTL_CHUNK_FLAGS);
    fs.w_u32				(Flags.get());
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_PHYSICS);
    fs.w_float				(fPHFriction);
    fs.w_float				(fPHDamping);
    fs.w_float				(fPHSpring);
    fs.w_float				(fPHBounceStartVelocity);
    fs.w_float				(fPHBouncing);
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_FACTORS);
    fs.w_float				(fShootFactor);
    fs.w_float				(fBounceDamageFactor);
    fs.w_float				(fVisTransparencyFactor);
    fs.w_float				(fSndOcclusionFactor);
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_FLOTATION);
    fs.w_float				(fFlotationFactor);
    fs.close_chunk			();
}

void SGameMtlPair::Load(IReader& fs)
{
	string128			buf;

	R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_PAIR));
    mtl0				= fs.r_u32();
    mtl1				= fs.r_u32();
    ID					= fs.r_u32();
    ID_parent			= fs.r_u32();
    OwnProps.set		(fs.r_u32());

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_BREAKING));
    fs.r_stringZ			(buf); 	BreakingSounds	= buf;
    
    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_STEP));
    fs.r_stringZ			(buf);	StepSounds		= buf;
    
    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_COLLIDE));
    fs.r_stringZ			(buf);	CollideSounds	= buf;
    fs.r_stringZ			(buf);	CollideParticles= buf;
    fs.r_stringZ			(buf);	CollideMarks	= buf;
}

void SGameMtlPair::Save(IWriter& fs)
{
    fs.open_chunk		(GAMEMTLPAIR_CHUNK_PAIR);
    fs.w_u32			(mtl0);
    fs.w_u32			(mtl1);
    fs.w_u32			(ID);
    fs.w_u32			(ID_parent);
    fs.w_u32			(OwnProps.get());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_BREAKING);
    fs.w_stringZ		(BreakingSounds.c_str());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_STEP);
    fs.w_stringZ		(StepSounds.c_str());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_COLLIDE);
    fs.w_stringZ		(CollideSounds.c_str());
    fs.w_stringZ		(CollideParticles.c_str());
    fs.w_stringZ		(CollideMarks.c_str());
	fs.close_chunk		();
}

void CGameMtlLibrary::Save()
{
	// save
	CMemoryWriter fs;
    fs.open_chunk		(GAMEMTLS_CHUNK_VERSION);
    fs.w_u16			(GAMEMTL_CURRENT_VERSION);
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLS_CHUNK_AUTOINC);
    fs.w_u32			(material_index);
    fs.w_u32			(material_pair_index);
	fs.close_chunk		();
    
    fs.open_chunk		(GAMEMTLS_CHUNK_MTLS);
    int count = 0;
    for(GameMtlIt m_it=materials.begin(); m_it!=materials.end(); m_it++){
        fs.open_chunk	(count++);
        (*m_it)->Save	(fs);
        fs.close_chunk	();
    }
	fs.close_chunk		();
                                                    
    fs.open_chunk		(GAMEMTLS_CHUNK_MTLS_PAIR);
    count = 0;
    for(GameMtlPairIt p_it=material_pairs.begin(); p_it!=material_pairs.end(); p_it++){
        fs.open_chunk	(count++);
        (*p_it)->Save	(fs);
        fs.close_chunk	();
    }
	fs.close_chunk		();

	AnsiString fn;
    FS.update_path		(fn,_game_data_,GAMEMTL_FILENAME);
    fs.save_to			(fn.c_str());
}

