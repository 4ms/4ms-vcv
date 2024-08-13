#pragma once

#include "../helpers/mapping.h"
using namespace Mapping;

#include "gcem/include/gcem.hpp"
#include <numbers>

namespace MetaModule
{

struct Range {
    static constexpr float min = 0.f;
    static constexpr float max = 1.f;
};

static constinit auto PanningTable = 
    Mapping::LookupTable_t<50>::generate<Range>([](auto potValue) {
        return gcem::sin(potValue * std::numbers::pi / 2);
    });

constexpr std::array<float, 101> LevelValues = {
    0.00000,
    0.00000,
    0.00000,
    0.00000,
    0.00000,
    0.00155,
    0.00590,
    0.01015,
    0.00865,
    0.01158,
    0.01481,
    0.01754,
    0.01885,
    0.02051,
    0.02361,
    0.02501,
    0.02639,
    0.02796,
    0.02916,
    0.03221,
    0.03433,
    0.03524,
    0.03658,
    0.03838,
    0.04037,
    0.04236,
    0.04392,
    0.04607,
    0.04678,
    0.04982,
    0.05093,
    0.05281,
    0.05568,
    0.05674,
    0.05874,
    0.06006,
    0.06150,
    0.06288,
    0.06597,
    0.06747,
    0.07005,
    0.07036,
    0.07328,
    0.07492,
    0.07628,
    0.07795,
    0.08231,
    0.08294,
    0.08893,
    0.09281,
    0.09599,
    0.10409,
    0.11188,
    0.12497,
    0.13919,
    0.15630,
    0.17596,
    0.19868,
    0.21935,
    0.24102,
    0.26323,
    0.28339,
    0.30416,
    0.32432,
    0.34480,
    0.36622,
    0.38738,
    0.40765,
    0.42796,
    0.45217,
    0.47344,
    0.49518,
    0.51407,
    0.53468,
    0.55631,
    0.57720,
    0.59738,
    0.61800,
    0.63912,
    0.66072,
    0.68212,
    0.70091,
    0.72191,
    0.74337,
    0.76408,
    0.78421,
    0.80476,
    0.82598,
    0.84734,
    0.86752,
    0.88819,
    0.90872,
    0.93067,
    0.95373,
    0.97391,
    0.98892,
    1.00000,
    1.00000,
    1.00000,
    1.00000,
    1.00000
};

constexpr inline LookupTable_t<LevelValues.size()> LevelTable(Range::min, Range::max, LevelValues);

}