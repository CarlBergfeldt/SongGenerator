#include <algorithm>
#include <numeric>

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
  InitializeChordPositions(true);
  InitializeLyricsPositions(true);
}

SongPart::SongPart(std::vector<Chord *> * chordProgression, std::string lyrics) :
  chordProgression(*chordProgression),
  lyrics(lyrics)
{
  InitializeChordPositions(true);
  InitializeLyricsPositions(true);
}

SongPart::~SongPart()
{
}

void SongPart::InitializeChordPositions(bool isRandomizerOn)
{
  uint32_t pos = 0;
  for (std::vector<Chord *>::iterator it = begin(chordProgression); it != end(chordProgression); ++it)
  {
    chordPositions[pos] = *it;

    if (isRandomizerOn)
    {
      uint32_t randomMultiplier = (1 + rand() % 2);
      // Increase position with random number 1..2 and use half of the max width increase with respect to number of bars used in the time signature
      pos += randomMultiplier * (MaxCharactersPerLine / timeSignature.lower) / 2;
    }
    else
    {
      pos += MaxCharactersPerLine / timeSignature.lower;
    }
  }
    
}

void SongPart::InitializeLyricsPositions(bool isRandomizerOn)
{
  uint32_t pos = 0;
  for (std::vector<std::string>::iterator it = begin(*(lyrics.GetLyrics())); it != end(*(lyrics.GetLyrics())); ++it)
  {
    if (isRandomizerOn)
    {
      uint32_t randomMultiplier = (1 + rand() % 2);

      // Increase position with random number 1..2 and use half of the max width increase with respect to number of bars used in the time signature
      if (randomMultiplier < 2 && pos != 0)
      {
        auto iter = lyricsPositions.find(pos-1);
        iter->second += " " + *(it);
      }
      else
      {
        lyricsPositions[pos] = *it;
      }
      pos += randomMultiplier / 2;
    }
    else
    {
      lyricsPositions[pos] = *it;
      pos++;
    }
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

  //s += lyrics.ToString();

  for (auto it = begin(lyricsPositions); it != end(lyricsPositions); ++it)
  {
    unsigned int numberOfChars = it->second.size();
    s += it->second;
    std::string stringFill(MaxCharactersPerLine/timeSignature.lower - numberOfChars, ' ');
    s += stringFill;
  }
  //unsigned int numberOfChars = accumulate(begin(lyricsPositions), end(lyricsPositions), 0, [](unsigned int count, const std::map<uint32_t, std::string>::value_type& lyricsPos) {return count + lyricsPos.second.size(); });
  
  s += "\n";

  return s;
}