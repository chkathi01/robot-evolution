/* Chandu Kathi
RobotEvolution.txt
May 18th */

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <iomanip>

using namespace std;

const int NUMBER_OF_GENES = 16;
/*  '+' = battery 
    '0' = emptySpace 
    '|' = wall */
const char SENSOR_CODES[3] = {'+', '0', '|'};
/* 'n'orth, 's'outh, 'e'ast, 'w'est */
const char MOVE_CODES[4] = {'n', 's', 'e', 'w'};

//***************************************************
// Class Name: Map
//********************************************************
class Map {
private:
  int batteries = 0;

public:
  int robotRow = 0;
  int robotColumn = 0;
  char *sensorCodes = new char[4];
  char **grid = new char *[12];

  Map();
  ~Map();
  void componentTest();
  void displayGrid();
  void displaySensoryCodes();
  char getMovementLocation(char);
  void initializeMap();
  void moveRobot(char);
  void place();
  void setSensorCodes();
};
Map::Map() {
  // The map is created
  initializeMap();

  // the batteries and robot is placed
  place();
}
Map::~Map() {
  delete [] sensorCodes;
  delete [] grid;
}
void Map::displayGrid() {
  // Grid is displayed
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 12; j++)
      cout << grid[i][j];
    cout << endl;
  }

  cout << endl;
}
void Map::displaySensoryCodes() {
  // North, South, East, West codes are displayed
  cout << "Sensory Code: ";
  for (int i = 0; i < 4; i++)
    cout << sensorCodes[i];
  cout << endl << endl;
}
char Map::getMovementLocation(char movement) {
  /* gets the direction and returns the location where the robot might move or 
  stay */
  char movementLocation = '!';
  switch (movement) {
    case 'n':  // North location
      movementLocation = grid[robotRow - 1][robotColumn];
      break;
    case 's':  // South location
      movementLocation = grid[robotRow + 1][robotColumn];
      break;
    case 'w':  // west location
      movementLocation = grid[robotRow][robotColumn - 1];
      break;
    case 'e':  // east location
      movementLocation = grid[robotRow][robotColumn + 1];
      break;
  }

  return movementLocation;
}
void Map::initializeMap() {
  /* the grid is created. 12 by 12 big grid with a 10 by 10 grid for the robot 
    to move. The outside ring is set to '|' (wall char) */
  for (int i = 0; i < 12; i++) {
    grid[i] = new char[12];
    if (i == 0 || i == 11)
      for (int j = 0; j < 12; j++)
        grid[i][j] = '|';
    else {
      for (int j = 0; j < 12; j++) {
        if (j == 0 || j == 11)
          grid[i][j] = '|';
        else
          grid[i][j] = '0';
      }
    }
  }
}
void Map::moveRobot(char movement) {
  /* Based on the movement character the robots moves in the certain direction

     Row, column values are changed and previous grid location is set to empty */
  if (movement == 'n') {
    grid[robotRow][robotColumn] = '0';
    grid[robotRow - 1][robotColumn] = 'R';
    robotRow -= 1;
  } else if (movement == 's') {
    grid[robotRow][robotColumn] = '0';
    grid[robotRow + 1][robotColumn] = 'R';
    robotRow += 1;
  } else if (movement == 'w') {
    grid[robotRow][robotColumn] = '0';
    grid[robotRow][robotColumn - 1] = 'R';
    robotColumn -= 1;
  } else if (movement == 'e') {
    grid[robotRow][robotColumn] = '0';
    grid[robotRow][robotColumn + 1] = 'R';
    robotColumn += 1;
  }
}
void Map::place() {
  bool robotPlaced = false;
  int row = 0;
  int column = 0;

  // 40% of the grid is batteries.. batteries are place using this loop
  while (batteries != 40) {
    row = rand() % 12;
    column = rand() % 12;
    // if there is already a battery then find another square
    if (grid[row][column] == '+' || grid[row][column] == '|')
      continue;
    // otherwise place the battery
    grid[row][column] = '+';
    batteries++;
  }

  // Place the robot in a empty spot  
  while (!robotPlaced) {
    row = rand() % 12;
    column = rand() % 12;
    if (grid[row][column] != '0')
      continue;
    
    grid[row][column] = 'R';

    // set robot location and set sensory codes
    robotRow = row;
    robotColumn = column;
    setSensorCodes();
    robotPlaced = true;
  }
}
void Map::setSensorCodes() {
  // Surroundings codes are set
  sensorCodes[0] = grid[robotRow - 1][robotColumn]; // North
  sensorCodes[1] = grid[robotRow + 1][robotColumn]; // South
  sensorCodes[2] = grid[robotRow][robotColumn + 1]; // East
  sensorCodes[3] = grid[robotRow][robotColumn - 1]; // West
}


