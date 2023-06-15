/* ========================================
 *
 * Brian Lucas
 * Bartol Research Institute, 2023
 *
 * Port of LandMonitor By Paul Evenson from VB6 to C++
 * V0.0 simple test program of one frame
 * ========================================
*/
#include "LandMonitorApp.h"
#include "LMMonitor.h"

#define MAJOR_VERSION 0 //changes on major revisions
#define MINOR_VERSION 0 //changes every settled change

IMPLEMENT_APP(LandMonitorApp);

bool LandMonitorApp::OnInit()
{
	LMMonitor* monitorF = new LMMonitor(NULL);
	monitorF->Show();
	return true;
}

