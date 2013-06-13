#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>

int main ()
{

  std::ifstream exams_file("ear-f-83.crs");

  // Count the newlines to get the total of exams
  exams_file.unsetf(std::ios_base::skipws); // Don't skip newlines
  unsigned total_exams = std::count(
    std::istream_iterator<char>(exams_file),
    std::istream_iterator<char>(),
    '\n');

  std::vector< std::vector<int> > conflicts;

  conflicts.resize(total_exams);

  // Initializes the matrix
  for (int i = 0; i < total_exams; ++i)
    conflicts[i].resize(total_exams);

  std::cout << conflicts[10][1];
  return 0;
}