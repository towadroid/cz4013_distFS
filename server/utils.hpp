//
// Created by towa-ubuntu on 2/27/20.
//

#ifndef SERVER_UTILS_HPP
#define SERVER_UTILS_HPP

/* I use pack and unpack for marshal and unmarshal respectively
 * because they are shorter to type :)
 *
 * TODO check for IEEE754 and check for sizes
 * if(!std::numeric_limits<float>::is_iec559;) throw std::runtime_error(IEEE754_NONCOMPLIANCE);
 */

namespace  utils{
    void packi32(unsigned  char *buf, const unsigned int x);
    signed int unpacki32(unsigned char *buf);
    unsigned int unpacku32(unsigned char *buf);
}


#endif //SERVER_UTILS_HPP
