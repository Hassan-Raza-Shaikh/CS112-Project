#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include <iomanip>
#include <cstdlib>
#include <algorithm>
#include "instructions.cpp"
using namespace std;

/**
* @file code_wd_buildings.cpp
* @brief Implementation of the Eco City game buildings and game mechanics
*
* This file contains the implementation of various buildings in the Eco City game,
* including their functionality, interactions, and game state management.
*/

// --------- Game Constants ---------
/**
* @brief Cost and reward constants for various game actions
*/
const int HOUSE_EAT_COST = 20; ///< Cost to eat at home
const int HOSPITAL_TREATMENT_COST = 50; ///< Cost for hospital treatment
const int OFFICE_WORK_REWARD = 100; ///< Reward for working at office
const int RESTAURANT_MEAL_COST = 50; ///< Cost for restaurant meal
const int RESTAURANT_ECO_POINTS = 20; ///< Eco points gained from restaurant
const int SCHOOL_STUDY_ECO_POINTS = 10; ///< Eco points gained from studying
const int BANK_INTEREST_RATE = 2; ///< Bank interest rate percentage
const int CASINO_BET_AMOUNT = 100; ///< Amount to bet at casino
const int CASINO_WIN_REWARD = 200; ///< Reward for winning at casino
const int HOUSE_RELAX_ECO_POINTS = 5; ///< Eco points gained from relaxing at home

/**
* @brief Health and hunger constants
*/
const int MAX_HEALTH = 100; ///< Maximum health value
const int MAX_HUNGER = 100; ///< Maximum hunger value
const int HUNGER_DECREASE_RATE = 2; ///< Hunger decrease per minute
const int HEALTH_DECREASE_RATE = 1; ///< Health decrease when hungry
const int HOSPITAL_HEALTH_RESTORE = 30; ///< Health restored at hospital
const int HOUSE_SLEEP_HEALTH_RESTORE = 20; ///< Health restored while sleeping
const int HOUSE_EAT_HUNGER_RESTORE = 30; ///< Hunger restored while eating

/**
* @brief Level and pollution constants
*/
const int LEVEL_POINTS_PER_ECO = 1; ///< Level points per eco point
const int POLLUTION_PENALTY = 2; ///< Pollution penalty multiplier
const int LEVEL_UP_THRESHOLD = 100; ///< Level points needed to level up

/**
* @brief Transportation delay constants in seconds
*/
const int WALK_DELAY = 10; ///< Walking delay in seconds
const int CYCLE_DELAY = 5; ///< Cycling delay in seconds
const int CAR_DELAY = 0; ///< Car delay in seconds

/**
* @brief Periodic reward constants
*/
const int PERIODIC_FUNDS_REWARD = 10; ///< Funds gained periodically
const int PERIODIC_FUNDS_INTERVAL = 60; ///< Interval for periodic rewards in seconds
const int OFFLINE_BONUS_PER_DAY = 1000; ///< Offline bonus per day

// Update Transport Constants
const int CYCLE_COST = 100;
const int CAR_COST = 300;
const int ELECTRIC_CAR_COST = 500;
const int CYCLE_REQUIRED_LEVEL = 2;
const int CAR_REQUIRED_LEVEL = 4;
const int ELECTRIC_CAR_REQUIRED_LEVEL = 6;
const int CAR_ECO_PENALTY = 5;
const int CAR_POLLUTION_INCREASE = 10;
const int ELECTRIC_CAR_ECO_BONUS = 5;

// New Restaurant Constants
const int BASIC_MEAL_COST = 10;
const int BASIC_MEAL_HUNGER = 10;
const int BASIC_MEAL_ECO_PENALTY = 5;

const int INTERMEDIATE_MEAL_COST = 25;
const int INTERMEDIATE_MEAL_HUNGER = 20;

const int PREMIUM_MEAL_COST = 50;
const int PREMIUM_MEAL_ECO_POINTS = 20;
const int PREMIUM_MEAL_HUNGER = 35;

// New Environment Constants
const int TREE_COST = 10;
const int TREE_ECO_POINTS = 10;
const int TREE_POLLUTION_REDUCTION = 10;

// Update Transport System
bool hasBicycle = false;
bool hasCar = false;
bool hasElectricCar = false;

