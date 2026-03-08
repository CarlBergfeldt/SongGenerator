#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "SongPart.h"

namespace
{
  std::string PadRight(const std::string & text, std::size_t width, char fill = ' ')
  {
    if (text.size() >= width)
    {
      return text.substr(0, width);
    }

    return text + std::string(width - text.size(), fill);
  }

  std::string BuildGridLine(const std::vector<std::string> & cells, uint32_t beatsPerBar, char fill = ' ')
  {
    std::string line = "|";

    for (std::size_t i = 0; i < cells.size(); ++i)
    {
      if (cells[i].empty())
      {
        line += std::string(6, fill);
      }
      else
      {
        line += PadRight(cells[i], 6, fill);
      }

      if ((i + 1) % beatsPerBar == 0)
      {
        line += "|";
      }
    }

    return line;
  }

  std::vector<std::string> SplitWords(const std::vector<std::string> & chunks)
  {
    std::vector<std::string> words;

    for (const auto & chunk : chunks)
    {
      std::stringstream stream(chunk);
      std::string token;

      while (stream >> token)
      {
        words.push_back(token);
      }
    }

    return words;
  }
}

SongPart::SongPart() :
  chordProgression({
  new Chord(Note::A, ChordQuality::Minor),
  new Chord(Note::E, ChordQuality::Minor),
  new Chord(Note::F, ChordQuality::Major),
  new Chord(Note::E, ChordQuality::Minor)
    }),
  lyrics("")
{
  InitializeChordPositions(false);
  InitializeLyricsPositions(false);
}

SongPart::SongPart(std::vector<Chord *> * chordProgression, std::string lyrics) :
  chordProgression(*chordProgression),
  lyrics(lyrics)
{
  InitializeChordPositions(false);
  InitializeLyricsPositions(false);
}

SongPart::~SongPart()
{
}

void SongPart::InitializeChordPositions(bool isRandomizerOn)
{
  chordPositions.clear();

  uint32_t pos = 0;
  for (std::vector<Chord *>::iterator it = begin(chordProgression); it != end(chordProgression); ++it)
  {
    chordPositions[pos] = *it;

    if (isRandomizerOn)
    {
      uint32_t randomMultiplier = (1 + rand() % 2);
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
  lyricsPositions.clear();

  uint32_t pos = 0;
  for (std::vector<std::string>::iterator it = begin(*(lyrics.GetLyrics())); it != end(*(lyrics.GetLyrics())); ++it)
  {
    if (isRandomizerOn)
    {
      uint32_t randomMultiplier = (1 + rand() % 2);

      if (randomMultiplier < 2 && pos != 0)
      {
        auto iter = lyricsPositions.find(pos - 1);
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
  if (number == 0 || number > MaxNoteResolution)
  {
    return false;
  }

  noteResolution = number;
  return true;
}

bool SongPart::SetNumberOfBars(uint32_t number)
{
  if (number == 0 || number > MaxNumberOfBars)
  {
    return false;
  }

  numberOfBars = number;
  return true;
}

bool SongPart::SetTimeSignature(uint32_t upper, uint32_t lower)
{
  if (upper == 0 || lower == 0)
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
  return ToTabString();
}

std::string SongPart::ToTabString()
{
  const uint32_t bars = std::max(1u, numberOfBars);
  const uint32_t beatsPerBar = std::max(1u, timeSignature.upper);
  const uint32_t totalBeats = bars * beatsPerBar;

  std::vector<std::string> beatCells(totalBeats);
  for (uint32_t beat = 0; beat < totalBeats; ++beat)
  {
    beatCells[beat] = std::to_string((beat % beatsPerBar) + 1);
  }

  std::vector<std::string> chordCells(totalBeats);
  if (!chordProgression.empty())
  {
    for (uint32_t bar = 0; bar < bars; ++bar)
    {
      const uint32_t beatIndex = bar * beatsPerBar;
      const uint32_t chordIndex = bar % static_cast<uint32_t>(chordProgression.size());
      chordCells[beatIndex] = chordProgression[chordIndex]->ToString();
    }
  }

  std::vector<std::string> lyricCells(totalBeats);
  std::vector<std::string> lyricWords = SplitWords(*(lyrics.GetLyrics()));

  if (!lyricWords.empty())
  {
    for (uint32_t i = 0; i < lyricWords.size(); ++i)
    {
      const uint32_t beatIndex = std::min(totalBeats - 1, (i * totalBeats) / static_cast<uint32_t>(lyricWords.size()));

      if (lyricCells[beatIndex].empty())
      {
        lyricCells[beatIndex] = lyricWords[i];
      }
      else
      {
        lyricCells[beatIndex] += "/" + lyricWords[i];
      }
    }
  }

  std::string s;
  s += "Time bars: " + std::to_string(bars) + "  Signature: " + std::to_string(timeSignature.upper) + "/" + std::to_string(timeSignature.lower) + "\n";
  s += "Beat   " + BuildGridLine(beatCells, beatsPerBar, '.') + "\n";
  s += "Chords " + BuildGridLine(chordCells, beatsPerBar, ' ') + "\n";
  s += "e      " + BuildGridLine(std::vector<std::string>(totalBeats), beatsPerBar, '-') + "\n";
  s += "B      " + BuildGridLine(std::vector<std::string>(totalBeats), beatsPerBar, '-') + "\n";
  s += "G      " + BuildGridLine(std::vector<std::string>(totalBeats), beatsPerBar, '-') + "\n";
  s += "D      " + BuildGridLine(std::vector<std::string>(totalBeats), beatsPerBar, '-') + "\n";
  s += "A      " + BuildGridLine(std::vector<std::string>(totalBeats), beatsPerBar, '-') + "\n";
  s += "E      " + BuildGridLine(std::vector<std::string>(totalBeats), beatsPerBar, '-') + "\n";
  s += "Lyrics " + BuildGridLine(lyricCells, beatsPerBar, ' ') + "\n";

  return s;
}
