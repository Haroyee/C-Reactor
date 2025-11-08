#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

class EventHandler
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