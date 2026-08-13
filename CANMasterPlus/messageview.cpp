#include "messageview.h"
#include <QTableWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QByteArray>
#include <QPushButton>
#include <QToolButton>
#include <chrono>
#include <QDateTime>
#include <QDebug>
#include <qthread.h>
QStringList strRecMsgHead = { "ID","Data","Len","Cycle","Count","Dir"};
MessageView::MessageView(QObject*parent):QObject(parent)
{
    initWidget();
    
}

void MessageView::newMsgReceive(uint32_t id, QByteArray data, uchar dir)
{
    auto it2 = m_msg_gap.find(id);
    static int grp_t;
    grp_t = 0;
    if (it2 != m_msg_gap.end())
    {
        grp_t = QDateTime::currentMSecsSinceEpoch() - it2.value();
        m_msg_gap[id] = QDateTime::currentMSecsSinceEpoch();
    }
    else
    {
        m_msg_gap[id] = QDateTime::currentMSecsSinceEpoch();
    }

    //static std::chrono::time_point<std::chrono::steady_clock>cur_time = std::chrono::high_resolution_clock::now();

    //static std::chrono::time_point<std::chrono::steady_clock>cur_time = std::chrono::high_resolution_clock::now();
   
    auto it = m_recMsg.find(id);
    //之前已经有了
    if (it != m_recMsg.end())
    {
        it->second.leng = data.size();
        ////获取当前时间
        //auto _now_ = std::chrono::high_resolution_clock::now();
        ////计算时间差
        //auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(_now_ - cur_time);
        ////更新
        //cur_time = _now_;
        //周期(ms)
        it->second.timecycle = grp_t;
        //计数+1
        it->second.count++;
        //方向，按道理这里不用
        it->second.dir = dir;
        //更新数据
        for (int i = 0; i < data.size() && i< _messageview_::MaxLen; i++)
        {
            it->second.data[i] = (uchar)data[i];
        }
    }
    else
    {
        //cur_time = std::chrono::high_resolution_clock::now();

        _messageview_::RecMessage msg;
        //计数
        msg.count = 1;
        //第一条消息周期为0
        msg.timecycle = 0;
        //方向，按道理这里不用
        msg.dir = dir;
        //长度
        msg.leng = data.size();

        //插入数据
        for (int i = 0; i < data.size() && i < _messageview_::MaxLen; i++)
        {
            msg.data[i] = (uchar)data[i];
        }
        m_recMsg.insert(std::make_pair(id, msg));
    }
    this->updata();
    QThread::msleep(0);
}

void MessageView::newMsgReceive(uint32_t id, uchar data[], int len, uchar dir)
{
    
    QByteArray bydata;
    bydata = QByteArray::fromRawData((char*)data, len);
    //bydata.resize(len);
   /* for (int i = 0; i < len; i++)
    {
        bydata.append(data[i]);

    }*/
    newMsgReceive(id, bydata, dir);
}

MessageView::~MessageView()
{
    /*if (tabWidget)
    {
        delete tabWidget;
        tabWidget = nullptr;
    }*/
}

void MessageView::initWidget()
{
    tabWidget = new QTabWidget;
    tabWidget->addTab(new QWidget(),QIcon(),QString(tr("Receive/Transmit")));
    tabWidget->addTab(new QWidget(),QIcon(),QString(tr("Detailed")));
    QVBoxLayout* recmsglyaout = new QVBoxLayout();
    QToolButton* c_btu = new QToolButton();
    c_btu->setText(tr("clear"));
    connect(c_btu, &QToolButton::clicked, this, &MessageView::on_clearCountMessage_clicked);
    m_recTableOW = new QTableWidget();
    m_recTableOW->setColumnCount(strRecMsgHead.size());
    m_recTableOW->setHorizontalHeaderLabels(strRecMsgHead);
    recmsglyaout->addWidget(c_btu);
    recmsglyaout->addWidget(m_recTableOW);
    m_recTableOW->setColumnWidth(0, 104);
    m_recTableOW->setColumnWidth(1, 190);
    m_recTableOW->setColumnWidth(2, 40);
    m_recTableOW->setColumnWidth(3, 50);
    m_recTableOW->setColumnWidth(4, 50);
    m_recTableOW->setColumnWidth(5, 50);
    tabWidget->widget(0)->setLayout(recmsglyaout);
    tabWidget->setMinimumWidth(500);
}
//更新数据显示
void MessageView::updata()
{
    
    if (!m_recTableOW)return;
    int row = m_recTableOW->rowCount();
    if (row < m_recMsg.size())
    {
        m_recTableOW->setRowCount(m_recMsg.size());
    }
    row = m_recTableOW->rowCount();
    //迭代器访问
    auto m_it = m_recMsg.begin();
    for (int i = 0; i < row && m_it!= m_recMsg.end(); i++)
    {
        //id
        auto it = m_recTableOW->item(i, 0);
        if (!it)
        {
            m_recTableOW->setItem(i, 0, new QTableWidgetItem("0x" + QString::number(m_it->first, 16).toUpper()));
        }
        else
        {
            it->setText("0x" + QString::number(m_it->first, 16).toUpper());
        }
            

        //data
        QString str;
        for (int m = 0; m < m_it->second.leng; m++)
        {
            str += QString("%1 ").arg(m_it->second.data[m],2,16,QChar('0'));
        }
        it = m_recTableOW->item(i, 1);
        if (!it)
        {
            m_recTableOW->setItem(i, 1, new QTableWidgetItem(str.toUpper()));
        }
        else
        {
            it->setText(str.toUpper());
        }
        

        //lenght
        it = m_recTableOW->item(i, 2);
        if (!it)
        {
            m_recTableOW->setItem(i, 2, new QTableWidgetItem(QString::number(m_it->second.leng)));
        }
        else
        {
            //qDebug() << "id:"<< m_it->first << ",leng:" << m_it->second.leng;
            it->setText(QString::number(m_it->second.leng));
        }
        

        //direction
        it = m_recTableOW->item(i, 5);
        if (!it)
        {
            m_recTableOW->setItem(i, 5, new QTableWidgetItem(QString(m_it->second.dir == 0 ? "Rx" : "Tx")));
        }
        else
        {
            it->setText(m_it->second.dir == 0 ? "Rx" : "Tx");
        }
        
        it = m_recTableOW->item(i, 4);
        if (!it)
        {
            m_recTableOW->setItem(i, 4, new QTableWidgetItem(QString::number(m_it->second.count)));
        }
        else
        {
            it->setText(QString::number(m_it->second.count));
        }
        

        it = m_recTableOW->item(i, 3);
        if (!it)
        {
            m_recTableOW->setItem(i, 3, new QTableWidgetItem(QString::number(m_it->second.timecycle)));
        }
        else
        {
            it->setText(QString::number(m_it->second.timecycle));
        }
        

        m_it++;
    }
    //m_recTableOW->update();
}

void MessageView::clear()
{
    m_recMsg.clear();
    
    if (!m_recTableOW)return;
    int row = m_recTableOW->rowCount();
    for (int i = 0; i < row; i++)
    {
        m_recTableOW->removeRow(row - i - 1);
    }
}
void MessageView::on_clearCountMessage_clicked()
{
    this->clear();
}
