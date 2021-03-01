#include	"stdafx.h"
#include	"dbfile.h"

#define CLOSE_FILE(hFile) {if(hFile!=INVALID_HANDLE_VALUE){CloseHandle(hFile);hFile=INVALID_HANDLE_VALUE;}}
/////////////////////////////////////////////////////////////////////////////
CDBFile::CDBFile()
{
	z_nField = 0;
	z_pField = NULL;
	z_pData = NULL;
	z_dwRecord = 0;
	z_hFile = INVALID_HANDLE_VALUE;
	memset(&z_iHd,0,sizeof(DBF_HEAD));
}

CDBFile::~CDBFile()
{
	CDBFile::Close();
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CDBFile::Open(LPCSTR FilePath)
{
	DWORD  BytesRead=0;
	SECURITY_ATTRIBUTES sa={0};
	sa.nLength = sizeof(sa);
	CDBFile::Close();
	z_hFile = CreateFile(FilePath,GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,OPEN_EXISTING,0,NULL);
	if(z_hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	if(ReadFile(z_hFile,&z_iHd,sizeof(DBF_HEAD),&BytesRead,NULL)&&
		(z_iHd.DbfFlag==0x03||z_iHd.DbfFlag==0x83)&&z_iHd.wHdSize>64&&
			(z_pData = new char[z_iHd.wRdSize+1])!=NULL&&
				(z_pField=new DBF_FIELD[z_nField=(z_iHd.wHdSize-33)/32])!=NULL&&
					ReadFile(z_hFile,z_pField,sizeof(DBF_FIELD)*z_nField,&BytesRead,NULL)) 
	{
		for(WORD i=0,FieldPos=1;i<z_nField;++i)
		{
			z_pField[i].FieldPos = FieldPos;
			z_pField[i].FieldPosLen = FieldPos+z_pField[i].FieldLength;
			z_pField[i].FieldName[10] = 0;
			FieldPos += z_pField[i].FieldLength;
		}
		memset(z_pData,0,z_iHd.wRdSize);
		return(Goto(1));
	}
	CDBFile::Close();
	return(FALSE);
}

void	CDBFile::Close()
{
	z_nField = 0;
	TDELA(z_pData);
	TDELA(z_pField);
	CLOSE_FILE(z_hFile);
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CDBFile::Goto(DWORD dwRecord)
{
	DWORD BytesRead = 0;
	SetFilePointer(z_hFile,4,NULL,FILE_BEGIN);
	if(!ReadFile(z_hFile,&z_iHd.dwCount,sizeof(DWORD),&BytesRead,NULL)||
		dwRecord<1||dwRecord>z_iHd.dwCount) return(FALSE);
	z_dwRecord = dwRecord;
	SetFilePointer(z_hFile,z_iHd.wHdSize+(dwRecord-1)*z_iHd.wRdSize,NULL,FILE_BEGIN);
	return(ReadFile(z_hFile,z_pData,z_iHd.wRdSize,&BytesRead,NULL));
}

BOOL	CDBFile::Skip()
{
	if(z_dwRecord>=z_iHd.dwCount)
		return FALSE;
	DWORD BytesRead = 0;
	++z_dwRecord;
	return(ReadFile(z_hFile,z_pData,z_iHd.wRdSize,&BytesRead,NULL));
}
/////////////////////////////////////////////////////////////////////////////
WORD	CDBFile::GetFieldNum()
{
	return(z_nField);
}

DWORD	CDBFile::GetRecordCount()
{
	CDBFile::Goto(z_dwRecord);
	return(z_iHd.dwCount);
}

PDBF_FIELD	CDBFile::GetFieldInfo(WORD wField)
{
	return(wField<z_nField?&z_pField[wField]:NULL);
}

WORD	CDBFile::GetFieldID(LPCSTR FieldName)
{
	for(WORD i=0;i<z_nField&&lstrcmpi(FieldName,z_pField[i].FieldName);++i);
	return(i<z_nField?i:0xFFFF);
}
/////////////////////////////////////////////////////////////////////////////
LPSTR	CDBFile::ReadString(LPCSTR FieldName,LPSTR lpString,WORD wMaxRead)
{
	for(WORD i=0;i<z_nField&&lstrcmpi(FieldName,z_pField[i].FieldName);++i);
	return(ReadString(i,lpString,wMaxRead));
}

LPSTR	CDBFile::ReadString(WORD wFieldID,LPSTR lpString,WORD wMaxRead)
{
	if(wFieldID>=z_nField)
		return NULL;
	if(wMaxRead)
	{
		--wMaxRead;
		wMaxRead = min(wMaxRead,z_pField[wFieldID].FieldLength);
	}
	else	wMaxRead = z_pField[wFieldID].FieldLength;
	memcpy(lpString,z_pData+z_pField[wFieldID].FieldPos,wMaxRead);
	lpString[wMaxRead] = 0;
	return(lpString);
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CDBFile::WriteString(LPCSTR FieldName,LPCSTR lpString,WORD wMaxWrite)
{
	for(WORD i=0;i<z_nField&&lstrcmpi(FieldName,z_pField[i].FieldName);++i);
	return(WriteString(i,lpString,wMaxWrite));
}

BOOL	CDBFile::WriteString(WORD wFieldID,LPCSTR lpString,WORD wMaxWrite)
{
	if(wFieldID>=z_nField)
		return FALSE;
	if(!wMaxWrite) wMaxWrite = lstrlen(lpString);
	wMaxWrite = min(wMaxWrite,z_pField[wFieldID].FieldLength);
	memcpy(z_pData+z_pField[wFieldID].FieldPos,lpString,wMaxWrite);
	if(wMaxWrite<z_pField[wFieldID].FieldLength)
	{
		memset(z_pData+z_pField[wFieldID].FieldPos+
			wMaxWrite,0x20,z_pField[wFieldID].FieldLength-wMaxWrite);
	}
	return(TRUE);
}

BOOL	CDBFile::UpdateRecord()
{
	DWORD BytesWrite = 0;
	if(z_dwRecord<1)
		return FALSE;
	SetFilePointer(z_hFile,z_iHd.wHdSize+(z_dwRecord-1)*z_iHd.wRdSize,NULL,FILE_BEGIN);
	if(WriteFile(z_hFile,z_pData,z_iHd.wRdSize,&BytesWrite,NULL))
	{
		if(z_dwRecord>z_iHd.dwCount)
		{
			z_iHd.dwCount = z_dwRecord;
			SetFilePointer(z_hFile,4,NULL,FILE_BEGIN);
			WriteFile(z_hFile,&z_iHd.dwCount,sizeof(DWORD),&BytesWrite,NULL);
			SetFilePointer(z_hFile,z_iHd.wHdSize+z_dwRecord*z_iHd.wRdSize,NULL,FILE_BEGIN);
		}
		return(TRUE);
	}
	return(FALSE);
}

BOOL	CDBFile::InsertRecord()
{
	z_dwRecord = z_iHd.dwCount+1;
	memset(z_pData,0x20,z_iHd.wRdSize);
	return(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL	CDBFile::Create(LPCSTR FilePath,WORD wFieldNum,PDBF_FIELD pField)
{
	SYSTEMTIME SysTime;
	GetLocalTime(&SysTime);
	DBF_HEAD  iHd = {0x03};
	iHd.Year = SysTime.wYear-1900;
	iHd.Month = BYTE(SysTime.wMonth);
	iHd.Day = BYTE(SysTime.wDay);
	iHd.wHdSize = (wFieldNum+1)*32+1;
	for(WORD i=0;i<wFieldNum;++i)
	{
		pField[i].FieldName[10] = 0;
		iHd.wRdSize += pField[i].FieldLength;
	}
	++iHd.wRdSize;
	char	EndEof = 0;
	DWORD  BytesWrite=0;
	SECURITY_ATTRIBUTES sa={0};
	sa.nLength = sizeof(sa);
	z_hFile = CreateFile(FilePath,GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,&sa,CREATE_ALWAYS,0,NULL);
	if(z_hFile==INVALID_HANDLE_VALUE)
		return FALSE;
	if(WriteFile(z_hFile,&iHd,32,&BytesWrite,NULL)&&
		WriteFile(z_hFile,pField,32*wFieldNum,&BytesWrite,NULL)&&
			WriteFile(z_hFile,&EndEof,1,&BytesWrite,NULL))
	{
		return(CDBFile::Open(FilePath));
	}
	CLOSE_FILE(z_hFile);
	DeleteFile(FilePath);
	return(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
