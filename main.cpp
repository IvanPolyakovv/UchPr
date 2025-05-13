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
        return 0.0;
    }
}

int safeStoi(const std::string& str) {
    try {
        return str.empty() ? 0 : std::stoi(str);
    } catch (const std::invalid_argument&) {
        return 0;
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

            // Добавляем тестовые данные, если таблица apartments пуста
            query.exec("SELECT COUNT(*) FROM apartments");
            if (query.next() && query.value(0).toInt() == 0) {
                query.exec(R"(
        INSERT INTO apartments (description, district, price, area, rooms, balcony, house_type, floor, transport, infrastructure, condition, lat, lon)
        VALUES
            ("Гродненский переулок, 16", "Центральный", 13770000, 48, 1, 0, "кирпичный", 2, 8.5, 7.2, "хороший", 59.941786, 30.362428),
            ("19-я линия В.О., 12", "Василеостровский", 19800000, 87, 2, 0, "кирпичный", 4, 6.0, 5.5, "хороший", 59.933722, 30.268787),
            ("Долгоозёрная улица, 6/2", "Приморский", 12990000, 88, 4, 1, "панельный", 7, 7.8, 8.0, "новый", 60.015997, 30.235640),
            ("Улица Крыленко, 14/3", "Невский", 9744000, 53, 2, 1, "монолитный", 2, 8.0, 7.5, "новый", 59.895772, 30.465483),
            ("Проспект Луначарского, 104/2", "Калининский", 19000000, 80, 3, 1, "панельный", 12, 8.5, 8.0, "хороший", 60.025866, 30.412850),
            ("Аптекарский проспект, 5", "Петроградский", 39980000, 104, 3, 1, "монолитный", 5, 9.0, 8.5, "новый", 59.973559, 30.321698),
            ("Московский проспект, 205А", "Московский", 7950000, 27, 1, 0, "кирпичный", 8, 9.5, 7.0, "хороший", 59.846875, 30.320944),
            ("Будапештская улица, 51", "Фрунзенский", 9800000, 51, 2, 1, "монолитный", 8, 6.2, 6.3, "хороший", 59.851964, 30.381795),
            ("Проспект Ветеранов, 194", "Красносельский", 7999000, 33, 1, 1, "монолитный", 2, 7.0, 7.1, "новый", 59.834917, 30.114672),
            ("Колпино, Тверская улица, 38", "Колпинский", 6300000, 46, 2, 1, "панельный", 8, 4.0, 6.0, "хороший", 59.739350, 30.611216),
            ("Кронштадт, Посадская улица, 17/14", "Кронштадтский", 5000000, 50, 1, 0, "панельный", 1, 5.5, 5.8, "хороший", 59.997185, 29.762290),
            ("Зеленогорск, Комсомольская улица, 3", "Курортный", 11500000, 48, 2, 0, "кирпичный", 9, 5.8, 7.0, "новый", 60.199424, 29.706900)
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
            int apartmentId = std::stoi(formData["apartment_id"]);

            if (isFavorite(username, apartmentId)) {
                removeFromFavorites(username, apartmentId);
            } else {
                addToFavorites(username, apartmentId);
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

            std::string username = username_param ? username_param : "";
            if (username.empty()) {
                return crow::response(400, "Нет имени пользователя");
            }

            // Получаем список всех квартир
            auto apartments = getTestApartments();

            // Генерация страницы сравнения
            return crow::response(generateComparisonPage(username, apartments));
        });

    CROW_ROUTE(server, "/compare_result")
        .methods("GET"_method)([](const crow::request& req) {
            const char* username_param = req.url_params.get("username");
            const char* apartments_param = req.url_params.get("apartments");
            const char* apartment1_param = req.url_params.get("apartment1");
            const char* apartment2_param = req.url_params.get("apartment2");

            std::string username = username_param ? username_param : "";
            if (username.empty()) {
                return crow::response(400, "Не указано имя пользователя (username)");
            }

            std::vector<int> apartmentIds;

            // Если передан параметр apartments
            if (apartments_param) {
                std::stringstream ss(apartments_param);
                std::string id;
                while (std::getline(ss, id, ',')) {
                    try {
                        int apartmentId = std::stoi(id);
                        apartmentIds.push_back(apartmentId);
                    } catch (const std::invalid_argument&) {
                        return crow::response(400, "Недопустимый формат ID квартир: " + id);
                    }
                }
            }
            // Если переданы параметры apartment1 и apartment2
            else if (apartment1_param && apartment2_param) {
                try {
                    apartmentIds.push_back(std::stoi(apartment1_param));
                    apartmentIds.push_back(std::stoi(apartment2_param));
                } catch (const std::invalid_argument&) {
                    return crow::response(400, "Недопустимый формат ID квартир");
                }
            }
            else {
                return crow::response(400, "Не указаны квартиры для сравнения");
            }

            // Проверяем, что выбрано как минимум две квартиры
            if (apartmentIds.size() < 2) {
                return crow::response(400, "Выберите как минимум две квартиры для сравнения");
            }

            // Получаем список всех квартир
            auto apartments = getTestApartments();
            std::vector<Apartment> selectedApartments;

            // Ищем выбранные квартиры
            for (const auto& apt : apartments) {
                if (std::find(apartmentIds.begin(), apartmentIds.end(), apt.id) != apartmentIds.end()) {
                    selectedApartments.push_back(apt);
                }
            }

            // Проверяем, что все выбранные квартиры найдены
            if (selectedApartments.size() != apartmentIds.size()) {
                return crow::response(400, "Не удалось найти одну из квартир");
            }

            // Генерация страницы с результатами сравнения
            return crow::response(generateComparisonResultsPage(username, selectedApartments));
        });

    server.port(8080).multithreaded().run();
    return app.exec();
}
