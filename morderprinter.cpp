#include "morderprinter.h"
#include <QBluetoothSocket>
#include "mprintcommand.h"

MOrderPrinter::MOrderPrinter(QObject *parent)
    : QObject(parent)
    , _letterPerLine(32)
{
}

void MOrderPrinter::printTakeoutOrder(const QString &orderJson)
{

}

void MOrderPrinter::printTakeoutOrder(QObject *orderObj)
{
    _printCom->initionPrint();

    //app title
    _printCom->setAlignment(1);
    _printCom->setPrintMode(0, 0, 1, 1, 0);
    _printCom->addText("#88 阿宝外卖");
    _printCom->printContent();

    // shop name
    _printCom->setAlignment(1);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("*尊宝比萨（石夏店）*");
    _printCom->printContent();

    // pay type
    _printCom->setAlignment(1);
    _printCom->setPrintMode(0, 0, 1, 1, 0);
    _printCom->addText("--已在线支付--");
    _printCom->printContent();

    // delivery_time
    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("期望送达时间：立即送餐");
    _printCom->printContent();

    // seprate line
    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("--------------------------------");
    _printCom->printContent();

    // pay time
    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("下单时间：2019-02-23 11：12：30");
    _printCom->printContent();

    // order Id
    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("订单编号：5217800961309660");
    _printCom->printContent();

    // seprate line
    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("--------------------------------");
    _printCom->printContent();
}


void MOrderPrinter::printTest()
{
    _printCom->initionPrint();

    _printCom->setAlignment(1);
    _printCom->setPrintMode(0, 1, 1, 1, 1);
    _printCom->addText("阿宝打印测试");
    _printCom->printContent();

    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 1, 0, 0);
    _printCom->addText("阿宝打印测试");
    _printCom->printContent();

    _printCom->setAlignment(2);
    _printCom->setPrintMode(0, 0, 0, 1, 0);
    _printCom->addText("阿宝打印测试");
    _printCom->printContent();

    _printCom->setAlignment(0);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("--------------------------------");
    _printCom->printContent();

    _printCom->setAlignment(1);
    _printCom->printBitmapwithPath(":/20190304164724.png");
    _printCom->initionPrint();

    _printCom->setAlignment(1);
    _printCom->setPrintMode(0, 0, 0, 0, 0);
    _printCom->addText("--------- 打印测试完成 ---------");
    _printCom->printContent();

    _printCom->printAndFeedLines(5);
}

//void MOrderPrinter::initPrintCom(QBluetoothSocket *bluetoothSocket)
void MOrderPrinter::initPrintCom(QIODevice *socket)
{
    _printCom = new MPrintCommand(socket, this);
}

void MOrderPrinter::setLetterPerLine(int lpl)
{
    _letterPerLine = lpl;
}

int MOrderPrinter::letterPerLine() const
{
    return _letterPerLine;
}
