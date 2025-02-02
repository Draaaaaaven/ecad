#pragma once
#include "model/thermal/EChipThermalModel.h"
#include <set>
namespace ecad::model::io {

ECAD_API UPtr<EGridThermalModel> makeGridThermalModelFromCTMv1File(std::string_view filename, size_t reduceOrder = 0, std::string * err = nullptr);
ECAD_API UPtr<EGridThermalModel> makeGridThermalModelFromCTMv1Model(const EChipThermalModelV1 & ctm, size_t reduceOrder = 0, std::string * err = nullptr);
ECAD_API UPtr<EChipThermalModelV1> makeChipThermalModelV1FromGridThermalModel(const EGridThermalModel & model, bool encrypted, size_t reduceOrder = 0, std::string * err = nullptr);

}//namespace ecad::model::io