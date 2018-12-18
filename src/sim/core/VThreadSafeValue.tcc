/**
 * Project VARI
 * @author Alexey Barashkov
 */


#ifndef _VTHREADSAFEVALUE_TCC
#define _VTHREADSAFEVALUE_TCC

#include <QReadWriteLock>

template <typename T>
class VThreadSafeValue
{
public:
    VThreadSafeValue() = default;

    VThreadSafeValue(const VThreadSafeValue<T> &other) : m_value(other.getValue())
    {}

    VThreadSafeValue(const T &value) : m_value(value)
    {}

    const VThreadSafeValue<T>& operator= (const T& value)
    {
        QWriteLocker locker(&m_lock);
        m_value = value;
        return *this;
    }

    void setValue(const T &value)
    {
        QWriteLocker locker(&m_lock);
        m_value = value;
    }

    operator T() const
    {
        QReadLocker locker(&m_lock);
        return m_value;
    }

    T getValue() const
    {
        QReadLocker locker(&m_lock);
        return m_value;
    }

private:
    mutable QReadWriteLock m_lock;
    T m_value;
};

#endif //_VTHREADSAFEVALUE_TCC
