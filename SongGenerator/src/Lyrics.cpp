#include <numeric> // accumulate

#include "Lyrics.h"

Lyrics::Lyrics(std::string text)
{
  BreakUpLyrics(text);
}

Lyrics::~Lyrics()
{
}

void Lyrics::BreakUpLyrics(std::string text)
{
  std::size_t posSpace = text.find(' ');
  //std::size_t posNewLine = text.find("\n");
  while (posSpace < text.length())
  {
    lyrics.push_back(text.substr(0, posSpace));
    text = text.substr(posSpace +1);
    posSpace = text.find(' ');
  }
  lyrics.push_back(text);
}

std::vector<std::string> * Lyrics::GetLyrics()
{
  return &lyrics;
}

std::string Lyrics::ToString()
{
  //std::string s = "";
  //for (std::vector<std::string>::iterator it = begin(lyrics); it != end(lyrics); ++it)
  //{
  //  s += *it;
  //  s += " ";
  //}

  std::string s = accumulate(begin(lyrics), end(lyrics), std::string{},
    [](const std::string & total, std::string & s) {return total + " " + s; });

  return s;
}