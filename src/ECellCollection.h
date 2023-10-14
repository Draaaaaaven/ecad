#pragma once
#include "interfaces/IDefinitionCollection.h"
#include "interfaces/ICellCollection.h"
#include "interfaces/IIterator.h"
#include "EBaseCollections.h"
namespace ecad {

class ICell;
using ECellIterator = EUnorderedMapCollectionIterator<std::string, UPtr<ICell> >;
class ECAD_API ECellCollection
 : public EUnorderedMapCollection<std::string, UPtr<ICell> > 
 , public ICellCollection
 , public IDefinitionCollection
{
    using BaseCollection = EUnorderedMapCollection<std::string, UPtr<ICell> >;
    using BaseCollection::CollectionContainer;
    ECAD_SERIALIZATION_FUNCTIONS_DECLARATION
public:
    ECellCollection();
    virtual ~ECellCollection();

    ///Copy
    ECellCollection(const ECellCollection & other);
    ECellCollection & operator= (const ECellCollection & other);

    Ptr<IDefinitionCollection> AddDefinitionCollection(EDefinitionType type) override;
    Ptr<IDefinitionCollection> GetDefinitionCollection(EDefinitionType type) const override;
    Ptr<IDefinition> AddDefinition(const std::string & name, UPtr<IDefinition> definition) override;
    Ptr<IDefinition> GetDefinition(const std::string & name, EDefinitionType type) const override;
    std::string GetNextDefName(const std::string & base, EDefinitionType type) const override;

    CellIter GetCellIter() const override;
    size_t Size() const override;
    void Clear() override;

protected:
    ///Copy
    virtual Ptr<ECellCollection> CloneImp() const override { return new ECellCollection(*this); }
};

}//namespace ecad
ECAD_SERIALIZATION_CLASS_EXPORT_KEY(ecad::ECellCollection)