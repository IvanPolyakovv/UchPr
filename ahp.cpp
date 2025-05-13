#include "ahp.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <iostream>

const std::vector<std::string> AHP::criteria = {
    "price", "area", "rooms", "district", "transport",
    "infrastructure", "condition", "house_type", "floor", "balcony"
};

//Шкала Саати
const std::vector<std::vector<double>> AHP::saatiScale = {
    {1, 1},       // 0: Одинаково важны
    {3, 1.0/3},   // 1: Слегка важнее (первый важнее второго в 3 раза)
    {5, 1.0/5},   // 2: Сильно важнее (первый важнее второго в 5 раз)
    {7, 1.0/7},   // 3: Очень сильно важнее (первый важнее второго в 7 раз)
    {9, 1.0/9},   // 4: Абсолютно важнее (первый важнее второго в 9 раз)
    {1.0/3, 3},   // 5: Слегка менее важны (второй важнее первого в 3 раза)
    {1.0/5, 5},   // 6: Сильно менее важны (второй важнее первого в 5 раз)
    {1.0/7, 7},   // 7: Очень сильно менее важны (второй важнее первого в 7 раз)
    {1.0/9, 9}    // 8: Абсолютно менее важны (второй важнее первого в 9 раз)
};

//Создание матрицы попарных сравнений
std::vector<std::vector<double>> AHP::createComparisonMatrix(const std::map<std::string, int>& comparisons) {
    size_t n = criteria.size();
    std::vector<std::vector<double>> matrix(n, std::vector<double>(n, 1.0));

    for (const auto& [pair, value] : comparisons) {
        size_t dash_pos = pair.find('-');
        std::string crit1 = pair.substr(0, dash_pos);
        std::string crit2 = pair.substr(dash_pos + 1);

        auto it1 = std::find(criteria.begin(), criteria.end(), crit1);
        auto it2 = std::find(criteria.begin(), criteria.end(), crit2);

        if (it1 != criteria.end() && it2 != criteria.end()) {
            size_t i = std::distance(criteria.begin(), it1);
            size_t j = std::distance(criteria.begin(), it2);

            if (value >= 0 && value < saatiScale.size()) {
                matrix[i][j] = saatiScale[value][0];
                matrix[j][i] = saatiScale[value][1];
            } else {
                std::cerr << "Ошибка: недопустимое значение value = " << value << " для пары " << pair << "\n";
                matrix[i][j] = 1.0;
                matrix[j][i] = 1.0;
            }
        } else {
            std::cerr << "Ошибка: не найдены критерии для пары " << pair << "\n";
        }
    }

    return matrix;
}

//Вычисление весов критериев
std::vector<double> AHP::calculateWeights(const std::vector<std::vector<double>>& matrix) {
    size_t n = matrix.size();
    std::vector<double> weights(n, 0.0);

    // Шаг 1: Вычисляем сумму каждого столбца
    std::vector<double> columnSums(n, 0.0);
    for (size_t j = 0; j < n; ++j) {
        for (size_t i = 0; i < n; ++i) {
            columnSums[j] += matrix[i][j];
        }
    }

    // Шаг 2: Нормируем матрицу (делим каждый элемент на сумму его столбца)
    std::vector<std::vector<double>> normalizedMatrix(n, std::vector<double>(n, 0.0));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            normalizedMatrix[i][j] = matrix[i][j] / columnSums[j];
        }
    }

    // Шаг 3: Вычисляем среднее значение для каждой строки (вес критерия)
    for (size_t i = 0; i < n; ++i) {
        double rowSum = 0.0;
        for (size_t j = 0; j < n; ++j) {
            rowSum += normalizedMatrix[i][j];
        }
        weights[i] = rowSum / n;
    }

    return weights;
}

//Вычисление отношения согласованности
double AHP::calculateConsistencyRatio(const std::vector<std::vector<double>>& matrix, const std::vector<double>& weights) {
    size_t n = matrix.size();
    if (n < 2) return 0.0;

    // Вычисление главного собственного значения
    double lambda_max = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double row_sum = 0.0;
        for (size_t j = 0; j < n; ++j) {
            row_sum += matrix[i][j] * weights[j];
        }
        lambda_max += row_sum / weights[i];
    }
    lambda_max /= n;

    // Индекс согласованности (CI)
    double CI = (lambda_max - n) / (n - 1);

    // Случайный индекс (RI) для n критериев
    static const std::map<size_t, double> RI = {
        {1, 0.0}, {2, 0.0}, {3, 0.58}, {4, 0.9}, {5, 1.12},
        {6, 1.24}, {7, 1.32}, {8, 1.41}, {9, 1.45}, {10, 1.49}
    };

    // Отношение согласованности (CR)
    return CI / RI.at(n);
}
