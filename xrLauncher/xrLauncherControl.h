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
	/// Summary for xrLauncherControl
	/// </summary>
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	public __gc class xrLauncherControl : public System::Windows::Forms::Form
	{	
	public:
		xrLauncherControl(void)	
		{
			InitializeComponent();
		}
	void Init();
	void InitModPage();
	void InitSoundPage();
	void ApplySoundPage();

	protected:
		void Dispose(Boolean disposing)	
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::Button *  applyButton;
	private: Crownwood::Magic::Controls::TabControl *  tabControl1;
	private: Crownwood::Magic::Controls::TabPage *  settingsPage;
	private: Crownwood::Magic::Controls::TabPage *  modPage;
	private: Crownwood::Magic::Controls::TabPage *  lanPage;
	private: Crownwood::Magic::Controls::TabPage *  soundPage;
	private: System::Windows::Forms::TrackBar *  sndVolEffectTrack;
	private: System::Windows::Forms::TrackBar *  sndVolMusicTrack;
	private: System::Windows::Forms::CheckBox *  sndAccelCheck;
	private: System::Windows::Forms::CheckBox *  sndEfxCheck;

	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Button *  CancelButton;
	private: System::Windows::Forms::Button *  OkButton;
	private: System::Windows::Forms::NumericUpDown *  sndTargetsUpDown;
	private: System::Windows::Forms::ListBox *  modList;







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
			this->applyButton = new System::Windows::Forms::Button();
			this->tabControl1 = new Crownwood::Magic::Controls::TabControl();
			this->soundPage = new Crownwood::Magic::Controls::TabPage();
			this->label3 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label1 = new System::Windows::Forms::Label();
			this->sndTargetsUpDown = new System::Windows::Forms::NumericUpDown();
			this->sndEfxCheck = new System::Windows::Forms::CheckBox();
			this->sndAccelCheck = new System::Windows::Forms::CheckBox();
			this->sndVolMusicTrack = new System::Windows::Forms::TrackBar();
			this->sndVolEffectTrack = new System::Windows::Forms::TrackBar();
			this->settingsPage = new Crownwood::Magic::Controls::TabPage();
			this->modPage = new Crownwood::Magic::Controls::TabPage();
			this->lanPage = new Crownwood::Magic::Controls::TabPage();
			this->CancelButton = new System::Windows::Forms::Button();
			this->OkButton = new System::Windows::Forms::Button();
			this->modList = new System::Windows::Forms::ListBox();
			this->soundPage->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolMusicTrack))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolEffectTrack))->BeginInit();
			this->modPage->SuspendLayout();
			this->SuspendLayout();
			// 
			// applyButton
			// 
			this->applyButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->applyButton->Location = System::Drawing::Point(264, 256);
			this->applyButton->Name = S"applyButton";
			this->applyButton->Size = System::Drawing::Size(72, 24);
			this->applyButton->TabIndex = 3;
			this->applyButton->Text = S"Apply";
			this->applyButton->Click += new System::EventHandler(this, applyButton_Click);
			// 
			// tabControl1
			// 
			this->tabControl1->HideTabsMode = Crownwood::Magic::Controls::TabControl::HideTabsModes::ShowAlways;
			this->tabControl1->IDEPixelBorder = true;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->PositionTop = true;
			this->tabControl1->SelectedIndex = 1;
			this->tabControl1->SelectedTab = this->modPage;
			this->tabControl1->Size = System::Drawing::Size(352, 248);
			this->tabControl1->TabIndex = 4;
			Crownwood::Magic::Controls::TabPage* __mcTemp__1[] = new Crownwood::Magic::Controls::TabPage*[4];
			__mcTemp__1[0] = this->settingsPage;
			__mcTemp__1[1] = this->modPage;
			__mcTemp__1[2] = this->lanPage;
			__mcTemp__1[3] = this->soundPage;
			this->tabControl1->TabPages->AddRange(__mcTemp__1);
			// 
			// soundPage
			// 
			this->soundPage->Controls->Add(this->label3);
			this->soundPage->Controls->Add(this->label2);
			this->soundPage->Controls->Add(this->label1);
			this->soundPage->Controls->Add(this->sndTargetsUpDown);
			this->soundPage->Controls->Add(this->sndEfxCheck);
			this->soundPage->Controls->Add(this->sndAccelCheck);
			this->soundPage->Controls->Add(this->sndVolMusicTrack);
			this->soundPage->Controls->Add(this->sndVolEffectTrack);
			this->soundPage->Location = System::Drawing::Point(0, 0);
			this->soundPage->Name = S"soundPage";
			this->soundPage->Selected = false;
			this->soundPage->Size = System::Drawing::Size(348, 221);
			this->soundPage->TabIndex = 3;
			this->soundPage->Title = S"Sound";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 40);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(128, 24);
			this->label3->TabIndex = 7;
			this->label3->Text = S"Music volume";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 16);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(128, 24);
			this->label2->TabIndex = 6;
			this->label2->Text = S"Sound volume";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 144);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(208, 24);
			this->label1->TabIndex = 5;
			this->label1->Text = S"Targets count";
			// 
			// sndTargetsUpDown
			// 
			this->sndTargetsUpDown->BackColor = System::Drawing::SystemColors::Control;
			this->sndTargetsUpDown->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->sndTargetsUpDown->Location = System::Drawing::Point(240, 144);
			this->sndTargetsUpDown->Name = S"sndTargetsUpDown";
			this->sndTargetsUpDown->Size = System::Drawing::Size(56, 14);
			this->sndTargetsUpDown->TabIndex = 4;
			this->sndTargetsUpDown->TextAlign = System::Windows::Forms::HorizontalAlignment::Center;
			// 
			// sndEfxCheck
			// 
			this->sndEfxCheck->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->sndEfxCheck->Location = System::Drawing::Point(8, 104);
			this->sndEfxCheck->Name = S"sndEfxCheck";
			this->sndEfxCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndEfxCheck->Size = System::Drawing::Size(288, 24);
			this->sndEfxCheck->TabIndex = 3;
			this->sndEfxCheck->Text = S"Sound efx";
			this->sndEfxCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndAccelCheck
			// 
			this->sndAccelCheck->BackColor = System::Drawing::SystemColors::Control;
			this->sndAccelCheck->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->sndAccelCheck->ForeColor = System::Drawing::SystemColors::ControlText;
			this->sndAccelCheck->Location = System::Drawing::Point(8, 80);
			this->sndAccelCheck->Name = S"sndAccelCheck";
			this->sndAccelCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndAccelCheck->Size = System::Drawing::Size(288, 24);
			this->sndAccelCheck->TabIndex = 2;
			this->sndAccelCheck->Text = S"Sound acceleration";
			this->sndAccelCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndVolMusicTrack
			// 
			this->sndVolMusicTrack->AutoSize = false;
			this->sndVolMusicTrack->LargeChange = 1;
			this->sndVolMusicTrack->Location = System::Drawing::Point(152, 40);
			this->sndVolMusicTrack->Name = S"sndVolMusicTrack";
			this->sndVolMusicTrack->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->sndVolMusicTrack->Size = System::Drawing::Size(152, 16);
			this->sndVolMusicTrack->TabIndex = 1;
			this->sndVolMusicTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			this->sndVolMusicTrack->Value = 5;
			// 
			// sndVolEffectTrack
			// 
			this->sndVolEffectTrack->AutoSize = false;
			this->sndVolEffectTrack->LargeChange = 1;
			this->sndVolEffectTrack->Location = System::Drawing::Point(152, 16);
			this->sndVolEffectTrack->Name = S"sndVolEffectTrack";
			this->sndVolEffectTrack->RightToLeft = System::Windows::Forms::RightToLeft::No;
			this->sndVolEffectTrack->Size = System::Drawing::Size(152, 16);
			this->sndVolEffectTrack->TabIndex = 0;
			this->sndVolEffectTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			this->sndVolEffectTrack->Value = 5;
			// 
			// settingsPage
			// 
			this->settingsPage->BackColor = System::Drawing::SystemColors::Control;
			this->settingsPage->Location = System::Drawing::Point(0, 0);
			this->settingsPage->Name = S"settingsPage";
			this->settingsPage->Selected = false;
			this->settingsPage->Size = System::Drawing::Size(348, 221);
			this->settingsPage->TabIndex = 0;
			this->settingsPage->Title = S"Settings";
			// 
			// modPage
			// 
			this->modPage->BackColor = System::Drawing::SystemColors::Control;
			this->modPage->Controls->Add(this->modList);
			this->modPage->Location = System::Drawing::Point(0, 0);
			this->modPage->Name = S"modPage";
			this->modPage->Size = System::Drawing::Size(348, 221);
			this->modPage->TabIndex = 1;
			this->modPage->Title = S"MOD";
			// 
			// lanPage
			// 
			this->lanPage->BackColor = System::Drawing::SystemColors::Control;
			this->lanPage->Location = System::Drawing::Point(0, 0);
			this->lanPage->Name = S"lanPage";
			this->lanPage->Selected = false;
			this->lanPage->Size = System::Drawing::Size(348, 221);
			this->lanPage->TabIndex = 2;
			this->lanPage->Title = S"LAN";
			// 
			// CancelButton
			// 
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->CancelButton->Location = System::Drawing::Point(176, 256);
			this->CancelButton->Name = S"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(72, 24);
			this->CancelButton->TabIndex = 5;
			this->CancelButton->Text = S"Cancel";
			// 
			// OkButton
			// 
			this->OkButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->OkButton->Location = System::Drawing::Point(88, 256);
			this->OkButton->Name = S"OkButton";
			this->OkButton->Size = System::Drawing::Size(72, 24);
			this->OkButton->TabIndex = 6;
			this->OkButton->Text = S"Ok";
			this->OkButton->Click += new System::EventHandler(this, button2_Click);
			// 
			// modList
			// 
			this->modList->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->modList->Location = System::Drawing::Point(8, 8);
			this->modList->Name = S"modList";
			this->modList->Size = System::Drawing::Size(328, 78);
			this->modList->TabIndex = 0;
			this->modList->SelectedIndexChanged += new System::EventHandler(this, modList_SelectedIndexChanged);
			// 
			// xrLauncherControl
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(352, 294);
			this->Controls->Add(this->OkButton);
			this->Controls->Add(this->CancelButton);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->applyButton);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = S"xrLauncherControl";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->soundPage->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolMusicTrack))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolEffectTrack))->EndInit();
			this->modPage->ResumeLayout(false);
			this->ResumeLayout(false);

		}
	private: System::Void applyButton_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 Close();
			 }

private: System::Void button2_Click(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: System::Void modList_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

};
}


