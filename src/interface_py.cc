#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "domain1d.h"
#include "domain2d.h"

namespace py = pybind11;

PYBIND11_PLUGIN(NIntegrate) {
    using Vector = Eigen::VectorXd;
    using Matrix = Eigen::MatrixXd;

    py::module m("NIntegrate", "NIntegrate");

    m.def("tessellate",
        &domain2d::tessellate,
        py::arg("polygons")
    );
    
    m.def("integration_points",
        &domain2d::pointsByFaces,
        py::arg("faces"),
        py::arg("degree")
    );

    m.def("integrate",
        py::overload_cast<const domain2d::Function<double> &, const domain2d::IntegrationPoints &>(&domain2d::integrate<double>),
        py::arg("function"),
        py::arg("points")
    );

    m.def("integrate",
        py::overload_cast<const domain2d::Function<double> &, const domain2d::Faces &, const int &>(&domain2d::integrate<double>),
        py::arg("function"),
        py::arg("faces"),
        py::arg("degree")
    );

    m.def("integrate_vector",
        py::overload_cast<const domain2d::Function<Vector> &, const domain2d::IntegrationPoints &>(&domain2d::integrate<Vector>),
        py::arg("function"),
        py::arg("points")
    );

    m.def("integrate_vector",
        py::overload_cast<const domain2d::Function<Vector> &, const domain2d::Faces &, const int &>(&domain2d::integrate<Vector>),
        py::arg("function"),
        py::arg("faces"),
        py::arg("degree")
    );

    m.def("integrate_matrix",
        py::overload_cast<const domain2d::Function<Matrix> &, const domain2d::IntegrationPoints &>(&domain2d::integrate<Matrix>),
        py::arg("function"),
        py::arg("points")
    );

    m.def("integrate_matrix",
        py::overload_cast<const domain2d::Function<Matrix> &, const domain2d::Faces &, const int &>(&domain2d::integrate<Matrix>),
        py::arg("function"),
        py::arg("faces"),
        py::arg("degree")
    );

    return m.ptr();
}