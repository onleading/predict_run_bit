#ifndef   __FUNC_H__
#define   __FUNC_H__

#ifdef BIG5
	LPSTR	_FL(LPSTR lpString);
	LPSTR	_F(LPCSTR lpString);
	LPSTR	_JL(LPSTR lpString);
	LPSTR	_J(LPCSTR lpString);
#else
	#define _FL(a) a
	#define _F(a)  a
	#define _JL(a) a
	#define _J(a)  a
#endif
extern BOOL IsBig5Version();
extern DWORD TDX_GetPrivateProfileString(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName);
extern int GetPrivateProfileChar(LPCSTR lpAppName,LPCSTR lpKeyName,char nDefault,LPCSTR lpFileName);

extern long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*内部申请*/,short nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved);
extern long CALLBACK SwitchCallBackFunc(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long nReserved);
extern long CALLBACK GpInfoHqCallBack(const char * Code,short setcode,char *Name,float *fNow,float *fClose);

extern char * GetCurDir(char * szPath,int len);
extern BOOL TestFileExist(CString filename);

extern int   TDX_MessageBox(HWND m_hWnd,const char *str,UINT nType,BOOL bSepTitle=FALSE);
extern void	 TDXBeep(int flag=0);
extern void  Tdx_OutputDebugString(const char *fmt,...);

extern int FjSort(const void * a,const void *b);
extern unsigned char FindFile(char * FileName);
//类型转换函数
extern void		MendPath(char *path);
extern void		AllTrim( char *s);
extern void		AllTrimEx( char *s);
extern void		TrimString(CString& theStr);
extern CString	IntToStr(int n, short intFlag=0);
extern CString	FloatToStr(float ftmp);
extern CString	FloatToStr2(float ftmp,short xsflag);
extern void		GetStr( const char *str1, char *str2, WORD nSize, short xh, char ch );
extern void		ParseMessageStr(const char *str1, char *str2, const char *pre,int xh);
extern void		GetStrDomainValue(const char *TotalStr, const char *KeyStr, char *Value,int MaxValueLen,const char *sep);
extern CTime	LongToTime(long ltmp);
extern long		SubLongTime(long ltmp1,long ltmp2);
extern long		AddLongTime(long ltmp1,long span);
extern long		TimeToLong(CTime time);
extern char		* MakeGB(DWORD ltmp);	//总股本
extern char		* MakeVol3(long ltmp);  //特大长整形的处理
extern char		* MakeVol2(long ltmp);  //成交量
extern char		* MakeVol(double ftmp); //成交额
extern char		* MakeJE3(float ftmp);
extern char		* MakeJE2(float ftmp);	
extern char		* MakeJE(float ftmp);   //现金额
extern char		* MakeFloat(double ftmp);//处理浮点数
extern short	FloatComp(float fFloat1,float fFloat2);
//删除通配符文件
extern void		DeleteMatchFile(CString path,CString tpf);
//删除目录下所有目录和文件
extern void		DeleteDirInfo(const char * Path);

extern int  sort_func( const void * a, const void * b );
extern int  sort_func2( const void * a, const void * b );
extern int  sort_func3( const void * a, const void * b );

extern float  cal_averagetmp(struct MemStkInfo *scode,struct CurrStockData * hqp);

extern short  GetXSFlag(struct MemStkInfo * scode);
extern BOOL	  UseSpecVolumeUnit(int StockType,short PeriodType);
extern float  CalcHSL(MemStkInfo *pInfo,float fVolume,float fRealLTGB,short nPeriod);

extern short  GetCorrespondingMin(BOOL bDSFlag=FALSE);
extern long	  GetYMD();
extern long	  GetHMS();
extern long   GetSeconds();
extern void	  GetYYYYMMDDHHMMSS(char *pStr);

extern short  GetFzNum(short setcode) ;
extern long   GetLsUnit(short setcode);
extern BOOL   testzs(const char * code,short setcode);
extern BOOL	  testzs(struct MemStkInfo *pStkInfo);
extern BOOL	  testhkzs(struct MemStkInfo *pStkInfo);
extern BOOL	  testhkwarrant(struct MemStkInfo *pStkInfo);
extern BOOL   IsLCIdx(struct MemStkInfo *pStkInfo);
extern BOOL   IsLCExt(struct MemStkInfo *pStkInfo);
extern BOOL   IsLCPt(struct MemStkInfo *pStkInfo);
extern BOOL	  IsLocalStock(struct MemStkInfo *pStkInfo);

