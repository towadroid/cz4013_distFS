//
// Created by Tobias on 23.02.2020.
//

#ifndef SERVER_CONSTANTS_HPP
#define SERVER_CONSTANTS_HPP

#include <string>

namespace constants{
    // invocation semantics
    constexpr int ATLEAST = 0;
    constexpr int ATMOST = 1;

    constexpr int BACKLOG = 10; // how many pending connections queue will hold

    const std::string IEEE754_NONCOMPLIANCE("Not IEC559/IEEE754 compliant, cannot (un-)marshal data");

    /** Path for files in the "file system".
     *
     * The working directory per default is "..."/cz4013_distFS/server/cmake-build-debug
     * Can change by setting "working directory" by editing run configuration
     */
    const std::string FILE_DIR_PATH("../filesystem_root/");
}


#endif //SERVER_CONSTANTS_HPP
