// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOM_H__94471B51_E215_4C67_B753_B02F66D79911__INCLUDED_)
#define AFX_HOM_H__94471B51_E215_4C67_B753_B02F66D79911__INCLUDED_
#pragma once

class ENGINE_API CHOM_Triangle;
class ENGINE_API CHOM_Node;

class ENGINE_API CHOM  
{
private:
	vector<CHOM_Triangle*>	m_pTris;
	vector<CHOM_Node*>		m_pNodes;
public:
	void					Load	(CStream* S);

	CHOM();
	~CHOM();
};

#endif // !defined(AFX_HOM_H__94471B51_E215_4C67_B753_B02F66D79911__INCLUDED_)
