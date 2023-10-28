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
#include <chrono>
#include <ctime>  
#include <iostream>
#include <fstream>
#include <sys/stat.h>

//#include <filesystem>

#ifdef _WIN32  // Windows specific code
#include <Windows.h>

#else  // Linux specific code
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#endif
using namespace std;


#define MAJOR_VERSION 0 //changes on major revisions
#define MINOR_VERSION 0 //changes every settled change
#define MAX_LINE_LEN 1000 //max num char in line

IMPLEMENT_APP(LandMonitorApp);

class RackThread : public wxThread
{
public:
    virtual ExitCode Entry();
};

//class Timer1Thread : public wxThread
//{
//public:
//    virtual ExitCode Entry();
//};

//New C++ port variables
LMMonitor* monitorF;
std::fstream MinuteFile;
std::fstream BareTrapFile;
int comRack_CommPort;
std::string comRack_Settings;
bool chkMinDiagnostics_Value = true; //TODO update from wx
bool chkTolerateGaps_Value = true; //TODO update from wx
const char* hexCharLUT = "0123456789ABCDEF"; //Hex char Lookup Table
bool chkEmulate616_Value = true; //TODO update from wx
bool chkLogZeroRemoteHits_Value = true; //TODO update from wx
bool chkLogDuplicates_Value = false; //TODO update from wx
bool chkRecABSTiming_Value = true; //TODO update from wx
bool chkLogBadEvents_Value = true; //TODO update from wx
std::chrono::system_clock::time_point lastTimer1; //New implimetation of VB Timer1, stores minimum time for next Timer1_Timer() 

#ifdef _WIN32  // Windows specific code
HANDLE comRack;
#else  // Linux specific code
int comRack;
#endif




//Converted Global variables from NuMonSubs-bas.txt
// Definitions for data transmission options
const int lDataCol = 3;
const int lDataRow = 8;
const int IdxMailSSE = 0;
const int IdxMailDay = 1;
const int IdxMailHHist = 2;
const int IdxMailCatch = 3;
const int IdxSFTPSSE = 4;
const int IdxSFTPDay = 5;
const int IdxSFTPHHist = 6;
const int IdxSFTPCatch = 7;
const int maxCatchDecimate = 20;
bool BareTrapEnabled;
bool BareTrapActive;
bool BareTrapOpen;
double BareTrapTime;
double BareTrapStartTime;
int nBareTrapUnits;
//std::vector<int> BareTrapUnits(2);
int BareTrapUnits[2];
long nBaresTrapped;
bool RecordRaw = true;// TODO init false
int RecordRawCount;
int BypU1;
int BypU2;
double BypInterval;
bool BypActive;
bool BypShowOnly;
double BypSensitiveTime;
double BypDuration;
int TrackGPSCount;
bool OrderTimingReset;
double TimeHorizon;
long DecimateCounter;
long DecimateModulo;
int DiagnoseLimitCount;
bool DiagnoseActive;
//std::vector<bool> Active(lDataRow);
//std::vector<int> ExTime(lDataRow);
bool Active[lDataRow];
int ExTime[lDataRow];
std::string BatSFTPSSE;
std::string BatSFTPDay;
std::string BatSFTPHHist;
std::string BatSFTPCatch;
int SSECount;
bool RecordHitSpool;
bool SpoolStartOK;
bool StartHitSpool;
int SpoolUnitsOpen;
int LastSpoolUnit;
const std::string B64Codes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const int minLookBack = 10;
const int minEventsInRing = 20;
const int mDim = 10;
const int maxMaster = 7;
const int Mod16Bit = 65536;
const int MaxLong = 2147483647;
const double MarkOvr = 4294967296.0;
const int AbsoluteLastUnit = 25;
const int maxHitsPerUnit = 40;
const int AbsoluteLastHistogram = 149;
const int AbsoluteLastCrossHistogram = 55;
const int RingLast = 1023;
const int RingMod = 1024;
const int LastRawSyncType = 4;
const int MaxHitTrack = 5;
//std::vector<std::vector<long>> HitTrack(MaxHitTrack, std::vector<long>(AbsoluteLastUnit));
//std::vector<long> RawSyncCount(LastRawSyncType);
//std::vector<long> RecordCount(5 + 2 * AbsoluteLastUnit);
//std::vector<bool> TimingLost(AbsoluteLastUnit);
//std::vector<bool> PrevTimingLost(AbsoluteLastUnit);
long HitTrack[MaxHitTrack][(AbsoluteLastUnit + 1)];
long RawSyncCount[LastRawSyncType];
long RecordCount[5 + 2 * AbsoluteLastUnit];
bool TimingLost[(AbsoluteLastUnit + 1)];
bool PrevTimingLost[(AbsoluteLastUnit + 1)];
bool RackInSync;
//long nRead;
DWORD nRead;
long nReadSecond;
double nReadTotal;
double nSyncSlip;
double nSyncSlipTotal;
double nOverflowGapA;
double nLongTubeInternalErr;
const int maxSyncType = 5;
double SyncEventCount;
//std::vector<double> SyncTypeCount(maxSyncType);
//std::vector<double> NoDelimiterCount(maxSyncType);
double SyncTypeCount[maxSyncType];
double NoDelimiterCount[maxSyncType];
std::string SequenceListString;
//std::vector<std::vector<std::string>> LatestRemoteTiming(AbsoluteLastUnit, std::vector<std::string>(2));
std::string LatestRemoteTiming[(AbsoluteLastUnit + 1)][2];
long nNoRackSync;
double ShellRetCode;
long n500Transmit;
bool Zap;
bool FeedWatchdog;
double ZapResolve;
double ZapOffset;
int TotalUnits;
bool CatchCompact;
int minHitUnit;
int maxHitUnit;
int UnitSpread;
int LastContiguousUnit;
std::string CatchDecimateInit;
//std::vector<int> CatchDecimateMod(maxCatchDecimate);
//std::vector<long> CatchAvailableCounter(maxCatchDecimate);
//std::vector<long> CatchDecimatedCounter(maxCatchDecimate);
//std::vector<std::vector<int>> HourCompactMatrix(maxCatchDecimate, std::vector<int>(AbsoluteLastUnit));
//std::vector<std::string> UnitList(AbsoluteLastUnit);
//std::vector<long> EmptyRemoteCount(AbsoluteLastUnit);
//std::vector<long> EarlyOverCount(AbsoluteLastUnit);
//std::vector<long> LateOverCount(AbsoluteLastUnit);
//std::vector<long> BadGPSCount(AbsoluteLastUnit);
//std::vector<long> UnitHitCount(AbsoluteLastUnit);
int CatchDecimateMod[maxCatchDecimate];
long CatchAvailableCounter[maxCatchDecimate];
long CatchDecimatedCounter[maxCatchDecimate];
int HourCompactMatrix[maxCatchDecimate][(AbsoluteLastUnit + 1)];
std::string UnitList[(AbsoluteLastUnit + 1)];
long EmptyRemoteCount[(AbsoluteLastUnit + 1)];
long EarlyOverCount[(AbsoluteLastUnit + 1)];
long LateOverCount[(AbsoluteLastUnit + 1)];
long BadGPSCount[(AbsoluteLastUnit + 1)];
long UnitHitCount[(AbsoluteLastUnit + 1)];

//std::vector<std::vector<double>> HitTimes(AbsoluteLastUnit, std::vector<double>(maxHitsPerUnit));
//std::vector<std::vector<int>> HitPHA(AbsoluteLastUnit, std::vector<int>(maxHitsPerUnit));
//std::vector<std::vector<bool>> HitSelect(AbsoluteLastUnit, std::vector<bool>(maxHitsPerUnit));
//std::vector<std::string> HitFileName(AbsoluteLastUnit);
double HitTimes[(AbsoluteLastUnit + 1)][maxHitsPerUnit];
int HitPHA[(AbsoluteLastUnit + 1)][maxHitsPerUnit];
bool HitSelect[(AbsoluteLastUnit + 1)][maxHitsPerUnit];
std::string HitFileName[(AbsoluteLastUnit + 1)];
long nTotalHits;
long nResolvedHits;
//bool RunSecondCopy; //Replace with macro RUNSECONDCOPY in LandMonitor.h
int EventDumpDefault;
bool FakeData;
long FakeSecond;
std::vector<bool> CrossHistogramActive(AbsoluteLastCrossHistogram);
int LastCrossHistogram;
std::vector<int> WhichCrossHistogram(AbsoluteLastCrossHistogram);
bool ListHistogramInput;
std::vector<bool> UnitIsBad(AbsoluteLastUnit);
const int maxBadGPSTimes = 20;
std::vector<bool> BufferReady(AbsoluteLastUnit);
std::vector<long> LatestTOsc(AbsoluteLastUnit);
std::vector<int> TOscCycle(AbsoluteLastUnit);
std::vector<int> MarkerPoints(AbsoluteLastUnit);
std::vector<double> MarkerBase(AbsoluteLastUnit);
std::vector<double> MarkerPeriod(AbsoluteLastUnit);
std::vector<double> MarkerError(AbsoluteLastUnit);
std::vector<double> LastMarker(AbsoluteLastUnit);
std::vector<bool> MarkerSync(AbsoluteLastUnit);
std::vector<long> PreviousThisTime(AbsoluteLastUnit);
std::vector<int> DumpCountDown(AbsoluteLastUnit);
double AbsoluteEarliestTime;
std::vector<double> EarliestSelectedTime(AbsoluteLastUnit);
const int MaxEventClass = 1;
int MaxGroupSize;
std::vector<std::vector<long>> GroupSizeDistribution(AbsoluteLastUnit, std::vector<long>(MaxEventClass));
int nValidCrossUnits;
const int LastGroupSizeRingMod = 3;
std::vector<int> LastGroupSize(LastGroupSizeRingMod - 1);
int LastGroupSizePointer;
bool CrossMultiplicityEnabled;
std::vector<std::vector<int>> CrossMatrix(AbsoluteLastUnit, std::vector<int>(AbsoluteLastUnit));
std::string StationNote;
bool StationNoteExists;
long SetInBufferSize;
// Monitor Operation Control Flags
bool AnimateMonitorSeries;
std::string MonitorDataSource;
bool AnyRemoteSeen;
int SuppressSSEMail;
bool SSEShelled;
// Data and data format
bool MinuteFileAvailable;
float MonitorPrevMaster;
bool MonitorNewRun;
std::string MonitorRunStart;
float MonitorElapsedTime;
// Remote Statistics Histograms
const int RemStatHistLastChannel = 200;
//const int LastRemStatHist = 2;
const int LastRemStatHist = 3;
//std::vector<std::vector<std::vector<long>>> RemStatHis(LastRemStatHist, std::vector<std::vector<long>>(RemStatHistLastChannel, std::vector<long>(AbsoluteLastUnit)));
//std::vector<long> RemStatHistMax(LastRemStatHist);
//std::vector<std::string> RemStatHistTag(LastRemStatHist);
//std::vector<int> EventsInSecond(AbsoluteLastUnit);
long RemStatHis[LastRemStatHist][RemStatHistLastChannel][(AbsoluteLastUnit + 1)];
long RemStatHistMax[LastRemStatHist];
std::string RemStatHistTag[LastRemStatHist];
int EventsInSecond[(AbsoluteLastUnit + 1)];
int StatHisNum;
bool StatHist;
bool MonitorHist;
bool CrossHist;
bool CrossHistShort;
// Remote Data Histograms
const int MonitorHisLastChan = 1023;
const int CrossHisLastChan = 1023;
const int MonitorRingLast = 200;
const int CrossRingLast = 200;
const int MonitorLimLast = 6;
const int MonitorLimColLast = 9;
const int CrossLimLast = 6;
const int CrossLimColLast = 9;
const int lSecondCountsRing = 7;
bool RecordMarkerRing;
bool RecordMarkerSolution;
bool TiltMetersInstalled;
bool EnableBigCatch;
bool SaveSecondRing;
long nBigCatch;
double CatchSensitiveTime;
double LastCatchTime;
double LastBigCatchTime;
int BigCatchMinHits;
int BigCatchMinUnits;
int BigCatchDisplayAt;
double CatchResolution;
float BigCatchPastLimit;
float BigCatchFutureLimit;
std::vector<std::string> SecondCountsRing(lSecondCountsRing);
int SecondCountsRingPointer;
bool CatchSecondCounts;
bool EnableRackCommands;

//std::vector<std::vector<float>> MonitorLimit(MonitorLimLast, std::vector<float>(AbsoluteLastHistogram));
float MonitorLimit[(MonitorLimLast + 1)][(AbsoluteLastHistogram + 1)];
//std::vector<std::vector<std::vector<float>>> CrossLimit(CrossLimLast, std::vector<std::vector<float>>(AbsoluteLastCrossHistogram, std::vector<float>(1)));
float CrossLimit[(CrossLimLast + 1)][(AbsoluteLastCrossHistogram + 1)][2];
//std::vector<float> MonitorHisDumpInterval(AbsoluteLastHistogram);
float MonitorHisDumpInterval[(AbsoluteLastHistogram + 1)];
//std::vector<std::vector<float>> CrossHisDumpInterval(AbsoluteLastHistogram, std::vector<float>(1));
float CrossHisDumpInterval[(AbsoluteLastHistogram + 1)][2];
// Histogram parameter mnemonics
const int WMin = 0;
const int WMax = 1;
const int HBase = 2;
const int HScl = 3;
const int HDisp = 4;
const int SMin = 5;
const int SMax = 6;
// The following are provided to store the initial values of
// the histogram parameters. They are assigned defaults in the
// Form Load process of frmMonitor. Then directives in the WhatAmI
// file can override the defaults.
std::string HisParPHA300;
std::string HisParPHA600;
std::string HisParPHA700;
std::string HisParPHA800;
std::string HisParPHA900;
std::string HisParMPS;
std::string HisParMPL;
std::string HisParCRS;
std::string HisParCRL;
std::vector<long> MonitorLimColWidth(MonitorLimColLast);
std::vector<std::string> MonitorLimColLabel(MonitorLimColLast);
std::vector<long> CrossLimColWidth(CrossLimColLast);
std::vector<std::string> CrossLimColLabel(CrossLimColLast);
const int MonitorDaColLast = 10;
std::vector<long> MonitorDaColWidth(MonitorDaColLast);
//std::vector<std::string> MonitorDaColLabel(MonitorDaColLast);
std::string MonitorDaColLabel[MonitorDaColLast];
// For the monitor histograms the last index is selected or unselected
// The long and short timing have different second index ranges
// For the cross histograms there is no selection. The last index
// indicates short or long
//std::vector<std::vector<std::vector<long>>> MonitorHis(MonitorHisLastChan, std::vector<std::vector<long>>(AbsoluteLastHistogram, std::vector<long>(1)));
long MonitorHis[(MonitorHisLastChan + 1)][(AbsoluteLastHistogram + 1)][2];
//std::vector<std::vector<std::vector<long>>> CrossHis(CrossHisLastChan, std::vector<std::vector<long>>(AbsoluteLastCrossHistogram, std::vector<long>(1)));
//// The following record errors in the channel assignments
//std::vector<std::vector<long>> MonitorHisErr(AbsoluteLastHistogram, std::vector<long>(1));
//std::vector<std::vector<long>> CrossHisErr(AbsoluteLastCrossHistogram, std::vector<long>(1));
//std::vector<std::vector<std::vector<float>>> MonitorRing(MonitorRingLast, std::vector<std::vector<float>>(AbsoluteLastHistogram, std::vector<float>(1)));
//std::vector<std::vector<int>> MonitorRingLoc(AbsoluteLastHistogram, std::vector<int>(1));
//std::vector<std::vector<std::vector<float>>> CrossRing(CrossRingLast, std::vector<std::vector<float>>(AbsoluteLastCrossHistogram, std::vector<float>(1)));
//std::vector<std::vector<int>> CrossRingLoc(AbsoluteLastCrossHistogram, std::vector<int>(1));
//std::vector<std::string> MonitorHisTag(AbsoluteLastHistogram);
long CrossHis[(CrossHisLastChan + 1)][(AbsoluteLastCrossHistogram + 1)][2];
// The following record errors in the channel assignments
long MonitorHisErr[(AbsoluteLastHistogram + 1)][2];
long CrossHisErr[(AbsoluteLastCrossHistogram + 1)][2];
float MonitorRing[(MonitorRingLast + 1)][(AbsoluteLastHistogram + 1)][2];
int MonitorRingLoc[(AbsoluteLastHistogram + 1)][2];
float CrossRing[(CrossRingLast + 1)][(AbsoluteLastCrossHistogram + 1)][2];
int CrossRingLoc[(AbsoluteLastCrossHistogram + 1)][2];
std::string MonitorHisTag[(AbsoluteLastHistogram + 1)];
std::string MonitorSetFile;
int CrossHisNum;
int MonitorHisNum;
int MonitorHisType;
int MonitorHisKind;
int PHAHisOrigin;
int MPSHisOrigin;
int MOLHisOrigin;
int MOSHisOrigin;
int MPLHisOrigin;
std::string LatestFullGPS;
std::string MailServerName;
std::string EMailDailyTo;
std::string EMailDailyFrom;
std::string EmailCatchTo;
std::string EmailCatchFrom;
std::string EmailHHistTo;
std::string EmailHHistFrom;
long FreshException;
long MinutesSinceNewlyLoaded;
bool VerboseDiagnostics;
//Date CurTime;
time_t CurTime; //manual type change -B
std::vector<long> LastTenSecond(AbsoluteLastUnit);
std::vector<long> TenSecondDelta(AbsoluteLastUnit);
std::string TenSecondString;
// To use FileSystemObject the "Project" "References" menu must be used
// to select "Microsoft Scripting Runtime "
//FileSystemObject fso;
struct stat fso; //manual type change -B
bool ClearToSendDayMinute;
bool ClearToSendDayHistogram;
bool ClearToSendDayCatch;
bool ClearToSendDayHour;
bool ClearToSFTPDay;
bool ClearToSFTPCatch;
bool PortStallEnable;
long PortStallCount;
long nOpenAttempts;
bool AutoRebootEnable;
// GPS Timing Related:
const int GPSSentencePointerLast = 69;
const int GPSSentencePointerMod = 70;
int GPSSentenceHistoryRingPointer;
std::vector<std::string> GPSSentenceHistoryRing(GPSSentencePointerLast);
const int GPSRingLast = 59;
const int GPSRingMod = 60;
int GPSRingPointer;
double LastGPSSecOfDay;
double GPSSecOfDay;

