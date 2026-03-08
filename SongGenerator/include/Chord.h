#pragma once

#include <cstdint>
#include <string>

enum class ChordQuality
{
  Major,
  Minor,
  Diminished,
  Augmented,
  Suspended2,
  Suspended4,
  Power
};

enum class ChordSeventh
{
  None,
  Dominant,
  Major,
  Diminished
};

enum class ChordExtension
{
  None,
  Ninth,
  Eleventh,
  Thirteenth
};

enum class HarmonicFunction
{
  Unknown,
  Tonic,
  Predominant,
  Dominant
};

enum class TonalMode
{
  Unknown,
  Major,
  NaturalMinor,
  HarmonicMinor,
  MelodicMinor,
  Dorian,
  Phrygian,
  Lydian,
  Mixolydian,
  Aeolian,
  Locrian
};

enum class Note
{
  C = 1,
  Db = 2,
  D = 3,
  Eb = 4,
  E = 5,
  F = 6,
  Gb = 7,
  G = 8,
  Ab = 9,
  A = 10,
  Bb = 11,
  B = 12
};

struct ChordAlterations
{
  bool flatFive = false;
  bool sharpFive = false;
  bool flatNine = false;
  bool sharpNine = false;
  bool flatEleventh = false;
  bool sharpEleventh = false;
  bool flatThirteenth = false;
  bool sharpThirteenth = false;
};

struct TonalContext
{
  bool hasTonalCenter = false;
  Note tonalCenter = Note::C;
  TonalMode mode = TonalMode::Unknown;
  HarmonicFunction function = HarmonicFunction::Unknown;
  uint32_t scaleDegree = 0;
  bool borrowedFromParallelMode = false;
};

class Chord
{
public:
  Chord(Note root, ChordQuality quality);
  Chord(Note root,
    ChordQuality quality,
    ChordSeventh seventh,
    ChordExtension extension,
    ChordAlterations alterations,
    bool addSixth,
    bool addNinth);

  ~Chord();

  ChordQuality GetQuality() const { return quality; };

  Note GetRoot() const { return root; };

  ChordSeventh GetSeventh() const { return seventh; };
  void SetSeventh(ChordSeventh value) { seventh = value; };

  ChordExtension GetExtension() const { return extension; };
  void SetExtension(ChordExtension value) { extension = value; };

  ChordAlterations GetAlterations() const { return alterations; };
  void SetAlterations(const ChordAlterations & value) { alterations = value; };

  bool HasAddedSixth() const { return addSixth; };
  void SetAddedSixth(bool value) { addSixth = value; };

  bool HasAddedNinth() const { return addNinth; };
  void SetAddedNinth(bool value) { addNinth = value; };

  bool IsThirdOmitted() const { return omitThird; };
  void SetOmitThird(bool value) { omitThird = value; };

  bool IsFifthOmitted() const { return omitFifth; };
  void SetOmitFifth(bool value) { omitFifth = value; };

  bool HasBass() const { return hasBassNote; };
  Note GetBass() const { return bassNote; };
  void SetBass(Note value)
  {
    bassNote = value;
    hasBassNote = true;
  }
  void ClearBass() { hasBassNote = false; };

  void SetTonalContext(const TonalContext & value) { tonalContext = value; };
  TonalContext GetTonalContext() const { return tonalContext; };

  std::string DescribeTonality() const;
  std::string ToString() const;

private:
  ChordQuality quality;
  Note root;
  ChordSeventh seventh = ChordSeventh::None;
  ChordExtension extension = ChordExtension::None;
  ChordAlterations alterations = {};
  bool addSixth = false;
  bool addNinth = false;
  bool omitThird = false;
  bool omitFifth = false;
  bool hasBassNote = false;
  Note bassNote = Note::C;
  TonalContext tonalContext = {};
};
