#include <iostream>  //C++ "replacement" for stdio.h not only wrapper like cstdio
#include <getopt.h>
#include "constants.hpp"
#include "utils.hpp"
#include "globalvar.hpp"

using std::cout;
using std::endl;

// use static?
int verbose_flag = 0; //flag set by '--verbose'
int semantic = 0;

int main(int argc, char **argv) {
    int c;
    double failure_rate = 0;

    static struct option long_options[] = {
            /* These options set a flag. */
            {"verbose",      no_argument,       &verbose_flag, 1},
            /* These options don’t set a flag.
             * Can use single character as val if it matches the single character option*/
            {"atleast",      no_argument,       nullptr,       2},
            {"atmost",       no_argument,       nullptr,       3},
            {"failure-rate", required_argument, nullptr,       'f'},
            {"port",         required_argument, nullptr,       'p'},
            {"help",         no_argument,       nullptr,       'h'},
            {nullptr,        0,                 nullptr,       0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;
    std::string option_arg;
    bool failure_rate_conv_failed = false;

    while ((c = getopt_long(argc, argv, "a:f:hp:v", long_options, &option_index)) != -1) {
        if (optarg) option_arg.assign(optarg);
        switch (c) {
            case 0:  //If flag is not a null pointer, flag is set and getopt_long returns 0
                break;
            case 'a':
                if ("l" == option_arg) semantic = ATLEAST;
                else if ("m" == option_arg) semantic = ATMOST;
                else {
                    std::cerr << "[invalid argument] option '-a' (invocation semantics)\n";
                    std::cerr << "    value you provided: " << option_arg << "\n";
                    std::cerr << "    use default value: 'at least' semantic!" << endl;
                }
                break;
            case 'f':
                try {
                    failure_rate = std::stod(option_arg, nullptr);
                    if (failure_rate < 0 || failure_rate > 1) {
                        std::cerr << "[invalid argument] option '-f': failure_rate must be in [0,1]" << option_arg
                                  << "\n";
                        failure_rate_conv_failed = true;
                    }
                } catch (const std::invalid_argument &e) {
                    std::cerr << "[invalid argument] option '-f': no conversion could be performed" << "\n";
                    failure_rate_conv_failed = true;
                } catch (const std::out_of_range &e) {
                    std::cerr << "[invalid argument] option '-f': value read is out "
                                 "of the range of representable values by a double\n";
                    failure_rate_conv_failed = true;
                }
                if (failure_rate_conv_failed) {
                    std::cerr << "    value you provided: " << option_arg << "\n";
                    std::cerr << "    use default value failure_rate = 0" << endl;
                    failure_rate = 0;
                }
                break;
            case 'h':
                cout << "Print help text here" << endl; //TODO generate help text
                break;
            case 'p':
                cout << "Set port number, needs to be implemented" << endl; //TODO handle port number
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case '?':
                // getopt() returns '?' if option character is not recognized
                // automatically prints error message to stderr
                // character is stored in optopt
                return 1;
            default:
                abort();
        }
    }

    if (verbose_flag) {
        cout << "\nSummary:\n" ;
        cout << "verbose_flag = " << verbose_flag << ", semantic = " << semantic
             << ", failure_rate = " << failure_rate << std::endl; //TODO add port number
    }
    if (optind < argc) {
        cout << "[wrong argument] The following arguments were not recognized and are being ignored" << endl;
        /* Print any remaining command line arguments (not options). */
        for (int index = optind; index < argc; ++index)
            std::cout << "    Non-option argument " << argv[index] << std::endl;
    }

    //TODO pass failure rate to udp server

    return 0;
}