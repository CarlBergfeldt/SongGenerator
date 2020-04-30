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
  std::vector<Chord *> chordProgressionVerse = {
  new Chord(Note::A, ChordQuality::Minor),
  new Chord(Note::E, ChordQuality::Minor),
  new Chord(Note::F, ChordQuality::Major),
  new Chord(Note::E, ChordQuality::Minor)
  };

  std::vector<Chord *> chordProgressionChorus = {
new Chord(Note::G, ChordQuality::Major),
new Chord(Note::A, ChordQuality::Minor),
new Chord(Note::G, ChordQuality::Major),
new Chord(Note::A, ChordQuality::Minor)
  };

  std::string lyricsVerse1 = "Bury all your secrets in my skin\n Come away with innocence, and leave me with my sins\n "
    "The air around me still feels like a cage\n And love is just a camouflage for what resembles rage again";

  SongPart verse = SongPart(&chordProgressionVerse, lyricsVerse1);
  verse.SetTimeSignature(4, 4);
  verse.SetNumberOfBars(16);

  Song song = Song({ &verse, new SongPart(&chordProgressionChorus, "") });


  std::cout << song.ToString() << "\n";
}

// Ideas
// 1. Transpose