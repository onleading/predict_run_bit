// MyFileFunc.cpp : implementation file
//
#include "stdafx.h"
#include "MyFileFunc.h"
#include <sys/stat.h>
#include <sys/types.h>

#ifdef	SUPPORT_ZIP
#include "UnzipFile.h"
#include "ZipFile.h"
#define UNZIPBUF_SIZE	1024*500
#define ZIPBUF_SIZE		1024*500
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CMyFileFunc g_FF;

/////////////////////////////////////////////////////////////////////////////
// CMyFileFunc

CMyFileFunc::CMyFileFunc()
{
}

CMyFileFunc::~CMyFileFunc()
{
}

void CMyFileFunc::FormatPath(CString &path,BOOL hasT)
{
	if(hasT)
	{
		if(path.Right(1)!="\\") path+="\\";
	}
	else
	{
		if(path.Right(1)=="\\") path=path.Left(path.GetLength()-1);
	}
}

BOOL CMyFileFunc::MakeSureDirectoryPathExists(const char *lpPath)
{
	CString pathname = lpPath;
	if(pathname.Right(1) != "\\")
		pathname += "\\" ;
	int end = pathname.ReverseFind('\\');
	int pt = pathname.Find('\\');
	if (pathname[pt-1] == ':')
		pt = pathname.Find('\\', pt+1);
	CString path;
	while(pt != -1 && pt<=end)
	{
		path = pathname.Left(pt+1);
		if(!FileExist(path)) CreateDirectory(path,NULL);
		pt = pathname.Find('\\', pt+1);
	}
	return true;
}

BOOL CMyFileFunc::FileExist(const char *file)
{
	struct _stat buf;
	int ret;
	ret=_stat(file,&buf);
	if(ret==-1 && errno==2)//2==ENOENT
	{
		return FALSE;
	}
	return TRUE;
}

CString CMyFileFunc::GetPathName(LPCSTR file)
{
	CString tmp=file;
	int j=tmp.ReverseFind('\\');
	return tmp.Left(j+1);
}

void CMyFileFunc::SelectFolder(LPCSTR Caption,CString &path,HWND hParentWnd)
{
	BROWSEINFO bi;			//控制Browse对话框上属性的结构体 
	LPITEMIDLIST ItemID;	
	char sDir[MAX_PATH];		//MAX_PATH:系统常量,指最大路径长度 
	char FolderName[MAX_PATH];	
	memset(&bi, 0, sizeof(BROWSEINFO));		//清零 
	memset(sDir, 0, MAX_PATH);	
	bi.hwndOwner      = hParentWnd;		//父窗口句柄 
	bi.pszDisplayName = FolderName;		//存放路径的缓冲区 
	bi.lpszTitle      = Caption;		//对话框标题 
	bi.ulFlags        = BIF_RETURNONLYFSDIRS;		//只显示文件系统文件夹 
	ItemID = SHBrowseForFolder(&bi);	
	if(ItemID==NULL) return;
	SHGetPathFromIDList(ItemID, sDir);
	GlobalFree(ItemID);	
	CString PathName=sDir;	
	if(PathName.IsEmpty()) return;
	if(PathName.Right(1)!="\\") PathName+="\\";
	path=PathName;
}

BOOL CMyFileFunc::DeleteDirectory(LPCSTR DirName) 
{ 
	CFileFind tempFind; 
	char tempFileFind[200]; 
	CString strDir;
	strDir.Format("%s",DirName);
	if(strDir.Right(1)!="\\") strDir+="\\";
	sprintf(tempFileFind,"%s",(LPCSTR)(strDir+"*.*")); 
	
	BOOL IsFinded=(BOOL)tempFind.FindFile(tempFileFind); 
	while(IsFinded) 
	{ 
		IsFinded=(BOOL)tempFind.FindNextFile(); 
		if(!tempFind.IsDots())
		{ 
			char foundFileName[200]; 
			strcpy(foundFileName,(LPCSTR)tempFind.GetFileName());
			foundFileName[199]=0;
			if(tempFind.IsDirectory()) 
			{ 
				char tempDir[200]; 
				sprintf(tempDir,"%s%s",(LPCSTR)strDir,foundFileName); 
				if(!DeleteDirectory(tempDir)) return FALSE;
			} 
			else 
			{ 
				char tempFileName[200]; 
				sprintf(tempFileName,"%s%s",(LPCSTR)strDir,foundFileName); 
				DeleteFile(tempFileName); 
			} 
		} 
	} 
	tempFind.Close(); 
	if(!RemoveDirectory((LPCSTR)strDir)) 
	{
		TRACE("--RemoveDirectory %s failed\n",strDir);
		return FALSE; 
	}
	return TRUE; 
} 

