#include <iostream>  //C++ "replacement" for stdio.h not only wrapper like cstdio
#include <getopt.h>
#include "constants.hpp"
#include "utils/utils.hpp"
#include "spdlog/spdlog.h"
#include "UdpServer_linux.hpp"
#include "Handler.hpp"

using std::cout;
using std::endl;

/**Convert string to number
 *
 * @param x
 * @param option 1 for double, 2 for int
 * @param value
 * @return
 */
bool convert_to_number(std::string x, int option, void *value) {
    try {
        if (1 == option) *((double *) value) = std::stod(x);
        else if (2 == option) *((int *) value) = std::stoi(x);
        else spdlog::warn("[Internal error] Invalid option value for \"convert to number\"");
    } catch (const std::invalid_argument &e) {
        spdlog::warn("[invalid argument] option '-f': no conversion could be performed");
        return false;
    } catch (const std::out_of_range &e) {
        spdlog::warn("[invalid argument] option '-f': value read is out "
                     "of the range of representable values by a double");
        return false;
    }
    return true;
}

/** Check if val is in [lower, upper]
 *
 * @tparam T
 * @param lower
 * @param upper
 * @return
 */
template<class T>
bool check_range(T val, T lower, T upper, std::string opt_name_short = "empty", std::string opt_name = "empty") {
    if (val < lower || val > upper) {
        spdlog::warn("[invalid argument] option '{}': {} must be in [{},{}]", opt_name_short, opt_name, lower, upper);
        return false;
    }
    return true;
}

constexpr char help_text[] = "Distributed File System - server v1.0\n"
                             "This application is the server side application for a distributed file system.\n\n"

                             "Usage:\n"
                             "./server [-am | -al] [-f <FAILURE RATE>] [-p <PORT>] [-v [<LOG LEVEL>]] [-s <SEED>] \n"
                             "Note: Uses default values for options which are not set, see below.\n\n"

                             "Options:\n"
                             "-am, --atmost\t\t\tUse at-most semantics\n"
                             "-al, --atleat\t\t\tUse at-least semantics (default value)\n"
                             "-f, --failure-rate <arg>\tSet failure rate, arg should be floating point in [0,1] (default: 0)\n"
                             "-p, --port <port>\t\tSet port number (default: 2302)\n"
                             "-v [arg], --log-level=[arg]\tSet level of logger, possible values are:\n"
                             "\t\t\t\t\ttrace, debug, info, warn, error, critical, off\n"
                             "\t\t\t\tIf arg not provided, level is set to debug (default: info)\n"
                             "-s, --seed <arg>\t\tSet seed for random number generator used for simulating send failures.\n"
                             "\t\t\t\tUse this option to get reproducable deterministic failure behaviour\n"
                             "\t\t\t\t(default: random)\n"
                             "-h, --help\t\t\tDisplay this help text";

const std::unordered_map<int, std::string> log_level_map = {
        {0, "trace"},
        {1, "debug"},
        {2, "info"},
        {3, "warn"},
        {4, "error"},
        {5, "critical"},
        {6, "off"}
};

const std::unordered_map<int, std::string> semantic_map = {
        {constants::ATMOST,  "at-most"},
        {constants::ATLEAST, "at-least"},
};

