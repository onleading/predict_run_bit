#include "stdafx.h"
#include "UBigVol.h"
#include "ComView.h"
#include "KeyGuy.h"

extern int CopyToMyWarnAtom(LPWARNATOM & pWarnAtom,int nFlag,int &nCurGP);
extern void DrawDeepWarnList(BOOL bInDlg,long nWarnNum,LPWARNATOM pWarnAtom,int nFlag,CDC *pDC,CRect rc,int & nCurSel,int & nLastPos,int & nDrawPos,int & nDrawNum,int & nRowNum);
extern int DeepWarnHitTest(long nWarnNum,LPWARNATOM pWarnAtom,CPoint point,CRect rc,int & nLastPos,int & nDrawPos,int & nDrawNum);

int l_nBigVol_Flag = 0;	//保存上次的过滤状态

UBigVol::UBigVol(LPCOMINFO pComInfo):UBase(pComInfo)
{
	BigVol_SetCode = -1;
	BigVol_Code[0] = 0;

	m_nCurSel = -1;
	m_nLastPos = -1;
	m_nRowNum = m_nDrawPos = m_nDrawNum = 0;

	m_nFlag = l_nBigVol_Flag;
	m_nCurGP = -1;
	m_pWarnAtom = NULL;
	m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);

	++g_nUnitDeepWarnFlag;
}

UBigVol::~UBigVol()
{
	TDEL(m_pWarnAtom);
	--g_nUnitDeepWarnFlag;
	l_nBigVol_Flag = m_nFlag;
}

int  UBigVol::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case UM_SET_STOCK:
		{
			UBase::ProcessMsg(message,wParam,lParam);
			if(m_nFlag==3)
			{
				m_nLastPos = -1;
				m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);
				ProcessMsg(WM_PAINT);
			}
		}
		return 1;
	case UM_GET_STOCK:
		return g_pStockIO->GetIndex(BigVol_Code, BigVol_SetCode);
	case UPDATE_WARNGP:
		m_nLastPos = -1;
		m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);
		ProcessMsg(WM_PAINT);
		return 1;
	case WM_PAINT:
		{
			CREATE_TMPDC
			g_d.Bar(&m_MyDC,&m_rcIn,BACK_BRUSH);
			m_MyDC.SetBkMode(TRANSPARENT);

			CFont *oldfont = g_d.SelectFont(&m_MyDC,SMALL_FONT);
			CPen *oldpen = g_d.SelectPen(&m_MyDC,AXIS_PEN);
			g_d.DrawLine(&m_MyDC,0,0,0,m_rcIn.bottom);
			if(g_GH.bLeftHQInfo && !m_nInCom2)
				g_d.DrawLine(&m_MyDC,m_rcIn.right-1,0,m_rcIn.right-1,m_rcIn.bottom);
			g_d.DrawLine(&m_MyDC,0,m_rcIn.bottom,m_rcIn.right,m_rcIn.bottom);
			DrawIt(&m_MyDC);
			g_d.RestorePen(&m_MyDC,oldpen);
			g_d.RestoreFont(&m_MyDC,oldfont);

			pDC->BitBlt (left,top,Width(),Height(),&m_MyDC,0,0,SRCCOPY); //从内存DC拷贝到屏幕DC
			DESTORY_TMPDC
		}
		return 1;
	case WM_KEYDOWN:
		{
			UINT nChar = (UINT)wParam;
			switch(nChar)
			{
			case VK_UP:
				{
					SHORT CtrlDown =::GetKeyState(VK_CONTROL);
					if(nChar==VK_UP&&CtrlDown<0) return 1;								
					if(m_nCurSel<m_nDrawPos || m_nCurSel>=m_nDrawPos+m_nDrawNum)
					{
						m_nCurSel=m_nDrawPos+m_nDrawNum-1;
						ProcessMsg(WM_PAINT);
					}
					else if(m_nCurSel>0)
					{
						if(m_nCurSel-1<m_nDrawPos)
							m_nLastPos-=1;
						m_nCurSel-=1;
						ProcessMsg(WM_PAINT);
					}
				}
				return 1;
			case VK_DOWN:
				{
					SHORT CtrlDown =::GetKeyState(VK_CONTROL);
					if(nChar==VK_DOWN&&CtrlDown<0) return 1;
					if(m_nCurSel<m_nDrawPos || m_nCurSel>=m_nDrawPos+m_nDrawNum)
					{
						m_nCurSel=m_nDrawPos;
						ProcessMsg(WM_PAINT);
					}
					else if(m_nCurSel<m_nWarnNum-1)
					{
						if(m_nCurSel+1>m_nLastPos)
							m_nLastPos++;
						m_nCurSel++;
						ProcessMsg(WM_PAINT);
					}
				}
				return 1;
			default:				
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			CPoint pt;
			pt.x = LOWORD(lParam)-left;
			pt.y = HIWORD(lParam)-top;
			CRect rc = m_rcIn;
			int nSel = DeepWarnHitTest(m_nWarnNum,m_pWarnAtom,pt,rc,m_nLastPos,m_nDrawPos,m_nDrawNum);
			if(nSel==-14)
			{
				AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DEEPWARN);
				break;
			}
			else if(nSel<=-10)
			{
				m_nFlag = (-nSel)-10;
				m_nLastPos = -1;
				m_nWarnNum = CopyToMyWarnAtom(m_pWarnAtom,m_nFlag,m_nCurGP);
			}
			else
				m_nCurSel = nSel;
			ProcessMsg(WM_PAINT);
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			CPoint pt;
			pt.x = LOWORD(lParam)-left;
			pt.y = HIWORD(lParam)-top;
			CRect rc = m_rcIn;
			int nSel = DeepWarnHitTest(m_nWarnNum,m_pWarnAtom,pt,rc,m_nLastPos,m_nDrawPos,m_nDrawNum);
			if(nSel<=-10) break;
			m_nCurSel = nSel;
			BigVol_SetCode = -1;
			BigVol_Code[0] = 0;
			ProcessMsg(WM_PAINT);
			if(BigVol_SetCode != -1)
			{
				char tmpGPValue[80];
				tmpGPValue[0] = '0'+BigVol_SetCode;
				strcpy(tmpGPValue+1,BigVol_Code);
				CKeyGuy::OnCommonProcess(0,tmpGPValue);
			}
		}
		break;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}

void UBigVol::DrawIt(CDC *pDC)
{
	CRect rc = m_rcIn;
	DrawDeepWarnList(FALSE,m_nWarnNum,m_pWarnAtom,m_nFlag,pDC,rc,m_nCurSel,m_nLastPos,m_nDrawPos,m_nDrawNum,m_nRowNum);

	if(m_nCurSel>=0)
	{
		BigVol_SetCode = m_pWarnAtom[m_nCurSel].SetCode;
		memcpy(BigVol_Code,m_pWarnAtom[m_nCurSel].Code,SH_CODE_LEN);
		BigVol_Code[SH_CODE_LEN]=0;
	}	
}

short UBigVol::GetCurrentGPIndex()
{
	return g_pStockIO->GetIndex(BigVol_Code, BigVol_SetCode);
}
