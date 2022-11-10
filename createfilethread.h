#ifndef CREATEFILETHREAD_H
#define CREATEFILETHREAD_H

#include <QThread>
#include <QObject>
#include <QDebug>

class CreateFileThread : public QThread
{
    Q_OBJECT
public:
    CreateFileThread();
    ~CreateFileThread();

    void startThread();
    void stopThread();
};

#endif // CREATEFILETHREAD_H
