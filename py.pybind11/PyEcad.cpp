#include "PyEcadBasic.hpp"
#include "PyEcadDesign.hpp"
void ecad_init_datamgr(py::module_ & m)
{
    //DataMgr
    py::class_<EDataMgr, std::unique_ptr<EDataMgr, py::nodelete>>(m, "EDataMgr")
        .def(py::init([]{ return std::unique_ptr<EDataMgr, py::nodelete>(&EDataMgr::Instance()); }))
        .def("init", [](ELogLevel level, const std::string & workDir)
            { EDataMgr::Instance().Init(level, workDir); })
        .def("init", [](ELogLevel level)
            { EDataMgr::Instance().Init(level); })
        .def("init", []
            { EDataMgr::Instance().Init(); })
        .def("shut_down", [](bool autoSave)
            { EDataMgr::Instance().ShutDown(autoSave); })
        .def("shut_down", []
            { EDataMgr::Instance().ShutDown(); })
#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
        .def("save_database", [](CPtr<IDatabase> database, const std::string & archive, EArchiveFormat fmt)
            { EDataMgr::Instance().SaveDatabase(database, archive, fmt); })
        .def("save_database", [](CPtr<IDatabase> database, const std::string & archive)
            { EDataMgr::Instance().SaveDatabase(database, archive); })
        .def("load_database", [](const std::string & archive, EArchiveFormat fmt)
            { return EDataMgr::Instance().LoadDatabase(archive, fmt); }, py::return_value_policy::reference)
        .def("load_database", [](const std::string & archive)
            { return EDataMgr::Instance().LoadDatabase(archive); }, py::return_value_policy::reference)
    ;
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT
}

PYBIND11_MODULE(PyEcad, ecad)
{
    ecad_init_basic(ecad);
    ecad_init_design(ecad);
    ecad_init_datamgr(ecad);
}