extern short  need_justcjl ( short setcode, char * code );
extern long   GetYear1st(long date);	// 得到某季的第一天
extern long	  GetSeason1st(long date);  // 得到某季的第一天
extern long   GetMonth1st(long date);	// 得到某月的第一天
extern long   GetFriday(long date );	//得到某日的星期五

extern CView *OpenNewView(const char *docstr);
extern void   CloseView(const char *docstr);
extern CView *FindViewThenActive(const char *docstr);
extern CView *SwitchToView(const char *docstr);
extern int    GetMatchedStr(char * lpszInStr,int nFlag,int nMax,int * lpnIndex);
extern BOOL   IsFBLQ(int xh);

extern void	ResetMemStkOtherInfo();	//填充MemStkInfo的初始值
extern BOOL Is_HQInit_Time();
extern BOOL	Is_Jy_Time(int nSkipMin=15);

extern int  System2Tdx(int m,short * fz);
extern int  GetRealTotalMinute(short *fz);
extern int  GetTdxMinute( short * fz );				// 取得到现在为止, 总共经过的分钟数
extern int	GetTdxSectNum(short *fz);
extern int  GetNowTdxSect(short *fz);
extern int	GetNowTdxSect(short nCurMin, short *fz);
extern char * GetTdxTimeStr(int itemnum);			//得到时间串
extern char * GetTdxTimeStrFromSecond(int nSecond);	//从秒数中得到时间串
extern char * GetTimeStrFromLong(long hms);
extern char * GetTdxDateStr(long date);
extern char * GetTdxDateWeekStr(long date);

extern int   GetMinuteXh( int lineperiod,short * fz,short mulnum);
extern int   GetRealMinuteXh( int lineperiod,short * fz,short mulnum);
extern short GetFZnoFromMinute( int m ,int lineperiod,short * fz,short mulnum);
extern int   GetMinuteFromFZ(int xh,int fznum,short * fz);
extern void  InitStkTime(short setcode,short * fz);
extern void  GetJBZL(void);
extern void  GetConceptBlock();
extern void  GetZHBBlockNum();

//根据自定义板块来填充序号缓冲区
int FiltBlockCode(int blockno,short *indexbuffer);
//根据行业和地域来填充序号缓冲区
extern int FiltDYHYCode(int flag,int blockno,short *indexbuffer);
//根据概念来填充序号缓冲区
extern int FiltConceptCode(int blockno,short *indexbuffer);
//根据组合来填充序号缓冲区
extern int FiltZHCode(int blockno,short *indexbuffer);
//得到三板股票
extern int FiltSBCode(short *indexbuffer);
//得到特色指数
extern int FiltSpecIndexCode(short *indexbuffer);
//根据A+H填充序号缓冲区
extern int FiltAJHIndexCode(short *indexbuffer);
//根据本地代码充序号缓冲区
extern int FiltLCCode(short *indexbuffer);

extern int FiltZJHHYCode(int blockno,short *indexbuffer);

extern CString GetBlockName(int index);
extern CString GetBlockFileName(int index);
extern int GetBlockIndexFromName(const char *pBlockName);

extern BOOL FindParAgForBg(char *lpAgCode, short &agSetCode, const char *lpBgCode, short bgSetCode);
extern int GetDomainGP(struct GpDomain_Struct GpDomain,short *indexbuffer);
extern int FiltMulGpDomain(struct GpDomain_Struct *GpDomain,int nSelDomain,short *indexbuffer);

extern short	Get_SS_Day(short Index);
extern int		GetWeek(long    Date);

extern double	PointToLine(int x,int y,int x1,int y1,int x2,int y2);
extern DWORD	on_filelength(FILE * ftmp);

//////////////////////////////////////////////////////////

extern float GetTPPrice(MemStkInfo *tmpInfo,float Close,BOOL bUp);
extern int   GetGPZS(int iNo);

extern char *GetSetStr(int setcode);

