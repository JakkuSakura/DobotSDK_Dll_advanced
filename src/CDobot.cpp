#include "CDobot.h"
#include <QCoreApplication>
#include <QThread>

CDobot *CDobot::instance()
{
    static CDobot *instance[64] = {0};
    int dobotId = getDobotID();
    if (instance[dobotId] == 0) {
        instance[dobotId] = new CDobot();
    }
    return instance[dobotId];
}
static int currentId = 0;
int CDobot::getDobotID()
{
    return currentId;
}
void CDobot::specifyDobotID(int id)
{
    currentId = id;
}

void CDobot::exec(void)
{
    if (qApp) {
        qApp->exec();
    }
}

CDobot::CDobot(QObject *parent)
    : QObject(parent)
{
    if (qApp == 0) {
        // In case when used in non-Qt Appliction!
        int argc = 0;
        char **argv = 0;
        app = new QCoreApplication(argc, argv);
    } else {
        app = 0;
    }

    connector = new CDobotConnector();
    connectorTargetThread = new QThread();
    connectorTargetThread->start();
    connector->moveToThread(connectorTargetThread);
    QMetaObject::invokeMethod(connector, "onInit", Qt::BlockingQueuedConnection);

    protocol = new CDobotProtocol();
    protocolTargetThread = new QThread();
    protocolTargetThread->start();
    protocol->moveToThread(protocolTargetThread);
    QMetaObject::invokeMethod(protocol, "onInit", Qt::BlockingQueuedConnection);

    communicator = new CDobotCommunicator();
    communicatorTargetThread = new QThread();
    communicatorTargetThread->start();
    communicator->moveToThread(communicatorTargetThread);
    QMetaObject::invokeMethod(communicator, "onInit", Qt::BlockingQueuedConnection);

    connect(connector, SIGNAL(newConnectStatus(bool)), protocol, SLOT(onNewConnectStatus(bool)));
    connect(connector, SIGNAL(newConnectStatus(bool)), communicator, SLOT(onNewConnectStatus(bool)));

    connect(protocol, SIGNAL(bytesToWrite(const char*,qint64)), connector, SLOT(onBytesToWrite(const char*,qint64)));
    connect(connector, SIGNAL(bytesWritten()), protocol, SLOT(onBytesWritten()));
    connect(connector, SIGNAL(bytesReady(QByteArray)), protocol, SLOT(onBytesReady(QByteArray)));

    connect(communicator, SIGNAL(messageToSend(Message)), protocol, SLOT(sendMessage(Message)));
    connect(protocol, SIGNAL(messageReady(Message)), communicator, SLOT(onMessageReady(Message)));
}

CDobot::~CDobot()
{
    delete connector;
    delete connectorTargetThread;
    delete protocol;
    delete protocolTargetThread;
    delete communicator;
    delete communicatorTargetThread;
    if (app) {
        delete app;
    }
}
