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
	/// Summary for xrLauncher_benchmark_res_frm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class xrLauncher_benchmark_res_frm : public System::Windows::Forms::Form
	{
	public: 
		xrLauncher_benchmark_res_frm(void)
		{
			InitializeComponent();
		}
        
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
	private: System::Windows::Forms::Button *  OK_button;
	private: System::Windows::Forms::Label *  label2;
	private : int		m_init_state;

   	private: void Init();
	public : void _Close(int res);
	public : int  _Show(int initial_state);

	private: System::Windows::Forms::Panel *  panel1;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  c1_min_lbl;
	private: System::Windows::Forms::Label *  c1_max_lbl;
	private: System::Windows::Forms::Label *  c1_avg_lbl;
	private: System::Windows::Forms::Label *  c2_avg_lbl;
	private: System::Windows::Forms::Label *  c2_max_lbl;
	private: System::Windows::Forms::Label *  c2_min_lbl;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  c3_avg_lbl;
	private: System::Windows::Forms::Label *  c3_max_lbl;
	private: System::Windows::Forms::Label *  c3_min_lbl;
	private: System::Windows::Forms::Label *  label9;
	private: System::Windows::Forms::Label *  c4_avg_lbl;
	private: System::Windows::Forms::Label *  c4_max_lbl;
	private: System::Windows::Forms::Label *  c4_min_lbl;
	private: System::Windows::Forms::Label *  label13;




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
			System::Resources::ResourceManager *  resources = new System::Resources::ResourceManager(__typeof(xrLauncher::xrLauncher_benchmark_res_frm));
			this->panel1 = new System::Windows::Forms::Panel();
			this->label2 = new System::Windows::Forms::Label();
			this->OK_button = new System::Windows::Forms::Button();
			this->c4_avg_lbl = new System::Windows::Forms::Label();
			this->c4_max_lbl = new System::Windows::Forms::Label();
			this->c4_min_lbl = new System::Windows::Forms::Label();
			this->label13 = new System::Windows::Forms::Label();
			this->c3_avg_lbl = new System::Windows::Forms::Label();
			this->c3_max_lbl = new System::Windows::Forms::Label();
			this->c3_min_lbl = new System::Windows::Forms::Label();
			this->label9 = new System::Windows::Forms::Label();
			this->c2_avg_lbl = new System::Windows::Forms::Label();
			this->c2_max_lbl = new System::Windows::Forms::Label();
			this->c2_min_lbl = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
			this->c1_avg_lbl = new System::Windows::Forms::Label();
			this->c1_max_lbl = new System::Windows::Forms::Label();
			this->c1_min_lbl = new System::Windows::Forms::Label();
			this->label1 = new System::Windows::Forms::Label();
			this->panel1->SuspendLayout();
			this->SuspendLayout();
			// 
			// panel1
			// 
			this->panel1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->panel1->BackgroundImage = (__try_cast<System::Drawing::Image *  >(resources->GetObject(S"panel1.BackgroundImage")));
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->OK_button);
			this->panel1->Controls->Add(this->c4_avg_lbl);
			this->panel1->Controls->Add(this->c4_max_lbl);
			this->panel1->Controls->Add(this->c4_min_lbl);
			this->panel1->Controls->Add(this->label13);
			this->panel1->Controls->Add(this->c3_avg_lbl);
			this->panel1->Controls->Add(this->c3_max_lbl);
			this->panel1->Controls->Add(this->c3_min_lbl);
			this->panel1->Controls->Add(this->label9);
			this->panel1->Controls->Add(this->c2_avg_lbl);
			this->panel1->Controls->Add(this->c2_max_lbl);
			this->panel1->Controls->Add(this->c2_min_lbl);
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->c1_avg_lbl);
			this->panel1->Controls->Add(this->c1_max_lbl);
			this->panel1->Controls->Add(this->c1_min_lbl);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Location = System::Drawing::Point(0, 0);
			this->panel1->Name = S"panel1";
			this->panel1->Size = System::Drawing::Size(256, 296);
			this->panel1->TabIndex = 0;
			// 
			// label2
			// 
			this->label2->BackColor = System::Drawing::Color::Transparent;
			this->label2->Font = new System::Drawing::Font(S"Microsoft Sans Serif", 12.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, (System::Byte)204);
			this->label2->Location = System::Drawing::Point(16, 8);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(224, 24);
			this->label2->TabIndex = 17;
			this->label2->Text = S"Benchmark results:";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// OK_button
			// 
			this->OK_button->BackColor = System::Drawing::Color::Transparent;
			this->OK_button->FlatStyle = System::Windows::Forms::FlatStyle::Flat;
			this->OK_button->Location = System::Drawing::Point(160, 264);
			this->OK_button->Name = S"OK_button";
			this->OK_button->Size = System::Drawing::Size(88, 24);
			this->OK_button->TabIndex = 16;
			this->OK_button->Text = S"Ok";
			this->OK_button->Click += new System::EventHandler(this, OK_button_Click);
			// 
			// c4_avg_lbl
			// 
			this->c4_avg_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c4_avg_lbl->Location = System::Drawing::Point(144, 240);
			this->c4_avg_lbl->Name = S"c4_avg_lbl";
			this->c4_avg_lbl->Size = System::Drawing::Size(100, 16);
			this->c4_avg_lbl->TabIndex = 15;
			this->c4_avg_lbl->Text = S"skipped";
			this->c4_avg_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c4_max_lbl
			// 
			this->c4_max_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c4_max_lbl->Location = System::Drawing::Point(144, 224);
			this->c4_max_lbl->Name = S"c4_max_lbl";
			this->c4_max_lbl->Size = System::Drawing::Size(100, 16);
			this->c4_max_lbl->TabIndex = 14;
			this->c4_max_lbl->Text = S"skipped";
			this->c4_max_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c4_min_lbl
			// 
			this->c4_min_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c4_min_lbl->Location = System::Drawing::Point(144, 208);
			this->c4_min_lbl->Name = S"c4_min_lbl";
			this->c4_min_lbl->Size = System::Drawing::Size(100, 16);
			this->c4_min_lbl->TabIndex = 13;
			this->c4_min_lbl->Text = S"skipped";
			this->c4_min_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label13
			// 
			this->label13->BackColor = System::Drawing::Color::Transparent;
			this->label13->Location = System::Drawing::Point(24, 224);
			this->label13->Name = S"label13";
			this->label13->Size = System::Drawing::Size(64, 16);
			this->label13->TabIndex = 12;
			this->label13->Text = S"Config4";
			// 
			// c3_avg_lbl
			// 
			this->c3_avg_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c3_avg_lbl->Location = System::Drawing::Point(144, 184);
			this->c3_avg_lbl->Name = S"c3_avg_lbl";
			this->c3_avg_lbl->Size = System::Drawing::Size(100, 16);
			this->c3_avg_lbl->TabIndex = 11;
			this->c3_avg_lbl->Text = S"skipped";
			this->c3_avg_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c3_max_lbl
			// 
			this->c3_max_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c3_max_lbl->Location = System::Drawing::Point(144, 168);
			this->c3_max_lbl->Name = S"c3_max_lbl";
			this->c3_max_lbl->Size = System::Drawing::Size(100, 16);
			this->c3_max_lbl->TabIndex = 10;
			this->c3_max_lbl->Text = S"skipped";
			this->c3_max_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c3_min_lbl
			// 
			this->c3_min_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c3_min_lbl->Location = System::Drawing::Point(144, 152);
			this->c3_min_lbl->Name = S"c3_min_lbl";
			this->c3_min_lbl->Size = System::Drawing::Size(100, 16);
			this->c3_min_lbl->TabIndex = 9;
			this->c3_min_lbl->Text = S"skipped";
			this->c3_min_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label9
			// 
			this->label9->BackColor = System::Drawing::Color::Transparent;
			this->label9->Location = System::Drawing::Point(24, 168);
			this->label9->Name = S"label9";
			this->label9->Size = System::Drawing::Size(64, 16);
			this->label9->TabIndex = 8;
			this->label9->Text = S"Config3";
			// 
			// c2_avg_lbl
			// 
			this->c2_avg_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c2_avg_lbl->Location = System::Drawing::Point(144, 128);
			this->c2_avg_lbl->Name = S"c2_avg_lbl";
			this->c2_avg_lbl->Size = System::Drawing::Size(100, 16);
			this->c2_avg_lbl->TabIndex = 7;
			this->c2_avg_lbl->Text = S"skipped";
			this->c2_avg_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c2_max_lbl
			// 
			this->c2_max_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c2_max_lbl->Location = System::Drawing::Point(144, 112);
			this->c2_max_lbl->Name = S"c2_max_lbl";
			this->c2_max_lbl->Size = System::Drawing::Size(100, 16);
			this->c2_max_lbl->TabIndex = 6;
			this->c2_max_lbl->Text = S"skipped";
			this->c2_max_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c2_min_lbl
			// 
			this->c2_min_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c2_min_lbl->Location = System::Drawing::Point(144, 96);
			this->c2_min_lbl->Name = S"c2_min_lbl";
			this->c2_min_lbl->Size = System::Drawing::Size(100, 16);
			this->c2_min_lbl->TabIndex = 5;
			this->c2_min_lbl->Text = S"skipped";
			this->c2_min_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label5
			// 
			this->label5->BackColor = System::Drawing::Color::Transparent;
			this->label5->Location = System::Drawing::Point(24, 112);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(64, 16);
			this->label5->TabIndex = 4;
			this->label5->Text = S"Config2";
			// 
			// c1_avg_lbl
			// 
			this->c1_avg_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c1_avg_lbl->Location = System::Drawing::Point(144, 72);
			this->c1_avg_lbl->Name = S"c1_avg_lbl";
			this->c1_avg_lbl->Size = System::Drawing::Size(100, 16);
			this->c1_avg_lbl->TabIndex = 3;
			this->c1_avg_lbl->Text = S"skipped";
			this->c1_avg_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c1_max_lbl
			// 
			this->c1_max_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c1_max_lbl->Location = System::Drawing::Point(144, 56);
			this->c1_max_lbl->Name = S"c1_max_lbl";
			this->c1_max_lbl->Size = System::Drawing::Size(100, 16);
			this->c1_max_lbl->TabIndex = 2;
			this->c1_max_lbl->Text = S"skipped";
			this->c1_max_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// c1_min_lbl
			// 
			this->c1_min_lbl->BackColor = System::Drawing::Color::Transparent;
			this->c1_min_lbl->Location = System::Drawing::Point(144, 40);
			this->c1_min_lbl->Name = S"c1_min_lbl";
			this->c1_min_lbl->Size = System::Drawing::Size(100, 16);
			this->c1_min_lbl->TabIndex = 1;
			this->c1_min_lbl->Text = S"skipped";
			this->c1_min_lbl->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// label1
			// 
			this->label1->BackColor = System::Drawing::Color::Transparent;
			this->label1->Location = System::Drawing::Point(24, 56);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(64, 16);
			this->label1->TabIndex = 0;
			this->label1->Text = S"Config1";
			// 
			// xrLauncher_benchmark_res_frm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(256, 296);
			this->Controls->Add(this->panel1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
			this->Name = S"xrLauncher_benchmark_res_frm";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = S"xrLauncher_benchmark_res_frm";
			this->panel1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
	private: System::Void OK_button_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 _Close(2);
			 }

};
}