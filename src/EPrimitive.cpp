#include "EPrimitive.h"
ECAD_SERIALIZATION_CLASS_EXPORT_IMP(ecad::EPrimitive)
ECAD_SERIALIZATION_CLASS_EXPORT_IMP(ecad::EGeometry2D)
ECAD_SERIALIZATION_CLASS_EXPORT_IMP(ecad::EText)

#include "interfaces/ILayer.h"
#include "interfaces/INet.h"
#include "EShape.h"

#include "generic/geometry/Utility.hpp"

namespace ecad {

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT

template <typename Archive>
ECAD_INLINE void EPrimitive::save(Archive & ar, const unsigned int version) const
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EPrimitive, IPrimitive>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EConnObj);
    ar & boost::serialization::make_nvp("layer", m_layer);
    ar & boost::serialization::make_nvp("type", m_type);
}

template <typename Archive>
ECAD_INLINE void EPrimitive::load(Archive & ar, const unsigned int version)
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EPrimitive, IPrimitive>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EConnObj);
    ar & boost::serialization::make_nvp("layer", m_layer);
    ar & boost::serialization::make_nvp("type", m_type);
}

ECAD_SERIALIZATION_FUNCTIONS_IMP(EPrimitive)
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

ECAD_INLINE EPrimitive::EPrimitive()
 : EPrimitive(std::string{}, noLayer, noNet)
{
}

ECAD_INLINE EPrimitive::EPrimitive(std::string name, ELayerId layer, ENetId net)
 : EConnObj(std::move(name), net)
 , m_layer(layer)
 , m_type(EPrimitiveType::Invalid)
{
}

ECAD_INLINE EPrimitive::EPrimitive(ENetId net)
 : EPrimitive(std::string{}, noLayer, net)
{
}

ECAD_INLINE EPrimitive::~EPrimitive()
{
}

ECAD_INLINE EPrimitive::EPrimitive(const EPrimitive & other)
{
    *this = other;
}

ECAD_INLINE EPrimitive & EPrimitive::operator= (const EPrimitive & other)
{
    EConnObj::operator=(other);
    m_layer = other.m_layer;
    m_type = other.m_type;
    return *this;
}

ECAD_INLINE Ptr<IText> EPrimitive::GetTextFromPrimitive()
{
    return dynamic_cast<Ptr<IText> >(this);  
}

ECAD_INLINE Ptr<IConnObj> EPrimitive::GetConnObjFromPrimitive()
{
    return dynamic_cast<Ptr<IConnObj> >(this);
}

ECAD_INLINE Ptr<IGeometry2D> EPrimitive::GetGeometry2DFromPrimitive()
{
    if (m_type != EPrimitiveType::Geometry2D) return nullptr;
    return dynamic_cast<Ptr<IGeometry2D> >(this);
}

ECAD_INLINE Ptr<IBondwire> EPrimitve::GetBondwireFromPrimitive()
{
    if (m_type != EPrimitiveType::Bondwire) return nullptr;
    return dynamic_cast<Ptr<IBondwire> >(this);
}

ECAD_INLINE void EPrimitive::SetNet(ENetId net)
{
    EConnObj::SetNet(net);
}

ECAD_INLINE ENetId EPrimitive::GetNet() const
{
    return EConnObj::GetNet();
}

ECAD_INLINE void EPrimitive::SetLayer(ELayerId layer)
{
    m_layer = layer;
}

ECAD_INLINE ELayerId EPrimitive::GetLayer() const
{
    return m_layer;
}

ECAD_INLINE EPrimitiveType EPrimitive::GetPrimitiveType() const
{
    return m_type;
}

ECAD_INLINE void EPrimitive::PrintImp(std::ostream & os) const
{
    os << "LAYER: " << m_layer << ECAD_EOL;
}

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT

template <typename Archive>
ECAD_INLINE void EGeometry2D::save(Archive & ar, const unsigned int version) const
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EGeometry2D, IGeometry2D>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EPrimitive);
    ar & boost::serialization::make_nvp("shape", m_shape);
}

template <typename Archive>
ECAD_INLINE void EGeometry2D::load(Archive & ar, const unsigned int version)
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EGeometry2D, IGeometry2D>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EPrimitive);
    ar & boost::serialization::make_nvp("shape", m_shape);
}

ECAD_SERIALIZATION_FUNCTIONS_IMP(EGeometry2D)
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

ECAD_INLINE EGeometry2D::EGeometry2D()
 : EGeometry2D(noLayer, noNet, nullptr)
{
}

ECAD_INLINE EGeometry2D::EGeometry2D(ELayerId layer, ENetId net, UPtr<EShape> shape)
 : EPrimitive("", layer, net)
 , m_shape(std::move(shape))
{
    m_type = EPrimitiveType::Geometry2D;
}

ECAD_INLINE EGeometry2D::EGeometry2D(ELayerId layer, ENetId net)
 : EGeometry2D(layer, net, nullptr)
{
}

ECAD_INLINE EGeometry2D::~EGeometry2D()
{
}

ECAD_INLINE EGeometry2D::EGeometry2D(const EGeometry2D & other)
{
    *this = other;
}

ECAD_INLINE EGeometry2D & EGeometry2D::operator= (const EGeometry2D & other)
{
    EPrimitive::operator=(other);
    m_shape = CloneHelper(other.m_shape);
    return *this;
}

