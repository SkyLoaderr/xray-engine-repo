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
void SGameMtl::FillProp		(PropItemVec& items)
{
	PropValue* P=0;
    P=PHelper.CreateText	(items,	"Name",					name,	sizeof(name));
    P->SetEvents			(FHelper.NameAfterEdit,FHelper.NameBeforeEdit,Tools.SMaterial.OnMaterialNameChange);
    P->Owner()->OnDrawEvent	= FHelper.NameDraw;
    P->Owner()->tag			= (int)FHelper.FindObject(fraLeftBar->tvMaterial,name); VERIFY(P->Owner()->tag);
	// flags
    PHelper.CreateFlag32	(items,	"Flags\\Breakable",		&Flags,	flBreakable);
    PHelper.CreateFlag32	(items,	"Flags\\Shootable",		&Flags,	flShootable);
    PHelper.CreateFlag32	(items,	"Flags\\Bounceable",	&Flags,	flBounceable);
    PHelper.CreateFlag32	(items,	"Flags\\Skidmark",		&Flags,	flSkidmark);
    PHelper.CreateFlag32	(items,	"Flags\\Bloodmark",		&Flags,	flBloodmark);
    PHelper.CreateFlag32	(items,	"Flags\\Climbable",		&Flags,	flClimbable);
    PHelper.CreateFlag32	(items, "Flags\\Walk On",		&Flags, flWalkOn);
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
}
SGameMtl* CGameMtlLibrary::AppendMaterial(SGameMtl* parent)
{
    SGameMtl* M	= new SGameMtl		();
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
    _DELETE		(*rem_it);
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
    if (sender==propFlotation)			mask = flFlotation;
    else if (sender==propBreakingSounds)mask = flBreakingSounds;
    else if (sender==propStepSounds)	mask = flStepSounds;
    else if (sender==propCollideSounds)	mask = flCollideSounds;
    else if (sender==propHitSounds)		mask = flHitSounds;
    else if (sender==propHitParticles)	mask = flHitParticles;
    else if (sender==propHitMarks)		mask = flHitMarks;
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
    
	propFlotation		= PHelper.CreateFloat		(items,	"Flotation",		&fFlotation,	0.f,1.f,0.01f,2);
	propBreakingSounds	= PHelper.CreateALibSound	(items,	"Braking Sounds",	&BreakingSounds);
	propStepSounds		= PHelper.CreateALibSound	(items,	"Step Sounds",		&StepSounds);
	propCollideSounds	= PHelper.CreateALibSound	(items,	"Collide Sounds",	&CollideSounds);
	propHitSounds		= PHelper.CreateALibSound	(items,	"Hit Sounds",		&HitSounds);
	propHitParticles	= PHelper.CreateALibPS		(items,	"Hit Particles",	&HitParticles);
	propHitMarks		= PHelper.CreateATexture	(items,	"Hit Marks",		&HitMarks);

    propFlotation->Owner()->m_Flags.set		(SetMask(show_CB,OwnProps,flFlotation));
    propBreakingSounds->Owner()->m_Flags.set(SetMask(show_CB,OwnProps,flBreakingSounds));
    propStepSounds->Owner()->m_Flags.set	(SetMask(show_CB,OwnProps,flStepSounds));
    propCollideSounds->Owner()->m_Flags.set	(SetMask(show_CB,OwnProps,flCollideSounds));
    propHitSounds->Owner()->m_Flags.set		(SetMask(show_CB,OwnProps,flHitSounds));
    propHitParticles->Owner()->m_Flags.set	(SetMask(show_CB,OwnProps,flHitParticles));
    propHitMarks->Owner()->m_Flags.set		(SetMask(show_CB,OwnProps,flHitMarks));

    propFlotation->OnChangeEvent 		= OnChange;
    propBreakingSounds->OnChangeEvent 	= OnChange;
    propStepSounds->OnChangeEvent 		= OnChange;
    propCollideSounds->OnChangeEvent 	= OnChange;
    propHitSounds->OnChangeEvent 		= OnChange;
    propHitParticles->OnChangeEvent 	= OnChange;
    propHitMarks->OnChangeEvent 		= OnChange;

    propBreakingSounds->Owner()->subitem= GAMEMTL_SUBITEM_COUNT;
    propStepSounds->Owner()->subitem	= GAMEMTL_SUBITEM_COUNT;
    propCollideSounds->Owner()->subitem	= GAMEMTL_SUBITEM_COUNT;
    propHitSounds->Owner()->subitem		= GAMEMTL_SUBITEM_COUNT;
    propHitParticles->Owner()->subitem	= GAMEMTL_SUBITEM_COUNT;
    propHitMarks->Owner()->subitem		= GAMEMTL_SUBITEM_COUNT;
    
    if (show_CB){
		SGameMtlPair* O; 
    	if (O=GetParentValueObj(this,parents,flFlotation)) 		fFlotation		= O->fFlotation;
    	if (O=GetParentValueObj(this,parents,flBreakingSounds))	BreakingSounds	= O->BreakingSounds;
    	if (O=GetParentValueObj(this,parents,flStepSounds)) 	StepSounds		= O->StepSounds;
    	if (O=GetParentValueObj(this,parents,flCollideSounds)) 	CollideSounds	= O->CollideSounds;
    	if (O=GetParentValueObj(this,parents,flHitSounds)) 		HitSounds		= O->HitSounds;
    	if (O=GetParentValueObj(this,parents,flHitParticles)) 	HitParticles	= O->HitParticles;
    	if (O=GetParentValueObj(this,parents,flHitMarks)) 		HitMarks		= O->HitMarks;
    }
}

