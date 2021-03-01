#include "stdafx.h"
#include "UQueue.h"
#include "ComView.h"

UQueue::UQueue(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_bDataReady = FALSE;
	m_nZoomFlag = 0;
	memset(&m_Queue,0,sizeof(struct CurrQueue));
	memset(&m_QueueCode,0,sizeof(struct CurrQueue_Code));
	
	m_pStkInfoBak	= NULL;
	memset(&m_QueueNew,0,sizeof(struct SavedQueue));
	memset(&m_QueueBak,0,sizeof(struct SavedQueue));
	memset(&m_QueueShow,0,sizeof(struct MixQueue));
}

UQueue::~UQueue()
{

}

int  UQueue::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(m_nCurGP==-1) //���UnitΪ�գ������д���
	{
		if(message==WM_PAINT)
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
			DESTORY_TMPDC
			return 1;
		}
		return UBase::ProcessMsg(message,wParam,lParam);
	}
	switch(message)
	{
	case UM_READDATA:
		m_bDataReady = FALSE;
		ReadData(FALSE,wParam==0);
		return 1;
	case UM_GETDATA_ACK://����Զ�̰�,����ͷ��طֿ�
		switch(wParam)
		{
		case 0:	
			m_bDataReady = TRUE;
			ReadData(TRUE);
			break;
		}
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_TIMER:
		if(wParam==0x1114)
			ProcessMsg(UM_READDATA,0);
		return 1;
	case WM_PAINT:
		{
			if(wParam == 1)
				ProcessMsg(UM_READDATA,lParam);

			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,INFO_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //���ڴ�DC��������ĻDC
			DESTORY_TMPDC
		}
		return 1;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam));
			pt.x-=left;pt.y-=top;
			if(m_nZoomFlag==0)
			{
				if(pt.y<m_rcIn.Height()/2)	m_nZoomFlag=1;
				else						m_nZoomFlag=2;
			}
			else m_nZoomFlag=0;
			ProcessMsg(WM_PAINT);
		}
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UQueue::ReadData(BOOL bDataReady,BOOL bReqData)
{
	g_pStockIO->ReadCurrQueue(m_pStkInfo,&m_Queue);
	if(!OnLine || !m_bLevel2Property)
		return;
	if(!bDataReady)
	{
		if(bReqData)
		{
			m_pView->SendMessage(UM_GETMAINID); //ֱ���ø�������дg_nMainID
			g_nAssisID = SetComAssisID(m_nWhichUnit,0);

			struct CurrQueue queue;
			g_pStockIO->ReadCurrQueue(m_pStkInfo,&queue);
			memcpy(m_SingleCodeHead,&m_pStkInfo->setcode,1);
			memcpy(m_SingleCodeHead+1,m_pStkInfo->Code,SH_CODE_LEN);
			memcpy(m_SingleCodeHead+1+SH_CODE_LEN,&queue.RefreshNum,sizeof(DWORD));
			g_pGetData->GetL2MultiCurrQueue(m_SingleCodeHead,1);
		}
	}
	else
	{
		g_pGetData->GetL2MultiCurrQueueACK(&m_QueueCode,1);
		g_pStockIO->ReadCurrQueue(m_pStkInfo,&m_Queue);
	}
}

