#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef NOGDI
#define NOGDI
#endif
#include <windows.h>

#include "Chord.h"
#include "SongPart.h"

namespace
{
  constexpr int kIdChordsEdit = 1001;
  constexpr int kIdLyricsEdit = 1002;
  constexpr int kIdTimeUpperEdit = 1003;
  constexpr int kIdTimeLowerEdit = 1004;
  constexpr int kIdBarsEdit = 1005;
  constexpr int kIdRenderButton = 1006;
  constexpr int kIdOutputEdit = 1007;

  constexpr uint32_t kDefaultUpper = 4;
  constexpr uint32_t kDefaultLower = 4;

  struct GuiState
  {
    HWND chordsEdit = nullptr;
    HWND lyricsEdit = nullptr;
    HWND timeUpperEdit = nullptr;
    HWND timeLowerEdit = nullptr;
    HWND barsEdit = nullptr;
    HWND renderButton = nullptr;
    HWND outputEdit = nullptr;
  };

  GuiState g_guiState;

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

  bool TryParseUInt(const std::string & text, uint32_t & value)
  {
    const std::string trimmed = Trim(text);
    if (trimmed.empty())
    {
      return false;
    }

    char * endPtr = nullptr;
    const unsigned long parsed = std::strtoul(trimmed.c_str(), &endPtr, 10);
    if (endPtr == trimmed.c_str() || *endPtr != '\0')
    {
      return false;
    }

    if (parsed > static_cast<unsigned long>(std::numeric_limits<uint32_t>::max()))
    {
      return false;
    }

    value = static_cast<uint32_t>(parsed);
    return true;
  }

  std::string ToUpperInvariant(const std::string & text)
  {
    std::string normalized;
    normalized.reserve(text.size());

    for (char c : text)
    {
      normalized.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
    }

    return normalized;
  }

  std::string ToLowerInvariant(const std::string & text)
  {
    std::string normalized;
    normalized.reserve(text.size());

    for (char c : text)
    {
      normalized.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }

    return normalized;
  }

