#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H
#include <memory>

class EventHandler : public std::enable_shared_from_this<EventHandler>
{
public:
    EventHandler(int);
    virtual ~EventHandler() = default;

    virtual void handleRead() = 0;
    virtual void handleWrite() = 0;

    int getFd() const;

protected:
    int fd_;
};

#endif