// Add House Relaxation Constant
const int HOUSE_RELAX_TIME_ACCELERATION = 2; ///< Time acceleration factor while relaxing

// --------- Function Declarations ---------
void clearScreen();
void displayStatusBar(int value, int max, const string& label, const string& color);
void displayStatus(int funds, int health, int hunger, int level, int levelPoints, int ecoPoints, int pollutionLevel);
void updateLevel(int& level, int& levelPoints, int ecoPoints, int pollutionLevel);
void updateHungerAndHealth(int& health, int& hunger, time_t& lastUpdateTime);
void transport_delay(int vehicle, int& ecoPoints, int& pollutionLevel);
void update_funds_periodically(int &funds, time_t &lastUpdateTime);
void offline_bonus(int &funds, time_t &lastSaveTime);
void save_game(string filename, int level, int ecopoints, int funds, int house, int hospital, int office, int restaurant,
int school, int bank, int casino, int vehicle, int pollutionlevel, time_t lastSaveTime, int levelPoints);
bool load_game(string filename, int &level, int &ecopoints, int &funds,
int &house, int &hospital, int &office, int &restaurant,
int &school, int &bank, int &casino, int &vehicle, int &pollutionlevel, time_t &lastSaveTime, int &levelPoints);

// --------- Building Classes ---------
/**
* @class House
* @brief Represents the player's house in the game
*
* The house provides basic activities like sleeping, eating, and relaxing.
* It affects both funds and eco points.
*/
class House {
private:
int& funds;
int& ecoPoints;
int& health;
int& hunger;
public:
House(int& f, int& e, int& h, int& hu) : funds(f), ecoPoints(e), health(h), hunger(hu) {}

void enter() {
while (true) {
displayStatus(funds, health, hunger, 0, 0, ecoPoints, 0);
std::cout << "\n🏠 Welcome to your House!\n";
std::cout << "1. 🛌 Sleep (Restores " << HOUSE_SLEEP_HEALTH_RESTORE << " health)\n";
std::cout << "2. 🍴 Eat (Costs " << HOUSE_EAT_COST << " funds, Restores " << HOUSE_EAT_HUNGER_RESTORE << " hunger)\n";
std::cout << "3. 🎮 Relax (+" << HOUSE_RELAX_ECO_POINTS << " ecoPoints)\n";
std::cout << "4. 🚪 Exit House\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

switch (choice) {
case 1:
sleep();
break;
case 2:
eat();
break;
case 3:
relax();
break;
case 4:
std::cout << "🚶 Leaving House...\n";
return;
default:
std::cout << "Invalid option. Try again!\n";
}
}
}

private:
void sleep() {
std::cout << "😴 Sleeping... (5 seconds)\n";
std::this_thread::sleep_for(std::chrono::seconds(5));
health = min(health + HOUSE_SLEEP_HEALTH_RESTORE, MAX_HEALTH);
std::cout << "You feel refreshed! (Health: " << health << "/" << MAX_HEALTH << ")\n";
}

void eat() {
if (funds >= HOUSE_EAT_COST) {
std::cout << "🍽️ Eating a meal... (3 seconds)\n";
funds -= HOUSE_EAT_COST;
hunger = MAX_HUNGER; // Restores to full
std::this_thread::sleep_for(std::chrono::seconds(3));
std::cout << "You feel energized! (Hunger: " << hunger << "/" << MAX_HUNGER << ")\n";
} else {
std::cout << "❌ Not enough funds!\n";
}
}

void relax() {
std::cout << "🎮 Relaxing... (2 seconds)\n";
this_thread::sleep_for(std::chrono::seconds(2));
// Time passes faster while relaxing
funds += PERIODIC_FUNDS_REWARD * HOUSE_RELAX_TIME_ACCELERATION;
std::cout << "You feel happy! (Time passed, gained " << PERIODIC_FUNDS_REWARD * HOUSE_RELAX_TIME_ACCELERATION << " funds)\n";
}
};

