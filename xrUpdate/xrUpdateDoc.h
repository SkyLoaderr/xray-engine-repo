// xrUpdateDoc.h : interface of the CxrUpdateDoc class
//


#pragma once
class CTask;

class CxrUpdateDoc : public CDocument
{
protected: // create from serialization only
	CxrUpdateDoc();
	DECLARE_DYNCREATE(CxrUpdateDoc)

// Attributes
public:
	CTask*			m_task;
// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CxrUpdateDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
protected:
	virtual BOOL SaveModified();
};


