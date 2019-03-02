#ifndef MLISTMODEL_H
#define MLISTMODEL_H

#include <QAbstractListModel>
#include "mbluetoothdevice.h"

class MListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit MListModel(QObject *parent = nullptr);
    ~MListModel();

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void setDataList(MBluetoothDeviceList *list);
    void updateInertRow(int first, int last);
    void updateRemoveRow(int first, int last);
    void updateMoveRow(int first,int last);
    Q_INVOKABLE void resetAll();

    Q_INVOKABLE QVariant get(int index);
    Q_INVOKABLE int count();
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE MBluetoothDevice *takeAt(int index);
    Q_INVOKABLE void move(int index,int toIndex);
    Q_INVOKABLE void insert(int index,MBluetoothDevice *obj);
private:
    MBluetoothDeviceList *dataList;
};

#endif // MLISTMODEL_H
