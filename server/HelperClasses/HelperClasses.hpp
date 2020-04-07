//
// Created by towa-ubuntu on 3/15/20.
//

#ifndef SERVER_HELPERCLASSES_HPP
#define SERVER_HELPERCLASSES_HPP

#include <netinet/in.h>

#include <utility>
#include "../utils/utils.hpp"
#include "../constants.hpp"
#include "exception"

class TimeoutElement_less {
public:
    bool operator()(const TimeoutElement &lhs, const TimeoutElement &rhs) {
        return std::get<0>(lhs) < std::get<0>(rhs);
    }
};

class SockaddrStor_Equal {
public:
    /** Two sockaddr storages are equal if address, port and family are the same
     *
     * Is that true in general??
     *
     * @param lhs
     * @param rhs
     * @return
     */
    bool operator()(const sockaddr_storage &lhs, const sockaddr_storage &rhs) const {
        bool name_equal = utils::get_in_addr_str(&rhs) == utils::get_in_addr_str(&lhs);
        bool port_equal = utils::get_in_port(&lhs) == utils::get_in_port(&rhs);
        bool family_equal = ((sockaddr *) &lhs)->sa_family == ((sockaddr *) &rhs)->sa_family;
        return name_equal && port_equal && family_equal;
    }
};


class SockaddrStor_Hasher {
private:
    template<class T>
    inline void hash_combine(std::size_t &s, const T &v) const {
        std::hash<T> h{};
        s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }

public:
    size_t operator()(const sockaddr_storage &p) const {
        size_t res = 0;
        hash_combine(res, utils::get_in_addr_str(&p));
        hash_combine(res, utils::get_in_port(&p));
        hash_combine(res, ((sockaddr *) &p)->sa_family);
        return res;
    }
};

class File_does_not_exist : std::runtime_error {
protected:
    std::string path;
public:
    File_does_not_exist(const std::string &arg, std::string path) : runtime_error(arg), path(std::move(path)) {};

    [[nodiscard]] char const *what() const noexcept override {
        std::string message{"The file at"};
        message.append(path);
        message.append("does not exist!");
        return message.c_str();
    }
};

class Offset_out_of_range : std::out_of_range {
protected:
    int max_range;
public:
    Offset_out_of_range(const std::string &arg, const int max_range) : out_of_range(arg),
                                                                       max_range(max_range) {};

    [[nodiscard]] char const *what() const noexcept override {
        std::string message{"The specified offset exceeds "};
        message.append(std::to_string(max_range));
        return message.c_str();
    }
};

class File_already_empty : std::runtime_error {
protected:
    std::string path;
public:
    File_already_empty(const std::string &arg, std::string path) : runtime_error(arg), path(std::move(path)) {};

    [[nodiscard]] char const *what() const noexcept override {
        std::string message{"The file at"};
        message.append(path);
        message.append("is already empty!");
        return message.c_str();
    }
};

#endif //SERVER_HELPERCLASSES_HPP
