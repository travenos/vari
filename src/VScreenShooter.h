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
#include <atomic>

#include <QObject>

class VScreenShooter : public QObject
{
    Q_OBJECT
public:
    VScreenShooter();
    VScreenShooter(const QWidget * widget, const QString &dirPath, float period);
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
    virtual void setDirPath(const QString &dirPath);
    void setPeriod(float period);
    void setSuffixDirName(const QString &name);
    const QWidget * getWidget() const;
    virtual const QString& getDirPath() const;
    const QString& getSlidesDirPath() const;
    float getPeriod() const;
    const QString& getSuffixDirName() const;
private:
    typedef std::chrono::duration<double, std::ratio<1> > second_t;
    typedef std::chrono::duration<int, std::ratio<1, 1000> > ms_int_t;
    typedef std::chrono::high_resolution_clock clock_t;
    typedef std::chrono::time_point<clock_t> time_point_t;

    const QWidget * m_pWidget;
    QString m_baseDirPath;
    QString m_suffixDirName;
    QString m_workDirPath;
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
    virtual void start();
    virtual void stop();
protected:
    static const char * const PICTURE_FORMAT_C;
    static const QString PICTURE_FORMAT;
    static const QString BASE_NAME;
    static const QString DEFAULT_SUFFIX_DIR_NAME;
    static const int FILENAME_TIME_PRECISION;
signals:
    void processStarted();
    void processStopped();
    void shouldBeStopped();
    void widgetChanged();
    void directoryChanged();
    void periodChanged();
    void suffixDirNameChanged();
};

#endif //_VSCHREENSHOOTER_H
