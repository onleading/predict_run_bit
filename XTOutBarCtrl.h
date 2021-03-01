//:Ignore
// XTOutBarCtrl.h interface for the CXTOutBarCtrl class.
//
// This file is a part of the Xtreme Toolkit for MFC.
// ©1998-2002 Codejock Software, All Rights Reserved.
//
// This source code can only be used under the terms and conditions 
// outlined in the accompanying license agreement.
//
// support@codejock.com
// http://www.codejock.com
//
//////////////////////////////////////////////////////////////////////
//:End Ignore

#if !defined(__XTOUTBARCTRL_H__)
#define __XTOUTBARCTRL_H__

//:Ignore
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
//:End Ignore
#include <afxtempl.h>
class CXTBarItem;
class CXTBarFolder;
////////////////////////////////////////////

const UINT XTWM_OUTBAR_NOTIFY      = (WM_USER+1018);

#define XT_IDC_LARGEICON                52778
#define XT_IDC_SMALLICON                52779
#define XT_IDC_REMOVEGROUP              52784
#define XT_IDC_RENAMEGROUP              52785
#define XT_IDC_REMOVEITEM               52780
#define XT_IDC_RENAMEITEM               52780

const UINT OBN_XT_ITEMCLICK        = 1;
const UINT OBN_XT_ONLABELENDEDIT   = 2;
const UINT OBN_XT_ONGROUPENDEDIT   = 3;
const UINT OBN_XT_DRAGITEM         = 4;
const UINT OBN_XT_FOLDERCHANGE     = 5;
const UINT OBN_XT_ITEMHOVER        = 6;
const UINT OBN_XT_DELETEITEM       = 7; 
const UINT OBN_XT_DELETEFOLDER     = 8;
const UINT OBN_XT_BEGINDRAG        = 9;
const UINT OBN_XT_ITEMRCLICK       = 10;

const UINT OBS_XT_SMALLICON        = 0x0001; // Sets small icon mode
const UINT OBS_XT_LARGEICON        = 0x0002; // Sets large icon mode
const UINT OBS_XT_EDITGROUPS       = 0x0004; // Enables folder local editing (renaming) 
const UINT OBS_XT_EDITITEMS        = 0x0008; // Enables item local editing (renaming) 
const UINT OBS_XT_REMOVEGROUPS     = 0x0010; // Enables the "Remove" command for folders in context menu
const UINT OBS_XT_REMOVEITEMS      = 0x0020; // Enables the "Remove" command for items in context menu 
const UINT OBS_XT_ADDGROUPS        = 0x0040; // Enables folder insertion
const UINT OBS_XT_DRAGITEMS        = 0x0080; // Enables item dragging to rearrange position
const UINT OBS_XT_ANIMATION        = 0x0100; // Enables animation while changing folder selection 
const UINT OBS_XT_SELHIGHLIGHT     = 0x0200; // Enables dimmed highlight of last pressed item 
const UINT OBS_XT_DEFAULT          = 0x00FC; // Same as OBS_XT_DRAGITEMS | OBS_XT_EDITGROUPS | OBS_XT_EDITITEMS | OBS_XT_REMOVEGROUPS | OBS_XT_REMOVEITEMS | OBS_XT_ADDGROUPS
///////////////////////////////////////////////////////////////
struct XT_OUTBAR_INFO 
{
    int     nIndex;    // Index of item.
    int     nDragFrom; // Starting drag index.
    int     nDragTo;   // Ending drag index.
    LPCTSTR lpszText;  // Item text.
    bool    bFolder;   // true if the item is a folder
};
/////////////////////////////////////////////////////////////////////////////
// CXTOutBarCtrl window

class  CXTOutBarCtrl : public CWnd
{
	DECLARE_DYNCREATE(CXTOutBarCtrl)

public:

    // Constructs a CXTOutBarCtrl object.
	CXTOutBarCtrl();

