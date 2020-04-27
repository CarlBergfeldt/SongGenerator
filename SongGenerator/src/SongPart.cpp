#include "SongPart.h"

SongPart::SongPart() :
  chordProgression({
  new Chord(Note::A, ChordQuality::Minor),
  new Chord(Note::E, ChordQuality::Minor),
  new Chord(Note::F, ChordQuality::Major),
  new Chord(Note::E, ChordQuality::Minor)
    }),
  lyrics("")
{
  InitializeChordPositions();
}

SongPart::SongPart(std::vector<Chord *> * chordProgression, std::string lyrics) :
  chordProgression(*chordProgression),
  lyrics(lyrics)
{
  InitializeChordPositions();
}

SongPart::~SongPart()
{
}

void SongPart::InitializeChordPositions()
{
  uint32_t pos = 0;
  for (std::vector<Chord *>::iterator it = begin(chordProgression); it != end(chordProgression); ++it)
  {
    chordPositions[pos] = *it;
    //TODO : Insert randomizer
    pos += MaxCharactersPerLine / timeSignature.lower;
  }
    
}

bool SongPart::SetNoteResolution(uint32_t number)
{
  if (number < 0)
  {
    return false;
  }

  if (number > MaxNoteResolution)
  {
    return false;
  }

  noteResolution = number;
  return true;
}

bool SongPart::SetNumberOfBars(uint32_t number)
{
  if (number < 0)
  {
    return false;
  }

  if (number > MaxNumberOfBars)
  {
    return false;
  }

  numberOfBars = number;
  return true;
}

bool SongPart::SetTimeSignature(uint32_t upper, uint32_t lower)
{
  if (upper < 0 || lower < 0)
  {
    return false;
  }

  if (upper > MaxTimeSignatureValue || lower > MaxTimeSignatureValue)
  {
    return false;
  }

  timeSignature.upper = upper;
  timeSignature.lower = lower;
  return true;
}

std::string SongPart::ToString()
{
  std::string s = "";
  //int p = numberOfBars / timeSignature.lower;
  
  //for (auto it = begin(chordProgression); it != end(chordProgression); ++it)
  //{
  //  s += (*(Chord *)*it).ToString();

  //  s += " ";
  //}

  uint32_t writePosition = 0;

  for (auto it = begin(chordPositions); it != end(chordPositions); ++it)
  {

    for (unsigned int i = 0; i < it->first - writePosition; i++)
    {
      s += " ";
    }

    s += (*(Chord *)it->second).ToString();

    s += " ";

    writePosition = it->first;
  }


  s += "\n";

  s += lyrics.ToString();

  s += "\n";

  return s;
}