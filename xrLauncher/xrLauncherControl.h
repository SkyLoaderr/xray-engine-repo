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
		}
	private: void Init();
	private: bool isChanged(); 
	private: void ApplyChanges();

	private: void InitSoundPage();
	private: void ApplySoundPage();
	private: bool SoundChanged();

	public : void _Close(int res);
	public : int  _Show(int initial_state);

	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private : int		m_modal_result;
	private : int		m_init_state;

	private: System::Windows::Forms::TabControl *  tabControl1;
	private: System::Windows::Forms::TabPage *  lanPage;
	private: System::Windows::Forms::TabPage *  soundPage;
	private: System::Windows::Forms::Button *  ApplyButton;
	private: System::Windows::Forms::Button *  OkButton;
	private: System::Windows::Forms::Button *  CancelButton;





	private: System::Windows::Forms::TrackBar *  sndVolEffectTrack;
	private: System::Windows::Forms::TrackBar *  sndVolMusicTrack;
	private: System::Windows::Forms::CheckBox *  sndAccelCheck;
	private: System::Windows::Forms::CheckBox *  sndEfxCheck;
	private: System::Windows::Forms::NumericUpDown *  sndTargetsUpDown;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
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
			this->tabControl1 = new System::Windows::Forms::TabControl();
			this->soundPage = new System::Windows::Forms::TabPage();
			this->label3 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label1 = new System::Windows::Forms::Label();
			this->sndTargetsUpDown = new System::Windows::Forms::NumericUpDown();
			this->sndEfxCheck = new System::Windows::Forms::CheckBox();
			this->sndAccelCheck = new System::Windows::Forms::CheckBox();
			this->sndVolMusicTrack = new System::Windows::Forms::TrackBar();
			this->sndVolEffectTrack = new System::Windows::Forms::TrackBar();
			this->lanPage = new System::Windows::Forms::TabPage();
			this->ApplyButton = new System::Windows::Forms::Button();
			this->OkButton = new System::Windows::Forms::Button();
			this->CancelButton = new System::Windows::Forms::Button();
			this->panel1 = new System::Windows::Forms::Panel();
			this->tabControl1->SuspendLayout();
			this->soundPage->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolMusicTrack))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolEffectTrack))->BeginInit();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->soundPage);
			this->tabControl1->Controls->Add(this->lanPage);
			this->tabControl1->Location = System::Drawing::Point(3, 4);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(365, 272);
			this->tabControl1->TabIndex = 0;
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
			this->soundPage->Size = System::Drawing::Size(357, 246);
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
			this->sndEfxCheck->Text = S"Enable EFX";
			this->sndEfxCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndAccelCheck
			// 
			this->sndAccelCheck->Location = System::Drawing::Point(8, 96);
			this->sndAccelCheck->Name = S"sndAccelCheck";
			this->sndAccelCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndAccelCheck->Size = System::Drawing::Size(336, 16);
			this->sndAccelCheck->TabIndex = 2;
			this->sndAccelCheck->Text = S"Enable sound acceleration";
			this->sndAccelCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndVolMusicTrack
			// 
			this->sndVolMusicTrack->AutoSize = false;
			this->sndVolMusicTrack->LargeChange = 1;
			this->sndVolMusicTrack->Location = System::Drawing::Point(200, 40);
			this->sndVolMusicTrack->Name = S"sndVolMusicTrack";
			this->sndVolMusicTrack->Size = System::Drawing::Size(152, 16);
			this->sndVolMusicTrack->TabIndex = 1;
			this->sndVolMusicTrack->TabStop = false;
			this->sndVolMusicTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// sndVolEffectTrack
			// 
			this->sndVolEffectTrack->AutoSize = false;
			this->sndVolEffectTrack->LargeChange = 1;
			this->sndVolEffectTrack->Location = System::Drawing::Point(200, 16);
			this->sndVolEffectTrack->Name = S"sndVolEffectTrack";
			this->sndVolEffectTrack->Size = System::Drawing::Size(152, 16);
			this->sndVolEffectTrack->TabIndex = 0;
			this->sndVolEffectTrack->TabStop = false;
			this->sndVolEffectTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// lanPage
			// 
			this->lanPage->Location = System::Drawing::Point(4, 22);
			this->lanPage->Name = S"lanPage";
			this->lanPage->Size = System::Drawing::Size(357, 246);
			this->lanPage->TabIndex = 2;
			this->lanPage->Text = S"LAN";
			// 
			// ApplyButton
			// 
			this->ApplyButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->ApplyButton->Location = System::Drawing::Point(268, 288);
			this->ApplyButton->Name = S"ApplyButton";
			this->ApplyButton->Size = System::Drawing::Size(80, 24);
			this->ApplyButton->TabIndex = 1;
			this->ApplyButton->Text = S"Apply";
			this->ApplyButton->Click += new System::EventHandler(this, ApplyButton_Click);
			// 
			// OkButton
			// 
			this->OkButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->OkButton->Location = System::Drawing::Point(60, 288);
			this->OkButton->Name = S"OkButton";
			this->OkButton->Size = System::Drawing::Size(80, 24);
			this->OkButton->TabIndex = 2;
			this->OkButton->Text = S"Ok";
			this->OkButton->Click += new System::EventHandler(this, OkButton_Click);
			// 
			// CancelButton
			// 
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->CancelButton->Location = System::Drawing::Point(156, 288);
			this->CancelButton->Name = S"CancelButton";
			this->CancelButton->Size = System::Drawing::Size(80, 24);
			this->CancelButton->TabIndex = 3;
			this->CancelButton->Text = S"Cancel";
			this->CancelButton->Click += new System::EventHandler(this, CancelButton_Click);
			// 
			// panel1
			// 
			this->panel1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->ApplyButton);
			this->panel1->Controls->Add(this->OkButton);
			this->panel1->Controls->Add(this->CancelButton);
			this->panel1->Controls->Add(this->tabControl1);
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(376, 324);
			this->panel1->TabIndex = 4;
			this->panel1->Paint += new System::Windows::Forms::PaintEventHandler(this, panel1_Paint);
			// 
			// xrLauncherControl
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(376, 324);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Location = System::Drawing::Point(4, 22);
			this->Name = S"xrLauncherControl";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncherControl";
			this->tabControl1->ResumeLayout(false);
			this->soundPage->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolMusicTrack))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndVolEffectTrack))->EndInit();
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		


private: System::Void ApplyButton_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void CancelButton_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 _Close(0);
		 }

private: System::Void OkButton_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void panel1_Paint(System::Object *  sender, System::Windows::Forms::PaintEventArgs *  e)
		 {
		 }

};
}