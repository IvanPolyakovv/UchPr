#include "database.h"
#include "ahp.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include <QSqlRecord>

QSqlDatabase openDatabase() {
    QString connectionName = QString("Connection_%1").arg((quintptr)QThread::currentThread()->currentThreadId());
    if (QSqlDatabase::contains(connectionName))
        return QSqlDatabase::database(connectionName);

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("database.db");

    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
    }
    return db;
}

bool checkCredentials(const std::string& username, const std::string& password) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username AND password = :password");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":password", QString::fromStdString(password));
    if (!query.exec()) {
        qDebug() << "Ошибка запроса:" << query.lastError().text();
        return false;
    }
    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;
    }
    return false;
}

bool registerUser(const std::string& username, const std::string& password) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (username, password) VALUES (:username, :password)");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":password", QString::fromStdString(password));
    if (!query.exec()) {
        qDebug() << "Ошибка регистрации:" << query.lastError().text();
        return false;
    }
    return true;
}

bool saveUserParameters(const std::string& username, const std::map<std::string, std::string>& params) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO user_parameters (username, budget, district, type, area_min, area_max, rooms, balcony, floor_min, floor_max, house_type, infrastructure, transport) "
                  "VALUES (:username, :budget, :district, :type, :area_min, :area_max, :rooms, :balcony, :floor_min, :floor_max, :house_type, :infrastructure, :transport)");

    query.bindValue(":username", QString::fromStdString(username));
    for (const auto& [key, value] : params) {
        query.bindValue(QString::fromStdString(":" + key), QString::fromStdString(value));
    }

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения параметров:" << query.lastError().text();
        return false;
    }
    return true;
}

std::map<std::string, std::string> getUserParameters(const std::string& username) {
    QSqlDatabase db = openDatabase();
    std::map<std::string, std::string> params;
    if (!db.isOpen()) return params;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM user_parameters WHERE username = :username");
    query.bindValue(":username", QString::fromStdString(username));

    if (!query.exec() || !query.next()) {
        return params;
    }

    QSqlRecord record = query.record();
    for (int i = 0; i < record.count(); ++i) {
        QString field = record.fieldName(i);
        if (field != "username") {
            params[field.toStdString()] = query.value(i).toString().toStdString();
        }
    }

    return params;
}

std::vector<Apartment> getApartmentsFromDB() {
    QSqlDatabase db = openDatabase();
    std::vector<Apartment> apartments;
    if (!db.isOpen()) return apartments;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM apartments");

    if (!query.exec()) {
        qDebug() << "Ошибка получения квартир:" << query.lastError().text();
        return apartments;
    }

    while (query.next()) {
        Apartment apt;
        apt.id = query.value("id").toInt();
        apt.price = query.value("price").toDouble();
        apt.area = query.value("area").toDouble();
        apt.rooms = query.value("rooms").toInt();
        apt.district = query.value("district").toString().toStdString();
        apt.transport = query.value("transport").toDouble();
        apt.infrastructure = query.value("infrastructure").toDouble();
        apt.condition = query.value("condition").toString().toStdString();
        apt.house_type = query.value("house_type").toString().toStdString();
        apt.floor = query.value("floor").toInt();
        apt.balcony = query.value("balcony").toBool();
        apt.description = query.value("description").toString().toStdString();
        apt.photo_url = query.value("photo_url").toString().toStdString();

        apartments.push_back(apt);
    }

    return apartments;
}

std::map<std::string, int> getComparisonsFromDB(const std::string& username) {
    QSqlDatabase db = openDatabase();
    std::map<std::string, int> comparisons;
    if (!db.isOpen()) return comparisons;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM user_comparisons WHERE username = :username");
    query.bindValue(":username", QString::fromStdString(username));

    if (query.exec()) {
        while (query.next()) {
            QString pair = query.value("criteria_pair").toString();
            int value = query.value("comparison_value").toInt();
            comparisons[pair.toStdString()] = value;
        }
    } else {
        qDebug() << "Ошибка получения сравнений:" << query.lastError().text();
    }

    return comparisons;
}

bool saveWeights(const std::string& username, const std::vector<double>& weights) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO user_weights (username, price_weight, area_weight, rooms_weight, district_weight, transport_weight, infrastructure_weight, condition_weight, house_type_weight, floor_weight, balcony_weight) "
                  "VALUES (:username, :price_weight, :area_weight, :rooms_weight, :district_weight, :transport_weight, :infrastructure_weight, :condition_weight, :house_type_weight, :floor_weight, :balcony_weight)");

    query.bindValue(":username", QString::fromStdString(username));
    for (size_t i = 0; i < weights.size(); ++i) {
        query.bindValue(QString::fromStdString(":" + AHP::criteria[i] + "_weight"), weights[i]);
    }

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения весов:" << query.lastError().text();
        return false;
    }
    return true;
}

std::vector<double> getWeights(const std::string& username) {
    QSqlDatabase db = openDatabase();
    std::vector<double> weights(AHP::criteria.size(), 1.0/AHP::criteria.size());
    if (!db.isOpen()) return weights;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM user_weights WHERE username = :username");
    query.bindValue(":username", QString::fromStdString(username));

    if (!query.exec() || !query.next()) {
        return weights;
    }

    for (size_t i = 0; i < AHP::criteria.size(); ++i) {
        weights[i] = query.value(QString::fromStdString(AHP::criteria[i] + "_weight")).toDouble();
    }

    return weights;
}
