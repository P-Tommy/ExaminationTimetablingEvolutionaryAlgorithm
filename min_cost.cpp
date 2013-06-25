#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <map>
#include <string>
#include <ctime>
#include <boost/tokenizer.hpp>

// Penalizations
#define W1 16
#define W2 8
#define W3 4
#define W4 2
#define W5 1

#define POP_SIZE 10
#define MAX_GENERATIONS 1000 // Number of total generations before stopping

#define PROB_MUTATION 0.05 // Probability of a mutation in one gene of a solution
#define PROB_CLIMB 0.2 // Probability of a HC ocurring

#define HC_ITERATIONS 30 // Maximum of iterations in the AC algorithm
#define MAX_FEASIBLE_RETRIES 20 // Maximum retries for a feasible solution

#define TOURNAMENT_SIZE 3 // Size of the tournaments for the selection algorithn

int total_exams; // Number of exams
int total_students; // Number of students
int timeslots; // For minimizing cost there is a fixed number of timeslots
std::string dataset_name; // Name of the dataset

// The conflict matrix saves in each cell (i,j) the number of students
// with a conflict between the exams i and j
std::vector< std::vector<int> > conflicts;

// Represents a solution
class Solution
{
private:
  // Assigns the timeslot passed by parameter to the exam that is in the genotype's location
  // exam
  void assign_timeslot(int exam, int timeslot)
  {
    genotype.at(exam) = timeslot;
  }

  void swap_exams(int a, int b)
  {
    int tmp = genotype.at(a);
    assign_timeslot(a, genotype.at(b));
    assign_timeslot(b, tmp);
  }

public:
  float aptitude;

  // Each gen in the genotype saves the Timeslot to which the exam is assigned
  std::vector<int> genotype;

  // Generates a random solution
  Solution()
  {
    for (int i = 0; i < total_exams; ++i)
    {
      // Fill every exam with it's own timeslot to generate a solution
      int random = (int)(((float) timeslots)*rand()/(RAND_MAX + 1.0));
      genotype.push_back(random);
    }

    calculate_aptitude();
  }

  bool operator<(Solution);

  // Calculates the solution aptitude (and sets it)
  void calculate_aptitude()
  {
    aptitude = 0;

    for (int i = 0; i < total_exams; ++i)
    {
      for (int j = i + 1; j < total_exams; ++j)
      {
        if (conflicts.at(i).at(j) > 0) // If there is a conflict
        {
          // Calculate the distance (timeslots in between) between the two conflicting exams
          int distance = abs(genotype.at(i) - genotype.at(j));

          // Depending on the distance, we assign a different penalization
          // and it gets amplified by the number of students with the same conflict
          switch (distance)
          {
          // case 0:
          //   aptitude += W0 * conflicts.at(i).at(j);
          //   break;
          case 1:
            aptitude += W1 * conflicts.at(i).at(j);
            break;
          case 2:
            aptitude += W2 * conflicts.at(i).at(j);
            break;
          case 3:
            aptitude += W3 * conflicts.at(i).at(j);
            break;
          case 4:
            aptitude += W4 * conflicts.at(i).at(j);
            break;
          case 5:
            aptitude += W5 * conflicts.at(i).at(j);
            break;
          }
        }
      }
    }

    aptitude /= total_students;
  }

  void mutate()
  {
    for (int exam = 0; exam < total_exams; ++exam)
      if ( (int)((1.0)*rand()/(RAND_MAX + 1.0)) < PROB_MUTATION)
        assign_timeslot(exam, (int)(( (float) timeslots )*rand()/(RAND_MAX + 1.0)));

    calculate_aptitude();
  }

