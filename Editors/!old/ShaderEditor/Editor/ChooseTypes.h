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
	shared_str				name;
	shared_str				hint;
    SChooseItem	(LPCSTR nm, LPCSTR ht):name(nm),hint(ht){}
};
DEFINE_VECTOR(SChooseItem,ChooseItemVec,ChooseItemVecIt);

// typedef
typedef fastdelegate::FastDelegate2<ChooseItemVec&,void*>		TOnChooseFillItems;
typedef fastdelegate::FastDelegate2<SChooseItem*, PropItemVec&>	TOnChooseSelectItem;
typedef fastdelegate::FastDelegate3<LPCSTR, HDC, const Irect&>	TOnDrawThumbnail;
typedef fastdelegate::FastDelegate0								TOnChooseClose;

typedef void (*TOnChooseFillEvents)();

struct SChooseEvents{
	shared_str				caption;
    TOnChooseFillItems	on_fill;
    TOnChooseSelectItem	on_sel;
    TOnDrawThumbnail    on_thm;
    TOnChooseClose	    on_close;
    					SChooseEvents	(){caption="Select Item";}
    					SChooseEvents	(LPCSTR capt, TOnChooseFillItems f, TOnChooseSelectItem s, TOnDrawThumbnail t, TOnChooseClose c)
    {
    	Set				(capt,f,s,t,c);
    }
    void				Set				(LPCSTR capt, TOnChooseFillItems f, TOnChooseSelectItem s, TOnDrawThumbnail t, TOnChooseClose c)
    {
        caption			= capt;
        on_fill			= f;
        on_sel			= s;
        on_thm			= t;
        on_close		= c;
    }
};

#define NONE_CAPTION "<none>"

#endif
