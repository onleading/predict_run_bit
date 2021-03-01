#ifndef U_BASECTRL_H_
#define U_BASECTRL_H_

#include "UBase.h"

class UBaseCtrl  
{
public:
	UBaseCtrl(LPCOMINFO pComInfo, char *ModuleCode="");
	virtual ~UBaseCtrl();

	virtual void	ExitUnit();													//退出前的清理
	virtual BOOL	ShowUnit(HWND hVessel);										//初始化显示组件
	virtual void	ResizeUnit(int cx, int cy);									//改变组件窗口大小
	virtual void	GetDataAck(WPARAM wParam, LPARAM lParam);					//数据返回处理
	virtual void	SetStock(MemStkInfo *pStock);								//股票数据联动
	virtual int		ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam);	
	virtual int		BroadcastUnitMsg(UINT message, WPARAM wParam, LPARAM lParam);//发布消息,通过ctrl中转发布到其他unit

private:
	COMINFO			m_ComInfo;
	IGNPLUGIN		*m_pModule;
	
	int				m_nWhichUnit;
};

#endif