//***************************************************
// Class Name: Robot
//********************************************************
class Robot {
  friend Map;
  friend float averageFitness(Robot*, int);
  friend void displayBestStats(int, float, Robot &);
  friend void sortRobots(Robot*&, int, Robot&);
  
  private:
    int fitness = 0;        // batteries consumed
    int greatestFitness = 0;
    int power = 5;          // power is the same as energy for the robot
    int turnsSurvived = 0;  // turns survived
    int greatestTurnsSurvived = 0;
    char **genes = new char *[NUMBER_OF_GENES];

  public:
    // genes are randomly generated for new robot
    Robot() { randomGeneGenerator(); }
    ~Robot();

    // Overload Assignment operator (Deep Copy)
    Robot &operator=(const Robot &robot) {  
      fitness = robot.fitness; 
      greatestFitness = robot.greatestFitness;
      turnsSurvived = robot.turnsSurvived;
      greatestTurnsSurvived = robot.greatestTurnsSurvived;
      power = robot.power;

      // Copies over the gene characters
      for (int i = 0; i < NUMBER_OF_GENES; i++) {
        for (int j = 0; j < 5; j++) {
          genes[i][j] = robot.genes[i][j];
        }
      }
  
      return *this;
    };

    // Overload + Operator
    Robot operator+(const Robot &parent2) {
      Robot child;

      // first half of the genes come from this robot
      for (int i = 0; i < NUMBER_OF_GENES/2; i++) {
        for (int j = 0; j < 5; j++) {
          child.genes[i][j] = genes[i][j];
        }
      }

      // second half of the genes comes from the second parent
      for (int i = NUMBER_OF_GENES/2; i < NUMBER_OF_GENES; i++) {
        for (int j = 0; j < 5; j++) {
          child.genes[i][j] = parent2.genes[i][j];
        }
      }
      
      return child;
    }
    int checkGenes(char *sensorCodes);
    void componentTest();
    void displayGenes();
    int getGreatestFitness();
    int getGreatestTurnsSurvived();
    void mutation();
    void randomGeneGenerator();
    void simulation();
};
Robot::~Robot() {
  delete [] genes;
}
int Robot::checkGenes(char *sensorCodes) {
  // the sensorCode array is checked with the genes to see if there is match
  int gene = 15;
  for (int i = 0; i < NUMBER_OF_GENES; i++) {
    for (int j = 0; j < 4; j++) {
      if (sensorCodes[j] != genes[i][j] && j != 3)
        break;

      // if match is found then it is returned
      else if (sensorCodes[j] == genes[i][j] && j == 3) {
        gene = i;
        break;
      }
    }
  }

  return gene;
}
void Robot::componentTest() {
  Robot test;
  cout << "Entering Compoenent Robot Class Componenet Test ...\n\n";
  cout << "Displaying Robot Genes...\n";
  test.displayGenes();
  cout << "\nDisplaying & Generating Genes - PASSED\n";
  cout << "\n**Constructor generated the random genes.\n";
}
void Robot::displayGenes() {
  // genes are displayed 1 - 16
  for (int i = 0; i < NUMBER_OF_GENES; i++) {
    cout << "Gene " << i + 1 << ": ";
    for (int j = 0; j < 5; j++)
      cout << genes[i][j];
    cout << endl;
  }

  cout << endl;
}
int Robot::getGreatestFitness() {return greatestFitness;}
int Robot::getGreatestTurnsSurvived() {return greatestTurnsSurvived;}
void Robot::mutation(){
  // mutation causes a shift in the genes. All genes move up
  char temp [5] = {'\0', '\0', '\0', '\0', '\0'};

  // copy the first gene in temp array
  for (int j = 0; j < 5; j++) {
    temp[j] = genes[0][j];
  }

  // shift genes up
  for (int i = 0; i < NUMBER_OF_GENES - 1; i++){
    for (int j = 0; j < 5; j++) {
      genes[i][j] = genes[i + 1][j];
    }
  }

  // top genes moves to the bottom
  for (int j = 0; j < 5; j++) {
    genes[15][j] = temp[j];
  }
}
void Robot::randomGeneGenerator() {
  // random genes are generated for the new robot
  for (int i = 0; i < NUMBER_OF_GENES; i++) {
    genes[i] = new char[5];

    for (int j = 0; j < 5; j++) {
      if (j < 4) {
        genes[i][j] = SENSOR_CODES[rand() % 3];
        continue;
      }
      genes[i][j] = MOVE_CODES[rand() % 4];
    }
  }
}
void Robot::simulation() {
  Map robotMap;
  int geneNumber = -1;
  char movement = '=';

  // Values are reset before each simulation
  fitness = 0;
  turnsSurvived = 0;
  power = 5;
  
  while (power != 0) {
    // surroundings and genes are compared to determine movement
    geneNumber = checkGenes(robotMap.sensorCodes);
    movement = genes[geneNumber][4];

    // if wall is encountered, minus 1 power and stay put
    if (robotMap.getMovementLocation(movement) == '|') {
      power--;
      turnsSurvived++;
      continue;

      // if battery is encountered, power and fitness are increased
    } else if (robotMap.getMovementLocation(movement) == '+') {
      power += 5;
      fitness++;

      // if empty space is encountered, minus 1 power and move
    } else if (robotMap.getMovementLocation(movement) == '0'){
      power--;
    } 
    
    turnsSurvived++;

    // Only happens if '0' or '+' are encountered
    robotMap.moveRobot(movement);
    robotMap.setSensorCodes();
  }
  if (fitness > greatestFitness) greatestFitness = fitness;
  if (turnsSurvived > greatestTurnsSurvived) greatestTurnsSurvived = turnsSurvived;
}

