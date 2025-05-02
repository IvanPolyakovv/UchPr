#ifndef APARTMENT_H
#define APARTMENT_H

#include <string>
#include <vector>
#include <map>

struct Apartment {
    int id;
    double price;
    double area;
    int rooms;
    std::string district;
    double transport;
    double infrastructure;
    std::string condition;
    std::string house_type;
    int floor;
    bool balcony;
    std::string description;
    std::string photo_url;

    double evaluate(const std::vector<double>& weights,
                    const std::map<std::string, std::string>& userParams) const;
    static double normalizeScore(double rawScore, double minScore, double maxScore);
};

#endif // APARTMENT_H
