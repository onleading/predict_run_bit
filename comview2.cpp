// ComView2.cpp : implementation file
#include "stdafx.h"
#include "tdxw.h"
#include "GridView.h"
#include "ComView2.h"
#include "MainFrm.h"
#include "HQGrid.h"
#include "UZhSort.h"
#include "USeat.h"
#include "UWeb.h"
#include "UWeb2.h"
#include "UWtfb.h"
#include "UGlhq.h"
#include "ZhSortOptDlg.h"
#include "InputName2.h"
#include "UBaseInner.h"

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CComView2, CView)

BEGIN_MESSAGE_MAP(CComView2, CUnitView)
	//////////////////////////////////////
	ON_MESSAGE(UM_GETDATA_ACK,OnGetDataAck)
	ON_MESSAGE(UM_READANDPAINT,OnReadAndPaint)
	ON_MESSAGE(UM_CONSTRUCTPAD_MESSAGE,ConstructPadMessage)
	ON_MESSAGE(UM_MYSWITCHBAR_CLICK,OnSwitchBarMsg)
	ON_MESSAGE(UM_REFRESHCUSTOMBAR,OnRefreshCustomBarMsg)
	//{{AFX_MSG_MAP(CComView2)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_CHAR()
	ON_WM_SETCURSOR()
	ON_COMMAND_RANGE(ID_INSERTHQGRID_ALL, ID_INSERTHQDS64, OnInsertUnit)
	ON_UPDATE_COMMAND_UI_RANGE(ID_INSERTHQGRID_ALL, ID_INSERTHQDS64, OnUpdateInsertUnit)
	ON_COMMAND_RANGE(ID_INSERTLEFT, ID_INSERTDOWN, OnSplitPad)
	ON_UPDATE_COMMAND_UI_RANGE(ID_INSERTLEFT, ID_INSERTDOWN, OnUpdateSplitPad)
	ON_COMMAND(ID_DELETEUNIT, OnDeleteunit)
	ON_UPDATE_COMMAND_UI(ID_DELETEUNIT, OnUpdateDeleteunit)
	ON_COMMAND(ID_SAVEPAD, OnSavepad)
	ON_UPDATE_COMMAND_UI(ID_SAVEPAD, OnUpdateSavepad)
	ON_COMMAND(ID_CONTRALLUNIT, OnContrallunit)
	ON_UPDATE_COMMAND_UI(ID_CONTRALLUNIT, OnUpdateContrallunit)
	ON_COMMAND_RANGE(ID_GPVAR1, ID_GPVAR20, OnSetGpVar)
	ON_UPDATE_COMMAND_UI_RANGE(ID_GPVAR1, ID_GPVAR20, OnUpdateSetGpVar)
	ON_COMMAND(ID_NEWPAD, OnNewpad)
	ON_UPDATE_COMMAND_UI(ID_NEWPAD, OnUpdateNewPad)
	ON_COMMAND(ID_BEGINSPLIT, OnBeginsplit)
	ON_UPDATE_COMMAND_UI(ID_BEGINSPLIT, OnUpdateBeginsplit)
	ON_COMMAND(ID_LOCKUNIT, OnLockunit)
	ON_UPDATE_COMMAND_UI(ID_LOCKUNIT, OnUpdateLockunit)
	ON_COMMAND(ID_DELETECURPAD, OnDeletecurpad)
	ON_UPDATE_COMMAND_UI(ID_DELETECURPAD, OnUpdateDeletecurpad)
	ON_UPDATE_COMMAND_UI(ID_SETUNITOPTION, OnUpdateSetunitoption)
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_SETUNITOPTION, OnSetunitoption)
	ON_COMMAND_RANGE(ID_SCSTART, ID_SCSTART+200, OnChangeZhSortSC)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SCSTART, ID_SCSTART+200, OnUpdateChangeZhSortSC)
	ON_COMMAND(ID_OPENURL,OnOpenURL)
	//////////////////////////////////////////////////////////////////////////
	ON_COMMAND(ID_LIMINWIDTH,OnLiminWidth)
	ON_UPDATE_COMMAND_UI(ID_LIMINWIDTH, OnUpdateLiminWidth)
	ON_COMMAND(ID_LIMINHEIGHT,OnLiminHeight)
	ON_UPDATE_COMMAND_UI(ID_LIMINHEIGHT, OnUpdateLiminHeight)
	ON_UPDATE_COMMAND_UI(ID_CHANGEGPTYPE, OnUpdateChangeGpType)

	ON_COMMAND(ID_BATCH,OnBatch)
	ON_COMMAND(ID_GOTOJBZL, OnGotojbzl)
	ON_COMMAND(ID_CHANGEGPTYPE, OnChangeGPType)
	ON_COMMAND(ID_AUTOPAGE,OnAutoPage)

	ON_COMMAND_RANGE(ID_RIGHT_OPEN,ID_RIGHT_DELETEFROMBLOCK,OnRightClickRow)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_OPEN,ID_RIGHT_DELETEFROMBLOCK,OnUpdateRightClickRow)
	
	ON_COMMAND_RANGE(ID_RIGHT_2,ID_RIGHT_49,OnBlockID)
	ON_COMMAND_RANGE(ID_QHRIGHT_2,ID_QHRIGHT_23,OnBlockID)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_2,ID_RIGHT_49,OnUpdateRightClickHead)
	ON_UPDATE_COMMAND_UI_RANGE(ID_QHRIGHT_2,ID_QHRIGHT_23,OnUpdateRightClickHead)

	ON_COMMAND_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnTypeID)
	ON_COMMAND_RANGE(ID_RANK_1,ID_RANK_26,OnTypeID)

	ON_COMMAND_RANGE(ID_DY1,ID_DY32,OnBlockID)
	ON_COMMAND_RANGE(ID_HY1,ID_HY52,OnBlockID)
	ON_COMMAND_RANGE(ID_CONCEPT_MENU_1,ID_CONCEPT_MENU_100,OnBlockID)
	ON_COMMAND_RANGE(ID_ZHB_MENU_1,ID_ZHB_MENU_100,OnBlockID)
	ON_COMMAND_RANGE(ID_BLOCK_MENU_1,ID_BLOCK_MENU_400,OnBlockID)
	ON_COMMAND_RANGE(ID_ZJHHY_MENU_1,ID_ZJHHY_MENU_200,OnBlockID)
	
	ON_COMMAND(ID_PADLINKAGE, OnLinkage)
	ON_UPDATE_COMMAND_UI(ID_PADLINKAGE, OnUpdateLinkage)

	ON_COMMAND_RANGE(IDM_DS1,IDM_DS1+MAX_MKT_NUM,OnDSMarket)
	ON_COMMAND_RANGE(ID_MDFHK_HY_1,ID_MDFHK_HY_300,OnDSMarket)
	ON_UPDATE_COMMAND_UI_RANGE(IDM_DS1,IDM_DS1+MAX_MKT_NUM,OnUpdateDSMarket)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MDFHK_HY_1,ID_MDFHK_HY_300,OnUpdateMDFHkHyClick)

	ON_COMMAND(ID_MARK_CANCEL, OnCancelMark)
	ON_COMMAND_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnMarkStockFlag)
	ON_UPDATE_COMMAND_UI_RANGE(ID_MARK_ADD1, ID_MARK_ADD10, OnUpdateMarkStockFlag)
	//////////////////////////////////////////////////////////////////////////
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIGHT_TYPE0,ID_RIGHT_TYPE20,OnUpdateClickCorner)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DY1,ID_DY32,OnUpdateDYBlock)
	ON_UPDATE_COMMAND_UI_RANGE(ID_HY1,ID_HY52,OnUpdateHYBlock)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONCEPT_MENU_1,ID_CONCEPT_MENU_100,OnUpdateConceptClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZHB_MENU_1,ID_ZHB_MENU_100,OnUpdateZHBClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_BLOCK_MENU_1,ID_BLOCK_MENU_400,OnUpdateMenuBlockClick)
	ON_UPDATE_COMMAND_UI_RANGE(ID_ZJHHY_MENU_1,ID_ZJHHY_MENU_200,OnUpdateZJHHYClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern int l_LastSetSHOWNUM;

/////////////////////////////////////////////////////////////////////////////
//与定制版面相关的函数
void	SetStrMap()
{
	int i,j;
	g_mUnitType.clear();
	g_mUnitType["ZST_UNIT"] = ZST_UNIT;	g_mUnitType["FXT_UNIT"] = FXT_UNIT;
	g_mUnitType["HQ_UNIT"] = HQ_UNIT;	g_mUnitType["TICK_UNIT"] = TICK_UNIT;
	g_mUnitType["FJB_UNIT"] = FJB_UNIT;	g_mUnitType["FSB_UNIT"] = FSB_UNIT;
	g_mUnitType["CDP_UNIT"] = CDP_UNIT;	g_mUnitType["MMP_UNIT"] = MMP_UNIT;
	g_mUnitType["HQGRID_UNIT"] = HQGRID_UNIT;	g_mUnitType["HISK_UNIT"] = HISK_UNIT;
	g_mUnitType["BIGVOL_UNIT"] = BIGVOL_UNIT;	g_mUnitType["ZHSORT_UNIT"] = ZHSORT_UNIT;
	g_mUnitType["CB_UNIT"] = CB_UNIT;	g_mUnitType["KLIST_UNIT"] = KLIST_UNIT;
	g_mUnitType["SEAT_UNIT"] = SEAT_UNIT;
	g_mUnitType["RADAR_UNIT"] = RADAR_UNIT;
	g_mUnitType["WEB_UNIT"] = WEB_UNIT;
	g_mUnitType["TICKZST_UNIT"] = TICKZST_UNIT;
	g_mUnitType["F10_UNIT"] = F10_UNIT;
	g_mUnitType["RMREPORT_UNIT"] = RMREPORT_UNIT;
	g_mUnitType["STAR_UNIT"] = STAR_UNIT;
	g_mUnitType["MTSORT_UNIT"] = MTSORT_UNIT;
	g_mUnitType["GLHQ_UNIT"] = GLHQ_UNIT;

	//新Level2组件
	g_mUnitType["LOWHQ_UNIT"] = LOWHQ_UNIT;
	g_mUnitType["HQ10_UNIT"] = HQ10_UNIT;
	g_mUnitType["TICK2_UNIT"] = TICK2_UNIT;
	g_mUnitType["QUEUE_UNIT"] = QUEUE_UNIT;
	g_mUnitType["TICKSTAT_UNIT"] = TICKSTAT_UNIT;
	g_mUnitType["SCANCEL_UNIT"] = SCANCEL_UNIT;
	g_mUnitType["TCANCEL_UNIT"] = TCANCEL_UNIT;
	g_mUnitType["WTRANK_UNIT"] = WTRANK_UNIT;
	g_mUnitType["HYCJ_UNIT"] = HYCJ_UNIT;
	g_mUnitType["TV_UNIT"] = TV_UNIT;
	g_mUnitType["WTFB_UNIT"] = WTFB_UNIT;
	//外部接口直接加入
	for(i=0;i<MAX_GNPLUGIN_NUM;i++)
	{
		sprintf(g_GNPlug_TypeStr[i],"EXTERN%d_UNIT",i+1);
		g_mUnitType[g_GNPlug_TypeStr[i]] = EXTERN1_UNIT+i;
	}
	
	//
	g_mUnitStyle.clear();
	g_mUnitStyle["ZST_BIG"] = ZST_BIG;	g_mUnitStyle["ZST_LB"] = ZST_LB;
	g_mUnitStyle["ZST_MMLD"] = ZST_MMLD;	g_mUnitStyle["ZST_MULTI"] = ZST_MULTI;
	g_mUnitStyle["ZST_SMALL"] = ZST_SMALL;	g_mUnitStyle["ZST_GPZS"] = ZST_GPZS;
	g_mUnitStyle["ZST_SHDP"] = ZST_SHDP;	g_mUnitStyle["ZST_SZDP"] = ZST_SZDP;
	g_mUnitStyle["FXT_BIG"] = FXT_BIG;	g_mUnitStyle["FXT_MULTI"] = FXT_MULTI;
	g_mUnitStyle["FXT_SHDP"] = FXT_SHDP;	g_mUnitStyle["FXT_SZDP"] = FXT_SZDP;
	g_mUnitStyle["TICK_BIG"] = TICK_BIG;	g_mUnitStyle["TICK_SMALL"] = TICK_SMALL;
	g_mUnitStyle["FJB_BIG"] = FJB_BIG;	g_mUnitStyle["FJB_SMALL"] = FJB_SMALL;
	g_mUnitStyle["FSB_BIG"] = FSB_BIG;	g_mUnitStyle["FSB_SMALL"] = FSB_SMALL;
	g_mUnitStyle["MMP_BIG"] = MMP_BIG;	g_mUnitStyle["MMP_SMALL"] = MMP_SMALL;
	g_mUnitStyle["HQGRID_ALL"] = HQGRID_ALL;g_mUnitStyle["HQGRID_AG"] = HQGRID_AG;
	g_mUnitStyle["HQGRID_BG"] = HQGRID_BG; g_mUnitStyle["HQGRID_QZ"] = HQGRID_QZ;
	g_mUnitStyle["HQGRID_ZQ"] = HQGRID_ZQ;g_mUnitStyle["HQGRID_JJ"] = HQGRID_JJ;
	g_mUnitStyle["HQGRID_SB"] = HQGRID_SB;g_mUnitStyle["HQGRID_SPEC"] = HQGRID_SPEC;
	g_mUnitStyle["HQGRID_ZX"] = HQGRID_ZX;
	g_mUnitStyle["HQGRID_DY"] = HQGRID_DY;g_mUnitStyle["HQGRID_HY"] = HQGRID_HY;
	g_mUnitStyle["HQGRID_GN"] = HQGRID_GN;g_mUnitStyle["HQGRID_ZH"] = HQGRID_ZH;
	g_mUnitStyle["HQGRID_ZD"] = HQGRID_ZD;
	g_mUnitStyle["HQGRID_ZJHHY"] = HQGRID_ZJHHY;
	g_mUnitStyle["HQGRID_SHAG"] = HQGRID_SHAG;g_mUnitStyle["HQGRID_SZAG"] = HQGRID_SZAG;
	g_mUnitStyle["HQGRID_ZXB"] = HQGRID_ZXB;
	g_mUnitStyle["HQGRID_CYB"] = HQGRID_CYB;
	g_mUnitStyle["HQGRID_DYNA"] = HQGRID_DYNA;

	g_mUnitStyle["RMREPORT_HQ"] = RMREPORT_HQ;
	g_mUnitStyle["RMREPORT_ZJ"] = RMREPORT_ZJ;
	
	for(i=0,j=0;i<g_GuiMarketNum;i++)
	{
		sprintf(g_ppHqStyleStr[j],"HQGRID_%s",g_GuiMarketSet[i].BigsetName);
		g_mUnitStyle[g_ppHqStyleStr[j]] = HQGRID_DS1+j;
		++j;
	}
}

int CComView2::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CUnitView::OnCreate(lpCreateStruct) == -1)
		return -1;

	long bk_white[12]={ IDB_MYSWITCHBAR_L_N,IDB_MYSWITCHBAR_R_N,IDB_MYSWITCHBAR_BK_N,
						IDB_MYSWITCHBAR_L_D,IDB_MYSWITCHBAR_R_D,IDB_MYSWITCHBAR_BK_D,
						IDB_MYSWITCHBAR_L_O,IDB_MYSWITCHBAR_R_O,IDB_MYSWITCHBAR_BK_O,
						0,0,IDB_MYSWITCHBAR_BK };
	long bk[12] =	  {	IDB_MYSWITCHBAR_L_D,IDB_MYSWITCHBAR_R_D,IDB_MYSWITCHBAR_BK_D,
						IDB_MYSWITCHBAR_L_N,IDB_MYSWITCHBAR_R_N,IDB_MYSWITCHBAR_BK_N,
						IDB_MYSWITCHBAR_L_O,IDB_MYSWITCHBAR_R_O,IDB_MYSWITCHBAR_BK_O,
						0,0,IDB_MYSWITCHBAR_BK };
	long bk_bt[6]=	  {	IDB_MYSWITCHBAR_BT_H_LEFT,IDB_MYSWITCHBAR_BT_H_LEFT,IDB_MYSWITCHBAR_BT_H_LEFT,
						IDB_MYSWITCHBAR_BT_H_RIGHT,IDB_MYSWITCHBAR_BT_H_RIGHT,IDB_MYSWITCHBAR_BT_H_RIGHT };

	m_CustomPad_Bar.Init(ID_SWITCHBAR_H,MSBS_H_T,0,0);
	m_CustomPad_Bar.SetItemOffset(10);
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
		m_CustomPad_Bar.SetBK(bk);
	else
		m_CustomPad_Bar.SetBK(bk_white);
	m_CustomPad_Bar.SetButtonBK(bk_bt);

	COLORREF SwitchBakColor=VipColor.BACKCOLOR;
	COLORREF TextColor_N=RGB(0,0,0);
	COLORREF TextColor_D=RGB(255,255,255);
	if(g_d.IsBlackColorScheme(VipColor.BACKCOLOR))
	{
		TextColor_N=RGB(255,255,255);
		TextColor_D=RGB(0,0,0);
	}
	m_CustomPad_Bar.SetBkColor(SwitchBakColor,g_GUI.DarkenScaleColor(62,SwitchBakColor),g_GUI.LightenScaleColor(62,SwitchBakColor));
	m_CustomPad_Bar.SetTextColor(TextColor_N,TextColor_D,0);
	
	m_CustomPad_Bar.Create(NULL,"",WS_VISIBLE|WS_CHILD|WS_TABSTOP,CRect(0,0,0,0),this,ID_SWITCHBAR_H);

	return 0;
}

UnitTreeNode* FindLeaf(UnitTreeNode* pNode, int nLOrR)
{
	UnitTreeNode* leaf = pNode;
	if(nLOrR==0)
	{
		if(pNode->pLeftChild&&pNode->pLeftChild->pCurUnit->toSplit>1)
			leaf = FindLeaf(pNode->pLeftChild, nLOrR);
		else if(pNode->pLeftChild&&pNode->pLeftChild->pCurUnit->toSplit<1&&pNode->pRightChild->pCurUnit->toSplit>1)
			leaf = FindLeaf(pNode->pRightChild, nLOrR);
	}
	else if(nLOrR==1)
	{
		if(pNode->pRightChild&&pNode->pRightChild->pCurUnit->toSplit>1)
			leaf = FindLeaf(pNode->pLeftChild, nLOrR);
		else if(pNode->pRightChild&&pNode->pRightChild->pCurUnit->toSplit<1&&pNode->pLeftChild->pCurUnit->toSplit>1)
			leaf = FindLeaf(pNode->pLeftChild, nLOrR);
	}
	return leaf;
}

void CComView2::GetDefaultGP(CString padini)
{
	int nInfo,setcode;
	char info[160],tmpStr[100],tmpStr2[100],Code[100];
	for(int i=0;i < MAX_COM2_GP;i++)
	{
		sprintf(tmpStr,"GPSetCode_Code%d",i+1);
		sprintf(tmpStr2,"1_60080%d",i);
		nInfo = GetPrivateProfileString("DEAFULTGP", tmpStr, tmpStr2, info, 64, padini);
		if(nInfo>0)	
		{
			ParseMessageStr(info,tmpStr2,"_",1);
			setcode=atoi(tmpStr2);
			ParseMessageStr(info,Code,"_",2);			
			m_aCurGPSet[i] = g_pStockIO->GetIndex(Code,setcode);
			if(m_aCurGPSet[i] < 0) 
				m_aCurGPSet[i] = 100+i;
		}
	}
}

void CComView2::WriteDefaultGP(CString padini)
{
	MemStkInfo *tmpInfo;
	char tmpStr[100],tmpStr2[100];
	for(int i=0;i < MAX_COM2_GP;i++)
	{
		tmpInfo = (*g_pStockIO)[m_aCurGPSet[i]];
		if(tmpInfo)
		{
			sprintf(tmpStr,"GPSetCode_Code%d",i+1);
			sprintf(tmpStr2,"%d_%s",tmpInfo->setcode,tmpInfo->Code);
			WritePrivateProfileString("DEAFULTGP",tmpStr,tmpStr2, padini);
		}
	}
}

