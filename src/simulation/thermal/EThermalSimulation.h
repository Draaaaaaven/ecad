#pragma once
#include "ECadSettings.h"
#include "ECadCommon.h"
namespace ecad {
class IModel;
namespace model {
class EGridThermalModel;
class EPrismaThermalModel;
} // model

namespace simulation {
using namespace ecad::model;
class ECAD_API EThermalSimulation
{
public:
    EThermalSimulationSetup setup;
    EThermalSimulation() = default;
    virtual ~EThermalSimulation() = default;
    bool Run(CPtr<IModel> model) const;
};

class ECAD_API EGridThermalSimulator
{
public:
    explicit EGridThermalSimulator(CPtr<EGridThermalModel> model, const EThermalSimulationSetup & setup);
    virtual ~EGridThermalSimulator() = default;

    bool RunStaticSimulation() const;
    bool RunTransientSimulation() const;
protected:
    EThermalNetworkSolveSettings m_settings;
    CPtr<EGridThermalModel> m_model{nullptr};
};

class ECAD_API EPrismaThermalSimulator
{
public:
    explicit EPrismaThermalSimulator(CPtr<EPrismaThermalModel> model, const EThermalSimulationSetup & setup);
    virtual ~EPrismaThermalSimulator() = default;

    bool RunStaticSimulation() const;
    bool RunTransientSimulation() const;
protected:
    EThermalNetworkSolveSettings m_settings;
    CPtr<EPrismaThermalModel> m_model{nullptr};
};
}//namespace simulations
}//namespace ecad