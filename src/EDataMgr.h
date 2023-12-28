#pragma once
#include "ECadSettings.h"
#include "Interface.h"
#include "EShape.h"

#include <unordered_map>
#include <memory>
#include <mutex>
namespace ecad {

using namespace generic::geometry;

class ECAD_API EDataMgr
{
public:
    EDataMgr(const EDataMgr &) = delete;
    EDataMgr & operator= (const EDataMgr &) = delete;

    ///Database
    SPtr<IDatabase> CreateDatabase(const std::string & name);
    SPtr<IDatabase> OpenDatabase(const std::string & name);
    bool RemoveDatabase(const std::string & name);
    void ShutDown(bool autoSave = true);

    ///Thirdpart
    SPtr<IDatabase> CreateDatabaseFromGds(const std::string & name, const std::string & gds, const std::string & lyrMap = std::string{});
    SPtr<IDatabase> CreateDatabaseFromXfl(const std::string & name, const std::string & xfl);

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    bool SaveDatabase(SPtr<IDatabase> database, const std::string & archive, EArchiveFormat fmt = EArchiveFormat::BIN);
    bool LoadDatabase(SPtr<IDatabase> database, const std::string & archive, EArchiveFormat fmt = EArchiveFormat::BIN);
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

    ///Cell
    Ptr<ICell> CreateCircuitCell(SPtr<IDatabase> database, const std::string & name);
    Ptr<ICell> FindCellByName(SPtr<IDatabase> database, const std::string & name);

    ///Net
    Ptr<INet> CreateNet(Ptr<ILayoutView> layout, const std::string & name);
    Ptr<INet> FindNetByName(Ptr<ILayoutView> layout, const std::string & name);

    ///Layer
    UPtr<ILayer> CreateStackupLayer(const std::string & name, ELayerType type, EFloat elevation, EFloat thickness,
                                    const std::string & conductingMat = sDefaultConductingMat,
                                    const std::string & dirlectricMat = sDefaultDielectricMat);
    
    ///ComponentDef
    Ptr<IComponentDef> CreateComponentDef(SPtr<IDatabase> database, const std::string & name);
    Ptr<IComponentDef> FindComponentDefByName(SPtr<IDatabase> database, const std::string & name);

    ///Material
    Ptr<IMaterialDef> CreateMaterialDef(SPtr<IDatabase> database, const std::string & name);
    Ptr<IMaterialDef> FindMaterialDefByName(SPtr<IDatabase> database, const std::string & name);
    UPtr<IMaterialProp> CreateSimpleMaterialProp(EFloat value);
    UPtr<IMaterialProp> CreateAnsiotropicMaterialProp(const std::array<EFloat, 3> & values);
    UPtr<IMaterialProp> CreateTensorMateriaProp(const std::array<EFloat, 9> & values);

    ///LayerMap
    Ptr<ILayerMap> CreateLayerMap(SPtr<IDatabase> database, const std::string & name);
    Ptr<ILayerMap> FindLayerMapByName(SPtr<IDatabase> database, const std::string & name);

    ///PadstackDef
    Ptr<IPadstackDef> CreatePadstackDef(SPtr<IDatabase> database, const std::string & name);
    Ptr<IPadstackDef> FindPadstackDefByName(SPtr<IDatabase> database, const std::string & name) const;
    UPtr<IPadstackDefData> CreatePadstackDefData();

    ///PadstackInst
    Ptr<IPadstackInst> CreatePadstackInst(Ptr<ILayoutView> layout, const std::string & name, CPtr<IPadstackDef> def, ENetId net,
                                          ELayerId topLyr, ELayerId botLyr, CPtr<ILayerMap> layerMap,
                                          const ETransform2D & transform); 

    ///CellInstance
    Ptr<ICellInst> CreateCellInst(Ptr<ILayoutView> layout, const std::string & name,
                                  Ptr<ILayoutView> defLayout, const ETransform2D & transform);

    ///Component
    Ptr<IComponent> CreateComponent(Ptr<ILayoutView> layout, const std::string & name, CPtr<IComponentDef> compDef,
                                    ELayerId layer, const ETransform2D & transform, bool flipped);
                                    
    ///Primitive
    Ptr<IPrimitive> CreateGeometry2D(Ptr<ILayoutView> layout, ELayerId layer, ENetId net, UPtr<EShape> shape);
    Ptr<IBondwire> CreateBondwire(Ptr<ILayoutView> layout, std::string name, ENetId net, const FPoint2D & start, const FPoint2D & end, EFloat radius);

    ///Shape
    UPtr<EShape> CreateShapeRectangle(const ECoordUnits & coordUnits, const FPoint2D & ll, const FPoint2D & ur);
    UPtr<EShape> CreateShapeCircle(const ECoordUnits & coordUnits, const FPoint2D & loc, EFloat radius);
    UPtr<EShape> CreateShapePath(const ECoordUnits & coordUnits, const std::vector<FPoint2D> & points, EFloat width);
    UPtr<EShape> CreateShapePolygon(const ECoordUnits & coordUnits, const std::vector<FPoint2D> & points);
    
    UPtr<EShape> CreateShapeRectangle(EPoint2D ll, EPoint2D ur);
    UPtr<EShape> CreateShapeCircle(EPoint2D loc, ECoord radius);
    UPtr<EShape> CreateShapePath(std::vector<EPoint2D> points, ECoord width);
    UPtr<EShape> CreateShapePolygon(std::vector<EPoint2D> points);
    UPtr<EShape> CreateShapePolygon(EPolygonData polygon);
    UPtr<EShape> CreateShapePolygonWithHoles(EPolygonWithHolesData pwh);
    UPtr<EShape> CreateShapeFromTemplate(ETemplateShape ts, ETransform2D trans = ETransform2D{});

    EPolygon CreatePolygon(const ECoordUnits & coordUnits, const std::vector<FPoint2D> & points);
    EBox2D CreateBox(const ECoordUnits & coordUnits, const FPoint2D & ll, const FPoint2D & ur);

    ///Transform
    ETransform2D CreateTransform2D(const ECoordUnits & coordUnits, EFloat scale, EFloat rotation, const FVector2D & offset, EMirror2D mirror = EMirror2D::No);

    ///Text
    Ptr<IText> CreateText(Ptr<ILayoutView> layout, ELayerId layer, const ETransform2D & transform, const std::string & text);

    ///ComponentDefPin
    Ptr<IComponentDefPin> CreateComponentDefPin(Ptr<IComponentDef> compDef, const std::string & pinName, FPoint2D loc, EPinIOType type, CPtr<IPadstackDef> psDef = nullptr, ELayerId lyr = noLayer);
    
    ///Settings
    size_t DefaultThreads() const;
    void SetDefaultThreads(size_t threads);

    size_t DefaultCircleDiv() const;

    static EDataMgr & Instance();

private:
    EDataMgr();
    ~EDataMgr();

    EDataMgrSettings m_settings;
    std::unordered_map<std::string, SPtr<IDatabase> > m_databases;
};

}//namespace ecad
