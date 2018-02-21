#include <iostream>
#include <limits>
#include <libtelegram/libtelegram.h>
#include <boost/asio.hpp>
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
    std::thread telegram_thread([&]{
      listener.run();                                                           // launch the Telegram listener - this call blocks until interrupted
    });

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::acceptor acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    for(;;) {
      boost::asio::ip::tcp::socket socket(io_context);
      acceptor.accept(socket);

      for(;;) {
        boost::system::error_code error;
        {
          boost::asio::streambuf buffer;
          boost::asio::read_until(socket, buffer, "(", error);
          if(error == boost::asio::error::eof) {                                // connection closed by peer
            break;
          } else if(error) {
            throw boost::system::system_error(error);
          }
        }
        try {
          std::cout << "DEBUG: read first bracket" << std::endl;
          boost::asio::streambuf buffer;
          boost::asio::read_until(socket, buffer, ")", error);
          if(error == boost::asio::error::eof) {                                // connection closed by peer
            std::cout << "DEBUG: got EOF from client" << std::endl;
            break;
          } else if(error) {
            throw boost::system::system_error(error);
          }

          std::cout << "DEBUG: read buffer to bracket" << std::endl;

          auto read_substr = [](boost::asio::streambuf &buffer, unsigned int digits) {
            std::string buf('\0', digits);
            buffer.sgetn(buf.data(), digits);
            return buf;
          };

          unsigned int serial  = 0;
          serial = std::stoi(read_substr(buffer, 12));
          std::cout << "DEBUG: serial: " << serial << std::endl;

          std::string command = read_substr(buffer, 4);
          std::cout << "DEBUG: command: " << command << std::endl;
          if(command == "BR00") {
            unsigned int year    = 0;
            unsigned int month   = 0;
            unsigned int day     = 0;
            unsigned int hours   = 0;
            unsigned int minutes = 0;
            unsigned int seconds = 0;
            double latitude      = 0.0;
            double longitude     = 0.0;
            double speed         = 0.0;
            double orientation   = 0.0;
            std::string iostate;
            std::string mileage;

            year  = std::stoi(read_substr(buffer, 2));
            month = std::stoi(read_substr(buffer, 2));
            day   = std::stoi(read_substr(buffer, 2));
            std::cout << "DEBUG: year: " << year << std::endl;
            std::cout << "DEBUG: month: " << month << std::endl;
            std::cout << "DEBUG: day: " << day << std::endl;

            char available = '\0';
            buffer.sgetn(&available, 1);
            if(available != 'A') {
              if(available == 'V') {
                std::cout << "DEBUG: no data available" << std::endl;
              } else {
                std::cout << "DEBUG: received nonsense instead of A or V" << std::endl;
              }
              break;
            }

            latitude = std::stod(read_substr(buffer, 9));
            char northsouth = '\0';
            buffer.sgetn(&northsouth, 1);
            if(northsouth != 'N') {
              if(northsouth == 'S') {
                latitude = 0.0 - latitude;
              } else {
                std::cout << "DEBUG: received nonsense instead of N/S heading" << std::endl;
                break;
              }
            }

            longitude = std::stod(read_substr(buffer, 10));
            char eastwest = '\0';
            if(eastwest != 'E') {
              if(eastwest == 'W') {
                longitude = 0.0 - longitude;
              } else {
                std::cout << "DEBUG: received nonsense instead of E/W heading" << std::endl;
                break;
              }
            }
            speed       = std::stod(read_substr(buffer, 5));
            hours       = std::stoi(read_substr(buffer, 2));
            minutes     = std::stoi(read_substr(buffer, 2));
            seconds     = std::stoi(read_substr(buffer, 2));
            orientation = std::stod(read_substr(buffer, 6));
            iostate     =           read_substr(buffer, 8);
            std::cout << "DEBUG: hours: " << hours << std::endl;
            std::cout << "DEBUG: minutes: " << minutes << std::endl;
            std::cout << "DEBUG: seconds: " << seconds << std::endl;
            std::cout << "DEBUG: latitude: " << latitude << std::endl;
            std::cout << "DEBUG: longitude: " << longitude << std::endl;
            std::cout << "DEBUG: speed: " << speed << std::endl;
            std::cout << "DEBUG: orientation: " << orientation << std::endl;
            std::cout << "DEBUG: iostate: " << iostate << std::endl;

            char milepost = '\0';
            buffer.sgetn(&milepost, 1);
            if(milepost != 'L') {
              std::cout << "DEBUG: received nonsense instead of L milepost" << std::endl;
              break;
            }

            mileage = read_substr(buffer, 8);
            std::cout << "DEBUG: mileage: " << mileage << std::endl;

            boost::asio::write(socket, boost::asio::buffer("OK\n"), error);
          } else {
            // TODO: handle other commands
          }
        } catch(std::exception const &e)  {
          std::cout << "Exception: " << e.what() << std::endl;
        }
      }
    }

    #ifndef NDEBUG
      if(announce_chat_id != 0) {
        sender.send_message(announce_chat_id, "DEBUG: Terminating.");
      }
    #endif // NDEBUG
    listener.stop();
    telegram_thread.join();
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
