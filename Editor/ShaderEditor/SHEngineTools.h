//---------------------------------------------------------------------------
#ifndef SHEngineToolsH
#define SHEngineToolsH

#include "ElTree.hpp"

// refs
class PropValue; 

DEFINE_VECTOR(CBlender*,TemplateVec,TemplateIt);
DEFINE_MAP_PRED(LPSTR,CConstant*,ConstantMap,ConstantPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CMatrix*,MatrixMap,MatrixPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CBlender*,BlenderMap,BlenderPairIt,str_pred);

class CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data)=0;
};

class CSHEngineTools
{
	BOOL				m_bFreezeUpdate;
	BOOL				m_bModified;
    BOOL				

	TemplateVec			m_TemplatePalette;

	ConstantMap			m_OptConstants;
	MatrixMap			m_OptMatrices;
	ConstantMap			m_Constants;
	MatrixMap			m_Matrices;
	BlenderMap			m_Blenders;

	CBlender*			FindBlender			(LPCSTR name);
    LPCSTR				GenerateBlenderName	(LPSTR name, LPCSTR source);

	void 				AddMatrixRef		(LPSTR name);
	CMatrix*			FindMatrix			(LPSTR name, bool bDuplicate);
    LPCSTR				GenerateMatrixName	(LPSTR name);
    LPCSTR				AppendMatrix		(CMatrix* src=0, CMatrix** dest=0);
    void				RemoveMatrix		(LPSTR name);

	void 				AddConstantRef		(LPSTR name);
	CConstant*			FindConstant		(LPSTR name, bool bDuplicate);
    LPCSTR				GenerateConstantName(LPSTR name);
    LPCSTR				AppendConstant		(CConstant* src=0, CConstant** dest=0);
    void				RemoveConstant		(LPSTR name);

friend class CCollapseBlender;
friend class CRefsBlender;
friend class CRemoveBlender;
friend class TfrmShaderProperties;
    void				CollapseMatrix		(LPSTR name);
    void				CollapseConstant	(LPSTR name);
    void				CollapseReferences	();
    void				UpdateMatrixRefs	(LPSTR name);
    void				UpdateConstantRefs	(LPSTR name);
    void				UpdateRefCounters	();

    void 				ParseBlender		(CBlender* B, CParseBlender& P);

	CFS_Memory 			m_BlenderStream;	// ������������ ��������� ���������� ������ ��� �������������
    bool 				m_bUpdateCurrent;	// ���� ������� ������ ���������������  Update____From___()
    bool				m_bCurBlenderChanged;

    void 				Save				(CFS_Memory& F);
    void 				PrepareRender		();

    // matrix props
	void __fastcall 	ModeOnAfterEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	MatrixOnAfterEdit	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	AddMatrixProps		(TElTreeItem* parent, LPSTR name);
	void __fastcall 	RemoveMatrixProps	(TElTreeItem* parent);
	void __fastcall 	UpdateMatrixModeProps(TElTreeItem* parent, CMatrix* M, DWORD mode);
	void __fastcall 	MCOnDraw			(PropValue* sender, LPVOID draw_val);
    // constant props
	void __fastcall 	ConstOnAfterEdit	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	AddConstProps		(TElTreeItem* parent, LPSTR name);
	void __fastcall 	RemoveConstProps	(TElTreeItem* parent);
public:
	CFS_Memory 			m_RenderShaders;

    CBlender*			m_CurrentBlender;
    CBlender*			AppendBlender		(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent);
    CBlender* 			CloneBlender		(LPCSTR name);
    void				RemoveBlender		(LPCSTR name);
	void				RenameBlender		(LPCSTR old_full_name, LPCSTR ren_part, int level);
	void				RenameBlender		(LPCSTR old_full_name, LPCSTR new_full_name);

    void				ResetCurrentBlender();
    void				UpdateStreamFromObject();
    void				UpdateObjectFromStream();

    void 				ClearData			();
public:
						CSHEngineTools		();
    virtual 			~CSHEngineTools		();

	void				Reload				();
	void				Load				();
	void				Save				();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void __fastcall		Modified			();

    void				OnCreate			();
    void				OnDestroy			();

    void				UpdateDeviceShaders	();

    // misc
    void				SetCurrentBlender	(CBlender* B, bool bApply=true);
    void				SetCurrentBlender	(LPCSTR name, bool bApply=true);
    void				ApplyChanges		(bool bForced=false);

	void 				UpdateProperties	();
};
//---------------------------------------------------------------------------
#endif
