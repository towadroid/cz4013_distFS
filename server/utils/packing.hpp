//
// Created by towa-ubuntu on 3/17/20.
//

#ifndef SERVER_PACKING_HPP
#define SERVER_PACKING_HPP

#include <string>
#include <cstring>
#include "../constants.hpp"
//#include "utils.hpp"

namespace internals {
    unsigned int calc_size();

    unsigned int calc_size(char);

    unsigned int calc_size(int);

    unsigned int calc_size(unsigned int);

    unsigned int calc_size(const std::string &a);

    template<typename... Rest>
    unsigned int calc_size(const unsigned int len, unsigned char *const, const Rest... rest);

    //needs to be under the declaration of specializations
    template<typename T, typename... Rest>
    unsigned int calc_size(const T t, const Rest... rest);

    unsigned int pack(unsigned char *);

    unsigned int pack(unsigned char *result, char a);

    unsigned int pack(unsigned char *result, int a);

    unsigned int pack(unsigned char *result, unsigned int a);

    void pack(unsigned char *result, float a);

    unsigned int pack(unsigned char *result, const std::string &str);

    template<typename... Rest>
    unsigned int
    pack(unsigned char *result, unsigned int len, unsigned char *content, const Rest... rest);

    template<typename T, typename... Rest>
    unsigned int pack(unsigned char *result, const T t, const Rest... rest);

    unsigned char *unpack(unsigned char *result, char &a);

    unsigned char *unpack(unsigned char *result, int &a);

    unsigned char *unpack(unsigned char *result, unsigned int &a);

    unsigned char *unpack(unsigned char *result, std::string &a);

    template<typename T, typename... Rest>
    unsigned char *unpack(unsigned char *result, T &t, Rest &... rest) {
        unsigned char *new_result = unpack(result, t);
        return unpack(new_result, rest...);
    }
}

#include "packing_template_impl.hpp"

namespace utils {
    /**Pack data based on the type passed
     *
     * for associated sizes for different types see internals::calc_size
     * unsigned char* that should be directly copied should be prepended with its length (unsigned int)!
     * (32-bit unsigned length is automatically prepended to strings)
     *
     * @tparam T
     * @tparam Rest
     * @param result
     * @param t
     * @param rest
     * @return
     */
    template<typename T, typename... Rest>
    unsigned int pack(BytePtr &result, const T t, const Rest... rest) {
        unsigned int size = internals::calc_size(t, rest...);
        result = BytePtr(new unsigned char[size]);
        return internals::pack(result.get(), t, rest...);
    }

    /**Unpack data from result based on the type passed and store them in the given reference
     *
     * assuming 32-bit unsigned length is prepended for strings
     *
     * @tparam T
     * @tparam Rest
     * @param result
     * @param t
     * @param rest
     * @return pointing to the next byte after the processed ones
     */
    template<typename T, typename... Rest>
    unsigned char *unpack(unsigned char *result, T &t, Rest &... rest) {
        return internals::unpack(result, t, rest...);
    }
}


#endif //SERVER_PACKING_HPP
