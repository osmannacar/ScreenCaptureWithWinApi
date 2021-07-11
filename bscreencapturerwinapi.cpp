#include "bscreencapturerwinapi.h"
#include <QDebug>
#include <QtWin>

BScreenCapturerWinAPI::BScreenCapturerWinAPI(QString pProjectName, uint8_t pFPS, QObject *parent)
    : QObject(parent)
    , mProjectName(pProjectName)
    , mFPS(pFPS)
    , mHWNDWindow(NULL)
    , mClientRect{0} // assign default value in struct
    , mHDCScreen(NULL)
    , mHDC(NULL)
    , mTimer(nullptr)
    , mClientWidth(0)
    , mClientHeight(0)
{
}

BScreenCapturerWinAPI::~BScreenCapturerWinAPI()
{
    if(mTimer){
        mTimer->stop();
        mTimer->deleteLater();
    }
    ::ReleaseDC(NULL, mHDCScreen);
    ::DeleteDC(mHDC);
}

bool BScreenCapturerWinAPI::start()
{
    if(mTimer){
        qDebug() << "BScreenCapturerWinAPI::start has been started";
        return false;
    }

    if(!findWindow()){
        qDebug() << "BScreenCapturerWinAPI::start error at findWindow()";
        return false;
    }

    if(!extractClientSize()){
        qDebug() << "BScreenCapturerWinAPI::start error at extractClientSize()";
        return false;
    }

    if(!extractDC()){
        qDebug() << "BScreenCapturerWinAPI::start error at extractDC()";
        return false;
    }

    extractImage();
    mTimer = new QTimer(this);
    QObject::connect(mTimer, &QTimer::timeout, this, &BScreenCapturerWinAPI::extractImage);
    mTimer->start(1000 / mFPS);

    return true;
}

bool BScreenCapturerWinAPI::findWindow()
{
    mHWNDWindow = ::FindWindow(0, mProjectName.toStdWString().c_str());
    if(mHWNDWindow == NULL){
        qDebug() << "BScreenCapturerWinAPI::findWindow can not find any aplication name of " << mProjectName;
        return false;
    }

    return true;
}

bool BScreenCapturerWinAPI::extractClientSize()
{
    if(!::GetClientRect(mHWNDWindow, &mClientRect)){
        qDebug() << "BScreenCapturerWinAPI::extractClientRect error while extracting ClientRect";
        return false;
    }

    mClientWidth = mClientRect.right - mClientRect.left;
    mClientHeight = mClientRect.bottom - mClientRect.top;

    return true;
}

bool BScreenCapturerWinAPI::extractDC()
{
    mHDCScreen = ::GetDC(mHWNDWindow);
    if(mHDCScreen == NULL){
        qDebug() << "BScreenCapturerWinAPI::extractDC error while extracting mHDCScreen";
        return false;
    }

    mHDC = ::CreateCompatibleDC(mHDCScreen);
    if(mHDC == NULL){
        qDebug() << "BScreenCapturerWinAPI::extractDC error while extracting mHDC";
        return false;
    }

    return true;
}

bool BScreenCapturerWinAPI::extractImage()
{
    ::SetForegroundWindow(mHWNDWindow);
    ::SetFocus(mHWNDWindow); //Just playing safe

    HBITMAP tBitmap = ::CreateCompatibleBitmap(mHDCScreen, mClientWidth, mClientHeight);
    if(tBitmap == NULL){
        qDebug() << "BScreenCapturerWinAPI::extractImage error while creating tBitmap";
        return false;
    }

    ::SelectObject(mHDC, tBitmap);

    ::BitBlt(mHDC, 0, 0, mClientWidth, mClientHeight, mHDCScreen, 0, 0, SRCCOPY);

    BITMAPINFO bmp_info = { 0 };
    bmp_info.bmiHeader.biSize = sizeof(bmp_info.bmiHeader);
    bmp_info.bmiHeader.biWidth = mClientWidth;
    bmp_info.bmiHeader.biHeight = mClientHeight;
    bmp_info.bmiHeader.biPlanes = 1;
    bmp_info.bmiHeader.biBitCount = 32;
    bmp_info.bmiHeader.biCompression = BI_RGB;
    bmp_info.bmiHeader.biSizeImage = 0;// 3 * mClientWidth * mClientHeight;

    BYTE *bmp_pixels = new BYTE[4 * mClientWidth * mClientHeight];
    ::GetDIBits(mHDC, tBitmap, 0, mClientHeight, bmp_pixels, &bmp_info, DIB_RGB_COLORS);


    emit onFrame(QtWin::fromHBITMAP(tBitmap).toImage());

    //clear tBitmap
    ::DeleteObject(tBitmap);
    delete[] bmp_pixels;

    return true;
}
