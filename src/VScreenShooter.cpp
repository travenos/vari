/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <functional>
#include <algorithm>

#include <QScreen>
#include <QWindow>
#include <QPixmap>
#include <QWidget>
#include <QDir>

#if defined(Q_WS_WIN) || defined (WIN32) || defined(__WIN32__)
#include <windows.h>
#include <QtWin>
#define SLEEP(period) Sleep(period)
#elif defined(linux) || defined(__linux__) || defined(Q_WS_MAC)
#include <unistd.h>
#define SLEEP(period) usleep(period * 1000)
#else
#define SLEEP(period) std::this_thread::sleep_for(std::chrono::milliseconds(period))
#endif

#if defined(Q_WS_WIN) || defined (WIN32) || defined(__WIN32__)
inline void getWinAPIscreen(int wx1, int wy1, int wx2, int wy2, QPixmap &pixmap)
{
	POINT a, b;
	a.x = wx1;
	a.y = wy1;
	b.x = wx2;
	b.y = wy2;
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, abs(b.x - a.x), abs(b.y - a.y));
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, abs(b.x - a.x), abs(b.y - a.y), hScreen, a.x, a.y, SRCCOPY);

	pixmap = QtWin::fromHBITMAP(hBitmap);

	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
}
#endif

#include "VScreenShooter.h"

/**
 * VScreenShooter implementation
 */

const char * const VScreenShooter::PICTURE_FORMAT_C = "png";
const QString VScreenShooter::PICTURE_FORMAT(VScreenShooter::PICTURE_FORMAT_C);
const QString VScreenShooter::BASE_NAME = QStringLiteral("slide_%1.") + VScreenShooter::PICTURE_FORMAT;
const QString VScreenShooter::SLIDES_SUFFIX_DIR_NAME("VARI_slideshow");
const int VScreenShooter::FILENAME_TIME_PRECISION = 4;

VScreenShooter::VScreenShooter():
    m_pWidget(nullptr),
    m_baseDirName(QDir::homePath()),
    m_period(1.0f)
{
    constructorBody();
}

VScreenShooter::VScreenShooter(const QWidget * widget, const QString &dirName, float period):
    m_pWidget(widget),
    m_baseDirName(dirName),
    m_period(period)
{
    constructorBody();
}

inline void VScreenShooter::constructorBody()
{
    setPeriod(m_period);
    setDirName(m_baseDirName);
    m_isWorking.store(false);
    connect(this, SIGNAL(shouldBeStopped()), this, SLOT(stop()));
}

VScreenShooter::~VScreenShooter()
{
    stop();
}

void VScreenShooter::start()
{
    if (!m_isWorking.load())
    {
        bool widgetNotNull = (m_pWidget != nullptr);
        bool dirExists = createWorkDir();
        bool periodOk = (m_msPeriod > 0);
        if (widgetNotNull && dirExists && periodOk)
        {
            if (m_takePictureThread && m_takePictureThread->joinable())
                stop();
            m_isWorking.store(true);
            m_pStopFlag.reset(new std::atomic<bool>(false));
            m_startTime = clock_t::now();
            m_takePictureThread.reset(new std::thread(std::bind(&VScreenShooter::pictureCycle, this)));
            emit processStarted();
        }
    }
}

void VScreenShooter::stop()
{
    if (m_isWorking.load())
    {        
        m_pStopFlag->store(true);
        if (m_processLock.try_lock())
        {
            if (m_takePictureThread && m_takePictureThread->joinable())
                m_takePictureThread->detach();
            m_processLock.unlock();
        }
        else
        {
            if (m_takePictureThread && m_takePictureThread->joinable())
                m_takePictureThread->join();
        }
        m_takePictureThread.reset();
        m_isWorking.store(false);
        emit processStopped();
    }
}

bool VScreenShooter::isWorking() const
{
    return m_isWorking.load();
}

void VScreenShooter::setWidget(const QWidget * widget)
{
    std::lock_guard<std::mutex> locker(m_processLock);
    m_pWidget = widget;
    emit widgetChanged();
}

void VScreenShooter::setDirName(const QString &dirName)
{
    std::lock_guard<std::mutex> locker(m_processLock);
    m_baseDirName = dirName;
    emit directoryChanged();
}

