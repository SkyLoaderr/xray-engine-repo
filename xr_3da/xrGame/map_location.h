
#pragma once


class CMapSpot;

class CMapLocation
{
private:
	flags32					m_flags;
	string512				m_hint;
	CMapSpot*				m_level_spot;
	CMapSpot*				m_zonemap_spot;
	u16						m_objectID;
	u16						m_refCount;
private:
							CMapLocation					(const CMapLocation&){}; //disable copy ctor

	void					LoadSpot						(LPCSTR type); 

public:
							CMapLocation					(LPCSTR type, u16 object_id);
	virtual					~CMapLocation					();
	virtual		LPCSTR		GetHint							()					{return m_hint;};
	void					SetHint							(LPCSTR hint)		{strcat(m_hint,hint);};
	CMapSpot*				LevelSpot						()					{return m_level_spot;};
	CMapSpot*				ZoneMapSpot						()					{return m_zonemap_spot;};

	Fvector2				Position						();
	Fvector2				Direction						();
	shared_str				LevelName						();
	u16						RefCount						() {return m_refCount;}
	void					SetRefCount						(u16 c)		{m_refCount=c;}
	u16						AddRef							() {++m_refCount; return m_refCount;}
	u16						Release							() {--m_refCount; return m_refCount;}
	bool					Update							(); //returns actual
};


/*


//////////////////////////////////////////////////////////////////////////

enum EMapLocationFlags
{
	eMapLocationPDAContact		= 0x01,
	eMapLocationInfoPortion		= 0x02,
	eMapLocationScript			= 0x04
};

//////////////////////////////////////////////////////////////////////////

struct SMapLocation
{
	SMapLocation();
	SMapLocation(const SMapLocation&);

	shared_str LevelName();

	//типы локаций
	flags32 type_flags;

	//имя уровня
	shared_str level_name;
	//координаты на уровне
	float x;
	float y;
	//имя локации
	shared_str name;
	//текст описания
	shared_str text;

	//присоединина ли локация к объекту
	bool attached_to_object;
	//id объекта на уровне
	u16 object_id;

  //индекс части информации
	INFO_INDEX info_portion_id;

	//размеры и положение иконки
	int icon_x, icon_y, icon_width, icon_height;
	// set this field to texture name you want to use instead of default
	shared_str shader_name;	
	
	//нужно ли показывать маркер (стрелку на краю карты)
	bool marker_show;
	//показывать специальную большую иконку вместо точки (сюжетную)
	bool big_icon;
	//цвет стрелки маркера и иконки на миникарте
	u32 icon_color;
	//показывать ли на глобальной карте имя локации (когда наводишь на карту)
	bool global_map_text;

	// Показывать или не показывать отметку этого маплокейшина на карте
	void	UpdateAnimation();
	// Задать цветовую анимацию для спота
	void	SetColorAnimation(const shared_str &animationName);

	ref_shader&	GetShader();

	// Динамическое проявление и исчезание
	bool	dynamic_manifestation;

private:
	// Анимация индикатора на карте
	CUIColorAnimatorWrapper	animation;
	ref_shader	m_iconsShader;
};


DEFINE_VECTOR (SMapLocation, LOCATIONS_VECTOR, LOCATIONS_VECTOR_IT);
DEFINE_VECTOR (SMapLocation*, LOCATIONS_PTR_VECTOR, LOCATIONS_PTR_VECTOR_IT);

*/