//---------------------------------------------------------------------------

#ifndef Unit2H
#define Unit2H
//---------------------------------------------------------------------------
class SceneObject;
struct strDX;
class TObjectPreview: public TThread
{
    strDX*  m_DX;
    SceneObject*    object;
    IAABox bbox;

    IMatrix m_Projection;
    IMatrix m_World;
	IMatrix m_Camera;
public:
    __fastcall TObjectPreview           (bool CreateSuspended, TPanel* p);
    virtual __fastcall ~TObjectPreview  ();

    void _fastcall SelectObject         (SceneObject* obj);
    virtual void __fastcall Execute     (void);
};

#endif
 