//将mainpath下所有文件的绝对/相对路径存入pathlist中
void CMyFileFunc::GetAllFilePath(CString mainpath,PATHLIST &pathlist,BOOL bAbsolute)
{
	GetAllFilePath2(mainpath,pathlist);
	if(!bAbsolute)
	{
		int n=pathlist.size();
		for(int i=0;i<n;i++)
			pathlist[i].Replace(mainpath,"");
	}
	
}
void CMyFileFunc::GetAllFilePath2(CString mainpath,PATHLIST &pathlist)
{
	CFileFind f;
	g_FF.FormatPath(mainpath,TRUE);
	BOOL bFind=f.FindFile(mainpath+"*.*");
	while(bFind)
	{
		bFind = f.FindNextFile();
		if(f.IsDots())
			continue;
		if (f.IsDirectory())
		{
			CString str = f.GetFilePath();
			g_FF.FormatPath(str,TRUE);
			GetAllFilePath(str,pathlist);
		}
		else
			pathlist.push_back(f.GetFilePath());
	}
}

//////////////////////////////////////////////////////////////////////////
//for ZIP operation
#ifdef SUPPORT_ZIP

//解压zip文件中的所有文件到指定目录
BOOL CMyFileFunc::UnZipAllFilesToPath(const char *zipfilename,const char *path)
{
	int dirtoken_pos,dirtoken_from;
	CString tmpFileName;
	CString tmpPath=path;
	FormatPath(tmpPath,TRUE);
	//先看有没有此Zip文件
	if(!FileExist(zipfilename)) return FALSE;
	
	CString tmpStr;
	char* pName = new char [MAX_PATH];
	char *pbuf=new char[UNZIPBUF_SIZE];

	try
	{
		CUnzipFile uf(zipfilename);
		uf.GoToFirstFile();
		do
		{
			memset(pName,0,MAX_PATH);
			unz_file_info ui;
			
			// 得到Zip包中的文件名
			uf.GetCurrentFileInfo(&ui,pName,MAX_PATH);
			
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
					//如果包括目录,必须先创建此目录
					tmpFileName=tmpFileName.Mid(0,dirtoken_pos);
					CreateDirectory(tmpPath+tmpFileName,NULL);
				}
			}while(dirtoken_pos>=0);
			tmpFileName=tmpPath + pName;
			CFile f;
			if(!f.Open(tmpFileName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone))
			{
				//尝试去掉只读属性
				CFileStatus fs;
				CFile::GetStatus(tmpFileName, fs);
				if(fs.m_attribute&CFile::readOnly)
				{
					fs.m_attribute &= ~CFile::readOnly;
					CFile::SetStatus(tmpFileName, fs);
				}
				if(!f.Open(tmpFileName, CFile::modeWrite | CFile::modeCreate | CFile::shareDenyNone))
					continue;
			}
			uf.OpenCurrentFile();
			int size_read;
			do
			{
				size_read = uf.ReadCurrentFile(pbuf, UNZIPBUF_SIZE);
				if (size_read > 0)
					f.WriteHuge(pbuf, size_read);
			}while (size_read == UNZIPBUF_SIZE);
			uf.UpdateFileStatus(f, ui);
		}while(uf.GoToNextFile());
		uf.Close();
	}
	catch (CException* e)
	{
		TDEL(pName);
		TDEL(pbuf);
		e->Delete();
		return FALSE;
	}
	TDEL(pName);
	TDEL(pbuf);
	return TRUE;
}

BOOL CMyFileFunc::ZipFiles(const char *mainpath,PATHLIST pathlist,const char *zipfilename)
{
	CString strmainpath=mainpath;
	int nfile=pathlist.size();
	if(nfile<1) return FALSE;
	CZipFile zf(zipfilename);
	zip_fileinfo zi;
	CFile filetemp;
	int size_read=0;
	char *pbuf=new char[ZIPBUF_SIZE];
	
	for(int i=0;i<nfile;i++)
	{
		if(!FileExist(strmainpath+pathlist[i]))	continue;
		if(filetemp.Open(strmainpath+pathlist[i],CFile::modeRead|CFile::shareDenyNone))
		{
			zf.UpdateZipInfo(zi, filetemp);
			zf.OpenNewFileInZip(pathlist[i], zi, Z_BEST_COMPRESSION);
			do
			{
				size_read = filetemp.ReadHuge(pbuf, ZIPBUF_SIZE);
				if (size_read)
					zf.WriteInFileInZip(pbuf, size_read);
			} while(size_read == ZIPBUF_SIZE);
			filetemp.Close();
		}
		else
		{
			zf.Close();
			TDEL(pbuf);
			return FALSE;
		}
	}
	zf.Close();
	if(!FileExist(zipfilename))
	{
		TDEL(pbuf);
		return FALSE;
	}
	TDEL(pbuf);
	return TRUE;
}
#endif