BOOL CComView2::GetCustomPadInfo(char *padName)
{
	CString padini; CFile ini;
	if(padName[0]==0) 
		padini = g_WSXHStr+"sheet\\tmp.spo";
	else if(strcmp(padName, nullPadName)==0) 
		padini = g_WSXHStr+(CString)"sheet\\"+(CString)padName+".spo";
	else 
		padini = g_WSXHStr+(CString)"sheet\\"+(CString)padName+".sp";
	if(!ini.Open(padini, CFile::modeRead|CFile::shareDenyNone))	
		return FALSE;
	ini.Close();
	strcpy(m_PadCode, padName);

	int i, nInfo, nUnitNum, nSplitWhich, nUnitType, nUnitStyle, nHowToSplit, nShowGpNo, nIsContrallUnit, nIsLocked=0;
	float fSplitRatio;
	char sStep[16], info[MAX_PATH];
	memset(sStep, 0, 16*sizeof(char)); memset(info, 0, MAX_PATH*sizeof(char));
	map<char*, int ,cmpstr>::iterator iter;
	//是否是系统的定制版面(不允许修改)
	m_Fixed = (BOOL)GetPrivateProfileInt("DEAFULTGP", "IsFixed", 0, padini);
	GetDefaultGP(padini);
	//Unit的数目
	nInfo = GetPrivateProfileString("DEAFULTGP", "UnitNum", IntToStr(3), info, 64, padini);
	if(nInfo>0)	nUnitNum = atoi(info);
	//
	for(i=0;i<nUnitNum;i++)
	{
		memset(sStep, 0, 16*sizeof(char)); memset(info, 0, 160*sizeof(char));
		sprintf(sStep, "STEP%d", i);

		nInfo = GetPrivateProfileString(sStep, "SplitWhich", IntToStr(i-1), info, 64, padini);
		if(nInfo>0) nSplitWhich = atoi(info);
		if(nSplitWhich>=i||nSplitWhich<0) nSplitWhich = i-1;

		nInfo = GetPrivateProfileString(sStep, "UnitType", "", info, 64, padini);
		iter = g_mUnitType.find(info);
		if(iter!=g_mUnitType.end()) nUnitType = (*iter).second;
		else nUnitType = -1;

		nInfo = GetPrivateProfileString(sStep, "UnitStyle", "", info, 64, padini);
		iter = g_mUnitStyle.find(info);
		if(iter!=g_mUnitStyle.end()) nUnitStyle = (*iter).second;
		else if(nUnitType>0) nUnitStyle = nUnitType*100+1;
		else nUnitStyle = -1;

		nInfo = GetPrivateProfileString(sStep, "HowToSplit", IntToStr(1), info, 64, padini);
		if(nInfo>0)	nHowToSplit = atoi(info);
		if(abs(nHowToSplit)>1) nHowToSplit = 1;

		nInfo = GetPrivateProfileString(sStep, "SplitRatio", IntToStr(50), info, 64, padini);
		if(nInfo>0)	fSplitRatio = atof(info);
		if(fSplitRatio<0&&fSplitRatio>100) fSplitRatio = 50;

		nInfo = GetPrivateProfileString(sStep, "ShowGpNo", IntToStr(1), info, 64, padini);
		if(nInfo>0)	nShowGpNo = atoi(info)-1;

		nInfo = GetPrivateProfileString(sStep, "IsContrallUnit", IntToStr(0), info, 64, padini);
		if(nInfo>0)	nIsContrallUnit = atoi(info);

		nInfo = GetPrivateProfileString(sStep, "IsLocked", IntToStr(0), info, 64, padini);
		if(nInfo>0)	nIsLocked = atoi(info);
		
		if(nUnitType==HQGRID_UNIT)
		{
			nInfo = GetPrivateProfileString(sStep, "GPType", IntToStr(6), info, 64, padini);
			if(nInfo>0)	m_WhichGPType[i] = atoi(info);
			else m_WhichGPType[i] = 6;

			nInfo = GetPrivateProfileString(sStep, "BlockFlag", IntToStr(-1), info, 64, padini);
			if(nInfo>0)	m_nBlockFlag[i] = atoi(info);
			else m_nBlockFlag[i] = -1;

			nInfo = GetPrivateProfileString(sStep, "JSZJFlag", IntToStr(-1), info, 64, padini);
			if(nInfo>0)	m_bJSZJFlag[i] = atoi(info);
			else m_bJSZJFlag[i] = -1;

			nInfo = GetPrivateProfileString(sStep, "SignalFlag", IntToStr(0), info, 64, padini);
			if(nInfo>0)	m_bSignalFlag[i] = atoi(info);
			else m_bSignalFlag[i] = 0;

			nInfo = GetPrivateProfileString(sStep, "ConFlag", IntToStr(-1), info, 64, padini);
			if(nInfo>0)	m_nConFlag[i] = atoi(info);
			else m_nConFlag[i] = -1;

			nInfo = GetPrivateProfileString(sStep, "ConFiltNum", IntToStr(20), info, 64, padini);
			if(nInfo>0)	m_nConFiltCount[i] = atoi(info);
			else m_nConFiltCount[i] = 20;

			int nTJFlag = -1;
			nInfo = GetPrivateProfileString(sStep, "JSTJFlag", IntToStr(-1), info, 64, padini);
			if(nInfo>0)	nTJFlag = atoi(info);
			int kk=0;
			if(nInfo>0)
			{
				for(int k = 0;k < g_nBlockNum;k++)
				{
					if(g_BlockInfo[k].KeyGuyCode[0]!='@') continue;
					if(kk==nTJFlag)
					{
						m_nBlockFlag[i] = k;
						m_WhichGPType[i] = -1;
						break;
					}
					kk++;
				}
			}

			//第三方市场容错
			if(m_WhichGPType[i]>=DS_START&&m_WhichGPType[i]<FY_START&&!g_bHasDS)
			{
				m_WhichGPType[i] = 6;
				m_nBlockFlag[i] = -1;
			}
			m_nSetFlag[i] = 1;
		}
		if(nUnitType==FXT_UNIT)	//在创建之前设置
		{
			GetPrivateProfileString(sStep,"InitShowNum",0,info,64,padini);
			l_LastSetSHOWNUM = atoi(info);			
		}
		if(i==0)	
			ConstructFirstUnit(nUnitType, nUnitStyle, nShowGpNo, nIsContrallUnit, nIsLocked);
		else 
			SplitPad(nSplitWhich, nUnitType, nUnitStyle, nHowToSplit, fSplitRatio, nShowGpNo, nIsContrallUnit, nIsLocked);

		if(nUnitType==FXT_UNIT)
		{
			l_LastSetSHOWNUM = -1;
			UFxt *pUFxt = (UFxt*)m_Units.pUnit[m_Units.Num-1];
			//
			FMLPACK tmpFMLPACK;
			char	tmpStr[100];
			memset(&tmpFMLPACK,0,sizeof(FMLPACK));
			GetPrivateProfileString(sStep,"WndNum",0,info,64,padini);
			tmpFMLPACK.lRectNum = atoi(info);
			nInfo = GetPrivateProfileString(sStep, "FXPeriod", IntToStr(4), info, 64, padini);
			if(nInfo>=0) 
				tmpFMLPACK.nPeriod = atoi(info)-1;
			else 
				tmpFMLPACK.nPeriod = -1;
			for(int j=0;j<tmpFMLPACK.lRectNum && j<10;j++)
			{
				sprintf(tmpStr,"Formula%02d",j+1);
				GetPrivateProfileString(sStep,tmpStr,0,tmpFMLPACK.aFormular[j],31,padini);
				tmpFMLPACK.aFormular[j][31] = 0;
				g_CF.TrimStr(tmpFMLPACK.aFormular[j]);
			}
			pUFxt->SetCom2Fml(&tmpFMLPACK,TRUE);
		}
		if(nUnitType==WEB_UNIT)
		{
			UWeb2 *pUWeb = (UWeb2*)m_Units.pUnit[m_Units.Num-1];
			nInfo = GetPrivateProfileString(sStep, "Url", "", info, MAX_PATH-1, padini);
			if(nInfo>0) pUWeb->SetURL(info,FALSE);
			nInfo = GetPrivateProfileString(sStep, "RefreshTime", "-1", info, MAX_PATH-1, padini);
			pUWeb->m_nRefreshEscape = atoi(info);
		}
		if(nUnitType==HQGRID_UNIT)
		{
			HQGrid *pHQGrid = (HQGrid*)m_Units.pUnit[m_Units.Num-1];
			pHQGrid->InsertSwitch(nUnitStyle-100*nUnitType);
		}
		if(nUnitType==MTSORT_UNIT)
		{
			USortGrid *pMTSort = (USortGrid*)m_Units.pUnit[m_Units.Num-1];
			int WhichType = (BOOL)GetPrivateProfileInt(sStep, "WhichType", TYPE_AG, padini);
			pMTSort->SetSCType(WhichType);
			nInfo = GetPrivateProfileString(sStep, "CalcCode", "100*(CLOSE-REF(CLOSE,1))/REF(CLOSE,1);", info, MAX_PATH-1, padini);
			if(nInfo>0) pMTSort->SetCalcCode(info);
		}
		if(nUnitType==ZHSORT_UNIT)
		{
			UZhSort *pUZhSort = (UZhSort*)m_Units.pUnit[m_Units.Num-1];
			int WhichGPType = 6;
			CString defVal = "012345678";
			nInfo = GetPrivateProfileString(sStep, "SortNum", IntToStr(2), info, 64, padini);
			if(nInfo>0)	
			{
				pUZhSort->m_nUnitRow = atoi(info);
				if(pUZhSort->m_nUnitRow!=2&&pUZhSort->m_nUnitRow!=3)
					pUZhSort->m_nUnitRow = 2;
			}
			nInfo = GetPrivateProfileString(sStep, "GPType", IntToStr(TYPE_AG), info, 64, padini);
			if(nInfo>0)	
			{
				pUZhSort->WhichType = atoi(info);
				if(pUZhSort->WhichType<0)
					pUZhSort->WhichType = TYPE_AG;
			}
			nInfo = GetPrivateProfileString(sStep, "SortOpt", defVal, info, 64, padini);
			CString sortVal(info);
			if(nInfo>0)	
			{
				for(int i=0;i<9;i++)
				{
					pUZhSort->m_aShowSort[i] = sortVal[i]-'0';
					if(pUZhSort->m_aShowSort[i]<0||pUZhSort->m_aShowSort[i]>8)
					{
						for(int j=0;j<9;j++) pUZhSort->m_aShowSort[j] = j;
						break;
					}
				}
			}
			if(pUZhSort->m_nUnitRow<3)
				pUZhSort->SetStyle(STYLE_2X2);
			else pUZhSort->SetStyle(STYLE_3X3);
			pUZhSort->ProcessMsg(WM_PAINT, 1);
		}
	}
	//
	for(i=0;i<nUnitNum;i++)
	{
		memset(sStep, 0, 16*sizeof(char)); memset(info, 0, 16*sizeof(char));
		sprintf(sStep, "STEP%d", i);
		int iNode = Index2NodeNo(i);
		if(iNode<0) continue;
		nInfo = GetPrivateProfileString(sStep, "Fixed_Width", NULL, info, 64, padini);
		if(nInfo>0)	m_apNode[iNode]->pCurUnit->nJHMin[0] = atoi(info);
		nInfo = GetPrivateProfileString(sStep, "Fixed_Height", NULL, info, 64, padini);
		if(nInfo>0)	m_apNode[iNode]->pCurUnit->nJHMin[1] = atoi(info);
		if(m_apNode[iNode]->pCurUnit->nJHMin[0]>0||m_apNode[iNode]->pCurUnit->nJHMin[1]>0) 
			ReJustJHMin(m_apNode[iNode]->pCurUnit);
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
CComView2::CComView2():CUnitView(TRUE)
{
	g_ppHqStyleStr = new LPSTR[MAX_MKT_NUM];
	for(int i=0;i<MAX_MKT_NUM;i++) g_ppHqStyleStr[i] = new char[64];
	SetStrMap();
	WindowMacro = COMVIEW2_WIN;

	for(i=0;i<MAX_NODE;i++)
	{
		m_WhichGPType[i] = -1;
		m_nBlockFlag[i] = -1;
		m_bJSZJFlag[i] = 0;
		m_bSignalFlag[i] = 0;
		m_nConFlag[i] = -1;
		m_nConFiltCount[i] = 20;
		m_nSetFlag[i] = 0;
	}

	m_Extern_GPSwitch = TRUE;	//缺省不会切换到comview
	
	m_bInSplit = FALSE;

	g_mUnitMenu.clear();
	for(i=0;i<MAX_NODE;i++) m_apNode[i] = NULL;
	memset(&m_Units, 0, sizeof(UNITS));
	m_aSplitInfo.clear();
	m_pMovingSplit = NULL;
	
	m_nSZZSIndex = g_pStockIO->GetIndex(SZ_DPCODE);
	m_nSHZSIndex = g_pStockIO->GetIndex(SH_DPCODE);

	m_PadName[0] = 0;
	m_PadCode[0] = 0;
	m_nNowPad = -1;
	m_NowPadSet = -1;
	m_NowPadCmdID = -1;
	m_pRootUnit = NULL;

	m_oldhit = -1;
	m_bCanMoveSplit = 0;
	m_bMoving = FALSE;
	
	for(i=0;i<MAX_COM2_GP;i++)
		m_aLastCUnit[i] = -1;

	m_nCurSel = -1;
}

CComView2::~CComView2()
{
	theViewManager.RemoveView(this);

	WriteCustomPadInfo(-1);
	DestroyTree();
	DestroyPad();
	m_aSplitInfo.clear();
	g_mUnitMenu.clear();

	for(int i=0;i<MAX_MKT_NUM;i++) delete [](g_ppHqStyleStr[i]);
	delete []g_ppHqStyleStr;
}

void CComView2::DestroyTree()
{
	for(int i=0;i<MAX_NODE;i++)
	{
		if(m_apNode[i])
		{
			if(m_apNode[i]->pCurUnit)
				TDEL(m_apNode[i]->pCurUnit);
			TDEL(m_apNode[i]);
		}
	}
}

void CComView2::InitMenuMap()
{
	g_mUnitMenu.clear();

	g_mUnitMenu[ID_INSERTHQGRID_ALL] = 1000*HQGRID_UNIT+HQGRID_ALL;
	g_mUnitMenu[ID_INSERTHQGRID_AG] = 1000*HQGRID_UNIT+HQGRID_AG;
	g_mUnitMenu[ID_INSERTHQGRID_BG] = 1000*HQGRID_UNIT+HQGRID_BG;
	g_mUnitMenu[ID_INSERTHQGRID_QZ] = 1000*HQGRID_UNIT+HQGRID_QZ;
	g_mUnitMenu[ID_INSERTHQGRID_ZQ] = 1000*HQGRID_UNIT+HQGRID_ZQ;
	g_mUnitMenu[ID_INSERTHQGRID_JJ] = 1000*HQGRID_UNIT+HQGRID_JJ;
	g_mUnitMenu[ID_INSERTHQGRID_SB] = 1000*HQGRID_UNIT+HQGRID_SB;
	g_mUnitMenu[ID_INSERTHQGRID_SPEC] = 1000*HQGRID_UNIT+HQGRID_SPEC;
	g_mUnitMenu[ID_INSERTHQGRID_ZX] = 1000*HQGRID_UNIT+HQGRID_ZX;
	g_mUnitMenu[ID_INSERTHQGRID_DY] = 1000*HQGRID_UNIT+HQGRID_DY;
	g_mUnitMenu[ID_INSERTHQGRID_HY] = 1000*HQGRID_UNIT+HQGRID_HY;
	g_mUnitMenu[ID_INSERTHQGRID_GN] = 1000*HQGRID_UNIT+HQGRID_GN;
	g_mUnitMenu[ID_INSERTHQGRID_ZH] = 1000*HQGRID_UNIT+HQGRID_ZH;
	g_mUnitMenu[ID_INSERTHQGRID_ZD] = 1000*HQGRID_UNIT+HQGRID_ZD;
	g_mUnitMenu[ID_INSERTHQGRID_ZJHHY] = 1000*HQGRID_UNIT+HQGRID_ZJHHY;
	g_mUnitMenu[ID_INSERTHQGRID_SHAG] = 1000*HQGRID_UNIT+HQGRID_SHAG;
	g_mUnitMenu[ID_INSERTHQGRID_SZAG] = 1000*HQGRID_UNIT+HQGRID_SZAG;
	g_mUnitMenu[ID_INSERTHQGRID_ZXB] = 1000*HQGRID_UNIT+HQGRID_ZXB;
	g_mUnitMenu[ID_INSERTHQGRID_CYB] = 1000*HQGRID_UNIT+HQGRID_CYB;
	for(int j=0;j<g_GuiMarketNum;j++)
	{
		g_mUnitMenu[ID_INSERTHQDS1+j] = 1000*HQGRID_UNIT+HQGRID_DS1+j;
	}
	g_mUnitMenu[ID_INSERTZHSORT] = 1000*ZHSORT_UNIT;

	g_mUnitMenu[ID_INSERTZST_BIG] = 1000*ZST_UNIT+ZST_BIG;
	g_mUnitMenu[ID_INSERTZST_LB] = 1000*ZST_UNIT+ZST_LB;
	g_mUnitMenu[ID_INSERTZST_MMLD] = 1000*ZST_UNIT+ZST_MMLD;
	g_mUnitMenu[ID_INSERTZST_MULTI] = 1000*ZST_UNIT+ZST_MULTI;
	g_mUnitMenu[ID_INSERTZST_SMALL] = 1000*ZST_UNIT+ZST_SMALL;
	g_mUnitMenu[ID_INSERTZST_GPZS] = 1000*ZST_UNIT+ZST_GPZS;
	g_mUnitMenu[ID_INSERTZST_SHDP] = 1000*ZST_UNIT+ZST_SHDP;
	g_mUnitMenu[ID_INSERTZST_SZDP] = 1000*ZST_UNIT+ZST_SZDP;

	g_mUnitMenu[ID_INSERTFXT_BIG] = 1000*FXT_UNIT+FXT_BIG;
	g_mUnitMenu[ID_INSERTFXT_MULTI] = 1000*FXT_UNIT+FXT_MULTI;
	g_mUnitMenu[ID_INSERTFXT_SHDP] = 1000*FXT_UNIT+FXT_SHDP;
	g_mUnitMenu[ID_INSERTFXT_SZDP] = 1000*FXT_UNIT+FXT_SZDP;

	g_mUnitMenu[ID_INSERTHQ] = 1000*HQ_UNIT;

	g_mUnitMenu[ID_INSERTTICK_BIG] = 1000*TICK_UNIT+TICK_BIG;
	g_mUnitMenu[ID_INSERTTICK_SMALL] = 1000*TICK_UNIT+TICK_SMALL;

	g_mUnitMenu[ID_INSERTFJB_BIG] = 1000*FJB_UNIT+FJB_BIG;
	g_mUnitMenu[ID_INSERTFJB_SMALL] = 1000*FJB_UNIT+FJB_SMALL;

	g_mUnitMenu[ID_INSERTCDP] = 1000*CDP_UNIT;

	g_mUnitMenu[ID_INSERTFSB_BIG] = 1000*FSB_UNIT+FSB_BIG;
	g_mUnitMenu[ID_INSERTFSB_SMALL] = 1000*FSB_UNIT+FSB_SMALL;

	g_mUnitMenu[ID_INSERTMMP_BIG] = 1000*MMP_UNIT+MMP_BIG;
	g_mUnitMenu[ID_INSERTMMP_SMALL] = 1000*MMP_UNIT+MMP_SMALL;

	//Level2组件
	g_mUnitMenu[ID_INSERT_HQ10] = 1000*HQ10_UNIT;
	g_mUnitMenu[ID_INSERT_QUEUE] = 1000*QUEUE_UNIT;
	g_mUnitMenu[ID_INSERT_TICKSTAT] = 1000*TICKSTAT_UNIT;
	g_mUnitMenu[ID_INSERT_TICK2] = 1000*TICK2_UNIT+TICK_SMALL;
	g_mUnitMenu[ID_INSERT_LOWHQ] = 1000*LOWHQ_UNIT;
	g_mUnitMenu[ID_INSERT_SCANCEL] = 1000*SCANCEL_UNIT;
	g_mUnitMenu[ID_INSERT_TCANCEL] = 1000*TCANCEL_UNIT;
	g_mUnitMenu[ID_INSERT_WTRANK] = 1000*WTRANK_UNIT;
	g_mUnitMenu[ID_INSERT_HYCJ] = 1000*HYCJ_UNIT;
	g_mUnitMenu[ID_INSERT_TV] = 1000*TV_UNIT;
	g_mUnitMenu[ID_INSERT_WTFB] = 1000*WTFB_UNIT;

	g_mUnitMenu[ID_INSERTCB] = 1000*CB_UNIT;

	g_mUnitMenu[ID_INSERTBIGVOL] = 1000*BIGVOL_UNIT;
	g_mUnitMenu[ID_INSERTRADAR] = 1000*RADAR_UNIT;
	g_mUnitMenu[ID_INSERTWEB] = 1000*WEB_UNIT;

	g_mUnitMenu[ID_INSERTBSSEAT] = 1000*SEAT_UNIT;

	g_mUnitMenu[ID_INSERTTICKZST] = 1000*TICKZST_UNIT;
	g_mUnitMenu[ID_INSERTF10] = 1000*F10_UNIT;
	g_mUnitMenu[ID_INSERTRMREPORT] = 1000*RMREPORT_UNIT;
	g_mUnitMenu[ID_INSERTSTAR] = 1000*STAR_UNIT;
}

void CComView2::InsertCustomPadSwitch(int nCurSel)
{
	m_CustomPad_Bar.SetMyRedraw(FALSE);

	m_CustomPad_Bar.Clear();	
//	m_CustomPad_Bar.InsertSwitch(-1,ID_CUSPAD_MENU,_F("定制版面"),_F("定制版面操作和总体切换菜单"),1);
//	m_CustomPad_Bar.ChangeLabelBehavior(0);
	int nShortCutNum = 0;
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(!g_vDynaCmdTab[i].ShowShortCut) continue;
		if(g_vDynaCmdTab[i].DefPadSet!=m_NowPadSet) continue;
		m_CustomPad_Bar.InsertSwitch(-1,ID_CUSPAD_BASE+i,_F(g_vDynaCmdTab[i].ChName),_F(g_vDynaCmdTab[i].ChName),1);
		if(strcmp(m_PadCode,g_vDynaCmdTab[i].EnName)==0)
			m_nCurSel = nShortCutNum;
		++nShortCutNum;
	}
	m_CustomPad_Bar.SetMyRedraw(TRUE);

	m_CustomPad_Bar.SetSel(nCurSel==-1?m_nCurSel:nCurSel);	
}

void CComView2::OnInitialUpdate()
{
	CUnitView::OnInitialUpdate();

	theViewManager.AddView("特色分析", this);
	int nDefNo = GetCusPadTypeParam(g_DefCusPadName);
	if(nDefNo>=0)	SendMessage(UM_CONSTRUCTPAD_MESSAGE, nDefNo,0);
	
	SendMessage(UM_REFRESHCUSTOMBAR);
}

void CComView2::ConstructFirstUnit(int newUnit, int style, int nGpNo, BOOL bContrallUnit, BOOL bLocked)
{
	m_pRootUnit = new UnitTreeNode; memset(m_pRootUnit, 0, sizeof(UnitTreeNode));
	m_pRootUnit->pCurUnit = new UnitNodeInfo; memset(m_pRootUnit->pCurUnit, 0, sizeof(UnitNodeInfo));

	ASSERT(m_Units.Num==0);

	short tmpGPNo = 0;
	if(nGpNo==-1)				 
		tmpGPNo=m_nSZZSIndex;
	else if(nGpNo==-2)			 
		tmpGPNo=m_nSHZSIndex;
	else if(nGpNo>=0 && nGpNo<MAX_COM2_GP) 
		tmpGPNo=m_aCurGPSet[nGpNo];
	CreateUnit(tmpGPNo, newUnit, style, m_Units.Num, &(m_pRootUnit->pCurUnit->pUnit));

	m_pRootUnit->pCurUnit->bContrallUnit = bContrallUnit;
	m_pRootUnit->pCurUnit->NodeID = 0;
	m_pRootUnit->pCurUnit->rect = m_Rect;
	m_pRootUnit->pCurUnit->splitType = 0;
	m_pRootUnit->pCurUnit->style = style;
	m_pRootUnit->pCurUnit->toSplit = 100;
	m_pRootUnit->pCurUnit->unitType = newUnit;
	m_pRootUnit->pCurUnit->var_GpIndex = nGpNo+GPINDEX_VAR1;
	m_pRootUnit->pCurUnit->bLocked = bLocked;
	memset(m_pRootUnit->pCurUnit->nJHMin, 0, 2*sizeof(long));
	//同步m_Units信息
	m_Units.pUnit[m_Units.Num] = m_pRootUnit->pCurUnit->pUnit;
	m_Units.Type[m_Units.Num] = m_pRootUnit->pCurUnit->unitType;
	m_Units.Rect[m_Units.Num] = m_pRootUnit->pCurUnit->rect;
	m_Units.Style[m_Units.Num] = m_pRootUnit->pCurUnit->style;
	m_Units.Var_GPIndex[m_Units.Num] = m_pRootUnit->pCurUnit->var_GpIndex;
	for(int k=0;k < MAX_COM2_GP;k++)
	{
		if(m_Units.Var_GPIndex[m_Units.Num] == GPINDEX_VAR1+k) 
			ChangeUnitGP(m_Units.pUnit[m_Units.Num], m_aCurGPSet[k]);
	}
	m_Units.nStatus[m_Units.Num] = U_SHOW;
	if(m_Units.Num<MAX_UNIT_NUM) ++m_Units.Num;

	m_apNode[0] = m_pRootUnit;
}

void CComView2::SwapUnit(int ua, int ub, BOOL bSwapSplit)
{
	if(ua==ub||ua>m_Units.Num||ub>m_Units.Num)	return;

	int nodea = Index2NodeNo(ua);
	int nodeb = Index2NodeNo(ub);
	if(nodea<0||nodeb<0) return;
	
	UnitNodeInfo tmpNodea, tmpNodeb;
	memcpy(&tmpNodea, m_apNode[nodeb]->pCurUnit, sizeof(UnitNodeInfo));
	memcpy(&tmpNodeb, m_apNode[nodea]->pCurUnit, sizeof(UnitNodeInfo));

	tmpNodea.NodeID = m_apNode[nodea]->pCurUnit->NodeID;
	tmpNodeb.NodeID = m_apNode[nodeb]->pCurUnit->NodeID;

	memcpy(m_apNode[nodea]->pCurUnit, &tmpNodea, sizeof(UnitNodeInfo));
	memcpy(m_apNode[nodeb]->pCurUnit, &tmpNodeb, sizeof(UnitNodeInfo));
}

BOOL CComView2::SplitPad(int nWhich, int newUnit, int style, int splitType, float fToSplit, int nGpNo, BOOL bContrallUnit, BOOL bLocked)
{
	int iNode = Index2NodeNo(nWhich);
	if(iNode>-1)
		return SplitPad(m_apNode[iNode], newUnit, style, splitType, fToSplit, nGpNo, bContrallUnit, bLocked);
	return FALSE;
}

BOOL CComView2::SplitPad(UnitTreeNode *nodeInTree, int newUnit, int style, int splitType, float fToSplit, int nGpNo, BOOL bContrallUnit, BOOL bLocked)
{
	if(!nodeInTree||!nodeInTree->pCurUnit||nodeInTree->pLeftChild||nodeInTree->pRightChild) return FALSE;			//失败后要重整Tree

	//新建左右子节点信息
	UnitNodeInfo *left = new UnitNodeInfo; memset(left, 0, sizeof(UnitNodeInfo));
	UnitNodeInfo *right = new UnitNodeInfo; memset(right, 0, sizeof(UnitNodeInfo));

	if(fToSplit<1) fToSplit = 0;
	else if(fToSplit>99) fToSplit = 100;
	//原节点重构
	memcpy(left, nodeInTree->pCurUnit, sizeof(UnitNodeInfo));
	nodeInTree->pCurUnit->bContrallUnit = FALSE;
	nodeInTree->pCurUnit->pUnit = NULL;
	nodeInTree->pCurUnit->splitType = splitType;
	nodeInTree->pCurUnit->style = -1;
	nodeInTree->pCurUnit->unitType = -1;
	nodeInTree->pCurUnit->var_GpIndex = -1;
	nodeInTree->pCurUnit->bLocked = TRUE;
	memset(nodeInTree->pCurUnit->nJHMin, 0, 2*sizeof(long));
	left->NodeID = 2*left->NodeID+1;
	if(left->NodeID>=MAX_NODE || m_Units.Num>=MAX_UNIT_NUM)
	{
		TDX_MessageBox(NULL,"不要再乱折腾啦,偶受不了的啦!",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	left->splitType = 0;
	left->toSplit = 100 - fToSplit;

	short tmpGPNo = 0;
	if(nGpNo==-1)				 
		tmpGPNo=m_nSZZSIndex;
	else if(nGpNo==-2)			 
		tmpGPNo=m_nSHZSIndex;
	else if(nGpNo>=0 && nGpNo<MAX_COM2_GP) 
		tmpGPNo=m_aCurGPSet[nGpNo];
	CreateUnit(tmpGPNo, newUnit, style, m_Units.Num, &(right->pUnit));

	right->bContrallUnit = bContrallUnit;
	right->NodeID = left->NodeID + 1;
	right->splitType = splitType;
	right->style = style;
	right->toSplit = fToSplit;
	right->unitType = newUnit;
	right->var_GpIndex = nGpNo+GPINDEX_VAR1;
	right->bLocked = bLocked;
	memset(right->nJHMin, 0, 2*sizeof(long));
	//重算区域
	SplitRect(left, right, nodeInTree->pCurUnit);
	//收集SplitManInfo
	SplitManInfo newSplit;
	newSplit.splitType = splitType;
	newSplit.node = nodeInTree;
	if(fToSplit<1) newSplit.cursorRt = CRect(0,0,0,0);
	if(splitType==1)
		newSplit.cursorRt = CRect(left->rect.right-CURSOR_WIDTH, left->rect.top, right->rect.left+CURSOR_WIDTH, right->rect.bottom);
	else if(splitType==-1)
		newSplit.cursorRt = CRect(left->rect.left, left->rect.bottom-CURSOR_WIDTH, right->rect.right, right->rect.top+CURSOR_WIDTH);
	else newSplit.cursorRt = CRect(0,0,0,0);
	newSplit.borderColor = VipColor.AXISCOLOR;
	m_aSplitInfo.push_back(newSplit);
	//同步m_Units信息
	m_Units.pUnit[m_Units.Num] = right->pUnit;
	m_Units.Type[m_Units.Num] = right->unitType;
	m_Units.Style[m_Units.Num] = right->style;
	m_Units.Rect[m_Units.Num] = right->rect; 
	m_Units.Var_GPIndex[m_Units.Num] = right->var_GpIndex;
	for(int k=0;k < MAX_COM2_GP;k++)
	{
		if(m_Units.pUnit[m_Units.Num] && m_Units.Var_GPIndex[m_Units.Num] == GPINDEX_VAR1+k) 
			ChangeUnitGP(m_Units.pUnit[m_Units.Num], m_aCurGPSet[k]);
	}
	if(fToSplit<1) 
		m_Units.nStatus[m_Units.Num] = U_HIDE;
	else 
		m_Units.nStatus[m_Units.Num] = U_SHOW;
	++m_Units.Num;
	for(int i=0;i<m_Units.Num;i++)
		if(m_Units.pUnit[i] == left->pUnit)
			m_Units.Rect[i] = left->rect;
	//新建左右子节点
	UnitTreeNode *leftNode = new UnitTreeNode; memset(leftNode, 0, sizeof(UnitTreeNode));
	UnitTreeNode *rightNode = new UnitTreeNode; memset(rightNode, 0, sizeof(UnitTreeNode));
	leftNode->pCurUnit = left;
	leftNode->pLeftChild = leftNode->pRightChild = NULL;
	leftNode->pParent = nodeInTree;
	rightNode->pCurUnit = right;
	rightNode->pLeftChild = rightNode->pRightChild = NULL;
	rightNode->pParent = nodeInTree;
	//重树
	nodeInTree->pLeftChild = leftNode;
	nodeInTree->pRightChild = rightNode;
	//节点索引
	m_apNode[left->NodeID] = leftNode;
	m_apNode[right->NodeID] = rightNode;

	return TRUE;
}

void CComView2::ReJustJHMin(UnitNodeInfo *pNode)
{
	int iNode = -1;
	for(int i=0;i<MAX_NODE;i++)
	{
		if(!m_apNode[i]||!m_apNode[i]->pCurUnit) continue;
		if(m_apNode[i]->pCurUnit==pNode)
		{
			iNode = i;
			break;
		}
	}
	if(iNode>=0&&m_apNode[iNode]->pParent&&m_apNode[iNode]->pParent->pCurUnit)
	{
		if(m_apNode[iNode]->pParent->pCurUnit->splitType==1)
		{
			m_apNode[iNode]->pParent->pCurUnit->nJHMin[0] = 
				m_apNode[iNode]->pParent->pLeftChild->pCurUnit->nJHMin[0] + 
				m_apNode[iNode]->pParent->pRightChild->pCurUnit->nJHMin[0];
			m_apNode[iNode]->pParent->pCurUnit->nJHMin[1] = 
				MAX(m_apNode[iNode]->pParent->pLeftChild->pCurUnit->nJHMin[1],  
				m_apNode[iNode]->pParent->pRightChild->pCurUnit->nJHMin[1]);
		}
		if(m_apNode[iNode]->pParent->pCurUnit->splitType==-1)
		{
			m_apNode[iNode]->pParent->pCurUnit->nJHMin[1] = 
				m_apNode[iNode]->pParent->pLeftChild->pCurUnit->nJHMin[1] + 
				m_apNode[iNode]->pParent->pRightChild->pCurUnit->nJHMin[1];
			m_apNode[iNode]->pParent->pCurUnit->nJHMin[0] = 
				MAX(m_apNode[iNode]->pParent->pLeftChild->pCurUnit->nJHMin[0],  
				m_apNode[iNode]->pParent->pRightChild->pCurUnit->nJHMin[0]);
		}
		ReJustJHMin(m_apNode[iNode]->pParent->pCurUnit);
	}
}

void CComView2::SplitRect(UnitNodeInfo *lNode, UnitNodeInfo *rNode, UnitNodeInfo *pNode)
{
	CRect lRect, rRect, oriRect = pNode->rect;
	lRect = oriRect; memset(&rRect, 0, sizeof(CRect));
	if(pNode->splitType==0) return;
	if(lNode->toSplit<1)
	{
		lRect = CRect(0, 0, 0, 0);
		rRect.CopyRect(&oriRect);
		lNode->rect.CopyRect(&lRect);
		rNode->rect.CopyRect(&rRect);
		return;
	}
	else if(lNode->toSplit>99)
	{
		rRect = CRect(0, 0, 0, 0);
		lRect.CopyRect(&oriRect);
		lNode->rect.CopyRect(&lRect);
		rNode->rect.CopyRect(&rRect);
		return;
	}
	int i, il=-1, ir=-1;
	FOR_UNIT(i)
	{
		if(il==-1&&lNode->pUnit==m_Units.pUnit[i])
		{
			il = i;
		}
		if(ir==-1&&rNode->pUnit==m_Units.pUnit[i])
		{
			ir = i;
		}
		if(il>=0 && ir>=0)
			break;
	}
	if(pNode->splitType == -1)
	{
		lRect.bottom = oriRect.top + (float)(lNode->toSplit/100.0f)*oriRect.Height();
		
		rRect.top = lRect.bottom;
		rRect.bottom = oriRect.bottom;
		rRect.left = oriRect.left;
		rRect.right = oriRect.right;

		if(lNode->nJHMin[1]>=lRect.Height()&&lNode->nJHMin[1]<=oriRect.Height())
		{
			lRect.bottom = oriRect.top + lNode->nJHMin[1];
			rRect.top = lRect.bottom;
		}
		else if(lNode->nJHMin[1]>=oriRect.Height())
		{
			lRect.CopyRect(&oriRect);
			rRect = CRect(0,0,0,0);
		}

		else if(rNode->nJHMin[1]>=rRect.Height()&&rNode->nJHMin[1]<=oriRect.Height())
		{
			rRect.top = oriRect.bottom - rNode->nJHMin[1];
			lRect.bottom = rRect.top;
		}
		else if(rNode->nJHMin[1]>=oriRect.Height())
		{
			rRect.CopyRect(&oriRect);
			lRect = CRect(0,0,0,0);
		}
		
		else if(lNode->nJHMin[1]>0)
		{
			lRect.bottom = max(lRect.bottom, oriRect.top + lNode->nJHMin[1]);
			rRect.top = lRect.bottom;
		}
		
		else if(rNode->nJHMin[1]>0)
		{
			rRect.top = min(rRect.top, oriRect.bottom - rNode->nJHMin[1]);
			lRect.bottom = rRect.top;
		}
	}
	else if(pNode->splitType == 1)
	{
		lRect.right = oriRect.left + (float)(lNode->toSplit/100.0f)*oriRect.Width();

		rRect.top = oriRect.top;
		rRect.bottom = oriRect.bottom;
		rRect.left = lRect.right;
		rRect.right = oriRect.right;

		if(lNode->nJHMin[0]>=lRect.Width()&&lNode->nJHMin[0]<oriRect.Width())
		{
			lRect.right = oriRect.left + lNode->nJHMin[0];
			rRect.left = lRect.right;			
		}
		else if(lNode->nJHMin[0]>=oriRect.Width())
		{
			lRect.CopyRect(&oriRect);
			rRect = CRect(0,0,0,0);
		}
		
		else if(rNode->nJHMin[0]>=rRect.Width()&&rNode->nJHMin[0]<oriRect.Width())
		{
			rRect.left = oriRect.right - rNode->nJHMin[0];
			lRect.right = rRect.left;
		}
		else if(rNode->nJHMin[0]>=oriRect.Width())
		{
			rRect.CopyRect(&oriRect);
			lRect = CRect(0,0,0,0);
		}

		else if(lNode->nJHMin[0]>0)
		{
			lRect.right = max(lRect.right, oriRect.left + lNode->nJHMin[0]);
			rRect.left = lRect.right;
		}

		else if(rNode->nJHMin[0]>0)
		{
			rRect.left = min(rRect.left, oriRect.right - rNode->nJHMin[0]);
			lRect.right = rRect.left;
		}
	}

	lNode->rect.CopyRect(&lRect);
	if(il>=0)
	{
		if(lNode->nJHMin[0]>lNode->rect.Width())
			lNode->rect.right = lNode->rect.left + lNode->nJHMin[0];
		if(lNode->nJHMin[1]>lNode->rect.Height())
			lNode->rect.bottom = lNode->rect.top + lNode->nJHMin[1];
	}

	rNode->rect.CopyRect(&rRect);
	if(ir>=0)
	{
		if(rNode->nJHMin[0]>rNode->rect.Width())
			rNode->rect.right = rNode->rect.left + rNode->nJHMin[0];
		if(rNode->nJHMin[1]>rNode->rect.Height())
			rNode->rect.bottom = rNode->rect.top + rNode->nJHMin[1];
	}
}

//隐藏某Unit，自下而上返回某父节点
UnitTreeNode* CComView2::HideOneUnit(UnitTreeNode* pNode)
{
	if(!pNode) return NULL;
	if(!pNode->pParent||!pNode->pParent->pCurUnit) return NULL;
	UnitTreeNode* pParNode = pNode->pParent;
	if(pParNode->pLeftChild&&pParNode->pLeftChild==pNode)
	{
		pParNode->pLeftChild->pCurUnit->toSplit=0;
		pParNode->pLeftChild->pCurUnit->nJHMin[0] = 
			pParNode->pLeftChild->pCurUnit->nJHMin[1] = 0;
		pParNode->pLeftChild->pCurUnit->rect = CRect(0,0,0,0);
		if(pParNode->pRightChild->pCurUnit->toSplit<1)
			pParNode = HideOneUnit(pParNode);
		else pParNode->pRightChild->pCurUnit->toSplit=100;
	}
	else if(pParNode->pRightChild&&pParNode->pRightChild==pNode)
	{
		pParNode->pRightChild->pCurUnit->toSplit=0;
		pParNode->pRightChild->pCurUnit->nJHMin[0] = 
			pParNode->pRightChild->pCurUnit->nJHMin[1] = 0;
		pParNode->pRightChild->pCurUnit->rect = CRect(0,0,0,0);
		if(pParNode->pLeftChild->pCurUnit->toSplit<1)
			pParNode = HideOneUnit(pParNode);
		else pParNode->pLeftChild->pCurUnit->toSplit=100;
	}
	return pParNode;
}

void CComView2::GetSplitInfo(UnitTreeNode* pNode)
{
	if(!pNode) return;
	if(!pNode->pCurUnit||pNode->pCurUnit->toSplit<1) return;
	SplitActInfo split;
	memset(&split, 0, sizeof(SplitActInfo));
	if((!pNode->pLeftChild||!pNode->pRightChild)&&pNode==m_pRootUnit)
	{
		split.nDepUnit = -1;
		split.nSplitType = 0;
		split.fToSplit = 100;
		split.pNode = m_pRootUnit;
		m_apSplitSeq.push_back(split);
		return;
	}
	if(!pNode->pLeftChild||!pNode->pRightChild) return;
	if(pNode==m_pRootUnit) m_apSplitSeq.clear();

	int i, nDep;
	UnitTreeNode* pLeftNode = NULL;
	UnitTreeNode* pRightNode = NULL;
	pLeftNode = FindLeaf(pNode, 0);
	//首次建的Unit
	if(pLeftNode&&!pLeftNode->pLeftChild&&!pLeftNode->pRightChild&&
		pLeftNode->pCurUnit->pUnit&&pLeftNode->pCurUnit->toSplit>1&&m_apSplitSeq.empty()) 
	{
		split.nDepUnit = -1;
		split.nSplitType = 0;
		split.fToSplit = 100;
		split.pNode = pLeftNode;
		m_apSplitSeq.push_back(split);
	}
	nDep = -1;
	for(i=0;i<m_apSplitSeq.size();i++)
	{
		if(m_apSplitSeq[i].pNode==pLeftNode)
		{
			nDep = i;
			break;
		}
	}
	memset(&split, 0, sizeof(SplitActInfo));
	pRightNode = FindLeaf(pNode->pRightChild, 0);
	if(pRightNode&&!pRightNode->pLeftChild&&!pRightNode->pRightChild&&pRightNode!=pLeftNode&&
		pRightNode->pCurUnit->pUnit&&pRightNode->pCurUnit->toSplit>1)
	{
		split.nDepUnit = nDep;
		split.nSplitType = pNode->pCurUnit->splitType;
		split.fToSplit = pNode->pRightChild->pCurUnit->toSplit;
		split.pNode = pRightNode;
		if(m_apSplitSeq.empty())				//首建节点已删除
		{
			split.nSplitType = 0;
			split.fToSplit = 100;
		}
		
		for(i=0;i<m_apSplitSeq.size();i++)
		{
			if(m_apSplitSeq[i].pNode==split.pNode)
				break;
		}
		if(i==m_apSplitSeq.size())
			m_apSplitSeq.push_back(split);
	}
	GetSplitInfo(pNode->pLeftChild);
	GetSplitInfo(pNode->pRightChild);
}

//重新分割部分PAD,从某父节点起自上而下
void CComView2::ReSplitPad(UnitTreeNode* pNode, int flag)
{
	int nLeafNode = m_Units.Num, i;

	if(m_apNode[0]&&(pNode == m_apNode[0]||pNode==m_pRootUnit)) pNode->pCurUnit->rect = m_Rect;
	if(pNode->pLeftChild&&pNode->pRightChild)
	{
		SplitRect(pNode->pLeftChild->pCurUnit, pNode->pRightChild->pCurUnit, pNode->pCurUnit);
		for(i=0;i<m_aSplitInfo.size();i++)			//调整分割条
		{
			if(m_aSplitInfo[i].node == pNode)
			{
				m_aSplitInfo[i].splitType = pNode->pCurUnit->splitType;
				if(pNode->pLeftChild->pCurUnit->rect == CRect(0, 0, 0, 0) ||	
					pNode->pRightChild->pCurUnit->rect == CRect(0,0,0,0))
					m_aSplitInfo[i].cursorRt = CRect(0,0,0,0);
				else if(m_aSplitInfo[i].splitType==1)
					m_aSplitInfo[i].cursorRt = CRect(pNode->pLeftChild->pCurUnit->rect.right-CURSOR_WIDTH, pNode->pLeftChild->pCurUnit->rect.top, pNode->pRightChild->pCurUnit->rect.left+CURSOR_WIDTH, pNode->pRightChild->pCurUnit->rect.bottom);
				else if(m_aSplitInfo[i].splitType==-1)
					m_aSplitInfo[i].cursorRt = CRect(pNode->pLeftChild->pCurUnit->rect.left, pNode->pLeftChild->pCurUnit->rect.bottom-CURSOR_WIDTH, pNode->pRightChild->pCurUnit->rect.right, pNode->pRightChild->pCurUnit->rect.top+CURSOR_WIDTH);
				else m_aSplitInfo[i].cursorRt = CRect(0,0,0,0);
			}
		}
		if(!pNode->pLeftChild->pLeftChild&&!pNode->pLeftChild->pRightChild)
		{
			for(i=0;i<nLeafNode;i++)
			{
				if(pNode->pLeftChild->pCurUnit->pUnit==m_Units.pUnit[i])
				{
					SetUnitReadAndPaint(i,pNode->pLeftChild->pCurUnit->rect,flag);
					break;
				}
			}
		}
		else ReSplitPad(pNode->pLeftChild, flag);
		if(!pNode->pRightChild->pLeftChild&&!pNode->pRightChild->pRightChild)
		{
			for(i=0;i<nLeafNode;i++)
			{
				if(pNode->pRightChild->pCurUnit->pUnit==m_Units.pUnit[i])
				{
					SetUnitReadAndPaint(i,pNode->pRightChild->pCurUnit->rect,flag);
					break;
				}
			}
		}
		else ReSplitPad(pNode->pRightChild, flag);
	}
	Invalidate();
}

void CComView2::ConstructPadMessage(WPARAM wParam,LPARAM lParam)//wParam:Pad类型 lParam:是否强制构建
{
	try	//关键代码,截获异常
	{
		tmp_SetGPtime=time(NULL);
		if(m_nNowPad==-1) tmp_SetGPtime+=1L;
		if(m_bInSplit&&(strcmp(m_PadName,"tmp")==0||strcmp(m_PadCode,"tmp")==0)&&
			m_nNowPad!=wParam)
			OnSavepad();
		if(wParam<0 || (m_nNowPad==wParam && lParam==0))	
			return;
		//重新计算深沪大盘的索引,因为DataIO可能已经销毁过
		m_nSZZSIndex = g_pStockIO->GetIndex(SZ_DPCODE);
		m_nSHZSIndex = g_pStockIO->GetIndex(SH_DPCODE);
		//
		WriteCustomPadInfo(-1);
		DestroyTree();
		DestroyPad();
		m_aSplitInfo.clear();
		m_apSplitSeq.clear();

		SetMainID();
		m_bInSplit = TRUE;
		m_nF = m_tmpNf = -1;
		m_nNowPad = wParam;
		if(m_nNowPad==1208) 
		{
			m_bInSplit = TRUE;
			strcpy(m_PadName, nullPadName);
			strcpy(m_PadCode, nullPadName);
		}
		else 
		{
			m_bInSplit = FALSE;
			if(m_nNowPad>=0&&m_nNowPad<g_vDynaCmdTab.size()) 
			{
				vector<DynaCmdTable>::iterator iter = g_vDynaCmdTab.begin();
				for(int i=0;i<m_nNowPad&&iter!=g_vDynaCmdTab.end();i++,iter++);
				strcpy(m_PadCode, g_vDynaCmdTab[i].EnName);
				strcpy(m_PadName, g_vDynaCmdTab[i].ChName);
				m_NowPadSet = g_vDynaCmdTab[i].DefPadSet;
				m_NowPadCmdID = g_vDynaCmdTab[i].DefPadCmdID;
			}
			else 
			{
				strcpy(m_PadName, nullPadName);
				strcpy(m_PadCode, nullPadName);
			}
		}

		if(!GetCustomPadInfo(m_PadCode))
		{
			OnNewpad();
			return;
		}
		ResizePad(COM_ReadDraw);	//第一次要读数据
		SendMessage(UM_REFRESHCUSTOMBAR);
		SetComTitle();
		m_nF = m_tmpNf = 0;
		m_oldhit = -1;
		Invalidate();
	}
	catch (...) 
	{
		m_nF = m_tmpNf = 0;
		m_oldhit = -1;
	}
	SetTimer(10000, 1000, NULL);
}

void CComView2::OnRefreshCustomBarMsg(WPARAM wParam,LPARAM lParam)
{
	InsertCustomPadSwitch(-1);
}

void CComView2::OnSwitchBarMsg(WPARAM wParam,LPARAM lParam)
{
	MySwitchBarMsg *pMsg=(MySwitchBarMsg *)lParam;
	//切换用的SwitchBar
	if(pMsg->id == ID_SWITCHBAR_H)
		OnSwitchCustomPad();
}

extern void AppendCustomPadMenu(CNewMenu* pPadMenu);
void CComView2::OnSwitchCustomPad()
{
	UINT	nID;
	int		nIndex;
	m_CustomPad_Bar.GetSel(nIndex,nID);
	if(nID==ID_CUSPAD_MENU)
	{
		CNewMenu menu;
		menu.LoadMenu(IDR_LONELY_MENU);
		CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(26);
		AppendCustomPadMenu(pPopup);
		POINT pp;
		pp.x = 0; pp.y = (g_bShowCustomPadSwitch?TOPSWITCH_HEIGHT:0);
		ClientToScreen(&pp);
		pPopup->TrackPopupMenu (TPM_LEFTALIGN,pp.x,pp.y,AfxGetMainWnd());
	}
	else if(nIndex>=0)
		PostMessage(UM_CONSTRUCTPAD_MESSAGE,nID-ID_CUSPAD_BASE);
}

//创建一个Unit
void CComView2::CreateUnit(int initGPNo,int type,int style,int nUnit,UBase **pUnit)
{
	TDEL(*pUnit);
	
	COMINFO ComInfo;
	ComInfo.pWnd = this;
	ComInfo.nNowPad = m_nNowPad;
	ComInfo.pUnits = &m_Units;
	ComInfo.nWhichUnit = nUnit;
	ComInfo.nUnitType = type;
	if(style==ZST_SHDP||style==ZST_SZDP) ComInfo.nUnitStyle = ZST_BIG;
	else ComInfo.nUnitStyle = style;
	ComInfo.nInCom2 = 1;
	ComInfo.nInDlg = 0;
	ComInfo.nInMultiPeriod = 0;
	switch(type)
	{
	case MTSORT_UNIT:
		*pUnit = new USortGrid(&ComInfo);
		break;
	case ZHSORT_UNIT:
		*pUnit = new UZhSort(&ComInfo);
		break;
	case HQGRID_UNIT:
		{	
			if(style==HQGRID_DYNA) 
				ComInfo.nMoreFlag = 0;
			else ComInfo.nMoreFlag = 1;
			*pUnit = new HQGrid(&ComInfo);		
			((HQGrid *)(*pUnit))->Create(this,CRect(0,0,0,0), style-100*type,nUnit);
		}
		break;
	case ZST_UNIT:
		*pUnit = new UZst(&ComInfo);
		break;
	case FXT_UNIT:
		*pUnit = new UFxt(&ComInfo);
		break;
	case HQ_UNIT:
		*pUnit = new UHq(&ComInfo);
		break;
	case TICK_UNIT:
		*pUnit = new UTick(&ComInfo);
		break;
	case FJB_UNIT:
		*pUnit = new UFjb(&ComInfo);
		break;
	case FSB_UNIT:
		*pUnit = new UFsb(&ComInfo);
		break;
	case CDP_UNIT:
		*pUnit = new UCdp(&ComInfo);
		break;
	case MMP_UNIT:
		*pUnit = new UMmp(&ComInfo);
		break;
	case CB_UNIT:
		*pUnit = new UCb(&ComInfo);
		break;
	case BIGVOL_UNIT:
		*pUnit = new UBigVol(&ComInfo);
		break;
	case RADAR_UNIT:
		*pUnit = new URadar(&ComInfo);
		break;
	case WEB_UNIT:
		*pUnit = new UWeb2(&ComInfo);
		break;
	case GLHQ_UNIT://关联报价
		*pUnit = new UGlHq(&ComInfo);
		((UGlHq *)(*pUnit))->Create(this,GLHQ_BLOCK_GP);
		break;
	case SEAT_UNIT:
		if(!g_bHasDS) break;
		*pUnit = new USeat(&ComInfo);
		break;
	case TICKZST_UNIT:
		*pUnit = new UTickZst(&ComInfo);
		break;
	case LOWHQ_UNIT://小行情图组件
		*pUnit = new UHq(&ComInfo,TRUE);
		break;
	case HQ10_UNIT:	//十档买卖盘组件
		*pUnit = new UHq10(&ComInfo);
		break;
	case TICK2_UNIT://逐笔成交组件
		*pUnit = new UTick2(&ComInfo);
		break;
	case QUEUE_UNIT://买卖队列组件
		*pUnit = new UQueue(&ComInfo);
		break;
	case TICKSTAT_UNIT://龙虎看盘组件
		*pUnit = new UTickStat(&ComInfo);
		break;
	case WTRANK_UNIT://委托排名组件
		*pUnit = new UWTRank(&ComInfo);
		break;
	case SCANCEL_UNIT://即时撤单排名
		*pUnit = new USCancel(&ComInfo);
		break;
	case TCANCEL_UNIT://累计撤单排名
		*pUnit = new UTCancel(&ComInfo);
		break;
	case HYCJ_UNIT:	  //行业成交排名
		*pUnit = new UHycj(&ComInfo);
		break;
	case TV_UNIT:	  //TopView组件
		*pUnit = new UTVGraph(&ComInfo);
		break;
	case WTFB_UNIT:
		*pUnit = new UWtfb(&ComInfo);
		break;
	case F10_UNIT:
	case RMREPORT_UNIT:
		*pUnit = new UBaseInner(&ComInfo,"");
		break;
	case STAR_UNIT:
		*pUnit = new UBaseInner(&ComInfo,"星空图");
		break;
	default:
		break;
	}
	//外部功能(支持Unit方式的)
	if(type>=EXTERN1_UNIT && type<EXTERN100_UNIT)
	{
		ComInfo.bPlugin = 1;
		char *pModuleCode = g_GNPlug.GetModuleCodeFromUnitID(type);
		if(pModuleCode)
		{
			IGNPLUGIN*	pModule = g_GNPlug.GetPluginFromModuleCode(pModuleCode);
			if(pModule)
				ComInfo.bPlugin_GPRel = pModule->iInfo.bbPlugin_GPRel;
			*pUnit = new UBaseInner(&ComInfo,pModuleCode);
		}
	}
	if(type==-1&&style==-1)
		*pUnit = new UBase(&ComInfo);

	if(style==ZST_SHDP||style==FXT_SHDP) ChangeUnitGP(*pUnit,m_nSHZSIndex);
	else if(style==ZST_SZDP||style==FXT_SZDP) ChangeUnitGP(*pUnit,m_nSZZSIndex);
	else ChangeUnitGP(*pUnit,initGPNo);
	if(type == FXT_UNIT)
	{
		if(m_nFxPeriod != -1)
			(*pUnit)->ProcessMsg(UM_SET_FXPERIOD,m_nFxPeriod);
		if(m_nOften301  != -1)
		{
			long iNo = g_pCalc->_CallDTopViewCalc_9(ZB_TYPE,g_strOftenUseZb[m_nOften301-301]);
			if(iNo >= 0)
				(*pUnit)->ProcessMsg(UM_SET_OFTENZB,iNo);
		}
	}
}

BOOL CComView2::ChangeSyncUnitGP(int setGPIndex)
{
	CString str;
	int nGPVar = m_Units.Var_GPIndex[m_nF] - GPINDEX_VAR1;
	int i; 
	MemStkInfo *tmpInfo = (*g_pStockIO)[setGPIndex];
	if(!tmpInfo) return FALSE;

	if(time(NULL) - tmp_SetGPtime< 1L) 
		return FALSE;
	SetMainID(); //当改变股票时,要改变MainID

	UINT tmpProperty=BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT|BE_NONULL_UNIT
		|BE_CHANGEGP_UNIT|BE_UNLOCKED_UNIT|BE_GPINAC_UNIT;
	if(g_nLinkage==2)
		tmpProperty=BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT|BE_NONULL_UNIT|BE_CHANGEGP_UNIT|BE_UNLOCKED_UNIT;

	m_aCurGPSet[nGPVar] = setGPIndex;
	if(TestUnitProperty(m_nF, tmpProperty))
	{
		ChangeUnitGP(m_Units.pUnit[m_nF], setGPIndex);
		m_Units.pUnit[m_nF]->ProcessMsg(WM_PAINT, 1);
		FOR_UNIT(i)
		{
			if(i==m_nF) continue;
			if(TestUnitProperty(i, tmpProperty))
			{
				ChangeUnitGP(m_Units.pUnit[i], setGPIndex);
				m_Units.pUnit[i]->ProcessMsg(WM_PAINT, 1);
			}
		}
		return TRUE;
	}
	BOOL bHasSyncUnit = FALSE;
	FOR_UNIT(i)
	{
		if(i==m_nF) continue;
		if(TestUnitProperty(i, tmpProperty))
		{
			bHasSyncUnit = TRUE;
			ChangeUnitGP(m_Units.pUnit[i], setGPIndex);
			m_Units.pUnit[i]->ProcessMsg(WM_PAINT, 1);
		}
	}
	//无被控单元切换到ComView
	if(!bHasSyncUnit)	ShowGpInCom(tmpInfo->Code, tmpInfo->setcode);
	return TRUE;
}

BOOL CComView2::ChangeUnitGP(UBase *pUnit,int iNo)
{
	if(!pUnit) return FALSE;
	int iUnit = -1;
	for(int i=0;i<m_Units.Num;i++)
		if(m_Units.pUnit[i]==pUnit)
			iUnit = i;
	if(TestUnitProperty(iUnit, BE_LOCKED_UNIT)) return FALSE;
	pUnit->ProcessMsg(UM_SET_STOCK,iNo);
	return TRUE;
}

void CComView2::SetComTitle()
{
	m_CustomPad_Bar.SetMyRedraw(FALSE);
	m_nCurSel = 0;
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(!g_vDynaCmdTab[i].ShowShortCut) continue;
		if(g_vDynaCmdTab[i].DefPadSet!=m_NowPadSet) continue;
		if(strcmp(m_PadCode,g_vDynaCmdTab[i].EnName)==0)
			break;
		++m_nCurSel;
	}
//	m_nCurSel++;
	m_CustomPad_Bar.SetSel(m_nCurSel);
	m_CustomPad_Bar.SetMyRedraw(TRUE);

	CString tmpTitle=m_PadName;
	if(m_bInSplit)
		tmpTitle="正在设置...";
	//设置窗口标题
	theViewManager.SetViewName(tmpTitle, this);
//	GetParent()->SetWindowText((CString)_F("特色分析-")+tmpTitle);
	AfxGetMainWnd()->SetWindowText(g_strTitleName);
}

void CComView2::PaintNullUnit(int nUnit)
{
	CDC *pDC = GetDC();
	if(nUnit<0)
		pDC->FillSolidRect(&m_Rect, VipColor.BACKCOLOR);
	else if(TestUnitProperty(nUnit, BE_NULL_UNIT|BE_VALID_UNIT|BE_NOHIDE_UNIT))
	{
		pDC->FillSolidRect(m_Units.Rect[nUnit].left, m_Units.Rect[nUnit].top, m_Units.Rect[nUnit].Width()+3, m_Units.Rect[nUnit].Height()+3, VipColor.BACKCOLOR);
		CFont *oldfont = g_d.SelectFont(pDC,GRAPH_FONT);
		if(m_bInSplit)
			g_d.DisplayText(pDC,m_Units.Rect[nUnit],VipColor.TXTCOLOR, DT_VCENTER|DT_CENTER|DT_SINGLELINE, _F("空白单元 请点右键进行设置"));
		else
			g_d.DisplayText(pDC,m_Units.Rect[nUnit],VipColor.TXTCOLOR, DT_VCENTER|DT_CENTER|DT_SINGLELINE, _F("空白单元"));
		g_d.RestoreFont(pDC,oldfont);
	}
	ReleaseDC(pDC);
}

BOOL CComView2::HasNoFixedHQGrid()
{
	int i;
	FOR_UNIT(i)
	{
		if(m_Units.Type[i]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
			if(!(pHQGrid->IsFixedGrid())) return TRUE;
		}
	}
	return FALSE;
}

void CComView2::ReadAndPaint(int allread,int Var_GPIndex)
{
	int i;
	FOR_UNIT(i)
	{
		if(!m_Units.pUnit[i]) continue;
		if(!allread && m_Units.Var_GPIndex[i]!=Var_GPIndex) continue;
		if(!TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT)) continue;
		if(TestUnitProperty(i, BE_NULL_UNIT)) PaintNullUnit(i);
		else if(m_Units.Type[i]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
			pHQGrid->ProcessHQRefresh();
		}
		else m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
	}
}

void CComView2::OnReadAndPaint(WPARAM wParam,LPARAM lParam)
{
	if(g_pComm)
	{
		g_pComm->m_bBuffEven = TRUE;
		g_pComm->m_bDirectEven = TRUE;
	}
	if(g_pComm3)
	{
		g_pComm3->m_bBuffEven = TRUE;
		g_pComm3->m_bDirectEven = TRUE;
	}
	SetMainID();
	tmp_SetGPtime=time(NULL);
	ReadAndPaint();	
}

void CComView2::SetUnitReadAndPaint(int i,CRect rc,int flag)
{
	if(TestUnitProperty(i, BE_INVALID_UNIT)||TestUnitProperty(i, BE_HIDE_UNIT)) return;
	if(rc!=CRect(0,0,0,0))
		m_Units.nStatus[i] = U_SHOW;
	m_Units.Rect[i] = rc;
	PaintNullUnit(i);
	long wParam,lParam;
 	wParam = MAKELONG(rc.left+1,rc.top+1);
 	lParam = MAKELONG(rc.Width()-1,rc.Height()-1);
	if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT))
		m_Units.pUnit[i]->ProcessMsg(WM_SIZE,wParam,lParam);
	if(TestUnitProperty(i, BE_NULL_UNIT))	return;
	int nGPVar = m_Units.Var_GPIndex[i];
	if(flag==COM_OnlyDraw || flag==COM_ReadDraw)
	{
		if(m_Units.Type[i]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
			pHQGrid->m_bJSZJStatus = m_bJSZJFlag[i];
			pHQGrid->m_bSigStatus  = m_bSignalFlag[i];
			pHQGrid->m_nConStatus = m_nConFlag[i];
			pHQGrid->m_nConFiltCount = m_nConFiltCount[i];
			pHQGrid->ResetRowCol();
			if(m_nSetFlag[i]>0) 
				pHQGrid->SetDomain(m_nBlockFlag[i], m_WhichGPType[i], FALSE, 0, TRUE);
			else pHQGrid->SetDomain(pHQGrid->GetBlockFlag(), pHQGrid->GetGPType(), FALSE, 0, TRUE);
			m_nSetFlag[i] = 0;
			pHQGrid->ProcessHQRefresh();
		}
		else if(TestUnitProperty(i, BE_NULL_UNIT))
			PaintNullUnit(i);
		else m_Units.pUnit[i]->ProcessMsg(WM_PAINT, flag);
	}
	else if(flag==COM_OnlyRead||m_Units.Type[i]==BIGVOL_UNIT||m_Units.Type[i]==RADAR_UNIT)
	{
		if(m_Units.Type[i]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
			pHQGrid->m_bJSZJStatus = m_bJSZJFlag[i];
			pHQGrid->m_bSigStatus  = m_bSignalFlag[i];
			pHQGrid->m_nConStatus = m_nConFlag[i];
			pHQGrid->m_nConFiltCount = m_nConFiltCount[i];
			pHQGrid->ResetRowCol();
			if(m_nSetFlag[i]>0) 
				pHQGrid->SetDomain(m_nBlockFlag[i], m_WhichGPType[i], FALSE, 0, TRUE);
			else pHQGrid->SetDomain(pHQGrid->GetBlockFlag(), pHQGrid->GetGPType(), FALSE, 0, TRUE);
			m_nSetFlag[i] = 0;
			pHQGrid->ProcessHQRefresh();
		}
		else m_Units.pUnit[i]->ProcessMsg(UM_READDATA);
	}
	else m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
	if(m_Units.Type[i]==HQGRID_UNIT)
	{
		HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
		pHQGrid->SetControlPos(this, rc);
	}
}

