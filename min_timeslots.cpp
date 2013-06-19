#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <map>
#include <boost/tokenizer.hpp>

#define PROBLEM_CRS "hec-s-92.crs"
#define PROBLEM_STU "hec-s-92.stu"

// Penalizations
#define W0 16
#define W1 8
#define W2 4
#define W3 2
#define W4 1

#define POP_SIZE 20
#define MAX_GENERATIONS 1000 // Number of total generations before stopping
#define ELITIST_SELECT 5

#define PROB_MUTATION 0.1 // Probability of a mutation in one gene of a solution
#define PROB_CLIMB 0.2 // Probability of a HC ocurring

#define HC_ITERATIONS 30 // Maximum of iterations in the AC algorithm
#define MAX_HC_RETRIES 100 // Maximum retries for searching a feasible solution

unsigned total_exams; // Number of exams

// The conflict matrix saves in each cell (i,j) the number of students
// with a conflict between the exams i and j
std::vector< std::vector<int> > conflicts;

// Represents a solution
class Solution
{
public:
  int aptitude;

  // Each gen in the genotype saves the Timeslot to which the exam is assigned
  std::vector<int> genotype;
  std::vector<int> used_timeslots;
  std::map<int, int> exams_in_timeslot; // map<Timeslot id, # of exams assigned>

  // Generates a random solution
  Solution()
  {
    for (int i = 0; i < total_exams; ++i)
    {
      // Fill every exam with it's own timeslot to generate a feasible solution
      int random;
      do {
        random = rand() % (i+1);
        genotype.push_back(random);
        if (is_feasible(i))
          break;
        else
          genotype.pop_back();
      } while(true);
      exams_in_timeslot[i] = 1;
    }

    recalculate_used_timeslots();
    calculate_aptitude();
  }

  bool operator<(Solution);

  // Calculates the solution aptitude (and sets it)
  void calculate_aptitude()
  {
    aptitude = 0;

    // Count the total used timeslots
    aptitude = used_timeslots.size();
  }

  void recalculate_used_timeslots()
  {
    used_timeslots.clear();

    for (std::map<int, int>::iterator timeslot=exams_in_timeslot.begin(); timeslot!=exams_in_timeslot.end(); ++timeslot)
      if (timeslot->second > 0)
        used_timeslots.push_back(timeslot->first);
  }

  // Returns true if the solution is feasible.
  bool is_feasible()
  {
    for (int i = 0; i < total_exams; ++i)
      for (int j = i + 1; j < total_exams; ++j)
        if (conflicts.at(i).at(j) > 0 && genotype.at(i) == genotype.at(j)) // If there is a conflict
          return false; // Solution not feasible

    return true;
  }

  bool is_feasible(int exam)
  {
    for (int i = 0; i < genotype.size(); ++i)
      if (i != exam && conflicts.at(exam).at(i) > 0 && genotype.at(exam) == genotype.at(i)) // If there is a conflict
        return false; // Solution not feasible

    return true;
  }

  void mutate()
  {
    for (std::vector<int>::iterator exam = genotype.begin() ; exam != genotype.end(); ++exam)
    {
      if (rand() % 100 / 100.0 < PROB_MUTATION)
      {
        // Mutate until we have a feasible solution
        for (int retries = 0; retries < MAX_HC_RETRIES; ++retries)
        {
          int prev_timeslot = *exam;
          exams_in_timeslot[*exam]--;
          *exam = used_timeslots.at(rand() % used_timeslots.size());
          exams_in_timeslot[*exam]++;

          if(is_feasible())
            break;
          else // Undo the changes
          {
            exams_in_timeslot[*exam]--;
            *exam = prev_timeslot;
            exams_in_timeslot[*exam]++;
          }
        };
      }
    }

    recalculate_used_timeslots();
    calculate_aptitude();
  }

  void hill_climb()
  {
    Solution candidate = *this;

    for (int i = 0; i < HC_ITERATIONS; ++i)
    {
      int prev_timeslot, retries = 0;
      int var = rand() % total_exams;
      while(retries < MAX_HC_RETRIES)
      {
        prev_timeslot = candidate.genotype.at(var);

        candidate.genotype.at(var) = used_timeslots.at(rand() % used_timeslots.size());
        retries++;

        // Iterate until we find a feasible solution
        if (candidate.is_feasible())
          break;
        else
          candidate.genotype.at(var) = prev_timeslot; // Reverse the change
      }

      recalculate_used_timeslots();
      candidate.calculate_aptitude();

      // If the new genotype has a better aptitude than the current best,
      // we make the swap in the current solution. We do not copy the candidate
      // over the current solution for performance reasons
      if (candidate.aptitude < aptitude)
      {
        genotype.at(var) = candidate.genotype.at(var);

        recalculate_used_timeslots();
        calculate_aptitude();
      }
      else
        candidate.genotype.at(var) = prev_timeslot; // Reverse the change
    }
  }

  // Prints the solution in stdout
  void print()
  {
    int i=0;
    for (std::vector<int>::iterator exam = genotype.begin() ; exam != genotype.end(); ++exam, ++i)
    {
      std::cout << " " << *exam;
    }
    std::cout << " | " << aptitude << "\n";
  }

  void print_conflicts()
  {
    // Print Conflicts
    std::cout << "\nConflicts: ";
    for (int i = 0; i < total_exams; ++i)
      for (int j = i + 1; j < total_exams; ++j)
        if (conflicts.at(i).at(j) > 0 && genotype.at(i) == genotype.at(j)) // If there is a conflict
          std::cout << " " << i << "&" << j << " ";
  }
};
bool Solution::operator< (Solution param) {
  return (aptitude < param.aptitude);
}

std::vector< Solution* > population;

// Fills the conflicts matrix
void fill_conflicts();

// Prints the population
void print_pop();

int get_total_exams();
void generate_population();
void destroy_population();

Solution* select_best_solution();

bool compare_sol(Solution* a, Solution* b)
{
  return (*a).aptitude < (*b).aptitude;
}

int main ()
{
  srand(time(NULL)); // Sets a random seed

  std::vector<Solution*> elite_solutions;
  total_exams = get_total_exams();

  fill_conflicts();

  generate_population();
    print_pop();

  for (int cur_generation = 0; cur_generation < MAX_GENERATIONS; ++cur_generation)
  {
    std::sort(population.begin(), population.end(), compare_sol);

    std::cout << "Generation " << cur_generation << ", Best: " << population.front()->aptitude << " Worst: " << population.back()->aptitude << "\n";
    // print_pop();
    // We select the best solutions
    for (std::vector< Solution* >::iterator solution = population.begin(); solution != population.begin() + ELITIST_SELECT; ++solution)
    {
      Solution* best = new Solution(*(*solution));
      elite_solutions.push_back(best);
    }

    for (std::vector< Solution* >::iterator solution = population.begin() ; solution != population.end(); ++solution)
    {
      if (rand() % 100 / 100.0 < PROB_CLIMB)
      {
        (*solution)->hill_climb();
      }
      (*solution)->mutate();
    }

    std::sort(population.begin(), population.end(), compare_sol);
    for (std::vector< Solution* >::iterator elite = elite_solutions.begin(), solution = population.end() - 1;
      solution != population.end() - ELITIST_SELECT; --solution, ++elite)
    {
      free(*solution);
      *solution = *elite;
    }

    elite_solutions.clear();
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