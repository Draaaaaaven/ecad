#pragma once
#include "interfaces/IPrimitiveCollection.h"
#include "interfaces/IIterator.h"
#include "EBaseCollections.h"
#include "EShape.h"
namespace ecad {

class IText;
class ILayerMap;
class IPrimitive;
using EPrimitiveIterator = EVectorCollectionIterator<UPtr<IPrimitive> >;
class ECAD_API EPrimitiveCollection : public EVectorCollection<UPtr<IPrimitive> >, public IPrimitiveCollection
{
    using BaseCollection = EVectorCollection<UPtr<IPrimitive> >;
    using BaseCollection::CollectionContainer;
    ECAD_SERIALIZATION_FUNCTIONS_DECLARATION
public:
    EPrimitiveCollection();
    virtual ~EPrimitiveCollection();
    
    ///Copy
    EPrimitiveCollection(const EPrimitiveCollection & other);
    EPrimitiveCollection & operator= (const EPrimitiveCollection & other);

    Ptr<IPrimitive> GetPrimitive(size_t index) const;
    Ptr<IPrimitive> AddPrimitive(UPtr<IPrimitive> primitive);
    bool SetPrimitive(UPtr<IPrimitive> primitive, size_t index);

    Ptr<IPrimitive> CreateGeometry2D(ELayerId layer, ENetId net, UPtr<EShape> shape);
    Ptr<IText> CreateText(ELayerId layer, const ETransform2D & transform, const std::string & text);

    void Map(CPtr<ILayerMap> lyrMap);

    PrimitiveIter GetPrimitiveIter() const;
    UPtr<IPrimitive> PopBack();

    size_t Size() const;
    void Clear();
    
protected:
    ///Copy
    virtual Ptr<EPrimitiveCollection> CloneImp() const override { return new EPrimitiveCollection(*this); }
};

}//namespace ecad
ECAD_SERIALIZATION_CLASS_EXPORT_KEY(ecad::EPrimitiveCollection)
