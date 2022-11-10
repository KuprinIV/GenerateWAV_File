#include "createfilethread.h"

CreateFileThread::CreateFileThread()
{

}

CreateFileThread::~CreateFileThread()
{
    qDebug()<<"Create file thread deleted";
    disconnect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void CreateFileThread::startThread()
{
    qDebug()<<"Create file thread started";
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    start();
}

void CreateFileThread::stopThread()
{
    quit();
    qDebug()<<"Create file thread stopped";
}
