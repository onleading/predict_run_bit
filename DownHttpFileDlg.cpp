// DownAttachDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tdxw.h"
#include "DownHttpFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDownHttpFileDlg dialog


CDownHttpFileDlg::CDownHttpFileDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDownHttpFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDownHttpFileDlg)
	//}}AFX_DATA_INIT
	m_strURL = "";
	m_strDestPathFile = "";
	m_hIcon = AfxGetApp()->LoadIcon(IDI_DOWNLOAD);
}


void CDownHttpFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDownHttpFileDlg)
	DDX_Control(pDX, IDC_INFO_STATIC, m_Info_Label);
	DDX_Control(pDX, IDC_HINT_STATIC, m_Hint_Static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDownHttpFileDlg, CDialog)
	//{{AFX_MSG_MAP(CDownHttpFileDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDownHttpFileDlg message handlers

BOOL CDownHttpFileDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Hint_Static.SetTextColor(RGB(0,0,255));
	m_Info_Label.SetTextColor(RGB(255,64,0));
	SetIcon(m_hIcon,TRUE);
	SetIcon(m_hIcon,FALSE);

	//开始收发数据	
	m_Hint_Static.SetText("正在下载文件...");
	PostMessage(WM_COMMAND,IDOK);

	return TRUE;
}

char *PROFILE_VSATBG1_CONTENT=
"<TR>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 ><A href=\"%s/YCPJ/YCPJYW.aspx?wjs=%s\">%s</A></td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
</TR>\r\n";

char *PROFILE_VSATBG2_CONTENT=
"<TR>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 ><A href=\"%s/YCPJ/YCPJYW.aspx?wjs=%s\">%s</A></td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
</TR>\r\n";

char *PROFILE_VSATXX1_CONTENT=
"<TR>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 ><A href=\"%s/YCPJ/GetContentWithHeader.aspx?bmlb=gpgg&istext=yes&wj=%s\">%s</A></td>\r\n\
<td align=\"left\" class=lb5 > %s </td>\r\n\
<td align=\"left\" class=lb5 ><A href=\"%s/YCPJ/GetContentWithHeader.aspx?bmlb=gpgg&istext=no&wj=%s\"><IMG boder:none border=0 height=16 width=16 src=\"图标PDF.bmp\"></A></td>\r\n\
</TR>\r\n";

char *PROFILE_VSATXX2_CONTENT=
"<TR>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 ><A href=\"%s/YCPJ/GetContentWithHeader.aspx?bmlb=gpgg&istext=yes&wj=%s\">%s</A></td>\r\n\
<td align=\"left\" class=lb6 > %s </td>\r\n\
<td align=\"left\" class=lb6 ><A href=\"%s/YCPJ/GetContentWithHeader.aspx?bmlb=gpgg&istext=no&wj=%s\"><IMG boder:none border=0 height=16 width=16 src=\"图标PDF.bmp\"></A></td>\r\n\
</TR>\r\n";


