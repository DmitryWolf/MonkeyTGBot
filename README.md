## Monkey telegram bot
Telegram bot written in C. The bot processes messages multithreaded using a [threadpool](https://github.com/DmitryWolf/StaticThreadPool_c) written by me.

It was conceived as a bot that sends monkey phrases or stickers with monkeys, but the interface is suitable for any ideas.

The bot looks at the messages that are sent to him/ in conversation with him, searches for the occurrence of bahword in the message and, if found, randomly sends either a random phrase or a random sticker in response to this message.

### Dependencies
First of all, you need to download [Docker](https://docs.docker.com/get-started/get-docker/).

Secondly, too run the program, you need to install libraries [openssl](https://github.com/openssl/openssl) and [jansson](https://github.com/akheron/jansson)
- How to install dependencies on Ubuntu 20.04:
```
$ sudo apt install libssl-dev
$ sudo apt install libjansson-dev
```
-  How to install dependencies on Mac OS via the brew package manager (https://brew.sh/):
```
$ brew install openssl
$ brew install jansson
```

### Usage

#### Parameters
There are several parameters that can be changed:
- The maximum number of banwords (banword.h)
```
#define MAX_BANWORDS 100
```
- The maximum number of possible texts of the bot's responses (monkey.h)
```
#define MAX_MONKEY_LINES 100
```
- Default size of array (monkeybot.h)
```
#define DEFAULT_SIZE 1024
```
- The maximum size of request (monkeybot.h)
```
#define REQUEST_SIZE 2048
```
- The maximum size of response (monkeybot.h)
```
#define RESPONSE_SIZE 8192
```
- The maximum size of the received JSON response (monkeybot.h)
```
#define LIMIT_REQUESTS 100
```
- Path to the token (monkeybot.h)
```
#define PATH_TO_TOKEN "/usr/src/monkeybot/resources/token.txt"
```
- Path to the banwords (monkeybot.h)
```
#define PATH_TO_BANWORDS "/usr/src/monkeybot/resources/banwords.txt"
```
- Path to text answers (monkeybot.h)
```
#define PATH_TO_TEXT_ANSWERS "/usr/src/monkeybot/resources/monkeyanswers.txt"
```
- Path to IDs of stickers (monkeybot.h)
```
#define PATH_TO_STICKERS_ANSWERS "/usr/src/monkeybot/resources/monkeystickers.txt"
```
- The maximum size of workers in Thread Pool (threadpool.h)
```
#define THREAD_NUM 16
```
- The maximum size of a line to be read from a file (tools.h)
```
#define MAX_LINE_LEN 256
```
- Path to requests logs (logger.h)
```
#define REQUEST_PATH "/usr/src/monkeybot/resources/logs/requests.txt"
```
- Path to responses logs (logger.h)
```
#define RESPONSE_PATH "/usr/src/monkeybot/resources/logs/responses.txt"
```
- Path to error logs (logger.h)
```
#define RESPONSE_PATH "/usr/src/monkeybot/resources/logs/errors.txt"
```
- Path to responses logs (logger.h)
```
#define ERROR_PATH "/usr/src/monkeybot/resources/logs/responses.txt"
```
- Path to other logs (logger.h)
```
#define SMTH_PATH "/usr/src/monkeybot/resources/logs/smth.txt"
```
---
#### Files
- The bot token is in the file **(just write token in 1 line)**:
```
resources/token.txt
```
- Banwords are stored in a file **(you should put 1 word in one line)**:
```
resources/banwords.txt
```

- The possible text answers of the bot are in the file **(you should put 1 answer in one line)**:
```
resources/monkeyanswers.txt
```

- The possible sticker answers of the bot are in the file **(you should put 1 sticker id in one line)**:
```
resources/monkeystickers.txt
```

- Logs are stored in
```
resources/logs/
```
---
#### Run
First, make the script executable:
```
$ chmod +x run_docker.sh
```
Secondly, run script (the bot will be launched in the docker):
```
$ ./run_docker.sh
```