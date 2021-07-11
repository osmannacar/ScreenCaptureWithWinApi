#include <QApplication>
#include <QLabel>
#include <BScreenCapturerWinAPI.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QLabel *lbl = new QLabel;
    lbl->resize(1023,768);
    lbl->show();
    QString tApplicationName = "Calculator";

    BScreenCapturerWinAPI *screenCapturer = new BScreenCapturerWinAPI(tApplicationName, 25);

    QObject::connect(screenCapturer, &BScreenCapturerWinAPI::onFrame, lbl, [lbl](QImage img){
       lbl->setPixmap(QPixmap::fromImage(img.scaled(lbl->size())));
    });
    screenCapturer->start();

    return a.exec();
}