  bool StartsWith(const std::string & value, const std::string & prefix)
  {
    return value.size() >= prefix.size() && value.compare(0, prefix.size(), prefix) == 0;
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

  bool TryParseSingleNoteToken(const std::string & rawToken, Note & note)
  {
    const std::string token = Trim(rawToken);
    if (token.empty() || token.size() > 2)
    {
      return false;
    }

    std::string normalized;
    normalized.reserve(2);

    const char noteLetter = static_cast<char>(std::toupper(static_cast<unsigned char>(token[0])));
    if (noteLetter < 'A' || noteLetter > 'G')
    {
      return false;
    }

    normalized.push_back(noteLetter);

    if (token.size() == 2)
    {
      const char accidental = token[1];
      if (accidental == '#')
      {
        normalized.push_back('#');
      }
      else if (accidental == 'b' || accidental == 'B')
      {
        normalized.push_back('B');
      }
      else
      {
        return false;
      }
    }

    return TryMapNote(normalized, note);
  }

  bool TryParseLeadingNoteToken(const std::string & token, std::size_t & consumedLength, Note & note)
  {
    consumedLength = 0;
    if (token.empty())
    {
      return false;
    }

    const char noteLetter = static_cast<char>(std::toupper(static_cast<unsigned char>(token[0])));
    if (noteLetter < 'A' || noteLetter > 'G')
    {
      return false;
    }

    consumedLength = 1;
    std::string normalized;
    normalized.push_back(noteLetter);

    if (token.size() > 1)
    {
      const char accidental = token[1];
      if (accidental == '#')
      {
        normalized.push_back('#');
        consumedLength = 2;
      }
      else if (accidental == 'b' || accidental == 'B')
      {
        normalized.push_back('B');
        consumedLength = 2;
      }
    }

    return TryMapNote(normalized, note);
  }

  bool TryConsumeToken(std::string & remaining, const std::string & token)
  {
    if (StartsWith(remaining, token))
    {
      remaining.erase(0, token.size());
      return true;
    }

    return false;
  }

  bool TryParseChordToken(const std::string & rawToken, Chord *& outChord)
  {
    outChord = nullptr;
    std::string token = Trim(rawToken);
    if (token.empty())
    {
      return false;
    }

    std::string chordBody = token;
    std::string bassToken;

    const std::size_t slash = token.find('/');
    if (slash != std::string::npos)
    {
      if (token.find('/', slash + 1) != std::string::npos)
      {
        return false;
      }

      chordBody = Trim(token.substr(0, slash));
      bassToken = Trim(token.substr(slash + 1));
      if (chordBody.empty() || bassToken.empty())
      {
        return false;
      }
    }

    Note root;
    std::size_t rootLength = 0;
    if (!TryParseLeadingNoteToken(chordBody, rootLength, root))
    {
      return false;
    }

    std::string descriptor = chordBody.substr(rootLength);
    descriptor.erase(std::remove_if(descriptor.begin(), descriptor.end(),
      [](char c)
      {
        return std::isspace(static_cast<unsigned char>(c)) != 0;
      }),
      descriptor.end());
    descriptor.erase(std::remove(descriptor.begin(), descriptor.end(), '('), descriptor.end());
    descriptor.erase(std::remove(descriptor.begin(), descriptor.end(), ')'), descriptor.end());

    std::string remaining = ToLowerInvariant(descriptor);
    if (StartsWith(remaining, "major"))
    {
      remaining.replace(0, 5, "maj");
    }

    if (StartsWith(remaining, "minor"))
    {
      remaining.replace(0, 5, "min");
    }

    ChordQuality quality = ChordQuality::Major;
    ChordSeventh seventh = ChordSeventh::None;
    ChordExtension extension = ChordExtension::None;
    ChordAlterations alterations = {};
    bool addSixth = false;
    bool addNinth = false;
    bool omitThird = false;
    bool omitFifth = false;

    if (TryConsumeToken(remaining, "min"))
    {
      quality = ChordQuality::Minor;
    }
    else if (StartsWith(remaining, "m") && !StartsWith(remaining, "maj"))
    {
      quality = ChordQuality::Minor;
      remaining.erase(0, 1);
    }
    else if (TryConsumeToken(remaining, "dim"))
    {
      quality = ChordQuality::Diminished;
    }
    else if (TryConsumeToken(remaining, "aug") || TryConsumeToken(remaining, "+"))
    {
      quality = ChordQuality::Augmented;
    }
    else if (TryConsumeToken(remaining, "sus2"))
    {
      quality = ChordQuality::Suspended2;
    }
    else if (TryConsumeToken(remaining, "sus4") || TryConsumeToken(remaining, "sus"))
    {
      quality = ChordQuality::Suspended4;
    }
    else if (TryConsumeToken(remaining, "5"))
    {
      quality = ChordQuality::Power;
    }

    if (TryConsumeToken(remaining, "maj13"))
    {
      seventh = ChordSeventh::Major;
      extension = ChordExtension::Thirteenth;
    }
    else if (TryConsumeToken(remaining, "maj11"))
    {
      seventh = ChordSeventh::Major;
      extension = ChordExtension::Eleventh;
    }
    else if (TryConsumeToken(remaining, "maj9"))
    {
      seventh = ChordSeventh::Major;
      extension = ChordExtension::Ninth;
    }
    else if (TryConsumeToken(remaining, "maj7"))
    {
      seventh = ChordSeventh::Major;
    }
    else
    {
      TryConsumeToken(remaining, "maj");
    }

    while (!remaining.empty())
    {
      if (TryConsumeToken(remaining, "add9"))
      {
        addNinth = true;
        continue;
      }

      if (TryConsumeToken(remaining, "add6"))
      {
        addSixth = true;
        continue;
      }

      if (TryConsumeToken(remaining, "omit3") || TryConsumeToken(remaining, "no3"))
      {
        omitThird = true;
        continue;
      }

      if (TryConsumeToken(remaining, "omit5") || TryConsumeToken(remaining, "no5"))
      {
        omitFifth = true;
        continue;
      }

      if (TryConsumeToken(remaining, "b13"))
      {
        alterations.flatThirteenth = true;
        continue;
      }

      if (TryConsumeToken(remaining, "#13"))
      {
        alterations.sharpThirteenth = true;
        continue;
      }

      if (TryConsumeToken(remaining, "b11"))
      {
        alterations.flatEleventh = true;
        continue;
      }

      if (TryConsumeToken(remaining, "#11"))
      {
        alterations.sharpEleventh = true;
        continue;
      }

      if (TryConsumeToken(remaining, "b9"))
      {
        alterations.flatNine = true;
        continue;
      }

      if (TryConsumeToken(remaining, "#9"))
      {
        alterations.sharpNine = true;
        continue;
      }

      if (TryConsumeToken(remaining, "b5"))
      {
        alterations.flatFive = true;
        continue;
      }

      if (TryConsumeToken(remaining, "#5"))
      {
        alterations.sharpFive = true;
        continue;
      }

      if (TryConsumeToken(remaining, "13"))
      {
        extension = ChordExtension::Thirteenth;
        if (seventh == ChordSeventh::None)
        {
          seventh = quality == ChordQuality::Diminished ? ChordSeventh::Diminished : ChordSeventh::Dominant;
        }
        continue;
      }

      if (TryConsumeToken(remaining, "11"))
      {
        extension = ChordExtension::Eleventh;
        if (seventh == ChordSeventh::None)
        {
          seventh = quality == ChordQuality::Diminished ? ChordSeventh::Diminished : ChordSeventh::Dominant;
        }
        continue;
      }

      if (TryConsumeToken(remaining, "9"))
      {
        extension = ChordExtension::Ninth;
        if (seventh == ChordSeventh::None)
        {
          seventh = quality == ChordQuality::Diminished ? ChordSeventh::Diminished : ChordSeventh::Dominant;
        }
        continue;
      }

      if (TryConsumeToken(remaining, "7"))
      {
        if (seventh == ChordSeventh::None)
        {
          seventh = quality == ChordQuality::Diminished ? ChordSeventh::Diminished : ChordSeventh::Dominant;
        }
        continue;
      }

      if (TryConsumeToken(remaining, "6"))
      {
        addSixth = true;
        continue;
      }

      return false;
    }

    outChord = new Chord(root, quality, seventh, extension, alterations, addSixth, addNinth);
    outChord->SetOmitThird(omitThird);
    outChord->SetOmitFifth(omitFifth);

    if (!bassToken.empty())
    {
      Note bass;
      if (!TryParseSingleNoteToken(ToUpperInvariant(bassToken), bass))
      {
        delete outChord;
        outChord = nullptr;
        return false;
      }

      outChord->SetBass(bass);
    }

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
      Chord * parsedChord = nullptr;
      if (TryParseChordToken(token, parsedChord))
      {
        chords.push_back(parsedChord);
      }
    }

    return chords;
  }

