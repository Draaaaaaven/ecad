#include "ECellInstCollection.h"
ECAD_SERIALIZATION_CLASS_EXPORT_IMP(ecad::ECellInstCollection)

#include "interface/ILayoutView.h"
#include "design/ECellInst.h"
namespace ecad {

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    
template <typename Archive>
void ECellInstCollection::serialize(Archive & ar, const unsigned int version)
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<ECellInstCollection, ICellInstCollection>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(BaseCollection);
}

ECAD_SERIALIZATION_FUNCTIONS_IMP(ECellInstCollection)
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

ECellInstCollection::ECellInstCollection()
{
}

ECellInstCollection::~ECellInstCollection()
{
}

ECellInstCollection::ECellInstCollection(const ECellInstCollection & other)
{
    *this = other;
}

ECellInstCollection & ECellInstCollection::operator= (const ECellInstCollection & other)
{
    BaseCollection::operator=(other);
    return *this;
}

Ptr<ICellInst> ECellInstCollection::AddCellInst(UPtr<ICellInst> cellInst)
{
   Append(std::move(cellInst));
   return Back().get();  
}

Ptr<ICellInst> ECellInstCollection::CreateCellInst(Ptr<ILayoutView> reflayout, const std::string & name,
                                                               Ptr<ILayoutView> defLayout, const ETransform2D & transform)
{
    auto cellInst = new ECellInst(name, reflayout, defLayout);
    cellInst->EHierarchyObj::SetTransform(transform);
    Append(UPtr<ICellInst>(cellInst));
    return Back().get();
}

CellInstIter ECellInstCollection::GetCellInstIter() const
{
    return CellInstIter(new ECellInstIterator(*this));
}

size_t ECellInstCollection::Size() const
{
    return BaseCollection::Size();
}

void ECellInstCollection::Clear()
{
    BaseCollection::Clear();
}

}//namespace ecad