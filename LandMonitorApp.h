#ifndef LANDMONITORAPP_H
#define LANDMONITORAPP_H

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

//#define RUNSECONDCOPY  //UNCOMMENT this to compile a second runinng copy of the program 

class LandMonitorApp : public wxApp
{
public:
	virtual bool OnInit();
	bool Monitor_Form_Load();
	void LogEntry(std::string Message);
	void PrepareFolder(std::string FolderName);
	void StandardizeString(std::string& OpString);
	float ChopSng(std::string InString);
	void MakeCSVString(std::string& OpString);
	void SetUpMaster(int MasterNum, std::string SetString);

};

#endif //LANDMONITORAPP_H
