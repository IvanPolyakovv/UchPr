#include "database.h"
#include "ahp.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include <QSqlRecord>

//Открыть БД
QSqlDatabase openDatabase() {
    QString connectionName = QString("Connection_%1").arg((quintptr)QThread::currentThread()->currentThreadId());

    if (QSqlDatabase::contains(connectionName)) {
        return QSqlDatabase::database(connectionName);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName("database.db");

    if (!db.open()) {
        qDebug() << "Ошибка подключения к базе данных:" << db.lastError().text();
    } else {
        qDebug() << "База данных успешно открыта";
    }
    return db;
}

//Проверить логин и пароль
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
//Зарегистрировать
bool registerUser(const std::string& username, const std::string& password, const std::map<std::string, std::string>& preferences) {
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

    // Сохранение предпочтений
    query.prepare("INSERT INTO user_preferences (username, budget, district, type, area_min, area_max, rooms, balcony, floor_min, floor_max, house_type) "
                  "VALUES (:username, :budget, :district, :type, :area_min, :area_max, :rooms, :balcony, :floor_min, :floor_max, :house_type)");

    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":budget", QString::fromStdString(preferences.at("budget")));
    query.bindValue(":district", QString::fromStdString(preferences.at("district")));
    query.bindValue(":type", QString::fromStdString(preferences.at("type")));
    query.bindValue(":area_min", QString::fromStdString(preferences.at("area_min")));
    query.bindValue(":area_max", QString::fromStdString(preferences.at("area_max")));
    query.bindValue(":rooms", QString::fromStdString(preferences.at("rooms")));
    query.bindValue(":balcony", QString::fromStdString(preferences.at("balcony")));
    query.bindValue(":floor_min", QString::fromStdString(preferences.at("floor_min")));
    query.bindValue(":floor_max", QString::fromStdString(preferences.at("floor_max")));
    query.bindValue(":house_type", QString::fromStdString(preferences.at("house_type")));

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения предпочтений:" << query.lastError().text();
        return false;
    }

    return true;
}

//Сохранить предпочтения
bool saveUserParameters(const std::string& username, const std::map<std::string, std::string>& params) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) {
        qDebug() << "Ошибка подключения к базе данных";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO user_preferences (username, budget, district, type, area_min, area_max, rooms, balcony, floor_min, floor_max, house_type) "
                  "VALUES (:username, :budget, :district, :type, :area_min, :area_max, :rooms, :balcony, :floor_min, :floor_max, :house_type)");

    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":budget", QString::fromStdString(params.at("budget")));
    query.bindValue(":district", QString::fromStdString(params.at("district")));
    query.bindValue(":type", QString::fromStdString(params.at("type")));
    query.bindValue(":area_min", QString::fromStdString(params.at("area_min")));
    query.bindValue(":area_max", QString::fromStdString(params.at("area_max")));
    query.bindValue(":rooms", QString::fromStdString(params.at("rooms")));
    query.bindValue(":balcony", QString::fromStdString(params.at("balcony")));
    query.bindValue(":floor_min", QString::fromStdString(params.at("floor_min")));
    query.bindValue(":floor_max", QString::fromStdString(params.at("floor_max")));
    query.bindValue(":house_type", QString::fromStdString(params.at("house_type")));

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения параметров:" << query.lastError().text();
        return false;
    }

    return true;
}

