#pragma once
//#include <cli/vector>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace xrLauncher
{

struct SmodInfo{
	string128	m_mod_name;
	string128	m_descr_short;
	string2048	m_descr_long;
	string64	m_version;
	string512	m_www;
//	xr_vector<string64>* m_credits;
//	SmodInfo(){m_credits = new xr_vector<string64>();}
};
typedef xr_vector<SmodInfo> MOD_INFO;

	/// <summary> 
	/// Summary for xrLauncherControl
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class xrLauncherControl : public System::Windows::Forms::Form
	{
	public: 
		xrLauncherControl(void)
		{
			InitializeComponent();
			m_mod_info = new MOD_INFO();
		}
	void Init();
	void InitModPage();
	void InitSoundPage();
	void ApplySoundPage();
    MOD_INFO*	m_mod_info;
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::TabControl *  tabControl1;
	private: System::Windows::Forms::TabPage *  settingsPage;
	private: System::Windows::Forms::TabPage *  modPage;
	private: System::Windows::Forms::TabPage *  lanPage;
	private: System::Windows::Forms::TabPage *  soundPage;
	private: System::Windows::Forms::Button *  ApplyButton;
	private: System::Windows::Forms::Button *  OkButton;
	private: System::Windows::Forms::Button *  CancelButton;
	private: System::Windows::Forms::ListBox *  modList;
	private: System::Windows::Forms::TabPage *  benchmarkPage;
	private: System::Windows::Forms::TrackBar *  sndVolEffectTrack;
	private: System::Windows::Forms::TrackBar *  sndVolMusicTrack;
	private: System::Windows::Forms::CheckBox *  sndAccelCheck;
	private: System::Windows::Forms::CheckBox *  sndEfxCheck;
	private: System::Windows::Forms::NumericUpDown *  sndTargetsUpDown;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::Label *  modShortDescrLbl;
	private: System::Windows::Forms::Label *  modLongDescrLbl;
	private: System::Windows::Forms::LinkLabel *  modLinkLbl;
	private: System::Windows::Forms::ListView *  modCreditsListView;

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
			this->tabControl1 = new System::Windows::Forms::TabControl();
			this->settingsPage = new System::Windows::Forms::TabPage();
			this->modPage = new System::Windows::Forms::TabPage();
			this->modLinkLbl = new System::Windows::Forms::LinkLabel();
			this->modLongDescrLbl = new System::Windows::Forms::Label();
			this->modShortDescrLbl = new System::Windows::Forms::Label();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->modCreditsListView = new System::Windows::Forms::ListView();
			this->modList = new System::Windows::Forms::ListBox();
			this->lanPage = new System::Windows::Forms::TabPage();
			this->soundPage = new System::Windows::Forms::TabPage();
			this->label3 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label1 = new System::Windows::Forms::Label();
			this->sndTargetsUpDown = new System::Windows::Forms::NumericUpDown();
			this->sndEfxCheck = new System::Windows::Forms::CheckBox();
			this->sndAccelCheck = new System::Windows::Forms::CheckBox();
			this->sndVolMusicTrack = new System::Windows::Forms::TrackBar();
			this->sndVolEffectTrack = new System::Windows::Forms::TrackBar();
			this->benchmarkPage = new System::Windows::Forms::TabPage();
			this->ApplyButton = new System::Windows::Forms::Button();
			this->OkButton = new System::Windows::Forms::Button();
			this->CancelButton = new System::Windows::Forms::Button();
			this->tabControl1->SuspendLayout();
			this->modPage->SuspendLayout();
			this->groupBox1->SuspendLayout();
			this->soundPage->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolMusicTrack))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolEffectTrack))->BeginInit();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->settingsPage);
			this->tabControl1->Controls->Add(this->modPage);
			this->tabControl1->Controls->Add(this->lanPage);
			this->tabControl1->Controls->Add(this->soundPage);
			this->tabControl1->Controls->Add(this->benchmarkPage);
			this->tabControl1->Location = System::Drawing::Point(8, 8);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(376, 272);
			this->tabControl1->TabIndex = 0;
			// 
			// settingsPage
			// 
			this->settingsPage->Location = System::Drawing::Point(4, 22);
			this->settingsPage->Name = S"settingsPage";
			this->settingsPage->Size = System::Drawing::Size(368, 246);
			this->settingsPage->TabIndex = 0;
			this->settingsPage->Text = S"Settings";
			// 
			// modPage
			// 
			this->modPage->Controls->Add(this->modLinkLbl);
			this->modPage->Controls->Add(this->modLongDescrLbl);
			this->modPage->Controls->Add(this->modShortDescrLbl);
			this->modPage->Controls->Add(this->groupBox1);
			this->modPage->Controls->Add(this->modList);
			this->modPage->Location = System::Drawing::Point(4, 22);
			this->modPage->Name = S"modPage";
			this->modPage->Size = System::Drawing::Size(368, 246);
			this->modPage->TabIndex = 1;
			this->modPage->Text = S"MOD";
			// 
			// modLinkLbl
			// 
			this->modLinkLbl->Location = System::Drawing::Point(8, 136);
			this->modLinkLbl->Name = S"modLinkLbl";
			this->modLinkLbl->Size = System::Drawing::Size(352, 16);
			this->modLinkLbl->TabIndex = 4;
			this->modLinkLbl->TabStop = true;
			this->modLinkLbl->Text = S"linkLabel1";
			// 
			// modLongDescrLbl
			// 
			this->modLongDescrLbl->Location = System::Drawing::Point(8, 96);
			this->modLongDescrLbl->Name = S"modLongDescrLbl";
			this->modLongDescrLbl->Size = System::Drawing::Size(352, 32);
			this->modLongDescrLbl->TabIndex = 3;
			this->modLongDescrLbl->Text = S"long";
			// 
			// modShortDescrLbl
			// 
			this->modShortDescrLbl->Location = System::Drawing::Point(8, 72);
			this->modShortDescrLbl->Name = S"modShortDescrLbl";
			this->modShortDescrLbl->Size = System::Drawing::Size(352, 16);
			this->modShortDescrLbl->TabIndex = 2;
			this->modShortDescrLbl->Text = S"short";
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->modCreditsListView);
			this->groupBox1->Location = System::Drawing::Point(8, 160);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(352, 80);
			this->groupBox1->TabIndex = 1;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"Credits";
			// 
			// modCreditsListView
			// 
			this->modCreditsListView->Alignment = System::Windows::Forms::ListViewAlignment::SnapToGrid;
			this->modCreditsListView->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->modCreditsListView->BackColor = System::Drawing::SystemColors::Control;
			this->modCreditsListView->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->modCreditsListView->HeaderStyle = System::Windows::Forms::ColumnHeaderStyle::None;
			this->modCreditsListView->Location = System::Drawing::Point(8, 16);
			this->modCreditsListView->MultiSelect = false;
			this->modCreditsListView->Name = S"modCreditsListView";
			this->modCreditsListView->Size = System::Drawing::Size(336, 56);
			this->modCreditsListView->TabIndex = 0;
			// 
			// modList
			// 
			this->modList->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->modList->Font = new System::Drawing::Font(S"Microsoft Sans Serif", 10.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->modList->ItemHeight = 17;
			this->modList->Location = System::Drawing::Point(8, 8);
			this->modList->Name = S"modList";
			this->modList->Size = System::Drawing::Size(352, 53);
			this->modList->TabIndex = 0;
			this->modList->SelectedIndexChanged += new System::EventHandler(this, modList_SelectedIndexChanged);
			// 
			// lanPage
			// 
			this->lanPage->Location = System::Drawing::Point(4, 22);
			this->lanPage->Name = S"lanPage";
			this->lanPage->Size = System::Drawing::Size(368, 246);
			this->lanPage->TabIndex = 2;
			this->lanPage->Text = S"LAN";
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
			this->soundPage->Location = System::Drawing::Point(4, 22);
			this->soundPage->Name = S"soundPage";
			this->soundPage->Size = System::Drawing::Size(368, 246);
			this->soundPage->TabIndex = 3;
			this->soundPage->Text = S"Sound";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 160);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(280, 16);
			this->label3->TabIndex = 7;
			this->label3->Text = S"Sound target count";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 40);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(144, 16);
			this->label2->TabIndex = 6;
			this->label2->Text = S"Sound music volume";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(144, 16);
			this->label1->TabIndex = 5;
			this->label1->Text = S"Sound effect volume";
			// 
			// sndTargetsUpDown
			// 
			this->sndTargetsUpDown->Location = System::Drawing::Point(304, 160);
			this->sndTargetsUpDown->Name = S"sndTargetsUpDown";
			this->sndTargetsUpDown->Size = System::Drawing::Size(40, 20);
			this->sndTargetsUpDown->TabIndex = 4;
			// 
			// sndEfxCheck
			// 
			this->sndEfxCheck->Location = System::Drawing::Point(8, 120);
			this->sndEfxCheck->Name = S"sndEfxCheck";
			this->sndEfxCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndEfxCheck->Size = System::Drawing::Size(336, 16);
			this->sndEfxCheck->TabIndex = 3;
			this->sndEfxCheck->Text = S"checkBox1";
			this->sndEfxCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndAccelCheck
			// 
			this->sndAccelCheck->Location = System::Drawing::Point(8, 96);
			this->sndAccelCheck->Name = S"sndAccelCheck";
			this->sndAccelCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndAccelCheck->Size = System::Drawing::Size(336, 16);
			this->sndAccelCheck->TabIndex = 2;
			this->sndAccelCheck->Text = S"checkBox1";
			this->sndAccelCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndVolMusicTrack
			// 
			this->sndVolMusicTrack->AutoSize = false;
			this->sndVolMusicTrack->Location = System::Drawing::Point(200, 40);
			this->sndVolMusicTrack->Name = S"sndVolMusicTrack";
			this->sndVolMusicTrack->Size = System::Drawing::Size(152, 16);
			this->sndVolMusicTrack->TabIndex = 1;
			this->sndVolMusicTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			this->sndVolMusicTrack->Scroll += new System::EventHandler(this, sndVolMusicTrack_Scroll);
			// 
			// sndVolEffectTrack
			// 
			this->sndVolEffectTrack->AutoSize = false;
			this->sndVolEffectTrack->Location = System::Drawing::Point(200, 16);
			this->sndVolEffectTrack->Name = S"sndVolEffectTrack";
			this->sndVolEffectTrack->Size = System::Drawing::Size(152, 16);
			this->sndVolEffectTrack->TabIndex = 0;
			this->sndVolEffectTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// benchmarkPage
			// 
			this->benchmarkPage->Location = System::Drawing::Point(4, 22);
			this->benchmarkPage->Name = S"benchmarkPage";
			this->benchmarkPage->Size = System::Drawing::Size(368, 246);
			this->benchmarkPage->TabIndex = 4;
			this->benchmarkPage->Text = S"Benchmark";
			// 
			// ApplyButton
			// 
			this->ApplyButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->ApplyButton->Location = System::Drawing::Point(280, 296);
			this->ApplyButton->Name = S"ApplyButton";
			this->ApplyButton->Size = System::Drawing::Size(80, 24);
			this->ApplyButton->TabIndex = 1;
			this->ApplyButton->Text = S"Apply";
			// 
			// OkButton
			// 
			this->OkButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->OkButton->Location = System::Drawing::Point(80, 296);
			this->OkButton->Name = S"OkButton";
			this->OkButton->Size = System::Drawing::Size(80, 24);
			this->OkButton->TabIndex = 2;
			this->OkButton->Text = S"Ok";
			// 
			// CancelButton
			// 
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->CancelButton->Location = System::Drawing::Point(168, 296);
			this->CancelButton->Name = S"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(80, 24);
			this->CancelButton->TabIndex = 3;
			this->CancelButton->Text = S"Cancel";
			// 
			// xrLauncherControl
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(400, 334);
			this->Controls->Add(this->CancelButton);
			this->Controls->Add(this->OkButton);
			this->Controls->Add(this->ApplyButton);
			this->Controls->Add(this->tabControl1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = S"xrLauncherControl";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncherControl";
			this->tabControl1->ResumeLayout(false);
			this->modPage->ResumeLayout(false);
			this->groupBox1->ResumeLayout(false);
			this->soundPage->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolMusicTrack))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolEffectTrack))->EndInit();
			this->ResumeLayout(false);

		}		
	private: System::Void sndVolMusicTrack_Scroll(System::Object *  sender, System::EventArgs *  e)
			 {
			 }

private: System::Void modList_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e);

};
}