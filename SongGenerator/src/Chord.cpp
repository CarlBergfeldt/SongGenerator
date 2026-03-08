#include "Chord.h"

#include <vector>

namespace
{
  std::string NoteToString(Note note)
  {
    switch (note)
    {
    case Note::C:
      return "C";
    case Note::Db:
      return "Db";
    case Note::D:
      return "D";
    case Note::Eb:
      return "Eb";
    case Note::E:
      return "E";
    case Note::F:
      return "F";
    case Note::Gb:
      return "Gb";
    case Note::G:
      return "G";
    case Note::Ab:
      return "Ab";
    case Note::A:
      return "A";
    case Note::Bb:
      return "Bb";
    case Note::B:
      return "B";
    default:
      return "";
    }
  }

  std::string ModeToString(TonalMode mode)
  {
    switch (mode)
    {
    case TonalMode::Major:
      return "major";
    case TonalMode::NaturalMinor:
      return "natural minor";
    case TonalMode::HarmonicMinor:
      return "harmonic minor";
    case TonalMode::MelodicMinor:
      return "melodic minor";
    case TonalMode::Dorian:
      return "dorian";
    case TonalMode::Phrygian:
      return "phrygian";
    case TonalMode::Lydian:
      return "lydian";
    case TonalMode::Mixolydian:
      return "mixolydian";
    case TonalMode::Aeolian:
      return "aeolian";
    case TonalMode::Locrian:
      return "locrian";
    default:
      return "unknown";
    }
  }

  std::string FunctionToString(HarmonicFunction function)
  {
    switch (function)
    {
    case HarmonicFunction::Tonic:
      return "tonic";
    case HarmonicFunction::Predominant:
      return "predominant";
    case HarmonicFunction::Dominant:
      return "dominant";
    default:
      return "unknown";
    }
  }

  std::string ExtensionNumber(ChordExtension extension)
  {
    switch (extension)
    {
    case ChordExtension::Ninth:
      return "9";
    case ChordExtension::Eleventh:
      return "11";
    case ChordExtension::Thirteenth:
      return "13";
    default:
      return "";
    }
  }
}

Chord::Chord(Note root, ChordQuality quality) :
  root(root),
  quality(quality)
{
}

Chord::Chord(Note root,
  ChordQuality quality,
  ChordSeventh seventh,
  ChordExtension extension,
  ChordAlterations alterations,
  bool addSixth,
  bool addNinth) :
  root(root),
  quality(quality),
  seventh(seventh),
  extension(extension),
  alterations(alterations),
  addSixth(addSixth),
  addNinth(addNinth)
{
}

Chord::~Chord()
{
}

std::string Chord::DescribeTonality() const
{
  std::string description;

  if (tonalContext.hasTonalCenter)
  {
    description += "Center " + NoteToString(tonalContext.tonalCenter);
  }

  if (tonalContext.mode != TonalMode::Unknown)
  {
    if (!description.empty())
    {
      description += " ";
    }
    description += "in " + ModeToString(tonalContext.mode);
  }

  if (tonalContext.function != HarmonicFunction::Unknown)
  {
    if (!description.empty())
    {
      description += ", ";
    }
    description += "function " + FunctionToString(tonalContext.function);
  }

  if (tonalContext.scaleDegree > 0)
  {
    if (!description.empty())
    {
      description += ", ";
    }
    description += "degree " + std::to_string(tonalContext.scaleDegree);
  }

  if (tonalContext.borrowedFromParallelMode)
  {
    if (!description.empty())
    {
      description += ", ";
    }
    description += "borrowed from parallel mode";
  }

  if (description.empty())
  {
    return "No tonal context";
  }

  return description;
}

std::string Chord::ToString() const
{
  std::string symbol = NoteToString(root);

  switch (quality)
  {
  case ChordQuality::Major:
    break;
  case ChordQuality::Minor:
    symbol += "m";
    break;
  case ChordQuality::Diminished:
    symbol += "dim";
    break;
  case ChordQuality::Augmented:
    symbol += "aug";
    break;
  case ChordQuality::Suspended2:
    symbol += "sus2";
    break;
  case ChordQuality::Suspended4:
    symbol += "sus4";
    break;
  case ChordQuality::Power:
    symbol += "5";
    break;
  default:
    break;
  }

  if (extension != ChordExtension::None)
  {
    const std::string extensionNumber = ExtensionNumber(extension);
    if (seventh == ChordSeventh::Major)
    {
      symbol += "maj" + extensionNumber;
    }
    else
    {
      symbol += extensionNumber;
    }
  }
  else
  {
    switch (seventh)
    {
    case ChordSeventh::Dominant:
      symbol += "7";
      break;
    case ChordSeventh::Major:
      symbol += "maj7";
      break;
    case ChordSeventh::Diminished:
      if (quality == ChordQuality::Diminished)
      {
        symbol += "7";
      }
      else
      {
        symbol += "dim7";
      }
      break;
    default:
      break;
    }
  }

  if (addSixth)
  {
    symbol += "add6";
  }
  if (addNinth && extension != ChordExtension::Ninth)
  {
    symbol += "add9";
  }

  std::vector<std::string> alterationsText;
  if (alterations.flatFive) { alterationsText.push_back("b5"); }
  if (alterations.sharpFive) { alterationsText.push_back("#5"); }
  if (alterations.flatNine) { alterationsText.push_back("b9"); }
  if (alterations.sharpNine) { alterationsText.push_back("#9"); }
  if (alterations.flatEleventh) { alterationsText.push_back("b11"); }
  if (alterations.sharpEleventh) { alterationsText.push_back("#11"); }
  if (alterations.flatThirteenth) { alterationsText.push_back("b13"); }
  if (alterations.sharpThirteenth) { alterationsText.push_back("#13"); }
  if (omitThird) { alterationsText.push_back("no3"); }
  if (omitFifth) { alterationsText.push_back("no5"); }

  if (!alterationsText.empty())
  {
    symbol += "(";
    for (std::size_t i = 0; i < alterationsText.size(); ++i)
    {
      if (i > 0)
      {
        symbol += " ";
      }
      symbol += alterationsText[i];
    }
    symbol += ")";
  }

  if (hasBassNote)
  {
    symbol += "/" + NoteToString(bassNote);
  }

  return symbol;
}
