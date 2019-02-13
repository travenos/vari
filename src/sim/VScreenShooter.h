/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VSCHREENSHOOTER_H
#define _VSCHREENSHOOTER_H

#include <memory>
#include <mutex>
#include <thread>
#include <chrono>

#include <QObject>
#include <QTimer>
#include <QThread> //TODO

class VScreenShooter : public QObject
{
    Q_OBJECT
public:
    VScreenShooter();
    VScreenShooter(const QWidget * widget, const QString &dirName, float period);
    virtual ~VScreenShooter();
    VScreenShooter(const VScreenShooter& ) = delete;
    VScreenShooter& operator= (const VScreenShooter& ) = delete;
    VScreenShooter(VScreenShooter&& temp) = delete;
    VScreenShooter& operator=(VScreenShooter&& temp) = delete;

    void start();
    void stop();
    bool isWorking() const;

    bool takePicture(const QString &fileName) const;
    inline bool areParametersCorrect() const;
    inline bool createDirIfNecessary() const;

    void setWidget(const QWidget * widget);
    void setDirName(const QString &dirName);
    void setPeriod(float period);
    const QWidget * getWidget() const;
    const QString& getDirName() const;
    float getPeriod() const;
private:
    typedef std::chrono::duration<double, std::ratio<1> > second_t;
    typedef std::chrono::high_resolution_clock clock_t;
    typedef std::chrono::time_point<clock_t> time_point_t;

    static const QString BASE_NAME;
    static const int TIME_PRECISION;

    //TODO QThread
    const QWidget * m_pWidget;
    QString m_dirName;
    float m_period;
    int m_msPeriod;
    QTimer * m_pTimer;
    std::atomic<bool> m_isWorking;
    std::atomic<bool> m_stopFlag;
    mutable std::mutex m_timerLock;
    mutable std::mutex m_pictureLock;
    std::unique_ptr<std::thread> m_takePictureThread;
    time_point_t m_startTime;

    inline void constructorBody();
    void takePictureWrapper();
private slots:
    void timerSlot();
signals:
    void processStarted();
    void processStopped();
};

#endif //_VSCHREENSHOOTER_H
