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
    struct ERR{
	    struct Face{
    		Fvector		p[3];
            u32			c;
            bool		i;
	    };
	    struct Line{
    		Fvector		p[2];
            u32			c;
            bool		i;
	    };
	    struct Point{
    		Fvector		p[1];
            u32			c;
            bool		i;
	    };
	    DEFINE_VECTOR(Face,FaceVec,FaceIt);
	    DEFINE_VECTOR(Line,LineVec,LineIt);
	    DEFINE_VECTOR(Point,PointVec,PointIt);
		DEFINE_VECTOR(Fobb,OBBVec,OBBVecIt);
    	FaceVec			m_Faces;
	    LineVec			m_Lines;
	    PointVec		m_Points;
		OBBVec 			m_OBB;
        void			Clear()
        {
        	m_Faces.clear	();
	    	m_Lines.clear	();
	    	m_Points.clear	();
            m_OBB.clear		();
        }
        void AppendPoint(const Fvector& p0, u32 c=0xff0000ff, bool i=true)
        {
        	m_Points.push_back(Point());
            m_Points.back().p[0].set(p0);
            m_Points.back().c	= c;
            m_Points.back().i	= i;
        }
        void AppendLine	(const Fvector& p0, const Fvector& p1, u32 c=0xff00ff00, bool i=true)
        {
        	m_Lines.push_back(Line());
        	m_Lines.back().p[0].set(p0);
        	m_Lines.back().p[1].set(p1);
            m_Lines.back().c	= c;
            m_Lines.back().i	= i;
        }
        void AppendFace	(const Fvector& p0, const Fvector& p1, const Fvector& p2, u32 c=0xffff0000, bool i=true)
        {
        	m_Faces.push_back(Face());
        	m_Faces.back().p[0].set(p0);
        	m_Faces.back().p[1].set(p1);
        	m_Faces.back().p[2].set(p2);
            m_Faces.back().c	= c;
            m_Faces.back().i	= i;
        }
        void AppendOBB	(const Fobb& obb)
        {
        	m_OBB.push_back(obb);
        }
    };
    ERR					m_Errors;
public:
    void 				ClearErrors			(){m_Errors.Clear();}
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
