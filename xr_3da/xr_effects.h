#ifndef __XR_EFFECTS_H__
#define __XR_EFFECTS_H__

extern	D3DLPOLYGON			IdentityLPolygon;
extern	D3DTLPOLYGON		IdentityTLPolygon;

class CEffectBase;

//-----------------------------------------------------------------------------------------------------------
//Effect Manager
//-----------------------------------------------------------------------------------------------------------
class					CEffectManager : public pureDeviceCreate, public pureDeviceDestroy
{
	friend class		CEffectClouds;
	friend class		CEffectSun;
	friend class		CEffectSky;
	friend class		CEffectStars;
	friend class		CEffectLensFlare;

	Fmatrix				save_matProj, matProj;
	__forceinline void	SetEffProjMat( );
	__forceinline void	ResetEffProjMat( );

	float				m_phase;
	float				deltaY;

	// список загруженных динамических эффектов

	CList<CEffectBase*>	effects_list;
public:
						CEffectManager	( );
						~CEffectManager	( );

	BOOL				Load				( CInifile* ini, LPSTR section );

// служебные функции
	void _fastcall		RenderNormal		( );
	void _fastcall		RenderAlphaFirst	( );
	void _fastcall		RenderAlphaLast		( );
	void				Update				( );
	void				OnMove				( );

	void				StopEffect			( CEffectBase *Effect);

// device dependance
	virtual void		OnDeviceDestroy		( );
	virtual void		OnDeviceCreate		( );
};

#endif //__XR_EFFECTS_H__
