/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VVIDEOSHOOTER_H
#define _VVIDEOSHOOTER_H

#include "VScreenShooter.h"

//TODO
//class VVideoException: public std::exception
//{
//    const QString m_msg;
//public:
//    VVideoException() = default;
//    VVideoException(const QString &msg):
//        m_msg(msg)
//    {}
//    const QString &what()
//    {return m_msg;}
//};

class VVideoShooter : public VScreenShooter
{
    Q_OBJECT
public:
    VVideoShooter();
    VVideoShooter(const QWidget * widget, const QString &dirName, int frequency);
    virtual ~VVideoShooter();

    void setFrequency(int frequency);
    int getFrequency() const;
    bool saveVideo();
    virtual const QString& getDirName() const override;
    virtual void setDirName(const QString &dirName) override;
    const QString& getVideoFileName() const;
public slots:
    virtual void start() override;
    virtual void stop() override;
private slots:
    void updateFrequency();
private:
    QString m_videoDirectory;
    QString m_videoFileName;
    int m_frequency;

    inline void constructorBody();
    inline bool createFileName();
protected:
    static const char * const VIDEO_FORMAT_C;
    static const QString VIDEO_FORMAT;
    static const QString BASE_VIDEO_FILE_NAME;
    static const QString BASE_SLIDES_DIR_NAME;
    static const QString SLIDES_DIR_PATH;
signals:
    void frequencyChanged();
    void videoSavingFinished(bool result);
};

#endif //_VVIDEOSHOOTER_H
