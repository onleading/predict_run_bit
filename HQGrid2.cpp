#include "stdafx.h"
#include "HQGrid.h"
#include "TdxW.h"
#include "MainFrm.h"
#include "GridView.h"
#include "MyFileFunc.h"
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

void HQGrid::InitDynaMap()
{
	m_DynaIndex.clear();
	m_DynaIndex["$ZQDM"]=DynaHQItem(ZQDM, "����");
	m_DynaIndex["$ZQJC"]=DynaHQItem(ZQJC, "����");
	m_DynaIndex["$ZRSP"]=DynaHQItem(ZRSP, "����");
	m_DynaIndex["$JRKP"]=DynaHQItem(JRKP, "��");
	m_DynaIndex["$ZGCJ"]=DynaHQItem(ZGCJ, "���");
	m_DynaIndex["$ZDCJ"]=DynaHQItem(ZDCJ, "���");
	m_DynaIndex["$ZJCJ"]=DynaHQItem(ZJCJ, "�ּ�");
	m_DynaIndex["$ZGJM"]=DynaHQItem(ZGJM, "�����");
	m_DynaIndex["$ZDJM"]=DynaHQItem(ZDJM, "������");
	m_DynaIndex["$CJL"]=DynaHQItem(CJL, "����");
	m_DynaIndex["$XS"]=DynaHQItem(XS, "����");
	m_DynaIndex["$QRSD"]=DynaHQItem(QRSD, "���ǵ�");
	m_DynaIndex["$ZAF"]=DynaHQItem(ZAF, "�Ƿ�%%");
	m_DynaIndex["$ZEF"]=DynaHQItem(ZEF, "���");
	m_DynaIndex["$JUNJ"]=DynaHQItem(JUNJ, "����");
	m_DynaIndex["$SYL"]=DynaHQItem(SYL, "��ӯ(��)");
	m_DynaIndex["$WTB"]=DynaHQItem(WTB, "ί��");
	m_DynaIndex["$LP"]=DynaHQItem(LP, "����");
	m_DynaIndex["$WP"]=DynaHQItem(WP, "����");
	m_DynaIndex["$LWB"]=DynaHQItem(LWB, "�����");
	m_DynaIndex["$WLC"]=DynaHQItem(WLC, "ί����");

	m_DynaIndex["$LIANGB"]=DynaHQItem(LIANGB, "����");
	m_DynaIndex["$J_HSL"]=DynaHQItem(J_HSL, "������");
	m_DynaIndex["$J_LTGB"]=DynaHQItem(J_LTGB, "��ͨ�ɱ�");
	m_DynaIndex["$J_LTSZ"]=DynaHQItem(J_LTSZ, "��ͨ��ֵ");
	m_DynaIndex["$J_ZSZ"]=DynaHQItem(J_ZSZ, "AB������ֵ");
	m_DynaIndex["$DKPH"]=DynaHQItem(DKPH, "���ƽ��");
	m_DynaIndex["$DTHL"]=DynaHQItem(DTHL, "��ͷ����");
	m_DynaIndex["$DTZS"]=DynaHQItem(DTZS, "��ͷֹ��");
	m_DynaIndex["$KTHB"]=DynaHQItem(KTHB, "��ͷ�ز�");
	m_DynaIndex["$KTZS"]=DynaHQItem(KTZS, "��ͷֹ��");
	m_DynaIndex["$QRD"]=DynaHQItem(QRD, "ǿ����");

	m_DynaIndex["$ZANGSU"]=DynaHQItem(ZANGSU, "����%%");
	m_DynaIndex["$HYD"]=DynaHQItem(HYD, "��Ծ��");
	m_DynaIndex["$MBZL"]=DynaHQItem(MBZL, "ÿ�ʾ���");
	m_DynaIndex["$MBHSL"]=DynaHQItem(MBHSL, "ÿ�ʻ���");

	m_DynaIndex["$J_START"]=DynaHQItem(J_START, "��������");
	m_DynaIndex["$J_ZGB"]=DynaHQItem(J_ZGB, "�ܹɱ�");
	m_DynaIndex["$J_BG"]=DynaHQItem(J_BG, "B��");
	m_DynaIndex["$J_HG"]=DynaHQItem(J_HG, "H��");
	m_DynaIndex["$J_ZZC"]=DynaHQItem(J_ZZC, "���ʲ�(��Ԫ)");
	m_DynaIndex["$J_LDZC"]=DynaHQItem(J_LDZC, "�����ʲ�");
	m_DynaIndex["$J_GDZC"]=DynaHQItem(J_GDZC, "�̶��ʲ�");
	m_DynaIndex["$J_WXZC"]=DynaHQItem(J_WXZC, "�����ʲ�");
	m_DynaIndex["$J_CQTZ"]=DynaHQItem(J_CQTZ, "����Ͷ��");
	m_DynaIndex["$J_LDFZ"]=DynaHQItem(J_LDFZ, "������ծ");
	m_DynaIndex["$J_CQFZ"]=DynaHQItem(J_CQFZ, "���ڸ�ծ");
	m_DynaIndex["$J_ZBGJJ"]=DynaHQItem(J_ZBGJJ, "�ʱ�������");
	m_DynaIndex["$J_JZC"]=DynaHQItem(J_JZC, "���ʲ�");
	m_DynaIndex["$J_ZYSY"]=DynaHQItem(J_ZYSY, "��Ӫ����");
	m_DynaIndex["$J_ZYLY"]=DynaHQItem(J_ZYLY, "��Ӫ����");
	m_DynaIndex["$SJL"]=DynaHQItem(SJL, "��ӯ(��)");
	m_DynaIndex["$J_YYLY"]=DynaHQItem(J_YYLY, "Ӫҵ����");
	m_DynaIndex["$J_TZSY"]=DynaHQItem(J_TZSY, "Ͷ������");
	m_DynaIndex["$J_JYXJL"]=DynaHQItem(J_JYXJL, "��Ӫ�ֽ�����");
	m_DynaIndex["$J_ZXJL"]=DynaHQItem(J_ZXJL, "���ֽ�����");
	m_DynaIndex["$J_CH"]=DynaHQItem(J_CH, "���");
	m_DynaIndex["$J_LYZE"]=DynaHQItem(J_LYZE, "�����ܶ�");
	m_DynaIndex["$J_SHLY"]=DynaHQItem(J_SHLY, "˰������");
	m_DynaIndex["$J_JLY"]=DynaHQItem(J_JLY, "������");
	m_DynaIndex["$J_WFPLY"]=DynaHQItem(J_WFPLY, "δ��������");
	m_DynaIndex["$J_TZMGJZ"]=DynaHQItem(J_TZMGJZ, "����ÿ�ɾ��ʲ�");
	m_DynaIndex["$J_MGWFP"]=DynaHQItem(J_MGWFP, "ÿ��δ����");
	m_DynaIndex["$J_MGSY"]=DynaHQItem(J_MGSY, "ÿ������");
	m_DynaIndex["$J_MGGJJ"]=DynaHQItem(J_MGGJJ, "ÿ�ɹ�����");
	m_DynaIndex["$J_MGJZC"]=DynaHQItem(J_MGJZC, "ÿ�ɾ��ʲ�");
	m_DynaIndex["$J_GDQYB"]=DynaHQItem(J_GDQYB, "�ɶ�Ȩ���");
}

