#include "StdAfx.h"
#include "xrLauncher_about_dlg.h"

using namespace xrLauncher;

void xrLauncher_about_dlg::Init(const SmodInfo& info)
{
	modNameLbl->Text		= S"";
	modShortDescrLbl->Text	= S"Short description: ";
	modLongDescrLbl->Text	= S"Long description: ";
	modLinkLbl->Text		= S"Website: ";
	modVersionLbl->Text		= S"Version: ";
	modCreditsList->Items->Clear();

	modShortDescrLbl->Text = String::Concat(modShortDescrLbl->Text, new String(info.m_descr_short) );
	modLongDescrLbl->Text = String::Concat(modLongDescrLbl->Text, new String(info.m_descr_long) );
	modLinkLbl->Text = String::Concat(modLinkLbl->Text, new String(info.m_www) );
	modVersionLbl->Text = String::Concat(modVersionLbl->Text, new String(info.m_version) );

	modNameLbl->Text = String::Concat(modNameLbl->Text, new String(info.m_mod_name) );

	for(u32 i=0; i<info.m_credits->size();++i)
		modCreditsList->Items->Add(new String(*(info.m_credits->at(i))) );

}

