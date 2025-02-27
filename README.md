# Binfinite

## About

Binfinite is in super early alpha. It allows you to connect to dedicated servers over the internet with support for modifying the tickrate and a few other things. This isn't for the faint of heart right now. It's not pretty, it's more of a research project at the moment rather than a product


## Build

This might not build in its current state. This git repo is just an archive at the moment lol

```
premake5 --copy-to="C:\Program Files (x86)\Steam\steamapps\common\Halo Infinite\game" vs2022
```

## Usage

Just launch the game without EAC and inject or patch halo's exe to load client.dll


launch the  server using -console -lan -server


Currently the servers connect to binfinitemaster.lh2.au this code can be found in main.cpp, Its sorta a cod master server.





## Credits

Todo update attribs 

To do but there are a lot of contributions. UniversalHook, InfiniteExt and a few more.