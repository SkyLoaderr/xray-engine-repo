#pragma once

#define TEMPLATE_SPECIALIZATION template <\
	typename _Object\
>

#define CStateBloodsuckerVampireHideAbstract CStateBloodsuckerVampireHide<_Object>

TEMPLATE_SPECIALIZATION
CStateBloodsuckerVampireHideAbstract::CStateBloodsuckerVampireHide(_Object *obj) : inherited(obj)
{
	//add_state	(eStateVampire_Hide_MoveToCover,	xr_new<CStateMonsterMoveToPointEx<_Object> >	(obj));
	//add_state	(eStateVampire_Hide_Camp,			xr_new<CStateMonsterCustomActionLook<_Object> >	(obj));
}

//TEMPLATE_SPECIALIZATION
//void CStateBloodsuckerVampireHideAbstract::initialize()
//{
//	inherited::initialize();
//	object->path().prepare_builder	();	
//}
//
//TEMPLATE_SPECIALIZATION
//void CStateBloodsuckerVampireHideAbstract::execute()
//{
//}
//
//TEMPLATE_SPECIALIZATION
//void CStateBloodsuckerVampireHideAbstract::setup_substates()
//{
//	
//}

#undef TEMPLATE_SPECIALIZATION
#undef CStateBloodsuckerVampireAbstract