void HQGrid::GetDynaInfo(char *strIniName, char *strShowName, UINT &nDynaID)
{
	map<char*, DynaHQItem, cmpstrs>::iterator iter = m_DynaIndex.find(strIniName);
	if(iter!=m_DynaIndex.end()) 
	{
		DynaHQItem dhi = (*iter).second;
		nDynaID = dhi.DynaID;
		strcpy(strShowName, dhi.Name);
	}
	else
	{
		strcpy(strShowName, strIniName);
		nDynaID = -1;
	}
}

int HQGrid::GetCodeIndex(int nBaseRow)
{
	if(m_nGPCode==-1)
		return -1;
	int nIndex = -1;
	char lpString[MAX_FYONE_LEN]={0}, *argv[MAX_FYCOL];
	strncpy(lpString, (LPCSTR)m_vBaseRowStr[nBaseRow], MAX_FYONE_LEN-1);
	int argc = GetCmdLine(lpString,argv,MAX_FYCOL,"|");
	char ncs[2] = {0};
	ncs[0] = argv[m_nGPCode][0];
	short nSetCode = atoi((const char *)ncs);
	char Code[SH_CODE_LEN+1]={0};
	strncpy(Code, &argv[m_nGPCode][1], SH_CODE_LEN);
	nIndex = g_pStockIO->GetIndex(Code, nSetCode);
	return nIndex;
}

BOOL HQGrid::ParseHeadInfo(ColHeadInfo &colinfo,char *colstr)
{
	colinfo.calcType = CALC_STATIC;
	colinfo.dynaID = -1;
	strcpy(colinfo.strTitle, "NULL");
	colinfo.width = DEF_ITEM_WIDTH;
	colinfo.dataType = TYPE_STRING;
	colinfo.strFormula="NULL";

	char	*argv[4];
	int argc = GetCmdLine(colstr,argv,4,"&");
	if(argc!=4)		//�����ʽ
	{
		return FALSE;
	}
	//��������
	switch(argv[0][0])  
	{
	case '$':
		colinfo.calcType = CALC_DYNA_TXT;
		GetDynaInfo(argv[0], colinfo.strTitle, colinfo.dynaID);
		break;
	case '^':
		break;
	default:
		colinfo.calcType = CALC_STATIC;
		colinfo.dynaID = -1;
		colinfo.strFormula=argv[0];
		strcpy(colinfo.strTitle,argv[1]);
		break;
	}
	//��������Ϣ	
	char DataType = argv[2][0];
	switch(DataType) 
	{
	case 'F':
		colinfo.dataType = TYPE_FLOAT;
		break;
	case 'D':
		colinfo.dataType = TYPE_INT;
		break;
	case 'S':
	default:
		colinfo.dataType = TYPE_STRING;
		break;
	}
	//�����Ϣ
	int w=atoi(argv[3])+1;
	if(w>120) 
		w=strlen(colinfo.strTitle)+1;
	colinfo.width = w;
	return TRUE;
}

const char strSubSep[] = {'#','$','#'};
void HQGrid::InitFYInfo()
{
	m_LoadFyOK=FALSE;
	if(!g_bFYDataAlwaysSync && !g_bDebugMode)
	{
		g_nDataSyncType=DATASYNC_TEND;
		AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_DATASYNC);
	}
	m_nGPCode=-1;
	int nGroup = m_WhichGPType-FY_START;
	int m_nSubReport = m_nBlockFlag+1;
	if(g_vfyGroup[nGroup].nType==1)
		return;
	char srrMsg[128]={0};
	char strFileName[MAX_PATH]={0};
	sprintf(strFileName, "%s%02d.dat",g_strTendName,g_vfyGroup[nGroup].nIndex_file);
	CString	Ini = g_WSXHStr+"LData\\cache\\"+(CString)strFileName;	
	CFile theFile;
	if(!theFile.Open(Ini,CFile::modeRead|CFile::shareDenyWrite))
		return;
	long filesize=theFile.GetLength();
	byte *fbuf=new byte[filesize];
	if(!fbuf)
	{
		theFile.Close();
		lstrcpy(srrMsg,"�ڴ治��!");
		TDX_MessageBox(m_pView->m_hWnd,srrMsg,MB_ICONEXCLAMATION|MB_OK);
		return;
	}
	long nRead=theFile.ReadHuge(fbuf,filesize);
	theFile.Close();
	if(nRead==0)
	{
		TDEL(fbuf);
		return;
	}
	byte *buf=NULL;
	long bufsize=0;
	if(g_vfyGroup[nGroup].bEncrypted)
	{	//����Ǽ����ļ�,�����
		BYTE key[]=TEA_KEY;
		if(!g_FF.mTEA_decipher(fbuf,filesize,buf,bufsize,key,TEA_KEY_LEN))
		{
			TDEL(fbuf);
			lstrcpy(srrMsg,"���ݴ���ʧ��");
			TDX_MessageBox(m_pView->m_hWnd,srrMsg,MB_ICONEXCLAMATION|MB_OK);
			return;
		}
		TDEL(fbuf);
	}
	else
	{
		buf=fbuf;
		bufsize=filesize;
	}

	char	lpString[MAX_FYONE_LEN],*argv[MAX_FYCOL];
	
	m_vBaseHeads.clear();
	m_vBaseRowStr.clear();
	for(int i=0;i<MAXGPNUM;i++)
		lpnUsedStkIndex[i] = -1;
	
	int nStarPos = -1, nEndPos = -1;
	int nSubGrid = -1;

	istrstream istr((const char*)buf,bufsize);
	for(i=0;istr;++i)
	{
		memset(lpString,0,MAX_FYONE_LEN);
		istr.getline(lpString,MAX_FYONE_LEN-1);

		if(strncmp(lpString, strSubSep, sizeof(strSubSep))==0) 
			++nSubGrid;		//����һ������
		if(nStarPos<1&&nSubGrid>=0&&nSubGrid==m_nSubReport-1)
		{
			nStarPos = i+1;
			continue;
		}
		if(nStarPos>0&&nSubGrid>m_nSubReport-1) break;
		if(i==nStarPos)		//��ȡ��������
		{
			int argc = GetCmdLine(lpString,argv,MAX_FYCOL,"|");
			if(argc==0)
			{
				TDEL(buf);
				lstrcpy(srrMsg,"��������Ϣ");
				TDX_MessageBox(m_pView->m_hWnd,srrMsg,MB_ICONEXCLAMATION|MB_OK);
				return;
			}
			for(int j=0;j<argc;j++)
			{
				char strTmp[128]={0};
				strncpy(strTmp, argv[j], 127);
				ColHeadInfo chi;
				if(!ParseHeadInfo(chi, strTmp))
				{
					CString tmpstr;
					tmpstr.Format("�����ļ���%d�������д���!",i+1);
					lstrcpy(srrMsg,tmpstr);
					TDX_MessageBox(m_pView->m_hWnd,srrMsg,MB_ICONEXCLAMATION|MB_OK);
					break;
				}
				m_ItemTextFormat[j]=(chi.dataType==TYPE_STRING)?0:2;
				if(chi.dynaID==ZQDM)
					m_nGPCode=j;
				chi.nHeadID = j;
				
				m_vBaseHeads.push_back(chi);

				if(m_vBaseHeads.size()>=TOTAL_COL)
				{
					lstrcpy(srrMsg,"������������!");
					TDX_MessageBox(m_pView->m_hWnd,srrMsg,MB_ICONEXCLAMATION|MB_OK);
					break;
				}
			}
		}
		if(nStarPos>0&&i>nStarPos)		//��ȡ������
		{
			CString strRow(lpString);
			TrimString(strRow);
			if(strRow.GetLength()>0)
			{
				if(m_nGPCode>=0)
				{
					CString tmpstr=strRow;
					char *argv[MAX_FYCOL];
					int argc = GetCmdLine(tmpstr.GetBuffer(MAX_FYONE_LEN),argv,MAX_FYCOL,"|");
					if(m_nGPCode>=argc)
						continue;
					int ncode=argv[m_nGPCode]-argv[0];
				
					MemStkInfo *pInfo=g_pStockIO->Get_MemStkInfo(argv[m_nGPCode]);
					if(pInfo)
					{
						strRow.Insert(ncode,pInfo->setcode+'0');
						tmpstr.ReleaseBuffer();
					}
					else
					{
						tmpstr.ReleaseBuffer();
						//����Ǵ�����,���˹�Ʊ������,�������
						continue;
					}	
				}
				m_vBaseRowStr.push_back(strRow);
				int nBaseRowXH = m_vBaseRowStr.size()-1;
				int nIndex = GetCodeIndex(nBaseRowXH);
				if(nIndex<0) 
					nIndex = -1 - nBaseRowXH;
				lpnUsedStkIndex[nBaseRowXH] = nIndex;
			}
		}
	}
	TDEL(buf);
	nTotalRow = m_vBaseRowStr.size();
	//�õ�m_lpnCol
	int nTotalHeads=m_vBaseHeads.size();
	for(i=0;i<nTotalHeads;i++)
	{
		switch(m_vBaseHeads[i].calcType)  
		{
		case CALC_DYNA_TXT:
			m_FYCol[i] = m_vBaseHeads[i].dynaID;
			break;
		case CALC_STATIC:
			m_FYCol[i] = m_vBaseHeads[i].dynaID = TOTAL_COL+i;
			break;
		case CALC_DYNA_NUM:	//CALC_DYNA_NUM�����ݲ�֧��
			break;
		}
	}
	m_LoadFyOK=TRUE;
}

