#include "Host.h"
#include "ContestantClient.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

#include <stdlib.h>     /* for exit         */
#include <unistd.h>     /* for getopt       */
#include <getopt.h>     /* for getopt_long  */

#define NON_OPTIONAL_PARAMS 2

namespace constants {
    static const char        *optString = ":nhv"; // options's string
    static const std::string  usage(
    "USAGE  : contestant TARGETHOST PORT\n"
    "DESC   : contestant runs the QuizNet contest client\n"
    "         to server TARGETHOST hosting a contest at PORT\n\n"
    "OPTIONS:\n"
    " -h --help                display usage help and exit\n"
    " -v --version             display version and exit\n");
    static const std::string welcome(
    "\n\n/==============================================\\\n"
        "|         QuizNet Contestant Client v2.0       |\n"
       "\\==============================================/\n");
    static const std::string version(
        "contestant v2.0\n"
        "QuizNet Contestant Application\n"
        "Written by David Cabrera @ dacabdi89@ufl.edu\n"
        "University of Florida, CISE Department\n"
        "Networking Fundamentals, Spring 2019\n"
        "Project 2 (contestant application)\n"
    );
}

struct {
    // program flags and parameters
    int         flagDisplayHelp   = 0 ;  // display help and exit
    int         flagDisplayVer    = 0 ;
    int         flagNonPersistent = 0 ;  // default persistent conn mode
    std::string paramTargetHost   = "";  // server's ip or hostname 
    uint16_t    paramTargetPort   = 0 ;  // server's port   
} globalParams;

static struct option longOptions[] = { // program's long options --option
//  { "optionname"   , argument-req, &flag-to-set                   , v }
    { "version"      , no_argument , &globalParams.flagDisplayVer   , 1 },
    { "help"         , no_argument , &globalParams.flagDisplayHelp  , 1 },
    { NULL           , no_argument , NULL                           , 0 }
};

void displayUsage(void)
{
    std::cout << constants::usage << std::flush;
}

void displayVersion(void)
{
    std::cout << constants::version << std::flush;
}

std::string displayInitSettings(void)
{
    std::ostringstream oss;

    oss << "\n\n------------ Initialization settings" 
        <<     "-----------\n" << std::endl;
    
    oss << "\tTarget host : " 
        << globalParams.paramTargetHost << std::endl;
    oss << "\tTarget port : "
        << globalParams.paramTargetPort << std::endl;

    oss << std::endl;

    return oss.str();
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

//--------------------READ-PARAMS-AND-OPTIONS-----------------------------

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

            case 'h' : // help
                displayUsage();
                exit(EXIT_SUCCESS);
            break;

            case 'v' : // version
                displayVersion();
                exit(EXIT_SUCCESS);
            break;

            case 0 : // for long options
                     // with NULL flags

                if(globalParams.flagDisplayHelp)
                {
                    displayUsage();
                    exit(EXIT_SUCCESS);
                } else if (globalParams.flagDisplayVer) {
                    displayVersion();
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

    //--------------------MAIN-PROGRAM----------------------------------------
    
    std::cout << constants::welcome     << std::flush;
    std::cout << displayInitSettings()  << std::flush;

    // build a host object
    Host host(globalParams.paramTargetHost, std::to_string(globalParams.paramTargetPort));

    // create the client object
    ContestantClient contestantClient(host);

    // run the client!
    contestantClient.run();

    exit(EXIT_SUCCESS);
}