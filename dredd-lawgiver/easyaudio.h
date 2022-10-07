#ifndef easyaudio_h
#define easyaudio_h

#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>

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
    DFPlayerMini_Fast _player;
    long lastPlaybackTime            = 0;
    const uint8_t _playbackDelay     = 100;

  public:
    EasyAudio(uint8_t rxPin, uint8_t txPin) : mySerial(rxPin, txPin) {};

    void begin(uint8_t vol) {
#ifdef ENABLE_EASY_AUDIO
      //Serial.println(F("setup audio"));
      mySerial.begin(9600);
      _player.begin(mySerial, 100); //set Serial for DFPlayer-mini mp3 module 
      _player.volume (vol);         //initial volume, 30 is max, 3 makes the wife not angry
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

    void playTrack(uint8_t track) {
#ifdef ENABLE_EASY_AUDIO
      lastPlaybackTime = millis();
      _player.playFromMP3Folder( track );
#endif
    }

    void playTrack(uint8_t track, bool wait) {
#ifdef ENABLE_EASY_AUDIO
      lastPlaybackTime = millis();
      _player.playFromMP3Folder( track );
      if (wait) delay(_playbackDelay);
#endif
    }};

#endif
