#ifndef MORDERPRINTER_H
#define MORDERPRINTER_H

#include <QObject>

class MPrintCommand;
class QBluetoothSocket;
class QIODevice;
class MOrderPrinter : public QObject
{
    Q_OBJECT
public:
    MOrderPrinter(QObject *parent = nullptr);

    void printTakeoutOrder(const QString &orderJson);
    void printTakeoutOrder(QObject *orderObj);
    void printTakeoutOrder(const QJsonObject &jsonObj);
    void printTest();

    //void initPrintCom(QBluetoothSocket *bluetoothSocket);
    void initPrintCom(QIODevice *socket);
    void setLetterPerLine(int lpl);
    int letterPerLine() const;

signals:

public slots:

private:
    MPrintCommand *_printCom;
    int _letterPerLine;
};

#endif // MORDERPRINTER_H
