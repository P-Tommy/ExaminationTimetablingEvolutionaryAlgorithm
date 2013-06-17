#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <boost/tokenizer.hpp>

#define PROBLEM_CRS "asd.crs"
#define PROBLEM_STU "asd.stu"

// Penalizations
#define W0 16
#define W1 8
#define W2 4
#define W3 2
#define W4 1

#define POP_SIZE 10
#define MAX_TIMESLOTS 35
#define MAX_GENERATIONS 50 // Number of total generations before stopping

#define PROB_MUTATION 0.05 // Probability of a mutation in one gene of a solution
#define PROB_CLIMB 0.1 // Probability of a HC ocurring

#define HC_ITERATIONS 30 // Maximum of iterations in the AC algorithm

unsigned total_exams; // Number of exams

// The conflict matrix saves in each cell (i,j) the number of students
// with a conflict between the exams i and j
std::vector< std::vector<int> > conflicts;

// Prints the conflict matrix
void print_conflicts()
{
  for (std::vector< std::vector<int> >::iterator i = conflicts.begin() ; i != conflicts.end(); ++i)
  {
    std::cout << "\n";
    for (std::vector<int>::iterator conflict = (*i).begin() ; conflict != (*i).end(); ++conflict)
      std::cout << ' ' << *conflict;
  }
}

// Represents a solution
class Solution
{
private:

  void swap_exams(int a, int b)
  {
    int tmp = genotype.at(a);
    genotype.at(a) = genotype.at(b);
    genotype.at(b) = tmp;
  }

public:
  int aptitude;

  // Each gen in the genotype saves the Timeslot to which the exam is assigned
  std::vector<int> genotype;

  // Generates a random solution
  Solution()
  {
    genotype.resize(total_exams);

    for (std::vector<int>::iterator exam = genotype.begin() ; exam != genotype.end(); ++exam)
    {
      // Fill every exam with it's own timeslot
      *exam = rand() % MAX_TIMESLOTS;
    }

    calculate_aptitude();
  }

  // Calculates the solution aptitude (and sets it)
  void calculate_aptitude()
  {
    aptitude = 0;

    // Count the total used timeslots
    // int used_timeslots = 0;
    // for (std::vector<int>::iterator timeslot = timeslots.begin(); timeslot != timeslots.end(); ++timeslot)
    //   if ((*timeslot)->exams > 0) used_timeslots++;

    for (int i = 0; i < total_exams; ++i)
    {
      for (int j = i; j < total_exams; ++j)
      {
        if (conflicts.at(i).at(j) > 0) // If there is a conflict
        {
          // Calculate the distance (timeslots in between) between the two conflicting exams
          int distance = abs(genotype.at(i) - genotype.at(j));

          // Depending on the distance, we assign a different penalization
          // and it gets amplified by the number of students with the same conflict
          switch (distance)
          {
          case 0:
            aptitude += W0 * conflicts.at(i).at(j);
            break;
          case 1:
            aptitude += W1 * conflicts.at(i).at(j);
            break;
          case 2:
            aptitude += W2 * conflicts.at(i).at(j);
            break;
          case 3:
            aptitude += W3 * conflicts.at(i).at(j);
            break;
          default:
            aptitude += W4 * conflicts.at(i).at(j);
            break;
          }
        }
      }
    }
  }

  void mutate()
  {
    for (std::vector<int>::iterator exam = genotype.begin() ; exam != genotype.end(); ++exam)
    {
      if (rand() % 100 / 100.0 < PROB_MUTATION)
      {
        // Assign a new random timeslot
        *exam = rand() % MAX_TIMESLOTS;
      }
    }

    calculate_aptitude();
  }

  void hill_climb()
  {
    Solution candidate = *this;

    int var_1, var_2;
    for (int i = 0; i < HC_ITERATIONS; ++i)
    {
      var_1 = rand() % total_exams;
      var_2 = rand() % total_exams;

      candidate.swap_exams(var_1, var_2);
      candidate.calculate_aptitude();

      // If the new genotype has a better aptitude than the current best,
      // we make the swap in the current solution. We do not copy the candidate
      // over the current solution for performance reasons
      if (candidate.aptitude < aptitude)
      {
        swap_exams(var_1, var_2);
        calculate_aptitude();
      }
      else
        candidate.swap_exams(var_1, var_2); // Reverse the change
    }
  }

