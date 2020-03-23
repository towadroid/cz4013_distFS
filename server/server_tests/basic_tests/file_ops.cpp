//
// Created by towa-ubuntu on 3/23/20.
//

#include "gtest/gtest.h"
#include "../../utils/utils.hpp"
#include <string>
#include "../../constants.hpp"
#include "../../HelperClasses.hpp"

TEST(Rm_all, empty) {
    std::filesystem::path file_path{constants::FILE_DIR_PATH + "test_ins"};
    utils::insert_to_file(file_path, std::string{"abc"}, 0); //insert content so that file is not empty
    utils::remove_content_from_file(file_path);
    std::string content;
    utils::read_file_to_string(file_path, &content);

    EXPECT_EQ(0, content.length());
}

TEST(Rm_last_char, empty) {
    std::filesystem::path file_path{constants::FILE_DIR_PATH + "test_ins"};
    utils::insert_to_file(file_path, std::string{"abc"}, 0);
    std::string content;
    utils::read_file_to_string(file_path, &content);
    content.pop_back();

    utils::remove_last_char(file_path);
    std::string content2;
    utils::read_file_to_string(file_path, &content2);

    EXPECT_EQ(content, content2);
}
