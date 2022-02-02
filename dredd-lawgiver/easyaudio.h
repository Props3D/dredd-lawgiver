#ifndef easyaudio_h
#define easyaudio_h

#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include "easyqueue.h"
#include "debug.h"

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
class EasyAudio
{
  private:
    SoftwareSerial mySerial;
    DFPlayerMini_Fast player;
    EasyQueue<uint8_t> tracks;
    const uint8_t playbackDelay = 100;

  public:
    EasyAudio(uint8_t rxPin, uint8_t txPin) : mySerial(rxPin, txPin), tracks(5) {};

    void begin(uint8_t vol) {
#ifdef ENABLE_EASY_AUDIO
      debugLog("setup audio");
      mySerial.begin(9600);
      player.begin(mySerial, 100); //set Serial for DFPlayer-mini mp3 module 
      player.volume (vol);         //initial volume, 30 is max, 3 makes the wife not angry
#endif
    }

    void queuePlayback(uint8_t track) {
#ifdef ENABLE_EASY_AUDIO
      tracks.push(track);
#endif
    }

    void playQueuedTrack() {
#ifdef ENABLE_EASY_AUDIO
      if (!tracks.empty()) {
        debugLog("playing queued track ");
        player.playFromMP3Folder( tracks.pop() ); 
      }
#endif
    }

    void playTrack(uint8_t track) {
#ifdef ENABLE_EASY_AUDIO
      debugLog("playing track ");
      player.playFromMP3Folder( track ); 
      delay(playbackDelay);
#endif
    }
};

#endif
