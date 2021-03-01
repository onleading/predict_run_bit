#include "stdafx.h"
#include "UBase.h"

UBase::UBase(LPCOMINFO pComInfo)
{
	m_nInCom2 = m_nInHisDlg = 0;
	if(pComInfo) 
	{
		m_pView = pComInfo->pWnd;
		m_pUnits = pComInfo->pUnits;
		m_nInitPad = pComInfo->nNowPad;
		m_nWhichUnit = pComInfo->nWhichUnit;
		m_nType = pComInfo->nUnitType;
		m_nStyle = pComInfo->nUnitStyle;
		m_nInCom2 = pComInfo->nInCom2;
		m_nInHisDlg = pComInfo->nInDlg;
		m_nInMultiPeriod = pComInfo->nInMultiPeriod;
	}
	m_lDate = 0;
	SetRect(0,0,0,0);

	m_nCurGP = -1;
	m_pStkInfo = NULL;
	m_Domain = SZ;
	m_XsFlag = 2;
	m_bABG = TRUE;
	m_bLcPt = m_bLcExt = m_bLcIdx = m_bZs = m_bHKZs = m_bGz = m_bQyzq = m_bQz = FALSE;
	m_bSZSHZS = FALSE;
	m_bETF = FALSE;
	m_bSZKFSJJ = FALSE;
	m_bSHKFSJJ = FALSE;
	m_bCloseJJ = FALSE;
	m_bIPO = FALSE;
	m_bMinuteTick_Special = m_bAnalyData_Special = FALSE;
	m_bDSFlag = FALSE;
	memset(TimeSec,0,sizeof(short)*8);

	m_nGxdFlag = -1;
	m_bLevel2Property = FALSE;
}

UBase::~UBase()
{
}

int  UBase::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_SET_STOCK:	//如设为-1,则表示此Unit为空
		{
			m_nCurGP = wParam;
			m_bABG = TRUE;
			m_bLcPt = m_bLcExt = m_bLcIdx = m_bZs = m_bHKZs = m_bGz = m_bQyzq = m_bQz = FALSE;
			m_bSZSHZS = FALSE;
			m_bETF = FALSE;
			m_bSZKFSJJ = FALSE;
			m_bSHKFSJJ = FALSE;
			m_bCloseJJ = FALSE;
			m_bIPO = FALSE;
			m_bMinuteTick_Special = m_bAnalyData_Special = FALSE;
			m_bDSFlag = FALSE;
			m_nGxdFlag = -1;
			m_bLevel2Property = FALSE;
			if(m_nCurGP == -1)
			{
				m_pStkInfo = NULL;
				m_Domain = SZ;
				m_XsFlag = 2;
				return 1;
			}
			if(m_nCurGP < DS_FROM) //基本股票
			{
				m_pStkInfo = (*g_pStockIO)[m_nCurGP];
				if(!m_pStkInfo) 
				{
					m_nCurGP = 14;
					m_pStkInfo = (*g_pStockIO)[m_nCurGP];
				}
				if(!m_pStkInfo)
					return 1;
				m_Domain = g_pStockIO->GetDomain(m_pStkInfo);
				m_XsFlag = GetXSFlag(m_pStkInfo);
				m_bZs = testzs(m_pStkInfo);
				m_bLcIdx= IsLCIdx(m_pStkInfo);
				m_bLcExt= IsLCExt(m_pStkInfo);
				m_bLcPt	= IsLCPt(m_pStkInfo);
				if(m_bZs)
				{
					m_bSZSHZS=(strcmp(m_pStkInfo->Code,HS300_SZZS)==0 || strcmp(m_pStkInfo->Code,HS300_SHZS)==0);
					if(strcmp(m_pStkInfo->Code,SH_DPCODE)==0)
						m_nGxdFlag = 0;
					else if(strcmp(m_pStkInfo->Code,SZ_DPCODE)==0)
						m_nGxdFlag = 1;
					else if(strcmp(m_pStkInfo->Code,HS300_SZZS)==0 || strcmp(m_pStkInfo->Code,HS300_SHZS)==0)
						m_nGxdFlag = 2;
					else if(strcmp(m_pStkInfo->Code,ZXB_DPCODE)==0)
						m_nGxdFlag = 3;
				}
				int itmp = GetStockType(m_pStkInfo->setcode,m_pStkInfo->Code);
				m_bGz = ((itmp==CODE_SHGZ) || (itmp==CODE_SZGZ));
				m_bQyzq	= ((itmp==CODE_SZZQ) || (itmp==CODE_SHZQ));
				m_bQz = ((itmp==CODE_SHQZ) || (itmp==CODE_SZQZ));
				m_bABG = (itmp==CODE_SHAG || itmp==CODE_SHBG || itmp==CODE_SZAG || itmp==CODE_SZBG || itmp==CODE_ZXB || itmp==CODE_CYB);
				m_bETF = IsETF(m_pStkInfo);
				m_bSZKFSJJ = (strncmp(m_pStkInfo->Code,"1599",4)==0 || strncmp(m_pStkInfo->Code,"16",2)==0);
				m_bSHKFSJJ = (m_pStkInfo->Code[0]=='5' && m_pStkInfo->Code[1]!='0');
				m_bCloseJJ = (itmp==CODE_SZJJ || itmp==CODE_SHJJ);
				m_bIPO = (m_pStkInfo->Name[0]=='N');
				memcpy(TimeSec,m_Domain==SZ?sz:sh,sizeof(short)*8);
				m_bMinuteTick_Special = MinuteTick_SpecialData(m_pStkInfo);
				m_bAnalyData_Special = AnalyData_SpecialData(m_pStkInfo);
				m_bLevel2Property = IsLevel2Property(m_pStkInfo);
			}
			else	//三方品种
			{
				if(g_nDSAutoStopMinute>0)
					DSConnectWhenNeedHq(TRUE);
				if(!(m_pStkInfo=g_pStockIO->GetDSStkInfo(m_nCurGP-DS_FROM)))
				{
					m_nCurGP = DS_FROM;
					m_pStkInfo = g_pStockIO->GetDSStkInfo(0);
				}
				m_Domain = m_pStkInfo->setcode;
				m_XsFlag = GetXSFlag(m_pStkInfo);
				m_bDSFlag = TRUE;
				m_bHKZs = testhkzs(m_pStkInfo);
				memset(TimeSec,0,sizeof(short)*8);
				m_pTimer = g_pStockIO->DSGetTimeInf(m_pStkInfo->TimerID);
				TimeInf2Fz(m_pTimer, TimeSec);
			}
		}
		return 1;
	case	UM_GET_STOCK:
		return m_nCurGP;
	case	UM_SET_DATE:
		{
			m_lDate = (long)wParam;
			//如果日期为打开的日期，则当成打开的日期处理,非扩展行情用OpenRq
			if(!m_bDSFlag && m_lDate==g_lOpenRq)	
				m_lDate = 0;
			else if(m_bDSFlag && m_lDate==g_iToday)
				m_lDate = 0;
		}
		return 1;
	case UM_GET_DATE:
		return m_lDate;
	case WM_SIZE:
		{
			int x = LOWORD(wParam),y = HIWORD(wParam);
			int cx = LOWORD(lParam),cy = HIWORD(lParam);
			if(cy>=65535||cx>=65535) cx = cy = 19;
			SetRect(x,y,x+cx,y+cy);
			m_rcIn.CopyRect(this);
			m_rcIn.OffsetRect(-left,-top);
		}
		return 1;
	default:
		break;
	}
	return 0;
}
