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
	wxCriticalSection csMinuteFile;
	wxThread * comThread[3];
	virtual bool OnInit();
	bool Monitor_Form_Load();
	void LogEntry(std::string Message);
	void PrepareFolder(std::string FolderName);
	void StandardizeString(std::string& OpString);
	float ChopSng(std::string InString);
	void MakeCSVString(std::string& OpString);
	void SetUpMaster(int MasterNum, std::string SetString);
	void RackData();
	void Exception(std::string Message);
	void CountSync(std::string Candidate);
	void CountNoDelimeter(std::string Candidate);
	void DecomLongTube(std::string LongTubeRecord);
	void FillIntegerArray(int IntegerArray[], int nLast, std::string InputDataString);
	void FillLongArray(long LongArray[], int nLast, std::string InputDataString);
	void FillSingleArray(float SingleArray[], int nLast, std::string InputDataString);
	void FillStringArray(std::string StringArray[], std::string InputDataString);
	void EnterMonitorHis(float Value, int NumHis, int TypHis);
	void EraseMonitorHis(float Value, int NumHis, int TypHis);
	void Process800(std::string TubeRecord800);
	int OffsetOldest(int RemAdr, int LookOffset);
	int OffsetNewest(int RemAdr, int LookOffset);
	std::string HistoryLine(int LocRing, int RemoteUnit);
	void ResetTiming();
	void Timer1_Timer();

#ifdef _WIN32  // Windows specific code
	HANDLE openSerialPort(const char* portName, const char* serialParams);
#else  // Linux specific code
	int openSerialPort(const char* portName, const char* serialParams);
#endif

};

#endif //LANDMONITORAPP_H
