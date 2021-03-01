#include "StdAfx.h"
#include "levin.h"

struct Extern_LevinCfg	g_ExternLevinCfg;
CImageList	g_LevinImage;
BOOL g_bHasAdvJy = FALSE;	//�Ƿ��������µ�����
BOOL g_bShowToken = TRUE;	//�Ƿ���ʾ�����µ���ͼ��

BOOL IsSameWTBH(struct LevinGN_WTCJListANS *pList1,struct LevinGN_WTCJListANS *pList2) //�Ƿ�����ͬ��ί�б��
{
	if(!pList1 || !pList2) return FALSE;
	if(strcmp(pList1->YybID,pList2->YybID)==0 && 
	   strcmp(pList1->Zjzh,pList2->Zjzh)==0 && 
	   strcmp(pList1->Gddm,pList2->Gddm)==0 && 
	   strcmp(pList1->Wtbh,pList2->Wtbh)==0
	  )
		return TRUE;
	return FALSE;
}

BOOL IsSameZjZh(struct LevinGN_ZhInfoANS *pList1,struct LevinGN_GPListANS *pList2) //�Ƿ�����ͬ���ʽ��ʻ�
{
	if(!pList1 || !pList2) return FALSE;
	if(strcmp(pList1->YybID,pList2->YybID)==0 && 
	   strcmp(pList1->Zjzh,pList2->Zjzh)==0
	  )
	  return TRUE;
	return FALSE;
}

BOOL GetWTList(struct MemStkInfo *pStkInfo,int & nWTNum,struct LevinGN_WTCJListANS *pWTList)
{
	nWTNum = 0;
	if(!g_bHasAdvJy || !TC_LevinGN_)
		return FALSE;
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	if(!(dwStatus&JY_LOGINED))
		return FALSE;
	struct LevinGN_ListREQ tmpReq;
	memset(&tmpReq,0,sizeof(struct LevinGN_ListREQ));
	//�õ�ί����Ϣ
	tmpReq.Setcode=pStkInfo->setcode;
	strcpy(tmpReq.Code,pStkInfo->Code);
	tmpReq.bTodayReq=1;
	tmpReq.lStartDate=tmpReq.lEndDate=g_lOpenRq;
	int tmpOut=MAX_WTCJLIST_NUM*sizeof(struct LevinGN_WTCJListANS);
	if(TC_LevinGN_(LEVINGN_WTLIST,&tmpReq,sizeof(tmpReq),pWTList,tmpOut,0))
	{
		if(tmpOut>0)
			nWTNum = tmpOut/sizeof(struct LevinGN_WTCJListANS);
		else 
			nWTNum = 0;
	}
	nWTNum=min(MAX_WTCJLIST_NUM,nWTNum);
	nWTNum=max(nWTNum,0);
	//�õ��ɽ���Ϣ
	int nCJNum = 0;
	struct LevinGN_WTCJListANS *pCJList = new struct LevinGN_WTCJListANS[MAX_WTCJLIST_NUM];
	tmpOut=MAX_WTCJLIST_NUM*sizeof(struct LevinGN_WTCJListANS);
	if(TC_LevinGN_(LEVINGN_CJLIST,&tmpReq,sizeof(tmpReq),pCJList,tmpOut,0))
	{
		if(tmpOut>0)
			nCJNum = tmpOut/sizeof(struct LevinGN_WTCJListANS);
		else 
			nCJNum = 0;
	}
	nCJNum=min(MAX_WTCJLIST_NUM,nCJNum);
	nCJNum=max(nCJNum,0);
	BOOL bHasCancel=FALSE;
	for(int i=0;i<nWTNum;i++) //��������ȫ���ɽ������ֳɽ�����Ϣ
	{		
		pWTList[i].lCjVol=0;
		pWTList[i].fCjPrice=0.0;
		bHasCancel=FALSE;
		for(int j=0;j<nCJNum;j++)
		{
			if(!IsSameWTBH(pWTList+i,pCJList+j)) 
				continue;
			if(pCJList[j].lCjVol<0)
				bHasCancel=TRUE;
			pWTList[i].lCjVol+=pCJList[j].lCjVol;
			if(pCJList[j].lCjVol>0)
			{
				pWTList[i].fCjPrice=pCJList[j].fCjPrice;
				pWTList[i].lSeconds=pCJList[j].lSeconds;
			}
		}
		if(bHasCancel)
			pWTList[i].nBSFlag=-1;
	}
	TDEL(pCJList);
	return TRUE;
}

