# Setup
- [DPP dependency](https://dpp.dev/buildcmake.html)
- Install dependencies (in the libs folder there is a `clone.sh` script to run, you can also just clone the DPP repository under `libs`)
- `mkdir -p build && cd build`
- Install CMake and do `cmake ..`, `cmake --build .`

# Running
- Register for an application on Discord and use these bot permissions:
  ![](res/img/bot_permissions.png)
- The bot must also have relevant `intent` set for reading message content (*this is only for examining the content of messages from webhooks created by the bot itself*).
- Make sure you filled the environment variables with your bot (see `env.h.template`)
- Build and run `./p4dc` under the `build` folder