// FeedSource.cpp: implementation of the CFeedSource class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FeedSource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFeedItem::CFeedItem()
{
}

CFeedItem::~CFeedItem()
{
}

CFeedSource::CFeedSource()
{

}

CFeedSource::~CFeedSource()
{

}

CFeed::CFeed()
{
	m_pDoc = NULL;
	m_nDepth = 0;
	m_bAdded = FALSE;
}

CFeed::CFeed( CString strXMLFile )
{
	m_pDoc = NULL;
	m_nDepth = 0;
	m_bAdded = FALSE;
	BuildFromFile( strXMLFile );
}

CFeed::~CFeed()
{

}

/////////////////////////////////////////////////////////////////////////////
// Build XML Feed Information from an XML File
//
// strXMLFile:		Parsed in XML File Name
// This function will build Feed Object from scratch by parsing XML Feed Information
// Result is stored in m_source and m_item objects
//
void CFeed::BuildFromFile(CString strXMLFile)
{
	CString		strTmpFile = g_WSXHStr + _T("\\AgileReader.xml");
	CFileStatus FileStatus;
	if( !CFile::GetStatus(strTmpFile,FileStatus) )
		strTmpFile = HomePath+_T("\\AgileReader.xml");

	// Step 0. Download XML File
	if ( URLDownloadToFile( NULL, strXMLFile, strTmpFile,0, NULL ) != S_OK )
	{
		AfxMessageBox( _T("Failed to download ") + strXMLFile );
		return;
	}


	// Step 1. Open XML Document, if open fails, then return
	if ( m_pDoc != NULL )
	{
		m_pDoc->Release();
		m_pDoc = NULL;
	}
	if ( SUCCEEDED (CoCreateInstance(MSXML2::CLSID_DOMDocument,
									NULL,
									CLSCTX_INPROC_SERVER,
									MSXML2::IID_IXMLDOMDocument,
									reinterpret_cast<void**>(&m_pDoc))))

	m_pDoc->put_async( VARIANT_FALSE );
	if ( m_pDoc->load( _bstr_t(strTmpFile) ) == VARIANT_FALSE )
	{
		// Failed to load XML Document, report error message
		AfxMessageBox( _T("Failed to load XML Document") );
		return;
	}

	// Step 2. Get version property if it is available
	// Step 3. Iterate to channel node, get the following child items
	// title
	// link
	// description
	// language
	// copyright
	// webMaster
	// lastBuildDate
	// ttl
	// generator
	// Then go to image node, get the following items
	//		title
	//		url
	//		link
	//		width
	//		height
	//		description

	// Step 4. go to item node, get the following items
	//		title
	//		description
	//		link
	//		author
	//		category
	//		pubDate
	//		subject
	MSXML2::IXMLDOMNode		*pNode = NULL;
	if ( SUCCEEDED(m_pDoc->QueryInterface(MSXML2::IID_IXMLDOMNode, 
				reinterpret_cast<void**>(&pNode))))
	{
		IterateChildNodes(pNode);
		pNode->Release();
		pNode = NULL;
	}

	// We are not using smart pointer, so we have to release it outself
	if ( m_pDoc )
	{
		m_pDoc->Release();
		m_pDoc = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Get Feed Version Information
//
void CFeed::GetVersion(MSXML2::IXMLDOMNode *pNode)
{
}

/////////////////////////////////////////////////////////////////////////////
// Iterate Child Node
//
void CFeed::IterateChildNodes(MSXML2::IXMLDOMNode *pNode)
{
	BSTR		bstrNodeName;

	if ( pNode )
	{
		m_nDepth++;
		CString strOutput;
		pNode->get_nodeName(&bstrNodeName);

		//
		// Find out the node type (as a string).
		//
		BSTR bstrNodeType;
		pNode->get_nodeTypeString(&bstrNodeType);
		CString strType;
		strType = CString( bstrNodeType );
		SysFreeString(bstrNodeType);

		MSXML2::DOMNodeType eEnum;
		pNode->get_nodeType(&eEnum);

		CString strValue;
		BSTR bstrValue;
		switch( eEnum )
		{
			case MSXML2::NODE_TEXT:
			{
				// Text string in the XML document
				BSTR bstrValue;
				pNode->get_text(&bstrValue);
				strOutput = CString( bstrValue );
				SysFreeString(bstrValue);
				break;
			}
			case MSXML2::NODE_COMMENT:
			{
				// Comment in the XML document
				VARIANT vValue;
				pNode->get_nodeValue(&vValue);
				VariantClear(&vValue);
				break;
			}
			case MSXML2::NODE_PROCESSING_INSTRUCTION:
			{
				// Processing instruction
				strOutput = CString( bstrNodeName );
				break;
			}
			case MSXML2::NODE_ELEMENT:
			{
				// Element
				strOutput = CString( bstrNodeName );
				if ( strOutput == _T("rss") )
				{
					GetVersion( pNode );
				}
				else if ( strOutput == _T("copyright") )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strCopyright = CString( bstrValue );
				}
				else if ( strOutput == _T("title") && m_nDepth == 4 )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strTitle = CString( bstrValue );
				}
				else if ( strOutput == _T("link") && m_nDepth == 4 )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strLink = CString( bstrValue );
				}
				else if ( strOutput == _T("description") && m_nDepth == 4 )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strDescription = CString( bstrValue );
				}
				else if ( strOutput == _T("language") )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strLanguage = CString( bstrValue );
				}
				else if ( strOutput == _T("webMaster") )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strWebMaster = CString( bstrValue );
				}
				else if ( strOutput == _T("lastBuildDate") )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strLastBuildDate = CString( bstrValue );
				}
				else if ( strOutput == _T("ttl") )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strTtl = CString( bstrValue );
				}
				else if ( strOutput == _T("generator") )
				{
					pNode->get_text(&bstrValue);
					m_source.m_strGenerator = CString( bstrValue );
				}
				else if ( strOutput == _T("image") )
				{
					BuildImage( pNode );
				}
				else if ( strOutput == _T("item") )
				{
					BuildItem( pNode );
				}
				break;
			}
			case MSXML2::NODE_DOCUMENT:
			{
				// Document
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
				break;
			}
			case MSXML2::NODE_DOCUMENT_TYPE:
			{
				// Document Type
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
				break;
			}
			case MSXML2::NODE_DOCUMENT_FRAGMENT:
			{
				// Document Fragment
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
				break;
			}
			case MSXML2::NODE_NOTATION:
			{
				// Notation
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
				break;
			}
			case MSXML2::NODE_ENTITY:
			{
				// Entity
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
				break;
			}
			case MSXML2::NODE_ENTITY_REFERENCE:
			{
				// Entity Reference
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
			}
			case MSXML2::NODE_CDATA_SECTION:
			{
				// CData section
				strOutput = CString( bstrNodeName ) + _T(" - ") + CString( strType );
			}
		}
		SysFreeString(bstrNodeName);
	}

	//
	// Any child nodes of this node need displaying too.
	//
	MSXML2::IXMLDOMNode *pNext = NULL;
	MSXML2::IXMLDOMNode *pChild;
	pNode->get_firstChild(&pChild);
	while( pChild )
	{
		IterateChildNodes(pChild);
		pChild->get_nextSibling(&pNext);
		pChild->Release();
		pChild = pNext;
	}
	m_nDepth--;
}