extern short ReadCacheData(MemStkInfo *scode,int setcode,struct AnalyData *AnalyDatap,int ItemNum,short Type,short dataflag);
extern void  WriteCacheData(MemStkInfo *scode,int setcode,struct AnalyData *AnalyDatap,int ItemNum,short Type,short dataflag=0);

extern short GetHisTickCache(MemStkInfo * scode, long nDate, TickData *indap=NULL, short num=0);

extern short GetMulNum(short Type);

extern void GetQZData(MemStkInfo *pStkInfo,CurrStockData *pHQ,float &fGgbl,float &fInnerVal,float &fYjl);

extern BOOL RawData2CalcData(char * Code,short nSetCode,short DataType,void *pRawData,void *pCalcData,short nDataNum,NTime tFrom);
extern BOOL TxtTestFound(const char *tmpStr);
extern BOOL NewsTestFound(const char *tmpStr);
extern BOOL TestFound(const char *filename,int style);

extern int  GetComboTextList(char str[][30],CComboBox* clist,int maxcount);

extern int	TransDynaKData(int nDataNum, AnalyData* pAnalyData);

extern void GetNodeMapCount(short szstknum,short shstknum,struct MemStkInfo *pszMem,struct MemStkInfo *pshMem);
extern void GetFromXXMCfg();

extern BOOL GetCahceMinuteData(MemStkInfo *scode,struct MinuteData *pMinData,long date=0,float *pfclose=NULL);
extern int  GetCahceTickData(MemStkInfo *scode,TICKDATA *pTickData);

extern void SetDateCtrlRange(CWnd *pWnd);

extern BOOL ExecExtern(char *exepath,HWND MainhWnd,char *param=NULL,BOOL bIsWT=FALSE);

extern BOOL	CompareCwcl(CWDATA *pCwData,const char *GpCode,short setcode=-1);
extern short GetStockType ( short setcode, const char * Code );

extern BOOL RefreshAllHq(int nInterval=10*60);
extern BOOL RefreshAllStatHq(int nInterval=10*60);

extern long GetBackDate(long date);
extern long GetForwardDate(long date);

extern long GetVipDocFirstDate();
extern long GetVipDocLastDate(BOOL bLocalDate=TRUE);
extern long GetFxMinVipDocLastDate(BOOL &bFindGen);

extern BOOL AddToVipdocDayData(MemStkInfo *pStkInfo,struct AnalyData *AnalyDatap,int nData,short type);
extern BOOL AddToVipdocFxMinData(MemStkInfo *pStkInfo,struct AnalyData *AnalyDatap,int nData,short type);

extern BOOL OneStock_PH(MemStkInfo *pStkInfo);

extern void Refresh_OftenUseZb_Menu();
extern void Refresh_OftenUseZb_CmdTable();
extern void Refresh_XXM_Menu();
extern int  GetGpIndex(int nBlockFlag,int WhichType,short *indexbuffer,int nMaxNum=MAXGPNUM);
extern MemStkInfo* GetLinkedHKStock(MemStkInfo* pAStock);

extern BOOL DeleteGPFromBlock(int whichGPType,int nBlockFlag,int gpno);
extern BOOL AddGPToBlock(int blockno,CString m_CurGPCode,short m_CurSetCode);
extern void FiltOtherBlock(int blockno,CString m_CurGPCode,short m_CurSetCode);
extern void ArrangeBlockGP_Shift(int blockno,int SrcGPNo,int DestGPNo);
extern void ArrangeBlockGP_InsertBefore(int blockno,int SrcGPNo,int DestGPNo);

extern BOOL InitCalcInterface(DTopViewCalc* &pCalc);

extern void GetCorrespondingTime(SYSTEMTIME *ltime);
extern void GetDSCorrespondingTime(SYSTEMTIME *ltime);

extern DWORD	GetLastDate(DWORD nDate);
extern long		GetNowWorkMinute(BOOL &bLastDate, PTIMEINF pTimer, BYTE Market);
extern long		GetNowWorkDate(PTIMEINF pTimer, long nDataDate);
extern long		GetDSCorrespondingDate(PTIMEINF pTimer);

