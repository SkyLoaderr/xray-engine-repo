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
	/// Summary for xrLauncher_benchmark_frm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class xrLauncher_benchmark_frm : public System::Windows::Forms::Form
	{
	public: 
		xrLauncher_benchmark_frm(void)
		{
			InitializeComponent();
		}
   	private: void Init();
	private: bool check_all_correct();
	private: void prepareBenchmarkFile(LPCSTR file_name);

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



	private: System::Windows::Forms::ComboBox *  qualityComboBox;
	private: System::Windows::Forms::CheckBox *  nosoundCheckBox;
	private: System::Windows::Forms::Button *  runBenchmarkBtn;
	private: System::Windows::Forms::Button *  cancelBtn;
	private: System::Windows::Forms::Panel *  panel2;
	private: System::Windows::Forms::CheckedListBox *  configsListBox;


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
			System::Resources::ResourceManager *  resources = new System::Resources::ResourceManager(__typeof(xrLauncher::xrLauncher_benchmark_frm));
			this->qualityComboBox = new System::Windows::Forms::ComboBox();
			this->nosoundCheckBox = new System::Windows::Forms::CheckBox();
			this->runBenchmarkBtn = new System::Windows::Forms::Button();
			this->cancelBtn = new System::Windows::Forms::Button();
			this->panel2 = new System::Windows::Forms::Panel();
			this->configsListBox = new System::Windows::Forms::CheckedListBox();
			this->panel2->SuspendLayout();
			this->SuspendLayout();
			// 
			// qualityComboBox
			// 
			this->qualityComboBox->BackColor = System::Drawing::Color::Silver;
			this->qualityComboBox->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->qualityComboBox->Location = System::Drawing::Point(168, 16);
			this->qualityComboBox->Name = S"qualityComboBox";
			this->qualityComboBox->Size = System::Drawing::Size(144, 21);
			this->qualityComboBox->TabIndex = 1;
			this->qualityComboBox->SelectionChangeCommitted += new System::EventHandler(this, qualityComboBox_SelectionChangeCommitted);
			// 
			// nosoundCheckBox
			// 
			this->nosoundCheckBox->BackColor = System::Drawing::Color::Transparent;
			this->nosoundCheckBox->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->nosoundCheckBox->Location = System::Drawing::Point(168, 56);
			this->nosoundCheckBox->Name = S"nosoundCheckBox";
			this->nosoundCheckBox->Size = System::Drawing::Size(144, 16);
			this->nosoundCheckBox->TabIndex = 4;
			this->nosoundCheckBox->Text = S"no sound";
			this->nosoundCheckBox->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			this->nosoundCheckBox->CheckedChanged += new System::EventHandler(this, nosoundCheckBox_CheckedChanged);
			// 
			// runBenchmarkBtn
			// 
			this->runBenchmarkBtn->BackColor = System::Drawing::Color::Transparent;
			this->runBenchmarkBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->runBenchmarkBtn->Location = System::Drawing::Point(384, 208);
			this->runBenchmarkBtn->Name = S"runBenchmarkBtn";
			this->runBenchmarkBtn->Size = System::Drawing::Size(104, 26);
			this->runBenchmarkBtn->TabIndex = 5;
			this->runBenchmarkBtn->Text = S"Run benchmark";
			this->runBenchmarkBtn->Click += new System::EventHandler(this, runBenchmarkBtn_Click);
			// 
			// cancelBtn
			// 
			this->cancelBtn->BackColor = System::Drawing::Color::Transparent;
			this->cancelBtn->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->cancelBtn->Location = System::Drawing::Point(272, 208);
			this->cancelBtn->Name = S"cancelBtn";
			this->cancelBtn->Size = System::Drawing::Size(104, 26);
			this->cancelBtn->TabIndex = 6;
			this->cancelBtn->Text = S"Cancel";
			this->cancelBtn->Click += new System::EventHandler(this, cancelBtn_Click);
			// 
			// panel2
			// 
			this->panel2->BackColor = System::Drawing::Color::FromArgb((System::Byte)224, (System::Byte)224, (System::Byte)224);
			this->panel2->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"panel2.BackgroundImage")));
			this->panel2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel2->Controls->Add(this->configsListBox);
			this->panel2->Controls->Add(this->cancelBtn);
			this->panel2->Controls->Add(this->runBenchmarkBtn);
			this->panel2->Controls->Add(this->nosoundCheckBox);
			this->panel2->Controls->Add(this->qualityComboBox);
			this->panel2->Location = System::Drawing::Point(0, 0);
			this->panel2->Name = S"panel2";
			this->panel2->Size = System::Drawing::Size(512, 240);
			this->panel2->TabIndex = 7;
			// 
			// configsListBox
			// 
			this->configsListBox->BackColor = System::Drawing::Color::Silver;
			this->configsListBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->configsListBox->Location = System::Drawing::Point(8, 8);
			this->configsListBox->Name = S"configsListBox";
			this->configsListBox->Size = System::Drawing::Size(144, 225);
			this->configsListBox->TabIndex = 7;
			this->configsListBox->SelectedIndexChanged += new System::EventHandler(this, configsListBox_SelectedIndexChanged);
			// 
			// xrLauncher_benchmark_frm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(512, 240);
			this->Controls->Add(this->panel2);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = S"xrLauncher_benchmark_frm";
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncher_benchmark_frm";
			this->KeyDown += new System::Windows::Forms::KeyEventHandler(this, xrLauncher_benchmark_frm_KeyDown);
			this->panel2->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
	private: System::Void runBenchmarkBtn_Click(System::Object *  sender, System::EventArgs *  e);

private: System::Void cancelBtn_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			 _Close(0);
		 }

private: System::Void xrLauncher_benchmark_frm_KeyDown(System::Object *  sender, System::Windows::Forms::KeyEventArgs *  e)
		 {
			 if(e->Alt&&e->KeyCode == System::Windows::Forms::Keys::F4)
				cancelBtn_Click(0,0);
		 }
private: System::Void configsListBox_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e);
private: System::Void qualityComboBox_SelectionChangeCommitted(System::Object *  sender, System::EventArgs *  e); 
private: System::Void nosoundCheckBox_CheckedChanged(System::Object *  sender, System::EventArgs *  e);
};
}