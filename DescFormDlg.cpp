// DescFormDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DescFormDlg.h"
#include "malloc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT	Edit_ID[] = {IDC_EDIT1, IDC_EDIT2, IDC_EDIT3, IDC_EDIT4, IDC_EDIT5, IDC_EDIT6, IDC_EDIT7, IDC_EDIT8, 
							IDC_EDIT9, IDC_EDIT10, IDC_EDIT11, IDC_EDIT12, IDC_EDIT13, IDC_EDIT14, IDC_EDIT15, IDC_EDIT16, NULL};
UINT	SPIN_ID[] = {IDC_SPIN1, IDC_SPIN2, IDC_SPIN3, IDC_SPIN4, IDC_SPIN5, IDC_SPIN6, IDC_SPIN7, IDC_SPIN8, 
							IDC_SPIN9, IDC_SPIN10, IDC_SPIN11, IDC_SPIN12, IDC_SPIN13, IDC_SPIN14, IDC_SPIN15, IDC_SPIN16, NULL};

const int nSpace = 8;
/////////////////////////////////////////////////////////////////////////////
// CDescFormDlg

IMPLEMENT_DYNCREATE(CDescFormDlg, CFormView)

CDescFormDlg::CDescFormDlg()
	: CFormView(CDescFormDlg::IDD)
{
	m_pTmpIndex = NULL;
	m_pView = NULL;
	m_nPeriod = DAY_ST;	//默认使用日线周期
	m_nEditLength = -1;
	m_font.CreateFont(
				   12,
				   0,
				   0,
				   0,
				   FW_NORMAL,
				   FALSE,
				   FALSE,
				   0,
				   DEFAULT_CHARSET,
				   OUT_DEFAULT_PRECIS,
				   CLIP_DEFAULT_PRECIS,
				   DEFAULT_QUALITY,
				   DEFAULT_PITCH,
				   SONG_FONT);
}

CDescFormDlg::~CDescFormDlg()
{
}

void CDescFormDlg::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDescFormDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDescFormDlg, CFormView)
	//{{AFX_MSG_MAP(CDescFormDlg)
	ON_WM_PAINT()
	ON_WM_VSCROLL()
	ON_WM_MOUSEACTIVATE()
	ON_MESSAGE(UM_RESTORE, OnCancelOp)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN4, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN5, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN6, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN7, OnDeltaposSpin1)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN8, OnDeltaposSpin1)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN9, OnDeltaposSpin1)	
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN10, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN11, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN12, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN13, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN14, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN15, OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN16, OnDeltaposSpin1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDescFormDlg diagnostics

#ifdef _DEBUG
void CDescFormDlg::AssertValid() const
{
	CFormView::AssertValid();
}

void CDescFormDlg::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDescFormDlg message handlers

BOOL CDescFormDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CDescFormDlg::PostNcDestroy() 
{	
//	CFormView::PostNcDestroy();
}

void CDescFormDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	LPTSTR pszChar, token;
	const int nParamLength = 7;
	int nActualLines;	//一行文字显示在dialog上的实际行数
	int nActualCurrLine = 0;	//记录实际的一行画完后的实际当前行
	CFont *pOldFont;
	COLORREF crSaveTextCol;
	CString strOutInfo;

	pOldFont = dc.SelectObject(&m_font);
	dc.SetBkMode(TRANSPARENT);
	crSaveTextCol = dc.SetTextColor(RGB(0, 0, 0));
	m_csText = dc.GetTextExtent(_T("X"));
	if(m_pTmpIndex == NULL)
		dc.FillSolidRect(m_rcSaveBak, ::GetSysColor(COLOR_WINDOW));
	else if(m_pTmpIndex->nParaNum == 0)
		dc.TextOut(1, nSpace, _F("本指标无参数需要设置"));
	else
	{
		if (m_pTmpIndex->lpszParamGuy == NULL)	
			return;
		int nLength = strlen(m_pTmpIndex->lpszParamGuy) + 1;
		pszChar = (TCHAR*)_alloca(nLength);
		memset(pszChar, 0, sizeof(nLength));
		strcpy(pszChar, _F(m_pTmpIndex->lpszParamGuy));
		strOutInfo.Format("%s", pszChar);
		token = strtok(pszChar, "\r\n");
		CString strLineStr;
		int nPos, nPos1, i, nCurrParam = 0, j, nCurrLine = 0;
		CRect rect(m_rcSavePos);
		GetParent()->ClientToScreen(rect);
		ScreenToClient(rect);
		int	nEditEqualChars, nLineChars, nChars, nOffset, nTmpLines;
		int nParaCount = 0;
		//nEditEqualChars保存一个edit+spin所占的空格数(主要是防止edit超出formview的右边界
		nEditEqualChars = m_nEditLength / m_csText.cx + 1;
		nLineChars = (m_rcSavePos.Width()-GetSystemMetrics(SM_CXVSCROLL)) / m_csText.cx;
		memset(&m_ptEditPos, 0, sizeof(CPoint) * MAX_PARAMNUM);
		while (token != NULL)	//以行为分析的基本单位
		{
			strLineStr = token;
			nTmpLines = 0;
			//防止汉字乱码
			for(i=0;i<strLineStr.GetLength()/nLineChars;i++)
			{
				int nNoChiChar = 0;
				for(j=i*nLineChars;j<(i+1)*nLineChars;j++)
				{
					if(!IsDBCSLeadByte(strLineStr[j]))
						nNoChiChar++;
				}
				if((nLineChars-nNoChiChar)%2)
				{
					if(IsDBCSLeadByte(strLineStr[(i+1)*nLineChars-1]))
						strLineStr.Insert((i+1)*nLineChars-1, " ");
				}
			}
			if (strLineStr.Find("Param#") != -1)
			{
				while ((nPos = strLineStr.Find("Param#")) != -1)
				{
					++nParaCount;
					nPos1 = strOutInfo.Find("Param#");
					nTmpLines = nPos / nLineChars;
					nChars = nLineChars * nTmpLines - nPos;
					if(nParaCount<10)
					{
						strOutInfo.Delete(nPos1, nParamLength);
						strLineStr.Delete(nPos, nParamLength);
					}
					else
					{
						strOutInfo.Delete(nPos1, nParamLength+1);
						strLineStr.Delete(nPos, nParamLength+1);
					}
					nOffset = nLineChars - abs(nChars);
					if (nOffset < nEditEqualChars)
					{
						for (j = 0; j < nOffset; j++)
						{
							strOutInfo.Insert(nPos1, ' ');
							strLineStr.Insert(nPos, ' ');
						}
						nTmpLines++;
						nChars = 0;
					}
					else
						nOffset = 0;
					if (nOffset < 0) nOffset = 0;
					for (j = 0; j < nEditEqualChars; j++)
					{
						strOutInfo.Insert(nPos1 + nOffset, ' ');
						strLineStr.Insert(nPos + nOffset, ' ');
					}
					m_ptEditPos[nCurrParam].x = nCurrLine+nTmpLines;
					if (nTmpLines != 0) nPos = abs(nChars);
					m_ptEditPos[nCurrParam].y = nPos;
					nCurrParam++;
				}
			}
			//防止汉字乱码
			for(i=0;i<strLineStr.GetLength()/nLineChars;i++)
			{
				int nNoChiChar = 0;
				for(j=i*nLineChars;j<(i+1)*nLineChars;j++)
				{
					if(!IsDBCSLeadByte(strLineStr[j]))
						nNoChiChar++;
				}
				if((nLineChars-nNoChiChar)%2)
				{
					if(IsDBCSLeadByte(strLineStr[(i+1)*nLineChars-1]))
						strLineStr.Insert((i+1)*nLineChars-1, " ");
				}
			}
			//以下用来绘出实际的文本
			if (strLineStr.GetLength() % nLineChars != 0) nActualLines = 1;
			else nActualLines = 0;
			nActualLines += strLineStr.GetLength() / nLineChars;
			nPos = 0;
			for (i = nActualCurrLine; i < nActualLines + nActualCurrLine; i++)
			{
				j = i * (nSpace + m_csText.cy) + rect.top + nSpace;
				dc.TextOut(1,j,strLineStr.Mid(nPos, nLineChars));
				nPos += nLineChars;
			}
			//
			nActualCurrLine += nActualLines;	//记录下一次画的实际开始行
			token = strtok(NULL, "\r\n");
			nCurrLine += nActualLines;
		}
		//计算当前文本显示在formview上所需要的矩形大小
		rect.right = m_rcSavePos.Width() + rect.left;
		rect.bottom = (nCurrLine + 1) * (m_csText.cy + nSpace) + rect.top;
		m_rcSavePos.bottom = m_rcSavePos.top + rect.Height();
		SetScrollSizes(MM_TEXT, CSize(0, m_rcSavePos.Height()));
	}
	dc.SetTextColor(crSaveTextCol);
	dc.SelectObject(pOldFont);
}

void CDescFormDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{	
	CFormView::OnVScroll(nSBCode, nPos, pScrollBar);
	CSize csTotal;
	static int nCurrPos = 0;
	int nYOffset = 0, nMaxPos;

	csTotal = GetTotalSize();
	nMaxPos = GetScrollLimit(SB_VERT);
	switch (nSBCode)
	{
	case SB_TOP:
		m_rcSavePos = m_rcSaveBak;	//主要是在切换指标时用于复位
		nCurrPos = 0;
		Invalidate();
		break;
	case SB_BOTTOM:
	case SB_THUMBPOSITION:
		break;
	case SB_LINEUP:
		if (nCurrPos <= 0)
			nCurrPos = 0;
		else
		{
			nYOffset = csTotal.cy / 100;
			if (nYOffset < 1) nYOffset = 1;
			nCurrPos -= nYOffset;
		}
		break;
	case SB_LINEDOWN:
		if (nCurrPos >= nMaxPos)
			nCurrPos = nMaxPos;
		else
		{
			nYOffset = csTotal.cy / 100;
			if (nYOffset < 1) nYOffset = -1;
			else nYOffset = -nYOffset;
			nCurrPos -= nYOffset;
		}
		break;
	case SB_PAGEUP:
			nYOffset = csTotal.cy / 10;
			if (nYOffset < 1) nYOffset = 1;
			nCurrPos -= nYOffset;
			if (nCurrPos <= 0)
			{
				nCurrPos = 0;
				nYOffset = 0;
				m_rcSavePos = m_rcSaveBak;
				Invalidate();
			}
		break;
	case SB_PAGEDOWN:
			nYOffset = csTotal.cy / 10;
			if (nYOffset < 1) nYOffset = -1;
			else nYOffset = -nYOffset;
			nCurrPos -= nYOffset;
			if (nCurrPos >= nMaxPos)
			{
				nCurrPos = nMaxPos;
				m_rcSavePos = m_rcSaveBak;
				nYOffset = -nCurrPos;
			}
		break;
	case SB_THUMBTRACK:
		if (nCurrPos <= nMaxPos || nCurrPos >= 0)
		{
			nYOffset = nCurrPos - nPos;
			nCurrPos = nPos;
		}
		else
			nYOffset = 0;
		break;
	}
	if (nYOffset != 0)
	{
		m_rcSavePos.OffsetRect(0, nYOffset);
		Invalidate();
	}
}

void CDescFormDlg::OnDeltaposSpin1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	int nParam, nPos;
	UINT uPos;
	CString tempParam;

	uPos = pNMUpDown->hdr.idFrom;
	for(int i=0;SPIN_ID[i]!=NULL;i++)
	{
		if(uPos==SPIN_ID[i])
		{
			uPos = Edit_ID[i];
			nPos = i;
			break;
		}
	}
	GetDlgItem(uPos)->GetWindowText(tempParam);
    nParam = atoi(tempParam);
	nParam -= pNMUpDown->iDelta; //向上为加
	if(nParam > m_pTmpIndex->aPara[nPos].nMax)
		nParam = m_pTmpIndex->aPara[nPos].nMin;
	if(nParam < m_pTmpIndex->aPara[nPos].nMin)
		nParam = m_pTmpIndex->aPara[nPos].nMax;

	m_pTmpIndex->aPara[nPos].nValue[m_nPeriod] = nParam;
	m_fCurrValue[nPos] = (float)nParam;
	tempParam.Format("%-d", nParam);
	GetDlgItem(uPos)->SetWindowText(tempParam);
	if (m_pView != NULL)
		m_pView->SendMessage(UM_DYNCHGPARAM);
	*pResult = 0;
}

void CDescFormDlg::Init(CRect rect, CWnd *pView)
{
	m_rcSavePos = rect;
	m_rcSaveBak = rect;
	m_pView = pView;
}

