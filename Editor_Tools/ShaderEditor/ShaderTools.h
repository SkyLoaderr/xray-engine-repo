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

class CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data)=0;
};

class CShaderTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CLibObject*			m_LibObject;
	CEditableObject*	m_EditObject;
	BOOL				m_Modified;

	TemplateVec			m_TemplatePalette;

	ConstantMap			m_OptConstants;
	MatrixMap			m_OptMatrices;
	ConstantMap			m_Constants;
	MatrixMap			m_Matrices;
	BlenderMap			m_Blenders;

	CBlender*			FindBlender			(LPCSTR name);
    LPCSTR				GenerateBlenderName	(LPSTR name, LPCSTR source);
    void				RemoveBlender		(LPCSTR name);

	CMatrix*			FindMatrix			(LPCSTR name);
    LPCSTR				GenerateMatrixName	(LPSTR name);
    LPCSTR				AppendMatrix		(CMatrix* src=0);
    void				RemoveMatrix		(LPCSTR name);

	CConstant*			FindConstant		(LPCSTR name);
    LPCSTR				GenerateConstantName(LPSTR name);
    LPCSTR				AppendConstant		(CConstant* src=0);
    void				RemoveConstant		(LPCSTR name);

friend class CCollapseBlender;
friend class CExpandBlender;
friend class TfrmShaderProperties;
    void				CollapseMatrix		(LPSTR name);
    void				CollapseConstant	(LPSTR name);
    void				ExpandMatrix		(LPSTR name);
    void				ExpandConstant		(LPSTR name);
    void				ExpandReferences	();
    void				CollapseReferences	();

    void 				ParseBlender		(CBlender* B, CParseBlender& P);
public:
    CBlender*			m_CurrentBlender;
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
    CBlender*			AppendBlender	(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent);
    bool				GetCurrentBlender(CFS_Base& data);
    void				SetCurrentBlender(CStream& data);
};
extern CShaderTools	SHTools;
//---------------------------------------------------------------------------
#endif
