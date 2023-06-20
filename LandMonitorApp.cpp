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




#define MAJOR_VERSION 0 //changes on major revisions
#define MINOR_VERSION 0 //changes every settled change

IMPLEMENT_APP(LandMonitorApp);


//#ifdef __linux__ 
//std::string PersonalityDirectory("~/LandBase/Personality/");
//#elif _WIN32
//std::string PersonalityDirectory("C:\\LandBase\\Personality\\");
//#endif

//Converted Global varibles from NuMonSubs-bas.txt
// Definitions for data transmission options
const int lDataCol = 3;
const int lDataRow = 7;
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
const int AbsoluteLastUnit = 24;
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
bool RunSecondCopy;
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
const int MonitorLimColLast = 8;
const int CrossLimLast = 6;
const int CrossLimColLast = 8;
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
const int mBar = 1;
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

//Converted Global varibles from BGSUBS-bas.txt
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


//Converted Global varibles from Monitor-frm.txt
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
//float PressDecAIR[2]; //declared  in multiple files in VB
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

LMMonitor* monitorF;

bool LandMonitorApp::OnInit()
{
	monitorF = new LMMonitor(NULL);
	const wxString ver(_("Land Monitor V") + std::to_string(MAJOR_VERSION) + _(".") + std::to_string(MINOR_VERSION));

	monitorF->setSoftwareVer( ver );
	monitorF->Show();
	return true;
}