  std::vector<Chord *> BuildDefaultProgression()
  {
    return {
      new Chord(Note::A, ChordQuality::Minor),
      new Chord(Note::E, ChordQuality::Minor),
      new Chord(Note::F, ChordQuality::Major),
      new Chord(Note::E, ChordQuality::Minor),
      new Chord(Note::G, ChordQuality::Major),
      new Chord(Note::A, ChordQuality::Minor),
      new Chord(Note::F, ChordQuality::Major),
      new Chord(Note::E, ChordQuality::Minor)
    };
  }

  void FreeChords(std::vector<Chord *> & chords)
  {
    for (Chord * chord : chords)
    {
      delete chord;
    }
    chords.clear();
  }

  uint32_t ClampRange(uint32_t value, uint32_t minValue, uint32_t maxValue)
  {
    return std::min(maxValue, std::max(minValue, value));
  }

  uint32_t CalculateAutoBars(std::size_t numberOfBeatChords, uint32_t beatsPerBar)
  {
    const uint32_t safeBeatsPerBar = std::max(1u, beatsPerBar);
    const uint32_t beats = std::max(1u, static_cast<uint32_t>(numberOfBeatChords));
    return (beats + safeBeatsPerBar - 1) / safeBeatsPerBar;
  }

  bool TryParseTimeSignature(const std::string & input, uint32_t & upper, uint32_t & lower)
  {
    const std::string trimmed = Trim(input);
    if (trimmed.empty())
    {
      return false;
    }

    const std::size_t slash = trimmed.find('/');
    if (slash == std::string::npos)
    {
      return false;
    }

    uint32_t parsedUpper = 0;
    uint32_t parsedLower = 0;
    if (!TryParseUInt(trimmed.substr(0, slash), parsedUpper))
    {
      return false;
    }

    if (!TryParseUInt(trimmed.substr(slash + 1), parsedLower))
    {
      return false;
    }

    upper = parsedUpper;
    lower = parsedLower;
    return true;
  }

