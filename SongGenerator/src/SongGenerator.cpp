#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#include "Chord.h"
#include "Song.h"
#include "SongPart.h"

namespace
{
  std::string Trim(const std::string & value)
  {
    const std::string whitespace = " \t\n\r";
    const std::size_t start = value.find_first_not_of(whitespace);
    if (start == std::string::npos)
    {
      return "";
    }

    const std::size_t end = value.find_last_not_of(whitespace);
    return value.substr(start, end - start + 1);
  }

  bool TryMapNote(const std::string & token, Note & note)
  {
    if (token == "C") { note = Note::C; return true; }
    if (token == "C#" || token == "DB") { note = Note::Db; return true; }
    if (token == "D") { note = Note::D; return true; }
    if (token == "D#" || token == "EB") { note = Note::Eb; return true; }
    if (token == "E") { note = Note::E; return true; }
    if (token == "F") { note = Note::F; return true; }
    if (token == "F#" || token == "GB") { note = Note::Gb; return true; }
    if (token == "G") { note = Note::G; return true; }
    if (token == "G#" || token == "AB") { note = Note::Ab; return true; }
    if (token == "A") { note = Note::A; return true; }
    if (token == "A#" || token == "BB") { note = Note::Bb; return true; }
    if (token == "B") { note = Note::B; return true; }

    return false;
  }

  bool TryParseChordToken(const std::string & rawToken, Chord *& chord)
  {
    chord = nullptr;
    std::string token = Trim(rawToken);
    if (token.empty())
    {
      return false;
    }

    std::string normalized;
    normalized.reserve(token.size());
    for (char c : token)
    {
      normalized.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    ChordQuality quality = ChordQuality::Major;
    if (normalized.size() >= 3 && normalized.substr(normalized.size() - 3) == "MIN")
    {
      quality = ChordQuality::Minor;
      normalized = normalized.substr(0, normalized.size() - 3);
    }
    else if (normalized.size() >= 3 && normalized.substr(normalized.size() - 3) == "MAJ")
    {
      quality = ChordQuality::Major;
      normalized = normalized.substr(0, normalized.size() - 3);
    }
    else if (normalized.size() > 1 && normalized.back() == 'M')
    {
      quality = ChordQuality::Minor;
      normalized = normalized.substr(0, normalized.size() - 1);
    }

    Note root;
    if (!TryMapNote(normalized, root))
    {
      return false;
    }

    chord = new Chord(root, quality);
    return true;
  }

  std::vector<Chord *> ParseChordProgression(const std::string & input)
  {
    std::vector<Chord *> chords;

    std::string sanitized = input;
    std::replace(sanitized.begin(), sanitized.end(), ',', ' ');

    std::stringstream stream(sanitized);
    std::string token;

    while (stream >> token)
    {
      Chord * chord = nullptr;
      if (TryParseChordToken(token, chord))
      {
        chords.push_back(chord);
      }
    }

    return chords;
  }
}

int main()
{
  std::cout << "Enter chords (example: Am Em F Em): ";
  std::string chordInput;
  std::getline(std::cin, chordInput);

  std::cout << "Enter lyrics: ";
  std::string lyricsInput;
  std::getline(std::cin, lyricsInput);

  std::vector<Chord *> chordProgression = ParseChordProgression(chordInput);
  if (chordProgression.empty())
  {
    chordProgression = {
      new Chord(Note::A, ChordQuality::Minor),
      new Chord(Note::E, ChordQuality::Minor),
      new Chord(Note::F, ChordQuality::Major),
      new Chord(Note::E, ChordQuality::Minor)
    };
  }

  if (lyricsInput.empty())
  {
    lyricsInput = "Bury all your secrets in my skin";
  }

  SongPart verse = SongPart(&chordProgression, lyricsInput);
  verse.SetTimeSignature(4, 4);
  verse.SetNumberOfBars(std::max(4u, static_cast<uint32_t>(chordProgression.size())));

  Song song = Song({ &verse });

  std::cout << "\n" << song.ToString() << "\n";

  return 0;
}