void UQueue::DrawPart(BOOL bBuy,CDC *pDC,int TotalHeight,int TextH,int XWidth,int XFrom[],int YFrom)
{
	short BSFlag = (bBuy?0:1);
	if(TotalHeight<2*TextH) return;
	int i,j,nCnt=0,row=(TotalHeight+2-TextH)/TextH;
	int nFixedWidth=m_rcIn.right;
	int HeadX1=HQ_XSPACE;
	int HeadX2=(nFixedWidth-2*HQ_XSPACE)/4-5;
	int HeadX3=(nFixedWidth-2*HQ_XSPACE)*2/4+5;
	int HeadX4=(nFixedWidth-2*HQ_XSPACE)*3/4;
	int HeadXWidth=(nFixedWidth-2*HQ_XSPACE)/4;
	if(bBuy) g_d.DisplayText(pDC,HeadX1,YFrom,VipColor.TXTCOLOR,_F("���"));
	else g_d.DisplayText(pDC,HeadX1,YFrom,VipColor.TXTCOLOR,_F("����"));
	if(m_QueueShow.price[BSFlag]>COMPPREC)
		g_d.BjPrintf(pDC,HeadX2,YFrom,m_QueueShow.price[BSFlag],m_pStkInfo->Close,m_XsFlag,HeadXWidth);
	if(m_QueueShow.volnum[BSFlag]>0)
		g_d.DisplayText(pDC,HeadX4,YFrom,VipColor.TXTCOLOR,HeadXWidth+5,((m_QueueNew.volnum[BSFlag]>999)?"%u":_F("%u��")),m_QueueNew.volnum[BSFlag]);
	YFrom+=TextH;
	g_d.DotLine(pDC,0,YFrom+1,m_rcIn.right,YFrom+1,VipColor.AXISCOLOR,3);
	YFrom+=1;
	for(i=0;i<row;i++)
	{
		for(j=0;j<5;j++)
		{
			if(nCnt>=min(102,m_QueueShow.volnum[BSFlag])) break;
			else
			{
				switch(m_QueueShow.state[BSFlag][nCnt])
				{
				case 1:
					g_d.DisplayText(pDC,XFrom[j],YFrom,RGB(96,96,96),XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					break;
				case 2:
					g_d.DisplayText(pDC,XFrom[j],YFrom,(bBuy?VipColor.KPCOLOR:VipColor.DOWNCOLOR),XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					break;
				case 3:
					g_d.DisplayText(pDC,XFrom[j],YFrom,RGB(192,192,192),XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					break;
				case 4:
					g_d.DisplayText(pDC,XFrom[j],YFrom,RGB(64,64,64),XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					break;
				case 5:
					g_d.DisplayText(pDC,XFrom[j],YFrom,VipColor.LEVELCOLOR,XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					break;
				default:
					if(IsHligtNowVol(m_QueueShow.vol[BSFlag][nCnt],m_pStkInfo))
					{
						g_d.DisplayText(pDC,XFrom[j],YFrom,RGB(192,0,192),XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					}
					else
					{
						g_d.DisplayText(pDC,XFrom[j],YFrom,VipColor.VOLCOLOR,XWidth,"%9u",m_QueueShow.vol[BSFlag][nCnt]);
					}
					break;
				}
			}
			nCnt++;
		}
		if(j==5)
		{
			YFrom+=TextH;
			j = 0;
		}
		if(nCnt>=min(102,m_QueueShow.volnum[BSFlag]))
		{
			if(m_QueueNew.volnum[BSFlag]>50) g_d.DisplayText(pDC,XFrom[j],YFrom,VipColor.VOLCOLOR,XWidth,"....");
			break;
		}
	}
}

void UQueue::DrawIt(CDC *pDC)
{
	if(Height() < 20) return;	//����ռ�̫С������
	if(m_bZs || m_bDSFlag) return;
	int nFixedWidth=m_rcIn.right;
	int TextH=HQ_TEXTH-4,YFrom=0,XFrom[5];
	XFrom[0]=1;
	XFrom[1]=(nFixedWidth-2)/5+1;
	XFrom[2]=(nFixedWidth-2)*2/5+1;
	XFrom[3]=(nFixedWidth-2)*3/5+1;
	XFrom[4]=(nFixedWidth-2)*4/5+1;
	int XWidth=(nFixedWidth-2)/5;
	Transform();
	CompareIt(FALSE);
	CompareIt(TRUE);
	memcpy(&m_QueueBak,&m_QueueNew,sizeof(struct SavedQueue));
	m_pStkInfoBak = m_pStkInfo;
	if(m_nZoomFlag==1)
		DrawPart(FALSE,pDC,m_rcIn.bottom,TextH,XWidth,XFrom,YFrom);
	else if(m_nZoomFlag==2)
		DrawPart(TRUE,pDC,m_rcIn.bottom,TextH,XWidth,XFrom,YFrom);
	else
	{
		DrawPart(FALSE,pDC,m_rcIn.bottom/2,TextH,XWidth,XFrom,YFrom);
		g_d.DrawLine(pDC,0,m_rcIn.bottom/2,m_rcIn.right,m_rcIn.bottom/2);
		YFrom=m_rcIn.bottom/2+2;
		DrawPart(TRUE,pDC,m_rcIn.bottom/2,TextH,XWidth,XFrom,YFrom);
	}
}

void UQueue::Transform()
{
	m_QueueNew.RefreshNum	= m_Queue.RefreshNum;
	m_QueueNew.price[0]		= m_Queue.Buyp;
	m_QueueNew.price[1]		= m_Queue.Sellp;
	m_QueueNew.volnum[0]	= m_Queue.buy1num;
	m_QueueNew.volnum[1]	= m_Queue.sell1num;
	for(int i=0;i<50;i++)
	{
		m_QueueNew.vol[0][i]= m_Queue.buy1vol[i];
		m_QueueNew.vol[1][i]= m_Queue.sell1vol[i];
	}
}

void UQueue::CompareIt(BOOL bBuy)
{
	short BSFlag = (bBuy?0:1);
	int i=0,j=0;
	BOOL bNewOne = FALSE;
	short svolnumNew = 0;
	short svolnumBak = 0;
	if(m_pStkInfo==NULL || m_pStkInfoBak==NULL || memcmp(m_pStkInfoBak,m_pStkInfo,sizeof(MemStkInfo))!=0
		|| !(equfz(m_QueueBak.price[BSFlag],m_QueueNew.price[BSFlag]))
		|| m_QueueBak.volnum[BSFlag]==0 || m_QueueNew.volnum[BSFlag]==0) bNewOne = TRUE;
	if(!bNewOne && m_QueueBak.volnum[BSFlag]==m_QueueNew.volnum[BSFlag] 
		&& memcmp(m_QueueBak.vol[BSFlag],m_QueueNew.vol[BSFlag],50*sizeof(unsigned short))==0) return;	//����ˢ��
	svolnumNew = min(50,m_QueueNew.volnum[BSFlag]);
	svolnumBak = min(50,m_QueueBak.volnum[BSFlag]);
	while(!bNewOne)			//��while,��������
	{
		memset(m_pTmpMap,0,50*sizeof(short));			//��ǰƥ��
		m_nTmpMap	= 0;								//ƥ��ĸ���,ͷһ�����μ�ƥ��,ʵ����ֵΪƥ�䵽�����һ�����������(0��)
		m_nQueueGap	= 0;								//һ���������ӵ�ֵ
		for(i=1;i<svolnumNew;i++)						//ͷһ��������ƥ��
		{
			for(j=i+m_nQueueGap;j<svolnumBak;j++)
			{
				if(m_QueueNew.vol[BSFlag][i]==m_QueueBak.vol[BSFlag][j])
				{
					m_pTmpMap[i]= j;					//ע��:������m_pTmpMap[i],����m_pTmpMap[m_nTmpMap]
					m_nQueueGap	= j-i;
					m_nTmpMap++;
					break;
				}
			}
			if(j==svolnumBak) break;					//�Ҳ����Ͳ�Ҫ������
		}
		if(m_nTmpMap<svolnumBak-1) m_pTmpMap[m_nTmpMap+1] = svolnumBak;	//������洦��
		//���е���
		if(m_nTmpMap==0)								//0��ƥ��,���Դ���Ľ��д���󷵻�,���ܴ��������,����New�Ĵ���
		{
			if(m_QueueBak.volnum[BSFlag]==1)			//����
			{
				if(m_QueueBak.vol[BSFlag][0] >= m_QueueNew.vol[BSFlag][0])		//�����ٵ���Ϊ�Ǳ���
				{
					m_QueueShow.price[BSFlag]	= m_QueueNew.price[BSFlag];
					m_QueueShow.volnum[BSFlag]	= 0;
					if(m_QueueBak.vol[BSFlag][0] == m_QueueNew.vol[BSFlag][0])
					{
						m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueNew.vol[BSFlag][0];
						m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 0;
						m_QueueShow.volnum[BSFlag]++;
					}
					else
					{
						m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueNew.vol[BSFlag][0];
						m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 2;
						m_QueueShow.volnum[BSFlag]++;
					}
					for(i=1;i<svolnumNew;i++)
					{
						m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueNew.vol[BSFlag][i];
						m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 5;
						m_QueueShow.volnum[BSFlag]++;
					}
					break;
				}
				else bNewOne = TRUE;
			}
			else bNewOne = TRUE;
			if(bNewOne) break;
		}
		//������ǰ������,���������ƥ��,ע��m_nTmpMapΪ1����49�����,�Ӵ�����,m_nTmpMap>0
		for(i=svolnumBak-1;i>0;i--)						//�������һ��,m_nTmpMap����1-49֮��(���߽�)
		{
			if(m_QueueBak.vol[BSFlag][i] == m_QueueNew.vol[BSFlag][m_nTmpMap])
			{
				m_pTmpMap[m_nTmpMap] = i;
				break;
			}
		}
		for(i=m_nTmpMap-1;i>1;i--)						//�����ڶ�����ʼ(�еĻ�),�ڶ�������ƥ��
		{
			for(j=m_pTmpMap[i+1]-1;j>m_pTmpMap[i];j--)
			{
				if(m_QueueNew.vol[BSFlag][i] == m_QueueBak.vol[BSFlag][j])
				{
					m_pTmpMap[i] = j;
					break;
				}
			}
		}
		//�Ż�ƥ����ǰ������
		m_nTmpClosePos	= 0;							//�ڶ���ƥ��,��ƥ��λ�õ�ǰһ������С�ڵ�һ��
		m_nTmpUnClosePos= 0;							//�ڶ���ƥ��
		m_nTmpFirstPos	= 0;							//����������С�ڵ�һ������λ��
		for(i=m_pTmpMap[2]-1;i>0;i--)					//ֻ������������,�����ʼƥ��(m_nTmpMapΪ1ʱ,m_pTmpMap[2]Ϊ50)
		{
			if(m_QueueBak.vol[BSFlag][i] != m_QueueNew.vol[BSFlag][1]) continue;
			for(j=i-1;j>=0;j--)
			{
				if(m_QueueNew.vol[BSFlag][0]>m_QueueBak.vol[BSFlag][j]) continue;
				if(m_nTmpUnClosePos==0)					//��¼����
				{
					m_nTmpUnClosePos= i;
					m_nTmpFirstPos	= j;
				}
				if(j==i-1)
				{
					m_nTmpClosePos	= i;				//ֱ������,���ٽ���ƥ��
					break;
				}
			}
			if(m_nTmpClosePos>0) break;
		}
		if(m_nTmpUnClosePos==0 && m_nTmpClosePos==0)
		{
			bNewOne = TRUE;
			break;
		}
		if(m_nTmpClosePos>0)							//���й���
		{
			m_nTmpUnClosePos= m_nTmpClosePos;
			m_nTmpFirstPos	= m_nTmpClosePos-1;
		}
		m_pTmpMap[1] = m_nTmpUnClosePos;				//����
		//�ϲ���������
		m_QueueShow.price[BSFlag]	= m_QueueNew.price[BSFlag];
		m_QueueShow.volnum[BSFlag]	= 0;
		//�ȴ���ͷһ��
		for(i=0;i<m_nTmpFirstPos;i++)					//��ǰ��Ļ�Ϊ���̵ĵ���
		{
			m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueBak.vol[BSFlag][i];
			m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 1;
			m_QueueShow.volnum[BSFlag]++;
		}
		if(m_QueueBak.vol[BSFlag][m_nTmpFirstPos]==m_QueueNew.vol[BSFlag][0])	//ͷһ�����
		{
			m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueBak.vol[BSFlag][m_nTmpFirstPos];
			m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 0;
			m_QueueShow.volnum[BSFlag]++;
		}
		else																	//������
		{
			m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueNew.vol[BSFlag][0];
			m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 2;
			m_QueueShow.volnum[BSFlag]++;
		}
		//�ϲ�����
		for(j=1,i+=1;i<svolnumBak||j<svolnumNew;)
		{
			if(i==svolnumBak)							//���涼�������ĵ���
			{
				while(j<svolnumNew)
				{
					m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueNew.vol[BSFlag][j];
					m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 5;
					m_QueueShow.volnum[BSFlag]++;
					j++;
				}
				break;
			}
			if(j>m_nTmpMap)								//����ƥ����,δƥ���ǰһƱ���볷��,��һƱ��һ��ѭ����������
			{
				while(i<svolnumBak)
				{
					m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueBak.vol[BSFlag][i];
					m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 4;
					m_QueueShow.volnum[BSFlag]++;
					i++;
				}
			}
			else										//ƥ������
			{
				if(i<=m_pTmpMap[j])
				{
					m_QueueShow.vol[BSFlag][m_QueueShow.volnum[BSFlag]]		= m_QueueBak.vol[BSFlag][i];
					m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 4;
					if(i==m_pTmpMap[j])
					{
						m_QueueShow.state[BSFlag][m_QueueShow.volnum[BSFlag]]	= 0;
						j++;
					}
					m_QueueShow.volnum[BSFlag]++;
					i++;
				}
				else j++;		//��һ�й��Ʋ���ִ��,���ڱ�֤ѭ���İ�ȫ
			}
		}
		break;		//���в�Ҫɾ����
	}
	if(bNewOne)
	{
		m_QueueShow.price[BSFlag]	= m_QueueNew.price[BSFlag];
		m_QueueShow.volnum[BSFlag]	= svolnumNew;
		for(i=0;i<50;i++)
		{
			m_QueueShow.vol[BSFlag][i]	= m_QueueNew.vol[BSFlag][i];
			m_QueueShow.state[BSFlag][i]= 0;
		}
	}
}
