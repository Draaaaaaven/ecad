#include "PyEcadBasic.hpp"
#include "PyEcadDesign.hpp"
#include "PyEcadUtility.hpp"
void ecad_init_datamgr(py::module_ & m)
{
    //DataMgr
    py::class_<EDataMgr, std::unique_ptr<EDataMgr, py::nodelete>>(m, "DataMgr")
        .def(py::init([]{ return std::unique_ptr<EDataMgr, py::nodelete>(&EDataMgr::Instance()); }))
        .def("init", [](ELogLevel level, const std::string & workDir)
            { EDataMgr::Instance().Init(level, workDir); })
        .def("init", [](ELogLevel level)
            { EDataMgr::Instance().Init(level); })
        .def("init", []
            { EDataMgr::Instance().Init(); })
        .def("create_database", [](const std::string & name)
            { return EDataMgr::Instance().CreateDatabase(name); }, py::return_value_policy::reference)
        .def("open_database", [](const std::string & name)
            { return EDataMgr::Instance().OpenDatabase(name); }, py::return_value_policy::reference)
        .def("remove_database", [](const std::string & name)
            { return EDataMgr::Instance().RemoveDatabase(name); })
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
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

        // cell
        .def("create_circuit_cell", [](Ptr<IDatabase> database, const std::string & name)
            { return EDataMgr::Instance().CreateCircuitCell(database, name); }, py::return_value_policy::reference)

        // material
        .def("create_material_def", [](Ptr<IDatabase> database, const std::string & name)
            { return EDataMgr::Instance().CreateMaterialDef(database, name); }, py::return_value_policy::reference)
        .def("create_simple_material_prop", [](EFloat value)
            { return EDataMgr::Instance().CreateSimpleMaterialProp(value); }) 
        .def("create_polynomial_material_prop", [](std::vector<std::vector<EFloat>> coefficients)
            { return EDataMgr::Instance().CreatePolynomialMaterialProp(std::move(coefficients)); })
        
        // component def
        .def("create_component_def", [](Ptr<IDatabase> database, const std::string & name)
            { return EDataMgr::Instance().CreateComponentDef(database, name); }, py::return_value_policy::reference)

        .def("create_component_def_pin", [](Ptr<IComponentDef> compDef, const std::string & pinName, FPoint2D loc, EPinIOType type, CPtr<IPadstackDef> psDef, ELayerId lyr)
            { return EDataMgr::Instance().CreateComponentDefPin(compDef, pinName, loc, type, psDef, lyr); }, py::return_value_policy::reference)
        .def("create_component_def_pin", [](Ptr<IComponentDef> compDef, const std::string & pinName, FPoint2D loc, EPinIOType type)
            { return EDataMgr::Instance().CreateComponentDefPin(compDef, pinName, loc, type); }, py::return_value_policy::reference)

       // padstack
        .def("create_padstack_def", [](Ptr<IDatabase> database, const std::string & name)
            { return EDataMgr::Instance().CreatePadstackDef(database, name); }, py::return_value_policy::reference)
        .def("create_padstack_def_data", []
            { return EDataMgr::Instance().CreatePadstackDefData(); })
        
        //Shape
        .def("create_shape_rectangle", [](const ECoordUnits & coordUnits, const FPoint2D & ll, const FPoint2D & ur)
            { return EDataMgr::Instance().CreateShapeRectangle(coordUnits, ll, ur); })
        .def("create_shape_circle", [](const ECoordUnits & coordUnits, const FPoint2D & loc, EFloat radius)
            { return EDataMgr::Instance().CreateShapeCircle(coordUnits, loc, radius); })
        .def("create_shape_path", [](const ECoordUnits & coordUnits, const std::vector<FPoint2D> & points, EFloat width)
            { return EDataMgr::Instance().CreateShapePath(coordUnits, points, width); })
        .def("create_shape_polygon", [](const ECoordUnits & coordUnits, const std::vector<FPoint2D> & points, EFloat cornerR)
            { return EDataMgr::Instance().CreateShapePolygon(coordUnits, points, cornerR); })
        .def("create_shape_polygon", [](const ECoordUnits & coordUnits, const std::vector<FCoord> & coords, EFloat cornerR)
            { return EDataMgr::Instance().CreateShapePolygon(coordUnits, coords, cornerR); })

        .def("create_box", [](const ECoordUnits & coordUnits, const FPoint2D & ll, const FPoint2D & ur)
            { return EDataMgr::Instance().CreateBox(coordUnits, ll, ur); })
            
        // settings
        .def("hier_sep", []
            { return EDataMgr::Instance().HierSep(); })
        .def("threads", []
            { return EDataMgr::Instance().Threads(); })   

    ;
}

PYBIND11_MODULE(PyEcad, ecad)
{
    ecad_init_basic(ecad);
    ecad_init_design(ecad);
    ecad_init_utility(ecad);
    ecad_init_datamgr(ecad);
}