#include "crow/crow.h"

#include "ahp.h"
#include "database.h"
#include "qsqlerror.h"
#include "utils.h"
#include "webui.h"
#include <QCoreApplication>
#include <QSqlQuery>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Инициализация БД
    {
        QSqlDatabase db = openDatabase();
        if (db.isOpen()) {
            QSqlQuery query(db);
            query.exec("CREATE TABLE IF NOT EXISTS users ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT UNIQUE,"
                       "password TEXT)");

            query.exec("CREATE TABLE IF NOT EXISTS user_parameters ("
                       "username TEXT PRIMARY KEY,"
                       "budget TEXT,"
                       "district TEXT,"
                       "type TEXT,"
                       "area_min TEXT,"
                       "area_max TEXT,"
                       "rooms TEXT,"
                       "balcony TEXT,"
                       "floor_min TEXT,"
                       "floor_max TEXT,"
                       "house_type TEXT,"
                       "infrastructure TEXT,"
                       "transport TEXT)");

            query.exec("CREATE TABLE IF NOT EXISTS user_weights ("
                       "username TEXT PRIMARY KEY,"
                       "price_weight REAL,"
                       "area_weight REAL,"
                       "rooms_weight REAL,"
                       "district_weight REAL,"
                       "transport_weight REAL,"
                       "infrastructure_weight REAL,"
                       "condition_weight REAL,"
                       "house_type_weight REAL,"
                       "floor_weight REAL,"
                       "balcony_weight REAL)");

            query.exec("CREATE TABLE IF NOT EXISTS user_comparisons ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT,"
                       "criteria_pair TEXT,"
                       "comparison_value INTEGER)");

            query.exec("CREATE TABLE IF NOT EXISTS apartments ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "price REAL,"
                       "area REAL,"
                       "rooms INTEGER,"
                       "district TEXT,"
                       "transport REAL,"
                       "infrastructure REAL,"
                       "condition TEXT,"
                       "house_type TEXT,"
                       "floor INTEGER,"
                       "balcony BOOLEAN,"
                       "description TEXT,"
                       "photo_url TEXT)");

            // Insert some sample apartments if table is empty
            query.exec("SELECT COUNT(*) FROM apartments");
            if (query.next() && query.value(0).toInt() == 0) {
                query.exec("INSERT INTO apartments (price, area, rooms, district, transport, infrastructure, condition, house_type, floor, balcony, description, photo_url) "
                           "VALUES (5000000, 45.5, 2, 'Центральный', 8.5, 7.2, 'хороший', 'кирпичный', 5, 1, 'Уютная 2-комнатная квартира в центре', 'apt1.jpg')");
                query.exec("INSERT INTO apartments (price, area, rooms, district, transport, infrastructure, condition, house_type, floor, balcony, description, photo_url) "
                           "VALUES (3500000, 33.0, 1, 'Северный', 6.0, 5.5, 'новый', 'панельный', 3, 0, 'Современная 1-комнатная квартира', 'apt2.jpg')");
                query.exec("INSERT INTO apartments (price, area, rooms, district, transport, infrastructure, condition, house_type, floor, balcony, description, photo_url) "
                           "VALUES (7000000, 65.0, 3, 'Западный', 7.8, 8.0, 'новый', 'монолитный', 10, 1, 'Просторная 3-комнатная квартира', 'apt3.jpg')");
            }
        }
    }

    crow::SimpleApp server;

    CROW_ROUTE(server, "/login").methods("GET"_method)([]() {
        return crow::response(R"(<html><head><meta charset='UTF-8'></head><body>
                   <h2>Вход в систему</h2>
                   <form action='/login' method='POST'>
                   Логин: <input type='text' name='username'><br>
                   Пароль: <input type='password' name='password'><br>
                   <input type='submit' value='Войти'>
                   </form><br>
                   <a href='/register'>Нет аккаунта? Зарегистрируйтесь</a>
                   </body></html>)");
    });

    CROW_ROUTE(server, "/login").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        auto username = formData["username"];
        auto password = formData["password"];

        if (checkCredentials(username, password)) {
            crow::response res;
            res.redirect("/parameters?username=" + username);
            return res;
        } else {
            return crow::response("<html><head><meta charset='UTF-8'></head><body><h2>Неверный логин или пароль!</h2><a href='/login'>Назад</a></body></html>");
        }
    });

    CROW_ROUTE(server, "/register").methods("GET"_method)([]() {
        return crow::response(R"(<html><head><meta charset='UTF-8'></head><body>
                   <h2>Регистрация</h2>
                   <form action='/register' method='POST'>
                   Логин: <input type='text' name='username'><br>
                   Пароль: <input type='password' name='password'><br>
                   <input type='submit' value='Зарегистрироваться'>
                   </form><br>
                   <a href='/login'>Уже есть аккаунт? Войти</a>
                   </body></html>)");
    });

    CROW_ROUTE(server, "/register").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        auto username = formData["username"];
        auto password = formData["password"];
        if (registerUser(username, password)) {
            crow::response res;
            res.redirect("/parameters?username=" + username);
            return res;
        } else {
            return crow::response("<html><head><meta charset='UTF-8'></head><body><h2>Ошибка регистрации! Логин занят или ошибка базы данных.</h2><a href='/register'>Попробовать снова</a></body></html>");
        }
    });

    CROW_ROUTE(server, "/parameters").methods("GET"_method)([](const crow::request& req) -> crow::response {
        const char* username_param = req.url_params.get("username");
        std::string username = username_param ? username_param : "";

        if (username.empty()) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        // Получаем текущие параметры пользователя
        auto params = getUserParameters(username);

        // Генерируем HTML-форму с текущими параметрами
        std::ostringstream html;
        html << "<html><head><meta charset='UTF-8'>"
             << "<style>"
             << "body { font-family: Arial, sans-serif; margin: 20px; }"
             << "form { max-width: 500px; margin: 0 auto; }"
             << "label { display: block; margin-top: 10px; }"
             << "input, select { width: 100%; padding: 8px; margin-top: 5px; }"
             << "</style></head><body>"
             << "<h2>Настройки пользователя: " << username << "</h2>"
             << "<form action='/parameters' method='POST'>"
             << "<input type='hidden' name='username' value='" << username << "'>"

             << "<label>Бюджет (руб):</label>"
             << "<input type='number' name='budget' value='" << (params.count("budget") ? params.at("budget") : "") << "'>"

             << "<label>Район:</label>"
             << "<input type='text' name='district' value='" << (params.count("district") ? params.at("district") : "") << "'>"

             << "<label>Минимальная площадь (м²):</label>"
             << "<input type='number' step='0.1' name='area_min' value='" << (params.count("area_min") ? params.at("area_min") : "") << "'>"

             << "<label>Максимальная площадь (м²):</label>"
             << "<input type='number' step='0.1' name='area_max' value='" << (params.count("area_max") ? params.at("area_max") : "") << "'>"

             << "<label>Количество комнат:</label>"
             << "<input type='number' name='rooms' value='" << (params.count("rooms") ? params.at("rooms") : "") << "'>"

             << "<label>Балкон:</label>"
             << "<select name='balcony'>"
             << "<option value='есть'" << (params.count("balcony") && params.at("balcony") == "есть" ? " selected" : "") << ">Есть</option>"
             << "<option value='нет'" << (params.count("balcony") && params.at("balcony") == "нет" ? " selected" : "") << ">Нет</option>"
             << "</select>"

             << "<label>Минимальный этаж:</label>"
             << "<input type='number' name='floor_min' value='" << (params.count("floor_min") ? params.at("floor_min") : "") << "'>"

             << "<label>Максимальный этаж:</label>"
             << "<input type='number' name='floor_max' value='" << (params.count("floor_max") ? params.at("floor_max") : "") << "'>"

             << "<label>Тип дома:</label>"
             << "<select name='house_type'>"
             << "<option value='кирпичный'" << (params.count("house_type") && params.at("house_type") == "кирпичный" ? " selected" : "") << ">Кирпичный</option>"
             << "<option value='панельный'" << (params.count("house_type") && params.at("house_type") == "панельный" ? " selected" : "") << ">Панельный</option>"
             << "<option value='монолитный'" << (params.count("house_type") && params.at("house_type") == "монолитный" ? " selected" : "") << ">Монолитный</option>"
             << "</select>"

             << "<input type='submit' value='Сохранить' style='margin-top: 20px; padding: 10px;'>"
             << "</form>"

             << "<div style='margin-top: 20px;'>"
             << "<a href='/compare_criteria?username=" << username << "'>Сравнить критерии</a><br>"
             << "<a href='/view_apartments?username=" << username << "'>Просмотреть квартиры</a><br>"
             << "<a href='/login'>Выйти</a>"
             << "</div>"
             << "</body></html>";

        return crow::response(html.str());
    });

    CROW_ROUTE(server, "/parameters").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        std::string username = formData.count("username") ? formData["username"] : "";

        if (username.empty()) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        formData.erase("username"); // Удаляем username из параметров

        if (saveUserParameters(username, formData)) {
            crow::response res;
            res.redirect("/parameters?username=" + username); // Важно: добавляем username в редирект
            return res;
        } else {
            return crow::response("<html><head><meta charset='UTF-8'></head><body><h2>Ошибка сохранения параметров!</h2><a href='/parameters?username=" + username + "'>Назад</a></body></html>");
        }
    });

    CROW_ROUTE(server, "/compare_criteria").methods("GET"_method)([](const crow::request& req) -> crow::response {
        const char* username_param = req.url_params.get("username");
        std::string username = username_param ? username_param : "";

        if (username.empty()) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        return crow::response(generateComparisonPage(username));
    });

    CROW_ROUTE(server, "/compare_criteria").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        const char* username_param = formData["username"].c_str();
        std::string username = username_param ? username_param : "";

        if (username.empty()) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        size_t currentPair = formData.count("current_pair") ? std::stoul(formData["current_pair"]) : 0;
        int comparison = formData.count("comparison") ? std::stoi(formData["comparison"]) : 0;

        // Сохраняем сравнение в БД
        if (currentPair < AHP::criteria.size() * (AHP::criteria.size() - 1) / 2) {
            size_t i = 0, j = 0, count = 0;
            for (i = 0; i < AHP::criteria.size(); ++i) {
                for (j = i + 1; j < AHP::criteria.size(); ++j) {
                    if (count == currentPair) {
                        QSqlDatabase db = openDatabase();
                        if (db.isOpen()) {
                            QSqlQuery query(db);
                            query.prepare("INSERT INTO user_comparisons (username, criteria_pair, comparison_value) "
                                          "VALUES (:username, :pair, :value)");
                            query.bindValue(":username", QString::fromStdString(username));
                            query.bindValue(":pair",
                                            QString::fromStdString(AHP::criteria[i] + "-" + AHP::criteria[j]));
                            query.bindValue(":value", comparison);
                            if (!query.exec()) {
                                qDebug() << "Ошибка сохранения сравнения:" << query.lastError().text();
                            }
                        }
                        goto done_saving;
                    }
                    ++count;
                }
            }
        done_saving:;
        }

        // Переходим к следующей паре или завершаем
        currentPair++;
        if (currentPair >= AHP::criteria.size() * (AHP::criteria.size() - 1) / 2) {
            // Все пары сравнены, вычисляем веса
            auto comparisons = getComparisonsFromDB(username);
            auto matrix = AHP::createComparisonMatrix(comparisons);
            auto weights = AHP::calculateWeights(matrix);
            double cr = AHP::calculateConsistencyRatio(matrix, weights);

            return crow::response(generateWeightsPage(username, weights, cr));
        }

        return crow::response(generateComparisonPage(username, currentPair));
    });

    CROW_ROUTE(server, "/save_weights").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        const char* username_param = formData["username"].c_str();
        std::string username = username_param ? username_param : "";

        if (username.empty()) {
            crow::response res;
            res.redirect("/login");
            return res;
        }

        std::vector<double> weights(AHP::criteria.size());
        for (size_t i = 0; i < AHP::criteria.size(); ++i) {
            weights[i] = std::stod(formData[AHP::criteria[i]]);
        }

        if (saveWeights(username, weights)) {
            crow::response res;
            res.redirect("/parameters?username=" + username);
            return res;
        } else {
            return crow::response("<html><head><meta charset='UTF-8'></head><body><h2>Ошибка сохранения весов!</h2><a href='/parameters?username=" + username + "'>Назад</a></body></html>");
        }
    });

    CROW_ROUTE(server, "/view_apartments").methods("GET"_method)([](const crow::request& req) -> crow::response {
        const char* username_param = req.url_params.get("username");
        std::string username = username_param ? username_param : "";

        /*if (username.empty()) {
            crow::response res;
            res.redirect("/login");
            return res;
        }*/

        auto weights = getWeights(username);
        auto userParams = getUserParameters(username);
        auto apartments = getApartmentsFromDB();
        auto evaluated = evaluateApartments(apartments, weights, userParams);

        return crow::response(generateApartmentList(username, evaluated));
    });

    // API endpoint to get apartments data
    CROW_ROUTE(server, "/api/apartments").methods("GET"_method)([](const crow::request& req) -> crow::response {
        const char* username_param = req.url_params.get("username");
        std::string username = username_param ? username_param : "";

        if (username.empty()) {
            return crow::response(401, "Unauthorized");
        }

        auto weights = getWeights(username);
        auto userParams = getUserParameters(username);
        auto apartments = getApartmentsFromDB();
        auto evaluated = evaluateApartments(apartments, weights, userParams);

        crow::json::wvalue result;
        for (size_t i = 0; i < evaluated.size(); ++i) {
            const auto& [score, apt] = evaluated[i];
            result["apartments"][i]["id"] = apt.id;
            result["apartments"][i]["score"] = score;
            result["apartments"][i]["price"] = apt.price;
            result["apartments"][i]["area"] = apt.area;
            result["apartments"][i]["rooms"] = apt.rooms;
            result["apartments"][i]["district"] = apt.district;
            result["apartments"][i]["transport"] = apt.transport;
            result["apartments"][i]["infrastructure"] = apt.infrastructure;
            result["apartments"][i]["condition"] = apt.condition;
            result["apartments"][i]["house_type"] = apt.house_type;
            result["apartments"][i]["floor"] = apt.floor;
            result["apartments"][i]["balcony"] = apt.balcony;
            result["apartments"][i]["description"] = apt.description;
            result["apartments"][i]["photo_url"] = apt.photo_url;
        }

        return crow::response(result);
    });

    // Admin endpoint to add new apartments
    CROW_ROUTE(server, "/admin/add_apartment").methods("GET"_method)([]() {
        return crow::response(R"(<html><head><meta charset='UTF-8'></head><body>
               <h2>Добавить квартиру</h2>
               <form action='/admin/add_apartment' method='POST'>
               Цена: <input type='text' name='price'><br>
               Площадь: <input type='text' name='area'><br>
               Комнат: <input type='text' name='rooms'><br>
               Район: <input type='text' name='district'><br>
               Транспорт (0-10): <input type='text' name='transport'><br>
               Инфраструктура (0-10): <input type='text' name='infrastructure'><br>
               Состояние: <select name='condition'>
                 <option value='новый'>Новый</option>
                 <option value='хороший'>Хороший</option>
                 <option value='удовлетворительный'>Удовлетворительный</option>
               </select><br>
               Тип дома: <select name='house_type'>
                 <option value='кирпичный'>Кирпичный</option>
                 <option value='панельный'>Панельный</option>
                 <option value='монолитный'>Монолитный</option>
               </select><br>
               Этаж: <input type='text' name='floor'><br>
               Балкон: <select name='balcony'>
                 <option value='1'>Есть</option>
                 <option value='0'>Нет</option>
               </select><br>
               Описание: <textarea name='description'></textarea><br>
               URL фото: <input type='text' name='photo_url'><br>
               <input type='submit' value='Добавить'>
               </form>
               </body></html>)");
    });

    CROW_ROUTE(server, "/admin/add_apartment").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);

        QSqlDatabase db = openDatabase();
        if (!db.isOpen()) {
            return crow::response(500, "Database error");
        }

        QSqlQuery query(db);
        query.prepare("INSERT INTO apartments (price, area, rooms, district, transport, infrastructure, condition, house_type, floor, balcony, description, photo_url) "
                      "VALUES (:price, :area, :rooms, :district, :transport, :infrastructure, :condition, :house_type, :floor, :balcony, :description, :photo_url)");

        for (const auto& [key, value] : formData) {
            if (key == "balcony") {
                query.bindValue(":balcony", value == "1" ? true : false);
            } else {
                query.bindValue(QString::fromStdString(":" + key), QString::fromStdString(value));
            }
        }

        if (!query.exec()) {
            qDebug() << "Ошибка добавления квартиры:" << query.lastError().text();
            return crow::response(500, "Failed to add apartment");
        }

        crow::response res;
        res.redirect("/admin/add_apartment");
        return res;
    });

    server.port(8080).multithreaded().run();
    return app.exec();
}
