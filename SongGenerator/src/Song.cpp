#include "Song.h"

Song::Song()
{


}

Song::Song(std::vector<SongPart *> songStructure) :
  songStructure(songStructure)
{
}

Song::~Song()
{
}

std::string Song::ToString()
{
  std::string s = "";
  for (std::vector<SongPart *>::iterator it = begin(songStructure); it != end(songStructure); ++it)
  {
    s += (*(SongPart *)*it).ToString();
    s += "\n";
  }

  return s;
}