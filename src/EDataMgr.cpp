#ifndef ECAD_HEADER_ONLY
#include "EDataMgr.h"
#endif

#include "extension/ECadExtension.h"
#include "EPadstackDefData.h"
#include "EMaterialProp.h"
#include "ELayoutView.h"
#include "EDatabase.h"
#include "ELayerMap.h"
#include "ELayer.h"
#include "EShape.h"
#include "ECell.h"
namespace ecad {

ECAD_INLINE EDataMgr::EDataMgr()
{
}

ECAD_INLINE EDataMgr::~EDataMgr()
{
}

ECAD_INLINE SPtr<IDatabase> EDataMgr::CreateDatabase(const std::string & name)
{
    // std::lock_guard<std::mutex> lock(m_databaseMutex);
    if(m_databases.count(name)) return nullptr;

    auto database = std::make_shared<EDatabase>(name);
    m_databases.insert(std::make_pair(name, database));
    return database;
}

ECAD_INLINE SPtr<IDatabase> EDataMgr::OpenDatabase(const std::string & name)
{
    // std::lock_guard<std::mutex> lock(m_databaseMutex);
    if(!m_databases.count(name)) return nullptr;

    return m_databases[name];
}

ECAD_INLINE bool EDataMgr::RemoveDatabase(const std::string & name)
{
    // std::lock_guard<std::mutex> lock(m_databaseMutex);
    return m_databases.erase(name) > 0;
}

ECAD_INLINE void EDataMgr::ShutDown(bool autoSave)
{
    // std::lock_guard<std::mutex> lock(m_databaseMutex);
    //todo
    m_databases.clear();
}

ECAD_INLINE SPtr<IDatabase> EDataMgr::CreateDatabaseFromGds(const std::string & name, const std::string & gds, const std::string & lyrMap)
{
    //wbteststd::lock_guard<std::mutex> lock(m_databaseMutex);
    if(m_databases.count(name)) return nullptr;

    auto database = ext::CreateDatabaseFromGds(name, gds, lyrMap);
    if(database) {
        m_databases.insert(std::make_pair(name, database));
    }
    return database;
}

ECAD_INLINE SPtr<IDatabase> EDataMgr::CreateDatabaseFromXfl(const std::string & name, const std::string & xfl)
{
    //wbteststd::lock_guard<std::mutex> lock(m_databaseMutex);
    if(m_databases.count(name)) return nullptr;

    auto database = ext::CreateDatabaseFromXfl(name, xfl);
    if(database) {
        m_databases.insert(std::make_pair(name, database));
    }
    return database;
}

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
ECAD_INLINE bool EDataMgr::SaveDatabase(SPtr<IDatabase> database, const std::string & archive, EArchiveFormat fmt)
{
    return database->Save(archive, fmt);
}
    
ECAD_INLINE bool EDataMgr::LoadDatabase(SPtr<IDatabase> database, const std::string & archive, EArchiveFormat fmt)
{
    return database->Load(archive, fmt);
}
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

ECAD_INLINE Ptr<ICell> EDataMgr::CreateCircuitCell(SPtr<IDatabase> database, const std::string & name)
{
    return database->CreateCircuitCell(name);
}

ECAD_INLINE Ptr<ICell> EDataMgr::FindCellByName(SPtr<IDatabase> database, const std::string & name)
{
    return database->FindCellByName(name);
}

ECAD_INLINE Ptr<INet> EDataMgr::CreateNet(Ptr<ILayoutView> layout, const std::string & name)
{
    return layout->CreateNet(name);
}

ECAD_INLINE Ptr<INet> EDataMgr::FindNetByName(Ptr<ILayoutView> layout, const std::string & name)
{
    return layout->FindNetByName(name);
}

ECAD_INLINE UPtr<ILayer> EDataMgr::CreateStackupLayer(const std::string & name, ELayerType type, FCoord elevation, FCoord thickness,
                                                      const std::string & conductingMat, const std::string & dielectricMat)
{
    auto stackupLayer = new EStackupLayer(name, type);
    stackupLayer->SetElevation(elevation);
    stackupLayer->SetThickness(thickness);
    stackupLayer->SetConductingMaterial(conductingMat);
    stackupLayer->SetDielectricMaterial(dielectricMat);
    return UPtr<ILayer>(stackupLayer);
}

ECAD_INLINE Ptr<IMaterialDef> EDataMgr::CreateMaterialDef(SPtr<IDatabase> database, const std::string & name)
{
    return database->CreateMaterialDef(name);
}

ECAD_INLINE Ptr<IMaterialDef> EDataMgr::FindMaterialDefByName(SPtr<IDatabase> database, const std::string & name)
{
    return database->FindMaterialDefByName(name);
}

ECAD_INLINE UPtr<IMaterialProp> EDataMgr::CreateSimpleMaterialProp(EValue value)
{
    auto prop = new EMaterialPropValue;
    prop->SetSimpleProperty(value);
    return UPtr<IMaterialProp>(new EMaterialPropValue);
}

ECAD_INLINE UPtr<IMaterialProp> EDataMgr::CreateAnsiotropicMaterialProp(const std::array<EValue, 3> & values)
{
    auto prop = new EMaterialPropValue;
    prop->SetAnsiotropicProerty(values);
    return UPtr<IMaterialProp>(prop);
}

ECAD_INLINE UPtr<IMaterialProp> EDataMgr::CreateTensorMateriaProp(const std::array<EValue, 9> & values)
{
    auto prop = new EMaterialPropValue;
    prop->SetTensorProperty(values);
    return UPtr<IMaterialProp>(prop);
}

ECAD_INLINE Ptr<ILayerMap> EDataMgr::CreateLayerMap(SPtr<IDatabase> database, const std::string & name)
{
    return database->CreateLayerMap(name);
}

ECAD_INLINE Ptr<ILayerMap> EDataMgr::FindLayerMapByName(SPtr<IDatabase> database, const std::string & name)
{
    return database->FindLayerMapByName(name);
}

ECAD_INLINE Ptr<IPadstackDef> EDataMgr::CreatePadstackDef(SPtr<IDatabase> database, const std::string & name)
{
    return database->CreatePadstackDef(name);
}

ECAD_INLINE Ptr<IPadstackDef> EDataMgr::FindPadstackDefByName(SPtr<IDatabase> database, const std::string & name) const
{
    return database->FindPadstackDefByName(name);
}

ECAD_INLINE UPtr<IPadstackDefData> EDataMgr::CreatePadstackDefData()
{
    auto defData = new EPadstackDefData;
    return UPtr<IPadstackDefData>(defData);
}

ECAD_INLINE Ptr<IPadstackInst> EDataMgr::CreatePadstackInst(Ptr<ILayoutView> layout, const std::string & name, CPtr<IPadstackDef> def, ENetId net,
                                                            ELayerId topLyr, ELayerId botLyr, CPtr<ILayerMap> layerMap,
                                                            const ETransform2D & transform)
{
    return layout->CreatePadstackInst(name, def, net, topLyr, botLyr, layerMap, transform);
}

ECAD_INLINE Ptr<ICellInst> EDataMgr::CreateCellInst(Ptr<ILayoutView> layout, const std::string & name,
                                                    Ptr<ILayoutView> defLayout, const ETransform2D & transform)
{
    return layout->CreateCellInst(name, defLayout, transform);
}

ECAD_INLINE Ptr<IPrimitive> EDataMgr::CreateGeometry2D(Ptr<ILayoutView> layout, ELayerId layer, ENetId net, UPtr<EShape> shape)
{
    return layout->CreateGeometry2D(layer, net, std::move(shape));
}

ECAD_INLINE UPtr<EShape> EDataMgr::CreateShapePath(std::vector<EPoint2D> points, ECoord width)
{
    auto shape = new EPath;
    shape->shape = std::move(points);
    shape->SetWidth(width);
    return UPtr<EShape>(shape);
}


ECAD_INLINE UPtr<EShape> EDataMgr::CreateShapePolygon(std::vector<Point2D<ECoord> > points)
{
    EPolygonData polygon;
    polygon.Set(std::move(points));
    return CreateShapePolygon(polygon);
}

ECAD_INLINE UPtr<EShape> EDataMgr::CreateShapePolygon(EPolygonData polygon)
{
    auto shape = new EPolygon;
    shape->shape = std::move(polygon);
    return UPtr<EShape>(shape);
}

ECAD_INLINE UPtr<EShape> EDataMgr::CreateShapePolygonWithHoles(EPolygonWithHolesData pwh)
{
    auto shape = new EPolygonWithHoles;
    shape->shape = std::move(pwh);
    return UPtr<EShape>(shape);
}

ECAD_INLINE UPtr<EShape> EDataMgr::CreateShapeFromTemplate(ETemplateShape ts, ETransform2D trans)
{
    auto shape = new EShapeFromTemplate(ts, trans);
    return UPtr<EShape>(shape);
}

ECAD_INLINE Ptr<IText> EDataMgr::CreateText(Ptr<ILayoutView> layout, ELayerId layer, const ETransform2D & transform, const std::string & text)
{
    return layout->CreateText(layer, transform, text);
}

ECAD_INLINE EDataMgr & EDataMgr::Instance()
{
    static EDataMgr mgr;
    return mgr;
}

}//namespace ecad