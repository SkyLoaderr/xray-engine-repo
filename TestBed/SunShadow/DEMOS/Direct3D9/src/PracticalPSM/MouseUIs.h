#ifndef __MOUSE_UI_H_included_
#define __MOUSE_UI_H_included_

class BaseMouseUI
{
protected:
    bool m_bLButtonDown;
    bool m_bKeysDown[6];
	unsigned int m_StartX;
	unsigned int m_StartY;
    HWND m_hWnd;
    float m_TranslationalSensitivityFactor;
	float m_RotationalSensitivityFactor;

    void SetLocation( const unsigned int& x, const unsigned int& y )
	{
		m_StartX = x;
		m_StartY = y;
    }
    BaseMouseUI(): m_hWnd(NULL),
                   m_TranslationalSensitivityFactor(1.f), 
                   m_RotationalSensitivityFactor(0.05f)
    {
        m_bKeysDown[0] = m_bKeysDown[1] = m_bKeysDown[2] = m_bKeysDown[3] = m_bKeysDown[4] = m_bKeysDown[5] = false;
    }
    BaseMouseUI(HWND hWnd): m_hWnd(hWnd), 
                            m_TranslationalSensitivityFactor(1.f), 
                            m_RotationalSensitivityFactor(0.05f)
    {
        m_bKeysDown[0] = m_bKeysDown[1] = m_bKeysDown[2] = m_bKeysDown[3] = m_bKeysDown[4] = m_bKeysDown[5] = false;
    }
    ~BaseMouseUI()
    {
        ::ReleaseCapture();
    }
public:
    virtual void Reset()=0;
    virtual void GetViewMatrix(D3DXMATRIX* pMatrix) const = 0;
    virtual void OnMouseMove( unsigned int x, unsigned int y ) = 0;
    virtual void Tick(float dt) = 0;

    bool IsMouseDown() const { return ( m_bLButtonDown ); }
	float GetRotationalSensitivityFactor() const { return m_RotationalSensitivityFactor; }
	void  SetRotationalSensitivityFactor( float rhs ) { m_RotationalSensitivityFactor = rhs; }
    float GetTranslationalSensitivityFactor() const { return m_TranslationalSensitivityFactor; }
    void SetTranslationalSensitivityFactor(float vel ) { m_TranslationalSensitivityFactor = vel; }

    // accepts window coordinates
	virtual void OnLButtonDown( unsigned int x, unsigned int y )
	{
		SetLocation( x, y );
		m_bLButtonDown = true;
        ::SetCapture(m_hWnd);
	}
	
	virtual void OnLButtonUp( unsigned int x, unsigned int y )
	{
		SetLocation( x, y );
		m_bLButtonDown = false;
        ::ReleaseCapture( );
	}

    void UpdateKeyState( )
    {
        m_bKeysDown[KEY_W] = (::GetAsyncKeyState(0x57)&0x8000L) ? true : false;
        m_bKeysDown[KEY_S] = (::GetAsyncKeyState(0x53)&0x8000L) ? true : false;
        m_bKeysDown[KEY_A] = (::GetAsyncKeyState(0x41)&0x8000L) ? true : false;
        m_bKeysDown[KEY_D] = (::GetAsyncKeyState(0x44)&0x8000L) ? true : false;
        m_bKeysDown[KEY_PGUP] = (::GetAsyncKeyState(VK_PRIOR)&0x8000L) ? true : false;
        m_bKeysDown[KEY_PGDN] = (::GetAsyncKeyState(VK_NEXT)&0x8000L) ? true : false;
    }

    enum 
    {
        KEY_W = 0,
        KEY_A = 1,
        KEY_S = 2,
        KEY_D = 3,
        KEY_PGUP = 4,
        KEY_PGDN = 5
    };
};


/************************************************************************************
 *  MouseFPSUI
 *    FPS-style user interface
 ************************************************************************************/

class MouseFPSUI: public BaseMouseUI
{
protected :
	float m_Yaw, m_Pitch;
    D3DXMATRIX m_RotationMatrix;
    D3DXVECTOR3 m_Translation;
	
