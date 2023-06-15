///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "LM.h"

///////////////////////////////////////////////////////////////////////////

Monitor::Monitor(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxGridBagSizer* gbSizer1;
	gbSizer1 = new wxGridBagSizer(0, 0);
	gbSizer1->SetFlexibleDirection(wxBOTH);
	gbSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Status")), wxVERTICAL);

	sbSizer7->SetMinSize(wxSize(700, -1));
	m_staticText1 = new wxStaticText(sbSizer7->GetStaticBox(), wxID_ANY, wxT("DateTime Sync"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	sbSizer7->Add(m_staticText1, 0, wxALL, 5);


	gbSizer1->Add(sbSizer7, wxGBPosition(0, 0), wxGBSpan(1, 2), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer8;
	sbSizer8 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("SSE Barometer")), wxHORIZONTAL);

	m_staticText6 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Use:"), wxPoint(0, -1), wxDefaultSize, 0);
	m_staticText6->Wrap(-1);
	sbSizer8->Add(m_staticText6, 0, wxALL, 5);

	m_staticText7 = new wxStaticText(sbSizer8->GetStaticBox(), wxID_ANY, wxT("Latest:"), wxPoint(200, 0), wxDefaultSize, 0);
	m_staticText7->Wrap(-1);
	sbSizer8->Add(m_staticText7, 0, wxALL, 5);


	gbSizer1->Add(sbSizer8, wxGBPosition(0, 2), wxGBSpan(1, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer71;
	sbSizer71 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("$GPGGA")), wxHORIZONTAL);

	sbSizer71->SetMinSize(wxSize(700, -1));
	m_staticText11 = new wxStaticText(sbSizer71->GetStaticBox(), wxID_ANY, wxT("at DateTime"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText11->Wrap(-1);
	sbSizer71->Add(m_staticText11, 0, wxALL, 5);


	gbSizer1->Add(sbSizer71, wxGBPosition(1, 0), wxGBSpan(1, 2), wxALIGN_TOP, 5);

	wxStaticBoxSizer* sbSizer711;
	sbSizer711 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Good GPS")), wxHORIZONTAL);

	sbSizer711->SetMinSize(wxSize(700, -1));
	m_staticText111 = new wxStaticText(sbSizer711->GetStaticBox(), wxID_ANY, wxT("N"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText111->Wrap(-1);
	sbSizer711->Add(m_staticText111, 0, wxALL, 5);

	m_staticText1111 = new wxStaticText(sbSizer711->GetStaticBox(), wxID_ANY, wxT("W"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1111->Wrap(-1);
	sbSizer711->Add(m_staticText1111, 0, wxALL, 5);

	m_staticText11111 = new wxStaticText(sbSizer711->GetStaticBox(), wxID_ANY, wxT("Time"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText11111->Wrap(-1);
	sbSizer711->Add(m_staticText11111, 0, wxALL, 5);

	m_staticText111111 = new wxStaticText(sbSizer711->GetStaticBox(), wxID_ANY, wxT("Computer:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText111111->Wrap(-1);
	sbSizer711->Add(m_staticText111111, 0, wxALL, 5);


	gbSizer1->Add(sbSizer711, wxGBPosition(2, 0), wxGBSpan(1, 2), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer81;
	sbSizer81 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Commands")), wxVERTICAL);

	m_staticText61 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT("A Sent:"), wxPoint(0, -1), wxDefaultSize, 0);
	m_staticText61->Wrap(-1);
	sbSizer81->Add(m_staticText61, 0, wxALL, 5);

	m_staticText71 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT(" CCCC"), wxPoint(200, 0), wxDefaultSize, 0);
	m_staticText71->Wrap(-1);
	sbSizer81->Add(m_staticText71, 0, wxALL, 5);

	m_staticText611 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT("A Seen:"), wxPoint(0, -1), wxDefaultSize, 0);
	m_staticText611->Wrap(-1);
	sbSizer81->Add(m_staticText611, 0, wxALL, 5);

	m_staticText711 = new wxStaticText(sbSizer81->GetStaticBox(), wxID_ANY, wxT(" CCCC"), wxPoint(200, 0), wxDefaultSize, 0);
	m_staticText711->Wrap(-1);
	sbSizer81->Add(m_staticText711, 0, wxALL, 5);


	gbSizer1->Add(sbSizer81, wxGBPosition(1, 2), wxGBSpan(3, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer72;
	sbSizer72 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Digiquartz Barometers")), wxVERTICAL);

	sbSizer72->SetMinSize(wxSize(500, -1));
	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer(0, 4, 0, 0);

	m_staticText12 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12->Wrap(-1);
	gSizer3->Add(m_staticText12, 0, wxALL, 5);

	m_staticText121 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText121->Wrap(-1);
	gSizer3->Add(m_staticText121, 0, wxALL, 5);

	m_staticText122 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText122->Wrap(-1);
	gSizer3->Add(m_staticText122, 0, wxALL, 5);


	gSizer3->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText1221 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("3"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1221->Wrap(-1);
	gSizer3->Add(m_staticText1221, 0, wxALL, 5);

	m_staticText1222 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("5"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1222->Wrap(-1);
	gSizer3->Add(m_staticText1222, 0, wxALL, 5);

	m_staticText1223 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("6"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1223->Wrap(-1);
	gSizer3->Add(m_staticText1223, 0, wxALL, 5);


	gSizer3->Add(0, 0, 1, wxEXPAND, 5);

	m_staticText12231 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("7"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12231->Wrap(-1);
	gSizer3->Add(m_staticText12231, 0, wxALL, 5);

	m_staticText12232 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("8"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12232->Wrap(-1);
	gSizer3->Add(m_staticText12232, 0, wxALL, 5);

	m_staticText12233 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("9"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12233->Wrap(-1);
	gSizer3->Add(m_staticText12233, 0, wxALL, 5);

	m_staticText12234 = new wxStaticText(sbSizer72->GetStaticBox(), wxID_ANY, wxT("0"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText12234->Wrap(-1);
	gSizer3->Add(m_staticText12234, 0, wxALL, 5);


	sbSizer72->Add(gSizer3, 1, wxEXPAND, 5);


	gbSizer1->Add(sbSizer72, wxGBPosition(3, 0), wxGBSpan(2, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer17;
	sbSizer17 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("!EXPERTS!")), wxVERTICAL);


	gbSizer1->Add(sbSizer17, wxGBPosition(3, 1), wxGBSpan(6, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer19;
	sbSizer19 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Remote Unit Reports")), wxVERTICAL);

	wxGridBagSizer* gbSizer2;
	gbSizer2 = new wxGridBagSizer(0, 0);
	gbSizer2->SetFlexibleDirection(wxBOTH);
	gbSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText53 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Differential"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText53->Wrap(-1);
	gbSizer2->Add(m_staticText53, wxGBPosition(0, 2), wxGBSpan(1, 2), wxALIGN_CENTER | wxALL, 5);

	m_staticText54 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Window"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText54->Wrap(-1);
	gbSizer2->Add(m_staticText54, wxGBPosition(0, 4), wxGBSpan(1, 2), wxALIGN_CENTER | wxALL, 5);

	m_staticText55 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Temperatures"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText55->Wrap(-1);
	gbSizer2->Add(m_staticText55, wxGBPosition(0, 6), wxGBSpan(1, 3), wxALIGN_CENTER | wxALL, 5);

	m_staticText56 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Scaler"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText56->Wrap(-1);
	gbSizer2->Add(m_staticText56, wxGBPosition(1, 1), wxGBSpan(1, 1), wxALL, 5);

	m_staticText57 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Second"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText57->Wrap(-1);
	gbSizer2->Add(m_staticText57, wxGBPosition(1, 2), wxGBSpan(1, 1), wxALL, 5);

	m_staticText571 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Second"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText571->Wrap(-1);
	gbSizer2->Add(m_staticText571, wxGBPosition(1, 4), wxGBSpan(1, 1), wxALL, 5);

	m_staticText58 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Hour"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText58->Wrap(-1);
	gbSizer2->Add(m_staticText58, wxGBPosition(1, 3), wxGBSpan(1, 1), wxALL, 5);

	m_staticText581 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Hour"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText581->Wrap(-1);
	gbSizer2->Add(m_staticText581, wxGBPosition(1, 5), wxGBSpan(1, 1), wxALL, 5);

	m_staticText59 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Tube 1"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText59->Wrap(-1);
	gbSizer2->Add(m_staticText59, wxGBPosition(1, 6), wxGBSpan(1, 1), wxALL, 5);

	m_staticText591 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Main"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText591->Wrap(-1);
	gbSizer2->Add(m_staticText591, wxGBPosition(1, 8), wxGBSpan(1, 1), wxALL, 5);

	m_staticText60 = new wxStaticText(sbSizer19->GetStaticBox(), wxID_ANY, wxT("Tube 2"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText60->Wrap(-1);
	gbSizer2->Add(m_staticText60, wxGBPosition(1, 7), wxGBSpan(1, 1), wxALL, 5);


	sbSizer19->Add(gbSizer2, 1, wxEXPAND, 5);


	sbSizer19->Add(0, 0, 1, wxEXPAND, 5);


	gbSizer1->Add(sbSizer19, wxGBPosition(5, 0), wxGBSpan(4, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer20;
	sbSizer20 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("System Time Sync")), wxHORIZONTAL);

	m_staticText70 = new wxStaticText(sbSizer20->GetStaticBox(), wxID_ANY, wxT("Inherent"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText70->Wrap(-1);
	sbSizer20->Add(m_staticText70, 0, wxALL, 5);

	m_staticText712 = new wxStaticText(sbSizer20->GetStaticBox(), wxID_ANY, wxT("System"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText712->Wrap(-1);
	m_staticText712->SetBackgroundColour(wxColour(0, 255, 0));

	sbSizer20->Add(m_staticText712, 0, wxALL, 5);


	gbSizer1->Add(sbSizer20, wxGBPosition(4, 2), wxGBSpan(1, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer201;
	sbSizer201 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Sequence Time")), wxHORIZONTAL);


	sbSizer201->Add(0, 0, 1, wxEXPAND, 5);


	gbSizer1->Add(sbSizer201, wxGBPosition(5, 2), wxGBSpan(1, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer2011;
	sbSizer2011 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("GPS Time")), wxHORIZONTAL);


	sbSizer2011->Add(0, 0, 1, wxEXPAND, 5);


	gbSizer1->Add(sbSizer2011, wxGBPosition(6, 2), wxGBSpan(1, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer82;
	sbSizer82 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("AIR Barometer")), wxHORIZONTAL);

	m_staticText62 = new wxStaticText(sbSizer82->GetStaticBox(), wxID_ANY, wxT("Use:"), wxPoint(0, -1), wxDefaultSize, 0);
	m_staticText62->Wrap(-1);
	sbSizer82->Add(m_staticText62, 0, wxALL, 5);

	m_staticText72 = new wxStaticText(sbSizer82->GetStaticBox(), wxID_ANY, wxT("Latest:"), wxPoint(200, 0), wxDefaultSize, 0);
	m_staticText72->Wrap(-1);
	sbSizer82->Add(m_staticText72, 0, wxALL, 5);


	gbSizer1->Add(sbSizer82, wxGBPosition(7, 2), wxGBSpan(1, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer33;
	sbSizer33 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxEmptyString), wxVERTICAL);

	m_staticText80 = new wxStaticText(sbSizer33->GetStaticBox(), wxID_ANY, wxT("Start Time:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText80->Wrap(-1);
	sbSizer33->Add(m_staticText80, 0, wxALL, 5);

	m_staticText81 = new wxStaticText(sbSizer33->GetStaticBox(), wxID_ANY, wxT(" DateTime"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText81->Wrap(-1);
	sbSizer33->Add(m_staticText81, 0, wxALL, 5);

	m_checkBox3 = new wxCheckBox(sbSizer33->GetStaticBox(), wxID_ANY, wxT("Feed Watchdog"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkBox3->SetValue(true);
	sbSizer33->Add(m_checkBox3, 0, wxALL, 5);

	m_staticText82 = new wxStaticText(sbSizer33->GetStaticBox(), wxID_ANY, wxT("Calibration Barometer"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText82->Wrap(-1);
	sbSizer33->Add(m_staticText82, 0, wxALL, 5);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	m_button_psi = new wxButton(sbSizer33->GetStaticBox(), wxID_ANY, wxT("PSI Off"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer1->Add(m_button_psi, 0, wxALL, 5);

	m_staticText84 = new wxStaticText(sbSizer33->GetStaticBox(), wxID_ANY, wxT("0.00"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText84->Wrap(-1);
	bSizer1->Add(m_staticText84, 0, wxALL, 5);


	sbSizer33->Add(bSizer1, 1, wxEXPAND, 5);

	m_checkBox2 = new wxCheckBox(sbSizer33->GetStaticBox(), wxID_ANY, wxT("Emulate RFM616"), wxDefaultPosition, wxDefaultSize, 0);
	m_checkBox2->SetValue(true);
	sbSizer33->Add(m_checkBox2, 0, wxALL, 5);


	gbSizer1->Add(sbSizer33, wxGBPosition(8, 2), wxGBSpan(1, 1), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer35;
	sbSizer35 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("SOFTWARE")), wxVERTICAL);

	lblSoftVersion = new wxStaticText(sbSizer35->GetStaticBox(), wxID_ANY, wxT("Version"), wxDefaultPosition, wxDefaultSize, 0);
	lblSoftVersion->Wrap(-1);
	sbSizer35->Add(lblSoftVersion, 0, wxALL, 5);


	gbSizer1->Add(sbSizer35, wxGBPosition(9, 0), wxGBSpan(1, 3), wxEXPAND, 5);

	wxStaticBoxSizer* sbSizer36;
	sbSizer36 = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, wxT("Local Quickbook")), wxHORIZONTAL);

	m_quickBookStatus = new wxStaticText(sbSizer36->GetStaticBox(), wxID_ANY, wxT("Off"), wxDefaultPosition, wxDefaultSize, 0);
	m_quickBookStatus->Wrap(-1);
	m_quickBookStatus->SetBackgroundColour(wxColour(255, 0, 0));

	sbSizer36->Add(m_quickBookStatus, 0, wxALL, 5);

	m_staticText88 = new wxStaticText(sbSizer36->GetStaticBox(), wxID_ANY, wxT("Set up"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText88->Wrap(-1);
	sbSizer36->Add(m_staticText88, 0, wxALL, 5);


	gbSizer1->Add(sbSizer36, wxGBPosition(10, 0), wxGBSpan(1, 3), wxEXPAND, 5);


	this->SetSizer(gbSizer1);
	this->Layout();

	this->Centre(wxBOTH);

	// Connect Events
	m_button_psi->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Monitor::m_button_psiOnButtonClick), NULL, this);
	m_quickBookStatus->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Monitor::m_quickBookStatusOnLeftDown), NULL, this);
}

Monitor::~Monitor()
{
	// Disconnect Events
	m_button_psi->Disconnect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(Monitor::m_button_psiOnButtonClick), NULL, this);
	m_quickBookStatus->Disconnect(wxEVT_LEFT_DOWN, wxMouseEventHandler(Monitor::m_quickBookStatusOnLeftDown), NULL, this);

}