/**
* @class Hospital
* @brief Represents the hospital building
*
* The hospital provides treatment services that cost funds.
*/
class Hospital {
private:
int& funds;
int& health;
public:
Hospital(int& f, int& h) : funds(f), health(h) {}

void enter() {
while (true) {
displayStatus(funds, health, 0, 0, 0, 0, 0);
std::cout << "\n🏥 Welcome to the Hospital!\n";
std::cout << "1. 💉 Get Treatment (Costs " << HOSPITAL_TREATMENT_COST << " funds, Restores " << HOSPITAL_HEALTH_RESTORE << " health)\n";
std::cout << "2. 🚪 Exit Hospital\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

if (choice == 1) {
if (funds >= HOSPITAL_TREATMENT_COST) {
std::cout << "💉 Treating... (3 seconds)\n";
funds -= HOSPITAL_TREATMENT_COST;
health = min(health + HOSPITAL_HEALTH_RESTORE, MAX_HEALTH);
std::this_thread::sleep_for(std::chrono::seconds(3));
std::cout << "You feel much better! (Health: " << health << "/" << MAX_HEALTH << ", Funds left: " << funds << ")\n";
} else {
std::cout << "❌ Not enough funds!\n";
}
}
else if (choice == 2) {
std::cout << "🚶 Leaving Hospital...\n";
return;
}
else {
std::cout << "Invalid option. Try again.\n";
}
}
}
};

/**
* @class Office
* @brief Represents the office building
*
* The office allows players to work and earn funds.
*/
class Office {
private:
int& funds;
public:
Office(int& f) : funds(f) {}

void enter() {
while (true) {
std::cout << "\n🏢 Welcome to your Office!\n";
std::cout << "1. 💼 Work (+" << OFFICE_WORK_REWARD << " funds)\n";
std::cout << "2. 🚪 Exit Office\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

if (choice == 1) {
std::cout << "💼 Working... (5 seconds)\n";
std::this_thread::sleep_for(std::chrono::seconds(5));
funds += OFFICE_WORK_REWARD;
std::cout << "You earned " << OFFICE_WORK_REWARD << " funds! (Total funds: " << funds << ")\n";
}
else if (choice == 2) {
std::cout << "🚶 Leaving Office...\n";
return;
}
else {
std::cout << "Invalid option. Try again.\n";
}
}
}
};

/**
* @class Restaurant
* @brief Represents the restaurant building
*
* The restaurant provides meals that cost funds but give eco points.
*/
class Restaurant {
private:
int& funds;
int& ecoPoints;
int& hunger; // Add hunger reference
public:
Restaurant(int& f, int& e, int& h) : funds(f), ecoPoints(e), hunger(h) {} // Add hunger parameter

void enter() {
while (true) {
displayStatus(funds, 0, hunger, 0, 0, ecoPoints, 0);
std::cout << "\n🍴 Welcome to the Restaurant!\n";
std::cout << "1. 🍞 Basic Meal (Costs " << BASIC_MEAL_COST << " funds, +" << BASIC_MEAL_HUNGER << " hunger, -" << BASIC_MEAL_ECO_PENALTY << " ecoPoints)\n";
std::cout << "2. 🍝 Intermediate Meal (Costs " << INTERMEDIATE_MEAL_COST << " funds, +" << INTERMEDIATE_MEAL_HUNGER << " hunger)\n";
std::cout << "3. 🍲 Premium Meal (Costs " << PREMIUM_MEAL_COST << " funds, +" << PREMIUM_MEAL_ECO_POINTS << " ecoPoints, +" << PREMIUM_MEAL_HUNGER << " hunger)\n";
std::cout << "4. 🚪 Exit Restaurant\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

switch (choice) {
case 1:
if (funds >= BASIC_MEAL_COST) {
funds -= BASIC_MEAL_COST;
hunger = std::min(hunger + BASIC_MEAL_HUNGER, MAX_HUNGER);
ecoPoints -= BASIC_MEAL_ECO_PENALTY;
std::cout << "🍽️ Eating basic meal...\n";
} else {
std::cout << "❌ Not enough funds!\n";
}
break;
case 2:
if (funds >= INTERMEDIATE_MEAL_COST) {
funds -= INTERMEDIATE_MEAL_COST;
hunger = std::min(hunger + INTERMEDIATE_MEAL_HUNGER, MAX_HUNGER);
std::cout << "🍽️ Eating intermediate meal...\n";
} else {
std::cout << "❌ Not enough funds!\n";
}
break;
case 3:
if (funds >= PREMIUM_MEAL_COST) {
funds -= PREMIUM_MEAL_COST;
ecoPoints += PREMIUM_MEAL_ECO_POINTS;
hunger = std::min(hunger + PREMIUM_MEAL_HUNGER, MAX_HUNGER);
std::cout << "🍽️ Eating premium meal...\n";
} else {
std::cout << "❌ Not enough funds!\n";
}
break;
case 4:
std::cout << "🚶 Leaving Restaurant...\n";
return;
default:
std::cout << "Invalid option. Try again!\n";
}
}
}
};

