// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOM_H__94471B51_E215_4C67_B753_B02F66D79911__INCLUDED_)
#define AFX_HOM_H__94471B51_E215_4C67_B753_B02F66D79911__INCLUDED_
#pragma once

class ENGINE_API occTri;

class ENGINE_API CHOM  
{
private:
	CDB::MODEL*				m_pModel;
	occTri*					m_pTris;
	vector<occTri*>			m_ZB;

	CVS*					m_VS;

//	IDirect3DTexture8*		m_pDBG;
//	CTexture*				pTexture;
//	Shader*					pShader;
//	CVertexStream*			pStream;
	
	void					Render_DB	(CFrustum& base);
public:
	void					Load		();
	void					Unload		();
	void					Render		(CFrustum& base);
	void					Render_ZB	();
	void					Debug		();

	BOOL					visible		(Fbox& B);
	BOOL					visible		(sPoly& P);
	
	CHOM();
	~CHOM();
};

#endif // !defined(AFX_HOM_H__94471B51_E215_4C67_B753_B02F66D79911__INCLUDED_)
