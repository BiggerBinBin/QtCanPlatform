#include "LogTrc.h"
#include <Qtconcurrent>
#include <QFile>
#include <QTextStream>
#include <QsLog.h>
LogTrc::LogTrc(LogBase* parent):LogBase(parent)
{

}
bool LogTrc::loadLogFile(const QString& file, const QStringList& fifter, messageMange* LogData)
{

	m_sAscName = file;
	m_fifter = fifter;
	m_RunState = true;
	QtConcurrent::run(this, &LogTrc::readTrc, LogData);
	m_isReadFile = true;
	return true;
}

void LogTrc::readTrc(messageMange* LogData)
{
	QFile fp(m_sAscName);
	if (!fp.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		emit sigLoadState(-1);
		return;
	}
	QTextStream in(&fp);
	//�ȶ�ȡ���У��鿴�汾��Ϣ
	QString line = in.readLine();
	if (line.isEmpty())
	{
		emit sigLoadState(-1);
		QLOG_WARN() << "trc file is empty";
		return;
	}
	if (line.at(0) != ";")
	{
		emit sigLoadState(-1);
		QLOG_WARN() << "trc file is damage";
		return;
	}
	line = line.remove(0, 2);
	QStringList list = line.split("=");
	//Ĭ�ϰ汾��1.0
	int ver = 10;
	//1.1��ʼ�а汾��¼��
	if (list.size() == 2)
	{
		ver = list.at(1).toFloat() * 10;
	}
	QStringList lineSplit;
	double firstTime = -1;
	while (!in.atEnd())
	{
		line = in.readLine();
		if (line.isEmpty() || line.at(0) == ";")continue;
		QStringList temp  = line.split(" ");
		lineSplit.clear();
		//ȥ���ո�
		for (auto s : temp)
		{
			if (s != " " && s != "")
			{
				lineSplit.push_back(s);
			}
		}
		if (lineSplit.size() < 5)
			continue;
		messageData msg;
		memset(msg.Data, 0xff, 8);
		if (firstTime == -1)
		{
			msg.TimeStemp = 0;
			firstTime = lineSplit.at(1).toDouble();
		}
		else
		{
			msg.TimeStemp = fabs(lineSplit.at(1).toDouble() - firstTime);
			firstTime = lineSplit.at(1).toDouble();
		}
		if (ver == 11)
		{
			if (lineSplit.at(2) != "Rx" && lineSplit.at(2) != "Tx")
				continue;
			uint id = lineSplit.at(3).toUInt(nullptr,16);
			//���ͺŵ�ID��Ҫ�������Ĳ�Ҫ
			for (auto sid : m_fifter)
			{
				if (sid.toUInt(nullptr, 16) != id)
					continue;
				msg.Id = id;
				//��5���ǳ���
				int len = lineSplit.at(4).toInt();
				//>8����Ϣ������FD��ʽ�ģ����ﲻ����
				if (len > 8)
					continue;
				for (int m = 5; m < 5 + len; m++)
				{
					msg.Data[m - 5] = lineSplit.at(m).toUShort(nullptr, 16);
				}
				break;
			}

		}
		else if (ver == 12)
		{
			if (lineSplit.at(3) != "Rx" && lineSplit.at(3) != "Tx")
				continue;
			uint id = lineSplit.at(4).toUInt(nullptr, 16);
			//���ͺŵ�ID��Ҫ�������Ĳ�Ҫ
			for (auto sid : m_fifter)
			{
				if (sid.toUInt(nullptr, 16) != id)
					continue;
				msg.Id = id;
				//��6���ǳ���
				int len = lineSplit.at(5).toInt();
				//>8����Ϣ������FD��ʽ�ģ����ﲻ����
				if (len > 8)
					continue;
				for (int m = 6; m < 6 + len; m++)
				{
					msg.Data[m - 6] = lineSplit.at(m).toUShort(nullptr, 16);
				}
				break;
			}
		}
		else if (ver == 13)
		{
			if (lineSplit.at(3) != "Rx" && lineSplit.at(3) != "Tx")
				continue;
			uint id = lineSplit.at(4).toUInt(nullptr, 16);
			//���ͺŵ�ID��Ҫ�������Ĳ�Ҫ
			for (auto sid : m_fifter)
			{
				if (sid.toUInt(nullptr, 16) != id)
					continue;
				msg.Id = id;
				//��7���ǳ���
				int len = lineSplit.at(6).toInt();
				//>8����Ϣ������FD��ʽ�ģ����ﲻ����
				if (len > 8)
					continue;
				if (lineSplit.at(7) == "RTR")
					break;
				for (int m = 7; m < 7 + len; m++)
				{
					msg.Data[m - 7] = lineSplit.at(m).toUShort(nullptr, 16);
				}
				break;
			}
		}
		else if (ver == 20)
		{
			if (lineSplit.at(4) != "Rx" && lineSplit.at(4) != "Tx")
				continue;
			if (lineSplit.at(2) != "DT")
				continue;
			uint id = lineSplit.at(3).toUInt(nullptr, 16);
			//���ͺŵ�ID��Ҫ�������Ĳ�Ҫ
			for (auto sid : m_fifter)
			{
				if (sid.toUInt(nullptr, 16) != id)
					continue;
				msg.Id = id;
				//��6���ǳ���
				int len = lineSplit.at(5).toInt();
				//>8����Ϣ������FD��ʽ�ģ����ﲻ����
				if (len > 8)
					continue;
				for (int m = 6; m < 6 + len; m++)
				{
					msg.Data[m - 6] = lineSplit.at(m).toUShort(nullptr, 16);
				}
				break;
			}
		}
		else if (ver == 10)
		{
			
			uint id = lineSplit.at(2).toUInt(nullptr, 16);
			//���ͺŵ�ID��Ҫ�������Ĳ�Ҫ
			for (auto sid : m_fifter)
			{
				if (sid.toUInt(nullptr, 16) != id)
					continue;
				if (lineSplit.at(4) == "RTR")
					break;
				msg.Id = id;
				//��6���ǳ���
				int len = lineSplit.at(3).toInt();
				//>8����Ϣ������FD��ʽ�ģ����ﲻ����
				if (len > 8)
					continue;
				for (int m = 4; m < 4 + len; m++)
				{
					msg.Data[m - 4] = lineSplit.at(m).toUShort(nullptr, 16);
				}
				break;
			}
		}
		else
		{
			continue;
		}
		LogData->vData.push_back(msg);
		LogData->iLen++;
		
	}
	emit sigLoadState(0);
}
