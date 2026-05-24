#ifndef RENDER_HAIR_AUXILIARY_HPP
#define RENDER_HAIR_AUXILIARY_HPP

#include <filesystem>
#include <format>
#include <string>

namespace RenderHair::Auxiliary {

inline std::string getHairDistributionMapFilename(
    const std::filesystem::path& mesh_filename,
    std::string_view extension_of_map_with_dot) {
  std::string map_filename{mesh_filename.stem()};
  return std::format("{}_hair_distribution_map{}", map_filename,
                     extension_of_map_with_dot);
}

}  // namespace RenderHair::Auxiliary

#endif  // RENDER_HAIR_AUXILIARY_HPP