void CComView2::ResizePad(int flag)
{
	if(m_Units.Num<1) return;
	if(m_Units.Num>1) ReSplitPad(m_pRootUnit, flag);
	else SetUnitReadAndPaint(0,m_Rect,flag);
	Invalidate();
}

void CComView2::DrawBorder()
{
	CDC *pDC=GetDC();
	if(!pDC) return;
	int i;
	pDC->SetBkMode(OPAQUE);
	FOR_UNIT(i)
	{
		if(TestUnitProperty(i, BE_INVALID_UNIT)||TestUnitProperty(i, BE_HIDE_UNIT)) 
			continue;
		g_d.DrawRect(pDC,m_Units.Rect[i],VipColor.AXISCOLOR);
	}
	if(m_Units.Num==0 || m_nF<0)
	{
		CRect rc;
		GetClientRect(&rc);
		pDC->FillSolidRect(&rc,VipColor.BACKCOLOR);
	}
	ReleaseDC(pDC);
}

void CComView2::OnDraw(CDC* pDC)
{
	if(g_pToolTipMark) 
		g_pToolTipMark->ClearToolTipInfo(this);
	int i;
	FOR_UNIT(i)
	{
		if(!m_Units.pUnit[i]) continue;
		if(!TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT)) continue;
		PaintNullUnit(i);
		m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
		if(m_Units.Type[i]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
			pHQGrid->SetControlPos(this, m_Units.Rect[i]);
		}
	}
	DrawBorder(); //自己得到DC,不用pDC,否则刷新会不正常?
}

