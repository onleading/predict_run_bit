#include "stdafx.h"
#include "tdxw.h"
#include "CWChartUnit.h"
#include "CWChartUCWnd.h"

CCWChartUnit::CCWChartUnit(LPCOMINFO pComInfo):UBase(pComInfo)
{
	m_pChart = NULL;
	m_pCUCtrlWnd = NULL;

	memset(&m_Cfg,0,sizeof(Chart_Cfg));
	memset(m_strGnID, 0, 21);
}

CCWChartUnit::~CCWChartUnit()
{
	if(m_pChart) 
		DeleteTChart(&m_pChart);
	if(m_pCUCtrlWnd)
	{
		m_pCUCtrlWnd->DestroyWindow();
		delete m_pCUCtrlWnd;
	}
}

//////////////////////////////////////////////////////////////////////////
BOOL	CCWChartUnit::InitChart(LPCSTR strGnID, LPCSTR strGnIni)
{
	if(!m_pView) return FALSE;

	if(!m_pCUCtrlWnd||!m_pCUCtrlWnd->m_hWnd)
		{
			TDEL(m_pCUCtrlWnd);
			m_pCUCtrlWnd = new CCWChartUCWnd(this);
			m_pCUCtrlWnd->Create(NULL,"hidetablewnd",WS_CHILD|WS_VISIBLE,CRect(0,0,0,0),m_pView,IDC_HIDENETWND);
			if(!m_pCUCtrlWnd||!m_pCUCtrlWnd->m_hWnd) return FALSE;
		}

	//功能id，用来控制和传递消息等
	strncpy(m_strGnID, strGnID, 20);

	CString strGnPath = HomePath+"cwcfg\\";
	if(!GetGnInfo(strGnPath,strGnIni))
		return FALSE;
	
	if(!m_pChart)
	{	
		m_pChart= CreateNewTChart(0, m_pCUCtrlWnd->GetSafeHwnd(), CRect(0,0,0,0), IDC_CHARTCTRLEX);
		if(!m_pChart)
			AfxMessageBox("Create Chart Error!");
	}
	return TRUE;
}

void	CCWChartUnit::ArrangeWnd(CRect &rc, BOOL bHide)
{
	if(!m_pCUCtrlWnd) return;
	if(bHide)
	{
		CRect rc(0,0,0,0);
		if(m_pChart&&m_pChart->m_hWnd)
			m_pChart->MoveWindow(rc);
		return;
	}

	if(!m_pView) return;

	if(m_pCUCtrlWnd&&m_pCUCtrlWnd->m_hWnd)
		m_pCUCtrlWnd->MoveWindow(&rc, FALSE);

	m_pCUCtrlWnd->GetClientRect(&rc);
	CRect rcchart(rc);

	if(m_pChart&&m_pChart->m_hWnd)
		m_pChart->MoveWindow(rcchart);

}

void	CCWChartUnit::ShowAll(BOOL bShow)
{
	if(bShow)
	{
		if(m_pCUCtrlWnd) m_pCUCtrlWnd->ShowWindow(SW_SHOW);
		if(m_pChart) m_pChart->ShowWindow(SW_SHOW);
	}
	else 
	{
		if(m_pCUCtrlWnd) m_pCUCtrlWnd->ShowWindow(SW_HIDE);
		if(m_pChart) m_pChart->ShowWindow(SW_HIDE);
	}
}

void	CCWChartUnit::RefreshColorFontCfg()
{
	if(m_pChart)
	{
		ChartColorFont ccf;
		ccf= m_pChart->GetChartColorFont();
		ccf.AxisColor		=VipColor.AXISCOLOR;
		ccf.BkColor			=VipColor.BACKCOLOR;
		ccf.ChartBkColor	=ccf.BkColor;
		ccf.AxisTxtColor	=VipColor.TXTCOLOR;
		ccf.ChartTxtColor	=VipColor.TXTCOLOR;
		ccf.TitleTextColor	=VipColor.TXTCOLOR;
		m_pChart->SetChartColorFont(ccf);
	}
}

