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
    VVideoShooter(const QWidget * widget, const QString &dirPath, int frequency);
    virtual ~VVideoShooter();
    void setFrequency(int frequency);
    int getFrequency() const;
    virtual const QString& getDirPath() const override;
    virtual void setDirPath(const QString &dirPath) override;
    void setSuffixFileName(const QString &name);
    const QString& getSuffixFileName() const;
    const QString& getVideoFilePath() const;
    bool isSaving() const;

public slots:
    virtual void start() override;
    virtual void stop() override;
private slots:
    void updateFrequency();
private:
    QString m_videoDirectory;
    QString m_videoFilePath;
    QString m_suffixFileName;
    int m_frequency;
    std::shared_ptr<std::thread> m_pSavingThread;
    std::atomic<bool> m_isSaving;

    inline void constructorBody();
    inline bool createFilePath();
    void saveVideoProcess();
    void waitForSaving();
protected:
    static const char * const VIDEO_FORMAT_C;
    static const int VIDEO_CODEC;
    static const QString VIDEO_FORMAT;
    static const QString DEFAULT_SUFFIX_FILE_NAME;
    static const QString BASE_VIDEO_FILE_NAME;
    static const QString BASE_SLIDES_DIR_NAME;
    static const QString SLIDES_DIR_PATH;
signals:
    void frequencyChanged();
    void videoSavingStarted();
    void videoSavingFinished(bool result);
    void suffixFileNameChanged();
};

#endif //_VVIDEOSHOOTER_H