void CComView2::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	theViewManager.OnActivateView(bActivate, this);
	if(!g_bSepEsc)	//如果是特殊ESC逻辑,则不用行情
		theViewManager.pOldActiveView2 = theViewManager.pActiveView;
	g_bSepEsc = FALSE;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(!bActivate)
	{
		::ClipCursor(NULL);
		::ReleaseCapture();
		PostMessage(UM_DELETE_INFODLG,1);
	}
	if(!m_bQuitting && bActivate)
	{
		AfxGetMainWnd()->SendMessage(UM_HIDESHOWBUTTON,HideShowBtn_PadSet);
	}
	PostMessage(UM_BLOCKMODIFIED,0,0);
}

void CComView2::SetControlSize()
{
	CRect wndrc;
	GetClientRect(&wndrc);
	int cx=wndrc.Width(),cy=wndrc.Height();

	m_Rect.SetRect(0,(g_bShowCustomPadSwitch?TOPSWITCH_HEIGHT:0),cx,cy);
	if(cx>0&&cy>0) 
		ResizePad(3);
	else 
	{
		int i;
		FOR_UNIT(i)
		{
			m_Units.Rect[i].SetRect(0,0,cx,cy);
			long wParam,lParam;
			wParam = MAKELONG(m_Units.Rect[i].left,m_Units.Rect[i].top+1);
			lParam = MAKELONG(m_Units.Rect[i].Width(),m_Units.Rect[i].Height());
			if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT))
			{
				m_Units.pUnit[i]->ProcessMsg(WM_SIZE,wParam,lParam);
				m_Units.pUnit[i]->ProcessMsg(WM_PAINT,0);
			}
		}
	}
	if(::IsWindow(m_CustomPad_Bar.m_hWnd) )
		m_CustomPad_Bar.MoveWindow(0,0,wndrc.Width(),(g_bShowCustomPadSwitch?TOPSWITCH_HEIGHT:0));
}

