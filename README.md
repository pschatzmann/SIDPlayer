# SID Player

In the old days I was rather a Commodere PC guy but I know of colleges of mine that were adicted to the the Commodore 64 and knew it inside out. Here is another project that makes you feel nostalgic: 

__Sound Interface Device (SID)__ is a audio format used by the __Commodore 64__ and 128. 

To get some SID files, I can recommend The [High Voltage SID Collection (HVSC)](https://www.hvsc.c64.org/) which is a freeware hobby project that organises Commodore 64 music into an archive for both musicians and fans alike.

Unfortunately we can not feed SID files incrementally to a codec, so this does not fit with my audio codecs that are used e.g. in the AudioPlayer of the AudioTools library.

Therefore this project provides a __SID player__ that can play SID from files or from in memory hex dumps __on Arduino__. The player is based on the AudioPlayer from the [Arduino Audio Tools](https://github.com/pschatzmann/arduino-audio-tools), so you can use __different audio sources and audio sinks__. The SID emulation is based on cSID light by Hermit (Mihaly Horvath), (Year 2017).

### Documentation

- [Class Documentation](https://pschatzmann.github.io/SIDPlayer/docs/html/annotated.html)
- [AudioTools Wiki](https://github.com/pschatzmann/arduino-audio-tools/wiki)
- [AudioTools Audio Player](https://github.com/pschatzmann/arduino-audio-tools/wiki/The-Audio-Player-Class)
- [Supported Output Devices](https://pschatzmann.github.io/arduino-audio-tools/group__io.html)

### Installation

For Arduino you can download the library as zip and call include Library -> zip library. Or you can git clone this project into the Arduino libraries folder e.g. with

```
cd  ~/Documents/Arduino/libraries
git clone pschatzmann/SIDPlayer.git
```

And please do not forget to also install the [AudioTools](https://github.com/pschatzmann/arduino-audio-tools)!