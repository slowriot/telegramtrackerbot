#include <iostream>
#include <libtelegram/libtelegram.h>
#include <args.hxx>
#include <INIReader.h>
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

  std::string config_filename{"telegramtrackerbot.conf"};                       // default config file name
  std::string const token_default{"123456:ABC-DEF1234ghIkl-zyx57W2v1u123ew11"}; // telegram's official example token
  std::string token{token_default};
  uint16_t port{5001};                                                          // default port

  args::ArgumentParser arg_parser("Telegram Tracker Bot acts as an interface between Telegram and a GPS tracker unit.", get_version());
  args::HelpFlag help(arg_parser, "help", "Display this help information.", {'h', "help"});
  args::Flag version(arg_parser, "version", "Show version information and exit.", {"version"});
  args::Flag verbose(arg_parser, "verbose", "Show verbose output.", {'v', "verbose"});
  args::ValueFlag<std::string> flag_token(arg_parser, "token", "The token the bot should use to communicate with Telegram.", {'t', "token"});
  args::ValueFlag<uint16_t> flag_port(arg_parser, "port", "The TCP port to listen for the tracker on.", {'p', "port"});
  args::ValueFlag<std::string> flag_config(arg_parser, "config", "Load a configuration from a config file other than the default " + config_filename + ".", {'c', "config"});

  try {
    arg_parser.ParseCLI(argc, argv);

    if(version) {
      std::cout << get_version() << std::endl;
      return EXIT_SUCCESS;
    }
    if(flag_config) {
      config_filename = args::get(flag_config);                                 // read the config file option before trying to load a config file
    }
    if(verbose) {
      std::cout << "Reading configuration from " << config_filename << "..." << std::endl;
    }
    INIReader config_file(config_filename);
    if(config_file.ParseError() >= 0) {
      token = config_file.Get("", "token", token);
      port = config_file.GetInteger("", "port", port);
      if(verbose) {
        std::cout << "Successfully loaded configuration from " << config_filename << "." << std::endl;
      }
    } else {
      if(verbose) {
        std::cout << "Failed to load " << config_filename << "." << std::endl;
      }
    }
    if(flag_token) {
      token = args::get(flag_token);                                            // if we've provided a token to overwrite the original, replace it here
    }
    if(flag_port) {
      port = args::get(flag_port);                                              // if we've provided a token to overwrite the original, replace it here
    }
    if(verbose) {
      std::cout << "Using bot token" << token;
      if(token == token_default) {
        std::cout << " (this is the default token, and must be changed)";
      }
      std::cout << "." << std::endl;
      std::cout << "Listening on TCP port " << port << std::endl;;
    }
    if(token.empty() || token == token_default) {
      std::cerr << "Please supply a valid bot token in the configuration file " << config_filename << ", or on the commandline using --token." << std::endl;
      return EXIT_FAILURE;
    }

    if(verbose) {
      std::cout << "Launching Telegram polling listener" << std::endl;
    }
    telegram::sender sender(token);                                             // initialise the telegram sender with our token
    telegram::listener::poll listener(sender);                                  // create a polling listener

    int_fast64_t announce_chat_id = 0;

    listener.set_callback_message([&](telegram::types::message const &message){
      auto const message_chat_id = message.chat.id;
      if(message.text == "/start") {
        sender.send_message(message_chat_id, "I am online.  You will now receive all tracker updates.");
        #ifndef NDEBUG
          sender.send_message(message_chat_id, "DEBUG: Chat ID is " + std::to_string(message_chat_id));
          sender.send_message(message_chat_id, "DEBUG: Listening TCP port is " + std::to_string(port));
        #endif // NDEBUG
        announce_chat_id = message_chat_id;
      }
    });

    listener.set_num_threads(1);                                                // run single-threaded
    listener.run();                                                             // launch the Telegram listener - this call blocks until interrupted
    #ifndef NDEBUG
      if(announce_chat_id != 0) {
        sender.send_message(announce_chat_id, "DEBUG: Terminating.");
      }
    #endif // NDEBUG
    if(verbose) {
      std::cout << "Telegram polling loop completed." << std::endl;
    }

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