void CComView2::OnSize(UINT nType, int cx, int cy) 
{
 	CView::OnSize(nType, cx, cy);
	SetControlSize();
}

BOOL CComView2::Get_Extern_GPSwitch() 
{ 
	for(int i=0;i < m_Units.Num;i++)
	{
		if(!TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) continue;
	//	if(m_Units.Type[i]==FXT_UNIT || m_Units.Type[i]==ZST_UNIT || m_Units.Type[i]==F10_UNIT || m_Units.Type[i]==HQ_UNIT || m_Units.Type[i]==TICK_UNIT || m_Units.Type[i]==TICKZST_UNIT) 
	//		break;
		UINT tmpProperty=BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT|BE_NONULL_UNIT|BE_CHANGEGP_UNIT|BE_UNLOCKED_UNIT|BE_GPINAC_UNIT;
		if(TestUnitProperty(i, tmpProperty))
			break;
	}
	if(i<m_Units.Num) return TRUE;
	return FALSE;
}


int CComView2::EscHandle()
{
	if(FocusExistAndShow)
		return m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,VK_ESCAPE);
	else 
		return 0;
}

void CComView2::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_NEXT:
	case VK_PRIOR:
		if(!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) break;
		if(m_Units.Type[m_nF] == TICK_UNIT || m_Units.Type[m_nF] == FJB_UNIT ||m_Units.Type[m_nF] == FSB_UNIT || m_Units.Type[m_nF] == MMP_UNIT )
		{
			m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
			break;
		}
		if(m_Units.Type[m_nF]==HQGRID_UNIT) 
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
			pHQGrid->OnKeyUp(nChar, nRepCnt, nFlags);
		}
		else if(m_Units.Type[m_nF]==ZST_UNIT||m_Units.Type[m_nF]==FXT_UNIT||m_Units.Type[m_nF]==HQ_UNIT)
		{
			if(m_Units.Var_GPIndex[m_nF]>=GPINDEX_VAR1)
			{
				//联动翻页
				int nGPIndex = m_Units.pUnit[m_nF]->ProcessMsg(UM_GET_STOCK);
				int GPNewIndex = GetNextGP(nGPIndex, nChar==VK_NEXT?TRUE:FALSE);
				ChangeSyncUnitGP(GPNewIndex);
			}
		}
		break;
	default:
		DispatchMsg(WM_KEYUP,nChar,nRepCnt);
		break;
	}
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CComView2::DispatchMsg(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		m_Units.pUnit[m_nF]->ProcessMsg(message,wParam,lParam);
}

void CComView2::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch(nChar)
	{
	case VK_LEFT:
	case VK_RIGHT:
		{
			if(m_nF<0 || !m_Units.pUnit[m_nF]) break;
			if(!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) break;
			if(m_Units.Type[m_nF]==HQGRID_UNIT) 
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
				pHQGrid->OnKeyDown(nChar, nRepCnt, nFlags);
			}
			else m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
		}
		break;
	case VK_UP:
	case VK_DOWN:
		{
			if(m_nF<0 || !m_Units.pUnit[m_nF]) break;
			if(!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) break;
			if(m_Units.Type[m_nF]==HQGRID_UNIT) 
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
				pHQGrid->OnKeyDown(nChar, nRepCnt, nFlags);
			}
			else m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
		}
		break;
	case VK_PRIOR:
	case VK_NEXT:
		{
			if(m_nF<0 || !m_Units.pUnit[m_nF]) break;
			if(!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) break;
			if(m_Units.Type[m_nF] == TICK_UNIT || m_Units.Type[m_nF] == FJB_UNIT ||m_Units.Type[m_nF] == FSB_UNIT || m_Units.Type[m_nF] == MMP_UNIT )
				break;
			if(m_Units.Type[m_nF]==HQGRID_UNIT) 
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
				pHQGrid->OnKeyDown(nChar, nRepCnt, nFlags);
			}
			else if(m_Units.Type[m_nF]==ZST_UNIT||m_Units.Type[m_nF]==FXT_UNIT||m_Units.Type[m_nF]==HQ_UNIT)
			{
			}
			else 
				m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
			SetComTitle();
		}
		break;
	case VK_RETURN:
		{
			if(m_nF<0) break;
			SHORT ShiftDown=::GetKeyState(VK_SHIFT);
			if(ShiftDown<0)
			{
				PostMessage(WM_COMMAND,ID_RIGHT_NEWWINDOW);
				break;
			}				
			if(m_Units.pUnit[m_nF] && TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|
				BE_NOCHANGEGP_UNIT|BE_CONTRALL_UNIT))
			{
				int iIndex = m_Units.pUnit[m_nF]->ProcessMsg(UM_GET_STOCK);
				
				MemStkInfo *tmpInfo = (*g_pStockIO)[iIndex];
				if(tmpInfo) ChangeSyncUnitGP(iIndex);
				int nGPVar = m_Units.Var_GPIndex[m_nF] - GPINDEX_VAR1;
				m_aLastCUnit[nGPVar] = m_nF;

			}
			else DispatchMsg(WM_KEYDOWN,nChar,nRepCnt);
		}
		break;
	case VK_DELETE:										//HQGRID单独处理								
		if(m_nF<0 || !m_Units.pUnit[m_nF]) break;	
		if(TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT|BE_NONULL_UNIT))
		{
			if(m_Units.Type[m_nF]==HQGRID_UNIT)
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
				pHQGrid->OnKeyDown(VK_DELETE, nChar, nRepCnt);
			}
			else
				m_Units.pUnit[m_nF]->ProcessMsg(WM_KEYDOWN,nChar,nRepCnt);
		}
		break;
	default:
		DispatchMsg(WM_KEYDOWN,nChar,nRepCnt);
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CComView2::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DispatchMsg(WM_CHAR,nChar,nRepCnt);

	CView::OnChar(nChar, nRepCnt, nFlags);
}

//////////////////////////////////////////////////////////////////////////
//HqGrid 的消息处理
void CComView2::OnBatch()
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pHQGrid->ProcessMsg(WM_COMMAND,ID_BATCH);
}

void CComView2::OnGotojbzl()
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
	{
		for(int i=0;i < m_Units.Num;i++)
		{
			if(i!=m_nF && m_Units.Type[i]==HQGRID_UNIT && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
				break;
		}
		if(i < m_Units.Num)
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
			if(pHQGrid)
				pHQGrid->ProcessMsg(WM_COMMAND,ID_GOTOJBZL);
		}
		else
		{
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
				tempWnd->SendMessage(WM_COMMAND,ID_GOTOJBZL);
		}
		return;
	}
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pHQGrid->ProcessMsg(WM_COMMAND,ID_GOTOJBZL);
}

void CComView2::OnChangeGPType()
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pHQGrid->ProcessMsg(WM_COMMAND,ID_CHANGEGPTYPE);
}

void CComView2::OnAutoPage()
{
	CUnitView::OnAutopage();
}

void CComView2::OnRightClickRow(UINT nID)
{
	if(nID==ID_RIGHT_ADDBLOCK)
	{
		OnRightAddBlock();
		return;
	}
	else if(nID==ID_RIGHT_ADDZXG)
	{
		OnRightAddZxg();
		return;
	}
	else if(nID==ID_DELFROMBLOCK)
	{
		OnDelFromBlock();
		return;
	}
	else
	{
		if(m_nF<0 || !m_Units.pUnit[m_nF]) return;
		if(m_Units.Type[m_nF]!=HQGRID_UNIT||
			!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
			return;
		if(nID==ID_RIGHT_OPEN)
		{
			int iIndex = m_Units.pUnit[m_nF]->ProcessMsg(UM_GET_STOCK);			
			MemStkInfo *tmpInfo = NULL;
			tmpInfo = (*g_pStockIO)[iIndex];
			if(tmpInfo) ChangeSyncUnitGP(iIndex);
			return;
		}
		if(nID==ID_RIGHT_NEWWINDOW)
		{
			int iIndex = m_Units.pUnit[m_nF]->ProcessMsg(UM_GET_STOCK);		

			MemStkInfo *tmpInfo = NULL;
			tmpInfo = (*g_pStockIO)[iIndex];
			if(tmpInfo) ShowGpInCom(tmpInfo->Code, tmpInfo->setcode);

			return;
		}
		HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
		if(pHQGrid)
			pHQGrid->ProcessMsg(WM_COMMAND,nID);
	}
}

void CComView2::OnUpdateRightClickHead(CCmdUI* pCmdUI)
{
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pHQGrid->OnUpdateRightClickHead(pCmdUI);
}

void CComView2::OnTypeID(UINT nID)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
	{
		for(int i=0;i < m_Units.Num;i++)
		{
			if(i!=m_nF && m_Units.Type[i]==HQGRID_UNIT && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
				break;
		}
		if(i < m_Units.Num)
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
			if(pHQGrid)
				pHQGrid->ProcessMsg(WM_COMMAND,nID);
		}
		else
		{
			g_bOldGridDomain=FALSE;
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
				tempWnd->SendMessage(WM_COMMAND,nID);
			g_bOldGridDomain=TRUE;
		}
		return;
	}
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pHQGrid->ProcessMsg(WM_COMMAND,nID);
}

void CComView2::OnBlockID(UINT nID)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
	{
		for(int i=0;i < m_Units.Num;i++)
		{
			if(i!=m_nF && m_Units.Type[i]==HQGRID_UNIT && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
				break;
		}
		if(i < m_Units.Num)
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
			if(pHQGrid)
				pHQGrid->ProcessMsg(WM_COMMAND,nID);
		}
		return;
	}
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pHQGrid->ProcessMsg(WM_COMMAND,nID);
}

extern BOOL ConnectDSAtOnce(BOOL &bOK,HWND hWnd);

void CComView2::OnDSMarket(UINT nID)
{
	BOOL bOK=FALSE;
	if(!ConnectDSAtOnce(bOK,m_hWnd))
		return;
	int nSel = -1;
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
	{
		for(int i=0;i < m_Units.Num;i++)
		{
			if(i!=m_nF && m_Units.Type[i]==HQGRID_UNIT && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_NODS_SC))
				break;
		}
		if(i < m_Units.Num)
			nSel = i;
	}
	else 
		nSel = m_nF;
	if(nSel>=0)
	{
		HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[nSel];
		BOOL bFixed = pHQGrid->IsFixedGrid();
		if(pHQGrid && (!bFixed || pHQGrid->GetGPType()>=DS_START))
		{
			if(bOK)
				pHQGrid->ProcessMsg(UM_GRIDREINIT);
			pHQGrid->ProcessMsg(WM_COMMAND,nID);
			return;
		}
	}
	g_bOldGridDomain=FALSE;
	CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
	if(tempWnd)
		tempWnd->SendMessage(WM_COMMAND,nID);
	g_bOldGridDomain=TRUE;
}

//////////////////////////////////////////////////////////////////////////
void CComView2::OnUpdateDYBlock(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) 
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_DY1 == pHQGrid->GetBlockFlag()-DY_START);
}

void CComView2::OnUpdateMDFHkHyClick(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
	{
		int nBlockFlag = pHQGrid->GetBlockFlag();
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_MDFHK_HY_1==nBlockFlag-HKHY_START);
	}
}

void CComView2::OnUpdateDSMarket(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(DSGetMarketXH(pCmdUI->m_nID-IDM_DS1)==pHQGrid->GetGPType()-DS_START);
}

void CComView2::OnUpdateHYBlock(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) 
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_HY1 == pHQGrid->GetBlockFlag()-200);
}

void CComView2::OnUpdateConceptClick(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) 
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_CONCEPT_MENU_1 == pHQGrid->GetBlockFlag()-CON_START);
}

void CComView2::OnUpdateZHBClick(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT)) 
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_ZHB_MENU_1 == pHQGrid->GetBlockFlag()-ZHB_START);
}

void CComView2::OnUpdateClickCorner(CCmdUI* pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_RIGHT_TYPE0 == pHQGrid->GetGPType());
}

void CComView2::OnUpdateMenuBlockClick(CCmdUI *pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_BLOCK_MENU_1 == pHQGrid->GetBlockFlag());
}

void CComView2::OnUpdateZJHHYClick(CCmdUI *pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		return;
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	if(pHQGrid)
		pCmdUI->SetCheck(pCmdUI->m_nID-ID_ZJHHY_MENU_1 == pHQGrid->GetBlockFlag()-ZJHHY_START);
}

