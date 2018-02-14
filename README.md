# Telegram Tracker Bot
A C++ Telegram bot for interfacing with a GPS tracker, using [LibTelegram](https://github.com/slowriot/libtelegram).

This is a standalone bot intended to be run on your own server with an individual GPS tracker.

**Currently a work in progress.**


## Supported tracker models
* TK103 / TK102B clones using the KX402 protocol.

## Building
A simple `make` should do the trick.  

A Code::Blocks workspace and project are also included for easy building.

## Usage
You'll need a bot token for your bot from the [BotFather](https://telegram.me/BotFather).

* Save the token in the `telegramtrackerbot.conf` config file, or provide it on the commandline with the `--token` option.
* Optionally, set a custom port for the tracking server to bind to.
* The --help commandline option provides help for other options.
* You will then need to get your tracker to talk to your tracking server.

#### Configuring your tracker
The configuration below is for a TK103 / TK102B type tracker; they may need to be adapted to other types.  Send the following SMS commands to your tracker to configure it for gprs mode and point it to your server:
* `reset123456`
* `begin123456`
* `apn123456 <apn name> <apn username> <apn password>`
* `gprs123456`
* `adminip123456 <ip of your server> <port of your server>`
* `tracker123456`

Enable tracking, in the example below with a 30 second interval.  Use m and h for minutes and hours instead:
* t030s***n123456

Minimum tracking period is 20 seconds.

TODO: Twilio integration to allow automatic tracker configuration
