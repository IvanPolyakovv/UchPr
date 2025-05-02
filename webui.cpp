#include "webui.h"
#include "ahp.h"
#include <sstream>
#include <iomanip>

std::string generateComparisonPage(const std::string& username, size_t currentPair) {
    if (currentPair >= AHP::criteria.size() * (AHP::criteria.size() - 1) / 2) {
        return "<html><head><meta charset='UTF-8'></head><body><h2>Все сравнения завершены!</h2>"
               "<form action='/save_weights' method='POST'>"
               "<input type='submit' value='Сохранить веса критериев'>"
               "</form></body></html>";
    }

    size_t i = 0, j = 0;
    size_t count = 0;
    for (i = 0; i < AHP::criteria.size(); ++i) {
        for (j = i + 1; j < AHP::criteria.size(); ++j) {
            if (count == currentPair) {
                goto found;
            }
            ++count;
        }
    }
found:

    std::ostringstream html;
    html << "<html><head><meta charset='UTF-8'></head><body>"
         << "<h2>Сравнение критериев (" << (currentPair + 1) << " из "
         << (AHP::criteria.size() * (AHP::criteria.size() - 1) / 2) << ")</h2>"
         << "<form action='/compare_criteria' method='POST'>"
         << "<input type='hidden' name='username' value='" << username << "'>"
         << "<input type='hidden' name='current_pair' value='" << currentPair << "'>"
         << "<h3>Что для вас важнее?</h3>"
         << "<input type='radio' name='comparison' value='0' id='equal' checked>"
         << "<label for='equal'>" << AHP::criteria[i] << " и " << AHP::criteria[j] << " одинаково важны</label><br>"
         << "<input type='radio' name='comparison' value='1' id='first_more'>"
         << "<label for='first_more'>" << AHP::criteria[i] << " немного важнее, чем " << AHP::criteria[j] << "</label><br>"
         << "<input type='radio' name='comparison' value='2' id='first_moderate'>"
         << "<label for='first_moderate'>" << AHP::criteria[i] << " умеренно важнее, чем " << AHP::criteria[j] << "</label><br>"
         << "<input type='radio' name='comparison' value='3' id='first_strong'>"
         << "<label for='first_strong'>" << AHP::criteria[i] << " сильно важнее, чем " << AHP::criteria[j] << "</label><br>"
         << "<input type='radio' name='comparison' value='4' id='first_extreme'>"
         << "<label for='first_extreme'>" << AHP::criteria[i] << " крайне важнее, чем " << AHP::criteria[j] << "</label><br>"
         << "<input type='radio' name='comparison' value='5' id='second_more'>"
         << "<label for='second_more'>" << AHP::criteria[j] << " немного важнее, чем " << AHP::criteria[i] << "</label><br>"
         << "<input type='radio' name='comparison' value='6' id='second_moderate'>"
         << "<label for='second_moderate'>" << AHP::criteria[j] << " умеренно важнее, чем " << AHP::criteria[i] << "</label><br>"
         << "<input type='radio' name='comparison' value='7' id='second_strong'>"
         << "<label for='second_strong'>" << AHP::criteria[j] << " сильно важнее, чем " << AHP::criteria[i] << "</label><br>"
         << "<input type='radio' name='comparison' value='8' id='second_extreme'>"
         << "<label for='second_extreme'>" << AHP::criteria[j] << " крайне важнее, чем " << AHP::criteria[i] << "</label><br>"
         << "<input type='submit' value='Продолжить'>"
         << "</form></body></html>";

    return html.str();
}

std::string generateWeightsPage(const std::string& username,
                                const std::vector<double>& weights,
                                double consistencyRatio) {
    std::ostringstream html;
    html << "<html><head><meta charset='UTF-8'></head><body>"
         << "<h2>Результаты сравнения критериев</h2>"
         << "<p>Коэффициент согласованности: "
         << std::fixed << std::setprecision(3) << consistencyRatio << "</p>";

    if (consistencyRatio > 0.1) {
        html << "<p style='color:red;'>Внимание: низкая согласованность оценок (CR > 0.1). "
             << "Рекомендуется пересмотреть сравнения.</p>";
    }

    html << "<h3>Веса критериев:</h3><table border='1'><tr><th>Критерий</th><th>Вес</th></tr>";

    for (size_t i = 0; i < AHP::criteria.size(); ++i) {
        html << "<tr><td>" << AHP::criteria[i] << "</td><td>"
             << std::fixed << std::setprecision(3) << weights[i] << "</td></tr>";
    }

    html << "</table>"
         << "<form action='/view_apartments' method='GET'>"
         << "<input type='hidden' name='username' value='" << username << "'>"
         << "<input type='submit' value='Посмотреть подобранные квартиры'>"
         << "</form>"
         << "<br><a href='/compare_criteria?username=" << username
         << "'>Пересмотреть сравнения</a>"
         << "<br><a href='/parameters?username=" << username
         << "'>Вернуться в профиль</a>"
         << "</body></html>";

    return html.str();
}

std::string generateApartmentList(const std::string& username,
                                  const std::vector<std::pair<double, Apartment>>& evaluated) {
    std::ostringstream html;
    html << "<html><head><meta charset='UTF-8'>"
         << "<style>"
         << "table { width: 100%; border-collapse: collapse; }"
         << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }"
         << "th { background-color: #f2f2f2; }"
         << ".score { font-weight: bold; color: "
         << (evaluated.empty() ? "black" : evaluated[0].first > 75 ? "green" :
                                           evaluated[0].first > 50 ? "orange" : "red") << "; }"
         << "</style></head><body>"
         << "<h2>Рекомендованные квартиры для " << username << "</h2>"
         << "<p>Найдено вариантов: " << evaluated.size() << "</p>"
         << "<table><tr>"
         << "<th>Рейтинг</th><th>Фото</th><th>Описание</th><th>Район</th>"
         << "<th>Цена</th><th>Площадь</th><th>Комнат</th><th>Балкон</th>"
         << "</tr>";

    for (const auto& [score, apt] : evaluated) {
        html << "<tr>"
             << "<td class='score'>" << std::fixed << std::setprecision(1) << score << "%</td>"
             << "<td><img src='" << (apt.photo_url.empty() ? "default.jpg" : apt.photo_url)
             << "' width='100' height='75'></td>"
             << "<td>" << apt.description << "</td>"
             << "<td>" << apt.district << "</td>"
             << "<td>" << std::fixed << std::setprecision(0) << apt.price << " руб.</td>"
             << "<td>" << std::fixed << std::setprecision(1) << apt.area << " м²</td>"
             << "<td>" << apt.rooms << "</td>"
             << "<td>" << (apt.balcony ? "Есть" : "Нет") << "</td>"
             << "</tr>";
    }

    html << "</table>"
         << "<br><a href='/parameters?username=" << username << "'>Вернуться в профиль</a>"
         << "</body></html>";

    return html.str();
}