void HQGrid::GetString(char *strValue, int maxlen,int nBaseRow, int nBaseCol)
{
	char lpString[MAX_FYONE_LEN]={0}, *argv[MAX_FYCOL];
	strncpy(lpString, (LPCSTR)m_vBaseRowStr[nBaseRow], MAX_FYONE_LEN-1);
	int argc = GetCmdLine(lpString,argv,MAX_FYCOL,"|");
	if(argc>nBaseCol)
		strncpy(strValue, argv[nBaseCol], maxlen);
	else strcpy(strValue, "");
}

float HQGrid::fGetFyValue(int nBaseRow, int nBaseCol)
{
	char lpString[MAX_FYONE_LEN]={0}, *argv[MAX_FYCOL];
	strncpy(lpString, (LPCSTR)m_vBaseRowStr[nBaseRow], MAX_FYONE_LEN-1);
	int argc = GetCmdLine(lpString,argv,MAX_FYCOL,"|");
	if(argc>nBaseCol)
		return atof(argv[nBaseCol]);
	else return MEANLESS_DATA;
}

BOOL HQGrid::GetFYTxt(LPCSTR infoFile,int nIndex,CString &strInfo)
{
	strInfo="";
	if(!TestFileExist(infoFile) || m_WhichGPType<FY_START) 
		return FALSE;
	CString f10file=infoFile;
	char srrMsg[128]={0};
	if(g_vfyGroup[m_WhichGPType-FY_START].bEncrypted)
	{	//����Ǽ����ļ�,�����������ʱ�ļ�
		CString tmpini;
		GetTempPath(MAX_PATH,tmpini.GetBuffer(MAX_PATH));
		tmpini.ReleaseBuffer();
		tmpini+="TopView\\";
		g_FF.MakeSureDirectoryPathExists(tmpini);
		char MDStr[33]={0};
		MD5_Buffer((BYTE*)infoFile,strlen(infoFile),MDStr);
		CString tmp=MDStr;
		tmp.MakeUpper();
		tmp+=".tmp";
		tmpini+=tmp;
		BYTE key[]=TEA_KEY;
		if(!g_FF.fTEA_decipher(infoFile,tmpini,key,TEA_KEY_LEN))
		{
			lstrcpy(srrMsg,"���ݴ���ʧ��");
			TDX_MessageBox(m_pView->m_hWnd,srrMsg,MB_ICONEXCLAMATION|MB_OK);
			return FALSE;
		}
		f10file=tmpini;
	}

	CFile cf;
	if(!cf.Open(f10file,CFile::modeRead|CFile::shareDenyWrite))
		return FALSE;
	int filelen=cf.GetLength();
	if(filelen>0)
	{
		char *buf = new char[filelen+100];
		char *buf1 = new char[filelen+100];
		cf.ReadHuge(buf,filelen);
		buf[filelen]=0;
		ParseMessageStr(buf,buf1,"#$#",nIndex+2);
		buf1[filelen]=0;
		strInfo=buf1;
		delete buf1;
		delete buf;
	}
	cf.Close();
	if(g_vfyGroup[m_WhichGPType-FY_START].bEncrypted)
		DeleteFile(f10file);
	return TRUE;
}

//�ڸ������滭ͼ��
void HQGrid::DrawImage(CDC *pDC,int row,int col,int iIndex)
{
	int width = 0;
	int x = GetCellLeft(col,width);
	//ĳЩ�в�������˴���
	if(width<1) return;
	int nLeftPos  = x;
	if(x+width >= m_DRect.Width()+3) //���ڳ������Ĳ���,����
		return;
	
	int y = nTitleHeight + row*nCellHeight;
	g_GridImage.Draw(pDC,iIndex,CPoint(nLeftPos+width/2-8,y),ILD_NORMAL);
}

//�ڸ������滭ͼ��
void HQGrid::DrawImage2(CDC *pDC,int row,int col,int iIndex1,int iIndex2)
{
	int width = 0;
	int x = GetCellLeft(col,width);
	//ĳЩ�в�������˴���
	if(width<1) return;
	int nLeftPos  = x;
	if(x+width >= m_DRect.Width()+3) //���ڳ������Ĳ���,����
		return;
	
	int y = nTitleHeight + row*nCellHeight;
	if(iIndex1>=0)
		g_GridImage.Draw(pDC,iIndex1+4,CPoint(nLeftPos+20,y),ILD_NORMAL);
	if(iIndex2>=0)
		g_GridImage.Draw(pDC,iIndex2+4,CPoint(nLeftPos+width/2+6,y),ILD_NORMAL);
}

