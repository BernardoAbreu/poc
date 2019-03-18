#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <boost/python/tuple.hpp>
#include <stdexcept>
#include <vector>
#include <list>

#include "process.h"


template<typename T>
using Matrix = std::vector<std::vector<T> >;


void obtain_patterns(const Matrix<mol_info> &points, std::list<Pattern> &out, int k){
    Graph g;

    build_graph(g, points, k);

    level1(g, out);

    post_process(out);
}


void build_matrix_from_python(PyObject* incoming, Matrix<mol_info> &data){
    if (PyList_Check(incoming)) {
        data.resize((int)PyList_Size(incoming));

        for(Py_ssize_t i = 0; i < PyList_Size(incoming); i++) {
            PyObject *row = PyList_GetItem(incoming, i);

            for(Py_ssize_t j = 0; j < PyList_Size(row); j++) {
                PyObject *value = PyList_GetItem(row, j);
                std::pair<int, double> p = std::pair<int, double>(PyFloat_AsDouble(PyTuple_GetItem(value, 0)), PyFloat_AsDouble(PyTuple_GetItem(value, 1)));
                data[i].push_back(p);
            }
        }
    } else {
        throw std::logic_error("Passed PyObject pointer was not a list!");
    }
}


void build_out_matrix(const std::list<Pattern> &out, boost::python::list &mols, boost::python::list &all_points){
    for(auto &pat : out){
        boost::python::list new_mols, new_points;

        for(auto &mol : pat.molecules){
            new_mols.append(mol);
        }
        mols.append(new_mols);

        for(auto &points : pat.points){
            new_points.append(points);
        }
        all_points.append(new_points);
    }
}


boost::python::tuple mine_patterns(PyObject* incoming, int k){
    Matrix<mol_info> points;
    std::list<Pattern> out;

    build_matrix_from_python(incoming, points);

    obtain_patterns(points, out, k);

    boost::python::list mols, all_points;

    build_out_matrix(out, mols, all_points);
    return boost::python::make_tuple(mols, all_points);
}


boost::python::tuple mine_patterns_default(PyObject* incoming){
    return mine_patterns(incoming, 1);
}


BOOST_PYTHON_MODULE(patmin)
{
    using namespace boost::python;
    def("mine_patterns", mine_patterns);
    def("mine_patterns", mine_patterns_default);
}

