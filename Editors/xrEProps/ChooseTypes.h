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
typedef fastdelegate::FastDelegate1<ChooseItemVec&> 											TOnChooseFill;
typedef fastdelegate::FastDelegate4<SChooseItem*, ECustomThumbnail*&, ref_sound&, PropItemVec&>	TOnChooseSelect;

typedef void (*TOnChooseFillEvents)();

struct SChooseEvents{
	ref_str				caption;
    TOnChooseFill 		on_fill;
    TOnChooseSelect 	on_sel;
    bool				thm;
    SChooseEvents(LPCSTR capt, TOnChooseFill f, TOnChooseSelect s, bool _thm):caption(capt),on_fill(f),on_sel(s),thm(_thm)
    {
    	VERIFY(on_fill);
    }
};

#define NONE_CAPTION "<none>"

#endif