	// Destroys a CXTOutBarCtrl object, handles cleanup and de-allocation.
    virtual ~CXTOutBarCtrl();

protected:

//:Ignore
	int			m_nFolderHeight;			// used internally
	int			m_nSelFolder;				// used internally
	int			m_nFolderHilighted;			// used internally
	int			m_nItemHilighted;			// used internally
	int			m_nLastFolderSelected;		// used internally
	int			m_nLastItemSelected;		// used internally
	int			m_nFirstItem;				// used internally
	int			m_nIconSpacingLarge;		// used internally
	int			m_nIconSpacingSmall;		// used internally
	int			m_nHitInternal1;			// used internally
	int			m_nHitInternal2;			// used internally
	int			m_nLastDragItemDraw;		// used internally
	int			m_nLastDragItemDrawType;	// used internally
	int			m_nSelAnimCount;			// used internally
	int			m_nSelAnimTiming;			// used internally
	int			m_nAnimationTickCount;		// used internally
	BOOL		m_bUpArrow;					// used internally
	BOOL		m_bDownArrow;				// used internally
	BOOL		m_bUpPressed;				// used internally
	BOOL		m_bDownPressed;				// used internally
    BOOL        m_bIconPressed;				// used internally
	BOOL		m_bLooping;					// used internally
	BOOL		m_bPressedHighlight;		// used internally
	CFont		m_font;						// used internally
	CPen		m_penBlack;					// used internally
	CImageList*	m_pLargeImageList;			// used internally
	CImageList*	m_pSmallImageList;			// used internally
	DWORD		m_dwFlags;					// used internally
	CRect		m_rcUpArrow;				// used internally
	CRect		m_rcDownArrow;				// used internally
	CSize		m_sizeOffset;				// used internally
	CSize		m_sizeMargin;				// used internally
	CPtrArray	m_arFolder;					// used internally
	COLORREF    m_clrBack;					// used internally
	COLORREF    m_clrText;					// used internally
    BOOL        m_bUserClrBack;				// used internally
    BOOL        m_bUserClrText;				// used internally
//:End Ignore

//:Ignore
    typedef enum { F_NORMAL, F_SELECT, F_HILIGHT } FOLDER_HILIGHT; // used internally
//:End Ignore

public:

	// Call this member function to set an animation effect for the currently selected item.  Not to be used
	// with OBS_XT_SELHIGHLIGHT flag.
	virtual void SetAnimSelHighlight(
		// Specifies the time in milliseconds that the selected item will animate.
		const int iTime);

	// Call this member function to retreive the item data that was set for the specified
	// folder.
	virtual DWORD GetFolderData(
		// The index of the folder to retrieve item data for.  If -1 the currently selected
		// folder item data is returned.
		int iFolder = -1);

	// Retrieve, if any, the CWnd object of the iFolder object; if iFolder is -1, the child of 
	// the currently selected folder is returned. If no CWnd object is linked with the folder, 
	// NULL is returned. 

	// Call this member function to return the CWnd object that has been set for the specified folder.  If
	// no object has been set for the folder the return value is NULL.
	virtual CWnd* GetFolderChild(
		// Index of the folder to retrieve the CWnd object for, if -1 the currently selected
		// folder CWnd object is used.
		int iFolder = -1);

	// Call this member function to add a folder with a CWnd child nested inside of it.  You can insert a 
	// folder with any CWnd object such as a tree control (see the OutlookBar sample).
	virtual int AddFolderBar(
		// Name of the folder to add
		LPCTSTR lpszFolderName,
		// Points to a valid CWnd object, the object must be created before inserting.
		CWnd* pWndChild,
		// Item data (lParam) for folder.
		const DWORD dwData = 0);

	// Call this member function to return the text of the specified item
	// for the currently selected folder.
	virtual CString GetItemText(
		// Index of the item to retreive text for.
		const int iIndex);

	// Set the tickcount (in milliseconds) between every animation frame in folder scrolling; if you 
	// set a value of -1 or minor no animation will be played. Animation also required the 
	// OBS_XT_ANIMATION flag.

	// Call this member function to set the tickcount in milliseconds between each animation frame in
	// folder scrolling.  Animation requries the OBS_XT_ANIMATION flag to be set.
	virtual void SetAnimationTickCount(
		// Specifies the time in milliseconds between animation, a value of -1 will disable animation playback.
		const long lValue);

	// Call this member function to return the current animation tick count.
	virtual int GetAnimationTickCount();

	// Set the image index in the imagelist for the "iIndex" item of the currently selected folder. 

	// Call this member function to set the image index in the image list for
	// the currently selected folder.
	virtual void SetItemImage(
		// Index of the item in the currently selected folder.
		const int iIndex,
		// Index of the image in the image list to use for the specified item.
		const int iImage);

