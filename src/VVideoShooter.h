/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VVIDEOSHOOTER_H
#define _VVIDEOSHOOTER_H

#include "VScreenShooter.h"

class VVideoShooter : public VScreenShooter
{
    Q_OBJECT
public:
    VVideoShooter();
    VVideoShooter(const QWidget * widget, const QString &dirName, int frequency);
    virtual ~VVideoShooter();
    void setFrequency(int frequency);
    int getFrequency() const;
    virtual const QString& getDirName() const override;
    virtual void setDirName(const QString &dirName) override;
    const QString& getVideoFileName() const;
    bool isSaving() const;

public slots:
    virtual void start() override;
    virtual void stop() override;
private slots:
    void updateFrequency();
private:
    QString m_videoDirectory;
    QString m_videoFileName;
    int m_frequency;
    std::shared_ptr<std::thread> m_pSavingThread;
    std::atomic<bool> m_isSaving;

    inline void constructorBody();
    inline bool createFileName();
    void saveVideoProcess();
    void waitForSaving();
protected:
    static const char * const VIDEO_FORMAT_C;
    static const QString VIDEO_FORMAT;
    static const QString BASE_VIDEO_FILE_NAME;
    static const QString BASE_SLIDES_DIR_NAME;
    static const QString SLIDES_DIR_PATH;
signals:
    void frequencyChanged();
    void videoSavingStarted();
    void videoSavingFinished(bool result);
};

#endif //_VVIDEOSHOOTER_H
