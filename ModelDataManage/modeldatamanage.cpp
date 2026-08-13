 
#include <qtreeview.h>
#include <qtablewidget.h>
#include <QStandardItemModel>
#include <QStandardItem>
#include <qtreewidget.h>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <qdebug.h>
#include <qresource.h>
#include <QComboBox>
#include <QCheckBox>
#include <qspinbox.h>
#include <QMessageBox>
#include <QSplitter>
#include "modeldatamanage.h"
#include "ui_modeldatamanage.h"
#include "Unities/qGboleData.h"
#include "QSetProperty.h"
static QStringList modelHeaderName = { QStringLiteral("协议"),QStringLiteral("波特率"),QStringLiteral("发送周期"),QStringLiteral("平台"),QStringLiteral("标准帧?"),QStringLiteral("LIN?")};
static QStringList canidHeaderName = { QStringLiteral("消息方向"),QStringLiteral("报文长度"),QStringLiteral("自定义周期")};
static QStringList buadRate = { "125k","250k","500k","800k","2400b","9600b","10400b","19200b"};
static uint32_t buadNumber[] = { 200000,250000,500000,800000,2400,9600,10400,19200 };
static QStringList platform = {"1kW","2kW", "3kW", "4kW", "5kW", "6kW", "7kW", "8kW", "10kW", "15kW", "19kW", "20kW", "25kW"};
static QStringList frameDataHeaderName = { QStringLiteral("信号名"),QStringLiteral("起始位"),QStringLiteral("长度"),QStringLiteral("精度"), QStringLiteral("偏移量"),QStringLiteral("信号值"),QStringLiteral("滚动显示"),QStringLiteral("数据来源"),QStringLiteral("Hex?") };
static QString buttonStyle = "QPushButton { min-width: 0; max-width: 16777215; }";
ModelDataManage::ModelDataManage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ModelDataManage)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    initUI();
    initModelData();
    this->setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "\\app-logo.ico"));
}

ModelDataManage::~ModelDataManage()
{
    delete ui;
    if (nullptr != m_modelView_ptr)
    {
        delete m_modelView_ptr; m_modelView_ptr = nullptr;
    }
}

void ModelDataManage::initUI()
{
    //==================== gui left side ====================
    m_addModel_ptr = new QPushButton(this);
    m_addModel_ptr->setText(tr("addModel"));
    m_addIDModel_ptr = new QPushButton(this);
    m_addIDModel_ptr->setText(tr("addIDModel"));
    m_copyModel_ptr = new QPushButton(this);
    m_copyModel_ptr->setText(tr("copy"));
    m_copyModel_ptr->setStyleSheet(buttonStyle);
    
    m_delModel_ptr = new QPushButton(this);
    m_delModel_ptr->setText(tr("del"));
    m_delModel_ptr->setStyleSheet(buttonStyle);
    m_upModel_ptr = new QPushButton(this);
    m_upModel_ptr->setText(tr("up"));
    m_upModel_ptr->setStyleSheet(buttonStyle);
    m_downModel_ptr = new QPushButton(this);
    m_downModel_ptr->setText(tr("down"));
    m_downModel_ptr->setStyleSheet(buttonStyle);
    m_saveModel_ptr = new QPushButton(this);
    m_saveModel_ptr->setText(tr("save"));
    m_saveModel_ptr->setStyleSheet(buttonStyle);

    connect(m_addModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_addModel_ptr_clicked);
    connect(m_addIDModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_addIDModel_ptr_clicked);
    connect(m_copyModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_copyModel_ptr_clicked);
    connect(m_delModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_delModel_ptr_clicked);
    connect(m_upModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_upModel_ptr_clicked);
    connect(m_downModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_downModel_ptr_clicked);
    connect(m_saveModel_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_saveModel_ptr_clicked);

    QHBoxLayout* buttonLayout_1 = new QHBoxLayout();
    buttonLayout_1->addWidget(m_addModel_ptr);
    buttonLayout_1->addWidget(m_addIDModel_ptr);
    buttonLayout_1->addWidget(m_copyModel_ptr);
    buttonLayout_1->addWidget(m_delModel_ptr);
    buttonLayout_1->addWidget(m_upModel_ptr);
    buttonLayout_1->addWidget(m_downModel_ptr);
    buttonLayout_1->addWidget(m_saveModel_ptr);
    //buttonLayout_1->addSpacerItem(new QSpacerItem(100, 50, QSizePolicy::MinimumExpanding));
    //buttonLayout_1->setContentsMargins(1, 0, 1, 0);
    buttonLayout_1->setSpacing(0);
    m_modelView_ptr = new QTreeView();
    connect(m_modelView_ptr, &QTreeView::clicked, this, &ModelDataManage::on_modelView_clicked);
   
    m_modelParamView_ptr = new QTableWidget();
    QVBoxLayout* modelviewLaout = new QVBoxLayout();
    modelviewLaout->addWidget(m_modelView_ptr);
    modelviewLaout->addWidget(m_modelParamView_ptr);
    modelviewLaout->setStretch(0, 5);
    modelviewLaout->setStretch(1, 1);
    connect(m_modelParamView_ptr, &QTableWidget::cellChanged, this, &ModelDataManage::on_modelParamCellChanged);
    QVBoxLayout* leftLayout = new QVBoxLayout();
    leftLayout->addLayout(buttonLayout_1);
    leftLayout->addLayout(modelviewLaout);
    leftLayout->setStretch(0, 1);
    leftLayout->setStretch(1, 10);

    //=================gui right side ===========================
    m_addSignal_ptr = new QPushButton(this);
    m_addSignal_ptr->setText(tr("addItem"));
    m_copySignal_ptr = new QPushButton(this);
    m_copySignal_ptr->setText(tr("copyItem"));
    m_delSignal_ptr = new QPushButton(this);
    m_delSignal_ptr->setText(tr("delItem"));
    m_upSignal_ptr = new QPushButton(this);
    m_upSignal_ptr->setText(tr("upItem"));
    m_downSignal_ptr = new QPushButton(this);
    m_downSignal_ptr->setText(tr("downItem"));
    m_saveSignal_ptr = new QPushButton(this);
    m_saveSignal_ptr->setText(tr("saveItem"));

    connect(m_addSignal_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_addSignal_ptr_clicked);
    connect(m_copySignal_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_copySignal_ptr_clicked);
    connect(m_delSignal_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_delSignal_ptr_clicked);
    connect(m_upSignal_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_upSignal_ptr_clicked);
    connect(m_downSignal_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_downSignal_ptr_clicked);
    connect(m_saveSignal_ptr, &QPushButton::clicked, this, &ModelDataManage::on_m_saveSignal_ptr_clicked);

    QHBoxLayout* buttonLayout_2 = new QHBoxLayout();
    buttonLayout_2->addWidget(m_addSignal_ptr);
    buttonLayout_2->addWidget(m_copySignal_ptr);
    buttonLayout_2->addWidget(m_delSignal_ptr);
    buttonLayout_2->addWidget(m_upSignal_ptr);
    buttonLayout_2->addWidget(m_downSignal_ptr);
    buttonLayout_2->addWidget(m_saveSignal_ptr);
    buttonLayout_2->addSpacerItem(new QSpacerItem(100, 50, QSizePolicy::MinimumExpanding));

    m_itemView_ptr = new QTableWidget();
    QVBoxLayout* itemviewLaout = new QVBoxLayout();
    itemviewLaout->addWidget(m_itemView_ptr);
    m_itemView_ptr->setColumnCount(frameDataHeaderName.size());
    m_itemView_ptr->setHorizontalHeaderLabels(frameDataHeaderName);
    connect(m_itemView_ptr, &QTableWidget::itemChanged, this, &ModelDataManage::on_frameItemChanged);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addLayout(buttonLayout_2);
    rightLayout->addLayout(itemviewLaout);

    rightLayout->setStretch(0, 1);
    rightLayout->setStretch(1, 10);
  
    //============= merge layout ===================
    QHBoxLayout* centralLaout = new QHBoxLayout();
    
    QSplitter* split = new QSplitter();
    QWidget* wl = new QWidget();
    wl->setLayout(leftLayout);
    QWidget* wr = new QWidget();
    wr->setLayout(rightLayout);
    split->addWidget(wl);
    split->addWidget(wr);
    split->setStretchFactor(0, 3);
    split->setStretchFactor(1, 7);
    centralLaout->addWidget(split);
    this->setLayout(centralLaout);
}