/////////////////////////////////////////////////////////////////////////////
// Build Image Section
//
void CFeed::BuildImage(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNode *pNext = NULL;
	MSXML2::IXMLDOMNode *pChild;
	CString		strOutput;
	BSTR		bstrNodeName;
	BSTR		bstrValue;
	pNode->get_firstChild(&pChild);
	while( pChild )
	{
		pChild->get_nodeName(&bstrNodeName);
		strOutput = CString( bstrNodeName );
		if ( strOutput == _T("title") )
		{
			pChild->get_text(&bstrValue);
			m_source.m_strImageTitle = CString( bstrValue );
		}
		else if ( strOutput == _T("url") )
		{
			pChild->get_text(&bstrValue);
			m_source.m_strImageUrl = CString( bstrValue );
		}
		else if ( strOutput == _T("link") )
		{
			pChild->get_text(&bstrValue);
			m_source.m_strImageLink = CString( bstrValue );
		}
		else if ( strOutput == _T("width") )
		{
			pChild->get_text(&bstrValue);
			m_source.m_strImageWidth = CString( bstrValue );
		}
		else if ( strOutput == _T("height") )
		{
			pChild->get_text(&bstrValue);
			m_source.m_strImageHeight = CString( bstrValue );
		}
		else if ( strOutput == _T("description") )
		{
			pChild->get_text(&bstrValue);
			m_source.m_strImageDescription = CString( bstrValue );
		}

		pChild->get_nextSibling(&pNext);
		pChild->Release();
		pChild = pNext;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Build Item Section
//
void CFeed::BuildItem(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNode *pNext = NULL;
	MSXML2::IXMLDOMNode *pChild;
	CString		strOutput;
	BSTR		bstrNodeName;
	CFeedItem	item;
	BSTR		bstrValue;
	pNode->get_firstChild(&pChild);
	while( pChild )
	{
		pChild->get_nodeName(&bstrNodeName);
		strOutput = CString( bstrNodeName );
		if ( strOutput == _T("title") )
		{
			pChild->get_text(&bstrValue);
			item.m_strTitle = CString( bstrValue );
		}
		else if ( strOutput == _T("description") )
		{
			pChild->get_text(&bstrValue);
			item.m_strDescription = CString( bstrValue );
		}
		else if ( strOutput == _T("link") )
		{
			pChild->get_text(&bstrValue);
			item.m_strLink = CString( bstrValue );
		}
		else if ( strOutput == _T("author") )
		{
			pChild->get_text(&bstrValue);
			item.m_strAuthor = CString( bstrValue );
		}
		else if ( strOutput == _T("category") )
		{
			pChild->get_text(&bstrValue);
			item.m_strCategory = CString( bstrValue );
		}
		else if ( strOutput == _T("pubDate") )
		{
			pChild->get_text(&bstrValue);
			item.m_strPubDate = CString( bstrValue );
		}
		else if ( strOutput == _T("subject") )
		{
			pChild->get_text(&bstrValue);
			item.m_strSubject = CString( bstrValue );
		}

		pChild->get_nextSibling(&pNext);
		pChild->Release();
		pChild = pNext;
	}
	m_item.Add( item );
}

/////////////////////////////////////////////////////////////////////////////
// Save Feed Information into Database
//
void CFeed::Save( BOOL bSaveSource /*= TRUE*/ )
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;
	int					nIndex;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Execute Insert Statement on FeedSource
	// In double click case, we don't want to save this
	if ( bSaveSource )
	{
		strSQL.Format( _T("insert into FeedSource (FeedLink, description, title, version, copyright, generator, feedlanguage, lastbuilddate, ttl, webmaster, imagedescription, imageheight, imagewidth, imagelink, imagetitle, imageurl ) values('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')"),
			EscapeQuote( m_source.m_strLink ),
			EscapeQuote( m_source.m_strDescription ),
			EscapeQuote( m_source.m_strTitle ),
			_T("2.0"),
			EscapeQuote( m_source.m_strCopyright ),
			EscapeQuote( m_source.m_strGenerator ),
			EscapeQuote( m_source.m_strLanguage ),
			EscapeQuote( m_source.m_strLastBuildDate ),
			EscapeQuote( m_source.m_strTtl ),
			EscapeQuote( m_source.m_strWebMaster ),
			EscapeQuote( m_source.m_strImageDescription ),
			EscapeQuote( m_source.m_strImageHeight ),
			EscapeQuote( m_source.m_strImageWidth ),
			EscapeQuote( m_source.m_strImageLink ),
			EscapeQuote( m_source.m_strImageTitle ),
			EscapeQuote( m_source.m_strImageUrl ) );
		if ( !ExecuteSQL( pCnn, strSQL, strMsg ) )
		{
			// AfxMessageBox( strMsg + strSQL );
		}
		else
		{
			m_bAdded = TRUE;
		}
	}

  // Step 4. Execute Insert Statement on FeedItem
	for( nIndex = 0; nIndex < m_item.GetSize(); nIndex++ )
	{
		strSQL.Format( _T("insert into FeedItem (FeedLink, title, link, description, pubdate, author, category, subject) values('%s','%s','%s','%s','%s','%s','%s','%s')"),
			EscapeQuote( m_source.m_strLink ),
			EscapeQuote( m_item[nIndex].m_strTitle ),
			EscapeQuote( m_item[nIndex].m_strLink ),
			EscapeQuote( m_item[nIndex].m_strDescription ),
			EscapeQuote( m_item[nIndex].m_strPubDate ),
			EscapeQuote( m_item[nIndex].m_strAuthor ),
			EscapeQuote( m_item[nIndex].m_strCategory ),
			EscapeQuote( m_item[nIndex].m_strSubject )
			);
		if ( !ExecuteSQL( pCnn, strSQL, strMsg ) )
		{
			// AfxMessageBox( strMsg + strSQL );
		}
	}

	pCnn.Release();
}

/////////////////////////////////////////////////////////////////////////////
// Execute SQL Statement and return error message if any
//
BOOL CFeed::ExecuteSQL(_ConnectionPtr &pCnn, CString &strSQL, CString& strMsg)
{
	try
	{
		pCnn->Execute( _bstr_t( strSQL ), NULL, adCmdText );
	}
	catch( _com_error& e )
	{
		strMsg = GetComError( e );
		return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Retrieve Com Error Information
//
CString CFeed::GetComError( _com_error& e )
{
	CString		strMsg;

    _bstr_t bstrSource(e.Source());
    _bstr_t bstrDescription(e.Description());
    
    strMsg.Format( _T("Code = %08lx\nCode meaning = %s\nSource = %s\nDescription = %s"),
		e.Error(),
		e.ErrorMessage(),
		(LPCSTR) bstrSource,
		(LPCSTR) bstrDescription );
	return strMsg;
}

/////////////////////////////////////////////////////////////////////////////
// Load Feed Information from database
//
void CFeed::LoadLocal(CString &strLink)
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;
	_RecordsetPtr		rs = NULL;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Read FeedSource and populate it into m_source object
	strSQL.Format( _T("select * from FeedSource where FeedLink = '%s'"), strLink );
	try
	{
		rs = pCnn->Execute( _bstr_t( strSQL ), NULL, adCmdText );
		if ( rs != NULL && rs->adoEOF != TRUE )
		{
			m_source.m_strLink = strLink;
			m_source.m_strDescription = GetFieldValue( rs->Fields, 1 );
			m_source.m_strTitle = GetFieldValue( rs->Fields, 2 );
			m_source.m_strVersion = GetFieldValue( rs->Fields, 3 );
			m_source.m_strCopyright = GetFieldValue( rs->Fields, 4 );
			m_source.m_strGenerator = GetFieldValue( rs->Fields, 5 );
			m_source.m_strLanguage = GetFieldValue( rs->Fields, 6 );
			m_source.m_strLastBuildDate = GetFieldValue( rs->Fields, 7 );
			m_source.m_strTtl = GetFieldValue( rs->Fields, 8 );
			m_source.m_strWebMaster = GetFieldValue( rs->Fields, 9 );
			m_source.m_strImageDescription = GetFieldValue( rs->Fields, 10 );
			m_source.m_strImageHeight = GetFieldValue( rs->Fields, 11 );
			m_source.m_strImageWidth = GetFieldValue( rs->Fields, 12 );
			m_source.m_strImageLink = GetFieldValue( rs->Fields, 13 );
			m_source.m_strImageTitle = GetFieldValue( rs->Fields, 14 );
			m_source.m_strImageUrl = GetFieldValue( rs->Fields, 15 );
		}
	}
	catch( _com_error& e )
	{
		AfxMessageBox( GetComError( e ) );
	}
	rs.Release();
	rs = NULL;

	// Step 4. Read FeedItem and populate it into m_item object
	strSQL.Format( _T("select * from FeedItem where FeedLink = '%s' order by pubDate desc, title asc"), strLink );
	try
	{
		rs = pCnn->Execute( _bstr_t( strSQL ), NULL, adCmdText );
		while( rs != NULL && rs->adoEOF != TRUE )
		{
			if ( rs->adoEOF )
			{
				break;
			}
			CFeedItem	item;
			item.m_strTitle = GetFieldValue( rs->Fields, 1 );
			item.m_strLink = GetFieldValue( rs->Fields, 2 );
			item.m_strDescription = GetFieldValue( rs->Fields, 3 );
			item.m_strPubDate = GetFieldValue( rs->Fields, 4 );
			item.m_strAuthor = GetFieldValue( rs->Fields, 5 );
			item.m_strCategory = GetFieldValue( rs->Fields, 6 );
			item.m_strSubject = GetFieldValue( rs->Fields, 7 );
			item.m_strReadStatus = GetFieldValue( rs->Fields, 8 );
			m_item.Add( item );
			rs->MoveNext();
			if ( rs->adoEOF )
			{
				break;
			}
		}
	}
	catch( _com_error& e )
	{
		AfxMessageBox( GetComError( e ) );
	}
	rs.Release();
	rs = NULL;

	pCnn.Release();
	pCnn = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// Get Field Value
//
CString CFeed::GetFieldValue(FieldsPtr fields, long nIndex)
{
	_variant_t vt = fields->GetItem( nIndex )->Value;
	if ( vt.vt == VT_NULL || vt.vt == VT_EMPTY )
	{
		return _T("");
	}
	return CString( (char*)_bstr_t( vt ) );
}

void CFeed::GetFeedSourceList( CStringArray& strTitleArray, CStringArray& strLinkArray )
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;
	_RecordsetPtr		rs = NULL;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Read FeedSource and populate it into m_source object
	strSQL = _T("select distinct title, FeedLink from FeedSource order by title");
	try
	{
		rs = pCnn->Execute( _bstr_t( strSQL ), NULL, adCmdText );
		while ( rs != NULL && rs->adoEOF != TRUE && rs->BOF != TRUE )
		{
			if ( rs->adoEOF )
			{
				break;
			}
			strTitleArray.Add( GetFieldValue( rs->Fields, 0 ) );
			strLinkArray.Add( GetFieldValue( rs->Fields, 1 ) );
			rs->MoveNext();
		}
	}
	catch( _com_error& e )
	{
		AfxMessageBox( GetComError( e ) );
	}
	rs.Release();
	rs = NULL;
	pCnn.Release();
}

/////////////////////////////////////////////////////////////////////////////
// Get Module File Path
//
CString CFeed::GetModuleFileDir()
{
	DWORD	dwLength, dwSize;
	TCHAR	szFileName [MAX_PATH];
	CString	strFileName;
	int		nPos;
	
	dwSize = sizeof (szFileName) / sizeof (szFileName [0]);
	dwLength = ::GetModuleFileName (AfxGetInstanceHandle(), szFileName, dwSize);
	if (dwLength <= 0) 
	{
		return _T("");
	}

	strFileName = szFileName;
	nPos = strFileName.ReverseFind( '\\' );
	return strFileName.Left( nPos );
}

/////////////////////////////////////////////////////////////////////////////
// Escape Single Quote
//
CString CFeed::EscapeQuote(CString strValue)
{
	strValue.Replace( _T("'"), _T("''") );
	return strValue;
}

/////////////////////////////////////////////////////////////////////////////
// Delete list from Database
//
void CFeed::DeleteListArray(CStringArray &strLinkArray)
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;
	int					nIndex;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Delete Link from FeedList table
	for(nIndex = 0; nIndex < strLinkArray.GetSize(); nIndex++ )
	{
		strSQL.Format( _T("delete from FeedItem where link = '%s'"), EscapeQuote( strLinkArray[nIndex] ) );
		ExecuteSQL( pCnn, strSQL, strMsg );
	}

	// Step 4. Done!
	pCnn.Release();
}

/////////////////////////////////////////////////////////////////////////////
// Delete Feed Source
//
void CFeed::DeleteFeedSource(CString strLink)
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Delete Link from FeedList table
	strSQL.Format( _T("delete from FeedSource where FeedLink = '%s'"), EscapeQuote( strLink ) );
	ExecuteSQL( pCnn, strSQL, strMsg );

	// Step 4. Done!
	pCnn.Release();
}

