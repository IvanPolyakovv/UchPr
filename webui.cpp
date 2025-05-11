#include "webui.h"
#include "ahp.h"
#include "database.h"
#include "qdebug.h"
#include "qlogging.h"
#include <sstream>
#include <iomanip>
#include <iomanip>
#include <locale> // Для std::numpunct

// Функция для форматирования цены
std::string formatPrice(double price) {
    std::ostringstream oss;
    oss.imbue(std::locale("")); // Используем системную локаль для разделителя тысяч
    oss << std::fixed << std::setprecision(0) << price; // Без дробной части
    return oss.str() + " руб.";
}
std::vector<std::string> getDistricts() {
    return {
        "Адмиралтейский", "Василеостровский", "Выборгский", "Калининский", "Кировский",
        "Колпинский", "Красногвардейский", "Красносельский", "Кронштадтский", "Курортный",
        "Московский", "Невский", "Петроградский", "Петродворцовый", "Приморский",
        "Пушкинский", "Фрунзенский", "Центральный"
    };
}

std::string generateLoginPage() {
    return R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        form { max-width: 300px; margin: 0 auto; background-color: #C0E0F0; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
        label { display: block; margin-bottom: 10px; font-weight: bold; }
        input[type='text'], input[type='password'] { width: 100%; padding: 8px; margin-top: 5px; border: 1px solid #99C2E0; border-radius: 4px; background-color: #D0E8FF; color: #002366; }
        input[type='submit'] { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; width: 100%; }
        input[type='submit']:hover { background-color: #002366; }
        a { color: #002366; text-decoration: none; display: block; text-align: center; margin-top: 10px; }
        a:hover { text-decoration: underline; }
    </style>
    </head><body>
    <h2>Вход в систему</h2>
    <form action='/login' method='POST'>
    <label>Логин: <input type='text' name='username'></label>
    <label>Пароль: <input type='password' name='password'></label>
    <input type='submit' value='Войти'>
    </form>
    <a href='/register'>Нет аккаунта? Зарегистрируйтесь</a>
    </body></html>)";
}

std::string generateRegisterPage() {
    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        form { max-width: 600px; margin: 0 auto; background-color: #C0E0F0; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
        label { display: block; margin-bottom: 10px; font-weight: bold; }
        input[type='text'], input[type='password'], input[type='number'], select { width: 100%; padding: 8px; margin-top: 5px; border: 1px solid #99C2E0; border-radius: 4px; background-color: #D0E8FF; color: #002366; }
        input[type='submit'] { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; width: 100%; }
        input[type='submit']:hover { background-color: #002366; }
    </style>
    <script>
        function preventNegative(event) {
            if (event.target.value < 0) {
                event.target.value = 0;
            }
        }
    </script>
    </head><body>
    <h2>Регистрация</h2>
    <form action='/register' method='POST'>
        <label>Логин: <input type='text' name='username' required></label>
        <label>Пароль: <input type='password' name='password' required></label>
        <h3>Предпочтения</h3>
        <label>Бюджет (руб.): <input type='number' name='budget' min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"( required></label>
                                          <label>Район:
                                              <select name='district' required>
                                          <option value="">Выберите район</option>
                                          <option value="Адмиралтейский">Адмиралтейский</option>
                                          <option value="Василеостровский">Василеостровский</option>
                                          <option value="Выборгский">Выборгский</option>
                                          <option value="Калининский">Калининский</option>
                                          <option value="Кировский">Кировский</option>
                                          <option value="Колпинский">Колпинский</option>
                                          <option value="Красногвардейский">Красногвардейский</option>
                                          <option value="Красносельский">Красносельский</option>
                                          <option value="Кронштадтский">Кронштадтский</option>
                                          <option value="Курортный">Курортный</option>
                                          <option value="Московский">Московский</option>
                                          <option value="Невский">Невский</option>
                                          <option value="Петроградский">Петроградский</option>
                                          <option value="Петродворцовый">Петродворцовый</option>
                                          <option value="Приморский">Приморский</option>
                                          <option value="Пушкинский">Пушкинский</option>
                                          <option value="Фрунзенский">Фрунзенский</option>
                                          <option value="Центральный">Центральный</option>
                                          </select>
                                          </label>
                                          <label>Тип квартиры:
                                              <select name='type' required>
                                          <option value='новостройка'>Новостройка</option>
                                          <option value='вторичка'>Вторичка</option>
                                          </select>
                                          </label>
                                          <label>Площадь (м²):
                                              <input type='number' name='area_min' placeholder='Минимальная площадь' min="0" oninput="preventNegative(event)"<< R"())"<<R"(")"<<R"( required>
                                          <input type='number' name='area_max' placeholder='Максимальная площадь' min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"( required>
                                          </label>
                                          <label>Количество комнат: <input type='number' name='rooms' min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"( required></label>
                                          <label>Балкон:
                                              <select name='balcony' required>
                                          <option value='1'>Есть</option>
                                          <option value='0'>Нет</option>
                                          </select>
                                          </label>
                                          <label>Этаж:
                                              <input type='number' name='floor_min' placeholder='Минимальный этаж' min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"( required>
                                          <input type='number' name='floor_max' placeholder='Максимальный этаж' min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"( required>
                                          </label>
                                          <label>Тип дома:
                                              <select name='house_type' required>
                                          <option value='кирпичный'>Кирпичный</option>
                                          <option value=' панельный'>Панельный</option>
                                          <option value='монолитный'>Монолитный</option>
                                          </select>
                                          </label>
                                          <input type='submit' value='Зарегистрироваться'>
                                          </form>
                                          <a href='/login'>Уже есть аккаунт? Войти</a>
                                   </body></html>)";
    return html.str();
}


std::string generatePreferencesPage(const std::string& username) {
    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        form { max-width: 600px; margin: 0 auto; background-color: #C0E0F0; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); }
        label { display: block; margin-bottom: 10px; font-weight: bold; }
        input[type='text'], input[type='number'], select { width: 100%; padding: 8px; margin-top: 5px; border: 1px solid #99C2E0; border-radius: 4px; background-color: #D0E8FF; color: #002366; }
        input[type='submit'] { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; width: 100%; }
        input[type='submit']:hover { background-color: #002366; }
    </style>
    </head><body>
    <h2>Выбор предпочтений</h2>
    <form action='/save_preferences' method='POST'>
        <input type='hidden' name='username' value=')" << username << R"('>
        <label>Бюджет (руб.): <input type='number' name='budget'></label>
        <label>Район: <input type='text' name='district'></label>
        <label>Тип квартиры:
            <select name='type'>
                <option value='новостройка'>Новостройка</option>
                <option value='вторичка'>Вторичка</option>
            </select>
        </label>
        <label>Площадь (м²):
            <input type='number' name='area_min' placeholder='Минимальная площадь'>
            <input type='number' name='area_max' placeholder='Максимальная площадь'>
        </label>
        <label>Количество комнат: <input type='number' name='rooms'></label>
        <label>Балкон:
            <select name='balcony'>
                <option value='1'>Есть</option>
                <option value='0'>Нет</option>
            </select>
        </label>
        <label>Этаж:
            <input type='number' name='floor_min' placeholder='Минимальный этаж'>
            <input type='number' name='floor_max' placeholder='Максимальный этаж'>
        </label>
        <label>Тип дома:
            <select name='house_type'>
                <option value='кирпичный'>Кирпичный</option>
                <option value='панельный'>Панельный</option>
                <option value='монолитный'>Монолитный</option>
            </select>
        </label>
        <input type='submit' value='Сохранить предпочтения'>
    </form>
    </body></html>)";
    return html.str();
}

std::string generateApartmentList(const std::string& username,
                                  const std::vector<std::pair<double, Apartment>>& evaluated,
                                  const std::map<std::string, std::string>& filters) {
    std::ostringstream html;

    // Начало HTML
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; margin: 0; padding: 0; display: flex; flex-direction: column; height: 100vh; background-color: #E0F7FE; color: #002366; }
        .header { background-color: #002366; color: white; padding: 10px; display: flex; justify-content: space-between; }
        .header a { color: white; text-decoration: none; margin: 0 10px; }
        .container { display: flex; flex: 1; }
        .filters { width: 300px; background-color: #C0E0F0; padding: 20px; }
        .map { flex: 1; position: relative; height: 100%; }
        .apartments-list { width: 300px; background-color: #C0E0F0; padding: 20px; overflow-y: auto; height: calc(100vh - 100px); }
        .apartment-item { margin-bottom: 10px; padding: 10px; background-color: white; border-radius: 5px; box-shadow: 0 0 5px rgba(0, 0, 0, 0.1); }
        .apartment-item h3 { margin: 0; color: #002366; }
        .apartment-item p { margin: 5px 0; color: #002366; }
        .error { color: red; font-size: 0.9em; }
        .filter-group { margin-bottom: 15px; }
        .filter-group label { display: block; font-weight: bold; margin-bottom: 5px; color: #002366; }
        .filter-group input, .filter-group select { width: 100%; padding: 8px; border: 1px solid #99C2E0; border-radius: 4px; background-color: #D0E8FF; color: #002366; }
        .button {
            background-color: #002366; /* Синий цвет */
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            text-decoration: none;
            display: inline-block;
            margin-top: 10px;
            text-align: center;
            font-family: Bahnschrift, sans-serif;
            font-size: 14px;
            height: 40px; /* Фиксированная высота */
            line-height: 20px; /* Выравнивание текста по центру */
            box-sizing: border-box; /* Учитываем padding в высоте */
        }

        .button:hover {
            background-color: #001F3F; /* Темно-синий цвет при наведении */
        }

        .favorite-button {
            background-color: #002366; /* Синий цвет по умолчанию */
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            text-decoration: none;
            display: inline-block;
            margin-top: 10px;
            text-align: center;
            font-family: Bahnschrift, sans-serif;
            font-size: 14px;
            height: 40px; /* Фиксированная высота */
            line-height: 20px; /* Выравнивание текста по центру */
            box-sizing: border-box; /* Учитываем padding в высоте */
        }

        .favorite-button.active {
            background-color: #4CAF50; /* Зеленый цвет, если квартира в избранном */
        }

        .favorite-button.active:hover {
            background-color: #45a049; /* Темно-зеленый цвет при наведении */
        }

    </style>

    <script src="https://api-maps.yandex.ru/2.1/?apikey=77887e3b-ee30-4865-9650-62a3a5ae40b0&lang=ru_RU" type="text/javascript"></script>
    <script>
        function initMap() {
            ymaps.ready(function() {
                var map = new ymaps.Map('map', {
                    center: [59.934280, 30.335099], // Санкт-Петербург
                    zoom: 11
                });

                var apartments = )" << generateApartmentsJson(evaluated) << R"(;

                apartments.forEach(function(apt) {
                    var placemark = new ymaps.Placemark([apt.lat, apt.lon], {
                        hintContent: apt.description,
                        balloonContent: apt.description
                    });
                    map.geoObjects.add(placemark);
                });
            });
        }

        function preventNegative(event) {
            if (event.target.value < 0) {
                event.target.value = 0;
            }
        }
    </script>
    </head><body onload="initMap()"<<R"())"<<R"(")"<<R"(>
        <div class="header">
        <div>
        <a href="/favorites?username=)" << username << R"(" class="button">Избранное</a>
                <a href="/profile?username=)" << username << R"(" class="button">Профиль</a>
            </div>
            <div>)" << username << R"(</div>
        </div>
        <div class="container">
            <div class="filters">
                <h3>Фильтры</h3>
                <form action="/view_apartments" method="GET">
                    <input type="hidden" name="username" value=")" << username << R"(">
                    <!-- Бюджет -->
                    <div class="filter-group">
                        <label>Бюджет (руб.):</label>
                        <input type="number" name="budget" value=")" << (filters.count("budget") ? filters.at("budget") : "") << R"(" placeholder="Максимальный бюджет" min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"(>
                    </div>

                    <!-- Площадь -->
                    <div class="filter-group">
                        <label>Площадь (м²):</label>
                        <input type="number" name="area_min" value=")" << (filters.count("area_min") ? filters.at("area_min") : "") << R"(" placeholder="Минимальная площадь" min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"(>
                        <input type="number" name="area_max" value=")" << (filters.count("area_max") ? filters.at("area_max") : "") << R"(" placeholder="Максимальная площадь" min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"(>
                    </div>

                    <!-- Количество комнат -->
                    <div class="filter-group">
                        <label>Количество комнат:</label>
                        <input type="number" name="rooms" value=")" << (filters.count("rooms") ? filters.at("rooms") : "") << R"(" placeholder="Количество комнат" min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"(>
                    </div>

                    <!-- Район -->
                    <div class="filter-group">
                        <label>Район:</label>
                        <select name="district">
                            <option value="">Любой</option>)";

    // Добавление районов Санкт-Петербурга в выпадающий список
    auto districts = getDistricts();
    for (const auto& district : districts) {
        html << "<option value=\"" << district << "\""
             << (filters.count("district") && filters.at("district") == district ? " selected" : "")
             << ">" << district << "</option>";
    }

    html << R"(</select>
                    </div>

                    <!-- Тип дома -->
                    <div class="filter-group">
                        <label>Тип дома:</label>
                        <select name="house_type">
                            <option value="">Любой</option>
                            <option value="кирпичный" )" << (filters.count("house_type") && filters.at("house_type") == "кирпичный" ? "selected" : "") << R"(>Кирпичный</option>
                            <option value="панельный" )" << (filters.count("house_type") && filters.at("house_type") == "панельный" ? "selected" : "") << R"(>Панельный</option>
                            <option value="монолитный", )" << (filters.count("house_type") && filters.at("house_type") == "монолитный" ? "selected" : "") << R"(>Монолитный</option>
                        </select>
                    </div>

                    <!-- Этаж -->
                    <div class="filter-group">
                            <label>Этаж:</label>
                            <input type="number" name="floor_min" value=")" << (filters.count("floor_min") ? filters.at("floor_min") : "") << R"(" placeholder="Минимальный этаж" min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"(>
                            <input type="number" name="floor_max" value=")" << (filters.count("floor_max") ? filters.at("floor_max") : "") << R"(" placeholder="Максимальный этаж" min="0" oninput="preventNegative(event)"<<R"())"<<R"(")"<<R"(>
                    </div>

                    <!-- Балкон -->
                    <div class="filter-group">
                        <label>Балкон:</label>
                        <select name="balcony">
                            <option value="">Не важно</option>
                            <option value="1" )" << (filters.count("balcony") && filters.at("balcony") == "1" ? "selected" : "") << R"(>Есть</option>
                            <option value="0" )" << (filters.count("balcony") && filters.at("balcony") == "0" ? "selected" : "") << R"(>Нет</option>
                        </select>
                    </div>
                    <input type="submit" class="button" value="Применить фильтры">
                </form>
                <div class="load-preferences">
                    <a href="/view_apartments?username=)" << username << R"(&load_preferences=true" class="button">Добавить сохранённые из профиля</a>
                </div>
                <div class="reset-filters">
                    <a href="/view_apartments?username=)" << username << R"(" class="button">Сбросить фильтры</a>
                </div>
            </div>
            <div class="map" id="map"></div>
            <div class="apartments-list">
                <h3>Квартиры</h3>
                <form action="/compare" method="POST">
                    <input type="hidden" name="username" value=")" << username << R"(">)";

    // Вывод квартир
    for (const auto& [score, apt] : evaluated) {
        html << R"(<div class="apartment-item">
    <h3>)" << apt.description << R"(</h3>
    <p>Район: )" << apt.district << R"(</p>
    <p>Цена: )" << formatPrice(apt.price) << R"(</p>
    <p>Площадь: )" << apt.area << R"( м²</p>
    <p>Комнат: )" << apt.rooms << R"(</p>
    <p>Балкон: )" << (apt.balcony ? "Есть" : "Нет") << R"(</p>
    <p>Оценка: )" << score << R"( баллов</p>
    <form action="/toggle_favorite" method="POST">
        <input type="hidden" name="username" value=")" << username << R"(">
        <input type="hidden" name="apartment_id" value=")" << apt.id << R"(">
        <button type="submit" class="favorite-button)" << (isFavorite(username, apt.id) ? " active" : "") << R"(">❤️</button>
    </form>
    <a href="/compare?username=)" << username << R"(&apartment_id=)" << apt.id << R"(" class="button">Сравнить</a>
</div>)";
    }

    html << R"(</div>
        </div>
    </body></html>)";
    return html.str();
}


std::string generateComparisonPage(const std::string& username, const std::vector<Apartment>& apartments) {
    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #99C2E0; padding: 8px; text-align: left; }
        th { background-color: #C0E0F0; }
        .back-button { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; text-decoration: none; }
        .back-button:hover { background-color: #001F3F; }
    </style>

    </head><body>
    <h2>Сравнение квартир</h2>
    <table>
        <tr>
            <th>Критерий</th>)";

    // Заголовки для квартир
    for (const auto& apt : apartments) {
        html << "<th>" << apt.description << "</th>";
    }
    html << R"(</tr>
        <tr>
            <td>Район</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << apt.district << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Цена (руб.)</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << formatPrice(apt.price) << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Площадь (м²)</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << apt.area << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Комнат</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << apt.rooms << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Балкон</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << (apt.balcony ? "Есть" : "Нет") << "</td>";
    }
    html << R"(</tr>
    </table>
    <br>
    <a href="/view_apartments?username=)" << username << R"(" class="back-button">Назад к списку</a>
    </body></html>)";
    return html.str();
}

std::string generateComparisonResultsPage(const std::string& username, const std::vector<Apartment>& apartments) {
    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #99C2E0; padding: 8px; text-align: left; }
        th { background-color: #C0E0F0; color: #002366; }
        .back-button { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; text-decoration: none; }
        .back-button:hover { background-color: #001F3F; }
    </style>

    </head><body>
    <h2>Результаты сравнения</h2>
    <table>
        <tr>
            <th>Критерий</th>)";

    // Заголовки для квартир
    for (const auto& apt : apartments) {
        html << "<th>" << apt.description << "</th>";
    }
    html << R"(</tr>
        <tr>
            <td>Район</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << apt.district << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Цена (руб.)</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << formatPrice(apt.price) << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Площадь (м²)</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << apt.area << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Комнат</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << apt.rooms << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Балкон</td>)";
    for (const auto& apt : apartments) {
        html << "<td>" << (apt.balcony ? "Есть" : "Нет") << "</td>";
    }
    html << R"(</tr>
        <tr>
            <td>Оценка</td>)";

    // Получаем веса и параметры пользователя
    auto weights = getWeights(username);
    auto userParams = getUserParameters(username);

    // Вычисляем оценку для каждой квартиры
    for (const auto& apt : apartments) {
        double score = apt.evaluate(weights, userParams);
        html << "<td>" << std::fixed << std::setprecision(2) << score << " баллов</td>";
    }

    html << R"(</tr>
    </table>
    <br>
    <a href="/compare?username=)" << username << R"(" class="back-button">Выбрать другие квартиры</a>
    <a href="/view_apartments?username=)" << username << R"(" class="back-button">Назад к списку</a>
    </body></html>)";
    return html.str();
}


std::string generateFavoritesPage(const std::string& username, const std::vector<std::pair<double, Apartment>>& favorites) {
    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        .header { background-color: #002366; color: white; padding: 10px; display: flex; justify-content: space-between; }
        .header a { color: white; text-decoration: none; margin: 0 10px; }
        .apartment-item { margin-bottom: 20px; padding: 15px; background-color: white; border: 1px solid #99C2E0; border-radius: 5px; box-shadow: 0 0 5px rgba(0, 0, 0, 0.1); }
        .apartment-item h3 { margin-top: 0; color: #002366; }
        .button { background-color: #002366; color: white; padding: 5px 10px; text-decoration: none; border-radius: 3px; }
        .button:hover { background-color: #001F3F; }
    </style>

    </head><body>
    <div class="header">
        <div><a href="/view_apartments?username=)" << username << R"(">Назад к списку</a></div>
        <div>)" << username << R"(</div>
    </div>
    <h2>Избранное</h2>)";

    if (favorites.empty()) {
        html << "<p>У вас пока нет избранных квартир.</p>";
    } else {
        for (const auto& [score, apt] : favorites) {
            html << R"(<div class="apartment-item">
                <h3>)" << apt.description << R"(</h3>
                <p>Район: )" << apt.district << R"(</p>
                <p>Цена: )" << formatPrice(apt.price) << R"(</p>
                <form action="/toggle_favorite" method="POST">
                    <input type="hidden" name="username" value=")" << username << R"(">
                    <input type="hidden" name="apartment_id" value=")" << apt.id << R"(">
                    <input type="submit" class="button" value="Удалить из избранного">
                </form>
            </div>)";
        }
    }

    html << R"(</body></html>)";
    return html.str();
}
std::vector<Apartment> getTestApartments() {
    std::vector<Apartment> apartments;

    QSqlDatabase db = openDatabase();
    if (!db.isOpen()) {
        qDebug() << "Ошибка: база данных не открыта";
        return apartments;
    }

    QSqlQuery query(db);
    query.exec("SELECT id, description, district, price, area, rooms, balcony, house_type, floor, lat, lon FROM apartments");

    while (query.next()) {
        Apartment apt;
        apt.id = query.value("id").toInt();
        apt.description = query.value("description").toString().toStdString();
        apt.district = query.value("district").toString().toStdString();
        apt.price = query.value("price").toDouble();
        apt.area = query.value("area").toDouble();
        apt.rooms = query.value("rooms").toInt();
        apt.balcony = query.value("balcony").toBool();
        apt.house_type = query.value("house_type"). toString().toStdString();
        apt.floor = query.value("floor").toInt();
        apt.lat = query.value("lat").toDouble();
        apt.lon = query.value("lon").toDouble();
        apartments.push_back(apt);
    }

    return apartments;
}

std::string generateCriteriaSelectionPage(const std::string& username) {
    // Получаем текущие сравнения из базы данных
    auto comparisons = getComparisonsFromDB(username);

    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
        <style>
            body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
            h2 { color: #002366; text-align: center; }
            form { max-width: 600px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); border: 1px solid #99C2E0; }
            label { display: block; margin-bottom: 10px; font-weight: bold; color: #002366; }
            select { width: 100%; padding: 8px; margin-top: 5px; border: 1px solid #99C2E0; border-radius: 4px; background-color: #D0E8FF; color: #002366; }
            input[type="submit"] { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; width: 100%; }
            input[type="submit"]:hover { background-color: #001F3F; }
        </style>
        </head><body>
        <h2>Выберите важность критериев</h2>
        <form action='/save_criteria' method='POST'>
            <input type='hidden' name='username' value=')" << username << R"('>)";

    // Генерация полей для пар критериев
    for (size_t i = 0; i < AHP::criteria.size(); ++i) {
        for (size_t j = i + 1; j < AHP::criteria.size(); ++j) {
            std::string key = AHP::criteria[i] + "-" + AHP::criteria[j];
            int currentValue = comparisons.count(key) ? comparisons[key] : 0; // По умолчанию 0

            html << R"(<label>Какой критерий важнее: )" << AHP::criteria[i] << " или " << AHP::criteria[j] << R"(?</label>
                <select name=")" << key << R"(">)";

            // Опции выбора
            html << "<option value='0'" << (currentValue == 0 ? " selected" : "") << ">Одинаково важны</option>";
            html << "<option value='1'" << (currentValue == 1 ? " selected" : "") << ">" << AHP::criteria[i] << " слегка важнее (3)</option>";
            html << "<option value='2'" << (currentValue == 2 ? " selected" : "") << ">" << AHP::criteria[i] << " сильно важнее (5)</option>";
            html << "<option value='3'" << (currentValue == 3 ? " selected" : "") << ">" << AHP::criteria[i] << " очень сильно важнее (7)</option>";
            html << "<option value='4'" << (currentValue == 4 ? " selected" : "") << ">" << AHP::criteria[i] << " абсолютно важнее (9)</option>";
            html << "<option value='5'" << (currentValue == 5 ? " selected" : "") << ">" << AHP::criteria[j] << " слегка важнее (1/3)</option>";
            html << "<option value='6'" << (currentValue == 6 ? " selected" : "") << ">" << AHP::criteria[j] << " сильно важнее (1/5)</option>";
            html << "<option value='7'" << (currentValue == 7 ? " selected" : "") << ">" << AHP::criteria[j] << " очень сильно важнее (1/7)</option>";
            html << "<option value='8'" << (currentValue == 8 ? " selected" : "") << ">" << AHP::criteria[j] << " абсолютно важнее (1/9)</option>";

            html << R"(</select>)";
        }
    }

    html << R"(<input type="submit" value="Сохранить"></form></body></html>)";
    return html.str();
}

std::string generateApartmentsJson(const std::vector<std::pair<double, Apartment>>& evaluated) {
    std::ostringstream json;
    json << "[";

    for (size_t i = 0; i < evaluated.size(); ++i) {
        const auto& [score, apt] = evaluated[i];
        json << "{"
             << "\"description\": \"" << apt.description << "\","
             << "\"lat\": " << apt.lat << ","
             << "\"lon\": " << apt.lon
             << "}";
        if (i < evaluated.size() - 1) {
            json << ",";
        }
    }

    json << "]";
    return json.str();
}

std::string generateProfilePage(const std::string& username, const std::vector<double>& weights, const std::map<std::string, std::string>& preferences, double CR) {
    std::ostringstream html;

    // Создаем маппинг английских названий критериев на русские
    std::map<std::string, std::string> criteriaTranslations = {
        {"price", "Цена"},
        {"area", "Площадь"},
        {"rooms", "Количество комнат"},
        {"district", "Район"},
        {"transport", "Транспортная доступность"},
        {"infrastructure", "Инфраструктура"},
        {"condition", "Состояние квартиры"},
        {"house_type", "Тип дома"},
        {"floor", "Этаж"},
        {"balcony", "Наличие балкона"}
    };

    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        .profile-columns { display: flex; gap: 20px; margin: 0 auto; max-width: 1200px; }
        .profile-container { flex: 1; background-color: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); border: 1px solid #99C2E0; }
        table { width: 100%; border-collapse: collapse; margin-top: 20px; }
        th, td { border: 1px solid #99C2E0; padding: 8px; text-align: left; }
        th { background-color: #C0E0F0; color: #002366; }
        .button { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; text-decoration: none; display: inline-block; margin-top: 10px; }
        .button:hover { background-color: #001F3F; }
        .button-container { display: flex; justify-content: space-between; margin-top: 20px; }
    </style>

    </head><body>
    <h2>Профиль пользователя: )" << username << R"(</h2>
    <div class='button-container'>
        <a href='/view_apartments?username=)" << username << R"(' class='button'>Назад</a>
        <a href='/login' class='button'>Выйти</a>
    </div>
    <div class='profile-columns'>
        <div class='profile-container'>
            <h3>Веса критериев</h3>
            <table>
                <tr><th>Критерий</th><th>Вес</th></tr>)";

    for (size_t i = 0; i < AHP::criteria.size(); ++i) {
        std::string criterion = AHP::criteria[i];
        std::string translatedCriterion = criteriaTranslations.count(criterion) ?
                                              criteriaTranslations.at(criterion) : criterion;

        html << "<tr><td>" << translatedCriterion << "</td><td>" << std::fixed << std::setprecision(3) << weights[i] << "</td></tr>";
    }

    html << R"(</table>
            <p>Отношение согласованности (CR): )" << CR << R"(</p>
            <a href='/select_criteria?username=)" << username << R"(' class='button'>Редактировать критерии</a>
        </div>

        <div class='profile-container'>
            <h3>Предпочтения</h3>
            <table>
                <tr><th>Параметр</th><th>Значение</th></tr>
                <tr><td>Бюджет</td><td>)" << (preferences.count("budget") ? preferences.at("budget") : "Не указано") << R"(</td></tr>
                <tr><td>Район</td><td>)" << (preferences.count("district") ? preferences.at("district") : "Не указано") << R"(</td></tr>
                <tr><td>Тип квартиры</td><td>)" << (preferences.count("type") ? preferences.at("type") : "Не указано") << R"(</td></tr>
                <tr><td>Минимальная площадь</td><td>)" << (preferences.count("area_min") ? preferences.at("area_min") : "Не указано") << R"(</td></tr>
                <tr><td>Максимальная площадь</td><td>)" << (preferences.count("area_max") ? preferences.at("area_max") : "Не указано") << R"(</td></tr>
                <tr><td>Количество комнат</td><td>)" << (preferences.count("rooms") ? preferences.at("rooms") : "Не указано") << R"(</td></tr>
                <tr><td>Балкон</td><td>)" << (preferences.count("balcony") ? (preferences.at("balcony") == "1" ? "Есть" : "Нет") : "Не указано") << R"(</td></tr>
                <tr><td>Минимальный этаж</td><td>)" << (preferences.count("floor_min") ? preferences.at("floor_min") : "Не указано") << R"(</td></tr>
                <tr><td>Максимальный этаж</td><td>)" << (preferences.count("floor_max") ? preferences.at("floor_max") : "Не указано") << R"(</td></tr>
                <tr><td>Тип дома</td><td>)" << (preferences.count("house_type") ? preferences.at("house_type") : "Не указано") << R"(</td></tr>
            </table>
            <a href='/edit_profile?username=)" << username << R"(' class='button'>Редактировать предпочтения</a>
        </div>
    </div>
    </body></html>)";
    return html.str();
}

std::string generateEditProfilePage(const std::string& username, const std::map<std::string, std::string>& params) {
    std::ostringstream html;
    html << R"(<html><head><meta charset='UTF-8'>
    <style>
        body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
        h2 { color: #002366; text-align: center; }
        form { max-width: 600px; margin: 0 auto; background-color: white; padding: 20px; border-radius: 5px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); border: 1px solid #99C2E0; }
        label { display: block; margin-bottom: 10px; font-weight: bold; color: #002366; }
        input[type='text'], input[type='number'], select { width: 100%; padding: 8px; margin-top: 5px; border: 1px solid #99C2E0; border-radius: 4px; background-color: #D0E8FF; color: #002366; }
        input[type='submit'] { background-color: #002366; color: white; padding: 10px 20px; border: none; border-radius: 4px; cursor: pointer; width: 100%; }
        input[type='submit']:hover { background-color: #001F3F; }
    </style>

    </head><body>
    <h2>Редактирование предпочтений</h2>
    <form action='/edit_profile' method='POST'>
        <input type='hidden' name='username' value=')" << username << R"('>
        <label>Бюджет (руб.): <input type='number' name='budget' value=')" << (params.count("budget") ? params.at("budget") : "") << R"('></label>
        <label>Район: <input type='text' name='district' value=')" << (params.count("district") ? params.at("district") : "") << R"('></label>
        <label>Тип квартиры:
            <select name='type'>
                <option value='новостройка' )" << (params.count("type") && params.at("type") == "новостройка" ? "selected" : "") << R"(>Новостройка</option>
                <option value='вторичка' )" << (params.count("type") && params.at("type") == "вторичка" ? "selected" : "") << R"(>Вторичка</option>
            </select>
        </label>
        <label>Площадь (м²):
            <input type='number' name='area_min' value=')" << (params.count("area_min") ? params.at("area_min") : "") << R"(' placeholder='Минимальная площадь'>
            <input type='number' name='area_max' value=')" << (params.count("area_max") ? params.at("area_max") : "") << R"(' placeholder='Максимальная площадь'>
        </label>
        <label>Количество комнат: <input type='number' name='rooms' value=')" << (params.count("rooms") ? params.at("rooms") : "") << R"('></label>
        <label>Балкон:
            <select name='balcony'>
                <option value='1' )" << (params.count("balcony") && params.at("balcony") == "1" ? "selected" : "") << R"(>Есть</option>
                <option value='0' )" << (params.count("balcony") && params.at("balcony") == "0" ? "selected" : "") << R"(>Нет</option>
            </select>
        </label>
        <label>Этаж:
            <input type='number' name='floor_min' value=')" << (params.count("floor_min") ? params.at("floor_min") : "") << R"(' placeholder='Минимальный этаж'>
            <input type='number' name='floor_max' value=')" << (params.count("floor_max") ? params.at("floor_max") : "") << R"(' placeholder='Максимальный этаж'>
        </label>
        <label>Тип дома:
            <select name='house_type'>
                <option value='кирпичный' )" << (params.count("house_type") && params.at("house_type") == "кирпичный" ? "selected" : "") << R"(>Кирпичный</option>
                <option value='панельный' )" << (params.count("house_type") && params.at("house_type") == "панельный" ? "selected" : "") << R"(>Панельный</option>
                <option value='монолитный' )" << (params.count("house_type") && params.at("house_type") == "монолитный" ? "selected" : "") << R"(>Монолитный</option>
            </select>
        </label>
        <input type='submit' value='Сохранить'>
    </form>
    </body></html>)";
    return html.str();
}


std::string generateWeightsPage(const std::string& username, const std::vector<double>& weights, double CR) {
    std::ostringstream html;
    html << R"(
        <html>
        <head>
            <meta charset="UTF-8">
            <title>Результаты</title>
            <style>
                body { font-family: Bahnschrift, sans-serif; background-color: #E0F7FE; margin: 0; padding: 20px; color: #002366; }
                h2 { color: #002366; text-align: center; }
                table { width: 100%; border-collapse: collapse; margin-top: 20px; }
                th, td { border: 1px solid #99C2E0; padding: 8px; text-align: left; }
                th { background-color: #C0E0F0; color: #002366; }
            </style>
        </head>
        <body>
            <h2>Веса критериев</h2>
            <table>
                <tr><th>Критерий</th><th>Вес</th></tr>
    )";

    for (size_t i = 0; i < AHP::criteria.size(); ++i) {
        html << "<tr><td>" << AHP::criteria[i] << "</td><td>" << std::fixed << std::setprecision(3) << weights[i] << "</td></tr>";
    }

    html << R"(
            </table>
            <p>Отношение согласованности (CR): )" << CR << R"(</p>
            <a href="/select_criteria?username=)" << username << R"(">Повторить сравнение</a>
        </body>
        </html>
    )";

    return html.str();
}
