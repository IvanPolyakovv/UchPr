#ifndef WEBUI_H
#define WEBUI_H

#include <string>
#include <vector>
#include "apartment.h"

std::string generateComparisonPage(const std::string& username, size_t currentPair = 0);
std::string generateWeightsPage(const std::string& username,
                                const std::vector<double>& weights,
                                double consistencyRatio);
std::string generateApartmentList(const std::string& username,
                                  const std::vector<std::pair<double, Apartment>>& evaluated);

#endif // WEBUI_H
