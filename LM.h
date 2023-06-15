///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 3.10.1-0-g8feb16b3)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/gbsizer.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class Monitor
///////////////////////////////////////////////////////////////////////////////
class Monitor : public wxFrame
{
private:

protected:
	wxStaticText* m_staticText1;
	wxStaticText* m_staticText6;
	wxStaticText* m_staticText7;
	wxStaticText* m_staticText11;
	wxStaticText* m_staticText111;
	wxStaticText* m_staticText1111;
	wxStaticText* m_staticText11111;
	wxStaticText* m_staticText111111;
	wxStaticText* m_staticText61;
	wxStaticText* m_staticText71;
	wxStaticText* m_staticText611;
	wxStaticText* m_staticText711;
	wxStaticText* m_staticText12;
	wxStaticText* m_staticText121;
	wxStaticText* m_staticText122;
	wxStaticText* m_staticText1221;
	wxStaticText* m_staticText1222;
	wxStaticText* m_staticText1223;
	wxStaticText* m_staticText12231;
	wxStaticText* m_staticText12232;
	wxStaticText* m_staticText12233;
	wxStaticText* m_staticText12234;
	wxStaticText* m_staticText53;
	wxStaticText* m_staticText54;
	wxStaticText* m_staticText55;
	wxStaticText* m_staticText56;
	wxStaticText* m_staticText57;
	wxStaticText* m_staticText571;
	wxStaticText* m_staticText58;
	wxStaticText* m_staticText581;
	wxStaticText* m_staticText59;
	wxStaticText* m_staticText591;
	wxStaticText* m_staticText60;
	wxStaticText* m_staticText70;
	wxStaticText* m_staticText712;
	wxStaticText* m_staticText62;
	wxStaticText* m_staticText72;
	wxStaticText* m_staticText80;
	wxStaticText* m_staticText81;
	wxCheckBox* m_checkBox3;
	wxStaticText* m_staticText82;
	wxButton* m_button_psi;
	wxStaticText* m_staticText84;
	wxCheckBox* m_checkBox2;
	wxStaticText* m_staticText86;
	wxStaticText* m_quickBookStatus;
	wxStaticText* m_staticText88;

	// Virtual event handlers, override them in your derived class
	virtual void m_button_psiOnButtonClick(wxCommandEvent& event) { event.Skip(); }
	virtual void m_quickBookStatusOnLeftDown(wxMouseEvent& event) { event.Skip(); }


public:

	Monitor(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Neutron Monitor on wxWidgets test"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(1051, 647), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~Monitor();

};

///////////////////////////////////////////////////////////////////////////////
/// Class House
///////////////////////////////////////////////////////////////////////////////
class House : public wxFrame
{
private:

protected:

public:

	House(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500, 300), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);

	~House();

};

