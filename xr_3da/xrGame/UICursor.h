#ifndef __XR_UICURSOR_H__
#define __XR_UICURSOR_H__
#pragma once

class CUICursor
{
	ref_shader		hShader;
	ref_geom		hGeom;	
	bool			bVisible;
	bool			bHoldMode;
	Fvector2		vPos;
	Fvector2		vDelta;
	Fvector2		vHoldPos;
	//чувствительность передвижение курсора
	float m_fSensitivity;
public:
					CUICursor		();
	virtual			~CUICursor		();
	void			Render			();
	void			HoldMode		(bool b);
	Ivector2		GetPosDelta		();
	void			SetPos			(int x, int y);
	void			GetPos			(int& x, int& y);
	Ivector2		GetPos			();
	void			MoveBy			(int dx, int dy);

	bool			IsVisible		() {return bVisible;}
	void			Show			() {bVisible = true;}
	void			Hide			() {bVisible = false;}


	void SetSensitivity(float sens) {m_fSensitivity = sens;}
	float GetSensitivity() {return m_fSensitivity;}
};

#endif //__XR_UICURSOR_H__
