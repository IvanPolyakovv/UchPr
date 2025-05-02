#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>
#include <string>
#include <map>
#include <vector>
#include "apartment.h"

QSqlDatabase openDatabase();
bool checkCredentials(const std::string& username, const std::string& password);
bool registerUser(const std::string& username, const std::string& password);
bool saveUserParameters(const std::string& username, const std::map<std::string, std::string>& params);
std::map<std::string, std::string> getUserParameters(const std::string& username);
std::vector<Apartment> getApartmentsFromDB();
std::map<std::string, int> getComparisonsFromDB(const std::string& username);
bool saveWeights(const std::string& username, const std::vector<double>& weights);
std::vector<double> getWeights(const std::string& username);

#endif // DATABASE_H
