#include <iostream>  //C++ "replacement" for stdio.h not only wrapper like cstdio
#include <getopt.h>
#include "constants.hpp"
#include "utils/utils.hpp"
#include "spdlog/spdlog.h"
#include "UdpServer_linux.hpp"
#include "Handler.hpp"
//#include "globalvar.hpp"

using std::cout;
using std::endl;

int semantic = 0;

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
        else spdlog::warn("Invalid option value for \"convert to number\"");
    } catch (const std::invalid_argument &e) {
        std::cerr << "[invalid argument] option '-f': no conversion could be performed" << "\n";
        return false;
    } catch (const std::out_of_range &e) {
        std::cerr << "[invalid argument] option '-f': value read is out "
                     "of the range of representable values by a double\n";
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

int main(int argc, char **argv) {
    spdlog::info("Only ASCII characters are supported!");
    if (!utils::is_expected_size_and_format())
        spdlog::error(
                "This platform's sizes and number formats are unusual and not supported. The application might still work.");
    int c;
    double failure_rate = 0;
    int port_no = 2302;

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
            {"help",         no_argument,       nullptr, 'h'},
            {nullptr, 0,                        nullptr, 0}
    };

    /* getopt_long stores the option index here. */
    int option_index = 0;
    std::string option_arg;
    bool conv_successful = false;

    while ((c = getopt_long(argc, argv, "a:f:hp:v", long_options, &option_index)) != -1) {
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
                    conv_successful = check_range(failure_rate, (double) 0, (double) 1, "-f", "failure_rate");
                if (!conv_successful) {
                    spdlog::warn("Value you provided: \"{}\". Using default value failure_rate = 0 instead",
                                 option_arg);
                    failure_rate = 0;
                }
                break;
            case 'h':
                cout << "Print help text here" << endl; //TODO generate help text
                break;
            case 'p':
                conv_successful = convert_to_number(option_arg, 2, &port_no);
                if (conv_successful) conv_successful = check_range(port_no, 1024, 49151, "-p", "port_no");
                if (!conv_successful) {
                    spdlog::warn("Value you provided: \"{}\". Using default value port_no = 2302 instead", option_arg);
                    port_no = 2302;
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
                else spdlog::warn("Value you provided: \"{}\". Using default value log-level = 2 instead", option_arg);
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
                 "log-level = {}, semantic = {}, failure_rate = {}, port number = {}",
                 spdlog::default_logger()->level(),
                 semantic, failure_rate, port_no);


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

    UdpServer_linux server{port_no, failure_rate};
    Handler handler{};

    while (true) {
        handler.receive_handle_message(server, semantic);
    }

    return 0;
}