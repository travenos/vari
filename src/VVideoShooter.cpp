/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <QDir>
#include <QPixmap>

#include <opencv2/opencv.hpp>

#include "VVideoShooter.h"

/**
 * VVideoShooter implementation
 */

const char * const VVideoShooter::VIDEO_FORMAT_C = "avi";
const QString VVideoShooter::VIDEO_FORMAT(VVideoShooter::VIDEO_FORMAT_C);
const QString VVideoShooter::BASE_VIDEO_FILE_NAME = QStringLiteral("VARI_video%1.") + VVideoShooter::VIDEO_FORMAT;
const QString VVideoShooter::BASE_SLIDES_DIR_NAME("_VARI_SLIDES_FOR_VIDEO_");
const QString VVideoShooter::SLIDES_DIR_PATH = QDir::cleanPath(QDir::tempPath() + QDir::separator() + VVideoShooter::BASE_SLIDES_DIR_NAME);

VVideoShooter::VVideoShooter():
    VScreenShooter()
{
    VScreenShooter::setDirName(SLIDES_DIR_PATH);
    updateFrequency();
    constructorBody();
}

VVideoShooter::VVideoShooter(const QWidget * widget, const QString &dirName, int frequency):
    VScreenShooter(widget, SLIDES_DIR_PATH, 1.0f/frequency),
    m_videoDirectory(dirName),
    m_frequency(frequency)
{
    constructorBody();
}

inline void VVideoShooter::constructorBody()
{
    connect(this, SIGNAL(periodChanged()), this, SLOT(updateFrequency()));
}

VVideoShooter::~VVideoShooter()
{
}

void VVideoShooter::setFrequency(int frequency)
{
    m_frequency = frequency;
    setPeriod(1.0f/frequency);
    emit frequencyChanged();
}

void VVideoShooter::updateFrequency()
{
    m_frequency = static_cast<int>(1 / getPeriod() + 0.5);
    emit frequencyChanged();
}

int VVideoShooter::getFrequency() const
{
    return m_frequency;
}

const QString& VVideoShooter::getDirName() const
{
    return m_videoDirectory;
}

void VVideoShooter::setDirName(const QString &dirName)
{
    m_videoDirectory = dirName;
    emit directoryChanged();
}

const QString& VVideoShooter::getVideoFileName() const
{
    return m_videoFileName;
}

void VVideoShooter::start()
{
    m_videoFileName.clear();
    VScreenShooter::start();
}

void VVideoShooter::stop()
{
    if (isWorking())
    {
        VScreenShooter::stop();
        bool result = saveVideo();
        emit videoSavingFinished(result);
    }
}

bool VVideoShooter::saveVideo()
{
    bool fileNameResult = createFileName();
    if (!fileNameResult)
        return false;
    QDir slideShowDir(getSlidesDirName());
    QStringList images = slideShowDir.entryList(QStringList() << (QStringLiteral("*.") + PICTURE_FORMAT),
                                                QDir::Files, QDir::Time | QDir::Reversed);
    if (images.size() < 1)
        return false;
    QString imagePath = (slideShowDir.absolutePath() + QDir::separator() + QDir::cleanPath(images.first()));
    QPixmap firstFrame(imagePath, PICTURE_FORMAT_C);
    try
    {
        cv::Size firstSize(firstFrame.width(),firstFrame.height());
        cv::VideoWriter video(m_videoFileName.toLocal8Bit().data(), CV_FOURCC('M','J','P','G'), m_frequency,
                              firstSize, true);
        foreach(const QString &filename, images)
        {
            imagePath = (slideShowDir.absolutePath() + QDir::separator() + QDir::cleanPath(filename));
            cv::Mat frame = cv::imread(imagePath.toLocal8Bit().data());
            if (frame.cols != firstSize.width || frame.rows != firstSize.height)
            {
                cv::Mat oldFrame = frame;
                cv::resize(oldFrame, frame, firstSize);
                oldFrame.release();
            }
            video.write(frame);
            frame.release();
        }
    }
    catch (...)
    {
        slideShowDir.removeRecursively();
        return false;
    }
    slideShowDir.removeRecursively();
    return true;
}

inline bool VVideoShooter::createFileName()
{
    if (m_videoDirectory.isEmpty() || !QDir(m_videoDirectory).exists())
    {
        m_videoFileName = QStringLiteral("");
        return false;
    }
    QString originalPath = QDir::cleanPath(m_videoDirectory + QDir::separator() + BASE_VIDEO_FILE_NAME);
    QString videoPath = originalPath.arg(QStringLiteral(""));
    QFile videoFile(videoPath);
    int counter = 0;
    while (videoFile.exists())
    {
        videoPath = originalPath.arg(++counter);
        videoFile.setFileName(videoPath);
    }
    m_videoFileName = videoPath;
    return true;
}
