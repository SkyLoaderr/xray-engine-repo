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
    smEntity,
    smLAnim,
    smVisual,
    smSkeletonAnims,
    smSkeletonBones,  
    smGameMaterial,
    smGameAnim,
    smGameSMotions,
};

struct SChooseItem{
	ref_str		name;
	ref_str		hint;
    bool		bTHM;
    SChooseItem	(LPCSTR nm, LPCSTR ht, bool thm=false):name(nm),hint(ht),bTHM(thm){}
};
DEFINE_VECTOR(SChooseItem,ChooseItemVec,ChooseItemVecIt);

// refs
class ECustomThumbnail;
// typedef
typedef fastdelegate::FastDelegate2<ChooseItemVec&,void*>										TOnChooseFillItems;
typedef fastdelegate::FastDelegate4<SChooseItem*, ECustomThumbnail*&, ref_sound&, PropItemVec&>	TOnChooseSelectItem;

typedef void (*TOnChooseFillEvents)();

struct SChooseEvents{
	ref_str				caption;
    TOnChooseFillItems	on_fill;
    TOnChooseSelectItem	on_sel;
    bool				thm;
    SChooseEvents(LPCSTR capt, TOnChooseFillItems f, TOnChooseSelectItem s, bool _thm):caption(capt),on_fill(f),on_sel(s),thm(_thm)
    {
    	VERIFY(on_fill);
    }
};

#define NONE_CAPTION "<none>"

#endif
