//---------------------------------------------------------------------------
#ifndef ShaderToolsH
#define ShaderToolsH

// refs
class CEditableObject;
class CLibObject;
class CBlender;

struct str_pred : public binary_function<char*, char*, bool>
{
    IC bool operator()(LPCSTR x, LPCSTR y) const
    {	return strcmp(x,y)<0;	}
};
DEFINE_VECTOR(CBlender*,TemplateVec,TemplateIt);
DEFINE_MAP_PRED(LPSTR,CConstant*,ConstantMap,ConstantPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CMatrix*,MatrixMap,MatrixPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CBlender*,BlenderMap,BlenderPairIt,str_pred);

class CShaderTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CLibObject*			m_LibObject;
	CEditableObject*	m_EditObject;
	BOOL				m_Modified;

	TemplateVec			m_TemplatePalette;

	ConstantMap			m_Constants;
	MatrixMap			m_Matrices;
	BlenderMap			m_Blenders;

    LPCSTR				GenerateBlenderName(LPSTR name, LPCSTR source);
public:
    CBlender*			m_CurrentBlender;
	CBlender*			FindBlenderByName(LPCSTR name);
public:
						CShaderTools	();
    virtual 			~CShaderTools	();

//    bool				Save			(LPCSTR name);

	void				Load			();
	void				Save			();

    void				Render			();
    void				Update			();

    bool				IfModified		();
    bool				IsModified		(){return m_Modified;}

    void				OnCreate		();
    void				OnDestroy		();

    void				ZoomObject		();

    virtual void		OnDeviceCreate	();
    virtual void		OnDeviceDestroy	();

    void				SelectPreviewObject(int p);
    void				ResetPreviewObject();

    // misc
    void				SetCurrentBlender(CBlender* B);
    CBlender*			AppendBlender	(CLASS_ID cls_id, CBlender* parent);
    bool				GetCurrentBlender(CFS_Base& data);
    void				SetCurrentBlender(CStream& data);
};
extern CShaderTools	SHTools;
//---------------------------------------------------------------------------
#endif
