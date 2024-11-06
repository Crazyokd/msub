#include <unistd.h>

#include "msub.h"

using msub::MSub;

class TestMessage : public msub::Message
{
    virtual bool match() override
    {
        return true;
    }
};

MSub *ms = new MSub();

#define SUB_TIMEOUT 15000
void thr1()
{
    TestMessage tm1;
    int ret = ms->subscribe(tm1, SUB_TIMEOUT);
    if (ret != 0) {
        printf("subscribe message failed\n");
    } else {
        printf("subscribe message successed\n");
    }
}

void thr2()
{
    ms->notifyOne([](msub::Message &) {
        return true;
    });
}
void thr3()
{
    ms->notifyOne();
}

#define SUB_THREAD_CNT 10
int main()
{
    if (ms->start() != 0) {
        printf("start msub failed\n");
        exit(1);
    }
    std::thread thrs[SUB_THREAD_CNT+5+2];
    for (int i = 0; i < SUB_THREAD_CNT; i++) {
        thrs[i] = std::thread(&thr1);
    }
    for (int i = 0; i < 2; i++) {
        thrs[SUB_THREAD_CNT+i] = std::thread(&thr2);
        sleep(1);
    }
    sleep(3);
    for (int i = 2; i < 5; i++) {
        thrs[SUB_THREAD_CNT+i] = std::thread(&thr2);
        // sleep(1);
    }
    sleep(1);
    for (int i = 0; i < 2; i++) {
        thrs[SUB_THREAD_CNT+5+i] = std::thread(&thr3);
    }
    for (int i = 0; i < SUB_THREAD_CNT; i++) {
        thrs[i].join();
    }
    for (int i = 0; i < 5+2; i++) {
        thrs[SUB_THREAD_CNT+i].join();
    }

    delete ms;
    return 0;
}
