// WallmarksEngine.h: interface for the CWallmarksEngine class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

class ENGINE_API	CWallmarksEngine
{
public:
	class  wallmark 
	{
	public:
		Fsphere				bounds;
		Shader*				shader;
		vector<FVF::LIT>	verts;
		float				ttl;
	};
private:
	vector<wallmark*>				pool;
	vector<wallmark*>				marks;
	CVertexStream*					VS;

	void		BuildMatrix			(Fmatrix &dest, float invsz, const Fvector& from);

	wallmark*	wm_allocate			(Shader*	S	);
	void		wm_render			(wallmark*	W, FVF::LIT* &V);
	void		wm_destroy			(wallmark*	W	);
public:
	void		AddWallmark			(CDB::TRI* tri, const Fvector &contact_point, Shader* hTexture, float sz);
	void		Render				();

	CWallmarksEngine				();
	~CWallmarksEngine				();
};