extern COLORREF GetItemColor(float f1,float f2);
extern CString	FormatFloat2Str(float f1,int xsflag=2,int postfix_flag=0);	//后两参数控制格式,按照指定的格式转换float成字符串
//////////////////////////////////////////////////////////////////////////
extern long		AfxStringTrimSpace(LPSTR  string);
extern short	GetCmdLine(LPSTR lpString,char*argv[],short MaxLine,char *sepstr=",");
extern BOOL		AllSameAlpha(LPSTR lpString, char a);

extern int	 GetSYSCurrentGPIndex();
extern int	 GetSYSCurrentGPSet(short *pPageGPIndex);
extern void	 GetSYSCurrentLSetInfo(LinkedSetInfo *&pLSetInfo);
extern void  Set_OP_Scheme();

extern int	 GetAllRecentSearch(RECENTSEARCH *pTmpRecentSearch);

extern char *ConvertString(char *str);
extern char *ReConvertString(char *str);
extern void MakeBlock(short *pIndex,int GPNum,int WhichBlock,BOOL bOpenHQ=TRUE);
extern void	SeveToNewBlock(vector<short> &pIndex, char *Code, char *Name, char BlockTag=-1);

extern short GetDefGridCatItems(vector<GRIDCATITEM> &AllTabItems,BOOL bAll=FALSE);
extern void	LoadCusGridTab(BOOL bIncludeDS);
extern void	SaveCusGridTab();
extern void	RefreshCusGridTab();
extern void RenameCusGridTab(int nWhich,const char *lpNewTitle);

extern void ToZst(const char* Code,short setcode,short bZst=1);
extern void ShowGpInCom(const char* Code,short setcode,BOOL bNewView=FALSE,short *pIndex=NULL,int TotalRow=-1,int CurrentNo=-1,LinkedSetInfo *pLSetInfo=NULL);
extern void OpenComView(int flag,const char* Code,short setcode,int MessageNo=0,BOOL bNewView=FALSE,short *pIndex=NULL,int TotalRow=-1,int CurrentNo=-1,LinkedSetInfo *pLSetInfo=NULL);
extern void ETSendCopyDataMessage(HWND ETHWnd,MemStkInfo *pStkInfo,struct CurrStockData * hqp);

/////////////////////////////////////////////////////////////////////////////

extern void		DSCheckSrvInfo(void);
extern BOOL		DSSaveSrvInfo(BOOL bInitEmpty);
extern void		DSWriteSrvInfo(short StkNum,PSTKINFO pStkInf);
extern void		DSWriteTimer(short TimerNum,PTIMEINF pTimer);
extern BOOL		DSLoadTimer(long TimerNum,PTIMEINF pTimer,short *pTimerMap);
extern void		StkInfo2Fz(MemStkInfo *pStock, short *TimeSec);
extern void		TimeInf2Fz(PTIMEINF pTime, short *TimeSec);
extern short	GetDSMinute(PTIMEINF pTime, BOOL bYestodayFlag);
extern long		GetLocalFileSize(LPCSTR FilePath);
extern long		GB2BIG(LPSTR strDest,LPCSTR strSrc,wchar_t*pws);

extern int		GetNextDef(void);
extern void		Num2KeyString(char *keyGuy, int nIndex);
extern void		GetDynaCusPadInfo(void);
extern int		GetCusPadTypeParam(char *padCodeName);

extern BOOL		DownDayData_AtOnce(CWnd *pWnd,char *HintStr,BOOL bShowHint=TRUE, BOOL bModal=TRUE);
extern BOOL		DSSaveMarketInfo(MKTINFO *pMktInfo,int MktNum);
extern BOOL		DSLoadMarketInfo(void);
extern int		DSGetMarketFromJc(char *dsjc);
extern int		DSGetMarketXH(BYTE Market);
extern int		DSGetMarketXHFromGuiOrder(int nGuiOrder);
extern char	*	DSGetMarketNameFromGuiSet(int nGuiSet);
extern char	*	DSGetMarketNameFromXH(int nMarketXH);

extern long		DrawHqItem(CDC*pDC,long Width,long Height,long y,long ItemNum,PHQITEM pItem,short XSFlag,float fClose,long PerSize=23,long LineHeight=8);

extern BOOL		IsZipFile(CString tmpFileName);
extern BOOL		UnZipSomeFile(const char *filename,const char *path,BOOL &bHasEmbwtFile,BOOL &bHasNewHostFile);