std::vector<double> GPSSecOfDayRing(GPSRingLast);
std::vector<std::vector<double>> TimeMarkerRing(GPSRingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<std::vector<std::string>> ABSStringRing(GPSRingLast, std::vector<std::string>(AbsoluteLastUnit));
std::vector<bool> ZeroABSString(AbsoluteLastUnit);
std::vector<double> SigmaDummy(GPSRingLast);
std::vector<double> SecDay(GPSRingLast);
std::vector<double> MarkTime(GPSRingLast);
long NgrGPSMessageCount;
std::string CurTimTag;
std::string CurDayTimTag;
bool GPSFresh;
bool GPSTimeSyncEnable;
bool LocalQuicklookOpen;
bool SequenceTimeSystem;
int SequenceSecond;
int SequenceMinute;
int SequenceHour;
long SequenceSecOfDay;
int GPSSecond;
int PrevGPSSecond;
int GPSMinute;
int GPSHour;
int GPSSyncSecond;
int GPSSyncCode;
long SinceGPS;
std::vector<long> GPSStatistics(3);
// (0) Number of perfect GPS (31)
// (1) Number of type 6 with incrementing time
// (2) Number of type 0 with incrementing time
// (3) Number with time not incrementing
// AIR Barometer Control
bool UseMemStick;
std::string MemStickDrive;
std::string StickFileName;
const std::string DriveChoice = "ABCDEFGHIJKL";
//std::vector<float> PressDecAIR(1);
//std::vector<std::string> FullRdgAIR(1);
//std::vector<std::string> RdgAIR(1);
std::vector<float> PressDecAIR(1);
std::vector<std::string> FullRdgAIR(1);
std::vector<std::string> RdgAIR(1);
int MonitorHisFinal;
int MonitorHisNo616;
const int maxMultiplicity = 5;
const float MultiWindow = 600.0f;
bool RecordREM0Timing;
bool RecordSyncTimes;
bool RecordFullSyncNotes;
bool RecordSSE;
std::vector<double> tInterval(AbsoluteLastUnit);
std::vector<int> cMulti(AbsoluteLastUnit);
// Command Settings File Control
//std::vector<int> Days(11);
int Days[12];
const std::string DaysString = "0,31,59,90,120,151,181,212,243,273,304,334,";
//std::vector<int> DaysLeap(11);
int DaysLeap[12];
const std::string DaysLeapString = "0,31,60,91,121,152,182,213,244,274,305,335,";
std::string StationID;
std::string StationTitle;
std::string SSEFileName;
std::string SSEString;
const int LastDisplayTag = 11;
int LastMaster;
std::vector<int> LastRemote(7);
std::vector<int> RemoteBaseAddress(7);
int LastUnit;
int NumberOfBares;
std::vector<bool> BareUnitMap(AbsoluteLastUnit);
std::vector<bool> EligibleCrossUnit(AbsoluteLastUnit);
std::vector<bool> ValidCrossUnit(AbsoluteLastUnit);
std::vector<std::vector<long>> MultiplicitySummary(maxMultiplicity, std::vector<long>(AbsoluteLastUnit));
std::vector<std::vector<long>> LastMultiplicitySummary(maxMultiplicity, std::vector<long>(AbsoluteLastUnit));
bool HitBufferOverflow;
std::vector<std::vector<long>> HitBufferOverCount(AbsoluteLastUnit, std::vector<long>(1));
const int LastOccupancyBin = 50;
float OccupancyStep;
std::vector<std::vector<long>> HitBufferOccupancy(AbsoluteLastUnit, std::vector<long>(LastOccupancyBin));
float HitBufferWarn;
float HitBufferTrip;
//std::vector<float> HitBufferLag(AbsoluteLastUnit);
//std::vector<int> NewestPointer(AbsoluteLastUnit);
//std::vector<int> OldestPointer(AbsoluteLastUnit);
//std::vector<bool> EarlyOverflow(AbsoluteLastUnit);
//std::vector<bool> LateOverflow(AbsoluteLastUnit);
//std::vector<int> LatestOverflowPointer(AbsoluteLastUnit);
//std::vector<std::vector<int>> RingPHA(RingLast, std::vector<int>(AbsoluteLastUnit));
//std::vector<std::vector<bool>> RingSelect(RingLast, std::vector<bool>(AbsoluteLastUnit));
//std::vector<std::vector<double>> RingRawFullTime(RingLast, std::vector<double>(AbsoluteLastUnit));
//std::vector<std::vector<double>> RingCorFullTime(RingLast, std::vector<double>(AbsoluteLastUnit));
//std::vector<std::vector<double>> RingGoodGPSTime(RingLast, std::vector<double>(AbsoluteLastUnit));
//std::vector<double> LastGoodGPSTime(AbsoluteLastUnit);
//std::vector<std::vector<double>> RingInterval(RingLast, std::vector<double>(AbsoluteLastUnit));
//std::vector<std::vector<long>> RingRawTOscLOB(RingLast, std::vector<long>(AbsoluteLastUnit));
//std::vector<std::vector<long>> RingRawTOscHOB(RingLast, std::vector<long>(AbsoluteLastUnit));
//std::vector<std::vector<long>> RingCorTOscHOB(RingLast, std::vector<long>(AbsoluteLastUnit));
//std::vector<std::vector<double>> RingPeriod(RingLast, std::vector<double>(AbsoluteLastUnit));
//std::vector<std::vector<double>> RingBase(RingLast, std::vector<double>(AbsoluteLastUnit));
//std::vector<std::vector<double>> RingGPSTime(RingLast, std::vector<double>(AbsoluteLastUnit));
float HitBufferLag[(AbsoluteLastUnit + 1)];
int NewestPointer[(AbsoluteLastUnit + 1)];
int OldestPointer[(AbsoluteLastUnit + 1)];
bool EarlyOverflow[(AbsoluteLastUnit + 1)];
bool LateOverflow[(AbsoluteLastUnit + 1)];
int LatestOverflowPointer[(AbsoluteLastUnit + 1)];
int RingPHA[(RingLast + 1)][(AbsoluteLastUnit + 1)];
bool RingSelect[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double RingRawFullTime[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double RingCorFullTime[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double RingGoodGPSTime[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double LastGoodGPSTime[(AbsoluteLastUnit + 1)];
double RingInterval[(RingLast + 1)][(AbsoluteLastUnit + 1)];
long RingRawTOscLOB[(RingLast + 1)][(AbsoluteLastUnit + 1)];
long RingRawTOscHOB[(RingLast + 1)][(AbsoluteLastUnit + 1)];
long RingCorTOscHOB[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double RingPeriod[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double RingBase[(RingLast + 1)][(AbsoluteLastUnit + 1)];
double RingGPSTime[(RingLast + 1)][(AbsoluteLastUnit + 1)];

//std::vector<long> Counts(AbsoluteLastUnit);
//std::vector<long> CurrentScaler(AbsoluteLastUnit);
//std::vector<long> PriorScaler(AbsoluteLastUnit);
//std::vector<long> Deltas(AbsoluteLastUnit);
//std::vector<long> PrevMinuteCounters(AbsoluteLastUnit);
//std::vector<long> MinuteCounters(AbsoluteLastUnit);
//std::vector<long> MinuteDeltas(AbsoluteLastUnit);
//std::vector<int> FirmVersion(AbsoluteLastUnit);
//std::vector<std::string> DisplayTag(LastDisplayTag);
//std::vector<std::string> UnitTag(AbsoluteLastUnit);
//std::vector<long> ResetDeltaRef(AbsoluteLastUnit);
//std::vector<bool> NotSeenYet(AbsoluteLastUnit);
//std::vector<std::vector<long>> House(6, std::vector<long>(AbsoluteLastUnit));
//std::vector<long> Below(AbsoluteLastUnit);
//std::vector<long> PrevInArray(AbsoluteLastUnit);
//std::vector<long> InArray(AbsoluteLastUnit);
//std::vector<long> Above(AbsoluteLastUnit);
//std::vector<long> HrBelow(AbsoluteLastUnit);
//std::vector<long> HrIn(AbsoluteLastUnit);
//std::vector<long> HrAbove(AbsoluteLastUnit);
//std::vector<long> MinBelow(AbsoluteLastUnit);
//std::vector<long> MinIn(AbsoluteLastUnit);
//std::vector<long> MinAbove(AbsoluteLastUnit);
long Counts[(AbsoluteLastUnit + 1)];
long CurrentScaler[(AbsoluteLastUnit + 1)];
long PriorScaler[(AbsoluteLastUnit + 1)];
long Deltas[(AbsoluteLastUnit + 1)];
long PrevMinuteCounters[(AbsoluteLastUnit + 1)];
long MinuteCounters[(AbsoluteLastUnit + 1)];
long MinuteDeltas[(AbsoluteLastUnit + 1)];
int FirmVersion[(AbsoluteLastUnit + 1)];
std::string DisplayTag[LastDisplayTag];
std::string UnitTag[(AbsoluteLastUnit + 1)];
long ResetDeltaRef[(AbsoluteLastUnit + 1)];
bool NotSeenYet[(AbsoluteLastUnit + 1)];
long House[6][(AbsoluteLastUnit + 1)];
long Below[(AbsoluteLastUnit + 1)];
long PrevInArray[(AbsoluteLastUnit + 1)];
long InArray[(AbsoluteLastUnit + 1)];
long Above[(AbsoluteLastUnit + 1)];
long HrBelow[(AbsoluteLastUnit + 1)];
long HrIn[(AbsoluteLastUnit + 1)];
long HrAbove[(AbsoluteLastUnit + 1)];
long MinBelow[(AbsoluteLastUnit + 1)];
long MinIn[(AbsoluteLastUnit + 1)];
long MinAbove[(AbsoluteLastUnit + 1)];
double LongIntervalLimit;
const int lRoger = 4;
//std::vector<std::string> RogersRing(lRoger);
std::string RogersRing[lRoger];
int RogersRingPointer;
std::string CRLF;
std::string CR;
std::string LF;
double MeanObservatoryPressure;
std::string QueueDirectory;
const int RackSyncLen = 4;
const std::string NgrGPSSync = "!GPS";
const int NgrGPSByteLen = 64;
const std::string NgrCtrSync = "!CTR";
const int NgrCtrByteLen = 204;
const std::string NgrTubeSync = "!REM";
const int NgrTubeByteLen = 76;
const std::string NgrErrorSyncA = "@ERR";
const int NgrErrorByteLenA = 8;
const std::string NgrMessageSyncA = "@MSG";
const int NgrMessageByteLenA = 8;
const std::string NgrCommandSyncA = "@CMD";
const int NgrCommandByteLenB = 4;
const std::string NgrErrorSyncB = "&ERR";
const int NgrErrorByteLenB = 8;
const std::string NgrMessageSyncB = "&MSG";
const int NgrMessageByteLenB = 8;
const std::string NgrCommandSyncB = "&CMD";
const int NgrCommandByteLenA = 4;
const std::string NgrDoneSync = "@END";
const int NgrDoneByteLen = 0;
const std::string CtrSync = "LSCN";
const int CtrByteLen = 153;
const std::string ShortTubeSync = "LSMN";
const int ShortTubeByteLen = 55;
const std::string LongTubeSync = "LSML";
const int LongTubeByteLen = 151;
const std::string DoneSync = "LEND";
const int DoneByteLen = 0;
long DoneMessageCount;
// The GPS puts out two records, but one immediately follows the other
// They are considered separately in earlier versions of the code:
const std::string GPS1Sync = "LGPS";
const int GPS1ByteLen = 44;
const std::string GPS2Sync = "SYNC";
const int GPS2ByteLen = 2;
// Now they are treated as one record
const std::string GPSSync = "SYNC";
const int GPSByteLen = 52;
bool GridLock;
bool GridCrossLock;
std::vector<std::vector<long>> CtrADC(1, std::vector<long>(12));
std::vector<std::vector<long>> CtrCTR(2, std::vector<long>(9));
float FrequencyTolerance;
double UnCorrectedCounts;
std::vector<double> UnCorrectedUnitCounts(AbsoluteLastUnit);
double CorrectedCounts;
double UnCorrectedBareCounts;
double CorrectedBareCounts;
double CorrectionPressure;
std::vector<bool> ValidRingPressure(59);
std::vector<float> PressureRing(59);
const int nEv500 = 12;
const int nEv600 = 16;
const int nEv700 = 48;
const int MaxEntry800 = 48;
int CurrentUnit;
int MasterNumber;
int RemoteInMaster;
std::vector<std::vector<float>> CalConst(1, std::vector<float>(4));
std::vector<std::string> ItemTag(4);
int ResetSent;
bool IgnoreCounter;
int ShowSelectedRemote;
int WatchThisRemote;
int DisplayThisRemote;
std::string PersonalityDirectory;
std::string VMailDirectory;
std::string MinuteDirectory;
std::string CatchDirectory;
std::string ScratchDirectory;
std::string HitSpoolDirectory;
bool MasterDebug;
std::string HourDirectory;
std::string BareTrapDirectory;
std::string HistogramDirectory;
std::string DogHouse;

std::string RemoteScratchFileName;
std::string MasterDebugFileName;
std::vector<double> DebugCount(4);
std::string MinuteFileName;
std::string CatchFileName;
std::string HourFileName;
std::string BareTrapFileName;
std::string HistogramFileName;
std::string DailyHistogramFileName;
std::string HourlyHistogramFileName;
std::string CrossMatrixFileName;
const int mBar = 2;
std::vector<std::string> BarometerFile(mBar);
std::vector<bool> BarLoaded(mBar);
std::vector<std::string> BarSerial(mBar);
std::vector<std::string> BarModel(mBar);
std::vector<double> X0(mBar);
std::vector<double> Y1(mBar);
std::vector<double> Y2(mBar);
std::vector<double> Y3(mBar);
std::vector<double> C1(mBar);
std::vector<double> C2(mBar);
std::vector<double> C3(mBar);
std::vector<double> D1(mBar);
std::vector<double> D2(mBar);
std::vector<double> T1(mBar);
std::vector<double> T2(mBar);
std::vector<double> T3(mBar);
std::vector<double> T4(mBar);
std::vector<double> T5(mBar);
std::vector<double> TSF(mBar);
std::vector<double> PSF(mBar);
const std::string CmdRackReset = "RSET";
const std::string CmdTest = "Test";
const std::string CmdAcknowledge = "Ackn";
const int MSCOMM_EV_SEND = 1;
const int MSCOMM_EV_RECEIVE = 2;
const int MSCOMM_EV_CTS = 3;
const int MSCOMM_EV_DSR = 4;
const int MSCOMM_EV_CD = 5;
const int MSCOMM_EV_RING = 6;
const int MSCOMM_EV_EOF = 7;
const int MSCOMM_ER_BREAK = 1001;
const int MSCOMM_ER_CTSTO = 1002;
const int MSCOMM_ER_DSRTO = 1003;
const int MSCOMM_ER_FRAME = 1004;
const int MSCOMM_ER_OVERRUN = 1006;
const int MSCOMM_ER_CDTO = 1007;
const int MSCOMM_ER_RXOVER = 1008;
const int MSCOMM_ER_RXPARITY = 1009;
const int MSCOMM_ER_TXFULL = 1010;

//Converted Global variables from BGSUBS-bas.txt
const int ColorRed = 0xFF;
const int ColorYellow = 0xFFFF;
const int ColorGreen = 0xFF00;
const int ColorBlue = 0xFFFF00;
const int ColorWhite = 0xFFFFFF;
const int ColorPink = 0xFF80FF;
const int maxDigiQ = 70;
bool SuspendGraphics;

// Standard Graphics
std::string LineFeed;
std::string CarRet;
std::string TabChar;

// Bit Operations Miscellany
long BitValLong[32];
int BitVal[16];
long BitMaskLong[32];

// Standard frmDisplay.graphics
// Standard Graphics
int GraphWindowIndex;
std::string GraphDest;
const float GraphWidth = 15.0;
const float GraphHeight = 18.0;
const float GraphOrX = 3.0;
const float GraphOrY = 2.0;
const float GraphCatchWidth = 15.0;
const float GraphCatchHeight = 18.0;
const float GraphCatchOrX = 3.0;
const float GraphCatchOrY = 2.0;
float StartAx[2];
float picCurrent[2];
float picCatchCurrent[2];
float picGraphicsScale;
std::string HisTypeTag[2];

// Data File Management
int CurYear;
int CurDay;
int CurMonth;
int CurDOY;
int CurHour;
int CurMin;
int PrevMin;
int CurSec;
std::string CurDate;


//Converted Global variables from Monitor-frm.txt
int CopyIndex;
int Sequence;
int Ret;
std::string Temp;
float ADC0;
float ADC1;
float ADC2;
float RawPitch[2];
float RawRoll[2];
float RawTemp[2];
int Neof;
//variant Tleof;
int Nrbo;
//auto Tlrbo;
long DelCycle;
int AirCom;
int AirOn;
int AirActive;
long AirPauseCt;
int AirCase;
long AirBeat;
//float PressDecAIR[2]; //declared  in multiple files in VB -B
float te[2];
int DigIn;
std::string LastTAS;
double DigiTemp[2];
double DigiPress[2];
long DigiA[2][maxDigiQ];
long DigiB[2][maxDigiQ];
bool DigiGood[2][maxDigiQ];
int nDQUsed[2];
int nDigiQ[2], SumDigiA[2], SSDigiA[2];
double MADigiA[2], MSDigiA[2];
double SumDigiB[2], SSDigiB[2];
double MADigiB[2], MSDigiB[2];
long MiRead;
double NRawRoll[2], SumRawRoll[2], SSRawRoll[2];
double MARawRoll[2], MSRawRoll[2];
double NRawPitch[2], SumRawPitch[2], SSRawPitch[2];
double MARawPitch[2], MSRawPitch[2];


bool LandMonitorApp::OnInit()
{
	monitorF = new LMMonitor(NULL);
	const wxString ver(_("Land Monitor V") + std::to_string(MAJOR_VERSION) + _(".") + std::to_string(MINOR_VERSION));

	monitorF->setSoftwareVer( ver );
	monitorF->Show();
    Monitor_Form_Load();
	return true;
}

//Converted code from 
bool LandMonitorApp::Monitor_Form_Load()
{
    //New C++ port variables
    std::fstream reuseFile;

    int ErrorCode;
    std::string InputStr; //manual type change -B
    int PortNo;
    std::string SetStr; //manual type change -B
    bool DeviceThere;
    int JJ;
    int I;
    int iFor;
    int jFor;
    int nBase;
    int nSlot;
    int nBlank;
    int nComma;
    int nCrossHis;
    int nKey;
    int nStar;
    std::string WorkFile; //manual type change -B
    std::string ReportString; //manual type change -B
    long vPos;
    long vIncr;
    bool MoreFiles;
    std::string KeyString; //manual type change -B
    std::string UnitListString; //manual type change -B
    std::string CrossMatrixString; //manual type change -B
    float MonDumpDefault;
    float CrossDumpDefault;
    std::string TempStr; //manual type change -B
    int nUnit;
    int nHis;
    int nScratch;
    std::string CatchString; //manual type change -B
    std::string BigCatchInit; //manual type change -B
    std::string BypassInit; //manual type change -B
    int ZapResIn;
    std::string BareTrapInit; //manual type change -B
    std::string ScratchString; //manual type change -B
    // The option to run as a second copy must be compiled in by
    // uncommenting one of the following:
    // RunSecondCopy = True
    //RunSecondCopy = false;
    // 
    // The second copy will look for the Personality folder in the
    // folder C:\\LandBaseII and will use C:\\LandBaseII\VmailerII
    // 
    // Default folders will be created in C:\LandBaseII but will not
    // have the "II" suffix. There is no restriction on the folder
    // names defined in the WhatAmI file, but these MUST be different
    // from the names in for the "first copy" as none of the generated
    // files will have the II as part of the name.
    // 
    // The Station Title will be appended to indicate whether the
    // first or second copy has generated a particular output.
    // 
    // No "first copy" of the program need be running to launch a "second copy"
    // 
    // 
    // Global Variables
    HitBufferTrip = 12; //manual # removal -B
    HitBufferWarn = 10; //manual # removal -B
    OccupancyStep = 50; //manual # removal -B
    TimeHorizon = 1; //manual # removal -B
    DecimateCounter = 0;
    DecimateModulo = 10;
    n500Transmit = 0;
    ZapResIn = 50;
    SuspendGraphics = false;
    LastGPSSecOfDay = 0; //manual # removal -B
    MinuteFileAvailable = false;
    RecordHitSpool = false;
    StartHitSpool = false;
    nValidCrossUnits = 0;
    //LineFeed = Chr(10);
    LineFeed = '\n'; //manual char change -B
    //CarRet = Chr(13);
    CarRet = '\r'; //manual char change -B
    //TabChar = Chr(9);
    TabChar = '\t'; //manual char change -B
    MeanObservatoryPressure = 1013.25;
    FreshException = 0;
    //HisTypeTag(0) = "Unselected ";
    HisTypeTag[0] = "Unselected "; //manual [] change -B
    HisTypeTag[1] = "Selected "; //manual [] change -B
    PortStallCount = 0;
    nOpenAttempts = 0;
    UseMemStick = false;
    StationNoteExists = false;
    EventDumpDefault = 10;
    MailServerName = "mail.bartol.udel.edu";
    EMailDailyTo = "nm.email.errors@gmail.com";
    EMailDailyFrom = "nm.email.errors@gmail.com";
    EmailCatchTo = "nm.email.errors@gmail.com";
    EmailCatchFrom = "nm.email.errors@gmail.com";
    EmailHHistTo = "nm.email.errors@gmail.com";
    EmailHHistFrom = "nm.email.errors@gmail.com";
    //BatSFTPSSE = @"C:\LandBase\SFTP\SSEMess.bat";
    BatSFTPSSE = "C:\\LandBase\\SFTP\\SSEMess.bat"; //TODO does this need landbseii options? -B
    BatSFTPDay = "C:\\LandBase\\SFTP\\NMData.bat";
    BatSFTPHHist = "C:\\LandBase\\SFTP\\HHist.bat";
    BatSFTPCatch = "C:\\LandBase\\SFTP\\Catch.bat";
    //BitVal(0) = 1;
    BitVal[0] = 1; //manual [] change -B
    PortStallCount = 0;
    for (iFor = 1; iFor <= 14; iFor++)
        //BitVal(iFor) = Math.Pow(2, iFor);
        BitVal[iFor] = pow(2, iFor);
    BitVal[15] = -32768;
    for (iFor = 0; iFor <= 30; iFor++)
        BitValLong[iFor] = pow(2, iFor);
    BitValLong[31] = 0x80000000;
    for (iFor = 0; iFor <= 31; iFor++)
        BitMaskLong[iFor] = (pow(2, iFor)) - 1;
    BitMaskLong[31] = 0xFFFF;
    // Defaults
    // Initialize arrays, etc., for histogram analysis
    MonitorHisNum = 0;
    MonitorHisType = 0;
    //MonitorLimColLabel(0) = "Limit";
    MonitorLimColLabel[0] = "Limit";
    MonitorLimColLabel[1] = "WMin";
    MonitorLimColLabel[2] = "WMax";
    MonitorLimColLabel[3] = "HBase";
    MonitorLimColLabel[4] = "HScl";
    MonitorLimColLabel[5] = "HDisp";
    MonitorLimColLabel[6] = "SMin";
    MonitorLimColLabel[7] = "SMax";
    MonitorLimColLabel[8] = "Dump";
    CrossLimColLabel[0] = "Limit";
    CrossLimColLabel[1] = "WMin";
    CrossLimColLabel[2] = "WMax";
    CrossLimColLabel[3] = "HBase";
    CrossLimColLabel[4] = "HScl";
    CrossLimColLabel[5] = "HDisp";
    CrossLimColLabel[6] = "SMin";
    CrossLimColLabel[7] = "SMax";
    CrossLimColLabel[8] = "Dump";
    // Histogram parameter mnemonics
    // The histograms are 1024 cells (or channels) long, numbered 0 to 1023
    // They are accumulated based on an input "Value"
    // The display program attempts to present the horizontal axis
    // in the units of this "Value", for example milliseconds.
    // 
    // The "Window" is expressed in units of the Value
    // WMin = 0 -- Window Minimum
    // WMax = 1 -- Window Maximum
    // The Window has two uses. In selecting the vertical scale of the plot,
    // only entries in the histogram between the two limits are used.
    // For the PHA values, the window determines classification for
    // the Below, In, and Above statistical accumulation.
    // 
    // For entry into the histogram, Channel=(Value-HBase)/HScl
    // Negative values are entered in Channel 0. Overflows are entered
    // into Channel 1023
    // HBase = 2
    // HScl = 3
    // 
    // The number of scaled channels displayed across the fixed width
    // of the plot is given by HDisp
    // HDisp = 4
    // 
    // SMin and SMax are currently unused in the LandMonitor program
    // SMin = 5
    // SMax = 6
    // These are the default initial settings
    HisParPHA300 = "10.0, 600.00, 0.00, 1.00, 1024.00, 0, 0";
    HisParPHA600 = "2.0, 75.00, 0.00, 1.00, 128.00, 0, 0";
    HisParPHA700 = "2.0, 75.00, 0.00, 1.00, 128.00, 0, 0";
    HisParPHA800 = "2.0, 75.00, 0.00, 1.00, 128.00, 0, 0";
    HisParPHA900 = "2.0, 75.00, 0.00, 1.00, 128.00, 0, 0";
    HisParMPS = "2.0, 1020.00, 0.00, 1.00, 1024.00, 0, 0";
    HisParMPL = "1.0, 65000.00, 0.00, 64.00, 1024.00, 0, 0";
    HisParCRS = "0.0, 4.90, 0.00, 0.00488, 1024.00, 0, 0";
    HisParCRL = "0.0, 1000.00, 0.00, 0.97656, 1024.00, 0, 0";
    BigCatchInit = "3 9 500 0.001 0.007 20";
    BypassInit = "1 17 250 T";
    BareTrapEnabled = false;
    BareTrapActive = false;
    BareTrapInit = "10 3 18 19 20 T";
    // CatchDecimateInit = "-1 1000 1000  300   100  80  40    20 10  6    2  2  1    1 1 1   1 1 1   1 1"
    CatchDecimateInit = "-1 1000 1000 1000   650 390 230   135 80 50   30 20 12   10 4 3   2 2 1   1 1";

    // Different initial settings may be entered via the WhatAmI file
    // During the run, settings may be changed in the "Display" form
    for (iFor = 0; iFor < AbsoluteLastCrossHistogram; iFor++)
        CrossHistogramActive[iFor] = false;
        //CrossHistogramActive(iFor) = false;
    MonDumpDefault = 1; //manual # removal -B
    //lblDebugMessage.Visible = false; //TODO connect to wx
    //frmTransmitData.Visible = true;
    //frmTransmitData.WindowState = 1;
    //if (RunSecondCopy)
    //{
    //    VMailDirectory = @"C:\LandBaseII\VMailer\";
    //        PersonalityDirectory = @"C:\LandBaseII\Personality\";
    //        MinuteDirectory = @"C:\LandBaseII\MinuteData\";
    //        ScratchDirectory = @"C:\LandBaseII\Scratch\";
    //        CatchDirectory = @"C:\LandBaseII\Catch\";
    //        HitSpoolDirectory = @"C:\LandBaseII\HitSpool\";
    //        HourDirectory = @"C:\LandBaseII\HourData\";
    //        BareTrapDirectory = @"C:\LandBaseII\BareTrap\";
    //        HistogramDirectory = @"C:\LandBaseII\Histograms\";
    //        QueueDirectory = @"C:\LandBaseII\SSE_Queue\";
    //        DogHouse = @"C:\LandBaseII\Doghouse\";
    //}
    //else
    //{
    //    VMailDirectory = @"C:\LandBase\VMailer\";
    //        PersonalityDirectory = @"C:\LandBase\Personality\";
    //        MinuteDirectory = @"C:\LandBase\MinuteData\";
    //        ScratchDirectory = @"C:\LandBase\Scratch\";
    //        CatchDirectory = @"C:\LandBase\Catch\";
    //        HitSpoolDirectory = @"C:\LandBase\HitSpool\";
    //        HourDirectory = @"C:\LandBase\HourData\";
    //        BareTrapDirectory = @"C:\LandBase\BareTrap\";
    //        HistogramDirectory = @"C:\LandBase\Histograms\";
    //        QueueDirectory = @"C:\LandBase\SSE_Queue\";
    //        DogHouse = @"C:\LandBase\Doghouse\";
    //}

    //Precompiler block to set default file locations -B
#ifdef defined( __linux__ ) && defined( RUNSECONDCOPY )

    VMailDirectory = "~/LandBaseII/VMailer/";
    PersonalityDirectory = "~/LandBaseII/Personality/";
    MinuteDirectory = "~/LandBaseII/MinuteData/";
    ScratchDirectory = "~/LandBaseII/Scratch/";
    CatchDirectory = "~/LandBaseII/Catch/";
    HitSpoolDirectory = "~/LandBaseII/HitSpool/";
    HourDirectory = "~/LandBaseII/HourData/";
    BareTrapDirectory = "~/LandBaseII/BareTrap/";
    HistogramDirectory = "~/LandBaseII/Histograms/";
    QueueDirectory = "~/LandBaseII/SSE_Queue/";
    DogHouse = "~/LandBaseII/Doghouse/";

#elif defined( __linux__ )

    VMailDirectory = "~/LandBase/VMailer/";
    PersonalityDirectory = "~/LandBase/Personality/";
    MinuteDirectory = "~/LandBase/MinuteData/";
    ScratchDirectory = "~/LandBase/Scratch/";
    CatchDirectory = "~/LandBase/Catch/";
    HitSpoolDirectory = "~/LandBase/HitSpool/";
    HourDirectory = "~/LandBase/HourData/";
    BareTrapDirectory = "~/LandBase/BareTrap/";
    HistogramDirectory = "~/LandBase/Histograms/";
    QueueDirectory = "~/LandBase/SSE_Queue/";
    DogHouse = "~/LandBase/Doghouse/";

#elif defined( _WIN32 ) && defined( RUNSECONDCOPY )

    VMailDirectory = "C:\\LandBaseII\\VMailer\\";
    PersonalityDirectory = "C:\\LandBaseII\\Personality\\";
    MinuteDirectory = "C:\\LandBaseII\\MinuteData\\";
    ScratchDirectory = "C:\\LandBaseII\\Scratch\\";
    CatchDirectory = "C:\\LandBaseII\\Catch\\";
    HitSpoolDirectory = "C:\\LandBaseII\\HitSpool\\";
    HourDirectory = "C:\\LandBaseII\\HourData\\";
    BareTrapDirectory = "C:\\LandBaseII\\BareTrap\\";
    HistogramDirectory = "C:\\LandBaseII\\Histograms\\";
    QueueDirectory = "C:\\LandBaseII\\SSE_Queue\\";
    DogHouse = "C:\\LandBaseII\\Doghouse\\";

#else

    VMailDirectory = "C:\\LandBase\\VMailer\\";
    PersonalityDirectory = "C:\\LandBase\\Personality\\";
    MinuteDirectory = "C:\\LandBase\\MinuteData\\";
    ScratchDirectory = "C:\\LandBase\\Scratch\\";
    CatchDirectory = "C:\\LandBase\\Catch\\";
    HitSpoolDirectory = "C:\\LandBase\\HitSpool\\";
    HourDirectory = "C:\\LandBase\\HourData\\";
    BareTrapDirectory = "C:\\LandBase\\BareTrap\\";
    HistogramDirectory = "C:\\LandBase\\Histograms\\";
    QueueDirectory = "C:\\LandBase\\SSE_Queue\\";
    DogHouse = "C:\\LandBase\\Doghouse\\";
#endif

    CrossMatrixFileName = ScratchDirectory + "Simple18_3.crm";
    StationID = "???";
    StationTitle = "Generic Neutron Monitor Station";
    FrequencyTolerance = 10; //manual # removal -B
    for (iFor = 0; iFor <= 1; iFor++)
    {
        //BarLoaded(iFor) = false;
        BarLoaded[iFor] = false;
        BarometerFile[iFor] = "Null";
    }
    // Send the daily file at this minute past midnight
    //ExTime(IdxMailDay) = 1;
    ExTime[IdxMailDay] = 1;
    ExTime[IdxSFTPDay] = 1;
    // When the daily file is mailed, suppress SSE mailing for this many minutes
    SuppressSSEMail = 0;
    for (iFor = 0; iFor < AbsoluteLastUnit; iFor++)
    {
        //UnitIsBad(iFor) = false;
        UnitIsBad[iFor] = false;
        BareUnitMap[iFor] = false;
        ValidCrossUnit[iFor] = false;
    }

    LastUnit = -1;
    MasterDebug = false;
    //lblMasterDebug.BackColor = ColorGreen; //TODO connect to wx
    //lblMasterDebug.Caption = "Off";
    PortStallEnable = false;
    AutoRebootEnable = false;
    LocalQuicklookOpen = false;
    for (iFor = 0; iFor < lDataRow; iFor++)
        Active[iFor] = false;
    AutoRebootEnable = false;
    GPSTimeSyncEnable = false;
    EnableRackCommands = false;
    SequenceTimeSystem = true;
    NumberOfBares = 0;
    UseMemStick = false;
    MemStickDrive = "E";
    RecordSSE = false;
    //chkLogBadEvents.Value = 0;//TODO connect to wx
    //chkRecABSTiming.Value = 0;
    RecordREM0Timing = false;
    RecordSyncTimes = false;
    //chkDiagnoseGPS.Value = 0;//TODO connect to wx
    RecordFullSyncNotes = false;
    RecordMarkerRing = false;
    RecordMarkerSolution = false;
    EnableBigCatch = false;
    SaveSecondRing = false;
    CrossMultiplicityEnabled = false;
    TiltMetersInstalled = false;
    FeedWatchdog = false;
    //chkFeedWatchdog.Value = 0;//TODO connect to wx
    SinceGPS = 3;
    SetInBufferSize = 8192;
    //chkSyncFail.Value = 0;//TODO connect to wx
    // Read and interpret the WhatAmI file
    //Close(); //no file should be open not sure need for close -B
    //Open(PersonalityDirectory + "WhatAmI.gpc");
    reuseFile.open( PersonalityDirectory + "WhatAmI.gpc", reuseFile.in);
    //Line(Input, InputStr);
    getline(reuseFile, InputStr);
    //MoreFiles = Strings.InStr(InputStr, "EndOfFileSpecs") == 0;
    MoreFiles = InputStr.find( "EndOfFileSpecs" ) == std::string::npos;
    while (MoreFiles)
    {
        //LogEntry("WhatAmI Entry:" + " " + InputStr);
        LogEntry("WhatAmI Entry:" + ' ' + InputStr);
        //if (Left(InputStr, 1) == "*")
        if (0 == InputStr.length()) //line is blank, can't check 1st char
            KeyString = "NoKey";
        else if ('*' == InputStr.at(0))
            KeyString = "Comment";
        else
        {
            //nKey = Strings.InStr(InputStr, ":");
            nKey = InputStr.find(':');
            if (nKey > 0)
            {
                //KeyString = Left(InputStr, nKey - 1);
                KeyString = InputStr.substr(0, nKey);
                //nStar = Strings.InStr(InputStr, "*");
                nStar = InputStr.find('*');
                if (nStar > 0)
                    InputStr.resize( nStar );
                    //InputStr = Left(InputStr, nStar - 1);
            }
            else
                KeyString = "NoKey";
        }
        //switch (KeyString)
        //{
        //case "SyncFailCycle":
        if(0 == KeyString.compare("SyncFailCycle")) //case replacement for string compare -B
        {
            //chkSyncFail.Value = 1; //TODO connect to wx
            //break; //case replacement for string compare -B
        }

        else if(0 == KeyString.compare("InBufferSize")) //case replacement for string compare -B
        {
            //SetInBufferSize = System.Convert.ToInt64(Mid(InputStr, nKey + 2));
            SetInBufferSize = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("CatchDirectory")) //case replacement for string compare -B
        {
            CatchDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HitSpoolDirectory")) //case replacement for string compare -B
        {
            HitSpoolDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("ScratchDirectory")) //case replacement for string compare -B
        {
            ScratchDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("MinuteDirectory")) //case replacement for string compare -B
        {
            MinuteDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HourDirectory")) //case replacement for string compare -B
        {
            HourDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BareTrapDirectory")) //case replacement for string compare -B
        {
            BareTrapDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HistogramDirectory")) //case replacement for string compare -B
        {
            HistogramDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("CrossMatrixFile")) //case replacement for string compare -B
        {
            CrossMultiplicityEnabled = true;
            CrossMatrixFileName = PersonalityDirectory + InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("QueueDirectory")) //case replacement for string compare -B
        {
            QueueDirectory = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("StationID")) //case replacement for string compare -B
        {
            StationID = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("StationTitle")) //case replacement for string compare -B
        {
            StationTitle = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("StationNote")) //case replacement for string compare -B
        {
            StationNote = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            StationNoteExists = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("MemoryStick")) //case replacement for string compare -B
        {
            //MemStickDrive = InputStr.substr( nKey + 2, 1); //replacement for Mid -B TODO Linux
            MemStickDrive = InputStr.at( nKey + 2); //replacement for Mid -B TODO Linux
            UseMemStick = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EndOfFileSpecs")) //case replacement for string compare -B
        {
            // Establish the current system time
            //CurTime = DateTime.Now;
            //CurDate = Format(CurTime, "yy_mm_dd");
            //CurDayTimTag = Format(CurTime, "yy/mm/dd hh:mm:ss");
            char fCurTime[20];
            std::time(&CurTime);
            tm* tmCurTime = std::localtime(&CurTime);
            strftime(fCurTime, std::size(fCurTime), "%y_%m_%d", tmCurTime);
            CurDate = fCurTime;
            strftime(fCurTime, std::size(fCurTime), "%y/%m/%d %T", tmCurTime);
            CurDayTimTag = fCurTime;
            //lblStartupTime.Caption = CurDayTimTag; //TODO connect to wx
            //CurYear = Val(Format(CurTime, "yyyy"));
            //CurMonth = Val(Format(CurTime, "mm"));
            //CurDay = Val(Format(CurTime, "dd"));
            //CurHour = Val(Format(CurTime, "hh"));
            //CurMin = Val(Format(CurTime, "nn"));
            //CurSec = Val(Format(CurTime, "ss"));
            //CurDOY = DayOfYear(CurMonth, CurDay, CurYear);
            tmCurTime = std::localtime(&CurTime);
            CurYear = 1900 + tmCurTime->tm_year;
            CurMonth = 1 + tmCurTime->tm_mon;
            CurDay = tmCurTime->tm_mday;
            CurHour = tmCurTime->tm_hour;
            CurMin = tmCurTime->tm_min;
            CurSec = tmCurTime->tm_sec;
            CurDOY = tmCurTime->tm_yday;
            //lblComputerTime.Caption = CurDayTimTag;  //TODO connect to wx
            // Initialize the GPS time to something reasonable
            GPSHour = CurHour;
            GPSMinute = CurMin;
            GPSSecond = CurSec;
            PrevGPSSecond = (GPSSecond + 59) % 60;
            PrepareFolder(DogHouse);
            PrepareFolder(HourDirectory);
            PrepareFolder(MinuteDirectory);
            PrepareFolder(ScratchDirectory);
            PrepareFolder(CatchDirectory);
            PrepareFolder(HitSpoolDirectory);
            PrepareFolder(HistogramDirectory);
            PrepareFolder(QueueDirectory);
            // Generate a name for the general histogram file
            // This is used for dumping specific histograms in real time
            // The daily and hourly histogram dumps generate their own file names
            HistogramFileName = HistogramDirectory + StationID + "_" + CurDate + ".HIS";
            // Generate the names of the data files
            CatchFileName = CatchDirectory + StationID + "_" + CurDate + ".Catch";
            MinuteFileName = MinuteDirectory + StationID + "_" + CurDate + ".DAT";
            //StickFileName = MemStickDrive + @":\" + StationID + "_" + CurDate + ".DAT";
            StickFileName = MemStickDrive + ":\\" + StationID + '_' + CurDate + ".DAT";
                // The HitSpool files may never be opened
                for (iFor = 0; iFor < AbsoluteLastUnit; iFor++)
                {
                    HitFileName[iFor] = HitSpoolDirectory + StationID;
                    //HitFileName[iFor] = HitFileName(iFor); //redundant -B
                    //HitFileName[iFor] = HitFileName(iFor) + "_" + CurDate + "_U" + Right(Format(iFor + 100), 2) + ".HitSpool";
                    HitFileName[iFor] += "_" + CurDate + "_U";
                    char mod100iFor[2];
                    snprintf(mod100iFor, 2, "%02d", iFor % 100);
                    HitFileName[iFor] += mod100iFor;
                    HitFileName[iFor] += ".HitSpool";
                }
            // Then open the data files
            //Close(); MinuteFileAvailable = false;
            MinuteFileAvailable = false;
            //if (fso.FileExists(MinuteFileName))
            if ( 0 == stat( MinuteFileName.c_str(), &fso))
            {
                //Open(MinuteFileName); MinuteFileAvailable = true;
                MinuteFile.open(MinuteFileName, std::fstream::app | std::fstream::ate | std::fstream::out); //open for append -B
                MinuteFileAvailable = true;
            }
            else
            {
                MinuteFile.open(MinuteFileName, std::fstream::out);
                MinuteFileAvailable = true;

                //Print #2, "From: LandBase"
                //Print #2, "To: Cosray"
                //Print #2, "Subject: " & MinuteFileName
                //Print #2,
                //If StationNoteExists Then LogEntry "Station Note - " & StationNote
                //Print #2, "Form Load: " + lblComputerTime.Caption
                //Print #2, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "
                MinuteFile << "From: LandBase";
                MinuteFile << "To: Cosray";
                MinuteFile << "Subject: " << MinuteFileName;
                MinuteFile << '\n';
                if (StationNoteExists)
                    LogEntry("Station Note - " + StationNote);
                //MinuteFile <<  "Form Load: " + lblComputerTime.Caption  //TODO connect to wx
                //MinuteFile <<  "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "From: LandBase"

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "To: Cosray"

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "Subject: " & MinuteFileName

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2,
        If StationNoteExists Then LogEntry "Station Note - " & StationNote

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "Form Load: " + lblComputerTime.Caption

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
            };/* Cannot convert OnErrorGoToStatementSyntax, CONVERSION ERROR: Conversion for OnErrorGoToLabelStatement not implemented, please report this issue in 'On Error GoTo NoStick' at character 12646
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitOnErrorGoToStatement(OnErrorGoToStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.OnErrorGoToStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      On Error GoTo NoStick

 */

            if (UseMemStick)
            {
                //Close(); Open(StickFileName);
                //MemStickFile.open(StickFileName); //TODO MemStick
            }

        //NoStick: //TODO Error handling 
            ;
            ;/* Cannot convert OnErrorGoToStatementSyntax, CONVERSION ERROR: Conversion for OnErrorGoToZeroStatement not implemented, please report this issue in 'On Error GoTo 0' at character 12783
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitOnErrorGoToStatement(OnErrorGoToStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.OnErrorGoToStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:

      On Error GoTo 0

 */
            //LogEntry("Program: " + lblSoftVersion + " Modified at: " + lblSoftLocation + " "); //TODO connect to wx
            //LogEntry("Program: " + lblSoftVersion + " Modified at: " + lblSoftLocation + " ");  //TODO connect to wx
            if (UseMemStick)
            {
                //LogEntry("Memory Stick Open " + StickFileName);
                //btnUseMemStick.Caption = "Memory Stick Backup ON";  //TODO MemStick
            }

            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BareTrap")) //case replacement for string compare -B
        {
            BareTrapEnabled = true;
            BareTrapInit = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            PrepareFolder(BareTrapDirectory);
            BareTrapFileName = BareTrapDirectory + StationID + "_" + CurDate + ".BareTrap";
            //Close(); if (fso.FileExists(BareTrapFileName))
            //    Open(BareTrapFileName);
            //else
            //{
            //    Open(BareTrapFileName);
            if (0 == stat(MinuteFileName.c_str(), &fso))
            {
                //Open(MinuteFileName); MinuteFileAvailable = true;
                BareTrapFile.open(BareTrapFileName, std::fstream::app | std::fstream::ate | std::fstream::out); //open for append -B
            }
            else
            {
                BareTrapFile.open(BareTrapFileName, std::fstream::out);
                //Print #97, "From: LandBase"
                //Print #97, "To: Cosray"
                //Print #97, "Subject: " & BareTrapFileName
                //Print #97,
                //If StationNoteExists Then Print #97, "Note: " & StationNote
                //Print #97, "Date Change: " + lblComputerTime.Caption
                //Print #97, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "
                BareTrapFile << "From: LandBase";
                BareTrapFile << "To: Cosray";
                BareTrapFile << "Subject: " << BareTrapFileName;
                BareTrapFile << '\n';
                if (StationNoteExists)
                    BareTrapFile << "Note: " << StationNote;
                //BareTrapFile << "Date Change: " + lblComputerTime.Caption;  //TODO connect to wx
                //BareTrapFile << "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " ";  //TODO connect to wx
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #97, "From: LandBase"

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #97, "To: Cosray"

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #97, "Subject: " & BareTrapFileName

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #97,
        If StationNoteExists Then Print #97, "Note: " & StationNote

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #97, "Date Change: " + lblComputerTime.Caption

 */
                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #97, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
            }

            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("MonDumpDefault")) //case replacement for string compare -B
        {
            //MonDumpDefault = System.Convert.ToSingle(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
            MonDumpDefault = strtof(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EventDumpDefault")) //case replacement for string compare -B
        {
            EventDumpDefault = strtof(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("FrequencyTolerance")) //case replacement for string compare -B
        {
            FrequencyTolerance = strtof(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("TiltMetersInstalled")) //case replacement for string compare -B
        {
            TiltMetersInstalled = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SSEUseBaro")) //case replacement for string compare -B
        {
            //lblUseBaro.Caption = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B TODO connect to wx
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("DayMailerPhase")) //case replacement for string compare -B
        {
            ExTime[IdxMailDay] = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            if (ExTime[IdxMailDay] < 1)
                ExTime[IdxMailDay] = 1;
            if (ExTime[IdxMailDay] > 40)
                ExTime[IdxMailDay] = 40;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("DaySFTPPhase")) //case replacement for string compare -B
        {
            ExTime[IdxSFTPDay] = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            if (ExTime[IdxSFTPDay] < 1)
                ExTime[IdxSFTPDay] = 1;
            if (ExTime[IdxSFTPDay] > 40)
                ExTime[IdxSFTPDay] = 40;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SuppressSSEMail")) //case replacement for string compare -B
        {
            SuppressSSEMail = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            if (SuppressSSEMail < 0)
                SuppressSSEMail = 0;
            if (SuppressSSEMail > 5)
                SuppressSSEMail = 5;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SequenceTimeGPS")) //case replacement for string compare -B
        {
            SequenceTimeSystem = false;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("MailServerName")) //case replacement for string compare -B
        {
            MailServerName = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EMailDailyTo")) //case replacement for string compare -B
        {
            EMailDailyTo = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EmailCatchTo")) //case replacement for string compare -B
        {
            EmailCatchTo = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EMailDailyFrom")) //case replacement for string compare -B
        {
            EMailDailyFrom = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EmailHHistTo")) //case replacement for string compare -B
        {
            EmailHHistTo = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EmailHHistFrom")) //case replacement for string compare -B
        {
            EmailHHistFrom = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BatSFTPSSE")) //case replacement for string compare -B
        {
            BatSFTPSSE = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BatSFTPDay")) //case replacement for string compare -B
        {
            BatSFTPDay = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BatSFTPHHist")) //case replacement for string compare -B
        {
            BatSFTPHHist = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BatSFTPCatch")) //case replacement for string compare -B
        {
            BatSFTPCatch = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("PortStallEnable")) //case replacement for string compare -B
        {
            PortStallEnable = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EMailSSEEnable")) //case replacement for string compare -B
        {
            Active[IdxMailSSE] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EMailDayEnable")) //case replacement for string compare -B
        {
            Active[IdxMailDay] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EMailCatchEnable")) //case replacement for string compare -B
        {
            Active[IdxMailCatch] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EMailHourHistEnable")) //case replacement for string compare -B
        {
            Active[IdxMailHHist] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SFTPSSEEnable")) //case replacement for string compare -B
        {
            Active[IdxSFTPSSE] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SFTPDayEnable")) //case replacement for string compare -B
        {
            Active[IdxSFTPDay] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SFTPCatchEnable")) //case replacement for string compare -B
        {
            Active[IdxSFTPCatch] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SFTPHourHistEnable")) //case replacement for string compare -B
        {
            Active[IdxSFTPHHist] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("AutoRebootEnable")) //case replacement for string compare -B
        {
            AutoRebootEnable = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("GPSTimeSyncEnable")) //case replacement for string compare -B
        {
            GPSTimeSyncEnable = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("FeedWatchdog")) //case replacement for string compare -B
        {
            //chkFeedWatchdog.Value = 1;  //TODO connect to wx
            FeedWatchdog = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EmulateREM616")) //case replacement for string compare -B
        {
            //chkEmulate616.Value = 1;  //TODO connect to wx
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("TimeHorizon")) //case replacement for string compare -B
        {
            TimeHorizon = strtod(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BufferLagLimits")) //case replacement for string compare -B
        {
            ScratchString = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            StandardizeString(ScratchString);
            HitBufferTrip = ChopSng(ScratchString);
            HitBufferWarn = ChopSng(ScratchString);
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("LastMaster")) //case replacement for string compare -B
        {
            LastMaster = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BareUnitList")) //case replacement for string compare -B
        {
            UnitListString = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            MakeCSVString(UnitListString);
            //while (UnitListString.find(',') > 0)
            while (UnitListString.find(',') != std::string::npos)
            {
                NumberOfBares = NumberOfBares + 1;
                nComma = UnitListString.find(',');
                BareUnitMap[strtol(UnitListString.c_str(), NULL, 10)] = true;
                if (NumberOfBares == 1)
                {
                    // The first bare declared ends the contiguous region
                    LastContiguousUnit = strtol(UnitListString.c_str(), NULL, 10) - 1;
                }
                UnitListString = UnitListString.substr( nComma + 1, std::string::npos); //replacement for Mid -B
            }

            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("CrossUnitList")) //case replacement for string compare -B
        {
            UnitListString = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            MakeCSVString(UnitListString);
            //while (UnitListString.find(',') > 0)
            while (UnitListString.find(',') != std::string::npos)
            {
                nComma = UnitListString.find(',');
                EligibleCrossUnit[strtol(UnitListString.c_str(), NULL, 10)] = true;
                UnitListString = UnitListString.substr( nComma + 1, std::string::npos); //replacement for Mid -B
            }

            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M0")) //case replacement for string compare -B
        {
            SetUpMaster(0, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M1")) //case replacement for string compare -B
        {
            SetUpMaster(1, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M2")) //case replacement for string compare -B
        {
            SetUpMaster(2, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M3")) //case replacement for string compare -B
        {
            SetUpMaster(3, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M4")) //case replacement for string compare -B
        {
            SetUpMaster(4, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M5")) //case replacement for string compare -B
        {
            SetUpMaster(5, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M6")) //case replacement for string compare -B
        {
            SetUpMaster(6, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("M7")) //case replacement for string compare -B
        {
            SetUpMaster(7, InputStr.substr( nKey + 2, std::string::npos)); //replacement for Mid -B
            LastSpoolUnit = LastUnit;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("Barometer0")) //case replacement for string compare -B
        {
            BarometerFile[0] = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            BarLoaded[0] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("Barometer1")) //case replacement for string compare -B
        {
            BarometerFile[1] = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            BarLoaded[1] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("MeanObservatoryPressure")) //case replacement for string compare -B
        {
            MeanObservatoryPressure = strtod(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("comPSI")) //case replacement for string compare -B
        {
            InputStr = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            nBlank = InputStr.find(' ');
            PortNo = strtol(InputStr.c_str(), NULL, 10);
            InputStr = InputStr.substr( nBlank + 1, std::string::npos); //replacement for Mid -B
            nBlank = InputStr.find(' ');
            SetStr = InputStr.substr(0, nBlank);
            InputStr = InputStr.substr( nBlank + 1, std::string::npos); //replacement for Mid -B
            //comPSI.CommPort = PortNo; //TODO comms
            //comPSI.Settings = SetStr;
            //if (Strings.InStr(InputStr, "On") > 0)
            //    btnPSIonoff_Click();
            //LogEntry("Set up " + KeyString + " P:" + Format(PortNo) + " S:" + SetStr + " A:" + InputStr);
            LogEntry("Set up " + KeyString + " P:" + std::to_string(PortNo) + " S:" + SetStr + " A:" + InputStr);
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("comRack")) //case replacement for string compare -B
        {
            InputStr = InputStr.substr(nKey + 2, std::string::npos); //replacement for Mid -B
            nBlank = InputStr.find(' ');
            PortNo = strtol(InputStr.c_str(), NULL, 10);
            InputStr = InputStr.substr(nBlank + 1, std::string::npos); //replacement for Mid -B
            nBlank = InputStr.find(' ');
            SetStr = InputStr.substr(0, nBlank);
            InputStr = InputStr.substr(nBlank + 1, std::string::npos); //replacement for Mid -B
            //comRack.CommPort = PortNo; //TODO comms
            //comRack.Settings = SetStr;
            //comRack.InBufferSize = SetInBufferSize;
            comRack_CommPort = PortNo;
            comRack_Settings = SetStr;
            //if (Strings.InStr(InputStr, "On") > 0)
            //    btnRackOnOff_Click();
            if (std::string::npos != InputStr.find("On"))
            {
                char comChar[] = "COM   ";
                _ltoa(comRack_CommPort, (comChar + 3), 10);
                comRack = openSerialPort(comChar, comRack_Settings.c_str());
            }
            //LogEntry("Set up " + KeyString + " P:" + Format(PortNo) + " S:" + SetStr + " A:" + InputStr);
            LogEntry("Set up " + KeyString + " P:" + std::to_string(PortNo) + " S:" + SetStr + " A:" + InputStr);
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("comLocalQuickLook")) //case replacement for string compare -B
        {
            InputStr = InputStr.substr(nKey + 2, std::string::npos); //replacement for Mid -B
            nBlank = InputStr.find(' ');
            PortNo = strtol(InputStr.c_str(), NULL, 10);
            InputStr = InputStr.substr(nBlank + 1, std::string::npos); //replacement for Mid -B
            nBlank = InputStr.find(' ');
            SetStr = InputStr.substr(0, nBlank);
            InputStr = InputStr.substr(nBlank + 1, std::string::npos); //replacement for Mid -B
            //comLocalQuicklook.CommPort = PortNo; //TODO comms
            //comLocalQuicklook.Settings = SetStr;
            //lblLastLocalQuicklook.Caption = "Set up " + KeyString + " P:" + Format(PortNo) + " S:" + SetStr + " A:" + InputStr;
            //lblLocalQuicklookEnable.Caption = "OFF";
            //if (Strings.InStr(InputStr, "On") > 0)
            //    lblLocalQuicklookEnable_Click();
            //LogEntry("Set up " + KeyString + " P:" + Format(PortNo) + " S:" + SetStr + " A:" + InputStr);
            LogEntry("Set up " + KeyString + " P:" + std::to_string(PortNo) + " S:" + SetStr + " A:" + InputStr);
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("DiagnoseGPS")) //case replacement for string compare -B
        {
            //chkDiagnoseGPS.Value = 1; //TODO connect to wx
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("RecordHitSpool")) //case replacement for string compare -B
        {
            RecordHitSpool = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("LastSpoolUnit")) //case replacement for string compare -B
        {
            LastSpoolUnit = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("RecordAbsoluteTiming")) //case replacement for string compare -B
        {
            //chkRecABSTiming.Value = 1; //TODO connect to wx
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("RecordSSEFile")) //case replacement for string compare -B
        {
            RecordSSE = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("RecordMarkerRing")) //case replacement for string compare -B
        {
            RecordMarkerRing = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EnableRackCommands")) //case replacement for string compare -B
        {
            EnableRackCommands = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SaveSecondRing")) //case replacement for string compare -B
        {
            SaveSecondRing = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("ZapResolution")) //case replacement for string compare -B
        {
            ZapResIn = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("EnableBigCatch")) //case replacement for string compare -B
        {
            EnableBigCatch = true;
            BigCatchInit = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("Bypass")) //case replacement for string compare -B
        {
            BypassInit = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("RecordMarkerSolution")) //case replacement for string compare -B
        {
            RecordMarkerSolution = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("BadUnit")) //case replacement for string compare -B
        {
            nUnit = strtol(InputStr.substr( nKey + 2, std::string::npos).c_str(), NULL, 10); //replacement for Mid -B
            UnitIsBad[nUnit] = true;
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParPHA300")) //case replacement for string compare -B
        {
            HisParPHA300 = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParPHA600")) //case replacement for string compare -B
        {
            HisParPHA600 = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParPHA700")) //case replacement for string compare -B
        {
            HisParPHA700 = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParPHA800")) //case replacement for string compare -B
        {
            HisParPHA800 = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParMPS")) //case replacement for string compare -B
        {
            HisParMPS = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParMPL")) //case replacement for string compare -B
        {
            HisParMPL = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParCRS")) //case replacement for string compare -B
        {
            HisParCRS = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("HisParCRL")) //case replacement for string compare -B
        {
            HisParCRL = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("SelCatch")) //case replacement for string compare -B
        {
            CatchDecimateInit = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("Comment")) //case replacement for string compare -B
        {
            //break; //case replacement for string compare -B
        }

        else if( 0 == KeyString.compare("NoKey")) //case replacement for string compare -B
        {
            LogEntry("No key, treated as comment");
            //break; //case replacement for string compare -B
        }

        else //case replacement for string compare -B
        {
            LogEntry(KeyString + " invalid in WhatAmI");
            //break; //case replacement for string compare -B
        }
        //}
        //Line(Input, InputStr);
        getline(reuseFile, InputStr);
        //MoreFiles = Strings.InStr(InputStr, "EndOfInformation") == 0;
        //MoreFiles = InputStr.find("EndOfFileSpecs") == std::string::npos;
        MoreFiles = InputStr.find("EndOfInformation") == std::string::npos;
    }


    //Close();
    reuseFile.close();
    for (iFor = 0; iFor < 59; iFor++)
    {
        PressureRing[iFor] = 0; //manual # removal -B
        ValidRingPressure[iFor] = false;
    }/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParPHA300" & " " & HisParPHA300

 */
    MinuteFile << ("HisParPHA300" + ' ' + HisParPHA300 + '\n');
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParPHA600" & " " & HisParPHA600

 */
    MinuteFile << ("HisParPHA600" + ' ' + HisParPHA600 + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParPHA700" & " " & HisParPHA700

 */
    MinuteFile << ("HisParPHA700" + ' ' + HisParPHA700 + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParPHA800" & " " & HisParPHA800

 */
    MinuteFile << ("HisParPHA800" + ' ' + HisParPHA800 + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParMPS" & " " & HisParMPS

 */
    MinuteFile << ("HisParMPS" + ' ' + HisParMPS + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParMPL" & " " & HisParMPL

 */
    MinuteFile << ("HisParMPL" + ' ' + HisParMPL + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParCRS" & " " & HisParCRS

 */
    MinuteFile << ("HisParCRS" + ' ' + HisParCRS + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "HisParCRL" & " " & HisParCRL

 */
    MinuteFile << ("HisParCRL" + ' ' + HisParCRL + '\n');
    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "SelCatch" & " " & CatchDecimateInit

 */
    MinuteFile << ("SelCatch" + ' ' + CatchDecimateInit + '\n');
 
    PHAHisOrigin = 0;
    MPSHisOrigin = LastUnit + 1;
    MPLHisOrigin = 2 * (LastUnit + 1);
    MOSHisOrigin = 3 * (LastUnit + 1);
    MOLHisOrigin = 4 * (LastUnit + 1);
    //if (RecordSSE)
        //btnRecordSSE.Caption = "Rec SSE"; //TODO connect to wx
    //else
        //btnRecordSSE.Caption = "No SSE"; //TODO connect to wx
    GridLock = false;
    GridCrossLock = false;
    //for (iFor = 0; iFor <= AbsoluteLastUnit; iFor++)
    for (iFor = 0; iFor < AbsoluteLastUnit; iFor++)
        NotSeenYet[iFor] = true;
    SSEString = "1946 027 05 45 06 15";
    //for (iFor = 0; iFor <= 4; iFor++)
    for (iFor = 0; iFor < 4; iFor++)
        RogersRing[iFor] = SSEString;
    RogersRingPointer = 0;
    FillIntegerArray(Days, 11, DaysString);
    FillIntegerArray(DaysLeap, 11, DaysLeapString);

    // Set up histograms based on remotes
    for (nUnit = 0; nUnit <= LastUnit; nUnit++)
    {
        ValidCrossUnit[nUnit] = false;
        nHis = PHAHisOrigin + nUnit;
        //MonitorHisTag[nHis] = "PHA" + UnitTag(nUnit);
        MonitorHisTag[nHis] = "PHA" + UnitTag[nUnit];
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParPHA300;
        //if ((FirmVersion(nUnit) == 600))
        if ((FirmVersion[nUnit] == 600))
            TempStr = HisParPHA600;
        //if ((FirmVersion(nUnit) == 700))
        if ((FirmVersion[nUnit] == 700))
            TempStr = HisParPHA700;
        //if ((FirmVersion(nUnit) == 800))
        if ((FirmVersion[nUnit] == 800))
        {
            //ValidCrossUnit[nUnit] = EligibleCrossUnit(nUnit) & (!UnitIsBad(nUnit));
            ValidCrossUnit[nUnit] = EligibleCrossUnit[nUnit] && (!UnitIsBad[nUnit]);
            //if (ValidCrossUnit(nUnit))
            if (ValidCrossUnit[nUnit])
                nValidCrossUnits = nValidCrossUnits + 1;
            TempStr = HisParPHA800;
        }
        //if ((FirmVersion(nUnit) == 900))
        if ((FirmVersion[nUnit] == 900))
            TempStr = HisParPHA900;
        MakeCSVString(TempStr);
        //for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            MonitorLimit[iFor][nHis] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MPSHisOrigin + nUnit;
        //MonitorHisTag[nHis] = "MPS" + UnitTag(nUnit);
        MonitorHisTag[nHis] = "MPS" + UnitTag[nUnit];
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPS;
        MakeCSVString(TempStr);
        //for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        for (iFor = 0; iFor < MonitorLimLast; iFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            MonitorLimit[iFor][nHis] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MPLHisOrigin + nUnit;
        //MonitorHisTag[nHis] = "MPL" + UnitTag(nUnit);
        MonitorHisTag[nHis] = "MPL" + UnitTag[nUnit];
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPL;
        MakeCSVString(TempStr);
        //for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        for (iFor = 0; iFor < MonitorLimLast; iFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            MonitorLimit[iFor][nHis] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MOSHisOrigin + nUnit;
        //MonitorHisTag[nHis] = "MOS" + UnitTag(nUnit);
        MonitorHisTag[nHis] = "MOS" + UnitTag[nUnit];
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPS;
        MakeCSVString(TempStr);
        //for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        for (iFor = 0; iFor < MonitorLimLast; iFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            MonitorLimit[iFor][nHis] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MOLHisOrigin + nUnit;
        //MonitorHisTag[nHis] = "MOL" + UnitTag(nUnit);
        MonitorHisTag[nHis] = "MOL" + UnitTag[nUnit];
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPL;
        MakeCSVString(TempStr);
        //for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        for (iFor = 0; iFor < MonitorLimLast; iFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            MonitorLimit[iFor][nHis] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
    }

    if (nValidCrossUnits <= AbsoluteLastUnit)
        MaxGroupSize = nValidCrossUnits;
    else
        MaxGroupSize = AbsoluteLastUnit;
    MonitorHisFinal = nHis;
    MonitorHisNo616 = nHis - 2 * nUnit;
    LastCrossHistogram = -1;
    nBase = 0;
    for (iFor = 0; iFor <= AbsoluteLastCrossHistogram; iFor++)
    {
        TempStr = HisParCRS;
        MakeCSVString(TempStr);
        //for (jFor = 0; jFor <= CrossLimLast; jFor++)
        for (jFor = 0; jFor < CrossLimLast; jFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //CrossLimit[jFor, iFor, 0] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            CrossLimit[jFor][nHis][0] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        //CrossHisDumpInterval[iFor, 0] = MonDumpDefault;
        CrossHisDumpInterval[iFor][0] = MonDumpDefault;
        TempStr = HisParCRL;
        MakeCSVString(TempStr);
        //for (jFor = 0; jFor <= CrossLimLast; jFor++)
        for (jFor = 0; jFor < CrossLimLast; jFor++)
        {
            //nComma = Strings.InStr(TempStr, ",");
            //CrossLimit[jFor, iFor, 1] = strtof(Left(TempStr, nComma - 1));
            nComma = TempStr.find(",");
            CrossLimit[jFor][nHis][1] = strtof(TempStr.substr(0, nComma - 1).c_str(), nullptr);
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        //CrossHisDumpInterval[iFor, 1] = MonDumpDefault;
        CrossHisDumpInterval[iFor][1] = MonDumpDefault;
    }
    // Now set up the column widths for the display grids
    TempStr = "1000,800,800,800,800,800,800,800,800";
    MakeCSVString(TempStr);
    //for (iFor = 0; iFor <= MonitorLimColLast; iFor++)
    for (iFor = 0; iFor < MonitorLimColLast; iFor++)
    {
        //nComma = Strings.InStr(TempStr, ",");
        //MonitorLimColWidth[iFor] = strtol(Left(TempStr, nComma - 1));
        nComma = TempStr.find(",");
        MonitorLimColWidth[iFor] = strtol(TempStr.substr(0, nComma - 1).c_str(), nullptr, 10);
        TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
    }

    TempStr = "250,800,1000,800,800,800,800,800,800,800,800";
    MakeCSVString(TempStr);
    //for (iFor = 0; iFor <= MonitorDaColLast; iFor++)
    for (iFor = 0; iFor < MonitorDaColLast; iFor++)
    {
        //nComma = Strings.InStr(TempStr, ",");
        //MonitorDaColWidth[iFor] = strtol(Left(TempStr, nComma - 1));
        nComma = TempStr.find(",");
        MonitorDaColWidth[iFor] = strtol(TempStr.substr(0, nComma - 1).c_str(), nullptr, 10);
        TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
    }

    TempStr = "1000,800,800,800,800,800,800,800,800";
    MakeCSVString(TempStr);
    //for (iFor = 0; iFor <= CrossLimColLast; iFor++)
    for (iFor = 0; iFor < CrossLimColLast; iFor++)
    {
        //nComma = Strings.InStr(TempStr, ",");
        //CrossLimColWidth[iFor] = strtol(Left(TempStr, nComma - 1));
        nComma = TempStr.find(",");
        CrossLimColWidth[iFor] = strtol(TempStr.substr(0, nComma - 1).c_str(), nullptr, 10);
        TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
    }

    //FillStringArray(MonitorDaColLabel(), "T,Item,Mean,Sigma,Skew,Total,Below,In,Above,LimL,LimH,");
    FillStringArray(MonitorDaColLabel, "T,Item,Mean,Sigma,Skew,Total,Below,In,Above,LimL,LimH,");
    // Read the cross (histogram) matrix
    // This is a (LastUnit+1) by (LastUnit+1) matrix
    // assigning a cross histogram to each ordered unit pair
    ResetTiming(); //TODO rest of Monitor_Form_Load()

    
    //timeThread = new Timer1Thread();
    //timeThread->Run();
    
    comThread[0] = new RackThread();
    comThread[0]->Run();
    
    return true; //TODO rest of Monitor_Form_Load()
    }
    /*
    if (CrossMultiplicityEnabled)
    {
        Close(); if (fso.FileExists(CatchFileName))
            Open(CatchFileName);
        else
        {
            Open(CatchFileName);
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #98, "From: LandBase"

 */
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #98, "To: Cosray"

 */
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #98, "Subject: " & CatchFileName

 */
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #98,
    If StationNoteExists Then Print #98, "Station Note - " & StationNote

 */
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #98, "Form Load: " + lblComputerTime.Caption

 */
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #98, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
/* TODO rest of Monitor_Form_Load()
* 
        }
        Open(CrossMatrixFileName); for (iFor = 0; iFor <= LastUnit; iFor++)
        {
            Line(Input, CrossMatrixString);
            MakeCSVString(CrossMatrixString);
            for (jFor = 0; jFor <= LastUnit; jFor++)
            {
                nComma = Strings.InStr(CrossMatrixString, ",");
                nCrossHis = strtol(Left(CrossMatrixString, nComma - 1));
                // Make sure this is not too big
                // Zero is completely legal
                // Negative is the code for ignore this pair
                if (nCrossHis > AbsoluteLastCrossHistogram)
                    nCrossHis = AbsoluteLastCrossHistogram;
                CrossMatrix[iFor, jFor] = nCrossHis;
                if (nCrossHis >= 0)
                {
                    if (CrossHistogramActive(nCrossHis))
                    {
                    }
                    else
                        CrossHistogramActive[nCrossHis] = true;
                }
                CrossMatrixString = CrossMatrixString.substr( nComma + 1, std::string::npos); //replacement for Mid -B
            }
        }
        Close();
        ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  Print #2, "Cross Histogram Matrix from " & CrossMatrixFileName & ":"

 */
/* TODO rest of Monitor_Form_Load()
* 
        LastGroupSizePointer = 0;
        ScratchString = CatchDecimateInit;
        StandardizeString(ScratchString);
        for (iFor = 0; iFor <= maxCatchDecimate; iFor++)
        {
            CatchDecimateMod[iFor] = ChopInt(ScratchString);
            CatchAvailableCounter[iFor] = -1;
            CatchDecimatedCounter[iFor] = 0;
        }
        for (iFor = 0; iFor <= AbsoluteLastUnit; iFor++)
        {
            for (jFor = 0; jFor <= MaxEventClass; jFor++)
                GroupSizeDistribution[iFor, jFor] = 0;
        }
        for (iFor = 0; iFor <= LastUnit; iFor++)
        {
            CrossMatrixString = "";
            for (jFor = 0; jFor <= LastUnit; jFor++)
                CrossMatrixString = CrossMatrixString + Right("    " + Format(CrossMatrix(iFor, jFor)), 4);
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2, CrossMatrixString

 */
/* TODO rest of Monitor_Form_Load()
* 
        }
        LastCrossHistogram = -1;
        for (iFor = 0; iFor <= AbsoluteLastCrossHistogram; iFor++)
        {
            if (CrossHistogramActive(iFor))
            {
                LastCrossHistogram = LastCrossHistogram + 1;
                WhichCrossHistogram[LastCrossHistogram] = iFor;
            }
        }
    }
    // Form Initializations
    if (TiltMetersInstalled)
    {
        frmTiltmeter.Visible = true;
        frmMonitor.Width = frmStatus.Width + frmSSEBarometer.Width + frmTiltmeter.Width;
    }
    else
    {
        frmTiltmeter.Visible = false;
        frmMonitor.Width = frmStatus.Width + frmSSEBarometer.Width;
    }

    frmTransmitData.lblParameter(IdxMailSSE).Caption = Format(0);
    if (SequenceTimeSystem == true)
        lblSequenceTimeSource.Caption = "System";
    else
        lblSequenceTimeSource.Caption = "GPS";
    lblVerboseDiagnostics.BackColor = ColorGreen;
    VerboseDiagnostics = false;
    lblVerboseDiagnostics.Caption = "Normal";
    for (iFor = 0; iFor <= lDataRow; iFor++)
    {
        if (Active(iFor) == false)
        {
            frmTransmitData.lblActive(iFor).Caption = "No";
            frmTransmitData.lblActive(iFor).BackColor = ColorRed;
        }
        else
        {
            frmTransmitData.lblActive(iFor).Caption = "Yes";
            frmTransmitData.lblActive(iFor).BackColor = ColorGreen;
        }
        frmTransmitData.lblExTime(iFor).Caption = Format(ExTime(iFor));
    }

    if (GPSTimeSyncEnable == false)
        lblGPSTimeSyncEnable.Caption = "Inherent";
    else
        lblGPSTimeSyncEnable.Caption = "GPS";
    if (PortStallEnable == false)
    {
        lblPortStallEnable.Caption = "Port Stall Cycle Disabled";
        lblPortStallEnable.BackColor = ColorRed;
    }
    else
    {
        lblPortStallEnable.Caption = "Port Stall Cycle Enabled";
        lblPortStallEnable.BackColor = ColorGreen;
    }

    if (AutoRebootEnable == false)
    {
        lblAutoRebootEnable.Caption = "Auto Reboot Disabled";
        lblAutoRebootEnable.BackColor = ColorRed;
    }
    else
    {
        lblAutoRebootEnable.Caption = "Auto Reboot Enabled";
        lblAutoRebootEnable.BackColor = ColorGreen;
    }

    if (!EnableRackCommands)
    {
        btnCmdTest.Visible = false;
        btnCmdReset.Visible = false;
    }

    lblPSIRead.Caption = "0.00";
    vPos = lblCts[0].Top;
    vIncr = lblCts[0].Height + 50;
    frmTransmitData.flbSSE.Path = QueueDirectory;
    for (iFor = 1; iFor <= LastRemote[0]; iFor++)
    {
        vPos = vPos + vIncr;
        Load(lblIsBare(iFor));
        lblIsBare(iFor).Top = vPos;
        lblIsBare(iFor).Visible = true;
        Load(lblCts(iFor));
        lblCts(iFor).Top = vPos;
        lblCts(iFor).Visible = true;
        Load(lblDel(iFor));
        lblDel(iFor).Top = vPos;
        lblDel(iFor).Visible = true;
        Load(lblWindowVerify(iFor));
        lblWindowVerify(iFor).Top = vPos;
        lblWindowVerify(iFor).Visible = true;
        Load(lblWindowSecondVerify(iFor));
        lblWindowSecondVerify(iFor).Top = vPos;
        lblWindowSecondVerify(iFor).Visible = true;
        Load(lblHourDel(iFor));
        lblHourDel(iFor).Top = vPos;
        lblHourDel(iFor).Visible = true;
        Load(lblT1(iFor));
        lblT1(iFor).Top = vPos;
        lblT1(iFor).Visible = true;
        Load(lblT2(iFor));
        lblT2(iFor).Top = vPos;
        lblT2(iFor).Visible = true;
        Load(lblT3(iFor));
        lblT3(iFor).Top = vPos;
        lblT3(iFor).Visible = true;
    }

    for (jFor = 1; jFor <= LastMaster; jFor++)
    {
        if (LastRemote(jFor) >= 0)
        {
            vPos = vPos + 100;
            for (iFor = RemoteBaseAddress(jFor); iFor <= RemoteBaseAddress(jFor) + LastRemote(jFor); iFor++)
            {
                vPos = vPos + vIncr;
                Load(lblIsBare(iFor));
                lblIsBare(iFor).Top = vPos;
                lblIsBare(iFor).Visible = true;
                Load(lblCts(iFor));
                lblCts(iFor).Top = vPos;
                lblCts(iFor).Visible = true;
                Load(lblDel(iFor));
                lblDel(iFor).Top = vPos;
                lblDel(iFor).Visible = true;
                Load(lblWindowVerify(iFor));
                lblWindowVerify(iFor).Top = vPos;
                lblWindowVerify(iFor).Visible = true;
                Load(lblWindowSecondVerify(iFor));
                lblWindowSecondVerify(iFor).Top = vPos;
                lblWindowSecondVerify(iFor).Visible = true;
                Load(lblHourDel(iFor));
                lblHourDel(iFor).Top = vPos;
                lblHourDel(iFor).Visible = true;
                Load(lblT1(iFor));
                lblT1(iFor).Top = vPos;
                lblT1(iFor).Visible = true;
                Load(lblT2(iFor));
                lblT2(iFor).Top = vPos;
                lblT2(iFor).Visible = true;
                Load(lblT3(iFor));
                lblT3(iFor).Top = vPos;
                lblT3(iFor).Visible = true;
            }
        }
    }

    for (I = 0; I <= LastUnit; I++)
        lblCts(I).Caption = "000";
    SSEFileName = "IgnoreThisFile";
    if (RunSecondCopy)
        StationTitle = StationTitle + " Second Copy";
    else
        StationTitle = StationTitle + " First Copy";
    frmMonitor.Caption = StationTitle;
    CR = Chr(13);
    LF = Chr(10);
    CRLF = Chr(13) + Chr(10);
    frmNewElectronics.Visible = true;
    frmNewElectronics.WindowState = 1;
    frmHouse.Visible = true;
    frmHouse.WindowState = 1;
    frmHouse.lblResetCount.Caption = Format(0);
    // Constant Initializations
    DelCycle = Math.Pow(2, 24);
    // Deg C 0.38100 0259.0
    ItemTag[0] = "Deg C";
    CalConst[0, 0] = 0.381;
    CalConst[1, 0] = 259; //manual # removal -B
    // +5 V 0.00804 0000.0
    ItemTag[1] = " +5 V";
    CalConst[0, 1] = 0.00804;
    CalConst[1, 1] = 0; //manual # removal -B
    // +15 V 0.01998 0000.0
    ItemTag[2] = "+15 V";
    CalConst[0, 2] = 0.01998;
    CalConst[1, 2] = 0; //manual # removal -B
    // -5 V -0.0083 0000.0
    ItemTag[3] = " -5 V";
    CalConst[0, 3] = -0.0083;
    CalConst[1, 3] = 0; //manual # removal -B
    // -15 V -0.0156 0000.0
    ItemTag[4] = "-15 V";
    CalConst[0, 4] = -0.0156;
    CalConst[1, 4] = 0; //manual # removal -B
    for (iFor = 0; iFor <= LastUnit; iFor++)
    {
        // Display the Unit Tag
        frmHouse.lblTag(iFor).Caption = UnitTag(iFor);
        // Display the firmware version
        frmHouse.lblFirmware(iFor).Caption = Format(FirmVersion(iFor));
    }

    for (iFor = 0; iFor <= 2; iFor++)
        frmHouse.lblMuxItem(iFor).Caption = ItemTag(iFor);
    FullRdgAIR[0] = "0000";
    FullRdgAIR[1] = "0000";
    LogEntry("Station ID " + StationID);
    LogEntry("Observatory Pressure " + Format(MeanObservatoryPressure));
    LogEntry("Queue Directory " + QueueDirectory);
    // Beginning in 2003 the barometer parameters are loaded fromQueueDirectory
    // instructions in the WhatAmI file
    // Slots 0 and 1 are used for barometers in the rack
    TSF[0] = 1;
    PSF[0] = 1;
    TSF[1] = 1;
    PSF[1] = 1;
    for (nSlot = 0; nSlot <= mBar; nSlot++)
    {
        lblDigiSerial(nSlot).Visible = false;
        lblDigiModel(nSlot).Visible = false;
        lblDigiT(nSlot).Visible = false;
        lblDigiT(nSlot).Caption = "0.00";
        lblDigiP(nSlot).Visible = false;
        lblDigiP(nSlot).Caption = "0.00";
        lblFreqA(nSlot).Visible = false;
        lblFreqAsd(nSlot).Visible = false;
        lblFreqB(nSlot).Visible = false;
        lblFreqBsd(nSlot).Visible = false;
        lblnDQRd(nSlot).Caption = "0";
        lblnDQRd(nSlot).Visible = false;
        if (BarLoaded(nSlot))
        {
            lblDigiSerial(nSlot).Visible = true;
            lblDigiModel(nSlot).Visible = true;
            lblDigiT(nSlot).Visible = true;
            lblDigiP(nSlot).Visible = true;
            lblFreqA(nSlot).Visible = true;
            lblFreqB(nSlot).Visible = true;
            if (nSlot < 2)
            {
                lblFreqAsd(nSlot).Visible = true;
                lblFreqBsd(nSlot).Visible = true;
                lblnDQRd(nSlot).Visible = true;
            }
            WorkFile = PersonalityDirectory + BarometerFile(nSlot);
            Open(WorkFile); LogEntry("Barometer" + Format(nSlot) + " Load from " + WorkFile);
            while (!FileSystem.EOF(1))
            {
                Line(Input, InputStr);
                LogEntry(InputStr);
                nKey = Strings.InStr(InputStr, ":");
                KeyString = " ";
                if (nKey > 0)
                    KeyString = Left(InputStr, nKey - 1);
                //switch (KeyString)
                //{
                if( 0 == KeyString.compare("Se")) //case replacement for string compare -B
                {
                    BarSerial[nSlot] = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
                    lblDigiSerial(nSlot).Caption = BarSerial(nSlot);
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("Mo")) //case replacement for string compare -B
                {
                    BarModel[nSlot] = InputStr.substr( nKey + 2, std::string::npos); //replacement for Mid -B
                    lblDigiModel(nSlot).Caption = BarModel(nSlot);
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("X0")) //case replacement for string compare -B
                {
                    X0[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("Y1")) //case replacement for string compare -B
                {
                    Y1[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("Y2")) //case replacement for string compare -B
                {
                    Y2[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("Y3")) //case replacement for string compare -B
                {
                    Y3[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("C1")) //case replacement for string compare -B
                {
                    C1[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("C2")) //case replacement for string compare -B
                {
                    C2[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("C3")) //case replacement for string compare -B
                {
                    C3[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("D1")) //case replacement for string compare -B
                {
                    D1[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("D2")) //case replacement for string compare -B
                {
                    D2[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("T1")) //case replacement for string compare -B
                {
                    T1[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("T2")) //case replacement for string compare -B
                {
                    T2[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("T3")) //case replacement for string compare -B
                {
                    T3[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("T4")) //case replacement for string compare -B
                {
                    T4[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else if( 0 == KeyString.compare("T5")) //case replacement for string compare -B
                {
                    T5[nSlot] = strtod(InputStr.substr( nKey + 2), std::string::npos); //replacement for Mid -B
                    //break; //case replacement for string compare -B
                }

                else //case replacement for string compare -B
                {
                    LogEntry("Invalid Key During Barometer Load");
                    //break; //case replacement for string compare -B
                }
                }
            }
            Close();
        }
    }

    ReportString = "Bare Unit Map ";
    for (iFor = 0; iFor <= LastUnit; iFor++)
    {
        lblIsBare(iFor).BackColor = ColorGreen;
        if (ValidCrossUnit(iFor))
            lblIsBare(iFor).BackColor = ColorPink;
        if (UnitIsBad(iFor))
            lblIsBare(iFor).BackColor = ColorYellow;
        if ((BareUnitMap(iFor)))
        {
            ReportString = ReportString + "T";
            lblIsBare(iFor).Caption = "B";
        }
        else
        {
            ReportString = ReportString + "F";
            lblIsBare(iFor).Caption = " ";
        }
    }

    LogEntry(ReportString);
    ListHistogramInput = false;
    frmDisplay.lblListInput.Caption = "No List";
    frmDisplay.lblListInput.BackColor = ColorGreen;
    vsbMemStick.Value = InStr(DriveChoice, MemStickDrive) - 1;
    MinutesSinceNewlyLoaded = 0;
    frmDisplay.Visible = true;
    frmDisplay.WindowState = 1;
    frmREMOTE.Visible = true;
    frmREMOTE.WindowState = 1;
    frmMultiplicity.Visible = true;
    frmMultiplicity.WindowState = 1;
    if (!RecordFullSyncNotes)
    {
        lblSyncNotes.Caption = "Compact";
        lblSyncNotes.BackColor = ColorGreen;
    }
    else
    {
        lblSyncNotes.Caption = "Extended";
        lblSyncNotes.BackColor = ColorRed;
    }
    // Cross multiplicity setup or hide
    FakeData = false;
    lblFakeData.Caption = "No Fake";
    lblFakeData.BackColor = ColorGreen;
    FakeSecond = 0;
    if (CrossMultiplicityEnabled)
    {
        frmClocker.Visible = true;
        frmClocker.WindowState = 1;
        GenerateTimeStamp(CatchString);
        ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  Print #98, CatchString & " frmMonitor.load Resetting Timing"

 */
/* TODO rest of Monitor_Form_Load()
* 
        ResetTiming();
    }
    else
    {
        lblFakeData.Visible = false;
        cmdMarker.Visible = false;
    }
    // If (nValidCrossUnits > 0) And (nValidCrossUnits <= CInt(frmClocker.vsbCatchAt.Max)) Then
    // frmClocker.vsbCatchAt.Value = nValidCrossUnits
    // frmClocker.vsbMinBig.Value = nValidCrossUnits * 3
    // End If
    ScratchString = BigCatchInit;
    StandardizeString(ScratchString);
    frmClocker.lblCatchSize(0).Caption = ChopInt(ScratchString);
    frmClocker.lblCatchSize(1).Caption = ChopInt(ScratchString);
    frmClocker.vsbCatchResolution.Value = ChopInt(ScratchString);
    frmClocker.hsbZapRes.Value = ZapResIn;
    frmClocker.lblZapRes.Caption = Format(ZapResIn);
    ZapResolve = 0.000001 * strtod(frmClocker.lblZapRes.Caption);
    BigCatchPastLimit = ChopSng(ScratchString);
    BigCatchFutureLimit = ChopSng(ScratchString);
    BigCatchDisplayAt = ChopInt(ScratchString);
    if (BigCatchDisplayAt == 0)
        BigCatchDisplayAt = 10;
    frmClocker.lblCatchSize(2).Caption = Format(BigCatchDisplayAt);
    CatchResolution = 0.000001 * strtod(frmClocker.lblCatchResolution.Caption);
    BypShowOnly = false;
    frmClocker.lblShowBypass.BackColor = ColorGreen;
    frmClocker.lblShowBypass.Caption = "Show All";
    if (SaveSecondRing)
    {
        frmClocker.lblOneSecondStatus.Caption = "Yes";
        frmClocker.lblOneSecondStatus.BackColor = ColorYellow;
    }
    else
    {
        frmClocker.lblOneSecondStatus.Caption = "No";
        frmClocker.lblOneSecondStatus.BackColor = ColorGreen;
    }
    // Initialize the BigCatch decimation bypass
    ScratchString = BypassInit;
    StandardizeString(ScratchString);
    BypU1 = ChopSng(ScratchString);
    BypU2 = ChopSng(ScratchString);
    if (BypU2 > LastUnit)
        BypU2 = LastUnit;
    BypInterval = 0.000001 * ChopDbl(ScratchString);
    BypActive = Strings.InStr(ScratchString, "T") > 0;
    if (BypActive)
    {
        frmClocker.lblBypassActive.Caption = "Bypass Active";
        frmClocker.lblBypassActive.BackColor = ColorGreen;
    }
    else
    {
        frmClocker.lblBypassActive.Caption = "No Bypass";
        frmClocker.lblBypassActive.BackColor = ColorYellow;
    }
    // The Click function changes the state then operates in this case
    frmTransmitData.lblLastSpoolUnit.Caption = Format(LastSpoolUnit);
    RecordHitSpool = !RecordHitSpool;
    frmTransmitData.lblRecordHitSpool_Click();
    // Set up raw data recording as Off
    RecordRaw = false;
    RecordRawCount = 0;
    lblRecordRaw.BackColor = ColorGreen;
    lblRecordRaw.Caption = "Off";
    txtRecordRawDuration.Text = "70";
    if (BareTrapEnabled)
    {
        // Global BareTrapTime As Double
        // Global nBareTrapUnits As Integer
        // Global BareTrapUnits(2) As Integer
        // Global BareTrapActive As Boolean
        BareTrapTime = ChopDbl(BareTrapInit);
        nBareTrapUnits = ChopSng(BareTrapInit);
        BareTrapUnits[0] = ChopSng(BareTrapInit);
        BareTrapUnits[1] = ChopSng(BareTrapInit);
        BareTrapUnits[2] = ChopSng(BareTrapInit);
        BareTrapActive = Strings.InStr(BareTrapInit, "T") > 0;
        frmClocker.lblBareTrapTime.Caption = Format(BareTrapTime);
        for (iFor = 0; iFor <= 2; iFor++)
        {
            if (nBareTrapUnits >= iFor + 1)
            {
                frmClocker.lblBareTrapUnit(iFor).Caption = Format(BareTrapUnits(iFor));
                frmClocker.lblBareTrapUnit(iFor).Visible = true;
            }
            else
                frmClocker.lblBareTrapUnit(iFor).Visible = false;
        }
        if (BareTrapActive)
        {
            frmClocker.lblBareTrapActive.BackColor = ColorGreen;
            frmClocker.lblBareTrapActive.Caption = "BareTrap Active";
        }
        else
        {
            frmClocker.lblBareTrapActive.BackColor = ColorRed;
            frmClocker.lblBareTrapActive.Caption = "BareTrap Off";
        }
    }
    else
    {
        frmClocker.lblBareTrapTime.Visible = false;
        for (iFor = 0; iFor <= 2; iFor++)
            frmClocker.lblBareTrapUnit(iFor).Visible = false;
        frmClocker.lblBareTrapActive.Visible = false;
    }

    frmREMOTE.lblLagTrip(0).Caption = Format(HitBufferTrip, "0.0");
    frmREMOTE.lblLagTrip(1).Caption = Format(HitBufferWarn, "0.0");
    if (NumberOfBares == 0)
        LastContiguousUnit = LastUnit;
	
    DocumentState();


    return true;
}
    /* TODO rest of Monitor_Form_Load()
*/

//public static void LogEntry(string Message)
void LandMonitorApp::LogEntry(std::string Message)
{
    if (MinuteFileAvailable)
    {
        std::time(&CurTime); //update time -B
        char fCurTime[20]; //for formatting timestamp -B
        strftime(fCurTime, std::size(fCurTime), "%y/%m/%d %T", std::localtime(&CurTime));
        //File.AppendAllText("PathToYourFile.txt", "Log: " + Message + " " + CurTime.ToString("yy/mm/dd hh:mm:ss") + Environment.NewLine);
        {
            wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
            MinuteFile << "Log: " << Message << ' ' << fCurTime << '\n' << std::flush;
        }
        //}
        //On Error GoTo NoStick;
        //if (UseMemStick) //TODO mem stick handling
        //{
        //    File.AppendAllText("PathToYourFile.txt", "Log: " + Message + " " + CurTime.ToString("yy/mm/dd hh:mm:ss") + Environment.NewLine);
        //}
    }
//NoStick:

    return;
}


void LandMonitorApp::PrepareFolder(std::string FolderName)
{
    // To use this, the "Project" > "References" menu must be used
    // to select "Microsoft Scripting Runtime."
    //if (!fso.FolderExists(FolderName))
    //{
    //    fso.CreateFolder(FolderName);
    //}
    if (0 != stat(FolderName.c_str(), &fso))
    {
        _mkdir(FolderName.c_str());
    }
}

void LandMonitorApp::StandardizeString(std::string& OpString) {
    size_t nChar;
    std::string CarRet = "\n";
    std::string TabChar = "\t";

    // Replace line breaks with spaces
    while ((nChar = OpString.find(CarRet)) != std::string::npos) {
        if (nChar == 0) {
            OpString = OpString.substr(1);
        }
        else if (nChar == OpString.length() - 1) {
            OpString = OpString.substr(0, nChar);
        }
        else {
            OpString = OpString.substr(0, nChar) + OpString.substr(nChar + 1);
        }
    }

    // Replace commas with spaces
    while ((nChar = OpString.find(",")) != std::string::npos) {
        if (nChar == 0) {
            OpString = " " + OpString.substr(1);
        }
        else if (nChar == OpString.length() - 1) {
            OpString = OpString.substr(0, nChar) + " ";
        }
        else {
            OpString = OpString.substr(0, nChar) + " " + OpString.substr(nChar + 1);
        }
    }

    // Replace tabs with spaces
    while ((nChar = OpString.find(TabChar)) != std::string::npos) {
        if (nChar == 0) {
            OpString = OpString.substr(1);
        }
        else if (nChar == OpString.length() - 1) {
            OpString = OpString.substr(0, nChar);
        }
        else {
            OpString = OpString.substr(0, nChar) + " " + OpString.substr(nChar + 1);
        }
    }

    // Replace multiple spaces with single spaces
    while ((nChar = OpString.find("  ")) != std::string::npos) {
        if (nChar == 0) {
            OpString = OpString.substr(2);
        }
        else if (nChar == OpString.length() - 2) {
            OpString = OpString.substr(0, nChar);
        }
        else {
            OpString = OpString.substr(0, nChar) + OpString.substr(nChar + 2);
        }
    }

    // Remove leading spaces
    while (OpString.substr(0, 1) == " ") {
        OpString = OpString.substr(1);
    }
}

void LandMonitorApp::SetUpMaster(int MasterNum, std::string SetString) {
    std::string ParsedString[20];
    int nUnit;
    //int LastRemote = std::stoi(SetString.substr(0, SetString.find(" ")));
    LastRemote[MasterNum] = std::stoi(SetString.substr(0, SetString.find(" ")));
    //if (LastRemote >= 0) {
    if (LastRemote[MasterNum] >= 0) {
        //RemoteBaseAddress = LastUnit + 1;
        RemoteBaseAddress[MasterNum] = LastUnit + 1;
        //LastUnit += LastRemote + 1;
        LastUnit += LastRemote[MasterNum] + 1;
        //for (int iFor = 0; iFor <= LastRemote; iFor++) {
        for (int iFor = 0; iFor <= LastRemote[MasterNum]; iFor++) {
            //nUnit = RemoteBaseAddress + iFor;
            nUnit = RemoteBaseAddress[MasterNum] + iFor;
            //FirmVersion[nUnit] = std::stoi(SetString.substr(iFor * 3 + 1, 2));
            std:string tmpstr = SetString.substr(2 + iFor * 4, 3);
            FirmVersion[nUnit] = std::stoi(tmpstr);
            UnitTag[nUnit] = std::to_string(MasterNum) + (iFor < 10 ? "0" : "") + std::to_string(iFor);
        }
    }
}

float LandMonitorApp::ChopSng(std::string InString) {
    float result = 0;
    size_t lBlank;
    while (InString.length() > 0) {
        if (InString.substr(0, 1) == " ") {
            InString = InString.substr(1);
        }
        else if (InString.find(" ") == std::string::npos) {
            result = std::stof(InString);
            InString = "";
        }
        else {
            lBlank = InString.find(" ");
            result = std::stof(InString.substr(0, lBlank));
            InString = InString.substr(lBlank + 1);
            break;
        }
    }
    return result;
}

//void LandMonitorApp::MakeCSVString(std::string& OpString) {
//    int nChar;
//    OpString = OpString + " ";
//    while (OpString.find(CarRet) != std::string::npos) {
//        nChar = OpString.find(CarRet);
//        if (nChar == 0) {
//            OpString = OpString.substr(1);
//        }
//        else if (nChar == OpString.length() - 1) {
//            OpString = OpString.substr(0, nChar);
//        }
//        else {
//            OpString = OpString.substr(0, nChar) + OpString.substr(nChar + 1);
//        }
//    }
//    while (OpString.find(",") != std::string::npos) {
//        nChar = OpString.find(",");
//        if (nChar == 0) {
//            OpString = " " + OpString.substr(1);
//        }
//        else if (nChar == OpString.length() - 1) {
//            OpString = OpString.substr(0, nChar - 1) + " ";
//        }
//        else {
//            OpString = OpString.substr(0, nChar - 1) + " " + OpString.substr(nChar + 1);
//        }
//    }
//    while (OpString.find(TabChar) != std::string::npos) {
//        nChar = OpString.find(TabChar);
//        if (nChar == 0) {
//            OpString = OpString.substr(1);
//        }
//        else if (nChar == OpString.length() - 1) {
//            OpString = OpString.substr(0, nChar);
//        }
//        else {
//            OpString = OpString.substr(0, nChar - 1) + " " + OpString.substr(nChar + 1);
//        }
//    }
//    while (OpString.find("  ") != std::string::npos) {
//        nChar = OpString.find("  ");
//        if (nChar == 0) {
//            OpString = OpString.substr(2);
//        }
//        else if (nChar == OpString.length() - 2) {
//            OpString = OpString.substr(0, nChar);
//        }
//        else {
//            OpString = OpString.substr(0, nChar) + OpString.substr(nChar + 2);
//        }
//    }
//    while (OpString.substr(0, 1) == " ") {
//        OpString = OpString.substr(1);
//    }
//    while (OpString.find(" ") != std::string::npos) {
//        nChar = OpString.find(" ");
//        if (nChar == 0) {
//            OpString = "," + OpString.substr(1);
//        }
//        else if (nChar == OpString.length() - 1) {
//            OpString = OpString.substr(0, nChar - 1) + ",";
//        }
//        else {
//            OpString = OpString.substr(0, nChar - 1) + "," + OpString.substr(nChar + 1);
//        }
//    }
//}

void LandMonitorApp::MakeCSVString(std::string& OpString) {
    size_t nChar = OpString.find(CarRet);
    while (nChar != std::string::npos) { //erase all CR
        OpString.erase(nChar, 1);
        nChar = OpString.find(CarRet);
    }
    nChar = OpString.find(' ');
    while (nChar != std::string::npos) { //replace all ' ' with ,
        OpString[nChar] = ',';
        nChar = OpString.find(' ');
    }
    nChar = OpString.find(TabChar);
    while (nChar != std::string::npos) { //replace all tabs with ,
        OpString[nChar] = ',';
        nChar = OpString.find(TabChar);
    }
    nChar = OpString.find(",,");
    while (nChar != std::string::npos) { //erase all duplicate ,,
        OpString.erase(nChar, 1);
        nChar = OpString.find(",,");
    }
    //while (',' == OpString[0]) { //erase leading ,
    //    OpString.erase(0, 1);
    //}
    if (',' != OpString.back()) { //force end ,
        OpString.push_back(',');
    }
}

void LandMonitorApp::FillIntegerArray(int IntegerArray[], int nLast, std::string InputDataString) {
    int I, nCom;
    std::string Partial, DataString;
    DataString = InputDataString;
    for (I = 0; I <= nLast; I++) {
        nCom = DataString.find(",");
        if (nCom != std::string::npos) {
            Partial = DataString.substr(0, nCom);
            IntegerArray[I] = std::stoi(Partial);
            DataString = DataString.substr(nCom + 1);
        }
        else {
            IntegerArray[I] = 0;
        }
    }
}

void LandMonitorApp::FillLongArray(long LongArray[], int nLast, std::string InputDataString) {
    int I, nCom;
    std::string Partial, DataString;
    DataString = InputDataString;
    for (I = 0; I <= nLast; I++) {
        nCom = DataString.find(",");
        if (nCom != std::string::npos) {
            Partial = DataString.substr(0, nCom);
            LongArray[I] = std::stol(Partial);
            DataString = DataString.substr(nCom + 1);
        }
        else {
            LongArray[I] = 0;
        }
    }
}

void LandMonitorApp::FillSingleArray(float SingleArray[], int nLast, std::string InputDataString) {
    int I, nCom;
    std::string Partial, DataString;
    DataString = InputDataString;
    for (I = 0; I <= nLast; I++) {
        nCom = DataString.find(",");
        if (nCom != std::string::npos) {
            Partial = DataString.substr(0, nCom);
            SingleArray[I] = std::stof(Partial);
            DataString = DataString.substr(nCom + 1);
        }
        else {
            SingleArray[I] = 0.0f;
        }
    }
}

void LandMonitorApp::EnterMonitorHis(float Value, int NumHis, int TypHis) {
    int Item;
    int RingLoc;
    float Scaled = (Value - MonitorLimit[HBase][NumHis]) / MonitorLimit[HScl][NumHis];
    if (Scaled > MonitorHisLastChan) {
    //if (Scaled > (MonitorHisLastChan - 1)) {
        Item = MonitorHisLastChan;
        //Item = MonitorHisLastChan - 1;
        //MonitorHis[Item][NumHis][TypHis] += 1;
        MonitorHis[Item][NumHis][TypHis]++;
    }
    else if (Scaled < 0) {
        MonitorHisErr[NumHis][TypHis] += 1;
    }
    else {
        Item = static_cast<int>(Scaled);
        MonitorHis[Item][NumHis][TypHis] += 1;
    }
    // Also keep track of the history in a ring buffer
    RingLoc = (MonitorRingLoc[NumHis][TypHis] + 1) % (MonitorRingLast + 1);
    MonitorRing[RingLoc][NumHis][TypHis] = Value;
    MonitorRingLoc[NumHis][TypHis] = RingLoc;
}

void LandMonitorApp::EraseMonitorHis(float Value, int NumHis, int TypHis) {
    // Same structure as EnterMonitorHis except it removes an earlier
    // entry by decrementing the histogram. There is no actual memory of
    // the entry, so exactly the same "Value" must be submitted so that
    // after scaling the proper channel is decremented. No bin in the histogram
    // will be decremented below zero. The ring buffer for the "series"
    // display is not affected.
    int Item;
    int RingLoc;
    float Scaled = (Value - MonitorLimit[HBase][NumHis]) / MonitorLimit[HScl][NumHis];
    if (Scaled > MonitorHisLastChan) {
    //if (Scaled > (MonitorHisLastChan - 1)) {
        Item = MonitorHisLastChan;
        //Item = MonitorHisLastChan - 1;
        if (MonitorHis[Item][NumHis][TypHis] > 0) {
            MonitorHis[Item][NumHis][TypHis] -= 1;
        }
    }
    else if (Scaled < 0) {
        if (MonitorHisErr[NumHis][TypHis] > 0) {
            MonitorHisErr[NumHis][TypHis] -= 1;
        }
    }
    else {
        Item = static_cast<int>(Scaled);
        if (MonitorHis[Item][NumHis][TypHis] > 0) {
            MonitorHis[Item][NumHis][TypHis] -= 1;
        }
    }
}

void LandMonitorApp::FillStringArray(std::string StringArray[], std::string InputDataString) {
    int iFor, nLast, nCom, nBlank;
    std::string Partial, DataString;
    nLast = sizeof(StringArray) / sizeof(StringArray[0]) - 1;
    DataString = InputDataString + ",";
    for (iFor = 0; iFor <= nLast; iFor++) {
        StringArray[iFor] = " ";
    }
    while (DataString.find("  ") != std::string::npos) {
        nBlank = DataString.find("  ");
        DataString = DataString.substr(0, nBlank) + DataString.substr(nBlank + 1);
    }
    while (DataString.substr(0, 1) == " ") {
        DataString = DataString.substr(1);
    }
    while (DataString.find(", ") != std::string::npos) {
        nBlank = DataString.find(", ");
        DataString = DataString.substr(0, nBlank) + DataString.substr(nBlank + 2);
    }
    while (DataString.find(" ,") != std::string::npos) {
        nBlank = DataString.find(" ,");
        DataString = DataString.substr(0, nBlank - 1) + DataString.substr(nBlank + 1);
    }
    while (DataString.find(" ") != std::string::npos) {
        nBlank = DataString.find(" ");
        DataString = DataString.substr(0, nBlank - 1) + "," + DataString.substr(nBlank + 1);
    }
    for (iFor = 0; iFor <= nLast; iFor++) {
        nCom = DataString.find(",");
        if (nCom != std::string::npos) {
            Partial = DataString.substr(0, nCom);
            StringArray[iFor] = Partial;
            DataString = DataString.substr(nCom + 1);
        }
    }
}

void LandMonitorApp::Exception(std::string Message)
{
    //frmMonitor.lblException[0].Caption = CurDayTimTag; //TODO connect to wx with event    
 // frmMonitor.lblException[1].Caption = Message;  //TODO connect to wx with event    
    // Allow for logging of exceptions at a limited rate.
    FreshException += 1;
    if (FreshException > 10)
    {
    }
    else if (MinuteFileAvailable && (FreshException > 0))
    {
        //File.AppendAllText("PathToYourFile.txt", "Exception: " + FreshException.ToString() + " " + frmMonitor.lblException[0].Caption + " " + frmMonitor.lblException[1].Caption + Environment.NewLine);
        wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
        MinuteFile << Message << '\n' << std::flush;
    }
    else
    {
    }
}

void LandMonitorApp::CountSync(std::string Candidate) {
    int Kind;
    SyncEventCount += 1.0;

    if (Candidate == GPSSync) {
        Kind = 0;
    }
    else if (Candidate == ShortTubeSync) {
        Kind = 1;
    }
    else if (Candidate == LongTubeSync) {
        Kind = 2;
    }
    else if (Candidate == CtrSync) {
        Kind = 3;
    }
    else if (Candidate == DoneSync) {
        Kind = 4;
    }
    else {
        Kind = maxSyncType;
    }

    SyncTypeCount[Kind] += 1.0;
}

void LandMonitorApp::CountNoDelimeter(std::string Candidate) {
    int Kind;
    SyncEventCount += 1.0;

    if (Candidate == GPSSync) {
        Kind = 0;
    }
    else if (Candidate == ShortTubeSync) {
        Kind = 1;
    }
    else if (Candidate == LongTubeSync) {
        Kind = 2;
    }
    else if (Candidate == CtrSync) {
        Kind = 3;
    }
    else if (Candidate == DoneSync) {
        Kind = 4;
    }
    else {
        Kind = maxSyncType;
    }

    NoDelimiterCount[Kind] += 1.0;
}

int LandMonitorApp::OffsetOldest(int RemAdr, int LookOffset) {
    // Returns the pointer to a different event without otherwise disturbing the buffer.
    // The offset can be positive or negative
    int nTemp = (RingMod + OldestPointer[RemAdr] + LookOffset) % RingMod;
    return nTemp;
}

int LandMonitorApp::OffsetNewest(int RemAdr, int LookOffset) {
    // Returns a pointer to a different event without otherwise disturbing the buffer.
    // The offset can be positive or negative
    int nTemp = (RingMod + NewestPointer[RemAdr] + LookOffset) % RingMod;
    return nTemp;
}

std::string LandMonitorApp::HistoryLine(int LocRing, int RemoteUnit) {
    std::string historyLine = "History:";
    historyLine += " " + std::to_string(LocRing);
    historyLine += " " + std::to_string(RingPHA[LocRing][RemoteUnit]);
    historyLine += " " + std::string(1, RingSelect[LocRing][RemoteUnit] ? 'T' : 'F');
    historyLine += " " + std::to_string(RingRawTOscLOB[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingRawTOscHOB[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingCorTOscHOB[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingRawFullTime[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingCorFullTime[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingInterval[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingGPSTime[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingGoodGPSTime[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(RingBase[LocRing][RemoteUnit]);
    historyLine += " " + std::to_string(1000000.0 * RingPeriod[LocRing][RemoteUnit]);
    return historyLine;
}


void LandMonitorApp::ResetTiming() {
    // This forces a complete restart of the timing data collection system
    int cRemote;
    int iFor;
    int jFor;
    std::string CatchString;

    // If uncommented, initialize the GPS time to something reasonable
    // This will cause a glitch if the computer is not set to GMT
    // GPSHour = CurHour
    // GPSMinute = CurMin
    // GPSSecond = CurSec
    // TrackGPSCount = 5
    // PrevGPSSecond = (GPSSecond + 59) % 60

    //GenerateTimeStamp(CatchString); //TODO add this method
    //CatchString += " Reset Timing";
    //CatchString += " " + std::to_string(GPSHour);
    //CatchString += " " + std::to_string(GPSMinute);
    //CatchString += " " + std::to_string(GPSSecond);
    //LogEntry(CatchString);
    BareTrapOpen = false;
    nBaresTrapped = 0;
    CatchSensitiveTime = 0.0;
    BypSensitiveTime = 0.0;
    //frmClocker.lblBypassCount.Caption = "0"; //TODO connect to wx with event

    //if (CrossMultiplicityEnabled) {  //TODO add file
    //    Print(98, CatchString);
    //}

    LastBigCatchTime = 0.0;
    LastCatchTime = 0.0;
    GPSSentenceHistoryRingPointer = GPSSentenceHistoryRingPointer % GPSSentencePointerMod;
    GPSRingPointer = 0;
    LastGroupSizePointer = 0;

    //for (iFor = 0; iFor <= AbsoluteLastUnit; iFor++) {
    for (iFor = 0; iFor < AbsoluteLastUnit; iFor++) {
        //for (jFor = 0; jFor <= MaxEventClass; jFor++) {
        for (jFor = 0; jFor < MaxEventClass; jFor++) {
            GroupSizeDistribution[iFor][jFor] = 0;
        }
    }

    //for (cRemote = 0; cRemote <= LastUnit; cRemote++) {
    for (cRemote = 0; cRemote < LastUnit; cRemote++) {
        if (FirmVersion[cRemote] == 800) {
            //frmClocker.MarkerSyncLoss(cRemote); //TODO connect to wx with event

            //for (iFor = 0; iFor <= RingLast; iFor++) {
            for (iFor = 0; iFor < RingLast; iFor++) {
                RingPHA[iFor][cRemote] = 0;
                RingSelect[iFor][cRemote] = false;
                RingRawFullTime[iFor][cRemote] = 0.0;
                RingInterval[iFor][cRemote] = 0.0;
                RingRawTOscLOB[iFor][cRemote] = 0;
                RingRawTOscHOB[iFor][cRemote] = 0;
                RingGPSTime[iFor][cRemote] = -1.0;
                RingCorTOscHOB[iFor][cRemote] = 0;
                RingCorFullTime[iFor][cRemote] = 0.0;
                RingGoodGPSTime[iFor][cRemote] = 0.0;
                RingBase[iFor][cRemote] = 0.0;
                RingPeriod[iFor][cRemote] = 0.0;
            }

            BadGPSCount[cRemote] = 0;
            OldestPointer[cRemote] = 0;
            NewestPointer[cRemote] = 0;
            LatestOverflowPointer[cRemote] = 0;
            TimingLost[cRemote] = false;
            PrevTimingLost[cRemote] = false;
            LastGoodGPSTime[cRemote] = 0.0;
            DumpCountDown[cRemote] = 0;
            TOscCycle[cRemote] = 0;
            LatestTOsc[cRemote] = -1;
            EarlyOverflow[cRemote] = false;
            LateOverflow[cRemote] = false;
        }
    }

    OrderTimingReset = false;
}

void LandMonitorApp::Process800(std::string TubeRecord800)
{
    int iFor;
    int jFor;
    //int jPointer;
    int jPointer = 0; //TODO email Paul about init issue
    int kFor;
    int iPoint;
    int nEntry;
    int TotalHits;
    int RecoveredHits;
    int StatHisBin;
    int iDiscFire;
    int jDiscFire;
    int Adr;
    int Flg;
    int CorrectionPointer;
    double TrialCorFullTime;
    double PrevTrialCorFullTime;
    int PrePointer;
    int LocRing;
    int ResetType;
    int nFirstOverflow;
    long FirstOverflow;
    long OverflowIncrement;
    int HitsSinceOverflow;
    long TempLong;
    long CurPls;
    float CurValue;
    long ThisTime;
    double SyncFullTime;
    double HouseStartTime;
    double HouseEndTime;
    double PrelimGPSTime;
    bool EventIsBad;
    bool BadInRecord;
    bool SciData;
    bool DiscFire;
    string ReportString;
    string ABSString;
    bool OrderABSOutput;
    string HitSpoolRecord;
    string CommentMessage;
    string DiagMessage;
    bool EarlyOverflow;
    std::string Process800Out;
    // This is a multiplicity enabled unit with extended event buffers
    // Science and housekeeping data formats are quite different
    // Housekeeping format is identical to that of the 600 and 700 series
    // and is handled by the "DecomShortTube" routine
    // Event format uses a longer record that is handled by this "DecomLongTube" routine
    // Housekeeping data alternately may appear "inline"
    //SequenceListString = SequenceListString + UnitTag(CurrentUnit) + ",";
    SequenceListString = SequenceListString + UnitTag[CurrentUnit] + ",";
    //SciData = (Strings.Asc(Mid(TubeRecord800, 2)) & 16) > 0;
    //SciData = (TubeRecord800[2] & 16) > 0;
    SciData = ((uint8_t)TubeRecord800[1] & 16) > 0;
//    frmHouse.lblRemStatus1(CurrentUnit).Caption = SciData; //TODO connect to wx with event
    if (!SciData)
    {
        //CommentMessage = "REM " + Format(CurrentUnit);
        CommentMessage = "REM " + std::to_string(CurrentUnit);
        CommentMessage = CommentMessage + " flagged housekeeping data in long readout";
        Exception(CommentMessage);
        return;
    }

    //TimingLost(CurrentUnit) = (Strings.Asc(Mid(TubeRecord800, 2)) & 64) > 0;
    //TimingLost[CurrentUnit] = (TubeRecord800[2] & 64) > 0;
    TimingLost[CurrentUnit] = ((uint8_t)TubeRecord800[1] & 64) > 0;
    BadInRecord = false;
    OrderABSOutput = false;
    if (SequenceTimeSystem)
    {
        //ABSString = "ABS" + UnitTag(CurrentUnit) + ": S " + Format(SequenceSecOfDay);
        ABSString = "ABS" + UnitTag[CurrentUnit] + ": S " + std::to_string(SequenceSecOfDay);
    }
    else
    {
        //ABSString = "ABS" + UnitTag(CurrentUnit) + ": G " + Format(SequenceSecOfDay);
        ABSString = "ABS" + UnitTag[CurrentUnit] + ": G " + std::to_string(SequenceSecOfDay);
    }
    // Format as science data
    // nEntry count includes markers and housekeeping data
    //nEntry = Strings.Asc(Mid(TubeRecord800, 6));
    //nEntry = TubeRecord800[6];
    nEntry = (uint8_t)TubeRecord800[5];
    // Total number of triggers, whether they fit into the readout or not
    //TotalHits = Strings.Asc(Mid(TubeRecord800, 7));
    //TotalHits = TubeRecord800[7];
    TotalHits = (uint8_t)TubeRecord800[6];
    // Keep track of the number of hits in the record
    RecoveredHits = 0;
    StatHisBin = TotalHits;
    if (StatHisBin > RemStatHistLastChannel)
        StatHisBin = RemStatHistLastChannel;
    //RemStatHis(0, StatHisBin, CurrentUnit) = RemStatHis(0, StatHisBin, CurrentUnit) + 1;
    RemStatHis[0][StatHisBin][CurrentUnit]++; 
    //ABSString = ABSString + " " + Format(nEntry);
    ABSString += " " + std::to_string(nEntry);
    //ABSStrin+g + " " + Format(TotalHits);
    ABSString += " " + std::to_string(TotalHits);
    if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
    {
//        frmREMOTE.lblSent.Caption = Format(nEntry); //TODO connect to wx with event
//        frmREMOTE.lblTotal.Caption = Format(TotalHits); //TODO connect to wx with event
        for (iFor = 0; iFor <= MaxEntry800 - 1; iFor++)
        {
//            frmREMOTE.lblPHA(iFor).Visible = false; //TODO connect to wx with event
//            frmREMOTE.lblTime1(iFor).Visible = false; //TODO connect to wx with event
//            frmREMOTE.lblTrueTime(iFor).Visible = false; //TODO connect to wx with event
        }
    }
    // Format errors of various kinds can make the
    // number of events sent in error, so limit it
    // to avoid array overflow problems, etc.
    if (nEntry == 0)
    {
        //CommentMessage = "REM " + Format(CurrentUnit);
        CommentMessage = "REM " + std::to_string(CurrentUnit);
        CommentMessage = CommentMessage + " has no entries in record";
        Exception(CommentMessage);
        return;
    }

    if (nEntry > MaxEntry800)
        nEntry = MaxEntry800;
    //if (PrevTimingLost(CurrentUnit))
    if (PrevTimingLost[CurrentUnit])
    {
        // The previous readout lost some timing information.
        // It could be a sync marker, in which case nothing can be done.
        // If it is an overlow marker, then a patch is possible.
        // Look for the first overflow marker in this readout.
        nFirstOverflow = -1;
        for (iFor = 0; iFor <= nEntry - 1; iFor++)
        {
            //DiscFire = (Strings.Asc(Mid(TubeRecord800, iFor + 8)) & 128) != 0;
            //DiscFire = (TubeRecord800[iFor + 8] & 128) != 0;
            DiscFire = ((uint8_t)TubeRecord800[iFor + 7] & 128) != 0;
            //CurPls = (Strings.Asc(Mid(TubeRecord800, iFor + 8)) & 127);
            //CurPls = (TubeRecord800[iFor + 8] & 127);
            CurPls = ((uint8_t)TubeRecord800[iFor + 7] & 127);
            //if (DiscFire & (CurPls == 123))
            if (DiscFire && (CurPls == 123))
            {
                nFirstOverflow = iFor;
                break;
            }
        }
        if (nFirstOverflow >= 0)
        {
            //FirstOverflow = Asc(Mid(TubeRecord800, nFirstOverflow + MaxEntry800 + 8));
            //FirstOverflow = 256 * FirstOverflow + Asc(Mid(TubeRecord800, nFirstOverflow + 2 * MaxEntry800 + 8));
            //OverflowIncrement = FirstOverflow - LatestTOsc(CurrentUnit);
            //FirstOverflow = TubeRecord800[nFirstOverflow + MaxEntry800 + 8];
            FirstOverflow = (uint8_t)TubeRecord800[nFirstOverflow + MaxEntry800 + 7];
            FirstOverflow <<= 8;
            //FirstOverflow += TubeRecord800[nFirstOverflow + 2 * MaxEntry800 + 8];
            FirstOverflow += (uint8_t)TubeRecord800[nFirstOverflow + 2 * MaxEntry800 + 7];
            OverflowIncrement = FirstOverflow - LatestTOsc[CurrentUnit];
            if (OverflowIncrement == 1)
            {
            }
            //else if (LatestTOsc(CurrentUnit) == -1)
            else if (LatestTOsc[CurrentUnit] == -1)
            {
                // This is a timing reset, so just make things go smoothly
                //LatestTOsc(CurrentUnit) = (Mod16Bit + FirstOverflow - 1) % Mod16Bit;
                LatestTOsc[CurrentUnit] = (Mod16Bit + FirstOverflow - 1) % Mod16Bit;
            }
            //else if ((OverflowIncrement > 0) & (OverflowIncrement < 10))
            else if ((OverflowIncrement > 0) && (OverflowIncrement < 10))
            {
                // Try to patch a small gap
//                if (chkMinDiagnostics.Value == 0) 
                if (chkMinDiagnostics_Value == 0) //TODO update from wx
                {
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
       at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
       at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
       at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
       at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
       at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
       at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
       at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
       at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
       at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
       at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
       at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
       at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
       at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
       at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
       at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
       at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

    Input:
    Print #2, "Patch Overflow A " & Format$(CurrentUnit) & " " & Format$(FirstOverflow) & " " & Format$(LatestTOsc(CurrentUnit))

     */
                    Process800Out += "Patch Overflow A " + std::to_string(CurrentUnit) + " " + std::to_string(FirstOverflow) + " " + std::to_string(LatestTOsc[CurrentUnit]) + '\n';
                }
                //LatestTOsc(CurrentUnit) = LatestTOsc(CurrentUnit) + OverflowIncrement - 1;
                LatestTOsc[CurrentUnit] += OverflowIncrement - 1;
            }
            else if (OverflowIncrement < 0)
            {
                if (((OverflowIncrement + Mod16Bit) % Mod16Bit) == 1)
                {
                }
                else if (((OverflowIncrement + Mod16Bit) % Mod16Bit) < 10)
                {
                    // Try to patch a small gap
//                    if (chkMinDiagnostics.Value == 0) 
                    if (chkMinDiagnostics_Value == 0) //TODO update from wx
                    {
                   
                        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Patch Overflow B " & Format$(CurrentUnit) & " " & Format$(FirstOverflow) & " " & Format$(LatestTOsc(CurrentUnit))

 */
                        Process800Out += "Patch Overflow B " + std::to_string(CurrentUnit) + " " + std::to_string(FirstOverflow) + " " + std::to_string(LatestTOsc[CurrentUnit]) + '\n';
                    }
                    //LatestTOsc(CurrentUnit) = (Mod16Bit + LatestTOsc(CurrentUnit) + OverflowIncrement - 1) % Mod16Bit;
                    //TOscCycle(CurrentUnit) = TOscCycle(CurrentUnit) + 1;
                    LatestTOsc[CurrentUnit] = (Mod16Bit + LatestTOsc[CurrentUnit] + OverflowIncrement - 1) % Mod16Bit;
                    TOscCycle[CurrentUnit]++;
                }
                else
                {
                    //frmClocker.MarkerSyncLoss(CurrentUnit); //TODO convert frmClocker
                }
            }
        }
        else
        {
        }
    }
    // "Pulse Height" Data
    for (iFor = 0; iFor <= nEntry - 1; iFor++)
    {
        //DiscFire = (Strings.Asc(Mid(TubeRecord800, iFor + 8)) & 128) != 0;
        //DiscFire = (TubeRecord800[iFor + 8] & 128) != 0;
        DiscFire = ((uint8_t)TubeRecord800[iFor + 7] & 128) != 0;
        if (DiscFire)
        {
            iDiscFire = 1;
            //ABSString = ABSString + " +";
            ABSString += " +";
        }
        else
        {
            iDiscFire = 0;
            //ABSString = ABSString + " -";
            ABSString = " -";
        }
        //        CurPls = (Strings.Asc(Mid(TubeRecord800, iFor + 8)) & 127);
        //CurPls = (TubeRecord800[iFor + 8] & 127);
        CurPls = ((uint8_t)TubeRecord800[iFor + 7] & 127);
        //        ThisTime = Asc(Mid(TubeRecord800, iFor + MaxEntry800 + 8));
        //ThisTime = TubeRecord800[iFor + MaxEntry800 + 8];
        //ThisTime = 256 * ThisTime + TubeRecord800[iFor + 2 * MaxEntry800 + 8];
        ThisTime = (uint8_t)TubeRecord800[iFor + MaxEntry800 + 7];
        ThisTime = 256 * ThisTime + (uint8_t)TubeRecord800[iFor + 2 * MaxEntry800 + 7];
        //ABSString = ABSString + Format(CurPls) + " " + Format(ThisTime);
        ABSString += std::to_string(CurPls) + " " + std::to_string(ThisTime);
        //if (DiscFire & (CurPls == 123))
        if (DiscFire && (CurPls == 123))
        {
            // This is a timer overflow marker
//            OverflowIncrement = ThisTime - LatestTOsc(CurrentUnit);
            OverflowIncrement = ThisTime - LatestTOsc[CurrentUnit];
            if (OverflowIncrement == 1)
            {
                // There is no problem
//                LatestTOsc(CurrentUnit) = ThisTime;
                LatestTOsc[CurrentUnit] = ThisTime;
            }
            //            else if (LatestTOsc(CurrentUnit) == -1)
            else if (LatestTOsc[CurrentUnit] == -1)
            {
                // This is a timing reset, so just make things go smoothly
//                LatestTOsc(CurrentUnit) = (Mod16Bit + ThisTime - 1) % Mod16Bit;
                LatestTOsc[CurrentUnit] = (Mod16Bit + ThisTime - 1) % Mod16Bit;
            }
            else if (OverflowIncrement == 0)
            {
                //                if (chkMinDiagnostics.Value == 0) 
                if (chkMinDiagnostics_Value == 0) //TODO update from wx
                {
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Overflow Increment Zero " & Format$(CurrentUnit) & " " & Format$(ThisTime) & " " & Format$(LatestTOsc(CurrentUnit))

 */
                    Process800Out += "Overflow Increment Zero " + std::to_string(CurrentUnit) + " " + std::to_string(ThisTime) + " " + std::to_string(LatestTOsc[CurrentUnit]) + '\n';
                }
                //                LatestTOsc(CurrentUnit) = ThisTime;
                LatestTOsc[CurrentUnit] = ThisTime;
            }
            else if (OverflowIncrement > 1)
            {
                //                if (chkMinDiagnostics.Value == 0) 
                if (chkMinDiagnostics_Value == 0) //TODO update from wx
                {
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Overflow Increment Gap A " & Format$(CurrentUnit) & " " & Format$(ThisTime) & " " & Format$(LatestTOsc(CurrentUnit))

 */
                    Process800Out += "Overflow Increment Gap A " + std::to_string(CurrentUnit) + " " + std::to_string(ThisTime) + " " + std::to_string(LatestTOsc[CurrentUnit]) + '\n';
                }
                nOverflowGapA++;
                //                LatestTOsc(CurrentUnit) = ThisTime;
                LatestTOsc[CurrentUnit] = ThisTime;
            }
            else if (OverflowIncrement < 0)
            {
                //                TOscCycle(CurrentUnit) = TOscCycle(CurrentUnit) + 1;
                TOscCycle[CurrentUnit]++;
                if (((OverflowIncrement + Mod16Bit) % Mod16Bit) == 1)
                {
                    // There is no problem
//                    LatestTOsc(CurrentUnit) = ThisTime;
                    LatestTOsc[CurrentUnit] = ThisTime;
                }
                else if (((OverflowIncrement + Mod16Bit) % Mod16Bit) > 1)
                {
                    //                    if (chkMinDiagnostics.Value == 0) 
                    if (chkMinDiagnostics_Value == 0) //TODO update from wx
                    {
                        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Overflow Increment Gap B " & Format$(CurrentUnit) & " " & Format$(ThisTime) & " " & Format$(LatestTOsc(CurrentUnit))

 */
                        Process800Out += "Overflow Increment Gap B " + std::to_string(CurrentUnit) + " " + std::to_string(ThisTime) + " " + std::to_string(LatestTOsc[CurrentUnit]) + '\n';
                    }
                    //                    LatestTOsc(CurrentUnit) = ThisTime;
                    LatestTOsc[CurrentUnit] = ThisTime;
                }
            }
            //            frmHouse.lblRemStatus2(CurrentUnit).Caption = Format(ThisTime); //TODO connect to wx with event
            //            frmHouse.lblRemStatus2(CurrentUnit).BackColor = ColorPink; //TODO connect to wx with event
                        //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                //                frmREMOTE.lblPHA(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).Caption = Strings.Format(CurPls); //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).BackColor = ColorPink; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Caption = Strings.Format(ThisTime); //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).BackColor = ColorPink; //TODO connect to wx with event
            }
        }
        //else if (DiscFire & (CurPls == 125))
        else if (DiscFire && (CurPls == 125))
        {
            // This is an embedded housekeeping start
            // The housekeeping data have no identifiers or delimiters
            // The format is positional within the block of bytes
            // Remotes ensure that a block is completely contained within
            // one transmission.
            // Entries with a pulseheight of 125+ and 124+ delimit a 24 byte housekeeping block
            // The housekeeping bytes occupy positions in the buffer as noted here:
            // 
            // "PHA"     TimeHOB  TimeLOB
            // 125       OnTimeH  OnTimeL    ;The time is when the interrupts go off
            // Version   ADC1H    ADC1L
            // Revision  ADC2H    ADC2L
            // ResetCode ADC3H    ADC3L
            // TovrCount ADC4H    ADC4L
            // Temp2H    Temp1H   Temp1L
            // Temp3L    Temp2L   Temp3H
            // 124       OffTimeH OffTimeL   ;The time is when the interrupts come back on
//            HouseStartTime = System.Convert.ToDouble(65536 * TOscCycle(CurrentUnit) + ThisTime);
            HouseStartTime = 65536 * (double)TOscCycle[CurrentUnit] + (double)ThisTime;
            //            frmHouse.lblHODB(CurrentUnit).Caption = Format(ThisTime); //TODO connect to wx with event
            //            frmHouse.lblHODB(CurrentUnit).BackColor = ColorGreen; //TODO connect to wx with event
                        //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                //                frmREMOTE.lblPHA(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).Caption = Strings.Format(CurPls); //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).BackColor = ColorGreen; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Caption = Strings.Format(ThisTime); //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).BackColor = ColorGreen; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Caption = Strings.Format(HouseStartTime); //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).BackColor = ColorGreen; //TODO connect to wx with event
            }
            iFor = iFor + 1;
            //            frmHouse.lblVersion(CurrentUnit).Caption = Strings.Asc(Mid(TubeRecord800, iFor + 8)); //TODO connect to wx with event
                        // Voltages
            for (jFor = 0; jFor <= 3; jFor++)
            {
                //                TempLong = Asc(Mid(TubeRecord800, iFor + jFor + MaxEntry800 + 8));
                //TempLong = TubeRecord800[iFor + jFor + MaxEntry800 + 8];
                TempLong = (uint8_t)TubeRecord800[iFor + jFor + MaxEntry800 + 7];
                //                TempLong = (4 * TempLong) + (Asc(Mid(TubeRecord800, iFor + jFor + 2 * MaxEntry800 + 8)) / 64);
                //TempLong = (4 * TempLong) + (TubeRecord800[iFor + jFor + 2 * MaxEntry800 + 8] / 64);
                TempLong = (4 * TempLong) + ((uint8_t)TubeRecord800[iFor + jFor + 2 * MaxEntry800 + 7] / 64);
                //                House(jFor, CurrentUnit) = TempLong;
                House[jFor][CurrentUnit] = TempLong;
                //                Adr = Asc(Mid(TubeRecord800, iFor + jFor + 2 * MaxEntry800 + 8)) & 7;
                //Adr = TubeRecord800[iFor + jFor + 2 * MaxEntry800 + 8] & 7;
                Adr = (uint8_t)TubeRecord800[iFor + jFor + 2 * MaxEntry800 + 7] & 7;
                //                Flg = Asc(Mid(TubeRecord800, iFor + jFor + 2 * MaxEntry800 + 8)) & 8;
                //Flg = TubeRecord800[iFor + jFor + 2 * MaxEntry800 + 8] & 8;
                Flg = (uint8_t)TubeRecord800[iFor + jFor + 2 * MaxEntry800 + 7] & 8;
                if (Adr != (jFor + 1))
                {
                    //House(jFor, CurrentUnit) = -House(jFor, CurrentUnit);
                    House[jFor][CurrentUnit] *= -1;
                }
            }
            iFor = iFor + 1;
            //            frmHouse.lblRevision(CurrentUnit).Caption = Strings.Asc(Mid(TubeRecord800, iFor + 8)); //TODO connect to wx with event
            iFor = iFor + 1;
            //            ResetType = Strings.Asc(Mid(TubeRecord800, iFor + 8));
//            ResetType = TubeRecord800[iFor + 8];
            ResetType = (uint8_t)TubeRecord800[iFor + 7];
            //            frmHouse.lblResetType(CurrentUnit).Caption = ResetType; //TODO connect to wx with event
            if (ResetType > 0)
            {
                //Exception("REM " + Format(CurrentUnit) + " Reset Code " + Format(ResetType));
                Exception("REM " + std::to_string(CurrentUnit) + " Reset Code " + std::to_string(ResetType));
            }
            iFor = iFor + 1;
            iFor = iFor + 1;
            // Temperature Sensors
            // T1
//            if ((Asc(Mid(TubeRecord800, iFor + 2 * MaxEntry800 + 8)) & 16) != 0)
//            if ((TubeRecord800[iFor + 2 * MaxEntry800 + 8] & 16) != 0)
            if (((uint8_t)TubeRecord800[iFor + 2 * MaxEntry800 + 7] & 16) != 0)
            {
                //                House(4, CurrentUnit) = -((Asc(Mid(TubeRecord800, iFor + MaxEntry800 + 8)) ^ 255) + 1);
//                House(4, CurrentUnit) = -((TubeRecord800[iFor + MaxEntry800 + 8] ^ 255) + 1);
//                House[4][CurrentUnit] = -((TubeRecord800[iFor + MaxEntry800 + 8] ^ 255) + 1);
                House[4][CurrentUnit] = -(((uint8_t)TubeRecord800[iFor + MaxEntry800 + 7] ^ 255) + 1);
            }
            else
            {
                //                House(4, CurrentUnit) = Asc(Mid(TubeRecord800, iFor + MaxEntry800 + 8));
//                House(4, CurrentUnit) = TubeRecord800[iFor + MaxEntry800 + 8];
//                House[4][CurrentUnit] = TubeRecord800[iFor + MaxEntry800 + 8];
                House[4][CurrentUnit] = (uint8_t)TubeRecord800[iFor + MaxEntry800 + 7];
                // T2
            }
            //            if ((Asc(Mid(TubeRecord800, iFor + MaxEntry800 + 9)) & 16) != 0)
            if (((uint8_t)TubeRecord800[iFor + MaxEntry800 + 9] & 16) != 0)
            {
                //                House(5, CurrentUnit) = -((Strings.Asc(Mid(TubeRecord800, iFor + 8)) ^ 255) + 1);
//                House(5, CurrentUnit) = -((TubeRecord800[iFor + 8] ^ 255) + 1);
//                House[5][CurrentUnit] = -((TubeRecord800[iFor + 8] ^ 255) + 1);
                House[5][CurrentUnit] = -(((uint8_t)TubeRecord800[iFor + 7] ^ 255) + 1);
            }
            else
            {
                //                House(5, CurrentUnit) = Strings.Asc(Mid(TubeRecord800, iFor + 8));
//                House(5, CurrentUnit) = TubeRecord800[iFor + 8];
//                House[5][CurrentUnit] = TubeRecord800[iFor + 8];
                House[5][CurrentUnit] = (uint8_t)TubeRecord800[iFor + 7];
            }
            // T3
//            if ((Strings.Asc(Mid(TubeRecord800, iFor + 9)) & 16) != 0)
            if (((uint8_t)TubeRecord800[iFor + 9] & 16) != 0)
            {
                //                House(6, CurrentUnit) = -((TubeRecord800[iFor + 2 * MaxEntry800 + 9] ^ 255) + 1);
                House[6][CurrentUnit] = -(((uint8_t)TubeRecord800[iFor + 2 * MaxEntry800 + 9] ^ 255) + 1);
            }
            else
            {
                //                House(6, CurrentUnit) = TubeRecord800[iFor + 2 * MaxEntry800 + 9];
                House[6][CurrentUnit] = (uint8_t)TubeRecord800[iFor + 2 * MaxEntry800 + 9];
            }
            iFor = iFor + 1;
            //RemoteVoltages(CurrentUnit); //TODO RemoteVoltages
//            lblT1(CurrentUnit).Caption = Format((0.5 * House(4, CurrentUnit)), "###0.0"); //TODO connect to wx with event
//            lblT2(CurrentUnit).Caption = Format((0.5 * House(5, CurrentUnit)), "###0.0"); //TODO connect to wx with event
//            lblT3(CurrentUnit).Caption = Format((0.5 * House(6, CurrentUnit)), "###0.0"); //TODO connect to wx with event
        }
        //else if (DiscFire & (CurPls == 124))
        else if (DiscFire && (CurPls == 124))
        {
            // This is a housekeeping stop marker
            //HouseEndTime = System.Convert.ToDouble(65536 * TOscCycle(CurrentUnit) + ThisTime);
//            HouseEndTime = (double)(65536.0 * (double)TOscCycle(CurrentUnit) + (double)ThisTime);
            HouseEndTime = (double)(65536.0 * (double)TOscCycle[CurrentUnit] + (double)ThisTime);
            //            frmHouse.lblLODB(CurrentUnit).Caption = Format(HouseEndTime - HouseStartTime); //TODO connect to wx with event
            //            frmHouse.lblLODB(CurrentUnit).BackColor = ColorBlue; //TODO connect to wx with event
                        //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                //                frmREMOTE.lblPHA(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).Caption = Strings.Format(CurPls); //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).BackColor = ColorBlue; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Caption = Strings.Format(ThisTime); //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).BackColor = ColorBlue; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Caption = Strings.Format(HouseEndTime); //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).BackColor = ColorBlue; //TODO connect to wx with event
            }
        }
        //else if (DiscFire & (CurPls == 126))
        else if (DiscFire && (CurPls == 126))
        {
            // This is a time sync
            //SyncFullTime = 65536#  * System.Convert.ToDouble(65536 * TOscCycle(CurrentUnit) + LatestTOsc(CurrentUnit)) + System.Convert.ToDouble(ThisTime);
            SyncFullTime = 65536.0 * (double)(65536.0 * (double)TOscCycle[CurrentUnit] + (double)LatestTOsc[CurrentUnit]) + (double)(ThisTime);
            //            TimeMarkerRing(GPSRingPointer, CurrentUnit) = SyncFullTime;
            TimeMarkerRing[GPSRingPointer][CurrentUnit] = SyncFullTime;
            //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                //                frmREMOTE.lblPHA(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).Caption = Strings.Format(CurPls); //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).BackColor = ColorRed; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Caption = Strings.Format(ThisTime); //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).BackColor = ColorRed; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Caption = Strings.Format(SyncFullTime); //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).BackColor = ColorRed; //TODO connect to wx with event
            }
        }
        else
        {
            // This is an actual event
            RecoveredHits = RecoveredHits + 1;
            EventIsBad = false;
            //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                //                frmREMOTE.lblPHA(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblPHA(iFor).Caption = Strings.Format(CurPls); //TODO connect to wx with event
                if (!DiscFire)
                {
                    //                    frmREMOTE.lblPHA(iFor).BackColor = ColorYellow; //TODO connect to wx with event
                    //                    frmREMOTE.lblTime1(iFor).BackColor = ColorYellow; //TODO connect to wx with event
                    //                    frmREMOTE.lblTrueTime(iFor).BackColor = ColorYellow; //TODO connect to wx with event
                }
                else
                {
                    //                    frmREMOTE.lblPHA(iFor).BackColor = ColorWhite; //TODO connect to wx with event
                    //                    frmREMOTE.lblTime1(iFor).BackColor = ColorWhite; //TODO connect to wx with event
                    //                    frmREMOTE.lblTrueTime(iFor).BackColor = ColorWhite; //TODO connect to wx with event
                }
            }
            //CurValue = System.Convert.ToSingle(CurPls);
            CurValue = (float)(CurPls);
            EnterMonitorHis(CurValue, PHAHisOrigin + CurrentUnit, iDiscFire);
            //if (chkEmulate616.Value == 1)
            if (chkEmulate616_Value == 1)
            {
                // Emulate the REM616 timing distributions
                //CurValue = System.Convert.ToSingle((Mod16Bit + ThisTime - PreviousThisTime(CurrentUnit)) % Mod16Bit);
                CurValue = (Mod16Bit + ThisTime - PreviousThisTime[CurrentUnit]) % Mod16Bit;
                EnterMonitorHis(CurValue, MOSHisOrigin + CurrentUnit, iDiscFire);
                EnterMonitorHis(CurValue, MOLHisOrigin + CurrentUnit, iDiscFire);
                //PreviousThisTime(CurrentUnit) = ThisTime;
                PreviousThisTime[CurrentUnit] = ThisTime;
            }
            // Accumulate "Window" statistics
//            if (CurPls < MonitorLimit(WMin, CurrentUnit))
            if (CurPls < MonitorLimit[WMin][CurrentUnit])
            {
                //Below(CurrentUnit) = Below(CurrentUnit) + 1;
                Below[CurrentUnit]++;
            }
            //            else if (CurPls > MonitorLimit(WMax, CurrentUnit))
            else if (CurPls > MonitorLimit[WMax][CurrentUnit])
            {
                //Above(CurrentUnit) = Above(CurrentUnit) + 1;
                Above[CurrentUnit]++;
            }
            else
            {
                //InArray(CurrentUnit) = InArray(CurrentUnit) + 1;
                InArray[CurrentUnit]++;
            }
            // Enter the event in the ring buffers
            // Keep track of where the hits end up
//            HitTrack(0, CurrentUnit) = HitTrack(0, CurrentUnit) + 1;
            HitTrack[0][CurrentUnit]++;
            //IncrementNewest(CurrentUnit); //TODO IncrementNewest
            //RingPHA(NewestPointer(CurrentUnit), CurrentUnit) = CurPls;
            //RingSelect(NewestPointer(CurrentUnit), CurrentUnit) = DiscFire;
            //RingRawTOscLOB(NewestPointer(CurrentUnit), CurrentUnit) = ThisTime;
            //RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) = 65536 * TOscCycle(CurrentUnit) + LatestTOsc(CurrentUnit);
            //RingGoodGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = LastGoodGPSTime(CurrentUnit);
            //RingBase(NewestPointer(CurrentUnit), CurrentUnit) = MarkerBase(CurrentUnit);
            //RingPeriod(NewestPointer(CurrentUnit), CurrentUnit) = MarkerPeriod(CurrentUnit);
            RingPHA[NewestPointer[CurrentUnit]][CurrentUnit] = CurPls;
            RingSelect[NewestPointer[CurrentUnit]][CurrentUnit] = DiscFire;
            RingRawTOscLOB[NewestPointer[CurrentUnit]][CurrentUnit] = ThisTime;
            RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] = 65536 * TOscCycle[CurrentUnit] + LatestTOsc[CurrentUnit];
            RingGoodGPSTime[NewestPointer[CurrentUnit]][CurrentUnit] = LastGoodGPSTime[CurrentUnit];
            RingBase[NewestPointer[CurrentUnit]][CurrentUnit] = MarkerBase[CurrentUnit];
            RingPeriod[NewestPointer[CurrentUnit]][CurrentUnit] = MarkerPeriod[CurrentUnit];
            //if (RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) > RingRawTOscHOB(OffsetNewest(CurrentUnit, -1), CurrentUnit))
            //    LatestOverflowPointer(CurrentUnit) = NewestPointer(CurrentUnit);
            //RingRawFullTime(NewestPointer(CurrentUnit), CurrentUnit) = 65536#  * System.Convert.ToDouble(RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit)) + System.Convert.ToDouble(ThisTime);
            //if (LateOverflow(CurrentUnit))
            //{
            //    // First see if the overflow has "come in"
            //    if (RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) == RingRawTOscHOB(OffsetNewest(CurrentUnit, -1), CurrentUnit))
            //        // Nothing has happened, so keep incrementing
            //        RingCorTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) = RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) + 1;
            //    else
            //    {
            //        // Do not increment and remove the late overflow flag
            //        RingCorTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) = RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit);
            //        LateOverflow(CurrentUnit) = false;
            //    }
            //}
            //else
            //    RingCorTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) = RingRawTOscHOB(NewestPointer(CurrentUnit), CurrentUnit);
            if (RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] > RingRawTOscHOB[OffsetNewest(CurrentUnit, -1)][CurrentUnit])
            {
                LatestOverflowPointer[CurrentUnit] = NewestPointer[CurrentUnit];
            }
            RingRawFullTime[NewestPointer[CurrentUnit]][CurrentUnit] = 65536.0 * (double)(RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit]) + (double)(ThisTime);
            if (LateOverflow[CurrentUnit])
            {
                // First see if the overflow has "come in"
                if (RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] == RingRawTOscHOB[OffsetNewest(CurrentUnit, -1)][CurrentUnit])
                {
                    // Nothing has happened, so keep incrementing
                    RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] = RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] + 1;
                }
                else
                {
                    // Do not increment and remove the late overflow flag
                    RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] = RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit];
                    LateOverflow[CurrentUnit] = false;
                }
            }
            else
            {
                RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] = RingRawTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit];
            }
            RingCorFullTime[NewestPointer[CurrentUnit]][CurrentUnit] = 65536.0 * (double)(RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit]) + (double)RingRawTOscLOB[NewestPointer[CurrentUnit]][CurrentUnit];
            RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit] = -1;
            // Other than a hardware or data link problem these oscillator counts must be monotonic
            if (RingRawFullTime[NewestPointer[CurrentUnit]][CurrentUnit] < RingRawFullTime[OffsetNewest(CurrentUnit, -1)][CurrentUnit])
            {
                // Deal with step back in LOB time counter
                // There are only two cases logically possible, an early overflow or a late overflow.
                // For an early overflow, the information for the hit is
                // correct, but some number of earlier hit times were
                // incorrect. These would have had monotonically increasing
                // times.
                // For a late overflow, the information for the his is
                // incorrect, and some number of later hist may also have
                // incorrect hit times
                // To determine which case, based only on information available
                // now, see if the preceeding hit times ALL remain monotonic
                // under the assumption that the latest oscillator timeer
                // was premature. The location of the latest overflow was noted.
//                HitsSinceOverflow = (RingMod + NewestPointer(CurrentUnit) - LatestOverflowPointer(CurrentUnit)) % RingMod;
//                PrePointer = (RingMod + LatestOverflowPointer(CurrentUnit) - 1) % RingMod;
//                PrevTrialCorFullTime = 65536#  * System.Convert.ToDouble(RingRawTOscHOB(PrePointer, CurrentUnit)) + System.Convert.ToDouble(RingRawTOscLOB(PrePointer, CurrentUnit));
//                DiagMessage = "Early/Late Decision";
//                DiagMessage = DiagMessage + " " + Format(CurrentUnit);
//                DiagMessage = DiagMessage + " " + Format(HitsSinceOverflow);
//                DiagMessage = DiagMessage + " " + Format(LatestOverflowPointer(CurrentUnit));
//                DiagMessage = DiagMessage + " " + Format(RingCorFullTime(LatestOverflowPointer(CurrentUnit), CurrentUnit));
//                DiagMessage = DiagMessage + " " + Format(PrePointer);
//                DiagMessage = DiagMessage + " " + Format(PrevTrialCorFullTime);
                HitsSinceOverflow = (RingMod + NewestPointer[CurrentUnit] - LatestOverflowPointer[CurrentUnit]) % RingMod;
                PrePointer = (RingMod + LatestOverflowPointer[CurrentUnit] - 1) % RingMod;
                PrevTrialCorFullTime = 65536.0 * (double)(RingRawTOscHOB[PrePointer][CurrentUnit]) + (double)(RingRawTOscLOB[PrePointer][CurrentUnit]);
                DiagMessage = "Early/Late Decision";
                DiagMessage += " " + std::to_string(CurrentUnit);
                DiagMessage += " " + std::to_string(HitsSinceOverflow);
                DiagMessage += " " + std::to_string(LatestOverflowPointer[CurrentUnit]);
                DiagMessage += " " + std::to_string(RingCorFullTime[LatestOverflowPointer[CurrentUnit]][CurrentUnit]);
                DiagMessage += " " + std::to_string(PrePointer);
                DiagMessage += " " + std::to_string(PrevTrialCorFullTime);
                // Print #2, DiagMessage
                EarlyOverflow = true;
                // Go through all the hits beginning with the last overflow
                for (jFor = 0; jFor <= HitsSinceOverflow - 1; jFor++)
                {
                    jPointer = (PrePointer + 1 + jFor) % RingMod;
                    //                    TrialCorFullTime = 65536#  * System.Convert.ToDouble(RingRawTOscHOB(jPointer, CurrentUnit) - 1) + System.Convert.ToDouble(RingRawTOscLOB(jPointer, CurrentUnit));
                    TrialCorFullTime = 65536.0 * (double)(RingRawTOscHOB[jPointer][CurrentUnit] - 1) + (double)(RingRawTOscLOB[jPointer][CurrentUnit]);
                    if (TrialCorFullTime > PrevTrialCorFullTime)
                        // This hit is OK, move on to the next one
                    {
                        PrevTrialCorFullTime = TrialCorFullTime;
                    }
                    else
                    {
                        // One strike and you are out
                        EarlyOverflow = false;
                        break;
                    }
                }
                if (EarlyOverflow)
                {
                    // Still one more test
                    //if (PrevTrialCorFullTime > RingCorFullTime(NewestPointer(CurrentUnit), CurrentUnit))
                    if (PrevTrialCorFullTime > RingCorFullTime[NewestPointer[CurrentUnit]][CurrentUnit])
                        // Test fails
                    {
                        EarlyOverflow = false;
                    }
                }
                // 
                // Here the decision has been made, so just act on it
                if (EarlyOverflow)
                {
                    //EarlyOverCount(CurrentUnit) = EarlyOverCount(CurrentUnit) + 1;
                    EarlyOverCount[CurrentUnit]++;
                    // CommentMessage = CommentMessage & " " & "Early Overflow " & Format$(NewestPointer(CurrentUnit)) & " " & Format$(CurrentUnit)
                    // The GPS times are also wrong
//                    LastGoodGPSTime(CurrentUnit) = RingGPSTime(PrePointer, CurrentUnit);
                    LastGoodGPSTime[CurrentUnit] = RingGPSTime[PrePointer][CurrentUnit];
                    // Print #2, "  " & Format$(LatestOverflowPointer(CurrentUnit)) & " " & Format$(RingRawFullTime(LatestOverflowPointer(CurrentUnit), CurrentUnit))
                    for (jFor = 0; jFor <= HitsSinceOverflow - 1; jFor++)
                    {
                        jPointer = (PrePointer + 1 + jFor) % RingMod;
                        //                        RingCorTOscHOB(jPointer, CurrentUnit) = RingRawTOscHOB(jPointer, CurrentUnit) - 1;
                        RingCorTOscHOB[jPointer][CurrentUnit] = RingRawTOscHOB[jPointer][CurrentUnit] - 1;
                        //                        RingCorFullTime(jPointer, CurrentUnit) = 65536#  * System.Convert.ToDouble(RingCorTOscHOB(jPointer, CurrentUnit)) + RingRawTOscLOB(jPointer, CurrentUnit);
                        RingCorFullTime[jPointer][CurrentUnit] = 65536.0 * (double)(RingCorTOscHOB[jPointer][CurrentUnit]) + RingRawTOscLOB[jPointer][CurrentUnit];
                        // Unselected/Selected status does not change
                        jDiscFire = 0;
                        //                        if (RingSelect(jPointer, CurrentUnit))
                        if (RingSelect[jPointer][CurrentUnit])
                            jDiscFire = 1;
                        // Remove possibly erroneous timing histogram entry
//                        CurValue = System.Convert.ToSingle(RingInterval(jPointer, CurrentUnit));
                        CurValue = (float)(RingInterval[jPointer][CurrentUnit]);
                        EraseMonitorHis(CurValue, MPSHisOrigin + CurrentUnit, jDiscFire);
                        EraseMonitorHis(CurValue, MPLHisOrigin + CurrentUnit, jDiscFire);
                        // Correct the time interval
//                        RingInterval(jPointer, CurrentUnit) = RingCorFullTime(jPointer, CurrentUnit) - RingCorFullTime(PrePointer, CurrentUnit);
                        RingInterval[jPointer][CurrentUnit] -= RingCorFullTime[PrePointer][CurrentUnit];
                        // Put the correct value into the histograms
//                        CurValue = System.Convert.ToSingle(RingInterval(jPointer, CurrentUnit));
                        CurValue = (float)(RingInterval[jPointer][CurrentUnit]);
                        EnterMonitorHis(CurValue, MPSHisOrigin + CurrentUnit, jDiscFire);
                        EnterMonitorHis(CurValue, MPLHisOrigin + CurrentUnit, jDiscFire);
                        //                        if (MarkerSync(CurrentUnit))
                        if (MarkerSync[CurrentUnit])
                        {
                            //                            PrelimGPSTime = MarkerBase(CurrentUnit) + MarkerPeriod(CurrentUnit) * RingCorFullTime(jPointer, CurrentUnit);
                            //                            PrelimGPSTime = MarkerBase(CurrentUnit) + MarkerPeriod(CurrentUnit) * RingCorFullTime[jPointer][CurrentUnit];
                            //                            PrelimGPSTime = MarkerBase(CurrentUnit) + MarkerPeriod[CurrentUnit] * RingCorFullTime[jPointer][CurrentUnit];
                            PrelimGPSTime = MarkerBase[CurrentUnit] + MarkerPeriod[CurrentUnit] * RingCorFullTime[jPointer][CurrentUnit];
                            //                            // Print #2, "PrelimGPSTime : " & Format$(PrelimGPSTime) & " " & Format$(LastGoodGPSTime(CurrentUnit))
                                                        // Print #2, "PrelimGPSTime : " & Format$(PrelimGPSTime) & " " & Format$(LastGoodGPSTime[CurrentUnit])
                            //                            if (PrelimGPSTime >= LastGoodGPSTime(CurrentUnit))
                            if (PrelimGPSTime >= LastGoodGPSTime[CurrentUnit])
                            {
                                // Also replace a previously calculated GPS time.
//                                RingGPSTime(jPointer, CurrentUnit) = PrelimGPSTime;
                                RingGPSTime[jPointer][CurrentUnit] = PrelimGPSTime;
                                //                                LastGoodGPSTime(CurrentUnit) = PrelimGPSTime;
                                LastGoodGPSTime[CurrentUnit] = PrelimGPSTime;
                            }
                            else
                            {
                                //                                RingGPSTime(jPointer, CurrentUnit) = -1#;
                                RingGPSTime[jPointer][CurrentUnit] = -1;
                                //                                BadGPSCount(CurrentUnit) = BadGPSCount(CurrentUnit) + 1;
                                BadGPSCount[CurrentUnit]++;
                                BadInRecord = true;
                            }
                        }
                    }
                    //                    RingInterval(NewestPointer(CurrentUnit), CurrentUnit) = RingCorFullTime(NewestPointer(CurrentUnit), CurrentUnit) - RingCorFullTime(jPointer, CurrentUnit);
                    //                    RingInterval(NewestPointer(CurrentUnit), CurrentUnit) = RingCorFullTime(NewestPointer(CurrentUnit), CurrentUnit) - RingCorFullTime[jPointer][CurrentUnit];
                    //                    RingInterval(NewestPointer(CurrentUnit), CurrentUnit) = RingCorFullTime[NewestPointer[CurrentUnit]][CurrentUnit] - RingCorFullTime[jPointer][CurrentUnit];
                    RingInterval[NewestPointer[CurrentUnit]][CurrentUnit] -= RingCorFullTime[jPointer][CurrentUnit];
                }
                else
                {
                    // It is a late overflow
//                    LateOverCount(CurrentUnit) = LateOverCount(CurrentUnit) + 1;
                    LateOverCount[CurrentUnit]++;
                    // CommentMessage = CommentMessage & " " & "Late Overflow " & Format$(NewestPointer(CurrentUnit)) & " " & Format$(CurrentUnit)
//                    LateOverflow(CurrentUnit) = true;
                    LateOverflow[CurrentUnit] = true;
                    //RingCorTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) = RingCorTOscHOB(NewestPointer(CurrentUnit), CurrentUnit) + 1;
                    //RingCorFullTime(NewestPointer(CurrentUnit), CurrentUnit) = 65536#  * System.Convert.ToDouble(RingCorTOscHOB(NewestPointer(CurrentUnit), CurrentUnit)) + RingRawTOscLOB(NewestPointer(CurrentUnit), CurrentUnit);
//                    RingCorTOscHOB[NewestPointer(CurrentUnit)][CurrentUnit] = RingCorTOscHOB[NewestPointer(CurrentUnit)][CurrentUnit] + 1;
//                    RingCorTOscHOB[NewestPointer(CurrentUnit)][CurrentUnit] = RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] + 1;
                    RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] = RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit] + 1;
                    //                    RingCorFullTime[NewestPointer(CurrentUnit)][CurrentUnit] = 65536.0 * (double)(RingCorTOscHOB[NewestPointer(CurrentUnit)][CurrentUnit]) + RingRawTOscLOB[NewestPointer(CurrentUnit)][CurrentUnit];
                    //                    RingCorFullTime[NewestPointer(CurrentUnit)][CurrentUnit] = 65536.0 * (double)(RingCorTOscHOB[NewestPointer(CurrentUnit)][CurrentUnit]) + RingRawTOscLOB[NewestPointer[CurrentUnit]][CurrentUnit];
                    //                    RingCorFullTime[NewestPointer(CurrentUnit)][CurrentUnit] = 65536.0 * (double)(RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit]) + RingRawTOscLOB[NewestPointer[CurrentUnit]][CurrentUnit];
                    RingCorFullTime[NewestPointer[CurrentUnit]][CurrentUnit] = 65536.0 * (double)(RingCorTOscHOB[NewestPointer[CurrentUnit]][CurrentUnit]) + RingRawTOscLOB[NewestPointer[CurrentUnit]][CurrentUnit];
                }
            }
            //            if (MarkerSync(CurrentUnit))
            if (MarkerSync[CurrentUnit])
            {
                // Calculate the GPS Time
//                RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = MarkerBase(CurrentUnit) + MarkerPeriod(CurrentUnit) * RingCorFullTime(NewestPointer(CurrentUnit), CurrentUnit);
//                RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = MarkerBase(CurrentUnit) + MarkerPeriod(CurrentUnit) * RingCorFullTime(NewestPointer[CurrentUnit)][CurrentUnit];
//                RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = MarkerBase(CurrentUnit) + MarkerPeriod[CurrentUnit] * RingCorFullTime(NewestPointer[CurrentUnit)][CurrentUnit];
//                RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = MarkerBase[CurrentUnit] + MarkerPeriod[CurrentUnit] * RingCorFullTime(NewestPointer[CurrentUnit)][CurrentUnit];
                RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit] = MarkerBase[CurrentUnit] + MarkerPeriod[CurrentUnit] * RingCorFullTime[NewestPointer[CurrentUnit]][CurrentUnit];
                // Do a final sanity check
//                if (RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) < LastGoodGPSTime(CurrentUnit))
//                if (RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) < LastGoodGPSTime[CurrentUnit])
                if (RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit] < LastGoodGPSTime[CurrentUnit])
                {
                    //                    RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = -1#;
                    RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit] = -1;
                    EventIsBad = true;
                    BadInRecord = true;
                }
                else
                    //                    LastGoodGPSTime(CurrentUnit) = RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit);
                    //                    LastGoodGPSTime(CurrentUnit) = RingGPSTime(NewestPointer[CurrentUnit)][CurrentUnit];
                {
                    LastGoodGPSTime[CurrentUnit] = RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit];
                }
            }
            if (EventIsBad)
            {
                //                RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit) = -1#;
                RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit] = -1;
                //                BadGPSCount(CurrentUnit) = BadGPSCount(CurrentUnit) + 1;
                //                BadGPSCount(CurrentUnit) = BadGPSCount[CurrentUnit] + 1;
                BadGPSCount[CurrentUnit]++;
                //ABSString = ABSString + " Bad ";
                ABSString += " Bad ";
            }
            //            RingInterval(NewestPointer(CurrentUnit), CurrentUnit) = RingCorFullTime(NewestPointer(CurrentUnit), CurrentUnit) - RingCorFullTime(OffsetNewest(CurrentUnit, -1), CurrentUnit);
            //            RingInterval(NewestPointer(CurrentUnit), CurrentUnit) = RingCorFullTime(NewestPointer[CurrentUnit)][CurrentUnit] - RingCorFullTime(OffsetNewest(CurrentUnit, -1), CurrentUnit);
            RingInterval[NewestPointer[CurrentUnit]][CurrentUnit] -= RingCorFullTime[OffsetNewest(CurrentUnit, -1)][CurrentUnit];
            //            ABSString = ABSString + " " + Format(RingInterval(NewestPointer(CurrentUnit), CurrentUnit)) + " " + Format(RingRawFullTime(NewestPointer(CurrentUnit), CurrentUnit));
            //            ABSString = ABSString + " " + Format(RingInterval(NewestPointer(CurrentUnit), CurrentUnit)) + " " + Format(RingRawFullTime(NewestPointer[CurrentUnit)][CurrentUnit]);
            char tempRing[50];
            snprintf(tempRing, 49,  " %d %d", RingInterval[NewestPointer[CurrentUnit]][CurrentUnit], RingRawFullTime[NewestPointer[CurrentUnit]][CurrentUnit]);
            ABSString += tempRing;
            //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                //                frmREMOTE.lblTime1(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTime1(iFor).Caption = Format(RingInterval(NewestPointer(CurrentUnit)][CurrentUnit]); //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Visible = true; //TODO connect to wx with event
                //                frmREMOTE.lblTrueTime(iFor).Caption = Format(RingRawFullTime(NewestPointer(CurrentUnit), CurrentUnit)); //TODO connect to wx with event
            }
            // Do the multiplicity accounting
            //frmMultiplicity.Calculate(RingInterval(NewestPointer(CurrentUnit), CurrentUnit), RingSelect(NewestPointer(CurrentUnit), CurrentUnit)); //TODO frmMultiplicity
            // Enter the elapsed time into the proper histograms
//            CurValue = System.Convert.ToSingle(RingInterval(NewestPointer(CurrentUnit), CurrentUnit));
            CurValue = (float)(RingInterval[NewestPointer[CurrentUnit]][CurrentUnit]);
            EnterMonitorHis(CurValue, MPSHisOrigin + CurrentUnit, iDiscFire);
            EnterMonitorHis(CurValue, MPLHisOrigin + CurrentUnit, iDiscFire);
            // Add the event to the HitSpool File
            //if (RecordHitSpool & (CurrentUnit <= LastSpoolUnit))
            if (RecordHitSpool && (CurrentUnit <= LastSpoolUnit))
            {
                //                HitSpoolRecord = Right("                      " + Format(RingGPSTime(NewestPointer(CurrentUnit), CurrentUnit), "0.000000"), 12);
                HitSpoolRecord = "                      " + std::to_string(RingGPSTime[NewestPointer[CurrentUnit]][CurrentUnit]);
                //TODO insert leading spaces
//                HitSpoolRecord = HitSpoolRecord + Right("    " + Format(RingPHA(NewestPointer(CurrentUnit), CurrentUnit)), 4);
                HitSpoolRecord += "    " + std::to_string(RingPHA[NewestPointer[CurrentUnit]][CurrentUnit]);
                //TODO insert leading spaces
//                HitSpoolRecor+d + " " + Left(Format(RingSelect(NewestPointer(CurrentUnit), CurrentUnit)), 1);
                HitSpoolRecord += " " + std::to_string(RingSelect[NewestPointer[CurrentUnit]][CurrentUnit]);
                //Print(); //TODO HitSpoolFile
            }
        }
    }
//    EventsInSecond(CurrentUnit) = EventsInSecond(CurrentUnit) + RecoveredHits;
    EventsInSecond[CurrentUnit] += RecoveredHits;
    StatHisBin = RecoveredHits;
    if (StatHisBin > RemStatHistLastChannel)
    {
        StatHisBin = RemStatHistLastChannel;
        //RemStatHis(1, StatHisBin, CurrentUnit) = RemStatHis(1, StatHisBin, CurrentUnit) + 1;
        RemStatHis[1][StatHisBin][CurrentUnit]++;
    }

    //if ((chkLogDuplicates.Value == 1) & (LatestRemoteTiming(CurrentUnit, CopyIndex) != ""))
    if ((chkLogDuplicates_Value == 1) && (LatestRemoteTiming[CurrentUnit][CopyIndex] != ""))
    {
        // Duplicate readout
        //LogEntry("Prev " + LatestRemoteTiming(CurrentUnit, CopyIndex));
        LogEntry("Prev " + LatestRemoteTiming[CurrentUnit][CopyIndex]);
        LogEntry("This " + ABSString);
    }

    //LatestRemoteTiming(CurrentUnit, CopyIndex) = ABSString;
    LatestRemoteTiming[CurrentUnit][CopyIndex] = ABSString;
    //if (OrderABSOutput | (chkRecABSTiming.Value == 1))
    if (OrderABSOutput || (chkRecABSTiming_Value == 1))
    {
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, ABSString

 */
        Process800Out += ABSString + '\n';
    }
    DecimateCounter = DecimateCounter + 1;
    if (BadInRecord)
    {
        //if ((chkLogBadEvents.Value == 1) | ((DecimateCounter % DecimateModulo) == 0))
        if ((chkLogBadEvents_Value == 1) || ((DecimateCounter % DecimateModulo) == 0))
        {
            //if ((!OrderABSOutput) & (chkRecABSTiming.Value == 0))
            if ((!OrderABSOutput) && (chkRecABSTiming_Value == 0))
            {
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, ABSString

 */
                Process800Out += ABSString + '\n';
            }
            if (CrossMultiplicityEnabled)
            {
                //GenerateTimeStamp(ReportString); //TODO GenerateTimeStamp
                //ReportString = ReportString + " Remote: " + Format(CurrentUnit);
                //ReportString = ReportString + " Base: " + Format(MarkerBase(CurrentUnit));
                //ReportString = ReportString + " Period: " + Format(MarkerPeriod(CurrentUnit));
                ReportString += " Remote: " + std::to_string(CurrentUnit);
                //                ReportString += " Base: " + std::to_string(MarkerBase(CurrentUnit));
                ReportString += " Base: " + std::to_string(MarkerBase[CurrentUnit]);
                //                ReportString += " Period: " + std::to_string(MarkerPeriod(CurrentUnit));
                ReportString += " Period: " + std::to_string(MarkerPeriod[CurrentUnit]);
                //                if (chkMinDiagnostics.Value == 0) 
                if (chkMinDiagnostics_Value == 0) //TODO update from wx
                {
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, ReportString

 */
                    //TODO print
                }
//                if (chkMinDiagnostics.Value == 0) 
                 if (chkMinDiagnostics_Value == 0) //TODO update from wx
                 {
                     /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
    at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
    at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
    at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
    at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
    at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
    at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
    at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
    at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
    at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
    at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
    at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
    at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
    at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
    at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
    at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

 Input:
 Print #2, ReportString

  */
                     Process800Out += ReportString + '\n';
                 }
 // If chkMinDiagnostics.Value = 0 Then Print #2, CommentMessage
                for (jFor = TotalHits + 2; jFor >= 0; jFor += -1)
                {
                    LocRing = OffsetNewest(CurrentUnit, -jFor);
//                    if (chkMinDiagnostics.Value == 0) 
                    if (chkMinDiagnostics_Value == 0) //TODO update from wx
                    {
                        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, HistoryLine(LocRing, CurrentUnit)

 */
                        Process800Out += HistoryLine(LocRing, CurrentUnit) + '\n';  
                    }
                }
            }
            DecimateCounter = 0;
        }
    }

    if (Sequence == 0)
//        ABSStringRing(GPSRingPointer, CurrentUnit) = ABSString;
    {
        ABSStringRing[GPSRingPointer][CurrentUnit] = ABSString;
    }
    else
//        ABSStringRing(GPSRingPointer, CurrentUnit) = ABSStringRing(GPSRingPointer, CurrentUnit) + " " + ABSString;
//        ABSStringRing(GPSRingPointer, CurrentUnit) = ABSStringRing[GPSRingPointer][CurrentUnit] + " " + ABSString;
    {
        ABSStringRing[GPSRingPointer][CurrentUnit] = ABSStringRing[GPSRingPointer][CurrentUnit] + " " + ABSString;
    }
    if ((TotalHits == 0))
//        EmptyRemoteCount(CurrentUnit) = EmptyRemoteCount(CurrentUnit) + 1;
    {
        EmptyRemoteCount[CurrentUnit]++;
    }
//    if ((TotalHits == 0) & (chkLogZeroRemoteHits.Value == 1) & ValidCrossUnit(CurrentUnit))
    if ((TotalHits == 0) && (chkLogZeroRemoteHits_Value == 1) && ValidCrossUnit[CurrentUnit])// TODO connect with wx event
    {
        // This should almost never happen for a non-bare unit
//        if (ZeroABSString(CurrentUnit))
        if (ZeroABSString[CurrentUnit])
        {
            // Continuation, so no pre-history
//            if (chkMinDiagnostics.Value == 0) 
            if (chkMinDiagnostics_Value == 0) //TODO update from wx
            {
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Xmit 0 Cont : " & ABSStringRing(GPSRingPointer, CurrentUnit)

 */
                Process800Out += "Xmit 0 Cont : " + ABSStringRing[GPSRingPointer][CurrentUnit] + '\n';
            }
        }
        else
        {
            for (iFor = -1; iFor <= 0; iFor++)
            {
                iPoint = (GPSRingMod + GPSRingPointer + iFor) % GPSRingMod;
//                if (chkMinDiagnostics.Value == 0) 
                if (chkMinDiagnostics_Value == 0) //TODO update from wx
                {
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Xmit 0 Start: " & ABSStringRing(iPoint, CurrentUnit)

 */
                    Process800Out += "Xmit 0 Start: " + ABSStringRing[iPoint][CurrentUnit] + '\n';
                }
            }
//            ZeroABSString(CurrentUnit) = true;
            ZeroABSString[CurrentUnit] = true;
        }
    }
//    else if (ZeroABSString(CurrentUnit))
    else if (ZeroABSString[CurrentUnit])
    {
//        if (chkMinDiagnostics.Value == 0) 
        if (chkMinDiagnostics_Value == 0) //TODO update from wx
        {
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Xmit 0 End  : " & ABSStringRing(GPSRingPointer, CurrentUnit)

 */
            Process800Out += "Xmit 0 End  : " + ABSStringRing[GPSRingPointer][CurrentUnit] + '\n';
        }
//        ZeroABSString(CurrentUnit) = false;
        ZeroABSString[CurrentUnit] = false;
    }
    else
    {
        //        ZeroABSString(CurrentUnit) = false;
        ZeroABSString[CurrentUnit] = false;
    }

    ABSString = "Null ABSString";
//    PrevTimingLost(CurrentUnit) = TimingLost(CurrentUnit);
//    PrevTimingLost(CurrentUnit) = TimingLost[CurrentUnit];
    PrevTimingLost[CurrentUnit] = TimingLost[CurrentUnit];
    if (Process800Out.size() > 0)
    {
        wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
        MinuteFile << Process800Out << std::flush;
        Process800Out.clear();
    }
}


void LandMonitorApp::DecomLongTube(std::string LongTubeRecord)
{
    std::string ABSString;
    std::string HexString;
    bool SciData;
    bool DiscFire;
    int iDiscFire;
    long ScalerCount;
    long CurPls;
    long ThisTime;
    double SyncFullTime;
    double HouseStartTime;
    double HouseEndTime;
    float CurValue;
    //long[] Time1 = new long[48];
    long Time1[48];
    int I;
    int iFor;
    int jFor;
    int kFor;
    int LocRing;
    int DatFlag;
    int EvSent;
    int EvTotal;
    int EvActual;
    int MaxEvent;
    int CurHist;
    int StatHisBin;
    int nIgnore;
    int nLookBackAvailable;
    double PrelimGPSTime;
    bool EventIsBad;
    int CorrectionPointer;
    int PrePointer;
    std::string HitSpoolRecord;
    long TempLong;
    int Adr;
    int Flg;
    std::string LongTubeOut;
    /* Cannot convert OnErrorGoToStatementSyntax, CONVERSION ERROR: Conversion for OnErrorGoToLabelStatement not implemented, please report this issue in 'On Error GoTo LongTubeError' at character 910
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitOnErrorGoToStatement(OnErrorGoToStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.OnErrorGoToStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
On Error GoTo LongTubeError

 */
    AnyRemoteSeen = true;
    if (VerboseDiagnostics)
    {
        //HexString = Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 1))), 2);
        //HexString = HexString + " " + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 2))), 2);
        //HexString = HexString + " " + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 3))), 2);
        //HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 4))), 2);
        //HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 5))), 2);
        //HexString = HexString + " " + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 6))), 2);
        //HexString = HexString + " " + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, 7))), 2);
        //HexString = HexString + " ";
        //for (iFor = 8; iFor <= Strings.Len(LongTubeRecord); iFor++)
        //    HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(LongTubeRecord, iFor))), 2);
        for (iFor = 0; iFor < LongTubeRecord.size(); iFor++)
        {
            if ((1 == iFor) || (2 == iFor) || (5 == iFor) || (6 == iFor) || (7 == iFor))
                HexString.push_back(' ');
            uint8_t tempNib = (uint8_t) LongTubeRecord[iFor] >> 4;
            HexString.push_back(hexCharLUT[tempNib]);
            tempNib = LongTubeRecord[iFor] & 0x0f;
            HexString.push_back(hexCharLUT[tempNib]);
        }
        ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  Print #2, "DecomLongTube: " & HexString

 */
        LongTubeOut += "DecomLongTube: " + HexString + '\n';
    }
    // The first five bytes are independent of firmware version
    // Master Board Number
    //MasterNumber = Strings.Asc(LongTubeRecord) & 7;
    //MasterNumber = std::stoi(LongTubeRecord.substr(0,1).c_str(), nullptr, 10);
    MasterNumber = (uint8_t)LongTubeRecord[0];
    Sequence = MasterNumber & 48; // both masked in the same convereted byte
    MasterNumber &= 7;
    if ((MasterNumber < 0) || (MasterNumber > LastMaster))
    {
        //Exception("Bad Master Number " + Format(MasterNumber));
        Exception("Bad Master Number " + MasterNumber);
        return;
    }
    // Multiple readouts per second are possible
    //Sequence = Strings.Asc(LongTubeRecord) & 48;
    //Sequence = Sequence / (double)16;
    Sequence >>= 4;
    // Distinguish between first copy and all later copies
    if (Sequence == 0)
        CopyIndex = 0;
    else
        CopyIndex = 1;
    if (VerboseDiagnostics)
    {
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Sequence " & Format$(Sequence)

 */// Remote Unit Address
        LongTubeOut += ("Sequence " + std::to_string(Sequence) + '\n');
    }

    //RemoteInMaster = Strings.Asc(Mid(LongTubeRecord, 2)) & 15;
    RemoteInMaster = (uint8_t)LongTubeRecord[1] & 15;


    //if ((RemoteInMaster < 0) | (RemoteInMaster > LastRemote(MasterNumber)))
    if ((RemoteInMaster < 0) || (RemoteInMaster > LastRemote[MasterNumber]))
    {
        //Exception("Bad Remote Number " + Format(MasterNumber) + " " + Format(RemoteInMaster));
        Exception("Bad Remote Number " + std::to_string(MasterNumber) + " " + std::to_string(RemoteInMaster));
        return;
    }

    //CurrentUnit = RemoteBaseAddress(MasterNumber) + RemoteInMaster;
    CurrentUnit = RemoteBaseAddress[MasterNumber] + RemoteInMaster;
    if (Sequence == 0)
    {
        //RecordCount(CurrentUnit + 3) = RecordCount(CurrentUnit + 3) + 1;
        RecordCount[CurrentUnit + 3] = RecordCount[CurrentUnit + 3] + 1;
    }
    else
    {
        //RecordCount(CurrentUnit + LastUnit + 4) = RecordCount(CurrentUnit + LastUnit + 4) + 1;
        RecordCount[CurrentUnit + LastUnit + 4] = RecordCount[CurrentUnit + LastUnit + 4] + 1;
    }
    if (Sequence == 0)
    {
        // Track the distribution of events per second
        //StatHisBin = EventsInSecond(CurrentUnit);
        StatHisBin = EventsInSecond[CurrentUnit];
        if (StatHisBin > RemStatHistLastChannel)
            StatHisBin = RemStatHistLastChannel;
        //RemStatHis(2, StatHisBin, CurrentUnit) = RemStatHis(2, StatHisBin, CurrentUnit) + 1;
        (RemStatHis[2][StatHisBin][CurrentUnit])++;
        //EventsInSecond(CurrentUnit) = 0;
        EventsInSecond[CurrentUnit] = 0;
        // This is the first (and possibly only) readout for this second
        // Get the non-resetting counting scaler and compute the "deltas"
        //ScalerCount = Strings.Asc(Mid(LongTubeRecord, 3));
        //ScalerCount = (256 * ScalerCount) + Strings.Asc(Mid(LongTubeRecord, 4));
        //ScalerCount = (256 * ScalerCount) + Strings.Asc(Mid(LongTubeRecord, 5));
        //ScalerCount = (uint8_t)LongTubeRecord[3];
        ScalerCount = (uint8_t)LongTubeRecord[2];
        ScalerCount <<= 8;
        //ScalerCount += (uint8_t)LongTubeRecord[4];
        ScalerCount += (uint8_t)LongTubeRecord[3];
        ScalerCount <<= 8;
        //ScalerCount += (uint8_t)LongTubeRecord[5];
        ScalerCount += (uint8_t)LongTubeRecord[4];
        if (VerboseDiagnostics)
        {
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Scaler Count " & Format$(ScalerCount)

 */
            LongTubeOut += "Scaler Count ";
            LongTubeOut += std::to_string(ScalerCount);
            LongTubeOut.push_back('\n');
        }

        //Deltas(CurrentUnit) = ScalerCount - Counts(CurrentUnit);
        //if (Deltas(CurrentUnit) < 0)
        //    Deltas(CurrentUnit) = Deltas(CurrentUnit) + DelCycle;
        //Counts(CurrentUnit) = ScalerCount;
        Deltas[CurrentUnit] = ScalerCount - Counts[CurrentUnit];
        if (Deltas[CurrentUnit] < 0)
            Deltas[CurrentUnit] += DelCycle;
        Counts[CurrentUnit] = ScalerCount;
        //        lblCts(CurrentUnit).Caption = Format(Counts(CurrentUnit)); //TODO connect to wx with event        
        //        lblDel(CurrentUnit).Caption = Format(Deltas(CurrentUnit)); //TODO connect to wx with event        
                //if (NotSeenYet(CurrentUnit))
                //{
                //    ResetDeltaRef(CurrentUnit) = ScalerCount;
                //    NotSeenYet(CurrentUnit) = false;
                //}        
        if (NotSeenYet[CurrentUnit])
        {
            ResetDeltaRef[CurrentUnit] = ScalerCount;
            NotSeenYet[CurrentUnit] = false;
        }
        //        lblHourDel(CurrentUnit).Caption = Format(ScalerCount - ResetDeltaRef(CurrentUnit)); //TODO connect to wx with event    }
    }
    //if (FirmVersion(CurrentUnit) == 700)
    if (FirmVersion[CurrentUnit] == 700)
    {
        // Firmware version 700 code copied from Version 8.41
        // This is a multiplicity enabled unit with extended event buffers
        // Science and housekeeping data formats are quite different
        // Housekeeping format is identical to that of the 600 series
        // and is handled by the "DecomShortTube" routine
        // Event format uses a longer record that is handled by this "DecomLongTube" routine
        //DatFlag = (Strings.Asc(Mid(LongTubeRecord, 2))) / 16;
        DatFlag = (uint8_t)LongTubeRecord[2] / 16;
//        frmHouse.lblRemStatus1(CurrentUnit).Caption = DatFlag; //TODO connect to wx with event
        SciData = (DatFlag & 1) == 1;
        if (SciData)
        {
            // Format as science data
//            frmHouse.lblRemStatus2(CurrentUnit).Caption = "Science"; //TODO connect to wx with event            
            //EvSent = Strings.Asc(Mid(LongTubeRecord, 6));
            //EvTotal = Strings.Asc(Mid(LongTubeRecord, 7)); 
            EvSent = (uint8_t)LongTubeRecord[6];
            EvTotal = (uint8_t)LongTubeRecord[7];
            //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
//                frmREMOTE.lblSent.Caption = Format(EvSent); //TODO connect to wx with event                
//                frmREMOTE.lblTotal.Caption = Format(EvTotal); //TODO connect to wx with event
                //for (iFor = 0; iFor <= nEv700 - 1; iFor++)
//                    frmREMOTE.lblTrueTime(iFor).Visible = false; //TODO connect to wx with event
            }
            MaxEvent = EvSent;
            if (EvSent > 0)
            {
                // Format errors of various kinds can make the
                // number of events sent in error, so limit it
                // to avoid array overflow problems, etc.
                if (MaxEvent > nEv700)
                    MaxEvent = nEv700;
                // Pulse Height Data
                //for (iFor = 0; iFor <= MaxEvent - 1; iFor++)
                for (iFor = 0; iFor < MaxEvent - 1; iFor++)
                {
                    //DiscFire = (Strings.Asc(Mid(LongTubeRecord, iFor + 8)) & 128) != 0;
                    //CurPls = (Strings.Asc(Mid(LongTubeRecord, iFor + 8)) & 127);
                    //Time1[iFor] = Asc(Mid(LongTubeRecord, iFor + nEv700 + 8));
                    //Time1[iFor] = 256 * Time1[iFor] + Asc(Mid(LongTubeRecord, iFor + 2 * nEv700 + 8));
//                    DiscFire = ((uint8_t)LongTubeRecord[iFor + 8] & 128) != 0;
                    DiscFire = ((uint8_t)LongTubeRecord[iFor + 7] & 128) != 0;
//                    CurPls = ((uint8_t)LongTubeRecord[iFor + 8]) & 127;
                    CurPls = ((uint8_t)LongTubeRecord[iFor + 7]) & 127;
//                    Time1[iFor] = (uint8_t)LongTubeRecord[iFor + nEv700 + 8];
                    Time1[iFor] = (uint8_t)LongTubeRecord[iFor + nEv700 + 7];
                    Time1[iFor] <<= 8;
//                    Time1[iFor] += (uint8_t)LongTubeRecord[iFor + 2 * nEv700 + 8];
                    Time1[iFor] += (uint8_t)LongTubeRecord[iFor + 2 * nEv700 + 7];
                    if ((CurrentUnit <= LastUnit))
                    {
                        //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
                        if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
                        {
//                            frmREMOTE.lblPHA(iFor).Visible = true; //TODO connect to wx with event
//                            frmREMOTE.lblTime1(iFor).Visible = true; //TODO connect to wx with event
//                            frmREMOTE.lblPHA(iFor).Caption = Strings.Format(CurPls); //TODO connect to wx with event
                            if (!DiscFire)
                            { 
//                                frmREMOTE.lblPHA(iFor).BackColor = 0xFFFF &; //TODO connect to wx with event
                            }
                            else
                            {
                                //                                frmREMOTE.lblPHA(iFor).BackColor = 0xFFFFFF; //TODO connect to wx with event
                            }
//                            frmREMOTE.lblTime1(iFor).Caption = Strings.Format(Time1[iFor]); //TODO connect to wx with event
                        }
                        // Keep track of where the hits end up
                        //HitTrack(0, CurrentUnit) = HitTrack(0, CurrentUnit) + 1;
                        (HitTrack[0][CurrentUnit])++;
                        // First bump the relevant counters
                        //if (CurPls < MonitorLimit(WMin, CurrentUnit))
                        //    Below(CurrentUnit) = Below(CurrentUnit) + 1;
                        //else if (CurPls > MonitorLimit(WMax, CurrentUnit))
                        //    Above(CurrentUnit) = Above(CurrentUnit) + 1;
                        //else
                        //    InArray(CurrentUnit) = InArray(CurrentUnit) + 1;
                        if (CurPls < MonitorLimit[WMin][CurrentUnit])
                            Below[CurrentUnit]++;
                        else if (CurPls > MonitorLimit[WMax][CurrentUnit])
                            Above[CurrentUnit]++;
                        else
                            InArray[CurrentUnit]++;
                        // Do the multiplicity accounting
                        // Note that the remote address is a global variable
                        //frmMultiplicity.Calculate(System.Convert.ToDouble(Time1[I]), DiscFire); //TODO convert frmMultiplicity
                        // Enter the PH and elapsed time into the proper histograms
                        if (!DiscFire)
                        {
                            CurHist = CurrentUnit;
                            CurValue = CurPls;
                            EnterMonitorHis(CurValue, CurHist, 0);
                            CurValue = Time1[iFor];
                            CurHist = (LastUnit + 1) + CurrentUnit;
                            EnterMonitorHis(CurValue, CurHist, 0);
                            CurHist = 2 * (LastUnit + 1) + CurrentUnit;
                            EnterMonitorHis(CurValue, CurHist, 0);
                        }
                        else
                        {
                            CurHist = CurrentUnit;
                            CurValue = CurPls;
                            EnterMonitorHis(CurValue, CurHist, 1);
                            CurValue = Time1[iFor];
                            CurHist = (LastUnit + 1) + CurrentUnit;
                            EnterMonitorHis(CurValue, CurHist, 1);
                            CurHist = 2 * (LastUnit + 1) + CurrentUnit;
                            EnterMonitorHis(CurValue, CurHist, 1);
                        }
                    }
                }
                //if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
                if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
                {
                    if (MaxEvent < nEv700)
                    {
                        for (iFor = MaxEvent; iFor <= nEv700 - 1; iFor++)
                        {
//                            frmREMOTE.lblPHA(iFor).Visible = false;
 //TODO connect to wx with event
//                            frmREMOTE.lblTime1(iFor).Visible = false;
 //TODO connect to wx with event
                        }
                    }
                }
            }
            //else if (ShowSelectedRemote & (CurrentUnit == WatchThisRemote))
            else if (ShowSelectedRemote && (CurrentUnit == WatchThisRemote))
            {
                for (iFor = 0; iFor <= nEv700 - 1; iFor++)
                {
//                    frmREMOTE.lblPHA(iFor).Visible = false;
 //TODO connect to wx with event
//                    frmREMOTE.lblTime1(iFor).Visible = false;
 //TODO connect to wx with event
                }
            }
        }
        else
            Exception("Housekeeping data in long readout for 700 series");
    }
    //else if (FirmVersion(CurrentUnit) == 800)
    //    Process800(LongTubeRecord);
    //else if (FirmVersion(CurrentUnit) == 900)
    //    frmNewElectronics.Remote500Long(LongTubeRecord);
    else if (FirmVersion[CurrentUnit] == 800)
    {
        //LongTubeOut += "800tube \n";
        if (LongTubeOut.size() > 0)
        {
            wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
            MinuteFile << LongTubeOut << std::flush;
            LongTubeOut.clear();
        }
        Process800(LongTubeRecord); 
    }
    else if (FirmVersion[CurrentUnit] == 900)
    {
        //frmNewElectronics.Remote500Long(LongTubeRecord);  //TODO process other tubes
    }
    
    if (LongTubeOut.size() > 0)
    {
        wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
        MinuteFile << LongTubeOut << std::flush;
        LongTubeOut.clear();
    }
    return;
//LongTubeError: //TODO try catch to hndle errors
//    ;
//    nLongTubeInternalErr = nLongTubeInternalErr + 1#;
//    if (frmMonitor.chkMinDiagnostics.Value == 0)
//        Exception("Long Tube Processing Internal Error");
//    return;
}

void LandMonitorApp::Timer1_Timer()
{
    // Many operations are controlled by the computer clock,
    // however it sometimes misses a beat. Therefore there is
    // a provision to execute some operations whenever a sync
    // appears in the datastream from the GPS. If for some reason
    // this does not appear, then the computer clock is used.
    // All of this is contained in the routine "LogicalSecond"
    // Dim CurHist As Integer
    // Dim CurValue As Single
    int ErrorCode;
    int PrevMin;
    int PrevHour;
    double Temp;
    int I;
    int II;
    int J;
    int iDq;
    int nUnit;
    int nMult;
    int nControl;
    bool NewDay;
    int iFor;
    int jFor;

    
    wxCriticalSectionLocker csTime(wxGetApp().csTimer1); //mutex to prevent Timer1_Timer & RackData running at same time
    auto nowTimer1 = std::chrono::system_clock::now();
    std::chrono::duration<double> sec = nowTimer1 - lastTimer1;
    std::cout << sec.count() << '\n';
    lastTimer1 = nowTimer1;
    monitorF->

    if (FreshException > 0)
    {
        if (MinuteFileAvailable)
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "Total: " & Format$(FreshException) & " Exceptions"

 */
        MinuteFile << "Total: " << std::to_string(FreshException) << " Exceptions";
        FreshException = 0;
    }
    // Check to see that at least some remote units are reporting
    if (AnyRemoteSeen)
    {
        MiRead = MiRead + 1;
        AnyRemoteSeen = false;
    }

    if (OrderTimingReset)
        ResetTiming();
    // Show the current time and parse it for control
    //CurTime = DateTime.Now;
    //CurTimTag = Format(CurTime, "hh:mm:ss");
    //CurDayTimTag = Format(CurTime, "yy/mm/dd hh:mm:ss");
    //CurYear = Val(Format(CurTime, "yyyy"));
    //CurMonth = Val(Format(CurTime, "mm"));
    //CurDay = Val(Format(CurTime, "dd"));
    //CurHour = Val(Format(CurTime, "hh"));
    //CurMin = Val(Format(CurTime, "nn"));
    //CurSec = Val(Format(CurTime, "ss"));
    //CurDOY = DayOfYear(CurMonth, CurDay, CurYear);
    char fCurTime[20];
    std::time(&CurTime);
    tm* tmCurTime = std::localtime(&CurTime);
    strftime(fCurTime, std::size(fCurTime), "%T", tmCurTime);
    CurTimTag = fCurTime;
    strftime(fCurTime, std::size(fCurTime), "%y/%m/%d %T", tmCurTime);
    CurDayTimTag = fCurTime;
    CurYear = tmCurTime->tm_year + 1900;
    CurMonth = tmCurTime->tm_mon + 1;
    CurDay = tmCurTime->tm_mday;
    CurHour = tmCurTime->tm_hour;
    CurMin = tmCurTime->tm_min;
    CurSec = tmCurTime->tm_sec;
    CurDOY = tmCurTime->tm_yday + 1;
//    lblComputerTime.Caption.Caption //TODO connect to wx
    // Check for a date change
    strftime(fCurTime, std::size(fCurTime), "%y_%m_%d", tmCurTime);
    CurDate = fCurTime;
    //if (CurDate != Format(CurTime, "yy_mm_dd"))
    if (0 == CurDate.compare(fCurTime))
    {
        // Switch data files
        //CurDate = Format(CurTime, "yy_mm_dd");
        CurDate = fCurTime;
        //Close(); MinuteFileAvailable = false;
        MinuteFile.close();
        MinuteFileAvailable = false;
        MinuteFileName = MinuteDirectory + StationID + "_" + CurDate + ".DAT";
        if (0 == stat(MinuteFileName.c_str(), &fso))
        {
            MinuteFile.open(MinuteFileName, std::fstream::app | std::fstream::ate | std::fstream::out); //open for append -B
            MinuteFileAvailable = true;
        }
        else
        {
            MinuteFile.open(MinuteFileName, std::fstream::out);
            MinuteFileAvailable = true;
        }
        //if (fso.FileExists(MinuteFileName))
        //{
        //    Open(MinuteFileName); MinuteFileAvailable = true;
        //}
        //else
        //{
        //    Open(MinuteFileName); MinuteFileAvailable = true;
        //    ;
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2, "From: LandBase"

 */
            //;
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2, "To: Cosray"

 */
            //;
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2, "Subject: " & MinuteFileName

 */
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2,
    If StationNoteExists Then Print #2, "Note: " & StationNote

 */
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2, "Date Change: " + lblComputerTime.Caption

 */
            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #2, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
        //}
        //if (BareTrapEnabled) //TODO BareTrap file
        //{
        //    Close(); BareTrapFileName = BareTrapDirectory + StationID + "_" + CurDate + ".BareTrap";
        //    if (fso.FileExists(BareTrapFileName))
        //        Open(BareTrapFileName);
        //    else
        //    {
        //        Open(BareTrapFileName);
        //        ;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #97, "From: LandBase"

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #97, "To: Cosray"

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #97, "Subject: " & BareTrapFileName

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #97,
      If StationNoteExists Then Print #97, "Note: " & StationNote

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #97, "Date Change: " + lblComputerTime.Caption

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #97, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
        //    }
        //}
        // New names for HitSpool files (that may never be opened)
        //for (iFor = 0; iFor <= AbsoluteLastUnit; iFor++) //TODO HitFile
        //{
        //    HitFileName(iFor) = HitSpoolDirectory + StationID;
        //    HitFileName(iFor) = HitFileName(iFor);
        //    HitFileName(iFor) = HitFileName(iFor) + "_" + CurDate + "_U" + Right(Format(iFor + 100), 2) + ".HitSpool";
        //}
        // Switch HitSpool if operating
//        if (RecordHitSpool)
//        {
//            for (iFor = 0; iFor <= LastSpoolUnit; iFor++)
//                Close();
//            RecordHitSpool = false;
//            SpoolStartOK = false;
//            StartHitSpool = true;
//            SpoolUnitsOpen = 0;
////            frmTransmitData.lblRecordHitSpool.Caption.Caption //TODO connect to wx
////            frmTransmitData.lblRecordHitSpool.BackColor.BackColor //TODO connect to wx
////            frmTransmitData.lblSpoolUnitsOpen.Caption.Caption //TODO connect to wx
//        }
        //DocumentState();
        nBigCatch = 0;
//        if (CrossMultiplicityEnabled) //TODO CatchFile
//        {
////            Close(); frmClocker.lblNumberCaught.Caption.Caption //TODO connect to wx
//            LastBigCatchTime = 0#;
//            LastCatchTime = 0#;
//            CatchFileName = CatchDirectory + StationID + "_" + CurDate + ".Catch";
//            if (fso.FileExists(CatchFileName))
//                Open(CatchFileName);
//            else
//            {
//                Open(CatchFileName);
//                ;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #98, "From: LandBase"

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #98, "To: Cosray"

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #98, "Subject: " & CatchFileName

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #98,
      If StationNoteExists Then Print #98, "Station Note - " & StationNote

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #98, "Date Change: " + lblComputerTime.Caption

 */
                //;
                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
      Print #98, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
        //    }
        //};
        /* Cannot convert OnErrorGoToStatementSyntax, CONVERSION ERROR: Conversion for OnErrorGoToLabelStatement not implemented, please report this issue in 'On Error GoTo NoStick' at character 4438
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitOnErrorGoToStatement(OnErrorGoToStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.OnErrorGoToStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  On Error GoTo NoStick

 */
    //    if (UseMemStick) //TODO StickFile
    //    {
    //        Close(); StickFileName = MemStickDrive + @":\" + StationID + "_" + CurDate + ".DAT";
    //            Open(StickFileName);
    //    }

    //NoStick:
    //    ;
    //    ;
        /* Cannot convert OnErrorGoToStatementSyntax, CONVERSION ERROR: Conversion for OnErrorGoToZeroStatement not implemented, please report this issue in 'On Error GoTo 0' at character 4633
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitOnErrorGoToStatement(OnErrorGoToStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.OnErrorGoToStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:

  On Error GoTo 0

 */
//        LogEntry("Date Change: " + lblComputerTime.Caption.Caption //TODO connect to wx
        //LogEntry("Program: " + lblSoftVersion + " Modified at: " + lblSoftLocation + " ");
        // Update the current file name and
        // add an entry to the Hour File
        // with housekeeping
        //HourFileName = HourDirectory + StationID + "_" + CurDate + ".LOG"; //TODO HourFile
        //if (fso.FileExists(HourFileName))
        //    Open(HourFileName);
        //else
        //{
        //    Open(HourFileName);
        //    ;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #3, "From: LandBase"

 */
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #3, "To: Cosray"

 */
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #3, "Subject: " & HourFileName

 */
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #3,
    If StationNoteExists Then Print #3, "Note: " & StationNote

 */
        //};
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  Print #3, "Date Change: " + lblComputerTime.Caption

 */
        //;
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  Print #3, "Program: " & lblSoftVersion & " Modified at: " & lblSoftLocation & " "

 */
        //Close();   // Generate a name for the general histogram file
        // This is used for dumping specific histograms in real time
        // The daily and hourly histogram dumps generate their own file names
        //HistogramFileName = HistogramDirectory + StationID + "_" + CurDate + ".HIS"; //TODO 
        // Allow deferral of some new day actions.
        NewDay = true;
    }
    // Once per second
//    if (SpoolStartOK) //TODO Hitfile
//    {
//        RecordHitSpool = true;
//        StartHitSpool = false;
//        SpoolStartOK = false;
////        frmTransmitData.lblRecordHitSpool.Caption.Caption //TODO connect to wx
////        frmTransmitData.lblRecordHitSpool.BackColor.BackColor //TODO connect to wx
//    }
    // Open HitSpool files one at a time
//    if ((CurSec > 1) & StartHitSpool & (SpoolUnitsOpen < LastSpoolUnit + 1))
//    {
//        Open(HitFileName(SpoolUnitsOpen)); SpoolUnitsOpen = SpoolUnitsOpen + 1;
////        frmTransmitData.lblSpoolUnitsOpen.Caption.Caption //TODO connect to wx
//        if (SpoolUnitsOpen == LastSpoolUnit + 1)
//        {
//            SpoolStartOK = true;
//            StartHitSpool = false;
//        }
//    }
    // To minimize interference with sending SSE e-mails the daily file
    // is sent about 30 seconds after a minute boundary
    //if (ClearToSFTPDay & (CurSec > 25) & (CurSec < 35))  //TODO email
    //{
    //    if (fso.FileExists(BatSFTPDay))
    //    {
    //        LogEntry("Shelling " + BatSFTPDay);
    //        ShellRetCode = Shell(BatSFTPDay, Constants.vbMinimizedNoFocus);
    //    }
    //    ClearToSFTPDay = false;
    //}

    //if (ClearToSendDayMinute & (CurSec > 25) & (CurSec < 35))
    //{
    //    if (fso.FileExists(VMailDirectory + "DayMinute.bat"))
    //    {
    //        LogEntry("Shelling " + VMailDirectory + "DayMinute.bat");
    //        ShellRetCode = Shell(VMailDirectory + "DayMinute.bat", Constants.vbMinimizedNoFocus);
    //    }
    //    ClearToSendDayMinute = false;
    //}

    //if (ClearToSendDayHistogram & (CurSec > 25) & (CurSec < 35))
    //{
    //    if (fso.FileExists(VMailDirectory + "DayHistogram.bat"))
    //    {
    //        LogEntry("Shelling " + VMailDirectory + "DayHistogram.bat");
    //        ShellRetCode = Shell(VMailDirectory + "DayHistogram.bat", Constants.vbMinimizedNoFocus);
    //    }
    //    ClearToSendDayHistogram = false;
    //}

    //if (ClearToSendDayHour & (CurSec > 25) & (CurSec < 35))
    //{
    //    if (fso.FileExists(VMailDirectory + "DayHour.bat"))
    //    {
    //        LogEntry("Shelling " + VMailDirectory + "DayHour.bat");
    //        ShellRetCode = Shell(VMailDirectory + "DayHour.bat", Constants.vbMinimizedNoFocus);
    //    }
    //    ClearToSendDayHour = false;
    //}

    //if (ClearToSendDayCatch & (CurSec > 25) & (CurSec < 35))
    //{
    //    if (fso.FileExists(VMailDirectory + "DayCatch.bat"))
    //    {
    //        LogEntry("Shelling " + VMailDirectory + "DayCatch.bat");
    //        ShellRetCode = Shell(VMailDirectory + "DayCatch.bat", Constants.vbMinimizedNoFocus);
    //    }
    //    ClearToSendDayCatch = false;
    //}

    //if (ClearToSFTPCatch & (CurSec > 25) & (CurSec < 35))
    //{
    //    if (fso.FileExists(BatSFTPCatch))
    //    {
    //        LogEntry("Shelling " + BatSFTPCatch);
    //        ShellRetCode = Shell(BatSFTPCatch, Constants.vbMinimizedNoFocus);
    //    }
    //    ClearToSFTPCatch = false;
    //}

//    lblSinceGPS.Caption.Caption //TODO connect to wx
    if (GPSTimeSyncEnable)
    {
        if (SinceGPS > 2)
        {
//            if (lblTimeSyncSource.Caption.Caption //TODO connect to wx
            {
//                lblTimeSyncSource.BackColor.BackColor //TODO connect to wx
//                lblTimeSyncSource.Caption.Caption //TODO connect to wx
            }
        }
//        else if (lblTimeSyncSource.Caption.Caption //TODO connect to wx
        {
//            lblTimeSyncSource.BackColor.BackColor //TODO connect to wx
//            lblTimeSyncSource.Caption.Caption //TODO connect to wx
        }
    }
    else
    {
//        lblTimeSyncSource.BackColor.BackColor //TODO connect to wx
//        lblTimeSyncSource.Caption.Caption //TODO connect to wx
    }

    if (SequenceTimeSystem)
    {
        // The system time controls the sequence time unconditionally
        SequenceSecond = CurSec;
        SequenceMinute = CurMin;
        SequenceHour = CurHour;
        //SequenceSecOfDay = 3600 * System.Convert.ToInt64(SequenceHour) + 60 * SequenceMinute + SequenceSecond;
        SequenceSecOfDay = 3600 * (uint64_t)(SequenceHour) + 60 * SequenceMinute + SequenceSecond;
//        lblSequenceTimeActual.Caption.Caption //TODO connect to wx
//        lblGPSTimeSource.Visible.Visible //TODO connect to wx
        //LogicalSecond(); //TODO LogicalSecond
    }
    else
    {
        // If there is no indication that the GPS is working, then
        // the system steps in and begins generating a sequence time.
        // Note that if the host computer is not set to GMT this will
        // cause some strange stuff. However the system always controls
        // the date change, so files will continue to be updated and the
        // records will be monotonically labelled with the system time.
//        lblGPSTimeSource.Visible.Visible //TODO connect to wx
        if (SinceGPS > 4)
        {
            SequenceSecond = CurSec;
            SequenceMinute = CurMin;
            SequenceHour = CurHour;
            //SequenceSecOfDay = 3600 * System.Convert.ToInt64(SequenceHour) + 60 * SequenceMinute + SequenceSecond;
            SequenceSecOfDay = 3600 * (uint64_t)(SequenceHour)+60 * SequenceMinute + SequenceSecond;
//            if (lblSequenceTimeActual.Caption.Caption //TODO connect to wx
                LogEntry("System overriding GPS");
//            lblSequenceTimeActual.Caption.Caption //TODO connect to wx
//            lblGPSTimeSource.Visible.Visible //TODO connect to wx
            //LogicalSecond(); //TODO LogicalSecond
        }
    }

    SinceGPS = SinceGPS + 1;
    //if (PortStallEnable & (nReadSecond < 20))
    if (PortStallEnable && (nReadSecond < 20))
    {
        if ((PortStallCount == 0))
        {
//            btnRackOnOff.Caption.Caption //TODO connect to wx
            //switch (frmMonitor.comRack.PortOpen)
           
#ifdef _WIN32  // Windows specific code
            //case false:
            if  (INVALID_HANDLE_VALUE == comRack)
            {
                Exception("Input stall -- Port Already Closed");
                //break;
            }

            //default:
            else
            {
                //frmMonitor.comRack.RThreshold = 0;
                //frmMonitor.comRack.InBufferCount = 0;
                //frmMonitor.comRack.PortOpen = false;
                Exception("Input stall -- Closing Port");
                //break;
            }
#else  // TODO Linux specific code

#endif
            
            PortStallCount = 1;
        }
        else
        {
            PortStallCount = PortStallCount + 1;
            if (PortStallCount > 5)
            {
                nOpenAttempts = nOpenAttempts + 1;
                //switch (frmMonitor.comRack.PortOpen)
#ifdef _WIN32  // Windows specific code
                //case true:
                if (INVALID_HANDLE_VALUE == comRack)
                {
                    ;
                    /* Cannot convert OnErrorGoToStatementSyntax, CONVERSION ERROR: Conversion for OnErrorGoToLabelStatement not implemented, please report this issue in 'On Error GoTo Shutsys' at character 10637
                    at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.DefaultVisit(SyntaxNode node)
                    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitOnErrorGoToStatement(OnErrorGoToStatementSyntax node)
                    at Microsoft.CodeAnalysis.VisualBasic.Syntax.OnErrorGoToStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
                    at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
                    at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
                    at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

                    Input:
                            On Error GoTo Shutsys

                        */
                        //frmMonitor.comRack.PortOpen = true;
                        //frmMonitor.comRack.InBufferCount = 0;
                        //frmMonitor.comRack.RThreshold = 1;
                    IgnoreCounter = true;
                    Exception("Stall Recovery -- Opening Port");
                    //                    btnRackOnOff.Caption.Caption //TODO connect to wx
                    //break;
                }

                //default:
                else
                {
                    Exception("Stall Recovery -- Port Already Open");
                    //                    btnRackOnOff.Caption.Caption //TODO connect to wx
                    //break;
                        
                }
#else  // TODO Linux specific code

#endif
                PortStallCount = 0;
            }
        }
    }
    else
    {
        PortStallCount = 0;
        nOpenAttempts = 0;
    }

//    lblPortStallCount.Caption.Caption //TODO connect to wx
//    lblOpenAttempts.Caption.Caption //TODO connect to wx
    if (nOpenAttempts > 3)
    {
        LogEntry("Too Many Port Open Attempts Force Restart");
//        btnReStart_Click_Click //TODO connect to wx
    }

    goto NoShutSys;
Shutsys:
    ;
    if (AutoRebootEnable)
    {
        LogEntry("Port Open error forced computer reboot!!!");
//        btnReBoot_Click_Click //TODO connect to wx
    }
    else
        LogEntry("Port Open error, reboot disabled");
NoShutSys:
    ;
    nReadSecond = 0;
//    frmHouse.lblSecRead.Caption.Caption //TODO connect to wx
    //if (AnimateMonitorSeries) //TODO Graphs
    //{
    //    GraphDest = "Monitor";
    //    frmDisplay.ShowMonitorSeries(MonitorHisNum, MonitorHisType);
    //    GraphDest = " ";
    //}

    for (nUnit = 0; nUnit <= LastUnit; nUnit++)
    {
        //if ((FirmVersion(nUnit) == 600) | (FirmVersion(nUnit) == 700) | (FirmVersion(nUnit) == 800))
        if ((FirmVersion[nUnit] == 600) || (FirmVersion[nUnit] == 700) || (FirmVersion[nUnit] == 800))
        {
            for (nMult = 0; nMult <= maxMultiplicity; nMult++)
            {
                nControl = nUnit * (maxMultiplicity + 1) + nMult;
//                frmMultiplicity.lblMultiplicitySummary(nControl).Caption.Caption //TODO connect to wx
            }
        }
    }
    // Every Minute
    //if (CurMin != PrevMin)
    //{
    //    PrevMin = CurMin;
    //    MinutesSinceNewlyLoaded = MinutesSinceNewlyLoaded + 1;
//        if ((CurHour == 0) & (CurMin == ExTime(IdxSFTPDay)) & Active(IdxSFTPDay)) //TODO Email
//        {
//            // At "ExTime(idxmailday)" minutes past midnight, release the email
//            // of the data for the previous day.
////            frmTransmitData.lblParameter(IdxMailSSE).Caption.Caption //TODO connect to wx
//            ClearToSFTPDay = true;
//        }
//        if ((CurHour == 0) & (CurMin == ExTime(IdxMailDay)) & Active(IdxMailDay))
//        {
//            // At "ExTime(idxmailday)" minutes past midnight, release the email
//            // of the data for the previous day.
////            frmTransmitData.lblParameter(IdxMailSSE).Caption.Caption //TODO connect to wx
//            ClearToSendDayMinute = true;
//        }
//        if ((CurHour == 0) & (CurMin == ExTime(IdxMailDay) + 4) & Active(IdxMailDay))
//        {
//            // At "ExTime(idxmailday)" minutes past midnight, release the email
//            // of the data for the previous day.
////            frmTransmitData.lblParameter(IdxMailSSE).Caption.Caption //TODO connect to wx
//            ClearToSendDayHistogram = true;
//        }
//        if ((CurHour == 0) & (CurMin == ExTime(IdxMailDay) + 8) & Active(IdxMailDay))
//        {
//            // At "ExTime(idxmailday)" minutes past midnight, release the email
//            // of the data for the previous day.
////            frmTransmitData.lblParameter(IdxMailSSE).Caption.Caption //TODO connect to wx
//            ClearToSendDayHour = true;
//        }
//        if ((CurHour == 1) & (CurMin == 1) & Active(IdxMailCatch))
//            // At 1 minute past 1 AM, release the email
//            // of the catch for the previous day.
//            ClearToSendDayCatch = true;
//        if ((CurHour == 1) & (CurMin == 1) & Active(IdxSFTPCatch))
//            // At 1 minute past 1 AM, release the email
//            // of the catch for the previous day.
//            ClearToSFTPCatch = true;
//        if ((CurHour == 0) & (CurMin == ExTime(IdxMailDay) + 15))
//        {
//            // Generate batch files to send the data for this day
//            Open(VMailDirectory + "DayHistogram.bat");
//            ;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #5, VMailDirectory & "vmailer.exe " & HourlyHistogramFileName & " " & MailServerName & " " & EMailDailyTo & " " & EMailDailyFrom

 */
            //Close(); 
            //Open(VMailDirectory + "DayMinute.bat");
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #5, VMailDirectory & "vmailer.exe " & MinuteFileName & " " & MailServerName & " " & EMailDailyTo & " " & EMailDailyFrom

 */
            //Close(); Open(VMailDirectory + "DayHour.bat");
            //;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #5, VMailDirectory & "vmailer.exe " & HourFileName & " " & MailServerName & " " & EMailDailyTo & " " & EMailDailyFrom

 */
            //Close();
        //}
        //if ((CurHour == 1) & (CurMin == 15))
        //{
        //    // Generate a batch file to send the catch data for this day
        //    Open(VMailDirectory + "DayCatch.bat");
        //    ;
            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
    Print #5, VMailDirectory & "vmailer.exe " & CatchFileName & " " & MailServerName & " " & EmailCatchTo & " " & EmailCatchFrom

 */
            //Close();
        //}
    //}
    // Every Hour
    //if (CurHour != PrevHour)
    //{
        // Record and clear indicated histograms
        //frmDisplay.DumpGroup(1); //TODO Graphs
        //PrevHour = CurHour;
    //}

    if (NewDay)
    {
        // Record and clear indicated histograms
        //frmDisplay.DumpGroup(2); //TODO Graphs
        // Then clear all internal statistics
//        frmDisplay.btnClearStat_Click_Click //TODO connect to wx
    }

    NewDay = false;
    csTime.~wxCriticalSectionLocker(); //end critical section and unlock

}

void LandMonitorApp::RackData()
{
    bool StillHope;
    bool NewData;
    std::string ReportString;
    //std::string DeviceData;//moved to loop
    std::string RawDataRead;
    std::string HexString;
    std::string GPSHex;
    std::string GPSString;
    std::string GPSDiagnoseString;
    std::string RdoAMessage;
    std::string RdoBMessage;
    std::string GPSB64;
    std::string CtrHex;
    std::string CtrB64;
    std::string TubeHex;
    std::string TubeB64;
    std::string ErrHex;
    std::string ErrB64;
    std::string CommandA;
    //std::string Dta;
    uint8_t Dta[1600];
    //std::string Current; //now static vector
    static std::vector<uint8_t> Current;
    //std::string SyncCandidate; //moved to loop
    int lRackBuffer;
    int iFor;
    int nUnit;
    std::string LastGGA;
    std::string Parse;
    int nCom;
    int nLen;
    int GPSSecondRead;
    int GPSMinuteRead;
    int GPSHourRead;
    std::string GPSLatRead;
    std::string GPSLonRead;
    std::string GPSGMTRead;
    int GPSQualityRead;
    int GPSNumSatRead;
    int FreshCount;
    std::string MinuteFileOut;
    VerboseDiagnostics = true; //TODO comment out
    //std::chrono::system_clock::time_point nowTimer1Sec = std::chrono::system_clock::now();
    //if (nowTimer1Sec >= nextTimer1Sec) //check if is past the min time for next sec, starts with thread start not sec change
    //{
    //    MinuteFileOut += "RackData running Timer1_Timer()";
    //    nextTimer1Sec += std::chrono::seconds(1); //increment for next sec
    //    //Timer1_Timer();// replaces Timer1 in VB which has an event every sec for timing upkeep
    //}
    // Read the Rack port
    ReadFile(comRack, &Dta, 1600, &nRead, NULL);

    wxCriticalSectionLocker csTime(wxGetApp().csTimer1); //mutex to prevent Timer1_Timer & RackData running at same time

    //Dta = comRack.Input;
    //nRead = Strings.Len(Dta);
    //if (nRead > 16000) //no longer needed as ReadFile will not return above max
    //    Exception("Long input from rack: " + Format(nRead, "0"));
    if (RecordRaw)
    {
        RawDataRead = "RAW: ";
        //RawDataRead = RawDataRead + Right("     " + Format(nRead), 5) + " ";
        RawDataRead += "     " + std::to_string(nRead) + ' ';
        //for (iFor = 1; iFor <= nRead; iFor++)
        //    RawDataRead = RawDataRead + Right("00" + Hex(Strings.Asc(Mid(Dta, iFor, 1))), 2);
        //;
        for (iFor = 0; iFor < nRead; iFor++)
        {
            //RawDataRead.push_back(hexCharLUT[Dta[iFor] >> 4]);
            //RawDataRead.push_back(hexCharLUT[Dta[iFor] & 0x0f]);
            char tempNib = Dta[iFor] >> 4;
            RawDataRead.push_back(hexCharLUT[tempNib]);
            tempNib = Dta[iFor] & 0x0f;
            RawDataRead.push_back(hexCharLUT[tempNib]);
        }
        //RawDataRead.push_back('\n');

        MinuteFileOut += RawDataRead + '\n'; //Buffer writes to file
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
  Print #2, RawDataRead

 */
    }


    //    frmHouse.lblNRead(1).Caption = Format(nRead, "0"); //TODO connect to wx with event
    nReadSecond = nReadSecond + nRead;
    nReadTotal = nReadTotal + nRead;
    //    frmHouse.lblSecRead.Caption = Format(nReadSecond, "0"); //TODO connect to wx with event
        // Report large data string
        //lRackBuffer = Strings.Len(Current);
    lRackBuffer = Current.size();
    if (lRackBuffer > 4096)
    {
        //Exception("Large Internal Rack Buffer: " + Format(lRackBuffer, "0"));
        Exception("Large Internal Rack Buffer: " + std::to_string(lRackBuffer));
        //if (CrossMultiplicityEnabled) //TODO 
        //    ;
        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
   at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
   at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
   at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
   at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
   at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
   at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
   at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
   at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
   at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Large Internal Rack Buffer: " & Format$(lRackBuffer, "0")

 */
    }
    // Add the new data to the buffer
    //if (RackInSync | (chkSyncFail.Value == 0))
    if (RackInSync)//TODO connect to wx with event
        nNoRackSync = 0;
    else
    {
        //nNoRackSync = nNoRackSync + Strings.Len(Dta);
        nNoRackSync = nNoRackSync + nRead;
        if (nNoRackSync > 300)
        {
            //            btnRackOnOff_Click(); //TODO connect to wx with event
            LogEntry("Too Many Reads Without Sync");
            nNoRackSync = 0;
            //            btnRackOnOff_Click(); //TODO connect to wx with event
        }
    }
    // Diagnostic for sync failures
    // If Not RackInSync Then
    // HexString = Format$(Len(Current)) & " "
    // For iFor = 1 To Len(Current)
    // HexString = HexString & Right$("00" & Hex$(Asc(Mid$(Current, iFor, 1))), 2)
    // Next iFor
    // Print #2, "NoSync Current Initial: " & HexString
    // HexString = Format$(Len(Dta)) & " "
    // For iFor = 1 To Len(Dta)
    // HexString = HexString & Right$("00" & Hex$(Asc(Mid$(Dta, iFor, 1))), 2)
    // Next iFor
    // Print #2, "NoSync Dta: " & HexString
    // End If
    //Current = Current + Dta;
    Current.insert(Current.end(), Dta, Dta + nRead);
    // If Not RackInSync Then
    // HexString = Format$(Len(Current)) & " "
    // For iFor = 1 To Len(Current)
    // HexString = HexString & Right$("00" & Hex$(Asc(Mid$(Current, iFor, 1))), 2)
    // Next iFor
    // Print #2, "NoSync Current Plus: " & HexString
    // End If
    NewData = true;
    StillHope = true;
    //while (StillHope & Strings.Len(Current) > RackSyncLen)
    while (StillHope && (Current.size() > RackSyncLen))
    {
        //SyncCandidate = Left(Current, 4);
        std::string SyncCandidate(Current.begin(), Current.begin() + 4);
        // Print #2, SyncCandidate, Len(Current)
        //switch (SyncCandidate)
        //{
        //    case object _ when ShortTubeSync :
        //    {
        if (0 == SyncCandidate.compare(ShortTubeSync))
        {
            //RawSyncCount(4) = RawSyncCount(4) + 1;
            RawSyncCount[3] += 1;

            //if (Strings.Len(Current) >= ShortTubeByteLen + RackSyncLen + 2)
            if (Current.size() >= ShortTubeByteLen + RackSyncLen + 2)
            {
                if (VerboseDiagnostics)
                {
                    //HexString = "";
                    HexString.clear();
                    //for (iFor = 1; iFor <= ShortTubeByteLen + RackSyncLen + 2; iFor++)
                    //    HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                    for (iFor = 0; iFor < ShortTubeByteLen + RackSyncLen + 2; iFor++)
                    {
                        char tempNib = Current[iFor] >> 4;
                        HexString.push_back(hexCharLUT[tempNib]);
                        tempNib = Current[iFor] & 0x0f;
                        HexString.push_back(hexCharLUT[tempNib]);
                    }
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "TubeSync: " & HexString

 */
                    MinuteFileOut += "TubeSync: " + HexString + '\n';
                }
                // The string is long enough, look for delimiter
                //if (Mid(Current, ShortTubeByteLen + RackSyncLen + 1, 2) == CRLF)
                if (('\r' == Current[ShortTubeByteLen + RackSyncLen]) && ('\n' == Current[ShortTubeByteLen + RackSyncLen + 1]))
                {
                    if (!RackInSync)
                    {
                        RackInSync = true;
                        CountSync(SyncCandidate);
                        if (CrossMultiplicityEnabled)
                        {
                            //                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
                            if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                            {
                                /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
              Print #98, "Reset timing: No delimiter short tube"

 */
                                OrderTimingReset = true;
                            }
                        }
                    }
                    //DeviceData = Mid(Current, RackSyncLen + 1, ShortTubeByteLen);
                    //std::string DeviceData(Current.begin() + RackSyncLen + 1, Current.begin() + ShortTubeByteLen);
                    std::string DeviceData(Current.begin() + RackSyncLen, Current.begin() + RackSyncLen + ShortTubeByteLen);
                    if (VerboseDiagnostics)
                    {
                        MinuteFileOut += "ShortTube found";
                        MinuteFileOut.push_back('\n');
                    }
                    /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "ShortTube found"

 */
 //DecomShortTube(DeviceData); //TODO
// Remove the message from the buffer
//if (Strings.Len(Current) > ShortTubeByteLen + RackSyncLen + 2)
                    if (Current.size() > (ShortTubeByteLen + RackSyncLen + 2))
                    {
                        //Current = Mid(Current, ShortTubeByteLen + RackSyncLen + 3);
                        Current.erase(Current.begin(), Current.begin() + ShortTubeByteLen + RackSyncLen + 2);
                    }
                    else
                    {
                        //Current = "";
                        Current.clear();
                    }
                }
                else
                {
                    // Although there are enough characters, there
                    // is no delimeter here. So just delete the initial
                    // character and look for sync again.
                    if (RecordFullSyncNotes)
                    {
                        HexString = "ShortTube No Delimiter ";
                        //for (iFor = 1; iFor <= ShortTubeByteLen + RackSyncLen + 2; iFor++)
                        //    HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                        for (iFor = 0; iFor < ShortTubeByteLen + RackSyncLen + 2; iFor++)
                        {
                            char tempNib = Current[iFor] >> 4;
                            HexString.push_back(hexCharLUT[tempNib]);
                            tempNib = Current[iFor] & 0x0f;
                            HexString.push_back(hexCharLUT[tempNib]);
                        }
                        Exception(HexString);
                    }
                    CountNoDelimeter(SyncCandidate);
                    //Current = Mid(Current, 2);
                    Current.erase(Current.begin());
                    nSyncSlip = nSyncSlip + 1;
                    RackInSync = false;
                }
            }
            else
            {
                // There is a proper sync, but not a long enough
                // string, so here one must just wait.
                StillHope = false;
                //break;
            }

            //break;
        }


            //case object _ when LongTubeSync :
        if (0 == SyncCandidate.compare(LongTubeSync))
        {
            //RawSyncCount(3) = RawSyncCount(3) + 1;
            RawSyncCount[2] += 1;
            //if (Strings.Len(Current) >= LongTubeByteLen + RackSyncLen + 2)
            if (Current.size() >= LongTubeByteLen + RackSyncLen + 2)
            {
                if (VerboseDiagnostics)
                {
                    //HexString = "";
                    HexString.clear();
                    //for (iFor = 1; iFor <= LongTubeByteLen + RackSyncLen + 2; iFor++)
                    for (iFor = 0; iFor < LongTubeByteLen + RackSyncLen + 2; iFor++)
                    {
                        char tempNib = Current[iFor] >> 4;
                        HexString.push_back(hexCharLUT[tempNib]);
                        tempNib = Current[iFor] & 0x0f;
                        HexString.push_back(hexCharLUT[tempNib]);
                    }
                        //HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);

                        /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
        Print #2, "TubeSync: " & HexString

 */
                    MinuteFileOut += "TubeSync: ";
                    MinuteFileOut += HexString;
                    MinuteFileOut.push_back('\n');

                }
                    // The string is long enough, look for delimiter
//                    if (Mid(Current, LongTubeByteLen + RackSyncLen + 1, 2) == CRLF)
//                    {
//                        if (!RackInSync)
//                        {
//                            RackInSync = true;
//                            CountSync(SyncCandidate);
//                            if (CrossMultiplicityEnabled)
//                            {
////                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
//                                if (chkTolerateGaps_Value == false) //TODO connect to wx with event
//                                {
////                                    if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
//                                    if (chkMinDiagnostics_Value == 0) //TODO connect to wx with event
//                                        ;
                if (('\r' == Current[LongTubeByteLen + RackSyncLen]) && ('\n' == Current[LongTubeByteLen + RackSyncLen + 1]))
                {
                    if (!RackInSync)
                    {
                        RackInSync = true;
                        CountSync(SyncCandidate);
                        if (CrossMultiplicityEnabled)
                        {
                            //                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
                            if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                            {
/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Reset timing: No delimiter long tube"

 */
                        //             OrderTimingReset = true;
                        //        }
                        //    }
                        //}
                        //DeviceData = Mid(Current, RackSyncLen + 1, LongTubeByteLen);
                        //if (VerboseDiagnostics)
                            //;
                                OrderTimingReset = true;
                            }
                        }
                    }
                    //std::string DeviceData(Current.begin() + RackSyncLen + 1, Current.begin() + LongTubeByteLen);
                    std::string DeviceData(Current.begin() + RackSyncLen, Current.begin() + RackSyncLen + LongTubeByteLen);
                    if (VerboseDiagnostics)
                    {
                        MinuteFileOut += "LongTube found";
                        MinuteFileOut.push_back('\n');
                    }
                            /* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "LongTube found"

 */        // Remove the message from the buffer
                //         if (Strings.Len(Current) > LongTubeByteLen + RackSyncLen + 2)
                //            Current = Mid(Current, LongTubeByteLen + RackSyncLen + 3);
                //        else
                //            Current = "";
                //        DecomLongTube(DeviceData);
                //    }
                //    else
                //    {
                //        // Although there are enough characters, there
                //        // is no delimeter. So just delete the initial
                //        // character and look for sync again.
                //        if (RecordFullSyncNotes)
                //        {
                //            HexString = "LongTube No Delimiter ";
                //            for (iFor = 1; iFor <= LongTubeByteLen + RackSyncLen + 2; iFor++)
                //                HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                //            Exception(HexString);
                //        }
                //        CountNoDelimeter(SyncCandidate);
                //        Current = Mid(Current, 2);
                //        nSyncSlip = nSyncSlip + 1;
                //        RackInSync = false;
                //    }
                //}
                //else
                //{
                //    // There is a proper sync, but not a long enough
                //    // string, so here one must just wait.
                //    StillHope = false;
                //    break;
                //}
                    if (MinuteFileOut.size() > 0)
                    {
                        wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
                        MinuteFile << MinuteFileOut << std::flush;
                        MinuteFileOut.clear();
                    }
                    DecomLongTube(DeviceData);
                    if (Current.size() > (LongTubeByteLen + RackSyncLen + 2))
                    {
                        //Current = Mid(Current, LongTubeByteLen + RackSyncLen + 3);
                        Current.erase(Current.begin(), Current.begin() + LongTubeByteLen + RackSyncLen + 2);
                    }
                    else
                    {
                        //Current = "";
                        Current.clear();
                    }
                }
                else
                {
                    // Although there are enough characters, there
                    // is no delimeter here. So just delete the initial
                    // character and look for sync again.
                    if (RecordFullSyncNotes)
                    {
                        HexString = "LongTube No Delimiter ";
                        //for (iFor = 1; iFor <= LongTubeByteLen + RackSyncLen + 2; iFor++)
                        //    HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                        for (iFor = 0; iFor < LongTubeByteLen + RackSyncLen + 2; iFor++)
                        {
                            char tempNib = Current[iFor] >> 4;
                            HexString.push_back(hexCharLUT[tempNib]);
                            tempNib = Current[iFor] & 0x0f;
                            HexString.push_back(hexCharLUT[tempNib]);
                        }
                        Exception(HexString);
                    }
                    CountNoDelimeter(SyncCandidate);
                    //Current = Mid(Current, 2);
                    Current.erase(Current.begin());
                    nSyncSlip = nSyncSlip + 1;
                    RackInSync = false;
                }
            }
            else
            {
                // There is a proper sync, but not a long enough
                // string, so here one must just wait.
                StillHope = false;
                //break;
            }

            //break;
        }
           
        else
        {
            Current.erase(Current.begin());
        }
    

            /* TODO rest of RackData()
 /* TODO rest of RackData()

            case object _ when NgrTubeSync :
            {
                if (Strings.Len(Current) >= NgrTubeByteLen + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrTubeByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
//                            if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                            if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
                                if (frmMonitor.chkMinDiagnostics.Value == 0)
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                    OrderTimingReset = true;
                            }
                        }
                        TubeB64 = Mid(Current, RackSyncLen + 1, NgrTubeByteLen);
                        TubeHex = "";
                        // Print #2, "TubeB64: " & TubeB64
                        for (iFor = 1; iFor <= 51; iFor++)
                            TubeHex = TubeHex + B64toHex(Mid(TubeB64, 4 * (iFor - 1) + 1, 4));
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
          Print #2, "NgrTubeSync: " & TubeHex

 */
 /* TODO rest of RackData()
                         NgrDecomTube(TubeHex);
                        // Remove the message from the buffer
                        if (Strings.Len(Current) > NgrTubeByteLen + RackSyncLen + 2)
                            Current = Mid(Current, NgrTubeByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        HexString = "NgrTube No Delimiter ";
                        for (iFor = 1; iFor <= NgrTubeByteLen + RackSyncLen + 2; iFor++)
                            HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                        Exception(HexString);
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when CtrSync :
            {
                // Print #2, "Counter Here"
                RawSyncCount(2) = RawSyncCount(2) + 1;
                if (Strings.Len(Current) >= CtrByteLen + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, CtrByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            CountSync(SyncCandidate);
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                    if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                    {
//                                    if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                        if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Reset timing: No delimiter counter"

 */
 /* TODO rest of RackData()
                                     OrderTimingReset = true;
                                }
                            }
                        }
                        DeviceData = Mid(Current, RackSyncLen + 1, CtrByteLen);
                        SequenceListString = SequenceListString + "C,";
//                        if ((chkRecABSTiming.Value == 1)) //TODO connect to wx with event
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "CtrSync"

 */
 /* TODO rest of RackData()
                         RecordCount(2) = RecordCount(2) + 1;
                        if (VerboseDiagnostics)
                        {
                            HexString = "";
                            for (iFor = 1; iFor <= CtrByteLen; iFor++)
                                HexString = HexString + Right("00" + Hex(Asc(Mid(Current, RackSyncLen + iFor))), 2);
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
          Print #2, "CtrSync: " & HexString

 */
 /* TODO rest of RackData()
                         }
                        DecomCtrIO(DeviceData);
                        ResetSent = false;
                        // Remove the message from the buffer
                        if (Strings.Len(Current) > CtrByteLen + RackSyncLen + 2)
                            Current = Mid(Current, CtrByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        if (RecordFullSyncNotes)
                        {
                            HexString = "Counter No Delimiter ";
                            for (iFor = 1; iFor <= CtrByteLen + RackSyncLen + 2; iFor++)
                                HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                            Exception(HexString);
                        }
                        CountNoDelimeter(SyncCandidate);
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrCtrSync :
            {
                if (Strings.Len(Current) >= NgrCtrByteLen + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrCtrByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                    if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                        OrderTimingReset = true;
                            }
                        }
                        CtrB64 = Mid(Current, RackSyncLen + 1, NgrCtrByteLen);
                        CtrHex = "";
                        // Print #2, "CtrB64: " & CtrB64
                        for (iFor = 1; iFor <= 51; iFor++)
                            CtrHex = CtrHex + B64toHex(Mid(CtrB64, 4 * (iFor - 1) + 1, 4));
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
          Print #2, "NgrCtrSync: " & CtrHex

 */
 /* TODO rest of RackData()
                         NgrDecomCtrIO(CtrHex);
                        ResetSent = false;
                        // Remove the message from the buffer
                        if (Strings.Len(Current) > NgrCtrByteLen + RackSyncLen + 2)
                            Current = Mid(Current, NgrCtrByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        HexString = "NGR Counter No Delimiter ";
                        if (RecordFullSyncNotes)
                        {
                            for (iFor = 1; iFor <= NgrCtrByteLen + RackSyncLen + 2; iFor++)
                                HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                        }
                        Exception("No Delimiter NgrCtr" + HexString);
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrErrorSyncA :
            {
                if (Strings.Len(Current) >= NgrErrorByteLenA + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrErrorByteLenA + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                    if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                        OrderTimingReset = true;
                            }
                        }
                        ErrB64 = Mid(Current, RackSyncLen + 1, NgrErrorByteLenA);
                        ErrHex = "";
                        // Print #2, "ErrB64: " & ErrB64
                        for (iFor = 1; iFor <= 2; iFor++)
                            ErrHex = ErrHex + B64toHex(Mid(ErrB64, 4 * (iFor - 1) + 1, 4));
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
          Print #2, "NgrErrSync: " & ErrHex

 */
 /* TODO rest of RackData()
                         Exception("NGR A Error: " + ErrHex);
                        LogEntry("NGR A Error: " + ErrHex);
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
          Print #2, "NGR A ErrorSync: " & ErrHex

 */
 // Remove the message from the buffer
/* TODO rest of RackData()
                        if (Strings.Len(Current) > NgrErrorByteLenA + RackSyncLen + 2)
                            Current = Mid(Current, NgrErrorByteLenA + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        Exception("No Delimiter NgrErrorSyncA");
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when DoneSync :
            {
                RawSyncCount(1) = RawSyncCount(1) + 1;
                if (Strings.Len(Current) >= DoneByteLen + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, DoneByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            CountSync(SyncCandidate);
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                {
                                    OrderTimingReset = true;
//                                    if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                    if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                        ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Reset timing: No delimiter on Done"

 */
 /* TODO rest of RackData()
                                 }
                            }
                        }
                        // This is a valid Done indicator
                        // Count it
                        RecordCount(1) = RecordCount(1) + 1;
                        SequenceListString = SequenceListString + "D,";
//                        if (VerboseDiagnostics | (chkRecABSTiming.Value == 1)) //TODO connect to wx with event
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "DoneSync"

 */
 /* TODO rest of RackData()
                         DoneMessageCount = DoneMessageCount + 1;
//                        frmHouse.lblDoneCount.Caption = Format(DoneMessageCount); //TODO connect to wx with event
                        // Remove the message from the buffer
                        if (Strings.Len(Current) > DoneByteLen + RackSyncLen + 2)
                            Current = Mid(Current, DoneByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        CountNoDelimeter(SyncCandidate);
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrDoneSync :
            {
                if (Strings.Len(Current) >= NgrDoneByteLen + RackSyncLen + 2)
                {
                    if (!RackInSync)
                    {
                        RackInSync = true;
                        Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                        if (CrossMultiplicityEnabled)
                        {
//                            if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                            if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                            if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                            if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                OrderTimingReset = true;
                        }
                    }
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrDoneByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        // This is a valid Done indicator
                        // Count it
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "NgrDoneSync"

 */
 /* TODO rest of RackData()
                         DoneMessageCount = DoneMessageCount + 1;
//                        frmHouse.lblDoneCount.Caption = Format(DoneMessageCount); //TODO connect to wx with event
                        SendCommand(CmdAcknowledge);
                        // Remove the message from the buffer
                        if (Strings.Len(Current) > NgrDoneByteLen + RackSyncLen + 2)
                            Current = Mid(Current, NgrDoneByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        Exception("No Delimiter NgrDoneSync");
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrGPSSync :
            {
                // This code is obsolete. The proper handling of the GPS record is now
                // much more elaborate. This is here mainly to illustrate how to extract
                // the GPS data from the NGR stream.
                if (Strings.Len(Current) >= NgrGPSByteLen + RackSyncLen + 2)
                {
                    // Print #2, "Current NgrGPSSync: " & Current
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrGPSByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                    OrderTimingReset = true;
                            }
                        }
                        // This is a valid transmission from the GPS Master
                        GPSB64 = Mid(Current, RackSyncLen + 1, NgrGPSByteLen);
                        GPSString = "";
                        GPSHex = "";
                        // Print #2, "GPSB64: " & GPSB64
                        for (iFor = 1; iFor <= 16; iFor++)
                            GPSHex = GPSHex + B64toHex(Mid(GPSB64, 4 * (iFor - 1) + 1, 4));
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "NgrGPSSync: " & GPSHex

 */        // The Ngr Readout combines the old GPS2 (SYNC) and GPS1 (LGPS)
 // This section of code derives from GPS2
/* TODO rest of RackData()
                        NgrGPSMessageCount = NgrGPSMessageCount + 1;
//                        frmHouse.lblnGPSSync(1).Caption = Format(NgrGPSMessageCount); //TODO connect to wx with event
                        GPSSyncSecond = System.Convert.ToInt32("&H" + Left(GPSHex, 2));
                        if (Mid(GPSHex, 3, 2) == "30")
                        {
                            GPSFresh = false;
//                            frmHouse.lblGPSSync.Caption = Format(GPSSyncSecond) + " " + "Stale"; //TODO connect to wx with event
                        }
                        else
                        {
                            GPSFresh = true;
//                            frmHouse.lblGPSSync.Caption = Format(GPSSyncSecond) + " " + "Fresh"; //TODO connect to wx with event
                        }
                        // This section of code derives from GPS1
                        for (iFor = 1; iFor <= 44; iFor++)
                            GPSString = GPSString + Chr(System.Convert.ToInt32("&H" + Mid(GPSHex, 2 * (iFor - 1) + 5, 2)));
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "GPSString: " & GPSString

 */
 /* TODO rest of RackData()
                         if (Left(GPSString, 7) == "$GPGGA,")
                        {
                            // This is a proper GPS message
                            lblLatestGPS.Caption = Mid(GPSString, 8, GPS1ByteLen - 7) + " at " + Format(CurTime, "yy/mm/dd hh:mm:ss");
                            ShowRackGPSData(Mid(GPSString, 8, GPS1ByteLen - 7));
                        }
                        // Remove the message from the buffer
                        if (Strings.Len(Current) > NgrGPSByteLen + RackSyncLen + 2)
                            Current = Mid(Current, NgrGPSByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        Exception("No Delimiter NgrGPS");
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when GPSSync :
            {
                // Print #2, "GPS Here", GPSByteLen + RackSyncLen + 2
                RawSyncCount(0) = RawSyncCount(0) + 1;
                // The GPS readout is formatted as two independent records,
                // but they always appear together, so treat this as one record.
                if (Strings.Len(Current) >= GPSByteLen + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, GPSByteLen + RackSyncLen + 1, 2) == CRLF)
                    {
                        // Print #2, "GPS Starting"
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            CountSync(SyncCandidate);
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                {
                                    OrderTimingReset = true;
//                                    if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                    if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                        ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Reset timing: No delimiter GPS"

 */
 /* TODO rest of RackData()
                                 }
                            }
                        }
                        DeviceData = Mid(Current, RackSyncLen + 1, GPSByteLen);
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "GPS found"

 */
 /* TODO rest of RackData()
                         DecomGPS(DeviceData);
                        // Remove the message from the buffer
                        // Print #2, "Remove GPS String", SequenceSecond, Len(Current)
                        if (Strings.Len(Current) > GPSByteLen + RackSyncLen + 2)
                            Current = Mid(Current, GPSByteLen + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        if (RecordFullSyncNotes)
                        {
                            HexString = "GPS No Delimiter ";
                            for (iFor = 1; iFor <= GPSByteLen + RackSyncLen + 2; iFor++)
                                HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                            Exception(HexString);
                        }
                        CountNoDelimeter(SyncCandidate);
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrCommandSyncA :
            {
                if (Strings.Len(Current) >= NgrCommandByteLenA + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrCommandByteLenA + RackSyncLen + 1, 2) == CRLF)
                    {
                        // This is a valid command message from Readout A
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                    OrderTimingReset = true;
                            }
                        }
                        CommandA = Mid(Current, RackSyncLen + 1, NgrCommandByteLenA);
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "NgrCommandA: " & CommandA

 */
//                        lblCommandSeenTime.Caption = Format(CurTime, "yy/mm/dd hh:mm:ss"); //TODO connect to wx with event
//                        lblCommandSeen.Caption = CommandA; //TODO connect to wx with event
/* TODO rest of RackData()
                        if (Strings.InStr(CommandA, "Ackn") == 0)
                            LogEntry("NgrCommandA: " + CommandA);
                        if (Strings.Len(Current) > GPS2ByteLen + RackSyncLen + 2)
                            Current = Mid(Current, NgrCommandByteLenA + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        Exception("No Delimiter NgrCommandSyncA");
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrMessageSyncA :
            {
                if (Strings.Len(Current) >= NgrMessageByteLenA + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrMessageByteLenA + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                    if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                        OrderTimingReset = true;
                            }
                        }
                        // This is a valid message from Readout A
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "NgrMessageSyncA: " & Mid$(Current, RackSyncLen + 1, NgrMessageByteLenA)

 */
 /* TODO rest of RackData()
                         RdoAMessage = Mid(Current, RackSyncLen + 1, NgrMessageByteLenA);
                        HandleRdoAMessage(RdoAMessage);
                        if (Strings.Len(Current) > NgrMessageByteLenA + RackSyncLen + 2)
                            Current = Mid(Current, NgrMessageByteLenA + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        Exception("No Delimiter NgrMessageSyncA");
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            case object _ when NgrMessageSyncB :
            {
                if (Strings.Len(Current) >= NgrMessageByteLenB + RackSyncLen + 2)
                {
                    // The string is long enough, look for delimiter
                    if (Mid(Current, NgrMessageByteLenB + RackSyncLen + 1, 2) == CRLF)
                    {
                        if (!RackInSync)
                        {
                            RackInSync = true;
                            Exception("Sync " + SyncCandidate + " after " + Format(nSyncSlip));
                            if (CrossMultiplicityEnabled)
                            {
//                                if (frmMonitor.chkMinDiagnostics.Value == 0) //TODO connect to wx with event
                                if (chkMinDiagnostics_Value == false) //TODO connect to wx with event
                                    ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #98, "Sync after " & Format$(nSyncSlip)

 */
//                                if (chkTolerateGaps.Value == 0) //TODO connect to wx with event
/* TODO rest of RackData()
                                    if (chkTolerateGaps_Value == false) //TODO connect to wx with event
                                        OrderTimingReset = true;
                            }
                        }
                        // This is a valid message from Readout B
                        if (VerboseDiagnostics)
                            ;/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
at System.ThrowHelper.ThrowArgumentException(ExceptionResource resource)
at System.Collections.Generic.Dictionary`2.Insert(TKey key, TValue value, Boolean add)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.WithDelegateToParentAnnotation(SyntaxToken lastSourceToken, SyntaxToken destination)
at ICSharpCode.CodeConverter.Shared.TriviaConverter.PortConvertedTrivia[T](SyntaxNode sourceNode, T destination)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitIdentifierName(IdentifierNameSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.IdentifierNameSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitSimpleArgument(SimpleArgumentSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.SimpleArgumentSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.<>c__DisplayClass83_0.<ConvertArguments>b__0(ArgumentSyntax a, Int32 i)
at System.Linq.Enumerable.<SelectIterator>d__5`2.MoveNext()
at System.Linq.Enumerable.WhereEnumerableIterator`1.MoveNext()
at Microsoft.CodeAnalysis.CSharp.SyntaxFactory.SeparatedList[TNode](IEnumerable`1 nodes)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitArgumentList(ArgumentListSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ArgumentListSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.NodesVisitor.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingNodesVisitor.DefaultVisit(SyntaxNode node)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.VisitInvocationExpression(InvocationExpressionSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.InvocationExpressionSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at ICSharpCode.CodeConverter.CSharp.VisualBasicConverter.MethodBodyVisitor.VisitExpressionStatement(ExpressionStatementSyntax node)
at Microsoft.CodeAnalysis.VisualBasic.Syntax.ExpressionStatementSyntax.Accept[TResult](VisualBasicSyntaxVisitor`1 visitor)
at Microsoft.CodeAnalysis.VisualBasic.VisualBasicSyntaxVisitor`1.Visit(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.ConvertWithTrivia(SyntaxNode node)
at ICSharpCode.CodeConverter.CSharp.CommentConvertingMethodBodyVisitor.DefaultVisit(SyntaxNode node)

Input:
Print #2, "NgrMessageSyncB: " & Mid$(Current, RackSyncLen + 1, NgrMessageByteLenB)

 */
 /* TODO rest of RackData()
                         RdoBMessage = Mid(Current, RackSyncLen + 1, NgrMessageByteLenB);
                        HandleRdoBMessage(RdoBMessage);
                        if (Strings.Len(Current) > NgrMessageByteLenB + RackSyncLen + 2)
                            Current = Mid(Current, NgrMessageByteLenB + RackSyncLen + 3);
                        else
                            Current = "";
                    }
                    else
                    {
                        // Although there are enough characters, there
                        // is no delimeter here. So just delete the initial
                        // character and look for sync again.
                        Exception("No Delimiter NgrMessageSyncB");
                        Current = Mid(Current, 2);
                        nSyncSlip = nSyncSlip + 1;
                        RackInSync = false;
                    }
                }
                else
                {
                    // There is a proper sync, but not a long enough
                    // string, so here one must just wait.
                    StillHope = false;
                    break;
                }

                break;
            }

            default:
            {
                // The start of the buffer does not have a recognized sync sequence
                if (NewData)
                {
                    if (RecordFullSyncNotes)
                    {
                        HexString = "No Sync Starts Current: ";
                        for (iFor = 1; iFor <= Strings.Len(Current); iFor++)
                            HexString = HexString + Right("00" + Hex(Strings.Asc(Mid(Current, iFor, 1))), 2);
                        Exception(HexString);
                    }
                    // Only do this once per read
                    NewData = false;
                }
                Current = Mid(Current, 2);
                nSyncSlip = nSyncSlip + 1;
                RackInSync = false;
                break;
            }
        }
    }
/* TODO rest of RackData() */
        if(MinuteFileOut.size() > 0)
        {
            wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
            MinuteFile << MinuteFileOut << std::flush;
            MinuteFileOut.clear();
        }
    }
//    frmHouse.lblNCurrent(1).Caption = Strings.Format(Strings.Len(Current), "0"); //TODO connect to wx with event
    csTime.~wxCriticalSectionLocker(); //end critical section and unlock

}

#ifdef _WIN32  // Windows specific code

HANDLE LandMonitorApp::openSerialPort(const char* portName, const char* serialParams)
{
    HANDLE hSerial = CreateFileA(portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        // Handle error opening the port
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams))
    {
        // Handle error getting the serial port state
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    // Parse serial parameters
    int baudRate, byteSize, stopBits;
    char parity;
    if (sscanf(serialParams, "%d,%c,%d,%d", &baudRate, &parity, &byteSize, &stopBits) != 4)
    {
        // Handle error parsing serial parameters
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    // Set serial parameters
    dcbSerialParams.BaudRate = baudRate;
    dcbSerialParams.ByteSize = byteSize;
    dcbSerialParams.StopBits = stopBits;
    //dcbSerialParams.Parity = parity;
    if (('n' == parity) || ('N' == parity))
    {
        dcbSerialParams.Parity = NOPARITY; //fix for WhatAmI.gpc file format -B
    }
    else
    {
        dcbSerialParams.Parity = ODDPARITY; //TODO any other type of parity
    }

    if (!SetCommState(hSerial, &dcbSerialParams))
    {
        // Handle error setting the serial port state
        CloseHandle(hSerial);
        return INVALID_HANDLE_VALUE;
    }

    return hSerial;
}




/*
void comRack_OnComm()
{
    std::string EVMsg;
    EVMsg = "EVMsg";

    // Branch according to the CommEvent Prop..
    switch (comRack.CommEvent)
    {
        // Event messages
    case MSCOMM_EV_RECEIVE:
        RackData();
        EVMsg = "";
        break;
    case MSCOMM_EV_SEND:
        EVMsg = "";
        break;
    case MSCOMM_EV_CTS:
        EVMsg = "Change in CTS Detected";
        break;
    case MSCOMM_EV_DSR:
        EVMsg = "Change in DSR Detected";
        break;
    case MSCOMM_EV_CD:
        EVMsg = "Change in CD Detected";
        break;
    case MSCOMM_EV_RING:
        EVMsg = "The Phone is Ringing";
        break;
    case MSCOMM_EV_EOF:
        EVMsg = "End of File Detected";
        break;
        // Error messages
    case MSCOMM_ER_BREAK:
        EVMsg = "Break Received";
        break;
    case MSCOMM_ER_CTSTO:
        EVMsg = "CTS Timeout";
        break;
    case MSCOMM_ER_DSRTO:
        EVMsg = "DSR Timeout";
        break;
    case MSCOMM_ER_FRAME:
        EVMsg = "Framing Error";
        break;
    case MSCOMM_ER_OVERRUN:
        EVMsg = "Overrun Error";
        break;
    case MSCOMM_ER_CDTO:
        EVMsg = "Carrier Detect Timeout";
        break;
    case MSCOMM_ER_RXOVER:
        EVMsg = "Receive Buffer Overflow";
        Nrbo = Neof + 1;
        Tlrbo = CurTime;
        break;
    case MSCOMM_ER_RXPARITY:
        EVMsg = "Parity Error";
        break;
    case MSCOMM_ER_TXFULL:
        EVMsg = "Transmit Buffer Full";
        break;
    default:
        EVMsg = "Unknown error or event";
        break;
    }

    if (EVMsg.length() > 0)
    {
        Exception("comRack Event " + EVMsg);
        EVMsg = "";
    }
}
*/
#else  // Linux specific code
int LandMonitorApp::openSerialPort(const char* portName, const char* serialParams)
{
    int fd = open(portName, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1)
    {
        // Handle error opening the port
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0)
    {
        // Handle error getting the serial port attributes
        close(fd);
        return -1;
    }

    // Parse serial parameters
    int baudRate, byteSize, stopBits;
    char parity;
    if (sscanf(serialParams, "%d,%c,%d,%d", &baudRate, &parity, &byteSize, &stopBits) != 4)
    {
        // Handle error parsing serial parameters
        close(fd);
        return -1;
    }

    // Set serial parameters
    cfsetospeed(&tty, baudRate);
    cfsetispeed(&tty, baudRate);

    switch (parity)
    {
    case 'N':
        tty.c_cflag &= ~PARENB;
        tty.c_iflag &= ~INPCK;
        break;
    case 'E':
        tty.c_cflag |= PARENB;
        tty.c_cflag &= ~PARODD;
        tty.c_iflag |= INPCK;
        break;
    case 'O':
        tty.c_cflag |= PARENB;
        tty.c_cflag |= PARODD;
        tty.c_iflag |= INPCK;
        break;
    default:
        // Handle invalid parity
        close(fd);
        return -1;
    }

    tty.c_cflag &= ~CSTOPB;

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_oflag = 0;

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        // Handle error setting the serial port attributes
        close(fd);
        return -1;
    }

    return fd;
}

#endif




wxThread::ExitCode RackThread::Entry()
{
    //nextTimer1Sec = std::chrono::system_clock::now();
    while (true)
    {
        //char readBuf[50];
        //DWORD bRead;
        //ReadFile(comRack, &readBuf, 50, &bRead, NULL);
        //if(bRead>0)
        //{
        //    wxCriticalSectionLocker csLock(wxGetApp().csMinuteFile); //lock access to minute file
        //    for (int i = 0; i < bRead; i++)
        //        MinuteFile << std::hex << (int)readBuf[i];
        //    MinuteFile << '\n' << std::flush;
        //}
        wxGetApp().RackData();
    }
    //MinuteFile.close();
    return 0;
}


//wxThread::ExitCode Timer1Thread::Entry()
//{
//    nextTimer1Sec = std::chrono::system_clock::now();
//    while (true)
//    {
//        wxGetApp().Timer1_Timer();
//    }
//    return 0;
//}