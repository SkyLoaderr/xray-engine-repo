// file: MeshExpUtility.h


#ifndef __MeshExpUtility__H__INCLUDED__
#define __MeshExpUtility__H__INCLUDED__

#include "NetDeviceLog.h"
#include "MeshExpUtility.rh"

#define EXPORTER_VERSION	1
#define EXPORTER_BUILD		5
//using namespace std;

#define	EXP_UTILITY_CLASSID Class_ID(0x69107b71, 0x1d5644be)

class ExportItem {
public:
	INode *pNode;
	ExportItem(){ pNode = 0;};
	ExportItem( INode* _pNode ){ pNode = _pNode; };
	~ExportItem(){};
};

class MeshExpUtility : public UtilityObj {
public:

	IUtil		*iu;
	Interface	*ip;

	HWND		hPanel;
	HWND		hCtlList;
protected:
	char		m_ExportName[MAX_PATH];
	std::vector<ExportItem> m_Items;

	void		RegRead		();
	void		RegSave		();

	void		RefreshExportList();
	void		UpdateSelectionListBox();

	bool		SaveAsObject	(const char* n);
	bool		SaveAsSkin		(const char* n);
	bool		SaveSkinKeys	(const char* n);
public:
	int			m_ObjectFlipFaces;
	int			m_ObjectSuppressSmoothGroup;
	int			m_ObjectNoOptimize;

	int			m_SkinSuppressSmoothGroup;
	int			m_SkinProgressive;
public:
				MeshExpUtility	();
	virtual		~MeshExpUtility	();

	void		BeginEditParams	(Interface *ip,IUtil *iu);
	void		EndEditParams	(Interface *ip,IUtil *iu);
	void		SelectionSetChanged(Interface *ip,IUtil *iu);
	void		DeleteThis		() {}

	void		Init			(HWND hWnd);
	void		Destroy			(HWND hWnd);
	void		ExportAsObject	();
	void		ExportAsSkin	();
	void		ExportSkinKeys	();
};

extern MeshExpUtility U;



#endif /*__MeshExpUtility__H__INCLUDED__*/


