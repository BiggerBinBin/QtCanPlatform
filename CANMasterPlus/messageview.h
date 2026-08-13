#ifndef MESSAGEVIEW_H
#define MESSAGEVIEW_H

#include <QObject>
#include <QTabWidget>
#include <QSharedPointer>
#include <QMap>
#include <unordered_map>
class QTableWidget;
namespace _messageview_
{
    const uchar MaxLen = 16;
    typedef struct recmessage
    {
        uchar data[MaxLen];
        uchar leng;
        uchar dir;
        uint32_t count;
        uint32_t timecycle;
    }RecMessage;
};
class MessageView : public QObject
{
    Q_OBJECT
public:
    MessageView(QObject* parent=nullptr);
    QTabWidget* getTabWidget(){return tabWidget;}
    /*
    * @brief: add new msg to map, and show in right tablewidget
    * @parameters:
    *           uint32_t id: msg identify
    *           QByteArray data: msg data
    *           uchar dir: msg direction,0 Receive,1 Transmit
    */
    void newMsgReceive(uint32_t id, QByteArray data,uchar dir);
    void newMsgReceive(uint32_t id, uchar data[],int len, uchar dir);
    ~MessageView();
    void clear();
private:
    QTabWidget* tabWidget=nullptr;
    QTableWidget* m_recTableOW;
    std::unordered_map<uint32_t, _messageview_::RecMessage>m_recMsg;
    QMap<uint32_t, uint64_t>m_msg_gap;
private:
    void initWidget();
    void updata();
private Q_SLOTS:
    void on_clearCountMessage_clicked();
};

#endif // MESSAGEVIEW_H
