//
// Created by towa-ubuntu on 3/18/20.
//

#ifndef SERVER_PACKING_TEMPLATE_IMPL_HPP
#define SERVER_PACKING_TEMPLATE_IMPL_HPP

namespace internals {
    template<typename... Rest>
    unsigned int calc_size(const unsigned int len, unsigned char *const, const Rest... rest) {
        return len + calc_size(rest...);
    }

//needs to be under the declaration of specializations
    template<typename T, typename... Rest>
    unsigned int calc_size(const T t, const Rest... rest) {
        return calc_size(t) + calc_size(rest...);
    }

    template<typename... Rest>
    unsigned int
    pack(unsigned char *result, const unsigned int len, unsigned char *const content, const Rest... rest) {
        unsigned int elem_size = len;
        memcpy(result, content, len);
        unsigned int rest_size = pack(result + elem_size, rest...);
        return elem_size + rest_size;
    }

    template<typename T, typename... Rest>
    unsigned int pack(unsigned char *result, const T t, const Rest... rest) {
        unsigned int elem_size = pack(result, t);
        unsigned int rest_size = pack(result + elem_size, rest...);
        return elem_size + rest_size;
    }
}


#endif //SERVER_PACKING_TEMPLATE_IMPL_HPP
