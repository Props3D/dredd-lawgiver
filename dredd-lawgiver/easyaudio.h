#ifndef easyaudio_h
#define easyaudio_h

#define MINI_BAUD_RATE 9600
#define PRO_BAUD_RATE 115200
// uncomment if you are using the DFPlayer Pro
//#define ENABLE_EASY_AUDIO_PRO 1

#include <SoftwareSerial.h>
#ifdef ENABLE_EASY_AUDIO_PRO
#include "dfplayer_pro.h"
#else
#include "dfplayer_mini.h"
#endif

/**
 * EasyAudio is based on DF Player Mini, and provides simple setup, and easy track playback.
 * 
 * Constructor takes rx and tx pins as inputs, but will default to 0 and 1.
 * eg: EasyAudio audio(0, 1);
 * 
 * Call the begin function to initialize the serial comms, and set the volume.
 * Volume range is 0 - 30, default is 25.
 * eg.audio.begin(15);
 * 
 * Playback is by track index. The track index is determined by the order in which 
 * the files are loaded/copied onto the SD card.
 * eg. audio.playTrackNow(1);
 * 
 * When using in main loop, better to queue tracks for playback and eliminate using delays.
 * eg. audio.queuePlayback(1);
 * 
 * In the main loop, playback the next queued track:
 * eg. audio.playQueuedTrack();
 */
class EasyAudio {
private:
  SoftwareSerial _mySerial;
#ifdef ENABLE_EASY_AUDIO_PRO
  DFPlayerPro _player;
#else
  DFPlayerMini _player;
#endif

  long lastPlaybackTime = 0;
  const uint16_t _playbackDelay = 100;

public:
  EasyAudio(uint8_t rxPin, uint8_t txPin)
    : _mySerial(rxPin, txPin){};

  void begin(uint8_t vol) {
#if ENABLE_EASY_AUDIO == 1
    DBGLN(F("setup audio"));
#ifdef ENABLE_EASY_AUDIO_PRO
    _mySerial.begin(PRO_BAUD_RATE);
    while (!_player.begin(_mySerial)) {
      DBGLN(F("DFPlayer failed"));
      delay(3000);
    }
    _player.setVolume(vol);    // initial volume, 30 is max, 25 makes the wife not angry
    _player.musicMode();       // Enter music mode
    _player.singlePlayMode();  // Set playback mode to Play single and pause
    _player.enableAMP();       // Enable amplifier chip
#else
    _mySerial.begin(MINI_BAUD_RATE);
    _player.begin(_mySerial, true);  //set Serial for DFPlayer-mini mp3 module
    _player.volume(vol);             //initial volume, 30 is max, 3 makes the wife not angry
#endif
    delay(450);
#endif
  }

  /**
   * Poor version of checking playback instead of adding delays.
   * THe proper solution would be to check whether the component is busy.
   * Our wiring doesn't support it at the moment
   */
  bool isBusy() {
    return millis() < (lastPlaybackTime + _playbackDelay);
  }

  void playTrack(int track) {
#if ENABLE_EASY_AUDIO == 1
    lastPlaybackTime = millis();
#ifdef ENABLE_EASY_AUDIO_PRO
    _player.playFileNum(track);
#else
    _player.playFromMP3Folder(track);
#endif
#endif
  }

  void playTrack(int track, bool wait) {
#if ENABLE_EASY_AUDIO == 1
    lastPlaybackTime = millis();
#ifdef ENABLE_EASY_AUDIO_PRO
    _player.playFileNum(track);
#else
    _player.playFromMP3Folder(track);
#endif
    if (wait) delay(_playbackDelay);
#endif
  }
};
#endif
