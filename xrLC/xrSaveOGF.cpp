#include "stdafx.h"
#include "build.h"
#include "OGF_Face.h"
 
VBContainer				g_VB;
IBContainer				g_IB;
vector<string>			g_Strings;

int	RegisterString(string &T) {
	vector<string>::iterator W = find(g_Strings.begin(), g_Strings.end(), T);
	if (W!=g_Strings.end()) return W-g_Strings.begin();
	g_Strings.push_back(T);
	return g_Strings.size()-1;
}

void CBuild::SaveTREE(CFS_Base &fs)
{
	CFS_Memory MFS;

	Status				("Visuals...");
	fs.open_chunk		(fsL_VISUALS | CFS_CompressMark);
	for (vector<OGF_Base*>::iterator it = g_tree.begin(); it!=g_tree.end(); it++)
	{
		DWORD			idx = it-g_tree.begin();
		MFS.open_chunk	(idx);
		(*it)->Save		(MFS);
		MFS.close_chunk	();
		Progress		(float(idx)/float(g_tree.size()));
	}
	fs.write_compressed(MFS.pointer(),MFS.size());
	fs.close_chunk	();

	Status				("Geometry : vertices ...");
	MFS.clear();
	fs.open_chunk		(fsL_VBUFFERS | CFS_CompressMark);
	g_VB.Save			(MFS);
	fs.write_compressed	(MFS.pointer(),MFS.size());
	fs.close_chunk		();

	Status				("Geometry : indices ...");
	MFS.clear();
	fs.open_chunk		(fsL_IBUFFERS | CFS_CompressMark);
	g_IB.Save			(MFS);
	fs.write_compressed	(MFS.pointer(),MFS.size());
	fs.close_chunk		();

	Status				("String table...");
	fs.open_chunk		(fsL_STRINGS);
	fs.Wdword			(g_Strings.size());
	for (vector<string>::iterator T=g_Strings.begin(); T!=g_Strings.end(); T++)
		fs.write(T->c_str(),T->length()+1);
	fs.close_chunk		();
}
