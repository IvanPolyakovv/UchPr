#include "apartment.h"
#include <algorithm>
#include <cmath>

double Apartment::evaluate(const std::vector<double>& weights,
                           const std::map<std::string, std::string>& userParams) const {
    std::vector<double> scores(weights.size(), 0.0);

    double userBudget = userParams.count("budget") ? std::stod(userParams.at("budget")) : 10000000;
    scores[0] = (userBudget - std::max(0.0, price - userBudget)) / userBudget;

    double area_min = userParams.count("area_min") ? std::stod(userParams.at("area_min")) : 0;
    double area_max = userParams.count("area_max") ? std::stod(userParams.at("area_max")) : 200;
    if (area >= area_min && area <= area_max) {
        scores[1] = 1.0;
    } else {
        double dist = std::min(std::abs(area - area_min), std::abs(area - area_max));
        scores[1] = 1.0 / (1.0 + dist/10.0);
    }

    int preferredRooms = userParams.count("rooms") ? std::stoi(userParams.at("rooms")) : rooms;
    scores[2] = rooms == preferredRooms ? 1.0 : 0.5;

    std::string preferredDistrict = userParams.count("district") ? userParams.at("district") : "";
    scores[3] = (district == preferredDistrict) ? 1.0 : 0.2;

    scores[4] = transport / 10.0;
    scores[5] = infrastructure / 10.0;

    scores[6] = (condition == "новый") ? 1.0 : (condition == "хороший") ? 0.8 : 0.5;

    std::string preferredHouseType = userParams.count("house_type") ? userParams.at("house_type") : "";
    scores[7] = (house_type == preferredHouseType) ? 1.0 : 0.5;

    int floor_min = userParams.count("floor_min") ? std::stoi(userParams.at("floor_min")) : 1;
    int floor_max = userParams.count("floor_max") ? std::stoi(userParams.at("floor_max")) : 20;
    if (floor >= floor_min && floor <= floor_max) {
        scores[8] = 1.0;
    } else {
        double dist = std::min(std::abs(floor - floor_min), std::abs(floor - floor_max));
        scores[8] = 1.0 / (1.0 + dist/5.0);
    }

    std::string preferredBalcony = userParams.count("balcony") ? userParams.at("balcony") : "есть";
    scores[9] = (balcony == (preferredBalcony == "есть")) ? 1.0 : 0.0;

    double total = 0.0;
    for (size_t i = 0; i < weights.size(); ++i) {
        total += weights[i] * scores[i];
    }

    return total * 100;
}

double Apartment::normalizeScore(double rawScore, double minScore, double maxScore) {
    if (maxScore == minScore) return 50.0;
    return 100.0 * (rawScore - minScore) / (maxScore - minScore);
}
