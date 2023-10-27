#pragma once
#include "interfaces/IComponentCollection.h"
#include "interfaces/IIterator.h"
#include "EBaseCollections.h"
namespace ecad {

class IComponent;
using EComponentIterator = EUnorderedMapCollectionIterator<std::string, UPtr<IComponent> >;
class ECAD_API EComponentCollection
 : public EUnorderedMapCollection<std::string, UPtr<IComponent> >
 , public IComponentCollection
{
    using BaseCollection = EUnorderedMapCollection<std::string, UPtr<IComponent> >;
    using BaseCollection::CollectionContainer;
    ECAD_SERIALIZATION_FUNCTIONS_DECLARATION
public:
    EComponentCollection();
    virtual ~EComponentCollection();

    ///Copy
    EComponentCollection(const EComponentCollection & other);
    EComponentCollection & operator= (const EComponentCollection & other);

    Ptr<IComponent> CreateComponent(const std::string & name, CPtr<IComponentDef> compDef, ELayerId layer, const ETransform2D & transform) override;

    ComponentIter GetComponentIter() const override;

    size_t Size() const override;
    void Clear() override;

protected:
    ///Copy
    virtual Ptr<EComponentCollection> CloneImp() const override { return new EComponentCollection(*this); }
};
}//namespace ecad
ECAD_SERIALIZATION_CLASS_EXPORT_KEY(ecad::EComponentCollection)