	// Call this member function to set the item data (lParam) for the specified item in the
	// currently selected folder.
	virtual void SetItemData(
		// Index of the item to set item data for.
		const int iIndex,
		// Item data (lParam) to set.
		const DWORD dwData);

	// Call this member function to retrieve the index of the image associated with
	// the specified item in the currently selected folder.
	virtual int  GetItemImage(
		// Index of the item to retrieve the image index for.
		const int iIndex) const;

	// Call this member function to get the item data (lParam) for the specified item
	// in the currently selected folder.
	virtual DWORD GetItemData(
		// Index of the item to retrieve item data for.
		const int iIndex) const;

	// Call this member function to remove the specifed item from the currently selected
	// folder
	virtual void RemoveItem(
		// Index of the item to remove.
		const int iIndex);

	// Call this member function to set the text for the specified item in the
	// currently selected folder.
	virtual void SetItemText(
		// Index of the item to set the text for.
		const int iIndex,
		// Points to a NULL terminated string.
		LPCTSTR lpszItemName);

	// Call this member function to begin local editing of the specified item in the 
	// currently selected folder.
	virtual void StartItemEdit(
		// Index of the item to begin editing for.
		const int iIndex);

	// Call this member function to set the text label for the specified folder.
	virtual void SetFolderText(
		// Index of the folder to set the text label for.
		const int iIndex,
		// Points to a NULL terminated string.
		LPCTSTR lpszFolderName);

	// Call this member function to begin editing of the specified folder item's label.
	virtual void StartGroupEdit(
		// Index of the folder to begin editing.
		const int iIndex);

	// Call this member function to return a pointer to the imagelist for the specified folder.
	virtual CImageList* GetFolderImageList(
		// Index of the folder to retrieve the imagelist for.
		const int iIndex,
		// TRUE to return the small image list, FALSE to return the large image list.
		const BOOL bSmall) const;

	// Call this member function to return the global image list for the OutlookBar control.
	virtual CImageList* GetImageList(
		// If OBS_XT_SMALLICON the small image list is retured, if OBS_XT_LARGEICON the large
		// image list is returned.
		DWORD dwImageList);

	// Call this member function to set the image list for the specified folder.  Returns a pointer
	// to the previously set image list, or NULL if no previouse image list exists.
	virtual CImageList* SetFolderImageList(
		// Index of the folder to set the image list for.
		const int iFolder,
		// Points to the new image list.
		CImageList* pImageList,
		// If OBS_XT_SMALLICON the small image list is set, if OBS_XT_LARGEICON the large
		// image list is set.
		DWORD dwImageList);

	// This member function will set the main imagelist; you can link different imagelists to the folders
	// using the SetFolderImageList function. If a folder has been linked to an imagelist with the 
	// SetFolderImageList function, it will own the linked imagelist; otherwise, it will use the use 
	// setted with this function.  Returns a pointer to the previously set image list, or NULL if no 
	// previouse image list exists.
	virtual CImageList* SetImageList(
		// Points to the new image list.
		CImageList* pImageList,
		// If OBS_XT_SMALLICON the small image list is set, if OBS_XT_LARGEICON the large
		// image list is set.
		DWORD dwImageList);

	// Call this member function to remove the specified folder and its items.
	virtual void RemoveFolder(
		// Index of the folder to remove.
		const int iIndex);

	// This member function will return the index of the currently selected folder.
	virtual int GetSelFolder() const;

	// This member function will return the total number of folders found it the Outlook Bar.
	virtual int GetFolderCount() const;

	// This member function will set the selected folder for the Outlook Bar.
	virtual void SetSelFolder(
		// Index of the new selected folder.
		const int iIndex);

	// This member function returns the number of items found int the currently selected folder.
	virtual int GetItemCount() const;

	// Call this member function to insert an item into the specified folder.  Returns the index of the
	// newly inserted item.
	virtual int InsertItem(
		// Index of the folder to insert the item into.
		const int iFolder,
		// Index or position of the item to insert into the folder.
		const int iIndex,
		// A NULL terminated string that represents the item label, this value cannot be set to NULL.
		LPCTSTR lpszItemName,
		// Index into the folder's image list.
		const int iImage = -1,
		// User defined item data that you can assign to the item, use GetItemData and SetItemData to 
		// access and change this data.
		const DWORD dwData = 0);

