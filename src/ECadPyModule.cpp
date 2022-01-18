#include "ECadCommon.h"

#ifdef ECAD_BOOST_PYTHON_SUPPORT
#include "EPadstackDefData.h"
#include "ENetCollection.h"
#include "EPadstackDef.h"
#include "ELayoutView.h"
#include "ELayerMap.h"
#include "EDatabase.h"
#include "EDataMgr.h"
#include "ELayer.h"
#include "ECell.h"
#include "ENet.h"
#include "EObject.h"

namespace {
    using namespace ecad;
    using namespace boost::python;

    /**
     * @brief Adapter a non-member function that returns a unique_ptr to
     * a python function object that returns a raw pointer but
     * explicitly passes ownership to Python.
     * 
     * The lambda delegates to the original function, 
     * and releases() ownership of the instance to Python,
     * and the call policy indicates that Python will take ownership of the value returned from the lambda.
     * The mpl::vector describes the call signature to Boost.Python,
     * allowing it to properly manage function dispatching between the languages.
     */
    template <typename T, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (*fn)(Args...))
    {
        return make_function([fn](Args... args){ return fn(args...).release(); },
                                return_value_policy<manage_new_object>(),
                                boost::mpl::vector<T*, Args...>());
    }

    /**
     * @brief Adapter of member function
     */
    template <typename T, typename C, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (C::*fn)(Args...))
    {
        return make_function([fn](C & self, Args... args){ return (self.*fn)(args...).release(); },
                                return_value_policy<manage_new_object>(),
                                boost::mpl::vector<T *, C &, Args...>());
    }

    /**
     * @brief Adapter of const member function
     */
    template <typename T, typename C, typename ...Args>
    object adapt_unique(std::unique_ptr<T> (C::*fn)(Args...) const)
    {
        return make_function([fn](C & self, Args... args){ return (self.*fn)(args...).release(); },
                                return_value_policy<manage_new_object>(),
                                boost::mpl::vector<T *, C &, Args...>());
    }

    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EDataMgrSaveDatabaseTxt, EDataMgr::SaveDatabase, 2, 3)
    BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(EDataMgrLoadDatabaseTxt, EDataMgr::LoadDatabase, 2, 3)

    BOOST_PYTHON_MODULE(ECAD_LIB_NAME)
    {
        //Enum
        enum_<EArchiveFormat>("EArchiveFormat")
            .value("TXT", EArchiveFormat::TXT)
            .value("XML", EArchiveFormat::XML)
            .value("BIN", EArchiveFormat::BIN)
        ;

        enum_<ELayerType>("ELayerType")
            .value("Invalid", ELayerType::Invalid)
            .value("DielectricLayer", ELayerType::DielectricLayer)
            .value("ConductingLayer", ELayerType::ConductingLayer)
            .value("MetalizedSignal", ELayerType::MetalizedSignal)
        ;

        //Point
        class_<EPoint2D>("EPoint2D", init<ECoord, ECoord>())
        ;

        //Object
        class_<EObject>("EObject")
            .def("set_name", &EObject::SetName)
            .def("get_name", &EObject::GetName, return_value_policy<copy_const_reference>())
            .def("suuid", &EObject::sUuid)
        ;

        //Definition
        class_<IDefinition, boost::noncopyable>("IDefinition", no_init)
        ;

        class_<EDefinition, bases<EObject, IDefinition> >("EDefinition", no_init)
        ;

        //Net
        class_<INet, boost::noncopyable>("INet", no_init)
        ;

        class_<ENet, bases<EObject, INet> >("ENet", no_init)
        ;

        class_<IIterator<INet>, boost::noncopyable>("INetIter", no_init)
        ;

        class_<ENetIterator, bases<IIterator<INet> >, boost::noncopyable>("ENetIter", no_init)
            .def("next", &ENetIterator::Next, return_internal_reference<>())
            .def("current", &ENetIterator::Current, return_internal_reference<>())
        ;

        //Layer
        class_<ILayer, boost::noncopyable>("ILayer", no_init)
        ;

        class_<ELayer, bases<EObject, ILayer>, boost::noncopyable>("ELayer", no_init)
        ;

        class_<EStackupLayer, bases<ELayer> >("EStackupLayer", init<std::string, ELayerType>())
            .add_property("elevation", &EStackupLayer::GetElevation, &EStackupLayer::SetElevation)
            .add_property("thickness", &EStackupLayer::GetThickness, &EStackupLayer::GetThickness)
        ;

        //Layer Map
        class_<ILayerMap, boost::noncopyable>("ILayerMap", no_init)
        ;

        class_<ELayerMap, bases<EDefinition, ILayerMap> >("ELayerMap", no_init)
            .def("get_database", &ELayerMap::GetDatabase, return_internal_reference<>())
        ;

        //Padstack Def
        class_<IPadstackDef, boost::noncopyable>("IPadstackDef", no_init)
        ;

        class_<EPadstackDef, bases<EDefinition, IPadstackDef> >("EPadstackDef", init<std::string>())
        ;

        //Padstack Def Data
        class_<IPadstackDefData, boost::noncopyable>("IPadstackDefData", no_init)
        ;

        class_<EPadstackDefData, bases<IPadstackDefData> >("EPadstackDefData", no_init)
        ;

        //LayoutView
        class_<ILayoutView, boost::noncopyable>("ILayoutView", no_init)
        ;

        class_<ELayoutView, bases<ILayoutView> >("ELayoutView", no_init)
            .def("get_net_iter", adapt_unique(&ELayoutView::GetNetIter))
        ;

        //Cell
        class_<ICell, boost::noncopyable>("ICell", no_init)
        ;

        class_<ECell, bases<EDefinition, ICell>, boost::noncopyable>("ECell", no_init)
            .def("get_name", &ECell::GetName, return_value_policy<copy_const_reference>())
            .def("get_database", &ECell::GetDatabase, return_internal_reference<>())
            .def("get_layout_view", &ECell::GetLayoutView,  return_internal_reference<>()) 
        ;

        class_<ECircuitCell, bases<ECell> >("ECircuitCell", no_init)
            .def("get_layout_view", &ECircuitCell::GetLayoutView,  return_internal_reference<>()) 
        ;

        //Database
        class_<IDatabase, std::shared_ptr<IDatabase>, boost::noncopyable>("IDatabase", no_init)
        ;
        
        class_<EDatabase, bases<IDatabase>, std::shared_ptr<EDatabase> >("EDatabase", init<std::string>())
            .def("set_name", &EDatabase::SetName)
            .def("get_name", &EDatabase::GetName, return_value_policy<copy_const_reference>())
            .def("suuid", &EDatabase::sUuid)
        ;

        implicitly_convertible<std::shared_ptr<EDatabase>, std::shared_ptr<IDatabase> >();

        //DataMgr
        class_<EDataMgr,boost::noncopyable>("EDataMgr", no_init)
            .def("instance", &EDataMgr::Instance, return_value_policy<reference_existing_object>())
            .staticmethod("instance")
            .def("create_database", &EDataMgr::CreateDatabase)
            .def("open_database", &EDataMgr::OpenDatabase)
            .def("remove_database", &EDataMgr::RemoveDatabase)
#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
            .def("save_database", &EDataMgr::SaveDatabase)
            .def("save_database", static_cast<bool(EDataMgr::*)(SPtr<IDatabase>, const std::string &, EArchiveFormat)>(&EDataMgr::SaveDatabase), EDataMgrSaveDatabaseTxt())
            .def("load_database", &EDataMgr::LoadDatabase)
            .def("load_database", static_cast<bool(EDataMgr::*)(SPtr<IDatabase>, const std::string &, EArchiveFormat)>(&EDataMgr::LoadDatabase), EDataMgrLoadDatabaseTxt())
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT
            .def("create_circuit_cell", &EDataMgr::CreateCircuitCell, return_internal_reference<>())
            .def("find_cell_by_name", &EDataMgr::FindCellByName, return_internal_reference<>())
            .def("create_net", &EDataMgr::CreateNet, return_internal_reference<>())
            .def("create_stackup_layer", adapt_unique(&EDataMgr::CreateStackupLayer))
            .def("create_layer_map", &EDataMgr::CreateLayerMap, return_internal_reference<>())
            .def("create_padstack_def", &EDataMgr::CreatePadstackDef, return_internal_reference<>())
            .def("create_padstack_def_data", adapt_unique(&EDataMgr::CreatePadstackDefData))
        ;
    }
}
#endif//ECAD_BOOST_PYTHON_SUPPORT