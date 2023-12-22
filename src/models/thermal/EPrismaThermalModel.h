#pragma once

#include "generic/geometry/Triangulation.hpp"
#include <boost/geometry/index/rtree.hpp>

#include "EThermalModel.h"
#include "EShape.h"

namespace ecad {

using namespace generic::geometry::tri;
class ILayoutView;
class IMaterialDef;
class IMaterialDefCollection;
namespace emodel::etherm {

struct ECAD_API ECompactLayout
{
    using Height = int;
    using LayerRange = std::pair<Height, Height>;
    struct PowerBlock
    {
        size_t polygon;
        Height position;
        LayerRange range;
        ESimVal powerDensity;
        PowerBlock(size_t polygon, Height position, LayerRange range, ESimVal powerDensity);
    };

    struct Bondwire
    {
        ENetId netId;
        EValue radius{0};
        EMaterialId matId;
        EValue current{0};
        std::array<size_t, 2> layer;
        std::vector<FCoord> heights;
        std::vector<EPoint2D> pt2ds;
    };

    std::vector<ENetId> nets;
    std::vector<LayerRange> ranges; 
    std::vector<Bondwire> bondwires;
    std::vector<EMaterialId> materials;
    std::vector<EPolygonData> polygons;
    std::unordered_map<size_t, PowerBlock> powerBlocks;

    explicit ECompactLayout(EValue vScale2Int);
    virtual ~ECompactLayout() = default;
    void AddShape(ENetId netId, EMaterialId solidMat, EMaterialId holeMat, CPtr<EShape> shape, FCoord elevation, FCoord thickness);
    size_t AddPolygon(ENetId netId, EMaterialId matId, EPolygonData polygon, bool isHole, FCoord elevation, FCoord thickness);
    void AddPowerBlock(EMaterialId matId, EPolygonData polygon, ESimVal totalP, FCoord elevation, FCoord thickness, EValue position = 0.9);
    bool WriteImgView(std::string_view filename, size_t width = 512) const;

    void BuildLayerPolygonLUT();

    size_t TotalLayers() const;
    bool hasPolygon(size_t layer) const;
    size_t SearchPolygon(size_t layer, const EPoint2D & pt) const;
    bool GetLayerHeightThickness(size_t layer, FCoord & elevation, FCoord & thickness) const;
    const EPolygonData & GetLayoutBoundary() const;
private:
    LayerRange GetLayerRange(FCoord elevation, FCoord thickness) const;
private:
    using RtVal = std::pair<EBox2D, size_t>;
    using Rtree = boost::geometry::index::rtree<RtVal, boost::geometry::index::rstar<8>>;
    std::unordered_map<size_t, std::shared_ptr<Rtree> > m_rtrees;
    std::unordered_map<size_t, std::vector<size_t> > m_lyrPolygons;
    std::unordered_map<Height, size_t> m_height2Index;
    std::vector<Height> m_layerOrder;
    EValue m_vScale2Int;
};

ECAD_API UPtr<ECompactLayout> makeCompactLayout(CPtr<ILayoutView> layout);

class ECAD_API EPrismaThermalModel : public EThermalModel
{
public:
    struct LineElement
    {
        ENetId netId;
        EMaterialId matId;
        EValue radius{0};
        EValue current{0};
        size_t id{invalidIndex};
        std::array<size_t, 2> endPoints;
        std::array<size_t, 2> neighbors;//global index
    };

    using PrismaTemplate = tri::Triangulation<EPoint2D>;
    struct PrismaElement
    {
        ENetId netId;
        EMaterialId matId;
        ESimVal avePower{0};
        size_t id{invalidIndex};
        size_t templateId{invalidIndex};
        inline static constexpr size_t TOP_NEIGHBOR_INDEX = 3;
        inline static constexpr size_t BOT_NEIGHBOR_INDEX = 4;
        std::array<size_t, 5> neighbors = {noNeighbor, noNeighbor, noNeighbor, noNeighbor, noNeighbor};//[edge1, edge2, edge3, top, bot];
    };

    struct PrismaLayer
    {
        size_t id;
        FCoord elevation;
        FCoord thickness;
        std::vector<PrismaElement> elements;
        
        PrismaElement & operator[] (size_t index) { return elements[index]; }
        const PrismaElement & operator[] (size_t index) const { return elements.at(index); }

        PrismaElement & AddElement(size_t templateId)
        {
            auto & ele = elements.emplace_back(PrismaElement{});
            ele.id = elements.size() - 1;
            ele.templateId = templateId;
            return ele;
        }

        size_t TotalElements() const { return elements.size(); }
    };

