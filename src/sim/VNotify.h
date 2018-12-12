/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VNOTIFY_H
#define _VNOTIFY_H

#include <atomic>
#include <condition_variable>
#include <mutex>

/**
* This class is used for notification when the data is changed.
* It is needed for synchronization with VGraphicsViewer.
* The methods waitForNewData() and cancelWaitingForNewData() are used.
*/
class VNotify
{
public:
    VNotify();
    VNotify(const VNotify&) = delete;
    VNotify& operator= (const VNotify& ) = delete;
    void notifyAll();
    void notifyOne();
    void wait();
private:
    std::condition_variable m_cv;
    std::mutex m_mutex;
    std::atomic<bool> m_flag;
};

#endif //_VNOTIFY_H
