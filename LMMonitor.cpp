#include "LMMonitor.h"

LMMonitor::LMMonitor( wxWindow* parent )
:
Monitor( parent )
{

}

void LMMonitor::m_button_psiOnButtonClick( wxCommandEvent& event )
{
// TODO: Implement m_button_psiOnButtonClick
}

void LMMonitor::m_quickBookStatusOnLeftDown( wxMouseEvent& event )
{
	this->m_quickBookStatus->SetBackgroundColour(wxColour(0, 255, 0));
	this->Update();
	this->Refresh();
}
