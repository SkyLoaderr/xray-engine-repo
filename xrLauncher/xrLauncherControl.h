#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


#undef GetObject
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




	private: System::Windows::Forms::Button *  ApplyButton;
	private: System::Windows::Forms::Button *  OkButton;
	private: System::Windows::Forms::Button *  CancelButton;













	private: System::Windows::Forms::Panel *  panel1;
	private: System::Windows::Forms::Panel *  soundPanel;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::ComboBox *  soundQualityCombo;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::ComboBox *  soundSampleRateCombo;
	private: System::Windows::Forms::CheckBox *  disableSoundCheck;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::NumericUpDown *  sndTargetsUpDown;
	private: System::Windows::Forms::CheckBox *  sndEfxCheck;
	private: System::Windows::Forms::CheckBox *  sndAccelCheck;
	private: System::Windows::Forms::Panel *  renderPanel;
	private: System::Windows::Forms::TrackBar *  rasterTrack;
	private: System::Windows::Forms::TrackBar *  textureLodTrack;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::CheckBox *  disableShadowsCheck;
	private: System::Windows::Forms::CheckBox *  ditherShadowsCheck;
	private: System::Windows::Forms::CheckBox *  disableDistortionCheck;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::ComboBox *  renderCombo;
	private: System::Windows::Forms::CheckBox *  force60HzCheck;
	private: System::Windows::Forms::CheckBox *  vertSyncCheck;
	private: System::Windows::Forms::Button *  goSoundBtn;
	private: System::Windows::Forms::Button *  goRenderBtn;





































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
			System::Resources::ResourceManager *  resources = new System::Resources::ResourceManager(__typeof(xrLauncher::xrLauncherControl));
			this->ApplyButton = new System::Windows::Forms::Button();
			this->OkButton = new System::Windows::Forms::Button();
			this->CancelButton = new System::Windows::Forms::Button();
			this->panel1 = new System::Windows::Forms::Panel();
			this->goRenderBtn = new System::Windows::Forms::Button();
			this->goSoundBtn = new System::Windows::Forms::Button();
			this->renderPanel = new System::Windows::Forms::Panel();
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
			this->soundPanel = new System::Windows::Forms::Panel();
			this->label6 = new System::Windows::Forms::Label();
			this->soundQualityCombo = new System::Windows::Forms::ComboBox();
			this->label5 = new System::Windows::Forms::Label();
			this->soundSampleRateCombo = new System::Windows::Forms::ComboBox();
			this->disableSoundCheck = new System::Windows::Forms::CheckBox();
			this->label3 = new System::Windows::Forms::Label();
			this->sndTargetsUpDown = new System::Windows::Forms::NumericUpDown();
			this->sndEfxCheck = new System::Windows::Forms::CheckBox();
			this->sndAccelCheck = new System::Windows::Forms::CheckBox();
			this->panel1->SuspendLayout();
			this->renderPanel->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->rasterTrack))->BeginInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->textureLodTrack))->BeginInit();
			this->soundPanel->SuspendLayout();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->BeginInit();
			this->SuspendLayout();
			// 
			// ApplyButton
			// 
			this->ApplyButton->BackColor = System::Drawing::Color::Transparent;
			this->ApplyButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->ApplyButton->Location = System::Drawing::Point(272, 252);
			this->ApplyButton->Name = S"ApplyButton";
			this->ApplyButton->Size = System::Drawing::Size(80, 24);
			this->ApplyButton->TabIndex = 1;
			this->ApplyButton->Text = S"Apply";
			this->ApplyButton->Click += new System::EventHandler(this, ApplyButton_Click);
			// 
			// OkButton
			// 
			this->OkButton->BackColor = System::Drawing::Color::Transparent;
			this->OkButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->OkButton->Location = System::Drawing::Point(96, 252);
			this->OkButton->Name = S"OkButton";
			this->OkButton->Size = System::Drawing::Size(80, 24);
			this->OkButton->TabIndex = 2;
			this->OkButton->Text = S"Ok";
			this->OkButton->Click += new System::EventHandler(this, OkButton_Click);
			// 
			// CancelButton
			// 
			this->CancelButton->BackColor = System::Drawing::Color::Transparent;
			this->CancelButton->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->CancelButton->Location = System::Drawing::Point(180, 252);
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
			this->panel1->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"panel1.BackgroundImage")));
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->goRenderBtn);
			this->panel1->Controls->Add(this->goSoundBtn);
			this->panel1->Controls->Add(this->renderPanel);
			this->panel1->Controls->Add(this->soundPanel);
			this->panel1->Controls->Add(this->ApplyButton);
			this->panel1->Controls->Add(this->OkButton);
			this->panel1->Controls->Add(this->CancelButton);
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(360, 284);
			this->panel1->TabIndex = 4;
			this->panel1->Paint += new System::Windows::Forms::PaintEventHandler(this, panel1_Paint);
			// 
			// goRenderBtn
			// 
			this->goRenderBtn->BackColor = System::Drawing::Color::Transparent;
			this->goRenderBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->goRenderBtn->Location = System::Drawing::Point(80, 1);
			this->goRenderBtn->Name = S"goRenderBtn";
			this->goRenderBtn->Size = System::Drawing::Size(80, 24);
			this->goRenderBtn->TabIndex = 7;
			this->goRenderBtn->Text = S"Render";
			this->goRenderBtn->Click += new System::EventHandler(this, goRenderBtn_Click);
			// 
			// goSoundBtn
			// 
			this->goSoundBtn->BackColor = System::Drawing::Color::Transparent;
			this->goSoundBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->goSoundBtn->Location = System::Drawing::Point(1, 1);
			this->goSoundBtn->Name = S"goSoundBtn";
			this->goSoundBtn->Size = System::Drawing::Size(80, 24);
			this->goSoundBtn->TabIndex = 6;
			this->goSoundBtn->Text = S"Sound";
			this->goSoundBtn->Click += new System::EventHandler(this, goSoundBtn_Click);
			// 
			// renderPanel
			// 
			this->renderPanel->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"renderPanel.BackgroundImage")));
			this->renderPanel->Controls->Add(this->rasterTrack);
			this->renderPanel->Controls->Add(this->textureLodTrack);
			this->renderPanel->Controls->Add(this->label4);
			this->renderPanel->Controls->Add(this->label2);
			this->renderPanel->Controls->Add(this->disableShadowsCheck);
			this->renderPanel->Controls->Add(this->ditherShadowsCheck);
			this->renderPanel->Controls->Add(this->disableDistortionCheck);
			this->renderPanel->Controls->Add(this->label1);
			this->renderPanel->Controls->Add(this->renderCombo);
			this->renderPanel->Controls->Add(this->force60HzCheck);
			this->renderPanel->Controls->Add(this->vertSyncCheck);
			this->renderPanel->Location = System::Drawing::Point(1, 24);
			this->renderPanel->Name = S"renderPanel";
			this->renderPanel->Size = System::Drawing::Size(356, 224);
			this->renderPanel->TabIndex = 5;
			this->renderPanel->Visible = false;
			// 
			// rasterTrack
			// 
			this->rasterTrack->AutoSize = false;
			this->rasterTrack->BackColor = System::Drawing::Color::Silver;
			this->rasterTrack->LargeChange = 1;
			this->rasterTrack->Location = System::Drawing::Point(209, 147);
			this->rasterTrack->Maximum = 4;
			this->rasterTrack->Minimum = 1;
			this->rasterTrack->Name = S"rasterTrack";
			this->rasterTrack->Size = System::Drawing::Size(136, 16);
			this->rasterTrack->TabIndex = 28;
			this->rasterTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			this->rasterTrack->Value = 1;
			// 
			// textureLodTrack
			// 
			this->textureLodTrack->AutoSize = false;
			this->textureLodTrack->BackColor = System::Drawing::Color::Silver;
			this->textureLodTrack->LargeChange = 1;
			this->textureLodTrack->Location = System::Drawing::Point(212, 124);
			this->textureLodTrack->Maximum = 3;
			this->textureLodTrack->Name = S"textureLodTrack";
			this->textureLodTrack->Size = System::Drawing::Size(136, 16);
			this->textureLodTrack->TabIndex = 27;
			this->textureLodTrack->TickStyle = System::Windows::Forms::TickStyle::None;
			// 
			// label4
			// 
			this->label4->BackColor = System::Drawing::Color::Transparent;
			this->label4->Location = System::Drawing::Point(12, 124);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(168, 16);
			this->label4->TabIndex = 26;
			this->label4->Text = S"Texture quality";
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Location = System::Drawing::Point(11, 147);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(168, 16);
			this->label2->TabIndex = 25;
			this->label2->Text = S"Rasterization quality. R1 only";
			// 
			// disableShadowsCheck
			// 
			this->disableShadowsCheck->BackColor = System::Drawing::Color::Transparent;
			this->disableShadowsCheck->Location = System::Drawing::Point(9, 195);
			this->disableShadowsCheck->Name = S"disableShadowsCheck";
			this->disableShadowsCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->disableShadowsCheck->Size = System::Drawing::Size(336, 16);
			this->disableShadowsCheck->TabIndex = 24;
			this->disableShadowsCheck->Text = S"Disable shadows from transluent surfaces. R2 only";
			this->disableShadowsCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// ditherShadowsCheck
			// 
			this->ditherShadowsCheck->BackColor = System::Drawing::Color::Transparent;
			this->ditherShadowsCheck->Location = System::Drawing::Point(9, 171);
			this->ditherShadowsCheck->Name = S"ditherShadowsCheck";
			this->ditherShadowsCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->ditherShadowsCheck->Size = System::Drawing::Size(336, 16);
			this->ditherShadowsCheck->TabIndex = 23;
			this->ditherShadowsCheck->Text = S"Dither shadows. R2 only";
			this->ditherShadowsCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// disableDistortionCheck
			// 
			this->disableDistortionCheck->BackColor = System::Drawing::Color::Transparent;
			this->disableDistortionCheck->Location = System::Drawing::Point(8, 96);
			this->disableDistortionCheck->Name = S"disableDistortionCheck";
			this->disableDistortionCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->disableDistortionCheck->Size = System::Drawing::Size(336, 16);
			this->disableDistortionCheck->TabIndex = 22;
			this->disableDistortionCheck->Text = S"Disable screen distortion effect";
			this->disableDistortionCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Location = System::Drawing::Point(12, 68);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(168, 16);
			this->label1->TabIndex = 21;
			this->label1->Text = S"Renderer";
			// 
			// renderCombo
			// 
			this->renderCombo->BackColor = System::Drawing::Color::Silver;
			this->renderCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__1[] = new System::Object*[2];
			__mcTemp__1[0] = S"R1";
			__mcTemp__1[1] = S"R2";
			this->renderCombo->Items->AddRange(__mcTemp__1);
			this->renderCombo->Location = System::Drawing::Point(204, 64);
			this->renderCombo->Name = S"renderCombo";
			this->renderCombo->Size = System::Drawing::Size(144, 21);
			this->renderCombo->TabIndex = 20;
			// 
			// force60HzCheck
			// 
			this->force60HzCheck->BackColor = System::Drawing::Color::Transparent;
			this->force60HzCheck->Location = System::Drawing::Point(8, 28);
			this->force60HzCheck->Name = S"force60HzCheck";
			this->force60HzCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->force60HzCheck->Size = System::Drawing::Size(336, 16);
			this->force60HzCheck->TabIndex = 19;
			this->force60HzCheck->Text = S"Force 60Hz refresh-rate";
			this->force60HzCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// vertSyncCheck
			// 
			this->vertSyncCheck->BackColor = System::Drawing::Color::Transparent;
			this->vertSyncCheck->Location = System::Drawing::Point(9, 6);
			this->vertSyncCheck->Name = S"vertSyncCheck";
			this->vertSyncCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->vertSyncCheck->Size = System::Drawing::Size(336, 16);
			this->vertSyncCheck->TabIndex = 18;
			this->vertSyncCheck->Text = S"Vertical syncronisation";
			this->vertSyncCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// soundPanel
			// 
			this->soundPanel->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"soundPanel.BackgroundImage")));
			this->soundPanel->Controls->Add(this->label6);
			this->soundPanel->Controls->Add(this->soundQualityCombo);
			this->soundPanel->Controls->Add(this->label5);
			this->soundPanel->Controls->Add(this->soundSampleRateCombo);
			this->soundPanel->Controls->Add(this->disableSoundCheck);
			this->soundPanel->Controls->Add(this->label3);
			this->soundPanel->Controls->Add(this->sndTargetsUpDown);
			this->soundPanel->Controls->Add(this->sndEfxCheck);
			this->soundPanel->Controls->Add(this->sndAccelCheck);
			this->soundPanel->Location = System::Drawing::Point(1, 24);
			this->soundPanel->Name = S"soundPanel";
			this->soundPanel->Size = System::Drawing::Size(356, 224);
			this->soundPanel->TabIndex = 4;
			this->soundPanel->Visible = false;
			// 
			// label6
			// 
			this->label6->BackColor = System::Drawing::Color::Transparent;
			this->label6->Location = System::Drawing::Point(4, 156);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(168, 16);
			this->label6->TabIndex = 21;
			this->label6->Text = S"Sound quality";
			// 
			// soundQualityCombo
			// 
			this->soundQualityCombo->BackColor = System::Drawing::Color::Silver;
			this->soundQualityCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__2[] = new System::Object*[4];
			__mcTemp__2[0] = S"Default";
			__mcTemp__2[1] = S"Normal";
			__mcTemp__2[2] = S"Light";
			__mcTemp__2[3] = S"High";
			this->soundQualityCombo->Items->AddRange(__mcTemp__2);
			this->soundQualityCombo->Location = System::Drawing::Point(196, 156);
			this->soundQualityCombo->Name = S"soundQualityCombo";
			this->soundQualityCombo->Size = System::Drawing::Size(144, 21);
			this->soundQualityCombo->TabIndex = 20;
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Location = System::Drawing::Point(8, 100);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(168, 16);
			this->label5->TabIndex = 19;
			this->label5->Text = S"Sample rate";
			// 
			// soundSampleRateCombo
			// 
			this->soundSampleRateCombo->BackColor = System::Drawing::Color::Silver;
			this->soundSampleRateCombo->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			System::Object* __mcTemp__3[] = new System::Object*[2];
			__mcTemp__3[0] = S"22khz";
			__mcTemp__3[1] = S"44khz";
			this->soundSampleRateCombo->Items->AddRange(__mcTemp__3);
			this->soundSampleRateCombo->Location = System::Drawing::Point(200, 100);
			this->soundSampleRateCombo->Name = S"soundSampleRateCombo";
			this->soundSampleRateCombo->Size = System::Drawing::Size(144, 21);
			this->soundSampleRateCombo->TabIndex = 18;
			// 
			// disableSoundCheck
			// 
			this->disableSoundCheck->BackColor = System::Drawing::Color::Transparent;
			this->disableSoundCheck->Location = System::Drawing::Point(4, 124);
			this->disableSoundCheck->Name = S"disableSoundCheck";
			this->disableSoundCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->disableSoundCheck->Size = System::Drawing::Size(336, 16);
			this->disableSoundCheck->TabIndex = 17;
			this->disableSoundCheck->Text = S"Disable sound";
			this->disableSoundCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label3
			// 
			this->label3->BackColor = System::Drawing::Color::Transparent;
			this->label3->Location = System::Drawing::Point(4, 68);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(280, 16);
			this->label3->TabIndex = 16;
			this->label3->Text = S"Sound target count";
			// 
			// sndTargetsUpDown
			// 
			this->sndTargetsUpDown->Location = System::Drawing::Point(300, 68);
			this->sndTargetsUpDown->Name = S"sndTargetsUpDown";
			this->sndTargetsUpDown->Size = System::Drawing::Size(40, 20);
			this->sndTargetsUpDown->TabIndex = 15;
			// 
			// sndEfxCheck
			// 
			this->sndEfxCheck->BackColor = System::Drawing::Color::Transparent;
			this->sndEfxCheck->Location = System::Drawing::Point(4, 28);
			this->sndEfxCheck->Name = S"sndEfxCheck";
			this->sndEfxCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndEfxCheck->Size = System::Drawing::Size(336, 16);
			this->sndEfxCheck->TabIndex = 14;
			this->sndEfxCheck->Text = S"Enable EFX";
			this->sndEfxCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// sndAccelCheck
			// 
			this->sndAccelCheck->BackColor = System::Drawing::Color::Transparent;
			this->sndAccelCheck->Location = System::Drawing::Point(4, 4);
			this->sndAccelCheck->Name = S"sndAccelCheck";
			this->sndAccelCheck->RightToLeft = System::Windows::Forms::RightToLeft::Yes;
			this->sndAccelCheck->Size = System::Drawing::Size(336, 16);
			this->sndAccelCheck->TabIndex = 13;
			this->sndAccelCheck->Text = S"Enable sound acceleration";
			this->sndAccelCheck->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// xrLauncherControl
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"$this.BackgroundImage")));
			this->ClientSize = System::Drawing::Size(360, 284);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Location = System::Drawing::Point(4, 22);
			this->Name = S"xrLauncherControl";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncherControl";
			this->panel1->ResumeLayout(false);
			this->renderPanel->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->rasterTrack))->EndInit();
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->textureLodTrack))->EndInit();
			this->soundPanel->ResumeLayout(false);
			(__try_cast<System::ComponentModel::ISupportInitialize *  >(this->sndTargetsUpDown))->EndInit();
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

private: System::Void goSoundBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 renderPanel->Visible = false;
			 soundPanel->Visible = true;
		 }

private: System::Void goRenderBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 soundPanel->Visible = false;
			 renderPanel->Visible = true;
		 }

};
}