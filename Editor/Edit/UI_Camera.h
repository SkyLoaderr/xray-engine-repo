//---------------------------------------------------------------------------
#ifndef UI_CameraH
#define UI_CameraH

enum ECameraStyle{
	csPlaneMove=0,
	cs3DArcBall,
    csFreeFly
};

class CUI_Camera{
	ECameraStyle	m_Style;
    bool			m_bMoving;
    POINT			m_SavePos;
    Fvector2		m_CenterPos;
    TShiftState	 	m_Shift;
    float 			m_FlySpeed;
    float 			m_FlyAltitude;

    Fmatrix			m_CamMat;
    Fvector			m_HPB;
    Fvector			m_Position;
protected:
	friend class	CDevice;
	friend class	TUI;

	float 			m_Aspect;
	float 			m_FOV;
	float 			m_Znear;
	float 			m_Zfar;
    float			m_SR, m_SM;

    void 			Pan			(float X, float Z);
    void 			Scale		(float Y);
    void 			Rotate		(float X, float Y);
public:
					CUI_Camera	();
    				~CUI_Camera	();

	void			BuildCamera	();
	void			Reset		();
    void 			Update		(float dt);
    void			SetStyle	(ECameraStyle style){ m_Style=style; }

    bool			MoveStart	(TShiftState Shift);
    bool			MoveEnd		(TShiftState Shift);
    bool			IsMoving	(){return m_bMoving;}
	bool 			Process		(TShiftState Shift);
    bool			KeyDown		(WORD Key, TShiftState Shift);
    bool			KeyUp		(WORD Key, TShiftState Shift);

    void			ViewFront	(){m_HPB.set(0.f,0.f,0.f); BuildCamera();}
    void			ViewBack 	(){m_HPB.set(M_PI,0.f,0.f); BuildCamera();}
    void			ViewLeft	(){m_HPB.set(-PI_DIV_2,0.f,0.f); BuildCamera();}
    void			ViewRight 	(){m_HPB.set(PI_DIV_2,0.f,0.f); BuildCamera();}
    void			ViewTop		(){m_HPB.set(0.f,-PI_DIV_2,0.f); BuildCamera();}
    void			ViewBottom 	(){m_HPB.set(0.f,PI_DIV_2,0.f); BuildCamera();}
    
    void			GetView		(Fmatrix& V) const {return V.invert(m_CamMat);}
    const Fvector&	GetHPB		() const {return m_HPB;}
    const Fvector&	GetPosition	() const {return m_Position;}
    const Fvector&	GetRight	() const {return m_CamMat.i;}
    const Fvector&	GetNormal	() const {return m_CamMat.j;}
    const Fvector&	GetDirection() const {return m_CamMat.k;}
    void			Set			(float h, float p, float b, float x, float y, float z);
    void			Set			(const Fvector& hpb, const Fvector& pos);
    void			SetSensitivity(float sm, float sr);
    void			SetViewport	(float _near, float _far, float _fov);
    void			SetFlyParams(float speed, float fAltitude){ m_FlySpeed=speed; m_FlyAltitude=fAltitude; }

    void			ZoomExtents	(const Fbox& bb);

	void 			MouseRayFromPoint(Fvector& start, Fvector& direction, const Fvector2& point);
};
#endif
