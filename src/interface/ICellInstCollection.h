#pragma once
#include "basic/ECadCommon.h"
#include "IIterator.h"
#include "ICellInst.h"
namespace ecad {

class ILayoutView;
class ECAD_API ICellInstCollection : public Clonable<ICellInstCollection>
{
    ECAD_SERIALIZATION_ABSTRACT_CLASS_FUNCTIONS_DECLARATION
public:
    virtual ~ICellInstCollection() = default;
    virtual Ptr<ICellInst> AddCellInst(UPtr<ICellInst> cellInst) = 0;
    virtual Ptr<ICellInst> CreateCellInst(Ptr<ILayoutView> layout, const std::string & name,
                                  Ptr<ILayoutView> defLayout, const ETransform2D & transform) = 0;
    virtual CellInstIter GetCellInstIter() const = 0;
    virtual size_t Size() const = 0;
    virtual void Clear() = 0;
};
}//namespace ecad
ECAD_SERIALIZATION_ABSTRACT_CLASS(ecad::ICellInstCollection)