void ModelDataManage::initModelData()
{
    QResource::registerResource(QApplication::applicationDirPath() + "/Resources/icon.rcc");
    qGboleData* qgb = qGboleData::getInstance();
    //Initinalize data if not init
    if (!qgb->getIsInit())
    {
        qgb->read();
    }
    m_modelView_ptr->blockSignals(true);

    //Hidden header
    m_modelView_ptr->setHeaderHidden(true);

    //Global modelin treeview
    model = new QStandardItemModel(m_modelView_ptr);
    for (int i = 0; i < qgb->pGboleData.size(); i++)
    {

        QStandardItem *secondItem = new QStandardItem(QIcon(QStringLiteral(":/icon/model.ico")),qgb->pGboleData.at(i).modelName);
        
        model->appendRow(secondItem);
        for (int j = 0; j < qgb->pGboleData.at(i).cItem.size(); j++)
        {
            QStandardItem* thirdItem = new QStandardItem(QIcon(QStringLiteral(":/icon/idcard-fill.ico")),qgb->pGboleData.at(i).cItem.at(j).strCanId);
            thirdItem->setCheckable(true);
            if(qgb->pGboleData.at(i).cItem.at(j).isSend)
                thirdItem->setCheckState(Qt::Checked);
            else
                thirdItem->setCheckState(Qt::Unchecked);
            secondItem->appendRow(thirdItem);
        }

    }
    m_modelView_ptr->setModel(model);
    connect(model, &QStandardItemModel::itemChanged, this, &ModelDataManage::on_modelItemChanged);
    m_modelView_ptr->blockSignals(false);
}
void ModelDataManage::resetParameter(const canIdData& canid)
{
    m_modelParamView_ptr->blockSignals(true);

    m_modelParamView_ptr->clear();
    //set number of column, must be set before use
    m_modelParamView_ptr->setColumnCount(canidHeaderName.size());
    //set table Header
    m_modelParamView_ptr->setHorizontalHeaderLabels(canidHeaderName);
    //set  number of  rows, must be set before to use
    m_modelParamView_ptr->setRowCount(1);

    //messages direction,send or receive
    QComboBox* direction = new QComboBox();
    direction->addItem(QString("Receive"));
    direction->addItem(QString("Send"));
    direction->setCurrentIndex(canid.opt);
    m_modelParamView_ptr->setCellWidget(0, 0, direction);

    //frame data lenght(only LIN)
    m_modelParamView_ptr->setItem(0, 1,new QTableWidgetItem(QString::number(canid.len)));

    //custom circle time by per id
    m_modelParamView_ptr->setItem(0, 2,new QTableWidgetItem(QString::number(canid.circle)));

    showFrameItem(canid.pItem);

    m_modelParamView_ptr->blockSignals(false);
    connect(direction, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModelDataManage::on_msgDirction_currentIndexChanged);
}