BOOL GetGPList(int & nGPNum,struct LevinGN_GPListANS *pGPList)
{
	nGPNum = 0;
	if(!g_bHasAdvJy || !TC_LevinGN_)
		return FALSE;
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	if(!(dwStatus&JY_LOGINED))
		return FALSE;
	struct LevinGN_GPZhInfoREQ tmpReq;
	memset(&tmpReq,0,sizeof(struct LevinGN_GPZhInfoREQ));
	//�õ���Ʊ��Ϣ
	int tmpOut=MAX_GPLIST_NUM*sizeof(struct LevinGN_GPListANS);
	if(TC_LevinGN_(LEVINGN_GPLIST,&tmpReq,sizeof(tmpReq),pGPList,tmpOut,0))
	{
		if(tmpOut>0)
			nGPNum = tmpOut/sizeof(struct LevinGN_GPListANS);
		else 
			nGPNum = 0;
	}
	nGPNum=min(MAX_GPLIST_NUM,nGPNum);
	nGPNum=max(nGPNum,0);
	return TRUE;
}

BOOL GetYmdList(struct MemStkInfo *pStkInfo,int & nYmdNum,struct MAIDAN *pYmdList)
{
	nYmdNum = 0;
	if(!g_bHasAdvJy || !TC_GetYmdData_)
		return FALSE;
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	if(!(dwStatus&JY_LOGINED))
		return FALSE;
	struct MAIDAN	pAllYmdList[MAX_YMD_NUM];
	int nAllYmdNum = TC_GetYmdData_(pAllYmdList,MAX_YMD_NUM);
	for(int i=0;i<nAllYmdNum;i++)
	{
		if(strcmp(pAllYmdList[i].zqdm,pStkInfo->Code)==0)
		{
			pYmdList[nYmdNum]=pAllYmdList[i];
			nYmdNum++;
		}
	}
	return TRUE;
}

void SendLevinCancel(CWnd *pView,struct LevinGN_WTCJListANS *pOne)
{
	struct LevinGN_Cancel tmpCancel;
	memset(&tmpCancel,0,sizeof(struct LevinGN_Cancel));
	strcpy(tmpCancel.YybID,pOne->YybID);
	strcpy(tmpCancel.Zjzh,pOne->Zjzh);
	strcpy(tmpCancel.Wtbh,pOne->Wtbh);
	struct LevinGN_BuySellCancelANS tmpAns;
	int tmpAnsLen=sizeof(struct LevinGN_BuySellCancelANS);
	TC_LevinGN_(LEVINGN_CANCEL,&tmpCancel,sizeof(tmpCancel),&tmpAns,tmpAnsLen,0);
	if(tmpAns.Errmsg[0]!=0)
		TDX_MessageBox(pView->m_hWnd,tmpAns.Errmsg,MB_OK|MB_ICONINFORMATION);
}


/***************************************************************************************

  ���������ֹ��ֹӮ��ʵ��

***************************************************************************************/

struct LevinOver_Info
{
	char	Wtbh[31];
};

int							l_nTotalCJNum = 0;
struct LevinGN_WTCJListANS	* l_pCJList = NULL;
BOOL						l_bDetecting = FALSE;

int							l_nLevinOverNum = 0;
struct LevinOver_Info		* l_pLevinOverInfo = NULL;

BOOL GetAllCJList(int & nCJNum,struct LevinGN_WTCJListANS *pCJList)
{
	nCJNum = 0;
	if(!g_bHasAdvJy || !TC_LevinGN_)
		return FALSE;
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	if(!(dwStatus&JY_LOGINED))
		return FALSE;
	struct LevinGN_ListREQ tmpReq;
	memset(&tmpReq,0,sizeof(struct LevinGN_ListREQ));
	tmpReq.bTodayReq=1;
	tmpReq.lStartDate=tmpReq.lEndDate=g_lOpenRq;
	int tmpOut=MAX_WTCJLIST_NUM*sizeof(struct LevinGN_WTCJListANS);
	if(TC_LevinGN_(LEVINGN_CJLIST,&tmpReq,sizeof(tmpReq),pCJList,tmpOut,0))
	{
		if(tmpOut>0)
			nCJNum = tmpOut/sizeof(struct LevinGN_WTCJListANS);
		else 
			nCJNum = 0;
	}
	nCJNum=min(MAX_WTCJLIST_NUM,nCJNum);
	nCJNum=max(nCJNum,0);
	return TRUE;
}

#include "MsgDlg.h"
BOOL ShowMsg(CWnd *pWnd,CString tmpInfo,int nMsgFlag)
{
	CMsgDlg dlg(pWnd);
	dlg.m_strCaption = "�����µ���ʾ";
	dlg.m_strMsg = tmpInfo;
	dlg.m_nMsgFlag = nMsgFlag;
	return (dlg.DoModal()==IDOK);
}

