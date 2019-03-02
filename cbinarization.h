#ifndef CBINARIZATION_H
#define CBINARIZATION_H

#include <QImage>
#include <QColor>

using namespace std;

class CBinarization
{
public:
    CBinarization(QString imgPath);

    QImage* grayScaleImg();
    int Otsu(QImage* img);
    QImage* process(QImage* img);
    int     threshold;
    QByteArray getBitmapData();

private:
    QImage* _img;

    vector<int> Histogram(QImage* img);
};

#endif // CBINARIZATION_H
