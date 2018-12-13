/**
 * Project VARI
 * @author Alexey Barashkov
 */

#include "VNotify.h"

/**
 * VNotify implementation
 */

VNotify::VNotify() :
    m_flag(false)
{
}

void VNotify::wait()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    while(!m_flag)
        m_cv.wait(locker);
    m_flag.store(false);
}

void VNotify::notifyAll()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_flag.store(true);
    m_cv.notify_all();
}

void VNotify::notifyOne()
{
    std::unique_lock<std::mutex> locker(m_mutex);
    m_flag.store(true);
    m_cv.notify_one();
}