  std::string RenderTabFromInput(std::vector<Chord *> & chords,
    const std::string & lyrics,
    uint32_t upper,
    uint32_t lower,
    uint32_t barsOverride)
  {
    if (chords.empty())
    {
      chords = BuildDefaultProgression();
    }

    const uint32_t clampedUpper = ClampRange(upper, 1u, 16u);
    const uint32_t clampedLower = ClampRange(lower, 1u, 16u);

    uint32_t bars = barsOverride;
    if (bars == 0)
    {
      bars = CalculateAutoBars(chords.size(), clampedUpper);
    }
    bars = ClampRange(bars, 1u, 64u);

    std::string safeLyrics = lyrics;
    if (Trim(safeLyrics).empty())
    {
      safeLyrics = "Bury all your secrets in my skin";
    }

    SongPart section(&chords, safeLyrics);
    section.SetTimeSignature(clampedUpper, clampedLower);
    section.SetNumberOfBars(bars);

    return section.ToTabString();
  }

  std::string NormalizeForEditControl(const std::string & text)
  {
    std::string normalized;
    normalized.reserve(text.size() + text.size() / 8);

    for (std::size_t i = 0; i < text.size(); ++i)
    {
      if (text[i] == '\n' && (i == 0 || text[i - 1] != '\r'))
      {
        normalized.push_back('\r');
      }
      normalized.push_back(text[i]);
    }

    return normalized;
  }

  std::string GetWindowTextString(HWND control)
  {
    const int length = GetWindowTextLengthA(control);
    if (length <= 0)
    {
      return "";
    }

    std::vector<char> buffer(static_cast<std::size_t>(length) + 1, '\0');
    GetWindowTextA(control, buffer.data(), length + 1);
    return std::string(buffer.data());
  }

  void RenderGuiOutput()
  {
    std::string chordInput = GetWindowTextString(g_guiState.chordsEdit);
    std::string lyricsInput = GetWindowTextString(g_guiState.lyricsEdit);

    uint32_t upper = kDefaultUpper;
    uint32_t lower = kDefaultLower;
    uint32_t parsed = 0;
    if (TryParseUInt(GetWindowTextString(g_guiState.timeUpperEdit), parsed))
    {
      upper = parsed;
    }
    if (TryParseUInt(GetWindowTextString(g_guiState.timeLowerEdit), parsed))
    {
      lower = parsed;
    }

    uint32_t bars = 0;
    if (TryParseUInt(GetWindowTextString(g_guiState.barsEdit), parsed))
    {
      bars = parsed;
    }

    std::vector<Chord *> chords = ParseChordProgression(chordInput);
    std::string output = RenderTabFromInput(chords, lyricsInput, upper, lower, bars);
    FreeChords(chords);

    const std::string formatted = NormalizeForEditControl(output);
    SetWindowTextA(g_guiState.outputEdit, formatted.c_str());
  }