void SGameMtlPair::TransferFromParent(SGameMtlPair* parent)
{
    R_ASSERT(parent);
    if (!OwnProps.is(flFlotation))		fFlotation 		= parent->fFlotation;
    if (!OwnProps.is(flStepSounds))		BreakingSounds  = parent->BreakingSounds;
    if (!OwnProps.is(flBreakingSounds))	StepSounds		= parent->StepSounds;
    if (!OwnProps.is(flCollideSounds))	CollideSounds	= parent->CollideSounds;
    if (!OwnProps.is(flHitSounds))		HitSounds		= parent->HitSounds;
    if (!OwnProps.is(flHitParticles))	HitParticles	= parent->HitParticles;
    if (!OwnProps.is(flHitMarks))		HitMarks		= parent->HitMarks;
}

//------------------------------------------------------------------------------
// material library routines
//------------------------------------------------------------------------------
LPCSTR CGameMtlLibrary::MtlPairToName		(int mtl0, int mtl1)
{
    static string128 buf;
    SGameMtl* M0	= GetMaterialByID(mtl0);	R_ASSERT(M0);
    SGameMtl* M1	= GetMaterialByID(mtl1);	R_ASSERT(M1);
    sprintf			(buf,"%s # %s",M0->name,M1->name);
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
        SGameMtlPair* M	= new SGameMtlPair	(this);
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
    _DELETE			(*rem_it);
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
void SGameMtl::Save(CFS_Base& fs)
{
	fs.open_chunk			(GAMEMTL_CHUNK_MAIN);
	fs.Wdword				(ID);
	fs.WstringZ				(name);
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_FLAGS);
    fs.Wdword				(Flags.get());
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_PHYSICS);
    fs.Wfloat				(fPHFriction);
    fs.Wfloat				(fPHDamping);
    fs.Wfloat				(fPHSpring);
    fs.Wfloat				(fPHBounceStartVelocity);
    fs.Wfloat				(fPHBouncing);
    fs.close_chunk			();

	fs.open_chunk			(GAMEMTL_CHUNK_FACTORS);
    fs.Wfloat				(fShootFactor);
    fs.Wfloat				(fBounceDamageFactor);
    fs.Wfloat				(fVisTransparencyFactor);
    fs.Wfloat				(fSndOcclusionFactor);
    fs.close_chunk			();
}

void SGameMtlPair::Load(CStream& fs)
{
	string128			buf;

	R_ASSERT(fs.FindChunk(GAMEMTLPAIR_CHUNK_PAIR));
    mtl0				= fs.Rdword();
    mtl1				= fs.Rdword();
    ID					= fs.Rdword();
    ID_parent			= fs.Rdword();
    OwnProps.set		(fs.Rdword());

    R_ASSERT(fs.FindChunk(GAMEMTLPAIR_CHUNK_FLOTATION));
    fFlotation			= fs.Rfloat();

    R_ASSERT(fs.FindChunk(GAMEMTLPAIR_CHUNK_BREAKING));
    fs.RstringZ			(buf); 	BreakingSounds	= buf;
    
    R_ASSERT(fs.FindChunk(GAMEMTLPAIR_CHUNK_STEP));
    fs.RstringZ			(buf);	StepSounds		= buf;
    
    R_ASSERT(fs.FindChunk(GAMEMTLPAIR_CHUNK_COLLIDE));
    fs.RstringZ			(buf);	CollideSounds	= buf;
    
    R_ASSERT(fs.FindChunk(GAMEMTLPAIR_CHUNK_HIT));
    fs.RstringZ			(buf);	HitSounds		= buf;
    fs.RstringZ			(buf);	HitParticles	= buf;
    fs.RstringZ			(buf);	HitMarks		= buf;
}

void SGameMtlPair::Save(CFS_Base& fs)
{
    fs.open_chunk		(GAMEMTLPAIR_CHUNK_PAIR);
    fs.Wdword			(mtl0);
    fs.Wdword			(mtl1);
    fs.Wdword			(ID);
    fs.Wdword			(ID_parent);
    fs.Wdword			(OwnProps.get());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_FLOTATION);
    fs.Wfloat			(fFlotation);
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_BREAKING);
    fs.WstringZ			(BreakingSounds.c_str());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_STEP);
    fs.WstringZ			(StepSounds.c_str());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_COLLIDE);
    fs.WstringZ			(CollideSounds.c_str());
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLPAIR_CHUNK_HIT);
    fs.WstringZ			(HitSounds.c_str());
    fs.WstringZ			(HitParticles.c_str());
    fs.WstringZ			(HitMarks.c_str());
	fs.close_chunk		();
}

void CGameMtlLibrary::Save(LPCSTR name)
{
	// save
	CFS_Memory fs;
    fs.open_chunk		(GAMEMTLS_CHUNK_VERSION);
    fs.Wword			(GAMEMTL_CURRENT_VERSION);
	fs.close_chunk		();

    fs.open_chunk		(GAMEMTLS_CHUNK_AUTOINC);
    fs.Wdword			(material_index);
    fs.Wdword			(material_pair_index);
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

    fs.SaveTo			(name,0);
}