ECAD_INLINE void EGeometry2D::SetShape(UPtr<EShape> shape)
{
    m_shape = std::move(shape);
}

ECAD_INLINE Ptr<EShape> EGeometry2D::GetShape() const
{
    return m_shape.get();
}

ECAD_INLINE void EGeometry2D::Transform(const ETransform2D & transform)
{
    if(m_shape) m_shape->Transform(transform);
}

ECAD_INLINE void EGeometry2D::PrintImp(std::ostream & os) const
{
    os << "TYPE: " << "GEOMETRY2D" << ECAD_EOL;
    os << *m_shape << ECAD_EOL;
    EPrimitive::PrintImp(os);
}

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT

template <typename Archive>
ECAD_INLINE void EBondwire::save(Archive & ar, const unsigned int version) const
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EBondwire, IBondwire>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EPrimitive);
    ar & boost::serialization::make_nvp("end_layer", m_endLayer);
    ar & boost::serialization::make_nvp("start_loc", m_startLoc);
    ar & boost::serialization::make_nvp("end_loc", m_endLoc);
    ar & boost::serialization::make_nvp("radius", m_radius);
}

template <typename Archive>
ECAD_INLINE void EBondwire::load(Archive & ar, const unsigned int version)
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EBondwire, IBondwire>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EPrimitive);
    ar & boost::serialization::make_nvp("end_layer", m_endLayer);
    ar & boost::serialization::make_nvp("start_loc", m_startLoc);
    ar & boost::serialization::make_nvp("end_loc", m_endLoc);
    ar & boost::serialization::make_nvp("radius", m_radius);
}

ECAD_SERIALIZATION_FUNCTIONS_IMP(EBondwire)
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

ECAD_INLINE EBondwire::EBondwire()
 : EBondwire(std::string{}, noNet, noLayer, noLayer, EPoint2D{0, 0}, EPoint2D{0, 0})
{
}

ECAD_INLINE EBondwire::EBondwire(std::string name, ENetId net, ELayerId startLyr, ELayerId endLyr, EPoint2D startLoc, EPoint2D endLoc)
 : EPrimitive(std::move(name), startLyr, net), m_endLayer(endLyr), m_startLoc(std::move(startLoc)), m_endLoc(std::move(endLoc))
{
    m_type = EPrimitiveType::Bondwire;
}

ECAD_INLINE EBondwire::EBondwire(const EBondwire & other)
{
    *this = other;
}

ECAD_INLINE EBondwire & EBondwire::operator= (const EBondwire & other)
{
    EPrimitive::operator=(other);
    m_endLayer = other.m_endLayer;
    m_startLoc = other.m_startLoc;
    m_endLoc = other.m_endLoc;
    m_radius = other.m_radius;
    return *this;
}

ECAD_INLINE void EBondwire::SetRadius(FCoord r)
{
    m_radius = r;
}

ECAD_INLINE FCoord EBondwire::GetRadius() const
{
    return m_radius;
}

ECAD_INLINE void EBondwire::Transform(const ETransform2D & transform)
{
    auto trans = transform.GetTransform();
    generic::geometry::Transform(m_startLoc, trans);
    generic::geometry::Transform(m_endLoc, trans);
}

ECAD_INLINE void EBondwire::PrintImp(std::ostream & os) const
{
    os << "TYPE: " << "BONDWIRE" << ECAD_EOL;
    os << "START LAYER: " << m_layer << ", END LAYER: " << m_endLayer << ECAD_EOL;
    os << "START LOCATION: " << m_startLoc << ", END LOCATION: " << m_endLoc << ECAD_EOL;
    os << "RADIUS: " << m_radius << ECAD_EOL;
    EPrimitive::PrintImp(os);
}

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT

template <typename Archive>
ECAD_INLINE void EText::save(Archive & ar, const unsigned int version) const
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EText, IText>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EPrimitive);
    ar & boost::serialization::make_nvp("text", m_text);
    ar & boost::serialization::make_nvp("transform", m_transform);
}

template <typename Archive>
ECAD_INLINE void EText::load(Archive & ar, const unsigned int version)
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EText, IText>();
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(EPrimitive);
    ar & boost::serialization::make_nvp("text", m_text);
    ar & boost::serialization::make_nvp("transform", m_transform);
}

ECAD_SERIALIZATION_FUNCTIONS_IMP(EText)
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

ECAD_INLINE EText::EText()
 : EText(std::string{}, noLayer, noNet)
{
}

ECAD_INLINE EText::EText(std::string text, ELayerId layer, ENetId net)
 : EPrimitive("", layer, net)
 , m_text(std::move(text))
{
    m_type = EPrimitiveType::Text;
}

ECAD_INLINE EText::EText(std::string text)
 : EText(std::move(text), noLayer, noNet)
{
}

ECAD_INLINE EText::~EText()
{
}

ECAD_INLINE EText::EText(const EText & other)
{
    *this = other;
}

ECAD_INLINE EText & EText::operator= (const EText & other)
{
    EPrimitive::operator=(other);
    m_text = other.m_text;
    m_transform = other.m_transform;
    return *this;
}

ECAD_INLINE const std::string & EText::GetText() const
{
    return m_text;
}

ECAD_INLINE EPoint2D EText::GetPosition() const
{
    auto transform = m_transform.GetTransform();
    return EPoint2D(transform(0, 2), transform(1, 2));
}

}//namespace ecad