#include "stdafx.h"

System::Void on_btn_mouse_down(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!btn->Enabled)
				return;

			btn->ImageIndex = 1;
}

System::Void on_btn_mouse_up(System::Object *  sender, System::Windows::Forms::MouseEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!btn->Enabled)
				return;

			btn->ImageIndex = 0;
}

System::Void on_btn_enabled_changed(System::Object *  sender, System::EventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(btn->Enabled)
				btn->ImageIndex = 0;
			btn->ImageIndex = 2;
}

System::Void on_btn_key_down(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!(e->KeyCode == System::Windows::Forms::Keys::Space))
				return;

			if(!btn->Enabled)
				return;

			btn->ImageIndex = 1;
}

System::Void on_btn_key_up(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
{
			System::Windows::Forms::Button* btn = static_cast<System::Windows::Forms::Button*>(sender);
			if(!(e->KeyCode == System::Windows::Forms::Keys::Space))
				return;

			if(!btn->Enabled)
				return;

			btn->ImageIndex = 0;
}



void convert(System::String* src, char* dst)
{
/*	LPCSTR f_name = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(info_file_name);
	Marshal::FreeHGlobal(str2);
*/
	for(int i=0; i< src->Length; ++i)
		dst[i] = (char)src->get_Chars(i);
}