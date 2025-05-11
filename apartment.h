#ifndef APARTMENT_H
#define APARTMENT_H

#include <string>
#include <map>
#include <vector>

class Apartment {
public:
    std::string description;
    std::string district;
    int id = 0;  // Добавлено
    double price;
    double area;
    int rooms;
    bool balcony;
    std::string house_type;
    int floor;
    double lat; // Широта
    double lon; // Долгота
    double transport; // Транспортная доступность
    double infrastructure; // Инфраструктура
    std::string condition; // Состояние квартиры

    double evaluate(const std::vector<double>& weights, const std::map<std::string, std::string>& userParams) const;
    static double normalizeScore(double rawScore, double minScore, double maxScore);
};

#endif // APARTMENT_H