	// Call this member function to add a folder to the Outlook Bar control.  Returns the index of the
	// newly inserted folder.
	virtual int AddFolder(
		// A NULL terminated string that represents the folders label.
		LPCTSTR lpszFolderName,
		// User defined item data for the folder.
		const DWORD dwData);

    // Call this member function to modify a outlook bar style. Styles to 
    // be added or removed can be combined by using the bitwise OR (|) 
    // operator.
	//
    // The desired styles for the outlook bar can be one or more of the 
    // following:
    //[pre]
    // OBS_XT_EDITGROUPS   - Enables folder local editing (renaming) 
    // OBS_XT_EDITITEMS    - Enables item local editing (renaming) 
    // OBS_XT_REMOVEGROUPS - Enables the "Remove" command for folders in context menu
    // OBS_XT_REMOVEITEMS  - Enables the "Remove" command for items in context menu 
    // OBS_XT_ADDGROUPS    - Enables folder insertion
    // OBS_XT_DRAGITEMS    - Enables item dragging to rearrange position
    // OBS_XT_ANIMATION    - Enables animation while changing folder selection 
    // OBS_XT_SELHIGHLIGHT - Enables dimmed highlight of last pressed item 
    // OBS_XT_DEFAULT      - Same as OBS_XT_DRAGITEMS | OBS_XT_EDITGROUPS | OBS_XT_EDITITEMS | OBS_XT_REMOVEGROUPS | OBS_XT_REMOVEITEMS | OBS_XT_ADDGROUPS
    //[/pre]
    //
	virtual void ModifyFlag(
        // Specifies OBS_XT_ styles to be removed during style 
        // modification.
		const DWORD& dwRemove,
        // Specifies OBS_XT_ styles to be added during style 
        // modification.
		const DWORD& dwAdd,
		// TRUE to redraw the outlook bar.
		const UINT nRedraw = 0);

	// Call this member function to get the current style set for the outlook bar.
	virtual DWORD GetFlag() const;

	// Call this member function to set the size of the icons displayed in the 
	// Outlook bar control for the specified folder.
	virtual void SetSmallIconView(
		// TRUE to display small icons, or FALSE to display large icons.
		const BOOL bSet,
		// Index of the folder to set the icon size for.  If -1 all folder icons
		// are set.
		const int iFolder=-1);

	// Call this member function to return the current state of the icon display
	// for the outlook bar control.  Returns TRUE if small icons are displayed, and
	// FALSE if large icons are displayed.
	virtual BOOL IsSmallIconView(
		// Index of the folder to check; if -1 the currently selected folder is checked.
		const int iFolder=-1) const;

	// Call this member function to create the outlook bar control.  Returns TRUE if successful,
	// otherwise returns FALSE.
	virtual BOOL Create(
		// Style for the outlook bar, usually includes the WS_CHILD|WS_VISIBLE flags.
		DWORD dwStyle,
		// Size of the outlook bar.
		const RECT& rect,
		// Parent of the control.
		CWnd* pParentWnd,
		// Identifier of the outlook bar control.
		UINT nID,
		// Specifies the style flags for the control, see ModifyFlag for a list
		// of availiable styles.
		const DWORD dwFlag = OBS_XT_DEFAULT);

	// Call this member function to create the outlook bar control.  Returns TRUE if successful,
	// otherwise returns FALSE.
	virtual BOOL CreateEx(
		// Extended style for the outlook bar such as WS_EX_STATICEDGE, can be NULL.
		DWORD dwExStyle,
		// Style for the outlook bar, usually includes the WS_CHILD|WS_VISIBLE flags.
		DWORD dwStyle,
		// Size of the outlook bar.
		const RECT& rect,
		// Parent of the control.
		CWnd* pParentWnd,
		// Identifier of the outlook bar control.
		UINT nID,
		// Specifies the style flags for the control, see ModifyFlag for a list
		// of availiable styles.
		const DWORD dwFlag = OBS_XT_DEFAULT);

	// Call this member function to set the font used by the outlook bar control.
	virtual void SetFontX(
		// Points to the font to be used by the outlook bar.
		CFont* pFont);

	// Call this member function to retrieve the font used by the outlook bar control.
	// Returns a pointer to a CFont object.
	virtual CFont* GetFontX();

