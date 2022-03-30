#ifndef ECAD_HEADER_ONLY
#include "models/thermal/EChipThermalModel.h"
#endif

#include "generic/tools/Format.hpp"

namespace ecad {
namespace emodel {
namespace etherm {

using namespace generic::format;

ECAD_INLINE std::ostream & operator<< (std::ostream & os, const ECTMv1LayerStackup & stackup)
{
    os << std::setiosflags(std::ios::left);
    os << std::fixed << std::setprecision(6);
    os << std::setw(16) << "Name"<< std::setw(16) << "Height" << std::setw(16) << "Thickness" << GENERIC_DEFAULT_EOL;
    for(const auto & layer : stackup.layers) {
        os << std::setw(16) << layer.name << std::setw(16) << layer.elevation << std::setw(16) << layer.thickness << GENERIC_DEFAULT_EOL;
    }
}

ECAD_INLINE EChipThermalModelV1::~EChipThermalModelV1()
{
}

ECAD_INLINE std::string EChipThermalModelV1::GetLastMatelLayerInStackup() const
{
    auto stackup = GetLayerStackup();
    if(nullptr == stackup) return std::string{};

    size_t size = stackup->names.size();
    for(size_t i = 0; i < size; ++i) {
        const auto & names = stackup->names[size - i - 1];
        for(const auto & name : names) {
            if(isMetalLayer(name))
                return stackup->layers[size - i - 1].name;
        }
    }
    return std::string{};
}

ECAD_INLINE bool EChipThermalModelV1::GetLayerHeightThickness(const std::string & name, EValue & height, EValue & thickness) const
{
    //if metal
    for(const auto & layer : header.metalLayers) {
        if(name == layer.name) {
            height = layer.elevation;
            thickness = layer.thickness;
            return true;
        }
    }
    //if via
    for(const auto & layer : header.viaLayers) {
        if(name == layer.name) {
            EValue topH, topT, botH, botT;
            if(GetLayerHeightThickness(layer.topLayer, topH, topT) &&
                GetLayerHeightThickness(layer.botLayer, botH, botT)) {
                height = topH - topT;
                thickness = height - botH;
                return true;
            }
        }
    }
    return false;
}

ECAD_INLINE CPtr<ECTMv1LayerStackup> EChipThermalModelV1::GetLayerStackup(std::string * info) const
{
    BuildLayerStackup(info);
    return m_layerStackup.get();
}

ECAD_INLINE bool EChipThermalModelV1::isMetalLayer(const std::string & name) const
{
    for(const auto & layer : header.metalLayers) {
        if(layer.name == name) return true;
    }
    return false;
}

ECAD_INLINE void EChipThermalModelV1::BuildLayerStackup(std::string * info) const
{
    if(m_layerStackup) return;

    m_layerStackup.reset(new ECTMv1LayerStackup);
    std::list<ECTMv1Layer> allLayers;
    for(const auto & name : header.techLayers) {
        EValue height, thickness;
        if(!GetLayerHeightThickness(name, height, thickness)) {
            if(info) *info += Format2String("Warning: failed to get height and thickness of layer: %1%, ignored!\n", name);
        }
        else {
            ECTMv1Layer layer;
            layer.name = name;
            layer.elevation = height;
            layer.thickness = thickness;
            allLayers.emplace_back(std::move(layer));
        }
    }
    allLayers.sort([](const ECTMv1Layer & l1, const ECTMv1Layer & l2){ return l1.elevation > l2.elevation; });

    ECTMv1Layer currLayer;
    EValue tolerance = 1e-6;
    auto & names = m_layerStackup->names;
    std::vector<std::pair<double, std::list<double> > > stackups;
    while(!allLayers.empty()) {
        currLayer = allLayers.front();
        allLayers.pop_front();

        std::unordered_set<std::string> layers { currLayer.name };
        auto stackup = std::make_pair(currLayer.elevation, std::list<EValue>());
        stackup.second.push_back(currLayer.thickness);
        while(!allLayers.empty() && math::EQ<EValue>(currLayer.elevation, allLayers.front().elevation, tolerance)) {
            if(info) *info += Format2String("Warning: find layers in same height: %1% and %2%, merged!\n", currLayer.name, allLayers.front().name);
            currLayer = allLayers.front();
            allLayers.pop_front();
            layers.insert(currLayer.name);
            stackup.second.push_back(currLayer.thickness);
        }
        names.push_back(layers);
        stackups.push_back(stackup);
    }
    auto getName = [](const auto & names){
        std::string result;
        for(const auto & name : names) {
            result.append(name);
            result.append("_");
        }
        result.pop_back();
        return result;
    };
    
    for(size_t i = 0; i < names.size(); ++i) {
        ECTMv1Layer layer;
        layer.name = getName(names[i]);
        layer.elevation = stackups[i].first;
        if(i == names.size() - 1)
            layer.thickness = *(std::max_element(stackups[i].second.begin(), stackups[i].second.end()));
        else layer.thickness = stackups[i].first - stackups[i + 1].first;
        m_layerStackup->layers.emplace_back(layer);
    }
}

}//namespace etherm
}//namespace emodel
}//namespace ecad
