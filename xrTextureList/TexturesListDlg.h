// TexturesListDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "TextureList.h"
#include "Image.h"


// CTexturesListDlg dialog
class CTexturesListDlg : public CDialog
{
// Construction
public:
	CTexturesListDlg(CWnd* pParent = NULL);	// standard constructor
	~CTexturesListDlg();

	CRect m_textureRect;
	CRect m_cur_tex_rect;

// Dialog Data
	enum { IDD = IDD_TEXTURESLIST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	CTextureList m_texList;
	HICON m_hIcon;
	CImage m_texture;

	void UpdateList();
	void UpdatePicture();
	void xrLoadTexture();

	CRect GetDestRect();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_list;
	afx_msg void OnBnClickedBtnAdd();
	CEdit m_editTextureID;
	afx_msg void OnBnClickedLoad();
	CEdit m_editX;
	CEdit m_editY;
	CEdit m_editWidth;
	CEdit m_editHeight;
	afx_msg void OnLbnSelchangeList1();
	CEdit m_editTexName;
	CEdit m_editTexParams;
	afx_msg void OnBnClickedBtnRemove();
	afx_msg void OnBnClickedBtnRenameFile();
	CButton m_btnRename;
	CEdit m_editNewName;
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedBtnSaveas();
	CButton m_btnSave;
	afx_msg void OnLbnDblclkList1();
	CStatic m_pic;
	CButton m_checkFitImage;
	afx_msg void OnBnClickedCheck1();
	CEdit m_editSearch;
	afx_msg void OnEnChangeEdit1();
};
