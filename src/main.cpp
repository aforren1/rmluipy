#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <RmlUi/Core.h>

namespace py = pybind11;

class Animal
{
public:
    virtual std::string go(int n_times) = 0;
};

class PyAnimal : public Animal
{
public:
    using Animal::Animal;

    std::string go(int n_times) override
    {
        PYBIND11_OVERRIDE_PURE(
            std::string,
            Animal,
            go,
            n_times);
    }
};

std::string call_go(Animal *animal)
{
    return animal->go(5);
}

class PySystemInterface : public Rml::SystemInterface
{
    // https://github.com/mikke89/RmlUi/blob/master/Include/RmlUi/Core/SystemInterface.h
public:
    using Rml::SystemInterface::SystemInterface;

    double GetElapsedTime() override
    {
        PYBIND11_OVERRIDE_PURE(
            double,
            SystemInterface,
            GetElapsedTime);
    }
};

class HelperRender
{
public:
    virtual void RenderGeometry2(py::memoryview vertices, py::memoryview indices, Rml::TextureHandle texture, const Rml::Vector2f &translation) = 0;
};

class PyRenderInterface : public HelperRender,
                          public Rml::RenderInterface
{
    // https://github.com/mikke89/RmlUi/blob/master/Include/RmlUi/Core/RenderInterface.h
public:
    using Rml::RenderInterface::RenderInterface;
    // RenderGeometry (python-side) should take structured array or something opaque as input?
    // It looks like the fixed-function examples rely on indexing into the layout...
    // But with moderngl we could probably just take the right number of bytes (b/c layout won't change)
    // and then we don't need a runtime numpy dependency?
    // RenderGeometry only ever gets called by C++ side, so we have to end up providing that exact interface
    // But we can make a *new* virtual fn that takes the right buffer
    void RenderGeometry(Rml::Vertex *vertices, int num_vertices, int *indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f &translation)
    {
        auto verts = py::memoryview::from_memory(
            vertices,
            sizeof(Rml::Vertex) * num_vertices);

        auto inds = py::memoryview::from_memory(
            indices,
            sizeof(int) * num_indices);

        RenderGeometry2(verts, inds, texture, translation);
    }

    void RenderGeometry2(py::memoryview vertices, py::memoryview indices, Rml::TextureHandle texture, const Rml::Vector2f &translation) override
    {
        PYBIND11_OVERRIDE_PURE_NAME(
            void,
            HelperRender,
            "render_geometry",
            RenderGeometry2,
            vertices, indices, texture, translation);
    }
    void EnableScissorRegion(bool enable) override
    {
        PYBIND11_OVERRIDE_PURE(
            void,
            RenderInterface,
            EnableScissorRegion,
            enable);
    }

    void SetScissorRegion(int x, int y, int width, int height) override
    {
        PYBIND11_OVERRIDE_PURE(
            void,
            RenderInterface,
            SetScissorRegion,
            x, y, width, height);
    }
};

PYBIND11_MODULE(rmlui, m)
{
    py::class_<Animal, PyAnimal>(m, "Animal")
        .def(py::init<>())
        .def("go", &Animal::go);

    m.def("call_go", &call_go);

    py::class_<Rml::SystemInterface, PySystemInterface>(m, "SystemInterface")
        .def(py::init<>())
        .def("elapsed_time", &Rml::SystemInterface::GetElapsedTime);

    // TODO: do we actually need to expose these?
    py::class_<Rml::Vector2f>(m, "Vector2f").def(py::init<float, float>());
    // PYBIND11_NUMPY_DTYPE(Rml::Vector2f, x, y);
    // PYBIND11_NUMPY_DTYPE(Rml::Colourb, red, green, blue, alpha);
    // PYBIND11_NUMPY_DTYPE(Rml::Vertex, position, colour, tex_coord);

    // py::class_<HelperRender, PyRenderInterface>(m, "RenderInterface")
    //     .def(py::init<>())
    //     .def("render_geometry", &HelperRender::RenderGeometry2);
}