#ifdef SUPPORT_TEA
//输入: s 待加密串,len_s 串长度,key 密钥,len_k 密钥长度
//输出: d 加密后字符串,len_d 串长度
BOOL CMyFileFunc::TEA_encipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k)
{
//	TRACE("--TEA_encipher KEY:%08X%08X%08X%08X\n",*(ULONG*)key,*(ULONG*)(key+4),*(ULONG*)(key+8),*(ULONG*)(key+12));
	TDEL(d);
	len_d=0;
	if(len_s==0 || !s || !key || len_k==0) return FALSE;
	if(len_s%8!=0)
	{
		len_d=1+(8-len_s%8)+len_s+7;
		d=new byte[len_d];
		if(!d)
		{
			len_d=0;
			return FALSE;
		}
		d[0]=8-len_s%8;
		srand((unsigned)time(NULL));
		memset(d+1,rand(),d[0]);
		memcpy(d+1+d[0],s,len_s);
		memset(d+1+d[0]+len_s,0,7);
	}
	else
	{
		len_d=1+len_s+7;
		d=new byte[len_d];
		if(!d)
		{
			len_d=0;
			return FALSE;
		}
		d[0]=0;
		memcpy(d+1,s,len_s);
		memset(d+1+len_s,0,7);
	}

	byte key_md5[16];
	MD5BIN_Buffer(key,len_k,key_md5);
	MD5BIN_Buffer(key_md5,16,key_md5);
	byte tmp_key[16];

	memcpy(tmp_key,key_md5,16);
	for(long i=0;i<len_d;i+=8)
	{
		_TEA_encipher((ULONG*)(d+i),(ULONG*)tmp_key,16);
		memcpy(tmp_key,d+i,8);
	}
	return TRUE;
}

//输入: s 待解密串,len_s 串长度,key 密钥,len_k 密钥长度
//输出: d 解密后字符串,len_d 串长度
BOOL CMyFileFunc::TEA_decipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k)
{
//	TRACE("--TEA_decipher KEY:%08X%08X%08X%08X\n",*(ULONG*)key,*(ULONG*)(key+4),*(ULONG*)(key+8),*(ULONG*)(key+12));
	TDEL(d);
	len_d=0;
	if(len_s==0 || !s || len_s%8!=0 || !key || len_k==0) return FALSE;

	byte *buf=new byte[len_s];
	if(!buf) return FALSE;
	memcpy(buf,s,len_s);

	byte key_md5[16];
	MD5BIN_Buffer(key,len_k,key_md5);
	MD5BIN_Buffer(key_md5,16,key_md5);

	byte tmp_key[16];
	memcpy(tmp_key+8,key_md5+8,8);
	for(long i=len_s-8;i>=8;i-=8)
	{
		memcpy(tmp_key,buf+i-8,8);
		_TEA_decipher((ULONG*)(buf+i),(ULONG*)tmp_key,16);
	}
	memcpy(tmp_key,key_md5,16);
	_TEA_decipher((ULONG*)buf,(ULONG*)tmp_key,16);
	
	BOOL bWrong=FALSE;
	for(i=0;i<7;i++)
	{
		if(buf[len_s-1-i]!=0)
		{
			bWrong=TRUE;
			break;
		}
	}
	if(bWrong)
	{
		delete[] buf;
		return FALSE;
	}
	len_d=len_s-8-buf[0];
	d=new byte[len_d];
	if(!d)
	{
		len_d=0;
		delete[] buf;
		return FALSE;
	}
	memcpy(d,buf+(1+buf[0]),len_d);
	delete[] buf;
	return TRUE;
}

