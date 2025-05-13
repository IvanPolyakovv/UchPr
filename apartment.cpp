#include "apartment.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>

//Расчёт оценки квартиры
double Apartment::evaluate(const std::vector<double>& weight, const std::map<std::string, std::string>& userParams) const {
    std::vector<double> score(weight.size(), 0.0);

    // Бюджет
    double userBudget = 10000000; // Значение по умолчанию
    if (userParams.count("budget")) {
        try {
            userBudget = std::stod(userParams.at("budget"));
        } catch (...) {}
    }
    // Оценка от 0 (цена = бюджет) до 1 (цена = 0)
    score[0] = std::max(0.0, 1.0 - (price / userBudget));

    // Площадь
    double area_min = 0; // Значение по умолчанию
    double area_max = 200; // Значение по умолчанию
    if (userParams.count("area_min")) {
        try {
            area_min = std::stod(userParams.at("area_min"));
        } catch (const std::invalid_argument&) {
            // Обработка ошибки: используем значение по умлчанию
        }
    }
    if (userParams.count("area_max")) {
        try {
            area_max = std::stod(userParams.at("area_max"));
        } catch (const std::invalid_argument&) {
            // Обработка ошибки: используем значение по умлчанию
        }
    }
    if (area >= area_min && area <= area_max) {
        score[1] = 1.0;
    } else {
        double dist = std::min(std::abs(area - area_min), std::abs(area - area_max));
        score[1] = 1.0 / (1.0 + dist / 10.0);
    }

    // Количество комнат
    int preferredRooms = rooms; // Значение по умлчанию
    if (userParams.count("room")) {
        try {
            preferredRooms = std::stoi(userParams.at("room"));
        } catch (const std::invalid_argument&) {
            // Обработка ошибки: используем значение по умлчанию
        }
    }
    score[2] = rooms == preferredRooms ? 1.0 : (rooms == preferredRooms + 1 || rooms == preferredRooms - 1) ? 0.5 : 0.0;

    // Район
    std::string preferredDistrict = userParams.count("district") ? userParams.at("district") : "";
    if (district == preferredDistrict){
        score[3] = 1.0;
    } else if (preferredDistrict == "Адмиралтейский"){
        if (district == "Центральный" || district == "Фрунзенский" || district == "Кировский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Выборгский"){
        if (district == "Калининский" || district == "Приморский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Калининский"){
        if (district == "Выборгский" || district == "Красногвардейский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Кировский"){
        if (district == "Московский" || district == "Красносельский" || district == "Адмиралтейский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Красногвардейский"){
        if (district == "Невский" || district == "Калининский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Красносельский"){
        if (district == "Кировский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Московский"){
        if (district == "Фрунзенский" || district == "Кировский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Невский"){
        if (district == "Фрунзенский" || district == "Красногвардейский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Приморский"){
        if (district == "Выборгский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Колпинский"){
        if (district == "Пушкинский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Пушкинский"){
        if (district == "Колпинский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Петродворцовый"){
        if (district == "Красносельский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Фрунзенский"){
        if (district == "Московский" || district == "Кировский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else if (preferredDistrict == "Центральный"){
        if (district == "Адмиралтейский") {
            score[3] = 0.5;
        }
        else score[3] = 0.0;
    }
    else score[3] = 0.0;

    // Транспортная доступность
    score[4] = transport / 10.0;

    // Инфраструктура
    score[5] = infrastructure / 10.0;

    // Состояние квартиры
    score[6] = (condition == "новый") ? 1.0 : (condition == "хороший") ? 0.8 : 0.5;

    // Тип дома
    std::string preferredHouseType = userParams.count("house_type") ? userParams.at("house_type") : "";
    score[7] = (house_type == preferredHouseType) ? 1.0 : 0.2;

    // Этаж
    int floor_min = 1; // Значение по умлчанию
    int floor_max = 20; // Значение по умлчанию
    if (userParams.count("floor_min")) {
        try {
            floor_min = std::stoi(userParams.at("floor_min"));
        } catch (const std::invalid_argument&) {
            // Обработка ошибки: используем значение по умлчанию
        }
    }
    if (userParams.count("floor_max")) {
        try {
            floor_max = std::stoi(userParams.at("floor_max"));
        } catch (const std::invalid_argument&) {
            // Обработка ошибки: используем значение по умлчанию
        }
    }
    if (floor >= floor_min && floor <= floor_max) {
        score[8] = 1.0;
    } else {
        double dist = std::min(std::abs(floor - floor_min), std::abs(floor - floor_max));
        score[8] = 1.0 / (1.0 + dist / 5.0);
    }

    // Балкон
    std::string preferredBalcony = userParams.count("balcony") ? userParams.at("balcony") : "есть";
    score[9] = (balcony == (preferredBalcony == "есть")) ? 1.0 : (balcony == (preferredBalcony == "нет")) ? 0.8 : 0.3;

    // Итоговый балл
    double total = 0.0;
    for (size_t i = 0; i < weight.size(); ++i) {
        total += weight[i] * score[i];
    }

    return total * 100;
}
//Нормализация оценки
double Apartment::normalizeScore(double rawScore, double minScore, double maxScore) {
    if (maxScore == minScore) return 50.0; // Возвращаем исходную оценку, если все оценки равны
    return rawScore; // Приводим к шкале относительно максимального балла
}
