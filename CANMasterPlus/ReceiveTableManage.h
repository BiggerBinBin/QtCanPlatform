#pragma once

#include <QObject>
#include <QTableWidget>
#include <QColor>
#include <QReadWriteLock>
#include <QMap>
#include <QDateTime>
#include "../../CANMasterPlus/ModelDataManage/Unities/qGboleData.h"
#include "TP700TemperatureModel.h"
class DataSave;
class ReceiveTableManage  : public QObject
{
	Q_OBJECT

public:
	ReceiveTableManage(QObject *parent);
	~ReceiveTableManage();
	QTableWidget* getTableWidget()
	{
		return tableWidget;

	}
	void setTeamperaturePointer(TP700TemperatureModel* p)
	{
		m_GetTempPointer = p;
	}
	void transferMsg(int ch, unsigned int fream_id, QByteArray data);
	void saveCanData();
	void clearData();
private:
	QTableWidget* tableWidget = nullptr;
	int currentModelIndex = -1;
	//保存当前接到的数据
	std::vector<showTableData>showTableVec;
	//接收信息
	std::vector<canIdData>recCanData;
	DataSave* saveData = nullptr;
	//保存excel的表头
	QString m_excelTitle;
	QStringList m_dataSaveList;
	uint m_cur_count_item = 0;
	//背景色
	QColor recBackgroudColor = QColor(10, 50, 122);
	//字体颜色
	QColor recFontColor = QColor(255, 250, 255);
	//界面显示的列数
	int tableColumn = 11;
	
	protoData m_CurrentModel;

	QReadWriteLock m_readwrite_lock;

	QMap<uint32_t, uint64_t>m_msg_gap;

	//配置文件的路径
	QString iniPath;
	//自动保存数据的条数
	int m_iAutoSaveNum = 3000;
	//是否自动保存
	int m_bIsAutoSave = 1;
	//uint64_t lasttime = 0;
	QDateTime lastTime;

	QTimer* m_TimerShow = nullptr;
	QTimer* m_TimerLoss = nullptr;

	TP700TemperatureModel* m_GetTempPointer = nullptr;
private:
	bool recDataIntoTab();
	
	unsigned char checksumXOR(const uchar data[]);
	//void recAnalyseIntel(int ch, unsigned int fream_id, QByteArray data);
	void msgParser(int ch, unsigned int fream_id, QByteArray data);
	bool readIni(QString path);
public Q_SLOTS:
	void on_currentModelIndex_Changed(int index);
Q_SIGNALS:
	void sigNewMsg();
	void on_sigClearUIData();
private Q_SLOTS:
	bool onNewMsgIntoTable();
	void onStopShow();
};
