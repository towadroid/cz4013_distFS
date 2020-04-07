//
// Created by towa-ubuntu on 3/23/20.
//

#include "gtest/gtest.h"
#include "../../utils/utils.hpp"
#include <string>
#include <fstream>
#include "../../constants.hpp"
#include "../../HelperClasses/HelperClasses.hpp"

std::filesystem::path create_non_empty_file(std::string file_name) {
    file_name.insert(0, constants::FILE_DIR_PATH);
    auto path = std::filesystem::path{file_name};
    std::ofstream ofs;
    ofs.open(path, std::ofstream::out | std::ofstream::trunc);
    ofs << "0123456789\n";
    for (int i = 0; i < 20; ++i) {
        ofs << "123456789\n";
    }
    return path;
}

TEST(Rm_all, empty) {
    auto path = create_non_empty_file(std::string{"test_file"});

    utils::remove_content_from_file(path);
    std::string content = utils::read_file_to_string(path);

    EXPECT_EQ(0, content.length());
}

TEST(Rm_last_char, empty) {
    auto path = create_non_empty_file(std::string{"test_file"});
    std::string content = utils::read_file_to_string(path);
    content.pop_back();

    utils::remove_last_char(path);
    std::string content2 = utils::read_file_to_string(path);

    EXPECT_EQ(content, content2);
}
