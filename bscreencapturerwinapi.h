#ifndef BSCREENCAPTURERWINAPI_H
#define BSCREENCAPTURERWINAPI_H

#include <Windows.h>
#include <QObject>
#include <QPixmap>
#include <QTimer>

class BScreenCapturerWinAPI : public QObject
{
    Q_OBJECT
public:
    explicit BScreenCapturerWinAPI(QString pProjectName, uint8_t pFPS, QObject *parent = nullptr);
    ~BScreenCapturerWinAPI();
    bool start();

signals:
    void onFrame(QImage pFrame);

private:
    bool findWindow();
    bool extractClientSize();
    bool extractDC();

private slots:
    bool extractImage();


private:
    //windows
    HWND mHWNDWindow;
    RECT mClientRect;
    HDC mHDCScreen;
    HDC mHDC;
    //
    QTimer *mTimer;
    uint8_t mFPS;
    uint16_t mClientWidth;
    uint16_t mClientHeight;
    QString mProjectName;
};

#endif // BSCREENCAPTURERWINAPI_H
