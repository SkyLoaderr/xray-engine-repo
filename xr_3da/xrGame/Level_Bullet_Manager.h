// Level_Bullet_Manager.h:  для обеспечения полета пули по траектории
//							все пули и осколки передаются сюда
//////////////////////////////////////////////////////////////////////

#pragma once


#include "weaponammo.h"
#include "tracer.h"

//структура, описывающая пулю и ее свойства в полете
struct SBullet
{
	SBullet();
	~SBullet();

	void Init(const Fvector& position,
		const Fvector& direction,
		float start_speed,
		float power,
		float impulse,
		u16	sender_id,
		u16 sendersweapon_id,
		ALife::EHitType e_hit_type,
		float maximum_distance,
		const CCartridge& cartridge,
		float tracer_length);

	//номер кадра на котором была запущена пуля
	u32				frame_num;
	
	enum EBulletFlags {
		RICOCHET_FLAG				= u16(1 << 0),	//пуля срикошетила
		PARTICLES_FLAG				= u16(1 << 1),
		LIFE_TIME_FLAG				= u16(1 << 2),
		TRACER_FLAG					= u16(1 << 3),
		RICOCHET_ENABLED_FLAG		= u16(1 << 4)	//разрешить рикошет
	};
	
	Flags16			flags;
	//текущая позиция
	Fvector			pos;
	
	//смещения рендера трассы относительно текущей позиции
	//(в процентах от текущей длины)
	//задается случайно на инициализации
	//чтоб трассы разных пуль не рисовались в одном месте
	float			render_offset;

	//текущая скорость
	Fvector			dir;
	float			speed;
	//ID персонажа который иницировал действие
	u16				parent_id;
	//ID оружия из которого была выпущены пуля
	u16				weapon_id;
	//для отладки
	//предыдущая позиция
	Fvector			prev_pos;



	//дистанция которую пуля пролетела
	float			fly_dist;

	//коэфициенты и параметры патрона
	float			hit_power;
	float			hit_impulse;
	
	float			max_speed;

	float			max_dist;
	float			dist_k;
	float			hit_k;
	float			impulse_k;
	float			pierce_k;
	float			wallmark_size;

    //максимальная длина трассера для данной пули
	float			tracer_max_length;

	u16				bullet_material_idx;

	//тип наносимого хита
	ALife::EHitType hit_type;

};



class CLevel;



class CBulletManager
{
	friend CLevel;
public:
	CBulletManager();
	virtual ~CBulletManager();

	void Load       ();

	void Clear		();

	void AddBullet	(const Fvector& position, const Fvector& direction, float starting_speed,
					float power, float impulse, u16	sender_id, u16 sendersweapon_id,
					ALife::EHitType e_hit_type, float maximum_distance, const CCartridge& cartridge,
					float tracer_length = flt_max);
	void Update		();

	void Render		();


	//default материал для пули и осколков (инициализируется в CLevel::Load)
	static u16 bullet_material_idx;

protected:

	//функция обработки хитов объектов
	static BOOL __stdcall firetrace_callback(Collide::rq_result& result, LPVOID params);
	//попадание по динамическому объекту
	void DynamicObjectHit	(SBullet* bullet, const Fvector& end_point, 
							Collide::rq_result& R, 	u16 target_material);
	//попадание по статическому объекту
	void StaticObjectHit	(SBullet* bullet, const Fvector& end_point, 
							Collide::rq_result& R, u16 target_material);
	//отметка на пораженном объекте
	void FireShotmark		(const SBullet* bullet, const Fvector& vDir, 
							 const Fvector &vEnd, Collide::rq_result& R, 
							 u16 target_material,
							 Fvector& vNormal);

	//просчет полета пули за некоторый промежуток времени
	//принимается что на этом участке пуля движется прямолинейно
	//и равномерно, а после просчета также изменяется текущая
	//скорость и положение с учетом гравитации и ветра
	//возвращаем true если пуля продолжает полет
	bool CalcBullet (SBullet* bullet, u32 delta_time);


	DEFINE_VECTOR(SBullet,BulletVec,BulletVecIt);
	//список пуль находящихся в данный момент на уровне
	BulletVec m_Bullets;

	//остаток времени, который не был учтен на предыдущем кадре
	u32	m_dwTimeRemainder;


	//отрисовка трассеров от пуль
	CTracer tracers;

	//фиксированное время шага просчета пули
	u32	m_dwStepTime;	
	//минимальная скорость, на которой пуля еще считается
	static float m_fMinBulletSpeed;

	//константа G
	float m_fGravityConst;
	//сопротивление воздуха, процент, который отнимается от скорости
	//полета пули
	float m_fAirResistanceK;
	//cколько процентов энергии потеряет пуля при столкновении с материалом (при падении под прямым углом)
	float m_fCollisionEnergyMin;
	//сколькол процентов энергии устанется у пули при любом столкновении
	float m_fCollisionEnergyMax;

	//параметры отрисовки трассеров
	float m_fTracerWidth;
	float m_fTracerLength;
	float m_fTracerLengthMin;
	float m_fLengthToWidthRatio;
	//границы в которых изменяется размер трассера (как его видит актер)
	float m_fMinViewDist;
	float m_fMaxViewDist;
};