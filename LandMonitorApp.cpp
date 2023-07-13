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


#define MAJOR_VERSION 0 //changes on major revisions
#define MINOR_VERSION 0 //changes every settled change
#define MAX_LINE_LEN 1000 //max num char in line

IMPLEMENT_APP(LandMonitorApp);



//New C++ port variables
LMMonitor* monitorF;
std::fstream MinuteFile;
std::fstream BareTrapFile;
int comRack_CommPort;
std::string comRack_Settings;
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
std::vector<int> BareTrapUnits(2);
long nBaresTrapped;
bool RecordRaw;
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
std::vector<bool> Active(lDataRow);
std::vector<int> ExTime(lDataRow);
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
std::vector<std::vector<long>> HitTrack(MaxHitTrack, std::vector<long>(AbsoluteLastUnit));
std::vector<long> RawSyncCount(LastRawSyncType);
std::vector<long> RecordCount(5 + 2 * AbsoluteLastUnit);
std::vector<bool> TimingLost(AbsoluteLastUnit);
std::vector<bool> PrevTimingLost(AbsoluteLastUnit);
bool RackInSync;
long nRead;
long nReadSecond;
double nReadTotal;
double nSyncSlip;
double nSyncSlipTotal;
double nOverflowGapA;
double nLongTubeInternalErr;
const int maxSyncType = 5;
double SyncEventCount;
std::vector<double> SyncTypeCount(maxSyncType);
std::vector<double> NoDelimiterCount(maxSyncType);
std::string SequenceListString;
std::vector<std::vector<std::string>> LatestRemoteTiming(AbsoluteLastUnit, std::vector<std::string>(2));
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
std::vector<int> CatchDecimateMod(maxCatchDecimate);
std::vector<long> CatchAvailableCounter(maxCatchDecimate);
std::vector<long> CatchDecimatedCounter(maxCatchDecimate);
std::vector<std::vector<int>> HourCompactMatrix(maxCatchDecimate, std::vector<int>(AbsoluteLastUnit));
std::vector<std::string> UnitList(AbsoluteLastUnit);
std::vector<long> EmptyRemoteCount(AbsoluteLastUnit);
std::vector<long> EarlyOverCount(AbsoluteLastUnit);
std::vector<long> LateOverCount(AbsoluteLastUnit);
std::vector<long> BadGPSCount(AbsoluteLastUnit);
std::vector<long> UnitHitCount(AbsoluteLastUnit);

std::vector<std::vector<double>> HitTimes(AbsoluteLastUnit, std::vector<double>(maxHitsPerUnit));
std::vector<std::vector<int>> HitPHA(AbsoluteLastUnit, std::vector<int>(maxHitsPerUnit));
std::vector<std::vector<bool>> HitSelect(AbsoluteLastUnit, std::vector<bool>(maxHitsPerUnit));
std::vector<std::string> HitFileName(AbsoluteLastUnit);
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
const int LastRemStatHist = 2;
std::vector<std::vector<std::vector<long>>> RemStatHis(LastRemStatHist, std::vector<std::vector<long>>(RemStatHistLastChannel, std::vector<long>(AbsoluteLastUnit)));
std::vector<long> RemStatHistMax(LastRemStatHist);
std::vector<std::string> RemStatHistTag(LastRemStatHist);
std::vector<int> EventsInSecond(AbsoluteLastUnit);
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

