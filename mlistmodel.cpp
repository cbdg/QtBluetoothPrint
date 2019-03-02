/* ***********************************
 * listview 数据模型
 * 2019-02-20
 * magic
 * ***********************************/
#include "mlistmodel.h"
#include <QDebug>

MListModel::MListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

MListModel::~MListModel()
{

}

int MListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (dataList) {
        return dataList->size();
    }

    return 0;
}

QVariant MListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if( !dataList || dataList->count() <=0 )
        return QVariant();

    if( role == Qt::UserRole)
        return QVariant::fromValue(dataList->at(index.row()));

    return QVariant();
}

QHash<int, QByteArray> MListModel::roleNames() const
{
    QHash<int, QByteArray> hash;
    hash.insert(Qt::UserRole,"modelData");
    return hash;
}

void MListModel::setDataList(MBluetoothDeviceList *list)
{
    dataList = list;
}

void MListModel::updateInertRow(int first, int last)
{
    if( first > last )
        return;
    beginInsertRows(QModelIndex(), first, last);
    endInsertRows();
}

void MListModel::updateRemoveRow(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
    endRemoveRows();
}

void MListModel::updateMoveRow(int first, int last)
{
    beginMoveRows(QModelIndex(),first,first,QModelIndex(),last);
    endMoveRows();
}

void MListModel::resetAll()
{
    beginResetModel();
    endResetModel();
}

QVariant MListModel::get(int index)
{
    if(!dataList)
        return QVariant();
    if( index >= 0 && index < dataList->count())
        return QVariant::fromValue(dataList->at(index));
    return QVariant();
}

int MListModel::count()
{
    return dataList ? dataList->count() : 0;
}

void MListModel::remove(int index)
{
    if(!dataList)
        return;
    if( index >=0 && index < count() ){
        MBluetoothDevice *dataObj = dataList->at(index);
        dataList->removeAt(index);
        updateRemoveRow(index,index);
        if (dataObj) {
            dataObj->deleteLater();
        }
    }
}

MBluetoothDevice *MListModel::takeAt(int index)
{
    if(!dataList)
        return NULL;
    if( index >=0 && index < count() ){
        MBluetoothDevice *dataObj = dataList->takeAt(index);
        updateRemoveRow(index,index);
        return dataObj;
    }
    return NULL;
}

void MListModel::move(int index, int toIndex)
{
    if(!dataList)
        return;
    if( index >=0 && index < count() && toIndex>=0 && toIndex<count() ){
        dataList->move(index,toIndex);
        updateMoveRow(index,toIndex);
    }
}

void MListModel::insert(int index, MBluetoothDevice *obj)
{
    if( !dataList )
        return;
    if( index == 0 ) {
        dataList->insert(0,obj);
        updateInertRow(0,1);
    }
    else if( index <0 || index >= count()) {
        dataList->append(obj);
        updateInertRow(count()-1,count());
    }
    else {
        dataList->insert(index,obj);
        updateInertRow(index,index+1);
    }
}
