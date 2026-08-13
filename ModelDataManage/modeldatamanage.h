#ifndef MODELDATAMANAGE_H
#define MODELDATAMANAGE_H

#include <QWidget>
#include "Unities/dataStruct.h"
#include "ModelDataManage_global.h"
class QTreeView;
class QTreeWidget;
class QTableWidget;
class QPushButton;
class QStandardItem;
class QStandardItemModel;
class QTableWidgetItem;
class QSetProperty;
namespace Ui {
class ModelDataManage;
}

class MODELDATAMANAGE_EXPORT ModelDataManage : public QWidget
{
    Q_OBJECT

public:
    explicit ModelDataManage(QWidget *parent = nullptr);
    ~ModelDataManage();
private:
    void initUI();
    void initModelData();
    void resetParameter(const canIdData& canid);
    void resetParameter(const protoData& proto);
    //show per signal for specially id
    void showFrameItem(const std::vector<struct protoItem>& item);

private Q_SLOTS:
    void on_modelItemChanged(QStandardItem* item);
    void on_frameItemChanged(QTableWidgetItem* item);
    void on_modelParamCellChanged(int row,int col);
    void on_modelView_clicked(const QModelIndex& index);

    //void on_RollCheckStateChanged(int state);
    void on_ItemCheckStateChanged(int state);
    void on_ItemSpinboxChanged(int value);
    void on_property_clicked();

    void on_m_saveModel_ptr_clicked(bool b);
    void on_m_addModel_ptr_clicked(bool b);
    void on_m_addIDModel_ptr_clicked(bool b);
    void on_m_delModel_ptr_clicked(bool b);
    void on_m_upModel_ptr_clicked(bool b);
    void on_m_downModel_ptr_clicked(bool b);
    void on_m_copyModel_ptr_clicked(bool b);

    void on_m_saveSignal_ptr_clicked(bool b);
    void on_m_addSignal_ptr_clicked(bool b);
    void on_m_delSignal_ptr_clicked(bool b);
    void on_m_upSignal_ptr_clicked(bool b);
    void on_m_downSignal_ptr_clicked(bool b);
    void on_m_copySignal_ptr_clicked(bool b);

    void on_msgDirction_currentIndexChanged(int index);
    void on_msgAgreement_currentIndexChanged(int index);
    void on_msgBuadRate_currentIndexChanged(int index);
    void on_msgPlatform_currentIndexChanged(int index);
    void on_msgIsStandard_checkState(int states);
    void on_msgIsLIN_checkState(int states);
    
private:
    Ui::ModelDataManage *ui;

    QTreeView* m_modelView_ptr = nullptr;               //型号的树形表格指针

    QTableWidget* m_itemView_ptr = nullptr;             //信号的表格指针

    QTableWidget* m_modelParamView_ptr = nullptr;       //型号参数的表格指针

    QPushButton* m_addModel_ptr = nullptr;              //添加型号按钮
    QPushButton* m_addIDModel_ptr = nullptr;              //添加型号按钮
    QPushButton* m_delModel_ptr = nullptr;              //删除型号按钮
    QPushButton* m_upModel_ptr = nullptr;               //上移型号按钮
    QPushButton* m_downModel_ptr = nullptr;             //下称型号按钮
    QPushButton* m_saveModel_ptr = nullptr;             //保存型号按钮
    QPushButton* m_copyModel_ptr = nullptr;             //保存型号按钮


    QPushButton* m_addSignal_ptr = nullptr;             //添加信号按钮            
    QPushButton* m_delSignal_ptr = nullptr;             //删除信号按钮 
    QPushButton* m_upSignal_ptr= nullptr;               //上移信号按钮 
    QPushButton* m_downSignal_ptr = nullptr;            //下移信号按钮 
    QPushButton* m_saveSignal_ptr = nullptr;            //保存信号按钮 
    QPushButton* m_copySignal_ptr = nullptr;            //保存信号按钮 

    QStandardItemModel* model;                          //型号模型

    QSetProperty* pp = nullptr;

};

#endif // MODELDATAMANAGE_H