/**
* @class School
* @brief Represents the school building
*
* The school allows players to study and gain eco points.
*/
class School {
private:
int& ecoPoints;
public:
School(int& e) : ecoPoints(e) {}

void enter() {
while (true) {
std::cout << "\n🎓 Welcome to School!\n";
std::cout << "1. 📚 Study (+" << SCHOOL_STUDY_ECO_POINTS << " ecoPoints)\n";
std::cout << "2. 🚪 Exit School\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

if (choice == 1) {
std::cout << "📚 Studying... (4 seconds)\n";
std::this_thread::sleep_for(std::chrono::seconds(4));
ecoPoints += SCHOOL_STUDY_ECO_POINTS;
std::cout << "You gained " << SCHOOL_STUDY_ECO_POINTS << " ecoPoints! (Total ecoPoints: " << ecoPoints << ")\n";
}
else if (choice == 2) {
std::cout << "🚶 Leaving School...\n";
return;
}
else {
std::cout << "Invalid option. Try again.\n";
}
}
}
};

/**
* @class Bank
* @brief Represents the bank building
*
* The bank allows players to earn interest on their funds.
*/
class Bank {
private:
int& funds;
public:
Bank(int& f) : funds(f) {}

void enter() {
while (true) {
std::cout << "\n🏦 Welcome to the Bank!\n";
std::cout << "1. 💰 Deposit and get interest (+" << BANK_INTEREST_RATE << "% funds)\n";
std::cout << "2. 🚪 Exit Bank\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

if (choice == 1) {
std::cout << "💰 Depositing and waiting for interest... (5 seconds)\n";
std::this_thread::sleep_for(std::chrono::seconds(5));
int interest = (funds * BANK_INTEREST_RATE) / 100;
funds += interest;
std::cout << "You earned " << interest << " funds as interest! (Total funds: " << funds << ")\n";
}
else if (choice == 2) {
std::cout << "🚶 Leaving Bank...\n";
return;
}
else {
std::cout << "Invalid option. Try again.\n";
}
}
}
};

/**
* @class Casino
* @brief Represents the casino building
*
* The casino allows players to gamble their funds for a chance to win more.
*/
class Casino {
private:
int& funds;
public:
Casino(int& f) : funds(f) {
std::srand(std::time(nullptr));
}

void enter() {
while (true) {
std::cout << "\n🎰 Welcome to the Casino!\n";
std::cout << "1. 🎲 Simple Bet (Bet " << CASINO_BET_AMOUNT << " funds)\n";
std::cout << "2. 🎯 High Stakes (Bet " << CASINO_BET_AMOUNT * 2 << " funds)\n";
std::cout << "3. 🎮 Lucky Number (Bet " << CASINO_BET_AMOUNT / 2 << " funds)\n";
std::cout << "4. 🚪 Exit Casino\n";
std::cout << "Choose an option: ";

int choice;
std::cin >> choice;
std::cin.ignore();

switch (choice) {
case 1:
gamble(CASINO_BET_AMOUNT);
break;
case 2:
gamble(CASINO_BET_AMOUNT * 2);
break;
case 3:
gamble(CASINO_BET_AMOUNT / 2);
break;
case 4:
std::cout << "🚶 Leaving Casino...\n";
return;
default:
std::cout << "Invalid option. Try again!\n";
}
}
}

private:
void gamble(int bet) {
if (funds >= bet) {
funds -= bet;
int roll = rand() % 2;
std::cout << "🎲 Rolling... (3 seconds)\n";
std::this_thread::sleep_for(std::chrono::seconds(3));

if (roll == 1) {
std::cout << "🎉 You WON " << bet * 2 << " funds!\n";
funds += bet * 2;
} else {
std::cout << "😢 You LOST the gamble.\n";
}
std::cout << "Current funds: " << funds << "\n";
} else {
std::cout << "❌ Not enough funds!\n";
}
}
};

