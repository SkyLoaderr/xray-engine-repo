#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xrLauncher
{
	/// <summary> 
	/// Summary for xrLauncher_about_dlg
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class xrLauncher_about_dlg : public System::Windows::Forms::Form
	{
	public: 
		xrLauncher_about_dlg(void)
		{
			InitializeComponent();
		}
    void Init(const SmodInfo&);    
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::ListBox *  modCreditsList;
	private: System::Windows::Forms::Label *  modVersionLbl;
	private: System::Windows::Forms::LinkLabel *  modLinkLbl;
	private: System::Windows::Forms::Label *  modLongDescrLbl;
	private: System::Windows::Forms::Label *  modShortDescrLbl;
	private: System::Windows::Forms::Button *  button1;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  modNameLbl;
	private: System::Windows::Forms::Panel *  panel1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->modCreditsList = new System::Windows::Forms::ListBox();
			this->modVersionLbl = new System::Windows::Forms::Label();
			this->modLinkLbl = new System::Windows::Forms::LinkLabel();
			this->modLongDescrLbl = new System::Windows::Forms::Label();
			this->modShortDescrLbl = new System::Windows::Forms::Label();
			this->button1 = new System::Windows::Forms::Button();
			this->label1 = new System::Windows::Forms::Label();
			this->modNameLbl = new System::Windows::Forms::Label();
			this->panel1 = new System::Windows::Forms::Panel();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// modCreditsList
			// 
			this->modCreditsList->BackColor = System::Drawing::SystemColors::Control;
			this->modCreditsList->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->modCreditsList->Font = new System::Drawing::Font(S"Microsoft Sans Serif", 8.24F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->modCreditsList->Location = System::Drawing::Point(10, 184);
			this->modCreditsList->Name = S"modCreditsList";
			this->modCreditsList->SelectionMode = System::Windows::Forms::SelectionMode::None;
			this->modCreditsList->Size = System::Drawing::Size(296, 104);
			this->modCreditsList->TabIndex = 13;
			// 
			// modVersionLbl
			// 
			this->modVersionLbl->Location = System::Drawing::Point(10, 16);
			this->modVersionLbl->Name = S"modVersionLbl";
			this->modVersionLbl->Size = System::Drawing::Size(392, 16);
			this->modVersionLbl->TabIndex = 12;
			this->modVersionLbl->Text = S"version";
			// 
			// modLinkLbl
			// 
			this->modLinkLbl->Location = System::Drawing::Point(10, 136);
			this->modLinkLbl->Name = S"modLinkLbl";
			this->modLinkLbl->Size = System::Drawing::Size(392, 16);
			this->modLinkLbl->TabIndex = 11;
			this->modLinkLbl->TabStop = true;
			this->modLinkLbl->Text = S"linkLabel1";
			// 
			// modLongDescrLbl
			// 
			this->modLongDescrLbl->Location = System::Drawing::Point(10, 64);
			this->modLongDescrLbl->Name = S"modLongDescrLbl";
			this->modLongDescrLbl->Size = System::Drawing::Size(392, 64);
			this->modLongDescrLbl->TabIndex = 10;
			this->modLongDescrLbl->Text = S"long";
			// 
			// modShortDescrLbl
			// 
			this->modShortDescrLbl->Location = System::Drawing::Point(10, 40);
			this->modShortDescrLbl->Name = S"modShortDescrLbl";
			this->modShortDescrLbl->Size = System::Drawing::Size(392, 16);
			this->modShortDescrLbl->TabIndex = 9;
			this->modShortDescrLbl->Text = S"short";
			// 
			// button1
			// 
			this->button1->DialogResult = System::Windows::Forms::DialogResult::OK;
			this->button1->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->button1->Location = System::Drawing::Point(328, 264);
			this->button1->Name = S"button1";
			this->button1->Size = System::Drawing::Size(88, 24);
			this->button1->TabIndex = 14;
			this->button1->Text = S"Ok";
			this->button1->Click += new System::EventHandler(this, button1_Click);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(10, 160);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(48, 16);
			this->label1->TabIndex = 15;
			this->label1->Text = S"credits:";
			// 
			// modNameLbl
			// 
			this->modNameLbl->Location = System::Drawing::Point(10, 0);
			this->modNameLbl->Name = S"modNameLbl";
			this->modNameLbl->Size = System::Drawing::Size(392, 16);
			this->modNameLbl->TabIndex = 16;
			this->modNameLbl->Text = S"name";
			// 
			// panel1
			// 
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->modLinkLbl);
			this->panel1->Controls->Add(this->modLongDescrLbl);
			this->panel1->Controls->Add(this->modShortDescrLbl);
			this->panel1->Controls->Add(this->modCreditsList);
			this->panel1->Controls->Add(this->modNameLbl);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->button1);
			this->panel1->Controls->Add(this->modVersionLbl);
			this->panel1->Location = System::Drawing::Point(1, 0);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(423, 296);
			this->panel1->TabIndex = 17;
			// 
			// xrLauncher_about_dlg
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(424, 296);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = S"xrLauncher_about_dlg";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncher_about_dlg";
			this->KeyDown += new System::Windows::Forms::KeyEventHandler(this, xrLauncher_about_dlg_KeyDown);
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
	private: System::Void xrLauncher_about_dlg_KeyDown(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
			 {
				if(e->Alt&&e->KeyCode == System::Windows::Forms::Keys::F4)
					button1_Click(0,0);
			 }

private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 Close();
		 }

};
}