//
// Created by towa-ubuntu on 2/28/20.
//

#include <iostream>
#include "constants.hpp"
#include "UdpServer_linux.hpp"
#include "utils.hpp"

using std::cout;
using std::endl;

int main(int argc, char **argv) {
    std::string tfile;
    utils::read_file_to_string(FILE_DIR_PATH+"file1", &tfile);
    cout << tfile;

    utils::insert_to_file(FILE_DIR_PATH+"test_ins","abc",2);
}