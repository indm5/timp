#ifndef CLIENTAPI_H
#define CLIENTAPI_H

#include <QObject>

class ClientAPI : public QObject
{
    Q_OBJECT
public:
    explicit ClientAPI(QObject *parent = nullptr);
};

#endif // CLIENTAPI_H
