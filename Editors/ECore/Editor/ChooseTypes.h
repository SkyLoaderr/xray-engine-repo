#ifndef ChooseTypesH
#define ChooseTypesH
 
enum EChooseMode{
    smCustom = 0,
    smSoundSource,
    smSoundEnv,
    smObject,
    smEShader,
    smCShader,
//        smPS,
    smPE,
    smParticles,
    smTexture,
    smEntity,
    smLAnim,
    smGameObject,
    smGameMaterial,
    smGameAnim,
    smMaxMode
};

struct SChooseItem{
	AnsiString	name;
	AnsiString	hint;
    SChooseItem(LPCSTR nm, LPCSTR ht):name(nm),hint(ht){}
};
DEFINE_VECTOR(SChooseItem,ChooseItemVec,ChooseItemVecIt);
 
#endif