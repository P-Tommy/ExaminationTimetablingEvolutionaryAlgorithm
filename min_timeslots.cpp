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

#define POP_SIZE 10
#define MAX_GENERATIONS 5000 // Number of total generations before stopping

#define PROB_MUTATION 0.05 // Probability of a mutation in one gene of a solution
#define PROB_CLIMB 0.25 // Probability of a HC ocurring

#define HC_ITERATIONS 100 // Maximum of iterations in the AC algorithm
#define MAX_FEASIBLE_RETRIES 30 // Maximum retries for a feasible solution

#define TOURNAMENT_SIZE 3 // Size of the tournaments for the selection algorithn

int total_exams; // Number of exams
std::string dataset_name;

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
    exams_in_timeslot[genotype.at(exam)]--;
    genotype.at(exam) = timeslot;
    exams_in_timeslot[timeslot]++;
  }

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
        random = (int)(((float) total_exams-1)*rand()/(RAND_MAX + 1.0));
        genotype.push_back(random);
        if (is_feasible_up_to(i))
          break;
        else
          genotype.pop_back();
      } while(true);
      exams_in_timeslot[random]++;
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

  // Checks every timeslot and checks if it's being
  // used by an exam. If it is, it's added to the
  // used_timeslots vector.
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

  // Returns true if the solution is feasible, considering
  // only one exam compared to the others
  bool is_feasible(int exam)
  {
    for (int i = 0; i < total_exams; ++i)
      if (exam != i &&
        conflicts.at(exam).at(i) > 0 &&
        genotype.at(exam) == genotype.at(i)) // If there is a conflict
        return false; // Solution not feasible

    return true;
  }

  // Returns true if the solution is feasible considering
  // the exams up to the passed exam
  bool is_feasible_up_to(int exam)
  {
    for (int i = 0; i < (int) genotype.size(); ++i)
      if (i != exam && conflicts.at(exam).at(i) > 0 && genotype.at(exam) == genotype.at(i)) // If there is a conflict
        return false; // Solution not feasible

    return true;
  }

  // Iterate
  void mutate()
  {
    for (int exam = 0; exam < total_exams; ++exam)
    {
      if ( ((1.0)*rand()/(RAND_MAX + 1.0)) < PROB_MUTATION)
      {
        // Mutate until we have a feasible solution
        for (int retries = 0; retries < MAX_FEASIBLE_RETRIES; ++retries)
        {
          int prev_timeslot = genotype.at(exam);

          assign_timeslot(exam, (int)(( (float) total_exams-1 )*rand()/(RAND_MAX + 1.0)));
          if(is_feasible(exam))
            break;
          else // Undo the changes
          {
            assign_timeslot(exam, prev_timeslot);
          }
        };
      }
    }

    recalculate_used_timeslots();
    calculate_aptitude();
  }

  void hill_climb()
  {
    for (int i = 0; i < HC_ITERATIONS; ++i)
    {
      Solution candidate = *this;

      int var = (int)(( (float) total_exams-1 )*rand()/(RAND_MAX + 1.0));

      for(std::vector<int>::iterator timeslot = used_timeslots.begin(); timeslot != used_timeslots.end(); ++timeslot)
      {
        candidate.assign_timeslot(var, *timeslot);
        candidate.recalculate_used_timeslots();
        candidate.calculate_aptitude();

        // If the new genotype has a better aptitude than the current best,
        // we make the swap in the current solution. We do not copy the candidate
        // over the current solution for performance reasons
        if (candidate.is_feasible(var) && candidate.aptitude < aptitude)
        {
          assign_timeslot(var, *timeslot);

          used_timeslots = candidate.used_timeslots;
          aptitude = candidate.aptitude;

          // std::cout << aptitude << std::endl;
          break;
        }
      }

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

std::vector< Solution > population;

// Fills the conflicts matrix
void fill_conflicts();

// Prints the population
void print_pop();

int get_total_exams();
void generate_population();
void destroy_population();

Solution select_best_solution();
void selection();

// Return true if the solution a has a better (less or equal) aptitude than b
bool compare_solutions(const Solution a, const Solution b);

int main(int argc, char* argv[])
{
  srand(10000);
  // srand(123456);
  // srand(854234);
  // srand (time(NULL)); // Sets a random seed

  std::vector<std::string> params(argv, argv+argc);
  if (argc < 2) { // Missing arguments
    std::cerr << "Usage: " << argv[0] << " DATASET_NAME" << std::endl;
    return 1;
  }

  dataset_name = params.at(1);

  total_exams = get_total_exams();

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
      if ( ((1.0)*rand()/(RAND_MAX + 1.0)) < PROB_MUTATION)
        population.at(i).mutate();

      if ( ((1.0)*rand()/(RAND_MAX + 1.0)) < PROB_CLIMB)
        population.at(i).hill_climb();

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

  if(!infile.is_open()) {
    std::cout << "Error reading file" << std::endl;
    exit(0);
  }

  // We parse every line of the file, filling the conflict matrix
  std::string line;

  while (std::getline(infile, line))
  {

    // We tokenize the line and mark as conflicts every pair of exams
    // that appears in the same line
    std::stringstream strstr(line);
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    std::vector<std::string> exams(it, end);

    // We iterate on all the remaining exams in the line, after the current one
    // and mark the couple (current, next) and (next, current) as conflicted
    for (std::vector<std::string>::iterator exam = exams.begin() ; exam != exams.end(); ++exam)
      for (std::vector<std::string>::iterator other_exam = exam + 1; other_exam != exams.end(); ++other_exam)
      {
        conflicts[atoi((*exam).c_str()) - 1][atoi((*other_exam).c_str()) - 1]++;
        conflicts[atoi((*other_exam).c_str()) - 1][atoi((*exam).c_str()) - 1]++;
      }
  }

  infile.close();
}

int get_total_exams()
{
  std::string filename = dataset_name + ".crs";
  std::ifstream exams_file(filename.c_str());

  if(!exams_file.is_open()) {
    std::cout << "Error reading file" << std::endl;
    exit(0);
  }

  // Count the newlines to get the total of exams
  exams_file.unsetf(std::ios_base::skipws); // Don't skip newlines
  unsigned total_exams = std::count(
                           std::istream_iterator<char>(exams_file),
                           std::istream_iterator<char>(),
                           '\n');
  exams_file.close();

  return total_exams;
}

void generate_population()
{
  population.resize(POP_SIZE);

  for (std::vector< Solution >::iterator solution = population.begin() ; solution != population.end(); ++solution)
  {
    *solution = Solution();
  }
}