#pragma once
#include "ECadCommon.h"
#include "Protocol.h"
namespace ecad {

class ECAD_API IHierarchyObj : public Clonable<IHierarchyObj>, public Transformable2D
{
    ECAD_SERIALIZATION_ABSTRACT_CLASS_FUNCTIONS_DECLARATION
public:
    virtual ~IHierarchyObj() = default;
};
}//namespace ecad
ECAD_SERIALIZATION_ABSTRACT_CLASS(ecad::IHierarchyObj)