//PosFlag��ʾ�ڷ�����д��λ�ã�0:����,1:ƫ��,2:ƫ��,PosFlag2��ʾ���ֶ��뷽ʽ,0:��,1:����,2:��
void HQGrid::DrawCellStringText(CDC *pDC,int row,int col,int PosFlag,int PosFlag2,COLORREF color,BOOL bTranBig5,char *fmt,...)
{
    va_list arg_ptr;
    char tmp[256];	//���֧��255���ַ�
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
	CString tmpstr=tmp;
	tmpstr.TrimLeft();
	tmpstr.TrimRight();

	int width = 0;
	int x = GetCellLeft(col,width);
	//ĳЩ�в�������˴���
	if(width<1) return;
	int nLeftPos  = x;
	if(x+width >= m_DRect.Width()+3) //���ڳ������Ĳ���,����
		return;

	UINT nFormat1,nFormat2;
	switch(PosFlag)  
	{
	case 0:
	    nFormat1=DT_VCENTER;
		break;
	case 1:
		nFormat1=DT_TOP;
	    break;
	case 2:
		nFormat1=DT_BOTTOM;
		break;
	}
	switch(PosFlag2)  
	{
	case 0:
		nFormat2=DT_LEFT;
		break;
	case 1:
		nFormat2=DT_CENTER;
		break;
	case 2:
		nFormat2=DT_RIGHT;
		break;
	}
	int y=nTitleHeight + row*nCellHeight;
	int oldmode=pDC->SetBkMode(TRANSPARENT);
	COLORREF oldcor= pDC->SetTextColor(color);
	if(bTranBig5)
		pDC->DrawText(_F(tmpstr),CRect(nLeftPos+4,y,nLeftPos+width-4,y+nCellHeight),DT_SINGLELINE|nFormat1|nFormat2);
	else
		pDC->DrawText(tmpstr,CRect(nLeftPos+4,y,nLeftPos+width-4,y+nCellHeight),DT_SINGLELINE|nFormat1|nFormat2);
	pDC->SetTextColor(oldcor);
	pDC->SetBkMode(oldmode);
}

