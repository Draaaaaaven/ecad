#define ECAD_UNIT_TEST
#include <boost/test/included/unit_test.hpp>
#include <boost/stacktrace.hpp>

#include "SerializationTest.hpp"
#include "SimulationTest.hpp"
#include "ExtensionTest.hpp"
#include "FunctionTest.hpp"
#include "UtilityTest.hpp"
#include "SolverTest.hpp"
#include "ModelTest.hpp"
#include "FlowTest.hpp"
using namespace boost::unit_test;

extern test_suite * create_ecad_serialization_test_suite();
extern test_suite * create_ecad_simulation_test_suite();
extern test_suite * create_ecad_extension_test_suite();
extern test_suite * create_ecad_function_test_suite();
extern test_suite * create_ecad_utility_test_suite();
extern test_suite * create_ecad_solver_test_suite();
extern test_suite * create_ecad_model_test_suite();
extern test_suite * create_ecad_flow_test_suite();

void t_additional()
{
    //add additional test here
    BOOST_CHECK(true);
}

void SignalHandler(int signum)
{
    ::signal(signum, SIG_DFL);
    std::cout << boost::stacktrace::stacktrace();
    ::raise(SIGABRT);
}

test_suite *
init_unit_test_suite( int argc, char* argv[] )
{
    ::signal(SIGSEGV, &SignalHandler);
    ::signal(SIGABRT, &SignalHandler);

    framework::master_test_suite().add(create_ecad_serialization_test_suite());
    framework::master_test_suite().add(create_ecad_simulation_test_suite());
    framework::master_test_suite().add(create_ecad_extension_test_suite());
    framework::master_test_suite().add(create_ecad_function_test_suite());
    framework::master_test_suite().add(create_ecad_utility_test_suite());
    framework::master_test_suite().add(create_ecad_solver_test_suite());
    framework::master_test_suite().add(create_ecad_model_test_suite());
    framework::master_test_suite().add(create_ecad_flow_test_suite());
    framework::master_test_suite().add(BOOST_TEST_CASE(&t_additional));
    return 0;
}