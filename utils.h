#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <map>
#include <vector>
#include "apartment.h"

std::string urlDecode(const std::string& src);
std::map<std::string, std::string> parseFormData(const std::string& body);
std::vector<std::pair<double, Apartment>> evaluateApartments(
    const std::vector<Apartment>& apartments,
    const std::vector<double>& weights,
    const std::map<std::string, std::string>& userParams);

#endif // UTILS_H