// --------- New Environment Class ---------
class Environment {
private:
int& funds;
int& ecoPoints;
int& pollutionLevel;
int treesPlanted;
public:
Environment(int& f, int& e, int& p) : funds(f), ecoPoints(e), pollutionLevel(p), treesPlanted(0) {}

void enter() {
while (true) {
displayStatus(funds, 0, 0, 0, 0, ecoPoints, pollutionLevel);
cout << "\n🌳 Welcome to the Environment Center!\n";
cout << "1. 🌱 Plant a Tree (Costs " << TREE_COST << " funds, +" << TREE_ECO_POINTS << " ecoPoints, -" << TREE_POLLUTION_REDUCTION << " pollution)\n";
cout << "2. 📊 View Statistics\n";
cout << "3. 🚪 Exit\n";
cout << "Choose an option: ";

int choice;
cin >> choice;
cin.ignore();

switch (choice) {
case 1:
plantTree();
break;
case 2:
viewStatistics();
break;
case 3:
cout << "🚶 Leaving Environment Center...\n";
return;
default:
cout << "Invalid option. Try again!\n";
}
}
}

private:
void plantTree() {
if (funds >= TREE_COST) {
funds -= TREE_COST;
ecoPoints += TREE_ECO_POINTS;
pollutionLevel -= TREE_POLLUTION_REDUCTION;
treesPlanted++;
cout << "🌱 Tree planted! Total trees: " << treesPlanted << "\n";
} else {
cout << "❌ Not enough funds!\n";
}
}

void viewStatistics() {
cout << "\n📊 Environment Statistics:\n";
cout << "🌳 Trees Planted: " << treesPlanted << "\n";
cout << "🌱 Eco Points: " << ecoPoints << "\n";
cout << "🌫️ Pollution Level: " << pollutionLevel << "\n";
if (pollutionLevel < 0) {
cout << "✨ Your city is pollution-free!\n";
}
}
};

// --------------------------------------

// ---------- File Saving & Loading ----------
void save_game(string filename, int level, int ecopoints, int funds, int house, int hospital, int office, int restaurant,
int school, int bank, int casino, int vehicle, int pollutionlevel, time_t lastSaveTime, int levelPoints)
{
ofstream ufile(filename);
if (ufile.is_open())
{
ufile << "level: " << level << endl;
ufile << "ecopoints: " << ecopoints << endl;
ufile << "funds: " << funds << endl;
ufile << "house: " << house << endl;
ufile << "hospital: " << hospital << endl;
ufile << "office: " << office << endl;
ufile << "restaurant: " << restaurant << endl;
ufile << "school: " << school << endl;
ufile << "bank: " << bank << endl;
ufile << "casino: " << casino << endl;
ufile << "vehicle: " << vehicle << endl;
ufile << "pollutionlevel: " << pollutionlevel << endl;
ufile << "lastSaveTime: " << lastSaveTime << endl;
ufile << "levelPoints: " << levelPoints << endl;
ufile << "hasBicycle: " << hasBicycle << endl;
ufile << "hasCar: " << hasCar << endl;
ufile << "hasElectricCar: " << hasElectricCar << endl;
ufile.close();
}
else
{
cout << "Error saving game!" << endl;
}
}

bool load_game(string filename, int &level, int &ecopoints, int &funds,
int &house, int &hospital, int &office, int &restaurant,
int &school, int &bank, int &casino, int &vehicle, int &pollutionlevel, time_t &lastSaveTime, int &levelPoints)
{
ifstream ufile(filename);
if (ufile.is_open())
{
string line;
while (getline(ufile, line))
{
stringstream ss(line);
string label;
ss >> label;
if (label == "level:")
ss >> level;
else if (label == "ecopoints:")
ss >> ecopoints;
else if (label == "funds:")
ss >> funds;
else if (label == "house:")
ss >> house;
else if (label == "hospital:")
ss >> hospital;
else if (label == "office:")
ss >> office;
else if (label == "restaurant:")
ss >> restaurant;
else if (label == "school:")
ss >> school;
else if (label == "bank:")
ss >> bank;
else if (label == "casino:")
ss >> casino;
else if (label == "vehicle:")
ss >> vehicle;
else if (label == "pollutionlevel:")
ss >> pollutionlevel;
else if (label == "lastSaveTime:")
ss >> lastSaveTime;
else if (label == "levelPoints:")
ss >> levelPoints;
else if (label == "hasBicycle:")
ss >> hasBicycle;
else if (label == "hasCar:")
ss >> hasCar;
else if (label == "hasElectricCar:")
ss >> hasElectricCar;
}
ufile.close();
return true;
}
else
{
cout << "No existing file. New game will start." << endl;
funds = 500;
lastSaveTime = time(nullptr);
level = 0;
levelPoints = 0;
hasBicycle = false;
hasCar = false;
hasElectricCar = false;
return false;
}
}

