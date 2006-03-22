// Level_Bullet_Manager.h:  ��� ����������� ������ ���� �� ����������
//							��� ���� � ������� ���������� ����
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"
#include "tracer.h"

//���������, ����������� ���� � �� �������� � ������
struct SBullet
{
	u32				frame_num			;			//����� ����� �� ������� ���� �������� ����
	union			{
		struct			{
			u16			ricochet_was	: 1	;			//���� �����������
			u16			explosive		: 1	;			//special explosive mode for particles
			u16			allow_tracer	: 1	;
			u16			allow_ricochet	: 1	;			//��������� �������
			u16			allow_sendhit	: 1	;			//statistics
			u16			skipped_frame	: 1	;			//������� ������ ��������� 
		};
		u16				_storage			;
	}				flags				;
	u16				bullet_material_idx	;

	Fvector			pos					;			//������� �������
	Fvector			dir					;			
	float			speed				;			//������� ��������
	
	u16				parent_id			;			//ID ��������� ������� ���������� ��������
	u16				weapon_id			;			//ID ������ �� �������� ���� �������� ����
	
	float			fly_dist			;			//��������� ������� ���� ���������

	//����������� � ��������� �������
	float			hit_power			;			// power*cartridge
	float			hit_impulse			;			// impulse*cartridge
	//-------------------------------------------------------------------
	float			ap					;
	float			air_resistance		;
	//-------------------------------------------------------------------
	float			max_speed			;			// maxspeed*cartridge
	float			max_dist			;			// maxdist*cartridge
	float			pierce				;
	float			wallmark_size		;
	
	//��� ���������� ����
	ALife::EHitType hit_type			;
	//---------------------------------
	u32				m_dwID				;
	ref_sound		m_whine_snd			;
	ref_sound		m_mtl_snd			;
	//---------------------------------
	u16				targetID			;
	bool			operator	==		(u32 ID){return	ID == m_dwID;}
public:
					SBullet				();
					~SBullet			();

	void			Init				(const	Fvector& position,
										const	Fvector& direction,
										float	start_speed,
										float	power,
										float	impulse,
										u16		sender_id,
										u16		sendersweapon_id,
										ALife::EHitType e_hit_type,
										float	maximum_distance,
										const	CCartridge& cartridge,
										bool	SendHit);
};

class CLevel;

class CBulletManager
{
	DEFINE_VECTOR						(ref_sound,SoundVec,SoundVecIt);
	DEFINE_VECTOR						(SBullet,BulletVec,BulletVecIt);
	typedef std::pair<float,float>		_hit		;
	friend	CLevel;

	enum EventType {
		EVENT_HIT	= u8(0),
		EVENT_REMOVE,

		EVENT_DUMMY = u8(-1),
	};
	struct	_event			{
		EventType			Type;
		BOOL				dynamic		;
		BOOL				Repeated	;	// ���������������� ��������� ��������� � ������������ ������
		_hit				result		;
		SBullet				bullet		;
		Fvector				normal		;
		Fvector				point		;
		collide::rq_result	R			;
		u16					tgt_material;
	};
protected:
	SoundVec				m_WhineSounds		;
	RStringVec				m_ExplodeParticles	;

	//������ ���� ����������� � ������ ������ �� ������
	xrCriticalSection		m_Lock				;
	BulletVec				m_Bullets			;	// working set, locked
	BulletVec				m_BulletsRendered	;	// copy for rendering
	xr_vector<_event>		m_Events			;	

	//������� �������, ������� �� ��� ����� �� ���������� �����
	u32						m_dwTimeRemainder;

	//��������� ��������� �� ����
	CTracer					tracers;

	//������������� ����� ���� �������� ����
	u32						m_dwStepTime;	
	//����������� ��������, �� ������� ���� ��� ���������
	static float			m_fMinBulletSpeed;

	float					m_fHPMaxDist;

	//��������� G
	float					m_fGravityConst;
	//������������� �������, �������, ������� ���������� �� ��������
	//������ ����
	float					m_fAirResistanceK;
	//c������ ��������� ������� �������� ���� ��� ������������ � ���������� (��� ������� ��� ������ �����)
	float					m_fCollisionEnergyMin;
	//�������� ��������� ������� ��������� � ���� ��� ����� ������������
	float					m_fCollisionEnergyMax;

	//��������� ��������� ���������
	float					m_fTracerWidth;
	float 					m_fTracerLengthMax;
	float 					m_fTracerLengthMin;
protected:
	void					PlayWhineSound		(SBullet* bullet, CObject* object, const Fvector& pos);
	void					PlayExplodePS		(const Fmatrix& xf);
	//������� ��������� ����� ��������
	static BOOL 			test_callback		(const collide::ray_defs& rd, CObject* object, LPVOID params);
	static BOOL				firetrace_callback	(collide::rq_result& result, LPVOID params);

	// Deffer event
	void					RegisterEvent		(EventType Type, BOOL _dynamic, SBullet* bullet, const Fvector& end_point, collide::rq_result& R, u16 target_material);
	
	//��������� �� ������������� �������
	void					DynamicObjectHit	(_event& E);
	
	//��������� �� ������������ �������
	void					StaticObjectHit		(_event& E);

	//��������� �� ������ �������, �� ������ - ������� � ���� ���������� ����� �������
	_hit					ObjectHit			(SBullet* bullet, const Fvector& end_point, 
												collide::rq_result& R, u16 target_material, Fvector& hit_normal);
	//������� �� ���������� �������
	void					FireShotmark		(SBullet* bullet, const Fvector& vDir, 
												const Fvector &vEnd,    collide::rq_result& R,  u16 target_material,
												const Fvector& vNormal, bool ShowMark = true);
	//������� ������ ���� �� ��������� ���������� �������
	//����������� ��� �� ���� ������� ���� �������� ������������
	//� ����������, � ����� �������� ����� ���������� �������
	//�������� � ��������� � ������ ���������� � �����
	//���������� true ���� ���� ���������� �����
	bool					CalcBullet			(collide::rq_results & rq_storage, xr_vector<ISpatial*>& rq_spatial, SBullet* bullet, u32 delta_time);
	void 		__stdcall	UpdateWorkload		();
public:
							CBulletManager		();
	virtual					~CBulletManager		();

	void 					Load				();
	void 					Clear				();
	void 					AddBullet			(const Fvector& position, const Fvector& direction, 
												float starting_speed, float power, float impulse, 
												u16	sender_id, u16 sendersweapon_id,
												ALife::EHitType e_hit_type, float maximum_distance, 
												const CCartridge& cartridge, bool SendHit);

	void					CommitEvents		();	// @ the start of frame
	void					CommitRenderSet		();	// @ the end of frame
	void 					Render				();
};