	// Call this member function to return a pointer to the CXTBarFolder data that
	// is associated with the specified folder.  Returns a pointer to a CXTBarFolder
	// object.
	virtual CXTBarFolder* GetBarFolder(
		// Index of the folder to retrieve.
		const int iFolder);

	// Call this member function to return a pointer to the CXTBarItem data that is
	// associated with the specified folder and item.  Returns a pointer to a 
	// CXTBarItem object.
	virtual CXTBarItem* GetBarFolderItem(
		// Index of the folder to retrieve.
		const int iFolder,
		// Index of the item to retrieve.
		const int iIndex);

	// Call this member function to set the background color for the outlook bar control.
	virtual void SetBackColor(
		// A RGB value that represents the background color.
		COLORREF clrBack);

	// Call this member function to set the text color for items in the outlook bar
	// control.
	virtual void SetTextColor(
		// A RGB value that represents the text item color.
		COLORREF clrText);

	// Call this member function to retrieve the size of the label for the specified
	// item.
	virtual void GetLabelRect(
		// Index of the folder where the item is located.
		const int iFolder,
		// Index of the item.
		const int iIndex,
		// Address of a CRect object that will receive the label size.
		CRect& rect);

	// Call this member function to retrieve the size of the icon for the specified
	// item.
	virtual void GetIconRect(
		// Index of the folder where the item is located.
		const int iFolder,
		// Index of the item.
		const int iIndex,
		// Address of a CRect object that will receive the icon size.
		CRect& rect);

	// Call this member function to retrieve the size of the client area for the
	// outlook bar.  This is the inside area that contains the folders.
	virtual void GetInsideRect(
		// Address of a CRect object that will receive the size.
		CRect& rect) const;

	// Call this member function to retrieve the size of the specified item.  The
	// size includes the area occupied by the items label and icon.
	virtual void GetItemRect(
		// Index of the folder where the item is located.
		const int iFolder,
		// Index of the item.
		const int iIndex,
		// Address of a CRect object that will receive the item size.
		CRect& rect);

	// Call this member function to retrieve the size of the specified folder.
	virtual BOOL GetFolderRect(
		// Index of the folder item.
		const int iIndex,
		// Address of a CRect object that will receive the folder size.
		CRect& rect) const;

	// This member function will return the index of the currently selected item
	// for the currently selected folder.
	//
    inline int GetCurSel()
	{
        return m_nItemHilighted;
    }

	// This member function will set the current selected item for the currently selected
	// folder.
    inline void SetCurSel(
		// Index of the item to select.
		int iItem,
		// true if the item is to be pressed when selected.
		const BOOL bPressed=false)
	{
        HighlightItem(iItem, bPressed);
    }

protected:

	//:Ignore
	virtual void DrawItem(CDC* pDC, const int iFolder, CRect rc, const int iIndex, const BOOL bOnlyImage = false);
	virtual void DrawDragArrow(CDC* pDC, const int iFrom, const int iTo);
	virtual void DrawAnimItem(const int iOffsetX, const int iOffsetY, const int iIndex);
	virtual void DrawFolder(CDC* pDC, const int iIndex, CRect rect, const FOLDER_HILIGHT eHilight);
    virtual void DrawIcon(CDC* pDC, int iIcon, int iFolder, bool bHilight);
	virtual void PaintItems(CDC* pDC, const int iFolder, CRect rc);
	virtual void GetVisibleRange(const int iFolder, int& iFirst, int& iLast);
	virtual int GetDragItemRect(const int iIndex, CRect& rect);
	virtual CSize GetItemSize(const int iFolder, const int iIndex, const int iType);
	virtual void AnimateFolderScroll(const int iFrom, const int iTo);
	virtual void HighlightItem(const int iIndex, const BOOL bPressed = false);
	virtual void HighlightFolder(const int iIndex);
	virtual int HitTestEx(const CPoint& point, int& iIndex);
	virtual BOOL IsValidItem(const int iIndex) const;
    //:End Ignore

	//:Ignore
    //{{AFX_VIRTUAL(CXTOutBarCtrl)
	//}}AFX_VIRTUAL
    //:End Ignore

