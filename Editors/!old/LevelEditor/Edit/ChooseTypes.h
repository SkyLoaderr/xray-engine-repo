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
    smGameObject,
    smGameMaterial,
    smGameAnim,
};

struct SChooseItem{
	AnsiString	name;
	AnsiString	hint;
    bool		bTHM;
    SChooseItem	(LPCSTR nm, LPCSTR ht, bool thm=false):name(nm),hint(ht),bTHM(thm){}
};
DEFINE_VECTOR(SChooseItem,ChooseItemVec,ChooseItemVecIt);

// refs
class EImageThumbnail;
// typedef
typedef void __fastcall (*TOnChooseFill)			(ChooseItemVec& lst);
typedef void __fastcall (*TOnChooseSelect)			(SChooseItem* item, EImageThumbnail*& thm, ref_sound& snd, PropItemVec& info_items);

typedef void __fastcall (*TOnChooseFillEvents)();

struct SChooseEvents{
	AnsiString			caption;
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