extern BOOL		IsETF(struct MemStkInfo *scode);
extern BOOL		MinuteTick_SpecialData(struct MemStkInfo *scode);
extern BOOL		AnalyData_SpecialData(struct MemStkInfo *scode);
/////////////////////////////////////////////////////////////////////////////
extern BOOL		BelongZXG(MemStkInfo *scodetmp);
extern void		LoadColorfulBlockGP();
extern COLORREF	GetBlockGPColor(MemStkInfo *scodetmp,COLORREF clrDefault,int nGPType=-1,int nBlock=-1);
extern BOOL		IsHligtNowVol(DWORD dwVol,MemStkInfo *pStkInfo);

extern short		GetQZType(MemStkInfo *pInfo);
extern MemStkInfo	*GetQZLinkNow(MemStkInfo *tmpInfo);
extern float		GetAbsPrice(MemStkInfo *pStkInfo,DWORD dwPrice,short XSFlag);
extern DWORD		GetRelPrice(MemStkInfo *pStkInfo,float fPrice,short XSFlag);
extern void			DynaZoom(CWnd *pWnd,CRect rect1,CRect rect2);

extern BOOL		CanProcesLocalData(CWnd *pWnd, int nType=0);
extern int		GetListRefreshTime();
extern int		GetGPRefreshTime();

extern void		MarkStockFlag(MemStkInfo *pInfo, char nFlag);
extern	void	MarkStockTipFlag(MemStkInfo *pInfo, CString lpString);
extern BOOL		GetToolText(CString &strRes, MemStkInfo *pStock,BOOL bGetHead=TRUE);
extern COLORREF GetMarkColor(int codeGroup);

extern BOOL		HasGPInBlockAndContinue(int nBlockNo);
extern BOOL		CanShowZangsu(struct MemStkInfo *pStkInfo);
extern void		GoToHelp();
extern void		Modify_CorpWebMenu();

extern BOOL		CanUseEmbedWTFunc(HWND m_hWnd);

extern BOOL		CanLevel2();
extern BOOL		CanAdvLevel2();
extern BOOL		CanUseVipHost();
extern BOOL		CanUseLevel2Host();
extern BOOL		IsLevel2Property(MemStkInfo *tmpInfo);
extern BOOL		IsSeatProperty(MemStkInfo *tmpInfo);

extern BOOL		GetSpecialTextColor(int n,COLORREF &cor);
extern void		SetFYGroupActive(int nIndex);
extern BOOL		CheckFYGroupActive(UINT nIndex);
extern BOOL		PeekMsg();

extern BOOL		IsRemoteGet(LinkedSetInfo *pLSetInfo);
extern void		Func_DelFromBlock(int tempCurrentNo,BOOL bNotify);
extern void		Func_RightAddblock(int tempCurrentNo,BOOL bNotify);
extern void		Func_RightAddZxg(int tempCurrentNo,BOOL bNotify);

extern BOOL		SureAutoPage(CWnd *pView);
extern BOOL		FiltTabKeyCom();
extern CSize	DrawMarkFlag(MemStkInfo *pStock,CDC *pDC,int x,int y,int left,int top,CWnd *pWnd);
extern void		Format_PostFix(int postfix_flag,BOOL bPositive,char *buf,int buflen);
extern void		Hlzq_Encrypt(char *szAccount,char *szEnAccount);
extern void		Zxjt_Base64Info(CString & strParam);
extern void		TransferToRealURL(CString & url,struct MemStkInfo *pInfo);
extern void		OpenCusWebPage(CWnd *pWnd,CString tmpInfoURL,BOOL bInsideOpenPage);	

extern int		GetColorFromReason(short nReason,short InOutFlag);
extern BOOL		ProcessKeyGuy(MSG *pMsg,CWnd *pWnd,BOOL &KeyGuyFlag);
extern BOOL		InStatusBarGP(MemStkInfo *pStkInfo);
extern BOOL		GetFundBaseData();
extern CString	FormatLongString(CString strInString, short OutDataFormat);

extern BOOL		GetHYCodeName(char *strAppFlag, char *Name, char *Code);
extern BOOL		IsIdleStatus();
extern void		GetResPadBit(CView *pView, HICON &bitmap);

