#include <boost/stacktrace.hpp>
#include <string_view>
#include <filesystem>
#include <cassert>
#include <csignal>

#include "../test/TestData.hpp"
#include "simulation/EThermalNetworkExtraction.h"
#include "EDataMgr.h"

void SignalHandler(int signum)
{
    ::signal(signum, SIG_DFL);
    std::cout << boost::stacktrace::stacktrace();
    ::raise(SIGABRT);
}

int main(int argc, char * argv[])
{
    ::signal(SIGSEGV, &SignalHandler);
    ::signal(SIGABRT, &SignalHandler);

    using namespace ecad;
    using namespace generic::fs;

    auto & eDataMgr = EDataMgr::Instance();
    eDataMgr.SetDefaultThreads(1);
    std::string filename = ecad_test::GetTestDataPath() + "/gdsii/4004.gds";
    std::string layermap = ecad_test::GetTestDataPath() + "/gdsii/4004.layermap";
    auto database = eDataMgr.CreateDatabaseFromGds("4004", filename, layermap);
    // std::string filename = ecad_test::GetTestDataPath() + "/xfl/fccsp.xfl";
    // auto database = eDataMgr.CreateDatabaseFromXfl("fccsp", filename);
    if (nullptr == database) return EXIT_FAILURE;

    std::vector<Ptr<ICell> > cells;
    database->GetCircuitCells(cells);
    assert(cells.size() == 1);
    
    auto layout = cells.front()->GetLayoutView();
    auto bbox = layout->GetBoundary()->GetBBox();
    auto iter = layout->GetLayerCollection()->GetLayerIter();
    while (auto layer = iter->Next())
        std::cout << "thickness: " << layer->GetStackupLayerFromLayer()->GetThickness() << std::endl;

    ELayoutPolygonMergeSettings mergeSettings;
    mergeSettings.outFile = ecad_test::GetTestDataPath() + "/simulation/thermal";
    layout->MergeLayerPolygons(mergeSettings);

    EThermalNetworkExtractionSettings extSettings;
    extSettings.outDir = ecad_test::GetTestDataPath() + "/simulation/thermal";
    extSettings.dumpHotmaps = true;
    extSettings.dumpDensityFile = true;
    extSettings.dumpTemperatureFile = true;

    size_t xGrid = 50;
    extSettings.grid = {xGrid, static_cast<size_t>(xGrid * EValue(bbox.Width()) / bbox.Length())};
    extSettings.mergeGeomBeforeMetalMapping = false;

    esim::EThermalNetworkExtraction ne;
    ne.SetExtractionSettings(extSettings);
    auto gridModel = ne.GenerateGridThermalModel(layout);

    EDataMgr::Instance().ShutDown();

    return EXIT_SUCCESS;
}