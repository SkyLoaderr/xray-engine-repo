//---------------------------------------------------------------------------
#ifndef UI_ToolsCustomH
#define UI_ToolsCustomH

#include "eltree.hpp"
#include "mxplacemnt.hpp"
#include "ItemListHelper.h"
#include "EditObject.h"
#include "SkeletonCustom.h"

// refs
class TProperties;
class CEditableObject;
class PropValue;
class TfrmKeyBar;
class CBlend;

enum ETAction{
    etaSelect=0,
    etaAdd,
    etaMove,
    etaRotate,
    etaScale,
    etaMaxActions
};

enum ETAxis{
    etAxisX=0,
	etAxisY,
    etAxisZ,
    etAxisZX
};

enum ETFlags{
	etfCSParent    	= (1<<0),
	etfNUScale      = (1<<1),
	etfNormalAlign  = (1<<2),
	etfGSnap      	= (1<<3),
	etfOSnap      	= (1<<4),
	etfMTSnap      	= (1<<5),
	etfVSnap      	= (1<<6),
	etfASnap      	= (1<<7),
	etfMSnap      	= (1<<8),
};

class ECORE_API CToolsCustom//: public pureDeviceCreate, public pureDeviceDestroy
{
	friend class CEditorPreferences;
protected:
    bool				m_bReady;

    ETAction			m_Action;
    ETAxis				m_Axis;
    Flags32				m_Settings;
    
    bool				m_bHiddenMode;
	// move
    Fvector				m_MoveXVector;
    Fvector				m_MoveYVector;
    Fvector				m_MoveReminder;
    Fvector				m_MoveAmount;
	// scale
    Fvector				m_ScaleCenter;
    Fvector				m_ScaleAmount;
    // rotate
    Fvector				m_RotateCenter;
    Fvector				m_RotateVector;
    float				m_fRotateSnapValue;
    float				m_RotateAmount;
public:
    float				m_MoveSnap;
    float				m_MoveSnapTo;
    float				m_RotateSnapAngle;
public:
    float 				fFogness;
    u32					dwFogColor;
public:
	AnsiString 			m_LastFileName;
public:
						CToolsCustom 		();
    virtual 			~CToolsCustom		();

    ETAction			GetAction			()						{return m_Action;}				
    ETAxis				GetAxis				()						{return m_Axis;}				
    BOOL				GetSettings			(u32 mask)				{return m_Settings.is(mask);}				
    virtual void		SetAction			(ETAction act);
    virtual void		SetAxis				(ETAxis axis);
    virtual void		SetSettings			(u32 mask, BOOL val);
    
	virtual void		SetFog				(u32 fog_color, float fogness){dwFogColor=fog_color;fFogness=fogness;}
    virtual void		GetCurrentFog		(u32& fog_color, float& s_fog, float& e_fog);
    
    virtual void		Render				()=0;
	virtual void		RenderEnvironment	()=0;
    virtual void		OnFrame				()=0;

    virtual bool		OnCreate			()=0;
    virtual void		OnDestroy			()=0;

    virtual bool		IfModified			()=0;
    virtual bool		IsModified			()=0;
    virtual void		Modified			()=0; 

    virtual LPCSTR		GetInfo				()=0;
    
    virtual void		ZoomObject			(bool bSelOnly)=0;

    virtual bool		Load				(LPCSTR path, LPCSTR name)=0;
    virtual bool		Save				(LPCSTR path, LPCSTR name, bool bInternal=false)=0;
    virtual void		Reload				()=0;
    
    virtual void		OnDeviceCreate		()=0;
    virtual void		OnDeviceDestroy		()=0;

    virtual void		Clear				()=0;

    virtual void		OnShowHint			(AStringVec& SS)=0;

    virtual bool __fastcall 	MouseStart 	(TShiftState Shift)=0;
    virtual bool __fastcall 	MouseEnd   	(TShiftState Shift)=0;
    virtual void __fastcall 	MouseMove  	(TShiftState Shift)=0;
	virtual bool __fastcall 	HiddenMode 	(){return m_bHiddenMode;}
    virtual bool __fastcall 	KeyDown    	(WORD Key, TShiftState Shift){return false;}
    virtual bool __fastcall 	KeyUp      	(WORD Key, TShiftState Shift){return false;}
    virtual bool __fastcall 	KeyPress   	(WORD Key, TShiftState Shift){return false;}

    virtual bool		Pick				(TShiftState Shift)=0;
	virtual bool 		RayPick				(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt=0, Fvector* n=0)=0;

    virtual void		ShowProperties		()=0;
    virtual void		UpdateProperties	(bool bForced=false)=0;
    virtual void		RefreshProperties	()=0;

    const AnsiString&	GetEditFileName		()	{ return m_LastFileName; }
};
extern ECORE_API CToolsCustom*	Tools;

#define SURFACES_PREFIX "Surfaces"
#define BONES_PREFIX 	"Bones"
#define MOTIONS_PREFIX 	"Motions"
#define OBJECT_PREFIX 	"Object"
//---------------------------------------------------------------------------
#endif
