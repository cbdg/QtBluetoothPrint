#ifndef MRINTCOMMAND_H
#define MRINTCOMMAND_H

#include <QObject>
#include <QBluetoothSocket>
#include <QIODevice>
#include <QByteArray>

class MPrintCommand : public QObject
{
    Q_OBJECT
public:
    MPrintCommand(QIODevice *Socket, QObject *parent = nullptr);


    void setBluetoothSocket(QIODevice *bluetoothSocket);
    /**
     * 方法说明：打印机初始化，必须是第一个打印命令0x1b,0x40
     */
    void initionPrint();

    void closeSocket();

    /**
     * 方法说明：设置对齐方式
     * @param n 左 中 右对齐，0左对齐,1中间对齐,2右对齐
     */
    void setAlignment(int n);

    /**
     * 方法说明：设置行间距，不用设置，打印机默认为30
     * @param n  行间距高度，包含文字
     */
    void setLineSpacing(int n);

    /**
     * 方法说明：设置打印模式，0x1B 0x21 n(0-255)，根据n的值设置字符打印模式
     * @param font     选择FONTA or FONTB
     * @param emphasized    是否加粗
     * @param doubleheight  是否倍高，当倍宽和倍高模式同时选择时，字符同时在横向和纵向放大两倍。
     * @param doublewidth   是否倍宽
     * @param underline     是否下划线
        *参数n二进制默认为00000000(0X0),10001000(0X88)表示下划线和加粗，00001000(0X08)表示加粗，10000000(0X80)表示下划线
     */
    void setPrintMode(int font, int bold, int doubleheight, int doublewidth, int underline);

    /*
     十六进制码 1D 76 30 m xL xH yL yH d1...dk
     0 ≤ m ≤ 3, 48 ≤ m ≤ 51
     0 ≤ xL ≤ 255
     0 ≤ xH ≤ 255
     0 ≤ yL ≤ 255
     0 ≤ yH ≤ 255
     0 ≤ d ≤ 255
     k = ( xL + xH × 256) × ( yL + yH × 256) ( k ≠ 0)
     参 数 说 明
     m 模式
     0, 48 正常
     1, 49 倍宽
     2, 50 倍高
     3, 51 倍宽、倍高
     xL、 xH表示水平方向位图字节数（ xL+ xH × 256）
     yL、 yH表示垂直方向位图点数（  yL+ yH × 256）
     data 影像数据
     */
    void printBitmapwithM(int m, int xL, int xH, int yL, int yH, QByteArray data);
    void printBitmapwithPath(const QString &path);

    /*
     * 添加文本
     * */
    void addText(const QString &txt);

    void printContent();

    /**
     * 方法说明：打印并且走纸多少行，默认为8行，打印完内容后发送
     * @param n 行数
     */
    void printAndFeedLines(int n);

private:
    QIODevice *p_socket;
    QByteArray commandByteArray;
};

#endif // MRINTCOMMAND_H