void ModelDataManage::resetParameter(const protoData& proto)
{
    m_modelParamView_ptr->blockSignals(true);
    m_modelParamView_ptr->clear();
    m_modelParamView_ptr->setColumnCount(modelHeaderName.size());
    m_modelParamView_ptr->setHorizontalHeaderLabels(modelHeaderName);
    m_modelParamView_ptr->setRowCount(1);

    // CAN frame data format
    QComboBox* agreement = new QComboBox();
    agreement->addItem(QString("Intel"));
    agreement->addItem(QString("MotorolaMSB"));
    agreement->addItem(QString("MotorolaLSB"));
    agreement->setCurrentIndex(proto.agreement);
    m_modelParamView_ptr->setCellWidget(0, 0, agreement);
    connect(agreement, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModelDataManage::on_msgAgreement_currentIndexChanged);
    //buad rate
    QComboBox* buad = new QComboBox();
    for (int i = 0; i < buadRate.size(); i++)
    {
        buad->addItem(buadRate[i]);
        buad->setItemData(i, buadNumber[i]);
    }
    buad->setCurrentIndex(proto.bundRate);
    m_modelParamView_ptr->setCellWidget(0, 1, buad);
    connect(buad, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModelDataManage::on_msgBuadRate_currentIndexChanged);
    
    //message send circle time
    m_modelParamView_ptr->setItem(0, 2, new QTableWidgetItem(QString::number(proto.circle)));

    //platform,such as 1kw 2kw
    QComboBox* cplatfor = new QComboBox();
    for (int i = 0; i < platform.size(); i++)
    {
        cplatfor->addItem(platform[i]);
        cplatfor->setItemData(i, platform[i]);
    }
    cplatfor->setCurrentText(proto.sPlatform);
    m_modelParamView_ptr->setCellWidget(0, 3, cplatfor);

    connect(cplatfor, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ModelDataManage::on_msgPlatform_currentIndexChanged);
    //Whether it is standard frame
    QCheckBox* cb = new QCheckBox();
    cb->setChecked(proto.bStandardId);
    m_modelParamView_ptr->setCellWidget(0, 4, cb);
    connect(cb, &QCheckBox::stateChanged, this, &ModelDataManage::on_msgIsStandard_checkState);
    //Whether it is LIN protocol
    QCheckBox* lin = new QCheckBox();
    lin->setChecked(proto.isLIN);
    m_modelParamView_ptr->setCellWidget(0, 5, lin);
    connect(lin, &QCheckBox::stateChanged, this, &ModelDataManage::on_msgIsLIN_checkState);
    m_modelParamView_ptr->blockSignals(false);
    
}
void ModelDataManage::showFrameItem(const std::vector<struct protoItem>& pItem)
{
    //disconnect(m_itemView_ptr, &QTableWidget::itemChanged, this, &ModelDataManage::on_frameItemChanged);
    m_itemView_ptr->blockSignals(true);
    int rowCount = m_itemView_ptr->rowCount();
    for (int m = 0; m < rowCount; m++)
        m_itemView_ptr->removeRow(rowCount - m - 1);
    for (int i = 0; i < pItem.size(); i++)
    {
        //short int crow = tableView->rowCount();
        m_itemView_ptr->setRowCount(i + 1);
        m_itemView_ptr->setItem(i, 0, new QTableWidgetItem(pItem.at(i).bitName));

        QSpinBox* startbitBox = new QSpinBox();
        startbitBox->setRange(0, 63);
        startbitBox->setValue(pItem.at(i).startBit);
        m_itemView_ptr->setCellWidget(i, 1, startbitBox);

        QSpinBox* lenghtBox = new QSpinBox();
        lenghtBox->setRange(1, 64);
        lenghtBox->setValue(pItem.at(i).bitLeng);
        m_itemView_ptr->setCellWidget(i, 2, lenghtBox);

        connect(startbitBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
        connect(lenghtBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
        //m_itemView_ptr->setItem(i, 1, new QTableWidgetItem(QString::number(pItem.at(i).startBit)));
        //m_itemView_ptr->setItem(i, 2, new QTableWidgetItem(QString::number(pItem.at(i).bitLeng)));
        m_itemView_ptr->setItem(i, 3, new QTableWidgetItem(QString::number(pItem.at(i).precision)));
        m_itemView_ptr->setItem(i, 4, new QTableWidgetItem(QString::number(pItem.at(i).offset)));
        QPushButton* property = new QPushButton(tr("ValueMap"));
        connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
        m_itemView_ptr->setCellWidget(i, 5, property);
        QCheckBox* isRoll = new QCheckBox();
        if (pItem.at(i).isRoll)
            isRoll->setCheckState(Qt::Checked);
        m_itemView_ptr->setCellWidget(i, 6, isRoll);
       
        m_itemView_ptr->setItem(i, 7, new QTableWidgetItem(pItem.at(i).dataFrom));
        QCheckBox* isHex = new QCheckBox();
        m_itemView_ptr->setCellWidget(i, 8, isHex);
        isHex->setChecked(pItem.at(i).octhex);

        //这个connect一定放在setCheckState(Qt::Checked);这个函数后面，不然一调用这个函数，就会触发信号
        connect(isRoll, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);
        connect(isHex, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);

        if (0)
        {
            property->setEnabled(false);
            isRoll->setEnabled(false);
            isHex->setEnabled(false);
        }
        else
        {
            property->setEnabled(true);
            isRoll->setEnabled(true);
            isHex->setEnabled(true);
        }
    }
    m_itemView_ptr->blockSignals(false);
}
void ModelDataManage::on_frameItemChanged(QTableWidgetItem* item)
{
    if (!m_modelView_ptr->currentIndex().isValid())
        return;
    auto item_t = model->itemFromIndex(m_modelView_ptr->currentIndex());
    
    if (!item_t->parent())
    {//当前选中的是型号项的，不允许修改
        QMessageBox::warning(this, tr("Warning"), tr("Please Selection a ID item before changed signal value"));
        return;
    }
    auto Index = model->indexFromItem(item_t->parent());
    if (!Index.isValid())return;
    int parent_row = Index.row();
    int son_row = m_modelView_ptr->currentIndex().row();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    //检查边界
    if (parent_row < 0 || parent_row >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("parent_row Over range"));
        return;
    }
    if (son_row < 0 || son_row >= qgb->pGboleData.at(parent_row).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("son_row Over range"));
        return;
    }
    int c_row = item->row();
    int c_col = item->column();

    if (c_row < 0 || c_row >= qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("c_row Over range"));
        return;
    }
    switch (c_col)
    {
    case 0:
        qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(c_row).bitName = item->text();
        break;
    case 3:
        qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(c_row).precision = item->text().toFloat();
        break;
    case 4:
        qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(c_row).offset = item->text().toInt();
        break;
    case 7:
        qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(c_row).dataFrom = item->text();
        break;
    default:
        break;
    }
 

}
void ModelDataManage::on_modelParamCellChanged(int row,int col)
{
    
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;

    auto item = model->itemFromIndex(m_modelView_ptr->currentIndex());
    if (!item)return;

    int mrow = m_modelView_ptr->currentIndex().row();
    if (item->parent())
    {//有父级对象，说明它是ID那一项的
        int prow = model->indexFromItem(item->parent()).row();
        if (prow < 0 || prow >= qgb->pGboleData.size())
        {
            QMessageBox::warning(this, tr("Warning"), tr("over range!"));
            return;
        }
        if (mrow < 0 || mrow >= qgb->pGboleData.at(prow).cItem.size())
        {
            QMessageBox::warning(this, tr("Warning"), tr("over range!"));
            return;
        }
        if (row != 0)return;
        QTableWidgetItem* item = m_modelParamView_ptr->item(row, col);
        if (!item)return;
        switch (col)
        {
        case 1://报文长度
            qgb->pGboleData.at(prow).cItem.at(mrow).len = item->text().toInt();
            break;
        case 2://自定义报文周期
            qgb->pGboleData.at(prow).cItem.at(mrow).circle = item->text().toInt();
            break;
        default:
            break;
        }
    }
    else
    {
        if (mrow < 0 || mrow >= qgb->pGboleData.size())
        {
            QMessageBox::warning(this, tr("Warning"), tr("over range!"));
            return;
        }
        if (row != 0)return;
        QTableWidgetItem* item = m_modelParamView_ptr->item(row, col);
        if (!item)return;
        switch (col)
        {
        case 2://报文周期
            qgb->pGboleData.at(mrow).circle = item->text().toInt();
            break;
        default:
            break;
        }
    }
    
    

}
/*
* @brief: treeView 表格上的item点击之后发出的信号，关联到这里
* 当item被点击时，分两种情况
* 1、点击的是型号的，这个时候下面的tableWidget就要显示这个型号的参数
* 2、点击的是ID，下面的tableWidget就是要显示这个id的参数
* @praremeter:
*   const QModelIndex& index： 被点击的index
* @return:
*   void
*/
void ModelDataManage::on_modelView_clicked(const QModelIndex& index)
{
    if (!index.isValid())return;
    qDebug() << "IndexRow: " << index.row();
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;
    qGboleData* qgb = qGboleData::getInstance();
    
    if (item->parent()!=nullptr)
    {
        if (item->parent()->row() >= qgb->pGboleData.size())
            return;
        if (index.row() >= qgb->pGboleData.at(item->parent()->row()).cItem.size())
            return;
        resetParameter(qgb->pGboleData.at(item->parent()->row()).cItem.at(index.row()));
    }
    else
    {
        //检查边界，在新建型号的时候可能会越界
        if (index.row() >= qgb->pGboleData.size())
            return;
        resetParameter(qgb->pGboleData.at(index.row()));
    }

}
void ModelDataManage::on_ItemCheckStateChanged(int state)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;
    //如果父级是最顶层的，则不能修改
    if (item->parent() == nullptr)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please Selected ID item before"));
        return;
    }
    int modelRow = item->parent()->row();
    int idRow = item->row();
    QCheckBox* ck = dynamic_cast<QCheckBox*>(sender());
    if (!ck)return;
    //获取widget所在表格中的相对位置
    QModelIndex cIndex = m_itemView_ptr->indexAt(QPoint(ck->geometry().x(), ck->geometry().y()));
    if (!cIndex.isValid())return;
    //checkbox所在行
    int citemRow = cIndex.row();
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    //确保所有的index都在我的范围之内
    if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("modelRow over range"));
        return;
    }
    if (idRow < 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("idRow over range"));
        return;
    }
    if (citemRow < 0 || citemRow >= qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("citemRow over range"));
        return;
    }
    if(cIndex.column()==8)
    {
        //修改
        qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.at(citemRow).octhex = (state > 0 ? true : false);
    }
    else if (cIndex.column() == 6)
    {
        //修改
        qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.at(citemRow).isRoll = (state > 0 ? true : false);
    }

}
void ModelDataManage::on_ItemSpinboxChanged(int value)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;
    //如果父级是最顶层的，则不能修改
    if (item->parent() == nullptr)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please Selected ID item before"));
        return;
    }
    int modelRow = item->parent()->row();
    int idRow = item->row();
    QSpinBox* ck = dynamic_cast<QSpinBox*>(sender());
    if (!ck)return;
    //获取widget所在表格中的相对位置
    QModelIndex cIndex = m_itemView_ptr->indexAt(QPoint(ck->geometry().x(), ck->geometry().y()));
    if (!cIndex.isValid())return;
    //QSpinBox所在行
    int citemRow = cIndex.row();
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    //确保所有的index都在我的范围之内
    if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("modelRow over range"));
        return;
    }
    if (idRow < 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("idRow over range"));
        return;
    }
    if (citemRow < 0 || citemRow >= qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("citemRow over range"));
        return;
    }
    if (cIndex.column() == 1)
    {
        //修改
        qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.at(citemRow).startBit=value;
        qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.at(citemRow).startByte=value/8;
    }
    else if (cIndex.column() == 2)
    {
        //修改
        qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.at(citemRow).bitLeng = value;
    }
}
void ModelDataManage::on_property_clicked()
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;
    //如果父级是最顶层的，则不能修改
    if (item->parent() == nullptr)
    {
        QMessageBox::warning(this, tr("Warning"), tr("Please Selected ID item before"));
        return;
    }
    int modelRow = item->parent()->row();
    int idRow = item->row();
    QPushButton* ck = dynamic_cast<QPushButton*>(sender());
    if (!ck)return;
    //获取widget所在表格中的相对位置
    QModelIndex cIndex = m_itemView_ptr->indexAt(QPoint(ck->geometry().x(), ck->geometry().y()));
    if (!cIndex.isValid())return;
    //QSpinBox所在行
    int citemRow = cIndex.row();
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    //确保所有的index都在我的范围之内
    if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("modelRow over range"));
        return;
    }
    if (idRow < 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("idRow over range"));
        return;
    }
    if (citemRow < 0 || citemRow >= qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("citemRow over range"));
        return;
    }
    if (!pp)
    {
        pp = new QSetProperty();
    }
    //取出该项的map
    //std::map<QString, cellProperty>&mapMM = qGb->pGboleData.at(mCurRow).cItem.at(cCurRow).pItem.at(row).itemProperty;
    //取出该项的vector
    std::vector<cellProperty>& stlMM = qgb->pGboleData.at(modelRow).cItem.at(idRow).pItem.at(citemRow).stl_itemProperty;
    //丢个指针进去（C/C++指针真TMD的好）
    pp->setIntoMap(&stlMM);
    pp->setWindowFlag(Qt::Window);
    pp->show();
}
void ModelDataManage::on_m_saveModel_ptr_clicked(bool b)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    qgb->save();
}
void ModelDataManage::on_m_addModel_ptr_clicked(bool b)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    QStandardItem* secondItem = new QStandardItem(QIcon(QStringLiteral(":/icon/model.ico")), tr("Model"));
    model->appendRow(secondItem);
    protoData pmodel;
    qgb->pGboleData.push_back(pmodel);
}
void ModelDataManage::on_m_addIDModel_ptr_clicked(bool b)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;
    
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    int modelRow;
 
    QStandardItem* son = new QStandardItem(QIcon(QStringLiteral(":/icon/idcard-fill.ico")), tr("0x12345678"));
    son->setCheckable(true);
    son->setCheckState(Qt::Checked);
    if (item->parent() == nullptr)
    {
        modelRow = item->row();
        item->appendRow(son);
    }
    else
    {
        modelRow = item->parent()->row();
        item->parent()->appendRow(son);
    }
    if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
        return;
    canIdData cItem;
    cItem.strCanId = "0x12345678";
    cItem.circle = -1;
    cItem.isSend = true;
    cItem.len = 8;
    cItem.opt = 0;
    qgb->pGboleData.at(modelRow).cItem.push_back(cItem);
    
}
void ModelDataManage::on_m_delModel_ptr_clicked(bool b)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    int modelRow=0;
    if (item->parent() == nullptr)
    {
        modelRow = item->row();
        if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
            return;
        model->removeRow(modelRow);
        auto vec_begin = qgb->pGboleData.begin();
        auto vec_end = qgb->pGboleData.end();
        int index = 0;
        while (vec_begin != vec_end)
        {
            if (index == modelRow)
            {
                qgb->pGboleData.erase(vec_begin);
                break;
            }
            index++;
            vec_begin++;
        }
    }
    else
    {
        modelRow = item->parent()->row();
        int idRow = item->row();

        if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
            return;
        if (idRow < 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size())
            return;
        //移除model的数据
        item->parent()->removeRow(idRow);

        auto vec_begin = qgb->pGboleData.at(modelRow).cItem.begin();
        auto vec_end = qgb->pGboleData.at(modelRow).cItem.end();
        int index = 0;
        //移除数据库的数据
        while (vec_begin != vec_end)
        {
            if (index == idRow)
            {
                qgb->pGboleData.at(modelRow).cItem.erase(vec_begin);
                break;
            }
            index++;
            vec_begin++;
        }
    }

}
void ModelDataManage::on_m_upModel_ptr_clicked(bool b)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    int modelRow = 0;

    if (item->parent() == nullptr)
    {
        modelRow = item->row();
        if (modelRow <= 0 || modelRow >= qgb->pGboleData.size())
            return;
        //把当前数据取出来
        auto m_t = model->takeItem(modelRow);
        //插入到上面
        model->insertRow(modelRow - 1, m_t);
        //移除行
        model->removeRow(modelRow + 1);
        //交换内存中的数据
        std::swap(qgb->pGboleData.at(modelRow), qgb->pGboleData.at(modelRow - 1));
    }
    else
    {
        modelRow = item->parent()->row();
        int idRow = item->row();

        if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
            return;
        if (idRow <= 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size())
            return;
        auto i_p = item->parent();
        //取出数据，会自动删除表格
        auto i_t = i_p->takeRow(idRow);
        i_p->insertRow(idRow -1 , i_t);
        m_modelView_ptr->setCurrentIndex(index);
        //不需要删除行了
        //i_p->removeRow(idRow+1); 

        //交换内存中的数据
        std::swap(qgb->pGboleData.at(modelRow).cItem.at(idRow), qgb->pGboleData.at(modelRow).cItem.at(idRow-1));
    }

}
void ModelDataManage::on_m_downModel_ptr_clicked(bool b)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    int modelRow = 0;

    if (item->parent() == nullptr)
    {
        modelRow = item->row();
        if (modelRow < 0 || modelRow >= qgb->pGboleData.size()-1)
            return;
        //把当前数据取出来
        auto m_t = model->takeItem(modelRow);
        //插入到下面
        model->insertRow(modelRow + 2, m_t);
        //移除行
        model->removeRow(modelRow);
        //交换内存中的数据
        std::swap(qgb->pGboleData.at(modelRow), qgb->pGboleData.at(modelRow + 1));
    }
    else
    {
        modelRow = item->parent()->row();
        int idRow = item->row();

        if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
            return;
        //这里后面的-1是最后一个不能下移
        if (idRow < 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size()-1)
            return;
        auto i_p = item->parent();
        //取出数据，会自动删除表格
        auto i_t = i_p->takeRow(idRow);
        i_p->insertRow(idRow + 1, i_t);
        m_modelView_ptr->setCurrentIndex(index);
        //不需要删除行了
        //i_p->removeRow(idRow+1); 
        //交换内存中的数据
        std::swap(qgb->pGboleData.at(modelRow).cItem.at(idRow), qgb->pGboleData.at(modelRow).cItem.at(idRow + 1));
    }
}
void ModelDataManage::on_m_copyModel_ptr_clicked(bool b)
{
    //获取父级
    QModelIndex index = m_modelView_ptr->currentIndex();
    if (!index.isValid())return;
    QStandardItem* item = model->itemFromIndex(index);
    if (item == nullptr)
        return;

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    int modelRow = 0;


    if (item->parent() == nullptr)
    {
        modelRow = item->row();
        if (modelRow < 0 || modelRow > qgb->pGboleData.size() - 1)
            return;
        QStandardItem* secondItem = new QStandardItem(QIcon(QStringLiteral(":/icon/model.ico")), qgb->pGboleData.at(modelRow).modelName);
        for (int j = 0; j < qgb->pGboleData.at(modelRow).cItem.size(); j++)
        {
            QStandardItem* thirdItem = new QStandardItem(QIcon(QStringLiteral(":/icon/idcard-fill.ico")), qgb->pGboleData.at(modelRow).cItem.at(j).strCanId);
            
            thirdItem->setCheckable(true);
            if (qgb->pGboleData.at(modelRow).cItem.at(j).isSend)
                thirdItem->setCheckState(Qt::Checked);
            else
                thirdItem->setCheckState(Qt::Unchecked);
            secondItem->appendRow(thirdItem);
        }
        
        //插入到下面
        model->insertRow(modelRow + 1, secondItem);
        qgb->pGboleData.insert(qgb->pGboleData.begin()+modelRow + 1, qgb->pGboleData.at(modelRow));
       
    }
    else
    {
        modelRow = item->parent()->row();
        int idRow = item->row();

        if (modelRow < 0 || modelRow > qgb->pGboleData.size())
            return;
        //这里后面的-1是最后一个不能下移
        if (idRow < 0 || idRow > qgb->pGboleData.at(modelRow).cItem.size() - 1)
            return;
        auto i_p = item->parent();
        QStandardItem* thirdItem = new QStandardItem(QIcon(QStringLiteral(":/icon/idcard-fill.ico")), qgb->pGboleData.at(modelRow).cItem.at(idRow).strCanId);
        thirdItem->setCheckable(true);
        thirdItem->setCheckState(Qt::Checked);
        i_p->insertRow(idRow + 1, thirdItem);
        m_modelView_ptr->setCurrentIndex(index);
        //插入，使用迭代器形式
        qgb->pGboleData.at(modelRow).cItem.insert(qgb->pGboleData.at(modelRow).cItem.begin() + idRow + 1, qgb->pGboleData.at(modelRow).cItem.at(idRow));


    }
}
void ModelDataManage::on_m_saveSignal_ptr_clicked(bool b)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)
    {
        return;
    }
    qgb->save();
}
void ModelDataManage::on_m_addSignal_ptr_clicked(bool b)
{
    
    if (!m_modelView_ptr->currentIndex().isValid())
        return;
    auto item_t = model->itemFromIndex(m_modelView_ptr->currentIndex());

    if (!item_t->parent())
    {//当前选中的是型号项的，不允许修改
        QMessageBox::warning(this, tr("Warning"), tr("Please Selection a ID item before changed signal value"));
        return;
    }
    auto Index = model->indexFromItem(item_t->parent());
    if (!Index.isValid())return;
    int parent_row = Index.row();
    int son_row = m_modelView_ptr->currentIndex().row();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    //检查边界
    if (parent_row < 0 || parent_row >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("parent_row Over range"));
        return;
    }
    if (son_row < 0 || son_row >= qgb->pGboleData.at(parent_row).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("son_row Over range"));
        return;
    }
    m_itemView_ptr->blockSignals(true);
    protoItem tempItem;
    int i = m_itemView_ptr->rowCount();
    m_itemView_ptr->setRowCount(i + 1);
    m_itemView_ptr->setItem(i, 0, new QTableWidgetItem(tr("Signal") + QString::number(i)));
    tempItem.bitName = tr("Signal") + QString::number(i);
    QSpinBox* startbitBox = new QSpinBox();
    startbitBox->setRange(0, 63);
    startbitBox->setValue(0);
    m_itemView_ptr->setCellWidget(i, 1, startbitBox);
    tempItem.startBit = 0;
    tempItem.startByte = 0;
    QSpinBox* lenghtBox = new QSpinBox();
    lenghtBox->setRange(1, 64);
    lenghtBox->setValue(1);
    m_itemView_ptr->setCellWidget(i, 2, lenghtBox);
    tempItem.bitLeng = 1;
    connect(startbitBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    connect(lenghtBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    //m_itemView_ptr->setItem(i, 1, new QTableWidgetItem(QString::number(pItem.at(i).startBit)));
    //m_itemView_ptr->setItem(i, 2, new QTableWidgetItem(QString::number(pItem.at(i).bitLeng)));
    m_itemView_ptr->setItem(i, 3, new QTableWidgetItem(QString::number(1)));
    m_itemView_ptr->setItem(i, 4, new QTableWidgetItem(QString::number(0)));
    tempItem.precision = 1;
    tempItem.offset = 0;
    QPushButton* property = new QPushButton(tr("ValueMap"));
    connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
    m_itemView_ptr->setCellWidget(i, 5, property);
    QCheckBox* isRoll = new QCheckBox();
    isRoll->setCheckState(Qt::Unchecked);
    m_itemView_ptr->setCellWidget(i, 6, isRoll);
    tempItem.isRoll = false;
    m_itemView_ptr->setItem(i, 7, new QTableWidgetItem("-1"));
    tempItem.dataFrom = "-1";
    QCheckBox* isHex = new QCheckBox();
    m_itemView_ptr->setCellWidget(i, 8, isHex);
    isHex->setChecked(false);
    tempItem.octhex = false;
    //这个connect一定放在setCheckState(Qt::Checked);这个函数后面，不然一调用这个函数，就会触发信号
    connect(isRoll, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);
    connect(isHex, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);

    qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.push_back(tempItem);
    m_itemView_ptr->blockSignals(false);
}
void ModelDataManage::on_m_delSignal_ptr_clicked(bool b)
{
    
    if (!m_modelView_ptr->currentIndex().isValid())
        return;
    auto item_t = model->itemFromIndex(m_modelView_ptr->currentIndex());

    if (!item_t->parent())
    {//当前选中的是型号项的，不允许修改
        QMessageBox::warning(this, tr("Warning"), tr("Please Selection a ID item before changed signal value"));
        return;
    }
    auto Index = model->indexFromItem(item_t->parent());
    if (!Index.isValid())return;
    int parent_row = Index.row();
    int son_row = m_modelView_ptr->currentIndex().row();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    //检查边界
    if (parent_row < 0 || parent_row >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("parent_row Over range"));
        return;
    }
    if (son_row < 0 || son_row >= qgb->pGboleData.at(parent_row).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("son_row Over range"));
        return;
    }
    m_itemView_ptr->blockSignals(true);

    int s_item = m_itemView_ptr->currentRow();
    m_itemView_ptr->removeRow(s_item);

    auto it = qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.begin();
    auto it_end = qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.end();
    int index = 0;
    while(it!=it_end)
    {
        if (index == s_item)
        {
            qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.erase(it);
            break;
        }
        it++;
        index++;
    }
    m_itemView_ptr->blockSignals(false);
}
void ModelDataManage::on_m_upSignal_ptr_clicked(bool b)
{
   
    if (!m_modelView_ptr->currentIndex().isValid())
        return;
    auto item_t = model->itemFromIndex(m_modelView_ptr->currentIndex());

    if (!item_t->parent())
    {//当前选中的是型号项的，不允许修改
        QMessageBox::warning(this, tr("Warning"), tr("Please Selection a ID item before changed signal value"));
        return;
    }
    auto Index = model->indexFromItem(item_t->parent());
    if (!Index.isValid())return;
    int parent_row = Index.row();
    int son_row = m_modelView_ptr->currentIndex().row();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    //检查边界
    if (parent_row < 0 || parent_row >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("parent_row Over range"));
        return;
    }
    if (son_row < 0 || son_row >= qgb->pGboleData.at(parent_row).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("son_row Over range"));
        return;
    }
    int s_item = m_itemView_ptr->currentRow();
    if(s_item<=0 || s_item>= qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("row Over range"));
        return;
    }

    m_itemView_ptr->blockSignals(true);

    protoItem pItem = qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item);
    m_itemView_ptr->insertRow(s_item - 1);


    m_itemView_ptr->setItem(s_item - 1, 0, new QTableWidgetItem(pItem.bitName));

    QSpinBox* startbitBox = new QSpinBox();
    startbitBox->setRange(0, 63);
    startbitBox->setValue(pItem.startBit);
    m_itemView_ptr->setCellWidget(s_item - 1, 1, startbitBox);

    QSpinBox* lenghtBox = new QSpinBox();
    lenghtBox->setRange(1, 64);
    lenghtBox->setValue(pItem.bitLeng);
    m_itemView_ptr->setCellWidget(s_item - 1, 2, lenghtBox);

    connect(startbitBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    connect(lenghtBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    m_itemView_ptr->setItem(s_item - 1, 3, new QTableWidgetItem(QString::number(pItem.precision)));
    m_itemView_ptr->setItem(s_item - 1, 4, new QTableWidgetItem(QString::number(pItem.offset)));
    QPushButton* property = new QPushButton(QStringLiteral("信号值"));
    connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
    m_itemView_ptr->setCellWidget(s_item - 1, 5, property);
    QCheckBox* isRoll = new QCheckBox();
    if (pItem.isRoll)
        isRoll->setCheckState(Qt::Checked);
    m_itemView_ptr->setCellWidget(s_item - 1, 6, isRoll);

    m_itemView_ptr->setItem(s_item - 1, 7, new QTableWidgetItem(pItem.dataFrom));
    QCheckBox* isHex = new QCheckBox();
    m_itemView_ptr->setCellWidget(s_item - 1, 8, isHex);
    isHex->setChecked(pItem.octhex);

    //这个connect一定放在setCheckState(Qt::Checked);这个函数后面，不然一调用这个函数，就会触发信号
    connect(isRoll, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);
    connect(isHex, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);

    m_itemView_ptr->removeRow(s_item + 1);
    m_itemView_ptr->blockSignals(false);

    std::swap(qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item), qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item - 1));
}
void ModelDataManage::on_m_downSignal_ptr_clicked(bool b)
{
    if (!m_modelView_ptr->currentIndex().isValid())
        return;
    auto item_t = model->itemFromIndex(m_modelView_ptr->currentIndex());

    if (!item_t->parent())
    {//当前选中的是型号项的，不允许修改
        QMessageBox::warning(this, tr("Warning"), tr("Please Selection a ID item before changed signal value"));
        return;
    }
    auto Index = model->indexFromItem(item_t->parent());
    if (!Index.isValid())return;
    int parent_row = Index.row();
    int son_row = m_modelView_ptr->currentIndex().row();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    //检查边界
    if (parent_row < 0 || parent_row >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("parent_row Over range"));
        return;
    }
    if (son_row < 0 || son_row >= qgb->pGboleData.at(parent_row).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("son_row Over range"));
        return;
    }
    int s_item = m_itemView_ptr->currentRow();
    if (s_item < 0 || s_item >= qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.size()-1)
    {
        QMessageBox::warning(this, tr("Warning"), tr("row Over range"));
        return;
    }

    m_itemView_ptr->blockSignals(true);

    protoItem pItem = qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item);
    m_itemView_ptr->insertRow(s_item + 2);


    m_itemView_ptr->setItem(s_item + 2, 0, new QTableWidgetItem(pItem.bitName));

    QSpinBox* startbitBox = new QSpinBox();
    startbitBox->setRange(0, 63);
    startbitBox->setValue(pItem.startBit);
    m_itemView_ptr->setCellWidget(s_item + 2, 1, startbitBox);

    QSpinBox* lenghtBox = new QSpinBox();
    lenghtBox->setRange(1, 64);
    lenghtBox->setValue(pItem.bitLeng);
    m_itemView_ptr->setCellWidget(s_item + 2, 2, lenghtBox);

    connect(startbitBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    connect(lenghtBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    m_itemView_ptr->setItem(s_item + 2, 3, new QTableWidgetItem(QString::number(pItem.precision)));
    m_itemView_ptr->setItem(s_item + 2, 4, new QTableWidgetItem(QString::number(pItem.offset)));
    QPushButton* property = new QPushButton(tr("ValueMap"));
    connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
    m_itemView_ptr->setCellWidget(s_item + 2, 5, property);
    QCheckBox* isRoll = new QCheckBox();
    if (pItem.isRoll)
        isRoll->setCheckState(Qt::Checked);
    m_itemView_ptr->setCellWidget(s_item + 2, 6, isRoll);

    m_itemView_ptr->setItem(s_item + 2, 7, new QTableWidgetItem(pItem.dataFrom));
    QCheckBox* isHex = new QCheckBox();
    m_itemView_ptr->setCellWidget(s_item + 2, 8, isHex);
    isHex->setChecked(pItem.octhex);

    //这个connect一定放在setCheckState(Qt::Checked);这个函数后面，不然一调用这个函数，就会触发信号
    connect(isRoll, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);
    connect(isHex, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);

    m_itemView_ptr->removeRow(s_item);
    m_itemView_ptr->blockSignals(false);

    std::swap(qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item), qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item + 1));
}
void ModelDataManage::on_m_copySignal_ptr_clicked(bool b)
{
    if (!m_modelView_ptr->currentIndex().isValid())
        return;
    auto item_t = model->itemFromIndex(m_modelView_ptr->currentIndex());

    if (!item_t->parent())
    {//当前选中的是型号项的，不允许修改
        QMessageBox::warning(this, tr("Warning"), tr("Please Selection a ID item before changed signal value"));
        return;
    }
    auto Index = model->indexFromItem(item_t->parent());
    if (!Index.isValid())return;
    int parent_row = Index.row();
    int son_row = m_modelView_ptr->currentIndex().row();

    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    //检查边界
    if (parent_row < 0 || parent_row >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("parent_row Over range"));
        return;
    }
    if (son_row < 0 || son_row >= qgb->pGboleData.at(parent_row).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("son_row Over range"));
        return;
    }
    int s_item = m_itemView_ptr->currentRow();
    if (s_item < 0 || s_item >= qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("row Over range"));
        return;
    }

    m_itemView_ptr->blockSignals(true);

    protoItem pItem = qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.at(s_item);
    m_itemView_ptr->insertRow(s_item + 1);


    m_itemView_ptr->setItem(s_item + 1, 0, new QTableWidgetItem(pItem.bitName));

    QSpinBox* startbitBox = new QSpinBox();
    startbitBox->setRange(0, 63);
    startbitBox->setValue(pItem.startBit);
    m_itemView_ptr->setCellWidget(s_item + 1, 1, startbitBox);

    QSpinBox* lenghtBox = new QSpinBox();
    lenghtBox->setRange(1, 64);
    lenghtBox->setValue(pItem.bitLeng);
    m_itemView_ptr->setCellWidget(s_item + 1, 2, lenghtBox);

    connect(startbitBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    connect(lenghtBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &ModelDataManage::on_ItemSpinboxChanged);
    m_itemView_ptr->setItem(s_item + 1, 3, new QTableWidgetItem(QString::number(pItem.precision)));
    m_itemView_ptr->setItem(s_item + 1, 4, new QTableWidgetItem(QString::number(pItem.offset)));
    QPushButton* property = new QPushButton(tr("ValueMap"));
    connect(property, SIGNAL(clicked()), this, SLOT(on_property_clicked()));
    m_itemView_ptr->setCellWidget(s_item + 1, 5, property);
    QCheckBox* isRoll = new QCheckBox();
    if (pItem.isRoll)
        isRoll->setCheckState(Qt::Checked);
    m_itemView_ptr->setCellWidget(s_item + 1, 6, isRoll);

    m_itemView_ptr->setItem(s_item + 1, 7, new QTableWidgetItem(pItem.dataFrom));
    QCheckBox* isHex = new QCheckBox();
    m_itemView_ptr->setCellWidget(s_item + 1, 8, isHex);
    isHex->setChecked(pItem.octhex);

    //这个connect一定放在setCheckState(Qt::Checked);这个函数后面，不然一调用这个函数，就会触发信号
    connect(isRoll, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);
    connect(isHex, &QCheckBox::stateChanged, this, &ModelDataManage::on_ItemCheckStateChanged);

    //m_itemView_ptr->removeRow(s_item);
    m_itemView_ptr->blockSignals(false);

    qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.insert(qgb->pGboleData.at(parent_row).cItem.at(son_row).pItem.begin() + s_item + 1, pItem);
}
void ModelDataManage::on_msgDirction_currentIndexChanged(int index)
{
    //获取子对象的行
    int idRow =  m_modelView_ptr->currentIndex().row();
    //检测是否有父对象
    QStandardItem* item = model->itemFromIndex(m_modelView_ptr->currentIndex());
    if (!item || !item->parent())
        return;
    //获取父对象的行
    int modelRow = model->indexFromItem(item->parent()).row();
   
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;

    //检测一下边界
    if (modelRow < 0 || modelRow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("modelRow over range!"));
        return;
    }
    if (idRow < 0 || idRow >= qgb->pGboleData.at(modelRow).cItem.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("idRow over range!"));
        return;
    }
    //修改内存中的数值
    qgb->pGboleData.at(modelRow).cItem.at(idRow).opt = index;

}
void ModelDataManage::on_msgAgreement_currentIndexChanged(int index)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    int mrow = m_modelView_ptr->currentIndex().row();
    if (mrow < 0 || mrow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("over range!"));
        return;
    }
    qgb->pGboleData.at(mrow).agreement = index;
}
void ModelDataManage::on_msgBuadRate_currentIndexChanged(int index)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    int mrow = m_modelView_ptr->currentIndex().row();
    if (mrow < 0 || mrow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("over range!"));
        return;
    }
    qgb->pGboleData.at(mrow).bundRate = index;
}
void ModelDataManage::on_msgPlatform_currentIndexChanged(int index)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    int mrow = m_modelView_ptr->currentIndex().row();
    if (mrow < 0 || mrow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("over range!"));
        return;
    }
    QComboBox* cb = dynamic_cast<QComboBox*>(sender());
    if (!cb)return;
    qgb->pGboleData.at(mrow).sPlatform = cb->currentText();
}
void ModelDataManage::on_msgIsStandard_checkState(int state)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    int mrow = m_modelView_ptr->currentIndex().row();
    if (mrow < 0 || mrow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("over range!"));
        return;
    }
    
    qgb->pGboleData.at(mrow).bStandardId = (bool)state;
}
void ModelDataManage::on_msgIsLIN_checkState(int states)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (!qgb)return;
    int mrow = m_modelView_ptr->currentIndex().row();
    if (mrow < 0 || mrow >= qgb->pGboleData.size())
    {
        QMessageBox::warning(this, tr("Warning"), tr("over range!"));
        return;
    }
   
    qgb->pGboleData.at(mrow).isLIN = (bool)states;
}
/*
* @brief: treeView 表格上的item修改了之后发出的信号，关联到这里
* 修改了要保存到数据结构里面去
* @praremeter:
*   QStandardItem*item： 被修改的item
* @return:
*   void
*/
void ModelDataManage::on_modelItemChanged(QStandardItem* item)
{
    qGboleData* qgb = qGboleData::getInstance();
    if (item->parent())
    {//CANID项
        int parentRow = item->parent()->row();
        int itemRow = item->row();
        //确保不超出范围
        if (parentRow < 0 || parentRow >= qgb->pGboleData.size())
        {
            return;
        }
        if (itemRow < 0 || itemRow >= qgb->pGboleData.at(parentRow).cItem.size())
        {
            return;
        }
        if (item->isCheckable())
        {
            qgb->pGboleData.at(parentRow).cItem.at(itemRow).isSend=item->checkState() == 0 ? false : true;
        }
        qgb->pGboleData.at(parentRow).cItem.at(itemRow).strCanId = item->text();

    }
    else
    {//型号项
        int itemRow = item->row();
        if (itemRow < 0 || itemRow >= qgb->pGboleData.size())
        {
            return;
        }
        qgb->pGboleData.at(itemRow).modelName = item->text();
    }
}