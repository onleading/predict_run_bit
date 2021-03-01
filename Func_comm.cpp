#include "stdafx.h"
#include "func_comm.h"
#include "zlib.h"
#include "MainFrm.h"
#include "GridView.h"
#include "ComView2.h"
#include "loaddll.h"

//g_nMainID[WindowMacro]的高6位保存了接收窗口类型
short MainIDPlus(short& MainID,short WindowMacro)
{
	if(MainID == 0)	MainID = WindowMacro*0x0400;
	unsigned short tmpAddID = MainID & 0x03FF;
	tmpAddID++;
	if( tmpAddID > 0x03FF )	tmpAddID = 1;
	MainID = WindowMacro*0x0400 + tmpAddID;
	return MainID;
}

short GetWinFromMainID(short MainID)
{
	unsigned short tmpMain = MainID&0xFC00;
	return tmpMain >> 10;
}

short SetComAssisID(short whichunit,short whichreq)
{
	return (whichunit+1)*256+whichreq;
}

void GetComAssisID(short AssisID,short &whichunit,short &whichreq)
{
	whichunit = AssisID/256-1;
	whichreq = AssisID%256;
}

short GetReqType(short AssisID)
{
	if(AssisID == HQ_ASSISID||AssisID == HQ_ASSISID+1)
		return HQ_REQTYPE;
	if(AssisID >= 40 && AssisID <=99 )
		return DIRECT_REQTYPE;
	if(AssisID >= 100 && AssisID <=255 )
		return RELAY_REQTYPE;
	return BUFFER_REQTYPE;
}

BOOL ReceiveBytes(CAsyncSocket *pSock,char *buff,short len)
{
	int		nBytesRead,nBytes=0;
	time_t   t;
	t = time ( NULL );
	while(nBytes < len)
	{
		PeekMsg();
		if(time ( NULL ) - t >= 15) //超时为15秒
			return FALSE;
		nBytesRead = pSock->Receive(buff+nBytes,len);
		if(nBytesRead <= 0)
		{
			if(GetLastError()!=WSAEWOULDBLOCK)	
				return FALSE;
			else
				continue;
		}
		nBytes+=nBytesRead;	
	}
	return TRUE;
}

//在某个Socket上发送一个缓冲区。返回-1,表示失败，0表示成功
int SendToSocket(CAsyncSocket *pSocket,char *buf,int len)
{
	int nLeft,idx,ret;
	nLeft = len;
	idx = 0;
	while(nLeft > 0)
	{
		ret = pSocket->Send(buf+idx,nLeft,0);
		if(ret<0 && GetLastError()==WSAEWOULDBLOCK)
		{
			PeekMsg();
			continue;			
		}
		else if(ret<0)
			return -1;
		nLeft -= ret;
		idx += ret;
	}
	return 0;
}

int ReceiveHQData(SOCKET m_sock,char *pReceive)
{
	ANSHEADER	Header;
	int		nBytesRead;
	DWORD	nBytes = 0;
	short	ansheadlen = sizeof(ANSHEADER);

	fd_set fdR;
	struct timeval timeout;
	timeout.tv_sec=7;
	timeout.tv_usec=0;
	
	while(1)
	{
		//采用select方式来判断超时
		FD_ZERO(&fdR); 
		FD_SET(m_sock,&fdR);
		if(select(m_sock+1,&fdR,NULL,NULL,&timeout)!=1)
			return 0;
		
		nBytesRead = recv(m_sock,pReceive+nBytes,ANSBUFFER_LEN+20-nBytes,0);
		if(nBytesRead<=0)
			return 0;
		nBytes+=nBytesRead;
		if(nBytes < ansheadlen)//一直读完
			continue;
		memcpy(&Header,pReceive,ansheadlen);
		if(nBytes < ansheadlen+Header.PacketLen)
			continue;
		memcpy(pReceive,pReceive+ansheadlen,Header.PacketLen);
		//解压
		if (Header.Info.Compressed)
		{
			DWORD nRawLen = Header.RawLen;
			char  * pRawBuf = new char[nRawLen];
			if (!pRawBuf)	return 0;
			if(!uncompress((BYTE *)pRawBuf,&nRawLen,(BYTE *)pReceive,Header.PacketLen))
			{
				memcpy(pReceive,pRawBuf,nRawLen);
				Header.RawLen = nRawLen;
			}
			delete pRawBuf;
		}
		return Header.RawLen;
	}
	return 0;
}

