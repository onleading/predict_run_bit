#ifndef _BASEWIN_H
#define _BASEWIN_H

#define MENU_DRAW_PUTBITMAP  0
#define MENU_DRAW_ALL        1

class BaseWin : public CRect
{
protected:
	CWnd	* m_pView;
public:
	BaseWin();
	virtual ~BaseWin();
	void	 SetTitle(CString str);
	//Биаыжиди
	virtual int Handle(UINT message, WPARAM wParam, LPARAM lParam = 0);
};

#endif