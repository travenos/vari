/**
 * Project VARI
 * @author Alexey Barashkov
 */

#ifdef DEBUG_MODE
#include <QDebug>
#endif

#include <algorithm>
#include <functional>

#include <QDir>
#include <QPixmap>

#include <opencv2/videoio.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "VVideoShooter.h"

/**
 * VVideoShooter implementation
 */

const char * const VVideoShooter::VIDEO_FORMAT_C = "avi";
const int VVideoShooter::VIDEO_CODEC = CV_FOURCC('M', 'J', 'P', 'G');
const QString VVideoShooter::VIDEO_FORMAT(VVideoShooter::VIDEO_FORMAT_C);
const QString VVideoShooter::DEFAULT_SUFFIX_FILE_NAME("VARI_video");
const QString VVideoShooter::BASE_VIDEO_FILE_NAME = QStringLiteral("%1%2.") + VVideoShooter::VIDEO_FORMAT;
const QString VVideoShooter::BASE_SLIDES_DIR_NAME("_VARI_SLIDES_FOR_VIDEO_");
const QString VVideoShooter::SLIDES_DIR_PATH = QDir::cleanPath(QDir::tempPath() + QDir::separator() + VVideoShooter::BASE_SLIDES_DIR_NAME);

VVideoShooter::VVideoShooter():
    VScreenShooter(),
    m_videoDirectory(QDir::homePath())
{
    VScreenShooter::setDirPath(SLIDES_DIR_PATH);
    setFrequency(10);
    constructorBody();
}

VVideoShooter::VVideoShooter(const QWidget * widget, const QString &dirPath, int frequency):
    VScreenShooter(widget, SLIDES_DIR_PATH, 1.0f/frequency),
    m_videoDirectory(dirPath)
{
    setFrequency(frequency);
    constructorBody();
}

inline void VVideoShooter::constructorBody()
{
    m_isSaving.store(false);
    setSuffixFileName(DEFAULT_SUFFIX_FILE_NAME);
}

VVideoShooter::~VVideoShooter()
{
    stop();
    waitForSaving();
}

void VVideoShooter::setFrequency(int frequency)
{
    m_frequency = std::max(frequency, 1);
    VScreenShooter::setPeriod(1.0f/frequency);
    emit frequencyChanged();
}

int VVideoShooter::getFrequency() const
{
    return m_frequency;
}

const QString& VVideoShooter::getDirPath() const
{
    return m_videoDirectory;
}

void VVideoShooter::setDirPath(const QString &dirPath)
{
    m_videoDirectory = dirPath;
    emit directoryChanged();
}

void VVideoShooter::setSuffixFileName(const QString &name)
{
    if (!name.isEmpty())
        m_suffixFileName = name;
    else
        m_suffixFileName = DEFAULT_SUFFIX_FILE_NAME;
    emit suffixFileNameChanged();
}

const QString& VVideoShooter::getSuffixFileName() const
{
    return m_suffixFileName;
}

const QString& VVideoShooter::getVideoFilePath() const
{
    return m_videoFilePath;
}

void VVideoShooter::setPeriod(float period)
{
    setFrequency(1.0f/period);
}

void VVideoShooter::start()
{
    waitForSaving();
    m_videoFilePath.clear();
    if (!m_suffixFileName.isEmpty())
        VScreenShooter::start();
    else
        emit processStopped();
}

void VVideoShooter::stop()
{
    if (isWorking())
    {
        VScreenShooter::stop();
        waitForSaving();
        m_pSavingThread.reset(new std::thread(&VVideoShooter::saveVideoProcess, this));
    }
}

void VVideoShooter::saveVideoProcess()
{
    m_isSaving.store(true);
    emit videoSavingStarted();
    bool result = false;
    bool filePathResult = createFilePath();
    QDir slideShowDir(getSlidesDirPath());
    QVector<QString> images = slideShowDir.entryList(QStringList() << (QStringLiteral("*.") + PICTURE_FORMAT),
                                                QDir::Files).toVector();

    auto sortFunc = [](const QString &str1, const QString &str2)
    {
        return floatFromStr(str1) < floatFromStr(str2);
    };
    std::sort(images.begin(), images.end(), sortFunc);

    if (images.size() >= 1 && filePathResult)
    {
        QString imagePath = QDir::cleanPath(slideShowDir.absolutePath() + QDir::separator() + images.first());
        QPixmap firstFrame(imagePath, PICTURE_FORMAT_C);
        try
        {
            if (firstFrame.width() > 0 && firstFrame.height() > 0)
            {
                cv::Size firstSize(firstFrame.width(),firstFrame.height());
                cv::VideoWriter video(m_videoFilePath.toLocal8Bit().data(), VIDEO_CODEC, m_frequency,
                                      firstSize, true);
                foreach(const QString &filename, images)
                {
                    #ifdef DEBUG_MODE
                    qDebug() << filename;
                    #endif
                    imagePath = QDir::cleanPath(slideShowDir.absolutePath() + QDir::separator() + filename);
                    cv::Mat frame = cv::imread(imagePath.toLocal8Bit().data());
                    if (frame.cols != firstSize.width || frame.rows != firstSize.height)
                    {
                        cv::Mat oldFrame = frame;
                        cv::resize(oldFrame, frame, firstSize);
                    }
                    video.write(frame);
                }
                result = true;
            }
            else
                result = false;
        }
        catch (...)
        {
            result = false;
        }
    }
    else
        result = false;
    slideShowDir.removeRecursively();
    m_isSaving.store(false);
    emit videoSavingFinished(result);
}

inline bool VVideoShooter::createFilePath()
{
    if (m_videoDirectory.isEmpty() || !QDir(m_videoDirectory).exists() || m_suffixFileName.isEmpty())
    {
        m_videoFilePath = QStringLiteral("");
        return false;
    }
    QString originalPath = QDir::cleanPath(m_videoDirectory + QDir::separator()
                                           + BASE_VIDEO_FILE_NAME.arg(m_suffixFileName));
    QString videoPath = originalPath.arg(QStringLiteral(""));
    QFile videoFile(videoPath);
    int counter = 0;
    while (videoFile.exists())
    {
        videoPath = originalPath.arg(++counter);
        videoFile.setFileName(videoPath);
    }
    m_videoFilePath = videoPath;
    return true;
}

void VVideoShooter::waitForSaving()
{
    if (m_pSavingThread && m_pSavingThread->joinable())
    {
        m_pSavingThread->join();
        m_pSavingThread.reset();
    }
}

bool VVideoShooter::isSaving() const
{
    return m_isSaving.load();
}

inline float VVideoShooter::floatFromStr(const QString &str)
{
    const QRegExp FLOAT_REGEX(QStringLiteral("[0-9]+([.][0-9]*)?|[.][0-9]+"));
    int start=str.indexOf(FLOAT_REGEX);
    int finish=str.lastIndexOf(FLOAT_REGEX);
    return str.midRef(start, finish - start + 1).toFloat();
}