void	CCWChartUnit::BeginShowChart()
{
	UINT ChartType = TCS_STATIC;
	if(m_Cfg.StaticChart.ynames.size()>0)
		ChartType = TCS_STATIC;
	else if(m_Cfg.DynaChart.DynaChartFields.size()>0)
		ChartType = TCS_DYNA;
	else if(m_Cfg.StarChart.StarCharts.size()>0)
		ChartType = TCS_STAR;
	
	DWORD dwStyle=0;
	dwStyle|=TCS_3D;
	dwStyle |= (
			ChartType
			|m_Cfg.StaticChart.nFlag		//大类型，静态图表；添加的动态图表数据，星空图数据都无效，只根据静态图数据画;
		//	|TCS_CAKE   //小类型
			|TCS_STATIC_STYLE_CHANGEABLE//容许在小类型之间切换
			|TCS_ALLINONE_EXAMPLE_RIGHT//浮动图例
		//	|TCS_ALLINONE_EXAMPLE_2Row//图例显示为2行
			|TCS_3D//3d
			|TCS_HGRID//横向网格
		//	|TCS_VGRID//纵.....
			|TCS_FLOATINGBOX//浮动窗口
			|TCS_ALLINONE_EXAMPLE_TOP);
	
	if(m_pChart)
	{
		DWORD dwStyleMask = 0XFFFFFFFF;
		if(dwStyle&TCS_DYNA) dwStyle|=TCS_SYSCLR_DYNA;
		if(dwStyle&TCS_STATIC) dwStyle|=TCS_SYSCLR_STATIC;
		m_pChart->ChangeStyle(dwStyleMask, dwStyle);
	}

	RefreshColorFontCfg();
}
//////////////////////////////////////////////////////////////////////////
BOOL	CCWChartUnit::GetGnInfo(CString GnPath,CString GnINI)
{
	if(!GetFromGNIni(&m_Cfg,GnPath+GnINI))
		return FALSE;
	strncpy(m_Cfg.GnIniFile, (char*)(LPCSTR)GnINI, MAX_PATH-1);
	return TRUE;
}

