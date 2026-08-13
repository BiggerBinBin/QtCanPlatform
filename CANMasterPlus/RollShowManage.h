#pragma once

#include <QObject>
#include <QVBoxLayout>
#include <QMutex>
#include <QReadWriteLock>
#include <QMap>
#include "../ModelDataManage/Unities/dataStruct.h"
#include "QLogPlot.h"
#include "QSignalManage.h"
#include "TP700TemperatureModel.h"
class QTableWidget;
class QTabWidget;
class QTableWidget;
class QLabel;
class RollShowManage  : public QObject
{
	Q_OBJECT

public:
	RollShowManage(QObject *parent);
	~RollShowManage();
	QTabWidget* getRollShowLayout() { return tabWidget; }
	void setTeamperaturePointer(TP700TemperatureModel* p)
	{
		m_GetTempPointer = p;
	}
	void transferMsg(int ch, unsigned int fream_id, QByteArray data);
	void clearData();
private:
	QSignalManage* plotSignalMn = nullptr;
	QLogPlot* m_plot_dw_ptr = nullptr;
	QWidget* m_plot_ptr = nullptr;
	QVBoxLayout* layout = nullptr;
	QTabWidget* tabWidget = nullptr;
	QTableWidget* rollTableWidget = nullptr;
	QTableWidget* titleRollTableWidget = nullptr;
	protoData m_currentModel;
	protoData m_currentModel_Plot;
	int m_currModelIndex;											//当前型号下标
	QStringList titleRoll;
	//滚动显示的数据
	std::vector<RollStruct>RollShowData;
	QMutex m_mutex;
	QReadWriteLock m_read_write_lock;
	QMap<uint32_t, uint64_t>m_msg_gap;
	uint32_t time_index = 0;

	int m_iAutoSaveNum = 3000;
	int m_bIsAutoSave = 1;
	QString iniPath;

	QTimer* m_TimerShow = nullptr;
	QTimer* m_TimerLoss = nullptr;

	TP700TemperatureModel* m_GetTempPointer = nullptr;
private:
	void initTable();
	void titleRest();
	unsigned char checksumXOR(const uchar data[], int lenght);
	bool readIni(QString path);
Q_SIGNALS:
	void sigRollMes();
	void sigNewMessageToGraph(unsigned int fream_id, QByteArray byte, int dx);
public Q_SLOTS:
	void on_currentModelIndex_Changed(int index);
	void on_slotClearData();

private Q_SLOTS:
	void msgParser(int ch, unsigned int fream_id, QByteArray data);
	void onNewRollMes();
	void on_m_singal_mg_clicked();
	void on_showWidget();
	void onStopShow();
	void doTimerOperator();
};
