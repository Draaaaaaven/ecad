#pragma once
#define BOOST_TEST_INCLUDED
#include <boost/test/unit_test.hpp>
#include <boost/test/test_tools.hpp>
#include "generic/tools/FileSystem.hpp"
#include "models/thermal/io/EChipThermalModelIO.h"
#include "TestData.hpp"
using namespace boost::unit_test;
using namespace ecad;
using namespace ecad::emodel::etherm;
void s_ctm_model_io_test()
{
    std::string err;
    std::string ctm = ecad_test::GetTestDataPath() + "/ctm/rhsc_ctm5.tar.gz";
    std::string ctmFolder = ecad_test::GetTestDataPath() + "/ctm/rhsc_ctm5";
    auto model = io::makeChipThermalModelFromCTMv1File(ctm, &err);
    BOOST_CHECK(generic::filesystem::PathExists(ctmFolder));
    generic::filesystem::RemoveDir(ctmFolder);
    BOOST_CHECK(err.empty());
    BOOST_CHECK(model);

    std::string outFile = "rhsc_ctm_out_test";
    std::string outFolder = ecad_test::GetTestDataPath() + "/ctm/rhsc_ctm5_out";
    bool res = io::GenerateCTMv1FileFromChipThermalModelV1(*model, outFolder, outFile, &err);
    BOOST_CHECK(res);
}

test_suite * create_ecad_model_test_suite()
{
    test_suite * model_suite = BOOST_TEST_SUITE("s_model_test");
    //
    model_suite->add(BOOST_TEST_CASE(&s_ctm_model_io_test));
    //
    return model_suite;
}