    struct PrismaInstance
    {
        CPtr<PrismaLayer> layer{nullptr};
        CPtr<PrismaElement> element{nullptr};
        std::array<size_t, 6> vertices;//top, bot
        std::array<size_t, 5> neighbors = {noNeighbor, noNeighbor, noNeighbor, noNeighbor, noNeighbor};//[edge1, edge2, edge3, top, bot];
    };
    
    ESimVal uniformBcSide{0};
    BCType sideBCType{BCType::HTC};
    PrismaTemplate prismaTemplate;
    std::vector<PrismaLayer> layers;
    explicit EPrismaThermalModel(CPtr<ILayoutView> layout);
    virtual ~EPrismaThermalModel() = default;

    CPtr<IMaterialDefCollection> GetMaterialLibrary() const;

    PrismaLayer & AppendLayer(PrismaLayer layer);
    LineElement & AddLineElement(FPoint3D start, FPoint3D end, ENetId netId, EMaterialId matId, EValue radius, EValue current);

    void BuildPrismaModel(EValue scaleH2Unit, EValue scale2Meter);
    void AddBondWire(const ECompactLayout::Bondwire & bondwire);
    EValue Scale2Meter() const;  
    size_t TotalLayers() const;
    size_t TotalElements() const;
    size_t TotalLineElements() const;
    size_t TotalPrismaElements() const;
    size_t GlobalIndex(size_t lineIdx) const;
    size_t GlobalIndex(size_t lyrIndex, size_t eleIndex) const;
    std::pair<size_t, size_t> PrismaLocalIndex(size_t index) const;//[lyrIndex, eleIndex]
    size_t LineLocalIndex(size_t index) const;
    bool isPrima(size_t index) const;

    const std::vector<FPoint3D> GetPoints() const { return m_points; }
    const FPoint3D & GetPoint(size_t index) const { return m_points.at(index); }
    const PrismaInstance & GetPrisma(size_t index) const { return m_prismas.at(index); }
    const LineElement & GetLine(size_t index) const { return m_lines.at(index); }

    bool NeedIteration() const { return false; } //wbtest,todo
    bool isTopLayer(size_t lyrIndex) const;
    bool isBotLayer(size_t lyrIndex) const;
    size_t AddPoint(FPoint3D point);
    FPoint3D GetPoint(size_t lyrIndex, size_t eleIndex, size_t vtxIndex) const;

    size_t SearchPrismaInstance(size_t layer, const EPoint2D & pt) const;//todo, eff
private:
    EValue m_scaleH2Unit;
    EValue m_scale2Meter;
    CPtr<ILayoutView> m_layout;
    std::vector<FPoint3D> m_points;
    std::vector<LineElement> m_lines;
    std::vector<PrismaInstance> m_prismas;
    std::vector<size_t> m_indexOffset;
};

ECAD_ALWAYS_INLINE EValue EPrismaThermalModel::Scale2Meter() const
{
    return m_scale2Meter;
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::TotalLayers() const
{
    return layers.size();
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::TotalElements() const
{
    return TotalLineElements() + TotalPrismaElements();
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::TotalLineElements() const
{
    return m_lines.size();
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::TotalPrismaElements() const
{
    return m_indexOffset.back();
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::GlobalIndex(size_t lineIdx) const
{
    return m_indexOffset.back() + lineIdx;
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::GlobalIndex(size_t lyrIndex, size_t eleIndex) const
{
    return m_indexOffset[lyrIndex] + eleIndex;
}

ECAD_ALWAYS_INLINE std::pair<size_t, size_t> EPrismaThermalModel::PrismaLocalIndex(size_t index) const
{
    size_t lyrIdex = 0;
    while (not (m_indexOffset[lyrIdex] <= index && index < m_indexOffset[lyrIdex + 1])) lyrIdex++;
    return std::make_pair(lyrIdex, index - m_indexOffset[lyrIdex]);
}

ECAD_ALWAYS_INLINE size_t EPrismaThermalModel::LineLocalIndex(size_t index) const
{
    ECAD_ASSERT(index >= m_indexOffset.back());
    return index - m_indexOffset.back();
}

ECAD_ALWAYS_INLINE bool EPrismaThermalModel::isPrima(size_t index) const
{
    return index < m_indexOffset.back();
}

ECAD_ALWAYS_INLINE bool EPrismaThermalModel::isTopLayer(size_t lyrIndex) const
{
    return 0 == lyrIndex;
}

ECAD_ALWAYS_INLINE bool EPrismaThermalModel::isBotLayer(size_t lyrIndex) const
{
    return lyrIndex + 1 == TotalLayers();
}

} // namespace emodel::etherm
} // namespace ecad