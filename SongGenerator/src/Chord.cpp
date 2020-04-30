#include "Chord.h"

Chord::Chord(Note root, ChordQuality quality) :
  root(root),
  quality(quality)
{
}

Chord::~Chord()
{
}

std::string Chord::ToString()
{
  std::string s = "";

  switch (root)
  {
  case Note::C:
    s = "C";
    break;
  case Note::Db:
    s = "Db";
    break;
  case Note::D:
    s = "D";
    break;
  case Note::Eb:
    s = "Eb";
    break;
  case Note::E:
    s = "E";
    break;
  case Note::F:
    s = "F";
    break;
  case Note::Gb:
    s = "Gb";
    break;
  case Note::G:
    s = "G";
    break;
  case Note::Ab:
    s = "Ab";
    break;
  case Note::A:
    s = "A";
    break;
  case Note::Bb:
    s = "Bb";
    break;
  case Note::B:
    s = "B";
    break;
  default:
    break;
  }

  switch (quality)
  {
  case ChordQuality::Major:
    break;
  case ChordQuality::Minor:
    s += "m";
    break;
  default:
    break;
  }

  return s;
}