// ----------- Fund Management -----------
void offline_bonus(int &funds, time_t &lastSaveTime)
{
time_t now = time(nullptr);
double seconds_passed = difftime(now, lastSaveTime);
double days_passed = seconds_passed / (60 * 60 * 24);

int bonus = static_cast<int>(days_passed * OFFLINE_BONUS_PER_DAY);
if (bonus > 0)
{
cout << "You received an offline bonus of " << bonus << " funds!" << endl;
funds += bonus;
}

lastSaveTime = now;
}

void update_funds_periodically(int &funds, time_t &lastUpdateTime)
{
time_t now = time(nullptr);
if (difftime(now, lastUpdateTime) >= PERIODIC_FUNDS_INTERVAL)
{
funds += PERIODIC_FUNDS_REWARD;
lastUpdateTime = now;
cout << "(+" << PERIODIC_FUNDS_REWARD << " funds for being active!) Current funds: " << funds << endl;
}
}

void transport_delay(int vehicle, int& ecoPoints, int& pollutionLevel) {
if (vehicle == 0) { // Walking
cout << "Walking... Please wait " << WALK_DELAY << " seconds.\n";
this_thread::sleep_for(chrono::seconds(WALK_DELAY));
}
else if (vehicle == 1) { // Cycling
if (!hasBicycle) {
cout << "❌ You need to purchase a bicycle first!\n";
return;
}
cout << "Cycling... Please wait " << CYCLE_DELAY << " seconds.\n";
this_thread::sleep_for(chrono::seconds(CYCLE_DELAY));
}
else if (vehicle == 2) { // Regular Car
if (!hasCar) {
cout << "❌ You need to purchase a car first!\n";
return;
}
cout << "Driving... Immediate arrival!\n";
ecoPoints -= CAR_ECO_PENALTY;
pollutionLevel += CAR_POLLUTION_INCREASE;
this_thread::sleep_for(chrono::seconds(CAR_DELAY));
}
else if (vehicle == 3) { // Electric Car
if (!hasElectricCar) {
cout << "❌ You need to purchase an electric car first!\n";
return;
}
cout << "Driving electric car... Immediate arrival!\n";
ecoPoints += ELECTRIC_CAR_ECO_BONUS;
this_thread::sleep_for(chrono::seconds(CAR_DELAY));
}
}

// --------- Status Display Functions ---------
void clearScreen() {
#ifdef _WIN32
system("cls");
#else
system("clear");
#endif
}

void displayStatusBar(int value, int max, const string& label, const string& color) {
const int barWidth = 20;
int filled = (value * barWidth) / max;
cout << color << label << ": [";
for (int i = 0; i < barWidth; i++) {
cout << (i < filled ? "█" : "░");
}
cout << "] " << value << "/" << max << "\033[0m" << endl;
}

void displayStatus(int funds, int health, int hunger, int level, int levelPoints, int ecoPoints, int pollutionLevel) {
clearScreen();
cout << "\n=== Eco City Status ===\n";
cout << "💰 Funds: " << funds << endl;
displayStatusBar(health, MAX_HEALTH, "Health", "\033[1;31m");
displayStatusBar(hunger, MAX_HUNGER, "Hunger", "\033[1;33m");
cout << "📊 Level: " << level << " (Points: " << levelPoints << "/" << LEVEL_UP_THRESHOLD << ")" << endl;
cout << "🌱 Eco Points: " << ecoPoints << endl;
cout << "🌫️ Pollution Level: " << pollutionLevel << endl;
cout << "=====================\n\n";
}

