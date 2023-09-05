#include "HardWareTcan.h"

HardWareTcan::HardWareTcan(QObject *parent)
	: QThread(parent)
{
    m_curHandle = 0x00;
}

HardWareTcan::~HardWareTcan()
{
    CloseHardWare();
}

bool HardWareTcan::OpenHardWare(QString dev, int bundrate, int attach)
{
	int index = dev.toInt();
	if (index / 2 > m_hardNum)
		return false;
	bool state = USB_OpenDevice(m_hardHandle[index / 2]);
	if(!state)
		return false;
    //初始化配置CAN
    CAN_INIT_CONFIG CANConfig;

    CANConfig.CAN_Mode = 0;//正常模式

    CANConfig.CAN_ABOM = 0;//禁止自动离线
    CANConfig.CAN_NART = 1;//禁止报文重传
    CANConfig.CAN_RFLM = 0;//FIFO满之后覆盖旧报文
    CANConfig.CAN_TXFP = 1;//发送请求决定发送顺序
    //配置波特率,波特率 = 42M/(BRP*(SJW+BS1+BS2)),CAN_BRP:500-4,250-8,125-16
    
    CANConfig.CAN_BRP = 2000/bundrate;
    CANConfig.CAN_BS1 = 16;
    CANConfig.CAN_BS2 = 4;
    CANConfig.CAN_SJW = 1;
    state = CAN_Init(m_hardHandle[index / 2], index % 2, &CANConfig);
    if (state != CAN_SUCCESS) {
        
        return false;
    }
    struct _CAN_FILTER_CONFIG CANFilter;
    CANFilter.Enable = 1;
    CANFilter.ExtFrame = 0;
    CANFilter.FilterIndex = 0;
    CANFilter.FilterMode = 0;
    CANFilter.MASK_IDE = 0;
    CANFilter.MASK_RTR = 0;
    CANFilter.MASK_Std_Ext = 0;
    CAN_Filter_Init(m_hardHandle[index / 2], index % 2, &CANFilter);
    m_curHandle = m_hardHandle[index / 2];
    m_curIndex = index % 2;
    CAN_StartGetMsg(m_curHandle, m_curIndex);
    runGetMessage = true;
    this->start();
    return true;
}

bool HardWareTcan::SendMessage(uint id, uchar data[], int resever[])
{
    struct _CAN_MSG CanMsg;
    CanMsg.DataLen = 8;
    CanMsg.ID = id;
    CanMsg.RemoteFlag = 0;
    CanMsg.ExternFlag = (id > 0x7ff ? 1 : 0);
    memcpy(CanMsg.Data, data, 8);
    int ret = CAN_SendMsg(m_curHandle, m_curIndex, &CanMsg, 1);
    return ret == CAN_SUCCESS;
	//return false;
}

QStringList HardWareTcan::GetHardWare()
{
	int ret = USB_ScanDevice(m_hardHandle);
	if(ret<=0)
		return QStringList();
	m_hardNum = ret;
	QStringList tcan_list;
	for (int i = 0; i < ret; i++)
	{
      /*  HARDWARE_INFO h_info;
        DEV_GetHardwareInfo(m_hardHandle[i], &h_info);
        for(int j=0;j<h_info.CANChannelNum;j++)*/
        {
            tcan_list.append("TCAN-" + QString::number(i));
           
        }
	}
	return tcan_list;
}
void HardWareTcan::run()
{
    CAN_MSG CanMsgBuffer[10240];
    while (runGetMessage)
    {
        int num = CAN_GetMsg(m_curHandle, m_curIndex, CanMsgBuffer);
        static QByteArray bdata;
        bdata.clear();
        if (num > 0)
        {
            for (int i = 0; i < CanMsgBuffer[0].DataLen; i++)
                bdata.append(CanMsgBuffer[0].Data[i]);
            emit getProtocolData(m_curIndex, CanMsgBuffer[0].ID, bdata);
        }
        QThread::msleep(5);
    }
   
}
bool HardWareTcan::CloseHardWare()
{
    runGetMessage = false;
    if(m_curHandle)
        USB_CloseDevice(m_curHandle);
	return true;
}
