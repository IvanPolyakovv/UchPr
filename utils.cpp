#include "utils.h"
#include "apartment.h"
#include "ahp.h"
#include <algorithm>
#include <numeric>
#include <sstream>

std::string urlDecode(const std::string& src) {
    std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < src.length(); i++) {
        if (int(src[i]) == int('%')) {
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i = i + 2;
        } else {
            if (src[i] == '+') ret += ' ';
            else ret += src[i];
        }
    }
    return ret;
}

std::map<std::string, std::string> parseFormData(const std::string& body) {
    std::map<std::string, std::string> result;
    std::istringstream iss(body);
    std::string pair;

    while (std::getline(iss, pair, '&')) {
        auto pos = pair.find('=');
        if (pos != std::string::npos) {
            auto key = urlDecode(pair.substr(0, pos));
            auto value = urlDecode(pair.substr(pos + 1));
            result[key] = value;
        }
    }
    return result;
}

std::vector<std::pair<double, Apartment>> evaluateApartments(
    const std::vector<Apartment>& apartments,
    const std::vector<double>& weights,
    const std::map<std::string, std::string>& userParams) {
    std::vector<std::pair<double, Apartment>> evaluated;
    if (apartments.empty() || weights.size() != AHP::criteria.size()) return evaluated;

    for (const auto& apt : apartments) {
        double score = apt.evaluate(weights, userParams);
        evaluated.emplace_back(score, apt);
    }

    auto [minIt, maxIt] = std::minmax_element(
        evaluated.begin(), evaluated.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    double minScore = minIt->first;
    double maxScore = maxIt->first;

    for (auto& [score, apt] : evaluated) {
        score = Apartment::normalizeScore(score, minScore, maxScore);
    }

    std::sort(evaluated.begin(), evaluated.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });

    return evaluated;
}