void DetectLevinCJ(CWnd *pMainWnd)
{
	if(!g_bHasAdvJy || !TC_LevinGN_)
		return;
	if(l_bDetecting) return;
	l_bDetecting = TRUE;

	//��ʼ��ȫ�ֱ���
	if(!l_pCJList)
	{
		l_pCJList = new struct LevinGN_WTCJListANS[MAX_WTCJLIST_NUM];
		memset(l_pCJList,0,MAX_WTCJLIST_NUM*sizeof(struct LevinGN_WTCJListANS));
	}
	if(!l_pLevinOverInfo)
	{
		l_pLevinOverInfo = new struct LevinOver_Info[MAX_WTCJLIST_NUM];
		memset(l_pLevinOverInfo,0,MAX_WTCJLIST_NUM*sizeof(struct LevinOver_Info));
	}
	//
	CString tmpInfo;
	MemStkInfo *pStkInfo=NULL;
	CurrStockData hqInfo;
	int i,j,k,l;
	int nCJNum=0,nGPNum=0;
	struct LevinGN_WTCJListANS * pCJList = new struct LevinGN_WTCJListANS[MAX_WTCJLIST_NUM];
	struct LevinGN_GPListANS   * pGPList = new struct LevinGN_GPListANS[MAX_GPLIST_NUM];
	if(GetGPList(nGPNum,pGPList) && GetAllCJList(nCJNum,pCJList))
	{
		if(l_nTotalCJNum>0)
		{
			for(i=0;i<nCJNum;i++)
			{
				//�����Ȳ��淶��¼ȥ��
				if(pCJList[i].fCjPrice<COMPPREC || pCJList[i].lCjVol<=0)
					continue;
				pStkInfo=g_pStockIO->Get_MemStkInfo(pCJList[i].Code,pCJList[i].Setcode);
				if(!pStkInfo) 
					continue;
				int nXsFlag = GetXSFlag(pStkInfo);
				int Type=GetStockType(pStkInfo->setcode,pStkInfo->Code);
				//------------------------�����������浥------------------------
				for(j=0;j<l_nTotalCJNum;j++)
				{
					if(IsSameWTBH(pCJList+i,l_pCJList+j)) 
						break;
				}
				if(j>=l_nTotalCJNum && pCJList[i].lCjVol==pCJList[i].lWtVol)	//? ������������,�ɽ��б���ͬһί�б�ſ����ж����ɽ�
				{
					if(pCJList[i].nBSFlag==0 && g_ExternLevinCfg.bGenSell && (Type==CODE_SZQZ||Type==CODE_SHQZ))
					{
						tmpInfo.Format("�����������:\r\n\r\n��⵽[%s %s]��[%.3f]��λ���Ѿ�����ɹ���[%d]��,\r\n\r\n�Ƿ�������������?",pStkInfo->Code,pStkInfo->Name,pCJList[i].fCjPrice,pCJList[i].lCjVol);
						if(ShowMsg(pMainWnd,tmpInfo,MSG_GREEN))
						{
							ToZst(pStkInfo->Code,pStkInfo->setcode);
							TC_DoLevinJy_(1,-1,pStkInfo->setcode,pStkInfo->Code,pStkInfo->Name,FloatToStr2(pCJList[i].fCjPrice*(1+0.01*g_ExternLevinCfg.fGenSellRate),nXsFlag),IntToStr(pCJList[i].lCjVol),LF_MMGSORDER);
						}
					}
					else if(pCJList[i].nBSFlag==1 && g_ExternLevinCfg.bGenBuy)
					{
						tmpInfo.Format("�����������:\r\n\r\n��⵽[%s %s]��[%.3f]��λ���Ѿ������ɹ���[%d]��,\r\n\r\n�Ƿ�����������?",pStkInfo->Code,pStkInfo->Name,pCJList[i].fCjPrice,pCJList[i].lCjVol);
						if(ShowMsg(pMainWnd,tmpInfo,MSG_RED))
						{
							ToZst(pStkInfo->Code,pStkInfo->setcode);
							TC_DoLevinJy_(0,-1,pStkInfo->setcode,pStkInfo->Code,pStkInfo->Name,FloatToStr2(pCJList[i].fCjPrice*(1-0.01*g_ExternLevinCfg.fGenBuyRate),nXsFlag),IntToStr(pCJList[i].lCjVol),LF_MMGSORDER);
						}
					}
				}
				//------------------------����ֹ��ֹӮ��------------------------
				if(g_ExternLevinCfg.bStop && (Type==CODE_SZQZ||Type==CODE_SHQZ))
				{
					int lCanUseVol = 0;
					for(k=0;k<nGPNum;k++)
					{
						if(pGPList[k].Setcode==pStkInfo->setcode && strcmp(pGPList[k].Code,pStkInfo->Code)==0)
							lCanUseVol=pGPList[k].lCanUseVol;
					}
					if(lCanUseVol > 0)
					{
						//�����ж��Ƿ��Ѿ������(�����û�ȡ������)
						for(k=0;k<l_nLevinOverNum;k++)
						{
							if(strcmp(l_pLevinOverInfo[k].Wtbh,pCJList[i].Wtbh)==0)
								break;
						}
						if(k<l_nLevinOverNum)
							continue;
						//Ҫ�ж��Ƿ�����ֻ��Ʊ����һ��
						BOOL bIsNewestWTBH=TRUE;
						for(l=0;l<nCJNum;l++)
						{
							//�����Ȳ��淶��¼ȥ��
							if(pCJList[l].fCjPrice<COMPPREC || pCJList[l].lCjVol<=0)
								continue;
							if(l!=i && strcmp(pCJList[l].Code,pCJList[i].Code)==0)
							{
								if(pCJList[l].lSeconds>pCJList[i].lSeconds)
								{
									bIsNewestWTBH=FALSE;
									break;
								}
							}
						}
						if(!bIsNewestWTBH)
							continue;
						//���ж��Ƿ�ﵽֹ��ֹӮ����(Ŀǰֻ�����򵥵�ֹ��ֹӮ)
						BOOL bLevinOver=FALSE;
						g_pStockIO->ReadCurrStockData(pStkInfo,&hqInfo);				
						if(hqInfo.Now>COMPPREC && pCJList[i].nBSFlag==0)
						{
							if(g_ExternLevinCfg.fStopLossRate>COMPPREC && 
								(pCJList[i].fCjPrice-hqInfo.Now)>(pCJList[i].fCjPrice*g_ExternLevinCfg.fStopLossRate*0.01))
							{
								bLevinOver=TRUE;
								tmpInfo.Format("ֹ��ֹӮ����:\r\n\r\n��⵽[%s %s]����ɽ����Ŀ�����ʴﵽ[%.2f%%],\r\n\r\n�Ƿ���������ֹ������?",pStkInfo->Code,pStkInfo->Name,g_ExternLevinCfg.fStopLossRate);
								if(ShowMsg(pMainWnd,tmpInfo,MSG_GREEN))
								{
									ToZst(pStkInfo->Code,pStkInfo->setcode);
									TC_DoLevinJy_(1,-1,pStkInfo->setcode,pStkInfo->Code,pStkInfo->Name,FloatToStr2(hqInfo.Buyp[0],nXsFlag),IntToStr(pCJList[i].lCjVol),LF_STOPLOSSORDER);
								}

							}
							if(g_ExternLevinCfg.fStopEarnRate>COMPPREC && 
								(hqInfo.Now-pCJList[i].fCjPrice)>(pCJList[i].fCjPrice*g_ExternLevinCfg.fStopEarnRate*0.01))
							{
								bLevinOver=TRUE;
								tmpInfo.Format("ֹ��ֹӮ����:\r\n\r\n��⵽[%s %s]����ɽ�����ӯ�����ʴﵽ[%.2f%%],\r\n\r\n�Ƿ���������ֹӮ����?",pStkInfo->Code,pStkInfo->Name,g_ExternLevinCfg.fStopEarnRate);
								if(ShowMsg(pMainWnd,tmpInfo,MSG_GREEN))
								{
									ToZst(pStkInfo->Code,pStkInfo->setcode);
									TC_DoLevinJy_(1,-1,pStkInfo->setcode,pStkInfo->Code,pStkInfo->Name,FloatToStr2(hqInfo.Buyp[0],nXsFlag),IntToStr(pCJList[i].lCjVol),LF_STOPEARNORDER);
								}
							}
						}
						//��������ˣ�����뵽�Ѵ���ĺ�ͬ����
						if(bLevinOver && l_nLevinOverNum<MAX_WTCJLIST_NUM)
						{
							strcpy(l_pLevinOverInfo[l_nLevinOverNum].Wtbh,pCJList[i].Wtbh);
							l_nLevinOverNum++;
						}
					}
				}
			}
		}
		l_nTotalCJNum=nCJNum;
		memcpy(l_pCJList,pCJList,nCJNum*sizeof(struct LevinGN_WTCJListANS));
	}
	l_bDetecting = FALSE;
	TDEL(pCJList);
	TDEL(pGPList);
}

//�˳������µ�ģ��
void ExitLevinCJ()
{
	TDEL(l_pCJList);
	TDEL(l_pLevinOverInfo);
}
