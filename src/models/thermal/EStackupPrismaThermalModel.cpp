#include "EStackupPrismaThermalModel.h"
#include "models/thermal/utils/EStackupPrismaThermalModelQuery.h"
#include "generic/geometry/BooleanOperation.hpp"
namespace ecad::model {

ECAD_INLINE EStackupPrismaThermalModel::EStackupPrismaThermalModel(CPtr<ILayoutView> layout)
 : EPrismaThermalModel(layout)
{
}

ECAD_INLINE void EStackupPrismaThermalModel::BuildPrismaModel(EFloat scaleH2Unit, EFloat scale2Meter)
{
    m_scaleH2Unit = scaleH2Unit;
    m_scale2Meter = scale2Meter;
    m_indexOffset = std::vector<size_t>{0};
    for (size_t i = 0; i < TotalLayers(); ++i)
        m_indexOffset.emplace_back(m_indexOffset.back() + layers.at(i).TotalElements());
    

    auto total = TotalPrismaElements();
    m_prismas.resize(total);
    m_points.clear();
    for (size_t i = 0; i < total; ++i) {
        auto & instance = m_prismas[i];
        auto [lyrIdx, eleIdx] = PrismaLocalIndex(i);
        instance.layer = &layers.at(lyrIdx);
        instance.element = &instance.layer->elements.at(eleIdx);    
        //points
        const auto & triangles = m_prismaTemplates.at(lyrIdx)->triangles;
        const auto & vertices = triangles.at(instance.element->templateId).vertices;
        for (size_t v = 0; v < vertices.size(); ++v) {
            instance.vertices[v] = AddPoint(GetPoint(lyrIdx, eleIdx, v));
            instance.vertices[v + 3] = AddPoint(GetPoint(lyrIdx, eleIdx, v + 3));
        }

        //side neighbors
        for (size_t n = 0; n < 3; ++n) {
            if (auto nid = instance.element->neighbors.at(n); noNeighbor != nid) {
                auto nb = GlobalIndex(lyrIdx, instance.element->neighbors.at(n));
                instance.neighbors[n] = nb;
            }
        }
    }
    //top/bot neighbors
    auto getIntersectArea = [](const ETriangle2D & t1, const ETriangle2D & t2) {
        EFloat area = 0;
        std::vector<EPolygonData> output;
        generic::geometry::boolean::Intersect(t1, t2, output);
        std::for_each(output.begin(), output.end(), [&area](const EPolygonData & p) { area += p.Area(); });
        return area;
    };
    utils::EStackupPrismaThermalModelQuery query(this);
    using RtVal = model::utils::EStackupPrismaThermalModelQuery::RtVal;
    for (size_t i = 0; i < total; ++i) {
        auto & instance = m_prismas[i];
        auto [lyrIdx, eleIdx] = PrismaLocalIndex(i);
        auto triangle1 = query.GetPrismaInstanceTemplate(i);
        auto triangle1Area = triangle1.Area();
        if (isTopLayer(lyrIdx)) instance.neighbors[PrismaElement::TOP_NEIGHBOR_INDEX] = noNeighbor;
        else {
            auto topLyr = lyrIdx - 1;
            std::vector<RtVal> results;
            query.IntersectsPrismaInstance(topLyr, i, results);
            for (size_t j = 0; j < results.size(); ++j) {
                auto triangle2 = query.GetPrismaInstanceTemplate(results.at(j).second);
                if (auto area = getIntersectArea(triangle1, triangle2); area > 0)
                   instance.contactInstances.front().emplace_back(results.at(j).second, EFloat(area) / triangle1Area); 
            }
            if (instance.contactInstances.empty())
                instance.neighbors[PrismaElement::TOP_NEIGHBOR_INDEX] = noNeighbor;
            else instance.neighbors[PrismaElement::TOP_NEIGHBOR_INDEX] = i;
        }

        if (isBotLayer(lyrIdx)) instance.neighbors[PrismaElement::BOT_NEIGHBOR_INDEX] = noNeighbor;
        else {
            auto botLyr = lyrIdx + 1;
            std::vector<RtVal> results;
            query.IntersectsPrismaInstance(botLyr, i, results);
            for (size_t j = 0; j < results.size(); ++j) {
                auto triangle2 = query.GetPrismaInstanceTemplate(results.at(j).second);
                if (auto area = getIntersectArea(triangle1, triangle2); area > 0)
                    instance.contactInstances.back().emplace_back(results.at(j).second, EFloat(area) / triangle1Area);
            }
            if (instance.contactInstances.empty())
                instance.neighbors[PrismaElement::BOT_NEIGHBOR_INDEX] = noNeighbor;
            else instance.neighbors[PrismaElement::BOT_NEIGHBOR_INDEX] = i;
        }
    }
}
} //namespace ecad::model