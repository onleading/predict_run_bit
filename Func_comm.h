#ifndef   __FUNC_COMM_H__
#define   __FUNC_COMM_H__

extern short MainIDPlus(short& MainID,short WindowMacro);
extern short GetWinFromMainID(short MainID);
extern short SetComAssisID(short whichunit,short whichreq);
extern void  GetComAssisID(short AssisID,short &whichunit,short &whichreq);
extern short GetReqType(short AssisID);

extern BOOL ReceiveBytes(CAsyncSocket *pSock,char *buff,short len);
extern int	SendToSocket(CAsyncSocket *pSocket,char *buf,int len);
extern int  ReceiveHQData(SOCKET m_sock,char *pReceive);

extern BOOL	AsynConnectResult(BOOL bIsComm3,WPARAM wParam);

extern BOOL	DSConnectWhenNeedHq(BOOL bAtOnce);
extern void LoadAndResetGridTab();
extern BOOL OnDSLoginGetDataACK(CWnd *m_pLogoWnd,short WindowMacro,BOOL & m_bHasPackTimer3,short & m_nMainID,CProgressCtrl *pProgress,char *pInfo);

extern BOOL DetectHackProc();

extern HANDLE	UserThreadHandle;;
extern HANDLE	QsDetectThreadHandle;;

extern DWORD WINAPI SendLivePacket(LPVOID lParam);
extern DWORD WINAPI QsDetectThread(LPVOID lParam);

extern void AfterSocketClose(int nCommType);
extern void SendHQStatReq(short *lpnStkIndex,int nCnt,int nType);

extern BOOL SendData(int & sock,char *buf,int len);
extern BOOL	RecvData(int & sock,char *buf,long len);

#endif