void updateLevel(int& level, int& levelPoints, int ecoPoints, int pollutionLevel) {
// Calculate level points based on eco points and pollution
levelPoints = ecoPoints - (pollutionLevel * POLLUTION_PENALTY);
if (levelPoints < 0) levelPoints = 0;

// Calculate new level (100 points = level 1, 200 points = level 2, etc.)
int newLevel = (levelPoints / 100) + 1;

if (newLevel > level) {
level = newLevel;
cout << "\n🎉 Level Up! You are now level " << level << "!\n";
cout << "You need " << (level * 100) << " points for the next level.\n";
}
}

void updateHungerAndHealth(int& health, int& hunger, time_t& lastUpdateTime) {
time_t now = time(nullptr);
double minutesPassed = difftime(now, lastUpdateTime) / 60;

if (minutesPassed >= 1) {
hunger -= static_cast<int>(minutesPassed * HUNGER_DECREASE_RATE);
if (hunger < 0) hunger = 0;

if (hunger == 0) {
health -= static_cast<int>(minutesPassed * HEALTH_DECREASE_RATE);
if (health < 0) health = 0;
}

lastUpdateTime = now;
}
}

// ------------ Main Menu ------------
void menu(int &funds, int &vehicle, int &level, time_t &lastUpdateTime, int &ecopoints, int &health, int &hunger, int &levelPoints, int &pollutionLevel)
{
time_t lastHealthUpdate = time(nullptr);
int initialFunds = funds;

while (true)
{
update_funds_periodically(funds, lastUpdateTime);
updateHungerAndHealth(health, hunger, lastHealthUpdate);
updateLevel(level, levelPoints, ecopoints, pollutionLevel);
displayStatus(funds, health, hunger, level, levelPoints, ecopoints, pollutionLevel);

cout << "\n--- Eco City Menu ---\n";
cout << "1. Go to House\n";
cout << "2. Go to Hospital\n";
cout << "3. Go to Office\n";
cout << "4. Go to Restaurant\n";
cout << "5. Go to School\n";
cout << "6. Go to Bank\n";
cout << "7. Go to Casino\n";
cout << "8. Change Transport Mode\n";
cout << "9. View Instructions\n";
cout << "10. Environment Center\n";
cout << "0. Save and Exit\n";
cout << "Enter your choice: ";
int choice;
cin >> choice;

int netFunds = funds - initialFunds;

switch (choice)
{
case 1:
transport_delay(vehicle, ecopoints, pollutionLevel);
House(funds, ecopoints, health, hunger).enter();
break;
case 2:
transport_delay(vehicle, ecopoints, pollutionLevel);
Hospital(funds, health).enter();
break;
case 3:
transport_delay(vehicle, ecopoints, pollutionLevel);
Office(funds).enter();
break;
case 4:
transport_delay(vehicle, ecopoints, pollutionLevel);
Restaurant(funds, ecopoints, hunger).enter();
break;
case 5:
transport_delay(vehicle, ecopoints, pollutionLevel);
School(ecopoints).enter();
break;
case 6:
transport_delay(vehicle, ecopoints, pollutionLevel);
Bank(funds).enter();
break;
case 7:
transport_delay(vehicle, ecopoints, pollutionLevel);
Casino(funds).enter();
break;
case 8:
cout << "Choose your transport:\n";
cout << "0. Walk (Free)\n";

// Show bicycle options based on level and ownership
if (level >= CYCLE_REQUIRED_LEVEL) {
if (hasBicycle) {
cout << "1. Cycle (Owned)\n";
} else {
cout << "1. Buy Cycle (" << CYCLE_COST << " funds)\n";
}
}

// Show regular car options based on level and ownership
if (level >= CAR_REQUIRED_LEVEL) {
if (hasCar) {
cout << "2. Regular Car (Owned)\n";
} else {
cout << "2. Buy Regular Car (" << CAR_COST << " funds)\n";
}
}

// Show electric car options based on level and ownership
if (level >= ELECTRIC_CAR_REQUIRED_LEVEL) {
if (hasElectricCar) {
cout << "3. Electric Car (Owned)\n";
} else {
cout << "3. Buy Electric Car (" << ELECTRIC_CAR_COST << " funds)\n";
}
}

cout << "Choice: ";
cin >> choice;

if (choice == 1) {
if (level < CYCLE_REQUIRED_LEVEL) {
cout << "❌ You need to be level " << CYCLE_REQUIRED_LEVEL << " to buy a bicycle!\n";
} else if (!hasBicycle) {
if (funds >= CYCLE_COST) {
funds -= CYCLE_COST;
hasBicycle = true;
vehicle = 1;
cout << "✅ Bicycle purchased! You can now cycle.\n";
} else {
cout << "❌ Not enough funds!\n";
}
} else {
vehicle = 1;
cout << "✅ Transport mode changed to cycling!\n";
}
} else if (choice == 2) {
if (level < CAR_REQUIRED_LEVEL) {
cout << "❌ You need to be level " << CAR_REQUIRED_LEVEL << " to buy a regular car!\n";
} else if (!hasCar) {
if (funds >= CAR_COST) {
funds -= CAR_COST;
hasCar = true;
vehicle = 2;
cout << "✅ Regular car purchased! You can now drive.\n";
} else {
cout << "❌ Not enough funds!\n";
}
} else {
vehicle = 2;
cout << "✅ Transport mode changed to regular car!\n";
}
} else if (choice == 3) {
if (level < ELECTRIC_CAR_REQUIRED_LEVEL) {
cout << "❌ You need to be level " << ELECTRIC_CAR_REQUIRED_LEVEL << " to buy an electric car!\n";
} else if (!hasElectricCar) {
if (funds >= ELECTRIC_CAR_COST) {
funds -= ELECTRIC_CAR_COST;
hasElectricCar = true;
vehicle = 3;
cout << "✅ Electric car purchased! You can now drive eco-friendly.\n";
} else {
cout << "❌ Not enough funds!\n";
}
} else {
vehicle = 3;
cout << "✅ Transport mode changed to electric car!\n";
}
} else if (choice == 0) {
vehicle = 0;
cout << "✅ Transport mode changed to walking!\n";
} else {
cout << "Invalid choice!\n";
}
break;
case 9:
displayInstructions("eco_city_instructions.txt");
break;
case 10:
Environment(funds, ecopoints, pollutionLevel).enter();
break;
case 0:
clearScreen();
cout << "\n=== Game Summary ===\n";
cout << "💰 Funds: " << funds << " (" << (netFunds >= 0 ? "+" : "") << netFunds << ")\n";
cout << "❤️ Health: " << health << "/" << MAX_HEALTH << "\n";
cout << "🍽️ Hunger: " << hunger << "/" << MAX_HUNGER << "\n";
cout << "📊 Level: " << level << " (Points: " << levelPoints << "/" << LEVEL_UP_THRESHOLD << ")\n";
cout << "🌱 Eco Points: " << ecopoints << "\n";
cout << "🌫️ Pollution Level: " << pollutionLevel << "\n";
cout << "===================\n\n";
return;
default:
cout << "Invalid choice. Try again!\n";
break;
}
}
}

