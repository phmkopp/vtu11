//          __        ____ ____
// ___  ___/  |_ __ _/_   /_   |
// \  \/ /\   __\  |  \   ||   |
//  \   /  |  | |  |  /   ||   |
//   \_/   |__| |____/|___||___|
//
//  License: BSD License ; see LICENSE
//

#include "vtu11/vtu11.hpp"
#include "vtu11_testing.hpp"

namespace vtu11
{

TEST_CASE( "ComputeOffsets_test" )
{
    std::vector<VtkCellType> types { 1, 1, 3, 10, 1, 14, 9, 12, 21 };
    std::vector<VtkIndexType> expected_offsets { 1, 2, 4, 8, 9, 14, 18, 26, 29};

    std::vector<VtkIndexType> offsets = ComputeOffsets(types);

    CHECK(expected_offsets.size() == offsets.size());
    CHECK(types.size() == offsets.size());

    for (std::size_t i=0; i<types.size(); ++i) {
        CAPTURE(i);
        CHECK(expected_offsets[i] == offsets[i]);
    }

} // ComputeOffsets_test

} // namespace vtu11