  void InitializeGuiControls(HWND window)
  {
    CreateWindowExA(0, "STATIC", "Chords (per beat)", WS_CHILD | WS_VISIBLE,
      12, 12, 200, 20, window, nullptr, nullptr, nullptr);

    g_guiState.chordsEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT",
      "Am Em F Em G Am F Em",
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
      12, 32, 980, 24, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdChordsEdit)), nullptr, nullptr);

    CreateWindowExA(0, "STATIC", "Lyrics", WS_CHILD | WS_VISIBLE,
      12, 64, 200, 20, window, nullptr, nullptr, nullptr);

    g_guiState.lyricsEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT",
      "Bury all your secrets in my skin and leave me with my sins",
      WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
      12, 84, 980, 24, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdLyricsEdit)), nullptr, nullptr);

    CreateWindowExA(0, "STATIC", "Time", WS_CHILD | WS_VISIBLE,
      12, 116, 50, 20, window, nullptr, nullptr, nullptr);

    g_guiState.timeUpperEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "4",
      WS_CHILD | WS_VISIBLE | ES_NUMBER,
      62, 112, 40, 24, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdTimeUpperEdit)), nullptr, nullptr);

    CreateWindowExA(0, "STATIC", "/", WS_CHILD | WS_VISIBLE,
      108, 116, 10, 20, window, nullptr, nullptr, nullptr);

    g_guiState.timeLowerEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "4",
      WS_CHILD | WS_VISIBLE | ES_NUMBER,
      120, 112, 40, 24, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdTimeLowerEdit)), nullptr, nullptr);

    CreateWindowExA(0, "STATIC", "Bars (0=auto)", WS_CHILD | WS_VISIBLE,
      180, 116, 100, 20, window, nullptr, nullptr, nullptr);

    g_guiState.barsEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "0",
      WS_CHILD | WS_VISIBLE | ES_NUMBER,
      286, 112, 60, 24, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdBarsEdit)), nullptr, nullptr);

    g_guiState.renderButton = CreateWindowExA(0, "BUTTON", "Render",
      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
      360, 111, 100, 26, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdRenderButton)), nullptr, nullptr);

    g_guiState.outputEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
      WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | ES_WANTRETURN,
      12, 148, 980, 560, window, reinterpret_cast<HMENU>(static_cast<INT_PTR>(kIdOutputEdit)), nullptr, nullptr);

    RenderGuiOutput();
  }

  LRESULT CALLBACK WindowProc(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
  {
    (void)lParam;

    switch (message)
    {
    case WM_CREATE:
      InitializeGuiControls(window);
      return 0;
    case WM_COMMAND:
      if (LOWORD(wParam) == kIdRenderButton && HIWORD(wParam) == BN_CLICKED)
      {
        RenderGuiOutput();
        return 0;
      }
      break;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      break;
    }

    return DefWindowProcA(window, message, wParam, lParam);
  }

  int RunGuiMode()
  {
    WNDCLASSA windowClass = {};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandleA(nullptr);
    windowClass.lpszClassName = "SongGeneratorTabWindow";
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (RegisterClassA(&windowClass) == 0)
    {
      return 1;
    }

    HWND window = CreateWindowExA(
      0,
      windowClass.lpszClassName,
      "SongGenerator - Tab & Lyrics",
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      1020,
      760,
      nullptr,
      nullptr,
      windowClass.hInstance,
      nullptr);

    if (window == nullptr)
    {
      return 1;
    }

    ShowWindow(window, SW_SHOW);
    UpdateWindow(window);

    MSG message = {};
    while (GetMessageA(&message, nullptr, 0, 0) > 0)
    {
      TranslateMessage(&message);
      DispatchMessageA(&message);
    }

    return static_cast<int>(message.wParam);
  }

  int RunConsoleMode()
  {
    std::cout << "Enter chords per beat (example: Am Em F Em G Am F Em): ";
    std::string chordInput;
    std::getline(std::cin, chordInput);

    std::cout << "Enter lyrics: ";
    std::string lyricsInput;
    std::getline(std::cin, lyricsInput);

    std::cout << "Enter time signature (example: 4/4, Enter for default): ";
    std::string timeSignatureInput;
    std::getline(std::cin, timeSignatureInput);

    std::cout << "Enter bars (0 = auto): ";
    std::string barsInput;
    std::getline(std::cin, barsInput);

    uint32_t upper = kDefaultUpper;
    uint32_t lower = kDefaultLower;
    uint32_t parsedBars = 0;

    uint32_t parsedUpper = 0;
    uint32_t parsedLower = 0;
    if (TryParseTimeSignature(timeSignatureInput, parsedUpper, parsedLower))
    {
      upper = parsedUpper;
      lower = parsedLower;
    }

    TryParseUInt(barsInput, parsedBars);

    std::vector<Chord *> chords = ParseChordProgression(chordInput);
    std::string output = RenderTabFromInput(chords, lyricsInput, upper, lower, parsedBars);

    std::cout << "\n" << output << "\n";

    FreeChords(chords);
    return 0;
  }
}

int main(int argc, char * argv[])
{
  if (argc > 1)
  {
    const std::string mode = argv[1];
    if (mode == "--gui")
    {
      return RunGuiMode();
    }
  }

  return RunConsoleMode();
}