#define TEA_BUFFERSIZE 0x100000	//1024*1024
//加密文件
BOOL CMyFileFunc::fTEA_encipher(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,byte *key,long len_key)
{
	if(!key || len_key==0) return FALSE;
	CString tmp=lpNewFileName;
	if(tmp.Right(1)=="\\") return FALSE;
	int i=tmp.ReverseFind('\\');
	tmp=tmp.Left(i+1);
	if(!MakeSureDirectoryPathExists((LPCSTR)tmp)) return FALSE;
	CFile f,f_new;
	if(!f.Open(lpExistingFileName,CFile::modeRead|CFile::shareDenyWrite))
		return FALSE;
	if(!f_new.Open(lpNewFileName,CFile::modeCreate|CFile::shareDenyNone|CFile::modeWrite))
	{
		f.Close();
		return FALSE;
	}
	byte *buf=new byte[TEA_BUFFERSIZE-8];
	if(!buf)
	{
		f.Close();
		f_new.Close();
		return FALSE;
	}

	byte tmp_key[16]={0};
	MD5BIN_Buffer(key,len_key,tmp_key);

	DWORD size_read=0;
	byte *buf_new=NULL;
	long len_buf_new=0;

	do {
		size_read=f.ReadHuge(buf,TEA_BUFFERSIZE-8);
		BOOL bWork=TEA_encipher(buf,size_read,buf_new,len_buf_new,tmp_key,16);
		if(!bWork || !buf_new)
		{
			f.Close();
			f_new.Close();
			TDEL(buf);
			return FALSE;
		}
		f_new.WriteHuge(buf_new,len_buf_new);
		memcpy(tmp_key,buf_new+len_buf_new-16,16);
		TDEL(buf_new);
	} while(size_read==TEA_BUFFERSIZE-8);

	f.Close();
	f_new.Close();
	TDEL(buf);
	return TRUE;
}

//解密文件
BOOL CMyFileFunc::fTEA_decipher(LPCSTR lpExistingFileName,LPCSTR lpNewFileName,byte *key,long len_key)
{
	if(!key || len_key==0) return FALSE;
	CString tmp=lpNewFileName;
	if(tmp.Right(1)=="\\") return FALSE;
	int i=tmp.ReverseFind('\\');
	tmp=tmp.Left(i+1);
	if(!MakeSureDirectoryPathExists((LPCSTR)tmp)) return FALSE;

	CFile f,f_new;
	if(!f.Open(lpExistingFileName,CFile::modeRead|CFile::shareDenyWrite))
		return FALSE;
	if(!f_new.Open(lpNewFileName,CFile::modeCreate|CFile::shareDenyNone|CFile::modeWrite))
	{
		f.Close();
		return FALSE;
	}
	byte *buf=new byte[TEA_BUFFERSIZE];
	if(!buf)
	{
		f.Close();
		f_new.Close();
		return FALSE;
	}
	
	byte tmp_key1[16]={0};
	byte tmp_key2[16]={0};
	MD5BIN_Buffer(key,len_key,tmp_key2);

	DWORD size_read=0;
	byte *buf_new=NULL;
	long len_buf_new=0;

	do {
		size_read=f.ReadHuge(buf,TEA_BUFFERSIZE);
		if(size_read>=16)
		{
			memcpy(tmp_key1,tmp_key2,16);
			memcpy(tmp_key2,buf+size_read-16,16);
			TEA_decipher(buf,size_read,buf_new,len_buf_new,tmp_key1,16);
		}
		if(!buf_new)
		{
			f.Close();
			f_new.Close();
			TDEL(buf);
			return FALSE;
		}
		f_new.WriteHuge(buf_new,len_buf_new);
		TDEL(buf_new);
	} while(size_read==TEA_BUFFERSIZE);
	
	f.Close();
	f_new.Close();
	TDEL(buf);
	return TRUE;

}
//分块加密
BOOL CMyFileFunc::mTEA_encipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k)
{
	TDEL(d);
	len_d=0;
	if(len_s==0 || !s || !key || len_k==0) return FALSE;

	//计算待加密数据将分成多少块处理,每块大小为TEA_BUFFERSIZE-8,加密后大小TEA_BUFFERSIZE
	long nBlock=len_s/(TEA_BUFFERSIZE-8);
	if(len_s%(TEA_BUFFERSIZE-8)!=0) nBlock++;
	//申请内存
	long bufsize=nBlock*TEA_BUFFERSIZE;
	byte *buf=new byte[bufsize];
	if(!buf) return FALSE;
	
	byte tmp_key[16]={0};
	MD5BIN_Buffer(key,len_k,tmp_key);
	
	byte *p1=s,*p2=buf,*pNew=NULL;
	long len_new=0,len_rest=len_s;
	bufsize=0;
	for(int i=0;i<nBlock;i++)
	{
		BOOL bWork=TEA_encipher(p1,(len_rest>(TEA_BUFFERSIZE-8))?TEA_BUFFERSIZE-8:len_rest,pNew,len_new,tmp_key,16);
		if(!bWork || !pNew)
		{
			delete[] buf;
			return FALSE;
		}
		memcpy(p2,pNew,len_new);
		p1+=(len_rest>(TEA_BUFFERSIZE-8))?(TEA_BUFFERSIZE-8):len_rest;
		p2+=len_new;
		bufsize+=len_new;
		memcpy(tmp_key,pNew+len_new-16,16);
		TDEL(pNew);
		len_new=0;
		len_rest-=TEA_BUFFERSIZE-8;
	}
	d=buf;
	len_d=bufsize;
	return TRUE;
}
//分块解密
BOOL CMyFileFunc::mTEA_decipher(byte *s,long len_s,byte *&d,long &len_d,byte *key,long len_k)
{
	TDEL(d);
	len_d=0;
	if(len_s==0 || !s || len_s%8!=0 || !key || len_k==0 ) return FALSE;

	//计算待解密数据将分成多少块处理,每块大小为TEA_BUFFERSIZE,解密后大小在TEA_BUFFERSIZE-8到TEA_BUFFERSIZE-15之间(包括)
	long nBlock=len_s/TEA_BUFFERSIZE;
	if(len_s%(TEA_BUFFERSIZE-8)!=0) nBlock++;
	//申请内存
	long bufsize=nBlock*TEA_BUFFERSIZE;
	byte *buf=new byte[bufsize];
	if(!buf) return FALSE;

	byte tmp_key1[16]={0};
	byte tmp_key2[16]={0};
	MD5BIN_Buffer(key,len_k,tmp_key2);
	
	byte *p1=s,*p2=buf,*pNew=NULL;
	long len_new=0,len_rest=len_s;
	bufsize=0;
	for(int i=0;i<nBlock;i++)
	{
		if(len_rest>=16)
		{
			memcpy(tmp_key1,tmp_key2,16);
			memcpy(tmp_key2,p1+(len_rest>TEA_BUFFERSIZE?TEA_BUFFERSIZE:len_rest)-16,16);
			TEA_decipher(p1,len_rest>TEA_BUFFERSIZE?TEA_BUFFERSIZE:len_rest,pNew,len_new,tmp_key1,16);
		}
		if(!pNew)
		{
			TDEL(buf);
			return FALSE;
		}
		memcpy(p2,pNew,len_new);
		p1+=len_rest>TEA_BUFFERSIZE?TEA_BUFFERSIZE:len_rest;
		p2+=len_new;
		bufsize+=len_new;
		TDEL(pNew);
		len_new=0;
		len_rest-=TEA_BUFFERSIZE;
	}
	d=buf;
	len_d=bufsize;
	return TRUE;
}

