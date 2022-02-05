#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <sstream>

namespace py = pybind11;

// from: https://github.com/pybind/pybind11/pull/1152
#define PYBIND11_NUMPY_OBJECT_DTYPE(Type)                                      \
  namespace pybind11 {                                                         \
  namespace detail {                                                           \
  template <> struct npy_format_descriptor<Type> {                             \
  public:                                                                      \
    enum { value = npy_api::NPY_OBJECT_ };                                     \
    static pybind11::dtype dtype() {                                           \
      if (auto ptr = npy_api::get().PyArray_DescrFromType_(value)) {           \
        return reinterpret_borrow<pybind11::dtype>(ptr);                       \
      }                                                                        \
      pybind11_fail("Unsupported buffer format!");                             \
    }                                                                          \
    static constexpr auto name = _("object");                                  \
  };                                                                           \
  }                                                                            \
  }

struct Point {
  Point(double x, double y) : x(x), y(y) {
    std::cout << "constructor called: " << this << "  <Point (" << this->x
              << " " << this->y << ")>" << std::endl;
  };
  Point(const Point &p) : x(p.x), y(p.y) {
    std::cout << "copy constructor called from " << &p << ", created" << this
              << "  <Point (" << this->x << " " << this->y << ")>" << std::endl;
  }

  ~Point() {
    std::cout << "destructor called: " << this << "  <Point (" << this->x << " "
              << this->y << ")>" << std::endl;
  }

  double x;
  double y;
};

// make Point valid for use in numpy
PYBIND11_NUMPY_OBJECT_DTYPE(Point);

py::array_t<Point> create(py::array_t<double> xs, py::array_t<double> ys) {
  py::buffer_info xbuf = xs.request(), ybuf = ys.request();
  if (xbuf.ndim != 1 || ybuf.ndim != 1) {
    throw std::runtime_error("Number of dimensions must be one");
  }
  if (xbuf.size != ybuf.size) {
    throw std::runtime_error("Input shapes must match");
  }

  auto result = py::array_t<Point>(xbuf.size);
  py::buffer_info rbuf = result.request();

  double *xptr = static_cast<double *>(xbuf.ptr);
  double *yptr = static_cast<double *>(ybuf.ptr);
  py::object *rptr = static_cast<py::object *>(rbuf.ptr);

  size_t size = static_cast<size_t>(xbuf.shape[0]);

  std::cout << "constructing point array " << std::endl;

  for (size_t i = 0; i < size; i++) {
    // this copies the entire object during assignment
    py::object obj = py::cast(Point(xptr[i], yptr[i]));
    rptr[i] = obj;
  }

  std::cout << "returning point array " << std::endl;

  return result;
}

PYBIND11_MODULE(pybind11_obj_test, m) {
  py::class_<Point>(m, "Point").def("__repr__", [](const Point &p) {
    std::ostringstream os;
    os << "<Point (" << p.x << " " << p.y << ")>";
    return os.str();
  });
  m.def("test", &create);
}