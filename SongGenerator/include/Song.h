#pragma once
#include <vector>

#include "SongPart.h"

class Song
{
public:

  Song();

  Song(std::vector<SongPart *> songStructure);

  ~Song();

  std::string ToString();

private:
  std::vector<SongPart *> songStructure;

};

