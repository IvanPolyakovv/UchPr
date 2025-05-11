#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QThread>
#include <QSqlRecord>
#include <map>
#include <string>
#include <vector>
#include "apartment.h"
#include "ahp.h"

QSqlDatabase openDatabase();
bool checkCredentials(const std::string& username, const std::string& password);
bool registerUser(const std::string& username, const std::string& password, const std::map<std::string, std::string>& preferences);
bool saveUserParameters(const std::string& username, const std::map<std::string, std::string>& params);
std::map<std::string, std::string> getUserParameters(const std::string& username);
std::vector<Apartment> getApartmentsFromDB();
std::map<std::string, int> getComparisonsFromDB(const std::string& username);
bool saveWeights(const std::string& username, const std::vector<double>& weights);
std::vector<double> getWeights(const std::string& username);
bool addToFavorites(const std::string& username, int apartmentId);
bool removeFromFavorites(const std::string& username, int apartmentId);
bool isFavorite(const std::string& username, int apartmentId);
std::vector<Apartment> getFavorites(const std::string& username);

#endif // DATABASE_H