/**********************************************************
TEA加密算法

  Input values: 
  long v[2]    64-bit plaintext block
  long k[4]	128-bit key
  long n		加密轮数
  Output values:
  long v[2]    64-bit ciphertext block 
**********************************************************/
#define DELTA 0x9e3779b9 /* sqr(5)-1 * 2^31 */
void CMyFileFunc::_TEA_encipher(unsigned long *  v,const unsigned long *  k ,long n) 
{
	//	TRACE("--en IN:%08X%08X, ",v[0],v[1]);
    register unsigned long y = v[0], z = v[1], sum = 0, delta = DELTA, 
        a = k[0], b = k[1], c = k[2], d = k[3]; 
    while ( n-- > 0 ) 
    { 
        sum += delta; 
        y += ( z << 4 ) + a ^ z + sum ^ ( z >> 5 ) + b; 
        z += ( y << 4 ) + c ^ y + sum ^ ( y >> 5 ) + d; 
    } 
    v[0] = y; 
    v[1] = z; 
	//	TRACE("OUT:%08X%08X, KEY:%08X%08X%08X%08X\n",v[0],v[1],k[0],k[1],k[2],k[3]);
} 
void CMyFileFunc::_TEA_decipher(unsigned long *  v,const unsigned long *  k,long n ) 
{ 
	//	TRACE("--de IN:%08X%08X, ",v[0],v[1]);
    register unsigned long y = v[0], z = v[1], sum = DELTA*n, delta = DELTA, 
        a = k[0], b = k[1], c = k[2], d = k[3]; 
    while ( n-- > 0 ) 
    { 
        z -= ( y << 4 ) + c ^ y + sum ^ ( y >> 5 ) + d; 
        y -= ( z << 4 ) + a ^ z + sum ^ ( z >> 5 ) + b; 
        sum -= delta; 
    } 
    v[0] = y; 
    v[1] = z; 
	//	TRACE("OUT:%08X%08X, KEY:%08X%08X%08X%08X\n",v[0],v[1],k[0],k[1],k[2],k[3]);
}
#endif
