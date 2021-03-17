#!/bin/bash

Path="source/vtu11"
Single="vtu11.hpp"

declare -a InclusionOrder=("external/filesystem/filesystem.hpp"
                           "inc/alias.hpp" 
                           "inc/writer.hpp"
                           "inc/utilities.hpp"
                           "external/zlib/inc/zlibWriter.hpp"
                           "vtu11.hpp"
                           "impl/utilities_impl.hpp"
                           "impl/writer_impl.hpp"
#                           "external/zlib/inc/zlibWriter_impl.hpp"
                           "impl/vtu11_impl.hpp")

echo "//          __        ____ ____        " > ${Single}
echo "// ___  ___/  |_ __ _/_   /_   |       " >> ${Single}
echo "// \\  \\/ /\\   __\\  |  \\   ||   |  " >> ${Single}
echo "//  \\   /  |  | |  |  /   ||   |      " >> ${Single}
echo "//   \\_/   |__| |____/|___||___|      " >> ${Single}
echo "//                                     " >> ${Single}
echo "//  License: BSD License ; see LICENSE " >> ${Single} 
echo "//                                     " >> ${Single}

echo "                                              " >> ${Single}
echo "// AUTOMATICALLY GENERATED SINGLE HEADER FILE." >> ${Single}
echo "                                              " >> ${Single}

for header in ${InclusionOrder[@]}
do
    cat ${Path}/${header} | grep -v -e "^[[:space:]]*#include \"inc/.*\.hpp"      | 
                            grep -v -e "^[[:space:]]*#include \"external/.*\.hpp" | 
                            grep -v -e "^[[:space:]]*#include \"impl/.*\.hpp"     | 
                            grep -v    "^[[:space:]]*//" >> ${Single}
done

