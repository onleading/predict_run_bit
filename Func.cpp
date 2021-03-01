#include "stdafx.h"
#include "func.h"
#include "dataio.h"
#include "share.h"
#include "dbf.h"  //dbf��̬���ӿ��ͷ�ļ�
#include "tdxw.h"
#include "Comview.h"
#include "Comview2.h"
#include "BigMenuView.h"
#include "HomePageView.h"
#include "GridView.h"
#include "TxtView.h"
#include "NewsView.h"
#include "IEBrowserView.h"
#include "JbfxView.h"
#include "AdvHqView.h"
#include "MainFrm.h"
#include "JJView.h"
#include "Tvview.h"
#include "RssView.h"
#include "HQControl.h"

#ifdef	BIG5	//���������BIG5�ĺ�,��_F,_JΪ����,����Ϊһ�պ�

	long	GB2BIG(LPSTR strDest,LPCSTR strSrc,wchar_t*pws)
	{
		long	SrcSize=lstrlen(strSrc);
		if(SrcSize>33*1024-10) 
		{
			strDest[0]=0;
			return 0;
		}
		long	r1=0,r=LCMapString(
			MAKELCID(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED),SORT_CHINESE_PRC),
			LCMAP_TRADITIONAL_CHINESE,strSrc,SrcSize,NULL,0);
		r = LCMapString(
			MAKELCID(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED),SORT_CHINESE_PRC),
			LCMAP_TRADITIONAL_CHINESE,strSrc,SrcSize,strDest,r+1);
		if(r)
		{
			strDest[r]=0; 
			r1=MultiByteToWideChar(936,0,strDest,r,pws,r+1);
			BOOL f=FALSE;
			r1=WideCharToMultiByte(950,0,pws,r1,strDest,r+1,"?",&f);
			strDest[r1]=0;
		}
		return(r1);
	}

	long	BIG2GB(LPSTR strDest,LPCSTR strSrc,wchar_t*pws)
	{
		long	SrcSize=lstrlen(strSrc);
		if(SrcSize>33*1024-10) 
		{
			strDest[0]=0;
			return 0;
		}
		long	r1=0,r=LCMapString(
			MAKELCID(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_TRADITIONAL),SORT_CHINESE_BIG5),
			SUBLANG_CHINESE_SIMPLIFIED,strSrc,SrcSize,NULL,0);
		r = LCMapString(
			MAKELCID(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_TRADITIONAL),SORT_CHINESE_BIG5),
			LCMAP_SIMPLIFIED_CHINESE,strSrc,SrcSize,strDest,r+1);
		if(r)
		{
			strDest[r]=0; 
			r1=MultiByteToWideChar(950,0,strDest,r,pws,r+1);
			BOOL f=FALSE;
			r1=WideCharToMultiByte(936,0,pws,r1,strDest,r+1,"?",&f);
			strDest[r1]=0;
		}
		return(r1);
	}

	char	g_FtstrDest[33*1024];
	wchar_t	g_FtWChar[33*1024];

	LPSTR _FL(LPSTR lpString)
	{
		GB2BIG(g_FtstrDest,lpString,g_FtWChar);
		return(lstrcpy(lpString,g_FtstrDest));
	}

	LPSTR _F(LPCSTR lpString)
	{
		GB2BIG(g_FtstrDest,lpString,g_FtWChar);
		return g_FtstrDest;
	}

	LPSTR _JL(LPSTR lpString)
	{
		BIG2GB(g_FtstrDest,lpString,g_FtWChar);
		return(lstrcpy(lpString,g_FtstrDest));
	}

	LPSTR _J(LPCSTR lpString)
	{
		BIG2GB(g_FtstrDest,lpString,g_FtWChar);
		return g_FtstrDest;
	}
#endif 
	
BOOL IsBig5Version()
{
#ifdef	BIG5
	TDX_MessageBox(NULL,"������ݲ�֧�ִ˹���!",MB_OK|MB_ICONEXCLAMATION);
	return TRUE;
#else
	return FALSE;
#endif
}

DWORD TDX_GetPrivateProfileString(LPCSTR lpAppName,LPCSTR lpKeyName,LPCSTR lpDefault,LPSTR lpReturnedString,DWORD nSize,LPCSTR lpFileName)
{
	DWORD nRet=GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpReturnedString,nSize,lpFileName);
#ifdef BIG5
	if(strlen(lpReturnedString)>0)
		_FL(lpReturnedString);
#endif
	return nRet;
}

void TDXBeep(int flag)
{
	if(flag==0)
	{
		MessageBeep(0xFFFFFFFF);
		MessageBeep(MB_ICONASTERISK);
	}
	else
	{
		MessageBeep(MB_ICONQUESTION);
		MessageBeep(0xFFFFFFFF);
	}
}

void Tdx_OutputDebugString(const char *fmt,...)
{
    va_list arg_ptr;
    char tmp[1024];
    va_start(arg_ptr,fmt);
    vsprintf(tmp,fmt,arg_ptr);
    va_end(arg_ptr);
	OutputDebugString(tmp);
}

int GetPrivateProfileChar(LPCSTR lpAppName,LPCSTR lpKeyName,char nDefault,LPCSTR lpFileName)
{
	char tmpChar[20];
	char DefaultChar[2];
	DefaultChar[0]=nDefault;
	DefaultChar[1]=0;
	GetPrivateProfileString(lpAppName,lpKeyName,DefaultChar,tmpChar,19,lpFileName);
	AllTrim(tmpChar);
	return tmpChar[0];
}

char * GetCurDir(char * szPath,int len)
{
	GetModuleFileName(NULL,szPath,len);
	char	*	lpstr = szPath;
	while ( strstr(lpstr,"\\") )
		lpstr = strstr(lpstr,"\\")+1;
	if ( lpstr )	
		lpstr[0] = 0;
	return szPath;
}

BOOL TestFileExist(CString filename)
{
	CFile tmpFile1;
	if(!tmpFile1.Open(filename,CFile::modeRead|CFile::shareDenyNone))
		return FALSE;
	tmpFile1.Close();
	return TRUE;
}

int TDX_MessageBox(HWND m_hWnd,const char *str,UINT nType,BOOL bSepTitle)
{
	int nLen = (strlen(str)+1)*2;
	char *tempStr = new char[nLen+1];
	strcpy(tempStr,str);
#ifdef BIG5
	int nRet =  MessageBox(m_hWnd,_F(tempStr),g_strPreName,nType);
#else
	int nRet = 0;
	if(bSepTitle)
		nRet =  MessageBox(m_hWnd,tempStr,g_strPreName,nType);
	else
		nRet =  MessageBox(m_hWnd,tempStr,g_GC.version_title,nType);
#endif
	delete [] tempStr;
	return nRet;	
}

/////////////////////////////////////////////////////////////////////////////

long	AfxStringTrimSpace(LPSTR  string)
{	
	const char strmask[5]=" \t\r\n";
	for(long len=lstrlen(string),i=len-1,space=0;i>=0;i--)
	{
		if(memchr(strmask,string[i],4)==NULL) break;
		string[i]=0;
	}
	for(;space<i&&memchr(strmask,string[space],4);space++);
	if(space&&(i-space+2)) memmove(string,string+space,i-space+2);
	return(i-space+1);
}

short	GetCmdLine(LPSTR lpString,char*argv[],short MaxLine,char *sepstr)
{
	int nsepLen = strlen(sepstr);
	for(short i=0,t=0,argc=0,l=lstrlen(argv[0]=lpString);i<=l&&argc<MaxLine;++i)
	{
		if(memchr(sepstr,lpString[i],nsepLen+1)==NULL) ++t;
		else
		{
			if(lpString[i])
			{
				if(t&&argc+1<MaxLine) argv[argc+1] = &lpString[i+1];
				if(!t)
				{
					argv[argc++][t] = 0;
					if(argc<MaxLine)
						argv[argc] = &lpString[i+1];
				}
			}
			if(t)
			{
				argv[argc][t] = 0;
				if(AfxStringTrimSpace(argv[argc])) ++argc;
				t = 0;
			}
		}
	}
	return(argc);
}

BOOL	AllSameAlpha(LPSTR lpString, char a)
{
	int nLen = strlen(lpString);
	for(int i=0;i<nLen;i++)
	{
		if(lpString[i]!=a)
			return FALSE;
	}
	return TRUE;
}

//���ڱȽϷּ۵Ļص�����
int FjSort(const void * a,const void *b)
{
	struct UFjContent *sorta,*sortb;
	sorta = (struct UFjContent *)a;
	sortb = (struct UFjContent *)b;
	if(sorta->price < sortb->price) return 1;
	if(sorta->price > sortb->price) return -1;
	return 0;
}

unsigned char FindFile(char * FileName)	//����Ϊ1,��ʾ�ҵ�,Ϊ0,��ʾû��
{   
	unsigned char ReturnValue = 0;
	struct _finddata_t  fileinfo;
	long                handle;
	handle = _findfirst( FileName, &fileinfo );
	if (handle != -1)		ReturnValue = 1;
	_findclose(handle);
	return (ReturnValue);
}

void MendPath(char *path)
{
	int len = strlen(path);
	if(len < 1 || len > 255) return;
	if(path[len-1]!='\\')
	{
		path[len] = '\\';
		path[len+1] = 0;
	}
}

//����ת������
void AllTrim( char *s)
{
	int  i=0;
	if(s==NULL)		return;
	if(strlen(s)<=0)return;	
	while (s[i] == ' ') i ++;
	if (i) memmove( s, s + i, strlen(s)+1-i );
	i = max(0,strlen(s) - 1);
	while ((i) && (s[i] == ' ')) s[i--] = 0;
}

void AllTrimEx( char *s) //AllTrim2 �س���
{
	char stmp[256];
	BOOL head = TRUE;
	int tailblank = 0,count = 0;
	for(unsigned int i = 0;i < strlen(s);i++)
	{
		if(s[i]!=' '&&s[i]!='\t'&&s[i]!='\n'&&s[i]!='\r')
		{
			head	  = FALSE;
			tailblank = 0;
		}
		else	tailblank++;
		if(!head)
		{
			stmp[count] = s[i];
			count++;
		}
	}
	if(tailblank==strlen(s)) tailblank = 0;
	if(count-tailblank > 0)
	{
		strncpy(s,stmp,count-tailblank);
		s[count-tailblank] = '\0';
	}
	else
		s[0] = '\0';
}
void TrimString(CString& theStr)
{
	theStr.TrimLeft ();
	theStr.TrimRight();
}

CString IntToStr(int n, short intFlag)
{
	CString tempStr;
	switch(intFlag) 
	{
	case 2:
		tempStr.Format ("%02d",n);
		break;
	case 3:
		tempStr.Format ("%03d",n);
		break;
	default:
		tempStr.Format ("%d",n);
		break;
	}
	return tempStr;
}

CString FloatToStr(float ftmp)
{
	CString tempStr;
	tempStr.Format ("%.3f",ftmp);
	return tempStr;
}

CString FloatToStr2(float ftmp,short xsflag)
{
	CString tempStr;
	if(xsflag==3)
		tempStr.Format ("%.3f",ftmp);
	else
		tempStr.Format ("%.2f",ftmp);
	return tempStr;
}

void GetStr( const char *str1, char *str2, WORD nSize, short xh, char ch )
{
	int len, i = 1, m = 0, k = 0;
	
	str2[0] = '\0';
	len = strlen ( str1 );
	//	xh += 3;
	while ( i < xh && m < len )
	{
		if ( str1[m] == ch ) i++;
		m++;
	}
	if ( m >= len ) return;
	while ( str1[m] != ch && m < len )
	{
		str2[k] = str1[m];
		i++; k++; m++;
		if( k>=nSize-1 )	break;
	}
	str2[k] = '\0';
}

void ParseMessageStr(const char *str1, char *str2, const char *pre,int xh)
{
	int len, i = 1, m = 0, k = 0;
	int prelen=strlen(pre);
	str2[0] = '\0';
	len = strlen ( str1 );
	while ( i < xh && m < len-prelen )
	{
		if ( strncmp(str1+m,pre,prelen)==0)
			i++;
		m++;
	};
	if(i!=xh)
	{
		str2[k] = '\0';
		return;
	}
	m+=prelen-1;	
	if ( m >= len-prelen+1) return;
	while(m < len && strncmp(str1+m,pre,prelen))
	{
		str2[k] = str1[m];
		i++;
		k++;
		m++;
	};
	str2[k] = '\0';
}

//"SignHost=%s\r\nSignDate=%s\r\nSignValid=%s\r\nAccount=%s\r\nUserClass=%s\r\n"
void GetStrDomainValue(const char *TotalStr, const char *KeyStr, char *Value,int MaxValueLen,const char *sep)
{
	Value[0]=0;
	int KeyLen=strlen(KeyStr),TotalLen=strlen(TotalStr),tmpLen;
	char *p1=strstr(TotalStr,KeyStr);
	if(!p1) return;
	p1+=KeyLen+1;
	char *p2=strstr(p1,sep);
	if(!p2) 
		tmpLen=min(MaxValueLen-1,TotalLen-(p1-TotalStr));
	else
		tmpLen=min(MaxValueLen-1,p2-p1);
	strncpy(Value,p1,tmpLen);
	Value[tmpLen]=0;
}

CTime LongToTime(long ltmp)
{
	CTime NowDate = CTime::GetCurrentTime();
	if(ltmp <= 0) return NowDate;

	int year,month,day;
	year = ltmp / 10000;
	if(year <= 1900) return NowDate;
	month = (ltmp % 10000) / 100;
	if(month <= 0 || month > 12) return NowDate;
	day = (ltmp % 10000) % 100;
	if(day <= 0 || day > 31) return NowDate;
	return(CTime(year,month,day,0,0,0));
}

long TimeToLong(CTime time)
{
	long ltmp;
	ltmp = (time.GetDay()+time.GetMonth()*100)+time.GetYear()*10000;
	return ltmp;
}

long SubLongTime(long ltmp1,long ltmp2)
{
	CTime time1=LongToTime(ltmp1);
	CTime time2=LongToTime(ltmp2);
	CTimeSpan tp=time1-time2;
	return tp.GetDays();
}

long AddLongTime(long ltmp1,long span)
{
	CTime time1=LongToTime(ltmp1);
	CTimeSpan tp(span,0,0,0);
	CTime time2=time1+tp;
	return TimeToLong(time2);
}

char * MakeGB(DWORD ltmp)
{
	static char stri[80];
	stri[0] = 0; 
	if (abs(ltmp) > 10000000000) 
		return MEANLESS_STR;
	if(abs(ltmp) >= 1000000)
		sprintf(stri,_F("%6.0f��"),ltmp/10000.0);	
	else if(abs(ltmp) >= 100000)
		sprintf(stri,_F("%6.1f��"),ltmp/10000.0);
	else if(abs(ltmp) >= 10000)
		sprintf(stri,_F("%6.2f��"),ltmp/10000.0);
	else
		sprintf(stri,_F("%4d��"),ltmp);
	return stri;
}

char * MakeVol3(long ltmp)  //�ش����εĴ���
{
	static char stri[80];
	stri[0] = 0; 
	if (abs(ltmp) > 4000000000) 
		return MEANLESS_STR;
	if(abs(ltmp) < 100000)
		sprintf(stri,"%6d",ltmp);
	else if(abs(ltmp) < 1000000)
		sprintf(stri,_F("%7.1f��"),ltmp/10000.0);
	else if(abs(ltmp) < 100000000)
		sprintf(stri,_F("%6d��"),ltmp/10000);
	else
		sprintf(stri,_F("%7.2f��"),ltmp/100000000.0);
	return stri;
}

char * MakeVol2(long ltmp)  //�ش����εĴ���
{
	static char stri[80];
	stri[0] = 0; 
	if (abs(ltmp) > 4000000000) 
		return MEANLESS_STR;
	if(abs(ltmp) < 100000)
		sprintf(stri,"%7d",ltmp);
	else
		sprintf(stri,_F("%7d��"),ltmp/10000);
	return stri;
}

char * MakeVol(double ftmp)  //�ش󸡵��εĴ���
{
	static char stri[80];
	stri[0] = 0; 
	if (ftmp < 0 || fabs(ftmp) > 1000000000000000.0)
		return MEANLESS_STR;
	if (fabs(ftmp) < 10000.0)
		sprintf(stri,"%7.2f",ftmp);
	else if (fabs(ftmp) < 100000000.0)
		sprintf(stri,_F("%7.1f��"),ftmp/10000.0);
	else sprintf(stri,_F("%7.2f��"),ftmp/100000000.0);

	return stri;
}

char * MakeJE2(float ftmp)
{
	static char stri[80];
	if(ftmp < 0) return "";
	sprintf(stri,_F("%9.0f��"),ftmp/10000);
	AllTrim(stri);
	return stri;
}

char *MakeJE3(float ftmp)
{
	static char stri[80];
	if (ftmp <= 0 || fabs(ftmp) > 10000000000000000.0)
		return MEANLESS_STR;
	if (fabs(ftmp) < 10000.0)
		sprintf(stri,"%6.1f",ftmp);
	else if (fabs(ftmp) < 100000000.0)
		sprintf(stri,_F("%5.0f��"),ftmp/10000.0);
	else if (fabs(ftmp) < 100000000000.0)
		sprintf(stri,_F("%6.1f��"),ftmp/100000000.0);
	else 
		sprintf(stri,_F("%6.0f��"),ftmp/100000000.0);
	return stri;
}

char * MakeJE(float ftmp)  //�ֽ��
{ 
	if(ftmp < 0) //�Ѿ����
		return MEANLESS_STR;
	return MakeVol(ftmp); //�ô���󸡵�ɽ����ķ����������ֽ��
}

char * MakeFloat(double ftmp)  //�ش󸡵��εĴ���
{
	static char str_Float[100];
	str_Float[0] = 0; 
	if (fabs(ftmp) < 10000.0)
		sprintf(str_Float,"%7.2f",ftmp);
	else if (fabs(ftmp) < 100000000.0)
		sprintf(str_Float,_F("%7.1f��"),ftmp/10000.0);
	else 
		sprintf(str_Float,_F("%7.2f��"),ftmp/100000000.0);

	return str_Float;
}

//�����е�
short FloatComp(float fFloat1,float fFloat2)
{   
	if(fabs(fFloat1-fFloat2)<0.00001) return 0;
	if(fFloat1>fFloat2)             return 1;
	return -1;
}

void DeleteMatchFile(CString path,CString tpf)
{
	struct _finddata_t	ffblk;
	int					done;
	long handle = _findfirst(path+tpf,&ffblk);
	if (handle==-1) done = -1;
	else            done = 0;
	while (!done)
	{   
		unlink(path+ffblk.name);
		done = _findnext(handle,&ffblk);
	}
	if(handle != -1)
		_findclose(handle);
}

void DeleteDirInfo(const char * Path)
{
	long          handle;
	struct _finddata_t ffblk;
	int           done;
	char          Str[MAX_PATH];
     
	sprintf(Str,"%s\\*.*",Path);
	handle = _findfirst(Str,&ffblk);
	if (handle==-1) done=-1;
	else            done=0;
	while (!done)
	{
		if (ffblk.name[0]!='.')
		{
			sprintf(Str,"%s\\%s",Path,ffblk.name);
			if (ffblk.attrib&_A_SUBDIR)
			{
				DeleteDirInfo(Str);
				RemoveDirectory(Str);
			}
			else
				unlink(Str);
		}
		done = _findnext(handle,&ffblk);
	}
	if(handle != -1)
		_findclose(handle);
}

//������
int sort_func( const void * a, const void * b )
{
	struct sort_struc *fsorttmpa,*fsorttmpb;
	fsorttmpa = (struct sort_struc*) a;
	fsorttmpb = (struct sort_struc*) b;
	if ( fsorttmpa->fValue > fsorttmpb->fValue ) return 1;
	if ( fsorttmpa->fValue < fsorttmpb->fValue ) return -1;
	if ( fsorttmpa->nIndex > fsorttmpb->nIndex ) return 1;
	if ( fsorttmpa->nIndex < fsorttmpb->nIndex ) return -1;
	return 0;
}

//��Ʊ����ר��������
int sort_func2( const void * a, const void * b )
{
	struct sort_struc2 *fsorttmpa,*fsorttmpb;
	fsorttmpa = (struct sort_struc2*) a;
	fsorttmpb = (struct sort_struc2*) b;
	if(strcmp(fsorttmpa->StrValue,fsorttmpb->StrValue)>0) return 1;
	if(strcmp(fsorttmpa->StrValue,fsorttmpb->StrValue)<0) return -1;
	if ( fsorttmpa->nIndex > fsorttmpb->nIndex ) return 1;
	if ( fsorttmpa->nIndex < fsorttmpb->nIndex ) return -1;
	return 0;
}

//���Ű��ר��
int sort_func3( const void * a, const void * b )
{
	struct sort_struc3 *fsorttmpa,*fsorttmpb;
	fsorttmpa = (struct sort_struc3*) a;
	fsorttmpb = (struct sort_struc3*) b;
	if ( fsorttmpa->fValue > fsorttmpb->fValue ) return 1;
	if ( fsorttmpa->fValue < fsorttmpb->fValue ) return -1;
	if ( fsorttmpa->nItemNum < fsorttmpb->nItemNum ) return 1;
	if ( fsorttmpa->nItemNum > fsorttmpb->nItemNum ) return -1;
	if ( fsorttmpa->nIndex > fsorttmpb->nIndex ) return 1;
	if ( fsorttmpa->nIndex < fsorttmpb->nIndex ) return -1;
	return 0;
}

short  GetXSFlag(struct MemStkInfo * scode)
{	
	if(scode == NULL) 
		return 0;
	//��������Ʒ�ֽ����ر���
	BOOL bLCPt = IsLCPt(scode);	
	if(bLCPt && g_pLcIdxMng)
	{
		LPLCPTSET pPtSet = g_pLcIdxMng->GetLcPTSet(scode->Code);
		if(pPtSet)
		{
			if(strncmp(pPtSet->BaseCode,"58",2)==0 || strncmp(pPtSet->BaseCode,"03",2)==0 || strncmp(pPtSet->BaseCode,"900",3)==0)
				return XS3;
			return XS2;
		}		
	}
	short setcode = g_pStockIO->GetDomain(scode);
	//�������������
	if(setcode>1 && setcode<MAX_MKT_NUM && scode->Index>=DS_FROM)
	{
		MemStkInfo *pInfo = g_pStockIO->GetDSStkInfo(setcode,scode->Code);
		if(pInfo==NULL) 
			return 0;		
		if(pInfo->bigmarket==IDX_TYPE) 
			return 2;
		if(pInfo->bigmarket==HKGP_TYPE && pInfo->Close>4000.0) 
			return 0;
		if(pInfo->Close>999.999 && pInfo->DocNum==3)
			pInfo->DocNum=2;
		if(pInfo->Close>9999.99 && pInfo->DocNum==2)
			pInfo->DocNum=1;
		return pInfo->DocNum;
	}
	//����һ���AB��
	if (testzs(scode)) return XS2;
	if (setcode==SH && scode->Code[0]=='9' && atol(scode->Code)<999000L)
		  return XS3;  //�Ϻ�B��
	if (setcode==SH && scode->Code[0]=='2')
		return XS3;
	if (setcode==SZ && strncmp(scode->Code,"131",3)==0)
		return XS3;
	if (strncmp(scode->Code,"42",2)==0)
		return XS3;
	if (strncmp(scode->Code,"15",2)==0 || strncmp(scode->Code,"16",2)==0 || strncmp(scode->Code,"17",2)==0 || strncmp(scode->Code,"18",2)==0 || strncmp(scode->Code,"5",1)==0)
		return XS3;
	if (strncmp(scode->Code,"03",2)==0 || strncmp(scode->Code,"08",2)==0)
		return XS3;
	return XS2;
}

BOOL UseSpecVolumeUnit(int StockType,short PeriodType)
{
	if(!g_bUseSpecVolumeUnit)
	{
		return ((StockType==CODE_SZQZ || StockType==CODE_SHQZ) && 
			(PeriodType==PER_WEEK || PeriodType==PER_MONTH || PeriodType==PER_DAYN || PeriodType==PER_SEASON || PeriodType==PER_YEAR));
	}
	return (PeriodType==PER_WEEK || PeriodType==PER_MONTH || PeriodType==PER_DAYN || PeriodType==PER_SEASON || PeriodType==PER_YEAR);
}

float CalcHSL(MemStkInfo *pInfo,float fVolume,float fRealLTGB,short nPeriod)
{
	if(!pInfo)
		return 0.0;
	int itmp = GetStockType(pInfo->setcode,pInfo->Code);
	BOOL bSepc = UseSpecVolumeUnit(itmp,nPeriod);
	DWORD nUnit = 1;
	if(itmp==CODE_SHQZ || itmp==CODE_SZQZ) 
		nUnit = 100;
	if(bSepc)
		return fVolume*nUnit*100.0*100.0/(fRealLTGB*10000.0);
	return fVolume*nUnit*100.0/(fRealLTGB*10000.0);
}

//�������
float cal_averagetmp(struct MemStkInfo *scode,struct CurrStockData * hqp)
{
	if(scode == NULL || scode->setcode>=2) 
		return 0.001f;
	int itmp = GetStockType(scode->setcode,scode->Code);
	float last_price,average;
	BOOL bZS = testzs(scode);
	short  setcode = g_pStockIO->GetDomain(scode);
	if (bZS&&hqp->Sellp[2]>0.01f)
		last_price = hqp->Sellp[2];
	else   
		last_price = hqp->Close;
	if (bZS)
	{ 
		if(hqp->Yield_VolInStock.Yield<0.01f) 
			average = last_price;
		else average = hqp->Yield_VolInStock.Yield;
	}
	else
	{ 
		if(hqp->Volume==0||hqp->Amount<0.001) 
			average = last_price;
		else
			average = hqp->Amount/(1.0*hqp->Volume*scode->Unit+hqp->Flag.RestVol);
	}
	if ( setcode == SZ && scode->Code[0] == '1' && scode->Code[1] == '3' ) // ��ծ�ع�,������Ϊ13��ͷ
		average = hqp->Now;
	if ( setcode == SH && scode->Code[0] == '2' ) // ��ծ�ع�
		average = hqp->Now;
	return average;
}

int System2Tdx(int m,short * fz)
{   int  itemnum;
    if   (m<=fz[0])   itemnum=1;
    else if(m<fz[1])  itemnum=m-fz[0]+1;
    else if(m<fz[2])  itemnum=fz[1]-fz[0];
    else if(m<fz[3])  itemnum=m-fz[2]+1+(fz[1]-fz[0]);
    else if(m<fz[4])  itemnum=fz[3]-fz[2]+(fz[1]-fz[0]);
    else if(m<fz[5])  itemnum=m-fz[4]+1+(fz[3]-fz[2])+(fz[1]-fz[0]);
    else if(m<fz[6])  itemnum=fz[5]-fz[4]+(fz[3]-fz[2])+(fz[1]-fz[0]);
    else if(m<fz[7])  itemnum=m-fz[6]+1+(fz[5]-fz[4])+(fz[3]-fz[2])+(fz[1]-fz[0]);
    else              itemnum=fz[7]-fz[6]+(fz[5]-fz[4])+(fz[3]-fz[2])+(fz[1]-fz[0]);
    return itemnum;
}

int GetRealTotalMinute( short *fz )
{
	return fz[7]-fz[6]+(fz[5]-fz[4])+(fz[3]-fz[2])+(fz[1]-fz[0]);
}

// ȡ�õ�����Ϊֹ, �ܹ������ķ�����
int GetTdxMinute( short * fz )
{
	int  m;
	SYSTEMTIME  nowtime;
	if (g_bYesterdayFlag==1)
		return fz[7]-fz[6]+(fz[5]-fz[4])+(fz[3]-fz[2])+(fz[1]-fz[0]);
	GetCorrespondingTime( &nowtime );
	m = nowtime.wHour*60+nowtime.wMinute;  //����
    return System2Tdx( m,fz );
}

int GetTdxSectNum(short *fz)
{
	for(int i = 0;i < 8;i+=2)
		if(fz[i] == fz[i+1]) break;
	return i/2;
}

int GetNowTdxSect(short *fz)
{
	int FzNum = GetTdxMinute(fz);
	int TotalFz=0;
	for(int i=0;i<8;i+=2)
	{
		if(fz[i+1]==fz[i]) break;
		TotalFz+=fz[i+1]-fz[i];
		if(FzNum <= TotalFz) break;
	}
	return i/2;
}

int GetNowTdxSect(short nCurMin, short *fz) 
{
	for(int i=0;i<8;i+=2)
	{
		if(fz[i+1]==fz[i]) break;
		if(nCurMin >= fz[i] && nCurMin <= fz[i+1]) break;
	}
	return i/2;
}

char * GetTdxTimeStr(int itemnum)
{
	static char TdxTimeStr[20];
	sprintf(TdxTimeStr,"%02d:%02d",(itemnum/60)%24,itemnum%60);
	return TdxTimeStr;
}

char * GetTdxTimeStrFromSecond(int nSecond)
{
	static char TdxTimeStr2[20];
	sprintf(TdxTimeStr2,"%02d:%02d:%02d",nSecond/3600,(nSecond%3600)/60,nSecond%60);
	return TdxTimeStr2;
}

char * GetTimeStrFromLong(long hms)
{
	static char TdxTimeStr3[20];
	sprintf(TdxTimeStr3,"%02d:%02d:%02d",hms/10000,(hms%10000)/100,hms%100);
	return TdxTimeStr3;
}

char * GetTdxDateStr(long date)
{
	static char TdxDateStr[20];
	sprintf(TdxDateStr,"%02d/%02d",(date%10000)/100,date%100);
	return TdxDateStr;
}

char * GetTdxDateWeekStr(long date)
{
	static char TdxDateWeekStr[80];
	sprintf(TdxDateWeekStr,"%04d-%02d-%02d,%s",date/10000,(date%10000)/100,date%100,_F(strWeekInfo[GetWeek(date)]));
	return TdxDateWeekStr;
}

// fznum Ϊ 1 ��ʾ 5 ����, Ϊ 3 ��ʾ 15 �����ߵȵ�
// ����Ĺ�������ȡ�õ�����Ϊֹ, �Ѿ������˶��ٸ� 5 (��15, ��) ������
int GetMinuteXh( int lineperiod,short * fz,short mulnum)
{   int  itemnum;
    itemnum = GetTdxMinute( fz );
	int tmpRatio = anFZRatio[lineperiod];
	if(lineperiod == PER_MINN)	tmpRatio = mulnum;
    return ( itemnum + tmpRatio - 1 )/( tmpRatio );
}

int GetRealMinuteXh( int lineperiod,short * fz,short mulnum)
{   
	int  itemnum = GetRealTotalMinute(fz);
	int tmpRatio = anFZRatio[lineperiod];
	if(lineperiod == PER_MINN)	tmpRatio = mulnum;
    return ( itemnum + tmpRatio - 1 )/( tmpRatio );
}

short GetFZnoFromMinute( int m ,int lineperiod,short * fz,short mulnum)
{   int itemnum = System2Tdx(m,fz);
	int tmpRatio = anFZRatio[lineperiod];
	if(lineperiod == PER_MINN)	tmpRatio = mulnum;
    return ( itemnum + tmpRatio - 1 )/( tmpRatio );
}

//��������������̵������, ������ŵõ���ǰӦ�õķ�����
int  GetMinuteFromFZ(int xh,int fznum,short * fz)
{    int i;
     i=fz[0]+(xh+1)*fznum;
     if (i>=fz[1]) i+=fz[2]-fz[1];
     if (i>=fz[3]) i+=fz[4]-fz[3];
     if (i>=fz[5]) i+=fz[6]-fz[5];
     if (i>=fz[7]) i=fz[7];
     return i;
}

void InitStkTime(short setcode,short * fz)
{
     if (setcode==SZ)
     {    
		 fz[0]=sz[0];fz[2]=sz[2];fz[4]=sz[4];fz[6]=sz[6];
         fz[1]=sz[1];fz[3]=sz[3];fz[5]=sz[5];fz[7]=sz[7];
     }
     else
     {  
		 fz[0]=sh[0];fz[2]=sh[2];fz[4]=sh[4];fz[6]=sh[6];
		 fz[1]=sh[1];fz[3]=sh[3];fz[5]=sh[5];fz[7]=sh[7];
     }
}

void GetJBZL(void)
{
	char        str1[40],str2[40];
	DBH_Head    CONFIG;
	MemStkInfo * scodetmp;
    float *     basef;
	int			k,i;
	short		setcode;
	char		tmpstr[MAX_PATH];
	sprintf(tmpstr,"%sLData\\cache\\bd.dbf",g_WSXHStr);
	if(OpenBase(&CONFIG,tmpstr,O_BINARY|O_RDONLY) != 0)
	{
		TDX_MessageBox(NULL,"��bd.dbf�ļ�ʧ��!",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	static char JBZLtitle[][7] = 
	{
		"ZGB","GJG","FQRFRG","FRG","BG",
		"HG", "ZGG", "ZZC", "LDZC","GDZC",
		"WXZC","CQTZ","LDFZ","CQFZ","ZBGJJ", "JZC",
		"ZYSY","ZYLY","QTLY","YYLY", "TZSY","BTSY",
		"YYWSZ","SNSYTZ","LYZE","SHLY","JLY","WFPLY","TZMGJZ"
	};

	//�Ƚ���ͨ�ɱ���0
	for (i = 0;i < g_pStockIO->GetStockNum(ALL_ALL);i++)
	    (*g_pStockIO)[i]->ActiveCapital=0.0;

	for (BaseDataTop(&CONFIG);;)
	{ 
		k = GetFieldStr(&CONFIG, "GPDM", str1);
		AllTrim(str1);
		k = GetFieldStr2(&CONFIG,"SC",str2);
		if(k < 0)	setcode = -1;
		else		setcode = atol(str2);
		scodetmp = g_pStockIO->Get_MemStkInfo(str1,setcode);
		if (scodetmp != NULL && scodetmp->ActiveCapital<COMPPREC)
		{   
			k = GetFieldStr(&CONFIG, "LTAG", str1);
			scodetmp->ActiveCapital = atof(str1);
			k = GetFieldStr(&CONFIG, "GXRQ", str1);
			scodetmp->J_gxrq = atol(str1);
			k = GetFieldStr2(&CONFIG,"SSDATE",str1);
			if(k < 0)	scodetmp->J_start = 0;
			else		scodetmp->J_start = atol(str1);
            k = GetFieldStr(&CONFIG, "DY", str1);//����ʡ��
            scodetmp->J_addr=atoi(str1);
            k = GetFieldStr(&CONFIG, "HY", str1);
            scodetmp->J_hy  =atoi(str1);       //������ҵ
			k = GetFieldStr2(&CONFIG, "ZBNB", str1);
			scodetmp->J_HalfYearFlag = atof(str1);  //ȫ������Ϊ����Ϊ��λ
			if(k==-1||scodetmp->J_HalfYearFlag<COMPPREC) scodetmp->J_HalfYearFlag=12;
			basef=(float *) &(scodetmp->J_zgb);
			for (i = 0;i < 29;i++)
			{ 
				k = GetFieldStr(&CONFIG, JBZLtitle[i], str1);
				if(i>=7 && i<=27) //���е��ʲ��������λ
					*(basef+i) = atof(str1)/10.0;
				else
					*(basef+i) = atof(str1);
			}
		}
		k = Skip(1,&CONFIG);
		if (k == -1) break;
	}
	CloseBase(&CONFIG);	
	//�Ի������Ͻ��д���
	MemStkInfo *scode;
	int stockkind=0;
	for (i = 0;i < g_pStockIO->GetStockNum(ALL_ALL);i++)
	{
		scode=(*g_pStockIO)[i];
		if(!scode) continue;
		stockkind=GetStockType(scode->setcode,scode->Code);
		//��������ͬ��
		scode->J_oldjly=scode->J_oldjly/10.0;		
		float ftmp=scode->J_oldjly;
		if(ftmp<COMPPREC)
			ftmp=fabs(scode->J_jly);
		if(fabs(scode->J_oldjly)>COMPPREC && fabs(scode->J_jly)>COMPPREC)
			scode->J_oldjly=(scode->J_jly-scode->J_oldjly)*100/ftmp;
		else
			scode->J_oldjly=0;
		//������Ӫ����ͬ��
		scode->J_oldzysy=scode->J_oldzysy/10.0;
		ftmp=scode->J_oldzysy;
		if(ftmp<COMPPREC)
			ftmp=fabs(scode->J_zysy);
		if(fabs(scode->J_oldzysy)>COMPPREC && fabs(scode->J_zysy)>COMPPREC)
			scode->J_oldzysy=(scode->J_zysy-scode->J_oldzysy)*100/ftmp;
		else
			scode->J_oldzysy=0;
		//
		if(scode->ActiveCapital<COMPPREC)	scode->ActiveCapital=0;
		if(scode->J_zgb<COMPPREC)			scode->J_zgb=0;
		if((stockkind==CODE_SZGZ||stockkind==CODE_SZZQ||stockkind==CODE_SZKZHZQ||stockkind==CODE_SZGZHG
			 ||(stockkind==CODE_SHGZ||stockkind==CODE_SHZQ||stockkind==CODE_SHKZHZQ||stockkind==CODE_SHGZHG))
		  )
			memset(&(scode->J_addr),0,sizeof(float)*32+2*sizeof(short)); //��ʱ��֧��ծȯ�ȵĻ�������
	}
}

//ȡ������
void GetConceptBlock()
{
	try
	{
		short	nVersionMark,i,j,nBlockNum,nCode;
		DSK_BLOCK	*pBlock = NULL;
		CRegistry*  pRegistry = new CRegistry("Registry ver:1.0 (1999-9-28)");
		char blockfile[MAX_PATH];
		sprintf(blockfile,"%sLData\\cache\\block.dat",g_WSXHStr);
		nVersionMark = pRegistry->OpenReg(blockfile);
		memset(g_ConceptBlock,0,sizeof(MEM_BLOCK)*100);
		g_ConceptBlockNum = 0;
		if(nVersionMark!=FAIL && pRegistry->RegRead("Block\\",&nBlockNum,sizeof(short))!=FAIL && nBlockNum>0)
		{   
			nBlockNum=MIN(100,nBlockNum); //ֻȡ100�����
			TMALLOC(pBlock,sizeof(DSK_BLOCK)*nBlockNum,DSK_BLOCK);
			pRegistry->RegRead("Block\\Val",pBlock,sizeof(DSK_BLOCK)*nBlockNum);
			for(i=0;i<nBlockNum;i++)
			{	strncpy(g_ConceptBlock[i].strBlockName,pBlock[i].strBlockName,8);
				g_ConceptBlock[i].strBlockName[8] = 0;
				for(j=0;j<pBlock[i].nStockNum&&j<USER_SIZE;j++)
				{
					pBlock[i].strContent[j][6]=0;
					if((nCode=g_pStockIO->GetIndex(pBlock[i].strContent[j]))>=0)
						g_ConceptBlock[i].nContent[g_ConceptBlock[i].nStockNum++] = nCode;
				}
				g_ConceptBlockNum++;
			}
			TDEL(pBlock);
		}
		TDEL(pRegistry);
	}
	catch(...)
	{
	}
}

void GetZHBBlockNum()
{
	if(g_pLcIdxMng) g_ZHBBlockNum = g_pLcIdxMng->GetIdxNum();
	else g_ZHBBlockNum = 0;
	
	TDEL(g_ZHBBlock);
	g_ZHBBlock = new ZHB_BlockInfo_Struct[g_ZHBBlockNum+1];	
	memset(g_ZHBBlock,0,(g_ZHBBlockNum+1)*sizeof(struct ZHB_BlockInfo_Struct));

	LPLCIDXSET pLcIdxSet = NULL;
	if(g_pLcIdxMng) g_pLcIdxMng->GetIdxSet(pLcIdxSet);
	for(int i=0;i<g_ZHBBlockNum;i++)
	{
		strncpy(g_ZHBBlock[i].zhmc,pLcIdxSet[i].Name,10);			
		LPLCIDXSTK pTmpLcStk = NULL;
		long m_nLcStkNum = 0,m_nLcStkSpc = 0;
		if(g_pLcIdxMng) g_pLcIdxMng->GetIdxStk(pTmpLcStk,m_nLcStkNum,m_nLcStkSpc,i);
		for(int j=0;j<min(MAXGPNUM,m_nLcStkNum);j++)
		{
			g_ZHBBlock[i].nContent[j]=g_pStockIO->GetIndex(pTmpLcStk[j].Code,pTmpLcStk[j].nSetCode);
			g_ZHBBlock[i].nStockNum++;
		}
		TDEL(pTmpLcStk);
	}
}

void ResetMemStkOtherInfo()	//���MemStkInfo�ĳ�ʼֵ
{
    short  nStockNum,i;
    struct MemStkInfo*   pMemStockInf;
    nStockNum = g_pStockIO->GetStockNum(ALL_ALL);//�����й�Ʊ���в���
    for(i = 0; i < nStockNum; i++)
	{
		pMemStockInf = (*g_pStockIO)[i];
		pMemStockInf->AmoUnit = 1.0f;
		pMemStockInf->CloseStopFlag = 0;
		pMemStockInf->BuySellFlag = 0;
		pMemStockInf->nImgStatLastSeconds = 0;
		pMemStockInf->nTckStatLastSeconds = 0;
		pMemStockInf->nWarnCount = 0;
		pMemStockInf->J_HalfYearFlag = 12; //ȱʡΪ�걨
    }
}

BOOL Is_HQInit_Time()
{
	SYSTEMTIME  nowtime;
	GetCorrespondingTime( &nowtime );
	int m = nowtime.wHour*60+nowtime.wMinute;
	if(m < 560 && m > 520) return TRUE; //8:40-9:20��40�������������,˵���Ƿ������ڳ�ʼ��
	return FALSE;
}

BOOL	Is_Jy_Time(int nSkipMin)
{
	if(g_bYesterdayFlag==1) return FALSE;
	short nNowTime = GetCorrespondingMin();
	for(int i=0;i<4;i++)
	{
		short nSecStart = min(sh[2*i],sz[2*i])-nSkipMin;
		short nSecEnd	= max(sh[2*i+1],sz[2*i+1])+nSkipMin;
		if(nNowTime>=nSecStart && nNowTime<=nSecEnd) return TRUE;
	}
	return FALSE;
}

short GetCorrespondingMin(BOOL bDSFlag)
{  
	SYSTEMTIME  nowtime;
	if(bDSFlag) GetDSCorrespondingTime( &nowtime );
	else GetCorrespondingTime( &nowtime );
	return (nowtime.wHour*60+nowtime.wMinute);
}

long GetYMD()
{
	CTime NowTime=CTime::GetCurrentTime();
	return NowTime.GetYear()*10000+NowTime.GetMonth()*100+NowTime.GetDay();
}

long GetHMS()
{
	CTime NowTime=CTime::GetCurrentTime();
	return NowTime.GetHour()*10000L+NowTime.GetMinute()*100L+NowTime.GetSecond();
}

long GetSeconds()
{
	CTime NowTime=CTime::GetCurrentTime();
	return NowTime.GetHour()*3600+NowTime.GetMinute()*60+NowTime.GetSecond();
}

short  GetFzNum(short setcode)
{
	return g_TdxCfg.GPFZNUM;
}

long GetLsUnit(short setcode)
{ 
	short fznum,knum;
	fznum = GetFzNum(setcode);
	knum = fznum/5;
	return ((long)fznum*sizeof(struct MinuteData)+(long)knum*sizeof(struct AnalyData));
}

BOOL testzs(const char * code,short setcode)
{
	if(setcode!=SZ && setcode!=SH) 
		return FALSE;
	if(!g_bG2DT && code[0]=='8' && (code[1]=='3' || code[1]=='8'))	//����1�����������ϱ�,Ҳ����ָ��������
		return TRUE;
	if(g_bG2DT && setcode==SH && code[0]=='8')						//����2�����Ϻ���8��ʼ�Ķ���ָ��
		return TRUE;
    long l = atol(code);
	if(setcode==SH && !g_bG2DT)
	{
		if(l > 999000l || l < 30l || l == 300l || (l>=900l && l<=999l)) return TRUE;
	}
	else if(setcode==SH && g_bG2DT)
	{
		if(l >= 990000l || l <= 999) return TRUE;
	}
	else if(setcode==SZ)
	{
		if(code[0]=='3' && code[1]=='9') return TRUE;
	}
	return FALSE;
}

//�ж��Ƿ���ָ��
BOOL testzs(struct MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;
	return testzs(pStkInfo->Code,pStkInfo->setcode);
}

//�ж��Ƿ������ָ��
BOOL testhkzs(struct MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;
	if(pStkInfo->setcode==SZ || pStkInfo->setcode==SH)
		return FALSE;
	int nXH = DSGetMarketXH(pStkInfo->setcode);
	if(nXH==-1)
		return FALSE;
	return strcmp(g_pDSMarket[nXH].Jc,"FH")==0;
}

//�ж��Ƿ������Ȩ֤
BOOL testhkwarrant(struct MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;
	if(pStkInfo->setcode==SZ || pStkInfo->setcode==SH)
		return FALSE;
	int nXH = DSGetMarketXH(pStkInfo->setcode);
	if(nXH==-1)
		return FALSE;
	return strcmp(g_pDSMarket[nXH].Jc,"KR")==0;
}

//�ж��Ƿ��Ǳ��ش���
BOOL IsLCIdx(struct MemStkInfo *pStkInfo)
{
	if(pStkInfo==NULL || g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0) 
		return FALSE;
	if(pStkInfo->setcode!=SZ) return FALSE;
	if(strncmp(pStkInfo->Code,LCIDX_FIX,3)==0)
		return TRUE;
	return FALSE;
}

BOOL IsLCExt(struct MemStkInfo *pStkInfo)
{
	if(pStkInfo==NULL || g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0) 
		return FALSE;
	if(pStkInfo->setcode!=SZ) return FALSE;
	if(strncmp(pStkInfo->Code,LCEXT_FIX,3)==0)
		return TRUE;
	return FALSE;
}

BOOL IsLCPt(struct MemStkInfo *pStkInfo)
{
	if(pStkInfo==NULL || g_pLcIdxMng==NULL || g_pLcIdxMng->GetLcNum()==0) 
		return FALSE;
	if(pStkInfo->setcode!=SZ) return FALSE;
	if(strncmp(pStkInfo->Code,LCPT_FIX,3)==0)
		return TRUE;
	return FALSE;
}

BOOL IsLocalStock(struct MemStkInfo *pStkInfo)
{
	return (IsLCIdx(pStkInfo) || IsLCExt(pStkInfo) || IsLCPt(pStkInfo));
}

short need_justcjl ( short setcode, char * code )
{
    short flag = 1;
    if (!testzs(code,setcode))
    {
	    switch(GetStockType(setcode,code))
	    {
	    case CODE_SZGZ:
        case CODE_SZZQ:
        case CODE_SZKZHZQ:
        case CODE_SZGZHG:
	    case CODE_SHGZ:
        case CODE_SHZQ:
        case CODE_SHKZHZQ:
        case CODE_SHGZHG:
		case CODE_SZQZ:  //Modify 20051208 ����Ȩ֤
		case CODE_SHQZ:
		    flag = 0;
		    break;
	    }
    }
    return flag;
}

//ȡǰһ����
static char days_of_mon[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

long GetSeason1st(long date)  // �õ�ĳ���ĵ�һ��
{
	short year = date/10000;
	short mon = (date % 10000) / 100;
	if(year<1900 || year>4000 || mon<=0 || mon>12)
		return 0;
	return year*10000+(((mon-1)/3)*3+1)*100+1;
}

long GetYear1st(long date)  // �õ�ĳ��ĵ�һ��
{
	short year = date/10000;
	short mon = (date % 10000) / 100;
	if(year<1900 || year>4000 || mon<=0 || mon>12)
		return 0;
	return year*10000+101;
}

long GetMonth1st(long date)  // �õ�ĳ�µĵ�һ��
{
	short year = date/10000;
	short mon = (date % 10000) / 100;
	if(year<1900 || year>4000 || mon<=0 || mon>12)
		return 0;
	short yearmon=date/100;
	return yearmon*100+1;
}

long GetFriday(long date ) // �õ�ĳ�յ�������
{
  short  i,year,mon,day;
  long days;

  year = date / 10000;
  mon = (date % 10000) / 100;
  day = (date % 10000) % 100;
  if(year<1900 || year>4000 || mon<=0 || mon>12 || day<=0 || day>31) 
	  return 0;
  days = 0;
  for (i = 1980; i < year; i++)  {
      days += 365;
      if (i % 400 == 0 || (i % 4 == 0 && i % 100 != 0) ) // ����
         days++;
      }
  for (i = 1; i < mon; i++)  {
	  days += days_of_mon[i-1];
	  if (i == 2 && (year % 400 == 0 || (year % 4 ==0 && year % 100 != 0) ) )
         days++;
      }
  days += day - 1; // ��1980�������չ��ж�����
  days -= 3; // 1980��1��1��Ϊ���ڶ��������������������Ҫ��ȥ3
  i = days % 7;
  if (i > 2) // �õ������������ڵ������塣��Ϊ�����塢�����գ���Ϊ����
     day += (7 - i) % 7;
  i = days_of_mon[mon-1];
  if (mon == 2 && (year % 400 == 0 || (year % 4 ==0 && year % 100 != 0) ) )
     i++;
  if (day > i) {
     day -= i;
     mon++;
     }
  if (mon > 12) {
     mon = 1;
	 year++;
	 }
  return (long)year * 10000L + (long)mon * 100L + (long)day;
}

//////////////////////////////////////////////////////////////////////////
CView *OpenNewView(const char *docstr)
{
	try
	{
		//ȱʡ������ͼ
		POSITION curTemplatePos = AfxGetApp()->GetFirstDocTemplatePosition();
		while(curTemplatePos != NULL)
		{
			CDocTemplate* curTemplate =
				AfxGetApp()->GetNextDocTemplate(curTemplatePos);
			CString str;
			curTemplate->GetDocString(str, CDocTemplate::docName);
			if(str == _T(docstr))
			{
				curTemplate->OpenDocumentFile(NULL);
				//����˴���
				CFrameWnd *tempFrameWnd = (CFrameWnd *)((CView *)theViewManager.arViews[theViewManager.GetWindowNum()-1])->GetParent ();
				tempFrameWnd->ActivateFrame();
				return (CView *)theViewManager.arViews[theViewManager.GetWindowNum()-1];
			}
		}
		return NULL;
	}
	catch(...)
	{
	}
	return NULL;
}

void CloseView(const char *docstr)
{
	for(int i = 0;i<theViewManager.GetWindowNum();i++)
	{
		BOOL bRet=FALSE;
		if(strcmp(docstr,"BigMenu")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CBigMenuView));
		else if(strcmp(docstr,"HomePage")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CHomePageView));
		else if(strcmp(docstr,"Grid")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CGridView));
		else if(strcmp(docstr,"News")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CNewsView));		
		else if(strcmp(docstr,"Txt")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CTxtView));			
		else if(strcmp(docstr,"Com")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView));
		else if(strcmp(docstr,"Com2")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView2));
		else if(strcmp(docstr,"WebInfo")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CIEBrowserView));			
		else if(strcmp(docstr,"Jbfx")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CJbfxView));				
		else if(strcmp(docstr,"AdvHq")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CAdvHqView));		
		else if(strcmp(docstr,"Jj")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CJJView));
		else if(strcmp(docstr,"TopView")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CTvView));
		else if(strcmp(docstr,"Rss")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CRssView));
		if(bRet)
		{			
			CFrameWnd *tempFrameWnd = (CFrameWnd *)((CView *)theViewManager.arViews[i])->GetParent ();
			tempFrameWnd->SendMessage (WM_CLOSE);
			break;
		}
	}		
}

CView *FindViewThenActive(const char *docstr)
{
	for(int i = 0;i<theViewManager.GetWindowNum();i++)
	{
		BOOL bRet=FALSE;
		if(strcmp(docstr,"BigMenu")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CBigMenuView));
		else if(strcmp(docstr,"HomePage")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CHomePageView));		
		else if(strcmp(docstr,"Grid")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CGridView));
		else if(strcmp(docstr,"News")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CNewsView));		
		else if(strcmp(docstr,"Txt")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CTxtView));			
		else if(strcmp(docstr,"Com")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView));
		else if(strcmp(docstr,"Com2")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CComView2));
		else if(strcmp(docstr,"WebInfo")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CIEBrowserView));			
		else if(strcmp(docstr,"Jbfx")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CJbfxView));				
		else if(strcmp(docstr,"AdvHq")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CAdvHqView));		
		else if(strcmp(docstr,"Jj")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CJJView));	
		else if(strcmp(docstr,"TopView")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CTvView));
		else if(strcmp(docstr,"Rss")==0)
			bRet=((CView *)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CRssView));
		if(bRet)
		{			
			if(theViewManager.arViews[i]!=theViewManager.pActiveView)
			{
				CFrameWnd *tempFrameWnd = (CFrameWnd *)((CView *)theViewManager.arViews[i])->GetParent ();
				tempFrameWnd->ActivateFrame();
				tempFrameWnd->SetActiveView((CView *)theViewManager.arViews[i],TRUE);
			}
			return (CView *)theViewManager.arViews[i];
		}
	}	
	return NULL;
}

CView *SwitchToView(const char *docstr)
{
	if(strcmp(docstr, "HomePage"))
	{
		if(g_bHasHomePage)
		{
			if(theViewManager.pActiveView&&theViewManager.pActiveView->IsKindOf (RUNTIME_CLASS(CHomePageView)))
			{
				g_Scheme_King.HomePage = g_InitDefHomePage;
				SwitchToView("HomePage");
			}
		}
		else 
			CloseView("HomePage");
	}
	CView *pView = FindViewThenActive(docstr);
	if(!pView)
		pView = OpenNewView(docstr);	
	return pView;
}

//////////////////////////////////////////////////////////////////////////
//����������ĺ���
int strtdxcmp(const char *s1, const char *s2, size_t num)
{   
	int i;
	for (i=0;i<num;i++)
	{ if (s1[i]=='?')   continue;
	  if (s2[i]=='?')   continue;
	  if (s1[i]=='Z' && s2[i]=='C') continue;	//֧��ģ����
	  if (s1[i]=='C' && s2[i]=='Z') continue;
	  if (s1[i]=='N' && s2[i]=='L') continue;
	  if (s1[i]=='L' && s2[i]=='N') continue;
	  if (s1[i]=='X' && s2[i]=='H') continue;
	  if (s1[i]=='Q' && s2[i]=='X') continue;	//֧�ֻ�ܰʵҵ��
	  if (s1[i]!=s2[i]) return 1;
	}
	return 0;
}

//���ڼ��̾���
int GetMatchedStr(char * lpszInStr,int nFlag,int nMax,int * lpnIndex)
{
	char tmpCode[20];
    int len=strlen(lpszInStr);
	int DSize=len;
    int i,j,k=0;
    short stocknum;
    if (len==0) return 0;
	//�Ż����Զ����ݼ�
	if(g_mShortCut.size()>0)
	{
		CString tmpStr=lpszInStr;
		map<CString, ShortCut_Cfg, cmpstr >::iterator iter = g_mShortCut.find(tmpStr);
		if(iter!=g_mShortCut.end())
		{
			ShortCut_Cfg scn = (*iter).second;
			int nIndex = g_pStockIO->GetIndex(scn.RawCode,scn.setcode);
			if(nIndex>=DS_FROM)
			{
				for(i=0;i<g_pStockIO->m_nDSNum;++i)
				{
					MemStkInfo *pStkInfo = g_pStockIO->GetDSStkInfo(i);
					if(pStkInfo && pStkInfo->Index==nIndex)
					{  
						lpnIndex[k++]=i+KEYGUY_DSCODE;
						break;
					}
				}				
			}
			else if(nIndex>=0)
				lpnIndex[k++]=nIndex;
			if (k>=nMax) return k;
		}		
	}
    if (nFlag)//�������ָ��Ͱ��
    {
		//��ͼ����ģʽ
		for(i=0;i<g_MainLineNum;i++)
		{
			if(strncmp(g_MainLineName[i].KeyString,lpszInStr,len)==0)
				lpnIndex[k++]=i+KEYGUY_MAINLINE;
		}
		//g_pCmdTable�����,�����ر����Ѷ����˵ĳ���ָ��
		for (i=0;g_pCmdTable[i].Code[0]!='\0';i++)
		{
			if (!strncmp(g_pCmdTable[i].Code,lpszInStr,len))
			{
				if(g_pCmdTable[i].wParam==2 &&g_pCmdTable[i].lParam>=301 && g_pCmdTable[i].lParam<=322)
				{
					if((g_pCmdTable[i].lParam-300)<=g_nOftenUseZb)
						lpnIndex[k++]=i+KEYGUY_OPER;
				}
				else
					lpnIndex[k++]=i+KEYGUY_OPER;
			}
		}
		//g_vDynaCmdTab����
		for(j=0;j<g_vDynaCmdTab.size();j++)
			if (!strncmp(g_vDynaCmdTab[j].CmdInfo.Code,lpszInStr,len)) lpnIndex[k++]=j+i+1+KEYGUY_OPER;
		TINDEXINFO *tmpIndexInfo;
		int nTotalNum = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
        for (i=0;i<nTotalNum;i++)
		{
			tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,i);
			if (!strncmp(tmpIndexInfo->acCode,lpszInStr,len))
				lpnIndex[k++]=i+KEYGUY_ZB;
			else if (!strtdxcmp(GetPYStr(tmpIndexInfo->acCode,5),lpszInStr,len)) //֧���Ǻ������Ĺ�ʽ
				lpnIndex[k++]=i+KEYGUY_ZB;
		}
		for(i=0;i<g_nSpeclIndexNum;i++)
		{
			if (!strncmp(g_SpeclIndex[i].acCode,lpszInStr,len))
				lpnIndex[k++]=i+KEYGUY_SEPCZB;			
		}
		if (!strncmp("self",lpszInStr,len))	//��ѡ��
			lpnIndex[k++]=KEYGUY_BLOCK;
		if (!strncmp("calc",lpszInStr,len))	//��Ʊ��
			lpnIndex[k++]=KEYGUY_BLOCK+1;
		//�������Ĵ���301-399
		int i,kk,nCnt=0;
		for(i=0;i<g_GuiMarketNum;i++)
		{
			for(kk=0;kk < MAX_MKT_NUM;kk++)
			{
				int nDSMarketID = g_GuiMarketSet[i].MarketSet[kk];
				if(nDSMarketID==0) continue;
				sprintf(tmpCode,".%d",600+nDSMarketID);
				if (!strncmp(tmpCode,lpszInStr,len)) lpnIndex[k++]=nDSMarketID+KEYGUY_DSTYPE;
			}					
		}
    }
	if(nFlag)
	{
		//֧���Զ������51-59����
		char tmpstr[10];
		tmpstr[0] = '5',tmpstr[2] = 0;
		for(i=0;i<9;i++)
		{
			if(i >= g_nBlockNum) break;			
			tmpstr[1] = i+'1';
			if (!strncmp(tmpstr,lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK;
		}
		//֧���Զ�����ϵͳ���
		for (i=0;i<g_nBlockNum;i++)			//�Զ�����
			if (!strtdxcmp(g_BlockInfo[i].KeyGuyCode,lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK;
		for (i=0;i<TOTAL_DY_NUM;i++)		//�ط����
			if (!strtdxcmp(g_pDyBlockPy[i],lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK+DY_START;
		for (i=0;i<TOTAL_HY_NUM;i++)		//��ҵ���
			if (!strtdxcmp(g_pHyBlockPy[i],lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK+HY_START;
		for (i=0;i<g_ConceptBlockNum;i++)	//������
			if (!strtdxcmp(GetPYStr(g_ConceptBlock[i].strBlockName,5),lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK+CON_START;
		for (i=0;i<g_ZHBBlockNum;i++)	//��ϰ��
			if (!strtdxcmp(GetPYStr(g_ZHBBlock[i].zhmc,5),lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK+ZHB_START;
		for (i=0;i<g_pConstMng->GetZJHHYBlockNum();i++)	//֤�����ҵ���
		{
			if (!strtdxcmp(GetPYStr(g_pConstMng->GetHYFromCommandIDXH(i),5),lpszInStr,len))
				lpnIndex[k++]=i+2+KEYGUY_BLOCK+ZJHHY_START;
		}


	}
	//����������������ȴ���
	if(g_bHasDS)
	{
		stocknum=g_pStockIO->m_nDSNum;
		int nMustLen=2;
		if(!g_GC.bOnlyHK && lpszInStr[0]>='0'&&lpszInStr[0]<='9')
			nMustLen=5;
		for(i=0;i<stocknum;++i)
		{
			MemStkInfo *pStkInfo = g_pStockIO->GetDSStkInfo(i);
			if(pStkInfo && (len>=nMustLen && strstr(pStkInfo->Code,lpszInStr)) )
			{  
				lpnIndex[k++]=i+KEYGUY_DSCODE;
				if (k>=nMax) return k;
			}
		}
	}	
	//�����OnlyHK���صĻ���ֻ��ʾ�۹ɴ����
	if(!g_GC.bOnlyHK)
	{
		stocknum=g_pStockIO->GetStockNum(ALL_ALL);
		if (lpszInStr[0]>='0'&&lpszInStr[0]<='9')
		{   
			//�������λ����,���Ȱ��Ϻ���;�������λ����,���Ȱ����ڵ�
			strcpy(tmpCode,lpszInStr);
			BOOL bHasTrans=FALSE;
			if(len==4)
			{	sprintf(tmpCode,"00%s",lpszInStr);
				DSize=strlen(tmpCode);
				bHasTrans=TRUE;
			}
			else if(len==3)
			{	sprintf(tmpCode,"600%s",lpszInStr);
				DSize=strlen(tmpCode);
				bHasTrans=TRUE;
			}
			if(bHasTrans==TRUE)
			{
				for (i=0;i<stocknum;i++)
				if (!strtdxcmp((*g_pStockIO)[i]->Code,tmpCode,DSize))
				{  lpnIndex[k++]=i;
				   if (k>=nMax) return k;
				}
			}
			for (i=0;i<stocknum;i++)
			{
				if(bHasTrans && strcmp((*g_pStockIO)[i]->Code,tmpCode)==0) continue;
				if (len==1 && (*g_pStockIO)[i]->Code[0]==lpszInStr[0])
				{
					lpnIndex[k++]=i;
					if(k>=nMax) return k;
				}
				else if(len>=2 && strstr((*g_pStockIO)[i]->Code,lpszInStr))
				{  
					lpnIndex[k++]=i;
					if(k>=nMax) return k;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			//created by guo
			if(nFlag && g_pFundBase)
			{
				for(i=0;i<g_nFundNum;i++)
				{
					if (strstr(g_pFundBase[i].szSymbol,lpszInStr))
					{  
						lpnIndex[k++]=i+KEYGUY_FUND;
						if (k>=nMax) return k;
					}
				}
			}
		}
		else
		{			
			char strTmp[6],strSpellCode[SPELLCODE_SIZE];
			for (i=0;i<stocknum;i++)
			{
				memset(strTmp, 0, 6*sizeof(char));strTmp[0] = '*';
				if(!strnicmp("ST", lpszInStr, 2))
				{
					strcat(strTmp, lpszInStr);
					len = strlen(strTmp);
					if(!strtdxcmp((*g_pStockIO)[i]->SpellCode, strTmp, len))
						lpnIndex[k++]=i;
				}
				len = strlen(lpszInStr);
				strncpy(strSpellCode,(*g_pStockIO)[i]->SpellCode,SPELLCODE_SIZE-1);
				strSpellCode[SPELLCODE_SIZE-1]=0;
				if (!strtdxcmp(strSpellCode,lpszInStr,len))
				{
					lpnIndex[k++]=i;
					if (k>=nMax) return k;
				}
				else if(len>2 && strSpellCode[0]=='N' && !strtdxcmp(strSpellCode+1,lpszInStr,len))
				{
					lpnIndex[k++]=i;
					if (k>=nMax) return k;
				}
				else if(!strtdxcmp((*g_pStockIO)[i]->Name, lpszInStr,len))
				{
					lpnIndex[k++]=i;
					if (k>=nMax) return k;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			//created by guo
			if(nFlag && g_pFundBase)
			{
				char strFundName[20];
				for(i=0;i<g_nFundNum;i++)
				{
					memset(strFundName,0,20*sizeof(char));
					strcpy(strFundName,g_pFundBase[i].szName);
					len = strlen(lpszInStr);
					if (!strtdxcmp(GetPYStr(strFundName,9),lpszInStr,len))
					{  
						lpnIndex[k++]=i+KEYGUY_FUND;
						if (k>=nMax) return k;
					}
				}
			}
			//�Ծ����ļ���
			TDXSTKOLDNAME *pAllFaceInfo=NULL;
			long  nFaceNum = g_pStockIO->GetAllFaceData(pAllFaceInfo);
			if(nFaceNum>0&&pAllFaceInfo)
			{
				for(i=0;i<nFaceNum;i++)
				{
					memset(strTmp, 0, 6*sizeof(char));strTmp[0] = '*';
					if(!strnicmp("ST", lpszInStr, 2))
					{
						strcat(strTmp, lpszInStr);
						len = strlen(strTmp);
						if(!strtdxcmp(GetPYStr(pAllFaceInfo[i].Name,5), strTmp, len))
						{
							MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(pAllFaceInfo[i].Code, pAllFaceInfo[i].SetCode);
							if(pInfo&&strcmp(pAllFaceInfo[i].Name, pInfo->Name))
							{
								lpnIndex[k++]=10000+i;
								if (k>=nMax) return k;
							}
						}
					}
					len = strlen(lpszInStr);
					if (!strtdxcmp(GetPYStr(pAllFaceInfo[i].Name,5),lpszInStr,len))
					{
						MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(pAllFaceInfo[i].Code, pAllFaceInfo[i].SetCode);
						if(pInfo&&strcmp(pAllFaceInfo[i].Name, pInfo->Name))
						{
							lpnIndex[k++]=KEYGUY_OLDNAME+i;
							if (k>=nMax) return k;
						}
					}
				}
			}
		}
	}
	//��������������(������)����AB�ɴ���
	if(g_bHasDS)
	{
		stocknum=g_pStockIO->m_nDSNum;
		for(i=0;i<stocknum;++i)
		{
			MemStkInfo *pStkInfo = g_pStockIO->GetDSStkInfo(i);
			if(pStkInfo && !strtdxcmp(pStkInfo->SpellCode,lpszInStr,min(SPELLCODE_SIZE-1,len)))
			{  
				lpnIndex[k++]=i+KEYGUY_DSCODE;
				if (k>=nMax) return k;
			}
		}
	}
    return k;
}

BOOL IsFBLQ(int xh)
{   
	int i,j,k;
    for (i=1,j=1;i<=xh;)
    {   if (i==xh) return TRUE;
        k=i;
        i=i+j;
        j=k;
    }
    return FALSE;
}

CString GetBlockName(int index)
{
	CString tempStr = "";
	if( index == 0 )	
		tempStr = "��ѡ��";
	else if(index == 1)	
		tempStr = "������";
	else	
		tempStr = g_BlockInfo[index-2].Name;
	return tempStr;
}

CString GetBlockFileName(int index)
{
	CString tempStr="";
	if( index == 0 )	
		tempStr = "self";
	else if(index == 1)	
		tempStr = "calc";
	else	
		tempStr = g_BlockInfo[index-2].KeyGuyCode;
	return tempStr;
}

int GetBlockIndexFromName(const char *pBlockName)
{
	int nIndex=-1;
	if(strcmp(pBlockName,"��ѡ��")==0)
		nIndex = 0;
	else if(strcmp(pBlockName,"��Ʊ��")==0)
		nIndex = 1;
	else
	{
		for(int j=0;j<g_nBlockNum;j++)
		{
			if(strcmp(g_BlockInfo[j].Name,pBlockName)==0)
			{
				nIndex = 2+j;
				break;
			}
		}
	}
	return nIndex;
}

///////////////////////////////////////////////////////////////////////////

//�����Զ������������Ż�����
int FiltBlockCode(int blockno,short *indexbuffer)
{
	int		nRetNum = 0;
	CString tempBlockName,tempPath,tempLine;
	tempBlockName = GetBlockFileName(blockno);
	tempPath = (CString)g_strBlockNew+tempBlockName+(CString)".blk";
	CStdioFile m_File;
	if( m_File.Open(HomePath+tempPath,CFile::modeRead|CFile::shareDenyNone))
	{
		int i = 0;
		while(m_File.ReadString(tempLine) != FALSE)
		{
			TrimString(tempLine);
			if(tempLine.IsEmpty()) continue;
			indexbuffer[i] = g_pStockIO->GetIndex_FromMultiBuf(tempLine);
			if(indexbuffer[i] == -1) continue;
			i++;
		}
		nRetNum = i;
		m_File.Close();
	}
	return nRetNum;
}

int FiltPoolTJCode(int nPoolID,short *indexbuffer)
{
	int nRet = 0,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	MemStkInfo *tempInfo;
	for(int i=0;i < stocknum;i++)
	{
		tempInfo = (*g_pStockIO)[i];
		if(!tempInfo||testzs(tempInfo)) continue;
		if(TBE_MEANLESS(tempInfo->CalcRes1.fOut[nPoolID])&&tempInfo->CalcRes1.fOut[nPoolID]>COMPPREC)
			indexbuffer[nRet++] = i;
	}
	return nRet;
}

//������ҵ�͵����������Ż�����
int FiltDYHYCode(int flag,int blockno,short *indexbuffer)
{
	if(flag == 0 && (blockno< 0 || blockno >= TOTAL_DY_NUM)) return 0;
	if(flag != 0 && (blockno< 0 || blockno >= TOTAL_HY_NUM)) return 0;
	int nRet = 0,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	MemStkInfo *tempInfo;
	for(int i=0;i < stocknum;i++)
	{
		tempInfo = (*g_pStockIO)[i];
		if(!tempInfo) continue;
		if(flag == 0)
		{
			if(tempInfo->J_addr == blockno+1)//��1����
				indexbuffer[nRet++] = i;
		}
		else
		{
			if(tempInfo->J_hy == blockno+1)
				indexbuffer[nRet++] = i;
		}
	}
	return nRet;
}

int FiltHKHYCode(int blockno,short *indexbuffer)
{
	if(blockno < 0 || blockno >= g_SectorCN.size()) 
		return 0;
	char *	hycode = g_SectorCN[blockno].SectorCode;

	char	IniPath[MAX_PATH];
	wsprintf(IniPath,"%sLData\\cache\\hkblock.dat",g_WSXHStr);
	FILE *	fp=_fsopen(IniPath,"rt",SH_DENYNO);
	if(!fp) 
		return 0;

	int		nRet = 0;
	char	lpString[256];	
	short	nSetCode = DSGetMarketFromJc("KH");
	short	nSetCode2 = DSGetMarketFromJc("KR");
	BOOL	bInBlock = FALSE;
	for(rewind(fp);nRet<MAXGPNUM && !feof(fp);)
	{
		memset(lpString,0,256);
		fgets(lpString,255,fp);
		if(lpString[0]=='#'&&strcmp(hycode, &lpString[1])==0)
			bInBlock = TRUE;
		if(lpString[0]=='#'&&strcmp(hycode, &lpString[1]))
			bInBlock = FALSE;
		if(bInBlock&&lpString[0]!='#')				//��Ʊ
		{
			char *argv[3]={0};
			GetCmdLine(&lpString[0], argv, 3, ",\r\n");
			if(argv[0])
			{
				short nHKIndex = g_pStockIO->GetIndex(argv[0], nSetCode);
				if(nHKIndex<0)
				{
					nHKIndex = g_pStockIO->GetIndex(argv[0], nSetCode2);
					//��������г��֣��ţ����Զ�ƥ�����е���չ�������
					if(nHKIndex<0 && strstr(argv[0],"?")>0)
					{
						char tmpCode[100]={0};
						strcpy(tmpCode,argv[0]);
						int nLen=strlen(tmpCode);
						for(int i=0;i<nLen;i++)
							if(tmpCode[i]=='?') tmpCode[i]=0;
						nLen=strlen(tmpCode);
						if(nLen>0)
						{
							for(int k=DS_FROM;k<(DS_FROM+g_pStockIO->GetStockNum(DS_ALL)) && nRet<MAXGPNUM;k++)
							{
								MemStkInfo *pDSInfo = (*g_pStockIO)[k];
								if(pDSInfo && strncmp(pDSInfo->Code,tmpCode,nLen)==0)
									indexbuffer[nRet++] = k;
							}
						}
					}
				}
				if(nHKIndex>=0) 
					indexbuffer[nRet++] = nHKIndex;
			}
		}
	}
	fclose(fp);

	return nRet;
}

//���ݸ������������Ż�����
int FiltConceptCode(int blockno,short *indexbuffer)
{
	if(blockno < 0 || blockno >= g_ConceptBlockNum) return 0;
	int nRet=0,i,nStockType,tmpIndex,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	//
	if(strcmp(g_ConceptBlock[blockno].strBlockName,"��С��ҵ")==0)
	{
		MemStkInfo *tmpInfo;
		for (i=0;i<stocknum;i++)
		{
			tmpInfo=(*g_pStockIO)[i];
			if(tmpInfo && tmpInfo->setcode==SZ && strncmp(tmpInfo->Code,"002",3)==0)
				indexbuffer[nRet++] = i;
		}
		return nRet;
	}	
	if(strcmp(g_ConceptBlock[blockno].strBlockName,"����֤ȯ")==0)
	{
		MemStkInfo *tmpInfo;
		for (i=0;i<stocknum;i++)
		{
			tmpInfo=(*g_pStockIO)[i];
			if(tmpInfo && tmpInfo->setcode==SZ && tmpInfo->Code[0]=='4')
				indexbuffer[nRet++] = i;
		}
		return nRet;
	}
	if(strcmp(g_ConceptBlock[blockno].strBlockName,"���о�ʾ")==0)
	{
		MemStkInfo *tmpInfo;
		for (i=0;i<stocknum;i++)
		{
			tmpInfo=(*g_pStockIO)[i];
			if(tmpInfo && strstr(tmpInfo->Name,"*ST"))
				indexbuffer[nRet++] = i;
		}
		return nRet;
	}
	if(strcmp(g_ConceptBlock[blockno].strBlockName,"���¹�")==0)
	{
		MemStkInfo *tmpInfo;
		for (i=0;i<stocknum;i++)
		{
			tmpInfo=(*g_pStockIO)[i];			
			if(tmpInfo && tmpInfo->J_start>g_lOpenRq-10000 && tmpInfo->J_start!=g_lOpenRq)
			{
				nStockType = GetStockType(tmpInfo->setcode,tmpInfo->Code);
				if(nStockType==CODE_SHAG || nStockType==CODE_SZAG || nStockType==CODE_ZXB || nStockType==CODE_CYB)
					indexbuffer[nRet++] = i;
			}
		}
		return nRet;
	}
	if(strcmp(g_ConceptBlock[blockno].strBlockName,"����ͨ")==0)
	{
		MemStkInfo *tmpInfo;
		for (i=0;i<stocknum;i++)
		{
			tmpInfo=(*g_pStockIO)[i];
			if(tmpInfo && tmpInfo->setcode==SH && strncmp(tmpInfo->Code,"519",3)==0)
				indexbuffer[nRet++] = i;
		}
		return nRet;
	}
	if(strcmp(g_ConceptBlock[blockno].strBlockName,"ST���")==0)
	{
		MemStkInfo *tmpInfo;
		for (i=0;i<stocknum;i++)
		{
			tmpInfo=(*g_pStockIO)[i];
			if(tmpInfo && strstr(tmpInfo->Name,"ST"))
				indexbuffer[nRet++] = i;
		}
		return nRet;
	}
	for(i=0;i < g_ConceptBlock[blockno].nStockNum;i++)
	{
		tmpIndex = g_ConceptBlock[blockno].nContent[i];
		if(tmpIndex >= 0 && tmpIndex < stocknum)
			indexbuffer[nRet++] = tmpIndex;
	}
	return nRet;
}

//����֤�����ҵ����������Ż�����
int FiltZJHHYCode(int blockno,short *indexbuffer)
{
	if(blockno < 0 || blockno >= g_pConstMng->GetZJHHYBlockNum()) return 0;
	int nRet=0,i,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	//
	MemStkInfo *tmpInfo;
	for (i=0;i<stocknum;i++)
	{
		tmpInfo=(*g_pStockIO)[i];
		if(!tmpInfo || tmpInfo->J_zjhhy<COMPPREC) continue;
		if(g_pConstMng->GetHyXHFromHYID(tmpInfo->J_zjhhy)==blockno)
 			indexbuffer[nRet++] = i;
	}
	return nRet;
}

//������ϰ���������Ż�����
int FiltZHCode(int blockno,short *indexbuffer)
{
	if(blockno < 0 || blockno >= g_ZHBBlockNum) return 0;
	int nRet = 0,tmpIndex,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	for(int i=0;i < g_ZHBBlock[blockno].nStockNum;i++)
	{
		tmpIndex = g_ZHBBlock[blockno].nContent[i];
		if(tmpIndex >= 0 && tmpIndex < stocknum)
			indexbuffer[nRet++] = tmpIndex;
	}
	return nRet;
}

//��������֤ȯ�������Ż�����
int FiltSBCode(short *indexbuffer)
{
	int nRet=0,i,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	MemStkInfo *tmpInfo;
	for (i=0;i<stocknum;i++)
	{
		tmpInfo=(*g_pStockIO)[i];
		if(tmpInfo->setcode==SZ && tmpInfo->Code[0]=='4')
			indexbuffer[nRet++] = i;
	}
	return nRet;
}

//������ɫָ���������Ż�����
int FiltSpecIndexCode(short *indexbuffer)
{
	int nRet=0,i,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	MemStkInfo *tmpInfo;
	for (i=0;i<stocknum;i++)
	{
		tmpInfo=(*g_pStockIO)[i];
		if(strncmp(tmpInfo->Code,"88888",5)==0) continue;
		if(tmpInfo->Code[0]=='8')
			indexbuffer[nRet++] = i;
	}
	return nRet;
}

//����A+H�����Ż�����
int FiltAJHIndexCode(short *indexbuffer)
{
	int nRet=0,i;
	MemStkInfo *tmpInfo;
	for(i=0;i < g_ConceptBlockNum;i++)
	{
		if(strcmp(g_ConceptBlock[i].strBlockName, "��H��")==0)
		{
			for(int j=0;j<g_ConceptBlock[i].nStockNum;j++)
			{
				int nIDIndex = g_ConceptBlock[i].nContent[j];
				tmpInfo = (*g_pStockIO)[nIDIndex];
				if(!tmpInfo) continue;
				indexbuffer[nRet++] = nIDIndex;
			}
			break;
		}
	}
	return nRet;
}

//���ݱ��ش������Ż�����
int FiltLCCode(short *indexbuffer)
{
	int nRet=0,i,stocknum = g_pStockIO->GetStockNum(ALL_ALL);
	MemStkInfo *tmpInfo;
	for (i=0;i<stocknum;i++)
	{
		tmpInfo=(*g_pStockIO)[i];
		if(tmpInfo && (strncmp(tmpInfo->Code,LCIDX_FIX,3)==0 || strncmp(tmpInfo->Code,LCEXT_FIX,3)==0
			 || strncmp(tmpInfo->Code,LCPT_FIX,3)==0))
			indexbuffer[nRet++] = i;
	}
	return nRet;
}

MemStkInfo* GetLinkedHKStock(MemStkInfo* pAStock)
{
	short nSetCode = DSGetMarketFromJc("KH");
	STKRELATION  LinkData[100] = {0};
	short nLinkNum = g_pStockIO->GetRLTData(pAStock, LinkData, 100);
	for(int k=0;k<nLinkNum;k++)
	{
		if(strlen(LinkData[k].Code)<1) continue;
		if(strcmp(LinkData[k].Typestr, "H��")==0)
		{
			MemStkInfo *pHKStock = g_pStockIO->Get_MemStkInfo(LinkData[k].Code, nSetCode);
			return pHKStock;
		}
	}
	return NULL;
}

//ZXG��TJG��һ���ر�İ��,�ڴ˴�����WhichType
int GetGpIndex(int nBlockFlag,int WhichType,short *indexbuffer,int nMaxNum)
{
	int nRet = 0;
	//��ȡ���ù�Ʊ�����
	if(WhichType>=DS_START&&WhichType<FY_START)
		nRet = g_pStockIO->DSFiltCode(WhichType-DS_START,indexbuffer,nMaxNum);
	else if(!BeBlock(WhichType))
	{
	    int		nSetDomain;          //SZ/SH/SZ_SH
		union   flunion ftype;       //��Ʊ��������
		
		nSetDomain = lpnSysDomainType[WhichType];
		ftype.fshort = lpsSysClassType_new[WhichType];
	    //ȡʹ�ù�Ʊ
		nRet = g_pStockIO->FiltCode(nSetDomain, ftype, indexbuffer);
	}
	else if(WhichType==TYPE_AJH)				//A+H�ɶԱ�
		nRet = FiltAJHIndexCode(indexbuffer);
	else if(WhichType==TYPE_LC)
		nRet = FiltLCCode(indexbuffer);
	else if(WhichType==TYPE_SB)
		nRet = FiltSBCode(indexbuffer);
	else if(WhichType==TYPE_SPECINDEX)
		nRet = FiltSpecIndexCode(indexbuffer);
	else if(WhichType==TYPE_ZXG)
		nRet = FiltBlockCode(0,indexbuffer);
	else if(WhichType==TYPE_TJG)
		nRet = FiltBlockCode(1,indexbuffer);
	else if(WhichType==TYPE_DEFTJ_POOL)
		nRet = FiltPoolTJCode(nBlockFlag, indexbuffer);
	else if(nBlockFlag < DY_START)
		nRet = FiltBlockCode(nBlockFlag+2,indexbuffer);
	else if(nBlockFlag >= HKHY_START)	//�۹���ҵ
		nRet = FiltHKHYCode(nBlockFlag-HKHY_START,indexbuffer);
	else if(nBlockFlag >= ZJHHY_START)	//֤�����ҵ
		nRet = FiltZJHHYCode(nBlockFlag-ZJHHY_START,indexbuffer);
	else if(nBlockFlag >= ZHB_START)	//��Ϸ�����
		nRet = FiltZHCode(nBlockFlag-ZHB_START,indexbuffer);
	else if(nBlockFlag >= CON_START)	//���������
		nRet = FiltConceptCode(nBlockFlag-CON_START,indexbuffer);
	else if(nBlockFlag >= HY_START)	//��ҵ������
		nRet = FiltDYHYCode(1,nBlockFlag-HY_START,indexbuffer);
	else						//���������
		nRet = FiltDYHYCode(0,nBlockFlag-DY_START,indexbuffer);
	return nRet;
}

BOOL FindParAgForBg(char *lpAgCode, short &agSetCode, const char *lpBgCode, short bgSetCode)
{	
	int stocktype=GetStockType(bgSetCode,lpBgCode);
	if(stocktype!=CODE_SHBG&&stocktype!=CODE_SZBG) return FALSE;
	MemStkInfo *pBgStkInfo, *pAgStkInfo;
	pBgStkInfo = g_pStockIO->Get_MemStkInfo(lpBgCode, bgSetCode);
	if(!pBgStkInfo) return FALSE;
	short agIndex[MAXGPNUM];
	int cAgNum = GetGpIndex(-1,6,agIndex);
	int isOK1;
	for(int i=0;i<cAgNum;i++)
	{
		pAgStkInfo = (*g_pStockIO)[agIndex[i]];
		if(!pAgStkInfo) return FALSE;
		stocktype = GetStockType(pAgStkInfo->setcode,pAgStkInfo->Code);
		if(stocktype==CODE_SHBG||stocktype==CODE_SZBG) continue;
		isOK1 = fabs(pAgStkInfo->J_zgb-pBgStkInfo->J_zgb)<1;
		if(isOK1)
		{
			strcpy(lpAgCode, pAgStkInfo->Code);
			agSetCode = (short)pAgStkInfo->setcode;
			return TRUE;
		}
	}
	return FALSE;
}

int GetDomainGP(struct GpDomain_Struct GpDomain,short *indexbuffer)
{
	int nRetNum=0;
	if(GpDomain.flag==DOMAIN_TYPE)
		nRetNum=GetGpIndex(-1,GpDomain.which,indexbuffer);
	else if(GpDomain.flag==DOMAIN_DY)
		nRetNum=GetGpIndex(DY_START+GpDomain.which,-1,indexbuffer);
	else if(GpDomain.flag==DOMAIN_HY)
		nRetNum=GetGpIndex(HY_START+GpDomain.which,-1,indexbuffer);
	else if(GpDomain.flag==DOMAIN_CON)
		nRetNum=GetGpIndex(CON_START+GpDomain.which,-1,indexbuffer);
	else if(GpDomain.flag==DOMAIN_ZHB)
		nRetNum=GetGpIndex(ZHB_START+GpDomain.which,-1,indexbuffer);
	else if(GpDomain.flag==DOMAIN_USERBLOCK)
		nRetNum=GetGpIndex(GpDomain.which,-1,indexbuffer);
	return nRetNum;
}

int FiltMulGpDomain(struct GpDomain_Struct *GpDomain,int nSelDomain,short *indexbuffer)
{
	short  tmpIndexBuff[MAXGPNUM];
	int nRetNum=0,nTotalNum=0,i,j,k;
	for(i=0;i < nSelDomain;i++)
	{
		nRetNum=GetDomainGP(GpDomain[i],tmpIndexBuff);
		for(j=0;j<nRetNum;j++)
		{
			for(k=0;k<nTotalNum;k++)
			{
				if(indexbuffer[k]==tmpIndexBuff[j]) break;
			}
			if(k>=nTotalNum)
				indexbuffer[nTotalNum++]=tmpIndexBuff[j];
		}
	}
	return nTotalNum;
}

COLORREF GetItemColor(float f1,float f2)
{
	COLORREF tempcolor;
	if (f1-f2>COMPPREC)      
		tempcolor = GridColor.Grid_Up_Color;
	else if (f2-f1>COMPPREC) 
		tempcolor = GridColor.Grid_Down_Color;
	else            
		tempcolor = GridColor.Grid_Level_Color;
	return tempcolor;
}

CString FormatFloat2Str(float f1,int xsflag,int postfix_flag)
{
	CString str;
	char* buf=str.GetBuffer(80);
	switch (xsflag) 
	{
	case	NOXS://û��С��
		sprintf(buf,"%20.f",f1);
		break;
	case	1:
		sprintf(buf,"%20.1f",f1);
		break;
	case	XS2:
		sprintf(buf,"%20.2f",f1);
		break;
	case	XS3:
		sprintf(buf,"%20.3f",f1);
		break;
	case	XS4:
		sprintf(buf,"%20.4f",f1);
		break;
	case	XS5:
		sprintf(buf,"%20.5f",f1);
		break;
	default:
		sprintf(buf,"%20.2f",f1);
		break;
	}
	buf[20] = 0;
	Format_PostFix(postfix_flag,(f1>COMPPREC),buf,80);
	str.ReleaseBuffer();
	return str;
}
//*********************************************************//
//����ͼ�о������õ��ĺ���

short Get_SS_Day(short Index)
{
	MemStkInfo *tmpStkInfo=(*g_pStockIO)[Index];
	if(!tmpStkInfo) return 0;
	int ltmp=SubLongTime(g_iToday,tmpStkInfo->J_start);
	return max(ltmp,0);
}

double PointToLine(int x,int y,int x1,int y1,int x2,int y2)
{
	if(x1==x2 && y1==y2)
		return(sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1)));
	double Dx=double(x2-x1),Dy=double(y2-y1);
	double MinDis = pow((Dx*(y-y1)-Dy*(x-x1)),2);
	MinDis = MinDis/(Dx*Dx+Dy*Dy);
	MinDis = sqrt(MinDis);
	return MinDis;
}

DWORD on_filelength(FILE * ftmp)
{
	DWORD oldpos,filelen;
	oldpos = ftell(ftmp);
   	fseek(ftmp,0L,SEEK_END);
	filelen = ftell(ftmp);
	fseek(ftmp,oldpos,SEEK_SET);
	return filelen;
}

int GetWeek(long    Date)
// ����YYYYMMDD(����)
//����������Чֵ��ΧΪ(0-6)��0��ʾ������
{
	short  i,nYear,nMonth,nDay;
	long AllDays;
	nYear = Date / 10000;
	nMonth = (Date % 10000) / 100;
	nDay = (Date % 10000) % 100;
	if(nYear<1900 || nYear>4000 || nMonth<=0 || nMonth>12 || nDay<=0 || nDay>31) 
		return 0;
	AllDays = 0;
	for (i = 1980; i < nYear; i++)  {
	  AllDays += 365;
	  if (i % 400 == 0 || (i % 4 == 0 && i % 100 != 0) ) // ����
		 AllDays++;
	  }
	for (i = 1; nMonth>0 && nMonth<13 && i < nMonth; i++)  {
	  AllDays += days_of_mon[i-1];
	  if (i == 2 && (nYear % 400 == 0 || (nYear % 4 ==0 && nYear % 100 != 0) ) )
		 AllDays++;
	  }
	AllDays += nDay + 1; // ��1980�������չ��ж�����		Modify 1999.12.13.AM
	// 1980��1��1��Ϊ���ڶ��������������������Ҫ��ȥ3
	AllDays = max(0,AllDays);
	return  (AllDays % 7);
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

float GetTPPrice(MemStkInfo *tmpInfo,float Close,BOOL bUp)
{
	if(!tmpInfo) return 0.0;

	float fRet,fStep,fUnit = 100.0;
	if (GetXSFlag(tmpInfo) == 3) 
		fUnit = 1000.0;
	int Type=GetStockType(tmpInfo->setcode,tmpInfo->Code);

	if(tmpInfo->Name[0]=='N') //���Ƶ�һ��Ĺ�Ʊ(����ͣ����)
	{
		fRet  = 0.0;
	}
	else if(strstr(tmpInfo->Name,"ST") || tmpInfo->Name[0]=='S') //����ST�Ĺ�Ʊ
	{	if(bUp)	
		{
			fStep = (int)(Close * 0.05 * fUnit + 0.5+0.001)/fUnit;
			fRet  = (int)((Close + fStep)*fUnit+0.5+0.001)/fUnit;
		}
		else	
			fRet  = (int)(Close * 0.95 * fUnit + 0.5+0.001)/fUnit;
	}
	else if(Type==CODE_SB)	//����
	{	if (bUp) 
		{
			fStep = (int)(Close * 0.05 * fUnit + 0.5+0.001)/fUnit;
			fRet  = (int)((Close + fStep)*fUnit+0.5+0.001) /fUnit;
		}
		else	
			fRet  = (int)(Close * 0.95 * fUnit + 0.5+0.001)/fUnit;
	}
	else if(Type==CODE_SZQZ || Type==CODE_SHQZ)			//Ȩ֤
	{
		fRet = 0.0;
		STKRELATION  LinkData[100] = {0};
		short nLinkNum = g_pStockIO->GetRLTData(tmpInfo, LinkData, 100);
		for(int i=0;i<nLinkNum;i++)
		{
			MemStkInfo *pInfo = g_pStockIO->Get_MemStkInfo(LinkData[i].Code, LinkData[i].SetCode);
			if(!pInfo) continue;
			float xqbl=tmpInfo->J_wxzc; //Ȩ֤��J_wxzc�зŵ�����Ȩ����
			if(xqbl<COMPPREC) xqbl=1;
			int SAType=GetStockType(pInfo->setcode,pInfo->Code);
			if(SAType==CODE_SHAG)
			{
				float agClose = pInfo->Close;
				float qzClose = tmpInfo->Close;
				if(agClose<COMPPREC||qzClose<COMPPREC) continue;
				if(bUp)
					fRet = qzClose+(GetTPPrice(pInfo,agClose,TRUE)-agClose)*1.25*xqbl;
				else 
					fRet = qzClose-(agClose-GetTPPrice(pInfo,agClose,FALSE))*1.25*xqbl;
				float fQzUnit = 1000.0;
				fRet = (int)(fRet*fQzUnit+0.5+0.01)/fQzUnit;
				if(fRet<COMPPREC) fRet = 0.0;
			}
			if(SAType==CODE_SZAG || SAType==CODE_ZXB || SAType==CODE_CYB)
			{
				float agClose = pInfo->Close;
				float qzClose = tmpInfo->Close;
				if(agClose<COMPPREC||qzClose<COMPPREC) continue;
				float fQzUnit = 1000.0;
				if(bUp)
				{
					float fStep1 = (int)(agClose * 0.10 * fQzUnit + 0.5+0.001)/fQzUnit;
					float fRet1  = (int)((agClose + fStep1)*fQzUnit+0.5+0.001) /fQzUnit;
					fRet = qzClose+(fRet1-agClose)*1.25*xqbl;
				}
				else 
				{
					float fRet1  = (int)(agClose * 0.90 * fQzUnit + 0.5+0.001)/fQzUnit;
					fRet = qzClose-(agClose-fRet1)*1.25*xqbl;
				}
				fRet = (int)(fRet*fQzUnit+0.5+0.01)/fQzUnit;
				if(fRet<COMPPREC) fRet = 0.001f;
			}
		}	
		//�������Ȩ֤������ɼ���1Ԫ���£�����ͣ�۰�:10%����,1ë����:20%,1������:50%
		if(Type==CODE_SZQZ && bUp && tmpInfo->Close>COMPPREC && tmpInfo->Close<1.000)
		{
			if(tmpInfo->Close<0.01)
			{
				fStep = (int)(Close * 0.50 * fUnit + 0.5+0.001)/fUnit;
				fRet  = (int)((Close + fStep)*fUnit+0.5+0.001) /fUnit;
			}
			else if(tmpInfo->Close<0.1)
			{
				fStep = (int)(Close * 0.20 * fUnit + 0.5+0.001)/fUnit;
				fRet  = (int)((Close + fStep)*fUnit+0.5+0.001) /fUnit;
			}
			else
			{
				fStep = (int)(Close * 0.10 * fUnit + 0.5+0.001)/fUnit;
				fRet  = (int)((Close + fStep)*fUnit+0.5+0.001) /fUnit;
			}
		}
	}
	else if(Type==CODE_SZQZ || Type==CODE_SZGZ || Type==CODE_SZZQ || Type==CODE_SZKZHZQ || Type==CODE_SZGZHG 
		 || Type==CODE_SHQZ || Type==CODE_SHGZ || Type==CODE_SHZQ || Type==CODE_SHKZHZQ || Type==CODE_SHGZHG)//��ծ��
	{
		fRet = 0.0;
	}
	else if(Type==CODE_CYB)
	{
		float fCYBZDRatio=0.10;
		if(fCYBZDRatio<COMPPREC)
			fRet = 0.0;
		else if (bUp) 
		{
			fStep = (int)(Close * fCYBZDRatio * fUnit + 0.5+0.001)/fUnit;
			fRet  = (int)((Close + fStep)*fUnit+0.5+0.001) /fUnit;
		}
		else	
			fRet  = (int)(Close * (1-fCYBZDRatio) * fUnit + 0.5+0.001)/fUnit;
	}
	else												   //����
	{	if (bUp) 
		{
			fStep = (int)(Close * 0.10 * fUnit + 0.5+0.001)/fUnit;
			fRet  = (int)((Close + fStep)*fUnit+0.5+0.001) /fUnit;
		}
		else	
			fRet  = (int)(Close * 0.90 * fUnit + 0.5+0.001)/fUnit;
	}
	return fRet;
}

int  GetGPZS(int iNo)	//�õ�ĳ��Ʊ��Ӧ��ָ��
{
	MemStkInfo * tmpInfo = (*g_pStockIO)[iNo];
	if(!tmpInfo) 
		return -1;
	if(tmpInfo->setcode==SZ)
		return g_pStockIO->GetIndex(SZ_DPCODE,SZ);
	else if(tmpInfo->bigmarket==HKGP_TYPE)
		return g_pStockIO->GetIndex(HK_DPCODE,HK_DPSETCODE);
	else
		return g_pStockIO->GetIndex(SH_DPCODE,SH);
}

char *  GetSetStr(int setcode)
{   
	static char pathname[4][3]={"sz","sh","--","ds"};
	if(setcode>1) setcode = 3;
	return pathname[setcode];
}

char *GetCacheExt(short Type)
{
	static char *pCacheExt[3] = {"day","wek","mon"};
	switch(Type)
	{
	case PER_WEEK:	return pCacheExt[1];
	case PER_MONTH:	return pCacheExt[2];
	default:		return pCacheExt[0];
	}
}

//�������ݴ��� ��NTFS��Ȩ�޿����¶�ȡ���ܻ����.
//dataflag 0��ʾ������ 1000:ȫ������ 1:�����ڼ�����������(�������ߵ�������Ч)
short ReadCacheData(MemStkInfo *scode,int setcode,struct AnalyData *AnalyDatap,int ItemNum,short Type,short dataflag)
{
	short nRet = 0;
	if(scode && dataflag!=0 && Type==PER_DAY)
	{
		char string[256];
		BOOL bDSStk = g_pStockIO->BeDSStock(scode);
		if(bDSStk) sprintf(string,"%scache\\%d#%s.%s",g_WSXHStr,(scode->setcode!=32)?scode->setcode:31,scode->Code,GetCacheExt(Type));
		else sprintf(string,"%scache\\%s%s.%s",g_WSXHStr,GetSetStr(setcode),scode->Code,GetCacheExt(Type));
		FILE * tmpfp = _fsopen( string, "rb",_SH_DENYNO);
		if(tmpfp != NULL)
		{
			struct CacheFile_Head cachehead;
			if(fread(&cachehead, sizeof(CacheFile_Head),1,tmpfp) == 1)
			{
				if(dataflag==1000 && cachehead.dataflag==1000)
					nRet = fread(AnalyDatap,sizeof(struct AnalyData),(long)ItemNum,tmpfp);
				else if(dataflag <= cachehead.dataflag)
				{
					fseek(tmpfp,(dataflag-1)*RECENTNUM*sizeof(struct AnalyData),SEEK_CUR);
					nRet = fread(AnalyDatap,sizeof(struct AnalyData),(long)ItemNum,tmpfp);
				}
			}
			fclose(tmpfp);
		}
	}
	return nRet;
}

void WriteCacheData(MemStkInfo *scode,int setcode,struct AnalyData *AnalyDatap,int ItemNum,short Type,short dataflag)
{
	//Զ�̰�(�������칫��)д�����ص�LData��ȥ
	if(ItemNum>0 && Type==PER_DAY )
	{
		if(dataflag==0 && strcmp(scode->Code,"000001")==0 && scode->setcode==SH) return; //sh000001��Ϊһ���ر�ı�ʶ���ص㣬���ʱ����д����
		
		int i;
		struct AnalyData *tmpAnalyDatap = new struct AnalyData[ItemNum]; //Ҫ���¸���һ��,���ܸ�дAnalyDatap
		for(i=0;i < ItemNum;i++)
			tmpAnalyDatap[i] = AnalyDatap[i];
		//ע��:��Ҫת�������ߵĴ��̸�ʽ
		short stockkind = GetStockType(scode->setcode,scode->Code);
		int flag=(GetXSFlag(scode)==XS3)?1000:100;
		if(stockkind==CODE_SZGZHG ||stockkind==CODE_SHGZHG)
			flag = 100;
		for (i=0; i<ItemNum; i++)
		{  //תΪС��
		   tmpAnalyDatap[i].l_Open  = flag * (tmpAnalyDatap[i].Open)+0.5;
		   tmpAnalyDatap[i].l_Close = flag * (tmpAnalyDatap[i].Close)+0.5;
		   tmpAnalyDatap[i].l_High  = flag * (tmpAnalyDatap[i].High)+0.5;
		   tmpAnalyDatap[i].l_Low   = flag * (tmpAnalyDatap[i].Low)+0.5;
		}
		////
		if(stockkind==CODE_SZJJ || stockkind==CODE_SHJJ)
		{
			for(i=0;i<ItemNum&&tmpAnalyDatap[i].Time.Date<20030303;++i)
			{
				tmpAnalyDatap[i].Open  /= 10;
				tmpAnalyDatap[i].Close /= 10;
				tmpAnalyDatap[i].High  /= 10;
				tmpAnalyDatap[i].Low   /= 10;
			}
		}
		////
	//	if(tmpAnalyDatap[ItemNum-1].Time.Date >= g_iToday) //�����������
	//		ItemNum-=1;
		if(ItemNum>0)
			AddToVipdocDayData(scode,tmpAnalyDatap,ItemNum,PER_DAY);
		delete tmpAnalyDatap;
	}
}

short GetMulNum(short Type)
{
	if(Type == PER_DAYN) return g_nDaynNum;
	if(Type == PER_MINN) return g_nMinnNum;
	return 1;
}

void GetQZData(MemStkInfo *pStkInfo,CurrStockData *pHQ,float &fGgbl,float &fInnerVal,float &fYjl)
{
	fGgbl = 0.0;
	fInnerVal = 0.0;
	fYjl = 0.0;
	if(!pStkInfo || !pHQ || pStkInfo->J_tzmgjz<COMPPREC) return;
	int nType = GetQZType(pStkInfo);
	MemStkInfo *LinkInfo=GetQZLinkNow(pStkInfo);
	if(nType!=0 && LinkInfo)
	{
		float fLinkNow = g_pStockIO->GetStockNow(LinkInfo);
		if(fLinkNow<COMPPREC)
			fLinkNow = g_pStockIO->GetStockClose(LinkInfo);
		float fNow = pHQ->Now;
		if(fNow<COMPPREC) fNow = pHQ->Close;
		float xqbl=pStkInfo->J_wxzc; //Ȩ֤��J_wxzc�зŵ�����Ȩ����
		if(xqbl<COMPPREC) xqbl=1;
		if(fNow>COMPPREC) 
			fGgbl = fLinkNow/(fNow/xqbl);
		if(nType==1)		//�Ϲ�Ȩ֤
		{
			fInnerVal = (fLinkNow - pStkInfo->J_tzmgjz)*xqbl;
			if(fNow>COMPPREC&&fLinkNow>COMPPREC)
				fYjl = 100.0f*((pStkInfo->J_tzmgjz+fNow/xqbl)/fLinkNow-1.0f);
		}
		else if(nType==-1)	//�Ϲ�Ȩ֤
		{
			fInnerVal = (pStkInfo->J_tzmgjz - fLinkNow)*xqbl;
			if(fNow>COMPPREC&&fLinkNow>COMPPREC)
				fYjl = 100.0f*(1.0f-(pStkInfo->J_tzmgjz-fNow/xqbl)/fLinkNow);
		}
	}
}

/************************************************************************************/
//			�������������ʹ�ԭʼ���ݸ�ʽ���Calcģ����������ݸ�ʽ
//			nSetCode��ʱ����0,�������������ݽӿ�
/************************************************************************************/
BOOL RawData2CalcData(char * Code,short nSetCode,short DataType,void *pRawData,void *pCalcData,short nDataNum,NTime tFrom)
{
	if(nDataNum<1 || !pRawData || !pCalcData) return FALSE;
	BOOL bZs = testzs(Code,nSetCode);
	int i;
	switch(DataType)
	{
	case REPORT_DAT:	//ʵʱ����
		{
			struct CurrStockData *pHq = (struct CurrStockData *)pRawData;
			LPREPORTDAT pReport = (LPREPORTDAT)pCalcData;
			pReport->ItemNum = pHq->ItemNum;
			pReport->Close = pHq->Close;
			pReport->Open = pHq->Open;
			pReport->Max = pHq->Max;
			pReport->Min = pHq->Min;
			pReport->Now = pHq->Now;
			pReport->Volume = pHq->Volume;
			pReport->NowVol = pHq->NowVol;
			pReport->Amount = pHq->Amount;
			pReport->Inside = pHq->Volume-pHq->Outside;
			pReport->Outside = pHq->Outside;
			pReport->TickDiff = pHq->TickDiff;
			pReport->InOutFlag = pHq->Flag.InOutFlag;
			if(!bZs)
			{
				(pReport->Other).Ggpv.Buy = pHq->Buyp[0];
				(pReport->Other).Ggpv.Sell = pHq->Sellp[0];
				for(i=0;i<3;i++) (pReport->Other).Ggpv.Buyp[i] = pHq->Buyp[i];
				for(i=0;i<3;i++) (pReport->Other).Ggpv.Buyv[i] = pHq->Buyv[i];
				for(i=0;i<3;i++) (pReport->Other).Ggpv.Sellp[i] = pHq->Sellp[i];
				for(i=0;i<3;i++) (pReport->Other).Ggpv.Sellv[i] = pHq->Sellv[i];
			}
			else
			{
				(pReport->Other).Zspv.LxValue = pHq->Lead;
				(pReport->Other).Zspv.Yield = pHq->Yield_VolInStock.Yield;
				(pReport->Other).Zspv.UpHome = pHq->Buyv[0];
				(pReport->Other).Zspv.DownHome = pHq->Sellv[0];
				(pReport->Other).Zspv.JJAmount = pHq->Buyp[0];
				(pReport->Other).Zspv.ZQAmount = pHq->Buyp[1];
				(pReport->Other).Zspv.GZAmount = pHq->Buyp[2];
				(pReport->Other).Zspv.QZAmount = pHq->Sellp[0];
				(pReport->Other).Zspv.QTAmount = pHq->Sellp[1];
			}
			break;
		}
	case STKINFO_DAT:	//��Ʊ��Ϣ
		{
			struct MemStkInfo *pStkInfo = (struct MemStkInfo *)pRawData;
			LPSTOCKINFO pInfo = (LPSTOCKINFO)pCalcData;
			memset(pInfo,0,sizeof(STOCKINFO));
			strncpy(pInfo->Name,pStkInfo->Name,8); //ע��˴�,����Խ��
			pInfo->Unit = pStkInfo->Unit;
			pInfo->VolBase = pStkInfo->VolBase;
			pInfo->cdp = pStkInfo->cdp;
			pInfo->nh = pStkInfo->nh;
			pInfo->ah = pStkInfo->ah;
			pInfo->al = pStkInfo->al;
			pInfo->dkflag = pStkInfo->dkflag;
			pInfo->tbp = pStkInfo->tbp;
			pInfo->stoplost = pStkInfo->stoplost;
			pInfo->leave = pStkInfo->leave;
			pInfo->ActiveCapital = pStkInfo->ActiveCapital*10000;
			pInfo->J_start = pStkInfo->J_start;
			pInfo->J_addr = pStkInfo->J_addr;
			pInfo->J_hy = pStkInfo->J_hy;
			pInfo->J_zgb = pStkInfo->J_zgb*10000;
			pInfo->J_zjhhy = 0;
			pInfo->J_oldjly = 0;
			pInfo->J_oldzysy = 0;
			pInfo->J_bg = pStkInfo->J_bg*10000;
			pInfo->J_hg = pStkInfo->J_hg*10000;
			pInfo->J_mgsy2 = 0;
			pInfo->J_zzc = pStkInfo->J_zzc*10000.0;
			pInfo->J_ldzc = pStkInfo->J_ldzc*10000.0;
			pInfo->J_gdzc = pStkInfo->J_gdzc*10000.0;
			pInfo->J_wxzc = pStkInfo->J_wxzc*10000.0;
			pInfo->J_cqtz = pStkInfo->J_cqtz*10000.0;
			pInfo->J_ldfz = pStkInfo->J_ldfz*10000.0;
			pInfo->J_cqfz = pStkInfo->J_cqfz*10000.0;
			pInfo->J_zbgjj = pStkInfo->J_zbgjj*10000.0;
			pInfo->J_jzc = pStkInfo->J_jzc*10000.0;
			pInfo->J_zysy = pStkInfo->J_zysy*10000.0;
			pInfo->J_zyly = pStkInfo->J_zyly*10000.0;
			pInfo->J_yszk = pStkInfo->J_yszk*10000.0;
			pInfo->J_yyly = pStkInfo->J_yyly*10000.0;
			pInfo->J_tzsy = pStkInfo->J_tzsy*10000.0;
			pInfo->J_jyxjl = pStkInfo->J_jyxjl*10000.0;
			pInfo->J_zxjl = pStkInfo->J_zxjl*10000.0;
			pInfo->J_ch = pStkInfo->J_ch*10000.0;
			pInfo->J_lyze = pStkInfo->J_lyze*10000.0;
			pInfo->J_shly = pStkInfo->J_shly*10000.0;
			pInfo->J_jly = pStkInfo->J_jly*10000.0;
			pInfo->J_wfply = pStkInfo->J_wfply*10000.0;
			pInfo->J_tzmgjz = pStkInfo->J_tzmgjz;
			//���Ӳ���
			if(pStkInfo->J_jzc > COMPPREC)	
				pInfo->J_jyl = pStkInfo->J_jly/pStkInfo->J_jzc*100.0;
			else						
				pInfo->J_jyl = 0.00;
			int type=GetStockType(pStkInfo->setcode,pStkInfo->Code);
			if(type==CODE_SZQZ || type==CODE_SHQZ)
			{
				pInfo->J_mgwfp = 0.00;
				pInfo->J_mgsy = 0.00;
				pInfo->J_mggjj = 0.00;
				pInfo->J_mgjzc = 0.00;
				CurrStockData tmpHQ;
				g_pStockIO->ReadCurrStockData(pStkInfo,&tmpHQ);
				GetQZData(pStkInfo,&tmpHQ,pInfo->J_mgwfp,pInfo->J_mgsy,pInfo->J_mggjj);
			}
			else if(pStkInfo->J_zgb > 1)	
			{
				pInfo->J_mgwfp = pStkInfo->J_wfply/pStkInfo->J_zgb;
				pInfo->J_mgsy = (pStkInfo->J_jly*12/pStkInfo->J_HalfYearFlag)/pStkInfo->J_zgb; //��Ҫ��������ӯ��,�����ÿ������,���д�
				pInfo->J_mggjj = pStkInfo->J_zbgjj/pStkInfo->J_zgb;
				pInfo->J_mgjzc = pStkInfo->J_jzc/pStkInfo->J_zgb;
			}
			else
			{
				pInfo->J_mgwfp = 0.00;
				pInfo->J_mgsy = 0.00;
				pInfo->J_mggjj = 0.00;
				pInfo->J_mgjzc = 0.00;
			}
			if(fabs(pInfo->J_mgjzc)<COMPPREC)
				pInfo->J_mgjzc = pInfo->J_tzmgjz;			
			if(pStkInfo->J_zzc > COMPPREC)	
				pInfo->J_gdqyb = pStkInfo->J_jzc/pStkInfo->J_zzc;
			else						
				pInfo->J_gdqyb = 0.00;
			break;
		}
	case PER_MIN1:	//�����������
	case PER_MIN5:
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:
	case PER_MINN:
	case PER_DAY:
	case PER_WEEK:
	case PER_MONTH:
	case PER_DAYN:
	case PER_SEASON:
	case PER_YEAR:
		{
			struct AnalyData *pAnalyData = (struct AnalyData *)pRawData;
			LPHISDAT pHisData = (LPHISDAT)pCalcData;
			BOOL bSpec = UseSpecVolumeUnit(GetStockType(nSetCode,Code),DataType);
			for(i=0;i<nDataNum;i++)
			{
				pHisData[i].Time.year = (pAnalyData[i].Time.Date)/10000;
				pHisData[i].Time.month = ((pAnalyData[i].Time.Date)%10000)/100;
				pHisData[i].Time.day = ((pAnalyData[i].Time.Date)%10000)%100;
				if(DataType==PER_MIN1||DataType==PER_MIN5||DataType==PER_MIN15||DataType==PER_MIN30||DataType==PER_HOUR||DataType==PER_MINN)
				{
					pHisData[i].Time.year = pAnalyData[i].Time.Daye.Year+2004;
					pHisData[i].Time.month = pAnalyData[i].Time.Daye.Day/100;
					pHisData[i].Time.day =  pAnalyData[i].Time.Daye.Day%100;
					pHisData[i].Time.hour = pAnalyData[i].Time.Daye.Minute/60;
					pHisData[i].Time.minute = pAnalyData[i].Time.Daye.Minute%60;
				}
				else
					pHisData[i].Time.hour = pHisData[i].Time.minute = 0;
				pHisData[i].Time.second = 0;
				memcpy(&pHisData[i].Open,&pAnalyData[i].Open,5*sizeof(float));
				pHisData[i].fVolume = pAnalyData[i].Volume;
				pHisData[i].YClose = pAnalyData[i].YClose;
				pHisData[i].zd.up = pAnalyData[i].zd.up;
				pHisData[i].zd.down = pAnalyData[i].zd.down;
				if(nSetCode!=SZ && nSetCode!=SH)
				{
				}
				else if(!bZs || (bZs && pHisData[i].Time.year < 1997)) //��һЩ������ǵ������޳�.
				{
					pHisData[i].zd.up = 0;
					pHisData[i].zd.down = 0;
				}
				//������ر�����,����*100
				if(bSpec)
					pHisData[i].fVolume=100.0*pHisData[i].fVolume;
			}
			break;
		}
	case GBINFO_DAT:	//�ɱ���Ϣ����ת��
		break;
	default:
		break;
	}
	return TRUE;
}

CURFXTZB g_CurFxtZb;
/******************************************************************************************************************/
//  ���ڷ������ݣ�
//	����-1,��ʾʧ�ܣ�������ʾ���ӵĸ�����õ���ʵ�����ݸ���
//	nDataNumΪ-1����ʾ��Ҫ�������ݸ���
//		���tFrom,tEndȫΪ0���򷵻�ȫ�����ݸ���
//		���tFrom��ֵ,tEndΪ0���򷵻�tFrom�Ժ�����ݸ���
//		���tEnd��ֵ,tFromΪ0���򷵻�tEnd��ǰ�����ݸ���
//		���tFrom,tEndȫ��ֵ���򷵻ظ��������ݸ���
//	nDataNum��Ϊ-1����ʾʵ��Ҫ����
//		���tFrom,tEndȫΪ0�����ĩβ����nDataNum������
//		���tFrom��ֵ,tEndΪ0��������tFrom�Ժ������
//		���tEnd��ֵ,tFromΪ0��������tEnd��ǰ������
//		���tFrom,tEndȫ��ֵ�������������tEndǰ���ݸ���

//  ����Tick���ݺ�BSP����
//  nDataNumΪ-1,��ʾ��Ҫ�������ݸ���(ֻ�õ�tFrom����)
//	nDataNum��Ϊ-1����ʾʵ��Ҫ����
 
/******************************************************************************************************************/
long CALLBACK CalcCallBackFunc(char * Code,short nSetCode,short DataType,void * pData/*�ڲ�����*/,short nDataNum,NTime tFrom,NTime tEnd,BYTE nTQ,unsigned long nReserved)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState()); 

	MemStkInfo *pStkInfo=NULL;
	BOOL bZs = FALSE;
	if(DataType!=ALLGPINFO_DAT && DataType!=FLGINFO_DAT && DataType!=DYGINFO_DAT &&DataType!=HYGINFO_DAT && DataType!=GNGINFO_DAT && DataType!=ZDGINFO_DAT && DataType!=ZHGINFO_DAT && DataType!=SYSINFO_DAT && DataType!=CUR_FXTZB_DAT && DataType!=CURGPINDEX_DAT && DataType!=GPINFO_DAT && DataType!=JYINFO_DAT && DataType!=WINRECT_DAT)
	{
		pStkInfo=g_pStockIO->Get_MemStkInfo(Code,nSetCode);
		if(!pStkInfo) 
			return -1;
		bZs = testzs(Code,nSetCode);
	}
	int nRet=0;
	if(tFrom.year < 1990 && tFrom.year != 0) tFrom.year=1990; //����������ʱ�䣬������CTime�������,��ʱ��һ��Ҫ������0,��Ϊ0���ڻ�������
	if(tEnd.year < 1990 && tEnd.year != 0) tEnd.year=1990;
	switch(DataType)
	{
	case REPORT_DAT:	//ʵʱ����
		{
			LPREPORTDAT pReport = (LPREPORTDAT)pData;
			struct CurrStockData hq;
			g_pStockIO->ReadCurrStockData(pStkInfo,&hq);
			if(RawData2CalcData(Code,nSetCode,DataType,&hq,pReport,1,tFrom))
				nRet = 1;
			break;
		}
	case STKINFO_DAT:	//��Ʊ��Ϣ
		{
			LPSTOCKINFO pInfo = (LPSTOCKINFO)pData;
			if(RawData2CalcData(Code,nSetCode,DataType,pStkInfo,pInfo,1,tFrom))
				nRet = 1;
			break;
		}
	case PER_MIN1:	//�����������
	case PER_MIN5:
	case PER_MIN15:
	case PER_MIN30:
	case PER_HOUR:
	case PER_MINN:
	case PER_DAY:
	case PER_WEEK:
	case PER_MONTH:
	case PER_DAYN:
	case PER_SEASON:
	case PER_YEAR:
		{
			if(nDataNum == -1)
			{
				nRet = g_pStockIO->GetAnalyDataNum(pStkInfo,DataType,GetMulNum(DataType));
				if(DataType!=PER_DAY && DataType!=PER_WEEK && DataType!=PER_MONTH && DataType!=PER_DAYN && DataType!=PER_SEASON && DataType!=PER_YEAR)
					break;
				int Multiple=1;
				switch(DataType)
				{
					case PER_WEEK:Multiple=5;break;
					case PER_MONTH:Multiple=24;break;
					case PER_DAYN:Multiple=g_nDaynNum;break;
					case PER_SEASON:Multiple=24*3;break;
					case PER_YEAR:Multiple=24*12;break;
				}
				if(memcmp(&tFrom,&g_NoTime,sizeof(NTime)) && !memcmp(&tEnd,&g_NoTime,sizeof(NTime)))
				{
					CTime FromTime(tFrom.year,tFrom.month,tFrom.day,tFrom.hour,tFrom.minute,tFrom.second);
					CTimeSpan timeSpan = g_tNowTime-FromTime;
					nRet = MIN(nRet,timeSpan.GetDays()/Multiple);
				}
				if(!memcmp(&tFrom,&g_NoTime,sizeof(NTime)) && memcmp(&tEnd,&g_NoTime,sizeof(NTime)))
				{
					CTime EndTime(tEnd.year,tEnd.month,tEnd.day,tEnd.hour,tEnd.minute,tEnd.second);
					CTimeSpan timeSpan = g_tNowTime-EndTime;
					nRet = MAX(0,(nRet-timeSpan.GetDays())/Multiple);
				}
				if(memcmp(&tFrom,&g_NoTime,sizeof(NTime)) && memcmp(&tEnd,&g_NoTime,sizeof(NTime)))
				{
					CTime FromTime(tFrom.year,tFrom.month,tFrom.day,tFrom.hour,tFrom.minute,tFrom.second);
					CTime EndTime(tEnd.year,tEnd.month,tEnd.day,tEnd.hour,tEnd.minute,tEnd.second);
					CTimeSpan timeSpan = EndTime-FromTime;
					nRet = MIN(nRet,timeSpan.GetDays()/Multiple);
				}
				break;
			}
			if(nDataNum < 0) break;
			if(!pData) break;
			LPHISDAT pHisData = (LPHISDAT)pData;
			AnalyData *pAnalyData = new AnalyData[nDataNum];
			memset(pAnalyData,0,nDataNum*sizeof(AnalyData));
			if(DataType!=PER_DAY && DataType!=PER_WEEK && DataType!=PER_MONTH && DataType!=PER_DAYN && DataType!=PER_SEASON && DataType!=PER_YEAR) //���ڷ������ݣ����ܰ�ʱ���
				nRet = g_pStockIO->ReadAnalyDataOffset(pStkInfo,pAnalyData,nDataNum,DataType,0,GetMulNum(DataType));
			else
			{
				long StartDate,EndDate;
				if(!memcmp(&tFrom,&g_NoTime,sizeof(NTime))) 
					StartDate=0;
				else 
					StartDate=tFrom.year*10000+tFrom.month*100+tFrom.day;
				if(!memcmp(&tEnd,&g_NoTime,sizeof(NTime)))  
					EndDate=99991231;
				else 
					EndDate=tEnd.year*10000+tEnd.month*100+tEnd.day;
				nRet = g_pStockIO->ReadAnalyDataFromTime(pStkInfo,pAnalyData,nDataNum,StartDate,EndDate,DataType,GetMulNum(DataType));
				if(nTQ && nRet > 0)
					g_pStockIO->TQData(pStkInfo,pAnalyData,0,nRet-1,nRet,1,DataType);
			}
			//����ת��Ϊ��������
			if(nReserved == DYNAKTYPE)
				nRet = TransDynaKData(nRet, pAnalyData);
			if(!RawData2CalcData(Code,nSetCode,DataType,pAnalyData,pHisData,nRet,tFrom))
				nRet = 0;
			delete  pAnalyData;
			break;
		}
	case EXDAY_L2:
		{
			nRet = g_pStockIO->GetExDayDataNum(pStkInfo)+10;
			if(nDataNum==-1 || pData==NULL) break;
			if(nDataNum < 0 || !pData || nRet<=0)
			{
				nRet = 0;
				break;
			}
			LPEXDAYINFO pExdayInfo	= (LPEXDAYINFO)pData;
			memset(pExdayInfo,0,nDataNum*sizeof(EXDAYINFO));
			nRet = g_pStockIO->ReadExDayData(pStkInfo,pExdayInfo,nRet);
		}
		break;
	case GBINFO_DAT:	//�ɱ���Ϣ
		{
			LPGBINFO pGBInfo = (LPGBINFO)pData;
			if(nReserved == 0) //���nReservedΪ0����ʾֻҪһ���
			{
				long tmpDate = tFrom.year*10000+tFrom.month*100+tFrom.day;
				g_pStockIO->ReadLTGB_ZGB(pStkInfo,&tmpDate,&(pGBInfo->Ltgb),&(pGBInfo->Zgb),1,pStkInfo->ActiveCapital,pStkInfo->J_zgb);
				pGBInfo->Ltgb *= 10000.0;
				pGBInfo->Zgb *= 10000.0;
			}
			else			  //����Ҫһ�������
			{
				float *pZgb = new float[nDataNum];
				float *pLtgb = new float[nDataNum];
				g_pStockIO->ReadLTGB_ZGB(pStkInfo,(long *)nReserved,pLtgb,pZgb,nDataNum,pStkInfo->ActiveCapital,pStkInfo->J_zgb);
				for(int i=0;i < nDataNum;i++)
				{
					pGBInfo[i].Ltgb = pLtgb[i]*10000.0;
					pGBInfo[i].Zgb = pZgb[i]*10000.0;
				}
				delete pZgb;
				delete pLtgb;
			}
			nRet = 1;
			break;
		}
	case ALLGPINFO_DAT:	//���й�Ʊ�����Ϣ,���pDataΪ�գ��򷵻ػ�����
		{
			int gpnum = g_pStockIO->GetStockNum(ALL_ALL);
			if(nDataNum==-1 || !pData) //�������Ϊ��,�򷵻���������ռ�
				nRet = sizeof(ALLGPINFO)-1+gpnum*(1+6+8);
			else
			{
				MemStkInfo *tmpInfo;
				LPALLGPINFO pAllgpInfo = (LPALLGPINFO)pData;
				pAllgpInfo->StockNum = gpnum;
				int pos = 0;
				for(int i=0;i < gpnum;i++)
				{
					tmpInfo = (*g_pStockIO)[i];
					memcpy(pAllgpInfo->buf+pos,&(tmpInfo->setcode),1);
					memcpy(pAllgpInfo->buf+pos+1,tmpInfo->Code,6);
					memcpy(pAllgpInfo->buf+pos+1+6,tmpInfo->Name,8);
					pos+=1+6+8;
				}
				nRet = 1;
			}
		}
		break;
	case FLGINFO_DAT:	//�������Ϣ  ���pDataΪ�գ��򷵻ػ�����
	case DYGINFO_DAT:	//��������Ϣ
	case HYGINFO_DAT:	//��ҵ�����Ϣ
	case GNGINFO_DAT:	//��������Ϣ
	case ZDGINFO_DAT:	//�Զ������Ϣ
	case ZHGINFO_DAT:	//
		{
			MemStkInfo *tmpInfo;
			LPFLINFO pFlgInfo = (LPFLINFO)pData;
			int i,j,pos = 0;
			//�ȵõ�������Ϣ
			struct GpDomain_Struct m_GpDomain[100];
			int domainNum;
			if(DataType==FLGINFO_DAT)
			{
				domainNum = TOTAL_TYPENUM;
				for(i=0;i < domainNum;i++)
				{
					m_GpDomain[i].flag = DOMAIN_TYPE;
					m_GpDomain[i].which = i;
					strcpy(m_GpDomain[i].Name,GPType[i]);
				}
			}
			else if(DataType==DYGINFO_DAT)
			{
				domainNum = TOTAL_DY_NUM;
				for(int i=0;i < domainNum;i++)
				{
					m_GpDomain[i].flag = DOMAIN_DY;
					m_GpDomain[i].which = i;
					strcpy(m_GpDomain[i].Name,g_cwAddrTopic[i]);
				}
			}
			else if(DataType==HYGINFO_DAT)
			{
				domainNum = TOTAL_HY_NUM;
				for(int i=0;i < domainNum;i++)
				{
					m_GpDomain[i].flag = DOMAIN_HY;
					m_GpDomain[i].which = i;
					strcpy(m_GpDomain[i].Name,g_cwHYstr[i]);
				}
			}
			else if(DataType==GNGINFO_DAT)
			{
				domainNum = g_ConceptBlockNum;
				for(int i=0;i < domainNum;i++)
				{
					m_GpDomain[i].flag = DOMAIN_CON;
					m_GpDomain[i].which = i;
					strcpy(m_GpDomain[i].Name,g_ConceptBlock[i].strBlockName);
				}
			}
			else if(DataType==ZHGINFO_DAT)
			{
				domainNum = g_ZHBBlockNum;
				for(int i=0;i < domainNum;i++)
				{
					m_GpDomain[i].flag = DOMAIN_ZHB;
					m_GpDomain[i].which = i;
					strcpy(m_GpDomain[i].Name,g_ZHBBlock[i].zhmc);
				}
			}
			else if(DataType==ZDGINFO_DAT)
			{
				domainNum = g_nBlockNum;
				for(int i=0;i < domainNum;i++)
				{
					m_GpDomain[i].flag = DOMAIN_USERBLOCK;
					m_GpDomain[i].which = i;
					strcpy(m_GpDomain[i].Name,GetBlockFileName(i+2));
				}				
			}
			if(nDataNum==-1 || !pData)
				nRet = sizeof(FLINFO)-1;
			//�ٵõ������µĹ�Ʊ��Ϣ
			short indexbuffer[MAXGPNUM];
			int   nGPNum = 0;
			for(i=0;i < domainNum;i++)
			{
				nGPNum=GetDomainGP(m_GpDomain[i],indexbuffer);
				if(nDataNum==-1 || !pData)
					nRet += nGPNum*(1+6+8);
				else
				{
					strcpy(pFlgInfo->FLName[i],m_GpDomain[i].Name);
					pFlgInfo->FLStockNum[i] = nGPNum;
					for(j=0;j < nGPNum;j++)
					{
						tmpInfo = (*g_pStockIO)[indexbuffer[j]];
						memcpy(pFlgInfo->buf+pos,&(tmpInfo->setcode),1);
						memcpy(pFlgInfo->buf+pos+1,tmpInfo->Code,6);
						memcpy(pFlgInfo->buf+pos+1+6,tmpInfo->Name,8);
						pos+=1+6+8;			
					}
				}
			}
			if(nDataNum!=-1 && pData)
			{
				pFlgInfo->FLNum = domainNum;
				nRet = 1;
			}
		}
		break;
	case TPPRICE_DAT:
		{
			LPTPPRICE pTPPrice = (LPTPPRICE)pData;
			if(pStkInfo && pTPPrice->Close>COMPPREC)
			{
				pTPPrice->TPTop=GetTPPrice(pStkInfo,pTPPrice->Close,TRUE);
				pTPPrice->TPBottom=GetTPPrice(pStkInfo,pTPPrice->Close,FALSE);
				nRet=1;
			}
		}
		break;
	case SYSINFO_DAT:
		{
			LPSYSINFO pSysInfo = (LPSYSINFO)pData;
			memset(pSysInfo,0,sizeof(SYSINFO));
			pSysInfo->bOnLine=OnLine?1:0;
			//������չ�����Լ����ض��ƴ���,����ɽ����Ȳ���Ч
			pStkInfo=g_pStockIO->Get_MemStkInfo(Code,nSetCode);
			if(pStkInfo && (IsLocalStock(pStkInfo) || (pStkInfo->setcode!=SZ && pStkInfo->setcode!=SH)))
				pSysInfo->bYesterday=1;
			else
				pSysInfo->bYesterday=g_bYesterdayFlag?1:0;
			//
			pSysInfo->lOpenRq=g_lOpenRq;
			strcpy(pSysInfo->productnbbs,g_GC.productnbbs);
			pSysInfo->bDSOnLine = DSOnLine?1:0;
			pSysInfo->bIsSSDS = g_GC.bIsSSDS?1:0;
		#ifdef LEVEL2
			pSysInfo->cIsStdVersion = 1;
		#endif
			nRet=1;
		}
		break;
	case CURGPINDEX_DAT:	//�õ�ϵͳ��ǰ�Ĺ�ƱID
		nRet = GetSYSCurrentGPIndex();
		break;
	case GPINDEX_DAT:
		nRet = g_pStockIO->GetIndex(Code);
		break;
	case GPINFO_DAT:		//�õ���ƱID(����nReserved��)�õ���Ʊ�Ļ�����Ϣ
		{
			PGPINFO pInfo = (PGPINFO)pData;
			memset(pInfo,0,sizeof(GPINFO));
			MemStkInfo *pStkInfo = (*g_pStockIO)[nReserved];
			if(pStkInfo)
			{
				strcpy(pInfo->strCode,pStkInfo->Code);
				strcpy(pInfo->strName,pStkInfo->Name);
				pInfo->setcode = pStkInfo->setcode;
				pInfo->xsflag = GetXSFlag(pStkInfo);
				nRet=1;
			}
			break;
		}
	case CUR_FXTZB_DAT:
		{
			PCURFXTZB pInfo = (PCURFXTZB)pData;
			memset(pInfo,0,sizeof(CURFXTZB));
			memset(&g_CurFxtZb,0,sizeof(CURFXTZB));
			if(theViewManager.pActiveView && theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			{
				theViewManager.pActiveView->SendMessage(UM_GET_FXTDATA,0,0);
				memcpy(pInfo,&g_CurFxtZb,sizeof(CURFXTZB));
				nRet=1;
			}
		}
		break;
	case JYINFO_DAT:
		{
			PJYINFODATA pInfo = (PJYINFODATA)pData;
			memset(pInfo,0,sizeof(JYINFODATA));
		#ifdef OEM_NEWJY
			DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
			if(dwStatus&JY_LOGINED)
				pInfo->isLogin=1;
			pInfo->yybid=g_JyInfo.yybid;
			pInfo->khqx=g_JyInfo.khqx;
			pInfo->vipuserflag=g_JyInfo.vipuserflag;
			pInfo->MngRight=g_MngRight;
			strcpy(pInfo->zjzh,g_JyInfo.zjzh);
		#endif
			nRet=1;
		}
		break;
	case WINRECT_DAT:
		{
			PWINRECT_DAT pInfo = (PWINRECT_DAT)pData;
			memset(pInfo,0,sizeof(WINRECT_DAT));
			CRect rc;
			if(AfxGetMainWnd())
			{
				AfxGetMainWnd()->GetWindowRect(&rc);
				pInfo->MainFrameRect.left=rc.left;
				pInfo->MainFrameRect.right=rc.right;
				pInfo->MainFrameRect.top=rc.top;
				pInfo->MainFrameRect.bottom=rc.bottom;
			}
			if(theViewManager.pActiveView)
			{
				theViewManager.pActiveView->GetWindowRect(&rc);
				pInfo->ActiveViewRect.left=rc.left;
				pInfo->ActiveViewRect.right=rc.right;
				pInfo->ActiveViewRect.top=rc.top;
				pInfo->ActiveViewRect.bottom=rc.bottom;
			}
			nRet=1;
		}
		break;
	case OTHER_DAT:
		{
			LPOTHERINFO pOtherInfo = (LPOTHERINFO)pData;
			if(g_pStockIO->GetDomain(pStkInfo) == SZ)
				pOtherInfo->RelativeMin = GetTdxMinute(sz);
			else
				pOtherInfo->RelativeMin = GetTdxMinute(sh);
			nRet = 1;
		}
		break;
	default:
		break;
	}
	return nRet;
}

/******************************************************************************************************************/
//  ʵ�ֹ����л��Ļص�����
/******************************************************************************************************************/
long CALLBACK SwitchCallBackFunc(char * Code,short nSetCode,char *buf,long buflen,short FuncType,short Option,unsigned long nReserved)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState()); 

	if(!g_pMainWin) return 1;
	static short Switch_SelectedStk[MAXGPNUM];
	static short Switch_SelectedStkNum;
	Switch_SelectedStkNum = 0;
	SwitchCurGPCode = Code;
	SwitchCurSetCode = nSetCode;
	if(FuncType==SWITCH_SAVENEWBLOCK)	//��������Ϊ���
	{
		short setcode;
		char  code[SH_CODE_LEN+1];
		for(int i=0;i < buflen/(SH_CODE_LEN+1);i++)
		{
			setcode = buf[i*(SH_CODE_LEN+1)];
			memcpy(code,buf+i*(SH_CODE_LEN+1)+1,SH_CODE_LEN);
			code[SH_CODE_LEN] = 0;
			Switch_SelectedStk[Switch_SelectedStkNum] = g_pStockIO->GetIndex(code,setcode);
			if(Switch_SelectedStk[Switch_SelectedStkNum] >= 0) 
				Switch_SelectedStkNum++;
		}
		g_pMainWin->PostMessage(UM_SWITCHCALLBACK,MAKELONG(FuncType,Switch_SelectedStkNum),(long)Switch_SelectedStk);
		return 1;
	}
	else if(FuncType==SWITCH_OPENURL)
	{
		if(buflen>0 && g_pMainWin)
		{
			char *pTmp = new char[buflen+1];
			memcpy(pTmp,buf,buflen);
			pTmp[buflen]=0;

			//�������׷�ʽ�ͽ���ȫ��״̬��,���ⲿ��IE
			if((((CMainFrame *)g_pMainWin)->m_wndRestspace).IsFullSize())
				TDX_MessageBox(g_pMainWin->m_hWnd,"�������׷�ʽ�ͽ���ȫ��״̬��,����ʹ�ô˹���",MB_OK|MB_ICONEXCLAMATION);
			else
				OpenCusWebPage(g_pMainWin,pTmp,TRUE);

			delete pTmp;
		}
		return 1;
	}	
	else if(FuncType==SWITCH_INZXG)
	{
		MemStkInfo *pStkInfo=g_pStockIO->Get_MemStkInfo(Code,nSetCode);
		if(!pStkInfo) return 1;
		return BelongZXG(pStkInfo)?1:0;
	}
	else if(FuncType==SWITCH_JY_ISFULLSIZE)
	{
		if(g_pMainWin && (((CMainFrame *)g_pMainWin)->m_wndRestspace).IsFullSize())
			return 1;
		return 0;
	}
	else if(FuncType==SWITCH_ADDTOZXG)
	{
		AddGPToBlock(0,Code,nSetCode);
		return 1;
	}
	g_pMainWin->PostMessage(UM_SWITCHCALLBACK,MAKELONG(FuncType,0),nReserved);
	return 1;
}

//����ί��DLLʹ�õĻص�����
long CALLBACK GpInfoHqCallBack(const char * Code,short setcode,char *Name,float *fNow,float *fClose)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState()); 

	if(Name)	
	{
		strcpy(Name,Code);
		MemStkInfo *tmpInfo = g_pStockIO->Get_MemStkInfo(Code,setcode);
		if(tmpInfo)	 strcpy(Name,tmpInfo->Name);
	}
	struct CurrStockData tmphq;
	g_pStockIO->ReadCurrStockDataForCode(&tmphq,Code,setcode);
	if(fNow)	*fNow = tmphq.Now;
	if(fClose)	*fClose = tmphq.Close;
	return 1;
}

BOOL TxtTestFound(const char *tmpStr)
{
	int pos,SubLen;
	char *p = (char *)tmpStr;
	for(int i=0;i < g_nTxtFindNum;i++)
	{
		SubLen = g_TxtFindStr[i].GetLength();
		pos = 0;
		while(p = strstr(tmpStr+pos,g_TxtFindStr[i]))
		{
			pos = p-tmpStr;
			pos+=SubLen;
		}
		if(pos == 0) break;
	}
	if(i >= g_nTxtFindNum)
		return TRUE;
	return FALSE;
}

BOOL NewsTestFound(const char *tmpStr)
{
	int pos,SubLen;
	char *p = (char *)tmpStr;
	for(int i=0;i < g_nNewsFindNum;i++)
	{
		SubLen = g_NewsFindStr[i].GetLength();
		pos = 0;
		while(p = strstr(tmpStr+pos,g_NewsFindStr[i]))
		{
			pos = p-tmpStr;
			pos+=SubLen;
		}
		if(pos == 0) break;
	}
	if(i >= g_nNewsFindNum)
		return TRUE;
	return FALSE;
}

BOOL TestFound(const char *filename,int style)
{
	BOOL bFind = FALSE;
	CFile tmpFile;
	if(tmpFile.Open(filename,CFile::modeRead|CFile::shareDenyNone))
	{
		int filelen=tmpFile.GetLength();
		char *tmpStr = new char[filelen+1];
		tmpFile.Read(tmpStr,filelen);
		tmpStr[filelen]=0;
		bFind = (style==0)?TxtTestFound(tmpStr):NewsTestFound(tmpStr);
		delete tmpStr;
		tmpFile.Close();
	}
	return bFind;
}

int GetComboTextList(char str[][30],CComboBox* clist,int maxcount)
{
	int	 i;
	char tmp[30];
	int nCount = clist->GetCount();
	clist->GetWindowText(tmp,30);
	AllTrim(tmp);
	int nPos = clist->FindStringExact(0,tmp);
	if( nPos == CB_ERR )
	{
		strcpy(str[0],tmp);
		for(i = 1;i < maxcount && i <= nCount;i++)
		{
			clist->GetLBText(i-1,tmp);
			strcpy(str[i],tmp);
		}
	}
	else
	{
		for(i = 0;i < maxcount && i < nCount;i++)
		{
			clist->GetLBText(i,tmp);
			strcpy(str[i],tmp);
		}
		strcpy(tmp,str[0]);
		strcpy(str[0],str[nPos]);
		strcpy(str[nPos],tmp);
	}
	return i;
}


//�õ��ͻ���Base��Gbbq�ĸ��´���,�������վ��ȡ�õĴ�����ͬ���򽫴�����Ϊ�෴��
//�Ժ�����Ƿ��Ǹ���,���Զ�����Base��Gbbq,????��Ҫ�����г�SZ,SH
void GetNodeMapCount(short szstknum,short shstknum,struct MemStkInfo *pszMem,struct MemStkInfo *pshMem)
{
	CStdioFile theFile;
	CString	tmpLine;
	char	tmpCode2[7];
	int nFreshCount;

	if(theFile.Open(g_WSXHStr+"LData\\cache\\gbbq.map",CFile::modeRead|CFile::shareDenyNone))
	{
		while(theFile.ReadString(tmpLine))
		{
			if(strlen(tmpLine)<7) continue;
			strcpy(tmpCode2,tmpLine.Left(6));
			if(tmpCode2[4] == 32) tmpCode2[4] = 0;
			nFreshCount = atoi(tmpLine.Mid(6));
			for(int i=0;i<szstknum;i++)
			{
				if(strcmp(pszMem[i].Code,tmpCode2) == 0)
				{
					if(pszMem[i].GbbqFreshCount == nFreshCount)
						pszMem[i].GbbqFreshCount = -nFreshCount;
					break;
				}
			}
			if(i < szstknum) continue;
			for(i=0;i<shstknum;i++)
			{
				if(strcmp(pshMem[i].Code,tmpCode2) == 0)
				{
					if(pshMem[i].GbbqFreshCount == nFreshCount)
						pshMem[i].GbbqFreshCount = -nFreshCount;
					break;
				}
			}
		}
		theFile.Close();
	}
	if(theFile.Open(g_WSXHStr+"LData\\cache\\base.map",CFile::modeRead|CFile::shareDenyNone))
	{
		while(theFile.ReadString(tmpLine))
		{
			if(strlen(tmpLine)<7) continue;
			strcpy(tmpCode2,tmpLine.Left(6));
			if(tmpCode2[4] == 32) tmpCode2[4] = 0;
			nFreshCount = atoi(tmpLine.Mid(6));
			for(int i=0;i<szstknum;i++)
			{
				if(strcmp(pszMem[i].Code,tmpCode2) == 0)
				{
					if(pszMem[i].BaseFreshCount == nFreshCount)
						pszMem[i].BaseFreshCount = -nFreshCount;
					break;
				}
			}
			if(i < szstknum) continue;
			for(i=0;i<shstknum;i++)
			{
				if(strcmp(pshMem[i].Code,tmpCode2) == 0)
				{
					if(pshMem[i].BaseFreshCount == nFreshCount)
						pshMem[i].BaseFreshCount = -nFreshCount;
					break;
				}
			}
		}
		theFile.Close();
	}
}

//ȡ�û����IFZ(����),TIC(���WANT_TICKS��),BSP(���WANT_MMPS��)
BOOL GetCahceMinuteData(MemStkInfo *scode,struct MinuteData *pMinData,long date,float *pfclose)
{
	short setcode = g_pStockIO->GetDomain(scode);
	if(date==0)
	{
		CString tempFileName;
		tempFileName.Format("%sLData\\cache\\%s.tfz",g_WSXHStr,GetSetStr(setcode));
		FILE *fpt = fopen( tempFileName,"rb");
		if(!fpt) 
			return FALSE;
		struct MinCacheHead tmpHead;
		fseek(fpt, scode->Index*sizeof(struct MinCacheHead), SEEK_SET);
		fread(&tmpHead,1,sizeof(struct MinCacheHead),fpt);
		if(tmpHead.suc == 1)
		{
			fseek(fpt,tmpHead.pos,SEEK_SET);
			fread(pMinData,1,240*sizeof(struct MinuteData),fpt);
		}
		fclose(fpt);
		return (tmpHead.suc==1);
	}
	else
	{
		long filelen,minNum;
		char string[256];
		sprintf(string,"%szst_cache\\%s%s.zst",g_WSXHStr,GetSetStr(setcode),scode->Code);
		FILE * tmpfp = _fsopen( string, "rb",SH_DENYNO);
		if (tmpfp)
		{
			filelen = on_filelength(tmpfp);
			minNum = filelen/sizeof(CACHEHISZST);
			if(minNum > 0)
			{
				CACHEHISZST *pTmpCacheZst = new CACHEHISZST[minNum];
				fread(pTmpCacheZst,sizeof(CACHEHISZST),minNum,tmpfp);
				for(int i = 0;i < minNum;i++)
				{
					if(pTmpCacheZst[i].date==date)
					{
						memcpy(pMinData,pTmpCacheZst[i].mindata,240*sizeof(struct MinuteData));
						*pfclose = pTmpCacheZst[i].Close;
						delete pTmpCacheZst;
						fclose(tmpfp);
						return TRUE;
					}
				}
				delete pTmpCacheZst;
			}
			fclose(tmpfp);
		}
	}
	return FALSE;
}

int GetCahceTickData(MemStkInfo *scode,TICKDATA *pTickData)
{
	short setcode = g_pStockIO->GetDomain(scode);
	CString tempFileName;
	tempFileName.Format("%sLData\\cache\\%s.tik",g_WSXHStr,GetSetStr(setcode));
	FILE *fpt = fopen( tempFileName, "rb");
	if(!fpt) 
		return 0;
	struct MinCacheHead tmpHead;
	fseek(fpt, scode->Index*sizeof(struct MinCacheHead), SEEK_SET);
	fread(&tmpHead,1,sizeof(struct MinCacheHead),fpt);
	if(tmpHead.suc == 1)
	{
		fseek(fpt,tmpHead.pos,SEEK_SET);
		fread(pTickData,1,WANT_TICKS*sizeof(TICKDATA),fpt);
	}
	fclose(fpt);
	if(tmpHead.suc == 1)
	{
		for(int i=0;i < WANT_TICKS;i++)
			if(pTickData[i].Minute==0) break;
		return i;
	}
	else return 0;
}

//////////////////////////////////////////////////////////////////////////
//������ʷtick, -1���� 0û��
short	GetHisTickCache(MemStkInfo * scode, long nDate, TickData *indap, short num)
{
	if(!scode) return -1;
	long nGPIndex = g_pStockIO->GetIndex(scode->Code, scode->setcode);
	if(nGPIndex<0) return -1;
	int nReadNum = -1;
	if(indap==NULL||num<1)			//�õ�����
	{
		for(int i=0;i<MAX_HISCACHE;i++)
		{
			if(g_HisTickCacheHdr[i].nGPIndex==nGPIndex&&g_HisTickCacheHdr[i].nDate==nDate)
				return g_HisTickCacheHdr[i].nTickNum;
		}
	}
	else
	{
		int nPos = -1;
		for(int i=0;i<MAX_HISCACHE;i++)
		{
			if(g_HisTickCacheHdr[i].nGPIndex==nGPIndex&&g_HisTickCacheHdr[i].nDate==nDate)
			{
				nPos = g_HisTickCacheHdr[i].nStartPos;
				break;
			}
		}
		if(i>=MAX_HISCACHE) 
			return 0;

		char  filename[MAX_PATH]={0};
		sprintf(filename,"%szst_cache\\histicache.htc",g_WSXHStr);
		FILE *fpt = _fsopen(filename,"a+b",SH_DENYNO);
		if(!fpt) return 0;

		nReadNum = min(num, g_HisTickCacheHdr[i].nTickNum);
		
		fseek(fpt, g_HisTickCacheHdr[i].nStartPos, SEEK_SET);
		int nReadLen = fread(indap, sizeof(TICKDATA), nReadNum, fpt);
		long filelen = ftell(fpt);
		fclose(fpt);
		return nReadNum;
	}
	return -1;
}

void SetDateCtrlRange(CWnd *pWnd)
{
	CTime m_EndDate = CTime::GetCurrentTime();	//����ʱ��
	CTimeSpan tempSpan(10000,0,0,0);        //��һ���
	CTime m_StartDate = m_EndDate-tempSpan;
	m_EndDate += tempSpan;
	((CDateTimeCtrl *)pWnd)->SetRange(&m_StartDate,&m_EndDate);
}

BOOL ExecExtern(char *exepath,HWND MainhWnd,char *param,BOOL bIsWT)
{
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartupInfo;
	memset(&siStartupInfo,0,sizeof(STARTUPINFO));
	siStartupInfo.cb = sizeof(STARTUPINFO);
	CString StartDir = exepath;
	if(StartDir.Find("\\",0)==-1) //����
		StartDir = ".\\"+StartDir;
	StartDir = StartDir.Left(StartDir.ReverseFind('\\'));
	char *tmpexepath = strlwr(exepath); //�����ǲ������ǵĽ��׿ͻ��˻��Ǻ��µĽ��׶�
	char *pRet = strstr(tmpexepath,"winwt");
	char *pRet2 = strstr(tmpexepath,"xiadan");
	char cmdline[255];
	if(bIsWT && (pRet||pRet2)) //�����ί�г���,ʹ��������
	{
		if(pRet)
			sprintf(cmdline,"\"%s\" onlyone",exepath);	//����onlyone
		else
			sprintf(cmdline,"\"%s\" %X",exepath,MainhWnd);		//���봰�ھ��
		if (CreateProcess(
				exepath,
				cmdline,
				NULL, NULL, FALSE, 
				0,NULL, StartDir,
				&siStartupInfo, 
				&piProcInfo))
		{
			CloseHandle(piProcInfo.hThread);
			CloseHandle(piProcInfo.hProcess);
			return TRUE;
		}
	}
	else
	{
		if(param)	sprintf(cmdline,"\"%s\" %s",exepath,param);
		else		strcpy(cmdline,exepath);
		if (CreateProcess(
				exepath,
				cmdline,
				NULL, NULL, FALSE, 
				0,NULL, StartDir,
				&siStartupInfo, 
				&piProcInfo))
		{
			CloseHandle(piProcInfo.hThread);
			CloseHandle(piProcInfo.hProcess);
			return TRUE;
		}
	}
	return FALSE;
}

BOOL CompareCwcl(CWDATA *pCwData,const char *GpCode,short setcode)
{
	if(strcmp(pCwData->Code,GpCode)==0)
	{
		if((setcode!=SZ&&setcode!=SH) || pCwData->Setcode==setcode)
			return TRUE;
	}
	return FALSE;
}

short GetStockType ( short setcode, const char * Code )
{
	short type = CODE_SZOTHER;
	switch (setcode)
	{
	case SZ:
		type = CODE_SZOTHER;
		switch (Code[0])
		{
		case '0':
		{
			switch (Code[1])
			{
			case '0':
				if(Code[2]=='2')
					type = CODE_ZXB;
				else
					type = CODE_SZAG;
				break;
			case '3':
			case '8':
				type = CODE_SZQZ;
				break;
			}
		}
		break;
		case '1':
		{
			switch (Code[1])
			{
			case '0':
				type = CODE_SZGZ;	//���ծ
				break;
			case '1':
				type = CODE_SZZQ;	//��ծȯ
				break;
			case '2':
				type = CODE_SZKZHZQ;//���ת��ծȯ
				break;
			case '3':
				type = CODE_SZGZHG;//���ծ�ع�
				break;
			case '5':
			case '6':
				type = CODE_KFJJ;
				break;
			case '7':
			case '8':
				type = CODE_SZJJ;
				break;
			}
		}
		break;
		case '2':
			type = CODE_SZBG;
			break;
		case '3':
			if(Code[1]=='0')
				type = CODE_CYB;
			break;
		case '4':
			type = CODE_SB;
			break;
		}
		break;
	case SH:
		type = CODE_SHOTHER;
		switch (Code[0])
		{
		case '0':
			if (atol(Code)>30L)		//����ծ
				type = CODE_SHGZ;
			break;
		case '1':
			if(Code[1]=='2' || Code[1]=='3')
				type = CODE_SHZQ;	//��ծȯ
			else
				type = CODE_SHKZHZQ;//����ת��ծȯ
			break;
		case '2':
			type = CODE_SHGZHG;		//����ծ�ع�
			break;
		case '5':
			if(Code[1]=='8')
				type = CODE_SHQZ;	//��Ȩ֤
			else
				type = CODE_SHJJ;	//������
			break;
		case '6':
			type = CODE_SHAG;		//��A
			break;
		case '7':
			if ('5'==Code[1]||'7'==Code[1])
				type = CODE_SHGZ;
			break;
		case '9':
			if(Code[1]=='0' && Code[2]=='0')
				type = CODE_SHBG;
			break;
		}
		break;
	}
	return type;
}

extern int g_Hookcmd;
//��ִ��ĳЩ���ܼ�,���Զ�ˢ�����е�����
BOOL RefreshAllHq(int nInterval)
{
	if(OnLine)
	{
		if( (g_bYesterdayFlag && g_lRefreshHQSeconds!=0) || (g_lRefreshHQSeconds > sh[3]*60 && g_lRefreshHQSeconds < 60*24*60) )
			; //�м����������Ҫˢ��
		else if(g_Hookcmd==SW_SHOW)	//������״̬(�ϰ��)
		{
			SYSTEMTIME  nowtime;
			GetCorrespondingTime( &nowtime );
			int m = nowtime.wHour*3600+nowtime.wMinute*60+nowtime.wSecond;
			if(m - g_lRefreshHQSeconds > nInterval) //����nInterval��,��Ҫˢ������
			{
				g_nSimhq2Stk = g_pStockIO->GetStockNum(ALL_ALL);
				g_nSimhq2Stk = min(g_nSimhq2Stk, MAXGPNUM);
				short       lpnUsedStkIndex[MAXGPNUM];
				for(int i=0;i<g_nSimhq2Stk;i++)
					lpnUsedStkIndex[i]=i;
				g_Simhq2StkIndex = lpnUsedStkIndex;
				g_lRefreshHQSeconds = m;				//��ʱ�Ժ���Ҫ���飬�����ظ�
				AfxGetMainWnd()->SendMessage(WM_COMMAND,ID_WIDEDATA_HQ); 
				return TRUE;
			}
		}
	}
	return FALSE;
}

//��ִ��ĳЩ���ܼ�,���Զ�ˢ�����е�����
BOOL RefreshAllStatHq(int nInterval)
{
	return FALSE;
	if(OnLine)
	{
		if( (g_bYesterdayFlag && g_lRefreshStatHQSeconds!=0) || (g_lRefreshStatHQSeconds > sh[3]*60 && g_lRefreshStatHQSeconds < 60*24*60) )
			; //�м����������Ҫˢ��
		else if(g_Hookcmd==SW_SHOW)	//������״̬(�ϰ��)
		{
			SYSTEMTIME  nowtime;
			GetCorrespondingTime( &nowtime );
			int m = nowtime.wHour*3600+nowtime.wMinute*60+nowtime.wSecond;
			if(m - g_lRefreshStatHQSeconds > nInterval) //����nInterval��,��Ҫˢ������
			{
				g_nSimhq2Stk = g_pStockIO->GetStockNum(ALL_ALL);
				g_nSimhq2Stk = min(g_nSimhq2Stk, MAXGPNUM);
				short       lpnUsedStkIndex[MAXGPNUM];
				for(int i=0;i<g_nSimhq2Stk;i++)
					lpnUsedStkIndex[i]=i;
				g_Simhq2StkIndex = lpnUsedStkIndex;
				g_lRefreshStatHQSeconds = m;				//��ʱ�Ժ���Ҫ���飬�����ظ�
				AfxGetMainWnd()->SendMessage(UM_WIDEDATA_HQSTAT,0); 
				AfxGetMainWnd()->SendMessage(UM_WIDEDATA_HQSTAT,1); 
				//ÿ��������Ϻ����¼���DDE
				g_pStockIO->CalcDDEReport();
				return TRUE;
			}
		}
	}
	return FALSE;
}

long GetBackDate(long date)
{
	CTime theTime(date/10000,(date%10000)/100,(date%10000)%100,0,0,0);
	CTimeSpan timeSpan(1,0,0,0);
	CTime BackTime = theTime-timeSpan;
	long lastdate = BackTime.GetYear()*10000+BackTime.GetMonth()*100+BackTime.GetDay();
	if(GetWeek(lastdate)==0||GetWeek(lastdate)==6)
		lastdate = GetBackDate(lastdate);	//�ݹ�
	return lastdate;
}

long GetForwardDate(long date)
{
	CTime theTime(date/10000,(date%10000)/100,(date%10000)%100,0,0,0);
	CTimeSpan timeSpan(1,0,0,0);
	CTime ForwardTime = theTime+timeSpan;
	long nextdate = ForwardTime.GetYear()*10000+ForwardTime.GetMonth()*100+ForwardTime.GetDay();
	if(GetWeek(nextdate)==0||GetWeek(nextdate)==6)
		nextdate = GetForwardDate(nextdate);	//�ݹ�
	return nextdate;
}

//�õ����������λ��(��ָ��Ϊ׼)
long GetVipDocLastDate(BOOL bLocalDate)
{
	CTime ct = CTime::GetCurrentTime();
	long lRet = 10000*(ct.GetYear()-2)+100*ct.GetMonth()+ct.GetDay();
	if(bLocalDate)
	{
	CFile FxFile;
	CString tmpFileName;
	tmpFileName.Format("%ssh\\day\\sh000001.day",g_TdxCfg.DocPath);
	g_HisDataReader.Lock();
	if(FxFile.Open(tmpFileName,CFile::modeRead))
	{
		if(FxFile.GetLength()>=sizeof(ANALYDATA))
		{
			ANALYDATA tmpAnalyData={0};
			FxFile.Seek(-sizeof(ANALYDATA),SEEK_END);
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			lRet = tmpAnalyData.Time.Date;
		}
		FxFile.Close();
	}
	g_HisDataReader.Unlock();
	}
	return max(lRet,20000101);
}

//�õ����������λ��(��ָ��Ϊ׼)
long GetVipDocFirstDate()
{
	CTime ct = CTime::GetCurrentTime();
	long lRet = 10000*(ct.GetYear()-1)+100*ct.GetMonth()+ct.GetDay();
	CFile FxFile;
	CString tmpFileName;
	tmpFileName.Format("%ssh\\day\\sh000001.day",g_TdxCfg.DocPath);
	g_HisDataReader.Lock();
	if(FxFile.Open(tmpFileName,CFile::modeRead))
	{
		if(FxFile.GetLength()>=sizeof(ANALYDATA))
		{
			ANALYDATA tmpAnalyData={0};
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			lRet = tmpAnalyData.Time.Date;
		}
		FxFile.Close();
	}
	g_HisDataReader.Unlock();
	return max(lRet,20000101);
}

//�õ�������ӵ�����λ��
long GetFxMinVipDocLastDate(BOOL &bFindGen)
{
	bFindGen=FALSE;
	CTime cn(g_iToday/10000, (g_iToday/100)%100, g_iToday%100, 0, 0, 0);
	CTime dst = cn-CTimeSpan(25,0,0,0);
	long ldt = 10000*dst.GetYear()+100*dst.GetMonth()+dst.GetDay();
	ldt=max(ldt,20070101);
	long lRet = ldt;
	CFile FxFile;
	CString tmpFileName;
	tmpFileName.Format("%ssh\\fzline\\sh000001.lc5",g_TdxCfg.DocPath);
	if(FxFile.Open(tmpFileName,CFile::modeRead))
	{
		if(FxFile.GetLength()>=sizeof(ANALYDATA))
		{
			ANALYDATA tmpAnalyData;
			FxFile.Seek(-sizeof(ANALYDATA),SEEK_END);
			FxFile.Read(&tmpAnalyData,sizeof(ANALYDATA));
			lRet = 10000*(g_lOpenRq/10000)+tmpAnalyData.Time.Daye.Day;
			//
			if(lRet==g_lOpenRq)
			{
				short nFZ[8]={0};
				InitStkTime(SH,nFZ);
				int nTodayDataNum = GetFZnoFromMinute(tmpAnalyData.Time.Daye.Minute, PER_MIN5, nFZ, 1);
				if(nTodayDataNum<GetMinuteXh(PER_MIN5,nFZ,1))
					bFindGen=TRUE;
			}
		}
		FxFile.Close();
	}
	return max(lRet,ldt);
}

//��һ���������ݼ��뵽LData���̺�������,ע��:��Ҫת�������ߵĴ��̸�ʽ
BOOL AddToVipdocDayData(MemStkInfo *pStkInfo,struct AnalyData *AnalyDatap,int nData,short type)
{
	ANALYDATA tmpAnalyData1,tmpAnalyData2;
	memcpy(&tmpAnalyData1,AnalyDatap,sizeof(ANALYDATA));
	memcpy(&tmpAnalyData2,AnalyDatap+(nData-1),sizeof(ANALYDATA));

	if(type==PER_DAY) //�ݽ�֧����������
	{
		CFile FxFile;
		CString tmpFileName;
		BOOL bDSStk = g_pStockIO->BeDSStock(pStkInfo);
		if(bDSStk)
			tmpFileName.Format("%sds\\day\\%d#%s.day",g_TdxCfg.DocPath,(pStkInfo->setcode!=32)?pStkInfo->setcode:31,pStkInfo->Code);
		else tmpFileName.Format("%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(pStkInfo->setcode),GetSetStr(pStkInfo->setcode),pStkInfo->Code);
		g_HisDataReader.Lock();
		if(FxFile.Open(tmpFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone))
		{
			long oldfilelen = FxFile.GetLength();
			int nOldData = oldfilelen/sizeof(ANALYDATA);
			if(nOldData < 0) nOldData = 0;

			ANALYDATA *analybuf = new ANALYDATA[nOldData+nData+RECENTNUM]; //Ԥ��RECENTNUM���ռ�,���⻺����������
			FxFile.Read(analybuf,nOldData*sizeof(ANALYDATA));
			int WriteDataNum = 0;
			//���ݴ����쳣�ػ�
			try
			{
				if(nOldData>0 && analybuf[0].Time.Date > tmpAnalyData2.Time.Date)	//���������ں�
				{
					memmove(analybuf+nData,analybuf,nOldData*sizeof(ANALYDATA));
					memcpy(analybuf,AnalyDatap,nData*sizeof(ANALYDATA));
					WriteDataNum = nData+nOldData;
				}
				else if(nOldData>0 && analybuf[nOldData-1].Time.Date < tmpAnalyData1.Time.Date)	//����������ǰ
				{
					memcpy(analybuf+nOldData,AnalyDatap,nData*sizeof(ANALYDATA));
					WriteDataNum = nData+nOldData;
				}
				else	//����н��������
				{
					int lTrunStart = -1,lTrunEnd = nOldData;
					for(int i=0;i < nOldData;i++)
					{
						if(lTrunStart==-1 && tmpAnalyData1.Time.Date <= analybuf[i].Time.Date)
							lTrunStart = i;
						if(tmpAnalyData2.Time.Date < analybuf[i].Time.Date)
						{
							lTrunEnd = i;
							break;
						}
					}
					if(lTrunStart==-1) lTrunStart = 0;
					memmove(analybuf+lTrunStart+nData,analybuf+lTrunEnd,(nOldData-lTrunEnd)*sizeof(ANALYDATA));
					memcpy(analybuf+lTrunStart,AnalyDatap,nData*sizeof(ANALYDATA));
					WriteDataNum = nOldData+nData-(lTrunEnd-lTrunStart);
				}
			}
			catch(...)
			{
				delete analybuf;
				FxFile.Close();	
				g_HisDataReader.Unlock();
				if(TDX_MessageBox(NULL,"�ڱ��ش��̹���������һ������,\n�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK)
					return TRUE;
				return FALSE;
			}
			//д���쳣�ػ�
			try
			{
				FxFile.SeekToBegin();
				FxFile.Write(analybuf,WriteDataNum*sizeof(ANALYDATA));
				FxFile.SetLength(WriteDataNum*sizeof(ANALYDATA));
			}
			catch(...)
			{
				TDX_MessageBox(NULL,"д�ļ�ʧ��,��ȷ��Ŀ¼�Ƿ��д,Ӳ�̿ռ��Ƿ��㹻!",MB_OK|MB_ICONEXCLAMATION);
				delete analybuf;
				FxFile.Close();	
				g_HisDataReader.Unlock();
				return FALSE; //ʧ���˳�
			}
			delete analybuf;
			FxFile.Close();	
		}
		else
		{
			g_HisDataReader.Unlock();
			if(TDX_MessageBox(NULL,"�ļ��򿪻򴴽�ʧ��!��ȷ��Ŀ¼�Ƿ��д���Ƿ��з������������!\n�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK)
				return TRUE;
			return FALSE; //ʧ���˳�
		}
		g_HisDataReader.Unlock();
	}
	return TRUE;
}

BOOL AddToVipdocFxMinData(MemStkInfo *pStkInfo,struct AnalyData *AnalyDatap,int nData,short type)
{
	ANALYDATA tmpAnalyData1,tmpAnalyData2;
	memcpy(&tmpAnalyData1,AnalyDatap,sizeof(ANALYDATA));
	memcpy(&tmpAnalyData2,AnalyDatap+(nData-1),sizeof(ANALYDATA));

	if(type==PER_MIN5) //�ݽ�֧��5����������
	{
		CFile FxFile;
		CString tmpFileName;
		tmpFileName.Format("%s%s\\fzline\\%s%s.lc5",g_TdxCfg.DocPath,GetSetStr(pStkInfo->setcode),GetSetStr(pStkInfo->setcode),pStkInfo->Code);
		if(FxFile.Open(tmpFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone))
		{
			long oldfilelen = FxFile.GetLength();
			int nOldData = oldfilelen/sizeof(ANALYDATA);
			if(nOldData < 0) nOldData = 0;

			ANALYDATA *analybuf = new ANALYDATA[nOldData+nData+RECENTNUM]; //Ԥ��RECENTNUM���ռ�,���⻺����������
			FxFile.Read(analybuf,nOldData*sizeof(ANALYDATA));
			int WriteDataNum = 0;
			//���ݴ����쳣�ػ�
			try
			{
				if(nOldData>0 && analybuf[0].Time.Daye.Day > tmpAnalyData2.Time.Daye.Day)	//���������ں�
				{
					memmove(analybuf+nData,analybuf,nOldData*sizeof(ANALYDATA));
					memcpy(analybuf,AnalyDatap,nData*sizeof(ANALYDATA));
					WriteDataNum = nData+nOldData;
				}
				else if(nOldData>0 && analybuf[nOldData-1].Time.Daye.Day < tmpAnalyData1.Time.Daye.Day)	//����������ǰ
				{
					memcpy(analybuf+nOldData,AnalyDatap,nData*sizeof(ANALYDATA));
					WriteDataNum = nData+nOldData;
				}
				else	//����н��������
				{
					int lTrunStart = -1,lTrunEnd = nOldData;
					for(int i=0;i < nOldData;i++)
					{
						if(lTrunStart==-1 && tmpAnalyData1.Time.Daye.Day <= analybuf[i].Time.Daye.Day)
							lTrunStart = i;
						if(tmpAnalyData2.Time.Daye.Day < analybuf[i].Time.Daye.Day)
						{
							lTrunEnd = i;
							break;
						}
					}
					if(lTrunStart==-1) lTrunStart = 0;
					memmove(analybuf+lTrunStart+nData,analybuf+lTrunEnd,(nOldData-lTrunEnd)*sizeof(ANALYDATA));
					memcpy(analybuf+lTrunStart,AnalyDatap,nData*sizeof(ANALYDATA));
					WriteDataNum = nOldData+nData-(lTrunEnd-lTrunStart);
				}
			}
			catch(...)
			{
				delete analybuf;
				FxFile.Close();	
				if(TDX_MessageBox(NULL,"�ڱ��ش��̹���������һ������,\n�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK)
					return TRUE;
				return FALSE;
			}
			//д���쳣�ػ�
			try
			{
				FxFile.SeekToBegin();
				FxFile.Write(analybuf,WriteDataNum*sizeof(ANALYDATA));
				FxFile.SetLength(WriteDataNum*sizeof(ANALYDATA));
			}
			catch(...)
			{
				TDX_MessageBox(NULL,"д�ļ�ʧ��,��ȷ��Ŀ¼�Ƿ��д,Ӳ�̿ռ��Ƿ��㹻!",MB_OK|MB_ICONEXCLAMATION);
				delete analybuf;
				FxFile.Close();	
				return FALSE; //ʧ���˳�
			}
			delete analybuf;
			FxFile.Close();	
		}
		else
		{
			if(TDX_MessageBox(NULL,"�ļ��򿪻򴴽�ʧ��!��ȷ��Ŀ¼�Ƿ��д���Ƿ��з������������!\n�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK)
				return TRUE;
			return FALSE; //ʧ���˳�
		}
	}
	return TRUE;
}

BOOL OneStock_PH(MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;
	if(pStkInfo->setcode==SH&&strcmp(pStkInfo->Code, "000001")==0)
		return TRUE;
	short nType = GetStockType(pStkInfo->setcode, pStkInfo->Code);
	if(nType!=CODE_SZAG&&nType!=CODE_SHAG&&nType!=CODE_ZXB&&nType!=CODE_CYB) 
		return TRUE;

	struct AnalyData ls,oldls;
	struct CurrStockData tmphq;
	int factor = 100;
	char   Str[MAX_PATH];
	FILE  *fp;
	short stockkind = GetStockType(pStkInfo->setcode,pStkInfo->Code);
	g_pStockIO->ReadCurrStockData(pStkInfo,&tmphq);
	//���ڿ���ʽ�����ETF������,Ҫ�������е�Open,Now,Max,Min�����ر���
	if ((pStkInfo->setcode==SZ&&strncmp(pStkInfo->Code,"16",2)==0) || (pStkInfo->setcode==SH&&strncmp(pStkInfo->Code,"519",3)==0))
	{
		if(tmphq.Open <= COMPPREC)	tmphq.Open = tmphq.Close;
		if(tmphq.Now <= COMPPREC)	tmphq.Now = tmphq.Close;
		if(tmphq.Max <= COMPPREC)	tmphq.Max = tmphq.Close;
		if(tmphq.Min <= COMPPREC)	tmphq.Min = tmphq.Close;
		tmphq.Max = max(tmphq.Max,tmphq.Now);
		tmphq.Min = min(tmphq.Min,tmphq.Now);
	}
	for(int i=0;pStkInfo->setcode==SH && i<g_nETFInfoNum;i++)
	{
		if(g_ETFInfo[i].setcode==SH && strcmp(pStkInfo->Code,g_ETFInfo[i].Code)==0)
		{
			if(tmphq.Open <= COMPPREC)	tmphq.Open = tmphq.Now;
			if(tmphq.Max <= COMPPREC)	tmphq.Max = tmphq.Now;
			if(tmphq.Min <= COMPPREC)	tmphq.Min = tmphq.Now;
			break;
		}
	}	
	//
    if (0==tmphq.ItemNum && (tmphq.Open<COMPPREC||tmphq.Max<COMPPREC||tmphq.Min<COMPPREC) )
        return TRUE;
	if (tmphq.Now<COMPPREC)
		return TRUE;
	if (!testzs(pStkInfo) && tmphq.Volume==0L && (tmphq.Open<COMPPREC||tmphq.Max<COMPPREC||tmphq.Min<COMPPREC) )
		return TRUE;

	if (GetXSFlag(pStkInfo)==3 && stockkind!=CODE_SZGZHG && stockkind!=CODE_SHGZHG)
		factor = 1000;
	else
		factor = 100;
	if ((stockkind==CODE_SZJJ || stockkind==CODE_SHJJ) && g_lOpenRq < 20030303)
		factor = 100;
	////
	ls.Time.Date = g_lOpenRq;
	ls.l_Open    = (DWORD)(tmphq.Open*factor+0.5);
	ls.l_High    = (DWORD)(tmphq.Max*factor+0.5);
	ls.l_Low     = (DWORD)(tmphq.Min*factor+0.5);
	ls.l_Close   = (DWORD)(tmphq.Now*factor+0.5);
	if (ls.l_Close!=0L)
	{
		if (ls.l_Open==0L)
			ls.l_Open = ls.l_Close;
		if (ls.l_High==0L)
			ls.l_High = ls.l_Close;
		if (ls.l_Low==0L)
			ls.l_Low = ls.l_Close;
	}
	ls.Amount    = tmphq.Amount;
	if (testzs(pStkInfo))
	{
		ls.Volume  = tmphq.Volume;
		ls.zd.up   = tmphq.Buyv[0];
		ls.zd.down = tmphq.Sellv[0];
	}
	else
	{
		if (need_justcjl(pStkInfo->setcode,pStkInfo->Code))
			ls.Volume = tmphq.Volume*pStkInfo->Unit+tmphq.Flag.RestVol;
		else
			ls.Volume = tmphq.Volume;
		ls.YClose = 0.0;
	}
	memset(&oldls,0,sizeof(struct AnalyData));
	sprintf(Str,"%s%s\\day\\%s%s.day",g_TdxCfg.DocPath,GetSetStr(pStkInfo->setcode),GetSetStr(pStkInfo->setcode),pStkInfo->Code);
	g_HisDataReader.Lock();
	fp = _fsopen(Str,"r+b",SH_DENYNO);
	if (!fp) 
		fp = _fsopen(Str,"wb",SH_DENYNO);
	if (!fp)
	{
		g_HisDataReader.Unlock();
		if(TDX_MessageBox(NULL,"�ļ��򿪻򴴽�ʧ��!��ȷ��Ŀ¼�Ƿ��д���Ƿ��з������������!\n�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDOK)
			return TRUE;
		return FALSE;
	}
	if (on_filelength(fp)>=sizeof(struct AnalyData))
	{
		fseek(fp,-(long)sizeof(struct AnalyData),SEEK_END);
		fread(&oldls,sizeof(struct AnalyData),1,fp);
	}
	if (ls.Time.Date==oldls.Time.Date)
		fseek(fp,-(long)sizeof(struct AnalyData),SEEK_END);
	else
		fseek(fp,0L,SEEK_END);
	fwrite(&ls,sizeof(struct AnalyData),1,fp);
	fclose(fp);
	g_HisDataReader.Unlock();
	return TRUE;
}

//ͬʱ����22�ֳ���ָ��
void Refresh_OftenUseZb_Menu()
{
	TINDEXINFO *pTmpInfo;
	int nfrom = g_awMenuStartNum[2*MENU_INDEX];
	CString tmpStr;
	for(int i = nfrom; i<nfrom+g_nOftenUseZb; i++)
	{										
		pTmpInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,g_strOftenUseZb[i-nfrom]);
		if(pTmpInfo)
		{
			tmpStr.Format("%s%s",pTmpInfo->acCode,pTmpInfo->acName);
			strncpy(g_pMenu[i].Name+3,tmpStr,20);
		}
		else
			strncpy(g_pMenu[i].Name+3,"δָ֪��",20);
	}
	g_awMenuStartNum[2*5+1]=g_nOftenUseZb;
}

void Refresh_OftenUseZb_CmdTable()
{
	return;

	int i,j;
	CString tmpStr;
	TINDEXINFO *pTmpInfo;
	for(i=0;g_pCmdTable[i].Code[0]!='\0';i++)
		if(g_pCmdTable[i].lParam == 301) break;
	for(j=0;j<g_nOftenUseZb;j++)
	{
		pTmpInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,g_strOftenUseZb[j]);
		if(pTmpInfo)
		{
			tmpStr.Format(".3%02d %s%s",j+1,pTmpInfo->acCode,pTmpInfo->acName);
			strncpy(g_pCmdTable[i+j].Name,tmpStr,39);
			g_pCmdTable[i+j].Name[39] = 0;
		}
		else
			strcpy(g_pCmdTable[i+j].Name,"δָ֪��");
	}
	for (i=0;g_pCmdTable[i].Code[0]!='\0';i++)
		_F(g_pCmdTable[i].Name);	
}

void Refresh_XXM_Menu()
{
	CString tmpStr;
	int nfrom = g_awMenuStartNum[2*MENU_XXM];
	for(int i = 0;i < XXMNum;i++)
		strncpy(g_pMenu[nfrom+i].Name+3,XXMTopic[i],70);
	int pos=XXMNum;
	if(g_bHasInfo)
		strncpy(g_pMenu[nfrom+pos++].Name+3,"�ƾ���Ѷ",70);
	if(g_bHasWebInfo||strlen(g_Url.WebInfoURL)>0)
		strncpy(g_pMenu[nfrom+pos++].Name+3,"������Ѷ",70);
	if(g_bExtern_Info)
		strncpy(g_pMenu[nfrom+pos++].Name+3,"�����Ѷ",70);
}

//������
BOOL DeleteGPFromBlock(int whichGPType,int nBlockFlag,int gpno)
{
	if(whichGPType==TYPE_SB || whichGPType==TYPE_SPECINDEX || whichGPType==TYPE_AJH || whichGPType==TYPE_LC) 
		return FALSE;	//����ɾ������֤ȯ,���ָ��,A+H�ɵ�
	if(gpno == -1)	return FALSE;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[gpno];
	if(!tempStkInfo) return FALSE;

	CString CurCode = tempStkInfo->Code;
	short	CurSetCode = tempStkInfo->setcode;
	CString	tempGPCode,tempStr,tempBuffer,tempFileName,tempLine2;
	short	setcode;
	
	if(whichGPType == TYPE_ZXG)		tempStr = "self";
	else if(whichGPType == TYPE_TJG)	tempStr = "calc";
	else	tempStr = GetBlockFileName(nBlockFlag+2);
	tempFileName = (CString)g_strBlockNew+tempStr+(CString)".blk";

	BOOL	bFind = FALSE;	
	CStdioFile m_File;
	if(m_File.Open(HomePath+tempFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone))
	{
		DWORD	LastPosition,CurPosition,TotalLength;
		TotalLength = m_File.GetLength();
		LastPosition = m_File.GetPosition();
		while(m_File.ReadString(tempGPCode))
		{
			TrimString(tempGPCode);
			if(tempGPCode.IsEmpty()) continue;
			int tmpb=tempGPCode.Find("#",0);
			int tmp = (tmpb==-1?0:tmpb);
			tempLine2 = tempGPCode.Mid(tmp+1);
			setcode = tmpb>=0?atoi(tempGPCode):tempGPCode[0]-'0';
			if(tempLine2==CurCode && setcode==CurSetCode)
			{
				bFind = TRUE;	
				break;
			}
			LastPosition = m_File.GetPosition();
		}
		if(bFind)
		{
			CurPosition = m_File.GetPosition();
			while(m_File.ReadString(tempGPCode))
				tempBuffer+=tempGPCode+"\n";
			m_File.Seek(LastPosition,CFile::begin);
			m_File.WriteString(tempBuffer);
			m_File.SetLength(TotalLength-CurPosition+LastPosition);
			
		}		
		m_File.Close();

		LoadColorfulBlockGP();
	}
	else
		TDX_MessageBox(NULL,"���ܴ򿪰���ļ�!",MB_ICONEXCLAMATION|MB_OK);
	
	return bFind;
}

void FiltOtherBlock(int blockno,CString m_CurGPCode,short m_CurSetCode)
{
	short nIDIndex = g_pStockIO->GetIndex(m_CurGPCode, m_CurSetCode);
	short indexbuff[MAXGPNUM]; memset(indexbuff,-1,MAXGPNUM*sizeof(short));
	int nBlockNum = 0, i, j;
	for(i = 0;i < g_nBlockNum;i++)
	{
		if(blockno==i+2) continue;
		int gpnum = FiltBlockCode(i+2,indexbuff);//i+2������ѡ�ɺ͹�Ʊ��
		for(j=0;j<gpnum;j++)
		{
			if(indexbuff[j]==nIDIndex)
				break;
		}
		if(j < gpnum)
		{
			char strMsg[256]={0};
			sprintf(strMsg,"�ù��Ѵ����ڰ��\'%s\'��,�Ƿ񽫹�Ʊ\'%s\'�Ӱ��\'%s\'��ɾ��?",
				g_BlockInfo[i].Name,(*g_pStockIO)[nIDIndex]->Name,g_BlockInfo[i].Name);
			if(TDX_MessageBox(NULL,strMsg,MB_YESNO|MB_ICONQUESTION)==IDYES)
				DeleteGPFromBlock(-1,i,nIDIndex);
		}
	}
}

BOOL AddGPToBlock(int blockno,CString m_CurGPCode,short m_CurSetCode) //�������ѡ��blockno=0,��Ʊ��blockno=1,����+2
{
	CString tempStr = GetBlockFileName(blockno);
	CString tempFileName = g_strBlockNew+tempStr+(CString)".blk";

	CStdioFile BlockFile;
	if( BlockFile.Open(HomePath+tempFileName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeReadWrite|CFile::shareDenyNone) )
	{					
		CString tempGPCode,tempLine2,tempBuffer;
		short	setcode;
		BOOL	bFind = FALSE;
		DWORD	LastPosition,CurPosition,TotalLength;
		TotalLength = BlockFile.GetLength();
		LastPosition = BlockFile.GetPosition();
		while(BlockFile.ReadString(tempGPCode))
		{
			TrimString(tempGPCode);
			if(tempGPCode.IsEmpty()) continue;
			int tmpb=tempGPCode.Find("#",0);
			int tmp = (tmpb==-1?0:tmpb);
			tempLine2 = tempGPCode.Mid(tmp+1);
			setcode = tmpb>=0?atoi(tempGPCode):tempGPCode[0]-'0';
			if(tempLine2==m_CurGPCode && setcode==m_CurSetCode)
			{
				bFind = TRUE;
				break;
			}
			LastPosition = BlockFile.GetPosition();
		}
		if(bFind) //����Ѿ����ڣ�����ɾ�����ټӵ�ĩβ
		{
			CurPosition = BlockFile.GetPosition();
			while(BlockFile.ReadString(tempGPCode))
				tempBuffer+=tempGPCode+"\n";
			BlockFile.Seek(LastPosition,CFile::begin);
			BlockFile.WriteString(tempBuffer);
			BlockFile.SetLength(TotalLength-CurPosition+LastPosition);
		}
		BlockFile.SeekToEnd();
		BOOL bDSStock = g_pStockIO->BeDSStock(m_CurSetCode,m_CurGPCode);
		tempGPCode.Format(bDSStock?"%d#%s":"%d%s",m_CurSetCode,m_CurGPCode);
		BlockFile.WriteString("\n"+tempGPCode);
		BlockFile.Close();
		if(g_GH.bAddBlockPrompt && blockno>1) 
			FiltOtherBlock(blockno,m_CurGPCode,m_CurSetCode);

		LoadColorfulBlockGP();
		return TRUE;
	}
	else
		TDX_MessageBox(NULL,"���ܴ򿪰����ļ�!",MB_ICONEXCLAMATION|MB_OK);

	return FALSE;
}

//�ı��Զ�����ɺ���ѡ�ɵȵ�����˳��
void ArrangeBlockGP_Shift(int blockno,int SrcGPNo,int DestGPNo)
{
	if(SrcGPNo==DestGPNo) return;
	CString tempStr = GetBlockFileName(blockno);
	CString tempFileName = g_strBlockNew+tempStr+(CString)".blk";
	CString tempLine;
	int		tempIndex;
	short   tmpStkIndex[MAXGPNUM+1],StkNum = 0;
	CStdioFile m_File;
	if( m_File.Open(HomePath+tempFileName,CFile::modeReadWrite|CFile::shareDenyNone) )
	{
		while(m_File.ReadString(tempLine))
		{
			TrimString(tempLine);
			if(tempLine.IsEmpty()) continue;
			tempIndex = g_pStockIO->GetIndex_FromMultiBuf(tempLine);
			if(tempIndex >= 0)
				tmpStkIndex[StkNum++] = tempIndex;
		}
		short tmpshort;
		int a=-1,b=-1;
		for(int i=0;i < StkNum;i++)
		{
			if(tmpStkIndex[i]==SrcGPNo) 
			{
				a=i;
				break;
			}
		}
		for(i = 0;i < StkNum;i++)
		{
			if(tmpStkIndex[i]==DestGPNo) 
			{
				b=i;
				break;
			}
		}
		if(a!=-1 && b!=-1)
		{
			if(a==0 && b==StkNum-1)
			{
				tmpshort=tmpStkIndex[a];
				memmove(tmpStkIndex,tmpStkIndex+1,(StkNum-1)*sizeof(short));
				tmpStkIndex[b]=tmpshort;
			}
			else if(a==StkNum-1 && b==0)
			{
				tmpshort=tmpStkIndex[a];
				memmove(tmpStkIndex+1,tmpStkIndex,(StkNum-1)*sizeof(short));
				tmpStkIndex[b]=tmpshort;
			}
			else
			{
				tmpshort=tmpStkIndex[b];
				tmpStkIndex[b]=tmpStkIndex[a];
				tmpStkIndex[a]=tmpshort;
			}
		}
		m_File.Seek(0,CFile::begin);
		MemStkInfo *tempStkInfo;
		for(i = 0;i < StkNum;i++)
		{
			tempStkInfo = (*g_pStockIO)[tmpStkIndex[i]];
			if(tempStkInfo)
			{
				BOOL bDSStock = g_pStockIO->BeDSStock(tempStkInfo);
				tempLine.Format(bDSStock?"%d#%s":"%d%s",tempStkInfo->setcode,tempStkInfo->Code);
				m_File.WriteString(tempLine+(CString)"\n");
			}
		}
		m_File.SetLength(m_File.GetPosition());
		m_File.Close();
	}
}

void ArrangeBlockGP_InsertBefore(int blockno,int SrcGPNo,int DestGPNo)
{
	if(SrcGPNo==DestGPNo) return;
	CString tempStr = GetBlockFileName(blockno);
	CString tempFileName = g_strBlockNew+tempStr+(CString)".blk";
	CString tempLine;
	int		tempIndex,i;
	short   tmpStkIndex[MAXGPNUM+1],StkNum = 0;
	CStdioFile m_File;
	if( m_File.Open(HomePath+tempFileName,CFile::modeReadWrite|CFile::shareDenyNone) )
	{
		while(m_File.ReadString(tempLine))
		{
			TrimString(tempLine);
			if(tempLine.IsEmpty()) continue;
			tempIndex = g_pStockIO->GetIndex_FromMultiBuf(tempLine);
			if(tempIndex >= 0)
				tmpStkIndex[StkNum++] = tempIndex;
		}
		int sa=-1,sb=-1;
		for(i=0;i < StkNum;i++)
		{
			if(tmpStkIndex[i]==SrcGPNo) 
			{
			//	memmove(tmpStkIndex+i,tmpStkIndex+i+1,sizeof(short)*(StkNum-i-1));
			//	StkNum--;
				sa = i;
				break;
			}
		}		
		for(i = 0;i < StkNum;i++)
		{
			if(tmpStkIndex[i]==DestGPNo) 
			{
			//	memmove(tmpStkIndex+i+1,tmpStkIndex+i,sizeof(short)*(StkNum-i));
			//	tmpStkIndex[i] = SrcGPNo;
			//	StkNum++;
				sb = i;
				break;
			}
		}
		if(sa>=0&&sb>=0)
		{
			short nTmp = tmpStkIndex[sa];
			tmpStkIndex[sa] = tmpStkIndex[sb];
			tmpStkIndex[sb] = nTmp;
		}

		m_File.Seek(0,CFile::begin);
		MemStkInfo *tempStkInfo;
		for(i = 0;i < StkNum;i++)
		{
			tempStkInfo = (*g_pStockIO)[tmpStkIndex[i]];
			if(tempStkInfo)
			{
				BOOL bDSStock = g_pStockIO->BeDSStock(tempStkInfo);
				tempLine.Format(bDSStock?"%d#%s":"%d%s",tempStkInfo->setcode,tempStkInfo->Code);
				m_File.WriteString(tempLine+(CString)"\n");
			}
		}
		m_File.SetLength(m_File.GetPosition());
		m_File.Close();
	}
}

BOOL InitCalcInterface(DTopViewCalc* &pCalc)
{
	if(pCalc) return TRUE;
	//��ʼ��g_pCalc
	pCalc = new DTopViewCalc;
	//����ĳЩ���˰�,����֧�ֵ�����ʽ
#if defined(OEM_STAR) || defined(OEM_STAR_HM)
	pCalc->_CallDTopViewCalc_1(TRUE,FALSE,g_lOpenRq,0,NULL,NULL);
#elif PERSONAL
	if(g_nPersonalVersion==PERSONAL_JJTZ)
		pCalc->_CallDTopViewCalc_1(TRUE,FALSE,g_lOpenRq,0,"������ɫָ��","������ʤ��ѡ��");
	else if(g_nPersonalVersion==PERSONAL_JJTZEX)
		pCalc->_CallDTopViewCalc_1(TRUE,FALSE,g_lOpenRq,0,"������ɫָ��","�������ܰ�ѡ��");
	else
		pCalc->_CallDTopViewCalc_1(TRUE,FALSE,g_lOpenRq,0,NULL,NULL);
#else
	pCalc->_CallDTopViewCalc_1(FALSE,FALSE,g_lOpenRq,0,NULL,NULL);
#endif
	if(!pCalc->_CallDTopViewCalc_0(HomePath.GetBuffer(0),g_WSXHStr.GetBuffer(0),(g_WSXHStr+"100nian.dat").GetBuffer(0)))
		return FALSE;
	pCalc->_CallDTopViewCalc_2((PDATAIOFUNC)CalcCallBackFunc);
	return TRUE;
}

void GetCorrespondingTime(SYSTEMTIME *ltime)
{
	GetLocalTime( ltime );
	unsigned long ltmp,lseconds = ltime->wHour*3600+ltime->wMinute*60+ltime->wSecond;
	if(OnLine && g_LoginServerSeconds != 0)
	{
		ltmp = g_LoginServerSeconds - g_LoginLocalSeconds + lseconds;
		ltime->wHour	  = ltmp/3600;
		ltime->wMinute = (ltmp%3600)/60;
		ltime->wSecond = ltmp%60;
	}
}

void GetDSCorrespondingTime(SYSTEMTIME *ltime)
{
	GetLocalTime( ltime );
	unsigned long ltmp,lseconds = ltime->wHour*3600+ltime->wMinute*60+ltime->wSecond;
	if(OnLine && g_LoginDSServerSubLocal != 0)
	{
		ltmp = g_LoginDSServerSubLocal + lseconds;
		ltime->wHour	  = ltmp/3600;
		ltime->wMinute = (ltmp%3600)/60;
		ltime->wSecond = ltmp%60;
	}
}

DWORD	GetLastDate(DWORD nDate)
{
	BYTE days_of_mon[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	long year=nDate/10000l,mon=(nDate%10000)/100,day=nDate%100;
	if(year<1900 || year>4000 || mon<=0 || mon>12 || day<=0 || day>31)
		return 0;	
	days_of_mon[1]=(year%400==0||year%4==0&&year%100)?29:28;
	if(day>1)  return(nDate-1l);
	if(mon>1)  return(year*10000l+100l*(mon-1)+days_of_mon[mon-2]);
	return((year-1l)*10000l+1231l);
}

long	GetNowWorkMinute(BOOL &bLastDate, PTIMEINF pTimer, BYTE Market)
{
	long nNowWorkMin = GetCorrespondingMin(TRUE);
	int nXH = DSGetMarketXH(Market);
	if(nXH==-1)
		return nNowWorkMin;
	int nDelayMin = g_pDSMarket[nXH].DelayHQ;
	if(nDelayMin>0) nNowWorkMin -= nDelayMin;
	bLastDate = FALSE;
	if(nNowWorkMin+1440<=pTimer->EndTimer)
		nNowWorkMin += 1440;
	if(nNowWorkMin>=1440) bLastDate = TRUE;
	return nNowWorkMin;
}

long	GetNowWorkDate(PTIMEINF pTimer, long nDataDate)
{
	if(!pTimer) return nDataDate;	
	BOOL bLastDate = FALSE; long nWorkDataDate =  nDataDate;
	GetNowWorkMinute(bLastDate,pTimer,pTimer->Market);
	if(bLastDate) nWorkDataDate = GetLastDate(nWorkDataDate);
	return nWorkDataDate;
}

long GetDSCorrespondingDate(PTIMEINF pTimer)
{
	SYSTEMTIME t;
	GetCorrespondingTime(&t);
	long nServerDate = 10000*t.wYear+100*t.wMonth+t.wDay;
	if(pTimer) return GetNowWorkDate(pTimer, nServerDate);
	return nServerDate;
}

int GetSYSCurrentGPIndex()
{
	int tempCurrentGPNo = -1;
	if(theViewManager.bClosing)	//������ڹر��У���ֱ�ӷ��ؿ�
		return tempCurrentGPNo;
	if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CGridView)) )
		tempCurrentGPNo = ((CGridView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
		tempCurrentGPNo = ((CComView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)) )
		tempCurrentGPNo = ((CComView2 *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTxtView)) )
		tempCurrentGPNo = ((CTxtView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CIEBrowserView)) )
		tempCurrentGPNo = ((CIEBrowserView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CJbfxView)) )
		tempCurrentGPNo = ((CJbfxView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CJJView)) )
		tempCurrentGPNo = ((CJJView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CTvView)) )
		tempCurrentGPNo = ((CTvView *)theViewManager.pActiveView)->GetCurrentGPIndex();
	
	return tempCurrentGPNo;
}

int GetSYSCurrentGPSet(short *pPageGPIndex)
{
	int nTempPageGPNum = 0;
	if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CGridView)) )
		nTempPageGPNum = ((CGridView *)theViewManager.pActiveView)->GetCurrentGPSet(pPageGPIndex);
	else if( theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)) )
		nTempPageGPNum = ((CComView *)theViewManager.pActiveView)->GetCurrentGPSet(pPageGPIndex);

	return nTempPageGPNum;
}

void GetSYSCurrentLSetInfo(LinkedSetInfo *&pLSetInfo)
{
	for(int i = 0;i<theViewManager.GetWindowNum();i++)
	{
		if( ((CView*)theViewManager.arViews[i])->IsKindOf(RUNTIME_CLASS(CGridView)) )
			((CGridView *)theViewManager.arViews[i])->GetSYSCurrentLSetInfo(pLSetInfo);
	}
}

void Set_OP_Scheme()
{
	SCHEME_COLOR tmpColor;
	SCHEME_FONT  tmpFont;
	SCHEME_MENU  tmpMenu;

	tmpColor.TXTCOLOR  = VipColor.TXTCOLOR;

	tmpColor.AXISCOLOR  = VipColor.AXISCOLOR;
	tmpColor.TIMECOLOR  = VipColor.TIMECOLOR;
	
	tmpColor.KPCOLOR  = VipColor.KPCOLOR;
	tmpColor.KNCOLOR  = VipColor.KNCOLOR;
	tmpColor.DOWNCOLOR  = VipColor.DOWNCOLOR;
	tmpColor.LEVELCOLOR  = VipColor.LEVELCOLOR;

	tmpColor.VOLCOLOR  = GridColor.Grid_Volume_Color;
	tmpColor.BIGTITLECOLOR  = VipColor.BIGTITLECOLOR;
	tmpColor.FLOATCURSORCOLOR  = VipColor.FLOATCURSORCOLOR;

	tmpColor.Grid_CodeName_Color  = GridColor.Grid_CodeName_Color;
	tmpColor.GridLineColor = GridColor.GridLineColor;
	tmpColor.GridHighLineColor = GridColor.GridHighLineColor;
	tmpColor.GridTitleTextColor = GridColor.GridTitleTextColor;
	tmpColor.GridTitleArrowColor = GridColor.GridTitleArrowColor;
	tmpColor.Grid_Spec_Color = GridColor.Grid_Other_Color;
	
	tmpColor.ZxColor = TxtColor.TxtForeColor;
	tmpColor.ZxTitleColor = TreeColor.TreeForeColor;
	tmpColor.MineTitleTxtColor = TxtColor.TxtForeColor;
	tmpColor.MineTitleSelColor = VipColor.FLOATCURSORCOLOR;
	tmpColor.MineTitleSpeColor = GridColor.Grid_Up_Color;
	tmpColor.F10TitleColor = TreeColor.TreeForeColor;
	tmpColor.F10TitleSelColor = GridColor.Grid_Up_Color;

	tmpColor.LinkColor = tmpColor.VOLCOLOR;
	tmpColor.ActiveLinkColor = VipColor.KPCOLOR;
	tmpColor.VisitedLinkColor = tmpColor.VOLCOLOR;
	tmpColor.WebSpecColor = GridColor.Grid_Other_Color;

	for(int i=0;i < 6;i++)
		tmpColor.OTHERCOLOR[i] = VipColor.ZBCOLOR[i];
	tmpColor.OTHERCOLOR[6] = RGB(255,0,0);
	tmpColor.OTHERCOLOR[7] = RGB(0,128,0);
	tmpColor.OTHERCOLOR[8] = RGB(64,0,64);
	tmpColor.OTHERCOLOR[9] = RGB(128,255,255);
	tmpColor.OTHERCOLOR[10] =VipColor.TIMECOLOR;

	tmpColor.OTHERCOLOR[0] = VipColor.ZBCOLOR[2];
	tmpColor.OTHERCOLOR[2] = VipColor.ZBCOLOR[0];

	strcpy(tmpFont.ListFontName,g_strFontName[0]);
	strcpy(tmpFont.GraphFontName,g_strFontName[1]);
	strcpy(tmpFont.ZxFontName,g_strFontName[2]);
	tmpFont.ListFontSize  = g_nFontSize[0];
	tmpFont.GraphFontSize = g_nFontSize[1];
	tmpFont.ZxFontSize    = g_nFontSize[2];

	tmpMenu.nMenuMode = g_nMenuMode;
	tmpMenu.bExtendXPStyle = g_bExtendXPStyle;
	tmpMenu.Reserved = 0;

	tmpColor.BACKCOLOR	= TxtColor.TxtBackColor;
	tmpColor.BACKCOLOR2 = TxtColor.TxtBackColor;

	tmpColor.BACKCOLOR	= VipColor.BACKCOLOR;
	tmpColor.BACKCOLOR2 = VipColor.BACKCOLOR;		
}

int GetAllRecentSearch(RECENTSEARCH *pTmpRecentSearch)
{
	int readcnt = 0;
	CFile theFile;
	if(theFile.Open(g_WSXHStr+(CString)"recentsearch.dat",CFile::modeRead|CFile::shareDenyNone))
	{
		//������,�������һ������
		readcnt = (theFile.Read(pTmpRecentSearch,10*sizeof(RECENTSEARCH)))/sizeof(RECENTSEARCH);
		theFile.Close();
	}
	return readcnt;
}

//���ַ������εĺ���
char *ConvertString(char *str)
{
	static char ConvertResult[255];
	ConvertResult[0] = 0;
	unsigned char singchar,tmpchar;
	int  k = 0;
	if(str)
	{
		int len = strlen(str);
		for(int i=0;i < len && k < 200;i++)
		{
			singchar = (unsigned char)str[i];
			tmpchar = (singchar/100+7)%10;
			ConvertResult[k] = tmpchar+'0';
			tmpchar = ((singchar%100)/10+8)%10;
			ConvertResult[k+1] = tmpchar+'0';
			tmpchar = (singchar%10+9)%10;
			ConvertResult[k+2] = tmpchar+'0';
			k+=3;
		}
		ConvertResult[k] = 0;
	}
	return ConvertResult;
}

char *ReConvertString(char *str)
{
	static char ReConvertResult[255];
	ReConvertResult[0] = 0;
	int  tmpdigit1,tmpdigit2,tmpdigit3;
	int  k = 0;
	if(str)
	{
		int len = strlen(str);
		for(int i=0;i < len && k < 200;i+=3)
		{
			tmpdigit1 = str[i]-'0';
			if(tmpdigit1 >= 7)	tmpdigit1 = tmpdigit1-7;
			else				tmpdigit1 = tmpdigit1+3;

			tmpdigit2 = str[i+1]-'0';
			if(tmpdigit2 >= 8)	tmpdigit2 = tmpdigit2-8;
			else				tmpdigit2 = tmpdigit2+2;

			tmpdigit3 = str[i+2]-'0';
			if(tmpdigit3 >= 9)	tmpdigit3 = tmpdigit3-9;
			else				tmpdigit3 = tmpdigit3+1;

			ReConvertResult[k++] = tmpdigit1*100+tmpdigit2*10+tmpdigit3;
		}
		ReConvertResult[k] = 0;
	}
	return ReConvertResult;
}

//��ѡ�ɽ������ļ�
void MakeBlock(short *pIndex,int GPNum,int WhichBlock,BOOL bOpenHQ)
{
	CStdioFile m_File;
	DWORD EndPosition;
	CString strBlock = GetBlockFileName(WhichBlock+2),tempLine;
	CString tempStr = (CString)g_strBlockNew+strBlock+".blk";
	if( m_File.Open(HomePath+tempStr,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) )
	{
		for(int i = 0; i < GPNum; i++)
		{
			if(pIndex[i]>=0)
			{
				BOOL bDSStock = g_pStockIO->BeDSStock((*g_pStockIO)[pIndex[i]]->setcode,(*g_pStockIO)[pIndex[i]]->Code);
				tempLine.Format(bDSStock?"%d#%s":"%d%s",(*g_pStockIO)[pIndex[i]]->setcode,(*g_pStockIO)[pIndex[i]]->Code);
				m_File.WriteString((CString)"\n"+tempLine);
			}
		}
		EndPosition = m_File.GetPosition();
		m_File.SetLength(EndPosition);
		m_File.Close();		

		LoadColorfulBlockGP();

		if(bOpenHQ)
		{
			g_bOldGridDomain=FALSE;
			CGridView *tempWnd = (CGridView *)SwitchToView("Grid");
			if(tempWnd)
				tempWnd->SendMessage(WM_CHANGEBLOCK,WhichBlock);
			g_bOldGridDomain=TRUE;
			AfxGetMainWnd()->SetFocus();
		}
	}
	else
		TDX_MessageBox(NULL,"���̴���,���ܴ�Ϊ���!������дȨ�޻�ϵͳ��Դ����!",MB_ICONEXCLAMATION|MB_OK);
}

void	SeveToNewBlock(vector<short> &pIndex, char *Code, char *Name, char BlockTag)
{
	int nNewXh = g_nBlockNum;
	for(int i = 0;i < g_nBlockNum;i++)
	{
		if(strcmp(g_BlockInfo[i].KeyGuyCode, Code)==0)
		{
			nNewXh = i;
			break;
		}
	}
	g_BlockInfo[nNewXh].tag = BlockTag;
	strcpy(g_BlockInfo[nNewXh].Name,Name);
	strcpy(g_BlockInfo[nNewXh].KeyGuyCode,Code);
	if(nNewXh==g_nBlockNum) g_nBlockNum++;

	//�������ļ�
	CString tempEmptyFile =(CString)g_strBlockNew+(CString)Code+(CString)".blk", tempLine;
	CStdioFile m_EmptyFile;
	if( m_EmptyFile.Open(HomePath+tempEmptyFile,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) == NULL)
		return;
	else
	{
		for(int i = 0; i < pIndex.size(); i++)
		{
			if(pIndex[i]>=0)
			{
				BOOL bDSStock = g_pStockIO->BeDSStock((*g_pStockIO)[pIndex[i]]->setcode,(*g_pStockIO)[pIndex[i]]->Code);
				tempLine.Format(bDSStock?"%d#%s":"%d%s",(*g_pStockIO)[pIndex[i]]->setcode,(*g_pStockIO)[pIndex[i]]->Code);
				m_EmptyFile.WriteString((CString)"\n"+tempLine);
			}
		}
		DWORD EndPosition = m_EmptyFile.GetPosition();
		m_EmptyFile.SetLength(EndPosition);
		m_EmptyFile.Close();
	}
}
/**********************************************************************
flagΪ0:ֻ����Ʊ 
flagΪ1:ֻ������,���ܺ�ΪMessageNo 
flagΪ2:�Ȼ���Ʊ�ֻ����ܣ����ܺ�ΪMessageNo
**********************************************************************/
void OpenComView(int flag,const char* Code,short setcode,int MessageNo,BOOL bNewView,short *pIndex,int TotalRow,int CurrentNo,LinkedSetInfo *pLSetInfo)
{
	if((flag==0 || flag==2) && Code)
	{
		ComCurGPCode = Code;
		ComCurSetCode = setcode;
	}

	if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
	{
		CComView2 *tmpView2=(CComView2 *)FindViewThenActive("Com2");
		if(tmpView2&&flag==1&&
			(MessageNo == 8 || (MessageNo >= 801 && MessageNo <= 812)))
		{
			tmpView2->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			return;
		}
	}
	CComView *tmpView=(CComView *)FindViewThenActive("Com");
	if(tmpView)
	{
		if(flag==0)
		{
			tmpView->SendMessage(WM_CHANGEGP_MESSAGE);
			if(!pIndex)	
			{
				if(!tmpView->IsInPageGP(ComCurSetCode,ComCurGPCode))
					tmpView->SetPageGP(NULL,-1,-1);
			}
			else		
				tmpView->SetPageGP(pIndex,TotalRow,CurrentNo,pLSetInfo);
		}
		else if(flag==1)
		{
			tmpView->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			if(MessageNo>=101 && MessageNo<=119) //����Ǵ���,�������÷�ҳ����
				tmpView->SetPageGP(NULL,-1,-1);
		}
		else if(flag==2)
		{
			tmpView->SendMessage(WM_CHANGEALL_MESSAGE,MessageNo);
			if(MessageNo!=900)
			{
				if(!pIndex)	
				{
					if(!tmpView->IsInPageGP(ComCurSetCode,ComCurGPCode))
						tmpView->SetPageGP(NULL,-1,-1);
				}
				else		
					tmpView->SetPageGP(pIndex,TotalRow,CurrentNo,pLSetInfo);
			}
		}
		if(!theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView)))
			theViewManager.OnActivateView(TRUE,tmpView); //��������һ��ComView,������ʱ�����ܵõ�����
		return;
	}

	if(MessageNo == 8 || (MessageNo >= 801 && MessageNo <= 812)) return; //�л�����,���������ɷ���ͼ
	if(flag==1 || flag==2) 
		ComCurMessageCode = MessageNo;
	tmpView=(CComView *)OpenNewView("Com");
	if(tmpView)
	{
		if(MessageNo!=900) //�ж���û�м����View,900������,���Լ��з�ҳ����
		{
			if(MessageNo >= 30000) //������ر�ָ��,�������䷢����Ϣ
				tmpView->SendMessage(WM_KEYGUY_MESSAGE,MessageNo);
			if(flag==0 || flag==2)
			{
				if(!pIndex)	
				{
					if(!tmpView->IsInPageGP(ComCurSetCode,ComCurGPCode))
						tmpView->SetPageGP(NULL,-1,-1);
				}
				else		
					tmpView->SetPageGP(pIndex,TotalRow,CurrentNo,pLSetInfo);
			}
			else if(MessageNo>=101 && MessageNo<=119)
				tmpView->SetPageGP(NULL,-1,-1);
		}
	}
}

void ToZst(const char* Code,short setcode,short bZst) //bZstΪTRUE,ǿ�ƽ�������ͼ
{
	if(bZst==1) 
		OpenComView(2,Code,setcode,501);
	else if(bZst==0)
		OpenComView(2,Code,setcode,300);	//ֻ����Ʊ
	else
	{
		//�����ǰ�Ƕ��ư���,���л������ͼ
		if(theViewManager.pActiveView->IsKindOf(RUNTIME_CLASS(CComView2)))	
		{
			//�����ComView2�������Ƿ�֧���ⲿ��Ʊ�л�
			CComView2 *pView = (CComView2 *)theViewManager.pActiveView;
			if(pView->Get_Extern_GPSwitch())
			{
				int iNo = g_pStockIO->GetIndex(Code,setcode);
				pView->ChangeSyncUnitGP(iNo);
				return;
			}
		}
	}
}

//��һ��ComView�д�һ����Ʊ
void ShowGpInCom(const char* Code,short setcode,BOOL bNewView,short *pIndex,int TotalRow,int CurrentNo,LinkedSetInfo *pLSetInfo)
{
	if(!HQControl::IsValidProgram1()) 
		return;
	OpenComView(0,Code,setcode,0,bNewView,pIndex,TotalRow,CurrentNo,pLSetInfo);	//ֻ����Ʊ
}

void ETSendCopyDataMessage(HWND ETHWnd,MemStkInfo *pStkInfo,struct CurrStockData * hqp)
{
	static struct ExternHQ l_lastHQ;
	static struct TdxJyHQ l_lastJyHQ;
	if(ETHWnd) //������ߵĴ��ڷ���������Ϣ
	{
		if(g_ET.IsTdxJyHQ)
		{
			g_ET.JyHQ.Close=hqp->Close;
			g_ET.JyHQ.Now=hqp->Now;
			for(int i=0;i<5;i++)
			{
				g_ET.JyHQ.Buyp[i]=hqp->Buyp[i];
				g_ET.JyHQ.Buyv[i]=hqp->Buyv[i];
				g_ET.JyHQ.Sellp[i]=hqp->Sellp[i];
				g_ET.JyHQ.Sellv[i]=hqp->Sellv[i];
				if(g_ET.JyHQ.Buyv[i]==0)
					g_ET.JyHQ.Buyp[i]=0.0;
				if(g_ET.JyHQ.Sellv[i]==0)
					g_ET.JyHQ.Sellp[i]=0.0;
			}
			g_ET.JyHQ.gzlx=hqp->Yield_VolInStock.Yield;
			g_ET.JyHQ.Volume=hqp->Volume;
			if(g_ET.Ready==0 || memcmp(&l_lastJyHQ,&g_ET.JyHQ,sizeof(struct TdxJyHQ))) //�������鲻һ��,�ŷ���
			{
				l_lastJyHQ = g_ET.JyHQ;
				COPYDATASTRUCT tmpCopyData;
				memset(&tmpCopyData,0,sizeof(COPYDATASTRUCT));
				tmpCopyData.dwData = 0;
				tmpCopyData.lpData = new char[sizeof(struct TdxJyHQ)+100];
				memcpy((char *)tmpCopyData.lpData,&g_ET.GPSetCode,1);
				memcpy((char *)tmpCopyData.lpData+1,g_ET.GPCode,SH_CODE_LEN+1);
				memcpy((char *)tmpCopyData.lpData+1+SH_CODE_LEN+1,&g_ET.JyHQ,sizeof(struct TdxJyHQ));
				tmpCopyData.cbData = 1+SH_CODE_LEN+1+sizeof(struct TdxJyHQ);
				::SendMessage(ETHWnd,WM_COPYDATA,NULL,(LPARAM)(&tmpCopyData));
				delete tmpCopyData.lpData;
			}
		}
		else
		{
			g_ET.HQ.last_price=(long)(hqp->Now*1000+0.5);
			g_ET.HQ.open_price=(long)(hqp->Open*1000+0.5);
			g_ET.HQ.close_price=(long)(hqp->Close*1000+0.5);
			g_ET.HQ.high_price=(long)(hqp->Max*1000+0.5);
			g_ET.HQ.low_price=(long)(hqp->Min*1000+0.5);
			g_ET.HQ.business_balance=hqp->Amount;
			g_ET.HQ.business_amount=hqp->Volume*100; //�˴����ܻ����
			g_ET.HQ.buy_price1=(long)(hqp->Buyp[0]*1000+0.5);
			g_ET.HQ.buy_price2=(long)(hqp->Buyp[1]*1000+0.5);
			g_ET.HQ.buy_price3=(long)(hqp->Buyp[2]*1000+0.5);
			g_ET.HQ.buy_price4=(long)(hqp->Buyp[3]*1000+0.5);
			g_ET.HQ.buy_price5=(long)(hqp->Buyp[4]*1000+0.5);
			g_ET.HQ.sale_price1=(long)(hqp->Sellp[0]*1000+0.5);
			g_ET.HQ.sale_price2=(long)(hqp->Sellp[1]*1000+0.5);
			g_ET.HQ.sale_price3=(long)(hqp->Sellp[2]*1000+0.5);
			g_ET.HQ.sale_price4=(long)(hqp->Sellp[3]*1000+0.5);
			g_ET.HQ.sale_price5=(long)(hqp->Sellp[4]*1000+0.5);
			g_ET.HQ.buy_amount1=hqp->Buyv[0]*100;
			g_ET.HQ.buy_amount2=hqp->Buyv[1]*100;
			g_ET.HQ.buy_amount3=hqp->Buyv[2]*100;
			g_ET.HQ.buy_amount4=hqp->Buyv[3]*100;
			g_ET.HQ.buy_amount5=hqp->Buyv[4]*100;
			g_ET.HQ.sale_amount1=hqp->Sellv[0]*100;
			g_ET.HQ.sale_amount2=hqp->Sellv[1]*100;
			g_ET.HQ.sale_amount3=hqp->Sellv[2]*100;
			g_ET.HQ.sale_amount4=hqp->Sellv[3]*100;
			g_ET.HQ.sale_amount5=hqp->Sellv[4]*100;
			sprintf(g_ET.HQ.Logo_Stock_Code,"GTJA%s",g_ET.GPCode);
			if(memcmp(&l_lastHQ,&g_ET.HQ,sizeof(struct ExternHQ))) //�������鲻һ��,�ŷ���
			{
				l_lastHQ = g_ET.HQ;
				COPYDATASTRUCT tmpCopyData;
				memset(&tmpCopyData,0,sizeof(COPYDATASTRUCT));
				tmpCopyData.dwData = 0;
				tmpCopyData.lpData = new char[sizeof(struct ExternHQ)+1];
				memcpy(tmpCopyData.lpData,&g_ET.HQ,sizeof(struct ExternHQ));
				tmpCopyData.cbData = sizeof(struct ExternHQ)+1;
				::SendMessage(ETHWnd,WM_COPYDATA,NULL,(LPARAM)(&tmpCopyData));
				delete tmpCopyData.lpData;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//					֧�ֵ������г��ĺ���
/////////////////////////////////////////////////////////////////////////////

void	DSCheckSrvInfo(void)
{
	SRVINFOANS	LocalSrvInfo={"",-1,-1,0,0,0};
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_code.dat","rb",_SH_DENYNO);
	if(fp)
	{
		fread(&LocalSrvInfo,sizeof(SRVINFOANS),1,fp);
		LocalSrvInfo.QSID[10]=0;
		fclose(fp);
	}
	if(stricmp(LocalSrvInfo.QSID,g_iDsSrvInfo.QSID))
		g_bDSNeedIdx = g_bDSNeedTimer = TRUE;
	else
	{
		g_bDSNeedIdx = (LocalSrvInfo.IndexVer!=g_iDsSrvInfo.IndexVer);
		g_bDSNeedTimer = (LocalSrvInfo.TimerVer!=g_iDsSrvInfo.TimerVer);
		if(!g_bDSNeedIdx)
			g_bDSNeedIdx=(LocalSrvInfo.IndexNum!=g_iDsSrvInfo.IndexNum);
		if(!g_bDSNeedTimer)
			g_bDSNeedTimer = (LocalSrvInfo.TimerNum!=g_iDsSrvInfo.TimerNum);
	}
	if(g_bDSNeedIdx||g_bDSNeedTimer) 
		g_bDSNeedIdx = g_bDSNeedTimer = TRUE;
	if(g_bDSNeedIdx || g_bDSNeedTimer)
	{
		DeleteFile(g_WSXHStr+"LData\\cache\\dshq.dat");
		DeleteFile(g_WSXHStr+"LData\\cache\\dshqex.dat");
	}
}

BOOL	DSSaveSrvInfo(BOOL bInitEmpty)
{
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_code.dat",bInitEmpty?"wb":"r+b",_SH_DENYNO);
	if(fp)
	{
		if(bInitEmpty)
		{
			SRVINFOANS	LocalSrvInfo={"",-1,-1,0,0,0};
			fwrite(&LocalSrvInfo,sizeof(SRVINFOANS),1,fp);
		}
		else
			fwrite(&g_iDsSrvInfo,sizeof(SRVINFOANS),1,fp);
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

void	DSWriteSrvInfo(short StkNum,PSTKINFO pStkInf)
{
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_code.dat","ab",_SH_DENYNO);
	if(fp)
	{
		fseek(fp,0,SEEK_END);
		fwrite(pStkInf,sizeof(STKINFO),StkNum,fp);
		fclose(fp);
	}
}

void	DSWriteTimer(short TimerNum,PTIMEINF pTimer)
{
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_time.dat","ab+",_SH_DENYNO);
	if(fp)
	{
		fseek(fp,0,SEEK_END);
		fwrite(pTimer,sizeof(TIMEINF),TimerNum,fp);
		fclose(fp);
	}
}

BOOL	DSLoadTimer(long TimerNum,PTIMEINF pTimer,short *pTimerMap)
{
	TIMEINF iTimer[MAX_DS_TIMERNUM]={0};
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_time.dat","rb",_SH_DENYNO);
	if(fp)
	{
		fread(iTimer,sizeof(TIMEINF),TimerNum,fp);
		for(int i=0;i<TimerNum;++i)
		{
			if(iTimer[i].OpenTimer[1]==0)
				iTimer[i].OpenTimer[1]=iTimer[i].CloseTimer[1]=iTimer[i].CloseTimer[0];

			int nNext = -1;
			for(int j=0;j<MAX_DS_TIMERNUM;j++)
			{
				if(iTimer[i].Market==pTimer[j].Market&&memcmp(iTimer[i].strMask,pTimer[j].strMask,4)==0)	//�ҵ���
				{
					memcpy(&pTimer[j], &iTimer[i], sizeof(TIMEINF));
					pTimerMap[i] = j;
					break;
				}
				if(nNext<0&&pTimer[j].Market==0&&pTimer[j].strMask[0]==0&&pTimer[j].EndTimer==0)				//�հ׵�
					nNext = j;
			}
			if(j==MAX_DS_TIMERNUM&&nNext>=0)
			{
				memcpy(&pTimer[nNext], &iTimer[i], sizeof(TIMEINF));
				pTimerMap[i] = nNext;
			}
		}
		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

//���г���Ϣ����
BOOL	DSSaveMarketInfo(MKTINFO *pMktInfo,int MktNum)
{
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_mrk.dat","wb",_SH_DENYNO);
	if(fp)
	{
	//	fwrite(pMktInfo,sizeof(MKTINFO),MktNum,fp);
		for(int i=0;i<MktNum;i++)
		{
			if(pMktInfo[i].MktType!=47&&pMktInfo[i].MktType!=28&&pMktInfo[i].MktType!=29&&pMktInfo[i].MktType!=30&&pMktInfo[i].MktType!=50) continue;
			fwrite(&pMktInfo[i],sizeof(MKTINFO),1,fp);
		}

		fclose(fp);
		return TRUE;
	}
	return FALSE;
}

//���г���Ϣ����
BOOL	DSLoadMarketInfo()
{
	short nMarketNum = 0;
	MKTINFO	pMktInfo[MAX_MKT_NUM]={0};
	FILE *fp=_fsopen(g_WSXHStr+"LData\\cache\\ds_mrk.dat","rb",_SH_DENYNO);
	if(fp)
	{
		nMarketNum = fread(pMktInfo,sizeof(MKTINFO),MAX_MKT_NUM,fp);
		fclose(fp);
		//����DSMarket��Ϣ
		int i,j,k,kk;
		memset(g_pDSMarket,0,MAX_MKT_NUM*sizeof(MKTINFO));
		g_nDSMarketNum=0;
		for(j=0;j < nMarketNum;j++)
		{
			if(pMktInfo[j].MktType<12)		continue;	//���ڴ˰汾,������ҪAB��
			if(strlen(pMktInfo[j].Name)==0) continue;	//�����Ƶ����Ͳ�����
			//���Ҫ�������GUI�У�����˴���
			BOOL bExistInGUI=FALSE;
			if(!g_DS.MustInGUI || g_GuiMarketNum==0)
				bExistInGUI=TRUE;
			else
			{
				for(k=0;k<g_GuiMarketNum;k++)
				{
					for(kk=0;kk < MAX_MKT_NUM;kk++)
					{
						if(pMktInfo[j].MktType==g_GuiMarketSet[k].MarketSet[kk])
						{
							bExistInGUI=TRUE;
							break;
						}
					}
					if(bExistInGUI) break;
				}
			}
			if(bExistInGUI)
			{
				g_pDSMarket[g_nDSMarketNum]=pMktInfo[j];
				g_nDSMarketNum++;
			}
		}
		//����GuiMarketSet��Ϣ
		if(g_GuiMarketNum==0)
		{
			for(i=0;i<min(MAX_MKTGUI_NUM,g_nDSMarketNum);i++)
			{
				strcpy(g_GuiMarketSet[i].BigsetName,g_pDSMarket[i].Name);
				g_GuiMarketSet[i].MarketSet[0]=g_pDSMarket[i].MktType;
				g_GuiMarketSet[i].MarketSet[1]=0;
				strcpy(g_GuiMarketSet[i].MarketName[0],g_pDSMarket[i].Name);
				g_GuiMarketNum++;
			}
		}
		else
		{
			BOOL bExistInGUI=FALSE;
			int  OtherBreedNum=0;
			for(i=0;i<min(MAX_MKTGUI_NUM,g_nDSMarketNum);i++)
			{
				bExistInGUI=FALSE;
				for(k=0;k<g_GuiMarketNum;k++)
				{
					for(kk=0;kk < MAX_MKT_NUM;kk++)
					{
						if(g_GuiMarketSet[k].MarketSet[kk]==0) break;
						if(g_pDSMarket[i].MktType==g_GuiMarketSet[k].MarketSet[kk])
						{
							bExistInGUI=TRUE;
							break;
						}
					}
					if(bExistInGUI) break;
				}
				if(!bExistInGUI && g_GuiMarketNum<MAX_MKTGUI_NUM-1 && OtherBreedNum<MAX_MKT_NUM-1)
				{
					strcpy(g_GuiMarketSet[g_GuiMarketNum].BigsetName,"����Ʒ��");
					g_GuiMarketSet[g_GuiMarketNum].MarketSet[OtherBreedNum]=g_pDSMarket[i].MktType;
					strcpy(g_GuiMarketSet[g_GuiMarketNum].MarketName[OtherBreedNum],g_pDSMarket[i].Name);
					g_GuiMarketSet[g_GuiMarketNum].MarketSet[OtherBreedNum+1]=0;
					OtherBreedNum++;
				}
			}
			if(OtherBreedNum>0) g_GuiMarketNum++;
		}
		return TRUE;
	}
	return FALSE;
}

int		DSGetMarketXH(BYTE Market)
{
	for(int i=0;i<g_nDSMarketNum&&g_pDSMarket[i].MktType!=Market;i++);
	if(i<g_nDSMarketNum) return i;
	return -1;
}

int		DSGetMarketFromJc(char *dsjc)
{
	for(int i=0;i<g_nDSMarketNum&&strcmp(g_pDSMarket[i].Jc,dsjc);i++);
	if(i<g_nDSMarketNum) return g_pDSMarket[i].MktType;
	return -1;
}

int		DSGetMarketXHFromGuiOrder(int nGuiOrder)
{
	int k,kk,nCnt=0;
	for(k=0;k<g_GuiMarketNum;k++)
	{
		for(kk=0;kk < MAX_MKT_NUM;kk++)
		{
			int nDSMarketID = g_GuiMarketSet[k].MarketSet[kk];
			if(nDSMarketID==0) continue;
			int nXH = DSGetMarketXH(nDSMarketID);
			if(nCnt==nGuiOrder)
				return nXH;
			nCnt++;
		}					
	}
	return -1;
}

char	*DSGetMarketNameFromGuiSet(int nGuiSet)
{
	int k,kk;
	for(k=0;k<g_GuiMarketNum;k++)
	{
		for(kk=0;kk < MAX_MKT_NUM;kk++)
		{
			int nDSMarketID = g_GuiMarketSet[k].MarketSet[kk];
			if(nDSMarketID==0) continue;
			if(nDSMarketID==nGuiSet)
				return g_GuiMarketSet[k].MarketName[kk];
		}					
	}
	return "";
}

char	*DSGetMarketNameFromXH(int nMarketXH)
{
	int k,kk;
	for(k=0;k<g_GuiMarketNum;k++)
	{
		for(kk=0;kk < MAX_MKT_NUM;kk++)
		{
			int nDSMarketID = g_GuiMarketSet[k].MarketSet[kk];
			if(nDSMarketID==0) continue;
			int nXH = DSGetMarketXH(nDSMarketID);
			if(nXH<0) continue;
			if(nXH==nMarketXH)
				return g_GuiMarketSet[k].MarketName[kk];
		}					
	}
	return "";
}

/////////////////////////////////////////////////////////////////////////////
long	GetLocalFileSize(LPCSTR FilePath)
{
	CFile thefile;
	int nLen = 0;
	if(thefile.Open(FilePath,CFile::modeRead|CFile::shareDenyNone))
	{
		nLen = thefile.GetLength();
		thefile.Close();
	}
	return nLen;
}

/////////////////////////////////////////////////////////////////////////////
void	StkInfo2Fz(MemStkInfo *pStock, short *TimeSec)
{
	if(g_pStockIO->BeDSStock(pStock))
	{
		PTIMEINF pTimer = g_pStockIO->DSGetTimeInf(pStock->TimerID);
		TimeInf2Fz(pTimer, TimeSec);
	}
	else 
	{
		short nDomain = g_pStockIO->GetDomain(pStock);
		memcpy(TimeSec,nDomain==SZ?sz:sh,sizeof(short)*8);
	}
}

void	TimeInf2Fz(PTIMEINF pTime, short *TimeSec)
{
	TimeSec[0] = pTime->OpenTimer[0];
	TimeSec[1] = pTime->CloseTimer[0];
	short i = 2,j=1;
	for(int k=1;k<MAX_TIMESEC;k++)
	{
		if(pTime->OpenTimer[k]&&pTime->OpenTimer[k]!=pTime->CloseTimer[k])
		{
			TimeSec[2*k] = pTime->OpenTimer[k];
			TimeSec[2*k+1] = pTime->CloseTimer[k];
			i = 2*k+2;
			j = 2*k+1;
		}
		else break;
	}
	for(;i<8;TimeSec[i++]=TimeSec[j]);
}

short	GetDSMinute(PTIMEINF pTime, BOOL bYestodayFlag)
{
	if(pTime==NULL) return(0);
	short fz[8]={0};
	TimeInf2Fz(pTime, fz);
	if(bYestodayFlag==TRUE)
		return GetRealTotalMinute(fz);

	SYSTEMTIME  nowtime;
	GetDSCorrespondingTime( &nowtime );
	int m = nowtime.wHour*60+nowtime.wMinute;  //����
	//����Ƿ�ά����ʱ����
	int nXH = DSGetMarketXH(pTime->Market);
	int nDelayMin = g_pDSMarket[nXH].DelayHQ;
	if(nDelayMin>0) m -= nDelayMin;
	//�����Ʊ�����ʱ��������ʹ�ñ���ʱ�䣬ȱʡ�ǵ�8ʱ��
	if(pTime->TimeZero)
		m=m+pTime->TimeZero*60;
	if(m+1440<=pTime->EndTimer) m += 1440;
	return System2Tdx(m,fz);
}

#define MIN_KEYNUM	1
#define MAX_KEYNUM	99
const char defNum[3] = {'0', '0', '0'};
void Num2KeyString(char *keyGuy, int nIndex)
{
	if(nIndex<MIN_KEYNUM||nIndex>MAX_KEYNUM) strcpy(keyGuy, "");
	
	CString str = IntToStr(nIndex);
	if(str.GetLength()>3) return;
	strcpy(keyGuy, defNum);
	for(int i=str.GetLength()-1, j=2;i>=0;i--, j--)
	{
		if(str[i]=='0') continue;
		else keyGuy[j] = str[i];
	}
}

int		GetNextDef()
{
	long nTmpDef[100]; memset(nTmpDef, 0, 100*sizeof(long));
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(g_vDynaCmdTab[i].nCmdNum<1||g_vDynaCmdTab[i].nCmdNum>=100) continue;
		nTmpDef[g_vDynaCmdTab[i].nCmdNum] = 1;
	}
	for(i=1;i<100;i++)
	{
		if(nTmpDef[i]==0)
			return i;
	}
	return 0;
}

int		GetCusPadTypeParam(char *padCodeName)
{
	for(int i=0;i<g_vDynaCmdTab.size();i++)
	{
		if(strcmp(g_vDynaCmdTab[i].ChName, padCodeName)==0)
			return i;
	}
	return -1;
}

//��̬������Ĵ���
void	GetDynaCusPadInfo()
{
	g_vDynaCmdTab.clear();
	vector<int> nDupliNum; nDupliNum.clear();
	//���ư���
	CFileFind finder;
	CString tmpPadIni;
	tmpPadIni.Format("%ssheet\\*.sp",g_WSXHStr);
	BOOL bWorking = finder.FindFile(tmpPadIni);
	int nInfo, i;
	char info[40];
	int nNextMin = 0;
	CString tmpStr;
	DynaCmdTable dynaTab;
	while (bWorking)
	{
		int nDynaTab = g_vDynaCmdTab.size();
		memset(&dynaTab, 0, sizeof(DynaCmdTable));
		bWorking = finder.FindNextFile();
		tmpStr=(LPCSTR)finder.GetFileTitle();
		strncpy(dynaTab.EnName, tmpStr,14);
		dynaTab.EnName[14]=0;
		if(strlen(dynaTab.EnName)<1) continue;
		nInfo = GetPrivateProfileString("DEAFULTGP", "Name", NULL, info, 38, (LPCSTR)finder.GetFilePath());
		if(nInfo>0) 
		{
			strcpy(dynaTab.ChName, info);
			strcpy(dynaTab.CmdInfo.Name, info);
		}
		else 
		{
			strcpy(dynaTab.ChName, dynaTab.EnName);
			strcpy(dynaTab.CmdInfo.Name, dynaTab.EnName);
		}

		nInfo = GetPrivateProfileString("DEAFULTGP", "ShowShortCut", "1", info, 8, (LPCSTR)finder.GetFilePath());
		if(nInfo>0)	dynaTab.ShowShortCut = atoi(info);
		
		nInfo = GetPrivateProfileString("DEAFULTGP", "CmdNum", NULL, info, 64, (LPCSTR)finder.GetFilePath());
		if(nInfo>0)	dynaTab.nCmdNum = atoi(info);
		else	dynaTab.nCmdNum = GetNextDef();
		
		for(int i=0;i<g_vDynaCmdTab.size();i++)
		{
			if(dynaTab.nCmdNum==g_vDynaCmdTab[i].nCmdNum)
				dynaTab.nCmdNum = GetNextDef();
		}

		nInfo = GetPrivateProfileString("DEAFULTGP", "DefPadSet", NULL, info, 64, (LPCSTR)finder.GetFilePath());
		if(nInfo>0)	dynaTab.DefPadSet = atoi(info);
		nInfo = GetPrivateProfileString("DEAFULTGP", "DefPadCmdID", NULL, info, 64, (LPCSTR)finder.GetFilePath());
		if(nInfo>0)	dynaTab.DefPadCmdID = atoi(info);
		
		char sNum[4]; memset(sNum, 0, 4*sizeof(char));
		Num2KeyString(sNum, dynaTab.nCmdNum);
		CString str = "." + (CString)sNum;
		int nStat = 0; BOOL bFind  = FALSE;
		while(strcmp(g_pCmdTable[nStat].Code, ""))
		{
			if(strcmp(g_pCmdTable[nStat].Code,str)==0)
			{
				bFind = TRUE;
				break;
			}
			nStat++;
		}
		if(bFind) continue;
		strcpy(dynaTab.CmdInfo.Code, (LPCSTR)str);
		dynaTab.CmdInfo.wParam = 0;
		
		vector<DynaCmdTable>::iterator iter = g_vDynaCmdTab.begin();
		for(i=0;i<g_vDynaCmdTab.size()&&iter!=g_vDynaCmdTab.end();i++,iter++)
		{
			if(g_vDynaCmdTab[i].nCmdNum>dynaTab.nCmdNum)
			{
				g_vDynaCmdTab.insert(iter, dynaTab);
				break;
			}
		}
		if(iter==g_vDynaCmdTab.end()) g_vDynaCmdTab.push_back(dynaTab);
	}
	for(i=0;i<g_vDynaCmdTab.size();i++)
		g_vDynaCmdTab[i].CmdInfo.lParam = 2000 + i;
}
/////////////////////////////////////////////////////////////////////////////

//��������
#include "DownLoadDlg.h"
BOOL DownDayData_AtOnce(CWnd *pWnd,char *HintStr,BOOL bShowHint, BOOL bModal)
{
	if(OnLine)
	{
		if(!bShowHint||TDX_MessageBox(pWnd->m_hWnd,HintStr,MB_YESNO|MB_ICONINFORMATION ) == IDYES)
		{
			if(!bModal)
			{
				if(g_pDownLoadDlg&&g_pDownLoadDlg->m_hWnd)
				{
					if (g_pDownLoadDlg->IsIconic()) 
						g_pDownLoadDlg->ShowWindow(SW_RESTORE); 
					return TRUE;
				}

				g_pDownLoadDlg = new CDownloadDlg;
				((CDownloadDlg *)g_pDownLoadDlg)->m_bDown_AtOnce = TRUE;
				((CDownloadDlg *)g_pDownLoadDlg)->Create(IDD_DOWNLOAD_DIALOG);
				g_pDownLoadDlg->ShowWindow (SW_SHOW);
				g_pDownLoadDlg->CenterWindow();
			}
			else
			{
				g_pDownLoadDlg = new CDownloadDlg;
				((CDownloadDlg *)g_pDownLoadDlg)->m_bModal = TRUE;
				((CDownloadDlg *)g_pDownLoadDlg)->m_bDown_AtOnce = TRUE;
				((CDownloadDlg *)g_pDownLoadDlg)->DoModal();
				TDEL(g_pDownLoadDlg);
			}

			return TRUE;
		}
	}
	else
		TDX_MessageBox(pWnd->m_hWnd,"�������ӵ�������վ!",MB_ICONINFORMATION|MB_OK);

	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
long	DrawHqItem(CDC*pDC,long Width,long Height,long y,long ItemNum,PHQITEM pItem,short XSFlag,float fClose,long PerSize,long LineHeight)
{
	char	lpString[32];
	for(long i=0,x1=HQ_XSPACE,x2=Width/2+HQ_XSPACE,xPos=Width/2-2*HQ_XSPACE+1;i<ItemNum;++i)
	{
		if(pItem[i].Type==HQ_LINE)
		{
			g_d.DrawLine(pDC,0,y+1,Width,y+1);
			y += LineHeight;
			continue;
		}
		if(y+16>Height) break;
		int x = pItem[i].Pos?x2:x1;
		if(lstrlen(pItem[i].lpScript))
			g_d.DisplayText(pDC,x,y,VipColor.TXTCOLOR,_F(pItem[i].lpScript));
		memset(lpString,0,32);
		switch(pItem[i].Type)
		{
		case	HQ_BJ_FLOAT:		//	�ǵ���ɫ��ʾfloat
			if(TBE_MEANLESS(pItem[i].fData))
				g_d.BjPrintf(pDC,x,y,pItem[i].fData,fClose,XSFlag,xPos,0);
			break;
		case	HQ_PLUS_FLOAT:		//	ͬ0�Ƚϵ�float��ʾ
			if(TBE_MEANLESS(pItem[i].fData))
				g_d.BjPrintf(pDC,x,y,pItem[i].fData,0.0,XSFlag,xPos,0);
			break;
		case	HQ_PLUS_FLOAT_PER:	//	ͬ0�Ƚϵİٷֱ�
			if(TBE_MEANLESS(pItem[i].fData))
				g_d.BjPrintf(pDC,x,y,pItem[i].fData,0.0,2,xPos,1);
			break;
		case	HQ_VOL_LONG:		//	��ʾ�ɽ���(�ַ���)
			if(pItem[i].lData==0xF4F4F4F4)
				g_d.DisplayText(pDC,x,y,VipColor.VOLCOLOR,xPos,MEANLESS_STR);
			else if(pItem[i].lData!=0xF8F8F8F8)
				g_d.DisplayText(pDC,x,y,VipColor.VOLCOLOR,xPos,MakeVol3(pItem[i].lData));
			break;
		case	HQ_RED_LONG:		//	��ʾ��ɫ(�ַ���)
			if(pItem[i].lData!=0xF8F8F8F8)
				g_d.DisplayText(pDC,x,y,VipColor.KPCOLOR,xPos,MakeVol3(pItem[i].lData));
			break;
		case	HQ_GREEN_LONG:		//	��ʾ��ɫ(�ַ���)
			if(pItem[i].lData!=0xF8F8F8F8)
				g_d.DisplayText(pDC,x,y,VipColor.DOWNCOLOR,xPos,MakeVol3(pItem[i].lData));
			break;
		case	HQ_VOL_PLONG:		//	��ʾ�ɽ���(��ȷ��ʾ����)
			if(pItem[i].lData!=0xF8F8F8F8)
				g_d.DisplayText(pDC,x,y,VipColor.VOLCOLOR,xPos,"%d",pItem[i].lData);
			break;
		case	HQ_FLOAT:			//	��ͨfloat
			if(TBE_MEANLESS(pItem[i].fData))
				g_d.NoBjPrintf(pDC,x,y,VipColor.TXTCOLOR,pItem[i].fData,XSFlag,xPos,0);
			break;
		case	HQ_FLOAT_PER:		//	�ٷֱ���ʾfloat
			if(TBE_MEANLESS(pItem[i].fData))
				g_d.NoBjPrintf(pDC,x,y,VipColor.TXTCOLOR,pItem[i].fData,2,xPos,1);
			break;
		case	HQ_LONG:			//	��ͨlong
			if(pItem[i].lData!=0xF8F8F8F8)
				g_d.DisplayText(pDC,x,y,VipColor.TXTCOLOR,xPos,MakeVol3(pItem[i].lData));
			break;
		case	HQ_STRING:			//	��ͨstring
			g_d.DisplayText(pDC,x,y,VipColor.TXTCOLOR,xPos,_F(pItem[i].lpString));
			break;
		case    HQ_PRICE_SEAT:
			if(TBE_MEANLESS(pItem[i].fData))
			{
				g_d.BjPrintf(pDC,x,y,pItem[i].fData,fClose,XSFlag,xPos,0);
				if(pItem[i].lData>0)
					g_d.DisplayText(pDC,x+xPos+2,y,VipColor.TXTCOLOR,"(%d)",pItem[i].lData);
			}
			break;
		case	HQ_AMOUNT_FLOAT:
			if(TBE_MEANLESS(pItem[i].fData))
				g_d.DisplayText(pDC,x,y,VipColor.TXTCOLOR,xPos,MakeJE3(pItem[i].fData));
			break;
		}
		if(pItem[i].Pos) y += PerSize;
	}
	return(y);
}

BOOL IsZipFile(CString tmpFileName)
{
	CString tmpStr=tmpFileName.Right(4);
	if(stricmp(tmpStr,".zip")==0) return TRUE;
	return FALSE;
}

#include "unzipFile.h"
#define UNZIPBUF_SIZE 4096
BOOL UnZipSomeFile(const char *filename,const char *path,BOOL &bHasEmbwtFile,BOOL &bHasNewHostFile)
{
	bHasEmbwtFile=FALSE;
	bHasNewHostFile=FALSE;
	int dirtoken_pos,dirtoken_from;
	CString tmpFileName;
	CString tmpPath=path;
	if(tmpPath.Right(1) != "\\")
		tmpPath += "\\";	
	//�ȿ���û�д�Zip�ļ�
	CFile theFile;
	if(!theFile.Open(filename,CFile::modeRead|CFile::shareDenyNone))
		return FALSE;
	theFile.Close();

	try
	{
		CUnzipFile uf(filename);
		uf.GoToFirstFile();
		do
		{
			unz_file_info ui;
			uf.GetCurrentFileInfo(&ui);
			int iNameSize = ui.size_filename + 1;
			char* pName = new char [iNameSize+1];
			// �õ�Zip���е��ļ���
			uf.GetCurrentFileInfo(NULL, pName, iNameSize);
			pName[iNameSize]=0;
			//
			dirtoken_pos=-1;
			do
			{
				tmpFileName=pName;
				dirtoken_from=dirtoken_pos+1;
				dirtoken_pos=tmpFileName.Find("\\",dirtoken_from);
				if(dirtoken_pos<0)
					dirtoken_pos=tmpFileName.Find("/",dirtoken_from);
				if(dirtoken_pos>=0)
				{
					//�������Ŀ¼,�����ȴ�����Ŀ¼
					tmpFileName=tmpFileName.Mid(0,dirtoken_pos);
					CreateDirectory(tmpPath+tmpFileName,NULL);
				}
			}while(dirtoken_pos>=0);
			CFile f;
			if(!f.Open((CString)tmpPath + pName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone))
			{
				//����ȥ��ֻ������
				CFileStatus fs;
				CFile::GetStatus((CString)path + pName, fs);
				if(fs.m_attribute&0x01)
				{
					fs.m_attribute ^= 0x01;
					CFile::SetStatus((CString)path + pName, fs);
				}
				if(!f.Open((CString)path + pName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone))
				{					
					delete[] pName;
					continue;
				}
			}
			if(stricmp(pName,"embwt.cfg")==0)
				bHasEmbwtFile=TRUE;
			if(stricmp(pName,"newhost.lst")==0)
				bHasNewHostFile=TRUE;
			delete[] pName;
			uf.OpenCurrentFile();
			char buf[UNZIPBUF_SIZE];
			int size_read;
			do
			{
				size_read = uf.ReadCurrentFile(buf, UNZIPBUF_SIZE);
				if (size_read > 0)
					f.Write(buf, size_read);
			}while (size_read == UNZIPBUF_SIZE);

			uf.UpdateFileStatus(f, ui);
		}while(uf.GoToNextFile());
		uf.Close();
	}
	catch (CException* e)
	{
		if(e)
			e->Delete();
		return FALSE;
	}
	return TRUE;
}

BOOL IsETF(struct MemStkInfo *scode)
{
	if(!g_bSupportETF || !scode) return FALSE;
	if(scode->setcode==SZ && strncmp(scode->Code,"1599",4)==0)
		return TRUE;
	for(int i=0;scode->setcode==SH && i<g_nETFInfoNum;i++)
	{
		if(g_ETFInfo[i].setcode==SH && strcmp(scode->Code,g_ETFInfo[i].Code)==0)
			return TRUE;
	}
	return FALSE;
}

BOOL MinuteTick_SpecialData(struct MemStkInfo *scode)
{
	if(!g_bSupportETF || !scode) return FALSE;
	if(scode->setcode==SZ && (strncmp(scode->Code,"1599",4)==0 || strncmp(scode->Code,"16",2)==0))
	{
		struct CurrStockData tmpHQ;
		g_pStockIO->ReadCurrStockData(scode,&tmpHQ);
		if(tmpHQ.Close>10.00) //����10Ԫ���ǻ���
			return FALSE;
		return TRUE;
	}
	for(int i=0;scode->setcode==SH && i<g_nETFInfoNum;i++)
	{
		if(g_ETFInfo[i].setcode==SH && strcmp(scode->Code,g_ETFInfo[i].Code)==0)
			return TRUE;
	}
	return FALSE;
}

BOOL AnalyData_SpecialData(struct MemStkInfo *scode)
{
	return FALSE;
	if(!g_bSupportETF || !scode) return FALSE;
	if(scode->setcode==SZ && (strncmp(scode->Code,"1599",4)==0 || strncmp(scode->Code,"16",2)==0))
		return TRUE;
	for(int i=0;scode->setcode==SH && i<g_nETFInfoNum;i++)
	{
		if(g_ETFInfo[i].setcode==SH && strcmp(scode->Code,g_ETFInfo[i].Code)==0)
			return TRUE;
	}
	int itmp = GetStockType(scode->setcode,scode->Code);
	return ((itmp==CODE_SHGZ) || (itmp==CODE_SZGZ));	
}

BOOL BelongZXG(MemStkInfo *scodetmp)
{
	if(!scodetmp) return FALSE;
	CString tmpStr;
	if(scodetmp->setcode==SZ||scodetmp->setcode==SH)
		tmpStr.Format("%d%s",scodetmp->setcode,scodetmp->Code);
	else
		tmpStr.Format("%d#%s",scodetmp->setcode,scodetmp->Code);
	for(int i=0;i<g_nMovingZXG;i++)
		if(strcmp(g_strMovingZXG[i],tmpStr)==0) break;
	return (i<g_nMovingZXG);
}

void LoadColorfulBlockGP()
{	
	CString		tempPath;
	CStdioFile	m_File;
	CString		strLine;
	MemStkInfo *pStkInfo;
	for(int i=0;i<8;i++)		
	{
		g_ColorBlockGpNum[i] = 0;
		memset(g_ColorBlockGp[i],0,MAXGPNUM*(SH_CODE_LEN+10)*sizeof(char));

		if(strlen(g_BlockColor[i].BlockName)>0)
		{
			int nIndex = GetBlockIndexFromName(g_BlockColor[i].BlockName);
			if(nIndex>=0)
			{
				tempPath = (CString)g_strBlockNew+GetBlockFileName(nIndex)+".blk";
				if(m_File.Open(HomePath+tempPath,CFile::modeRead|CFile::shareDenyNone))
				{
					while(m_File.ReadString(strLine))
					{
						TrimString(strLine);
						if(strLine.IsEmpty()) continue;
						pStkInfo = g_pStockIO->Get_MemStkInfo_FromMultiBuf(strLine);
						if(!pStkInfo) continue;
						strncpy(g_ColorBlockGp[i][g_ColorBlockGpNum[i]],strLine,SH_CODE_LEN+9);
						g_ColorBlockGp[i][g_ColorBlockGpNum[i]][SH_CODE_LEN+9]=0;
						g_ColorBlockGpNum[i]++;
						if(g_ColorBlockGpNum[i] >= MAXGPNUM) break;
					}
					m_File.Close();
				}
			}
		}
	}
	//����ȡ����ѡ����Ϣ
	int nOldMovingZXG = g_nMovingZXG;
	g_nMovingZXG = 0;
	CString tempLine;
	tempPath = (CString)"selfbk\\self.blk";
	if(m_File.Open(HomePath+tempPath,CFile::modeRead|CFile::shareDenyNone))
	{
		while(m_File.ReadString(tempLine) != FALSE)
		{
			TrimString(tempLine);
			if(tempLine.IsEmpty()) continue;
			pStkInfo = g_pStockIO->Get_MemStkInfo_FromMultiBuf(tempLine);
			if(!pStkInfo) continue;
			strncpy(g_strMovingZXG[g_nMovingZXG],tempLine,SH_CODE_LEN+9);
			g_strMovingZXG[g_nMovingZXG][SH_CODE_LEN+9]=0;
			g_nMovingZXG++;
			if(g_nMovingZXG >= MAXGPNUM) break;
		}
		m_File.Close();
	}
	if(LayoutFlag && g_nMovingZXG!=nOldMovingZXG)		
	{
		CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
		if(pMainWnd)
			pMainWnd->AfterLoadZXG();
	}
}

COLORREF GetBlockGPColor(MemStkInfo *scodetmp,COLORREF clrDefault,int nGPType,int nBlock)
{
	return clrDefault;
	if(scodetmp)
	{
		CString tmpStr;
		if(scodetmp->setcode==SZ||scodetmp->setcode==SH)
			tmpStr.Format("%d%s",scodetmp->setcode,scodetmp->Code);
		else
			tmpStr.Format("%d#%s",scodetmp->setcode,scodetmp->Code);
		int i=0,j=0;
		for(i=0;i<8;i++)
		{
			//�����ǰ������ѡ�ɰ��,�򲻱�ɫ
			if(nGPType==TYPE_ZXG && strcmp(g_BlockColor[i].BlockName,"��ѡ��")==0)
				continue;
			for(j=0;j<g_ColorBlockGpNum[i];j++)
			{
				if(strcmp(g_ColorBlockGp[i][j],tmpStr)==0) 
					return g_BlockColor[i].clrBlock;
			}
		}
	}
	return clrDefault;
}

BOOL IsHligtNowVol(DWORD dwVol,MemStkInfo *pStkInfo)
{
	return FALSE;
	if(!pStkInfo) return FALSE;
	int kind = GetStockType(pStkInfo->setcode,pStkInfo->Code);
	if(kind==CODE_SZAG || kind==CODE_SZBG || kind==CODE_SHAG || kind==CODE_SHBG || kind==CODE_ZXB || kind==CODE_CYB)
	{
		if(dwVol>=g_nHligtNowVol) return TRUE;
	}
	return FALSE;
}

short	GetDefGridCatItems(vector<GRIDCATITEM> &AllTabItems,BOOL bAll)
{
	int nSysItemNum = g_nSysDefGridCatNum;
	if(bAll)
		nSysItemNum = g_nSysDefGridCat_All;
	for(int i=0;i<nSysItemNum;i++)
	{
		if(bAll)
			AllTabItems.push_back(g_SysDefGridCat_All[i]);
		else
			AllTabItems.push_back(g_SysDefGridCat[i]);
	}
	return AllTabItems.size();
}

void LoadDSGridTab()
{
	int lData=0;
	if(g_DS.HasHKMarket)
	{
		GRIDCATITEM aci={0};
		aci.FlagType = DSOPT_ITEM;
		strncpy(aci.lpTitle, "�۹ɰ��", 12);
		aci.msg = WM_COMMAND;
		aci.lData = lData;
		aci.wParam = IDR_LONELY_MENU;
		aci.lParam = 0;
		aci.bMenu = TRUE;
		g_CustomGridCat.push_back(aci);
		lData++;		
	}
	for(int i=0;i<g_GuiMarketNum;i++)
	{
		GRIDCATITEM aci={0};
		aci.FlagType = DSOPT_ITEM;
		strncpy(aci.lpTitle, g_GuiMarketSet[i].BigsetName, 12);
		aci.msg = WM_COMMAND;
		aci.lData = lData;
		if(g_GuiMarketSet[i].MarketSet[1]==0)				//ֻ��һ��
		{			
			aci.wParam = IDM_DS1+g_GuiMarketSet[i].MarketSet[0];
			aci.lParam = 0;
			aci.bMenu = FALSE;
		}
		else
		{
			aci.wParam = IDR_LONELY_MENU;
			aci.lParam = -6-i;
			aci.bMenu = TRUE;
		}
		g_CustomGridCat.push_back(aci);
		lData++;
	}
}

void LoadCusGridTab(BOOL bIncludeDS)
{
	g_CustomGridCat.clear();
	if(g_DS.DSTabFirst)
	{
		if(bIncludeDS && g_bHasDS)
			LoadDSGridTab();
	}
	BOOL bLocalData = FALSE;
	/*
	CFile m_File;
	CString tempFileName = "celltab.dat";
	if( m_File.Open(g_WSXHStr+tempFileName,CFile::modeRead|CFile::shareDenyNone) )
	{
		long nFileLen = m_File.GetLength();
		if(nFileLen>0&&nFileLen%sizeof(GRIDCATITEM)==0)
		{
			bLocalData = TRUE;
			GRIDCATITEM gci={0};
			while (m_File.Read(&gci, sizeof(GRIDCATITEM))==sizeof(GRIDCATITEM))
				g_CustomGridCat.push_back(gci);
		}
		m_File.Close();
	}
	*/
	if(!bLocalData) GetDefGridCatItems(g_CustomGridCat,FALSE);
	if(!g_DS.DSTabFirst)
	{
		if(bIncludeDS && g_bHasDS)
			LoadDSGridTab();
	}
}

void SaveCusGridTab()
{
	int i;
	CString tempFileName = "celltab.dat";
	CFile m_File;
	if( m_File.Open(g_WSXHStr+tempFileName,CFile::modeCreate|CFile::modeWrite|CFile::shareDenyNone) )
	{
		for(i=0;i<g_CustomGridCat.size();i++)
		{
			if(g_CustomGridCat[i].FlagType!=DSOPT_ITEM)
				m_File.Write(&g_CustomGridCat[i], sizeof(GRIDCATITEM));
		}
	}
	m_File.Close();
}

void RefreshCusGridTab()
{
	int i,j;
	for(i=0;i<g_CustomGridCat.size();i++)
	{
		if(g_CustomGridCat[i].wParam>=ID_BLOCK_MENU_1 && g_CustomGridCat[i].msg<=ID_BLOCK_MENU_400)
		{
			for(j=0;j<g_nBlockNum;j++)
			{
				if(stricmp(g_CustomGridCat[i].lpTitle,g_BlockInfo[j].Name)==0)
					break;
			}			
			if(j<g_nBlockNum)
				g_CustomGridCat[i].wParam=ID_BLOCK_MENU_1+j;
			else
				g_CustomGridCat[i].wParam=ID_BLOCK_MENU_400;
		}
	}
	SaveCusGridTab();
}

void RenameCusGridTab(int nWhich,const char *lpNewTitle)
{
	int i;
	for(i=0;i<g_CustomGridCat.size();i++)
	{
		if(g_CustomGridCat[i].wParam==ID_BLOCK_MENU_1+nWhich)
		{
			strncpy(g_CustomGridCat[i].lpTitle,lpNewTitle,12);
			g_CustomGridCat[i].lpTitle[12]=0;
		}
	}
}

int	TransDynaKData(int nDataNum, AnalyData* pAnalyData)
{
	AnalyData *pTmpAnalyData = new AnalyData[nDataNum];
	memset(pTmpAnalyData, 0, nDataNum*sizeof(AnalyData));
	memcpy(pTmpAnalyData, pAnalyData, nDataNum*sizeof(AnalyData));
	int nTmpDataNum = nDataNum;
	memset(pAnalyData, 0, nDataNum*sizeof(AnalyData));
	
	short zdFlag = 0;		//0-���� 1-�� -1-��
	for(int i=0,j=0;i<nTmpDataNum&&j<nTmpDataNum;i++)
	{
		if(i==0) 
		{
			memcpy(&pAnalyData[0], &pTmpAnalyData[0], sizeof(AnalyData));
			j = 1;
			zdFlag = 0;
		}
		else	if(pTmpAnalyData[i].Close>pTmpAnalyData[i-1].Close)		//����
		{
			if(zdFlag==-1)
				memcpy(&pAnalyData[j++], &pTmpAnalyData[i], sizeof(AnalyData));
			else if(zdFlag==0||zdFlag==1)			//�ǣ��ۼ�
			{
				pAnalyData[j-1].Time = pTmpAnalyData[i].Time;
				pAnalyData[j-1].High = max(pAnalyData[j-1].High, pTmpAnalyData[i].High);
				pAnalyData[j-1].Low = min(pAnalyData[j-1].Low, pTmpAnalyData[i].Low);
				pAnalyData[j-1].Close = pTmpAnalyData[i].Close;
				pAnalyData[j-1].Volume += pTmpAnalyData[i].Volume;
				pAnalyData[j-1].Amount += pTmpAnalyData[i].Amount;
			}
			zdFlag = 1;
		}
		else if(pTmpAnalyData[i].Close<pTmpAnalyData[i-1].Close)		//�µ�
		{
			if(zdFlag==1)
				memcpy(&pAnalyData[j++], &pTmpAnalyData[i], sizeof(AnalyData));
			else if(zdFlag==0||zdFlag==-1)			//�����ۼ�
			{
				pAnalyData[j-1].Time = pTmpAnalyData[i].Time;
				pAnalyData[j-1].High = max(pAnalyData[j-1].High, pTmpAnalyData[i].High);
				pAnalyData[j-1].Low = min(pAnalyData[j-1].Low, pTmpAnalyData[i].Low);
				pAnalyData[j-1].Close = pTmpAnalyData[i].Close;
				pAnalyData[j-1].Volume += pTmpAnalyData[i].Volume;
				pAnalyData[j-1].Amount += pTmpAnalyData[i].Amount;
			}
			zdFlag = -1;
		}
		else															//ƽ�̣��ۼ�
		{
			pAnalyData[j-1].Time = pTmpAnalyData[i].Time;
				pAnalyData[j-1].High = max(pAnalyData[j-1].High, pTmpAnalyData[i].High);
				pAnalyData[j-1].Low = min(pAnalyData[j-1].Low, pTmpAnalyData[i].Low);
				pAnalyData[j-1].Close = pTmpAnalyData[i].Close;
				pAnalyData[j-1].Volume += pTmpAnalyData[i].Volume;
				pAnalyData[j-1].Amount += pTmpAnalyData[i].Amount;
		}
	}
	return j;
}

short	GetQZType(MemStkInfo *pInfo)
{
	if(!pInfo) return 0;
	int nLen = strlen(pInfo->Name);
	if(nLen<3) return 0;
	if(pInfo->Name[nLen-2]=='P' || pInfo->Name[nLen-1]=='P')	//�Ϲ���һ��"P"����������ǰ���һ��Z�ַ�
		return -1;
	return 1;
}

MemStkInfo	*GetQZLinkNow(MemStkInfo *tmpInfo)
{
	if(!tmpInfo) return NULL;
	MemStkInfo *pLinkInfo=NULL;
	int Type=GetStockType(tmpInfo->setcode,tmpInfo->Code);
	if(Type==CODE_SZQZ || Type==CODE_SHQZ)			//Ȩ֤
	{
		STKRELATION  LinkData[100] = {0};
		short nLinkNum = g_pStockIO->GetRLTData(tmpInfo, LinkData, 100);
		for(int i=0;i<nLinkNum;i++)
		{
			pLinkInfo = g_pStockIO->Get_MemStkInfo(LinkData[i].Code, LinkData[i].SetCode);
			if(!pLinkInfo) continue;
			int SAType=GetStockType(pLinkInfo->setcode,pLinkInfo->Code);
			if(SAType==CODE_SHAG||SAType==CODE_SZAG||SAType==CODE_ZXB||SAType==CODE_CYB)
				return pLinkInfo;
		}	
	}
	return NULL;
}

float	GetAbsPrice(MemStkInfo *pStkInfo,DWORD dwPrice,short XSFlag)
{
	if(pStkInfo && pStkInfo->bigmarket==FE_TYPE)
		return(dwPrice/10000.0f);
	if(XSFlag==4) 
		return(dwPrice/10000.0f);
	else if(XSFlag==5)	
		return(dwPrice/100000.0f);
	return(dwPrice/1000.0f);
}

DWORD	GetRelPrice(MemStkInfo *pStkInfo,float fPrice,short XSFlag)
{
	if(pStkInfo && pStkInfo->bigmarket==FE_TYPE)
		return(DWORD(fPrice*10000+0.5));
	if(XSFlag==4) 
		return(DWORD(fPrice*10000+0.5));
	else if(XSFlag==5)	
		return(DWORD(fPrice*100000+0.5));
	return(DWORD(fPrice*1000+0.5));
}

void DynaZoom(CWnd *pWnd,CRect rect1,CRect rect2)
{
	if(!g_bDynaZoom) return;
	CRect LastRect = rect1;
	CDC *pDC = pWnd->GetDC();
	CPen *oldpen = g_d.SelectPen(pDC,LEVEL_PEN);
	int oldrop = pDC->SetROP2(R2_XORPEN);
	int lSpace,rSpace,tSpace,bSpace;
	if(rect1.Width() > rect2.Width())
	{
		lSpace=max(0,(rect2.left-rect1.left)/6);rSpace=max(0,(rect1.right-rect2.right)/6);
		tSpace=max(0,(rect2.top-rect1.top)/6);bSpace=max(0,(rect1.bottom-rect2.bottom)/6);
		for(int i=0;i < 6;i++)
		{
			rect1.DeflateRect(lSpace,tSpace,rSpace,bSpace);
			if(i!=0) g_d.DrawRect(pDC,&LastRect,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
			g_d.DrawRect(pDC,&rect1,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
			LastRect = rect1;
			Sleep(15);
		}
		g_d.DrawRect(pDC,&LastRect,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
	}
	else
	{
		lSpace=max(0,(rect1.left-rect2.left)/6);rSpace=max(0,(rect2.right-rect1.right)/6);
		tSpace=max(0,(rect1.top-rect2.top)/6);bSpace=max(0,(rect2.bottom-rect1.bottom)/6);
		for(int i=0;i < 6;i++)
		{
			rect1.InflateRect(lSpace,tSpace,rSpace,bSpace);
			if(i!=0) g_d.DrawRect(pDC,&LastRect,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
			g_d.DrawRect(pDC,&rect1,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
			LastRect = rect1;
			Sleep(15);
		}
		g_d.DrawRect(pDC,&LastRect,VipColor.LEVELCOLOR^VipColor.BACKCOLOR);
	}
	pDC->SetROP2(oldrop);
	g_d.RestorePen(pDC,oldpen);
	pWnd->ReleaseDC(pDC);
}

BOOL CanProcesLocalData(CWnd *pWnd, int nType)
{
	if(nType==1)				//��������
	{
		BOOL bTmpGen;
		long VipDocLastDate=GetFxMinVipDocLastDate(bTmpGen);
		if( (bTmpGen || (VipDocLastDate<g_lOpenRq && GetForwardDate(VipDocLastDate)<g_lOpenRq)) && !g_bLastRqDown)
		{	
			if(TDX_MessageBox(pWnd->m_hWnd,"����5���������ݲ�ȫ,������[�̺���������]���ܲ���5����������,\n������ܻᵼ�½����׼ȷ,\n�Ծɼ�����?",MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL)
				return FALSE;
		}
		return TRUE;
	}

	BOOL bHint=FALSE;
	long VipDocLastDate=GetVipDocLastDate();
	if( (VipDocLastDate<g_lOpenRq && GetForwardDate(VipDocLastDate)<g_lOpenRq) && !g_bLastRqDown)
	{
		bHint=TRUE;
		if(OnLine && g_bCanDown)
		{
			if(DownDayData_AtOnce(pWnd,"�����������ݿ��ܲ�ȫ,��������Ҫʹ����������������,\n\n�Ƿ�����?(�������ز�ȫ,���ܻᵼ�½����׼ȷ)"))
			{
				if(!g_bLastRqDown) //���ȷ�������˵�û����������,�����½���
					return FALSE;
			}
		}
		else
		{
			if(TDX_MessageBox(pWnd->m_hWnd,"�����������ݲ�ȫ,������[�̺���������]���ܲ�����������,\n������ܻᵼ�½����׼ȷ,\n�Ծɼ�����?",MB_OKCANCEL|MB_ICONEXCLAMATION) == IDCANCEL)
				return FALSE;
		}
	}
	extern long GetLocalVipDocDateNum();
	if(!bHint && GetLocalVipDocDateNum()<100)
		TDX_MessageBox(pWnd->m_hWnd,"������������̫��,���ܻᵼ�½����׼ȷ!",MB_OK|MB_ICONINFORMATION);
	return TRUE;
}

int GetListRefreshTime()
{
	if(!HQControl::IsValidProgram2())
		return 60*1000;
	//��������
	if(g_nNetCondition==0)
		return (HQRefreshTime+4)*1000;
	//��ͳ���
	return (HQRefreshTime+2)*1000;
}

int GetGPRefreshTime()
{
	if(!HQControl::IsValidProgram2())
		return 60*1000;
	//Level2ˢ��
	if(CanLevel2())
		return 800;
	//��������ˢ�±���
	if(g_nNetCondition==0)
		return max(3000,HQRefreshTime*500);
	//��������Level2,�����ͻ�����Level2Ȩ�޵����
	if(g_bSupportLevel2 && (!g_GC.bIsSHLevel2 && !g_GC.bIsSZLevel2))
		return max(2000,HQRefreshTime*500);
	//��������򳬿����
	if(g_bG2DT || g_GC.bShortRefreshHQ)
		return max(1000,HQRefreshTime*200);
	//��ͳ���
	return max(2000,HQRefreshTime*500);
}

void	MarkStockFlag(MemStkInfo *pInfo, char nFlag)
{
	if(!pInfo || pInfo->Index>=DS_FROM) 
		return;
	pInfo->fl_flag = nFlag;
	CString strFile = g_WSXHStr+"mark.dat", strCode;
	strCode.Format("%02d%s", pInfo->setcode, pInfo->Code);
	WritePrivateProfileString("MARK", strCode, IntToStr(nFlag), strFile);
}

void	MarkStockTipFlag(MemStkInfo *pInfo, CString lpString)
{
	if(!pInfo || pInfo->Index>=DS_FROM) 
		return;
	pInfo->fl_flag = MAX_GROUPMARK;					
	CString strFile = g_WSXHStr+"mark.dat", strCode;
	strCode.Format("%02d%s", pInfo->setcode, pInfo->Code);
	WritePrivateProfileString("MARK", strCode, IntToStr(pInfo->fl_flag), strFile);
	lpString.Replace("\r\n", "#$");
	WritePrivateProfileString("TIP", strCode, lpString, strFile);
}

BOOL	GetToolText(CString &strRes, MemStkInfo *pStock,BOOL bGetHead)
{
	if(!pStock||pStock->fl_flag!=MAX_GROUPMARK) return FALSE;
	CString strFile = g_WSXHStr+"mark.dat", strCode;
	CString strBuf;
	strCode.Format("%02d%s", pStock->setcode, pStock->Code);
	GetPrivateProfileString("TIP", strCode, pStock->Name, strBuf.GetBuffer(1000), 1000, strFile);
	strBuf.ReleaseBuffer();
	strBuf.Replace("#$", "\r\n");
	if(bGetHead)
		strRes.Format("%s(%s)%s%s",pStock->Name,pStock->Code,_F("�������(ѡ�й�Ʊ��Ctrl+Q�޸�)\r\n"),strBuf);
	else
		strRes=strBuf;
	return TRUE;
}

COLORREF GetMarkColor(int codeGroup)
{
	switch(codeGroup)
	{
	case 1:return GridColor.Grid_Other_Color;
	case 2:return VipColor.KPCOLOR;
	case 3:return VipColor.ZBCOLOR[2];
	case 4:return GridColor.Grid_Other_Color;
	case 5:return VipColor.KPCOLOR;
	case 6:return VipColor.ZBCOLOR[2];
	}
	return GridColor.Grid_Other_Color;
}

BOOL HasGPInBlockAndContinue(int nBlockNo)
{
	if(nBlockNo<=-3) return FALSE;
	CStdioFile m_File;
	CString strBlock = GetBlockFileName(nBlockNo+2),tempLine;
	CString tempStr = (CString)g_strBlockNew+strBlock+".blk";
	BOOL bHasGP=FALSE;
	if( m_File.Open(HomePath+tempStr,CFile::modeRead|CFile::shareDenyNone) )
	{
		while(m_File.ReadString(tempLine) != FALSE)
		{
			TrimString(tempLine);
			if(!tempLine.IsEmpty())
			{
				bHasGP=TRUE;
				break;
			}
		}
		m_File.Close();
	}
	if(bHasGP)
	{
		if(TDX_MessageBox(NULL,"��������ԭ�����ڵĹ�Ʊ�������,�Ƿ����?",MB_ICONEXCLAMATION|MB_OKCANCEL|MB_DEFBUTTON1)==IDCANCEL)
			return FALSE;
	}
	return TRUE;
}

BOOL CanShowZangsu(struct MemStkInfo *pStkInfo)
{
	if(g_bG2DT) 
		return TRUE;
	if(testzs(pStkInfo))
		return FALSE;
	return TRUE;
}

void GoToHelp()
{
	if(strlen(g_Url.HelpURL)<1) //ֱ��ʹ��ϵͳ����
	{
		CString tmpStr;
		tmpStr = HomePath+"TopView.chm";
		::ShellExecute(NULL,NULL,tmpStr,NULL,NULL,SW_SHOW); 			
	}
	else							//��ҳ����
		::ShellExecute(NULL, _T("open"), (CString)"http://"+g_Url.HelpURL, NULL, NULL, SW_SHOW);	
}

void Modify_CorpWebMenu()
{
#if !defined(OEM_NEWJY)&&!defined(PERSONAL)
	CNewMenu *pTopMenu = (CNewMenu *)AfxGetMainWnd()->GetMenu();
	CNewMenu *pSubMenu = (CNewMenu *)pTopMenu->GetSubMenu(0);
	if(strlen(g_Url.Corp)>0)
//		pSubMenu->ModifyODMenu(g_Url.Corp,ID_CORP_WEB);
		pSubMenu->ModifyMenu(ID_CORP_WEB,MF_BYCOMMAND,ID_CORP_WEB,g_Url.Corp);
#endif
}

BOOL CanUseEmbedWTFunc(HWND m_hWnd)
{
	if(!TC_GetJyStatus_) return FALSE;
	DWORD dwStatus=TC_GetJyStatus_(NULL,NULL,NULL);
	if(!(dwStatus&JY_LOGINED))
	{
		TDX_MessageBox(m_hWnd,"����ϵͳ��δ��¼!����ʹ�ô˹���",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CanLevel2()
{
	return (g_bSupportLevel2 && (g_GC.bIsSHLevel2 || g_GC.bIsSZLevel2));
}

BOOL CanAdvLevel2()
{
	return (CanLevel2() && g_GC.bHasTdxAdv);
}

BOOL CanUseVipHost()
{
	return (g_JyInfo.vipuserflag && g_VipHQHost.HostNum>0);
}

BOOL CanUseLevel2Host()
{
	return (g_GC.bHasLevel2Engine && g_JyInfo.canqslevel2 && g_L2HQHost.HostNum>0);
}

BOOL IsLevel2Property(MemStkInfo *tmpInfo)
{
	if(!tmpInfo)		return FALSE;
	if(!CanLevel2())	return FALSE; 
	return (!testzs(tmpInfo) && ((tmpInfo->setcode==SZ && g_GC.bIsSZLevel2) || (tmpInfo->setcode==SH && g_GC.bIsSHLevel2)));
}

BOOL IsSeatProperty(MemStkInfo *tmpInfo)
{
	if(!tmpInfo)		return FALSE;
	if(!g_GC.bIsSSDS)	return FALSE;	//��ʱ�۹ɲ�֧�־�����λ
	return (tmpInfo->bigmarket==HKGP_TYPE && !testhkzs(tmpInfo));
}

BOOL GetSpecialTextColor(int n,COLORREF &cor)
{
	BOOL bWork=TRUE;
	switch(n)
	{
	case 1:
		cor=RGB(255,0,0);	//��
		break;
	case 2:
		cor=RGB(0,144,0);	//��
		break;
	case 3:
		cor=RGB(0,104,217);	//��
		break;
	case 4:
		cor=RGB(175,175,0);	//��
		break;
	case 5:
		cor=RGB(64,200,200);//��
		break;
	case 6:
		cor=RGB(255,0,164);	//��
		break;
	default:
		bWork=FALSE;	
	}
	return bWork;
} 

void SetFYGroupActive(int nIndex)
{
	int n=g_vfyGroup.size();
	for(int i=0;i<n;i++)
		g_vfyGroup[i].bActive=FALSE;
	if(nIndex>=0 && nIndex<n)
		g_vfyGroup[nIndex].bActive=TRUE;
}

BOOL CheckFYGroupActive(UINT nIndex)
{
	int n=g_vfyGroup.size();
	if(nIndex>=n) return FALSE;
	return g_vfyGroup[nIndex].bActive;
}

BOOL PeekMsg()
{
	MSG msg;
	while(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)) 
	{
		if(!LayoutFlag) 
			return FALSE;
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return TRUE;
}

BOOL IsRemoteGet(LinkedSetInfo *pLSetInfo)
{
	int WhichGPType = pLSetInfo->WhichGpMarket;
	if(pLSetInfo->SortType==0) 
		return FALSE;
	if(pLSetInfo->ColType>=J_GXRQ && pLSetInfo->ColType<=J_GDQYB) 
		return FALSE;
	return !BeBlock(WhichGPType);
}

#include "BlockGpDel.h"
void  Func_DelFromBlock(int tempCurrentNo,BOOL bNotify)
{
	if(tempCurrentNo<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[tempCurrentNo];
	if(!tempStkInfo) return;

	CBlockGpDel dlg;
	dlg.m_CurGPCode=tempStkInfo->Code;
	dlg.m_CurSetCode=tempStkInfo->setcode;
	if(dlg.DoModal() == IDOK && bNotify)
		g_bBlockModified = TRUE;	
}

#include "GetBlockDlg.h"
void  Func_RightAddblock(int tempCurrentNo,BOOL bNotify)
{
	if(tempCurrentNo<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[tempCurrentNo];
	if(!tempStkInfo) return;

	CGetBlockDlg dlg;
	dlg.m_bGetBlock = FALSE;
	dlg.m_CurGPCode=tempStkInfo->Code;
	dlg.m_CurSetCode=tempStkInfo->setcode;
	if(dlg.DoModal() == IDOK && bNotify)
		g_bBlockModified = TRUE;	
}

void  Func_RightAddZxg(int tempCurrentNo,BOOL bNotify)
{
	if(tempCurrentNo<0) return;
	MemStkInfo *tempStkInfo = (*g_pStockIO)[tempCurrentNo];
	if(!tempStkInfo) return;

	if(AddGPToBlock(0,tempStkInfo->Code,tempStkInfo->setcode) && bNotify)
		g_bBlockModified = TRUE;	
}

BOOL SureAutoPage(CWnd *pView)
{
	int nRet=TDX_MessageBox(pView->m_hWnd,"��ȷ���Զ���ҳ��?",MB_ICONEXCLAMATION|MB_OKCANCEL);
	return (nRet==IDOK);
}

BOOL FiltTabKeyCom()
{
	CMainFrame *pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame&&pMainFrame->m_bPadMenuShow) return TRUE;
	return FALSE;
}

CSize DrawMarkFlag(MemStkInfo *pStock,CDC *pDC,int x,int y,int left,int top,CWnd *pWnd)
{
	if(pStock && pStock->fl_flag>0)
	{
		CFont *pOldFont = g_d.SelectFont(pDC, BOLDGRAPH_FONT);
		CSize zs = g_d.DisplayText(pDC, x, y, GetMarkColor(pStock->fl_flag), "%s", g_strGroup[(pStock->fl_flag-1+MAX_GROUPMARK)%MAX_GROUPMARK]);
		g_d.RestoreFont(pDC, pOldFont);
		CRect ToolRect(x-10, y-2, x+pDC->GetTextExtent("T").cx+10, y+pDC->GetTextExtent("T").cy+2);
		ToolRect.OffsetRect(left, top);
		if(g_pToolTipMark&&pStock->fl_flag==MAX_GROUPMARK) 
			g_pToolTipMark->RegTipInfo(ToolRect, pStock, pWnd);
		return zs;
	}
	return CSize(0,0);
}

void Format_PostFix(int postfix_flag,BOOL bPositive,char *buf,int buflen)
{
	if(postfix_flag==1)	strcat(buf,"%%");
	AllTrim(buf);
	if(postfix_flag==2 && bPositive)
	{
		memmove(buf+1,buf,buflen-1);
		buf[0]='+';
	}
}

int isT(char ch)
{
	if(ch==' '||ch=='='||ch=='%'||ch=='.'||ch=='/'||ch&0x80) return 1;
	else return 0;
}

/* Unencode URL���뺯�� */
int UrlEncode(const char *s,char *d)
{
	if(!s||!d) return 0;
	for(;*s!=0;s++)
	{
		unsigned char *p=(unsigned char*)s;
		if(*p==' ')
		{
			*d='+';
			d++;
		}
		else if(isT(*p))
		{
			char a[3];
			*d='%';
			sprintf(a,"%02x",*p);
			*(d+1)=a[0];
			*(d+2)=a[1];
			d+=3;
		}
		else
		{
			*d=*p;
			d++;
		}
	}
	*d=0;
	return 1; 
}


/* Unencode URL���뺯�� */
int UrlDecode(const char *s,char *d)
{
	if(!s||!d) return 0;
	for(;*s!=0;s++)
	{
		if(*s=='+')
		{
			*d=' ';
			d++;
		}
		else if(*s=='%')
		{
			int code;
			if(sscanf(s+1,"%02x",&code)!=1) code='?';
			*d=code;
			s+=2;
			d++;
		}
		else
		{
			*d=*s;
			d++;
		}
	}
	*d=0;
	return 1;
}

void Hlzq_Encrypt(char *szAccount,char *szEnAccount)
{
	for(int i = 0; i < strlen(szAccount); i+=2)
	{
		int k1 = szAccount[i]; //ȡ��һ���ַ���ASCII��k1
		//��k1ת��Ϊ0��65֮�������k2���粻��Լ����Χ��ȡֵ94-93+36=37���ַ���^����
		int k2 = 100;
		if ((48<=k1) && (k1<=57))
			k2=k1-48;
		if ((65<=k1) && (k1<=90))
			k2=k1-65+10;
		if ((93<=k1) && (k1<=122))
			k2=k1-93+36;
		if (k2==100)
			k2=94-93+36;
		int k=k2*66;
		
		//ȡ�ڶ����ַ���ASCII��k1�����ַ�������Ϊ������ȡֵ95���ַ���_����
		if ((i+1)>=strlen(szAccount))
			k1=95;
		else
			k1=szAccount[i+1];
		k2=100;
		if ((48<=k1) && (k1<=57))
			k2=k1-48;
		if ((65<=k1) && (k1<=90))
			k2=k1-65+10;
		if ((93<=k1) && (k1<=122))
			k2=k1-93+36;
		if (k2==100)
			k2=94-93+36;
		k=k+k2;	//��ʱk��0��4355��Χ��

		k=(k+i+1)%4356+1;	//��ʱk���ַ���λ������1��4356��Χ��

		int l=k;
		for (int j=1; j<=8; j++)
			l=(l*l)%4357;
		l=(l*k)%4357;	// ��ʱl=(k^257)%4357 k��257�η���4357������
		
		l=l-1;

		char c1 = int(l/66)+60;
		char c2 = int(l%66)+60;
		szEnAccount[i]=c1;
		szEnAccount[i+1]=c2;
	}
}

void Zxjt_Base64Info(CString & strParam)
{
	CString tmpValiddata;
	tmpValiddata.Format("-F%s",g_JyInfo.zjzh);
	if(!Base64Encode(tmpValiddata,strParam))
		strParam="";
}

#include "URLEncode.h"
void TransferToRealURL(CString & url,struct MemStkInfo *pInfo)
{
	url.Replace("tdx_username",g_strLoginName);
	if(url.Find("tdx_md5password")>=0)
	{
		char md5Password[40];
		MD5_String(g_strPassword,md5Password);
		url.Replace("tdx_md5password",md5Password);
	}
	else
		url.Replace("tdx_password",g_strPassword);
	//
	url.Replace("##homepath##",HomePath);
	url.Replace("##privatepath##",g_WSXHStr);
	//
	if(pInfo)
	{
		url.Replace("xxxxxx",pInfo->Code);
		if(pInfo->setcode==SZ)
			url.Replace("!!","sz");
		else if(pInfo->setcode==SH)
			url.Replace("!!","sh");	
	}
}

void OpenCusWebPage(CWnd *pWnd,CString tmpInfoURL,BOOL bInsideOpenPage)
{
	if(tmpInfoURL.GetLength()<=0) return;

	if(strnicmp(tmpInfoURL,"http://treeid/",14)==0)
	{
		CString tmpGNStr = tmpInfoURL;
		tmpGNStr=tmpGNStr.Mid(14);
		TreeID_CarryOut(tmpGNStr);
		return;
	}
	//������ҵ9999��ID�ģ������ر���
	if(g_OEM.bXyzqEmbedWTVersion && tmpInfoURL.Find("treeid=9999")>0)
	{
		pWnd->PostMessage(UM_OPENWEBWIN,1);
		return;
	}
	//����Ǻ�����Ѷ����,��URL��ֻ��Ҫ����hxinfo
	if(stricmp(tmpInfoURL,"hxinfo")==0)
	{
		g_pMainWin->PostMessage(WM_COMMAND,ID_FUNC_HXINFO);
		return;
	}
	//��������Ž�Ͷ��Ѷ����,��URL��ֻ��Ҫ����zxjtinfo
	else if(stricmp(tmpInfoURL,"zxjtinfo")==0)
	{
		g_pMainWin->PostMessage(WM_COMMAND,ID_FUNC_ZXJTINFO);
		return;
	}
	int tmpIndex=GetSYSCurrentGPIndex();
	MemStkInfo *pInfo = NULL;
	if(tmpIndex>=0)
		pInfo = (*g_pStockIO)[tmpIndex];
	TransferToRealURL(tmpInfoURL,pInfo);
	if(bInsideOpenPage)
	{
		WebURL = tmpInfoURL;
		pWnd->SendMessage(UM_TREEVIEW_MESSAGE,WEB_TREETYPE,-1);
	}
	else
		ShellExecute(NULL, _T("open"), tmpInfoURL, NULL, NULL, SW_SHOW);
}

#include "KeyGuy.h"
BOOL ProcessKeyGuy(MSG *pMsg,CWnd *pWnd,BOOL &KeyGuyFlag)
{
	if(!HQControl::IsValidProgram1()) 
		return FALSE;
	if(pMsg->wParam>=0x81&&pMsg->wParam<=0xFE&&g_KeyGuyCnInfo.nKeyGuyCNing==0)	//���Ŀ�ʼ����
	{	
		memset(&g_KeyGuyCnInfo, 0, sizeof(g_KeyGuyCnInfo));
		g_KeyGuyCnInfo.nKeyGuyCNing = 1;
		g_KeyGuyCnInfo.pOwner = pWnd;
		g_KeyGuyCnInfo.strKeyGuyCN[0]=(char)pMsg->wParam;
		g_KeyGuyCnInfo.strKeyGuyCN[1]='\0';
	}
	else if(g_KeyGuyCnInfo.nKeyGuyCNing==1&&g_KeyGuyCnInfo.pOwner==pWnd)		//������
	{
		::KillTimer(AfxGetMainWnd()->GetSafeHwnd(), 100);
		int nCNLen = strlen(g_KeyGuyCnInfo.strKeyGuyCN);
		g_KeyGuyCnInfo.strKeyGuyCN[nCNLen]=(char)pMsg->wParam;
		g_KeyGuyCnInfo.strKeyGuyCN[nCNLen+1]='\0';
		//���ں�������
		::SetTimer(AfxGetMainWnd()->GetSafeHwnd(), 100, 10, NULL);
	}
	else if((pMsg->wParam>=0x30&&pMsg->wParam<=0x39)||(pMsg->wParam>='a'&&pMsg->wParam<='z')
		||(pMsg->wParam>='A'&&pMsg->wParam<='Z')||pMsg->wParam== '.'||pMsg->wParam== '?')
	{
		CKeyGuy dlg;
		CRect rect;
		pWnd->GetWindowRect(&rect);
		dlg.RightBottomPoint = CPoint(rect.right,rect.bottom);

		dlg.bHasCmd = TRUE;
		KeyGuyFlag = TRUE;

		char tempchar;
		char tempstr[2]={0};

		tempchar   = (char)pMsg->wParam;
		tempstr[0] = tempchar;
		tempstr[1] = '\0';
		dlg.m_edit = (CString)tempstr;

		if( dlg.DoModal() == IDOK && dlg.ReturnType != -1)
			CKeyGuy::OnCommonProcess(dlg.ReturnType ,dlg.ReturnValue);

		KeyGuyFlag = FALSE;

		return TRUE;
	}
	return FALSE;
}

BOOL InStatusBarGP(MemStkInfo *pStkInfo)
{
	if(!pStkInfo) return FALSE;
	for(int j=0;j<3;j++)
	{
		if( pStkInfo->setcode==g_StatusGP[j].setcode && (memcmp(g_StatusGP[j].Code,pStkInfo->Code,SH_CODE_LEN)==0) )
			break;
	}
	return (j<3);
}

//////////////////////////////////////////////////////////////////////////
//��ȡ����ƽ̨��ص�����
BOOL GetFundBaseData()
{
	if(g_pFundBase || g_pFundBase)
		return TRUE;
	TDEL(g_pFundBase);
	TDEL(g_pFundHoldStock);
	g_nFundNum=g_nHoldNum=0;

	int filelen = 0;
	int ntmpNum = 0;
	CString strPath = g_WSXHStr+"LData\\cache\\jjhold.dat";
	FILE *pfile = _fsopen(strPath,"rb",_SH_DENYNO);
	
	CBlowfish fish((BYTE *)"SECURE20071116_TDXSS",20);////����
	
	if (pfile)
	{
		filelen = on_filelength(pfile);
		ntmpNum = filelen/sizeof(struct FUNDHOLDSTOCK);
		g_pFundHoldStock = new FUNDHOLDSTOCK[ntmpNum];
		g_nHoldNum = ntmpNum;
		if (NULL == g_pFundHoldStock)
		{
			fclose(pfile);
			return FALSE;
		}
		memset(g_pFundHoldStock,0,sizeof(FUNDHOLDSTOCK)*ntmpNum);
		fread(g_pFundHoldStock,sizeof(FUNDHOLDSTOCK),ntmpNum,pfile);
		fish.Decrypt((BYTE *)g_pFundHoldStock,filelen/8*8);
		fclose(pfile);
	}
	else 
		return FALSE;

	strPath = g_WSXHStr+"LData\\cache\\jjbase.dat";
	pfile = _fsopen(strPath,"rb",_SH_DENYNO);
	if (pfile)
	{
		filelen = on_filelength(pfile);
		ntmpNum = filelen/sizeof(struct FUNDBASE);
		g_nFundNum = ntmpNum;
		g_pFundBase = new FUNDBASE[ntmpNum];
		if (NULL == g_pFundBase)
		{
			fclose(pfile);
			return FALSE;
		}
		memset(g_pFundBase,0,sizeof(FUNDBASE)*ntmpNum);
		fread(g_pFundBase,sizeof(FUNDBASE),ntmpNum,pfile);
		fish.Decrypt((BYTE *)g_pFundBase,filelen/8*8);
		fclose(pfile);
	}
	else return FALSE;
	return TRUE;
}

CString	FormatLongString(CString strInString, short OutDataFormat)
{
	if(strInString.GetLength()<8) return strInString;
	char	lpString[256]={0}, *argv[4];
	strncpy(lpString, strInString, 255);
	short argc = GetCmdLine(lpString,argv,4,"\r\n,:-/");
	switch(OutDataFormat) 
	{
	case 'G':				//SG_DATE_NORMAL yyyymmdd
		{
			long nDate = atoi((char*)(LPCSTR)strInString);
			short nYY = (nDate/10000)%100;
			short nMM = (nDate/100)%100;
			CString Ret = _T("");
			Ret.Format("%d/%d", nYY, nMM);
			return Ret;
		}
	case 'H':				//SG_DATE_SLASH б�ָܷ�ʱ��yyyy/mm/dd
	case 'I':				//SG_DATE_SHORTLINE �̺��߷ָ�ʱ��yyyy-mm-dd
		{
			if(argc<2) return strInString;
			CString strYYYY = argv[0];
			CString strYY = strYYYY.Right(2);
			CString Ret = _T("");
			Ret.Format("%s/%s", strYY, argv[1]);
			return Ret;
		}
	case 'J':				//SG_DATE_SLASH2 б�ָܷ�ʱ��mm/dd/yyyy
		{
			if(argc<2) return strInString;
			CString strYYYY = argv[2];
			CString strYY = strYYYY.Right(2);
			CString Ret = _T("");
			Ret.Format("%s/%s", strYY, argv[0]);
			return Ret;
		}
		break;
	case 'R':				//SG_DATE_NORMAL_SHORT	YYMMDD
		{
			long nDate = atoi((char*)(LPCSTR)strInString);
			short nYY = nDate/10000;
			short nMM = (nDate/100)%100;
			CString Ret = _T("");
			Ret.Format("%d/%d", nYY, nMM);
			return Ret;
		}
		break;
	case 'S':				//SG_DATE_SLASH_SHORT YY/MM/DD
	case 'T':				//SG_DATE_SHORTLINE_SHORT YY-MM-DD
		{
			if(argc<2) return strInString;
			CString Ret = _T("");
			Ret.Format("%s/%s", argv[0], argv[1]);
			return Ret;
		}
		break;
	case 'U':				//SG_DATE_SLASH2_SHORT MM/DD/YY
		{
			if(argc<2) return strInString;
			CString Ret = _T("");
			Ret.Format("%s/%s", argv[2], argv[0]);
			return Ret;
		}
		break;
	default:
		break;
	}
	return strInString;
}

//////////////////////////////////////////////////////////////////////////
BOOL	GetHYCodeName(char *strAppFlag, char *Name, char *Code)
{
	char strApp[100]={0};
	sprintf(strApp, "codename%s", strAppFlag);
	GetPrivateProfileString(strApp, Code, "", Name, 255, g_LayerPath);
	return Name[0]!=0;
}

BOOL	IsIdleStatus()
{
	return !Is_Jy_Time(18) && (g_nNetCondition==0);
}

void	GetResPadBit(CView *pView, HICON &bitmap)
{
	if(pView->IsKindOf(RUNTIME_CLASS(CBigMenuView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_BIGMENU));
	if(pView->IsKindOf(RUNTIME_CLASS(CHomePageView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_HOMEPAGE));
	if(pView->IsKindOf(RUNTIME_CLASS(CGridView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_GRID));
	if(pView->IsKindOf(RUNTIME_CLASS(CNewsView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_NEWS));
	if(pView->IsKindOf(RUNTIME_CLASS(CTxtView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_TXT));
	if(pView->IsKindOf(RUNTIME_CLASS(CComView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_COM));
	if(pView->IsKindOf(RUNTIME_CLASS(CComView2)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_COM2));
	if(pView->IsKindOf(RUNTIME_CLASS(CIEBrowserView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_WEB));
	if(pView->IsKindOf(RUNTIME_CLASS(CJbfxView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_JBFX));
	if(pView->IsKindOf(RUNTIME_CLASS(CAdvHqView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_ADVHQ));
	if(pView->IsKindOf(RUNTIME_CLASS(CJJView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_JJ));
	if(pView->IsKindOf(RUNTIME_CLASS(CTvView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_TV));
	if(pView->IsKindOf(RUNTIME_CLASS(CRssView)))
		bitmap = ::LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_TYPE_RSS));
}

BOOL RectPtIn(RECT rc,CPoint pt)
{
	return (pt.x>rc.left && pt.x<rc.right && pt.y>rc.top && pt.y<rc.bottom);
}

void SetRectZero(RECT & rc)
{
	rc.left=rc.top=rc.right=rc.bottom=0;	
}

void SetMenuBreak(int i,CMenu *pMenu)
{
	MENUITEMINFO info;
	TCHAR tsz[80];
	info.cbSize = sizeof(info);
	info.fMask = MIIM_TYPE;
	info.dwTypeData = tsz;
	info.cch = sizeof(tsz) / sizeof(tsz[0]);
	GetMenuItemInfo(pMenu->m_hMenu,i,MF_BYPOSITION,&info);
	info.fType |= MFT_MENUBARBREAK;
	SetMenuItemInfo(pMenu->m_hMenu,i,MF_BYPOSITION,&info);
}

void SwitchMenuCallBack(CWnd *pWnd,CWnd *ProcWnd,int left,int top,int right,int bottom,UINT message,UINT menuid,long whichmenu)
{
	AFX_MANAGE_STATE(AfxGetAppModuleState()); 

	CPoint point = CPoint(left,bottom);
	if(whichmenu==0)
	{
		if(g_SectorCN.size()<1)
		{
			g_SectorCN.clear();
			char	IniPath[MAX_PATH];
			wsprintf(IniPath,"%sLData\\cache\\hkblock.dat",g_WSXHStr);
			FILE *	fp=_fsopen(IniPath,"rt",SH_DENYNO);
			if(fp) 
			{
				char	lpString[256];
				for(rewind(fp);!feof(fp);)
				{
					memset(lpString,0,256);
					fgets(lpString,255,fp);
					FL_CodeName flcn={0};
					if(lpString[0]=='#')
					{
						strncpy(flcn.SectorCode, &lpString[1],20);
						strncpy(flcn.SectorName, &lpString[1],20);
						//����۹ɰ�鲻�ܵ���������(��Ϊ��\r��\n)
						int nLen=strlen(flcn.SectorName);
						if( nLen>0 && (flcn.SectorName[nLen-1]=='\r' || flcn.SectorName[nLen-1]=='\n') )
							flcn.SectorName[nLen-1]=0;

						g_SectorCN.push_back(flcn);
					}
				}
				fclose(fp);
			}
		}
		CNewMenu HKMenu;
		HKMenu.CreatePopupMenu();
		UINT StartID = ID_MDFHK_HY_1;
		for(int i=0;i<g_SectorCN.size();i++)
		{
			HKMenu.AppendMenu( MF_STRING,StartID+i,_F(g_SectorCN[i].SectorName));
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&HKMenu);
		}
		if(i > 0)
		{
			pWnd->ClientToScreen(&point);
			HKMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	//����ĸ�����
	else if(whichmenu==-1)
	{
		CNewMenu ConceptMenu;
		ConceptMenu.CreatePopupMenu();
		UINT StartID = ID_CONCEPT_MENU_1;
		for(int i = 0;i < g_ConceptBlockNum;i++)
		{
			ConceptMenu.AppendMenu( MF_STRING,StartID+i,_F(g_ConceptBlock[i].strBlockName));
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&ConceptMenu);
		}
		if(i > 0)
		{
			pWnd->ClientToScreen(&point);
			ConceptMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	//�������ϰ��
	/*
	else if(whichmenu==-2)
	{
		CNewMenu ZHBMenu;
		ZHBMenu.CreatePopupMenu();
		UINT StartID = ID_ZHB_MENU_1;
		for(int i = 0;i < g_ZHBBlockNum;i++)
		{
			ZHBMenu.AppendMenu( MF_STRING,StartID+i,_F(g_ZHBBlock[i].zhmc));
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&ZHBMenu);
		}
		if(i > 0)
		{
			pWnd->ClientToScreen(&point);
			ZHBMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	*/
	//����ѡ��
	else if(whichmenu==-2)
	{
		CNewMenu BlockMenu;
		BlockMenu.CreatePopupMenu();
		UINT StartID = ID_BLOCK_MENU_1;
		for(int i = 0;i < g_nBlockNum;i++)
		{
			if(g_BlockInfo[i].KeyGuyCode[0]!='@') continue;
			BlockMenu.AppendMenu( MF_STRING,StartID+i,g_BlockInfo[i].Name);
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&BlockMenu);
		}
		if(i > 0)
		{
			pWnd->ClientToScreen(&point);
			BlockMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	//������û����
	else if(whichmenu==-3)
	{
		CNewMenu BlockMenu;
		BlockMenu.CreatePopupMenu();
		UINT StartID = ID_BLOCK_MENU_1;
		for(int i = 0;i < g_nBlockNum;i++)
		{
			BlockMenu.AppendMenu( MF_STRING,StartID+i,g_BlockInfo[i].Name);
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&BlockMenu);
		}
		if(i > 0)
		{
			pWnd->ClientToScreen(&point);
			BlockMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	//���ܵ�ϵͳ���
	else if(whichmenu==-4)
	{
		int	i;
		CNewMenu HZMenu;
		HZMenu.CreatePopupMenu();

		CNewMenu menu,*pPopup;
		menu.LoadMenu (IDR_LONELY_MENU);
		pPopup=(CNewMenu *)menu.GetSubMenu(16);
		HZMenu.AppendMenu(MF_POPUP,(UINT)pPopup->m_hMenu,_F("�������"));
		pPopup=(CNewMenu *)menu.GetSubMenu(17);
		HZMenu.AppendMenu(MF_POPUP,(UINT)pPopup->m_hMenu,_F("��ҵ���"));

		CNewMenu ZJHHYMenu;
		ZJHHYMenu.CreatePopupMenu();
		for(i = 0;i < g_pConstMng->GetZJHHYBlockNum();i++)
		{
			char *strHYName = g_pConstMng->GetHYFromCommandIDXH(i);
			ZJHHYMenu.AppendMenu( MF_STRING,ID_ZJHHY_MENU_1+i,_F(strHYName));
			if( i != 0 && i % 25 == 0 )
				SetMenuBreak(i,&ZJHHYMenu);
		}
		if(i>0)
			HZMenu.AppendMenu(MF_POPUP,(UINT)ZJHHYMenu.m_hMenu,_F("֤�����ҵ���"));

		CNewMenu ConceptMenu;
		ConceptMenu.CreatePopupMenu();
		for(i = 0;i < g_ConceptBlockNum;i++)
		{
			ConceptMenu.AppendMenu( MF_STRING,ID_CONCEPT_MENU_1+i,_F(g_ConceptBlock[i].strBlockName));
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&ConceptMenu);
		}
		HZMenu.AppendMenu(MF_POPUP,(UINT)ConceptMenu.m_hMenu,_F("������"));

		CNewMenu ZHBMenu;
		ZHBMenu.CreatePopupMenu();
		for(i = 0;i < g_ZHBBlockNum;i++)
		{
			ZHBMenu.AppendMenu( MF_STRING,ID_ZHB_MENU_1+i,_F(g_ZHBBlock[i].zhmc));
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&ZHBMenu);
		}
		if(i>0)
			HZMenu.AppendMenu(MF_POPUP,(UINT)ZHBMenu.m_hMenu,_F("��ϰ��"));

		pWnd->ClientToScreen(&point);
		HZMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
	}
	//�����֤�����ҵ���
	else if(whichmenu==-5)
	{
		CNewMenu ZJHHYMenu;
		ZJHHYMenu.CreatePopupMenu();
		for(int i = 0;i < g_pConstMng->GetZJHHYBlockNum();i++)
		{
			char *strHYName = g_pConstMng->GetHYFromCommandIDXH(i);
			ZJHHYMenu.AppendMenu( MF_STRING,ID_ZJHHY_MENU_1+i,_F(strHYName));
			if( i != 0 && i % 25 == 0 )
				SetMenuBreak(i,&ZJHHYMenu);
		}
		if(i>0)
		{
			pWnd->ClientToScreen(&point);
			ZJHHYMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	//��չ�������
	else if(whichmenu<=-6&&whichmenu>-100)
	{
		BYTE  GUIMarket = -6-whichmenu;
		CNewMenu DSMenu;
		DSMenu.CreatePopupMenu();
		UINT StartID = IDM_DS1;
		for(int i = 0,j=0;i < MAX_MKT_NUM;i++)
		{
			int nDSMarketID = g_GuiMarketSet[GUIMarket].MarketSet[i];
			if(nDSMarketID==0) break;
			DSMenu.AppendMenu( MF_STRING,StartID+nDSMarketID,_F(g_GuiMarketSet[GUIMarket].MarketName[i]));
			++j;
		}
		pWnd->ClientToScreen(&point);
		DSMenu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
	}
	//���������·�switch�˵���
	else if(whichmenu<=-100 && whichmenu>-200)
	{
		int nWhichType = -FY_START-whichmenu;
		if(nWhichType<0||nWhichType>=g_vfyGroup.size()) 
			return;
		int nItemNum = g_vfyGroup[nWhichType].strItemName.size();
		CNewMenu FYMenu;
		FYMenu.CreatePopupMenu();
		UINT StartID = ID_FYFLAG_0+nWhichType*MAX_FYNUM_IN_GROUP;
		for(int i=0;i<nItemNum;i++)
		{
			FYMenu.AppendMenu( MF_STRING,StartID+i,_F(g_vfyGroup[nWhichType].strItemName[i]));
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&FYMenu);
		}
		if(i > 0)
		{
			pWnd->ClientToScreen(&point);
			FYMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	//���������Ϸ�switch�˵���
	else if(whichmenu<=-200)
	{
		int nWhichType = -200-whichmenu;
		if(nWhichType<0||nWhichType>=g_vfyGroup.size()) 
			return;
		int nSubGroupNum = g_vfyGroup[nWhichType].subGroup.size();
		CNewMenu FYMenu;
		FYMenu.CreatePopupMenu();
		for(int i=0;i<nSubGroupNum;i++)
		{
			int ns=g_vfyGroup[nWhichType].subGroup[i];
			FYMenu.AppendMenu( MF_STRING,ns+ID_RIGHT_FYGROUP,_F(g_vfyGroup[ns].strGroupName));
			if(CheckFYGroupActive(ns))
				FYMenu.CheckMenuItem(ns+ID_RIGHT_FYGROUP,MF_BYCOMMAND|MF_CHECKED);
			if( i != 0 && i % 18 == 0 )
				SetMenuBreak(i,&FYMenu);
		}
		if(i > 0)
		{
			point = CPoint(left,bottom);
			pWnd->ClientToScreen(&point);
			FYMenu.TrackPopupMenu (TPM_LEFTALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
		}
	}
	else
	{
		CNewMenu menu;
		menu.LoadMenu (menuid);
		CNewMenu *pPopup=(CNewMenu *)menu.GetSubMenu(whichmenu);
		pWnd->ClientToScreen(&point);
		if(pPopup->m_hMenu)
			pPopup->TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RIGHTBUTTON,point.x,point.y,ProcWnd);
	}
}

UINT CalcPeriodToTdxPeriod(UINT nCalcPeriod)
{
	switch(nCalcPeriod)
	{
	case MIN1_ST:	return	PER_MIN1;
	case MIN5_ST:	return	PER_MIN5;
	case MIN15_ST:	return	PER_MIN15;
	case MIN30_ST:	return	PER_MIN30;
	case HOUR_ST:	return	PER_HOUR;
	case DAY_ST:	return	PER_DAY;
	case WEEK_ST:	return	PER_WEEK;
	case MONTH_ST:	return	PER_MONTH;
	case MINN_ST:	return	PER_MINN;
	case DAYN_ST:	return	PER_DAYN;
	case SEASON_ST:	return	PER_SEASON;
	case YEAR_ST:	return	PER_YEAR;
	}
	return PER_DAY;
}

UINT TdxPeriodToCalcPeriod(UINT nTdxPeriod)
{
	switch(nTdxPeriod)
	{
	case PER_MIN1:	return	MIN1_ST;
	case PER_MIN5:	return	MIN5_ST;
	case PER_MIN15:	return	MIN15_ST;
	case PER_MIN30:	return	MIN30_ST;
	case PER_HOUR:	return	HOUR_ST;
	case PER_DAY:	return	DAY_ST;
	case PER_WEEK:	return	WEEK_ST;
	case PER_MONTH:	return	MONTH_ST;
	case PER_MINN:	return	MINN_ST;
	case PER_DAYN:	return	DAYN_ST;
	case PER_SEASON:return	SEASON_ST;
	case PER_YEAR:	return	YEAR_ST;
	}
	return DAY_ST;
}

UINT KeyOrderToTdxPeriod(UINT nOrder)
{
	switch(nOrder)
	{
	case 0:	return	PER_MIN1;
	case 1:	return	PER_MIN5;
	case 2:	return	PER_MIN15;
	case 3:	return	PER_MIN30;
	case 4:	return	PER_HOUR;
	case 5: return	PER_MINN;

	case 6:	return	PER_DAY;
	case 7:	return	PER_WEEK;
	case 8:	return	PER_MONTH;
	case 9:	return	PER_DAYN;
	case 10:return	PER_SEASON;
	case 11:return	PER_YEAR;
	}
	return PER_DAY;
}

UINT TdxPeriodToKeyOrder(UINT nTdxPeriod)
{
	switch(nTdxPeriod)
	{
	case PER_MIN1:	return	0;
	case PER_MIN5:	return	1;
	case PER_MIN15:	return	2;
	case PER_MIN30:	return	3;
	case PER_HOUR:	return	4;
	case PER_MINN:	return	5;

	case PER_DAY:	return	6;
	case PER_WEEK:	return	7;
	case PER_MONTH:	return	8;
	case PER_DAYN:	return	9;
	case PER_SEASON:return	10;
	case PER_YEAR:	return	11;
	}
	return 6;
}

char *GetTdxPeriodStr(UINT nTdxPeriod,char *InvalidStr)
{
	if(nTdxPeriod<PER_MIN5 || nTdxPeriod>PER_YEAR)
		return InvalidStr;
	int nCalcPeriod = TdxPeriodToCalcPeriod(nTdxPeriod);
	return g_CalcPeriodStr[nCalcPeriod];
}

BOOL IsValidPeriod(UINT nTdxPeriod)
{
	if(nTdxPeriod<PER_MIN5 || nTdxPeriod>PER_YEAR)
		return FALSE;
	return TRUE;
}

BOOL BeUserBlock(int WhichGPType,int nBlockFlag)	//�ж��Ƿ����û����
{
	return WhichGPType==TYPE_ZXG || WhichGPType==TYPE_TJG || (WhichGPType==-1 && nBlockFlag!=-1 && nBlockFlag<DY_START);
}

BOOL BeBlock(int WhichGPType)		//�ж��Ƿ��ǰ��
{
	return (WhichGPType>=TYPE_ZXG && WhichGPType<=TYPE_LC) || (WhichGPType==-1) || (WhichGPType==TYPE_DEFTJ_POOL);
}

void DrawNoABHintInfo(CDC *pDC,CRect rc)
{
	CFont *oldfont = g_d.SelectFont(pDC,BIGHQ2_FONT);
	g_d.DisplayText(pDC,rc,VipColor.DOWNCOLOR, DT_VCENTER|DT_CENTER|DT_SINGLELINE, _F("��ʱֻ�ṩ��۹�Ʊ���۷���"));
	g_d.RestoreFont(pDC,oldfont);
}

BOOL CheckPointPos(CWnd *pDlg,int nItemID,CPoint point)
{
	CWnd *pWnd = pDlg->GetDlgItem(nItemID);
	CRect rect;
	if(pWnd)
	{
		pWnd->GetWindowRect(rect);
		pDlg->ScreenToClient(&rect);
		return rect.PtInRect(point);
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////////
//��ѹȫ�ֺ���
#include "unzipFile.h"
#define UNZIPBUF_SIZE 4096
BOOL UnZipSomeFile(const char *filename,const char *path,BOOL bBak)
{
	int dirtoken_pos,dirtoken_from;
	CString tmpFileName;
	CString tmpPath=path;
	if(tmpPath.Right(1) != "\\")
		tmpPath += "\\";	
	//�ȿ���û�д�Zip�ļ�
	CFile theFile;
	if(!theFile.Open(filename,CFile::modeRead|CFile::shareDenyNone))
		return FALSE;
	theFile.Close();

	CTime nowtime = CTime::GetCurrentTime();
	CString bakdir,tmpStr;
	bakdir.Format("%02d%02d%02d%02d_old",nowtime.GetYear()%100,nowtime.GetMonth(),nowtime.GetDay(),nowtime.GetHour());
	if(bBak)
	{
		DeleteDirInfo(HomePath+bakdir);
		CreateDirectory(HomePath+bakdir,NULL);
	}
	try
	{
		CUnzipFile uf(filename);
		uf.GoToFirstFile();
		do
		{
			unz_file_info ui;
			uf.GetCurrentFileInfo(&ui);
			int iNameSize = ui.size_filename + 1;
			char* pName = new char [iNameSize+1];
			// �õ�Zip���е��ļ���
			uf.GetCurrentFileInfo(NULL, pName, iNameSize);
			pName[iNameSize]=0;
			if(bBak)
			{
				tmpStr.Format("%s%s\\%s",HomePath,bakdir,pName);
				CopyFile((CString)path + pName,tmpStr,FALSE);
			}
			//
			dirtoken_pos=-1;
			do
			{
				tmpFileName=pName;
				dirtoken_from=dirtoken_pos+1;
				dirtoken_pos=tmpFileName.Find("\\",dirtoken_from);
				if(dirtoken_pos<0)
					dirtoken_pos=tmpFileName.Find("/",dirtoken_from);
				if(dirtoken_pos>=0)
				{
					//�������Ŀ¼,�����ȴ�����Ŀ¼
					tmpFileName=tmpFileName.Mid(0,dirtoken_pos);
					CreateDirectory(tmpPath+tmpFileName,NULL);
				}
			}while(dirtoken_pos>=0);
			CFile f;
			if(!f.Open((CString)tmpPath + pName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone))
			{
				//����ȥ��ֻ������
				CFileStatus fs;
				CFile::GetStatus((CString)path + pName, fs);
				if(fs.m_attribute&0x01)
				{
					fs.m_attribute ^= 0x01;
					CFile::SetStatus((CString)path + pName, fs);
				}
				if(!f.Open((CString)path + pName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone))
				{					
					delete[] pName;
					continue;
				}
			}
			delete[] pName;
			uf.OpenCurrentFile();
			char buf[UNZIPBUF_SIZE];
			int size_read;
			do
			{
				size_read = uf.ReadCurrentFile(buf, UNZIPBUF_SIZE);
				if (size_read > 0)
					f.Write(buf, size_read);
			}while (size_read == UNZIPBUF_SIZE);

			uf.UpdateFileStatus(f, ui);
		}while(uf.GoToNextFile());
		uf.Close();
	}
	catch (CException* e)
	{
		e->Delete();
		return FALSE;
	}
	return TRUE;
}

int GetExternWhichUnit(int nWhichUnit,int nInCom2)
{
	//���ư���ͳ��������,�����ظ�
	int nWhich = nWhichUnit+1;
	if(nInCom2==1)
		nWhich = nWhichUnit+101;
	return nWhich;
}
//////////////////////////////////////////////////////////////////////////
typedef   DWORD   (WINAPI   *PSLWA)(HWND,   DWORD,   BYTE,   DWORD); 

static   PSLWA   pSetLayeredWindowAttributes   =   NULL; 
BOOL   MakeWindowTransparent(HWND hWnd,unsigned char factor) 
{ 
	if(!pSetLayeredWindowAttributes)   
	{ 
		HMODULE hDLL = LoadLibrary("user32"); 	
		pSetLayeredWindowAttributes = (PSLWA)GetProcAddress(hDLL, "SetLayeredWindowAttributes"); 
	} 
	if(!pSetLayeredWindowAttributes)   
		return FALSE; 
	
	SetLastError(0); 
	
	SetWindowLong(hWnd, GWL_EXSTYLE, GetWindowLong(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED); 
	
	if(GetLastError()) 
		return FALSE; 
	
	return pSetLayeredWindowAttributes(hWnd, RGB(0,0,255), factor, LWA_COLORKEY|LWA_ALPHA); 
}
//////////////////////////////////////////////////////////////////////////
int GetInBlockInfo(int nCurGP,INBLOCKINFO *pBlockInf,BOOL bHasGLGP)
{
	int nBlockNum = 0;
	int i,j,gpnum;
	short indexbuff[MAXGPNUM];
	if(bHasGLGP)
	{		
		//���ָ��,����300ָ��,����Ʒ��
		if(IsLCIdx((*g_pStockIO)[nCurGP]))
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,_F("�ɷݹ�"));
			pBlockInf[nBlockNum].gpnum = 0;
			pBlockInf[nBlockNum].nBlockNo = -11;
			nBlockNum++;
		}
		else
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,_F("�������"));
			pBlockInf[nBlockNum].gpnum = 0;
			pBlockInf[nBlockNum].nBlockNo = -10;
			nBlockNum++;
		}
	}
	for(i = 0;i < TOTAL_DY_NUM;i++)
	{
		gpnum = FiltDYHYCode(0,i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==nCurGP) break;
		if(j < gpnum)
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,_F(g_cwAddrTopic[i]));
			pBlockInf[nBlockNum].gpnum = gpnum;
			pBlockInf[nBlockNum].nBlockNo = i+DY_START;
			nBlockNum++;
		}
	}
	for(i = 0;i < TOTAL_HY_NUM;i++)
	{
		gpnum = FiltDYHYCode(1,i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==nCurGP) break;
		if(j < gpnum)
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,_F(g_cwHYstr[i]));
			pBlockInf[nBlockNum].gpnum = gpnum;
			pBlockInf[nBlockNum].nBlockNo = i+HY_START;
			nBlockNum++;
		}
	}
	//��֤�����ҵ�в���
	ZJHHY_Cfg *	pzjhhy = NULL;
	MemStkInfo *tmpInfo = (*g_pStockIO)[nCurGP];
	if(tmpInfo) pzjhhy = g_pConstMng->GetHyFromHYID(tmpInfo->J_zjhhy);
	if(pzjhhy)
	{
		strcpy(pBlockInf[nBlockNum].strBlockName,_F(pzjhhy->HYName));
		pBlockInf[nBlockNum].nBlockNo = g_pConstMng->GetHyXHFromHYID(tmpInfo->J_zjhhy)+ZJHHY_START;		
		pBlockInf[nBlockNum].gpnum = 0;
		MemStkInfo *tmpInfo1;
		for (i=0;i<g_pStockIO->GetStockNum(ALL_ALL);i++)
		{
			tmpInfo1=(*g_pStockIO)[i];
			if(tmpInfo1 && g_pConstMng->GetHyXHFromHYID(tmpInfo1->J_zjhhy)==pBlockInf[nBlockNum].nBlockNo-ZJHHY_START)
				pBlockInf[nBlockNum].gpnum++;
		}
		nBlockNum++;
	}
	for(i = 0;i < g_ConceptBlockNum;i++)
	{
		gpnum = FiltConceptCode(i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==nCurGP) break;
		if(j < gpnum)
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,_F(g_ConceptBlock[i].strBlockName));
			pBlockInf[nBlockNum].gpnum = gpnum;
			pBlockInf[nBlockNum].nBlockNo = i+CON_START;
			nBlockNum++;
		}
	}
	for(i = 0;i < g_ZHBBlockNum;i++)
	{
		gpnum = FiltZHCode(i,indexbuff);
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==nCurGP) break;
		if(j < gpnum)
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,_F(g_ZHBBlock[i].zhmc));
			pBlockInf[nBlockNum].gpnum = gpnum;
			pBlockInf[nBlockNum].nBlockNo = i+ZHB_START;
			nBlockNum++;
		}
	}
	for(i = 0;i < g_nBlockNum;i++)
	{
		gpnum = FiltBlockCode(i+2,indexbuff);//i+2������ѡ�ɺ�������
		for(j=0;j<gpnum;j++)
			if(indexbuff[j]==nCurGP) break;
		if(j < gpnum)
		{
			strcpy(pBlockInf[nBlockNum].strBlockName,g_BlockInfo[i].Name);
			pBlockInf[nBlockNum].gpnum = gpnum;
			pBlockInf[nBlockNum].nBlockNo = i;
			nBlockNum++;
		}
	}
	return nBlockNum;
}
//////////////////////////////////////////////////////////////////////////
int GetGlGpIndex(int nCurGP,short *indexbuffer,int nMaxNum)
{
	MemStkInfo *pInfo = (*g_pStockIO)[nCurGP];
	if(!pInfo) return 0;
	int nCnt = 0,iNo = -1,iLinkNo = -1;
	if(IsLCExt(pInfo) || IsLCPt(pInfo))
		;
	else if(testzs(pInfo))
	{					
		iNo = g_pStockIO->GetIndex(SH_DPCODE,SH);
		if(iNo>=0)
			indexbuffer[nCnt++] = iNo;
		iNo = g_pStockIO->GetIndex(SZ_DPCODE,SZ);
		if(iNo>=0)
			indexbuffer[nCnt++] = iNo;
		iNo = g_pStockIO->GetIndex(ZXB_DPCODE,SZ);
		if(iNo>=0)
			indexbuffer[nCnt++] = iNo;
		iNo = g_pStockIO->GetIndex(CYB_DPCODE,SZ);
		if(iNo>=0)
			indexbuffer[nCnt++] = iNo;
		iNo = g_pStockIO->GetIndex(HS300_SZZS,SZ);
		if(iNo>=0)
			indexbuffer[nCnt++] = iNo;		
	}
	else
	{
		if(pInfo->J_zjhhy>0)
		{
			ZJHHY_Cfg *pZcfg = g_pConstMng->GetHyFromHYID(pInfo->J_zjhhy);
			if(!pZcfg) return nCnt;
			int nBlockNo = pZcfg->HYXH;
			char *Code = g_pLcIdxMng->GetIdxStkCode(nBlockNo);
			MemStkInfo *pIdxInfo = g_pStockIO->Get_MemStkInfo(Code, SZ);
			if(!pIdxInfo) return nCnt;
			iNo = g_pStockIO->GetIndex(pIdxInfo->Code,pIdxInfo->setcode);
			indexbuffer[nCnt++] = iNo;		
		}
	}
	return nCnt;	
}

int GetLcIdxGP(const char *strCode,short *indexbuffer,int nMaxNum)
{
	int nCnt = 0,LcNum = 0,iGPNo = -1;
	if(g_pLcIdxMng) LcNum = g_pLcIdxMng->GetIdxNum();
	LPLCIDXSET pLcIdxSet = NULL;
	if(g_pLcIdxMng) g_pLcIdxMng->GetIdxSet(pLcIdxSet);
	for(int i=0;i<LcNum;i++)
	{
		if(strcmp(pLcIdxSet[i].Code,strCode)==0)
		{
			LPLCIDXSTK pTmpLcStk = NULL;
			long nLcStkNum = 0,nLcStkSpc = 0;
			g_pLcIdxMng->GetIdxStk(pTmpLcStk,nLcStkNum,nLcStkSpc,i);
			for(int j=0;j<min(nMaxNum,nLcStkNum);j++)
			{
				iGPNo=g_pStockIO->GetIndex(pTmpLcStk[j].Code,pTmpLcStk[j].nSetCode);
				if(iGPNo>=0)
				{
					indexbuffer[nCnt]=iGPNo;
					nCnt++;
				}
			}
			TDEL(pTmpLcStk);	
			break;
		}
	}
	return nCnt;
}

void ReadDataTickStat(MemStkInfo *pTarStock, TICKSTAT *pTickStat)
{
	return;
	
	if(!g_pHQDBFile)
	{	
		g_pHQDBFile = new CDBFile;
		if(!g_pHQDBFile->Open(g_WSXHStr+"tick_stat.dbf"))
			return;
	}

	memset(pTickStat,0,sizeof(TICKSTAT));
	short nType = GetStockType(pTarStock->setcode, pTarStock->Code);
	if(nType!=CODE_SZAG&&nType!=CODE_SHAG&&nType!=CODE_ZXB&&nType!=CODE_CYB)
		return;

	short nIDIndex = g_pStockIO->GetIndex(pTarStock->Code, pTarStock->setcode);
	int allNum = g_pHQDBFile->GetRecordCount();
	long nDBX = -1;
	map<long, long, greater<long> >::iterator iter = g_DBIndexMap.find(nIDIndex);
	if(iter!=g_DBIndexMap.end())
		nDBX = (*iter).second;
	else 
	{
		int nRecXh = 1;
		g_pHQDBFile->Goto(nRecXh);
		char strContent[255]={0};
		while(nRecXh<=allNum)
		{
			g_pHQDBFile->ReadString("SETCODE", strContent, 2);
			int nSetCode = atoi(strContent);
			g_pHQDBFile->ReadString("CODE", strContent, 9);
			MemStkInfo *pStock = g_pStockIO->Get_MemStkInfo(strContent, nSetCode);
			if(pStock)
			{
				map<long, long, greater<long> >::iterator iter = g_DBIndexMap.find(nIDIndex);
				if(iter==g_DBIndexMap.end())
				{
					short nIndex = g_pStockIO->GetIndex(pStock->Code,pStock->setcode);
					g_DBIndexMap[nIndex] = nRecXh;
				}
				if(pStock==pTarStock)
				{
					nDBX = nRecXh;
					break;
				}
			}
			nRecXh++;
			if(!g_pHQDBFile->Skip())
				break;
		}
	}

	if(nDBX>=0&&nDBX<=allNum&&g_pHQDBFile->Goto(nDBX))
	{
		char strContent[255]={0};

		g_pHQDBFile->ReadString("MINUTE", strContent, 10);
		pTickStat->Minute = atoi(strContent);
		if(pTickStat->Minute==0)
			pTickStat->Minute = -1;
		g_pHQDBFile->ReadString("TFS1", strContent, 10);
		pTickStat->fVol[0][0] = atof(strContent);
		g_pHQDBFile->ReadString("TFS2", strContent, 10);
		pTickStat->fVol[0][1] = atof(strContent);
		g_pHQDBFile->ReadString("TFS3", strContent, 10);
		pTickStat->fVol[0][2] = atof(strContent);
		g_pHQDBFile->ReadString("TFS4", strContent, 10);
		pTickStat->fVol[0][3] = atof(strContent);
		g_pHQDBFile->ReadString("TFS5", strContent, 10);
		pTickStat->fVol[0][4] = atof(strContent);
		
		g_pHQDBFile->ReadString("TFB1", strContent, 10);
		pTickStat->fVol[1][0] = atof(strContent);
		g_pHQDBFile->ReadString("TFB2", strContent, 10);
		pTickStat->fVol[1][1] = atof(strContent);
		g_pHQDBFile->ReadString("TFB3", strContent, 10);
		pTickStat->fVol[1][2] = atof(strContent);
		g_pHQDBFile->ReadString("TFB4", strContent, 10);
		pTickStat->fVol[1][3] = atof(strContent);
		g_pHQDBFile->ReadString("TFB5", strContent, 10);
		pTickStat->fVol[1][4] = atof(strContent);
		
		g_pHQDBFile->ReadString("TFM1", strContent, 10);
		pTickStat->fVol[2][0] = atof(strContent);
		g_pHQDBFile->ReadString("TFM2", strContent, 10);
		pTickStat->fVol[2][1] = atof(strContent);
		g_pHQDBFile->ReadString("TFM3", strContent, 10);
		pTickStat->fVol[2][2] = atof(strContent);
		g_pHQDBFile->ReadString("TFM4", strContent, 10);
		pTickStat->fVol[2][3] = atof(strContent);
		g_pHQDBFile->ReadString("TFM5", strContent, 10);
		pTickStat->fVol[2][4] = atof(strContent);
		
		//
		g_pHQDBFile->ReadString("TNS1", strContent, 10);
		pTickStat->VolNum[0][0] = atol(strContent);
		g_pHQDBFile->ReadString("TNS2", strContent, 10);
		pTickStat->VolNum[0][1] = atol(strContent);
		g_pHQDBFile->ReadString("TNS3", strContent, 10);
		pTickStat->VolNum[0][2] = atol(strContent);
		g_pHQDBFile->ReadString("TNS4", strContent, 10);
		pTickStat->VolNum[0][3] = atol(strContent);
		g_pHQDBFile->ReadString("TNS5", strContent, 10);
		pTickStat->VolNum[0][4] = atol(strContent);
		
		g_pHQDBFile->ReadString("TNB1", strContent, 10);
		pTickStat->VolNum[1][0] = atol(strContent);
		g_pHQDBFile->ReadString("TNB2", strContent, 10);
		pTickStat->VolNum[1][1] = atol(strContent);
		g_pHQDBFile->ReadString("TNB3", strContent, 10);
		pTickStat->VolNum[1][2] = atol(strContent);
		g_pHQDBFile->ReadString("TNB4", strContent, 10);
		pTickStat->VolNum[1][3] = atol(strContent);
		g_pHQDBFile->ReadString("TNB5", strContent, 10);
		pTickStat->VolNum[1][4] = atol(strContent);
		
		g_pHQDBFile->ReadString("TNM1", strContent, 10);
		pTickStat->VolNum[2][0] = atol(strContent);
		g_pHQDBFile->ReadString("TNM2", strContent, 10);
		pTickStat->VolNum[2][1] = atol(strContent);
		g_pHQDBFile->ReadString("TNM3", strContent, 10);
		pTickStat->VolNum[2][2] = atol(strContent);
		g_pHQDBFile->ReadString("TNM4", strContent, 10);
		pTickStat->VolNum[2][3] = atol(strContent);
		g_pHQDBFile->ReadString("TNM5", strContent, 10);
		pTickStat->VolNum[2][4] = atol(strContent);

		memcpy(&pTarStock->its, pTickStat, sizeof(TICKSTAT));
	}
}

void ReadDataTickStat_All()
{
	g_bTickStatReaded = TRUE;

	if(!g_pHQDBFile)
	{		
		g_pHQDBFile = new CDBFile;
		if(!g_pHQDBFile->Open(g_WSXHStr+"tick_stat.dbf"))
		{
			TDEL(g_pHQDBFile);
			return;
		}
	}
	if(!g_pHQDBFile) return;
	int allNum = g_pHQDBFile->GetRecordCount();
	if(allNum<1||!g_pHQDBFile->Goto(1))
	{
		g_pHQDBFile->Close();
		TDEL(g_pHQDBFile);
		return;
	}
	char strContent[255]={0};
	while(OnLine&&g_pStockIO)
	{
		g_pHQDBFile->ReadString("SETCODE", strContent, 2);
		int nSetCode = atoi(strContent);
		g_pHQDBFile->ReadString("CODE", strContent, 9);
		MemStkInfo *pStock = g_pStockIO->Get_MemStkInfo(strContent, nSetCode);
		if(!pStock) 
		{
			if(!g_pHQDBFile->Skip())
			{
				g_pHQDBFile->Close();
				TDEL(g_pHQDBFile);
				return;
			}
			continue;
		}
		short nType = GetStockType(pStock->setcode, pStock->Code);
		if(nType!=CODE_SZAG&&nType!=CODE_SHAG&&nType!=CODE_ZXB&&nType!=CODE_CYB)
		{
			if(!g_pHQDBFile->Skip())
			{
				g_pHQDBFile->Close();
				TDEL(g_pHQDBFile);
				return;
			}
			continue;
		}
		
		TICKSTAT *pTickStat = &pStock->its;
		g_pHQDBFile->ReadString("MINUTE", strContent, 10);
		pTickStat->Minute = atoi(strContent);
		if(pTickStat->Minute==0)
			pTickStat->Minute = -1;

		int nFieldNo = 4;
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[0][0] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[0][1] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[0][2] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[0][3] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[0][4] = atof(strContent);
		
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[1][0] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[1][1] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[1][2] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[1][3] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[1][4] = atof(strContent);
		
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[2][0] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[2][1] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[2][2] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[2][3] = atof(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->fVol[2][4] = atof(strContent);
		
		//
		/*
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[0][0] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[0][1] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[0][2] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[0][3] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[0][4] = atol(strContent);
		
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[1][0] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[1][1] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[1][2] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[1][3] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[1][4] = atol(strContent);
		
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[2][0] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[2][1] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[2][2] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[2][3] = atol(strContent);
		g_pHQDBFile->ReadString(nFieldNo++, strContent, 10);
		pTickStat->VolNum[2][4] = atol(strContent);
		*/

		PeekMsg();
		//
		CURRDATA iData={0};
		g_pStockIO->ReadCurrStockData(pStock, &iData);
		float fAver = iData.Now;
		if(iData.Volume>0) fAver = iData.Amount/iData.Volume/100;
		if(fAver<COMPPREC) fAver = iData.Close;

		pTickStat->fAmount[0][0] = 100.0f*pTickStat->fVol[0][0]*fAver;
		pTickStat->fAmount[0][1] = 100.0f*pTickStat->fVol[0][1]*fAver;
		pTickStat->fAmount[0][2] = 100.0f*pTickStat->fVol[0][2]*fAver;
		pTickStat->fAmount[0][3] = 100.0f*pTickStat->fVol[0][3]*fAver;
		pTickStat->fAmount[0][4] = 100.0f*pTickStat->fVol[0][4]*fAver;

		pTickStat->fAmount[1][0] = 100.0f*pTickStat->fVol[1][0]*fAver;
		pTickStat->fAmount[1][1] = 100.0f*pTickStat->fVol[1][1]*fAver;
		pTickStat->fAmount[1][2] = 100.0f*pTickStat->fVol[1][2]*fAver;
		pTickStat->fAmount[1][3] = 100.0f*pTickStat->fVol[1][3]*fAver;
		pTickStat->fAmount[1][4] = 100.0f*pTickStat->fVol[1][4]*fAver;

		pTickStat->fAmount[2][0] = 100.0f*pTickStat->fVol[2][0]*fAver;
		pTickStat->fAmount[2][1] = 100.0f*pTickStat->fVol[2][1]*fAver;
		pTickStat->fAmount[2][2] = 100.0f*pTickStat->fVol[2][2]*fAver;
		pTickStat->fAmount[2][3] = 100.0f*pTickStat->fVol[2][3]*fAver;
		pTickStat->fAmount[2][4] = 100.0f*pTickStat->fVol[2][4]*fAver;
	
		/*
		//����
		if(iData.Now>COMPPREC&&iData.Open>COMPPREC&&iData.Close>COMPPREC)
		{
			float zf1 = iData.Now/iData.Close;
			float zf2 = fAver/iData.Open;
			float yz1 = 1.0f+10.0*(zf1-1);
			float yz2 = 1.0f+10.0*(zf2-1);
			if(iData.Amount>COMPPREC&&yz1>COMPPREC&&yz2>COMPPREC)
			{
				float fBB = iData.Amount*pow(pTickStat->fAmount[1][0]/iData.Amount, 1.0f/(yz1*yz2));
				float fMB = iData.Amount*pow(pTickStat->fAmount[2][0]/iData.Amount, 1.0f/yz1);
				float fBS = iData.Amount*pow(pTickStat->fAmount[1][1]/iData.Amount, 1.0f/(yz1*yz2));
				float fMS = iData.Amount*pow(pTickStat->fAmount[2][1]/iData.Amount, 1.0f/yz1);
				if(fBB+fMB+fBS+fMS+pTickStat->fAmount[0][4]<iData.Amount)
				{
					pTickStat->fAmount[1][0] = fBB;
					pTickStat->fAmount[2][0] = fMB;
					pTickStat->fAmount[1][1] = fBS;
					pTickStat->fAmount[2][1] = fMS;
				}
			}
		}
		*/

		if(!g_pHQDBFile->Skip())
		{
			g_pHQDBFile->Close();
			TDEL(g_pHQDBFile);
			return;
		}
	}
}

DWORD tdx_filelength(FILE * ftmp)
{
	DWORD oldpos,filelen;
	oldpos = ftell(ftmp);
   	fseek(ftmp,0L,SEEK_END);
	filelen = ftell(ftmp);
	fseek(ftmp,oldpos,SEEK_SET);
	return filelen;
}

int GetMessageNoFromZB(const char *Value,BOOL bSpecialZB)
{
	int MessageNo = 0,nTotalZb = g_pCalc->_CallDTopViewCalc_7(ZB_TYPE);
	TINDEXINFO *tmpIndexInfo = NULL;
	if(!bSpecialZB)
	{
		for(int i = 0;i < nTotalZb;i++)
		{
			tmpIndexInfo = g_pCalc->_CallDTopViewCalc_8(ZB_TYPE,i);
			if(!tmpIndexInfo || tmpIndexInfo->nDelStatus&HIDE_INDEX) 
				continue;
			if(strcmp(tmpIndexInfo->acCode,Value) == 0)
			{
				MessageNo = i + KEYGUY_ZB;
				break;
			}
		}
		if(i >= nTotalZb)
			return MessageNo;
	}
	else	//����ָ��
	{
		for(int i = 0;i < g_nSpeclIndexNum;i++)
		{
			tmpIndexInfo = g_SpeclIndex+i;
			if(tmpIndexInfo->nDelStatus&HIDE_INDEX) 
				continue;
			if(strcmp(tmpIndexInfo->acCode,Value) == 0)
			{
				MessageNo = i + KEYGUY_SEPCZB;
				break;
			}
		}
		if(i >= g_nSpeclIndexNum)
			return MessageNo;
	}
	return MessageNo;
}

void	CalcDXIndex1(MemStkInfo *pStock)
{
	//������Ҫ����ָ��
	if(!InitCalcInterface(g_pExtCalc)) return;
	time_t ct = time(NULL);
	if(ct-pStock->CalcRes1.CalcTmie<60*5) return;
	InterlockedExchange(&pStock->CalcRes1.CalcTmie, time(NULL));

	float *fOUT[MAX_LINEOUT] = {0};
	pStock->CalcRes1.nDataNum = g_pExtCalc->_CallDTopViewCalc_5(g_strCalc1,pStock->setcode,pStock->Code,fOUT);
	if(pStock->CalcRes1.nDataNum>0)
	{
		for(int k=0;k<MAX_LINEOUT;k++)
		{
			if(fOUT[k]&&TBE_MEANLESS(fOUT[k][pStock->CalcRes1.nDataNum-1]))
				pStock->CalcRes1.fOut[k] = fOUT[k][pStock->CalcRes1.nDataNum-1];
			g_pExtCalc->_CallDTopViewCalc_14(fOUT[k]);
		}
	}
	memset(pStock->CalcRes1.nCalcOKNum, 0, sizeof(pStock->CalcRes1.nCalcOKNum));
	if(pStock->CalcRes1.nDataNum>0)
	{
		for(int k=0;k<MAX_LINEOUT;k++)
		{
			if(pStock->CalcRes1.fOut[k]>0.5f)
				pStock->CalcRes1.nCalcOKNum[k]++;
		}
	}
}

void	CalcDXIndex2(MemStkInfo *pStock)
{
	//������Ҫ����ָ��
	if(!InitCalcInterface(g_pExtCalc)) return;
	time_t ct = time(NULL);
	if(ct-pStock->CalcRes2.CalcTmie<60*5) return;
	InterlockedExchange(&pStock->CalcRes2.CalcTmie, time(NULL));
	
	float *fOUT[MAX_LINEOUT] = {0};
	pStock->CalcRes2.nDataNum = g_pExtCalc->_CallDTopViewCalc_5(g_strCalc2,pStock->setcode,pStock->Code,fOUT);
	if(pStock->CalcRes2.nDataNum>0)
	{
		for(int k=0;k<MAX_LINEOUT;k++)
		{
			if(fOUT[k]&&TBE_MEANLESS(fOUT[k][pStock->CalcRes2.nDataNum-1]))
				pStock->CalcRes2.fOut[k] = fOUT[k][pStock->CalcRes2.nDataNum-1];
			g_pExtCalc->_CallDTopViewCalc_14(fOUT[k]);
		}
	}
	memset(pStock->CalcRes2.nCalcOKNum, 0, sizeof(pStock->CalcRes2.nCalcOKNum));
	if(pStock->CalcRes2.nDataNum>0)
	{
		for(int k=0;k<MAX_LINEOUT;k++)
		{
			if(pStock->CalcRes2.fOut[k]>0.5f)
				pStock->CalcRes2.nCalcOKNum[k]++;
		}
	}
}

void	CalcDXIndex3(MemStkInfo *pStock)
{
	//������Ҫ����ָ��
	if(!InitCalcInterface(g_pExtCalc)) return;
	time_t ct = time(NULL);
	if(ct-pStock->CalcRes3.CalcTmie<60*5) return;
	InterlockedExchange(&pStock->CalcRes3.CalcTmie, time(NULL));
	
	float *fOUT[MAX_LINEOUT] = {0};
	pStock->CalcRes3.nDataNum = g_pExtCalc->_CallDTopViewCalc_5(g_strCalc3,pStock->setcode,pStock->Code,fOUT);
	if(pStock->CalcRes3.nDataNum>0)
	{
		for(int k=0;k<MAX_LINEOUT;k++)
		{
			if(fOUT[k]&&TBE_MEANLESS(fOUT[k][pStock->CalcRes3.nDataNum-1]))
				pStock->CalcRes3.fOut[k] = fOUT[k][pStock->CalcRes3.nDataNum-1];
			g_pExtCalc->_CallDTopViewCalc_14(fOUT[k]);
		}
	}
	memset(pStock->CalcRes3.nCalcOKNum, 0, sizeof(pStock->CalcRes3.nCalcOKNum));
	if(pStock->CalcRes3.nDataNum>0)
	{
		for(int k=0;k<MAX_LINEOUT;k++)
		{
			if(pStock->CalcRes3.fOut[k]>0.5f)
				pStock->CalcRes3.nCalcOKNum[k]++;
		}
	}
}

float	Amp(float fInput, float fFlag)
{
	if(fInput<0.0f||fInput>1.0f||fFlag<1.0f) return fInput;
	return fInput*fFlag-(1.0f-fInput)*(fFlag-1.0f);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int WebContentDownload(char *strUrl, char *cFileName, char *Header_Ex)
{	
	CInternetSession *m_pInetSession = NULL;
	if(!m_pInetSession)
	{
		//�����û���֤��Session
		m_pInetSession = new CInternetSession(AfxGetAppName(),1,PRE_CONFIG_INTERNET_ACCESS);
		m_pInetSession->EnableStatusCallback();
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,6*1000);
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
	}
	if(!m_pInetSession) return 0;
	CHttpFile* File=NULL;
	CHttpConnection* pConnection=NULL;
	short 	bReadOK = 0;
	try
	{
		CString szHeaders = _T("Accept: */*\r\nUser-Agent: Mozilla/4.0\r\n");
		CString strTempURL(strUrl);
		CString ServerName;
		CString Object;
		INTERNET_PORT Port;
		DWORD ServiceType;
		AfxParseURL( strTempURL, ServiceType, ServerName, Object, Port );
		pConnection = m_pInetSession->GetHttpConnection( ServerName,Port );
		if(!pConnection) 
		{
			m_pInetSession->Close();
			delete m_pInetSession;
			return 0;
		}

		File = pConnection->OpenRequest( CHttpConnection::HTTP_VERB_GET, Object,
		NULL, 1, NULL, NULL,
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_SECURE |
		INTERNET_FLAG_NO_CACHE_WRITE);
		if(!File) 
		{
			pConnection->Close();
			delete pConnection;
			m_pInetSession->Close();
			delete m_pInetSession;

			return 0;
		}
		BOOL result = File->AddRequestHeaders(szHeaders);
		if(result&&Header_Ex)
			result = File->AddRequestHeaders(Header_Ex);
		if(result)
			result = File->SendRequest();
		if(result)
		{
			DWORD dwRet;
			File->QueryInfoStatusCode(dwRet);
			if(dwRet==HTTP_STATUS_OK)
			{
				CFile contentFile;
				if(contentFile.Open(cFileName, CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::shareExclusive))
				{
					char Buffer[512]={0};
					int numread = 0;
					do 
					{
						numread = File->Read(Buffer,500);
						if(numread>0) contentFile.Write(Buffer, numread);
					} while(numread>0);

					contentFile.Close();
					bReadOK = 1;
				}	
				if(File)
				{
					File->Close();
					delete File;
					File = NULL;
				}
				if(pConnection)
				{
					pConnection->Close();
					delete pConnection;
					pConnection = NULL;
					m_pInetSession->Close();
					delete m_pInetSession;
				}
			}
		}
		if(File)
		{
			File->Close();
			delete File;
			File = NULL;
		}
		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
			pConnection = NULL;
			m_pInetSession->Close();
			delete m_pInetSession;
		}
	}
	catch(...)
	{
		if(File)
		{
			File->Close();
			delete File;
			File = NULL;
		}
		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
			pConnection = NULL;
			m_pInetSession->Close();
			delete m_pInetSession;
		}
		return bReadOK;
	}
	return bReadOK;
}

int WebPostDownload(char *strUrl, char *strContent, char *strCookie, char *cFileName, char *Header_Ex)
{	
	CInternetSession *m_pInetSession = NULL;
	if(!m_pInetSession)
	{
		//�����û���֤��Session
		m_pInetSession = new CInternetSession(AfxGetAppName(),1,PRE_CONFIG_INTERNET_ACCESS);
		m_pInetSession->EnableStatusCallback();
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,6*1000);
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
		m_pInetSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
	}
	if(!m_pInetSession) return 0;

	CHttpFile* File=NULL;
	char *strHeader = NULL;
	CHttpConnection* pConnection=NULL;
	short 	bReadOK = 0;
	try
	{
		CString strTempURL(strUrl);
		CString ServerName;
		CString Object;
		INTERNET_PORT Port;
		DWORD ServiceType;
		AfxParseURL( strTempURL, ServiceType, ServerName, Object, Port );
		pConnection = m_pInetSession->GetHttpConnection( ServerName,Port );
		if(!pConnection) return 0;

		File = pConnection->OpenRequest( CHttpConnection::HTTP_VERB_POST, Object,
		NULL, 1, NULL, NULL,
		INTERNET_FLAG_RELOAD |
		INTERNET_FLAG_NO_CACHE_WRITE |
		INTERNET_FLAG_NO_COOKIES | 
		INTERNET_FLAG_SECURE);
		if(!File) 
		{
			pConnection->Close();
			delete pConnection;
			return 0;
		}

		int nTolLen = strlen(strContent)+strlen(strCookie)+1024;
		strHeader = new char[nTolLen];
		sprintf(strHeader, "Accept: application/json, text/javascript, */*; q=0.01\r\nContent-Type: application/x-www-form-urlencoded; charset=UTF-8\r\nContent-Length: %d\r\nPragma: no-cache\r\n", strlen(strContent));
		if(strCookie)
			strcat(strHeader, strCookie);	

		BOOL result = File->AddRequestHeaders(strHeader);
		if(result&&Header_Ex)
			result = File->AddRequestHeaders(Header_Ex);
		if(result)
		{
			result = File->SendRequestEx(strlen(strContent)); 
			if(result)
			{
				File->WriteString((CString)strContent);
				result = File->EndRequest();
				
				if(result)
				{
					//read
					DWORD dwRet;
					File->QueryInfoStatusCode(dwRet);
					if(dwRet==HTTP_STATUS_OK)
					{
						CFile contentFile;
						if(contentFile.Open(cFileName, CStdioFile::modeCreate|CStdioFile::modeWrite|CStdioFile::shareExclusive))
						{
							char Buffer[1024+10]={0};
							int numread = 0;
							do 
							{
								numread = File->Read(Buffer,1024);
								if(numread>0) contentFile.Write(Buffer, numread);
							} while(numread>0);
							
							contentFile.Close();
							bReadOK = 1;
						}	
						if(File)
						{
							File->Close();
							delete File;
							File = NULL;
						}
						if(pConnection)
						{
							pConnection->Close();
							delete pConnection;
							pConnection = NULL;
						}
					}
				}
			}
		}

		if(File)
		{
			delete []strHeader;
			File->Close();
			delete File;
			File = NULL;
		}
		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
			pConnection = NULL;
		}
	}
	catch(...)
	{
		if(File)
		{
			delete []strHeader;
			File->Close();
			delete File;
			File = NULL;
		}
		if(pConnection)
		{
			pConnection->Close();
			delete pConnection;
			pConnection = NULL;
		}
		return bReadOK;
	}
	return bReadOK;
}

//nFlag=-1,��ȡǰ��Ĵ�
//=0����ȡ����
//=1����ȡ�����
long	GetCmdLine(LPSTR lpString, vector<char*> &argv, long nMaxLine, char *sepstr, short nFlag)
{
	int nSepLen = strlen(sepstr), nStringLen = strlen(lpString);
	char *pHead = &lpString[0]; 
	if(memcmp(pHead, sepstr, nSepLen)) 
	{
		if(nFlag!=1)
			argv.push_back(pHead);
	}
	while (TRUE)
	{
		char *pFound = strstr(pHead, sepstr);
		if(!pFound) 
		{
			if(nFlag==-1)
				argv.pop_back();
			break;
		}
		int j=0;
		while (j<nSepLen)
			pFound[j++] = '\0';
		if(&pFound[j]-lpString<nStringLen) 
		{
			pHead = &pFound[j];
			argv.push_back(pHead);
		}
		else 
			break;
		if(nMaxLine>0&&argv.size()>nMaxLine) break;
	}
	//��ֹsepsepsep,�м�û�����ݵ����
	long nFinalRet = 0, nRet = argv.size();
	for(int i=0;i<nRet;i++)
	{
		if(strlen(argv[i])>0)
			argv[nFinalRet++] = argv[i];
		else argv[i] = 0;
	}
	while (argv.size()>nFinalRet)
	{
		argv.pop_back();
	}
	return nFinalRet;
}

long	GetCmdLine(LPSTR lpString, vector<char*> &argv, long nMaxLine, char *strHead, char *strTail)
{
	vector<char*> fstTail;
	GetCmdLine(lpString, fstTail, nMaxLine, strTail,-1);
	for(int i=0;i<fstTail.size();i++)
	{
		vector<char*> sub;
		int nSub = GetCmdLine(fstTail[i], sub, 0, strHead,1);
		if(nSub>0) argv.push_back(sub[nSub-1]);
	}
	return argv.size();
}

char *MonthString[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
#include "json/json.h"
#include <fstream>
int GetHistoryData_au(vector<AnalyData> &HisData, char *Code)
{
	char path[MAX_PATH]={0};
	sprintf(path, "%s%s.txt", g_WSXHStr, Code);
	
	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open (path);    
    if (reader.parse(is, root, FALSE))  
    {   
		Json::Value::Members mm = root.getMemberNames();
		long nMaxId = 0;
		for(int k=0;k<mm.size();k++)
		{
			long id = atol(mm[k].c_str());
			if(id>nMaxId)
				nMaxId = id;
		}
		
        for(int id=0;id<=nMaxId;id++)
        {
			char strId[10]={0};
			sprintf(strId, "%d", id);
			if(root.isMember(strId))
			{
				Json::Value vv = root[strId];

				AnalyData iData={0};
				char *strDateTime = (char*)vv["Date"].asCString();
				char *argv[7]={0};
				int argc = GetCmdLine(strDateTime, argv, 10, ",: \r\n");
				if(argv[0]&&argv[1]&&argv[2])
				{
					int nMonth=-1, nDay=-1, nYear=-1, nHour=0, nMin=0;
					for(int k=0;k<12;k++)
					{
						if(strcmp(argv[0], MonthString[k])==0)
							nMonth = k+1;
					}
					nDay = atoi(argv[1]);
					nYear = atoi(argv[2]);
					if(argv[3]) 
						nHour = atoi(argv[3]);
					if(argv[4]) 
						nMin = atoi(argv[4]);
					
					if(nMonth>=0&&nDay>=0&&nYear>=0&&nHour>=0&&nMin>=0)
					{
						if(nHour>0||nMin>0)
						{
							iData.Time.Daye.Year = nYear-2004;
							iData.Time.Daye.Day = 100*nMonth+nDay;
							iData.Time.Daye.Minute = 60*nHour+nMin;
						}
						else
							iData.Time.Date = 10000*nYear+100*nMonth+nDay;
						
						iData.High = atof(vv["High"].asCString());
						iData.Low = atof(vv["Low"].asCString());
						iData.Open = atof(vv["Open"].asCString());
						iData.Close = atof(vv["Close"].asCString());
						iData.Volume = atof(vv["Volume"].asCString());
						HisData.push_back(iData);
					}
					else 
						AfxMessageBox(vv["Date"].asCString());
				}
			}	
        }  
    }  
    is.close();
	return HisData.size();
}

int GetHistoryData_bitc(vector<AnalyData> &HisData, double &dMultyply, char *strFile)
{
	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open (strFile);    
    if (reader.parse(is, root, FALSE)) 
    {   
        int data_size = root.size();
        for(int i = 0; i < data_size; ++i)
        {
			if(i==0&&dMultyply==0.0)
			{
				double dPrice = root[i]["close"].asDouble();
				if(dPrice<0.0001) 
					dMultyply = 10000000.0;
				else if(dPrice<0.001) 
					dMultyply = 1000000.0;
				else if(dPrice<0.01) 
					dMultyply = 100000.0;
				else if(dPrice<0.1) 
					dMultyply = 10000.0;
				else if(dPrice<1.0)
					dMultyply = 1000.0;
				else if(dPrice<10.0)
					dMultyply = 100.0;
				else if(dPrice<100.0)
					dMultyply = 10.0;
				else 
					dMultyply = 1.0;
			}
			
			AnalyData iData={0};
			time_t tt = root[i]["date"].asUInt();
			tm *ttt = gmtime((const time_t*)&tt);
			iData.Time.Daye.Year = ttt->tm_year+1900-2004;
			iData.Time.Daye.Day = 100*(ttt->tm_mon+1)+ttt->tm_mday;
			iData.Time.Daye.Minute = ttt->tm_hour*60+ttt->tm_min;
			
			iData.High = root[i]["high"].asDouble()*dMultyply;
			iData.Low = root[i]["low"].asDouble()*dMultyply;
			iData.Open = root[i]["open"].asDouble()*dMultyply;
			iData.Close = root[i]["close"].asDouble()*dMultyply;
			iData.Volume = root[i]["quoteVolume"].asDouble();
			iData.Amount = root[i]["weightedAverage"].asDouble()*dMultyply*iData.Volume;
			HisData.push_back(iData);
        }  
    }  
    is.close();
	return 0;
}

int GetHistoryData_binance(vector<AnalyData> &HisData, double &dMultyply, time_t &ttLast, char *strFile)
{
	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open (strFile);    
    if (reader.parse(is, root, FALSE)) 
    {   
        int data_size = root.size();
        for(int i = 0; i < data_size; ++i)
        {
			Json::Value KLNode = root[i];
			Json::Value::iterator iter = KLNode.begin();
			AnalyData iData={0};
			for(int k=0;iter!=KLNode.end();iter++,k++)
			{
				Json::Value KLItem = (*iter);
				switch(k)
				{
				case 0:
					{
						ttLast = KLItem.asDouble()/1000.0;
						tm *ttt = localtime((const time_t*)&ttLast);
						iData.Time.Daye.Year = ttt->tm_year+1900-2004;
						iData.Time.Daye.Day = 100*(ttt->tm_mon+1)+ttt->tm_mday;
						iData.Time.Daye.Minute = ttt->tm_hour*60+ttt->tm_min;
					}
					break;
				case 1:
					{
						if(i==0&&dMultyply==0.0)
						{
							double dPrice = atof(KLItem.asCString());
							if(dPrice<0.0001) 
								dMultyply = 10000000.0;
							else if(dPrice<0.001) 
								dMultyply = 1000000.0;
							else if(dPrice<0.01) 
								dMultyply = 100000.0;
							else if(dPrice<0.1) 
								dMultyply = 10000.0;
							else if(dPrice<1.0)
								dMultyply = 1000.0;
							else if(dPrice<10.0)
								dMultyply = 100.0;
							else if(dPrice<100.0)
								dMultyply = 10.0;
							else 
								dMultyply = 1.0;
						}
						iData.Open = atof(KLItem.asCString())*dMultyply;
					}
					break;
				case 2:
					iData.High = atof(KLItem.asCString())*dMultyply;
					break;
				case 3:
					iData.Low = atof(KLItem.asCString())*dMultyply;
					break;
				case 4:
					iData.Close = atof(KLItem.asCString())*dMultyply;
					break;
				case 5:
					iData.Volume = 1000.0f*atof(KLItem.asCString());
					break;
				case 7:
					iData.Amount = atof(KLItem.asCString())*dMultyply;
					break;
				default:
					break;
				}
			}
			HisData.push_back(iData);
        }  
    }  
    is.close();
	return 0;
}

extern void	GetNewOneAnaly(AnalyData &NewData, AnalyData *pEnrollData, int nUnit);
void	CreateNewAnalyData(AnalyData *pAnalyData, long nDataNum, vector<AnalyData> &vNewData, int nUnit, int nOffset)
{
	vNewData.clear();
	for(int i=nOffset;i<nDataNum;i+=nUnit)
	{
		AnalyData iData={0};
		GetNewOneAnaly(iData, &pAnalyData[i], min(nUnit,nDataNum-i));
		vNewData.push_back(iData);
	}
}

void	GetDataItems_Bit(char *currencyPair, int nPerSeed, vector<AnalyData> &OutCome)
{
	const int nPerItem = 1;
	/*
	char strFile[100]={0};
	sprintf(strFile, "%s_Test.txt", currencyPair);
	char url[MAX_PATH]={0};
	time_t tt = time(NULL);
	long startt = tt-tt%(60*60*24)-60*60*24*2;
	sprintf(url, "https://poloniex.com/public?command=returnChartData&currencyPair=%s&start=%d&end=9999999999&period=%d",currencyPair,startt,nPerSeed);
	WebContentDownload(url, strFile);
	*/
	vector<AnalyData> AllHisData_Raw, AllHisData;
	double dMultyply=1.0;
//	GetHistoryData_bitc(AllHisData_Raw, dMultyply, "USDT_ETH_Cur.txt");
	time_t tt;
	char *intString = "";
	if(nPerSeed==1)		//1m
	{
		intString = "1";
	}
	else if(nPerSeed==2)
	{
		intString = "2";
	}
	else if(nPerSeed==4)
	{
		intString = "4";
	}
	else if(nPerSeed==8)
	{
		intString = "8";
	}
	else if(nPerSeed==16)
	{
		intString = "16";
	}
	char strCurFile[100]={0};
	sprintf(strCurFile, "%s_Binance_%s_cur.txt", currencyPair, intString);
	GetHistoryData_binance(AllHisData_Raw, dMultyply, tt, strCurFile);
	CreateNewAnalyData(&AllHisData_Raw[0], AllHisData_Raw.size(), OutCome, nPerItem, 0);
}

void	GetDataItems_Au(char *GPCode, char *DataPeriod, char *DataInterval, char *DataNum, int nPerItem, vector<AnalyData> &OutCome)
{
	//ȡ��cookies
	char	m_CookieData[4*1024]={0};
	CStdioFile CookieFile;
	if(CookieFile.Open("comm_cookie.txt", CStdioFile::modeRead|CStdioFile::shareDenyNone)) 
	{
		int nFileLen = CookieFile.GetLength();
		nFileLen = min(nFileLen, 1024);
		int nRead = CookieFile.Read(m_CookieData, nFileLen);
		CookieFile.Close();
	}
	else 
		return;

	char strFile[100]={0};
	sprintf(strFile, "%s.txt", GPCode);
	
	char *posturl = "https://research.commsec.com.au/API/Charts/Charting_Load?env=staging";
	CString content = "inputData=%7B%22IssueId%22%3A%22---%22%2C%22Key%22%3A%22%3A0000002lanesqcba%22%2C%22DataPeriod%22%3A%22___%22%2C%22DataInterval%22%3A%22^^^%22%2C%22DrawingWidth%22%3A1588%2C%22NumberOfDays%22%3A%22~~~%22%2C%22Panels%22%3A%5B%7B%22Type%22%3A%22Candlestick%22%2C%22Uid%22%3A9%2C%22Parameters%22%3A%7B%22Comparisons%22%3A%5B%5D%7D%2C%22ExportData%22%3Atrue%2C%22Height%22%3A330%2C%22Overlays%22%3A%5B%7B%22Type%22%3A%22HighLow%22%2C%22Uid%22%3A10%2C%22Parameters%22%3A%7B%7D%7D%2C%7B%22Type%22%3A%22SMA%22%2C%22Uid%22%3A11%2C%22Parameters%22%3A%7B%22Period%22%3A%22360%22%7D%7D%5D%7D%2C%7B%22Type%22%3A%22Volume%22%2C%22Uid%22%3A12%2C%22Parameters%22%3A%7B%22Comparisons%22%3A%5B%5D%7D%2C%22ExportData%22%3Atrue%2C%22Height%22%3A100%2C%22Overlays%22%3A%5B%5D%7D%5D%7D";
	content.Replace("---", GPCode);
	content.Replace("___", DataPeriod);
	content.Replace("^^^", DataInterval);
	content.Replace("~~~", DataNum);
	WebPostDownload(posturl, (char*)(LPCSTR)content, m_CookieData, strFile);
	
	//load
	CStdioFile ProductsFile;
	if(!ProductsFile.Open(strFile, CStdioFile::modeRead|CStdioFile::shareDenyNone)) 
		return;
	int nFileLen = ProductsFile.GetLength();
	char *pData = new char[nFileLen+1];
	memset(pData, 0, nFileLen+1);
	int nRead = ProductsFile.Read(pData, nFileLen);
	ProductsFile.Close();
	if(nRead<1) 
	{
		delete []pData;
		return;
	}
	
	vector<AnalyData> AllHisData_Raw;
	vector<char*> vDataBlock;
	int nBlock = GetCmdLine(pData, vDataBlock, -1, "\\\"Points\\\":", ",\\\"Events\\\":");
	if(vDataBlock[0])
	{
		CString HisData = vDataBlock[0];
		HisData.Replace("\\", "");
		CStdioFile DataFile;
		if(DataFile.Open(strFile, CFile::modeCreate|CStdioFile::modeWrite|CStdioFile::shareDenyNone)) 
		{
			DataFile.WriteString(HisData);
			DataFile.Close();
		}
		GetHistoryData_au(AllHisData_Raw, GPCode);
		CreateNewAnalyData(&AllHisData_Raw[0], AllHisData_Raw.size(), OutCome, nPerItem, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
int CalcScanerValue(float fExpectProfit, long lStartDate, long lEndDate, vector<AnalyData> HisData)
{
	int nTol = HisData.size();
	int nValidCount=0, nInvalidCount=0, nTolCount=0;
	for(int i=nTol-1;i>=0;i--)
	{
		if(HisData[i].Time.Date<lStartDate) break;
		if(HisData[i].Time.Date>lStartDate&&HisData[i].Time.Date<=lEndDate)
		{
			nTolCount++;

			float fUpDist=0.0f;
			if(fabs(HisData[i].Low-HisData[i].Open)<COMPPREC) 
				fUpDist = HisData[i].High-HisData[i].Low;
			else if(fabs(HisData[i].High-HisData[i].Open)<COMPPREC)
				fUpDist = 0.5f*(HisData[i].High-HisData[i].Open)+0.5f*(HisData[i].Close-HisData[i].Low);
			else 
				fUpDist = (HisData[i].High-HisData[i].Low)/3.0f+(HisData[i].High-HisData[i].Open)/3.0f+(HisData[i].Close-HisData[i].Low)/3.0f;

			if(100.0f*fUpDist/HisData[i].Open>3.82f*fExpectProfit)
				nValidCount++;
			if(100.0f*fUpDist/HisData[i].Open<fExpectProfit)
				nInvalidCount++;
		}
	}
	if(nTolCount>0)
		return nValidCount-nInvalidCount;
	return -1000;
}
struct StockInfo
{
	char Code[21];
	float fNow;
};
void	GetASXListResults()
{
	FILE *fp_AsxList = _fsopen(g_WSXHStr+"ASXListedCompanies.csv","rt",_SH_DENYNO);
	if(!fp_AsxList) return;
	CTime now = CTime::GetCurrentTime();
	long lNow = 10000*now.GetYear()+100*now.GetMonth()+now.GetDay();
	CTime Start1 = now-CTimeSpan(30, 0, 0, 0);
	long lStart1 = 10000*Start1.GetYear()+100*Start1.GetMonth()+Start1.GetDay();
	CTime Start2 = Start1-CTimeSpan(30, 0, 0, 0);
	long lStart2 = 10000*Start2.GetYear()+100*Start2.GetMonth()+Start2.GetDay();
	CTime Start3 = Start2-CTimeSpan(30, 0, 0, 0);
	long lStart3 = 10000*Start3.GetYear()+100*Start3.GetMonth()+Start3.GetDay();

	map<int, StockInfo, greater_equal<int> > FilterResult;
	int nReadCount=0;
	for(rewind(fp_AsxList);!feof(fp_AsxList);)
	{
		char lpItem[255]={0};
		fgets(lpItem,255,fp_AsxList);
		char *argv[8]={0};
		int nBlock = GetCmdLine(lpItem, argv, 8, "\"");
		if(argv[2]&&argv[3]&&argv[4]&&strncmp((const char*)argv[2],",",1)==0&&strncmp((const char*)argv[4],",",1)==0)
		{
			nReadCount++;

			vector<AnalyData> HisData;
			GetHisDataFromASX_au(argv[3], HisData);
			int nVal1 = CalcScanerValue(1.0f, lStart1, lNow, HisData);
			int nVal2 = CalcScanerValue(1.0f, lStart2, lNow, HisData);
			int nVal3= CalcScanerValue(1.0f, lStart3, lNow, HisData);

			StockInfo si={0};
			strncpy(si.Code, argv[3], 20);
			if(HisData.size()>0)
				si.fNow = HisData[HisData.size()-1].Close;
			FilterResult[nVal1+nVal2+nVal3] = si;
		}
	}
	fclose(fp_AsxList);
	
	char strTitle[20]={0};
	sprintf(strTitle, "res\\ASX_Scaner_%d.csv", nReadCount);
	FILE *m_fpall = _fsopen(strTitle,"wt",SH_DENYNO);
	if(m_fpall)
	{
		map<int, StockInfo, greater_equal<int> >::iterator iter = FilterResult.begin();
		for(;iter!=FilterResult.end();iter++)
		{
			int nVal = (*iter).first;
			StockInfo si = (*iter).second;
			fprintf(m_fpall, "%d\t%s\t%.3f\r\n", nVal, si.Code, si.fNow);
		}
		fclose(m_fpall);
	}
}

void	GetHisDataFromASX_au(char *GpCode, vector<AnalyData> &HisData)
{
	char url[MAX_PATH]={0};
	time_t tt = time(NULL);
	sprintf(url, "https://www.asx.com.au/asx/1/chart/highcharts?asx_code=%s&complete=true", GpCode);
	WebContentDownload(url, "ASX_Data.txt");

	Json::Reader reader; 
    Json::Value root;
	
    ifstream is;  
    is.open ("ASX_Data.txt");    
    if (reader.parse(is, root, FALSE)) 
    {   
		Json::Value::iterator iter = root.begin();
		for(;iter!=root.end();iter++)
		{
			Json::Value OneItem = (*iter);
			AnalyData iData={0};
			Json::Value::iterator iter1 = OneItem.begin();
			BOOL bError = FALSE;
			for(int i=0;iter1!=OneItem.end();iter1++,i++)
			{
				Json::Value iVal = (*iter1);
				double info = iVal.asDouble();
				switch(i)
				{
				case 0:
					{
						time_t tt= (time_t)(unsigned long)(info/1000.0);
						CTime ct = CTime(tt);
						iData.Time.Date = 10000*ct.GetYear()+100*ct.GetMonth()+ct.GetDay();
					}
					break;
				case 1:
					iData.Open = (float)info;
					break;
				case 2:
					iData.High = (float)info;
					break;
				case 3:
					iData.Low = (float)info;
					break;
				case 4:
					iData.Close = (float)info;
					break;
				case 5:
					iData.Volume = (DWORD)info;
					break;
				default:
					bError = TRUE;
					break;
				}
			}
			if(!bError)
				HisData.push_back(iData);
		}
    }  
    is.close();
}

void	FiltBestFrom(double dMinDistFlag, vector<int> &Selected, map<double, int, greater_equal<double> > AllItems)
{
	if(AllItems.empty()) return;

	map<double, int, greater_equal<double> >::iterator iterStart = AllItems.begin();
	map<double, int, greater_equal<double> >::iterator iter = AllItems.end();
	iter--;
	map<double, int, greater_equal<double> >::iterator iterEnd = iter;
	double best1 = (*iterStart).first;
	double worst1 = (*iterEnd).first;
	double dAverGap1 = (best1-worst1)*dMinDistFlag;
	int nSkipItem = (double)AllItems.size()/10.0+0.5;
	int nRealSkip = 0;
	for(int i=0;i<nSkipItem;i++)
	{
		iterStart++;
		iterEnd--;
		nRealSkip++;
	}
	double best2 = (*iterStart).first;
	double worst2 = (*iterEnd).first;
	double dAverGap2 = (best2-worst2)*(1.0-2.0*(double)nRealSkip/(double)AllItems.size())*dMinDistFlag;
	double dAverGap = max(dAverGap1, dAverGap2);
	
	iterStart = AllItems.begin();
	Selected.push_back((*iterStart).second);
	double dSelectedVal = (*iterStart).first;
	while(TRUE)
	{
		iterStart++;
		if(iterStart==AllItems.end())
			break;
		double dThisVal = (*iterStart).first;
		if(dSelectedVal<dThisVal)
			AfxMessageBox("Wrong Order In SelectBestFrom");
		if(dSelectedVal-dThisVal<dAverGap+max(0.0, dThisVal*dMinDistFlag))
		{
			Selected.push_back((*iterStart).second);
			dSelectedVal = dThisVal;
		}
		else 
			break;
	}
}

double	CalcBestAverFrom1(vector<double> DataVal, double fRatioFlag, int nMinCnt)
{
	if(DataVal.empty())
		return 0.0;

	BOOL bFirst = TRUE;
	double fLastAver = 0.0;
	double fSum = 0.0;
	int nCnt = 0;
	double fRatio = 1.0;
	int nOriCnt = DataVal.size();
	while(TRUE)
	{
		for(int i=0;i<nOriCnt;i++)
		{
			if(bFirst||DataVal[i]>fLastAver-COMPPREC)
			{
				fSum += DataVal[i];
				nCnt++;
			}
		}
		if(nCnt<1) 
			return fLastAver;
		
		double fAver = fSum/(double)nCnt;
		if(!bFirst)
			fRatio = (float)nCnt/(float)nOriCnt;
		if(fRatio<fRatioFlag||nCnt<=nMinCnt)
			return fLastAver;
		if(!bFirst&&fabs(fAver-fLastAver)<COMPPREC)
			return fLastAver;

		fLastAver = fAver;
		bFirst = FALSE;

		fSum = 0.0;
		nCnt = 0;
	}
	return fLastAver;
}


//����1��Lnk�ļ�·��������2�����ش��Ŀ��·��
bool GetShellPath(char *Src,char *ShellPath)
{
	bool blret=false;
	::CoInitialize(NULL); //��ʼ��COM�ӿ�
	IShellLink *psl = NULL;
	//����COM�ӿڣ�IShellLink���󴴽�
	HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL,CLSCTX_INPROC_SERVER,IID_IShellLink, (LPVOID *)&psl);
	if (SUCCEEDED(hr))
	{
		IPersistFile *ppf ;
		hr=psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf);
		if (SUCCEEDED(hr))
		{
			WCHAR wsz[MAX_PATH] ;
			MultiByteToWideChar( CP_ACP, 0, Src, -1, wsz, MAX_PATH ) ;    //ת�¿��ֽ�
			hr=ppf->Load(wsz, STGM_READ);    //�����ļ�
			if (SUCCEEDED(hr))
			{
				WIN32_FIND_DATA wfd ;
				psl->GetPath(ShellPath,MAX_PATH, (WIN32_FIND_DATA*)&wfd,SLGP_SHORTPATH);  //��ȡĿ��·��
				blret=true;
			}
			ppf->Release(); 
		}
		psl->Release();  //�ͷŶ���
	}
	::CoUninitialize();   //�ͷ�COM�ӿ�
	return blret;	   
}