  // Prints the solution in stdout
  void print()
  {
    int i=0;
    for (std::vector<int>::iterator exam = genotype.begin() ; exam != genotype.end(); ++exam, ++i)
    {
      std::cout << i << " " << *exam << " \n";
    }
    std::cout << " | " << aptitude << "\n";
  }
};

std::vector< Solution* > population;

// Fills the conflicts matrix
void fill_conflicts();

// Prints the population
void print_pop();

int get_total_exams();
void generate_population();
void destroy_population();

Solution* select_best_solution();

int main ()
{
  srand(time(NULL)); // Sets a random seed

  total_exams = get_total_exams();

  fill_conflicts();

  generate_population();

  for (int cur_generation = 0; cur_generation < MAX_GENERATIONS; ++cur_generation)
  {
    for (std::vector< Solution* >::iterator solution = population.begin() ; solution != population.end(); ++solution)
    {
      if (rand() % 100 / 100.0 < PROB_CLIMB)
      {
        (*solution)->hill_climb();
      }
      (*solution)->mutate();
    }
  }

  Solution *best = select_best_solution();
  std::cout << "Best solution: ";
  best->print();

  destroy_population();
  return 0;
}

void print_pop()
{
  for (std::vector< Solution* >::iterator solution = population.begin() ; solution != population.end(); ++solution)
  {
    (*solution)->print();
  }
}

Solution* select_best_solution()
{
  Solution *best = *(population.begin());

  for (std::vector< Solution* >::iterator solution = population.begin() + 1; solution != population.end(); ++solution)
    if ((*solution)->aptitude < best->aptitude)
      best = *solution;

  return best;
}

void fill_conflicts()
{
  conflicts.resize(total_exams);

  // Initializes the matrix
  for (int i = 0; i < total_exams; ++i)
    conflicts[i].resize(total_exams);

  // Read the students file to fill the conflicts matrix
  std::ifstream infile(PROBLEM_STU);

  boost::char_separator<char> sep(" "); // Exams separated by spaces

  // We parse every line of the file, filling the conflict matrix
  std::string line;
  while (std::getline(infile, line))
  {

    // We tokenize the line and mark as conflicts every pair of exams
    // that appears in the same line
    std::istringstream iss(line);
    boost::tokenizer<boost::char_separator<char> > tokens(line, sep);
    if (tokens.begin() != tokens.end())   // Check if there is more than one exam
    {
      for(boost::tokenizer< boost::char_separator<char> >::iterator exam = tokens.begin(); exam != tokens.end(); ++exam)
      {
        boost::tokenizer< boost::char_separator<char> >::iterator other_exam = exam;

        // We iterate on all the remaining exams in the line, after the current one
        // and mark the couple (current, next) and (next, current) as conflicted
        for(other_exam++; other_exam != tokens.end(); ++other_exam)
        {
          conflicts[atoi((*exam).c_str()) - 1][atoi((*other_exam).c_str()) - 1]++;
          conflicts[atoi((*other_exam).c_str()) - 1][atoi((*exam).c_str()) - 1]++;
        }
      }
    }
  }
}

int get_total_exams()
{
  std::ifstream exams_file(PROBLEM_CRS);

  // Count the newlines to get the total of exams
  exams_file.unsetf(std::ios_base::skipws); // Don't skip newlines
  unsigned total_exams = std::count(
                           std::istream_iterator<char>(exams_file),
                           std::istream_iterator<char>(),
                           '\n');

  return total_exams;
}

void generate_population()
{
  population.resize(POP_SIZE);

  for (std::vector< Solution* >::iterator solution = population.begin() ; solution != population.end(); ++solution)
  {
    *solution = new Solution();
  }
}

void destroy_population()
{
  for (std::vector< Solution* >::iterator solution = population.begin() ; solution != population.end(); ++solution)
  {
    free(*solution);
  }
}