int main(int argc, char **argv) {
    spdlog::info("Only ASCII characters are supported!");
    if (!utils::is_expected_size_and_format())
        spdlog::error(
                "This platform's sizes and number formats are unusual and not supported. The application might still work.");
    int c;
    double failure_rate = 0;
    int port_no = 2302;
    int semantic = 0;
    bool truly_random = true;
    int seed = 1;

    static struct option long_options[] = {
            /* These options set a flag. */
            //none in this case
            /* These options don’t set a flag.
             * Can use single character as val if it matches the single character option*/
            {"atleast",      no_argument,       nullptr, 2},
            {"atmost",       no_argument,       nullptr, 3},
            {"failure-rate", required_argument, nullptr, 'f'},
            {"port",         required_argument, nullptr, 'p'},
            {"log-level",    optional_argument, nullptr, 'v'},
            {"seed",         required_argument, nullptr, 's'},
            {"help",         no_argument,       nullptr, 'h'},
            {nullptr, 0,                        nullptr, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;
    std::string option_arg;
    bool conv_successful = false;

    while ((c = getopt_long(argc, argv, "a:f:hp:v::", long_options, &option_index)) != -1) {
        if (optarg) option_arg.assign(optarg);
        switch (c) {
            case 0:  //If flag is not a null pointer, flag is set and getopt_long returns 0
                break;
            case 'a':
                if ("l" == option_arg) semantic = constants::ATLEAST;
                else if ("m" == option_arg) semantic = constants::ATMOST;
                else {
                    spdlog::warn("[invalid argument] option '-a' (invocation semantics)");
                    spdlog::warn("Value you provided: \"{}\". Using default value: 'at least' semantic!", option_arg);
                }
                break;
            case 'f':
                conv_successful = convert_to_number(option_arg, 1, &failure_rate);
                if (conv_successful && (failure_rate < 0 || failure_rate > 1))
                    conv_successful = check_range(failure_rate, (double) 0, (double) 1, "-f", "FAILURE RATE");
                if (!conv_successful) {
                    spdlog::warn("Value you provided: \"{}\". Using default value failure_rate = 0 instead",
                                 option_arg);
                    failure_rate = 0;
                }
                break;
            case 'h':
                cout << help_text << endl;
                return 0;
            case 'p':
                conv_successful = convert_to_number(option_arg, 2, &port_no);
                if (conv_successful) conv_successful = check_range(port_no, 1024, 49151, "-p", "PORT");
                if (!conv_successful) {
                    spdlog::warn("Value you provided: \"{}\". Using default value port_no = 2302 instead", option_arg);
                    port_no = 2302;
                }
                break;
            case 's':
                truly_random = false;
                conv_successful = convert_to_number(option_arg, 2, &seed);
                if (conv_successful) conv_successful = check_range(port_no, 0, 2147483647, "-s", "SEED");
                if (!conv_successful) {
                    spdlog::warn("Value you provided: \"{}\". Using seed = 1 instead", option_arg);
                    seed = 1;
                }
                break;
            case 'v':
                if (nullptr == optarg) spdlog::set_level(spdlog::level::debug);
                else if ("trace" == option_arg) spdlog::set_level(spdlog::level::trace);
                else if ("debug" == option_arg) spdlog::set_level(spdlog::level::debug);
                else if ("info" == option_arg) spdlog::set_level(spdlog::level::info);
                else if ("warn" == option_arg) spdlog::set_level(spdlog::level::warn);
                else if ("error" == option_arg) spdlog::set_level(spdlog::level::err);
                else if ("critical" == option_arg) spdlog::set_level(spdlog::level::critical);
                else if ("off" == option_arg) spdlog::set_level(spdlog::level::off);
                else
                    spdlog::warn("Value you provided: \"{}\". Using default value log-level = info instead",
                                 option_arg);
                break;
            case 2:
                semantic = constants::ATLEAST;
                break;
            case 3:
                semantic = constants::ATMOST;
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

    spdlog::info("Summary:\n"
                 "\tlog-level = {}, semantic = {}, failure_rate = {}, port number = {}",
                 log_level_map.at(spdlog::default_logger()->level()),
                 semantic_map.at(semantic), failure_rate, port_no);


    if (optind < argc) {
        std::string ignored_args{};
        for (int index = optind; index < argc; ++index) {
            ignored_args.append("Non-option argument ");
            ignored_args.append(argv[index]);
            ignored_args.append("\n");
        }
        spdlog::warn("[wrong argument] The following arguments were not recognized and are being ignored\n{}",
                     ignored_args);
        /* Print any remaining command line arguments (not options). */

    }

    UdpServer_linux server{port_no, failure_rate, (unsigned int) seed, truly_random};
    Handler handler{};

    while (true) {
        handler.receive_handle_message(server, semantic);
    }

    return 0;
}