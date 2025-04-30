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
const int HOUSE_EAT_COST = 20;              ///< Cost to eat at home
const int HOSPITAL_TREATMENT_COST = 50;     ///< Cost for hospital treatment
const int OFFICE_WORK_REWARD = 100;         ///< Reward for working at office
const int RESTAURANT_MEAL_COST = 50;        ///< Cost for restaurant meal
const int RESTAURANT_ECO_POINTS = 20;       ///< Eco points gained from restaurant
const int SCHOOL_STUDY_ECO_POINTS = 10;     ///< Eco points gained from studying
const int BANK_INTEREST_RATE = 2;           ///< Bank interest rate percentage
const int CASINO_BET_AMOUNT = 100;          ///< Amount to bet at casino
const int CASINO_WIN_REWARD = 200;          ///< Reward for winning at casino
const int HOUSE_RELAX_ECO_POINTS = 5;       ///< Eco points gained from relaxing at home

/**
 * @brief Health and hunger constants
 */
const int MAX_HEALTH = 100;                 ///< Maximum health value
const int MAX_HUNGER = 100;                 ///< Maximum hunger value
const int HUNGER_DECREASE_RATE = 1;         ///< Hunger decrease per minute
const int HEALTH_DECREASE_RATE = 1;         ///< Health decrease when hungry
const int HOSPITAL_HEALTH_RESTORE = 30;     ///< Health restored at hospital
const int HOUSE_SLEEP_HEALTH_RESTORE = 20;  ///< Health restored while sleeping
const int HOUSE_EAT_HUNGER_RESTORE = 30;    ///< Hunger restored while eating

/**
 * @brief Level and pollution constants
 */
const int LEVEL_POINTS_PER_ECO = 1;         ///< Level points per eco point
const int POLLUTION_PENALTY = 2;            ///< Pollution penalty multiplier
const int LEVEL_UP_THRESHOLD = 100;         ///< Level points needed to level up

/**
 * @brief Transportation delay constants in seconds
 */
const int WALK_DELAY = 10;                  ///< Walking delay in seconds
const int CYCLE_DELAY = 5;                  ///< Cycling delay in seconds
const int CAR_DELAY = 0;                    ///< Car delay in seconds

/**
 * @brief Periodic reward constants
 */
const int PERIODIC_FUNDS_REWARD = 10;       ///< Funds gained periodically
const int PERIODIC_FUNDS_INTERVAL = 60;     ///< Interval for periodic rewards in seconds
const int OFFLINE_BONUS_PER_DAY = 1000;     ///< Offline bonus per day

// --------- Function Declarations ---------
void clearScreen();
void displayStatusBar(int value, int max, const string& label, const string& color);
void displayStatus(int funds, int health, int hunger, int level, int levelPoints, int ecoPoints, int pollutionLevel);
void updateLevel(int& level, int& levelPoints, int ecoPoints, int pollutionLevel);
void updateHungerAndHealth(int& health, int& hunger, time_t& lastUpdateTime);
void transport_delay(int vehicle);
void update_funds_periodically(int &funds, time_t &lastUpdateTime);
void offline_bonus(int &funds, time_t &lastSaveTime);
void save_game(string filename, int level, int ecopoints, int funds, int house, int hospital, int office, int restaurant,
               int school, int bank, int casino, int vehicle, int pollutionlevel, time_t lastSaveTime);
bool load_game(string filename, int &level, int &ecopoints, int &funds,
               int &house, int &hospital, int &office, int &restaurant,
               int &school, int &bank, int &casino, int &vehicle, int &pollutionlevel, time_t &lastSaveTime);

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
                hunger = min(hunger + HOUSE_EAT_HUNGER_RESTORE, MAX_HUNGER);
                std::this_thread::sleep_for(std::chrono::seconds(3));
                std::cout << "You feel energized! (Hunger: " << hunger << "/" << MAX_HUNGER << ", Funds left: " << funds << ")\n";
            } else {
                std::cout << "❌ Not enough funds!\n";
            }
        }
    
        void relax() {
            std::cout << "🎮 Relaxing... (2 seconds)\n";
            ecoPoints += HOUSE_RELAX_ECO_POINTS;
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "You feel happy! (Eco Points: " << ecoPoints << ")\n";
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
    public:
        Restaurant(int& f, int& e) : funds(f), ecoPoints(e) {}
    
        void enter() {
            while (true) {
                std::cout << "\n🍴 Welcome to the Restaurant!\n";
                std::cout << "1. 🍔 Eat Premium Meal (Costs " << RESTAURANT_MEAL_COST << " funds, +" << RESTAURANT_ECO_POINTS << " ecoPoints)\n";
                std::cout << "2. 🚪 Exit Restaurant\n";
                std::cout << "Choose an option: ";
    
                int choice;
                std::cin >> choice;
                std::cin.ignore();
    
                if (choice == 1) {
                    if (funds >= RESTAURANT_MEAL_COST) {
                        std::cout << "🍽️ Eating... (4 seconds)\n";
                        funds -= RESTAURANT_MEAL_COST;
                        ecoPoints += RESTAURANT_ECO_POINTS;
                        std::this_thread::sleep_for(std::chrono::seconds(4));
                        std::cout << "Delicious! (Funds: " << funds << ", EcoPoints: " << ecoPoints << ")\n";
                    } else {
                        std::cout << "❌ Not enough funds!\n";
                    }
                }
                else if (choice == 2) {
                    std::cout << "🚶 Leaving Restaurant...\n";
                    return;
                }
                else {
                    std::cout << "Invalid option. Try again.\n";
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
                std::cout << "1. 🎲 Gamble (Bet " << CASINO_BET_AMOUNT << " funds)\n";
                std::cout << "2. 🚪 Exit Casino\n";
                std::cout << "Choose an option: ";
    
                int choice;
                std::cin >> choice;
                std::cin.ignore();
    
                if (choice == 1) {
                    if (funds >= CASINO_BET_AMOUNT) {
                        funds -= CASINO_BET_AMOUNT;
                        int roll = std::rand() % 2;
                        std::cout << "🎲 Rolling... (3 seconds)\n";
                        std::this_thread::sleep_for(std::chrono::seconds(3));
    
                        if (roll == 1) {
                            std::cout << "🎉 You WON " << CASINO_WIN_REWARD << " funds!\n";
                            funds += CASINO_WIN_REWARD;
                        } else {
                            std::cout << "😢 You LOST the gamble.\n";
                        }
                        std::cout << "Current funds: " << funds << "\n";
                    } else {
                        std::cout << "❌ Not enough funds!\n";
                    }
                }
                else if (choice == 2) {
                    std::cout << "🚶 Leaving Casino...\n";
                    return;
                }
                else {
                    std::cout << "Invalid option. Try again.\n";
                }
            }
        }
    };
// --------------------------------------