BOOL ProcessVsatLocalFile(BOOL bBG,BYTE *ContentBuf,DWORD nLen,const char *strDestPath)
{
	//将引号外的逗号换成'|',已便进行下次的替换
	BOOL bFindYH=FALSE;
	for(int k=0;k<nLen;k++)
	{
		if(ContentBuf[k]=='\"')
		{
			bFindYH=!bFindYH;
			ContentBuf[k]=' ';
		}
		else if(!bFindYH && ContentBuf[k]==',')
			ContentBuf[k]='|';
		if((ContentBuf[k]=='\r' || ContentBuf[k]=='\n') && bFindYH)
			bFindYH=FALSE;
	}
	//让内容变为文件
	CFile		file;
	char		tmpFile[MAX_PATH];
	sprintf(tmpFile,"%smeta_vsat.tmp",strDestPath);
	if(!file.Open(tmpFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
		return FALSE;
	file.Write(ContentBuf,nLen);
	file.Close();
	//一行一行存出内容进行分析并构造内容
	CString		OneLine="";
	CString		strIEBody="";
	CString		tempLine;
	CStdioFile	file2;
	if(!file2.Open(tmpFile,CFile::modeRead|CFile::shareDenyNone))
		return FALSE;
	int			nCnt=0;
	char		tmpCODE[255];
	char		tmpJC[255];
	char		tmpBBRQ[255];
	char		tmpLX[255];
	char		tmpBT[255];
	char		tmpCC[255];
	char		tmpZZ[255];
	char		tmpPJ[255];
	char		tmpWJ[255];
	char		tmpWZWJ[255];
	if(bBG)
	{
		file2.ReadString(tempLine);
		while(file2.ReadString(tempLine))
		{
			tmpCODE[0]=tmpJC[0]=tmpBBRQ[0]=tmpLX[0]=tmpBT[0]=tmpCC[0]=tmpZZ[0]=tmpPJ[0]=tmpWJ[0]=tmpWZWJ[0]=0;
			ParseMessageStr(tempLine,tmpCODE,"|",1);	//代码
			if(strlen(tmpCODE)!=6)
				continue;
			ParseMessageStr(tempLine,tmpJC,"|",2);		//简称
			ParseMessageStr(tempLine,tmpBBRQ,"|",3);	//报告日期
			ParseMessageStr(tempLine,tmpLX,"|",4);		//类型
			ParseMessageStr(tempLine,tmpBT,"|",5);		//标题
			ParseMessageStr(tempLine,tmpCC,"|",6);		//出处
			ParseMessageStr(tempLine,tmpZZ,"|",7);		//作者
			ParseMessageStr(tempLine,tmpPJ,"|",8);		//评级
			ParseMessageStr(tempLine,tmpWJ,"|",9);		//文件
			
			OneLine.Format(nCnt%2==0?PROFILE_VSATBG1_CONTENT:PROFILE_VSATBG2_CONTENT,tmpCODE,tmpJC,tmpBBRQ,g_OEM.strVsatHttp,tmpWJ,tmpBT,tmpCC,tmpZZ,tmpPJ);
			strIEBody+=OneLine;
			nCnt++;
		}
	}
	else
	{
		file2.ReadString(tempLine);
		while(file2.ReadString(tempLine))
		{
			tmpCODE[0]=tmpJC[0]=tmpBBRQ[0]=tmpLX[0]=tmpBT[0]=tmpCC[0]=tmpZZ[0]=tmpPJ[0]=tmpWJ[0]=tmpWZWJ[0]=0;
			ParseMessageStr(tempLine,tmpCODE,"|",1);	//代码
			if(strlen(tmpCODE)!=6)
				continue;
			ParseMessageStr(tempLine,tmpJC,"|",2);		//简称
			ParseMessageStr(tempLine,tmpBBRQ,"|",3);	//报告日期
			ParseMessageStr(tempLine,tmpLX,"|",4);		//类型
			ParseMessageStr(tempLine,tmpBT,"|",5);		//标题
			ParseMessageStr(tempLine,tmpCC,"|",6);		//出处
			ParseMessageStr(tempLine,tmpWJ,"|",7);		//文件
			ParseMessageStr(tempLine,tmpWZWJ,"|",8);	//附件文件

			OneLine.Format(nCnt%2==0?PROFILE_VSATXX1_CONTENT:PROFILE_VSATXX2_CONTENT,tmpCODE,tmpJC,tmpBBRQ,g_OEM.strVsatHttp,tmpWJ,tmpBT,tmpCC,g_OEM.strVsatHttp,tmpWZWJ);
			strIEBody+=OneLine;
			nCnt++;
		}
	}
	file2.Close();
	//读出模板
	CString tmpContent="";
	if(bBG)
		sprintf(tmpFile,"%smeta_vsatbg.dat",strDestPath);
	else
		sprintf(tmpFile,"%smeta_vsatxx.dat",strDestPath);
	if(!file.Open(tmpFile,CFile::modeRead|CFile::shareDenyNone))
		return FALSE;
	int nFileLen = file.GetLength();
	if(nFileLen>0)
	{
		char * tmpBuffer = new char[nFileLen+1];
		file.Read(tmpBuffer,nFileLen);
		tmpBuffer[nFileLen]=0;
		tmpContent = tmpBuffer;
		delete tmpBuffer;
	}
	file.Close();
	//替换内容
	tmpContent.Replace("@@@BODY@@@",strIEBody);
	//写出具体文件
	sprintf(tmpFile,"%smeta_vsat.htm",strDestPath);
	if(file.Open(tmpFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone))
	{
		file.Write(tmpContent,tmpContent.GetLength());
		file.Close();
		return TRUE;
	}
	return FALSE;
}

BOOL DownHttpFile(const char *url,BYTE* & ContentBuf,DWORD & nLen)
{
	nLen=0;
	BYTE *				szBuff = new BYTE[1024];
	CInternetSession*	pInetSession=NULL;
	CHttpFile		*	pHF=NULL;
	CMemFile			mFile((BYTE*)szBuff,1024,1024);

	BOOL		bSuc=TRUE;
	CString		slen;
	char		buf[1024];

	try
	{
		DWORD	dwStatus=0;
		DWORD	dwBuffLen = sizeof(dwStatus);
		pInetSession = new CInternetSession(" ",1,INTERNET_OPEN_TYPE_PRECONFIG);

		pHF=(CHttpFile*)pInetSession->OpenURL(url,1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
		pHF->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,&dwStatus, &dwBuffLen);
		while(dwStatus == HTTP_STATUS_PROXY_AUTH_REQ || dwStatus==HTTP_STATUS_DENIED)
		{
			DWORD	dwPrompt=pHF->ErrorDlg(NULL, ERROR_INTERNET_INCORRECT_PASSWORD,FLAGS_ERROR_UI_FLAGS_GENERATE_DATA | FLAGS_ERROR_UI_FLAGS_CHANGE_OPTIONS,NULL);
			if(dwPrompt!=ERROR_INTERNET_FORCE_RETRY)
				break;
			pHF=(CHttpFile*)pInetSession->OpenURL(url,1,INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_DONT_CACHE|INTERNET_FLAG_RELOAD);
			pHF->QueryInfoStatusCode(dwStatus);
		}
		if(dwStatus>= 200 && dwStatus<300 )
		{
			pHF->QueryInfo(HTTP_QUERY_CONTENT_LENGTH,slen);
			nLen=atol(slen);	
			DWORD dw;			
			pHF->QueryInfoStatusCode(dw);
			if (dw == HTTP_STATUS_OK)
			{
				UINT nRead = pHF->Read(buf, 1024);
				while(nRead > 0)
				{
					mFile.Write(buf,nRead);
					nRead=pHF->Read(buf,1024);
				}
			}
		}
		else
			bSuc=FALSE;
	}
	catch (CInternetException *pEx)
	{
		char err[1024];
		pEx->GetErrorMessage(err,1024);
		pEx->Delete();
		bSuc=FALSE;
	}
	
	DWORD mflen=mFile.GetLength();
	szBuff=mFile.Detach();

	if(bSuc)
	{
		if(mflen!=nLen || mflen==0 || nLen==0)
			bSuc=FALSE;
		else
			ContentBuf=szBuff;
	}

	if(!ContentBuf) 
	{
		TDEL(szBuff);
	}
	if(pHF)
	{
		pHF->Close();
		TDEL(pHF);
	}
	if(pInetSession)
	{
		pInetSession->Close();
		TDEL(pInetSession);
	}
	return bSuc;
}

void CDownHttpFileDlg::OnOK() 
{
	BYTE *	pBuf = NULL;
	DWORD	nLen = 0;
	if(!DownHttpFile(m_strURL,pBuf,nLen))
	{
		TDX_MessageBox(m_hWnd,"取数据失败!",MB_OK|MB_ICONERROR);
		return;
	}
	if(pBuf)
	{
		TDEL(pBuf);
	}
}

void CDownHttpFileDlg::OnCancel() 
{
	CDialog::OnCancel();
}
