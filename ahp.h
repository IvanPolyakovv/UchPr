#ifndef AHP_H
#define AHP_H

#include <vector>
#include <string>
#include <map>

class AHP {
public:
    static const std::vector<std::string> criteria;
    static const std::vector<std::vector<double>> saatiScale;

    static std::vector<std::vector<double>> createComparisonMatrix(const std::map<std::string, int>& comparisons);
    static std::vector<double> calculateWeights(const std::vector<std::vector<double>>& matrix);
    static double calculateConsistencyRatio(const std::vector<std::vector<double>>& matrix, const std::vector<double>& weights);
};

#endif // AHP_H
