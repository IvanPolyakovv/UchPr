#include "ahp.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>

const std::vector<std::string> AHP::criteria = {
    "price", "area", "rooms", "district", "transport",
    "infrastructure", "condition", "house_type", "floor", "balcony"
};

const std::vector<std::vector<double>> AHP::saatiScale = {
    {1, 1}, {2, 0.5}, {3, 1.0/3}, {4, 0.25}, {5, 0.2},
    {0.5, 2}, {1.0/3, 3}, {0.25, 4}, {0.2, 5}
};

std::vector<std::vector<double>> AHP::createComparisonMatrix(const std::map<std::string, int>& comparisons) {
    std::vector<std::vector<double>> matrix(criteria.size(), std::vector<double>(criteria.size(), 1.0));

    for (const auto& [pair, value] : comparisons) {
        size_t dash_pos = pair.find('-');
        std::string crit1 = pair.substr(0, dash_pos);
        std::string crit2 = pair.substr(dash_pos + 1);

        auto it1 = std::find(criteria.begin(), criteria.end(), crit1);
        auto it2 = std::find(criteria.begin(), criteria.end(), crit2);

        if (it1 != criteria.end() && it2 != criteria.end()) {
            size_t i = std::distance(criteria.begin(), it1);
            size_t j = std::distance(criteria.begin(), it2);
            matrix[i][j] = saatiScale[value][0];
            matrix[j][i] = saatiScale[value][1];
        }
    }

    return matrix;
}

std::vector<double> AHP::calculateWeights(const std::vector<std::vector<double>>& matrix) {
    std::vector<double> weights(matrix.size(), 1.0);

    for (size_t i = 0; i < matrix.size(); ++i) {
        double product = 1.0;
        for (size_t j = 0; j < matrix[i].size(); ++j) {
            product *= matrix[i][j];
        }
        weights[i] = std::pow(product, 1.0 / matrix.size());
    }

    double sum = std::accumulate(weights.begin(), weights.end(), 0.0);
    for (double& w : weights) {
        w /= sum;
    }

    return weights;
}

double AHP::calculateConsistencyRatio(const std::vector<std::vector<double>>& matrix,
                                      const std::vector<double>& weights) {
    size_t n = matrix.size();
    if (n < 2) return 0.0;

    double lambda_max = 0.0;
    for (size_t i = 0; i < n; ++i) {
        double row_sum = 0.0;
        for (size_t j = 0; j < n; ++j) {
            row_sum += matrix[i][j] * weights[j];
        }
        lambda_max += row_sum / weights[i];
    }
    lambda_max /= n;

    double CI = (lambda_max - n) / (n - 1);

    static const std::map<size_t, double> RI = {
        {1, 0.0}, {2, 0.0}, {3, 0.58}, {4, 0.9}, {5, 1.12},
        {6, 1.24}, {7, 1.32}, {8, 1.41}, {9, 1.45}, {10, 1.49}
    };

    return CI / RI.at(n);
}
