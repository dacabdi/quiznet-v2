#include <iostream>
#include <string>
#include <stdexcept>

#include <stdlib.h>     /* for exit         */
#include <unistd.h>     /* for getopt       */
#include <getopt.h>     /* for getopt_long  */

#define NON_OPTIONAL_PARAMS 2

namespace constants {
    static const char        *optString = ":nh"; // options's string
    static const std::string  usage(
    "USAGE  : qclient [OPTIONS] TARGETHOST [OPTIONS] PORT [OPTIONS]\n"
    "         Please notice that unless POSIXLY_CORRECT is set,\n"
    "         options will be taken in any order.\n\n"
    "DESC   : qclient runs the QuizNet client pointing\n"
    "         to server TARGETHOST:PORT\n\n"
    "OPTIONS:\n"
    " -n --nonpersistent       run in non-persistent connection mode\n"
    " -h --help                display usage help and exit\n");
}

struct {
    // program flags and parameters
    int         flagDisplayHelp   = 0 ;  // display help and exit
    int         flagNonPersistent = 0 ;  // default persistent conn mode
    std::string paramTargetHost   = "";  // server's ip or hostname 
    uint16_t    paramTargetPort   = 0 ;  // server's port   
} globalParams;

static struct option longOptions[] = { // program's long options --option
//  { "optionname"   , argument-req, &flag-to-set                   , v }
    { "nonpersistent", no_argument , &globalParams.flagNonPersistent, 0 },
    { "help"         , no_argument , &globalParams.flagDisplayHelp  , 1 },
    { NULL           , no_argument , NULL                           , 0 }
};

void displayUsage(void)
{
    std::cout << constants::usage << std::flush;
}

uint16_t parsePortNumber(const char * const arg)
{   
    try 
    {
        return (uint16_t)std::stoul(arg);
        // ^ TODO, ensure 16-bits unsigned value (0 to 65535) 
    } catch (const std::exception& e) {
        std::cerr << "ERROR: PORT parameter is not a valid port number: " 
                  << arg;
        std::cerr << std::endl;

        displayUsage();
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char * const argv[])
{
    // get options
    int currentOption = 0;
    int optionIndex   = 0;      // set by getopt_long
    
    opterr = 0;

    while(currentOption != -1)  // getopt returns -1 if no options left
    {   
        // save the current optind,
        // this will be used to
        // determine if getopt found
        // a valid option or not,
        // based on whether it moved
        int temp_optin = optind;
        bool optind_moved = false;
        
        // get the next option...
        currentOption = getopt_long(argc, argv, constants::optString,
                                    longOptions, &optionIndex);
        
        // check if it moved
        optind_moved = (optind != temp_optin);
        
        // [optind]     for current index         (getopt)
        // [nextchar]   for next character        (getopt)
        // [optarg*]    if the current option     (getopt)
        //              requires an argument,
        //              getopt sets a pointer to
        //              it in optarg

        switch(currentOption)
        {
            case '?' : 
                
                std::cerr << "ERROR  : Cannot recognize option. ";
                std::cerr << "(" 
                          << argv[(optind_moved ? optind-1 : optind)] 
                          << ")\n" << std::endl;

                displayUsage();
                exit(EXIT_FAILURE);

            break;

            case ':' :
                // missing argument
            break; 

            case 'n' : // also --nonpersistent
                // set to non persistent connection mode
                globalParams.flagNonPersistent = 1;
            break;

            case 'h' : // help
                displayUsage();
                exit(EXIT_SUCCESS);
            break;

            case 0 : // for long options
                     // with NULL flags

                if(globalParams.flagDisplayHelp)
                {
                    displayUsage();
                    exit(EXIT_SUCCESS);
                }

            break;
        }
    }

    // since getopt permuted the non-option
    // arguments to the end, we can read them
    // after the last option indices
    if (optind > (argc - NON_OPTIONAL_PARAMS))
    {
        std::cerr << "ERROR  : Missing target server arguments.\n" 
                  << std::endl;

        displayUsage();

        exit(EXIT_FAILURE);
    }

    // get host (must be a hostname or ip-address, ipv4)
    globalParams.paramTargetHost = argv[optind++];
    // get port (must be a 16 bits number, TODO: enforce uint16_t)
    globalParams.paramTargetPort = parsePortNumber(argv[optind]);

    exit(EXIT_SUCCESS);
}