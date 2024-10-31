#pragma once
#include "basic/ECadCommon.h"
#include "basic/ELookupTable.h"
#include <string_view>
#include <istream>

namespace ecad::ext::kicad {

ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_BOTTOM_ADHES_ID = 32;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_FRONT_ADHES_ID = 33;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_BOTTOM_PASTE_ID = 34;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_FRONT_PASTE_ID  = 35;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_BOTTOM_SILKS_ID = 36;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_FRONT_SILKS_ID = 37;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_BOTTOM_MASK_ID = 38;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_FRONT_MASK_ID = 39;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_EDGE_CUT_ID = 44;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_BOTTOM_CRTYD_ID = 46;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_FRONT_CRTYD_ID = 47;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_BOTTOM_FAB_ID = 48;
ECAD_ALWAYS_INLINE static constexpr EIndex ECAD_KICAD_PCB_LAYER_FRONT_FAB_ID = 49;
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_BOTTOM_ADHES_STR = "B.Adhes";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_FRONT_ADHES_STR = "F.Adhes";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_BOTTOM_PASTE_STR = "B.Paste";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_FRONT_PASTE_STR = "F.Paste";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_BOTTOM_SILKS_STR = "B.SilkS";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_FRONT_SILKS_STR = "F.SilkS";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_BOTTOM_MASK_STR = "B.Mask";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_FRONT_MASK_STR = "F.Mask";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_EDGE_CUT_STR = "Edge.Cuts";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_BOTTOM_CRTYD_STR = "B.CrtYd";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_FRONT_CRTYD_STR = "F.CrtYd";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_BOTTOM_FAB_STR = "B.Fab";
ECAD_ALWAYS_INLINE static constexpr std::string_view ECAD_KICAD_PCB_LAYER_FRONT_FAB_STR = "F.Fab";

enum class PadType
{
    UNKNOWN,
    SMD,
    THRU_HOLE,
    CONNECT,
    NP_THRU_HOLE,
};

enum class PadShape
{
    UNKNOWN,
    RECT,
    ROUNDRECT,
    CIRCLE,
    OVAL,
    TRAPEZOID,
};

enum class ViaType
{
    UNKNOWN,
    THROUGH,
    MICRO,
    BLIND_BURIED,
};
struct Arc
{
    EIndex layer{invalidIndex};
    EFloat angle{0}, width{0};
    FPoint2D start{0, 0}, end{0, 0};
};

struct Line
{ 
    EIndex layer{invalidIndex};
    Int64 angle{0};
    EFloat width{0};
    FPoint2D start{0, 0}, end{0, 0};
};

struct Circle
{
    EIndex layer{invalidIndex};
    EFloat width{0}; //2 * radius
    FPoint2D center{0, 0}, end{0, 0};
};

struct Poly
{
    EIndex layer{invalidIndex};
    EFloat width{0};
    std::vector<FPoint2D> shape;
};

struct Text
{
    bool refOrValue{true};//ref=true
    bool hide{false};
    EIndex layer{invalidIndex};
    FPoint2D loc{0, 0};
    std::string text{};
};

enum class LayerGroup { INVALID, POWER, SIGNAL, USER };
enum class LayerType
{
    INVALID,
    SILK_SCREEN,
    SOLDER_PASTE,
    SOLDER_MASK,
    CONDUCTING,
    DIELECTRIC,
};

struct Layer
{
    EIndex id{invalidIndex};
    LayerGroup group{LayerGroup::INVALID};
    LayerType type{LayerType::INVALID};
    EFloat thickness{0};
    EFloat epsilonR{0};
    EFloat lossTangent{0};
    std::string attr;
    std::string name;
    std::string material;

    Layer(EIndex id, std::string name) : id(id), name(std::move(name)) {}

    void SetGroup(const std::string & str);
    void SetType(const std::string & str);
};

struct Pin
{
    EIndex padId{invalidIndex};
    std::vector<EIndex> layers;

    Pin() = default;
    Pin(EIndex padId) : padId(padId) {}

};

struct Via
{
    EIndex netId{invalidIndex};
    EFloat size{.0};
    EFloat drillSize{.0};
    ViaType type{ViaType::UNKNOWN};
    FPoint2D pos{0, 0};
    std::string startLayer;
    std::string endLayer;
};
 
struct Segment
{
    EIndex netId{invalidIndex};
    EFloat width{0};
    FPoint2D start{0, 0};
    FPoint2D end{0, 0};
    std::string layer{};
    bool display{false};
};

struct Rule
{
    EFloat radius{0};
    EFloat clearance{0};
};

struct Padstack
{
    EIndex id{invalidIndex};
    std::string name{};
    Rule rule;
    PadType type;
    PadShape shape;
    EFloat angle{0};
    EFloat roundRectRatio{0};
    FPoint2D pos;
    FPoint2D size;
    std::vector<FPoint2D> shapeCoords;
    std::vector<FPoint2D> shapePolygon;
    
    void SetType(const std::string & str);
    void SetShape(const std::string & str);
};

struct Net
{
    EIndex id{invalidIndex};
    EIndex netClassId{invalidIndex};
    std::string name;
    std::vector<Pin> pins;
    std::vector<Via> vias;
    std::vector<Segment> segments;
    EPair<EIndex, EIndex> diffPair;
    Net(EIndex id, std::string name) : id(id), name(std::move(name)) {}
};

struct Component
{
    bool flipped{false};
    EIndex layerId{invalidIndex};
    FPoint2D location{0, 0};
    EFloat angle{0};
    EFloat width{0};
    EFloat height{0};
    std::string name;
    std::vector<Arc> arcs;
    std::vector<Line> lines;
    std::vector<Poly> polys;
    std::vector<Padstack> pads;
    std::vector<Circle> circles;

    Component(std::string name) : name(std::move(name)) {}

    EIndex GetPadstackId(const std::string & name) const
    {
        auto iter = pad2IndexMap.find(name);
        if (iter == pad2IndexMap.cend()) return invalidIndex;
        return iter->second;
    }

    // lut
    std::unordered_map<std::string, EIndex> pad2IndexMap;
};

struct Database
{
    std::unordered_map<std::string, Component> components;
    std::unordered_map<std::string, Layer> layers;
    std::unordered_map<EIndex, Net> nets;

    std::vector<Pin> unconnectedPins;
    std::vector<Line> boundaryLines;

    // lut
    std::unordered_map<std::string_view, Ptr<Net>> netLut;

    // add
    Layer & AddLayer(EIndex id, std::string name);
    Net & AddNet(EIndex id, std::string name);
    Component & AddComponent(std::string name);

    Ptr<Net> FindNet(EIndex id);
    Ptr<Net> FindNet(const std::string & name);

    Ptr<Layer> FindLayer(const std::string & name);



    bool isComponentId(EIndex id) const
    {
        return id < components.size();
    }    
};

} // namespace ecad::ext::kicad