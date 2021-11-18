#ifndef ECAD_EDEFINITIONCOLLECTION_H
#define ECAD_EDEFINITIONCOLLECTION_H
#include "interfaces/IDefinitionCollection.h"
#include "ECollectionCollection.h"

namespace ecad {

class ECAD_API EDefinitionCollection : public ECollectionCollection, public IDefinitionCollection
{
    ECAD_SERIALIZATION_FUNCTIONS_DECLARATION
public:
    EDefinitionCollection();
    explicit EDefinitionCollection(std::string name);
    virtual ~EDefinitionCollection();

    ///Copy
    EDefinitionCollection(const EDefinitionCollection & other);
    EDefinitionCollection & operator= (const EDefinitionCollection & other);

    virtual Ptr<IDefinitionCollection> AddDefinitionCollection(EDefinitionType type);
    virtual Ptr<IDefinitionCollection> GetDefinitionCollection(EDefinitionType type) const;
    virtual Ptr<IDefinition> AddDefinition(const std::string & name, UPtr<IDefinition> definition);
    virtual Ptr<IDefinition> GetDefinition(const std::string & name, EDefinitionType type) const;
    virtual std::string GetNextDefName(const std::string & name, EDefinitionType type) const;
    virtual size_t Size() const;
protected:
    ///Copy
    virtual Ptr<EDefinitionCollection> CloneImp() const override { return new EDefinitionCollection(*this); }
};

}//namespace ecad

ECAD_SERIALIZATION_CLASS_EXPORT_KEY(ecad::EDefinitionCollection)

#ifdef ECAD_HEADER_ONLY
#include "EDefinitionCollection.cpp"
#endif

#endif//ECAD_EDEFINITIONCOLLECTION_H