//---------------------------------------------------------------------------
#ifndef SHMapH
#define SHMapH
//---------------------------------------------------------------------------
#include "SceneObject.h"

struct LItem{
    ETexture*       texture;
    int             iSizeX,iSizeY;  // slot unit
    float           fRotate;
    int             iFirstSlot;     // # first slot
};

#define MAX_LAYER  6
typedef vector<LItem> Layer;

class SHMap : public SceneObject {
    friend class    TfraSelectHMap;
    BYTE*           HMap;
    int             width, height;
    float           fDimAlt;
    IVector         vOffset;
    float           fSlotSize;
protected:
    Layer           layers[MAX_LAYER];
public:
	SHMap           ( );
	virtual ~SHMap  ( );
    void Create     ( TPicture* pct );
    void Clear      ( );
	__inline float	GetAlt(int x, int y) { return (float(HMap[y*width+x])/255)*fDimAlt+vOffset.y;}
};

#endif