void CComView2::OnUpdateRightClickRow(CCmdUI *pCmdUI)
{
	if(pCmdUI->m_nID!=ID_RIGHT_ADDBLOCK&&pCmdUI->m_nID!=ID_RIGHT_ADDZXG&&pCmdUI->m_nID!=ID_DELFROMBLOCK)
	{
		if(m_Units.Type[m_nF]!=HQGRID_UNIT||
			!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		{
			pCmdUI->Enable(FALSE);
			return;
		}
		HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
		if(!pHQGrid) return;
		switch(pCmdUI->m_nID)
		{
		case ID_RIGHT_OPEN:
			pCmdUI->Enable(pHQGrid->GetTotalRow() > 0);
			break;
		case ID_RIGHT_NEWWINDOW:
			pCmdUI->Enable(pHQGrid->GetTotalRow() > 0);
			pCmdUI->Enable(theViewManager.ComWindowNum < 3);
			break;
		case ID_RIGHT_DELETEFROMBLOCK:
			{
				int tmpBlockFlag = pHQGrid->GetBlockFlag();
				int tmpGPType = pHQGrid->GetGPType();				
				if(!BeUserBlock(tmpGPType,tmpBlockFlag))
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(pHQGrid->GetTotalRow() > 0);
			}
			break;
		}
	}
}

void CComView2::OnSplitPad(UINT nID)
{
	int nInsPlace = nID - ID_INSERTLEFT;
	SplitPad(m_nF, -1, -1, nInsPlace==0||nInsPlace==1?1:-1, 50, 0, FALSE);
	if(nInsPlace==0||nInsPlace==2) SwapUnit(m_nF, m_Units.Num-1, TRUE);
	m_nF = m_Units.Num-1;
	ResizePad(1);
}

void CComView2::OnUpdateSplitPad(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CComView2::OnChangeZhSortSC(UINT nID)
{
	if(m_Units.Type[m_nF]==ZHSORT_UNIT)
	{
		UZhSort * pUzhSort = (UZhSort*)m_Units.pUnit[m_nF];
		pUzhSort->WhichType = nID - ID_SCSTART;
		pUzhSort->ProcessMsg(WM_CHANGESCTYPE);
	}
	if(m_Units.Type[m_nF]==MTSORT_UNIT)
	{
		USortGrid *pGridSort = (USortGrid*)m_Units.pUnit[m_nF];
		pGridSort->WhichType = nID - ID_SCSTART;
		pGridSort->ProcessMsg(WM_CHANGESCTYPE);
	}
}

void CComView2::OnUpdateChangeZhSortSC(CCmdUI *pCmdUI)
{
	if(m_Units.Type[m_nF]!=ZHSORT_UNIT) return;
	
	UZhSort * pUzhSort = (UZhSort*)m_Units.pUnit[m_nF];
	if(pUzhSort->m_nDSFlag==0) 
		pCmdUI->SetRadio(pCmdUI->m_nID==pUzhSort->WhichType+ID_SCSTART);
	else if(pUzhSort->m_nDSFlag==1)
		pCmdUI->SetRadio(pCmdUI->m_nID==pUzhSort->WhichType+ID_SCSTART);
}

void CComView2::OnInsertUnit(UINT nID)
{
	if(g_mUnitMenu.empty()) InitMenuMap();

	int nType=-1, nStyle=-1;
	map<UINT, int, greater<UINT> >::iterator iter = g_mUnitMenu.find(nID);
	if(iter==g_mUnitMenu.end()) return;
	int nTypeStyle = (*iter).second;
	nType = nTypeStyle/1000; nStyle = nTypeStyle%1000;
	
	int nTargetNode = Index2NodeNo(m_nF);
	if(nTargetNode<0) return;
	m_apNode[nTargetNode]->pCurUnit->unitType = nType;
	m_apNode[nTargetNode]->pCurUnit->style = nStyle;

	int nCurGPNo = 140;
	for(int k=0;k < MAX_COM2_GP;k++)
	{
		if(m_Units.Var_GPIndex[m_nF]==GPINDEX_VAR1+k) 
			nCurGPNo = m_aCurGPSet[k];
	}
	m_Units.Type[m_nF] = nType; m_Units.Style[m_nF] = nStyle;
	m_Units.Var_GPIndex[m_nF]=GPINDEX_VAR1;

	CreateUnit(nCurGPNo, nType, nStyle,m_nF,&m_Units.pUnit[m_nF]);
	m_apNode[nTargetNode]->pCurUnit->pUnit = m_Units.pUnit[m_nF];
	m_apNode[nTargetNode]->pCurUnit->var_GpIndex = GPINDEX_VAR1;
	m_apNode[nTargetNode]->pCurUnit->bLocked = FALSE;
	if(nType==HQGRID_UNIT || nType==BIGVOL_UNIT || nType==ZHSORT_UNIT || nType==RADAR_UNIT || nType==MTSORT_UNIT) //如果是行情Grid等,初始设为组控单位
		m_apNode[nTargetNode]->pCurUnit->bContrallUnit=1;
	if((nType==ZST_UNIT&&nStyle==ZST_SHDP)||(nType==FXT_UNIT&&nStyle==FXT_SHDP))
	{
		m_apNode[nTargetNode]->pCurUnit->var_GpIndex = m_Units.Var_GPIndex[m_nF] = GPINDEX_SHZS;
		m_apNode[nTargetNode]->pCurUnit->bLocked = TRUE;
	}
	else if((nType==ZST_UNIT&&nStyle==ZST_SZDP)||(nType==FXT_UNIT&&nStyle==FXT_SZDP))
	{
		m_apNode[nTargetNode]->pCurUnit->var_GpIndex = m_Units.Var_GPIndex[m_nF] = GPINDEX_SZZS;
		m_apNode[nTargetNode]->pCurUnit->bLocked = TRUE;
	}
	SetUnitReadAndPaint(m_nF,m_Units.Rect[m_nF],COM_ReadDraw);
}

void CComView2::OnUpdateInsertUnit(CCmdUI *pCmdUI)
{
	if(pCmdUI->m_nID==ID_INSERTBSSEAT)
		pCmdUI->Enable(g_bHasDS);
	else if(pCmdUI->m_nID>=ID_INSERT_HQ10 && pCmdUI->m_nID<=ID_INSERT_HYCJ)
		pCmdUI->Enable(CanLevel2());
	else if(pCmdUI->m_nID==ID_INSERT_TV)
		pCmdUI->Enable(g_bHasTopView);	
	else if(pCmdUI->m_nID==ID_INSERT_WTFB)
		pCmdUI->Enable(CanLevel2());
	else if(pCmdUI->m_nID==ID_INSERTSTAR)
		pCmdUI->Enable(!IsBig5Version());
	else pCmdUI->Enable(TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|!BE_LOCKED_UNIT));
}

int	CComView2::GetCurrentGPIndex()
{
	if(m_Units.Type[m_nF]==HQGRID_UNIT)
	{
		HQGrid* pUnit = (HQGrid*)m_Units.pUnit[m_nF];
		if(pUnit)
			return pUnit->GetCurrentGPIndex();
	}
	else if(m_Units.Type[m_nF]==ZHSORT_UNIT)
	{
		UZhSort *pUnit = (UZhSort *)m_Units.pUnit[m_nF];
		if(pUnit)
			return pUnit->GetCurrentGPIndex();
	}
	else if(m_Units.Type[m_nF]==MTSORT_UNIT)
	{
		USortGrid *pUnit = (USortGrid *)m_Units.pUnit[m_nF];
		if(pUnit)
			return pUnit->GetCurrentGPIndex();
	}
	else if(m_Units.Type[m_nF]==BIGVOL_UNIT)
	{
		UBigVol *pUnit = (UBigVol *)m_Units.pUnit[m_nF];
		if(pUnit)
			return pUnit->GetCurrentGPIndex();
	}
	else if(m_Units.Type[m_nF]==RADAR_UNIT)
	{
		URadar *pUnit = (URadar *)m_Units.pUnit[m_nF];
		if(pUnit)
			return pUnit->GetCurrentGPIndex();
	}
	else
	{
		if(m_Units.Var_GPIndex[m_nF]==GPINDEX_SHZS) return m_nSHZSIndex;
		if(m_Units.Var_GPIndex[m_nF]==GPINDEX_SZZS) return m_nSZZSIndex;
		for(int k=0;k < MAX_COM2_GP;k++)
		{
			if(m_Units.Var_GPIndex[m_nF]==GPINDEX_VAR1+k)
				return m_aCurGPSet[k];
		}
	}
	return m_aCurGPSet[0];
}

BOOL CComView2::HasVarGPIndex(int nVar_GP)
{
	BOOL bFind = FALSE;
	int i;
	FOR_UNIT(i)
	{
		if(m_Units.Var_GPIndex[i]==nVar_GP)
		{
			bFind = TRUE;break;
		}
	}
	return bFind;
}

BOOL CComView2::InGroupGP(int nGpIndex)
{
	if(nGpIndex==m_nSHZSIndex||nGpIndex==m_nSZZSIndex)
		return TRUE;
	for(int i=0;i<MAX_COM2_GP;i++)
	{
		if(nGpIndex==m_aCurGPSet[i])
			return TRUE;
	}
	return FALSE;
}

void CComView2::RefreshCurrPADHQ()
{
	if(!OnLine)		return;
	if(g_bDowning)	return;
	if(!g_pStockIO) return;

	//减少叠加品种和关联品种的刷新次数,主股票刷新10次他才刷新一次
	static int s_ComView2_OtherCodeHead = 0;

	char *CodeHead = new char[(1+SH_CODE_LEN)*MULHQ_WANTNUM];
	char *CodeHead3 = new char[sizeof(MULIDX)*MULHQ_WANTNUM];
	memset(CodeHead,0,(1+SH_CODE_LEN)*MULHQ_WANTNUM);
	memset(CodeHead3,0,sizeof(MULIDX)*MULHQ_WANTNUM);
	struct MemStkInfo *tmpInfo, *tmpInfo3;
	int nCount = 0, nCount3 = 0;
	//每个Unit的股票
	for(int i=0;i < MAX_COM2_GP;i++)
	{
		if(HasVarGPIndex(GPINDEX_VAR1+i))
		{
			if(m_aCurGPSet[i]<DS_FROM)
			{
				tmpInfo = (*g_pStockIO)[m_aCurGPSet[i]];
				if(tmpInfo && !IsLocalStock(tmpInfo))
				{
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&tmpInfo->setcode,1);
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tmpInfo->Code,SH_CODE_LEN);
					nCount++;
				}
				if(s_ComView2_OtherCodeHead%10==0)
					GetQZRelationCodeHead(m_aCurGPSet[i],CodeHead,nCount);
			}
			if(m_aCurGPSet[i]>=DS_FROM)
			{
				tmpInfo3 = (*g_pStockIO)[m_aCurGPSet[i]];
				if(tmpInfo3)
				{
					memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo3->setcode,1);
					memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo3->Code,CODE_SIZE);
					nCount3++;
				}
			}
		}
	}
	if(nCount<MULHQ_WANTNUM && HasVarGPIndex(GPINDEX_SHZS))
	{
		tmpInfo = (*g_pStockIO)[m_nSHZSIndex];
		if(tmpInfo)
		{
			memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&tmpInfo->setcode,1);
			memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tmpInfo->Code,SH_CODE_LEN);
			nCount++;
		}
	}
	if(nCount<MULHQ_WANTNUM && HasVarGPIndex(GPINDEX_SZZS))
	{
		tmpInfo = (*g_pStockIO)[m_nSZZSIndex];
		if(tmpInfo)
		{
			memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&tmpInfo->setcode,1);
			memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tmpInfo->Code,SH_CODE_LEN);
			nCount++;
		}
	}
	FOR_UNIT(i)
	{
		if(nCount>=MULHQ_WANTNUM) break;
		if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_CHANGEGP_UNIT))
		{
			short nGpIndex = m_Units.pUnit[i]->ProcessMsg(UM_GET_STOCK);
			if(InGroupGP(nGpIndex))
				continue;			
			if(nGpIndex<DS_FROM)
			{
				tmpInfo = (*g_pStockIO)[nGpIndex];
				if(tmpInfo && !IsLocalStock(tmpInfo))
				{
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN),&tmpInfo->setcode,1);
					memcpy(CodeHead+nCount*(1+SH_CODE_LEN)+1,tmpInfo->Code,SH_CODE_LEN);
					nCount++;
				}
			}
			if(nGpIndex>=DS_FROM)
			{
				tmpInfo3 = (*g_pStockIO)[nGpIndex];
				if(tmpInfo3)
				{
					memcpy(CodeHead3+nCount3*sizeof(MULIDX),&tmpInfo3->setcode,1);
					memcpy(CodeHead3+nCount3*sizeof(MULIDX)+1,tmpInfo3->Code,CODE_SIZE);
					nCount3++;
				}
			}
		}
	}
	//叠加的股票
	FOR_UNIT(i)
	{
		if(s_ComView2_OtherCodeHead%10==0)
		{
			GetOverlapCodeHead(i,CodeHead,nCount);
			GetOverlapCodeHead(i,CodeHead3,nCount3,TRUE);
		}
	}
	if(nCount > 0 || nCount3>0)
	{
		SendMessage(UM_GETMAINID); //直接让父窗口填写g_nMainID
		g_nAssisID = JUSTCURRHQ;
		if(CanLevel2())				//Level2方式下数据包不能丢失
			g_nPriority = 1;
		else
			g_nPriority = 2;
		if(nCount>0 && OnLine) g_pGetData->GetMultiCurrStockData(CodeHead,nCount);
		if(nCount3>0 && DSOnLine) g_pGetData->DSGetMulCurrData((PMULIDX)CodeHead3,nCount3);
		g_nPriority = 1;
	}
	delete []CodeHead; CodeHead = NULL;
	delete []CodeHead3; CodeHead3 = NULL;

	s_ComView2_OtherCodeHead++;
}

extern void AddGPToStatHQ(int iGPNo,int &nCnt,short *lpnStkIndex,int nMaxNum,int nType);
int CComView2::GetGP_NeedRefreshStatHQ(short *lpnStkIndex,int nMaxNum,int nType)
{
	int i,nCnt = 0;
	//每个Unit的股票
	for(i=0;i < MAX_COM2_GP;i++)
	{
		if(HasVarGPIndex(GPINDEX_VAR1+i))
			AddGPToStatHQ(m_aCurGPSet[i],nCnt,lpnStkIndex,nMaxNum,nType);
	}
	//特别大盘
	if(HasVarGPIndex(GPINDEX_SHZS))
		AddGPToStatHQ(m_nSHZSIndex,nCnt,lpnStkIndex,nMaxNum,nType);
	if(HasVarGPIndex(GPINDEX_SZZS))
		AddGPToStatHQ(m_nSZZSIndex,nCnt,lpnStkIndex,nMaxNum,nType);
	//
	FOR_UNIT(i)
	{
		if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_CHANGEGP_UNIT))
		{
			short nGpIndex = m_Units.pUnit[i]->ProcessMsg(UM_GET_STOCK);
			if(InGroupGP(nGpIndex))
				continue;			
			AddGPToStatHQ(nGpIndex,nCnt,lpnStkIndex,nMaxNum,nType);
		}
	}
	return nCnt;
}

void CComView2::OnTimer(UINT nIDEvent)
{
	int i;
	BOOL bIdle = IsIdleStatus();
	if((nIDEvent==1||nIDEvent==2||nIDEvent==3) && theViewManager.pActiveView == this) //HQGrid的刷新
	{
		if(g_bDowning)	return;
		FOR_UNIT(i)
		{
			if(!m_Units.pUnit[i]) continue;
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i] == HQGRID_UNIT)
				m_Units.pUnit[i]->ProcessMsg(WM_TIMER,nIDEvent);
		}
	}
	if((nIDEvent==0x1111||nIDEvent==0x1112||nIDEvent==0x1113||nIDEvent==0x1114||nIDEvent==0x1115) && !bIdle)				//各组件的刷新
	{
		if(g_bDowning)	return;
		FOR_UNIT(i)
		{
			if(m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW)
				m_Units.pUnit[i]->ProcessMsg(WM_TIMER,nIDEvent);
		}
	}
	if(nIDEvent==0x2222 && m_bAutoPage)									//自动翻页的定时
	{
		if(g_bDowning)	return;
		if(theViewManager.pActiveView != this)
		{ 
			m_bAutoPage = FALSE; 
			return; 
		}
		int allgp = g_pStockIO->GetStockNum(ALL_ALL);
		for(int k=0;k<MAX_COM2_GP;k++)
		{
			if(m_aCurGPSet[k]<DS_FROM)
			{
				if(m_aCurGPSet[k]+1 >= allgp)	
					m_aCurGPSet[k] = 0;
				else	
					++m_aCurGPSet[k];
			}
			else if(g_bHasDS)
			{
				if(m_aCurGPSet[k]+1 >= DS_FROM+g_pStockIO->m_nDSNum)
					m_aCurGPSet[k] = DS_FROM;
				else	
					++m_aCurGPSet[k];
			}
		}
		FOR_UNIT(i)
		{
			if(m_Units.pUnit[i] && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_CHANGEGP_UNIT|BE_UNLOCKED_UNIT|BE_SEE_UNIT))
			{
				if(m_Units.Style[i] == ZST_GPZS) //如果是股票指数Unit，则其股票为GetGPZS()
				{
					for(int k=0;k < MAX_COM2_GP;k++)
					{
						if(m_Units.Var_GPIndex[i]==GPINDEX_VAR1+k) 
							ChangeUnitGP(m_Units.pUnit[i],GetGPZS(m_aCurGPSet[k]));
					}
				}
				else 
				{
					for(int k=0;k < MAX_COM2_GP;k++)
					{
						if(m_Units.Var_GPIndex[i]==GPINDEX_VAR1+k) 
							ChangeUnitGP(m_Units.pUnit[i],m_aCurGPSet[k]);
					}
				}
				if(OnLine)	m_Units.pUnit[i]->ProcessMsg(WM_PAINT,1);
			}
		}
	}
	if(nIDEvent==0x3333 && !bIdle)											//当前版面的行情刷新
		RefreshCurrPADHQ();
	if(nIDEvent==0x4444 && g_bHasInfo && !bIdle)
	{
		if(g_bDowning)	return;
		FOR_UNIT(i)
		{
			if(!m_Units.pUnit[i]) continue;
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==ZST_UNIT)
				m_Units.pUnit[i]->ProcessMsg(WM_TIMER,0x4444);
		}
	}
	if(nIDEvent==0x6666 && !bIdle)											//全景排行的刷新
	{
		if(g_bDowning)	return;
		FOR_UNIT(i)
		{
			if(!m_Units.pUnit[i]) continue;
			if(m_Units.nStatus[i]==U_HIDE||m_Units.Type[i] != ZHSORT_UNIT) continue;
			m_Units.pUnit[i]->ProcessMsg(WM_TIMER,0x6666);
		}
	}
	if(nIDEvent==0x7777  && !bIdle)				//关联行情的定时器
	{
		FOR_UNIT(i)
		{
			if(!m_Units.pUnit[i]) continue;
			if(m_Units.nStatus[i]==U_HIDE||m_Units.Type[i] != GLHQ_UNIT) continue;
			m_Units.pUnit[i]->ProcessMsg(WM_TIMER,0x7777);
		}
	}

	short nHasZHSort = -1;
	FOR_UNIT(i)
	{
		if(!m_Units.pUnit[i]) continue;
		if(m_Units.nStatus[i]==U_HIDE) continue;
		if(m_Units.Type[i]==ZHSORT_UNIT)
		{
			nHasZHSort = i;
			break;
		}
	}
	if(nIDEvent==10000 && !bIdle)
	{
		KillTimer(10000);
		FOR_UNIT(i)
		{
			if(!m_Units.pUnit[i]) continue;
			if(m_Units.nStatus[i]==U_HIDE) continue;
			if(m_Units.Type[i]==RMREPORT_UNIT)
			{
				m_nF = i;
				m_Units.pUnit[i]->ProcessMsg(UM_COM2SWITCH1);
			}
		}
		SetTimer(10001, 1000, NULL);
	}
	if(nIDEvent==10001 && !bIdle)
	{
		KillTimer(10001);
		if(nHasZHSort>=0)
		{
			m_nF = nHasZHSort;
			m_Units.pUnit[nHasZHSort]->ProcessMsg(UM_COM2SWITCH1);
		}
		else
		{
			FOR_UNIT(i)
			{
				if(!m_Units.pUnit[i]) continue;
				if(m_Units.nStatus[i]==U_HIDE) continue;
				if(m_Units.Type[i]==HQGRID_UNIT)
				{
					m_nF = i;
					m_Units.pUnit[i]->ProcessMsg(UM_COM2SWITCH1);
				}
			}
			SetTimer(10002, 1000, NULL);
		}
	}
	if(nIDEvent==10002 && !bIdle)
	{
		KillTimer(10002);
		if(nHasZHSort>=0)
		{
			m_nF = nHasZHSort;
			m_Units.pUnit[nHasZHSort]->ProcessMsg(UM_COM2SWITCH1);
		}
	}
	CView::OnTimer(nIDEvent);
}

//为-1,没有点到任何Unit，>1000,表示点中某个Switch
int	CComView2::HitTest(CPoint point)
{
	int i,nRet = -1;
	FOR_UNIT(i)
	{
		if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT))
		{
			if((m_Units.Rect[i]).PtInRect(point))
			{
				nRet = i;
				break;
			}
		}
	}
	return nRet;
}

void CComView2::OnMouseMove(UINT nFlags, CPoint point)
{
	int i = m_tmpNf = HitTest(point), j;
	if(m_bMoving&&m_pMovingSplit)
	{
		if(!m_pMovingSplit->node->pCurUnit->rect.PtInRect(point)) return;		
		if(i>=0)
		{
			for(j=0;j<m_Units.Num;j++)
			{
				if(m_Units.Type[j]==-1&&m_Units.Style[j]==-1)
					PaintNullUnit(j);
				else if(TestUnitProperty(j,BE_VALID_UNIT|BE_NONULL_UNIT|BE_NOHIDE_UNIT|BE_SEE_UNIT)) 
					m_Units.pUnit[j]->ProcessMsg(WM_PAINT);
			}
		}
		CDC *pDC = GetDC();
		pDC->SetBkMode(TRANSPARENT);
		if(m_bCanMoveSplit==1)
			g_d.DotLine(pDC, point.x, m_pMovingSplit->cursorRt.top+1, point.x, m_pMovingSplit->cursorRt.bottom-1, RGB(255,255,0)^VipColor.BACKCOLOR, 4);
		else if(m_bCanMoveSplit==-1)
			g_d.DotLine(pDC, m_pMovingSplit->cursorRt.left+1, point.y, m_pMovingSplit->cursorRt.right-1, point.y, RGB(255,255,0)^VipColor.BACKCOLOR, 4);
		ReleaseDC(pDC);
		return;
	}

	if(i >= 0)
	{
		if(i != m_oldhit)
		{
			if(m_oldhit>=0 && m_Units.pUnit[m_oldhit] && TestUnitProperty(m_oldhit, BE_VALID_UNIT|BE_NOHIDE_UNIT))
				m_Units.pUnit[m_oldhit]->ProcessMsg(UM_MOUSELEAVE,nFlags,MAKELONG(point.x,point.y));
			m_oldhit = i;
			SetCursor(LoadCursor(NULL,IDC_ARROW));
		}
		if(!m_bCanMoveSplit && m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW)
			m_Units.pUnit[i]->ProcessMsg(WM_MOUSEMOVE,nFlags,MAKELONG(point.x,point.y));
	}
	for(i=0;i<m_aSplitInfo.size();i++)
	{
		if(m_aSplitInfo[i].cursorRt.PtInRect(point))
		{
			m_bCanMoveSplit = m_aSplitInfo[i].splitType;
			return;
		}
	}
	m_bCanMoveSplit = 0;
}

void CComView2::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(m_bMoving&&m_pMovingSplit)
	{
		if((m_pMovingSplit->node->pLeftChild->pCurUnit->nJHMin[0]>0||
			m_pMovingSplit->node->pRightChild->pCurUnit->nJHMin[0]>0)
			&&m_pMovingSplit->splitType==1)
		{
			m_bMoving = FALSE;	m_bCanMoveSplit = 0;
			m_pMovingSplit = NULL;
		}
		else if((m_pMovingSplit->node->pLeftChild->pCurUnit->nJHMin[1]>0||
			m_pMovingSplit->node->pRightChild->pCurUnit->nJHMin[1]>0)
			&&m_pMovingSplit->splitType==-1) 
		{
			m_bMoving = FALSE;	m_bCanMoveSplit = 0;
			m_pMovingSplit = NULL;
		}
		
		else
		{
			double fNewRatio;
			if(m_pMovingSplit->splitType==1)
				fNewRatio = 100.0*((double)point.x-(double)(m_pMovingSplit->node->pCurUnit->rect.left))/(double)(m_pMovingSplit->node->pCurUnit->rect.Width());
			else if(m_pMovingSplit->splitType==-1)
				fNewRatio = 100.0*((double)point.y-(double)(m_pMovingSplit->node->pCurUnit->rect.top))/(double)(m_pMovingSplit->node->pCurUnit->rect.Height());
			else fNewRatio = 0.0;
			if(fNewRatio>3.00 && fNewRatio<97.00) //避免太小
			{
				m_pMovingSplit->node->pLeftChild->pCurUnit->toSplit = (float)fNewRatio;
				m_pMovingSplit->node->pRightChild->pCurUnit->toSplit = 100.0f - (float)fNewRatio;
				ReSplitPad(m_pMovingSplit->node, 0);
			}
			m_bMoving = FALSE;	m_bCanMoveSplit = 0;
			m_pMovingSplit = NULL;
		}
		Invalidate();
	}
	int i = HitTest(point);
	if(i >= 0 && m_Units.pUnit[i] && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
		m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CComView2::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_nF >= 0 && TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT))
	{
		m_Units.pUnit[m_nF]->ProcessMsg(WM_LBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
		if(m_Units.Type[m_nF]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[m_nF]);
			pHQGrid->ProcessHQRefresh();
		}
		DrawBorder();
	}
	if(m_bCanMoveSplit)
	{
		m_pMovingSplit = NULL;
		for(int i=0;i<m_aSplitInfo.size();i++)
		{
			if(m_aSplitInfo[i].cursorRt.PtInRect(point))
				m_pMovingSplit = &m_aSplitInfo[i];
		}
		if(m_pMovingSplit) m_bMoving = TRUE;
	}
}

void CComView2::OnLButtonDblClk(UINT nFlags, CPoint point)
{	
	int i = HitTest(point);
	if(i<0) return;
	if(i>=0&&i<m_Units.Num) m_nF = i;
	if(m_nF<0 || !m_Units.pUnit[m_nF]) return;
	if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
	{
		if(TestUnitProperty(i,BE_NOCHANGEGP_UNIT))
		{
			int iIndex = m_Units.pUnit[m_nF]->ProcessMsg(UM_GET_STOCK);
			
			MemStkInfo *tmpInfo = NULL;
			tmpInfo = (*g_pStockIO)[iIndex];
			if(tmpInfo) ChangeSyncUnitGP(iIndex);
			int nGPVar = m_Units.Var_GPIndex[m_nF] - GPINDEX_VAR1;
			m_aLastCUnit[nGPVar] = m_nF;
		}
		else if(TestUnitProperty(i,BE_CHANGEGP_UNIT))
			m_Units.pUnit[i]->ProcessMsg(WM_LBUTTONDBLCLK,nFlags,MAKELONG(point.x,point.y));
	}
}

