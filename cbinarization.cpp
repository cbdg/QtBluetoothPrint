/* **********************************
 * 图像的二值化处理，即图片黑白打印
 * 作者：永远的魔术1号
 * 来源：CSDN
 * 原文：https://blog.csdn.net/zhango5/article/details/80378923
 * 版权声明：本文为博主原创文章，转载请附上博文链接！
 * 2019-02-20
 * *********************************/

#include "cbinarization.h"
#include <QDebug>

#define DPL 384 // 打印密度 384点/行

CBinarization::CBinarization( QString imgPath )
    : threshold(0)
{
    // 加载图片
    _img = new QImage();
    if (NULL != _img)
    {
        bool re = _img->load(imgPath);
    }
}

// 生成灰度图像
QImage* CBinarization::grayScaleImg()
{
    if (NULL == _img)
    {
        return NULL;
    }
//    int nWidth = _img->width();
//    int nHeight = _img->height();

//    // 这里留意，我使用了QImage::Format_Grayscale8格式的图片，这种图片每个像素只用了8bit存储灰度颜色值
//    QImage* grayImg = new QImage(nWidth, nHeight, QImage::Format_Grayscale8);
//    QRgb rgbVal = 0;
//    int grayVal = 0;

//    for (int x = 0; x < nWidth; ++x)
//    {
//        for (int y = 0; y < nHeight; ++y)
//        {
//            rgbVal = _img->pixel(x, y);

//            grayVal = qGray(rgbVal);    // 这里调用Qt的函数，使用(R * 11 + G * 16 + B * 5)/32的方法计算

//            grayImg->setPixel(x, y, QColor(grayVal, grayVal, grayVal).rgb());
//        }
//    }
    QImage* grayImg = new QImage(_img->convertToFormat(QImage::Format_Grayscale8));
    return grayImg;
}

int CBinarization::Otsu(QImage* img)
{
    if (NULL == img)
    {
        return -1;
    }
    vector<int> histogram = Histogram(img);
    int total = 0;
    for (int i = 0; i != histogram.size(); ++i)
    {
        total += histogram[i];
    }

    double sum = 0.0;
    for (unsigned int i = 1; i < histogram.size(); ++i)
        sum += i * histogram[i];
    double sumB = 0.0;
    double wB = 0.0;
    double wF = 0.0;
    double mB = 0.0;
    double mF = 0.0;
    double max = 0.0;
    double between = 0.0;
    double threshold = 0.0;
    for (int i = 0; i != 256; ++i)
    {
        wB += histogram[i];
        if (wB == 0)
            continue;
        wF = total - wB;
        if (wF == 0)
            break;
        sumB += i * histogram[i];
        mB = sumB / wB;
        mF = (sum - sumB) / wF;
        between = wB * wF * (mB - mF) * (mB - mF);
        if ( between > max )
        {
            threshold = i;
            max = between;
        }
    }
    return threshold;
}

QImage* CBinarization::process(QImage* img)
{
    if (NULL == img)
    {
        return NULL;
    }
    int width = img->width();
    int height = img->height();
    int bytePerLine = img->bytesPerLine();    // 每一行的字节数
    unsigned char *data = img->bits();
    unsigned char *binarydata = new unsigned char[bytePerLine * height];

    unsigned char g = 0;
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            g = *(data + i * bytePerLine + j);
            if(int(g) >= threshold)
            {
                binarydata[ i * bytePerLine + j] = 0xFF;
            }
            else
            {
                binarydata[ i * bytePerLine + j] = 0x00;
            }
        }
    }
    QImage *ret = new QImage(binarydata, width, height, bytePerLine, QImage::Format_Grayscale8);
    return ret;
}

QByteArray CBinarization::getBitmapData()
{
    QByteArray imgByteArray;
    imgByteArray.clear();

    QImage *img = grayScaleImg();
    if (NULL == img)
    {
        return imgByteArray;
    }

    QImage bitimg;
    if (img->width() > DPL) {
        bitimg = img->scaledToWidth(DPL);
        delete img;
        img = &bitimg;
    }

    int _threshold = Otsu(img);    // 这就是计算出的阈值
    if (-1 == threshold)
    {
        return imgByteArray;    // error
    }
    threshold = _threshold;
    if (threshold == 0)
        threshold = 127;

    int width = img->width();
    int height = img->height();
    int bytePerLine = img->bytesPerLine();    // 每一行的字节数
    unsigned char *data = img->bits();
    //unsigned char *binarydata = new unsigned char[bytePerLine * height];
    int bitW = (bytePerLine + 7) / 8;
    int bitH = (height + 23) / 24 * 24;

    imgByteArray.append(29);
    imgByteArray.append(118);
    imgByteArray.append(48);
    imgByteArray.append((char)0);
    imgByteArray.append((char)(bitW % 256));
    imgByteArray.append((char)(bitW / 256));
    imgByteArray.append((char)(bitH % 256));
    imgByteArray.append((char)(bitH / 256));

    unsigned char g = 0;
    for (int i = 0; i < bitH; ++i)
    {
        for (int j = 0; j < bitW; ++j)
        {
            char bit = 0x00;
            for (int bitj = 0; bitj < 8; bitj++) {
                int b = 0;
                int image_x = j * 8 + bitj;
                if (image_x >=  width) {
                    b = 0;
                } else if (i >= height) {
                    b = 0;
                }else {
                    g = *(data + i * bytePerLine + image_x);
                    if(int(g) >= threshold)
                    {
                        b = 0;
                    }
                    else
                    {
                        b = 1;
                    }
                }
                bit |= (b << (7 - bitj));
            }
            imgByteArray.append(bit);
        }
    }
    imgByteArray.append(0x0A);
    return imgByteArray;
}

std::vector<int> CBinarization::Histogram( QImage* img )
{
    unsigned char* graydata = img->bits();
    vector<int> hist(256);    // 256色
    for (int i = 0; i != img->width(); ++i)
    {
        for (int j = 0; j != img->height(); ++j)
        {
            int index = int(*graydata);
            hist[index] += 1;
            graydata += 1;    // step
        }
    }
    graydata = NULL;
    return hist;
}
