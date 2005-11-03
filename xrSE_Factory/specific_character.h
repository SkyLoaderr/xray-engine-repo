//////////////////////////////////////////////////////////////////////////
// specific_character.h:	������� ���������� ��� � ���������� 
//							���������� � ����
//////////////////////////////////////////////////////////////////////////

#pragma		once


#include "character_info_defs.h"
#include "shared_data.h"
#include "xml_str_id_loader.h"


#ifdef XRGAME_EXPORTS

#include "PhraseDialogDefs.h"
#include "character_community.h"

#endif


//////////////////////////////////////////////////////////////////////////
// SSpecificCharacterData: ������ � ���������� ���������
//////////////////////////////////////////////////////////////////////////
struct SSpecificCharacterData : CSharedResource
{

#ifdef  XRGAME_EXPORTS

	SSpecificCharacterData ();
	virtual ~SSpecificCharacterData ();

	//������� ��� ���������
	xr_string	m_sGameName;
	//����� � ���������� ��������� (���� �� string table)
	xr_string	m_sBioText;
	//������ ���������� ��������, ������� ����� ����������� 
	xr_string	m_sSupplySpawn;
	//��� ������ ������������ �������� NPC ��� ���������
	xr_string	m_sNpcConfigSect;
	//��� ������ ������������ ����� ��� NPC ���������
	xr_string	m_sound_voice_prefix;

	float		m_fPanic_threshold;
	float		m_fHitProbabilityFactor;
#endif

	//��� ������
	xr_string m_sVisual;

#ifdef  XRGAME_EXPORTS
	
	//��������� ������
	PHRASE_DIALOG_ID m_StartDialog;
	//������� ������, ������� ����� �������� ������ ��� ������� � ������ ����������
	DIALOG_ID_VECTOR m_ActorDialogs;

	//��������� ������� ������ (��� �������� � �������) � ����� � �������� 
	int	m_iIconX, m_iIconY;
	//��������� ��������� ������ (��� �����)
//	int	m_iMapIconX, m_iMapIconY;

	//������� 
	CHARACTER_COMMUNITY			m_Community;

#endif
	
	//����
	CHARACTER_RANK_VALUE		m_Rank;
	//���������
	CHARACTER_REPUTATION_VALUE	m_Reputation;

	//������ ��������� (�������-��������, ������ � �.�.)
	//� ������� �� �����������
	xr_vector<CHARACTER_CLASS>	m_Classes;


	//�������� �� �� ��� �������� �� ������������ ��� ���������� ������
	//� �������� ������ ����� ����� �������� ID
	bool m_bNoRandom;
	//���� �������� �������� ������� �� ��������� ��� ����� �������
	bool m_bDefaultForCommunity;
};

class CInventoryOwner;
class CCharacterInfo;
class CSE_ALifeTraderAbstract;


class CSpecificCharacter: public CSharedClass<SSpecificCharacterData, SPECIFIC_CHARACTER_ID, false>,
						  public CXML_IdToIndex<SPECIFIC_CHARACTER_ID, int, CSpecificCharacter>
{
private:
	typedef CSharedClass	<SSpecificCharacterData, SPECIFIC_CHARACTER_ID, false>				inherited_shared;
	typedef CXML_IdToIndex	<SPECIFIC_CHARACTER_ID, int, CSpecificCharacter>					id_to_index;

	friend id_to_index;
	friend CInventoryOwner;
	friend CCharacterInfo;
	friend CSE_ALifeTraderAbstract;
public:

	CSpecificCharacter();
	~CSpecificCharacter();

	//������������� �������
	//���� ����� id ������ �� ��������������
	//����� ��������� �� �����
	virtual void Load	(SPECIFIC_CHARACTER_ID		id);

protected:
	const SSpecificCharacterData* data() const { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}
	SSpecificCharacterData*		  data()	   { VERIFY(inherited_shared::get_sd()); return inherited_shared::get_sd();}

	//�������� �� XML �����
	virtual void load_shared		(LPCSTR);
	static void	 InitXmlIdToIndex	();

	SPECIFIC_CHARACTER_ID	m_OwnId;
public:

#ifdef  XRGAME_EXPORTS
	LPCSTR						Name		() const ;
	LPCSTR						Bio			() const ;
	const CHARACTER_COMMUNITY&	Community	() const ;
#endif

	CHARACTER_RANK_VALUE		Rank		() const ;
	CHARACTER_REPUTATION_VALUE	Reputation	() const ;
	LPCSTR						Visual		() const ;

#ifdef  XRGAME_EXPORTS
	LPCSTR						SupplySpawn				() const ;
	LPCSTR						NpcConfigSect			() const ;
	LPCSTR						sound_voice_prefix		() const ;
	float						panic_threshold			() const ;
	float						hit_probability_factor	() const ;

	int		TradeIconX	() const	 {return data()->m_iIconX;}
	int		TradeIconY	() const	 {return data()->m_iIconY;}
//	int		MapIconX	() const 	 {return data()->m_iMapIconX;}
//	int		MapIconY	() const	 {return data()->m_iMapIconY;}
#endif
};


//////////////////////////////////////////////////////////////////////////
