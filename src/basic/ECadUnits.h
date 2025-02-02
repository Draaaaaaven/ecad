#pragma once
#include <boost/core/ignore_unused.hpp>
#include <boost/serialization/split_member.hpp>
#include "generic/tools/Units.hpp"
#include "ECadSerialization.h"
#include "ECadAlias.h"
#define ECAD_UNUSED(ex) boost::ignore_unused(ex);

namespace ecad {

using namespace generic;
using namespace generic::geometry;

struct ESize2D
{
    size_t x = invalidIndex;
    size_t y = invalidIndex;
    ESize2D() = default;
    ESize2D(size_t x, size_t y) : x(x), y(y) {}

    size_t & operator[] (size_t i)
    {
        if(0 == i) return x;
        else return y; 
    }

    const size_t & operator[] (size_t i) const
    {
        if(0 == i) return x;
        else return y; 
    }

    bool operator == (const ESize2D & other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator != (const ESize2D & other) const
    {
        return !(*this == other);
    }

    bool isValid() const
    {
        return x != invalidIndex && y != invalidIndex;
    }

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ECAD_UNUSED(version)
        ar & boost::serialization::make_nvp("x", x);
        ar & boost::serialization::make_nvp("y", y);
    }
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT
};

struct ESize3D
{
    size_t x = invalidIndex;
    size_t y = invalidIndex;
    size_t z = invalidIndex;
    ESize3D() = default;
    ESize3D(size_t x, size_t y, size_t z) : x(x), y(y), z(z) {}
    ESize3D(const ESize2D & size, size_t z) : x(size.x), y(size.y), z(z) {}

    size_t & operator[] (size_t i)
    {
        if(0 == i) return x;
        else if(1 == i) return y;
        else return z;
    }

    const size_t & operator[] (size_t i) const
    {
        if(0 == i) return x;
        else if(1 == i) return y;
        else return z;
    }

    bool operator== (const ESize3D & other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!= (const ESize3D & other) const
    {
        return !(*this == other);
    }

    bool isValid() const
    {
        return x != invalidIndex && y != invalidIndex && z != invalidIndex;
    }

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ECAD_UNUSED(version)
        ar & boost::serialization::make_nvp("x", x);
        ar & boost::serialization::make_nvp("y", y);
        ar & boost::serialization::make_nvp("z", z);
    }
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT
};

struct ETemperature
{
    EFloat value{25};
    ETemperatureUnit unit{ETemperatureUnit::Celsius};
    ETemperature() = default;
    ETemperature(EFloat value, ETemperatureUnit unit) : value(value), unit(unit) {}
    EFloat inCelsius() const { return unit == ETemperatureUnit::Celsius ? value : generic::unit::Kelvins2Celsius(value); }
    EFloat inKelvins() const { return unit == ETemperatureUnit::Kelvins ? value : generic::unit::Celsius2Kelvins(value); }
    static EFloat Kelvins2Celsius(EFloat t) { return generic::unit::Kelvins2Celsius(t); }
    static EFloat Celsius2Kelvins(EFloat t) { return generic::unit::Celsius2Kelvins(t); }

    bool operator== (const ETemperature & t) const
    {
        return math::EQ(value, t.value) && unit == t.unit;
    }

    bool operator!= (const ETemperature & t) const
    {
        return not (*this == t);
    }

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ECAD_UNUSED(version)
        ar & boost::serialization::make_nvp("value", value);
        ar & boost::serialization::make_nvp("unit", unit);
    }
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT
};

class ECoordUnits
{
    EFloat unit = 1e-3;
    EFloat precision = 1e-9;
#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    friend class boost::serialization::access;

    template <typename Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ECAD_UNUSED(version)
        ar & boost::serialization::make_nvp("unit", unit);
        ar & boost::serialization::make_nvp("precision", precision);
    }
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT
public:
    using Unit = generic::unit::Length;
    ECoordUnits()
     : ECoordUnits(Unit::Micrometer, Unit::Nanometer)
    {
    }

    explicit ECoordUnits(Unit user)
     : ECoordUnits(user, Unit::Nanometer)
    {
    }

    /**
     * @brief Construct a new ECoordUnits object
     * 
     * @param user user unit, defualt is um
     * @param data database unit, default is nm
     */
    ECoordUnits(Unit user, Unit data)
    {
        precision = generic::unit::Scale2Meter(data);
        unit = precision / generic::unit::Scale2Meter(user);
    }
    
    void SetPrecision(EFloat value)
    {
        precision = value;
    }

    void SetUnit(EFloat value)
    {
        unit = value;
    }
    
    EFloat Scale2Unit() const
    {
        return unit;
    }

    EFloat Scale2Coord() const
    {
        return EFloat(1) / unit;
    }

    ECoord toCoord(const EFloat value) const
    {
        return value * Scale2Coord();
    }

    ECoord toCoord(const EFloat value, Unit unit) const
    {
        return value * generic::unit::Scale2Meter(unit) / precision;
    }

    FCoord toCoordF(const EFloat value) const
    {
        return value * Scale2Coord();
    }

    FCoord toCoordF(const EFloat value, Unit unit) const
    {
        return value * generic::unit::Scale2Meter(unit) / precision;
    }
    
    template <typename Coord>
    EFloat toUnit(const Coord coord) const
    {
        return coord * Scale2Unit();
    }

    template <typename Coord>
    EFloat toUnit(const Coord coord, Unit unit) const
    {
        return coord * precision / generic::unit::Scale2Meter(unit);
    }

    EPoint2D toCoord(const FPoint2D & fp) const
    {
        return EPoint2D(toCoord(fp[0]), toCoord(fp[1]));
    }

    EBox2D toCoord(const FBox2D & fb) const
    {
        return EBox2D(toCoord(fb[0]), toCoord(fb[1]));
    }

    std::vector<EPoint2D> toCoord(const std::vector<FPoint2D> & fpoints) const
    {
        std::vector<EPoint2D> points; points.reserve(fpoints.size());
        std::transform(fpoints.cbegin(), fpoints.cend(), std::back_inserter(points), [&](const FPoint2D & fp){ return toCoord(fp); });
        return points;
    }

    template <typename Coord>
    FPoint2D toUnit(const Point2D<Coord> & p) const
    {
        return FPoint2D(toUnit(p[0]), toUnit(p[1]));
    }
};

}//namespace ecad