void CComView2::OnRButtonUp(UINT nFlags, CPoint point)
{
	int i = HitTest(point);
	if(m_bInSplit)
	{
		//先装入已有的菜单项
		CNewMenu menu;
		menu.LoadMenu (IDR_LONELY_MENU);
//		menu.LoadToolBar(IDR_MAINFRAME);
		CNewMenu *pPopup = (CNewMenu *)menu.GetSubMenu(19);
		//行情表
		CNewMenu *pTypeSelPopup = (CNewMenu *)pPopup->GetSubMenu(0);
		CNewMenu *pHqGridPopup = (CNewMenu *)pTypeSelPopup->GetSubMenu(0);
		CNewMenu HqBlockMenu;
		HqBlockMenu.CreatePopupMenu();
		UINT	StartID = ID_INSERTHQDS1;
		for(int i=0,j=0;i<g_GuiMarketNum;i++)
		{
			HqBlockMenu.AppendMenu(MF_STRING, StartID+j, _F(g_GuiMarketSet[j].BigsetName));
			if( j != 0 && j % 18 == 0 )
				SetMenuBreak(j,&HqBlockMenu);
			++j;
		}
		if(j > 0)
			pHqGridPopup->AppendMenu(MF_POPUP,(UINT)(HqBlockMenu.m_hMenu),_F(g_DS.DSOEMName));

		//加入外部功能Unit
		CString tmpItemStr;
		pTypeSelPopup->AppendMenu(MF_SEPARATOR);
		GNPluginInfo PluginInfo;
		for(int k=0;k<g_GNPlug.GetPluginNum();k++)
		{
			if(!g_GNPlug.GetOnePluginInfo(k,&PluginInfo))
				continue;
			if(PluginInfo.nModuleType==GNPLUG_TYPE_EMBED && !PluginInfo.bOnlyInCom)
			{
				tmpItemStr.Format("%s",PluginInfo.strModuleDesc);
				pTypeSelPopup->AppendMenu(MF_STRING,PluginInfo.nUnitID-EXTERN1_UNIT+ID_INSERTEXTERN1,_F(tmpItemStr));
			}
		}

		ClientToScreen (&point);
		pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,AfxGetMainWnd());
	}
	else if(i >= 0 && !m_bInSplit && m_Units.pUnit[i] && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_HASMENU_UNIT))
		m_Units.pUnit[i]->ProcessMsg(WM_RBUTTONUP,nFlags,MAKELONG(point.x,point.y));
}

void CComView2::OnRButtonDown(UINT nFlags, CPoint point) 
{
	int i = HitTest(point);
	if(i >= 0 && m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW)
		m_nF = i;
	if(!m_bInSplit && m_nF>=0 && m_Units.pUnit[m_nF])
		m_Units.pUnit[m_nF]->ProcessMsg(WM_RBUTTONDOWN,nFlags,MAKELONG(point.x,point.y));
}

BOOL CComView2::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if(zDelta < 0)
	{
		OnKeyDown(VK_NEXT,1,0);
		OnKeyUp(VK_NEXT,1,0);
	}
	else
	{
		OnKeyDown(VK_PRIOR,1,0);
		OnKeyUp(VK_PRIOR,1,0);
	}
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CComView2::SetCurPadGP(int iNo)
{
	if(m_nF < 0 || !m_Units.pUnit[m_nF] || m_Units.nStatus[m_nF] == U_HIDE) return;
	int i; BOOL bContrall = FALSE;
	for(i=0;i<MAX_NODE;i++)
	{
		if(m_apNode[i]&&m_apNode[i]->pCurUnit&&m_apNode[i]->pCurUnit->pUnit&&m_apNode[i]->pCurUnit->pUnit==m_Units.pUnit[m_nF]&&m_apNode[i]->pCurUnit->bContrallUnit)
		{
			if(g_nLinkage)
				bContrall = TRUE;
			break;
		}
	}
	SetMainID();
	if(!bContrall)
	{
		ChangeUnitGP(m_Units.pUnit[m_nF],iNo);
		for(int k=0;k < MAX_COM2_GP;k++)
		{
			if(m_Units.Var_GPIndex[m_nF]==GPINDEX_VAR1+k) 
				ReadAndPaint(0,GPINDEX_VAR1+k);
		}
	}
	else
	{
		for(int k=0;k < MAX_COM2_GP;k++)
		{
			if(m_Units.Var_GPIndex[m_nF]==GPINDEX_VAR1+k) 
			{
				m_aCurGPSet[k] = iNo;
				FOR_UNIT(i)
				{
					if(m_Units.nStatus[i]==U_HIDE) continue;
					if(m_Units.Var_GPIndex[i]==GPINDEX_VAR1+k)
						ChangeUnitGP(m_Units.pUnit[i],m_aCurGPSet[k]);
				}
				ReadAndPaint(0,GPINDEX_VAR1+k);
			}
		}
	}
}

void CComView2::OnGetDataAck(WPARAM wParam,LPARAM lParam)
{
	short whichunit,whichreq,i;
	
	if(wParam==1)	//	三方市场
	{
		GetComAssisID(g_AnsHeader3.AssisID,whichunit,whichreq);
		if(whichreq==HQGRID_REQ+1&&m_nMainID==g_AnsHeader3.MainID)	//	三方市场
		{
			FOR_UNIT(i)
			{
				if(m_Units.Type[i]!=HQGRID_UNIT||i!=whichunit) continue;
				
				HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
				pHQGrid->GetDataACK(TRUE);
				if(g_AnsHeader3.Priority!=2)	//如果不是刷新行情请求，则激活下一缓冲请求
				{
					if(g_pComm3->m_HQBufFlag == 2)
					{
						g_pComm3->SendOneBuf((char*)&g_pComm3->m_HQSendBuf,
							sizeof(REQHEADER)+(g_pComm3->m_HQSendBuf).ReqHeader.PacketLen);
						g_pComm3->m_HQBufFlag = 1;
					}
					else	g_pComm3->m_HQBufFlag = 0;
				}
			}
		}
	}
	else
	{
		GetComAssisID(g_AnsHeader.AssisID,whichunit,whichreq);
		if(whichreq==HQGRID_REQ&&m_nMainID==g_AnsHeader.MainID)
		{
			FOR_UNIT(i)
			{
				if(m_Units.Type[i]!=HQGRID_UNIT||i!=whichunit) continue;
				
				HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[i]);
				pHQGrid->GetDataACK();
				if(g_AnsHeader.Priority!=2)
				{
					if(g_pComm->m_HQBufFlag == 2)
					{
						g_pComm->SendOneBuf((char*)&(g_pComm->m_HQSendBuf),sizeof(REQHEADER)+(g_pComm->m_HQSendBuf).ReqHeader.PacketLen);
						g_pComm->m_HQBufFlag = 1;
					}
					else	g_pComm->m_HQBufFlag = 0;
				}
			}
		}
	}

	CUnitView::OnGetDataAck(wParam, lParam);
}

BOOL CComView2::PreTranslateMessage(MSG* pMsg)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	if(pMsg->message==WM_LBUTTONDOWN||pMsg->message==WM_RBUTTONDOWN||
		pMsg->message==WM_LBUTTONUP||pMsg->message==WM_RBUTTONUP)
	{
		int iUnit = HitTest(pt);
		if(iUnit>=0&&iUnit<=m_Units.Num&&m_nF!=iUnit)
		{
			m_nF = iUnit;
			SetFocus();
			DrawBorder();
		}
	}
	if(m_bMoving&&(pMsg->message==WM_MOUSEMOVE||pMsg->message==WM_LBUTTONUP))
		SendMessage(pMsg->message,pMsg->wParam,MAKELPARAM(pt.x,pt.y));
	return CUnitView::PreTranslateMessage(pMsg);
}

LRESULT CComView2::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	BOOL bContrall = FALSE;
	//响应一些改变行情图的分类,排序
	if( message==WM_TREE_MESSAGE ||	//自定义消息
		message==WM_KEYGUY_MESSAGE || 
		message==UM_TBBD_MESSAGE || 
		message==WM_CHANGEBLOCK
	  )
	{
		BOOL bProcess=FALSE;
		if(m_nF>=0 && m_Units.Type[m_nF]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
			if(pHQGrid && !pHQGrid->IsFixedGrid())
			{
				pHQGrid->ProcessMsg(message,wParam,lParam);
				bProcess=TRUE;
			}
		}
		if(!bProcess)
		{
			int i;
			FOR_UNIT(i)
			{
				if(m_Units.Type[i]==HQGRID_UNIT)
				{
					HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
					if(pHQGrid && !pHQGrid->IsFixedGrid())
					{
						pHQGrid->ProcessMsg(message,wParam,lParam);
						break;
					}
				}
			}
		}
	}
	if( message==UM_BLOCKMODIFIED && g_bBlockModified )
	{
		g_bBlockModified = FALSE;
		int i;
		FOR_UNIT(i)
		{
			if(m_Units.Type[i]==HQGRID_UNIT)
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
				if(pHQGrid) pHQGrid->ProcessMsg(message,wParam,lParam);
			}
		}
	}
	if( message==UM_UNIT_BROADCAST)		//要拆包
	{
		int i;
		FOR_UNIT(i)
		{
			MSG *pMsg = (MSG*)wParam;
			int iTmpWhich = lParam;
			if((m_Units.Type[iTmpWhich]==RMREPORT_UNIT&&m_Units.Type[i]==HQGRID_UNIT)||m_Units.Var_GPIndex[iTmpWhich]==m_Units.Var_GPIndex[i])
				m_Units.pUnit[i]->ProcessMsg(pMsg->message,pMsg->wParam,pMsg->lParam);
		}
	}
	if( message==UM_GRIDREINIT || message==UM_CHANGEGRIDWIDTH)
	{
		int nDefNo = GetCusPadTypeParam(g_DefCusPadName);
		if(nDefNo>=0)	SendMessage(UM_CONSTRUCTPAD_MESSAGE, nDefNo,1); //lParam为1表示强制生成构建
	}
	if( message==UM_HSCROLLBAR || message==UM_VSCROLLBAR)
	{
		if(lParam>=0 && lParam<m_Units.Num && m_Units.Type[lParam]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[lParam];
			if(pHQGrid)
				pHQGrid->ProcessMsg(message,wParam,lParam);
		}
	}
	//远程雷到来的消息
	if ( UM_MINEMSG == message || UM_GGBWMSG == message || UM_FLZLMSG == message)
	{
		int i;
		FOR_UNIT(i)
		{
			if((m_Units.Type[i]==FXT_UNIT || m_Units.Type[i]==ZST_UNIT) && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
				m_Units.pUnit[i]->ProcessMsg(message,wParam,lParam);
		}
	}
	if(message==UM_RESETGRIDTAB)
	{
		int i;
		FOR_UNIT(i)
		{
			if(m_Units.Type[i]==HQGRID_UNIT)
			{
				HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[i];
				if(pHQGrid->m_bFixedGrid) continue;
				pHQGrid->InsertSwitch(0);
				pHQGrid->SetRect(CRect(0,0,0,0), TRUE);
			}
		}
	}
	if(message==UPDATE_WARNGP)
	{
		int i;
		FOR_UNIT(i)
		{
			if(m_Units.Type[i] == BIGVOL_UNIT||m_Units.Type[i] == RADAR_UNIT)
				m_Units.pUnit[i]->ProcessMsg(UPDATE_WARNGP);
		}
	}
	switch(message)
	{
	case WM_KEYGUY_MESSAGE:
		{
			if(m_nF<0 || !m_Units.pUnit[m_nF]) break;
			int MessageCode = (int)wParam;
			if(MessageCode == 8)	//F8切换周期
			{
				if(m_Units.Type[m_nF]==FXT_UNIT&&TestUnitProperty(m_nF, BE_VALID_UNIT|
					BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_UNLOCKED_UNIT))
				{
					((UFxt*)m_Units.pUnit[m_nF])->ProcessMsg(WM_KEYDOWN,VK_F8);
					break;
				}
				break;
			}
			if(MessageCode >= 801 && MessageCode <= 812) //切换周期
			{
				OnPeriod(MessageCode-801+ID_MIN1);
				break;
			}
			else if(MessageCode >= KEYGUY_SEPCZB) //特殊指标
			{
				if(m_Units.Type[m_nF]==FXT_UNIT&&TestUnitProperty(m_nF, BE_VALID_UNIT|
					BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_UNLOCKED_UNIT))
				{
					m_Units.pUnit[m_nF]->ProcessMsg(UM_CHANGESPEZB,MessageCode-KEYGUY_SEPCZB);
					break;
				}
				break;
			}
			else if(MessageCode >= KEYGUY_ZB) //指标
			{
				if(m_Units.Type[m_nF] == FXT_UNIT&&TestUnitProperty(m_nF, BE_VALID_UNIT|
					BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_UNLOCKED_UNIT))
				{
					m_Units.pUnit[m_nF]->ProcessMsg(UM_CHANGEZB,MessageCode-KEYGUY_ZB);
					break;
				}
				break;
			}
			int nTypeStyle = ZST_BIG;
			if(Proc_Special_MessageCode(MessageCode, nTypeStyle))
			{
				int nType = nTypeStyle/1000; 
				int nStyle = nTypeStyle%1000;
				
				int nTargetNode = Index2NodeNo(m_nF);
				if(nTargetNode<0) return FALSE;
				m_apNode[nTargetNode]->pCurUnit->unitType = nType;
				m_apNode[nTargetNode]->pCurUnit->style = nStyle;
				
				int nCurGPNo = 140;
				for(int k=0;k < MAX_COM2_GP;k++)
				{
					if(m_Units.Var_GPIndex[m_nF]==GPINDEX_VAR1+k) 
						nCurGPNo = m_aCurGPSet[k];
				}
				m_Units.Type[m_nF] = nType; m_Units.Style[m_nF] = nStyle;
				CreateUnit(nCurGPNo, nType, nStyle,m_nF,&m_Units.pUnit[m_nF]);
				m_apNode[nTargetNode]->pCurUnit->pUnit = m_Units.pUnit[m_nF];
				SetUnitReadAndPaint(m_nF,m_Units.Rect[m_nF],COM_ReadDraw);
			}
		}
		break;
	case WM_DESTROY:
		if(!m_bInSplit)	SavePadInfo(-1);
		break;
	case UM_REPAINTCOM_MESSAGE:
		{
			int i;
			FOR_UNIT(i)
			{
				if(m_Units.pUnit[i] && m_Units.nStatus[i]==U_SHOW) 
					m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
			}
		}
		break;
	default:
		break;
	}
	
	return CUnitView::WindowProc(message, wParam, lParam);
}

BOOL CComView2::Proc_Special_MessageCode(int tmpMessageCode, int &unitID)
{
	if(tmpMessageCode==506)
	{
		unitID=ZST_LB;
		return TRUE;
	}
	else if(tmpMessageCode==507)
	{
		unitID=ZST_MMLD;
		return TRUE;
	}
	else if(tmpMessageCode==501)
	{
		g_bTickTStatus=FALSE;
		unitID=ZST_BIG;
		return TRUE;
	}
	else if(tmpMessageCode==508)
	{
		g_bTickTStatus=TRUE;
		unitID=ZST_BIG;
		return TRUE;
	}
	int nVarGP = m_Units.Var_GPIndex[m_nF];
	if(tmpMessageCode>=101 && tmpMessageCode<=119 && TestUnitProperty(m_nF, BE_UNLOCKED_UNIT)) //如果是开始进入是大盘，则重新得到m_aCurGPSet;
	{
		if(tmpMessageCode <= 106)
			m_aCurGPSet[nVarGP] = g_pStockIO->GetIndex(DpGpCode[tmpMessageCode-101],SH); 
		else if(tmpMessageCode <= 113 || tmpMessageCode == 117 || tmpMessageCode == 119)
			m_aCurGPSet[nVarGP] = g_pStockIO->GetIndex(DpGpCode[tmpMessageCode-101],SZ); 
		else
			m_aCurGPSet[nVarGP] = g_pStockIO->GetIndex(DpGpCode[tmpMessageCode-101],SH); 
		if(tmpMessageCode==105 || tmpMessageCode==112)
			unitID = ZST_MMLD;
		else if(tmpMessageCode==106 || tmpMessageCode==113)
			unitID = ZST_LB;
		return TRUE;
	}
	if(tmpMessageCode >= 301 && tmpMessageCode <= 300+g_nOftenUseZb) //指标命令
	{
		m_nOften301 = tmpMessageCode;
		m_nFxPeriod = g_nMenuPeriod;
		return TRUE;
	}
	return FALSE;
}

BOOL CComView2::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if(m_bCanMoveSplit==1) SetCursor(AfxGetApp()->LoadCursor(IDC_VERSPLIT));
	else if(m_bCanMoveSplit==-1) SetCursor(AfxGetApp()->LoadCursor(IDC_HORSPLIT));
	else SetCursor(LoadCursor(NULL,IDC_ARROW));

	return CUnitView::OnSetCursor(pWnd, nHitTest, message);
}

void CComView2::OnDeleteunit()
{
	if(!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT)) return;
	UnitTreeNode* pNode = NULL; int nAcUnit=0, i;
	FOR_UNIT(i)
	{
		if(m_Units.nStatus[i]==U_SHOW)
			nAcUnit++;
	}
	if(nAcUnit<=1) return;
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	if(TDX_MessageBox (m_hWnd,"您确定删除本单元吗？",MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL) 
		return;
	pNode = HideOneUnit(m_apNode[iNode]);
	if(pNode)
	{
		m_Units.nStatus[m_nF] = U_HIDE;
		m_Units.pUnit[m_nF]->ProcessMsg(WM_SIZE,0,0);
		//有自画的控件等UNIT特殊处理隐藏
		if(m_Units.Type[m_nF] == HQGRID_UNIT)
		{
			HQGrid *pHqGrid = (HQGrid*)m_Units.pUnit[m_nF];
			pHqGrid->HideSwitchBar();
		}
		FOR_UNIT(i)
		{
			if(m_Units.nStatus[i]==U_SHOW)
			{
				m_nF = i;
				break;
			}
		}
		ReSplitPad(pNode, 3);
	}
	Invalidate();
}

void CComView2::OnUpdateDeleteunit(CCmdUI* pCmdUI)
{
	int i, nActUnit = 0;
	FOR_UNIT(i)
	{
		if(TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT))
			nActUnit++;
	}
	pCmdUI->Enable(nActUnit>1);
}

void CComView2::WriteCustomPadInfo(int nCmdNum)
{
	if(m_Units.Num<1) return;
	CString	padini, sStep; CFile ini;
	padini = g_WSXHStr+(CString)"sheet\\"+m_PadCode+".sp";
	if(!ini.Open(padini, CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite))	return;
	ini.Close();

	char buf[16];
	map<char*, int ,cmpstr>::iterator iter;

	int nAcUnit = 0,i;
	FOR_UNIT(i)
	{
		if(m_Units.nStatus[i]==U_SHOW)
			nAcUnit++;
	}
	GetSplitInfo(m_pRootUnit);

	WritePrivateProfileString("DEAFULTGP", "Name", m_PadName, padini);

	WriteDefaultGP(padini);

	WritePrivateProfileString("DEAFULTGP", "UnitNum", IntToStr(nAcUnit), padini);
	if(nCmdNum>-1) WritePrivateProfileString("DEAFULTGP", "CmdNum", IntToStr(nCmdNum), padini);
	for(i=0;i<m_apSplitSeq.size();i++)
	{
		memset(buf, 0, 16*sizeof(char));
		SplitActInfo split = m_apSplitSeq[i];
		if(!split.pNode->pCurUnit) continue;
		sStep = "STEP"+IntToStr(i);
		WritePrivateProfileString(sStep, "SplitWhich", IntToStr(split.nDepUnit), padini);
		for(iter=g_mUnitType.begin();iter!=g_mUnitType.end();iter++)
		{
			if((*iter).second==split.pNode->pCurUnit->unitType)
				strcpy(buf, (*iter).first);
		}
		WritePrivateProfileString(sStep, "UnitType", buf, padini);
		memset(buf, 0, 16*sizeof(char));
		for(iter=g_mUnitStyle.begin();iter!=g_mUnitStyle.end();iter++)
		{
			int	ns = (*iter).second;
			if((*iter).second==split.pNode->pCurUnit->style)
				strcpy(buf, (*iter).first);
		}
		WritePrivateProfileString(sStep, "UnitStyle", buf, padini);
		WritePrivateProfileString(sStep, "HowToSplit", IntToStr(split.nSplitType), padini);	
		WritePrivateProfileString(sStep, "SplitRatio", IntToStr(split.fToSplit), padini);	
		WritePrivateProfileString(sStep, "HowToSplit", IntToStr(split.nSplitType), padini);	
		WritePrivateProfileString(sStep, "ShowGpNo", IntToStr(split.pNode->pCurUnit->var_GpIndex-GPINDEX_VAR1+1), padini);
		WritePrivateProfileString(sStep, "IsContrallUnit", IntToStr(split.pNode->pCurUnit->bContrallUnit), padini);	
		WritePrivateProfileString(sStep, "IsLocked", IntToStr(split.pNode->pCurUnit->bLocked), padini);

		WritePrivateProfileString(sStep, "Fixed_Width", IntToStr(split.pNode->pCurUnit->nJHMin[0]), padini);
		WritePrivateProfileString(sStep, "Fixed_Height", IntToStr(split.pNode->pCurUnit->nJHMin[1]), padini);

		if(split.pNode->pCurUnit->unitType==FXT_UNIT)
		{
			UFxt *pUFxt = (UFxt*)split.pNode->pCurUnit->pUnit;
			WritePrivateProfileString(sStep, "InitShowNum", IntToStr(pUFxt->GetDrawNum()),padini);
			//
			FMLPACK tmpFMLPACK;
			char	tmpStr[100];
			memset(&tmpFMLPACK,0,sizeof(FMLPACK));
			pUFxt->GetCom2Fml(&tmpFMLPACK);
			WritePrivateProfileString(sStep, "WndNum", IntToStr(tmpFMLPACK.lRectNum),padini);
			WritePrivateProfileString(sStep, "FXPeriod", IntToStr(tmpFMLPACK.nPeriod+1), padini);
			for(int j=0;j<tmpFMLPACK.lRectNum && j<10;j++)
			{
				sprintf(tmpStr,"Formula%02d",j+1);
				WritePrivateProfileString(sStep,tmpStr,tmpFMLPACK.aFormular[j],padini);
			}
		}
		else if(split.pNode->pCurUnit->unitType==ZHSORT_UNIT)
		{
			UZhSort *pUZhSort = (UZhSort*)split.pNode->pCurUnit->pUnit;
			CString sSortVal; int nSort, WhichGPType;
			pUZhSort->GetOptValue(nSort, sSortVal, WhichGPType);
			WritePrivateProfileString(sStep, "SortNum", IntToStr(nSort), padini);
			WritePrivateProfileString(sStep, "GPType", IntToStr(WhichGPType), padini);
			WritePrivateProfileString(sStep, "SortOpt", sSortVal, padini);
		}
		else if(split.pNode->pCurUnit->unitType==HQGRID_UNIT)
		{
			HQGrid *pHqGrid = (HQGrid*)split.pNode->pCurUnit->pUnit;
			pHqGrid->GetDomain(m_WhichGPType[i], m_nBlockFlag[i]);
			WritePrivateProfileString(sStep, "GPType", IntToStr(m_WhichGPType[i]), padini);
			WritePrivateProfileString(sStep, "BlockFlag", IntToStr(m_nBlockFlag[i]), padini);
			WritePrivateProfileString(sStep, "JSZJFlag", IntToStr(m_bJSZJFlag[i]), padini);
		}
		else if(split.pNode->pCurUnit->unitType==WEB_UNIT)
		{
			UWeb2 *pUWeb = (UWeb2*)split.pNode->pCurUnit->pUnit;
			WritePrivateProfileString(sStep, "Url", pUWeb->GetURL(), padini);
		}
	}
}