BOOL AsynConnectResult(BOOL bIsComm3,WPARAM wParam)
{	
	if(bIsComm3 && g_pComm3) //第三方市场的通讯
	{
		if(wParam>=11 || wParam==1) //连接成功
		{
			if(wParam>=11)
			{
				if(!g_pComm3->AfterConnectProxy())
					return FALSE;
			}
			if(g_ProxyInfo.bTdxProxy)
			{
				if(!g_pComm3->ConnectTdxProxy())
					return FALSE;
			}
			return TRUE;
		}
		else
		{
			if(g_ProxyInfo.bTdxProxy)
				g_pComm3->m_sError="不能连接到绿色通道!";
			else
				g_pComm3->m_sError="不能连接到主站!";
		}
	}
	if(!bIsComm3 && g_pComm) //常规市场的处理
	{		
		if(wParam>=11 || wParam==1) //连接成功
		{
			if(wParam>=11)
			{
				if(!g_pComm->AfterConnectProxy())
					return FALSE;
			}
			if(g_ProxyInfo.bTdxProxy)
			{
				if(!g_pComm->ConnectTdxProxy())
					return FALSE;
			}
			return TRUE;		
		}
		else
		{
			if(g_ProxyInfo.bTdxProxy)
				g_pComm->m_sError="不能连接到绿色通道!";
			else
				g_pComm->m_sError="不能连接到主站!";
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
#include "DSConnectDlg.h"
BOOL	DSConnectWhenNeedHq(BOOL bAtOnce)
{
	if(g_bHasDS && OnLine && !DSOnLine && !g_pDSConnectWnd)
	{		
		//连接扩展行情
		g_pDSConnectWnd = g_pConnectWnd = new CDSConnectDlg(AfxGetMainWnd());
		((CDSConnectDlg *)g_pConnectWnd)->m_bAtOnce = bAtOnce;
		int nRet = ((CDialog *)g_pConnectWnd)->DoModal();
		TDEL(g_pConnectWnd);
		g_pDSConnectWnd = g_pConnectWnd = NULL;
		//如果连接成功
		if(nRet==IDOK)
		{
			DSOnLine = TRUE;
			g_pStockIO->AfterDSLoginSuccess();
			LoadAndResetGridTab();
			return TRUE;
		}
	}
	return FALSE;
}

void LoadAndResetGridTab()
{
	LoadCusGridTab(TRUE);
	for(int i = 0;i<theViewManager.GetWindowNum();i++)
	{
		CView *tempView = (CView *)theViewManager.arViews[i];
		if( tempView->IsKindOf(RUNTIME_CLASS(CGridView)) )
		{
			((CGridView *)tempView)->SendMessage(UM_RESETGRIDTAB);
			((CGridView *)tempView)->SendMessage(UM_GRIDREINIT);
		}
		else if(tempView->IsKindOf(RUNTIME_CLASS(CComView2)))
			((CComView2 *)tempView)->SendMessage(UM_RESETGRIDTAB);
	}
}

long l_IdxOffset=0,l_TimerOffset=0;
BOOL OnDSLoginGetDataACK(CWnd *m_pLogoWnd,short WindowMacro,BOOL & m_bHasPackTimer3,short & m_nMainID,CProgressCtrl *pProgress,char *pInfo)
{
	if(m_nMainID!=g_AnsHeader3.MainID) 
		return FALSE;
	if(m_bHasPackTimer3)
	{
		//第一数据包的超时时钟的释放
		m_pLogoWnd->KillTimer(0x201);
		m_bHasPackTimer3=FALSE;
	}
	switch(g_AnsHeader3.AssisID)
	{
	case DS_LOGIN_ASSISID:			//扩展主站的登录
		{
			char errmsg[100];
			BOOL bRet=g_pGetData->DSLoginACK(errmsg);
			if(!bRet)
			{
				strcpy(pInfo,errmsg);
				if(g_pComm3)
					g_pComm3->Disconnect();
				return FALSE;
			}
			g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
			g_nAssisID = DS_MKT_INFO_ASSIDID;
			strcpy(pInfo,"请求扩展行情服务...");
			g_pGetData->DSGetMktInfo();
		}
		break;
	case DS_MKT_INFO_ASSIDID:		//取市场信息
		if(g_pGetData->DSGetMktInfoACK())
		{
			g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
			g_nAssisID = DS_SERVER_INFO_ASSISID;
			g_pGetData->DSGetSrvInfo();
		}
		else
		{
			if(g_pComm3)
				g_pComm3->Disconnect();
			return FALSE;
		}
		break;
	case DS_SERVER_INFO_ASSISID:	//取服务器信息
		g_pGetData->DSGetSrvInfoACK(&g_iDsSrvInfo);
		DSCheckSrvInfo();
		strcpy(pInfo,"请求商品信息...");
		if(g_bDSNeedIdx)
		{
			l_IdxOffset = 0;
			g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
			g_nAssisID = DS_CODE_LIST_INFO_ASSISID;
			pProgress->SetRange(0,g_iDsSrvInfo.IndexNum);
			pProgress->SetPos(0);
			g_pGetData->DSGetStkInf(l_IdxOffset=0,500);
			break;
		}
		else
		{
			if(g_bDSNeedTimer||g_bDSNeedIdx)
			{
				l_TimerOffset = 0;
				g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
				g_nAssisID = DS_MARKET_TIMER_ASSIDID;
				pProgress->SetRange(0,g_iDsSrvInfo.TimerNum);
				pProgress->SetPos(0);
				g_pGetData->DSGetMarketTimer(l_TimerOffset=0,500);
				break;
			}
		}
		return TRUE;
	case DS_CODE_LIST_INFO_ASSISID:	//取码表信息
		{
			STKINFO	StkInf[500]={0};
			short IdxNum = g_pGetData->DSGetStkInfACK(StkInf);
			if(l_IdxOffset==0) 
				DSSaveSrvInfo(TRUE);	//写空的文件头
			if(IdxNum) 
				DSWriteSrvInfo(IdxNum,StkInf);
			if(IdxNum<500)
			{
				DSSaveSrvInfo(FALSE);	//将真正的文件头写入
				if(g_bDSNeedTimer||g_bDSNeedIdx)
				{
					l_TimerOffset = 0;
					g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
					g_nAssisID = DS_MARKET_TIMER_ASSIDID;
					pProgress->SetRange(0,g_iDsSrvInfo.TimerNum);
					pProgress->SetPos(0);
					g_pGetData->DSGetMarketTimer(l_TimerOffset=0,500);
					break;
				}
			}
			else
			{
				l_IdxOffset += IdxNum;
				g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
				g_nAssisID = DS_CODE_LIST_INFO_ASSISID;
				pProgress->SetPos(l_IdxOffset);
				sprintf(pInfo,"请求商品信息%3d%%",l_IdxOffset*100/g_iDsSrvInfo.IndexNum);
				g_pGetData->DSGetStkInf(l_IdxOffset,500);
				break;
			}
		}
		return TRUE;
	case DS_MARKET_TIMER_ASSIDID:	//取时间配置信息
		{
			TIMEINF iTimer[500]={0};
			WORD TimerNum = g_pGetData->DSGetMarketTimerACK(iTimer);
			if(TimerNum) 
				DSWriteTimer(TimerNum,iTimer);
			if(TimerNum==500)
			{
				l_TimerOffset += TimerNum;
				g_nMainID = MainIDPlus(m_nMainID,WindowMacro);
				g_nAssisID = DS_MARKET_TIMER_ASSIDID;
				pProgress->SetPos(l_TimerOffset);
				sprintf(pInfo,"请求配置信息%3d%%",l_TimerOffset*100/g_iDsSrvInfo.TimerNum);
				g_pGetData->DSGetMarketTimer(l_TimerOffset,500);
				break;
			}
		}
		return TRUE;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////////
// 后台处理线程
////////////////////////////////////////////////////////////////////////////////

HANDLE	UserThreadHandle = NULL;
HANDLE	QsDetectThreadHandle = NULL;
//用于检测破解工具
BOOL l_bFindHackToolsProcess=FALSE;
BOOL CALLBACK EnumHackProc(HWND hwnd,LPARAM lParam)
{
	if(hwnd == 0)
		return FALSE;
	char tempTitle[256];
	GetWindowText(hwnd,tempTitle,255);
	if( strncmp("WPE ",tempTitle,4) == 0 ||
		strncmp("0lly",tempTitle,4) == 0 ||
		strncmp("Olly",tempTitle,4) == 0
	  )
	{
		l_bFindHackToolsProcess=TRUE;
		return FALSE;
	}
	return TRUE;
}

BOOL DetectHackProc()
{
	l_bFindHackToolsProcess=FALSE;
	EnumWindows((WNDENUMPROC)EnumHackProc,0);
	if(l_bFindHackToolsProcess)
		return TRUE;
	return FALSE;
}
//取券商主站的相关信息等

#include "autoupinfo.h"

DWORD WINAPI QsDetectThread(LPVOID lParam)
{
	CMainFrame *pMainWnd = (CMainFrame *)lParam;

	CString strAddress="";
	UINT nPort=7709;
	if(!GetPrimaryHQServerInfo(strAddress,nPort,FALSE))
	{
		QsDetectThreadHandle=NULL;
		return 0;
	}
	long		tick1 = GetTickCount();
	SOCKET		m_hq_sock;
	char		errMsg[255]={0};
	if(!DirectConnect(m_hq_sock,g_ProxyInfo,strAddress,nPort,tick1,errMsg))
	{
		//第一个连接不了，跳下一个
		if( !GetPrimaryHQServerInfo(strAddress,nPort,TRUE) ||
			!DirectConnect(m_hq_sock,g_ProxyInfo,strAddress,nPort,tick1,errMsg))
		{
			QsDetectThreadHandle=NULL;
			return 0;
		}
	}
	//开始收发数据
	char		sockbuff[ANSBUFFER_LEN+20];
	REQHEADER	Header;
	//通告从普通主站来
    struct user_urgent_req * reqp = ( struct user_urgent_req * ) (sockbuff+sizeof(REQHEADER));
	memset(reqp,0,sizeof(struct user_urgent_req));
	memset(&Header,0,sizeof(REQHEADER));
	Header.Info.Version		= 0;
	Header.Priority			= 0;
	Header.PacketLen = Header.RawLen = sizeof(struct user_urgent_req);
	reqp->req		= USER_URGENT_REQ;
	if(g_OEM.bFormatAndUseYYBID)
	{
		CString tmpStr;
		tmpStr.Format("%04d",g_JyInfo.yybid);
		strcpy(reqp->username,tmpStr);
	}
	memcpy(sockbuff,&Header,sizeof(REQHEADER));
	if(SendToSocket(m_hq_sock,sockbuff,sizeof(REQHEADER)+sizeof(struct user_urgent_req))==-1)
	{
		closesocket(m_hq_sock);
		m_hq_sock = INVALID_SOCKET;
		QsDetectThreadHandle=NULL;
		return 0;
	}
	if(sizeof(struct urgent_ans)<=ReceiveHQData(m_hq_sock,sockbuff))
	{
		struct urgent_ans *ansp = (struct urgent_ans *)sockbuff;
		struct Cfg_Struct CfgStruct={0};
		CFile UrgentFile;
		if(1 == ansp->valid)
		{
			if(UrgentFile.Open(g_WSXHStr+"tmp\\bg_urgent.dat",CFile::modeCreate|CFile::modeWrite))
			{
				CfgStruct.EndTime = ansp->ldate;
				if(ansp->sourcelen>0)
					memcpy(CfgStruct.UrgentSource,ansp->buf+ansp->advertlen,ansp->sourcelen);
				CfgStruct.UrgentSource[30]=0;
				if(ansp->contentlen>0)
					memcpy(CfgStruct.UrgentContent,ansp->buf+ansp->advertlen+ansp->sourcelen,min(2048,ansp->contentlen));
				CfgStruct.UrgentContent[2048] = 0;
				UrgentFile.Write(&CfgStruct,sizeof(struct Cfg_Struct));
				UrgentFile.Close();
				if(pMainWnd)
					pMainWnd->PostMessage(UM_SHOWURGENT_MESSAGE,URGENT_BG);
			}
		}	
	}
	//升级也从普通主站来
	if(g_GC.bAutoupFromLevel1)
	{
		struct get_passport_req * reqp = ( struct get_passport_req * ) (sockbuff+sizeof(REQHEADER));
		memset(reqp,0,sizeof(struct get_passport_req));
		memset(&Header,0,sizeof(REQHEADER));
		Header.Info.Version		= 0;
		Header.Priority			= 0;
		Header.PacketLen = Header.RawLen = sizeof(struct get_passport_req);
		reqp->req			= GET_PASSPORT_NREQ;
		reqp->clientver		= atof(g_szMainExeVersion);
		reqp->AutoupOrder	= g_GC.WhichAutoUpInfo-1;
		strcpy(reqp->QsId,g_Scheme_King.QSID);
	#ifdef ZSZQJZ
		strcpy(reqp->ProductID,"招商金钻");
	#endif
		memcpy(sockbuff,&Header,sizeof(REQHEADER));
		if(SendToSocket(m_hq_sock,sockbuff,sizeof(REQHEADER)+sizeof(struct get_passport_req))==-1)
		{
			closesocket(m_hq_sock);
			m_hq_sock = INVALID_SOCKET;
			QsDetectThreadHandle=NULL;
			return 0;
		}
		if(sizeof(struct get_passport_ans)>ReceiveHQData(m_hq_sock,sockbuff))
		{
			closesocket(m_hq_sock);
			m_hq_sock = INVALID_SOCKET;
			QsDetectThreadHandle=NULL;
			return 0;
		}
		struct get_passport_ans *ansp = (struct get_passport_ans *)sockbuff;
		memset(&g_NeedAutoupInfo,0,sizeof(struct NeedAutoupInfo));
		if(ansp->Sucflag)
		{		
			g_NeedAutoupInfo.bNeedAutoup=ansp->bNeedAutoup;
			g_NeedAutoupInfo.ThisVer=ansp->ThisVer;
			if(ansp->ThisVer<0.001)
				g_NeedAutoupInfo.bNeedAutoup=0;		
			strcpy(g_NeedAutoupInfo.AutoupURL,ansp->AutoupURL);		
			strncpy(g_NeedAutoupInfo.Descripton,ansp->Descripton,2*1024-2);		
			g_NeedAutoupInfo.Descripton[2*1024-2]=0;
			g_NeedAutoupInfo.bAutoupFromHQHost=ansp->bAutoupFromHQHost;
		}
		WriteAutoupInfo(&g_NeedAutoupInfo);
		if(pMainWnd)
			pMainWnd->PostMessage(UM_PROCESSAUTOUPINFO,0,0);
	}
	closesocket(m_hq_sock);
	m_hq_sock = INVALID_SOCKET;	
	QsDetectThreadHandle=NULL;
	return 0;
}

#include "Hint2Dlg.h"
#include "waitdlg.h"
extern int g_Hookcmd;

void AfterSocketClose(int nCommType)
{
	if(g_bReLoginStatus) 
		return;
	//Sleep0.1s以防重连太快,造成资源冲击
	Sleep(100);
	if(nCommType==1)
	{
		DSOnLine = FALSE;
		if(g_pDSConnectWnd)
			g_pDSConnectWnd->PostMessage(UM_NETWORK_ERR,0,0);
		else
		{
			if(g_nDSAutoConnectFlag!=AUTOCONNECT_RECONNECT && g_pMainWin)
				g_pMainWin->PostMessage(UM_AUTOCONNECT,0,1);
		}
		g_nDSAutoConnectFlag = AUTOCONNECT_RESET;
		return;
	}
	g_nAutoConnectFlag = AUTOCONNECT_RESET;
	if(!g_bFirstStep)
	{
		if(g_bYesterdayFlag==1 && Is_HQInit_Time())
		{
			if(g_pMainWin)
			{
				g_bAuthSuccess = FALSE; //又置为需要认证状态
				g_pMainWin->PostMessage(UM_HQCONNECTTIMER_MESSAGE,0,0);
				if(g_pMainWin->IsWindowVisible()) //防止在隐藏状态出现此对话框
				{
					CWaitDlg dlg;
					dlg.m_strContent=_F("行情连接被断开!\n原因:主站正在初始化今天数据,\n系统会过几分钟后自动重新进入,请稍等!");
					dlg.m_nWaitSeconds=5*60;
					dlg.DoModal();
				}
			}
		}
		else
		{
			if(g_pMainWin)
			{
				CWnd *pActiveWnd = g_pMainWin->GetLastActivePopup(); //关闭最近一次激活的窗口
				if(!pActiveWnd) pActiveWnd=g_pMainWin;

				BOOL bTry = FALSE;
				if(!g_bDowning && g_bUseAutoConnect)
					bTry = TRUE;	//如果使用自动重连，则不断地尝试
				else if(g_pMainWin->IsWindowVisible())
				{
					if(LayoutFlag && g_GH.Roll.bShow && g_GH.Roll.bFloat)
					{
						if(g_Hookcmd==SW_SHOW)	//非隐藏状态(老板键)
							TDX_MessageBox(pActiveWnd->m_hWnd,"行情连接被断开,进入脱机运行状态!",MB_OK|MB_ICONQUESTION);
					}
					else if(g_bSelftPHing)
						;
					else if(g_bDowning)
					{
						TDX_MessageBox(pActiveWnd->m_hWnd,"行情连接被断开,下载失败,请重新连接行情主站后进行下载!",MB_OK|MB_ICONQUESTION);
						bTry=TRUE;
					}
					else
					{
						CHint2Dlg dlg;
						dlg.m_strHint = _F("行情连接被断开,是否尝试重新连接主站?");						
						dlg.m_strCheck = _F("掉线不提示,在后台自动重连几次");
						dlg.m_bCheck = g_bUseAutoConnect;
						bTry = (dlg.DoModal()==IDOK);
						g_bUseAutoConnect = dlg.m_bCheck;
					}
				}
				if(LayoutFlag && bTry) //自动弹出,如果有HisDlg,再连接会有问题
				{
					g_bDowning = FALSE;
					pActiveWnd->SendMessage(WM_COMMAND,IDCANCEL,0); //将此模态对话框消掉
					if(g_pHisDlg)
						g_pHisDlg->SendMessage(WM_COMMAND,IDCANCEL,0);
					if(!g_bUseAutoConnect)
					{
						g_pMainWin->PostMessage(WM_COMMAND,ID_CONNECT);
						g_nTmpDirectConnectHq = TRUE;
					}
					else
					{
						g_nAutoConnectNum = 0;	//自动重连次数复位
						g_pMainWin->PostMessage(UM_AUTOCONNECT,0,0);
					}
				}
			}
		}
	}
	else
	{
		if(g_pConnectWnd)
			g_pConnectWnd->SendMessage(UM_LOGOENBALE);
		if(g_pMainWin)
		{
			TDX_MessageBox(g_pMainWin->m_hWnd,"连接失败!可能原因:\n1.与主站端不匹配的客户端!\n2.网络故障,不能连接!\n3.主站端连接数已满,请选择另一主站.\n4.主站端太忙,请稍后再试.\n5.主站端不支持该客户端拨入.\n请检查通讯设置.",MB_OK|MB_ICONINFORMATION);
			if(g_pConnectWnd) g_pConnectWnd->PostMessage(UM_SWITCHNEXTHOST);
		}
	}
}

void SendHQStatReq(short *lpnStkIndex,int nCnt,int nType)
{
	char *	pTranBuff = new char[REQBUFFER_LEN];
	MemStkInfo *pStkInfo;
	SUMIMGSTAT	ImgStat;
	SUMTCKSTAT	TckStat;
	int			tmpLen=1+SH_CODE_LEN+sizeof(DWORD);
	int			i = 0,gpnum = 0;
	while(i < nCnt)
	{
		pStkInfo = (*g_pStockIO)[lpnStkIndex[i]];
		if(pStkInfo)
		{
			memcpy(pTranBuff+gpnum*tmpLen,&(pStkInfo->setcode),1);
			memcpy(pTranBuff+gpnum*tmpLen+1,pStkInfo->Code,SH_CODE_LEN);
			g_pStockIO->ReadCurrStatData(pStkInfo,&ImgStat,&TckStat);
			if(nType==IMG_STAT_HQ)
				memcpy(pTranBuff+gpnum*tmpLen+1+SH_CODE_LEN,&ImgStat.RefreshNum,sizeof(DWORD));
			else
				memcpy(pTranBuff+gpnum*tmpLen+1+SH_CODE_LEN,&TckStat.CJBS,sizeof(DWORD));
			gpnum++;
		}
		i++;
	}
	if(gpnum>0)
		g_pGetData->GetStatData(pTranBuff,gpnum,(nType==IMG_STAT_HQ));
	delete pTranBuff;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
BOOL SendData(int & sock,char *buf,int len)
{
	int nLeft,idx,ret;
	nLeft = len;
	idx = 0;
	while(nLeft > 0)
	{
		ret = send(sock,buf+idx,nLeft,0);
		if(ret<0 && GetLastError()==WSAEWOULDBLOCK)
		{
			PeekMsg();
			continue;
		}
		else if(ret<0)
			return FALSE;
		nLeft -= ret;
		idx += ret;
	}
	return TRUE;
}

BOOL	RecvData(int & sock,char *buf,long len)
{
	int nMaxTrys = 3;
	int nTrys = 0;
	for(long Offset=0,RcvSize=0;len>0;)
	{
		RcvSize = recv(sock,buf+Offset,len,0);
		if(RcvSize==0)
		{
			if(++nTrys>nMaxTrys)
				return FALSE;
		}
		else if(RcvSize==SOCKET_ERROR)
		{
			if(GetLastError()==WSAEWOULDBLOCK)
			{
				PeekMsg();
				continue;
			}
			return FALSE;
		}
		Offset += RcvSize;
		len -= RcvSize;
	}
	return TRUE;
}