    void MoveAlongCamera( float X, float Z, float t )
    {
        D3DXVECTOR3 xAxis( m_RotationMatrix._11, m_RotationMatrix._21, m_RotationMatrix._31);
        D3DXVECTOR3 zAxis( m_RotationMatrix._13, m_RotationMatrix._23, m_RotationMatrix._33);
        D3DXVECTOR3 compositeXlate(0.f, 0.f, 0.f);

        compositeXlate -= X*xAxis;
        compositeXlate -= Z*zAxis;  
        compositeXlate.y = 0.f;

        D3DXVec3Normalize(&compositeXlate, &compositeXlate);
        compositeXlate *= m_TranslationalSensitivityFactor*t;
        
        m_Translation.x += compositeXlate.x;
        m_Translation.z += compositeXlate.z;
    }

    void MoveUp( float Y, float t )
    {
        m_Translation.y -= Y*m_TranslationalSensitivityFactor*t;
    }

public :
	
    MouseFPSUI(): BaseMouseUI() { Reset(); }
    MouseFPSUI(HWND hWnd):BaseMouseUI(hWnd) { Reset(); }
    virtual ~MouseFPSUI() { }
	
	virtual void Reset()
	{
        ::ReleaseCapture();
        m_RotationalSensitivityFactor = 1.f;
        m_TranslationalSensitivityFactor = 0.05f;
		m_bLButtonDown = false;
		SetLocation( 0, 0 );
        m_Yaw = 0.f;
        m_Pitch = 0.f;
		D3DXMatrixIdentity( &m_RotationMatrix );
	}

    virtual void GetViewMatrix( D3DXMATRIX* pMatrix ) const
    {
        D3DXMATRIX translationMatrix;
        D3DXMatrixTranslation(&translationMatrix, m_Translation.x, m_Translation.y, m_Translation.z);
        D3DXMatrixMultiply(pMatrix, &translationMatrix, &m_RotationMatrix);
    }
	
	
	virtual void SetTranslate( float X, float Y, float Z )
	{
		m_Translation = D3DXVECTOR3(-X, -Y, -Z);
	}



    virtual void Tick( float dt )
    {
        float X=0.f, Y=0.f, Z=0.f;
        Z = (m_bKeysDown[KEY_W]?1.f:0.f) + (m_bKeysDown[KEY_S]?-1.f:0.f);
        Y = (m_bKeysDown[KEY_PGUP]?1.f:0.f) + (m_bKeysDown[KEY_PGDN]?-1.f:0.f);
        X = (m_bKeysDown[KEY_A]?-1.f:0.f) + (m_bKeysDown[KEY_D]?1.f:0.f);
        if (::GetAsyncKeyState(VK_SHIFT)&0x8000L)
        {
            dt*=10.f;
        }
        MoveAlongCamera(X, Z, dt);
        MoveUp(Y, dt);
    }

    virtual void OnMouseMove( unsigned int x, unsigned int y )
	{
		D3DXVECTOR3		vTrans;
		D3DXVECTOR4		vT4;

		if ( IsMouseDown() )
		{ 
			float deltax = ( (float)x - (float)m_StartX );
			float deltay = ( (float)y - (float)m_StartY );
			if ( !( ( deltax == 0.0f ) && ( deltay == 0.0f ) ) )
			{
				m_Yaw -= D3DXToRadian(- m_RotationalSensitivityFactor * deltax);
				if (m_Yaw < - D3DXToRadian(180.0f) )
					m_Yaw += D3DXToRadian(360.0f);
				else if (m_Yaw > D3DXToRadian(180.0f))
					m_Yaw -= D3DXToRadian(360.0f);
				m_Pitch -= D3DXToRadian(- m_RotationalSensitivityFactor * deltay);
				if (m_Pitch < - D3DXToRadian(90.0f))
					m_Pitch += D3DXToRadian(360.0f);
				else if (m_Pitch > D3DXToRadian(90.0f))
					m_Pitch -= D3DXToRadian(360.0f);
				D3DXMatrixRotationYawPitchRoll(&m_RotationMatrix, m_Yaw, m_Pitch, 0.0f);
				D3DXMatrixTranspose(&m_RotationMatrix, &m_RotationMatrix);
            }
			SetLocation( x, y );
		}
	}
};



#endif __MOUSE_UI_H_included_


