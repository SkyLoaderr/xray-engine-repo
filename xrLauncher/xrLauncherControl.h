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

	private: void InitRenderPage();
	private: void ApplyRenderPage();
	private: bool RenderChanged();

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

	private: System::Windows::Forms::TabPage *  soundPage;
	private: System::Windows::Forms::Button *  ApplyButton;
	private: System::Windows::Forms::Button *  OkButton;
	private: System::Windows::Forms::Button *  CancelButton;







	private: System::Windows::Forms::CheckBox *  sndAccelCheck;
	private: System::Windows::Forms::CheckBox *  sndEfxCheck;
	private: System::Windows::Forms::NumericUpDown *  sndTargetsUpDown;


	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Panel *  panel1;
	private: System::Windows::Forms::TabPage *  renderPage;
	private: System::Windows::Forms::CheckBox *  vertSyncCheck;


	private: System::Windows::Forms::Label *  label1;

	private: System::Windows::Forms::CheckBox *  disableDistortionCheck;
	private: System::Windows::Forms::CheckBox *  disableSoundCheck;
	private: System::Windows::Forms::CheckBox *  ditherShadowsCheck;

	private: System::Windows::Forms::ComboBox *  renderCombo;
	private: System::Windows::Forms::Label *  label2;

	private: System::Windows::Forms::Label *  label4;

	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::ComboBox *  soundSampleRateCombo;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::ComboBox *  soundQualityCombo;
	private: System::Windows::Forms::CheckBox *  force60HzCheck;
	private: System::Windows::Forms::TrackBar *  textureLodTrack;
	private: System::Windows::Forms::TrackBar *  rasterTrack;
	private: System::Windows::Forms::CheckBox *  disableShadowsCheck;













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
			this->label6 = new System::Windows::Forms::Label();
			this->soundQualityCombo = new System::Windows::Forms::ComboBox();
			this->label5 = new System::Windows::Forms::Label();
			this->soundSampleRateCombo = new System::Windows::Forms::ComboBox();
			this->disableSoundCheck = new System::Windows::Forms::CheckBox();
			this->label3 = new System::Windows::Forms::Label();
			this->sndTargetsUpDown = new System::Windows::Forms::NumericUpDown();
			this->sndEfxCheck = new System::Windows::Forms::CheckBox();
			this->sndAccelCheck = new System::Windows::Forms::CheckBox();
			this->renderPage = new System::Windows::Forms::TabPage();
			this->rasterTrack = new System::Windows::Forms::TrackBar();
			this->textureLodTrack = new System::Windows::Forms::TrackBar();
			this->label4 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->disableShadowsCheck = new System::Windows::Forms::CheckBox();
			this->ditherShadowsCheck = new System::Windows::Forms::CheckBox();
			this->disableDistortionCheck = new System::Windows::Forms::CheckBox();
			this->label1 = new System::Windows::Forms::Label();
			this->renderCombo = new System::Windows::Forms::ComboBox();
			this->force60HzCheck = new System::Windows::Forms::CheckBox();
			this->vertSyncCheck = new System::Windows::Forms::CheckBox();
			this->ApplyButton = new System::Windows::Forms::Button();
			this->OkButton = new System::Windows::Forms::Button();
			this->CancelButton = new System::Windows::Forms::Button();
			this->panel1 = new System::Windows::Forms::Panel();
			this->tabControl1->SuspendLayout();
			this->soundPage->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->BeginInit();
			this->renderPage->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->rasterTrack))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->textureLodTrack))->BeginInit();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->soundPage);
			this->tabControl1->Controls->Add(this->renderPage);
			this->tabControl1->Location = System::Drawing::Point(3, 4);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(365, 272);
			this->tabControl1->TabIndex = 0;
			// 
			// soundPage
			// 
			this->soundPage->Controls->Add(this->label6);
			this->soundPage->Controls->Add(this->soundQualityCombo);
			this->soundPage->Controls->Add(this->label5);
			this->soundPage->Controls->Add(this->soundSampleRateCombo);
			this->soundPage->Controls->Add(this->disableSoundCheck);
			this->soundPage->Controls->Add(this->label3);
			this->soundPage->Controls->Add(this->sndTargetsUpDown);
			this->soundPage->Controls->Add(this->sndEfxCheck);
			this->soundPage->Controls->Add(this->sndAccelCheck);
			this->soundPage->Location = System::Drawing::Point(4, 22);
			this->soundPage->Name = S"soundPage";
			this->soundPage->Size = System::Drawing::Size(357, 246);
			this->soundPage->TabIndex = 3;
			this->soundPage->Text = S"Sound";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(8, 168);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(168, 16);
			this->label6->TabIndex = 12;
			this->label6->Text = S"Sound quality";
			// 
			// soundQualityCombo
			// 
			this->soundQualityCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__1[] = new System::Object*[4];
			__mcTemp__1[0] = S"Default";
			__mcTemp__1[1] = S"Normal";
			__mcTemp__1[2] = S"Light";
			__mcTemp__1[3] = S"High";
			this->soundQualityCombo->Items->AddRange(__mcTemp__1);
			this->soundQualityCombo->Location = System::Drawing::Point(200, 168);
			this->soundQualityCombo->Name = S"soundQualityCombo";
			this->soundQualityCombo->Size = System::Drawing::Size(144, 21);
			this->soundQualityCombo->TabIndex = 11;
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(10, 109);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(168, 16);
			this->label5->TabIndex = 10;
			this->label5->Text = S"Sample rate";
			// 
			// soundSampleRateCombo
			// 
			this->soundSampleRateCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__2[] = new System::Object*[2];
			__mcTemp__2[0] = S"22khz";
			__mcTemp__2[1] = S"44khz";
			this->soundSampleRateCombo->Items->AddRange(__mcTemp__2);
			this->soundSampleRateCombo->Location = System::Drawing::Point(202, 109);
			this->soundSampleRateCombo->Name = S"soundSampleRateCombo";
			this->soundSampleRateCombo->Size = System::Drawing::Size(144, 21);
			this->soundSampleRateCombo->TabIndex = 9;
			// 
			// disableSoundCheck
			// 
			this->disableSoundCheck->Location = System::Drawing::Point(8, 136);
			this->disableSoundCheck->Name = S"disableSoundCheck";
			this->disableSoundCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->disableSoundCheck->Size = System::Drawing::Size(336, 16);
			this->disableSoundCheck->TabIndex = 8;
			this->disableSoundCheck->Text = S"Disable sound";
			this->disableSoundCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(8, 80);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(280, 16);
			this->label3->TabIndex = 7;
			this->label3->Text = S"Sound target count";
			// 
			// sndTargetsUpDown
			// 
			this->sndTargetsUpDown->Location = System::Drawing::Point(304, 80);
			this->sndTargetsUpDown->Name = S"sndTargetsUpDown";
			this->sndTargetsUpDown->Size = System::Drawing::Size(40, 20);
			this->sndTargetsUpDown->TabIndex = 4;
			// 
			// sndEfxCheck
			// 
			this->sndEfxCheck->Location = System::Drawing::Point(8, 40);
			this->sndEfxCheck->Name = S"sndEfxCheck";
			this->sndEfxCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndEfxCheck->Size = System::Drawing::Size(336, 16);
			this->sndEfxCheck->TabIndex = 3;
			this->sndEfxCheck->Text = S"Enable EFX";
			this->sndEfxCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndAccelCheck
			// 
			this->sndAccelCheck->Location = System::Drawing::Point(8, 16);
			this->sndAccelCheck->Name = S"sndAccelCheck";
			this->sndAccelCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndAccelCheck->Size = System::Drawing::Size(336, 16);
			this->sndAccelCheck->TabIndex = 2;
			this->sndAccelCheck->Text = S"Enable sound acceleration";
			this->sndAccelCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// renderPage
			// 
			this->renderPage->Controls->Add(this->rasterTrack);
			this->renderPage->Controls->Add(this->textureLodTrack);
			this->renderPage->Controls->Add(this->label4);
			this->renderPage->Controls->Add(this->label2);
			this->renderPage->Controls->Add(this->disableShadowsCheck);
			this->renderPage->Controls->Add(this->ditherShadowsCheck);
			this->renderPage->Controls->Add(this->disableDistortionCheck);
			this->renderPage->Controls->Add(this->label1);
			this->renderPage->Controls->Add(this->renderCombo);
			this->renderPage->Controls->Add(this->force60HzCheck);
			this->renderPage->Controls->Add(this->vertSyncCheck);
			this->renderPage->Location = System::Drawing::Point(4, 22);
			this->renderPage->Name = S"renderPage";
			this->renderPage->Size = System::Drawing::Size(357, 246);
			this->renderPage->TabIndex = 2;
			this->renderPage->Text = S"Render";
			// 
			// rasterTrack
			// 
			this->rasterTrack->AutoSize = false;
			this->rasterTrack->LargeChange = 1;
			this->rasterTrack->Location = System::Drawing::Point(208, 176);
			this->rasterTrack->Maximum = 4;
			this->rasterTrack->Minimum = 1;
			this->rasterTrack->Name = S"rasterTrack";
			this->rasterTrack->Size = System::Drawing::Size(136, 16);
			this->rasterTrack->TabIndex = 17;
			this->rasterTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			this->rasterTrack->Value = 1;
			// 
			// textureLodTrack
			// 
			this->textureLodTrack->AutoSize = false;
			this->textureLodTrack->LargeChange = 1;
			this->textureLodTrack->Location = System::Drawing::Point(208, 104);
			this->textureLodTrack->Maximum = 3;
			this->textureLodTrack->Name = S"textureLodTrack";
			this->textureLodTrack->Size = System::Drawing::Size(136, 16);
			this->textureLodTrack->TabIndex = 16;
			this->textureLodTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(10, 96);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(168, 16);
			this->label4->TabIndex = 15;
			this->label4->Text = S"Texture quality";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(10, 176);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(168, 16);
			this->label2->TabIndex = 13;
			this->label2->Text = S"Rasterization quality. R1 only";
			// 
			// disableShadowsCheck
			// 
			this->disableShadowsCheck->Location = System::Drawing::Point(8, 224);
			this->disableShadowsCheck->Name = S"disableShadowsCheck";
			this->disableShadowsCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->disableShadowsCheck->Size = System::Drawing::Size(336, 16);
			this->disableShadowsCheck->TabIndex = 11;
			this->disableShadowsCheck->Text = S"Disable shadows from transluent surfaces. R2 only";
			this->disableShadowsCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// ditherShadowsCheck
			// 
			this->ditherShadowsCheck->Location = System::Drawing::Point(8, 200);
			this->ditherShadowsCheck->Name = S"ditherShadowsCheck";
			this->ditherShadowsCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->ditherShadowsCheck->Size = System::Drawing::Size(336, 16);
			this->ditherShadowsCheck->TabIndex = 10;
			this->ditherShadowsCheck->Text = S"Dither shadows. R2 only";
			this->ditherShadowsCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// disableDistortionCheck
			// 
			this->disableDistortionCheck->Location = System::Drawing::Point(8, 75);
			this->disableDistortionCheck->Name = S"disableDistortionCheck";
			this->disableDistortionCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->disableDistortionCheck->Size = System::Drawing::Size(336, 16);
			this->disableDistortionCheck->TabIndex = 9;
			this->disableDistortionCheck->Text = S"Disable screen distortion effect";
			this->disableDistortionCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 8);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(168, 16);
			this->label1->TabIndex = 7;
			this->label1->Text = S"Renderer";
			this->label1->Click += new System::EventHandler(this, label1_Click);
			// 
			// renderCombo
			// 
			this->renderCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__3[] = new System::Object*[2];
			__mcTemp__3[0] = S"R1";
			__mcTemp__3[1] = S"R2";
			this->renderCombo->Items->AddRange(__mcTemp__3);
			this->renderCombo->Location = System::Drawing::Point(200, 8);
			this->renderCombo->Name = S"renderCombo";
			this->renderCombo->Size = System::Drawing::Size(144, 21);
			this->renderCombo->TabIndex = 6;
			this->renderCombo->SelectedIndexChanged += new System::EventHandler(this, qualityComboBox_SelectedIndexChanged);
			// 
			// force60HzCheck
			// 
			this->force60HzCheck->Location = System::Drawing::Point(8, 54);
			this->force60HzCheck->Name = S"force60HzCheck";
			this->force60HzCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->force60HzCheck->Size = System::Drawing::Size(336, 16);
			this->force60HzCheck->TabIndex = 5;
			this->force60HzCheck->Text = S"Force 60Hz refresh-rate";
			this->force60HzCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// vertSyncCheck
			// 
			this->vertSyncCheck->Location = System::Drawing::Point(8, 35);
			this->vertSyncCheck->Name = S"vertSyncCheck";
			this->vertSyncCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->vertSyncCheck->Size = System::Drawing::Size(336, 16);
			this->vertSyncCheck->TabIndex = 4;
			this->vertSyncCheck->Text = S"Vertical syncronisation";
			this->vertSyncCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
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
			this->renderPage->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->rasterTrack))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->textureLodTrack))->EndInit();
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

private: System::Void label1_Click(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

private: System::Void qualityComboBox_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
		 {
		 }

};
}