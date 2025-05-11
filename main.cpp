#include "crow/crow.h"
#include "ahp.h"
#include "database.h"
#include "utils.h"
#include "webui.h"
#include <QCoreApplication>
#include <QSqlQuery>

double safeStod(const std::string& str) {
    try {
        return std::stod(str);
    } catch (const std::invalid_argument&) {
        return 0.0; // Возвращаем значение по умолчанию
    }
}

int safeStoi(const std::string& str) {
    try {
        return str.empty() ? 0 : std::stoi(str);
    } catch (const std::invalid_argument&) {
        return 0; // Return default value
    }
}

std::vector<Apartment> filterApartments(const std::vector<Apartment>& apartments, const std::map<std::string, std::string>& filters) {
    std::vector<Apartment> filtered;

    for (const auto& apt : apartments) {
        bool matches = true;

        if (filters.count("budget") && !filters.at("budget").empty() && apt.price > safeStod(filters.at("budget"))) {
            matches = false;
        }
        if (filters.count("area_min") && !filters.at("area_min").empty() && apt.area < safeStod(filters.at("area_min"))) {
            matches = false;
        }
        if (filters.count("area_max") && !filters.at("area_max").empty() && apt.area > safeStod(filters.at("area_max"))) {
            matches = false;
        }
        if (filters.count("rooms") && !filters.at("rooms").empty() && apt.rooms != safeStoi(filters.at("rooms"))) {
            matches = false;
        }
        if (filters.count("district") && !filters.at("district").empty() && apt.district != filters.at("district")) {
            matches = false;
        }
        if (filters.count("house_type") && !filters.at("house_type").empty() && apt.house_type != filters.at("house_type")) {
            matches = false;
        }
        if (filters.count("floor_min") && !filters.at("floor_min").empty() && apt.floor < safeStoi(filters.at("floor_min"))) {
            matches = false;
        }
        if (filters.count("floor_max") && !filters.at("floor_max").empty() && apt.floor > safeStoi(filters.at("floor_max"))) {
            matches = false;
        }
        if (filters.count("balcony") && !filters.at("balcony").empty()) {
            std::string balcony = filters.at("balcony");
            if ((balcony == "1" && !apt.balcony) || (balcony == "0" && apt.balcony)) {
                matches = false;
            }
        }

        if (matches) {
            filtered.push_back(apt);
        }
    }

    return filtered;
}

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
                       "transport TEXT,"
                       "lat REAL,"
                       "lon REAL)");

            query.exec("CREATE TABLE IF NOT EXISTS user_preferences ("
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
                       "transport TEXT)"
                       );

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
                       "photo_url TEXT,"
                       "lat REAL,"
                       "lon REAL)");

            // Вставка тестовых данных, если таблица пуста
            query.exec("CREATE TABLE IF NOT EXISTS favorites ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT,"
                       "apartment_id INTEGER,"
                       "FOREIGN KEY(apartment_id) REFERENCES apartments(id))");
            query.exec("CREATE TABLE IF NOT EXISTS favorites ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "username TEXT,"
                       "apartment_id INTEGER,"
                       "FOREIGN KEY(apartment_id) REFERENCES apartments(id))");

            // Добавляем тестовые данные, если таблица apartments пуста
            query.exec("SELECT COUNT(*) FROM apartments");
            if (query.next() && query.value(0).toInt() == 0) {
                query.exec(R"(
                INSERT INTO apartments (description, district, price, area, rooms, balcony, house_type, floor, transport, infrastructure, condition, lat, lon)
                VALUES
                    ("Квартира в центре", "Центральный", 10000000, 80, 3, 1, "кирпичный", 5, 8.5, 7.2, "новый", 59.934280, 30.335099),
                    ("Квартира на Васильевском острове", "Василеостровский", 8000000, 70, 2, 0, "панельный", 3, 6.0, 5.5, "хороший", 59.941000, 30.259000),
                    ("Квартира в Приморском районе", "Приморский", 7000000, 60, 1, 1, "монолитный", 7, 7.8, 8.0, "новый", 60.001073, 30.265886),
                    ("Квартира в новостройке", "Невский", 9000000, 75, 2, 1, "монолитный", 10, 8.0, 7.5, "новый", 59.931000, 30.363000),
                    ("Вторичка в Калининском районе", "Калининский", 6500000, 65, 2, 0, "панельный", 2, 6.5, 6.0, "хороший", 59.960000, 30.390000)
            )");
            }
        }
    }

    crow::SimpleApp server;

    // Маршруты
    CROW_ROUTE(server, "/login").methods("GET"_method)([]() {
        return crow::response(generateLoginPage());
    });

    CROW_ROUTE(server, "/login").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        auto username = formData["username"];
        auto password = formData["password"];

        if (checkCredentials(username, password)) {
            crow::response res;
            res.redirect("/view_apartments?username=" + username);
            return res;
        } else {
            return crow::response("<html><head><meta charset='UTF-8'></head><body><h2>Неверный логин или пароль!</h2><a href='/login'>Назад</a></body></html>");
        }
    });

    CROW_ROUTE(server, "/register").methods("GET"_method)([]() {
        return crow::response(generateRegisterPage());
    });

    CROW_ROUTE(server, "/register").methods("POST"_method)([](const crow::request& req) {
        auto formData = parseFormData(req.body);
        auto username = formData["username"];
        auto password = formData["password"];

        // Собираем предпочтения
        std::map<std::string, std::string> preferences;
        preferences["budget"] = formData["budget"];
        preferences["district"] = formData["district"];
        preferences["type"] = formData["type"];
        preferences["area_min"] = formData["area_min"];
        preferences["area_max"] = formData["area_max"];
        preferences["rooms"] = formData["rooms"];
        preferences["balcony"] = formData["balcony"];
        preferences["floor_min"] = formData["floor_min"];
        preferences["floor_max"] = formData["floor_max"];
        preferences["house_type"] = formData["house_type"];

        if (registerUser(username, password, preferences)) {
            crow::response res;
            res.redirect("/select_criteria?username=" + username);
            return res;
        } else {
            return crow::response("<html><head><meta charset='UTF-8'></head><body><h2>Ошибка регистрации! Логин занят или ошибка базы данных.</h2><a href='/register'>Попробовать снова</a></body></html>");
        }
    });

    CROW_ROUTE(server, "/select_criteria").methods("GET"_method, "POST"_method)([](const crow::request& req) -> crow::response {
        const char* username_param = req.url_params.get("username");
        std::string username = username_param ? username_param : "";

        if (username.empty()) {
            return crow::response(400, "Нет имени пользователя");
        }

        return crow::response(generateCriteriaSelectionPage(username));
    });

    CROW_ROUTE(server, "/save_criteria").methods("POST"_method)([](const crow::request& req) -> crow::response {
        auto formData = parseFormData(req.body);
        std::string username = formData["username"];

        std::map<std::string, int> comparisons;
        for (size_t i = 0; i < AHP::criteria.size(); ++i) {
            for (size_t j = i + 1; j < AHP::criteria.size(); ++j) {
                std::string key = AHP::criteria[i] + "-" + AHP::criteria[j];
                if (formData.count(key)) {
                    try {
                        int value = std::stoi(formData[key]);
                        comparisons[key] = value;
                    } catch (const std::invalid_argument&) {
                        std::cerr << "Ошибка: недопустимое значение для пары " << key << "\n";
                        return crow::response(400, "Недопустимое значение для пары " + key);
                    }
                }
            }
        }

        auto matrix = AHP::createComparisonMatrix(comparisons);
        auto weights = AHP::calculateWeights(matrix);
        double CR = AHP::calculateConsistencyRatio(matrix, weights);

        saveWeights(username, weights);

        crow::response res;
        res.redirect("/view_apartments?username=" + username);
        return res;
    });


    CROW_ROUTE(server, "/save_preferences").methods("POST"_method)([](const crow::request& req) {
        auto formData = parseFormData(req.body);
        std::string username = formData["username"];

        std::map<std::string, std::string> preferences;
        preferences["budget"] = formData["budget"];
        preferences["district"] = formData["district"];
        preferences["type"] = formData["type"];
        preferences["area_min"] = formData["area_min"];
        preferences["area_max"] = formData["area_max"];
        preferences["rooms"] = formData["rooms"];
        preferences["balcony"] = formData["balcony"];
        preferences["floor_min"] = formData["floor_min"];
        preferences["floor_max"] = formData["floor_max"];
        preferences["house_type"] = formData["house_type"];

        if (saveUserParameters(username, preferences)) {
            crow::response res;
            res.redirect("/select_criteria?username=" + username);
            return res;
        } else {
            return crow::response(500, "Ошибка сохранения предпочтений");
        }
    });

    CROW_ROUTE(server, "/view_apartments")
        .methods("GET"_method, "POST"_method)
        ([](const crow::request& req) {
            const char* username_param = req.url_params.get("username");
            std::string username = username_param ? username_param : "";

            if (username.empty()) {
                return crow::response(400, "Нет имени пользователя");
            }

            // Получение предпочтений пользователя
            auto preferences = getUserParameters(username);

            // Получение фильтров
            std::map<std::string, std::string> filters;

            if (req.method == "GET"_method) {
                // Если параметр load_preferences=true, используем предпочтения из профиля
                const char* load_preferences_param = req.url_params.get("load_preferences");
                if (load_preferences_param && std::string(load_preferences_param) == "true") {
                    filters = preferences;
                } else {
                    // Для GET-запросов параметры берутся из URL
                    filters["budget"] = req.url_params.get("budget") ? req.url_params.get("budget") : "";
                    filters["area_min"] = req.url_params.get("area_min") ? req.url_params.get("area_min") : "";
                    filters["area_max"] = req.url_params.get("area_max") ? req.url_params.get("area_max") : "";
                    filters["rooms"] = req.url_params.get("rooms") ? req.url_params.get("rooms") : "";
                    filters["district"] = req.url_params.get("district") ? req.url_params.get("district") : "";
                    filters["house_type"] = req.url_params.get("house_type") ? req.url_params.get("house_type") : "";
                    filters["floor_min"] = req.url_params.get("floor_min") ? req.url_params.get("floor_min") : "";
                    filters["floor_max"] = req.url_params.get("floor_max") ? req.url_params.get("floor_max") : "";
                    filters["balcony"] = req.url_params.get("balcony") ? req.url_params.get("balcony") : "";
                }
            } else if (req.method == "POST"_method) {
                // Для POST-запросов параметры берутся из тела запроса
                auto formData = parseFormData(req.body);
                filters = formData;
            }

            // Получение квартир
            auto apartments = getTestApartments();

            // Фильтрация квартир
            auto filteredApartments = filterApartments(apartments, filters);

            // Получение весов и параметров пользователя
            auto weights = getWeights(username);
            auto userParams = getUserParameters(username);

            // Оценка квартир
            auto evaluated = evaluateApartments(filteredApartments, weights, userParams);

            // Генерация HTML
            return crow::response(generateApartmentList(username, evaluated, filters));
        });


    CROW_ROUTE(server, "/profile")
        .methods("GET"_method, "POST"_method)([](const crow::request& req) -> crow::response {
            const char* username_param = req.url_params.get("username");
            std::string username = username_param ? username_param : "";

            if (username.empty()) {
                return crow::response(400, "Нет имени пользователя");
            }

            if (req.method == "GET"_method) {
                auto weights = getWeights(username);
                auto params = getUserParameters(username);

                // Получаем сравнения из базы данных
                auto comparisons = getComparisonsFromDB(username);

                // Создаем матрицу сравнений и вычисляем CR
                auto matrix = AHP::createComparisonMatrix(comparisons);
                double CR = AHP::calculateConsistencyRatio(matrix, weights);

                return crow::response(generateProfilePage(username, weights, params, CR));
            } else if (req.method == "POST"_method) {
                auto formData = parseFormData(req.body);
                saveUserParameters(username, formData);
                return crow::response(200, "Профиль обновлен");
            } else {
                return crow::response(405, "Метод не поддерживается");
            }
        });


    CROW_ROUTE(server, "/edit_profile")
        .methods("GET"_method, "POST"_method)
        ([](const crow::request& req) {
            std::string username;

            if (req.method == "GET"_method) {
                // Извлечение username из URL для GET-запроса
                const char* username_param = req.url_params.get("username");
                username = username_param ? username_param : "";
            } else if (req.method == "POST"_method) {
                // Извлечение username из тела запроса для POST-запроса
                auto formData = parseFormData(req.body);
                if (!formData.count("username")) {
                    return crow::response(400, "Не указано имя пользователя");
                }
                username = formData["username"];
            } else {
                return crow::response(405, "Метод не поддерживается");
            }

            if (username.empty()) {
                return crow::response(400, "Нет имени пользователя");
            }

            if (req.method == "GET"_method) {
                // Показать форму редактирования
                auto params = getUserParameters(username);
                return crow::response(generateEditProfilePage(username, params));
            } else if (req.method == "POST"_method) {
                // Сохранить предпочтения
                auto formData = parseFormData(req.body);
                if (saveUserParameters(username, formData)) {
                    crow::response res;
                    res.redirect("/view_apartments?username=" + username);
                    return res;
                } else {
                    return crow::response(500, "Ошибка сохранения предпочтений");
                }
            }

            return crow::response(400, "Неизвестная ошибка");
        });


    // Добавление/удаление из избранного
    CROW_ROUTE(server, "/toggle_favorite")
        .methods("POST"_method)([](const crow::request& req) {
            auto formData = parseFormData(req.body);
            std::string username = formData["username"];
            int apartmentId = 0;
            try {
                apartmentId = std::stoi(formData["apartment_id"]);
            } catch (const std::exception& e) {
                qDebug() << "Ошибка преобразования apartment_id:" << e.what();
                return crow::response(400, "Invalid apartment_id");
            }

            qDebug() << "Запрос на добавление/удаление из избранного. Пользователь:" << username << ", ID квартиры:" << apartmentId;

            if (isFavorite(username, apartmentId)) {
                if (removeFromFavorites(username, apartmentId)) {
                    qDebug() << "Квартира удалена из избранного";
                } else {
                    qDebug() << "Ошибка при удалении из избранного";
                }
            } else {
                if (addToFavorites(username, apartmentId)) {
                    qDebug() << "Квартира добавлена в избранное";
                } else {
                    qDebug() << "Ошибка при добавлении в избранное";
                }
            }

            crow::response res;
            res.redirect("/view_apartments?username=" + username);
            return res;
        });



    // Страница избранного
    CROW_ROUTE(server, "/favorites")
        .methods("GET"_method)([](const crow::request& req) {
            const char* username_param = req.url_params.get("username");
            std::string username = username_param ? username_param : "";

            if (username.empty()) {
                return crow::response(400, "Нет имени пользователя");
            }

            auto favorites = getFavorites(username);
            std::vector<std::pair<double, Apartment>> evaluated;
            for (const auto& apt : favorites) {
                evaluated.emplace_back(100.0, apt); // Условный максимальный балл
            }

            return crow::response(generateFavoritesPage(username, evaluated));
        });

    CROW_ROUTE(server, "/compare")
        .methods("GET"_method)([](const crow::request& req) {
            const char* username_param = req.url_params.get("username");
            const char* apartment_id_param = req.url_params.get("apartment_id");

            std::string username = username_param ? username_param : "";
            if (username.empty()) {
                return crow::response(400, "Нет имени пользователя");
            }

            // Получаем список всех квартир
            auto apartments = getTestApartments();

            // Если передан ID квартиры, показываем список для выбора второй квартиры
            if (apartment_id_param) {
                int first_apartment_id = std::stoi(apartment_id_param);
                std::ostringstream html;

                html << R"(<html><head><meta charset='UTF-8'>
            <style>
                body {
                    font-family: Bahnschrift, sans-serif;
                    background-color: #E0F7FE;
                    margin: 0;
                    padding: 20px;
                    color: #002366;
                }
                h2 {
                    color: #002366;
                    text-align: center;
                    margin-bottom: 20px;
                }
                .apartment-list {
                    display: flex;
                    flex-wrap: wrap;
                    gap: 20px;
                    justify-content: center;
                }
                .apartment-item {
                    background-color: white;
                    padding: 20px;
                    border-radius: 10px;
                    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
                    width: 300px;
                    transition: transform 0.2s, box-shadow 0.2s;
                    box-sizing: border-box; /* Учитываем padding в ширине */
                }
                .apartment-item:hover {
                    transform: translateY(-5px);
                    box-shadow: 0 6px 12px rgba(0, 0, 0, 0.15);
                }
                .apartment-item h3 {
                    margin: 0 0 10px 0;
                    color: #002366;
                    font-size: 1.5em;
                }
                .apartment-item p {
                    margin: 5px 0;
                    color: #002366;
                    font-size: 1.1em;
                }
                .compare-button {
                    display: inline-block;
                    background-color: #002366;
                    color: white;
                    padding: 10px 20px;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                    text-decoration: none;
                    text-align: center;
                    font-size: 1em;
                    margin-top: 10px;
                    width: calc(100% - 40px); /* Учитываем padding контейнера */
                    box-sizing: border-box; /* Учитываем padding в ширине */
                    transition: background-color 0.2s;
                }
                .compare-button:hover {
                    background-color: #001F3F;
                }
                .back-button {
                    display: inline-block;
                    background-color: #002366;
                    color: white;
                    padding: 10px 20px;
                    border: none;
                    border-radius: 5px;
                    cursor: pointer;
                    text-decoration: null;
                    text-align: center;
                    font-size: 1em;
                    margin-top: 20px;
                    width: calc(100% - 40px); /* Учитываем padding контейнера */
                    box-sizing: border-box; /* Учитываем padding в ширине */
                    transition: background-color 0.2s;
                }
                .back-button:hover {
                    background-color: #001F3F;
                }
                .container {
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 0 20px;
                    box-sizing: border-box; /* Учитываем padding в ширине */
                }
            </style>
            </head><body>
            <div class="container">
                <h2>Выберите вторую квартиру для сравнения</h2>
                <div class="apartment-list">)";

                for (const auto& apt : apartments) {
                    if (apt.id != first_apartment_id) {
                        html << R"(<div class="apartment-item">
                    <h3>)" << apt.description << R"(</h3>
                    <p>Район: )" << apt.district << R"(</p>
                    <p>Цена: )" << formatPrice(apt.price) << R"(</p>
                    <a href="/compare_result?username=)" << username
                             << "&apartment1=" << first_apartment_id
                             << "&apartment2=" << apt.id
                             << R"(" class="compare-button">Сравнить с этой</a>
                </div>)";
                    }
                }

                html << R"(</div>
                <a href="/view_apartments?username=)" << username << R"(" class="back-button">Назад к списку</a>
            </div>
            </body></html>)";

                return crow::response(html.str());
            }

            // Если ID квартиры не передан, показываем обычную страницу сравнения
            return crow::response(generateComparisonPage(username, apartments));
        });



    CROW_ROUTE(server, "/compare_result")
        .methods("GET"_method)([](const crow::request& req) {
            const char* username_param = req.url_params.get("username");
            const char* apartment1_param = req.url_params.get("apartment1");
            const char* apartment2_param = req.url_params.get("apartment2");

            std::string username = username_param ? username_param : "";
            int apartment1_id = apartment1_param ? std::stoi(apartment1_param) : -1;
            int apartment2_id = apartment2_param ? std::stoi(apartment2_param) : -1;

            if (username.empty() || apartment1_id == -1 || apartment2_id == -1) {
                return crow::response(400, "Неверные параметры запроса");
            }

            // Получаем квартиры для сравнения
            auto apartments = getTestApartments();
            std::vector<Apartment> selectedApartments;

            for (const auto& apt : apartments) {
                if (apt.id == apartment1_id || apt.id == apartment2_id) {
                    selectedApartments.push_back(apt);
                }
            }

            if (selectedApartments.size() != 2) {
                return crow::response(400, "Не удалось найти одну из квартир");
            }

            return crow::response(generateComparisonResultsPage(username, selectedApartments));
        });
    server.port(8080).multithreaded().run();
    return app.exec();
}
