#pragma once
#include "EBaseCollections.h"
#include <unordered_map>
#include <string>
namespace ecad {

class INetCollection;
class ICellCollection;
class ILayerCollection;
class IConnObjCollection;
class ICellInstCollection;
class ILayerMapCollection;
class IComponentCollection;
class IPrimitiveCollection;
class IDefinitionCollection;
class IMaterialDefCollection;
class IPadstackDefCollection;
class IComponentDefCollection;
class IHierarchyObjCollection;
class IPadstackInstCollection;
class IPadstackInstCollection;
class IComponentDefPinCollection;
class ECAD_API ECollectionCollection : public EUnorderedMapCollection<ECollectionType, UPtr<ICollection> >
{
    using BaseCollection = EUnorderedMapCollection<ECollectionType, UPtr<ICollection> >;
    ECAD_SERIALIZATION_FUNCTIONS_DECLARATION
public:
    ECollectionCollection();
    explicit ECollectionCollection(std::string name);
    virtual ~ECollectionCollection();
    
    ///Copy
    ECollectionCollection(const ECollectionCollection & other);
    ECollectionCollection & operator= (const ECollectionCollection & other);

    Ptr<ICollection> AddCollection(ECollectionType type);
    Ptr<ICollection> GetCollection(ECollectionType type) const;

    Ptr<INetCollection> NetCollection() const;
    Ptr<ICellCollection> CellCollection() const;
    Ptr<ILayerCollection> LayerCollection() const;
    Ptr<IConnObjCollection> ConnObjCollection() const;
    Ptr<ICellInstCollection> CellInstCollection() const;
    Ptr<ILayerMapCollection> LayerMapCollection() const;
    Ptr<IComponentCollection> ComponentCollection() const;
    Ptr<IPrimitiveCollection> PrimitiveCollection() const;
    Ptr<IDefinitionCollection> DefinitionCollection() const;
    Ptr<IMaterialDefCollection> MaterialDefCollection() const;
    Ptr<IPadstackDefCollection> PadstackDefCollection() const;
    Ptr<IComponentDefCollection> ComponentDefCollection() const;
    Ptr<IHierarchyObjCollection> HierarchyObjCollection() const;
    Ptr<IPadstackInstCollection> PadstackInstCollection() const;
    Ptr<IComponentDefPinCollection> ComponentDefPinCollection() const;

    size_t Size() const override;

    const std::string & GetName() const;
    std::string sUuid() const;

protected:
    ///Copy
    virtual Ptr<ECollectionCollection> CloneImp() const override { return new ECollectionCollection(*this); }
};

ECAD_ALWAYS_INLINE const std::string & ECollectionCollection::GetName() const
{
    return ECollection::GetName();
}

ECAD_ALWAYS_INLINE std::string ECollectionCollection::sUuid() const
{
    return ECollection::sUuid();
}

}//namespace ecad
ECAD_SERIALIZATION_CLASS_EXPORT_KEY(ecad::ECollectionCollection)