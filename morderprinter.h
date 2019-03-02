#ifndef MORDERPRINTER_H
#define MORDERPRINTER_H

#include <QObject>

class MBluetoothPrintCommand;
class QBluetoothSocket;
class MOrderPrinter : public QObject
{
    Q_OBJECT
public:
    MOrderPrinter(QObject *parent = nullptr);

    void printTakeoutOrder(const QString &orderJson);
    void printTakeoutOrder(QObject *orderObj);
    void printTest();

    void initPrintCom(QBluetoothSocket *bluetoothSocket);

signals:

public slots:

private:
    MBluetoothPrintCommand *_printCom;
};

#endif // MORDERPRINTER_H
