#include <iostream>  //C++ "replacement" for stdio.h not only wrapper like cstdio
#include <unistd.h>  //unix standard header, provides variables for getopt
#include <getopt.h>
//#include <string>
#include "constants.hpp"


using std::cout;
using std::endl;

// use static?
static int verbose_flag = 0; //flag set by '--verbose'
static int semantic = 0;
static double failure_rate = 0;


int main(int argc, char **argv) {
    int c;

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
                    cout << "[invalid argument] option '-a' (used for invocation semantics)" << endl;
                    cout << "use 'at least' semantic!" << endl;
                }
                break;
            case 'f':
                try {
                    failure_rate = std::stod(option_arg, nullptr);
                    if (failure_rate < 0 || failure_rate > 1) {
                        cout << "[invalid argument] option '-f': failure_rate must be in [0,1]" << endl;
                        failure_rate_conv_failed = true;
                    }
                } catch (const std::invalid_argument &e) {
                    cout << "[invalid argument] option '-f': no conversion could be performed" << endl;
                    failure_rate_conv_failed=true;
                } catch (const std::out_of_range &e) {
                    cout << "[invalid argument] option '-f': value read is out "
                            "of the range of representable values by a double" << endl;
                    failure_rate_conv_failed=true;
                }
                if (failure_rate_conv_failed) {
                    cout << "use default failure-rate = 0" << endl;
                    failure_rate = 0;
                }
                break;
            case 'h':
                cout << "Print help text here" << endl; //TODO generate help text
                break;
            case 'p':
                cout << "Set port number" << endl; //TODO handle port number
            case 'v':
                verbose_flag = 1;
            case '?':
                // getopt() returns '?' if option character is not recognized
                // automatically prints error message to stderr
                // character is stored in optopt
                return 1;
            default:
                abort();
        }
    }

    cout << std::endl << "Summary: " << std::endl;
    cout << "verbose_flag = " << verbose_flag << ", semantic = " << semantic
         << ", failure_rate = " << failure_rate << std::endl;

    /* Print any remaining command line arguments (not options). */
    for (int index = optind; index < argc; ++index)
        std::cout << "Non-option argument " << argv[index] << std::endl;

    return 0;
}