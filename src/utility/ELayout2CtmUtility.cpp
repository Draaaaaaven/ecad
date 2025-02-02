#include "ELayout2CtmUtility.h"

#include "model/thermal/io/EChipThermalModelIO.h"
#include "model/thermal/io/EThermalModelIO.h"
#include "interface/ILayoutView.h"
#include "interface/ILayer.h"
#include "EMetalFractionMapping.h"
#include "basic/EShape.h"

namespace ecad {
namespace utils {
    
ECAD_INLINE ELayout2CtmUtility::ELayout2CtmUtility(Ptr<ILayoutView> layout)
 : m_layout(layout)
{
}

ECAD_INLINE ELayout2CtmUtility::~ELayout2CtmUtility()
{
}
    
ECAD_INLINE void ELayout2CtmUtility::SetLayout2CtmSettings(const ELayout2CtmSettings & settings)
{
    m_settings = settings;
}

ECAD_INLINE bool ELayout2CtmUtility::GenerateCTMv1File(std::string * err)
{
    if(nullptr == m_layout) return false;
    if(m_settings.dirName.empty() || m_settings.filename.empty()) return false;

    EGridThermalModelExtractionSettings settings(m_settings.dirName, m_settings.threads, m_settings.selectNets);
    auto & mfSettings = settings.metalFractionMappingSettings;
    mfSettings.outFile = std::string{};
    mfSettings.regionExtTop = m_settings.regionExtTop;
    mfSettings.regionExtBot = m_settings.regionExtBot;
    mfSettings.regionExtLeft = m_settings.regionExtLeft;
    mfSettings.regionExtRight = m_settings.regionExtRight;
    mfSettings.mergeGeomBeforeMapping = true;

    auto coordUnits = m_layout->GetCoordUnits();    
    auto boundary = m_layout->GetBoundary();
    auto bbox = boundary->GetBBox();

    bbox[0][0] -= coordUnits.toCoord(m_settings.regionExtLeft);
    bbox[0][1] -= coordUnits.toCoord(m_settings.regionExtBot);
    bbox[1][0] += coordUnits.toCoord(m_settings.regionExtRight);
    bbox[1][1] += coordUnits.toCoord(m_settings.regionExtTop);

    auto stride = coordUnits.toCoord(m_settings.resolution, ECoordUnits::Unit::Micrometer);
    auto [nx, ny] = generic::geometry::OccupancyGridMappingFactory::GetGridMapSize(bbox, {stride, stride});
    mfSettings.grid = {nx, ny};

    ELayoutMetalFractionMapper mapper(mfSettings);
    if(!mapper.GenerateMetalFractionMapping(m_layout)) {
        if(err) *err = "Error: failed to generate metal fraction mapping!";
        return false;
    }

    auto mf = mapper.GetLayoutMetalFraction();
    if(nullptr == mf) return false;

    using namespace ecad::model;
    auto gridModel = std::make_unique<EGridThermalModel>(settings, ESize2D(nx, ny));
    auto rx = coordUnits.toUnit(stride, ECoordUnits::Unit::Meter), ry = rx;
    gridModel->SetResolution(rx, ry);

    std::vector<Ptr<IStackupLayer> > layers;
    m_layout->GetStackupLayers(layers);
    ECAD_ASSERT(layers.size() == mf->size());

    for(size_t i = 0; i < layers.size(); ++i) {
        auto stackupLayer = layers.at(i);
        auto name = layers.at(i)->GetName();
        auto thickness = coordUnits.toUnit(stackupLayer->GetThickness(), ECoordUnits::Unit::Meter);
        auto layerMetalFraction = mf->at(i);
        EGridThermalLayer layer(name, layerMetalFraction);
        layer.SetIsMetal(layers.at(i)->GetLayerType() == ELayerType::ConductingLayer);
        layer.SetThickness(thickness);
        [[maybe_unused]] auto res = gridModel->AppendLayer(std::move(layer));
        ECAD_ASSERT(res != invalidIndex)
    }

    auto ctm = io::makeChipThermalModelV1FromGridThermalModel(*gridModel, false, 0, err);
    if(nullptr == ctm) return false;

    if(!io::GenerateCTMv1FileFromChipThermalModelV1(*ctm, m_settings.dirName, m_settings.filename, err)) return false;

    return true;
}

}//namespace utils
}//namespace ecad