void HQGrid::TransID2Str(UINT nID,MemStkInfo *pInfo,CurrStockData * hqp,EXCURRDATA *hqpex,LPDDEREPORT pDDEReport,FormatInfo &fi,float &float_out,CString & rs,CString &e_rs)
{
	long			wl, wlc;
	short			ii;
	float			ftmp;
	int				nColItem = nID;
	float			fStatData=MEANLESS_DATA;
	rs=MEANLESS_STR; e_rs="";
	float_out=.0f;
	if(nColItem>=TOTAL_COL||!pInfo)	//�Ƕ�̬����������δȷ���Ķ�̬����
		return ;
	if(nColItem != J_START && nColItem >= J_GXRQ && nColItem <= J_TZMGJZ)
	{
		if( !testzs(pInfo) )
		{
			if(nColItem == J_GXRQ)
				rs.Format("%ld",pInfo->J_gxrq);
			else if(nColItem == SJL) //�о���
			{
				float jly = 0.0f,tmpjz=pInfo->J_tzmgjz;
				if(pInfo->J_zgb > 1 && fabs(pInfo->J_jzc) > COMPPREC) 
					tmpjz=pInfo->J_jzc/pInfo->J_zgb;
				if(tmpjz>COMPPREC) 
				{
					if(hqp->Now>COMPPREC)
						jly = hqp->Now/tmpjz;
					else if(hqp->Close>COMPPREC)
						jly = hqp->Close/tmpjz;
					else if(pInfo->Close>COMPPREC)
						jly = pInfo->Close/tmpjz;
				}
				float_out=jly;
				rs.Format("%14.2f",float_out);
			}
			else
			{
				float_out=*((float *)&pInfo->J_gxrq+(nColItem-J_GXRQ));
				rs.Format("%14.2f",float_out);
			}
		}
		if(e_rs.GetLength()<1) e_rs = rs;
		return;
	}
	
	TICKSTAT its=pInfo->its;
	if(m_bJSZJStatus&&its.Minute==0)
		ReadDataTickStat(pInfo, &its);

	switch (nColItem)
	{
	case ZANGSU:	//��������
		if(OnLine && CanShowZangsu(pInfo) && fabs(pInfo->zangsu)>COMPPREC)
			ftmp = pInfo->zangsu;
		else		
			ftmp = MEANLESS_DATA;
		if(fabs(ftmp - MEANLESS_DATA)>COMPPREC && ftmp!=MEANLESS_DATA && hqp->Now>COMPPREC)
		{
			fi.color=GetItemColor(ftmp,.0);
			rs.Format("%14.2f",ftmp);
			float_out=ftmp;
		}
		else
			rs=MEANLESS_STR;
		break;
	case ZQDM:		//��Ʊ����
		fi.color=GridColor.Grid_CodeName_Color;
		fi.h_align=0;
		rs=pInfo->Code;
		break;
	case SPELL_CODE://��Ʊ���
		fi.color=GridColor.Grid_CodeName_Color;
		fi.h_align=0;
		rs=pInfo->SpellCode;
		break;
	case ZQJC:		//��Ʊ����
		fi.color=GetBlockGPColor(pInfo,GridColor.Grid_CodeName_Color,m_WhichGPType,m_nBlockFlag);
		fi.h_align=0;
		rs=pInfo->Name;
		break;
	case ZRSP:  //��������
		rs=FormatFloat2Str(hqp->Close,fi.xsflag);
		float_out=hqp->Close;
		break;
	case JRKP:  //���տ���
		if(hqp->Open>COMPPREC)
		{
			fi.color=GetItemColor(hqp->Open,hqp->Close);
			rs=FormatFloat2Str(hqp->Open,fi.xsflag);
		}
		else
			rs=MEANLESS_STR;
		float_out=hqp->Open;
		break;
	case ZGCJ:  //���
		if(hqp->Max>COMPPREC)
		{
			fi.color=GetItemColor(hqp->Max,hqp->Close);
			rs=FormatFloat2Str(hqp->Max,fi.xsflag);
		}
		else 
			rs=MEANLESS_STR;
		float_out=hqp->Max;
		break;
	case ZDCJ:  //���
		if(hqp->Min>COMPPREC)
		{
			fi.color=GetItemColor(hqp->Min,hqp->Close);
			rs=FormatFloat2Str(hqp->Min,fi.xsflag);
		}
		else
			rs=MEANLESS_STR;
		float_out=hqp->Min;
		break;
	case ZJCJ:  //��ǰ�ּ�  ������
		if(hqp->Now>COMPPREC)
			fi.color=GetItemColor(hqp->Now,hqp->Close);
		rs=FormatFloat2Str(hqp->Now,fi.xsflag);
		float_out=hqp->Now;
		break;
	case ZGJM:  //����
		if( !testzs(pInfo) && hqp->Buyp[0] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Buyp[0],hqp->Close);
			rs=FormatFloat2Str(hqp->Buyp[0],fi.xsflag);
			float_out=hqp->Buyp[0];
		}
		else
			rs=MEANLESS_STR;
		break;
	case ZDJM:  //����
		if( !testzs(pInfo) && hqp->Sellp[0] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Sellp[0],hqp->Close);
			rs=FormatFloat2Str(hqp->Sellp[0],fi.xsflag);
			float_out=hqp->Sellp[0];
		}
		else
			rs=MEANLESS_STR;
		break;
	case CJL:   //�ɽ���
		fi.color=GridColor.Grid_Volume_Color;
		rs.Format("%s",MakeVol3(hqp->Volume));
		e_rs.Format("%d", hqp->Volume);
		float_out=(float)hqp->Volume;
		break;
	case QRSD:  //���ǵ�
		ftmp=hqp->Now;
		//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
		if(ftmp<COMPPREC && hqp->Buyp[0]>COMPPREC && fabs(hqp->Buyp[0]-hqp->Sellp[0])<COMPPREC)
			ftmp=hqp->Buyp[0];
		if(ftmp>COMPPREC)
		{
			ftmp = (ftmp - hqp->Close);
			fi.color=GetItemColor(ftmp,.0);
			rs=FormatFloat2Str(ftmp,fi.xsflag);
		}
		else 
			rs=MEANLESS_STR;
		float_out=ftmp;
		break;
	case XS:    //����
		if( !testzs(pInfo) )
		{
			if(pInfo->setcode<2 && IsHligtNowVol(hqp->NowVol,pInfo)) //������g_nHligtNowVol�ֵĹ�Ʊ�ñ���ɫ�ķ�ʽ��ʾһ��(������AB��)
				fi.color=RGB(192,128,192);
			else
				fi.color=(hqp->Flag.InOutFlag==0)?(GridColor.Grid_Up_Color):((hqp->Flag.InOutFlag==1)?GridColor.Grid_Down_Color:GridColor.Grid_Level_Color);
			rs.Format("%s", MakeVol3(hqp->NowVol));
			e_rs.Format("%d", hqp->NowVol);
		}
		else
			rs=MEANLESS_STR;
		break;
	case CJJE:  //�ɽ����
		if(pInfo->bigmarket!=QH_TYPE && hqp->Amount>COMPPREC)
		{
			fi.color=GridColor.Grid_Other_Color;
			rs=MakeJE(hqp->Amount);
			e_rs.Format("%.f", hqp->Amount);
			float_out=hqp->Amount;
		}
		else
			rs=MEANLESS_STR;
		break;
	case LP:  //����
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Down_Color;
			rs.Format("%s",MakeVol3(hqp->Volume-hqp->Outside));
			e_rs.Format("%d",hqp->Volume-hqp->Outside);
			float_out=hqp->Volume-hqp->Outside;
		}
		else
			rs=MEANLESS_STR;
		break;
	case WP:  //����
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Up_Color;
			rs.Format("%s",MakeVol3(hqp->Outside));
			e_rs.Format("%d",hqp->Outside);
			float_out=hqp->Outside;
		}
		else
			rs=MEANLESS_STR;
		break;
	case LWB: //����� = ����/����*100%
		if( !testzs(pInfo) && hqp->Outside != 0)
		{
			fi.color=VipColor.TXTCOLOR;
			float_out=(float)(hqp->Volume-hqp->Outside)/(float)(hqp->Outside);
			rs.Format("%6.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case WLC: //ί���� = �������֮��-�������֮��
		if( !testzs(pInfo) )
		{
			wlc = 0;
			for (ii = 0;ii < 5;ii++)
				wlc = wlc+(long)hqp->Buyv[ii]-(long)hqp->Sellv[ii];
			fi.color=VipColor.TXTCOLOR;
			rs.Format("%s",MakeVol3(wlc));
			e_rs.Format("%d",wlc);
			float_out=(float)wlc;
		}
		else
			rs=MEANLESS_STR;
		break;
	case WTB: //ί�б�%
		if( !testzs(pInfo) )
		{
			wlc = wl = 0;
			for (ii = 0;ii < 5;ii++)
			{   
				wlc = wlc+(long)hqp->Buyv[ii]-(long)hqp->Sellv[ii];
				wl = wl +(long)hqp->Buyv[ii]+(long)hqp->Sellv[ii];
			}
			if (wl != 0l)
			{
				fi.color=GetItemColor((float)100.0*wlc/wl,.0);
				float_out=100.0*wlc/wl;
				rs=FormatFloat2Str(float_out,XS2);
				break;
			}
		}
		rs=MEANLESS_STR;
		break;
	case ZAF: //�Ƿ���=���ǵ�/��������*100
		if (hqp->Close>COMPPREC)
		{	
			ftmp=hqp->Now;
			//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
			if(ftmp<COMPPREC && hqp->Buyp[0]>COMPPREC && fabs(hqp->Buyp[0]-hqp->Sellp[0])<COMPPREC)
				ftmp=hqp->Buyp[0];
			if(ftmp>COMPPREC)
			{
				ftmp = ftmp - hqp->Close;
				fi.color=GetItemColor(ftmp/hqp->Close*(float)100.0,.0);
				float_out=ftmp/hqp->Close*(float)100.0;
				rs=FormatFloat2Str(float_out,XS2);
				break;
			}
		}
		rs=MEANLESS_STR;
		break;
	case ZEF: //�����=(���-���)/��������*100
		if (hqp->Close>COMPPREC && hqp->Now>COMPPREC)
		{
			if ( hqp->Max > COMPPREC && hqp->Min > COMPPREC )
				ftmp = hqp->Max - hqp->Min;
			else
				ftmp = 0.0;
			fi.color=VipColor.TXTCOLOR;
			float_out=ftmp/hqp->Close*100.0;
			rs.Format("%5.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case SYL:
		if( pInfo->setcode>1)
		{
			if(pInfo->bigmarket!=QH_TYPE) break;
			ftmp = hqp->Yield_VolInStock.VolInStock;
			if(ftmp<0) ftmp=0.0;
			if(ftmp>COMPPREC)
			{
				fi.color=VipColor.TXTCOLOR;
				rs.Format("%9.0f",ftmp);
				float_out=ftmp;
				break;
			}
		}
		else	if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			ftmp = (pInfo->J_jly*12/pInfo->J_HalfYearFlag)/(pInfo->J_zgb);
			if(ftmp > COMPPREC && (hqp->Now > COMPPREC || hqp->Close > COMPPREC))
			{
				if(hqp->Now > COMPPREC)
					ftmp = hqp->Now/ftmp;
				else
					ftmp = hqp->Close/ftmp;
				fi.color=VipColor.TXTCOLOR;
				rs.Format("%9.2f",ftmp);
				float_out=ftmp;
				break;
			}
		}
		rs=MEANLESS_STR;
		break;
	case JUNJ: //����
		{
			float tmpf=cal_averagetmp(pInfo,hqp);
			fi.color=GetItemColor(tmpf,hqp->Close);
			rs=FormatFloat2Str(tmpf,fi.xsflag);
			float_out=tmpf;
		}
		break;
	case LIANGB:  //����
		if( !testzs(pInfo) )
		{
			if(pInfo->setcode>1)
			{
				PTIMEINF pTime = g_pStockIO->DSGetTimeInf(pInfo->TimerID);
				BOOL  bYestodayFalg = (hqpex->HqDate!=GetDSCorrespondingDate(pTime));
				ii = GetDSMinute(pTime,bYestodayFalg);
			}
			else
			{
				short fz[8];
				InitStkTime(pInfo->setcode,fz);
				ii = GetTdxMinute(fz);
			}
			if (ii > 0 && pInfo->VolBase > 0l)
			{
				fi.color=VipColor.TXTCOLOR;
				float_out=(float)hqp->Volume/ii/pInfo->VolBase;
				rs.Format("%6.2f",float_out);
				break;
			}
		}
		rs=MEANLESS_STR;
		break;
	case BJ1:  //���1
		if( !testzs(pInfo) && hqp->Buyp[0] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Buyp[0],hqp->Close);
			rs=FormatFloat2Str(hqp->Buyp[0],fi.xsflag);
			float_out=hqp->Buyp[0];
		}
		else
			rs=MEANLESS_STR;
		break;
	case BJ2:  //���2
		if( !testzs(pInfo) && hqp->Buyp[1] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Buyp[1],hqp->Close);
			rs=FormatFloat2Str(hqp->Buyp[1],fi.xsflag);
			float_out=hqp->Buyp[1];
		}
		else
			rs=MEANLESS_STR;
		break;
	case BJ3:  //���3
		if( !testzs(pInfo) && hqp->Buyp[2] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Buyp[2],hqp->Close);
			rs=FormatFloat2Str(hqp->Buyp[2],fi.xsflag);
			float_out=hqp->Buyp[2];
		}
		else
			rs=MEANLESS_STR;
		break;
	case SJ1://����1
		if( !testzs(pInfo) && hqp->Sellp[0] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Sellp[0],hqp->Close);
			rs=FormatFloat2Str(hqp->Sellp[0],fi.xsflag);
			float_out=hqp->Sellp[0];
		}
		else
			rs=MEANLESS_STR;
		break;
	case SJ2://���۶�
		if( !testzs(pInfo) && hqp->Sellp[1] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Sellp[1],hqp->Close);
			rs=FormatFloat2Str(hqp->Sellp[1],fi.xsflag);
			float_out=hqp->Sellp[1];
		}
		else
			rs=MEANLESS_STR;
		break;
	case SJ3: //����3
		if( !testzs(pInfo) && hqp->Sellp[2] > COMPPREC)
		{
			fi.color=GetItemColor(hqp->Sellp[2],hqp->Close);
			rs=FormatFloat2Str(hqp->Sellp[2],fi.xsflag);
			float_out=hqp->Sellp[2];
		}
		else
			rs=MEANLESS_STR;
		break;
	case BJL1:  //�����1
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Volume_Color;
			rs.Format("%s",MakeVol3(hqp->Buyv[0]));
			e_rs.Format("%d",hqp->Buyv[0]);
			float_out=(float)hqp->Buyv[0];
		}
		else
			rs=MEANLESS_STR;
		break;
	case BJL2:  //�����2
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Volume_Color;
			rs.Format("%s",MakeVol3(hqp->Buyv[1]));
			e_rs.Format("%d",hqp->Buyv[1]);
			float_out=(float)hqp->Buyv[1];
		}
		else
			rs=MEANLESS_STR;
		break;
	case BJL3: //�����3
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Volume_Color;
			rs.Format("%s",MakeVol3(hqp->Buyv[2]));
			e_rs.Format("%d",hqp->Buyv[2]);
			float_out=(float)hqp->Buyv[2];
		}
		else
			rs=MEANLESS_STR;
		break;
	case SJL1:  //������1
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Volume_Color;
			rs.Format("%s",MakeVol3(hqp->Sellv[0]));
			e_rs.Format("%d",hqp->Sellv[0]);
			float_out=(float)hqp->Sellv[0];
		}
		else
			rs=MEANLESS_STR;
		break;
	case SJL2:  //������2
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Volume_Color;
			rs.Format("%s",MakeVol3(hqp->Sellv[1]));
			e_rs.Format("%d",hqp->Sellv[1]);
			float_out=(float)hqp->Sellv[1];
		}
		else
			rs=MEANLESS_STR;
		break;
	case SJL3: //������3
		if( !testzs(pInfo) )
		{
			fi.color=GridColor.Grid_Volume_Color;
			rs.Format("%s",MakeVol3(hqp->Sellv[2]));
			e_rs.Format("%d",hqp->Sellv[2]);
			float_out=(float)hqp->Sellv[2];
		}
		else
			rs=MEANLESS_STR;
		break;
	case DKPH: // ���ƽ��
		if( !testzs(pInfo) )
		{
			fi.color=GetItemColor(pInfo->tbp,hqp->Close);
			rs=FormatFloat2Str(pInfo->tbp,fi.xsflag);
			float_out=pInfo->tbp;
		}
		else
			rs=MEANLESS_STR;
		break;
	case DTHL: // ��ͷ����
		if( pInfo->dkflag == 1 && !testzs(pInfo) )
		{
			fi.color=GetItemColor(pInfo->leave,hqp->Close);
			rs=FormatFloat2Str(pInfo->leave,fi.xsflag);
			float_out=pInfo->leave;
		}
		else
			rs=MEANLESS_STR;
		break;
	case KTHB: // ��ͷ�ز�
		if(pInfo->dkflag == 0 && !testzs(pInfo) )
		{
			fi.color=GetItemColor(pInfo->leave,hqp->Close);
			rs=FormatFloat2Str(pInfo->leave,fi.xsflag);
			float_out=pInfo->leave;
		}
		else
			rs=MEANLESS_STR;
		break;
	case DTZS: // ��ͷֹ��
		if(pInfo->dkflag == 1  && !testzs(pInfo) )
		{
			fi.color=GetItemColor(pInfo->stoplost,hqp->Close);
			rs=FormatFloat2Str(pInfo->stoplost,fi.xsflag);
			float_out=pInfo->stoplost;
		}
		else
			rs=MEANLESS_STR;
		break;
	case KTZS: // ��ͷֹ��
		if(pInfo->dkflag == 0 && !testzs(pInfo) )
		{
			fi.color=GetItemColor(pInfo->stoplost,hqp->Close);
			rs=FormatFloat2Str(pInfo->stoplost,fi.xsflag);
			float_out=pInfo->stoplost;
		}
		else
			rs=MEANLESS_STR;
		break;
	case QRD: //ǿ����%
		if (hqp->Close>COMPPREC)
		{
			if ( hqp->Now < COMPPREC )	ftmp = 0.0;
			else						ftmp = hqp->Now - hqp->Close;
			if ( g_pStockIO->GetDomain ( pInfo ) == SZ )
			{
				fi.color=VipColor.TXTCOLOR;
				float_out=(ftmp/hqp->Close-m_SzzsZaf)*100.0;
				rs.Format("%6.2f",float_out);	//*100�ðٷֱȱ�ʾ
			}
			else
			{
				fi.color=VipColor.TXTCOLOR;
				float_out=(ftmp/hqp->Close-m_ShzsZaf)*100.0;
				rs.Format("%6.2f",float_out);	//*100�ðٷֱȱ�ʾ
			}
		}
		else
			rs=MEANLESS_STR;
		break;
	case HYD:
		if(pInfo->nWarnCount != 0 && !testzs(pInfo))
		{
			fi.color=VipColor.TXTCOLOR;
			rs.Format("%7ld",pInfo->nWarnCount);
			float_out=(float)pInfo->nWarnCount;
		}
		else
			rs=MEANLESS_STR;
		break;
	case MBZL:
		if(hqp->ItemNum > 0 && !testzs(pInfo))
		{
			fi.color=GridColor.Grid_Volume_Color;
			float_out=hqp->Volume*1.0/hqp->ItemNum;
			rs.Format("%9.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case MBHSL:
		if(hqp->ItemNum > 0 && pInfo->ActiveCapital > 1 && !testzs(pInfo))
		{
			fi.color=VipColor.TXTCOLOR;
			float_out=(hqp->Volume*pInfo->Unit/(pInfo->ActiveCapital*100.0))/hqp->ItemNum;
			rs.Format("%14.4f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_HSL   :          //������%
		if( !testzs(pInfo) && pInfo->ActiveCapital > 1 )
		{
			fi.color=VipColor.TXTCOLOR;
			float_out=hqp->Volume*pInfo->Unit/(pInfo->ActiveCapital*100.0);
			rs.Format("%14.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;		
	case J_JYL  :          //������%
		if( !testzs(pInfo) && pInfo->J_jzc > 1 )
		{
			char strFlag[32];
			switch((int)pInfo->J_HalfYearFlag)
			{
			case 3:
				strcpy(strFlag, "��");
				break;
			case 6:
				strcpy(strFlag, "��");
				break;
			case 9:
				strcpy(strFlag, "��");
				break;
			case 12:
				strcpy(strFlag, "��");
				break;
			default:
				strcpy(strFlag, "");
				break;
			}
			float_out=pInfo->J_jly/pInfo->J_jzc*100.0;
			rs.Format("%14.2f",float_out);
			rs+=strFlag;
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_LTSZ :          //��ͨ��ֵ
		if( !testzs(pInfo) )
		{
			if(hqp->Now > COMPPREC)	ftmp = hqp->Now;
			else					ftmp = hqp->Close;
			fi.color=GridColor.Grid_Other_Color;
			float_out=pInfo->ActiveCapital*ftmp/10000.0;
			rs.Format(_F("%14.2f��"),float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_LTGB  :          //��ͨ�ɱ�
		if( !testzs(pInfo) )
		{
			fi.color=VipColor.TXTCOLOR;
			rs.Format("%14.2f",pInfo->ActiveCapital);
			float_out=pInfo->ActiveCapital;
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_ZSZ:          //AB������ֵ
		if( !testzs(pInfo) )
		{
			if(hqp->Now > COMPPREC)	ftmp = hqp->Now;
			else					ftmp = hqp->Close;
			fi.color=GridColor.Grid_Other_Color;
			float_out=(pInfo->J_zgb-pInfo->J_hg)*ftmp/10000.0;
			rs.Format(_F("%14.2f��"),float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_MGWFP:          //ÿ��δ����
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			float_out=pInfo->J_wfply/pInfo->J_zgb;
			rs.Format("%14.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_MGSY:			  //ÿ������
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			char strFlag[32];
			switch((int)pInfo->J_HalfYearFlag)
			{
			case 3:
				strcpy(strFlag, "��");
				break;
			case 6:
				strcpy(strFlag, "��");
				break;
			case 9:
				strcpy(strFlag, "��");
				break;
			case 12:
				strcpy(strFlag, "��");
				break;
			default:
				strcpy(strFlag, "");
				break;
			}
			if(fabs(pInfo->J_mgsy2)>COMPPREC)
				float_out = pInfo->J_mgsy2;
			else
				float_out = pInfo->J_jly/pInfo->J_zgb;
			rs=FormatFloat2Str(float_out,3);
			rs+=strFlag;
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_MGGJJ:            //ÿ�ɹ�����
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			float_out=pInfo->J_zbgjj/pInfo->J_zgb;
			rs.Format("%14.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_MGJZC:            //ÿ�ɾ��ʲ�
		if( !testzs(pInfo) && pInfo->J_zgb > 1)
		{
			if(fabs(pInfo->J_jzc)>COMPPREC)
				ftmp = pInfo->J_jzc/pInfo->J_zgb;
			else 
				ftmp = pInfo->J_tzmgjz;
			float_out=ftmp;
			rs.Format("%14.2f", ftmp);
		}
		else
			rs=MEANLESS_STR;
		break;
	case J_GDQYB://�ɶ�Ȩ���
		if( !testzs(pInfo) && pInfo->J_zzc > 1)
		{
			float_out=pInfo->J_jzc*100.0/pInfo->J_zzc;
			rs.Format("%14.2f",float_out);
		}
		else
			rs=MEANLESS_STR;
		break;
	case	J_START:	//��������
		if(!testzs(pInfo) && pInfo->J_start > 0)
		{
			float_out=pInfo->J_start;
			rs.Format("%7ld",pInfo->J_start);
		}
		else
			rs=MEANLESS_STR;
		break;
	case	QH_JSJ:		//	�����
		if(pInfo->bigmarket!=QH_TYPE) break;
		fi.color=GetItemColor(hqp->ClearPrice,hqp->Close);
		rs=FormatFloat2Str(hqp->ClearPrice,fi.xsflag);
		float_out=hqp->ClearPrice;
		break;
	case	QH_YJSJ:	//	�ڻ�ǰ�����
		if(pInfo->bigmarket!=QH_TYPE) break;
		fi.color=GetItemColor(hqpex->PreClear,hqp->Close);
		rs=FormatFloat2Str(hqpex->PreClear,fi.xsflag);
		float_out=hqpex->PreClear;
		break;
	case SPREAD: //�������
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%.3f",hqpex->HSpread);
		float_out=hqpex->HSpread;
		break;
	case BSUNIT:	//������λ
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%d",hqpex->PreVol);
		float_out=hqpex->PreVol;
		break;
	case CURRENCYNAME:	//���ҵ�λ
		rs=MEANLESS_STR;
		break;
	case AVERPRICE:	//ƽ����
		fi.color=GetItemColor(hqpex->AverPrice,hqp->Close);
		rs=FormatFloat2Str(hqpex->AverPrice,fi.xsflag);
		float_out=hqpex->AverPrice;
		break;
	case YIELDVAL:	//������
		fi.color=GetItemColor(hqpex->YieldVal,(float)COMPPREC);
		rs=FormatFloat2Str(hqpex->YieldVal,fi.xsflag);
		float_out=hqpex->YieldVal;
		break;
	case HIS_HIGH:	//�����
		if(hqpex->HisHigh>COMPPREC)
		{
			fi.color=GetItemColor(hqpex->HisHigh,hqp->Close);
			rs=FormatFloat2Str(hqpex->HisHigh,fi.xsflag);
			float_out=hqpex->HisHigh;
		}
		break;
	case HIS_LOW:	//�����
		if(hqpex->HisLow>COMPPREC && hqpex->HisLow<10000.0)
		{
			fi.color=GetItemColor(hqpex->HisLow,hqp->Close);
			rs=FormatFloat2Str(hqpex->HisLow,fi.xsflag);
			float_out=hqpex->HisLow;
		}
		break;
	case IEP:		//�ο���
		fi.color=GetItemColor(hqpex->IEP,hqp->Close);
		rs=FormatFloat2Str(hqpex->IEP,fi.xsflag);
		float_out=hqpex->IEP;
		break;
	case IEV:		//�ο���
		fi.color=GridColor.Grid_Volume_Color;
		rs.Format("%.0f",hqpex->IEV);
		float_out=hqpex->IEV;
		break;
	case MRKCAP:	//��ֵ
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%s",MakeJE(hqpex->MrkCap));
		float_out=hqpex->MrkCap;
		break;
	case PE:		//��ӯ��
		if(hqpex->PE>0 && hqpex->PE<10000)
			ftmp=hqpex->PE;
		else
			ftmp=0.0;
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%.3f",ftmp);
		float_out=ftmp;
		break;
	case PREMINUM:	//	��ۣ�
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%.3f",hqpex->PreMinum);
		float_out=hqpex->PreMinum;
		break;
	case GEARING:	//	�ܸ˱���
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%.3f",hqpex->Gearing);
		float_out=hqpex->Gearing;
		break;
	case EXECPRICE:	//	��ʹ��
		fi.color=GetItemColor(hqpex->ExecPrice,hqp->Close);
		rs=FormatFloat2Str(hqpex->ExecPrice,fi.xsflag);
		float_out=hqpex->ExecPrice;
		break;
	case CONVRATIO:	//	�Ϲ�����
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%.3f",hqpex->ConvRatio);
		float_out=hqpex->ConvRatio;
		break;
	case EXPIREDATE://	��Ȩ������
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%s",hqpex->ExpireDate);
		break;
	case TODAYZC:
		if(pInfo->bigmarket!=QH_TYPE) break;
		if(hqp->Yield_VolInStock.VolInStock>0) float_out=(float)hqp->Yield_VolInStock.VolInStock-(float)hqpex->PreVolIn;
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,0);
		break;
	case TODAYKC:
		if( !testzs(pInfo) )
		{
			if(pInfo->bigmarket!=QH_TYPE) break;
			fi.color=GridColor.Grid_Other_Color;
			if(hqp->Volume>0) float_out=(float)hqp->OpenVolume;
			rs=FormatFloat2Str(float_out,0);
		}
		else
			rs=MEANLESS_STR;
		break;
	case TODAYPC:
		if( !testzs(pInfo) )
		{
			if(pInfo->bigmarket!=QH_TYPE) break;
			fi.color=GridColor.Grid_Other_Color;
			if(hqp->Volume>0) float_out=(float)(hqp->Volume-hqp->OpenVolume);
			rs=FormatFloat2Str(float_out,0);
		}
		else
			rs=MEANLESS_STR;
		break;
	case DDX_1:			//����DDX
		if(!pDDEReport)	break;
		float_out=pDDEReport->fDDX;
		fi.color=GetItemColor(float_out,0.00);
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case DDY_1:			//����DDY
		if(!pDDEReport)	break;
		float_out=pDDEReport->fDDY;
		fi.color=GetItemColor(float_out,0.00);
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case DDZ_1:			//����DDZ
		if(!pDDEReport)	break;
		float_out=pDDEReport->fDDZ;
		fi.color=GetItemColor(float_out,0.00);
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case DDX_60:		//60��DDX
		if(!pDDEReport)	break;
		float_out=pDDEReport->fDDX60;
		fi.color=GetItemColor(float_out,0.00);
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case DDY_60:		//60��DDY
		if(!pDDEReport)	break;
		float_out=pDDEReport->fDDY60;
		fi.color=GetItemColor(float_out,0.00);
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case DDX_REDNUM:	//DDX��������
		if(!pDDEReport)	break;
		float_out=pDDEReport->DDXRed;
		fi.color=GridColor.Grid_Other_Color;
		rs.Format("%d",pDDEReport->DDXRed);
		break;
	case DDX_REDIN10:	//10��DDX��
		if(!pDDEReport)	break;
		float_out=pDDEReport->DDXRed10;
		fi.color=GridColor.Grid_Other_Color;
		rs.Format("%d",pDDEReport->DDXRed10);
		break;
	case BIGIOAMOUNT:	//SUP������
		if(!pDDEReport)	break;
		float_out=pDDEReport->fBigIOAmount;
		fi.color=GetItemColor(pDDEReport->fBigIOAmount,0.0);
		rs.Format("%s",MakeFloat(pDDEReport->fBigIOAmount));
		break;
	case BIGIOWEIGHT:	//������Ծ��
		if(!pDDEReport)	break;
		float_out=pDDEReport->fBigIOWeight;
		fi.color=GridColor.Grid_Other_Color;
		rs.Format("%.2f",pDDEReport->fBigIOWeight);
		break;
	case BIGBUY_RAT:	//������%
		if(!pDDEReport)	break;
		float_out=pDDEReport->fBigBuyRate;
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case BIGSELL_RAT:	//������%
		if(!pDDEReport)	break;
		float_out=pDDEReport->fBigSellRate;
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case TOTALWTB:		//����ί��
		if(!pDDEReport)	break;
		float_out=pDDEReport->fBOrderRate;
		fi.color=GetItemColor(float_out,0.00);
		rs=FormatFloat2Str(float_out,XS2);
		break;
	case TICK2_JL:		//��ʾ���
		if(!pDDEReport)	break;
		float_out=pDDEReport->fAveTradeVol;
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,XS2);
		break;
	
	case JS_INDD:		//������
		if(its.Minute<1)	break;
		float_out=its.fVol[1][0]+its.fVol[1][2];
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,NOXS);
		break;
	case JS_OUTDD:		//������
		if(its.Minute<1)	break;
		float_out=its.fVol[1][1]+its.fVol[1][3];
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,NOXS);
		break;
	case JS_INZD:		//�е�����
		if(its.Minute<1)	break;
		float_out=its.fVol[2][0]+its.fVol[2][2];
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,NOXS);
		break;
	case JS_OUTZD:	//�е�����
		if(its.Minute<1)	break;
		float_out=its.fVol[2][1]+its.fVol[2][3];
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,NOXS);
		break;
	case JS_INXD:	//С������
		if(its.Minute<1)	break;
		float_out=its.fVol[0][0]+its.fVol[0][2]-its.fVol[1][0]-its.fVol[1][2]-its.fVol[2][0]-its.fVol[2][2];
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,NOXS);
		break;
	case JS_OUTXD:	//С������
		if(its.Minute<1)	break;
		float_out=its.fVol[0][1]+its.fVol[0][3]-its.fVol[1][1]-its.fVol[1][3]-its.fVol[2][1]-its.fVol[2][3];
		fi.color=GridColor.Grid_Other_Color;
		rs=FormatFloat2Str(float_out,NOXS);
		break;
		
	case H_ZQDM:		//��Ʊ����
		fi.color=GridColor.Grid_CodeName_Color;
		fi.h_align=0;
		rs=pInfo->Code;
		break;
		
	case H_ZQJC:		//��Ʊ����
		fi.color=GetBlockGPColor(pInfo,GridColor.Grid_CodeName_Color,m_WhichGPType,m_nBlockFlag);
		fi.h_align=0;
		rs=pInfo->Name;
		break;
		
	case H_ZJCJ:  //��ǰ�ּ�  ������
		if(hqp->Now>COMPPREC)
			fi.color=GetItemColor(hqp->Now,hqp->Close);
		rs=FormatFloat2Str(hqp->Now,3);
		float_out=hqp->Now;
		break;
		
	case H_ZAF: //�Ƿ���=���ǵ�/��������*100
		if (hqp->Close>COMPPREC)
		{	
			ftmp=hqp->Now;
			//�����ǵ��ͷ��ȵļ��㷽��,���ڿ���ʽ���Ͼ�������
			if(ftmp<COMPPREC && hqp->Buyp[0]>COMPPREC && fabs(hqp->Buyp[0]-hqp->Sellp[0])<COMPPREC)
				ftmp=hqp->Buyp[0];
			if(ftmp>COMPPREC)
			{
				ftmp = ftmp - hqp->Close;
				fi.color=GetItemColor(ftmp/hqp->Close*(float)100.0,.0);
				float_out=ftmp/hqp->Close*(float)100.0;
				rs=FormatFloat2Str(float_out,XS2);
				break;
			}
		}
		rs=MEANLESS_STR;
		break;
		
	case H_CJL:   //�ɽ���
		fi.color=GridColor.Grid_Volume_Color;
		rs.Format("%s",MakeVol3(hqp->Volume));
		e_rs.Format("%d", hqp->Volume);
		float_out=(float)hqp->Volume;
		break;

	case H_GB:
		fi.color=VipColor.TXTCOLOR;
		rs.Format("%14.2f",pInfo->J_hg);
		float_out=pInfo->J_hg;
		break;
	case H_HSL:
		fi.color=VipColor.TXTCOLOR;
		if(pInfo->J_hg>1)
			float_out=hqp->Volume*pInfo->Unit/(pInfo->J_hg*100.0);
		else
			float_out=0.0;
		rs.Format("%14.2f",float_out);
		break;
	case HY_STR: //������ҵ
		rs=MEANLESS_STR;
		break;
	default:  
		rs=MEANLESS_STR;
		float_out=.0f;
		break;
	}
}
