#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>

class Database
{
public:
    static Database& instance();
    bool initialize();
    bool validateUser(const QString &username, const QString &password);
    bool registerUser(const QString &username, const QString &password);
    QString getUserRole(const QString &username);

private:
    Database() = default;
    QSqlDatabase m_db;
    QString hashPassword(const QString &password);
};

#endif
