//---------------------------------------------------------------------------
#ifndef SHEngineToolsH
#define SHEngineToolsH

#include "SHToolsInterface.h"
#include "PropertiesList.h"

DEFINE_VECTOR(CBlender*,TemplateVec,TemplateIt);
DEFINE_MAP_PRED(LPSTR,CConstant*,ConstantMap,ConstantPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CMatrix*,MatrixMap,MatrixPairIt,str_pred);
DEFINE_MAP_PRED(LPSTR,CBlender*,BlenderMap,BlenderPairIt,str_pred);

// refs
class CSHEngineTools;

class CParseBlender{
public:
	virtual void Parse(CSHEngineTools* owner, DWORD type, LPCSTR key, LPVOID data)=0;
};

class CSHEngineTools: public ISHTools
{
	CEditableObject*		m_EditObject;
    bool					m_bCustomEditObject;

	BOOL					m_bFreezeUpdate;
    BOOL					m_bNeedResetShaders;
    BOOL					m_RemoteRenBlender;
    AnsiString				m_RenBlenderOldName;
    AnsiString				m_RenBlenderNewName;

	TemplateVec				m_TemplatePalette;

	ConstantMap				m_OptConstants;
	MatrixMap				m_OptMatrices;
	ConstantMap				m_Constants;
	MatrixMap				m_Matrices;
	BlenderMap				m_Blenders;

	CBlender*				FindItem			(LPCSTR name);
    virtual LPCSTR			GenerateItemName	(LPSTR name, LPCSTR pref, LPCSTR source);

	void 					AddMatrixRef		(LPSTR name);
	CMatrix*				FindMatrix			(LPSTR name, bool bDuplicate);
    LPCSTR					GenerateMatrixName	(LPSTR name);
    LPCSTR					AppendMatrix		(CMatrix* src=0, CMatrix** dest=0);
    void					RemoveMatrix		(LPSTR name);   

	void 					AddConstantRef		(LPSTR name);
	CConstant*				FindConstant		(LPSTR name, bool bDuplicate);
    LPCSTR					GenerateConstantName(LPSTR name);
    LPCSTR					AppendConstant		(CConstant* src=0, CConstant** dest=0);
    void					RemoveConstant		(LPSTR name);

friend class CCollapseBlender;
friend class CRefsBlender;
friend class CRemoveBlender;
friend class TfrmShaderProperties;
    void					CollapseMatrix		(LPSTR name);
    void					CollapseConstant	(LPSTR name);
    void					CollapseReferences	();
    void					UpdateMatrixRefs	(LPSTR name);
    void					UpdateConstantRefs	(LPSTR name);
    void					UpdateRefCounters	();

    void 					ParseBlender		(CBlender* B, CParseBlender& P);

	CMemoryWriter			m_BlenderStream;	// пользоваться функциями обновления стрима для синхронизации
    bool 					m_bUpdateCurrent;	// если менялся объект непосредственно  Update____From___()
    bool					m_bCurBlenderChanged;

    void 					Save				(CMemoryWriter& F);
    void 					PrepareRender		();

    // matrix props
	void __fastcall 		ModeOnAfterEdit		(PropValue* sender, LPVOID edit_val);
	void __fastcall 		MatrixOnAfterEdit	(PropItem* sender, LPVOID edit_val);
	void __fastcall 		FillMatrixProps		(PropItemVec& items, LPCSTR pref, LPSTR name);
	void __fastcall 		MCOnDraw			(PropValue* sender, LPVOID draw_val);
    // constant props
	void __fastcall 		ConstOnAfterEdit	(PropItem* sender, LPVOID edit_val);
	void __fastcall 		FillConstProps		(PropItemVec& items, LPCSTR pref, LPSTR name);
    // name
	void __fastcall 		NameOnAfterEdit		(PropItem* sender, LPVOID edit_val);

    void					RealResetShaders	();

	void __fastcall 		FillMatrix			(PropItemVec& values, LPCSTR pref, CMatrix* m);
	void __fastcall 		FillConst			(PropItemVec& values, LPCSTR pref, CConstant* c);
    void __fastcall			RefreshProperties	();

    void					ResetShaders		(bool bForced=false){m_bNeedResetShaders=true; if (bForced) RealResetShaders(); }
    void					UpdateObjectShader	();
public:
	CMemoryWriter			m_RenderShaders;

    CBlender*				m_CurrentBlender;
	void					RemoteRenameBlender	(LPCSTR old_full_name, LPCSTR new_full_name){m_RemoteRenBlender=TRUE;m_RenBlenderOldName=old_full_name;m_RenBlenderNewName=new_full_name;}

    Shader_xrLC*			m_Shader;
    virtual LPCSTR			AppendItem			(LPCSTR folder_name, LPCSTR parent=0);
    virtual void			RemoveItem			(LPCSTR name);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR ren_part, int level);
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR new_full_name);
	virtual void			FillItemList		();

    void					UpdateStreamFromObject();
    void					UpdateObjectFromStream();

    void 					ClearData			();
	void __fastcall 		PreviewObjClick		(TObject *Sender);
public:
							CSHEngineTools		(ISHInit& init);
    virtual 				~CSHEngineTools		();

    virtual LPCSTR			ToolsName			(){return "Engine Shader";}

	virtual void			Reload				();
	virtual void			Load				();
	virtual void			Save				();

    virtual bool			OnCreate			();
    virtual void			OnDestroy			();
    virtual void			OnActivate			(){;}
    virtual void			OnDeactivate		(){;}

    // misc
    virtual void			ResetCurrentItem	();
    virtual void			SetCurrentItem		(LPCSTR name);
    virtual void			ApplyChanges		(bool bForced=false);

	virtual void 			UpdateProperties	();

    virtual void			OnFrame				();
	virtual void 			OnRender			();

    virtual void			OnDeviceCreate		();
    virtual void			OnDeviceDestroy		(){;}

    virtual void			ZoomObject			(bool bOnlySel);
	virtual void 			OnShowHint			(AStringVec& ss);
};
//---------------------------------------------------------------------------
#endif
