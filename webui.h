#ifndef WEBUI_H
#define WEBUI_H

#include <string>
#include <map>
#include <vector>
#include "apartment.h"
#include "ahp.h"
std::string formatPrice(double price);
std::vector<Apartment> getTestApartments();
std::string generateLoginPage();
std::string generateRegisterPage();
std::string generatePreferencesPage(const std::string& username);
std::string generateApartmentList(const std::string& username,
                                  const std::vector<std::pair<double, Apartment>>& evaluated,
                                  const std::map<std::string, std::string>& filters);
std::string generateProfilePage(const std::string& username, const std::vector<double>& weights, const std::map<std::string, std::string>& preferences, double CR);
std::string generateApartmentsJson(const std::vector<std::pair<double, Apartment>>& evaluated);
std::string generateCriteriaSelectionPage(const std::string& username);
std::string generateWeightsPage(const std::string& username, const std::vector<double>& weights, double CR);
std::string generateEditProfilePage(const std::string& username, const std::map<std::string, std::string>& params);
std::string generateFavoritesPage(const std::string& username, const std::vector<std::pair<double, Apartment>>& favorites);
std::string generateComparisonPage(const std::string& username, const std::vector<Apartment>& apartments);
std::string generateComparisonResultsPage(const std::string& username, const std::vector<Apartment>& apartments);
#endif // WEBUI_H
