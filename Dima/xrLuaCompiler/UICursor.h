#ifndef __XR_UICURSOR_H__
#define __XR_UICURSOR_H__
#pragma once

class CUICursor
{
	ref_shader		hShader;
	ref_geom		hGeom;	
	bool			bVisible;
	Fvector2		vPos;

	//чувствительность передвижение курсора
	float m_fSensitivity;
public:
					CUICursor	();
					~CUICursor	();
	void			Render		();
	
	//const Fvector2&	GetPos		(){return vPos;}
	
	void SetPos(int x, int y);
	void GetPos(int& x, int& y);
	POINT GetPos();
	void MoveBy(int dx, int dy);

	bool IsVisible() {return bVisible;}
	void Show() {bVisible = true;}
	void Hide() {bVisible = false;}


	void SetSensitivity(float sens) {m_fSensitivity = sens;}
	float GetSensitivity() {return m_fSensitivity;}
};

#endif //__XR_UICURSOR_H__
