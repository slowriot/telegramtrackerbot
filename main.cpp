#include <iostream>
#include <args.hxx>
#include "version.h"

auto main(int argc, char *argv[])->int {
  auto get_version = []{
    std::string version(AutoVersion::STATUS);
    if(version.length() != 0) {
      version += " ";
    }
    version += AutoVersion::FULLVERSION_STRING;
    return version;
  };

  args::ArgumentParser arg_parser("Telegram Tracker Bot acts as an interface between Telegram and a GPS tracker unit.", get_version());
  args::HelpFlag help(arg_parser, "help", "Display this help information.", {'h', "help"});
  args::Flag version(arg_parser, "version", "Show version information and exit.", {"version"});
  args::Flag verbose(arg_parser, "verbose", "Show verbose output.", {'v', "verbose"});
  args::ValueFlag<std::string> flag_token(arg_parser, "token", "The token the bot should use to communicate with Telegram.", {'t', "token"});

  try {
    arg_parser.ParseCLI(argc, argv);

    if(version) {
      std::cout << get_version() << std::endl;
      return EXIT_SUCCESS;
    }

    std::string token;
    if(flag_token) {
      token = args::get(flag_token);
    } else {
      std::cout << "Please supply a bot token on the commandline using --token." << std::endl;
      return EXIT_FAILURE;
    }

    if(verbose) {
      std::cout << "Using bot token " << token << std::endl;
    }

    // TODO

    return EXIT_SUCCESS;
  } catch(args::Help) {
    std::cout << arg_parser;
    return EXIT_SUCCESS;
  } catch(args::ParseError e) {
    std::cout << "Argument parsing error: " << e.what() << std::endl;
    std::cout << arg_parser;
    return EXIT_FAILURE;
  } catch(args::ValidationError e) {
    std::cout << "Argument validation error: " << e.what() << std::endl;
    std::cout << arg_parser;
    return EXIT_FAILURE;
  } catch(std::exception const &e)  {
    std::cout << "Exception: " << e.what() << std::endl;
  } catch(...) {
    std::cout << "Unknown exception!" << std::endl;
  }
  return EXIT_FAILURE;
};
