#include "EThermalModel.h"
ECAD_SERIALIZATION_CLASS_EXPORT_IMP(ecad::model::EThermalModel)

namespace ecad {
namespace model {

#ifdef ECAD_BOOST_SERIALIZATION_SUPPORT
    
template <typename Archive>
void EThermalModel::serialize(Archive & ar, const unsigned int version)
{
    ECAD_UNUSED(version)
    boost::serialization::void_cast_register<EThermalModel, IModel>();
    ar & boost::serialization::make_nvp("uniform_BC", m_uniformBC);
}
    
ECAD_SERIALIZATION_FUNCTIONS_IMP(EThermalModel)
#endif//ECAD_BOOST_SERIALIZATION_SUPPORT

EGridPowerModel::EGridPowerModel(const ESize2D & size)
{
    m_table.reset(new EGridDataTable(size));
}

EGridPowerModel::EGridPowerModel(UPtr<EGridDataTable> table)
{
    m_table = std::move(table);
}

EFloat EGridPowerModel::Query(EFloat key, size_t x, size_t y, bool * success) const
{
    return m_table->Query(key, x, y, success);
}

std::pair<EFloat, EFloat> EGridPowerModel::GetRange() const
{
    return m_table->GetRange();
}

bool EGridPowerModel::NeedInterpolation() const
{
    return m_table->NeedInterpolation();
}

EBlockPowerModel::EBlockPowerModel(ESize2D ll, ESize2D ur, EFloat totalP)
 : ll(ll), ur(ur), totalPower(totalP)
{
}

EFloat EBlockPowerModel::Query(EFloat key, size_t x, size_t y, bool * success) const
{
    bool in{ll.x <= x && x <= ur.x && ll.y <= y && y <= ur.y};
    if (success) *success = in;
    return in ? totalPower / Size() : 0;
}

std::pair<EFloat, EFloat> EBlockPowerModel::GetRange() const
{
    return {totalPower / Size(), totalPower / Size()};
}

bool EBlockPowerModel::NeedInterpolation() const
{
    return false;
}

size_t EBlockPowerModel::Size() const
{
    return (ur.y - ll.y + 1) * (ur.x - ll.x + 1);
}

void EThermalModel::SetUniformBC(EOrientation orient, EThermalBoundaryCondition bc)
{
    m_uniformBC.emplace(orient, std::move(bc));
}

CPtr<EThermalBoundaryCondition> EThermalModel::GetUniformBC(EOrientation orient) const
{
    auto iter = m_uniformBC.find(orient);
    if (iter == m_uniformBC.cend()) return nullptr;
    if (not iter->second.isValid()) return nullptr;
    return &iter->second;
}

}//namespace model
}//namespace ecad
