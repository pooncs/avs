
#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
namespace avs { namespace core {
using id_t = std::uint64_t;
struct Image { int w=0,h=0,c=0; std::vector<unsigned char> data; };
struct Point3f { float x=0,y=0,z=0; };
struct PointCloud { std::vector<Point3f> pts; };
struct Pose { float T[16]{0}; };
struct Result { bool ok=true; std::string msg; };
}} // namespace