std::vector<std::vector<float>> MonitorLimit(MonitorLimLast, std::vector<float>(AbsoluteLastHistogram));
std::vector<std::vector<std::vector<float>>> CrossLimit(CrossLimLast, std::vector<std::vector<float>>(AbsoluteLastCrossHistogram, std::vector<float>(1)));
std::vector<float> MonitorHisDumpInterval(AbsoluteLastHistogram);
std::vector<std::vector<float>> CrossHisDumpInterval(AbsoluteLastHistogram, std::vector<float>(1));
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
std::vector<std::string> MonitorDaColLabel(MonitorDaColLast);
// For the monitor histograms the last index is selected or unselected
// The long and short timing have different second index ranges
// For the cross histograms there is no selection. The last index
// indicates short or long
std::vector<std::vector<std::vector<long>>> MonitorHis(MonitorHisLastChan, std::vector<std::vector<long>>(AbsoluteLastHistogram, std::vector<long>(1)));
std::vector<std::vector<std::vector<long>>> CrossHis(CrossHisLastChan, std::vector<std::vector<long>>(AbsoluteLastCrossHistogram, std::vector<long>(1)));
// The following record errors in the channel assignments
std::vector<std::vector<long>> MonitorHisErr(AbsoluteLastHistogram, std::vector<long>(1));
std::vector<std::vector<long>> CrossHisErr(AbsoluteLastCrossHistogram, std::vector<long>(1));
std::vector<std::vector<std::vector<float>>> MonitorRing(MonitorRingLast, std::vector<std::vector<float>>(AbsoluteLastHistogram, std::vector<float>(1)));
std::vector<std::vector<int>> MonitorRingLoc(AbsoluteLastHistogram, std::vector<int>(1));
std::vector<std::vector<std::vector<float>>> CrossRing(CrossRingLast, std::vector<std::vector<float>>(AbsoluteLastCrossHistogram, std::vector<float>(1)));
std::vector<std::vector<int>> CrossRingLoc(AbsoluteLastCrossHistogram, std::vector<int>(1));
std::vector<std::string> MonitorHisTag(AbsoluteLastHistogram);
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
std::vector<int> Days(11);
const std::string DaysString = "0,31,59,90,120,151,181,212,243,273,304,334,";
std::vector<int> DaysLeap(11);
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
std::vector<float> HitBufferLag(AbsoluteLastUnit);
std::vector<int> NewestPointer(AbsoluteLastUnit);
std::vector<int> OldestPointer(AbsoluteLastUnit);
std::vector<bool> EarlyOverflow(AbsoluteLastUnit);
std::vector<bool> LateOverflow(AbsoluteLastUnit);
std::vector<int> LatestOverflowPointer(AbsoluteLastUnit);
std::vector<std::vector<int>> RingPHA(RingLast, std::vector<int>(AbsoluteLastUnit));
std::vector<std::vector<bool>> RingSelect(RingLast, std::vector<bool>(AbsoluteLastUnit));
std::vector<std::vector<double>> RingRawFullTime(RingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<std::vector<double>> RingCorFullTime(RingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<std::vector<double>> RingGoodGPSTime(RingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<double> LastGoodGPSTime(AbsoluteLastUnit);
std::vector<std::vector<double>> RingInterval(RingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<std::vector<long>> RingRawTOscLOB(RingLast, std::vector<long>(AbsoluteLastUnit));
std::vector<std::vector<long>> RingRawTOscHOB(RingLast, std::vector<long>(AbsoluteLastUnit));
std::vector<std::vector<long>> RingCorTOscHOB(RingLast, std::vector<long>(AbsoluteLastUnit));
std::vector<std::vector<double>> RingPeriod(RingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<std::vector<double>> RingBase(RingLast, std::vector<double>(AbsoluteLastUnit));
std::vector<std::vector<double>> RingGPSTime(RingLast, std::vector<double>(AbsoluteLastUnit));

std::vector<long> Counts(AbsoluteLastUnit);
std::vector<long> CurrentScaler(AbsoluteLastUnit);
std::vector<long> PriorScaler(AbsoluteLastUnit);
std::vector<long> Deltas(AbsoluteLastUnit);
std::vector<long> PrevMinuteCounters(AbsoluteLastUnit);
std::vector<long> MinuteCounters(AbsoluteLastUnit);
std::vector<long> MinuteDeltas(AbsoluteLastUnit);
std::vector<int> FirmVersion(AbsoluteLastUnit);
std::vector<std::string> DisplayTag(LastDisplayTag);
std::vector<std::string> UnitTag(AbsoluteLastUnit);
std::vector<long> ResetDeltaRef(AbsoluteLastUnit);
std::vector<bool> NotSeenYet(AbsoluteLastUnit);
std::vector<std::vector<long>> House(6, std::vector<long>(AbsoluteLastUnit));
std::vector<long> Below(AbsoluteLastUnit);
std::vector<long> PrevInArray(AbsoluteLastUnit);
std::vector<long> InArray(AbsoluteLastUnit);
std::vector<long> Above(AbsoluteLastUnit);
std::vector<long> HrBelow(AbsoluteLastUnit);
std::vector<long> HrIn(AbsoluteLastUnit);
std::vector<long> HrAbove(AbsoluteLastUnit);
std::vector<long> MinBelow(AbsoluteLastUnit);
std::vector<long> MinIn(AbsoluteLastUnit);
std::vector<long> MinAbove(AbsoluteLastUnit);
double LongIntervalLimit;
const int lRoger = 4;
std::vector<std::string> RogersRing(lRoger);
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
            strftime(fCurTime, std::size(fCurTime), "%y_%m_%d", std::localtime(&CurTime));
            CurDate = fCurTime;
            strftime(fCurTime, std::size(fCurTime), "%y/%m/%d %T", std::localtime(&CurTime));
            CurDayTimTag = fCurTime;
            //lblStartupTime.Caption = CurDayTimTag; //TODO connect to wx
            //CurYear = Val(Format(CurTime, "yyyy"));
            //CurMonth = Val(Format(CurTime, "mm"));
            //CurDay = Val(Format(CurTime, "dd"));
            //CurHour = Val(Format(CurTime, "hh"));
            //CurMin = Val(Format(CurTime, "nn"));
            //CurSec = Val(Format(CurTime, "ss"));
            //CurDOY = DayOfYear(CurMonth, CurDay, CurYear);
            struct tm * tmCurTime = std::localtime(&CurTime);
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
            while (UnitListString.find(',') > 0)
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
            while (UnitListString.find(',') > 0)
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
    };/* Cannot convert ExpressionStatementSyntax, System.ArgumentException: An item with the same key has already been added.
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
Print #2, "HisParPHA600" & " " & HisParPHA600

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
Print #2, "HisParPHA700" & " " & HisParPHA700

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
Print #2, "HisParPHA800" & " " & HisParPHA800

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
Print #2, "HisParMPS" & " " & HisParMPS

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
Print #2, "HisParMPL" & " " & HisParMPL

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
Print #2, "HisParCRS" & " " & HisParCRS

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
Print #2, "HisParCRL" & " " & HisParCRL

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
Print #2, "SelCatch" & " " & CatchDecimateInit

 */
    MinuteFile.close(); //TODO rest of Monitor_Form_Load()

    return true; //TODO rest of Monitor_Form_Load()
    }

/* TODO rest of Monitor_Form_Load()
* 
    PHAHisOrigin = 0;
    MPSHisOrigin = LastUnit + 1;
    MPLHisOrigin = 2 * (LastUnit + 1);
    MOSHisOrigin = 3 * (LastUnit + 1);
    MOLHisOrigin = 4 * (LastUnit + 1);
    if (RecordSSE)
        btnRecordSSE.Caption = "Rec SSE";
    else
        btnRecordSSE.Caption = "No SSE";
    GridLock = false;
    GridCrossLock = false;
    for (iFor = 0; iFor <= AbsoluteLastUnit; iFor++)
        NotSeenYet[iFor] = true;
    SSEString = "1946 027 05 45 06 15";
    for (iFor = 0; iFor <= 4; iFor++)
        RogersRing[iFor] = SSEString;
    RogersRingPointer = 0;
    FillIntegerArray(Days, 11, DaysString);
    FillIntegerArray(DaysLeap, 11, DaysLeapString);
    // Set up histograms based on remotes
    for (nUnit = 0; nUnit <= LastUnit; nUnit++)
    {
        ValidCrossUnit[nUnit] = false;
        nHis = PHAHisOrigin + nUnit;
        MonitorHisTag[nHis] = "PHA" + UnitTag(nUnit);
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParPHA300;
        if ((FirmVersion(nUnit) == 600))
            TempStr = HisParPHA600;
        if ((FirmVersion(nUnit) == 700))
            TempStr = HisParPHA700;
        if ((FirmVersion(nUnit) == 800))
        {
            ValidCrossUnit[nUnit] = EligibleCrossUnit(nUnit) & (!UnitIsBad(nUnit));
            if (ValidCrossUnit(nUnit))
                nValidCrossUnits = nValidCrossUnits + 1;
            TempStr = HisParPHA800;
        }
        if ((FirmVersion(nUnit) == 900))
            TempStr = HisParPHA900;
        MakeCSVString(TempStr);
        for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MPSHisOrigin + nUnit;
        MonitorHisTag[nHis] = "MPS" + UnitTag(nUnit);
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPS;
        MakeCSVString(TempStr);
        for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MPLHisOrigin + nUnit;
        MonitorHisTag[nHis] = "MPL" + UnitTag(nUnit);
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPL;
        MakeCSVString(TempStr);
        for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MOSHisOrigin + nUnit;
        MonitorHisTag[nHis] = "MOS" + UnitTag(nUnit);
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPS;
        MakeCSVString(TempStr);
        for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        nHis = MOLHisOrigin + nUnit;
        MonitorHisTag[nHis] = "MOL" + UnitTag(nUnit);
        MonitorHisDumpInterval[nHis] = MonDumpDefault;
        TempStr = HisParMPL;
        MakeCSVString(TempStr);
        for (iFor = 0; iFor <= MonitorLimLast; iFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            MonitorLimit[iFor, nHis] = strtof(Left(TempStr, nComma - 1));
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
        for (jFor = 0; jFor <= CrossLimLast; jFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            CrossLimit[jFor, iFor, 0] = strtof(Left(TempStr, nComma - 1));
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        CrossHisDumpInterval[iFor, 0] = MonDumpDefault;
        TempStr = HisParCRL;
        MakeCSVString(TempStr);
        for (jFor = 0; jFor <= CrossLimLast; jFor++)
        {
            nComma = Strings.InStr(TempStr, ",");
            CrossLimit[jFor, iFor, 1] = strtof(Left(TempStr, nComma - 1));
            TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
        }
        CrossHisDumpInterval[iFor, 1] = MonDumpDefault;
    }
    // Now set up the column widths for the display grids
    TempStr = "1000,800,800,800,800,800,800,800,800";
    MakeCSVString(TempStr);
    for (iFor = 0; iFor <= MonitorLimColLast; iFor++)
    {
        nComma = Strings.InStr(TempStr, ",");
        MonitorLimColWidth[iFor] = strtol(Left(TempStr, nComma - 1));
        TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
    }

    TempStr = "250,800,1000,800,800,800,800,800,800,800,800";
    MakeCSVString(TempStr);
    for (iFor = 0; iFor <= MonitorDaColLast; iFor++)
    {
        nComma = Strings.InStr(TempStr, ",");
        MonitorDaColWidth[iFor] = strtol(Left(TempStr, nComma - 1));
        TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
    }

    TempStr = "1000,800,800,800,800,800,800,800,800";
    MakeCSVString(TempStr);
    for (iFor = 0; iFor <= CrossLimColLast; iFor++)
    {
        nComma = Strings.InStr(TempStr, ",");
        CrossLimColWidth[iFor] = strtol(Left(TempStr, nComma - 1));
        TempStr = TempStr.substr( nComma + 1, std::string::npos); //replacement for Mid -B
    }

    FillStringArray(MonitorDaColLabel(), "T,Item,Mean,Sigma,Skew,Total,Below,In,Above,LimL,LimH,");
    // Read the cross (histogram) matrix
    // This is a (LastUnit+1) by (LastUnit+1) matrix
    // assigning a cross histogram to each ordered unit pair
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
        MinuteFile << "Log: " << Message << ' ' << fCurTime << '\n';
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
    int LastRemote = std::stoi(SetString.substr(0, SetString.find(" ")));
    if (LastRemote >= 0) {
        int RemoteBaseAddress = LastUnit + 1;
        LastUnit += LastRemote + 1;
        for (int iFor = 0; iFor <= LastRemote; iFor++) {
            nUnit = RemoteBaseAddress + iFor;
            FirmVersion[nUnit] = std::stoi(SetString.substr(iFor * 3 + 1, 2));
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

void LandMonitorApp::MakeCSVString(std::string& OpString) {
    int nChar;
    OpString = OpString + " ";
    while (OpString.find(CarRet) != std::string::npos) {
        nChar = OpString.find(CarRet);
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
    while (OpString.find(",") != std::string::npos) {
        nChar = OpString.find(",");
        if (nChar == 0) {
            OpString = " " + OpString.substr(1);
        }
        else if (nChar == OpString.length() - 1) {
            OpString = OpString.substr(0, nChar - 1) + " ";
        }
        else {
            OpString = OpString.substr(0, nChar - 1) + " " + OpString.substr(nChar + 1);
        }
    }
    while (OpString.find(TabChar) != std::string::npos) {
        nChar = OpString.find(TabChar);
        if (nChar == 0) {
            OpString = OpString.substr(1);
        }
        else if (nChar == OpString.length() - 1) {
            OpString = OpString.substr(0, nChar);
        }
        else {
            OpString = OpString.substr(0, nChar - 1) + " " + OpString.substr(nChar + 1);
        }
    }
    while (OpString.find("  ") != std::string::npos) {
        nChar = OpString.find("  ");
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
    while (OpString.substr(0, 1) == " ") {
        OpString = OpString.substr(1);
    }
    while (OpString.find(" ") != std::string::npos) {
        nChar = OpString.find(" ");
        if (nChar == 0) {
            OpString = "," + OpString.substr(1);
        }
        else if (nChar == OpString.length() - 1) {
            OpString = OpString.substr(0, nChar - 1) + ",";
        }
        else {
            OpString = OpString.substr(0, nChar - 1) + "," + OpString.substr(nChar + 1);
        }
    }
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