#include "CusPadSaveDlg.h"
void CComView2::SavePadInfo(int nCmdNum)
{
	return;
	if(strcmp(m_PadCode, nullPadName)==0)
	{
		CCusPadSaveDlg dlg;
		if(dlg.DoModal()!=IDOK) 
		{
			SendMessage(WM_DESTROY);
			return;
		}
		strcpy(m_PadName, dlg.m_sChName.GetLength()?dlg.m_sChName:dlg.m_sEnName);
		strcpy(m_PadCode, dlg.m_sEnName);
		nCmdNum = dlg.m_nCmdNum;
	}
	m_bInSplit = FALSE;
	WriteCustomPadInfo(nCmdNum);
	GetDynaCusPadInfo();
	m_nNowPad = GetCusPadTypeParam(m_PadName);
	SetComTitle();
	((CMainFrame*)AfxGetMainWnd())->FillGnTree();
}

void CComView2::OnSavepad()
{
	BOOL bInSplit = m_bInSplit;
	CCusPadSaveDlg dlg;
	if(strcmp(m_PadCode, nullPadName)!=0)
	{
		dlg.m_sChName = m_PadName; 
		dlg.m_sEnName = m_PadCode;
	}
	if(dlg.DoModal()!=IDOK) 
		return;

	CString padini; CFile ini;
	padini = g_WSXHStr+(CString)"sheet\\"+m_PadCode+".sp";
	
	if(ini.Open(padini, CFile::modeReadWrite|CFile::shareDenyNone))
	{
		ini.Close();
		CFile::Remove( padini );
	}

	if(strcmp(g_DefCusPadName,m_PadName)==0)									//系统盘面改名
		strcpy(g_DefCusPadName, dlg.m_sChName.GetLength()?dlg.m_sChName:dlg.m_sEnName);
	strcpy(m_PadName, dlg.m_sChName.GetLength()?dlg.m_sChName:dlg.m_sEnName);
	strcpy(m_PadCode, dlg.m_sEnName);

	SavePadInfo(dlg.m_nCmdNum);
	m_bInSplit = bInSplit;
}

void CComView2::OnUpdateSavepad(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_Fixed);
}

void CComView2::OnContrallunit()
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	m_apNode[iNode]->pCurUnit->bContrallUnit = !m_apNode[iNode]->pCurUnit->bContrallUnit;
}

void CComView2::OnUpdateContrallunit(CCmdUI* pCmdUI)
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	pCmdUI->SetCheck(m_apNode[iNode]->pCurUnit->bContrallUnit);
}

void CComView2::OnSetGpVar(UINT nID)
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	for(int k=0;k < MAX_COM2_GP;k++)
	{
		if(nID==ID_GPVAR1+k) 
		{
			m_apNode[iNode]->pCurUnit->var_GpIndex = m_Units.Var_GPIndex[m_nF] = GPINDEX_VAR1+k;
			ChangeUnitGP(m_Units.pUnit[m_nF], m_aCurGPSet[k]);
		}
	}
	m_apNode[iNode]->pCurUnit->pUnit->ProcessMsg(WM_PAINT, 1);
}

void CComView2::OnUpdateSetGpVar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_UNLOCKED_UNIT));
	pCmdUI->SetRadio(GPINDEX_VAR1+pCmdUI->m_nID-ID_GPVAR1==m_Units.Var_GPIndex[m_nF]);
}

void CComView2::OnNewpad() 
{	
	if(g_vDynaCmdTab.size()>=100)
	{
		TDX_MessageBox (m_hWnd,"系统只能支持100个定制版面!",MB_ICONINFORMATION|MB_OK);	
		return;
	}
	CString tmpPadIni; CFile tmpIni;
	tmpPadIni = g_WSXHStr+(CString)"sheet\\"+nullPadName+".spo";
	if(!tmpIni.Open(tmpPadIni, CFile::modeCreate|CFile::modeWrite))	return;
	tmpIni.Close();
	WritePrivateProfileString("DEAFULTGP", "GPINDEX_VAR1", IntToStr(140), tmpPadIni);
	WritePrivateProfileString("DEAFULTGP", "UnitNum", IntToStr(1), tmpPadIni);

	SendMessage(UM_CONSTRUCTPAD_MESSAGE, 1208,0);
	
	TDX_MessageBox (m_hWnd,"提示:新建版面时,请点右键进行操作,\r\n新建完毕后,选择\'退出设置版面\'",MB_ICONINFORMATION|MB_OK);	
}

void CComView2::OnUpdateNewPad(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(strcmp(m_PadName,nullPadName));
}

void CComView2::OnBeginsplit()
{
	if(m_bInSplit)	SavePadInfo(-1);
	else m_bInSplit = TRUE;
	//窗口unit隐藏显示
	int i=0;
	FOR_UNIT(i)
	{
		if(m_Units.Type[i]==F10_UNIT||m_Units.Type[i]==RMREPORT_UNIT||m_Units.Type[i]==STAR_UNIT)
		{
			UBaseInner *pUBaseInner = (UBaseInner *)m_Units.pUnit[i];
			if(pUBaseInner) pUBaseInner->Resize();
		}
	}
	SetComTitle();

	SendMessage(UM_REFRESHCUSTOMBAR);
}

void CComView2::OnUpdateBeginsplit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_Fixed);
	if(!m_bInSplit)
	{
		pCmdUI->SetCheck(FALSE);
		pCmdUI->SetText(_F("开始设置版面"));
	}
	else 
	{
		pCmdUI->SetText(_F("退出设置版面"));
		pCmdUI->SetCheck(TRUE);
	}
}

void CComView2::OnLockunit()
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	m_apNode[iNode]->pCurUnit->bLocked = !m_apNode[iNode]->pCurUnit->bLocked;
}

void CComView2::OnUpdateLockunit(CCmdUI* pCmdUI)
{
	if(!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|BE_CHANGEGP_UNIT))
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	pCmdUI->SetCheck(m_apNode[iNode]->pCurUnit->bLocked);
}

void CComView2::OnLiminWidth()
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	if(m_apNode[iNode]->pCurUnit->nJHMin[0]==0)
		m_apNode[iNode]->pCurUnit->nJHMin[0] = m_apNode[iNode]->pCurUnit->rect.Width();
	else m_apNode[iNode]->pCurUnit->nJHMin[0]=0;
	ReJustJHMin(m_apNode[iNode]->pCurUnit);
}

void CComView2::OnUpdateLiminWidth(CCmdUI* pCmdUI)
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	pCmdUI->SetCheck(m_apNode[iNode]->pCurUnit->nJHMin[0]>0);
}

void CComView2::OnLiminHeight()
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	if(m_apNode[iNode]->pCurUnit->nJHMin[1]==0)
		m_apNode[iNode]->pCurUnit->nJHMin[1] = m_apNode[iNode]->pCurUnit->rect.Height();
	else m_apNode[iNode]->pCurUnit->nJHMin[1]=0;
	ReJustJHMin(m_apNode[iNode]->pCurUnit);
}

void CComView2::OnUpdateLiminHeight(CCmdUI* pCmdUI)
{
	int iNode = Index2NodeNo(m_nF);
	if(iNode<0) return;
	pCmdUI->SetCheck(m_apNode[iNode]->pCurUnit->nJHMin[1]>0);
}

void CComView2::OnUpdateChangeGpType(CCmdUI *pCmdUI)
{
	if(m_Units.Type[m_nF]!=HQGRID_UNIT||
		!TestUnitProperty(m_nF, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT))
	{
		pCmdUI->Enable(FALSE);
		return;
	}
	HQGrid* pHQGrid = (HQGrid*)m_Units.pUnit[m_nF];
	pCmdUI->Enable(!pHQGrid->IsFixedGrid());
}

//////////////////////////////////////////////////////////////////////////
//Help Functions
int CComView2::Index2NodeNo(int iIndex)
{
	for(int i=0;i<MAX_NODE;i++)
	{
		if(m_apNode[i]&&m_apNode[i]->pCurUnit&&m_apNode[i]->pCurUnit->pUnit&&
			m_apNode[i]->pCurUnit->pUnit==m_Units.pUnit[iIndex])
			return i;
	}
	return -1;
}

BOOL CComView2::TestUnitProperty(int iUnit, UINT nFlag)
{
	BOOL bOK = TRUE;
	if(iUnit<0)	return FALSE;
	if(bOK&&(nFlag&BE_VALID_UNIT))
	{
		if(!m_Units.pUnit[iUnit])
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_INVALID_UNIT))
	{
		if(m_Units.pUnit[iUnit])
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_NOHIDE_UNIT))
	{
		if(m_Units.nStatus[iUnit]==U_HIDE)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_HIDE_UNIT))
	{
		if(m_Units.nStatus[iUnit]==U_SHOW)
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_NOSEE_UNIT))
	{
		if(m_Units.Rect[iUnit].Width()>=1&&m_Units.Rect[iUnit].Height()>=1)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_SEE_UNIT))
	{
		if(m_Units.Rect[iUnit].Width()<1||m_Units.Rect[iUnit].Height()<1)
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_NONULL_UNIT))
	{
		if(m_Units.Type[iUnit]==-1&&m_Units.Style[iUnit]==-1)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_NULL_UNIT))
	{
		if(m_Units.Type[iUnit]!=-1)
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_CHANGEGP_UNIT))
	{
		if(m_Units.Type[iUnit]==HQGRID_UNIT||m_Units.Type[iUnit]==ZHSORT_UNIT||m_Units.Type[iUnit]==GLHQ_UNIT||
			m_Units.Type[iUnit]==BIGVOL_UNIT||m_Units.Type[iUnit]==RADAR_UNIT||m_Units.Type[iUnit]==MTSORT_UNIT)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_NOCHANGEGP_UNIT))
	{
		if(m_Units.Type[iUnit]!=HQGRID_UNIT&&m_Units.Type[iUnit]!=ZHSORT_UNIT&&m_Units.Type[iUnit]!=GLHQ_UNIT&&
			m_Units.Type[iUnit]!=BIGVOL_UNIT&&m_Units.Type[iUnit]!=RADAR_UNIT&&m_Units.Type[iUnit]!=MTSORT_UNIT)
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_CONTRALL_UNIT))
	{
		int iNode = Index2NodeNo(iUnit);
		if(iNode<0) bOK = FALSE;
		else if(!m_apNode[iNode]->pCurUnit->bContrallUnit)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_NOCONTRALL_UNIT))
	{
		int iNode = Index2NodeNo(iUnit);
		if(iNode<0) bOK = FALSE;
		else if(m_apNode[iNode]->pCurUnit->bContrallUnit)
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_LOCKED_UNIT))
	{
		int iNode = Index2NodeNo(iUnit);
		if(iNode<0) bOK = FALSE;
		else if(!m_apNode[iNode]->pCurUnit->bLocked)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_UNLOCKED_UNIT))
	{
		int iNode = Index2NodeNo(iUnit);
		if(iNode<0) bOK = FALSE;
		else if(m_apNode[iNode]->pCurUnit->bLocked)
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_GPINAC_UNIT))
	{
		if(m_Units.Var_GPIndex[iUnit]!=m_Units.Var_GPIndex[m_nF])
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_GPNOINAC_UNIT))
	{
		if(m_Units.Var_GPIndex[iUnit]==m_Units.Var_GPIndex[m_nF])
			bOK = FALSE;
	}

	if(bOK&&(nFlag&BE_NODS_UNIT))
	{
		if(GetCurrentGPIndex()<DS_FROM)
			bOK = FALSE;
	}
	if(bOK&&(nFlag&BE_DS_UNIT))
	{
		if(GetCurrentGPIndex()>=DS_FROM)
			bOK = FALSE;
	}

	return bOK;
}

void CComView2::OnDeletecurpad()
{
	CString tmpStr;
	tmpStr.Format("您确定删除当前版面[%s]吗?",m_PadName);
	if(TDX_MessageBox (m_hWnd,tmpStr,MB_ICONQUESTION|MB_OKCANCEL)==IDCANCEL)	
		return;
	char code[16], name[40];
	strcpy(code, m_PadCode); strcpy(name, m_PadName);

	CFrameWnd *tempFrameWnd = (CFrameWnd *)theViewManager.pActiveView->GetParent();
	int nDefCusPad = GetCusPadTypeParam(g_DefCusPadName);
	if(tempFrameWnd->m_hWnd != FirstWindow && nDefCusPad==-1)
		tempFrameWnd->SendMessage(WM_CLOSE);
	else if(nDefCusPad>=0)
		SendMessage(UM_CONSTRUCTPAD_MESSAGE, nDefCusPad,0);
	//重建动态键表
	CString padini; CFile ini;
	padini = g_WSXHStr+(CString)"sheet\\"+code+".sp";
	vector<DynaCmdTable> newDynaTab; newDynaTab.clear();
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(strcmp(name, g_vDynaCmdTab[i].ChName)==0) continue;
		newDynaTab.push_back(g_vDynaCmdTab[i]);
	}
	newDynaTab.swap(g_vDynaCmdTab);
	newDynaTab.clear();

	if(ini.Open(padini, CFile::modeReadWrite|CFile::shareDenyNone))
	{
		ini.Close();
		CFile::Remove( padini );
	}

	((CMainFrame*)AfxGetMainWnd())->FillGnTree();	

	SendMessage(UM_REFRESHCUSTOMBAR);
}

void CComView2::OnUpdateDeletecurpad(CCmdUI* pCmdUI)
{
	pCmdUI->Enable((strcmp(g_DefCusPadName,m_PadName)) && !m_Fixed);
}

void CComView2::OnUpdateSetunitoption(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Units.Type[m_nF]==ZHSORT_UNIT);	
}

void CComView2::OnSetunitoption()
{
	if(m_Units.Type[m_nF]!=ZHSORT_UNIT) return;

	ZhSortOptDlg dlg;
	UZhSort *pUZhSort = (UZhSort*)m_Units.pUnit[m_nF];
	dlg.m_nShowNum = pUZhSort->m_nUnitRow==3?0:1;
	pUZhSort->SetShowSort(&dlg.m_aShowSort);
	if(dlg.DoModal()==IDOK)
	{
		pUZhSort->SetStyle(dlg.m_nShowNum==0?STYLE_3X3:STYLE_2X2);
		pUZhSort->ProcessMsg(WM_PAINT, 1);
	}
}

void CComView2::OnOpenURL()
{
	if(m_Units.Type[m_nF]!=WEB_UNIT) return;
	UWeb2 *pUWeb = (UWeb2*)m_Units.pUnit[m_nF];

	CInputName2 dlg;
	dlg.Caption = _F("输入文件路径或网址");
	dlg.HintInfo = _F("请输入本地文件路径或网址:\r\n(网址中若有xxxxxx,浏览时替换为证券代码,!!替换为sz或sh)");
	dlg.m_NewName = pUWeb->GetURL();
	dlg.m_bHasBrowse = TRUE;
	dlg.m_strTypeStr = "浏览器可支持类型(*.*)|*.*||";
	if( dlg.DoModal() == IDOK )
	{
		TrimString(dlg.m_NewName);
		pUWeb->SetURL(dlg.m_NewName,TRUE);
	}
}

void CComView2::OnLinkage()
{
	if(g_nLinkage == 0)	g_nLinkage = 2;
	else				g_nLinkage = 0;
}

void CComView2::OnUpdateLinkage(CCmdUI* pCmdUI)
{
	if(g_nLinkage == 0)	pCmdUI->SetCheck(0);
	else				pCmdUI->SetCheck(1);	
}

void CComView2::OnF5()
{
	/*
	CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
	if(m_nF>=0 && m_Units.Type[m_nF]==FXT_UNIT)
	{
		UFxt *pFxt = (UFxt*)m_Units.pUnit[m_nF];
		int nPeriod=pFxt->GetCurrPeriod();
		int nMessageCode = 801+TdxPeriodToKeyOrder(nPeriod);
		pMainWnd->OnCommCommand(CMD_COM,300);
		//注意:由于周期的数据正在处理,此处的消息可能发不出去
		if(nPeriod!=g_GH.nDefPeriod)
			pMainWnd->OnCommCommand(CMD_COM,nMessageCode);
	}
	else
		pMainWnd->OnCommCommand(CMD_COM,501);
	*/
	if(m_nF>=0)
	{
		if(m_Units.Type[m_nF]==FXT_UNIT)
			PostMessage(WM_COMMAND, ID_INSERTZST_BIG);
		else if(m_Units.Type[m_nF]==ZST_UNIT)
			PostMessage(WM_COMMAND, ID_INSERTFXT_BIG);
	}
}

int	CComView2::GetNextGP(int nGPIndex, BOOL bNext)
{
	int nGPVar = m_Units.Var_GPIndex[m_nF] - GPINDEX_VAR1, i=0;
	if(nGPVar<0) return 0;
	if(m_aLastCUnit[nGPVar]>=0 && m_aLastCUnit[nGPVar]<MAX_COM2_GP &&
		m_Units.Type[m_aLastCUnit[nGPVar]]==HQGRID_UNIT)		//优先从该unit找，如果他是ufxt的话
	{
		HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[m_aLastCUnit[nGPVar]]);
		return pHQGrid->GetNextGP(nGPIndex, bNext);
	}
	int iii = -1;
	FOR_UNIT(i)
	{
		if(m_Units.pUnit[i] && TestUnitProperty(i, BE_VALID_UNIT|BE_NOHIDE_UNIT|BE_NONULL_UNIT|
				BE_NOCHANGEGP_UNIT|BE_GPNOINAC_UNIT))
		{
			if(iii<0) iii = i;
			if(TestUnitProperty(i, BE_CONTRALL_UNIT))
			{
				iii = i;
				break;
			}
		}
	}
	if(iii>=0)			//从unit中找
	{
		if(m_Units.Type[iii]==HQGRID_UNIT)
		{
			HQGrid* pHQGrid = (HQGrid*)(m_Units.pUnit[iii]);
			return pHQGrid->GetNextGP(nGPIndex, bNext);
		}
	}
	int allgp = g_pStockIO->GetStockNum(ALL_ALL);
	int nAdd = (bNext?1:-1);
	return ((nGPIndex+nAdd)+allgp)%allgp;
}

void CComView2::OnCancelMark()
{
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	MarkStockFlag(tempStkInfo, 0);
}

#include "TipMarkDlg.h"
void CComView2::OnMarkStockFlag(UINT nID)
{
	int i, nFlag = nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	if(nFlag==MAX_GROUPMARK)
	{
		CTipMarkDlg dlg;
		dlg.m_pStock = tempStkInfo;
		if(dlg.DoModal()==IDOK)
			MarkStockTipFlag(tempStkInfo, dlg.m_strTip);
	}
	else MarkStockFlag(tempStkInfo, (char)nFlag);
	FOR_UNIT(i)
	{
		if(m_Units.Type[i]==HQ_UNIT) 
			m_Units.pUnit[i]->ProcessMsg(WM_PAINT);
	}
}

void CComView2::OnUpdateMarkStockFlag(CCmdUI* pCmdUI)
{
	char nFlag = pCmdUI->m_nID - ID_MARK_ADD1+1;
	if(nFlag<=0||nFlag>10) return;
	int nCurGpIndex = GetCurrentGPIndex();
	if(nCurGpIndex<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[nCurGpIndex];
	if(!tempStkInfo) return;
	pCmdUI->SetCheck(nFlag==tempStkInfo->fl_flag);
}
