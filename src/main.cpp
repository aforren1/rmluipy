#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <RmlUi/Core.h>

namespace py = pybind11;

class Animal {
    public:
        virtual std::string go(int n_times) = 0;
};

class PyAnimal : public Animal {
    public:
        using Animal::Animal;

        std::string go(int n_times) override {
            PYBIND11_OVERRIDE_PURE(
                std::string,
                Animal,
                go,
                n_times
            );
        }
};

std::string call_go(Animal *animal) {
    return animal->go(5);
}

class PySystemInterface : public Rml::SystemInterface {
    // https://github.com/mikke89/RmlUi/blob/master/Include/RmlUi/Core/SystemInterface.h
    public:
        using Rml::SystemInterface::SystemInterface;

        double GetElapsedTime() override {
            PYBIND11_OVERRIDE_PURE(
                double,
                SystemInterface,
                GetElapsedTime
            );
        }
};

class PyRenderInterface : public Rml::RenderInterface {
    // https://github.com/mikke89/RmlUi/blob/master/Include/RmlUi/Core/RenderInterface.h
    public:
        using Rml::RenderInterface::RenderInterface;
        // RenderGeometry (python-side) should take structured array or something opaque as input?
        // It looks like the fixed-function examples rely on indexing into the layout...
        // But with moderngl we could probably just take the right number of bytes (b/c layout won't change)
        // and then we don't need a runtime numpy dependency?
};



PYBIND11_MODULE(rmlui, m) {
    py::class_<Animal, PyAnimal>(m, "Animal")
        .def(py::init<>())
        .def("go", &Animal::go);

    m.def("call_go", &call_go);

    py::class_<Rml::SystemInterface, PySystemInterface>(m, "SystemInterface")
        .def(py::init<>())
        .def("elapsed_time", &Rml::SystemInterface::GetElapsedTime);
    
    // TODO: do we actually need to expose these?
    PYBIND11_NUMPY_DTYPE(Rml::Vector2f, x, y);
    PYBIND11_NUMPY_DTYPE(Rml::Colourb, red, green, blue, alpha);
    PYBIND11_NUMPY_DTYPE(Rml::Vertex, position, colour, tex_coord);
}
