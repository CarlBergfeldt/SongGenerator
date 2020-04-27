#pragma once
#include <vector>

class Lyrics
{
public:
  Lyrics(std::string text);

  ~Lyrics();

  void BreakUpLyrics(std::string text);

  std::string ToString();

private:
  std::vector<std::string> lyrics;

};

