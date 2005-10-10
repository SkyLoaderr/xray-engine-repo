//************************************************************************** 
//* OGFexp.h	- OGF File Exporter
//* 
//* By Christer Janson
//* Kinetix Development
//*
//* January 20, 1997 CCJ Initial coding
//*
//* Class definition 
//*
//* Copyright (c) 1997, All Rights Reserved. 
//***************************************************************************

#ifndef __OGFEXP__H
#define __OGFEXP__H

#include "Max.h"
#include "resource.h"
#include "istdplug.h"
#include "stdmat.h"
#include "decomp.h"
#include "shape.h"
#include "interpik.h"

//STL
#include <list>

extern ClassDesc* GetOGFExpDesc();
extern TCHAR *GetString(int id);
extern HINSTANCE hInstance;
extern void DMsg( char *format, ... );

class ExpItem
{
public:
	INode *pNode;
	ExpItem(){pNode = 0;};
	ExpItem( INode* _pNode ){ pNode = _pNode; };
	~ExpItem(){};
};

// This is the main class for the exporter.

class OGFExp : public SceneExport {
protected:
	char	filename[MAX_PATH];
	std::list<ExpItem> exportlist;
	std::vector<int> copylist;
public:
	Interface	*ip;
	HWND		hDialog;

public:
			OGFExp					();
			~OGFExp					();

// SceneExport methods
	int		ExtCount				();			// Number of extensions supported 
	const TCHAR * Ext				(int n);    // Extension #n (i.e. "ASC")
	const TCHAR * LongDesc			();			// Long OGF description (i.e. "OGF Export") 
	const TCHAR * ShortDesc			();			// Short OGF description (i.e. "OGF")
	const TCHAR * AuthorName		();			// OGF Author name
	const TCHAR * CopyrightMessage	();			// OGF Copyright message 
	const TCHAR * OtherMessage1		();			// Other message #1
	const TCHAR * OtherMessage2		();			// Other message #2
	unsigned int Version			();		    // Version number * 100 (i.e. v3.01 = 301) 
	void	ShowAbout				(HWND hWnd);  // Show DLL's "About..." box
	int		DoExport				(const TCHAR *name,ExpInterface *ei,Interface *i, BOOL suppressPrompts=FALSE, DWORD options=0); // Export	file
	BOOL	SupportsOptions			(int ext, DWORD options);

//---------------------------------------------------------------------------
	void	Init					(HWND hw);
	void	Destroy					();
	void	AppendItem				(INode *pNode);
	void	ClearLists				();

	
	bool	GetSelectItem			();
	bool	IsItemSelected			(LPSTR name);

	bool	PrepareExport			();
	bool	RunExport				();

	TSTR	GetIndent				(int indentLevel);
	BOOL	nodeEnum				(INode* node, int indentLevel);

	TriObject *ExtractTriObject		( INode *node );
	TriObject *GetTriObjectFromNode	(INode *node, int &deleteIt);
};

#endif // __OGFEXP__H

