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

    bool isWorking() const;

    bool takePicture(const QString &fileName) const;
    inline bool areParametersCorrect() const;
    inline bool createWorkDir();

    void setWidget(const QWidget * widget);
    void setDirName(const QString &dirName);
    void setPeriod(float period);
    const QWidget * getWidget() const;
    const QString& getDirName() const;
    const QString& getSlidesDirName() const;
    float getPeriod() const;
private:
    typedef std::chrono::duration<double, std::ratio<1> > second_t;
    typedef std::chrono::duration<int, std::ratio<1, 1000> > ms_int_t;
    typedef std::chrono::high_resolution_clock clock_t;
    typedef std::chrono::time_point<clock_t> time_point_t;

    static const QString BASE_NAME;
    static const QString SLIDES_SUFFIX_DIR_NAME;
    static const int FILENAME_TIME_PRECISION;

    const QWidget * m_pWidget;
    QString m_baseDirName;
    QString m_workDirName;
    float m_period;
    std::atomic<int> m_msPeriod;
    std::atomic<bool> m_isWorking;
    mutable std::mutex m_processLock;
    std::unique_ptr<std::thread> m_takePictureThread;
    time_point_t m_startTime;

    std::shared_ptr< std::atomic<bool> > m_pStopFlag;

    void pictureCycle();
    inline void constructorBody();
    inline void takePictureWrapper(const std::shared_ptr<std::atomic<bool> > &stopFlag);
public slots:
    void start();
    void stop();
signals:
    void processStarted();
    void processStopped();
    void shouldBeStopped();
    void widgetChanged();
    void directoryChanged();
    void periodChanged();
};

#endif //_VSCHREENSHOOTER_H
