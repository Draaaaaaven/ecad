#pragma once
#include "ECadCommon.h"
#include "ECadDef.h"
#include <unordered_set>
#include <array>
namespace ecad {

struct EDataMgrSettings {
    size_t threads = 8;
};

struct ELayoutPolygonMergeSettings
{
    std::string outFile;
    bool includePadstackInst = true;
    bool includeDielectricLayer = true;
    bool skipTopBotDielectricLayers = false;
    std::unordered_set<ENetId> selectNets;
};

struct EMetalFractionMappingSettings
{
    std::string outFile;
    EValue regionExtTop = 0;
    EValue regionExtBot = 0;
    EValue regionExtLeft = 0;
    EValue regionExtRight = 0;
    bool mergeGeomBeforeMapping = true;
    std::array<size_t, 2> grid = {1, 1};
    std::unordered_set<ENetId> selectNets;
};

struct EThermalNetworkExtractionSettings
{
    std::string outDir;
    bool dumpHotmaps = false;
    bool dumpSpiceFile = false;
    bool dumpDensityFile = false;
    bool dumpTemperatureFile = false;
    EValue regionExtTop = 0;
    EValue regionExtBot = 0;
    EValue regionExtLeft = 0;
    EValue regionExtRight = 0;
    std::array<size_t, 2> grid = {1, 1};
    bool mergeGeomBeforeMetalMapping = true;
};

struct EThermalNetworkSolveSettings
{
    size_t iteration = 10;
    EValue residual = 0.5;
    std::string spiceFile;
};

struct ELayout2CtmSettings
{
    std::string dirName;
    std::string filename;
    EValue resolution = 10;//unit: um
    EValue regionExtTop = 0;
    EValue regionExtBot = 0;
    EValue regionExtLeft = 0;
    EValue regionExtRight = 0;
    std::unordered_set<ENetId> selectNets;
};

struct ELayoutViewRendererSettings
{
    enum class Format {
        PNG = 0,
        VTK = 1//todo
    };
    Format format;
    size_t width = 1024;
    std::string dirName;
    std::unordered_set<ENetId> selectNets;
    std::unordered_set<ELayerId> selectLayers;
};

}//namespace ecad