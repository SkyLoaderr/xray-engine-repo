//---------------------------------------------------------------------------
#ifndef ActorToolsH
#define ActorToolsH

// refs
class CEditableObject;

class CActorTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CEditableObject*	m_EditObject;
	BOOL				m_Modified;
public:
						CActorTools		();
    virtual 			~CActorTools	();

    bool				Load			(LPCSTR name);
    bool				Save			(LPCSTR name);

    void				Render			();
    void				Update			();

    bool				IfModified		();
    bool				IsModified		(){return m_Modified;}

    void				Init			();
    void				Clear			();

    void				ZoomObject		();

    virtual void		OnDeviceCreate	();
    virtual void		OnDeviceDestroy	();
};

//---------------------------------------------------------------------------
#endif
