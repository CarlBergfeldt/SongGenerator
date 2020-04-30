#pragma once

#include <string>

enum class ChordQuality { Major, Minor };
enum class Note { C = 1, Db = 2, D = 3, Eb = 4, E = 5, F = 6, Gb = 7, G = 8, Ab = 9, A = 10, Bb = 11, B = 12 };

class Chord
{
public:
  Chord(Note root, ChordQuality quality);

  ~Chord();

  ChordQuality GetQuality() { return quality; };

  Note GetRoot() { return root; };
    
  std::string ToString();

private:
  ChordQuality quality;
  Note root;


};