// ------------ Main ------------
int main()
{
int level = 0, ecopoints = 0, funds = 0;
int health = MAX_HEALTH, hunger = MAX_HUNGER, levelPoints = 0;
int house = 0, hospital = 0, office = 0, restaurant = 0;
int school = 0, bank = 0, casino = 0, vehicle = 0, pollutionlevel = 0;
time_t lastSaveTime = time(nullptr);
time_t lastUpdateTime = time(nullptr);

string name, pin, filename;
cout << "Enter your name: ";
getline(cin, name);
cout << "Enter your 4-digit PIN: ";
cin >> pin;
cin.ignore();

filename = name + "_" + pin + ".txt";

bool fileExists = load_game(filename, level, ecopoints, funds, house, hospital, office, restaurant,
school, bank, casino, vehicle, pollutionlevel, lastSaveTime, levelPoints);

if (!fileExists)
{
displayInstructions("eco_city_instructions.txt"); // only show for new users
level = 1;
}

offline_bonus(funds, lastSaveTime);

menu(funds, vehicle, level, lastUpdateTime, ecopoints, health, hunger, levelPoints, pollutionlevel);

save_game(filename, level, ecopoints, funds, house, hospital, office, restaurant,
school, bank, casino, vehicle, pollutionlevel, time(nullptr), levelPoints);

cout << "Game saved. Goodbye!" << endl;
return 0;
}
