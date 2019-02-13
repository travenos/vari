/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include <functional>

#include <QScreen>
#include <QWindow>
#include <QPixmap>
#include <QWidget>
#include <QDir>

#include "VScreenShooter.h"

/**
 * VScreenShooter implementation
 */

const QString VScreenShooter::BASE_NAME("slide_%1.png");
const int VScreenShooter::TIME_PRECISION = 4;

VScreenShooter::VScreenShooter():
    m_pWidget(nullptr),
    m_dirName(""),
    m_period(0)
{
    constructorBody();
}

VScreenShooter::VScreenShooter(const QWidget * widget, const QString &dirName, float period):
    m_pWidget(widget),
    m_dirName(dirName),
    m_period(period)
{
    constructorBody();
}

inline void VScreenShooter::constructorBody()
{
    setPeriod(m_period);
    m_stopFlag.store(true);
    m_isWorking.store(false);
    m_pTimer = new QTimer(this);
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(timerSlot()));
}

VScreenShooter::~VScreenShooter()
{
    stop();
    m_pTimer->deleteLater();
}

void VScreenShooter::start()
{
    if (!m_isWorking.load())
    {
        bool widgetNotNull = (m_pWidget != nullptr);
        bool dirExists = createDirIfNecessary();
        bool periodOk = (m_msPeriod > 0);
        if (widgetNotNull && dirExists && periodOk)
        {
            std::lock_guard<std::mutex> locker(m_timerLock);
            m_pTimer->setInterval(m_msPeriod);
            m_stopFlag.store(false);
            m_isWorking.store(true);
            m_startTime = clock_t::now();
            m_pTimer->start();
            emit processStarted();
        }
    }
}

void VScreenShooter::stop()
{
    if (m_isWorking.load())
    {
        {
            std::lock_guard<std::mutex> locker(m_timerLock);
            m_pTimer->stop();
        }
        m_stopFlag.store(true);
        if (m_takePictureThread && m_takePictureThread->joinable())
            m_takePictureThread->join();
        m_takePictureThread.reset();
        m_isWorking.store(false);
        emit processStopped();
    }
}

bool VScreenShooter::isWorking() const
{
    return m_isWorking.load();
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
    QPixmap originalPixmap = screen->grabWindow(m_pWidget->winId());
    bool ok = originalPixmap.save(fileName, "png");
    return ok;
}

void VScreenShooter::setWidget(const QWidget * widget)
{
    std::lock_guard<std::mutex> locker(m_pictureLock);
    m_pWidget = widget;
}

void VScreenShooter::setDirName(const QString &dirName)
{
    std::lock_guard<std::mutex> locker(m_pictureLock);
    m_dirName = dirName;
    createDirIfNecessary();
}

void VScreenShooter::setPeriod(float period)
{
    m_period = period;
    const int MS_IN_S = 1000;
    const float ROUND_CONST = 0.5f;
    m_msPeriod = static_cast<int>(m_period * MS_IN_S + ROUND_CONST);
    m_msPeriod = std::max(m_msPeriod, 0);
    if (m_msPeriod > 0)
    {
        std::lock_guard<std::mutex> locker(m_timerLock);
        m_pTimer->setInterval(m_msPeriod);
    }
    else
        stop();
}

const QWidget * VScreenShooter::getWidget() const
{
    return m_pWidget;
}

const QString& VScreenShooter::getDirName() const
{
    return m_dirName;
}

float VScreenShooter::getPeriod() const
{
    return m_period;
}

void VScreenShooter::timerSlot()
{
    if (m_takePictureThread && m_takePictureThread->joinable())
        m_takePictureThread->join();
    m_takePictureThread.reset(new std::thread(std::bind(&VScreenShooter::takePictureWrapper, this)));
}

void VScreenShooter::takePictureWrapper()
{
    if (!m_stopFlag.load())
    {
        bool result = false;
        {
            std::lock_guard<std::mutex> locker(m_pictureLock);
            if (areParametersCorrect())
            {
                double seconds = std::chrono::duration_cast<second_t>(clock_t::now() - m_startTime).count();
                QString secondsStr = QString::number(seconds, 'g', TIME_PRECISION);
                QString fileName = QDir::cleanPath(m_dirName + QDir::separator() + BASE_NAME.arg(secondsStr));
                result = takePicture(fileName);
            }
            else
                result = false;
        }
        if (!result)
            stop();
    }
}

inline bool VScreenShooter::areParametersCorrect() const
{
    bool widgetNotNull = (m_pWidget != nullptr);
    bool dirExists = (!m_dirName.isEmpty() && QDir(m_dirName).exists());
    bool periodOk = (m_msPeriod > 0);
    return (widgetNotNull && dirExists && periodOk);
}

inline bool VScreenShooter::createDirIfNecessary() const
{
    if (m_dirName.isEmpty())
        return false;
    if (QDir(m_dirName).exists())
        return true;
    return QDir().mkpath(m_dirName);
}
