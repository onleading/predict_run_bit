#ifndef U_BASECTRL_H_
#define U_BASECTRL_H_

#include "UBase.h"

class UBaseCtrl  
{
public:
	UBaseCtrl(LPCOMINFO pComInfo, char *ModuleCode="");
	virtual ~UBaseCtrl();

	virtual void	ExitUnit();													//�˳�ǰ������
	virtual BOOL	ShowUnit(HWND hVessel);										//��ʼ����ʾ���
	virtual void	ResizeUnit(int cx, int cy);									//�ı�������ڴ�С
	virtual void	GetDataAck(WPARAM wParam, LPARAM lParam);					//���ݷ��ش���
	virtual void	SetStock(MemStkInfo *pStock);								//��Ʊ��������
	virtual int		ProcessMsg(UINT message, WPARAM wParam, LPARAM lParam);	
	virtual int		BroadcastUnitMsg(UINT message, WPARAM wParam, LPARAM lParam);//������Ϣ,ͨ��ctrl��ת����������unit

private:
	COMINFO			m_ComInfo;
	IGNPLUGIN		*m_pModule;
	
	int				m_nWhichUnit;
};

#endif