/////////////////////////////////////////////////////////////////////////////
// Mark an item as read
//
void CFeed::MarkItemRead(CString strLink)
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Delete Link from FeedList table
	strSQL.Format( _T("update FeedItem set readstatus = 1 where link = '%s'"), EscapeQuote( strLink ) );
	ExecuteSQL( pCnn, strSQL, strMsg );

	// Step 4. Done!
	pCnn.Release();
}

/////////////////////////////////////////////////////////////////////////////
// Refresh All SubScriptions
//
void CFeed::RefreshAll()
{
	_ConnectionPtr		pCnn = NULL;
	CString				strSQL;
	CString				strMsg;
	CStringArray		strLinkArray;
	_RecordsetPtr		rs = NULL;

	// Step 1. Create object
	pCnn.CreateInstance( __uuidof( Connection ) );
	if ( pCnn == NULL )
	{
		AfxMessageBox( _T("Can not create connection object, please install MDAC!") );
		return;
	}

	// Step 2. Open connection
	try
	{
		CString		strCnn;
		strCnn.Format( _T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s\\AgileReader.mdb;Jet OLEDB:Database Password=philips;"), g_WSXHStr );
		pCnn->Open( _bstr_t(strCnn), "", "", adConnectUnspecified );
	}
	catch( _com_error& e )
	{
		AfxMessageBox( _T("Can not open connection:\n") + GetComError( e ) );
		pCnn.Release();
		return;
	}

	// Step 3. Get all the subscriptions
	strSQL = _T("select FeedLink from FeedSource");
	try
	{
		rs = pCnn->Execute( _bstr_t( strSQL ), NULL, adCmdText );
		while ( rs != NULL && rs->adoEOF != TRUE )
		{
			if ( rs->adoEOF )
			{
				break;
			}
			strLinkArray.Add( GetFieldValue( rs->Fields, 0 ) );
			rs->MoveNext();
		}
	}
	catch( _com_error& e )
	{
		AfxMessageBox( GetComError( e ) );
	}
	rs.Release();
	rs = NULL;

	// Step 4. Refreah each link
	for( int nIndex = 0; nIndex < strLinkArray.GetSize(); nIndex++ )
	{
		CFeed	feed( strLinkArray.GetAt( nIndex ) );
		feed.m_source.m_strLink = strLinkArray.GetAt( nIndex );
		feed.Save();
	}

	// Step 5. Done!
	pCnn.Release();
}