extern BOOL		RectPtIn(RECT rc,CPoint pt);
extern void		SetRectZero(RECT & rc);

extern void		SetMenuBreak(int i,CMenu *pMenu);
extern void		SwitchMenuCallBack(CWnd *pWnd,CWnd *ProcWnd,int left,int top,int right,int bottom,UINT message,UINT menuid,long whichmenu);

extern UINT		CalcPeriodToTdxPeriod(UINT nCalcPeriod);
extern UINT		TdxPeriodToCalcPeriod(UINT nTdxPeriod);
extern UINT		KeyOrderToTdxPeriod(UINT nOrder);
extern UINT		TdxPeriodToKeyOrder(UINT nTdxPeriod);
extern char *	GetTdxPeriodStr(UINT nTdxPeriod,char *InvalidStr);
extern BOOL		IsValidPeriod(UINT nTdxPeriod);

extern BOOL		BeUserBlock(int WhichGPType,int nBlockFlag);
extern BOOL		BeBlock(int WhichGPType);

extern void		DrawNoABHintInfo(CDC *pDC,CRect rc);
extern BOOL		CheckPointPos(CWnd *pDlg,int nItemID,CPoint point);

extern int		WebContentDownload(char *strUrl, char *cFileName, char *Header_Ex=NULL);
extern BOOL		UnZipSomeFile(const char *filename,const char *path,BOOL bBak);

extern int		GetExternWhichUnit(int nWhichUnit,int nInCom2);
extern int		GetGlGpIndex(int nCurGP,short *indexbuffer,int nMaxNum);
extern int		GetLcIdxGP(const char *strCode,short *indexbuffer,int nMaxNum);
extern BOOL		MakeWindowTransparent(HWND hWnd, unsigned char factor);

extern int		UrlDecode(const char *s,char *d);
extern int		GetMessageNoFromZB(const char *Value,BOOL bSpecialZB);
//////////////////////////////////////////////////////////////////////////
#define MAX_INBLOCK_NUM	(400+TOTAL_HY_NUM+TOTAL_DY_NUM+200+100+100)
typedef struct
{
	char	strBlockName[50];
	short	gpnum;
	short	nBlockNo;
}INBLOCKINFO;

extern int GetInBlockInfo(int nCurGP,INBLOCKINFO *pBlockInf,BOOL bHasGLGP=FALSE);
extern void ReadDataTickStat(MemStkInfo *pTarStock, TICKSTAT *pTickStat);
extern void ReadDataTickStat_All();

extern DWORD tdx_filelength(FILE * ftmp);

extern void	CalcDXIndex1(MemStkInfo *pStock);
extern void	CalcDXIndex2(MemStkInfo *pStock);
extern void	CalcDXIndex3(MemStkInfo *pStock);

extern float	Amp(float fInput, float fFlag);

long	GetCmdLine(LPSTR lpString, vector<char*> &argv, long nMaxLine, char *sepstr, short nFlag=0);
long	GetCmdLine(LPSTR lpString, vector<char*> &argv, long nMaxLine, char *strHead, char *strTail);
extern int WebPostDownload(char *strUrl, char *strContent, char *strCookie, char *cFileName, char *Header_Ex=NULL);

extern int GetHistoryData_au(vector<AnalyData> &HisData, char *Code);
extern int GetHistoryData_bitc(vector<AnalyData> &HisData, double &dMultyply, char *strFile);
extern int GetHistoryData_binance(vector<AnalyData> &HisData, double &dMultyply, time_t &ttLast, char *strFile);

extern void	GetDataItems_Bit(char *currencyPair, int nPerSeed, vector<AnalyData> &OutCome);
extern void	GetDataItems_Au(char *GPCode, char *DataPeriod, char *DataInterval, char *DataNum, int nPerItem, vector<AnalyData> &OutCome);

extern void	GetASXListResults();
extern void	GetHisDataFromASX_au(char *GpCode, vector<AnalyData> &HisData);

extern double	CalcBestAverFrom1(vector<double> DataVal, double fRatioFlag, int nMinCnt=-1);
extern void		FiltBestFrom(double dMinDistFlag, vector<int> &Selected, map<double, int, greater_equal<double> > AllItems);

extern bool GetShellPath(char *Src,char *ShellPath);

#endif