//Получить предпочтения
std::map<std::string, std::string> getUserParameters(const std::string& username) {
    QSqlDatabase db = openDatabase();
    std::map<std::string, std::string> params;
    if (!db.isOpen()) return params;

    QSqlQuery query(db);
    query.prepare("SELECT * FROM user_preferences WHERE username = :username");
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

//Получить список квартир
std::vector<Apartment> getApartmentsFromDB() {
    std::vector<Apartment> apartments;

    QSqlQuery query;
    query.exec("SELECT description, district, price, area, rooms, balcony, house_type, floor FROM apartments");

    while (query.next()) {
        Apartment apt;
        apt.description = query.value("description").toString().toStdString();
        apt.district = query.value("district").toString().toStdString();
        apt.price = query.value("price").toDouble();
        apt.area = query.value("area").toDouble();
        apt.rooms = query.value("rooms").toInt();
        apt.balcony = query.value("balcony").toBool();
        apt.house_type = query.value("house_type").toString().toStdString();
        apt.floor = query.value("floor").toInt();

        apartments.push_back(apt);
    }

    return apartments;
}

//Получить сравнения
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

//Сохранить веса критериев
bool saveWeights(const std::string& username, const std::vector<double>& weights) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT OR REPLACE INTO user_weights (username, price_weight, area_weight, rooms_weight, district_weight, transport_weight, infrastructure_weight, condition_weight, house_type_weight, floor_weight, balcony_weight) "
                  "VALUES (:username, :price_weight, :area_weight, :rooms_weight, :district_weight, :transport_weight, :infrastructure_weight, :condition_weight, :house_type_weight, :floor_weight, :balcony_weight)");

    query.bindValue(":username", QString::fromStdString(username));
    for (size_t i = 0; i < weights.size(); ++i) {
        query.bindValue(QString::fromStdString(":" + AHP::criteria[i] + "_weight"),
                        weights[i]);
    }

    if (!query.exec()) {
        qDebug() << "Ошибка сохранения весов:" << query.lastError().text();
        return false;
    }
    return true;
}

//Получить веса критериев
std::vector<double> getWeights(const std::string& username) {
    QSqlDatabase db = openDatabase();
    std::vector<double> weights(AHP::criteria.size(), 1.0 / AHP::criteria.size());
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

// Добавление квартиры в избранное
bool addToFavorites(const std::string& username, int apartmentId) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO favorites (username, apartment_id) VALUES (:username, :apartment_id)");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":apartment_id", apartmentId);

    if (!query.exec()) {
        qDebug() << "Ошибка добавления в избранное:" << query.lastError().text();
        return false;
    }
    qDebug() << "Квартира добавлена в избранное для пользователя" << username << ", ID квартиры:" << apartmentId;
    return true;
}

// Удаление квартиры из избранного
bool removeFromFavorites(const std::string& username, int apartmentId) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM favorites WHERE username = :username AND apartment_id = :apartment_id");
    query.bindValue(":username", QString::fromStdString(username));
    query.bindValue(":apartment_id", apartmentId);

    if (!query.exec()) {
        qDebug() << "Ошибка удаления из избранного:" << query.lastError().text();
        return false;
    }
    qDebug() << "Квартира удалена из избранного для пользователя" << username << ", ID квартиры:" << apartmentId;
    return true;
}

// Проверка, находится ли квартира в избранном
bool isFavorite(const std::string& username, int apartmentId) {
    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) {
        qDebug() << "Ошибка: не удалось открыть базу данных.";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(*) FROM favorites WHERE username = ? AND apartment_id = ?");
    query.bindValue(0, QString::fromStdString(username));
    query.bindValue(1, apartmentId);

    if (!query.exec()) {
        qDebug() << "Ошибка выполнения запроса: " << query.lastError().text().toStdString();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    } else {
        qDebug() << "Ошибка: запрос не вернул результатов.";
        return false;
    }
}


// Получение списка избранных квартир для пользователя
std::vector<Apartment> getFavorites(const std::string& username) {
    QSqlDatabase db = openDatabase();
    std::vector<Apartment> favorites;
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта";
        return favorites;
    }

    QSqlQuery query(db);
    query.prepare("SELECT a.* FROM apartments a "
                  "JOIN favorites f ON a.id = f.apartment_id "
                  "WHERE f.username = :username");
    query.bindValue(":username", QString::fromStdString(username));

    if (query.exec()) {
        while (query.next()) {
            Apartment apt;
            apt.id = query.value("id").toInt();
            apt.description = query.value("description").toString().toStdString();
            apt.district = query.value("district").toString().toStdString();
            apt.price = query.value("price").toDouble();
            apt.area = query.value("area").toDouble();
            apt.rooms = query.value("rooms").toInt();
            apt.balcony = query.value("balcony").toBool();
            apt.house_type = query.value("house_type").toString().toStdString();
            apt.floor = query.value("floor").toInt();
            favorites.push_back(apt);
        }
    } else {
        qDebug() << "Ошибка получения избранных квартир:" << query.lastError().text();
    }
    return favorites;
}

