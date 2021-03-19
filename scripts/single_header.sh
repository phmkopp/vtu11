#!/bin/bash

Path="../vtu11"
Single="vtu11.hpp"
cppversion=$1


InclusionOrder=()

case ${cppversion} in
    c++11) 
        echo Generating C++ 11 single header.
        InclusionOrder+="inc/filesystem.hpp"
        ;;
    c++17) 
        echo Generating C++ 17 single header.  
        ;;
    *) 
        echo Usage: ./single_header.sh [c++11\|c++17]
        exit
        ;;
esac

InclusionOrder+=("inc/alias.hpp"
                 "inc/writer.hpp"
                 "inc/utilities.hpp"
                 "inc/zlibWriter.hpp"
                 "vtu11.hpp"
                 "impl/utilities_impl.hpp"
                 "impl/writer_impl.hpp"
                 "impl/zlibWriter_impl.hpp"
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
    # Append header content without vtu11 includes and comments
    cat ${Path}/${header} | grep -v -e "^[[:space:]]*#include \"vtu11/inc/.*\.hpp"      |
                            grep -v -e "^[[:space:]]*#include \"vtu11/impl/.*\.hpp"     |
                            grep -v    "^[[:space:]]*//" >> ${Single}
done

