#include "utils.h"
#include "apartment.h"
#include "ahp.h"
#include "qdebug.h"
#include "qlogging.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>


std::string urlDecode(const std::string& src) {
    std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < src.length(); i++) {
        if (int(src[i]) == int('%')) {
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        } else {
            if (src[i] == '+') ret += ' ';
            else ret += src[i];
        }
    }
    return ret;
}

std::map<std::string, std::string> parseFormData(const std::string& body) {
    std::map<std::string, std::string> result;
    std::istringstream iss(body);
    std::string pair;

    while (std::getline(iss, pair, '&')) {
        auto pos = pair.find('=');
        if (pos != std::string::npos) {
            auto key = urlDecode(pair.substr(0, pos));
            auto value = urlDecode(pair.substr(pos + 1));
            result[key] = value;
        }
    }
    return result;
}

std::vector<std::pair<double, Apartment>> evaluateApartments(
    const std::vector<Apartment>& apartments,
    const std::vector<double>& weights,
    const std::map<std::string, std::string>& userParams) {
    std::vector<std::pair<double, Apartment>> evaluated;
    if (apartments.empty() || weights.size() != AHP::criteria.size()) return evaluated;

    // Оценка квартир
    for (const auto& apt : apartments) {
        double score = apt.evaluate(weights, userParams);
        if (std::isnan(score) || std::isinf(score)) {
            qDebug() << "Ошибка: некорректный балл для квартиры ID: " << apt.id;
            continue; // Пропустить некорректные записи
        }
        evaluated.emplace_back(score, apt);
    }

    // Проверка на пустой список после вычисления баллов
    if (evaluated.empty()) return evaluated;

    // Находим минимальное и максимальное значение "сырых" баллов
    auto [minIt, maxIt] = std::minmax_element(
        evaluated.begin(), evaluated.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    if (minIt == evaluated.end() || maxIt == evaluated.end()) {
        qDebug() << "Ошибка: не удалось найти минимальный или максимальный балл.";
        return evaluated;
    }

    double minScore = minIt->first;
    double maxScore = maxIt->first;

    // Если все оценки одинаковы, возвращаем исходные баллы
    if (minScore == maxScore) {
        qDebug() << "Предупреждение: все квартиры имеют одинаковую оценку. Нормализация невозможна.";
        for (auto& [score, apt] : evaluated) {
            score = 50.0; // По умолчанию 50, если все квартиры равны
        }
        return evaluated;
    }

    // Нормализация баллов
    for (auto& [score, apt] : evaluated) {
        score = (score - minScore) / (maxScore - minScore) * 100.0; // Приводим к диапазону 0–100
    }

    // Сортировка
    std::sort(evaluated.begin(), evaluated.end(),
              [](const auto& a, const auto& b) {
                  if (a.first != b.first) {
                      return a.first > b.first; // По убыванию оценки
                  } else {
                      return a.second.price < b.second.price; // По возрастанию цены
                  }
              });

    return evaluated;
}
