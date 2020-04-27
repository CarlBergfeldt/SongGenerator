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

  std::size_t pos;    
  pos = text.find(' ');
  while (pos < text.length())
  {
    lyrics.push_back(text.substr(0, pos));
    text = text.substr(pos+1);
    pos = text.find(' ');
  }
  lyrics.push_back(text);
}

std::string Lyrics::ToString()
{
  std::string s = "";
  for (std::vector<std::string>::iterator it = begin(lyrics); it != end(lyrics); ++it)
  {
    s += *it;
    s += " ";
  }

  return s;
}