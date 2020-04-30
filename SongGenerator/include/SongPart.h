#pragma once
#include <map>
#include <stdint.h>
#include <vector>

#include "Chord.h"
#include "Lyrics.h"
#include "TimeSignature.h"

class SongPart
{


public:
  SongPart();

  SongPart(std::vector<Chord *> * chordProgression, std::string lyrics);

  ~SongPart();

  void InitializeChordPositions(bool isRandomizerOn);

  void InitializeLyricsPositions(bool isRandomizerOn);

  bool SetNoteResolution(uint32_t number);

  bool SetNumberOfBars(uint32_t number);

  bool SetTimeSignature(uint32_t upper, uint32_t lower);

  std::string ToString();

private:
  uint32_t DefaultNoteResolution = 8;

  uint32_t DefaultNumberOfBars = 8;

  TimeSignature DefaultTimeSignature = { 4, 4 };

  uint32_t MaxCharactersPerLine = 180;

  uint32_t MaxNoteResolution = 64;

  uint32_t MaxNumberOfBars = 64;

  uint32_t MaxTimeSignatureValue = 16;

private:
  std::vector<Chord *> chordProgression;

  //TODO: Create a map with both Chords and Lyrics...or introduce a class 'Bar'
  std::map<uint32_t, Chord *> chordPositions;

  Lyrics lyrics;

  std::map<uint32_t, std::string> lyricsPositions;

  uint32_t noteResolution = DefaultNoteResolution;

  uint32_t numberOfBars = DefaultNumberOfBars;

  TimeSignature timeSignature = DefaultTimeSignature;

};