void VScreenShooter::setPeriod(float period)
{
    m_period = period;
    const int MS_IN_S = 1000;
    const float ROUND_CONST = 0.5f;
    int msPeriod = static_cast<int>(m_period * MS_IN_S + ROUND_CONST);
    msPeriod = std::max(msPeriod, 0);
    if (msPeriod < 0)
        stop();
    m_msPeriod = msPeriod;
    emit periodChanged();
}

const QWidget * VScreenShooter::getWidget() const
{
    return m_pWidget;
}

const QString& VScreenShooter::getDirName() const
{
    return m_baseDirName;
}

const QString& VScreenShooter::getSlidesDirName() const
{
    return m_workDirName;
}

float VScreenShooter::getPeriod() const
{
    return m_period;
}

void VScreenShooter::pictureCycle()
{
    std::shared_ptr< std::atomic<bool> > stopFlag = m_pStopFlag;
    auto scrStart = clock_t::now();
    takePictureWrapper(stopFlag);
    int lastProcessDuration = std::chrono::duration_cast<ms_int_t>(clock_t::now() - scrStart).count();
    int period;
    while(!stopFlag->load())
    {
        period = m_msPeriod - lastProcessDuration;
        period = std::max(period, 0);
        SLEEP(period);
        scrStart = clock_t::now();
        takePictureWrapper(stopFlag);
        lastProcessDuration = std::chrono::duration_cast<ms_int_t>(clock_t::now() - scrStart).count();
    }
}

inline void VScreenShooter::takePictureWrapper(const std::shared_ptr< std::atomic<bool> > & stopFlag)
{
    bool result = true;
    {
        std::lock_guard<std::mutex> locker(m_processLock);
        if(!stopFlag->load())
        {
            if (areParametersCorrect())
            {
                double seconds = std::chrono::duration_cast<second_t>(clock_t::now() - m_startTime).count();
                QString secondsStr = QString::number(seconds, 'f', FILENAME_TIME_PRECISION);
                QString fileName = QDir::cleanPath(m_workDirName + QDir::separator() + BASE_NAME.arg(secondsStr));
                result = takePicture(fileName);
            }
            else
                result = false;
        }
    }
    if (!result)
        emit shouldBeStopped();
}

bool VScreenShooter::takePicture(const QString &fileName) const
{
    QScreen *screen = m_pWidget->windowHandle()->screen();
    if (!screen)
        return false;

    int wx1, wy1, wx2, wy2, sx1, sy1, sx2, sy2;
    QRect widgetRect = m_pWidget->geometry();
    widgetRect.moveTopLeft(m_pWidget->parentWidget()->mapToGlobal(widgetRect.topLeft()));
    widgetRect.getCoords(&wx1, &wy1, &wx2, &wy2);
    QRect screenRect = screen->geometry();
    screenRect.getCoords(&sx1, &sy1, &sx2, &sy2);
    if (wx1 < sx1 || wy1 < sy1 || wx2 > sx2 || wy2 > sy2)
        return false;
	QPixmap originalPixmap;
	#if defined(Q_WS_WIN) || defined (WIN32) || defined(__WIN32__)
	getWinAPIscreen(wx1, wy1, wx2, wy2, originalPixmap);
	#else
	originalPixmap = screen->grabWindow(m_pWidget->winId());
	#endif
    bool ok = originalPixmap.save(fileName, PICTURE_FORMAT_C);
    return ok;
}

inline bool VScreenShooter::areParametersCorrect() const
{
    bool widgetNotNull = (m_pWidget != nullptr);
    bool dirExists = (!m_workDirName.isEmpty() && QDir(m_workDirName).exists());
    bool periodOk = (m_msPeriod > 0);
    return (widgetNotNull && dirExists && periodOk);
}

inline bool VScreenShooter::createWorkDir()
{
    if (m_baseDirName.isEmpty())
        return false;
    QString originalPath = QDir::cleanPath(m_baseDirName + QDir::separator() + SLIDES_SUFFIX_DIR_NAME);
    QString slideshowPath = originalPath;
    QDir slideshowDir(slideshowPath);
    int counter = 0;
    while (slideshowDir.exists())
    {
        ++counter;
        slideshowPath = originalPath + QString::number(counter);
        slideshowDir.setPath(slideshowPath);
    }
    bool result = slideshowDir.mkpath(slideshowPath);
    m_workDirName = slideshowPath;
    return result;
}
