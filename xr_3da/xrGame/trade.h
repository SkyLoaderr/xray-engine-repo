#pragma once

class CInventoryOwner;

class CTrade {
	bool	TradeState;					// режим торговли. true - включен

	typedef enum tagTraderType {
		TT_NONE,
		TT_TRADER,
		TT_STALKER,
		TT_ACTOR,
	} EOwnerType;

	struct SInventoryOwner {
		EOwnerType		type;
		CEntity			*base;
		CInventoryOwner	*inv_owner;

		void Set (EOwnerType t, CEntity	*b, CInventoryOwner *io) { type = t; base = b; inv_owner = io;}
	};
public:
	SInventoryOwner	pThis;
	SInventoryOwner pPartner;

public:
	
	CTrade(CInventoryOwner	*p_io);
	~CTrade();



	bool		CanTrade();								// Проверяет, может ли данный объект торговать и устанавливает pPartner
	bool		IsInTradeState() {return TradeState;}

	void		StartTrade();
	void		StopTrade();
	void		Communicate();
	
	bool		OfferTrade(SInventoryOwner man);					// man предлагает торговать
	
	void		ShowItems();
	void		ShowMoney();
private:
	bool		SetPartner(CEntity *p);

};