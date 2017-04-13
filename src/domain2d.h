#pragma once

#include <functional>
#include <utility>
#include <vector>
#include <Eigen/Core>

namespace domain2d {
    using Point = Eigen::Vector2d;
    
    using Path = std::vector<Point>;
    using Paths = std::vector<Path>;

    using Face = std::vector<Point>;
    using Faces = std::vector<Face>;

    using IntegrationPoint = std::pair<Point, double>;
    using IntegrationPoints = std::vector<IntegrationPoint>;
    
    template<typename ReturnType>
    using Function = std::function<ReturnType(double, double)>;

    Faces tessellate(const Paths &paths);

    const IntegrationPoints normTrianglePoints(const int &degree);

    IntegrationPoints pointsByTriangle(const Point &a, const Point &b, const Point &c, const int &degree);

    IntegrationPoints pointsByQuad(const Point &a, const Point &b, const Point &c, const Point &d, const int &degree);

    IntegrationPoints pointsByFaces(const Faces &faces, const int &degree);

    template<typename ReturnType>
    ReturnType integrate(const Function<ReturnType> &func, const IntegrationPoints &points);

    template<typename ReturnType>
    ReturnType integrate(const Function<ReturnType> &func, const Faces &faces, const int &degree);
}