#pragma once

#include <utility>
#include <vector>

namespace domain1d {
    using IntegrationPoint = std::pair<double, double>;
    using IntegrationPoints = std::vector<IntegrationPoint>;

    const IntegrationPoints normPoints(const int &degree);
}