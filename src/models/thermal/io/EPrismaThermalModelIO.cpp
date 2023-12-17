#include "EPrismaThermalModelIO.h"
#include "generic/tools/Color.hpp"
namespace ecad::emodel::etherm::io {

ECAD_INLINE bool GenerateVTKFile(std::string_view filename, const EPrismaThermalModel & model, const std::vector<ESimVal> * temperature, std::string * err)
{
    if (not fs::CreateDir(fs::DirName(filename))) {
        if (err) *err = "Error: fail to create folder " + fs::DirName(filename).string();
        return false;
    }

    std::ofstream out(filename.data());
    if(!out.is_open()){
        if(err) *err = "Error: fail to open: " + std::string(filename);
        return false;
    }

    char sp(32);
    out << "# vtk DataFile Version 2.0" << ECAD_EOL;
    out << "Unstructured Grid" << ECAD_EOL;
    out << "ASCII" << ECAD_EOL;
    out << "DATASET UNSTRUCTURED_GRID" << ECAD_EOL;
    
    const auto & points = model.GetPoints();
    out << "POINTS" << sp << points.size() << sp << "FLOAT" << ECAD_EOL;
    for(const auto & point : points){
        out << point[0] << sp << point[1] << sp << point[2] << ECAD_EOL;
    }

    out << ECAD_EOL; 
    out << "CELLS" << sp << model.TotalElements() << sp << model.TotalElements() * 7 << ECAD_EOL;
    for (size_t i = 0; i < model.TotalElements(); ++i) {
        const auto & prisma = model[i];
        out << '6';
        for (auto vertex : prisma.vertices)
            out << sp << vertex;
        out << ECAD_EOL; 
    }
    out << ECAD_EOL;

    out << "CELL_TYPES" << sp << model.TotalElements() << ECAD_EOL;
    for (size_t i = 0; i < model.TotalElements(); ++i) {
        out << "13" << ECAD_EOL;
    }

    if (temperature && temperature->size() == model.TotalElements()) {
        out << "CELL_DATA" << sp << model.TotalElements() << ECAD_EOL;
        out << "SCALARS SCALARS FLOAT 1 " << ECAD_EOL;
        out << "LOOKUP_TABLE TEMPERATURE" << ECAD_EOL;
        for (const auto & t : *temperature)
            out << t << ECAD_EOL;

        out << ECAD_EOL;
out << "LOOKUP_TABLE TEMPERATURE 100" << ECAD_EOL;
        int r, g, b;
        for (size_t i = 0; i < 100; ++i) {
            generic::color::RGBFromScalar(i * 0.01, r, g, b);
            out << r / 255.0 << sp << g / 255.0 << sp << b / 255.0 << sp << 1.0 << ECAD_EOL;
        }
    }

    out.close();
    return true;
}

} // namespace ecad::emodel::etherm::io