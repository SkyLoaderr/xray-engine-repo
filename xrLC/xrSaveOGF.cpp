#include "stdafx.h"
#include "build.h"
#include "OGF_Face.h"
 
VBContainer				g_VB;
vector<string>			g_T;
vector<string>			g_S;

int	RegisterTexture(string &T) {
	vector<string>::iterator W = find(g_T.begin(), g_T.end(), T);
	if (W!=g_T.end()) return W-g_T.begin();
	g_T.push_back(T);
	return g_T.size()-1;
}
int	RegisterShader(string &S) {
	vector<string>::iterator W = find(g_S.begin(), g_S.end(), S);
	if (W!=g_S.end()) return W-g_S.begin();
	g_S.push_back(S);
	return g_S.size()-1;
}

void CBuild::SaveTREE(CFS_Base &fs)
{
	CFS_Memory MFS;

	Status("Visuals...");
	fs.open_chunk(fsL_VISUALS | CFS_CompressMark);
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

	Status("Geometry...");
	MFS.clear();
	fs.open_chunk	(fsL_VBUFFERS | CFS_CompressMark);
	g_VB.Save		(MFS);
	fs.write_compressed(MFS.pointer(),MFS.size());
	fs.close_chunk	();

	Status("Texture links...");
	fs.open_chunk	(fsL_TEXTURES);
	fs.Wdword		(g_T.size());
	for (vector<string>::iterator T=g_T.begin(); T!=g_T.end(); T++)
		fs.write(T->c_str(),T->length()+1);
	fs.close_chunk	();

	Status("Shader links...");
	fs.open_chunk	(fsL_SHADERS);
	fs.Wdword		(g_S.size());
	for (vector<string>::iterator S=g_S.begin(); S!=g_S.end(); S++)
		fs.write(S->c_str(),S->length()+1);
	fs.close_chunk	();

	g_T.clear		();
	g_S.clear		();
}
