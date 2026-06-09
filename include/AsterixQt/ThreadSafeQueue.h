#pragma once

#include <QMutex>
#include <QMutexLocker>
#include <QQueue>

namespace asterixqt {

template <typename T>
class ThreadSafeQueue
{
public:
    void push(const T& value)
    {
        QMutexLocker locker(&mutex_);
        queue_.enqueue(value);
    }

    bool tryPop(T& out)
    {
        QMutexLocker locker(&mutex_);

        if (queue_.isEmpty()) {
            return false;
        }

        out = queue_.dequeue();
        return true;
    }

    void clear()
    {
        QMutexLocker locker(&mutex_);
        queue_.clear();
    }

private:
    mutable QMutex mutex_;
    QQueue<T> queue_;
};

} // namespace asterixqt
