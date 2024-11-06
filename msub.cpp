#include "msub.h"

#include "unistd.h"

namespace msub {
MSub::MSub()
{
    /* clear message list */
    msgs_.clear();
    tw_ = nullptr;
    loop_ = nullptr;
    stop_ = true;
}
MSub::~MSub()
{
    stop();
}
int MSub::start()
{
    /* init timewheel */
    tw_ = rk_tw_create(10, 1000, 10000);

    /* enable main loop */
    loop_ = new std::thread(&MSub::run, this);
    if (!loop_) {
        return -1;
    }

    return 0;
}

void MSub::run()
{
    stop_ = false;
    while (!stop_) {
        /* check timeout */
        rk_tw_check(tw_);
        /* todo: Determine sleep length by granularity */
        sleep(1);
    }
}

void MSub::stop()
{
    /* destroy timewheel */
    if (loop_) {
        while (stop_) {
            sleep(1);
        }
        stop_ = true;
        loop_->join();
        delete loop_;
        loop_ = nullptr;
    }
    rk_tw_destroy(tw_);

    /* notify all messages */
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = msgs_.begin();
    while (it != msgs_.end()) {
        std::unique_lock<std::mutex> lock((*it)->mtx_);
        (*it)->cv_.notify_all();
        it = msgs_.erase(it);
    }
}

int MSub::subscribe(Message &msg, uint32_t timeout)
{
    /* add timer and push to list */
    // std::function<int(void *)> timeout_cb =
    //     std::bind(&MSub::handleTimeout, this, std::placeholders::_1);
    // if (rk_tw_create_task(tw_, timeout, timeout_cb.target<int(void *)>(), &msg)
    //     == 0) {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        msgs_.push_back(&msg);
    }
    std::unique_lock<std::mutex> lock(msg.mtx_);
    auto status = msg.cv_.wait_for(lock, std::chrono::milliseconds(timeout));
    if (status == std::cv_status::no_timeout) {
        return 0;
    } else {
        std::lock_guard<std::mutex> lock(mutex_);
        msgs_.remove(&msg);
        return -1;
    }
    // }
}

int MSub::notifyAll()
{
    return notify([](Message &msg) { return msg.match(); }, -1);
}
int MSub::notifyAll(MatchFunc func)
{
    return notify(func, -1);
}
int MSub::notifyOne(MatchFunc func)
{
    return notify(func, 1);
}
int MSub::notifyOne()
{
    return notify([](Message &msg) { return msg.match(); }, 1);
}
int MSub::notify(MatchFunc func, int threshold)
{
    /* traverse message list and try to match */
    int res = 0;
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = msgs_.begin();
    while (it != msgs_.end()) {
        if (func(**it)) {
            std::unique_lock<std::mutex> lock((*it)->mtx_);
            (*it)->cv_.notify_all();
            it = msgs_.erase(it);
            ++res;
            if (res == threshold) {
                break;
            }
        } else {
            ++it;
        }
    }
    return res;
}
} // namespace msub
