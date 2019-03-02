/* *****************************
 * 蓝牙打印命令接口，封装 ESC POS指令
 * 2019-02-20
 * magic
 * *****************************/

#include "mbluetoothprintcommand.h"
#include <QTextCodec>
#include "cbinarization.h"

MBluetoothPrintCommand::MBluetoothPrintCommand(QBluetoothSocket *bluetoothSocket, QObject *parent)
    : QObject(parent)
    , p_bluetoothSocket(bluetoothSocket)
{

}

void MBluetoothPrintCommand::setBluetoothSocket(QBluetoothSocket *bluetoothSocket)
{
    p_bluetoothSocket = bluetoothSocket;
}

void MBluetoothPrintCommand::initionPrint()
{
    commandByteArray.clear();
    commandByteArray.append(0x1B);
    commandByteArray.append(0x40);
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::closeSocket()
{
    if (p_bluetoothSocket)
        p_bluetoothSocket->close();
}

void MBluetoothPrintCommand::setAlignment(int n)
{
    commandByteArray.clear();
    commandByteArray.append(0x1B);
    commandByteArray.append(0x61);
    commandByteArray.append((char)n);
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::setLineSpacing(int n)
{
    commandByteArray.clear();
    commandByteArray.append(0x1B);
    commandByteArray.append(0x33);
    commandByteArray.append((char)n);
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::setPrintMode(int font, int bold, int doubleheight, int doublewidth, int underline)
{
    char fontMod = 0x00;
    if (font > 0) {
        fontMod |= 0x01;
    }

    if (bold > 0) {
        fontMod |= 0x08;
    }

    if (doubleheight > 0) {
        fontMod |= 0x10;
    }

    if (doublewidth > 0) {
        fontMod |= 0x20;
    }

    if (underline > 0) {
        fontMod |= 0x80;
    }

    commandByteArray.clear();
    commandByteArray.append(0x1B);
    commandByteArray.append(0x21);
    commandByteArray.append(fontMod);
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::printBitmapwithM(int m, int xL, int xH, int yL, int yH, QByteArray data)
{
    CBinarization* binImg = new CBinarization(":/20180520000759880.png");
    commandByteArray.clear();
    commandByteArray = binImg->getBitmapData();
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::printBitmapwithPath(const QString &path)
{
    CBinarization* binImg = new CBinarization(path);
    commandByteArray.clear();
    commandByteArray = binImg->getBitmapData();
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::addText(const QString &txt)
{
    QTextCodec *codec = QTextCodec::codecForName("gbk");
    QByteArray encodedString = codec->fromUnicode(txt);
    p_bluetoothSocket->write(encodedString);
}

void MBluetoothPrintCommand::printContent()
{
    commandByteArray.clear();
    commandByteArray.append(0x0A);
    p_bluetoothSocket->write(commandByteArray);
}

void MBluetoothPrintCommand::printAndFeedLines(int n)
{
    commandByteArray.clear();
    //设置横向和纵向移动单位
    commandByteArray.append(0x1D);
    commandByteArray.append(0x50);
    commandByteArray.append((char)0);
    commandByteArray.append((char)10);

    //打印并走纸
    commandByteArray.append(0x1B);
    commandByteArray.append(0x4A);
    commandByteArray.append((char)n);
    commandByteArray.append(0x0A);
    p_bluetoothSocket->write(commandByteArray);
}
