#ifndef ChooseTypesH
#define ChooseTypesH

enum EChooseMode{
    smCustom = 0,
    smSoundSource,
    smSoundEnv,
    smObject,
    smEShader,
    smCShader,
    smPE,
    smParticles,
    smTexture,
    smEntityType,
    smSpawnItem,
    smLAnim,
    smVisual,
    smSkeletonAnims,
    smSkeletonBones,  
    smGameMaterial,
    smGameAnim,
    smGameSMotions,
};

struct SChooseItem{
	ref_str				name;
	ref_str				hint;
    SChooseItem	(LPCSTR nm, LPCSTR ht):name(nm),hint(ht){}
};
DEFINE_VECTOR(SChooseItem,ChooseItemVec,ChooseItemVecIt);

// typedef
typedef fastdelegate::FastDelegate2<ChooseItemVec&,void*>		TOnChooseFillItems;
typedef fastdelegate::FastDelegate2<SChooseItem*, PropItemVec&>	TOnChooseSelectItem;
typedef fastdelegate::FastDelegate3<LPCSTR, HDC, const Irect&>	TOnDrawThumbnail;

typedef void (*TOnChooseFillEvents)();

struct SChooseEvents{
	ref_str				caption;
    TOnChooseFillItems	on_fill;
    TOnChooseSelectItem	on_sel;
    TOnDrawThumbnail    on_thm;
    SChooseEvents(LPCSTR capt, TOnChooseFillItems f, TOnChooseSelectItem s, TOnDrawThumbnail t):caption(capt),on_fill(f),on_sel(s),on_thm(t)
    {
    	VERIFY(!on_fill.empty());
    }
};

#define NONE_CAPTION "<none>"

#endif