	//:Ignore
    //{{AFX_MSG(CXTOutBarCtrl)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLargeIcon();
	afx_msg void OnUpdateLargeIcon(CCmdUI* pCmdUI);
	afx_msg void OnSmallIcon();
	afx_msg void OnUpdateSmallIcon(CCmdUI* pCmdUI);
	afx_msg void OnRemoveItem();
	afx_msg void OnUpdateRemoveItem(CCmdUI* pCmdUI);
	afx_msg void OnRenameItem();
	afx_msg void OnUpdateRenameItem(CCmdUI* pCmdUI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg long OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
    //:End Ignore
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CXTBarItem is a helper class used by the CXTOutBarCtrl to maintain information
// about each folder item specified in the outlook bar control.

class  CXTBarItem
{
public:

    // Constructs a CXTBarItem object.
	CXTBarItem(
		// A NULL terminated string that represents the item name.
        LPCTSTR lpszName,
		// An index into the folders image list.
        const int nImageIndex,
		// User item data (lParam).
        DWORD dwData);

	// Destroys a CXTBarItem object, handles cleanup and de-allocation.
    virtual ~CXTBarItem();

	// Call this member function to return the zero based index of the 
	// folder item.
	int GetIndex();

	// Call this member function to set the zero index of the folder
	// item.
	void SetIndex(
		// New index of the folder item.
		int iIndex);

	// Call this member function to return the user specified item data (lParam)
	// for the folder item.  Returns a DWORD value that represents the user data.
	DWORD GetData();

	// Call this member function to set user data (lParam) value for the 
	// folder item.
	void SetData(
		// Specifies the user data (lparam) value to be associated with the folder item.
		DWORD dwData);

	// Call this member function to return the label of the folder item.  Returns a 
	// CString value.
	CString GetName();

	// Call this member function to set the label of the folder item.
	void SetName(
		// A NULL terminated string that represents the item label.
		CString strName);

protected:

	//:Ignore
	int		m_nIndex;	// used internally
	DWORD	m_dwData;	// used internally
	CString	m_strName;	// used internally
    //:End Ignore

	friend class CXTOutBarCtrl;
};

/////////////////////////////////////////////////////////////////////////////
// CXTBarFolder s a helper class used by the CXTOutBarCtrl to maintain information
// about each folder specified in the outlook bar control.

class  CXTBarFolder
{
public:

    // Constructs a CXTBarFolder object.
	CXTBarFolder(
		// A NULL terminated string that represents the item name.
        LPCTSTR lpszName,
		// User item data (lParam).
        DWORD dwData);

	// Destroys a CXTBarFolder object, handles cleanup and de-allocation.
    virtual ~CXTBarFolder();

	// Call this member function to return the number of folders found
	// in the outlook bar.
	int GetItemCount();

	// Call this member function to insert an item into the outlook bar
	// folder.
	int	InsertItem(
		// Index of the item
		int iIndex,
		// A NULL terminated string that represents the label of the item
		LPCTSTR lpszName,
		// Index into the folders image list.
		const int nImage,
		// User item data (lParam)
		const DWORD dwData);

	// Call this member function to retrieve the specified item.  Returns a
	// pointer to a CXTBarItem object.
	CXTBarItem* GetItemAt(
		// Index of the item to retrieve.
		int iIndex);

	// Call this member function to insert a folder item into the location 
	// specified by iIndex.
	void InsertItemAt(
		// Zero based index of where to insert the new item.
		int iIndex,
		// Points to a valid CXTBarItem object.
		CXTBarItem* pBarItem);

	// Call this member fuction to remove the specified item from the folder.
	// Returns a pointer to the removed item.
	CXTBarItem* RemoveItemAt(
		// Index of the item to remove.
		int iIndex);

	// Call this member function to return the label of the folder item.  Returns
	// a CString object.
	CString GetName();

	// Call this member function to set the label for the folder item.
	void SetName(
		// A NULL terminated string that represents the folders new label.
		CString strName);

	// Call this member function to return the user item data (lParam) for the folder.
	// Returns a DWORD value that represents the item data.
	DWORD GetData();

	// Call this member function to set the user item data (lParam) for the folder.
	void SetData(
		// User item data (lParam)
		DWORD dwData);

	// Call this member fuction to return a pointer to the large image list for
	// the folder.  Returns a CImageList object if successful, otherwise NULL.
	CImageList* GetLargeImageList();

	// Call this member function to set the large image list for the folder.
	void SetLargeImageList(
		// Points to a CImageList object.
		CImageList* pLargeList);

	// Call this member fuction to return a pointer to the small image list for
	// the folder.  Returns a CImageList object if successful, otherwise NULL.
	CImageList* GetSmallImageList();

	// Call this member function to set the small image list for the folder.
	void SetSmallImageList(
		// Points to a CImageList object.
		CImageList* pSmallList);

	// Call this member function to return a CWnd pointer to the child object 
	// that is associated with this folder item.  Returns NULL if no objects was
	// found.
	CWnd* GetChild();

	// Call this member function to set the CWnd child to be associated with this
	// folder item.
	void SetChild(
		// Points to a valid CWnd object.
		CWnd* pChild);

protected:

	//:Ignore
    CString                         m_strName;		// used internally
    DWORD                           m_dwData;		// used internally
    CImageList*                     m_pLargeList;	// used internally
    CImageList*                     m_pSmallList;	// used internally
    CWnd*                           m_pChild;		// used internally
	BOOL							m_bSmallIcons;  // used internally
    CList<CXTBarItem*, CXTBarItem*> m_barItems;		// used internally
    //:End Ignore

	friend class CXTOutBarCtrl;
};

//////////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTOutBarCtrl::SetFontX(CFont* pFont) {
	ASSERT_VALID(pFont); LOGFONT lf; pFont->GetLogFont(&lf); m_font.DeleteObject(); m_font.CreateFontIndirect(&lf);
}
AFX_INLINE CFont* CXTOutBarCtrl::GetFontX() {
	return &m_font;
}
AFX_INLINE void CXTOutBarCtrl::SetAnimationTickCount(const long lValue) {
	m_nAnimationTickCount = lValue;
}
AFX_INLINE int CXTOutBarCtrl::GetAnimationTickCount() {
	return m_nAnimationTickCount;
}
AFX_INLINE CXTBarFolder* CXTOutBarCtrl::GetBarFolder(const int iFolder) {
	return (CXTBarFolder*)m_arFolder.GetAt(iFolder);
}
AFX_INLINE CXTBarItem* CXTOutBarCtrl::GetBarFolderItem(const int iFolder, const int iIndex) {
	return GetBarFolder(iFolder)->GetItemAt(iIndex);
}
AFX_INLINE void CXTOutBarCtrl::SetBackColor(COLORREF clrBack) {
    m_bUserClrBack = TRUE;
	m_clrBack = clrBack;
}
AFX_INLINE void CXTOutBarCtrl::SetTextColor(COLORREF clrText) {
    m_bUserClrText = TRUE;
	m_clrText = clrText;
}

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTBarItem::GetIndex() {
	return m_nIndex;
}
AFX_INLINE void CXTBarItem::SetIndex(int iIndex) {
	m_nIndex = iIndex;
}
AFX_INLINE DWORD CXTBarItem::GetData() {
	return m_dwData;
}
AFX_INLINE void CXTBarItem::SetData(DWORD dwData) {
	m_dwData = dwData;
}
AFX_INLINE CString CXTBarItem::GetName() {
	return m_strName;
}
AFX_INLINE void CXTBarItem::SetName(CString strName) {
	m_strName = strName;
}

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE int CXTBarFolder::GetItemCount() {
	return (int)m_barItems.GetCount();
}
AFX_INLINE CString CXTBarFolder::GetName() {
	return m_strName;
}
AFX_INLINE void CXTBarFolder::SetName(CString strName) {
	m_strName = strName;
}
AFX_INLINE DWORD CXTBarFolder::GetData() {
	return m_dwData;
}
AFX_INLINE void CXTBarFolder::SetData(DWORD dwData) {
	m_dwData = dwData;
}
AFX_INLINE CImageList* CXTBarFolder::GetLargeImageList() {
	return m_pLargeList;
}
AFX_INLINE void CXTBarFolder::SetLargeImageList(CImageList* pLargeList) {
	m_pLargeList = pLargeList;
}
AFX_INLINE CImageList* CXTBarFolder::GetSmallImageList() {
	return m_pSmallList;
}
AFX_INLINE void CXTBarFolder::SetSmallImageList(CImageList* pSmallList) {
	m_pSmallList = pSmallList;
}
AFX_INLINE CWnd* CXTBarFolder::GetChild() {
	return m_pChild;
}
AFX_INLINE void CXTBarFolder::SetChild(CWnd* pChild) {
	m_pChild = pChild;
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // #if !defined(__XTOUTBARCTRL_H__)