// Function Prototypes -----------------------------------------
float averageFitness(Robot*, int);
void breedRobots(Robot*& robotGeneration, int generationSize);
void displayBestStats(int bestGeneration, float greatestAverageFitness, Robot & bestRobot);
float generation(Robot*& robotGeneration, int generationSize, Robot& bestRobot);
void mutateRobots(Robot*& robotGeneration, int generationSize);
void programGreeting();
void simulation(Robot*& robotGeneration, int generationSize);
void sortRobots(Robot*& robotGeneration, int generationSize, Robot& bestRobot);

// Main Function -----------------------------------------
int main() {
  srand(time(NULL));
  const int NUMBER_OF_GENERATIONS = 7500;
  float averageFitness = 0.00;
  float greatestAverageFitness = 0.00;
  int bestGeneration = 0;
  int generationSize = 200;
  int count = 0;

  programGreeting();
  
  Robot bestRobot;
  Robot* robotGeneration = new Robot[generationSize];

  while (count != NUMBER_OF_GENERATIONS) {
    cout << "Generation " << count + 1 << ":    ";
    averageFitness = generation(robotGeneration, generationSize, bestRobot);

    if (averageFitness > greatestAverageFitness) {
      bestGeneration = count;
      greatestAverageFitness = averageFitness;
    }
    
    count++;
  }

  displayBestStats(bestGeneration, greatestAverageFitness, bestRobot);

  
  delete [] robotGeneration;
}

//***************************************************
// Function Name: averageFitness
//********************************************************
float averageFitness(Robot* robotGeneration, int generationSize) {
  // Add all fitness of robot generation and calculates average fitness
  float totalFitness = 0;
  float averageFitness = 0;

  for (int i = 0; i < generationSize; i++) {
    totalFitness += robotGeneration[i].fitness; 
  }
  
  averageFitness = totalFitness / generationSize;
  return averageFitness;
}

