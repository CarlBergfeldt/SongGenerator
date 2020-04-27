#pragma once

//#include "pch.h"
#include <iostream>
#include <string>

#include "Chord.h"
#include "Lyrics.h"
#include "Song.h"
#include "SongPart.h"

int main()
{
  // Lambda test
  auto isChord = [=](Note n, ChordQuality q, Chord c) {return n == c.GetRoot() && q == c.GetQuality(); };

  Chord chord = Chord(Note::C, ChordQuality::Major);

  bool isCMaj = isChord(Note::C, ChordQuality::Major, chord);

  // Slipknot - Snuff
  std::vector<Chord *> chordProgression = {
  new Chord(Note::A, ChordQuality::Minor),
  new Chord(Note::E, ChordQuality::Minor),
  new Chord(Note::F, ChordQuality::Major),
  new Chord(Note::E, ChordQuality::Minor)
  };

  std::string lyrics = "This is not an exit.";
  std::string lyrics2 = "Not this \n Either.";

  SongPart intro = SongPart(&chordProgression, lyrics);
  intro.SetTimeSignature(4, 4); 
  intro.SetNumberOfBars(8);

  Song song = Song({ &intro, new SongPart(&chordProgression, lyrics2) });


  std::cout << song.ToString() << "\n";
}

// Ideas
// 1. Transpose