  void hill_climb()
  {
    Solution candidate = *this;

    for (int i = 0; i < HC_ITERATIONS; ++i)
    {
      int exam1 = (int)(( (float) total_exams )*rand()/(RAND_MAX + 1.0));
      int exam2 = (int)(( (float) total_exams )*rand()/(RAND_MAX + 1.0));

      candidate.swap_exams(exam1, exam2);

      candidate.calculate_aptitude();

      // If the new genotype has a better aptitude than the current best,
      // we make the swap in the current solution. We do not copy the candidate
      // over the current solution for performance reasons
      if (candidate.aptitude < aptitude)
      {
        swap_exams(exam1, exam2);
        calculate_aptitude();
        break;
      } else
        candidate.swap_exams(exam1, exam2); // Reverse the change
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
};
bool Solution::operator< (Solution param) {
  return (aptitude < param.aptitude);
}

std::vector< Solution > population;

// Fills the conflicts matrix
void fill_conflicts();

// Prints the population
void print_pop();

int get_total_exams();
int get_total_students();
void generate_population();
void destroy_population();

Solution select_best_solution();
void selection();

// Return true if the solution a has a better (less or equal) aptitude than b
bool compare_solutions(const Solution a, const Solution b);

int main(int argc, char* argv[])
{
  srand(time(NULL)); // Sets a random seed

  std::vector<std::string> params(argv, argv+argc);
  if (argc < 3) { // Missing arguments
    std::cerr << "Usage: " << argv[0] << " DATASET_NAME #TIMESLOTS" << std::endl;
    return 1;
  }

  dataset_name = params.at(1);
  timeslots = atoi(params.at(2).c_str());

  total_exams = get_total_exams();
  total_students = get_total_students();

  fill_conflicts();

  const clock_t begin_time = clock();
  generate_population();

  Solution best = select_best_solution();
  for (int cur_generation = 0; cur_generation < MAX_GENERATIONS; ++cur_generation)
  {
    std::cout << "Generation " << cur_generation << ", Best: " << best.aptitude << "\n";
    std::cout << "Elapsed time: " << float( clock () - begin_time ) /  (CLOCKS_PER_SEC/1000) << "ms\n";

    // Select the best solution for Elitism
    best = select_best_solution();

    // Make a selection of the current population to generate the next one
    selection();

    std::vector< Solution > new_population;
    for (int i = 0; i < POP_SIZE - 1; ++i)
    {
      if ( (int)((1.0)*rand()/(RAND_MAX + 1.0)) < PROB_CLIMB)
      {
        population.at(i).hill_climb();
      }
      population.at(i).mutate();

      new_population.push_back(population.at(i));
    }

    // Elitism, keep the best solution from the last generation
    new_population.push_back(best);

    population = new_population;
  }

  best = select_best_solution();

  std::cout << "Time used: " << float( clock () - begin_time ) /  (CLOCKS_PER_SEC/1000) << "ms\n";
  std::cout << "Best solution: ";
  best.print();

  return 0;
}

void selection()
{
  std::vector< Solution > new_population;
  std::vector< Solution > selected_solutions; // Solutions selected for the tournament

  for (int i = 0; i < POP_SIZE; ++i) // Generate POP_SIZE individuals
  {
    selected_solutions.clear(); // Start with no solutions
    for (int j = 0; j < TOURNAMENT_SIZE; ++j)
    {
      // Select a random solution from the population
      selected_solutions.push_back(population.at((int)(((float) POP_SIZE - 1)*rand()/(RAND_MAX + 1.0))));
    }

    // Select the best solution from the selected ones
    std::sort(selected_solutions.begin(), selected_solutions.end(), compare_solutions);

    new_population.push_back(selected_solutions.front()); // And add it to the new population
  }

  // The new population is now the real one
  population = new_population;
}

bool compare_solutions(const Solution a, const Solution b)
{
  return a.aptitude <= b.aptitude;
}

void print_pop()
{
  std::cout << "Sol: ";
  for (std::vector< Solution >::iterator solution = population.begin() ; solution != population.end(); ++solution)
  {
    std::cout << (*solution).aptitude << " ";
  }
  std::cout << "\n";
}

Solution select_best_solution()
{
  Solution *best = &(*population.begin());

  for (std::vector< Solution >::iterator solution = population.begin() + 1; solution != population.end(); ++solution)
    if ((*solution).aptitude < best->aptitude)
      best = &(*solution);

  return *best;
}

void fill_conflicts()
{
  conflicts.resize(total_exams);

  // Initializes the matrix
  for (int i = 0; i < total_exams; ++i)
    conflicts[i].resize(total_exams);

  // Read the students file to fill the conflicts matrix
  std::string filename = dataset_name + ".stu";
  std::ifstream infile(filename.c_str());

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
  std::string filename = dataset_name + ".crs";
  std::ifstream exams_file(filename.c_str());

  // Count the newlines to get the total of exams
  exams_file.unsetf(std::ios_base::skipws); // Don't skip newlines
  int total_exams = std::count(
                           std::istream_iterator<char>(exams_file),
                           std::istream_iterator<char>(),
                           '\n');

  return total_exams;
}

int get_total_students()
{
  std::string filename = dataset_name + ".stu";
  std::ifstream students_file(filename.c_str());

  // Count the newlines to get the total of exams
  students_file.unsetf(std::ios_base::skipws); // Don't skip newlines
  int total_students = std::count(
                           std::istream_iterator<char>(students_file),
                           std::istream_iterator<char>(),
                           '\n');

  return total_students;
}

void generate_population()
{
  population.resize(POP_SIZE);

  for (std::vector< Solution >::iterator solution = population.begin() ; solution != population.end(); ++solution)
  {
    *solution = Solution();
  }
}