//***************************************************
// Function Name: breedRobots
//********************************************************
void breedRobots(Robot*& robotGeneration, int generationSize) {
  int parent1 = 0;
  int parent2 = 1;

  /*  Loop starts half way. Breeds the first two parents. These two parents     
      produces 2 offspring. Next two parents produce 2 offspring and so on.
  */
  for (int i = (generationSize/2); i < generationSize; i++) {
    robotGeneration[i] = robotGeneration[parent1] + robotGeneration[parent2];
    i++;
    robotGeneration[i] = robotGeneration[parent2] + robotGeneration[parent1];
    parent1 += 2; 
    parent2 += 2;
  }
}

//***************************************************
// Function Name: displayBestStats
//********************************************************
void displayBestStats(int bestGeneration, float greatestAverageFitness, Robot & bestRobot) {
  cout << fixed << showpoint << setprecision(2);
  /* Displays the generation with the best fitness */
  cout << "\nBest Generation is #" << bestGeneration + 1 << ":    ";
  cout << "\n\tGreatest Average Fitness: " ;
  cout << greatestAverageFitness << endl << endl;

  /*Displays the statistics and genes of the best robot */
  cout << "Best Robot Stats and Genes are displayed below..." << endl;
  cout << "\tBest Robot Greatest Fitness: ";
  cout << bestRobot.greatestFitness << endl;
  cout << "\tBest Robot Greatest Turns Survived: "; 
  
  cout << bestRobot.greatestTurnsSurvived << endl << endl;
  bestRobot.displayGenes();
}

//***************************************************
// Function Name: programGreeting
//********************************************************
void programGreeting() {
  string temp;

  cout << "Welcome to the Robot Evolution Simulation!\n";
  cout << "Author: Chandu Kathi\n\n"; 

  cout << "This program is a reproduction of an experiment first conducted at Harvard University in 1968. This program shows the effect of evolution on a popualation of Robots that try to maneuver around a 10 x 10 grid looking for batteries.\n\n";
  
  cout << "The key is the robot behavior. The robots each have 16 genes that will help them navigate throught their map. When a robot collects batteries, it's fitness increases. After generation, the average fitness of all the robots is calculated is displayed. At the end of the simulation is over, the best average fitness is display along with the best robot stats to see how the robot has learned to navigate in a given situation.\n";
  
  // Code from Weston G.'s previous program
  cout << "\n\nPress enter to start the simulation...";
  getline(cin, temp);
}

//***************************************************
// Function Name: mutateRobots
//********************************************************
void mutateRobots(Robot*& robotGeneration, int generationSize) {
  int count = 0;
  int robotPosition = 0;

  // 5% of the robot's genes are mutated
  while (count <= (generationSize * 0.05)) {
    robotPosition = rand() % generationSize;
    robotGeneration[robotPosition].mutation();
    count++;
  }
}

//***************************************************
// Function Name: generation
//********************************************************
float generation(Robot*& robotGeneration, int generationSize, Robot& bestRobot) {
  /*  consists of the robots running the map, the average fitness being 
      calculated, the robots being sorted, bred, and mutated. 
  */
  float value = 0.00;
  simulation(robotGeneration, generationSize);  
  value = averageFitness(robotGeneration, generationSize);
  cout << "Average Fitness: " << value << endl;
  
  sortRobots(robotGeneration, generationSize, bestRobot);
  breedRobots(robotGeneration, generationSize);
  mutateRobots(robotGeneration, generationSize);

  return value;
}

void simulation(Robot*& robotGeneration, int generationSize) {
  // run simulations for all the robot
  for (int i = 0; i < generationSize; i++) robotGeneration[i].simulation();
}

//***************************************************
// Function Name: sortRobots
//********************************************************
void sortRobots(Robot*& robotGeneration, int generationSize, Robot& bestRobot){
  // Sorts robots based on the fitness
  Robot temp;

  for (int i = 0; i < generationSize ; i++)  { 
    for (int j = i + 1; j < generationSize; j++) {
      // swap robots to put the greater fitness robot first
      if (robotGeneration[i].fitness < robotGeneration[j].fitness) {
        temp = robotGeneration[i];
        robotGeneration[i] = robotGeneration[j];
        robotGeneration[j] = temp;
      }
    }
  }

  if (robotGeneration[0].greatestFitness > bestRobot.greatestFitness) 
    bestRobot = robotGeneration[0];
}