# SID Player

__Sound Interface Device (SID)__ is a audio format used by the __Commodore 64__ and 128. 

To get some SID files, I can recommend The [High Voltage SID Collection (HVSC)](https://www.hvsc.c64.org/) which is a freeware hobby project that organises Commodore 64 music into an archive for both musicians and fans alike.

This project provides a __SID player__ that can play SID from files or from in memory hex dumps on Arduino. The player is based on the AudioPlayer from the [Arduino Audio Tools](https://github.com/pschatzmann/arduino-audio-tools), so you can use __different audio sources and audio sinks__. Further information can be found [here](https://github.com/pschatzmann/arduino-audio-tools/wiki/The-Audio-Player-Class).


### Installation

For Arduino you can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone pschatzmann/SIDPlayer.git
```

Please do not forget to also install the [AudioTools](https://github.com/pschatzmann/arduino-audio-tools).