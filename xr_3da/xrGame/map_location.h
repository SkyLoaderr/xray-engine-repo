
#pragma once


class CMapSpot;
class CMiniMapSpot;
class CMapSpotPointer;
class CUICustomMap;


class CMapLocation
{
public:
enum ELocationFlags
{
	eSerailizable		= 0x01,
	eHideInOffline		= 0x02,
};

private:
	flags32					m_flags;
	string512				m_hint;
	CMapSpot*				m_level_spot;
	CMapSpotPointer*		m_level_spot_pointer;
	CMiniMapSpot*			m_minimap_spot;
	CMapSpotPointer*		m_minimap_spot_pointer;
	u16						m_objectID;
	u16						m_refCount;
	Fvector2				m_position_global; //last global position, actual time only current frame 
	Ivector2 				m_position_on_map; //last position on parent map, actual time only current frame
private:
							CMapLocation					(const CMapLocation&){}; //disable copy ctor

	void					LoadSpot						(LPCSTR type); 

	void					UpdateSpot						(CUICustomMap* map, CMapSpot* sp );
	void					UpdateSpotPointer				(CUICustomMap* map, CMapSpotPointer* sp );
	CMapSpotPointer*		GetSpotPointer					(CMapSpot* sp);
public:
							CMapLocation					(LPCSTR type, u16 object_id);
	virtual					~CMapLocation					();
	virtual		LPCSTR		GetHint							()					{return m_hint;};
	void					SetHint							(LPCSTR hint)		{strcpy(m_hint,hint);};

	void					UpdateMiniMap					(CUICustomMap* map);
	void					UpdateLevelMap					(CUICustomMap* map);

	Fvector2				Position						();
	Fvector2				Direction						();
	shared_str				LevelName						();
	u16						RefCount						() {return m_refCount;}
	void					SetRefCount						(u16 c)		{m_refCount=c;}
	u16						AddRef							() {++m_refCount; return m_refCount;}
	u16						Release							() {--m_refCount; return m_refCount;}
	bool					Update							(); //returns actual
	Fvector2				GetLastPosition					() {return m_position_global;};
	bool					Serializable					() const {return !!m_flags.test(eSerailizable);}
};