void CDescFormDlg::SetStatus(int nSet, int nIndex, int nPeriod, BOOL bIsSpecialLine,BOOL bInitPara, float *pInitPara)
{
	CRect rect;
	CString tmpStr;
	int i = 0;

	memset(m_fCurrValue, 0, sizeof(float) * MAX_PARAMNUM);
	memset(m_fBakValue, 0, sizeof(float) * MAX_PARAMNUM);
	m_nPeriod = nPeriod;
	SendMessage(WM_VSCROLL, SB_TOP, NULL);
	if (m_nEditLength < 0)
	{
		GetDlgItem(IDC_EDIT1)->GetWindowRect(rect);
		m_nEditLength = rect.Width();
		GetDlgItem(IDC_SPIN1)->GetWindowRect(rect);
		m_nEditLength += 10;
	}
	if (bIsSpecialLine)
		m_pTmpIndex = NULL;
	else
		m_pTmpIndex = g_pCalc->_CallDTopViewCalc_8(nSet, nIndex);
	SendMessage(WM_PAINT);
	if (m_pTmpIndex == NULL || m_pTmpIndex->lpszParamGuy == NULL) return;
	i = m_pTmpIndex->nParaNum;
	for (i = 0; i < m_pTmpIndex->nParaNum; i++)
	{
		if(bInitPara)
		{
			if(pInitPara[i]<m_pTmpIndex->aPara[i].nMin || pInitPara[i]>m_pTmpIndex->aPara[i].nMax)
			{
				m_fBakValue[i] = m_pTmpIndex->aPara[i].nDefault;
				m_fCurrValue[i] = m_pTmpIndex->aPara[i].nDefault;
			}
			else
			{
				m_fBakValue[i] = pInitPara[i];
				m_fCurrValue[i] = pInitPara[i];
			}
		}
		else
		{
			m_fBakValue[i] = m_pTmpIndex->aPara[i].nValue[m_nPeriod];
			m_fCurrValue[i] = m_pTmpIndex->aPara[i].nValue[m_nPeriod];
		}
	}
	for(int j=0;Edit_ID[j]!=NULL;j++)
	{
		if(i<=j) break;

		rect.left	= m_ptEditPos[j].y * m_csText.cx+2;
		if (rect.left < 0) rect.left = 0;
		rect.top	= m_ptEditPos[j].x * (m_csText.cy + nSpace)+6;
		rect.right	= rect.left + m_nEditLength;
		rect.bottom	= rect.top + m_csText.cy+4;
		GetDlgItem(Edit_ID[j])->MoveWindow(rect);
		tmpStr.Format("%-.f", m_fCurrValue[j]);
		GetDlgItem(Edit_ID[j])->SetWindowText(tmpStr);
		GetDlgItem(Edit_ID[j])->ShowWindow(SW_SHOW);
		((CSpinButtonCtrl*)GetDlgItem(SPIN_ID[j]))->SetBuddy(GetDlgItem(Edit_ID[j]));
		GetDlgItem(SPIN_ID[j])->ShowWindow(SW_SHOW);
	}

	Invalidate();
}

void CDescFormDlg::HideSelf()
{
	for(int j=0;Edit_ID[j]!=NULL;j++)
	{
		GetDlgItem(Edit_ID[j])->ShowWindow(SW_HIDE);
		GetDlgItem(SPIN_ID[j])->ShowWindow(SW_HIDE);
	}
}


int CDescFormDlg::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) 
{
	return MA_NOACTIVATE;
}

void CDescFormDlg::OnCancelOp(WPARAM wParam, LPARAM lParam)
{
	memcpy(m_fCurrValue, m_fBakValue, sizeof(float) * MAX_PARAMNUM);
}

BOOL CDescFormDlg::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			GetParent()->SendMessage(WM_CLOSE,0,0);
			return TRUE;
		}
		else
		if (pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F12)
			return TRUE;
	}
	return CFormView::PreTranslateMessage(pMsg);
}

void CDescFormDlg::GetCurrData(float *pCurrData)
{
	if (m_pTmpIndex == NULL || m_pTmpIndex->lpszParamGuy == NULL) return;

	CString tmpStr;
	float fTmp;

	for(int i=0;Edit_ID[i]!=NULL&&SPIN_ID[i]!=NULL;i++)
	{
		GetDlgItem(Edit_ID[i])->GetWindowText(tmpStr);
		fTmp = atof(tmpStr);
		if(fTmp > m_pTmpIndex->aPara[i].nMax)
			fTmp = m_pTmpIndex->aPara[i].nMax;
		if(fTmp < m_pTmpIndex->aPara[i].nMin)
			fTmp = m_pTmpIndex->aPara[i].nMin;
		m_pTmpIndex->aPara[i].nValue[m_nPeriod] = fTmp;
		tmpStr.Format("%-.f", fTmp);
		GetDlgItem(Edit_ID[i])->SetWindowText(tmpStr);
		m_fCurrValue[i] = fTmp;
	}

	if (m_pView != NULL)
		m_pView->SendMessage(UM_DYNCHGPARAM);
	if (pCurrData != NULL)
		memcpy(pCurrData,m_fCurrValue,MAX_PARAMNUM*sizeof(float));
}
