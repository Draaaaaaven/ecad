#pragma once
#include "ECadCommon.h"
#include "ECadDef.h"
#include "Protocol.h"
#include <string>
namespace ecad {
class ICell;
class IPadstackDef;
class IComponentDefPin;
class ECAD_API IComponentDef : public Clonable<IComponentDef>, public Printable
{
    ECAD_SERIALIZATION_ABSTRACT_CLASS_FUNCTIONS_DECLARATION
public:
    virtual ~IComponentDef() = default;
    virtual const std::string & GetName() const = 0;

    virtual void SetComponentType(EComponentType type) = 0;
    virtual EComponentType GetComponentType() const = 0;

    virtual void SetBondingBox(const EBox2D & bbox) = 0;
    virtual const EBox2D & GetBondingBox() const = 0;

    virtual void SetHeight(FCoord height) = 0;
    virtual FCoord GetHeight() const = 0;

    virtual Ptr<IComponentDefPin> CreatePin(const std::string & name, EPoint2D loc, EPinIOType type, CPtr<IPadstackDef> psDef = nullptr, ELayerId lyr = noLayer) = 0;

};
}//namespace ecad
ECAD_SERIALIZATION_ABSTRACT_CLASS(ecad::IComponentDef)