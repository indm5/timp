#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>

bool Database::initialize()
{
    QDir().mkpath(".");

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("users.db");

    if (!m_db.open()) {
        qDebug() << "[DB ERROR]" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE, "
               "password TEXT, "
               "role TEXT)");

    // Admin user
    query.prepare("SELECT COUNT(*) FROM users WHERE username = 'admin'");
    query.exec();
    if (query.next() && query.value(0).toInt() == 0) {
        query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
        query.addBindValue("admin");
        query.addBindValue(hashPassword("admin123"));
        query.addBindValue("admin");
        query.exec();
        qDebug() << "[DB] Admin created: admin/admin123";
    }

    // Regular user
    query.prepare("SELECT COUNT(*) FROM users WHERE username = 'user1'");
    query.exec();
    if (query.next() && query.value(0).toInt() == 0) {
        query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
        query.addBindValue("user1");
        query.addBindValue(hashPassword("pass123"));
        query.addBindValue("user");
        query.exec();
        qDebug() << "[DB] User created: user1/pass123";
    }

    return true;
}

bool Database::validateUser(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value(0).toString() == hashPassword(password);
    }
    return false;
}

bool Database::registerUser(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM users WHERE username = ?");
    query.addBindValue(username);
    query.exec();

    if (query.next() && query.value(0).toInt() > 0) {
        return false;
    }

    query.prepare("INSERT INTO users (username, password, role) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    query.addBindValue("user");

    return query.exec();
}

QString Database::getUserRole(const QString &username)
{
    QSqlQuery query;
    query.prepare("SELECT role FROM users WHERE username = ?");
    query.addBindValue(username);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "user";
}

QString Database::hashPassword(const QString &password)
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex();
}

Database& Database::instance()
{
    static Database instance;
    return instance;
}
