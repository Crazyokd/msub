#pragma once

#include <condition_variable>
#include <cstdint>
#include <functional>
#include <list>
#include <mutex>
#include <thread>

#include "timer/rk-tw.h"

namespace msub {

class MSub;
class Message
{
    friend class MSub;
public:
    virtual bool match() = 0;
private:
    /* lock and cv */
    std::mutex mtx_;
    std::condition_variable cv_;
};

class MSub final
{
public:
    MSub();
    ~MSub();
    using MatchFunc = std::function<bool(Message &msg)>;

    /**
     * subscribe message
     *
     * @param milliseconds for timeout
     * @return 0 if success
     */
    int subscribe(Message &msg, uint32_t timeout);
    /**
     * notify all matched message
     *
     * @return number of message notified
     */
    int notifyAll(MatchFunc func);
    /**
     * notify all matched message but use Message::match function
     */
    int notifyAll();
    /**
     * notify one matched message
     *
     * @return number of message notified
     */
    int notifyOne(MatchFunc func);
    /**
     * notify one matched message but use Message::match function
     */
    int notifyOne();
    /**
     * notifyAll but in the future
     *
     * @return number of message notified
     */
    int notifyFuture(MatchFunc func, uint32_t timeout);
    int start();
    void stop();

private:
    int notify(MatchFunc, int);
    void run();
    int handleTimeout(void *);
    std::list<Message *> msgs_;
    rk_tw_t *tw_;
    std::thread *loop_;
    bool stop_;
    std::mutex mutex_;
};
} // namespace msub
