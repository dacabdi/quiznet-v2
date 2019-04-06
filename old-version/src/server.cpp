#include "Exception.h"
#include "Host.h"
#include "QuizBook.h"
#include "QuizServer.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>

#include <stdlib.h>     /* for exit         */
#include <unistd.h>     /* for getopt       */
#include <getopt.h>     /* for getopt_long  */

#define NON_OPTIONAL_PARAMS 0

namespace constants {
    static const char        *optString = ":p:f:l:Vvh"; // options's string
    static const std::string  usage(
    "USAGE  : qserver [OPTIONS]\n"
    "         Please notice that unless POSIXLY_CORRECT is set,\n"
    "         options will be taken in any order.\n\n"
    "DESC   : qserver runs the QuizNet server on a port\n"
    "         assigned by the OS or specified by the user using\n"
    "         the option -p PORT (--port=PORT). Once the server\n"
    "         is up, it will report the port it is bound to, use\n"
    "         this value for client's reference\n\n"
    "OPTIONS:\n\n"
    "  -p --port=PORT                port to run on, 0 by default to let\n"
    "                                the OS randomly assign a free port\n"
    "  -f PATH --filename=PATH       file to use as questions repositor,\n"
    "                                the file will be created if it doesn't\n"
    "                                exists. default PATH=./\"qbank.qnb\"\n"
    "  -l (yes|no) --log=(yes|no)    log to stdout server's activity. this\n"
    "                                option is \"yes\" by default.\n"
    "  -V --verbose                  the server will log requests and responses\n"
    "                                to stdout, must be used in conjunction\n"
    "                                with -l=yes (--log=yes), disabled by default\n"
    "  -h --help                     display usage help and exit\n");
    static const std::string welcome(
    "\n\n/==============================================\\\n"
        "|              QuizNet Server v1.0             |\n"
       "\\==============================================/\n");
    static const std::string version(
        "qserver v1.0\n"
        "QuizNet Server Application\n"
        "Written by David Cabrera @ dacabdi89@ufl.edu\n"
        "University of Florida, CISE Department\n"
        "Networking Fundamentals, Spring 2019\n"
        "Project 1 (server application)\n"
    );
}

struct {
    // program flags and parameters
    int         flagDisplayHelp   = 0                  ;  // display help and exit
    int         flagDisplayVer    = 0                  ;  // display version and exit
    int         flagLog           = 1                  ;  // log activity
    int         flagVerbose       = 0                  ;  // verbose (display req and res)
    std::string paramRepoFilename = "qserver-repo.data";  // quiz book file
    uint16_t    paramPort         = 0 ;                   // port to run server
} globalParams;

static struct option longOptions[] = { // program's long options --option
//  { "option"   , argument-req      , &flag-to-set                  , v }
    { "port"     , required_argument , NULL                          , 1 },
    { "filename" , required_argument , NULL                          , 1 },
    { "log"      , required_argument , NULL                          , 1 },
    { "verbose"  , no_argument       , &globalParams.flagVerbose     , 1 },
    { "version"  , no_argument       , &globalParams.flagDisplayVer  , 1 },
    { "help"     , no_argument       , &globalParams.flagDisplayHelp , 1 },
    { NULL       , no_argument       , NULL                          , 0 }
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

    // port parameter
    std::string paramPortString = 
    (globalParams.paramPort == 0 ? "Any" : 
    std::to_string(globalParams.paramPort));

    oss 
    << "\n\n------------ Initialization settings" 
    <<     "-----------\n\n"
    << "\tPort       : "   << paramPortString << "\n"
    << "\tRepository : "   << globalParams.paramRepoFilename << "\n"
    << "\tLog        : "   << (globalParams.flagLog ? "Yes" : "No") << "\n"
    << "\tVerbose    : "   << (globalParams.flagVerbose ? "Yes" : "No") << "\n"
    << std::flush;

    return oss.str();
}

uint16_t parsePortNumber(const char * const arg)
{   
    try 
    {
        return (uint16_t)std::stoul(arg);
        // ^ TODO, ensure 16-bits unsigned value (0 to 65535) 
    } catch (const std::exception& e) {
        std::cerr << "ERROR  : PORT parameter is not a valid port number: " 
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
                
                std::cerr << "ERROR  : Cannot recognize option : ";
                std::cerr << "(" 
                          << argv[(optind_moved ? optind-1 : optind)] 
                          << ")\n" << std::endl;

                displayUsage();
                exit(EXIT_FAILURE);

            break;

            case ':' :

                // missing argument
                std::cerr << "ERROR  : Missing argument for option : ";
                std::cerr << "(" << argv[optind-1] << ")\n" << std::endl;
                displayUsage();
                exit(EXIT_FAILURE);

            break; 

            case 'p' :

                globalParams.paramPort = parsePortNumber(optarg);

            break;

            case 'f' :

                globalParams.paramRepoFilename = optarg;

            break;

            case 'l' :

                globalParams.flagLog = (strcmp("yes", optarg) == 0 ? 1 : 0);

            break;

            case 'V' :

                globalParams.flagVerbose = 1;

            break;

            case 'h' : // help

                displayUsage();
                exit(EXIT_SUCCESS);
            
            break;

            case 1 : 

                // long options with NULL flag
                if(        strcmp("port"    , longOptions[optionIndex].name ) == 0)
                    globalParams.paramPort = parsePortNumber(optarg);
                else if (  strcmp("filename", longOptions[optionIndex].name ) == 0)
                    globalParams.paramRepoFilename = optarg;
                else if (  strcmp("log"     , longOptions[optionIndex].name ) == 0)
                    globalParams.flagLog = (strcmp("yes", optarg) == 0 ? 1 : 0);
            
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

    std::cout << constants::welcome << std::endl;
    std::cout << displayInitSettings() << std::endl;

    QuizBook quizbook;
    Host host(std::to_string(globalParams.paramPort));
    Socket socket(IPv4, StreamSocket, TCP);
    
    QuizServer server(
        &quizbook, 
        &host, 
        &socket, 
        globalParams.paramRepoFilename
    );

    server.setLoggerVerbose(globalParams.flagVerbose);
    
    if(globalParams.flagLog) 
    {
        server.setLogger([&](const std::string& str){
            std::cout << str << std::flush;
        });
    }

    // now RUN!
    server.run();

    // be polite
    std::cout << "\nServer out!\nBh-Bh-Bye! :( \n" << std::endl;

    exit(EXIT_SUCCESS);
}