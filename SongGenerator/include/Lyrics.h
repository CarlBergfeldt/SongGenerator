#pragma once
#include <vector>

class Lyrics
{
public:
  Lyrics(std::string text);

  ~Lyrics();

  void BreakUpLyrics(std::string text);

  std::string ToString();

  std::vector<std::string> * GetLyrics();

public:
  std::vector<std::string> lyrics;

};