BOOL  CCWChartUnit::GetFromGNIni(struct Chart_Cfg *pCfg, const char *PathFile)
{
	if(!pCfg) return FALSE;
	memset(pCfg,0,sizeof(struct GN_Cfg));
	CString Ini = PathFile;
	CString tempStr;
	char tmpChar[200],tmpChar1[200];
	pCfg->GnIniVer = GetPrivateProfileInt("Gn","GNIniVer",0,Ini);
	GetPrivateProfileString("DataSrc","TableIni","",pCfg->DataSrcFile,MAX_PATH,Ini);

	GetPrivateProfileString("StaticAct","X_OutName","",tmpChar,30,Ini);
	pCfg->StaticChart.xname.name = tmpChar;

	int nFlag = GetPrivateProfileInt("StaticAct","ChartType", 0, Ini);
	switch(nFlag) 
	{
	case 0:
		pCfg->StaticChart.nFlag = TCS_CAKE;
		break;
	case 1:
		pCfg->StaticChart.nFlag = TCS_PARA;
		break;
	case 2:
		pCfg->StaticChart.nFlag = TCS_ALLINONE;
		break;
	default:
		pCfg->StaticChart.nFlag = TCS_PARA;
		break;
	}
	int Y_OutNameNum= GetPrivateProfileInt("StaticAct","Y_OutNameNum", 0, Ini);
	if(Y_OutNameNum> 0)
	{
		string name, caketitle, poletitle;
		int type, axis;
		for(int i= 0; i< Y_OutNameNum; i++)
		{
			tempStr.Format("Y_OutName%02d", i+ 1);
			GetPrivateProfileString("StaticAct",tempStr,"",tmpChar,199,Ini);
			name= tmpChar;

			tempStr.Format("Y_OutNameType%02d", i+ 1);
			type= GetPrivateProfileInt("StaticAct", tempStr, 1, Ini)-1;
			switch(type) 
			{
			case 0:
				type = TCSS_YITEM_POLE;
				break;
			case 1:
			default:
				type = TCSS_YITEM_LINE;
				break;
			}

			tempStr.Format("Y_OutNameAxis%02d", i+ 1);
			axis = GetPrivateProfileInt("StaticAct", tempStr, 1, Ini)-1;
			switch(axis) 
			{
			case 1:
				axis = TCSS_AXIS_RIGHT;
				break;
			case 0:
			default:
				axis = TCSS_AXIS_LEFT;
				break;
			}

			type |= axis;

			tempStr.Format("Y_OutNameCakeTitle%02d", i+ 1);
			GetPrivateProfileString("StaticAct", tempStr, "",tmpChar, 199, Ini);
			caketitle= tmpChar;

			tempStr.Format("Y_OutNamePoleTitle%02d", i+ 1);
			GetPrivateProfileString("StaticAct", tempStr, "",tmpChar, 199, Ini);
			poletitle= tmpChar;
			
			pCfg->StaticChart.ynames.push_back(StaticChartItem::StaticYItemType(name, type, VipColor.ZBCOLOR[i%20], caketitle, poletitle));
		}
	}
	//Act:DynaChartFields
	int nDynaChartNum= GetPrivateProfileInt("DynaAct","DynaChartNum", 0, Ini);
	if(nDynaChartNum> 0)
	{
		pCfg->DynaChart.DynaChartFields.resize(nDynaChartNum);
		for(int i= 0; i< nDynaChartNum; i++)
		{
			tempStr.Format("DynaChart_Fields%02d", i+ 1);
			GetPrivateProfileString("DynaAct",tempStr,"",tmpChar,199,Ini);
			tmpChar[199]= 0;
			int ii= 0;
			do
			{
				ParseMessageStr(tmpChar,tmpChar1,",",++ii);
				tmpChar1[30]= 0;
				AllTrim(tmpChar1);
				if(strlen(tmpChar1)== 0)
					break;
				pCfg->DynaChart.DynaChartFields[i].name.push_back(tmpChar1);
			}while(1);
			int nCount = pCfg->DynaChart.DynaChartFields[i].name.size();
			pCfg->DynaChart.DynaChartFields[i].vtitle.resize(nCount);
			tempStr.Format("DynaChart_Title%02d", i+ 1);
			GetPrivateProfileString("DynaAct", tempStr, "动态图表",tmpChar, 199, Ini);
			pCfg->DynaChart.DynaChartFields[i].title=tmpChar;
		}
	}
	//Act:starchart
	int nStarChartNum= GetPrivateProfileInt("StarAct","StarChartNum", 0, Ini);
	if(nStarChartNum> 0)
	{
		pCfg->StarChart.StarCharts.resize(nStarChartNum);
		for(int i= 0; i< nStarChartNum; i++)
		{
			tempStr.Format("StarChart_OutName%02d", i+ 1);
			GetPrivateProfileString("StarAct",tempStr,"",tmpChar,199,Ini);
			tmpChar[199]= 0;
			pCfg->StarChart.StarCharts[i].mainname.name= tmpChar;
			
			tempStr.Format("StarChart_XOutName%02d", i+ 1);
			GetPrivateProfileString("StarAct",tempStr,"",tmpChar,199,Ini);
			tmpChar[199]= 0;
			pCfg->StarChart.StarCharts[i].xname.name= tmpChar;

			tempStr.Format("StarChart_YOutName%02d", i+ 1);
			GetPrivateProfileString("StarAct",tempStr,"",tmpChar,199,Ini);
			tmpChar[199]= 0;
			pCfg->StarChart.StarCharts[i].yname.name= tmpChar;

			tempStr.Format("StarChart_Title%02d", i+ 1);
			GetPrivateProfileString("StarAct",tempStr,"星空图",tmpChar,199,Ini);
			tmpChar[199]= 0;
			pCfg->StarChart.StarCharts[i].mainname.title= tmpChar;
			
		}
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
int  CCWChartUnit::ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SIZE:
		UBase::ProcessMsg(message,wParam,lParam);
		ArrangeWnd(*this, FALSE);
		return 1;
	default:
		break;
	}
	return UBase::ProcessMsg(message,wParam,lParam);
}