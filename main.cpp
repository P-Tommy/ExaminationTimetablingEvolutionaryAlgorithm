#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <iterator>

int main ()
{
  std::map<int, int> exams; // Saves the number of students per exam

  std::ifstream myfile("ear-f-83.crs");

  // Count the newlines to get the total of exams
  myfile.unsetf(std::ios_base::skipws); // Don't skip newlines
  unsigned total_exams = std::count(
    std::istream_iterator<char>(myfile),
    std::istream_iterator<char>(),
    '\n');

  std::cout << total_exams;
  return 0;
}