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
#include <map>
#include <set>
#include "instructions.cpp"
#include <random>
#include <ctime>
#include <functional>
#include <vector>
#include <filesystem>
using namespace std;

// Forward declarations for news system
struct NewsItem {
    string title;
    string content;
    string category;
    string timestamp;
    bool isImportant;
};

vector<NewsItem> newsFeed;

void generateNews(const string& category, const string& title, const string& content, bool isImportant = false);
void generateRandomNews(int level, int ecoPoints, int pollutionLevel, const string& cityName);
string getCurrentTimeStr();

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
const int HUNGER_DECREASE_RATE = 2;         ///< Hunger decrease per minute
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
const int HOUSE_RELAX_TIME_ACCELERATION = 2;  ///< Time acceleration factor while relaxing

// Add Summit Constants
const int SUMMIT_REQUIRED_LEVEL = 3;
const int SUMMIT_ECO_POINTS = 50;
const int SUMMIT_FUNDS_COST = 200;
const int LOAN_INTEREST_RATE = 5;  // 5% interest on loans

// --------- Loan Enhancements ---------
const int LOAN_DAYS_BEFORE_PENALTY = 7;
const int LOAN_LATE_PENALTY = 10; // extra 10% interest if late

struct LoanInfo {
    bool hasLoan = false;
    int loanAmount = 0;
    string lenderName = "";
    string loanStartDate = "";
};

string getCurrentDateStrLoan() {
    time_t t = time(nullptr);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return string(buf);
}

int daysBetween(const string& start, const string& end) {
    tm tm1 = {}, tm2 = {};
    strptime(start.c_str(), "%Y-%m-%d", &tm1);
    strptime(end.c_str(), "%Y-%m-%d", &tm2);
    time_t t1 = mktime(&tm1);
    time_t t2 = mktime(&tm2);
    return (t2 - t1) / (60 * 60 * 24);
}

void notifyLenderOfRepayment(const string& lenderFile, const string& borrower) {
    ofstream out(lenderFile, ios::app);
    if (out.is_open()) {
        out << "[Notification] Loan repaid by " << borrower << " on " << getCurrentDateStrLoan() << "\n";
        out.close();
    }
}

// --------- Function Declarations ---------
void clearScreen();
void displayStatusBar(int value, int max, const string& label, const string& color);
void displayStatus(int funds, int health, int hunger, int level, int levelPoints, int ecoPoints, int pollutionLevel, const string& cityName);
void updateLevel(int& level, int& levelPoints, int ecoPoints, int pollutionLevel);
void updateHungerAndHealth(int& health, int& hunger, time_t& lastUpdateTime);
void transport_delay(int vehicle, int& ecoPoints, int& pollutionLevel);
void update_funds_periodically(int &funds, time_t &lastUpdateTime);
void offline_bonus(int &funds, time_t &lastSaveTime);
void save_game(string filename, int level, int ecopoints, int funds, int health, int hunger,
    int houseUp, int hospitalUp, int officeUp, int restaurantUp, int schoolUp, int bankUp, int casinoUp, int envUp, int recUp, int gardenUp,
    int vehicle, int pollutionlevel, time_t lastSaveTime, int levelPoints, bool hasLoan, int loanAmount, string lenderName, string loanStartDate, string cityName);
bool load_game(string filename, int &level, int &ecopoints, int &funds, int &health, int &hunger,
    int &houseUp, int &hospitalUp, int &officeUp, int &restaurantUp, int &schoolUp, int &bankUp, int &casinoUp, int &envUp, int &recUp, int &gardenUp,
    int& vehicle, int& pollutionlevel, time_t& lastSaveTime, int& levelPoints,
    bool &hasLoan, int &loanAmount, string &lenderName, string &loanStartDate, string &cityName);

// --------- ASCII Art for Buildings ---------
const string HOUSE_ART = R"(
  /\  
 /  \   _
/____\_|_|
| []  [] |
|   __   |
|__|  |__|
)";
const string HOSPITAL_ART = R"(
  _____
 |  +  |
 |_____| 
 |     |
 |_____| 
)";
const string OFFICE_ART = R"(
  _______
 |  _  _ |
 | | | ||
 | | | ||
 |_____/|
)";
const string RESTAURANT_ART = R"(
   _____
  /     \
 |  o o |
 |  --- |
 |_____/ 
)";
const string SCHOOL_ART = R"(
   _____
  /     \
 |  S S |
 |  --- |
 |_____/ 
)";
const string BANK_ART = R"(
  _______
 | $   $ |
 |_______|
 |       |
 |_______|
)";
const string CASINO_ART = R"(
  _______
 | 77777|
 |_______|
 |       |
 |_______|
)";
const string ENVIRONMENT_ART = R"(
   _/\_
  /    \
  \    /
   \  /
    \/
   (🌳)
)";
const string RECYCLING_ART = R"(
   _____
  /     \
 |  ♻️  |
 |      |
 |_____/ 
)";
const string GARDEN_ART = R"(
   _/\_
  (🌻🌻)
  (🌻🌻)
   \  /
    \/
)";

// --------- Building Upgrades ---------
struct BuildingUpgrade {
    int level = 1;
    static const int maxLevel = 3;
    int upgradeCost() const { return 200 * level; }
    bool canUpgrade() const { return level < maxLevel; }
    void upgrade(int& funds) {
        if (canUpgrade() && funds >= upgradeCost()) {
            funds -= upgradeCost();
            level++;
            cout << "Building upgraded to level " << level << "!\n";
        } else if (!canUpgrade()) {
            cout << "Already at max level!\n";
        } else {
            cout << "Not enough funds to upgrade!\n";
        }
    }
};

// --------- Achievements System ---------
struct Achievements {
    set<string> unlocked;
    int treesPlanted = 0;
    int summitsHosted = 0;
    int loansRepaid = 0;
    void checkAndUnlock() {
        if (treesPlanted >= 10) unlocked.insert("Tree Planter: Planted 10 trees");
        if (summitsHosted >= 3) unlocked.insert("Summit Leader: Hosted 3 summits");
        if (loansRepaid >= 1) unlocked.insert("Debt-Free: Repaid a loan");
    }
    void display() {
        cout << "\n🏅 Achievements & Badges 🏅\n";
        if (unlocked.empty()) {
            cout << "No achievements unlocked yet.\n";
        } else {
            for (const auto& ach : unlocked) {
                cout << "- " << ach << "\n";
            }
        }
        cout << endl;
    }
};
Achievements achievements;

// --------- Challenges System ---------
struct Challenge {
    string description;
    int target;
    int progress;
    bool completed;
    string rewardType; // "funds" or "eco"
    int rewardAmount;
    string type; // "daily" or "weekly"
    string id; // unique id for challenge
    string lastResetDate;
};
Challenge dailyChallenge = {"Plant 5 trees today", 5, 0, false, "funds", 100, "daily", "plant5trees", ""};
Challenge weeklyChallenge = {"Host 2 summits this week", 2, 0, false, "eco", 50, "weekly", "host2summits", ""};
string getCurrentDateStr() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&t));
    return string(buf);
}
string getCurrentWeekStr() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-W%V", localtime(&t));
    return string(buf);
}
void resetChallengeIfNeeded(Challenge &ch) {
    string today = (ch.type == "daily") ? getCurrentDateStr() : getCurrentWeekStr();
    if (ch.lastResetDate != today) {
        ch.progress = 0;
        ch.completed = false;
        ch.lastResetDate = today;
    }
}
void checkChallengeCompletion(Challenge &ch, int &funds, int &ecoPoints) {
    if (!ch.completed && ch.progress >= ch.target) {
        ch.completed = true;
        if (ch.rewardType == "funds") funds += ch.rewardAmount;
        if (ch.rewardType == "eco") ecoPoints += ch.rewardAmount;
        cout << "\n🎉 Challenge completed: " << ch.description << "! Reward: ";
        if (ch.rewardType == "funds") cout << ch.rewardAmount << " funds";
        if (ch.rewardType == "eco") cout << ch.rewardAmount << " eco points";
        cout << "\n";
    }
}
void showChallenges() {
    cout << "\n📅 Challenges:\n";
    cout << "- Daily:   " << dailyChallenge.description << " (" << dailyChallenge.progress << "/" << dailyChallenge.target << ") ";
    if (dailyChallenge.completed) cout << "✅";
    cout << "\n- Weekly:  " << weeklyChallenge.description << " (" << weeklyChallenge.progress << "/" << weeklyChallenge.target << ") ";
    if (weeklyChallenge.completed) cout << "✅";
    cout << "\n";
}

// --------- Player-to-Player Messaging ---------
void leaveMessage(const string& recipientFile, const string& sender, const string& context) {
    string msg;
    cout << "Enter a short message for " << recipientFile.substr(0, recipientFile.find('_')) << " (or press Enter to skip): ";
    getline(cin, msg);
    if (!msg.empty()) {
        ofstream out(recipientFile, ios::app);
        if (out.is_open()) {
            out << "[Message from " << sender << " about " << context << "]: " << msg << "\n";
            out.close();
        }
    }
}

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
        int& level;
        int& levelPoints;
        int& pollutionLevel;
        BuildingUpgrade upgrade;
        const string& cityName;
    public:
        House(int& f, int& e, int& h, int& hu, int& l, int& lp, int& p, const string& cn, int up=1) 
            : funds(f), ecoPoints(e), health(h), hunger(hu), level(l), levelPoints(lp), pollutionLevel(p), cityName(cn) { upgrade.level = up; }

        void enter() {
            clearScreen();
            cout << HOUSE_ART;
            cout << "[DEBUG] Entering House: health=" << health << ", hunger=" << hunger << endl;
            while (true) {
                displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
                cout << "\n🏠 Welcome to your House (Level " << upgrade.level << ")!\n";
                cout << "1. 🛌 Sleep (Restores " << (HOUSE_SLEEP_HEALTH_RESTORE * upgrade.level) << " health)\n";
                cout << "2. 🍴 Eat (Costs " << HOUSE_EAT_COST << " funds, Restores " << (HOUSE_EAT_HUNGER_RESTORE * upgrade.level) << " hunger)\n";
                cout << "3. 🎮 Relax (+" << (HOUSE_RELAX_ECO_POINTS * upgrade.level) << " ecoPoints)\n";
                if (upgrade.canUpgrade()) cout << "4. Upgrade House (Cost: " << upgrade.upgradeCost() << " funds)\n";
                cout << "5. 🚪 Exit House\n";
                cout << "Choose an option: ";
                int choice;
                cin >> choice;
                cin.ignore();
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
                        if (upgrade.canUpgrade()) upgrade.upgrade(funds);
                        else cout << "Invalid option. Try again!\n";
                        break;
                    case 5:
                        cout << "[DEBUG] Leaving House: health=" << health << ", hunger=" << hunger << endl;
                        cout << "🚶 Leaving House...\n";
                        clearScreen();
                        return;
                    default:
                        cout << "Invalid option. Try again!\n";
                }
            }
        }
    
    private:
        void sleep() {
            cout << "😴 Sleeping... (5 seconds)\n";
            this_thread::sleep_for(chrono::seconds(5));
            health = min(health + HOUSE_SLEEP_HEALTH_RESTORE * upgrade.level, MAX_HEALTH);
            cout << "You feel refreshed! (Health: " << health << "/" << MAX_HEALTH << ")\n";
        }
    
        void eat() {
            if (funds >= HOUSE_EAT_COST) {
                cout << "🍽️ Eating a meal... (3 seconds)\n";
                funds -= HOUSE_EAT_COST;
                hunger = min(hunger + HOUSE_EAT_HUNGER_RESTORE * upgrade.level, MAX_HUNGER);
                this_thread::sleep_for(chrono::seconds(3));
                cout << "You feel energized! (Hunger: " << hunger << "/" << MAX_HUNGER << ")\n";
            } else {
                cout << "❌ Not enough funds!\n";
            }
        }
    
        void relax() {
            cout << "🎮 Relaxing... (2 seconds)\n";
            this_thread::sleep_for(chrono::seconds(2));
            funds += PERIODIC_FUNDS_REWARD * HOUSE_RELAX_TIME_ACCELERATION * upgrade.level;
            ecoPoints += HOUSE_RELAX_ECO_POINTS * upgrade.level;
            cout << "You feel happy! (Time passed, gained " << PERIODIC_FUNDS_REWARD * HOUSE_RELAX_TIME_ACCELERATION * upgrade.level << " funds and " << HOUSE_RELAX_ECO_POINTS * upgrade.level << " eco points)\n";
        }
    public:
        int getUpgradeLevel() const { return upgrade.level; }
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
    int& hunger;
    int& level;
    int& levelPoints;
    int& ecoPoints;
    int& pollutionLevel;
    BuildingUpgrade upgrade;
    const string& cityName;
public:
    Hospital(int& f, int& h, int& hu, int& l, int& lp, int& e, int& p, const string& cn, int up=1) 
        : funds(f), health(h), hunger(hu), level(l), levelPoints(lp), ecoPoints(e), pollutionLevel(p), cityName(cn) { upgrade.level = up; }

    void enter() {
        clearScreen();
        cout << HOSPITAL_ART;
        while (true) {
            displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
            cout << "\n🏥 Welcome to the Hospital (Level " << upgrade.level << ")!\n";
            cout << "1. 💉 Get Treatment (Costs " << HOSPITAL_TREATMENT_COST << " funds, Restores " << (HOSPITAL_HEALTH_RESTORE * upgrade.level) << " health)\n";
            if (upgrade.canUpgrade()) cout << "2. Upgrade Hospital (Cost: " << upgrade.upgradeCost() << " funds)\n";
            cout << "3. 🚪 Exit Hospital\n";
            cout << "Choose an option: ";
            int choice;
            cin >> choice;
            cin.ignore();
            if (choice == 1) {
                if (funds >= HOSPITAL_TREATMENT_COST) {
                    cout << "💉 Treating... (3 seconds)\n";
                    funds -= HOSPITAL_TREATMENT_COST;
                    health = min(health + HOSPITAL_HEALTH_RESTORE * upgrade.level, MAX_HEALTH);
                    this_thread::sleep_for(chrono::seconds(3));
                    cout << "You feel much better! (Health: " << health << "/" << MAX_HEALTH << ", Funds left: " << funds << ")\n";
                } else {
                    cout << "❌ Not enough funds!\n";
                }
            } else if (choice == 2 && upgrade.canUpgrade()) {
                upgrade.upgrade(funds);
            } else if ((choice == 2 && !upgrade.canUpgrade()) || choice == 3) {
                cout << "🚶 Leaving Hospital...\n";
                clearScreen();
                return;
            } else {
                cout << "Invalid option. Try again.\n";
            }
        }
    }

    int getUpgradeLevel() const { return upgrade.level; }
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
        int& health;
        int& hunger;
        int& level;
        int& levelPoints;
        int& ecoPoints;
        int& pollutionLevel;
        BuildingUpgrade upgrade;
        const string& cityName;
    public:
        Office(int& f, int& h, int& hu, int& l, int& lp, int& e, int& p, const string& cn, int up=1) 
            : funds(f), health(h), hunger(hu), level(l), levelPoints(lp), ecoPoints(e), pollutionLevel(p), cityName(cn) { upgrade.level = up; }
    
        void enter() {
            clearScreen();
            cout << OFFICE_ART;
            while (true) {
                displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
                cout << "\n🏢 Welcome to your Office (Level " << upgrade.level << ")!\n";
                cout << "1. 💼 Work (+" << (OFFICE_WORK_REWARD * upgrade.level) << " funds)\n";
                if (upgrade.canUpgrade()) cout << "2. Upgrade Office (Cost: " << upgrade.upgradeCost() << " funds)\n";
                cout << "3. 🚪 Exit Office\n";
                cout << "Choose an option: ";
    
                int choice;
                cin >> choice;
                cin.ignore();
    
                if (choice == 1) {
                    cout << "💼 Working... (5 seconds)\n";
                    this_thread::sleep_for(chrono::seconds(5));
                    funds += OFFICE_WORK_REWARD * upgrade.level;
                    cout << "You earned " << OFFICE_WORK_REWARD * upgrade.level << " funds! (Total funds: " << funds << ")\n";
                } else if (choice == 2 && upgrade.canUpgrade()) {
                    upgrade.upgrade(funds);
                }
                else if ((choice == 2 && !upgrade.canUpgrade()) || choice == 3) {
                    cout << "🚶 Leaving Office...\n";
                    clearScreen();
                    return;
                }
                else {
                    cout << "Invalid option. Try again.\n";
                }
            }
        }

        int getUpgradeLevel() const { return upgrade.level; }
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
        int& hunger;
        int& health;
        int& level;
        int& levelPoints;
        int& pollutionLevel;
        BuildingUpgrade upgrade;
        const string& cityName;
    public:
        Restaurant(int& f, int& e, int& h, int& he, int& l, int& lp, int& p, const string& cn, int up=1) 
            : funds(f), ecoPoints(e), hunger(h), health(he), level(l), levelPoints(lp), pollutionLevel(p), cityName(cn) { upgrade.level = up; }
    
        void enter() {
            clearScreen();
            cout << RESTAURANT_ART;
            cout << "[DEBUG] Entering Restaurant: hunger=" << hunger << endl;
            while (true) {
                displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
                cout << "\n🍴 Welcome to the Restaurant (Level " << upgrade.level << ")!\n";
                cout << "1. 🍞 Basic Meal (Costs " << BASIC_MEAL_COST << " funds, +" << (BASIC_MEAL_HUNGER * upgrade.level) << " hunger, -" << BASIC_MEAL_ECO_PENALTY << " ecoPoints)\n";
                cout << "2. 🍝 Intermediate Meal (Costs " << INTERMEDIATE_MEAL_COST << " funds, +" << (INTERMEDIATE_MEAL_HUNGER * upgrade.level) << " hunger)\n";
                cout << "3. 🍲 Premium Meal (Costs " << PREMIUM_MEAL_COST << " funds, +" << (PREMIUM_MEAL_ECO_POINTS * upgrade.level) << " ecoPoints, +" << (PREMIUM_MEAL_HUNGER * upgrade.level) << " hunger)\n";
                if (upgrade.canUpgrade()) cout << "4. Upgrade Restaurant (Cost: " << upgrade.upgradeCost() << " funds)\n";
                cout << "5. 🚪 Exit Restaurant\n";
                cout << "Choose an option: ";
    
                int choice;
                cin >> choice;
                cin.ignore();
    
                switch (choice) {
                    case 1:
                        if (funds >= BASIC_MEAL_COST) {
                            funds -= BASIC_MEAL_COST;
                            hunger = min(hunger + BASIC_MEAL_HUNGER * upgrade.level, MAX_HUNGER);
                            ecoPoints -= BASIC_MEAL_ECO_PENALTY;
                            cout << "🍽️ Eating basic meal...\n";
                        } else {
                            cout << "❌ Not enough funds!\n";
                        }
                        break;
                    case 2:
                        if (funds >= INTERMEDIATE_MEAL_COST) {
                            funds -= INTERMEDIATE_MEAL_COST;
                            hunger = min(hunger + INTERMEDIATE_MEAL_HUNGER * upgrade.level, MAX_HUNGER);
                            cout << "🍽️ Eating intermediate meal...\n";
                        } else {
                            cout << "❌ Not enough funds!\n";
                        }
                        break;
                    case 3:
                        if (funds >= PREMIUM_MEAL_COST) {
                            funds -= PREMIUM_MEAL_COST;
                            ecoPoints += PREMIUM_MEAL_ECO_POINTS * upgrade.level;
                            hunger = min(hunger + PREMIUM_MEAL_HUNGER * upgrade.level, MAX_HUNGER);
                            cout << "🍽️ Eating premium meal...\n";
                        } else {
                            cout << "❌ Not enough funds!\n";
                        }
                        break;
                    case 4:
                        if (upgrade.canUpgrade()) upgrade.upgrade(funds);
                        else cout << "Invalid option. Try again!\n";
                        break;
                    case 5:
                        cout << "[DEBUG] Leaving Restaurant: hunger=" << hunger << endl;
                        cout << "🚶 Leaving Restaurant...\n";
                        clearScreen();
                        return;
                    default:
                        cout << "Invalid option. Try again!\n";
                }
            }
        }

        int getUpgradeLevel() const { return upgrade.level; }
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
        int& health;
        int& hunger;
        int& level;
        int& levelPoints;
        int& pollutionLevel;
        BuildingUpgrade upgrade;
        const string& cityName;
    public:
        School(int& e, int& h, int& hu, int& l, int& lp, int& p, const string& cn, int up=1) 
            : ecoPoints(e), health(h), hunger(hu), level(l), levelPoints(lp), pollutionLevel(p), cityName(cn) { upgrade.level = up; }
    
        void enter() {
            clearScreen();
            cout << SCHOOL_ART;
            while (true) {
                displayStatus(0, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
                cout << "\n🎓 Welcome to School (Level " << upgrade.level << ")!\n";
                cout << "1. 📚 Study (+" << (SCHOOL_STUDY_ECO_POINTS * upgrade.level) << " ecoPoints)\n";
                if (upgrade.canUpgrade()) cout << "2. Upgrade School (Cost: " << upgrade.upgradeCost() << " funds)\n";
                cout << "3. 🚪 Exit School\n";
                cout << "Choose an option: ";
    
                int choice;
                cin >> choice;
                cin.ignore();
    
                if (choice == 1) {
                    cout << "📚 Studying... (4 seconds)\n";
                    this_thread::sleep_for(chrono::seconds(4));
                    ecoPoints += SCHOOL_STUDY_ECO_POINTS * upgrade.level;
                    cout << "You gained " << SCHOOL_STUDY_ECO_POINTS * upgrade.level << " ecoPoints! (Total ecoPoints: " << ecoPoints << ")\n";
                } else if (choice == 2 && upgrade.canUpgrade()) {
                    upgrade.upgrade(ecoPoints);
                } else if ((choice == 2 && !upgrade.canUpgrade()) || choice == 3) {
                    cout << "🚶 Leaving School...\n";
                    clearScreen();
                    return;
                } else {
                    cout << "Invalid option. Try again.\n";
                }
            }
        }

        int getUpgradeLevel() const { return upgrade.level; }
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
        int& health;
        int& hunger;
        int& level;
        int& levelPoints;
        int& ecoPoints;
        int& pollutionLevel;
        BuildingUpgrade upgrade;
        const string& cityName;
    public:
        Bank(int& f, int& h, int& hu, int& l, int& lp, int& e, int& p, const string& cn, int up=1) 
            : funds(f), health(h), hunger(hu), level(l), levelPoints(lp), ecoPoints(e), pollutionLevel(p), cityName(cn) { upgrade.level = up; }
    
        void enter() {
            clearScreen();
            cout << BANK_ART;
            while (true) {
                displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
                cout << "\n🏦 Welcome to the Bank (Level " << upgrade.level << ")!\n";
                cout << "1. 💰 Deposit and get interest (+" << (BANK_INTEREST_RATE * upgrade.level) << "% funds)\n";
                if (upgrade.canUpgrade()) cout << "2. Upgrade Bank (Cost: " << upgrade.upgradeCost() << " funds)\n";
                cout << "3. 🚪 Exit Bank\n";
                cout << "Choose an option: ";
    
                int choice;
                cin >> choice;
                cin.ignore();
    
                if (choice == 1) {
                    cout << "💰 Depositing and waiting for interest... (5 seconds)\n";
                    this_thread::sleep_for(chrono::seconds(5));
                    int interest = (funds * BANK_INTEREST_RATE * upgrade.level) / 100;
                    funds += interest;
                    cout << "You earned " << interest << " funds as interest! (Total funds: " << funds << ")\n";
                } else if (choice == 2 && upgrade.canUpgrade()) {
                    upgrade.upgrade(funds);
                } else if ((choice == 2 && !upgrade.canUpgrade()) || choice == 3) {
                    cout << "🚶 Leaving Bank...\n";
                    clearScreen();
                    return;
                } else {
                    cout << "Invalid option. Try again.\n";
                }
            }
        }

        int getUpgradeLevel() const { return upgrade.level; }
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
        int& health;
        int& hunger;
        int& level;
        int& levelPoints;
        int& ecoPoints;
        int& pollutionLevel;
        BuildingUpgrade upgrade;
        const string& cityName;
    public:
        Casino(int& f, int& h, int& hu, int& l, int& lp, int& e, int& p, const string& cn, int up=1) 
            : funds(f), health(h), hunger(hu), level(l), levelPoints(lp), ecoPoints(e), pollutionLevel(p), cityName(cn) { upgrade.level = up; }
    
        void enter() {
            clearScreen();
            cout << CASINO_ART;
            while (true) {
                displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
                cout << "\n🎰 Welcome to the Casino (Level " << upgrade.level << ")!\n";
                cout << "1. 🎲 Simple Bet (Bet " << (CASINO_BET_AMOUNT * upgrade.level) << " funds)\n";
                cout << "2. 🎯 High Stakes (Bet " << (CASINO_BET_AMOUNT * 2 * upgrade.level) << " funds)\n";
                cout << "3. 🎮 Lucky Number (Bet " << (CASINO_BET_AMOUNT / 2 * upgrade.level) << " funds)\n";
                if (upgrade.canUpgrade()) cout << "4. Upgrade Casino (Cost: " << upgrade.upgradeCost() << " funds)\n";
                cout << "5. 🚪 Exit Casino\n";
                cout << "Choose an option: ";
    
                int choice;
                cin >> choice;
                cin.ignore();
    
                switch (choice) {
                    case 1:
                        gamble(CASINO_BET_AMOUNT * upgrade.level);
                        break;
                    case 2:
                        gamble(CASINO_BET_AMOUNT * 2 * upgrade.level);
                        break;
                    case 3:
                        gamble((CASINO_BET_AMOUNT / 2) * upgrade.level);
                        break;
                    case 4:
                        if (upgrade.canUpgrade()) upgrade.upgrade(funds);
                        else cout << "Invalid option. Try again!\n";
                        break;
                    case 5:
                        cout << "🚶 Leaving Casino...\n";
                        clearScreen();
                        return;
                    default:
                        cout << "Invalid option. Try again!\n";
                }
            }
        }

    private:
        void gamble(int bet) {
            if (funds >= bet) {
                funds -= bet;
                int roll = rand() % 2;
                cout << "🎲 Rolling... (3 seconds)\n";
                this_thread::sleep_for(chrono::seconds(3));
    
                if (roll == 1) {
                    cout << "🎉 You WON " << bet * 2 << " funds!\n";
                    funds += bet * 2;
                } else {
                    cout << "😢 You LOST the gamble.\n";
                }
                cout << "Current funds: " << funds << "\n";
            } else {
                cout << "❌ Not enough funds!\n";
            }
        }
    public:
        int getUpgradeLevel() const { return upgrade.level; }
    };

// --------- New Environment Class ---------
class Environment {
private:
    int& funds;
    int& ecoPoints;
    int& pollutionLevel;
    int& level;
    int& levelPoints;
    int& health;
    int& hunger;
    int treesPlanted;
    BuildingUpgrade upgrade;
    const string& cityName;
public:
    Environment(int& f, int& e, int& p, int& l, int& lp, int& h, int& hu, const string& cn, int up=1) 
        : funds(f), ecoPoints(e), pollutionLevel(p), level(l), levelPoints(lp), health(h), hunger(hu), cityName(cn), treesPlanted(0) { upgrade.level = up; }

    void enter() {
        clearScreen();
        cout << ENVIRONMENT_ART;
        while (true) {
            displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
            cout << "\n🌳 Welcome to the Environment Center (Level " << upgrade.level << ")!\n";
            cout << "1. 🌱 Plant a Tree (Costs " << (TREE_COST * upgrade.level) << " funds, +" << (TREE_ECO_POINTS * upgrade.level) << " ecoPoints, -" << (TREE_POLLUTION_REDUCTION * upgrade.level) << " pollution)\n";
            cout << "2. 📊 View Statistics\n";
            if (upgrade.canUpgrade()) cout << "3. Upgrade Environment Center (Cost: " << upgrade.upgradeCost() << " funds)\n";
            cout << "4. 🚪 Exit\n";
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
                    if (upgrade.canUpgrade()) upgrade.upgrade(funds);
                    else cout << "Invalid option. Try again!\n";
                    break;
                case 4:
                    cout << "🚶 Leaving Environment Center...\n";
                    clearScreen();
                    return;
                default:
                    cout << "Invalid option. Try again!\n";
            }
        }
    }

    int getUpgradeLevel() const { return upgrade.level; }

private:
    void plantTree() {
        if (funds >= TREE_COST * upgrade.level) {
            funds -= TREE_COST * upgrade.level;
            ecoPoints += TREE_ECO_POINTS * upgrade.level;
            pollutionLevel -= TREE_POLLUTION_REDUCTION * upgrade.level;
            treesPlanted++;
            
            // Generate news about tree planting
            string title = "New Trees Planted in " + cityName;
            string content = "The Environment Center has successfully planted new trees, contributing to the city's green initiative. Total trees planted: " + to_string(treesPlanted);
            generateNews("🌱 Environmental", title, content, true);
            
            cout << "🌱 Tree planted! Total trees: " << treesPlanted << "\n";
            achievements.treesPlanted++;
            achievements.checkAndUnlock();
            resetChallengeIfNeeded(dailyChallenge);
            if (!dailyChallenge.completed) {
                dailyChallenge.progress++;
                checkChallengeCompletion(dailyChallenge, funds, ecoPoints);
            }
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

// --------- Summit Class ---------
class Summit {
private:
    int& funds;
    int& ecoPoints;
    int& level;
    string currentUser;
    string secondUser;
    int secondUserFunds;
    int& loanAmount;
    bool& hasLoan;
    string& lenderName;
    string& loanStartDate;

    void loadSecondUserData(const string& filename, int &fundsOut) {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            while (getline(file, line)) {
                stringstream ss(line);
                string label;
                ss >> label;
                if (label == "funds:") {
                    ss >> fundsOut;
                }
            }
            file.close();
        }
    }

    void updateUserFunds(const string& filename, int newFunds) {
        // Read all lines, update funds, write back
        ifstream file(filename);
        vector<string> lines;
        string line;
        while (getline(file, line)) {
            if (line.find("funds:") == 0) {
                lines.push_back("funds: " + to_string(newFunds));
            } else {
                lines.push_back(line);
            }
        }
        file.close();
        ofstream out(filename);
        for (const auto& l : lines) out << l << endl;
        out.close();
    }

    void updateLoanStatus(const string& filename, bool hasLoan, int loanAmount, const string& lenderName) {
        // Read all lines, update or add loan info, write back
        ifstream file(filename);
        vector<string> lines;
        string line;
        bool foundHasLoan = false, foundLoanAmount = false, foundLender = false;
        while (getline(file, line)) {
            if (line.find("hasLoan:") == 0) {
                lines.push_back("hasLoan: " + to_string(hasLoan));
                foundHasLoan = true;
            } else if (line.find("loanAmount:") == 0) {
                lines.push_back("loanAmount: " + to_string(loanAmount));
                foundLoanAmount = true;
            } else if (line.find("lenderName:") == 0) {
                lines.push_back("lenderName: " + lenderName);
                foundLender = true;
            } else {
                lines.push_back(line);
            }
        }
        if (!foundHasLoan) lines.push_back("hasLoan: " + to_string(hasLoan));
        if (!foundLoanAmount) lines.push_back("loanAmount: " + to_string(loanAmount));
        if (!foundLender) lines.push_back("lenderName: " + lenderName);
        file.close();
        ofstream out(filename);
        for (const auto& l : lines) out << l << endl;
        out.close();
    }

public:
    Summit(int& f, int& e, int& l, const string& user, bool& hLoan, int& lAmount, string& lName, string& lStartDate)
        : funds(f), ecoPoints(e), level(l), currentUser(user), loanAmount(lAmount), hasLoan(hLoan), lenderName(lName), loanStartDate(lStartDate) {}

    void enter() {
        if (level < SUMMIT_REQUIRED_LEVEL) {
            cout << "❌ You need to be at least level " << SUMMIT_REQUIRED_LEVEL << " to host a summit!\n";
            return;
        }
        if (funds < SUMMIT_FUNDS_COST) {
            cout << "❌ You need " << SUMMIT_FUNDS_COST << " funds to host a summit!\n";
            return;
        }
        if (hasLoan) {
            cout << "❌ You already have an active loan from " << lenderName << ". Repay it before taking another loan.\n";
        }
        cout << "\n🌍 Welcome to the Eco Summit!\n";
        cout << "Host: " << currentUser << "\n";
        cout << "Please enter the name of the second participant: ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, secondUser);
        cout << "Please enter the PIN of " << secondUser << ": ";
        string secondPin;
        cin >> secondPin;
        cin.ignore();
        string secondUserFile = secondUser + "_" + secondPin + ".txt";
        ifstream checkFile(secondUserFile);
        if (!checkFile.good()) {
            cout << "❌ Invalid credentials for the second participant!\n";
            return;
        }
        // Load second user's data
        loadSecondUserData(secondUserFile, secondUserFunds);
        cout << "\n=== Eco Summit Begins ===\n";
        cout << "Participants: " << currentUser << " and " << secondUser << "\n\n";
        while (true) {
            cout << "\nSummit Activities:\n";
            cout << "1. Share Eco-Friendly Practices\n";
            cout << "2. Discuss Pollution Reduction Strategies\n";
            cout << "3. Plan Community Cleanup Events\n";
            cout << "4. Exchange Environmental Knowledge\n";
            cout << "5. Sign Eco-Partnership Agreement\n";
            cout << "6. Request Loan\n";
            cout << "7. Repay Loan\n";
            cout << "8. End Summit\n";
            cout << "Choose an activity: ";
            int choice;
            cin >> choice;
            cin.ignore();
            int requestAmount = 0;
            int payment = 0;
            int repayAmount = 0;
            switch (choice) {
                case 1:
                    cout << "\n🌱 " << currentUser << " and " << secondUser << " share their eco-friendly practices:\n";
                    cout << "- Using reusable shopping bags\n";
                    cout << "- Implementing water conservation techniques\n";
                    cout << "- Reducing plastic waste\n";
                    cout << "- Using public transportation\n";
                    cout << "- Composting organic waste\n";
                    cout << "- Using energy-efficient appliances\n";
                    cout << "- Participating in local environmental programs\n";
                    break;
                case 2:
                    cout << "\n🌫️ " << currentUser << " and " << secondUser << " discuss pollution reduction:\n";
                    cout << "- Implementing green energy solutions\n";
                    cout << "- Reducing carbon footprint\n";
                    cout << "- Promoting recycling programs\n";
                    cout << "- Supporting clean air initiatives\n";
                    cout << "- Developing waste management systems\n";
                    cout << "- Creating green spaces in urban areas\n";
                    cout << "- Implementing sustainable transportation\n";
                    break;
                case 3:
                    cout << "\n🧹 " << currentUser << " and " << secondUser << " plan community cleanup:\n";
                    cout << "- Organizing monthly cleanup drives\n";
                    cout << "- Planting trees in public spaces\n";
                    cout << "- Creating awareness campaigns\n";
                    cout << "- Establishing recycling centers\n";
                    cout << "- Setting up community gardens\n";
                    cout << "- Organizing environmental workshops\n";
                    cout << "- Implementing neighborhood composting\n";
                    break;
                case 4:
                    cout << "\n📚 " << currentUser << " and " << secondUser << " exchange environmental knowledge:\n";
                    cout << "- Sustainable farming techniques\n";
                    cout << "- Renewable energy solutions\n";
                    cout << "- Waste management strategies\n";
                    cout << "- Environmental conservation methods\n";
                    cout << "- Water conservation techniques\n";
                    cout << "- Green building practices\n";
                    cout << "- Wildlife protection strategies\n";
                    break;
                case 5:
                    cout << "\n🤝 " << currentUser << " and " << secondUser << " sign an eco-partnership agreement:\n";
                    cout << "- Committing to reduce carbon emissions\n";
                    cout << "- Supporting each other's green initiatives\n";
                    cout << "- Sharing resources and knowledge\n";
                    cout << "- Working together for a sustainable future\n";
                    cout << "- Regular environmental progress meetings\n";
                    cout << "- Joint community projects\n";
                    cout << "- Resource sharing program\n";
                    break;
                case 6:
                    if (hasLoan) {
                        cout << "❌ You already have an active loan!\n";
                        break;
                    }
                    cout << "\n💰 Loan Request\n";
                    cout << secondUser << "'s available funds: " << secondUserFunds << "\n";
                    cout << "Enter amount to request (0 to cancel): ";
                    cin >> requestAmount;
                    cin.ignore();
                    if (requestAmount <= 0) {
                        cout << "Loan request cancelled.\n";
                        break;
                    }
                    if (requestAmount > secondUserFunds) {
                        cout << "❌ " << secondUser << " doesn't have enough funds!\n";
                        break;
                    }
                    loanAmount = requestAmount;
                    hasLoan = true;
                    lenderName = secondUser;
                    funds += loanAmount;
                    secondUserFunds -= loanAmount;
                    // Update both users' files
                    updateUserFunds(secondUserFile, secondUserFunds);
                    updateLoanStatus(currentUser + "_" + lenderName + ".txt", hasLoan, loanAmount, lenderName);
                    updateLoanStatus(secondUserFile, false, 0, "");
                    cout << "✅ Loan of " << loanAmount << " funds received from " << secondUser << "!\n";
                    cout << "You will need to repay " << (loanAmount + (loanAmount * LOAN_INTEREST_RATE / 100)) << " funds.\n";
                    achievements.loansRepaid++;
                    achievements.checkAndUnlock();
                    loanStartDate = getCurrentDateStrLoan();
                    leaveMessage(secondUserFile, currentUser, "loan request");
                    break;
                case 7:
                    if (!hasLoan) {
                        cout << "❌ You don't have any active loans!\n";
                        break;
                    }
                    repayAmount = loanAmount + (loanAmount * LOAN_INTEREST_RATE / 100);
                    if (hasLoan && !loanStartDate.empty()) {
                        int days = daysBetween(loanStartDate, getCurrentDateStrLoan());
                        if (days > LOAN_DAYS_BEFORE_PENALTY) {
                            int penalty = (loanAmount * LOAN_LATE_PENALTY / 100);
                            repayAmount += penalty;
                            cout << "⚠️ Late repayment! Extra penalty applied: " << penalty << " funds.\n";
                        }
                    }
                    cout << "\n💰 Loan Repayment\n";
                    cout << "Amount to repay: " << repayAmount << " funds\n";
                    cout << "Your current funds: " << funds << "\n";
                    cout << "Enter amount to repay (0 to cancel): ";
                    cin >> payment;
                    cin.ignore();
                    if (payment <= 0) {
                        cout << "Repayment cancelled.\n";
                        break;
                    }
                    if (payment > funds) {
                        cout << "❌ You don't have enough funds!\n";
                        break;
                    }
                    if (payment < repayAmount) {
                        cout << "⚠️ Partial payment made. You still owe " << (repayAmount - payment) << " funds.\n";
                        loanAmount = repayAmount - payment;
                        funds -= payment;
                        updateLoanStatus(currentUser + "_" + lenderName + ".txt", hasLoan, loanAmount, lenderName);
                    } else {
                        cout << "✅ Loan fully repaid!\n";
                        funds -= repayAmount;
                        loanAmount = 0;
                        hasLoan = false;
                        lenderName = "";
                        updateLoanStatus(currentUser + "_" + lenderName + ".txt", hasLoan, loanAmount, lenderName);
                        loanStartDate = "";
                    }
                    achievements.loansRepaid++;
                    achievements.checkAndUnlock();
                    break;
                case 8:
                    cout << "\n✅ Summit completed successfully!\n";
                    cout << "💰 Cost: " << SUMMIT_FUNDS_COST << " funds\n";
                    cout << "🌱 Gained: " << SUMMIT_ECO_POINTS << " eco points\n";
                    funds -= SUMMIT_FUNDS_COST;
                    ecoPoints += SUMMIT_ECO_POINTS;
                    achievements.summitsHosted++;
                    achievements.checkAndUnlock();
                    resetChallengeIfNeeded(weeklyChallenge);
                    if (!weeklyChallenge.completed) {
                        weeklyChallenge.progress++;
                        checkChallengeCompletion(weeklyChallenge, funds, ecoPoints);
                    }
                    leaveMessage(secondUserFile, currentUser, "eco summit");
                    clearScreen();
                    return;
                default:
                    cout << "Invalid choice!\n";
                    break;
            }
        }
    }
};

// --------- New Building: Recycling Center ---------
class RecyclingCenter {
private:
    int& funds;
    int& pollutionLevel;
    int& level;
    int& levelPoints;
    int& ecoPoints;
    int& health;
    int& hunger;
    BuildingUpgrade upgrade;
    const string& cityName;
public:
    RecyclingCenter(int& f, int& p, int& l, int& lp, int& e, int& h, int& hu, const string& cn, int up=1) 
        : funds(f), pollutionLevel(p), level(l), levelPoints(lp), ecoPoints(e), health(h), hunger(hu), cityName(cn) { upgrade.level = up; }
    void enter() {
        clearScreen();
        cout << RECYCLING_ART;
        while (true) {
            displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
            cout << "\n♻️ Welcome to the Recycling Center (Level " << upgrade.level << ")!\n";
            cout << "1. Recycle Waste (Reduces pollution by " << (10 * upgrade.level) << ")\n";
            if (upgrade.canUpgrade()) cout << "2. Upgrade Center (Cost: " << upgrade.upgradeCost() << " funds)\n";
            cout << "3. Exit\nChoose an option: ";
            int choice;
            cin >> choice;
            cin.ignore();
            if (choice == 1) {
                int reduction = 10 * upgrade.level;
                int oldPollution = pollutionLevel;
                pollutionLevel = max(0, pollutionLevel - reduction);
                
                // Generate news about recycling
                string title = "Recycling Initiative Success in " + cityName;
                string content = "The Recycling Center has successfully processed waste, reducing pollution from " + to_string(oldPollution) + " to " + to_string(pollutionLevel);
                generateNews("🌱 Environmental", title, content);
                
                cout << "You recycled waste! Pollution reduced from " << oldPollution << " to " << pollutionLevel << ".\n";
                this_thread::sleep_for(chrono::seconds(2));
                clearScreen();
            } else if (choice == 2 && upgrade.canUpgrade()) {
                upgrade.upgrade(funds);
            } else if ((choice == 2 && !upgrade.canUpgrade()) || choice == 3) {
                cout << "Leaving Recycling Center...\n";
                clearScreen();
                return;
            } else {
                cout << "Invalid option. Try again!\n";
            }
        }
    }

    int getUpgradeLevel() const { return upgrade.level; }
};

// --------- New Building: Community Garden ---------
class CommunityGarden {
private:
    int& funds;
    int& ecoPoints;
    int& health;
    int& level;
    int& levelPoints;
    int& pollutionLevel;
    int& hunger;
    BuildingUpgrade upgrade;
    const string& cityName;
public:
    CommunityGarden(int& f, int& e, int& h, int& l, int& lp, int& p, int& hu, const string& cn, int up=1) 
        : funds(f), ecoPoints(e), health(h), level(l), levelPoints(lp), pollutionLevel(p), hunger(hu), cityName(cn) { upgrade.level = up; }
    void enter() {
        clearScreen();
        cout << GARDEN_ART;
        while (true) {
            displayStatus(funds, health, hunger, level, levelPoints, ecoPoints, pollutionLevel, cityName);
            cout << "\n🌻 Welcome to the Community Garden (Level " << upgrade.level << ")!\n";
            cout << "1. Volunteer Gardening (+" << (5 * upgrade.level) << " eco points, +" << (3 * upgrade.level) << " health)\n";
            if (upgrade.canUpgrade()) cout << "2. Upgrade Garden (Cost: " << upgrade.upgradeCost() << " funds)\n";
            cout << "3. Exit\nChoose an option: ";
            int choice;
            cin >> choice;
            cin.ignore();
            if (choice == 1) {
                ecoPoints += 5 * upgrade.level;
                health = min(health + 3 * upgrade.level, MAX_HEALTH);
                cout << "You volunteered in the garden! Eco points and health increased.\n";
            } else if (choice == 2 && upgrade.canUpgrade()) {
                upgrade.upgrade(funds);
            } else if ((choice == 2 && !upgrade.canUpgrade()) || choice == 3) {
                cout << "Leaving Community Garden...\n";
                clearScreen();
                return;
            } else {
                cout << "Invalid option. Try again!\n";
            }
        }
    }

    int getUpgradeLevel() const { return upgrade.level; }
};

// --------- Environmental News Feed ---------
string getCurrentTimeStr() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", localtime(&t));
    return string(buf);
}

void generateNews(const string& category, const string& title, const string& content, bool isImportant) {
    NewsItem news;
    news.title = title;
    news.content = content;
    news.category = category;
    news.timestamp = getCurrentTimeStr();
    news.isImportant = isImportant;
    newsFeed.push_back(news);
    
    // Create file if it doesn't exist and append news
    ofstream out("news_feed.txt", ios::app);
    if (!out.is_open()) {
        cerr << "Error: Could not open news_feed.txt for writing\n";
        return;
    }
    out << "[" << news.timestamp << "] " << news.category << " - " << news.title << "\n";
    out << news.content << "\n\n";
    out.close();
    
    // Print the news to console as well
    cout << "\n📰 Breaking News!\n";
    cout << "[" << news.timestamp << "] " << news.category << " - " << news.title << "\n";
    cout << content << "\n\n";
    this_thread::sleep_for(chrono::seconds(2));
}

void generateRandomNews(int level, int ecoPoints, int pollutionLevel, const string& cityName) {
    static default_random_engine rng(static_cast<unsigned>(time(nullptr)));
    uniform_int_distribution<int> dist(1, 100);
    
    if (dist(rng) <= 20) { // 20% chance to generate random news
        vector<string> categories = {"🌱 Environmental", "🏙️ City Development", "💰 Economy", "🌍 Global"};
        vector<string> environmentalNews = {
            "Local Wildlife Returns to City Parks",
            "New Species of Birds Spotted in Urban Areas",
            "Community Garden Yields Record Harvest",
            "Solar Panel Installation Reaches New Heights",
            "Air Quality Shows Significant Improvement"
        };
        vector<string> cityNews = {
            "New Eco-Friendly Buildings Rise in Downtown",
            "Public Transport System Expands",
            "City Announces Green Energy Initiative",
            "Urban Forest Project Takes Root",
            "Smart City Technology Implementation Begins"
        };
        vector<string> economyNews = {
            "Green Jobs Market Booms",
            "Eco-Tourism Revenue Increases",
            "Sustainable Business Awards Announced",
            "Local Farmers Market Success",
            "Renewable Energy Investment Grows"
        };
        vector<string> globalNews = {
            "International Climate Summit Results",
            "Global Green Technology Exchange",
            "Worldwide Pollution Reduction Goals Met",
            "International Eco-City Partnership",
            "Global Environmental Treaty Signed"
        };
        
        string category = categories[dist(rng) % categories.size()];
        string title;
        string content;
        
        if (category == "🌱 Environmental") {
            title = environmentalNews[dist(rng) % environmentalNews.size()];
            content = "Environmental conditions in " + cityName + " continue to improve. Local initiatives are showing positive results.";
        } else if (category == "🏙️ City Development") {
            title = cityNews[dist(rng) % cityNews.size()];
            content = "City development projects are progressing well. New infrastructure is being built with sustainability in mind.";
        } else if (category == "💰 Economy") {
            title = economyNews[dist(rng) % economyNews.size()];
            content = "The local economy is thriving with a focus on sustainable development and green initiatives.";
        } else {
            title = globalNews[dist(rng) % globalNews.size()];
            content = "Global environmental efforts are making progress. International cooperation is key to success.";
        }
        
        generateNews(category, title, content);
    }
}

void showNewsFeed() {
    clearScreen();
    cout << "\n📰 Environmental News Feed\n";
    cout << "========================\n\n";
    
    ifstream in("news_feed.txt");
    if (!in.is_open()) {
        cout << "No news to display yet. Take actions in the city to generate news!\n\n";
        cout << "Press Enter to continue...";
        cin.ignore();
        clearScreen();
        return;
    }
    
    string line;
    bool hasNews = false;
    
    while (getline(in, line)) {
        if (!line.empty()) {
            hasNews = true;
            cout << line << "\n";
        }
    }
    
    if (!hasNews) {
        cout << "No news to display yet. Take actions in the city to generate news!\n";
    }
    
    cout << "\nPress Enter to continue...";
    cin.ignore();
    cin.get();
    clearScreen();
}

struct RandomEvent {
    string description;
    function<void(int&, int&, int&, int&)> effect;  // Modified to take references to game state variables
};

vector<RandomEvent> randomEvents = {
    {"🌧️ Heavy Rainfall: Pollution decreases!", 
     [](int& funds, int& health, int& ecopoints, int& pollutionlevel) { 
         cout << "Pollution level decreased by 10!\n"; 
         pollutionlevel = max(0, pollutionlevel - 10); 
     }},
    {"💸 Eco Grant: You receive a government grant!", 
     [](int& funds, int& health, int& ecopoints, int& pollutionlevel) { 
         cout << "Funds increased by 200!\n"; 
         funds += 200; 
     }},
    {"🦠 Disease Outbreak: Health drops!", 
     [](int& funds, int& health, int& ecopoints, int& pollutionlevel) { 
         cout << "Health decreased by 20!\n"; 
         health = max(0, health - 20); 
     }},
    {"🌪️ Pollution Spike: Pollution increases!", 
     [](int& funds, int& health, int& ecopoints, int& pollutionlevel) { 
         cout << "Pollution level increased by 15!\n"; 
         pollutionlevel += 15; 
     }},
    {"🌱 Community Cleanup: Eco points increased!", 
     [](int& funds, int& health, int& ecopoints, int& pollutionlevel) { 
         cout << "Eco points increased by 30!\n"; 
         ecopoints += 30; 
     }},
    {"💰 Unexpected Expense: Funds decreased!", 
     [](int& funds, int& health, int& ecopoints, int& pollutionlevel) { 
         cout << "Funds decreased by 100!\n"; 
         funds = max(0, funds - 100); 
     }}
};

void triggerRandomEvent(int& funds, int& health, int& ecopoints, int& pollutionlevel) {
    static default_random_engine rng(static_cast<unsigned>(time(nullptr)));
    uniform_int_distribution<int> dist(1, 10); // 10% chance per menu loop
    if (dist(rng) == 1) {
        uniform_int_distribution<size_t> eventDist(0, randomEvents.size() - 1);
        size_t idx = eventDist(rng);
        cout << "\n=== Random Event! ===\n";
        cout << randomEvents[idx].description << "\n";
        randomEvents[idx].effect(funds, health, ecopoints, pollutionlevel);
        cout << "====================\n";
    }
}

// --------- Function Implementations ---------
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void displayStatusBar(int value, int max, const string& label, const string& color) {
    const int barWidth = 20;
    float ratio = static_cast<float>(value) / max;
    int filled = static_cast<int>(barWidth * ratio);
    cout << color << label << " [";
    for (int i = 0; i < barWidth; ++i) {
        if (i < filled) cout << "█";
        else cout << "░";
    }
    cout << "] " << value << "/" << max << "\033[0m\n";
}

void displayStatus(int funds, int health, int hunger, int level, int levelPoints, int ecoPoints, int pollutionLevel, const string& cityName) {
    cout << "\n🏙️ City: " << cityName << "\n";
    cout << "=== Status ===\n";
    cout << "💰 Funds: " << funds << "\n";
    displayStatusBar(health, MAX_HEALTH, "❤️ Health", "\033[1;31m");
    displayStatusBar(hunger, MAX_HUNGER, "🍽️ Hunger", "\033[1;33m");
    cout << "📊 Level: " << level << " (Points: " << levelPoints << "/" << LEVEL_UP_THRESHOLD << ")\n";
    cout << "🌱 Eco Points: " << ecoPoints << "\n";
    displayStatusBar(pollutionLevel, 100, "🌫️ Pollution", "\033[1;37m");
    cout << "==============\n";
}

void updateLevel(int& level, int& levelPoints, int ecoPoints, int pollutionLevel) {
    int totalPoints = ecoPoints - (pollutionLevel * POLLUTION_PENALTY);
    levelPoints = totalPoints % LEVEL_UP_THRESHOLD;
    level = (totalPoints / LEVEL_UP_THRESHOLD) + 1;
}

void updateHungerAndHealth(int& health, int& hunger, time_t& lastUpdateTime) {
    time_t currentTime = time(nullptr);
    int timeDiff = static_cast<int>(difftime(currentTime, lastUpdateTime));
    
    if (timeDiff >= 60) { // Update every minute
        hunger = max(0, hunger - (HUNGER_DECREASE_RATE * (timeDiff / 60)));
        if (hunger < 20) {
            health = max(0, health - (HEALTH_DECREASE_RATE * (timeDiff / 60)));
        }
        lastUpdateTime = currentTime;
    }
}

void transport_delay(int vehicle, int& ecoPoints, int& pollutionLevel) {
    switch (vehicle) {
        case 0: // Walking
            cout << "🚶 Walking...\n";
            this_thread::sleep_for(chrono::seconds(WALK_DELAY));
            break;
        case 1: // Cycling
            cout << "🚲 Cycling...\n";
            this_thread::sleep_for(chrono::seconds(CYCLE_DELAY));
            ecoPoints += 5;
            break;
        case 2: // Regular Car
            cout << "🚗 Driving...\n";
            this_thread::sleep_for(chrono::seconds(CAR_DELAY));
            ecoPoints -= CAR_ECO_PENALTY;
            pollutionLevel += CAR_POLLUTION_INCREASE;
            break;
        case 3: // Electric Car
            cout << "🚗 Driving (Electric)...\n";
            this_thread::sleep_for(chrono::seconds(CAR_DELAY));
            ecoPoints += ELECTRIC_CAR_ECO_BONUS;
            break;
    }
}

void update_funds_periodically(int& funds, time_t& lastUpdateTime) {
    time_t currentTime = time(nullptr);
    int timeDiff = static_cast<int>(difftime(currentTime, lastUpdateTime));
    
    if (timeDiff >= PERIODIC_FUNDS_INTERVAL) {
        funds += PERIODIC_FUNDS_REWARD * (timeDiff / PERIODIC_FUNDS_INTERVAL);
        lastUpdateTime = currentTime;
    }
}

void offline_bonus(int& funds, time_t& lastSaveTime) {
    time_t currentTime = time(nullptr);
    int days = static_cast<int>(difftime(currentTime, lastSaveTime) / (24 * 3600));
    
    if (days > 0) {
        funds += OFFLINE_BONUS_PER_DAY * days;
        cout << "💰 Offline bonus: " << (OFFLINE_BONUS_PER_DAY * days) << " funds\n";
    }
}

bool load_game(string filename, int& level, int& ecopoints, int& funds, int& health, int& hunger,
    int &houseUp, int &hospitalUp, int &officeUp, int &restaurantUp, int &schoolUp, int &bankUp, int &casinoUp, int &envUp, int &recUp, int &gardenUp,
    int& vehicle, int& pollutionlevel, time_t& lastSaveTime, int& levelPoints,
    bool &hasLoan, int &loanAmount, string &lenderName, string &loanStartDate, string &cityName) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string label;
        ss >> label;
        if (label == "level:") ss >> level;
        else if (label == "ecopoints:") ss >> ecopoints;
        else if (label == "funds:") ss >> funds;
        else if (label == "health:") ss >> health;
        else if (label == "hunger:") ss >> hunger;
        else if (label == "houseUp:") ss >> houseUp;
        else if (label == "hospitalUp:") ss >> hospitalUp;
        else if (label == "officeUp:") ss >> officeUp;
        else if (label == "restaurantUp:") ss >> restaurantUp;
        else if (label == "schoolUp:") ss >> schoolUp;
        else if (label == "bankUp:") ss >> bankUp;
        else if (label == "casinoUp:") ss >> casinoUp;
        else if (label == "envUp:") ss >> envUp;
        else if (label == "recUp:") ss >> recUp;
        else if (label == "gardenUp:") ss >> gardenUp;
        else if (label == "vehicle:") ss >> vehicle;
        else if (label == "pollutionlevel:") ss >> pollutionlevel;
        else if (label == "lastSaveTime:") ss >> lastSaveTime;
        else if (label == "levelPoints:") ss >> levelPoints;
        else if (label == "hasLoan:") ss >> hasLoan;
        else if (label == "loanAmount:") ss >> loanAmount;
        else if (label == "lenderName:") ss >> lenderName;
        else if (label == "loanStartDate:") ss >> loanStartDate;
        else if (label == "cityName:") getline(ss, cityName), cityName = cityName.substr(1);
        else if (label == "hasBicycle:") ss >> hasBicycle;
        else if (label == "hasCar:") ss >> hasCar;
        else if (label == "hasElectricCar:") ss >> hasElectricCar;
    }
    file.close();
    return true;
}

void save_game(string filename, int level, int ecopoints, int funds, int health, int hunger,
    int houseUp, int hospitalUp, int officeUp, int restaurantUp, int schoolUp, int bankUp, int casinoUp, int envUp, int recUp, int gardenUp,
    int vehicle, int pollutionlevel, time_t lastSaveTime, int levelPoints, bool hasLoan, int loanAmount, string lenderName, string loanStartDate, string cityName) {
    ofstream file(filename);
    if (file.is_open()) {
        file << "level: " << level << endl;
        file << "ecopoints: " << ecopoints << endl;
        file << "funds: " << funds << endl;
        file << "health: " << health << endl;
        file << "hunger: " << hunger << endl;
        file << "houseUp: " << houseUp << endl;
        file << "hospitalUp: " << hospitalUp << endl;
        file << "officeUp: " << officeUp << endl;
        file << "restaurantUp: " << restaurantUp << endl;
        file << "schoolUp: " << schoolUp << endl;
        file << "bankUp: " << bankUp << endl;
        file << "casinoUp: " << casinoUp << endl;
        file << "envUp: " << envUp << endl;
        file << "recUp: " << recUp << endl;
        file << "gardenUp: " << gardenUp << endl;
        file << "vehicle: " << vehicle << endl;
        file << "pollutionlevel: " << pollutionlevel << endl;
        file << "lastSaveTime: " << lastSaveTime << endl;
        file << "levelPoints: " << levelPoints << endl;
        file << "hasLoan: " << hasLoan << endl;
        file << "loanAmount: " << loanAmount << endl;
        file << "lenderName: " << lenderName << endl;
        file << "loanStartDate: " << loanStartDate << endl;
        file << "cityName: " << cityName << endl;
        file << "hasBicycle: " << hasBicycle << endl;
        file << "hasCar: " << hasCar << endl;
        file << "hasElectricCar: " << hasElectricCar << endl;
        file.close();
    }
}

// ------------ Main Menu ------------
void menu(int &funds, int &vehicle, int &level, time_t &lastUpdateTime, int &ecopoints, int &health, int &hunger, int &levelPoints, int &pollutionLevel, const string& currentUser, bool &hasLoan, int &loanAmount, string &lenderName, string &loanStartDate,
    int &houseUp, int &hospitalUp, int &officeUp, int &restaurantUp, int &schoolUp, int &bankUp, int &casinoUp, int &envUp, int &recUp, int &gardenUp, const string& cityName)
{
    time_t lastHealthUpdate = time(nullptr);
    int initialFunds = funds;
    static int daysWithZeroPollution = 0;
    static int lastMenuVisit = 0;  // Track menu visits instead of real time
    
    while (true)
    {
        // Check for game over conditions
        if (health <= 0 || hunger <= 0) {
            clearScreen();
            cout << "\n❌ GAME OVER ❌\n";
            cout << "Your health or hunger reached zero!\n";
            cout << "Final Stats:\n";
            cout << "Level: " << level << "\n";
            cout << "Eco Points: " << ecopoints << "\n";
            cout << "Funds: " << funds << "\n";
            return;
        }
        
        // Check for win condition (every 10 menu visits = 1 in-game day)
        lastMenuVisit++;
        if (lastMenuVisit >= 10) {  // One in-game day has passed
            if (pollutionLevel == 0) {
                daysWithZeroPollution++;
                if (level >= 10 && daysWithZeroPollution >= 5) {
                    clearScreen();
                    cout << "\n🎉 VICTORY! 🎉\n";
                    cout << "You have achieved the ultimate eco-city!\n";
                    cout << "Final Stats:\n";
                    cout << "Level: " << level << "\n";
                    cout << "Eco Points: " << ecopoints << "\n";
                    cout << "Funds: " << funds << "\n";
                    cout << "Days with Zero Pollution: " << daysWithZeroPollution << "\n";
                    return;
                }
            } else {
                daysWithZeroPollution = 0;
            }
            lastMenuVisit = 0;  // Reset for next in-game day
        }
        
        triggerRandomEvent(funds, health, ecopoints, pollutionLevel);
        update_funds_periodically(funds, lastUpdateTime);
        updateHungerAndHealth(health, hunger, lastHealthUpdate);
        updateLevel(level, levelPoints, ecopoints, pollutionLevel);
        
        // Generate random news periodically
        generateRandomNews(level, ecopoints, pollutionLevel, cityName);
        
        displayStatus(funds, health, hunger, level, levelPoints, ecopoints, pollutionLevel, cityName);

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
        cout << "11. Host Eco Summit\n";
        cout << "12. View Achievements\n";
        cout << "13. View Challenges\n";
        cout << "14. Recycling Center\n";
        cout << "15. Community Garden\n";
        cout << "16. View News Feed\n";
        cout << "0. Save and Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        int netFunds = funds - initialFunds;

        switch (choice)
        {
            case 1:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { House h(funds, ecopoints, health, hunger, level, levelPoints, pollutionLevel, cityName, houseUp); h.enter(); houseUp = h.getUpgradeLevel(); }
                break;
            case 2:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { Hospital h(funds, health, hunger, level, levelPoints, ecopoints, pollutionLevel, cityName, hospitalUp); h.enter(); hospitalUp = h.getUpgradeLevel(); }
                break;
            case 3:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { Office o(funds, health, hunger, level, levelPoints, ecopoints, pollutionLevel, cityName, officeUp); o.enter(); officeUp = o.getUpgradeLevel(); }
                break;
            case 4:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { Restaurant r(funds, ecopoints, hunger, health, level, levelPoints, pollutionLevel, cityName, restaurantUp); r.enter(); restaurantUp = r.getUpgradeLevel(); }
                break;
            case 5:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { School s(ecopoints, health, hunger, level, levelPoints, pollutionLevel, cityName, schoolUp); s.enter(); schoolUp = s.getUpgradeLevel(); }
                break;
            case 6:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { Bank b(funds, health, hunger, level, levelPoints, ecopoints, pollutionLevel, cityName, bankUp); b.enter(); bankUp = b.getUpgradeLevel(); }
                break;
            case 7:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                { Casino c(funds, health, hunger, level, levelPoints, ecopoints, pollutionLevel, cityName, casinoUp); c.enter(); casinoUp = c.getUpgradeLevel(); }
                break;
            case 8:
                cout << "\n=== Transport Options ===\n";
                cout << "Current vehicle: ";
                switch(vehicle) {
                    case 0: cout << "Walking"; break;
                    case 1: cout << "Bicycle"; break;
                    case 2: cout << "Regular Car"; break;
                    case 3: cout << "Electric Car"; break;
                }
                cout << "\n\nAvailable vehicles:\n";
                cout << "0. Walk (Free)\n";
                
                // Show bicycle options based on level and ownership
                if (level >= CYCLE_REQUIRED_LEVEL) {
                    if (hasBicycle) {
                        cout << "1. Cycle (Owned) ✓\n";
                    } else {
                        cout << "1. Buy Cycle (" << CYCLE_COST << " funds)\n";
                    }
                } else {
                    cout << "1. Cycle (Requires level " << CYCLE_REQUIRED_LEVEL << ")\n";
                }
                
                // Show regular car options based on level and ownership
                if (level >= CAR_REQUIRED_LEVEL) {
                    if (hasCar) {
                        cout << "2. Regular Car (Owned) ✓\n";
                    } else {
                        cout << "2. Buy Regular Car (" << CAR_COST << " funds)\n";
                    }
                } else {
                    cout << "2. Regular Car (Requires level " << CAR_REQUIRED_LEVEL << ")\n";
                }
                
                // Show electric car options based on level and ownership
                if (level >= ELECTRIC_CAR_REQUIRED_LEVEL) {
                    if (hasElectricCar) {
                        cout << "3. Electric Car (Owned) ✓\n";
                    } else {
                        cout << "3. Buy Electric Car (" << ELECTRIC_CAR_COST << " funds)\n";
                    }
                } else {
                    cout << "3. Electric Car (Requires level " << ELECTRIC_CAR_REQUIRED_LEVEL << ")\n";
                }
                
                cout << "\nChoose an option: ";
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
                { Environment e(funds, ecopoints, pollutionLevel, level, levelPoints, health, hunger, cityName, envUp); e.enter(); envUp = e.getUpgradeLevel(); }
                break;
            case 11:
                transport_delay(vehicle, ecopoints, pollutionLevel);
                Summit(funds, ecopoints, level, currentUser, hasLoan, loanAmount, lenderName, loanStartDate).enter();
                break;
            case 12:
                achievements.display();
                break;
            case 13:
                showChallenges();
                break;
            case 14:
                { RecyclingCenter r(funds, pollutionLevel, level, levelPoints, ecopoints, health, hunger, cityName, recUp); r.enter(); recUp = r.getUpgradeLevel(); }
                break;
            case 15:
                { CommunityGarden g(funds, ecopoints, health, level, levelPoints, pollutionLevel, hunger, cityName, gardenUp); g.enter(); gardenUp = g.getUpgradeLevel(); }
                break;
            case 16:
                showNewsFeed();
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
    int houseUp = 1, hospitalUp = 1, officeUp = 1, restaurantUp = 1, schoolUp = 1, bankUp = 1, casinoUp = 1, envUp = 1, recUp = 1, gardenUp = 1;
    int vehicle = 0, pollutionlevel = 0;
    time_t lastSaveTime = time(nullptr);
    time_t lastUpdateTime = time(nullptr);
    string currentUser;
    // Loan variables
    bool hasLoan = false;
    int loanAmount = 0;
    string lenderName = "";
    string loanStartDate = "";
    string cityName = "";

    string name, pin, filename;
    cout << "Enter your name: ";
    getline(cin, name);
    currentUser = name;
    cout << "Enter your 4-digit PIN: ";
    cin >> pin;
    cin.ignore();

    filename = name + "_" + pin + ".txt";

    bool fileExists = load_game(filename, level, ecopoints, funds, health, hunger,
        houseUp, hospitalUp, officeUp, restaurantUp, schoolUp, bankUp, casinoUp, envUp, recUp, gardenUp,
        vehicle, pollutionlevel, lastSaveTime, levelPoints, hasLoan, loanAmount, lenderName, loanStartDate, cityName);

    if (!fileExists)
    {
        displayInstructions("eco_city_instructions.txt"); // only show for new users
        level = 1;
        funds = 500;  // Give new players 500 starting funds
        cout << "Name your city: ";
        getline(cin, cityName);
    }

    offline_bonus(funds, lastSaveTime);

    // Generate initial news when game starts
    generateNews("🌱 Environmental", "Welcome to " + cityName + "!", 
                "Welcome to your eco-city! Start building a sustainable future by making environmentally conscious decisions.", true);
    generateNews("🏙️ City Development", "New City Established", 
                "A new eco-friendly city has been established. Citizens are encouraged to participate in sustainable development.", true);

    menu(funds, vehicle, level, lastUpdateTime, ecopoints, health, hunger, levelPoints, pollutionlevel, currentUser, hasLoan, loanAmount, lenderName, loanStartDate,
        houseUp, hospitalUp, officeUp, restaurantUp, schoolUp, bankUp, casinoUp, envUp, recUp, gardenUp, cityName);

    save_game(filename, level, ecopoints, funds, health, hunger,
        houseUp, hospitalUp, officeUp, restaurantUp, schoolUp, bankUp, casinoUp, envUp, recUp, gardenUp,
        vehicle, pollutionlevel, time(nullptr), levelPoints, hasLoan, loanAmount, lenderName, loanStartDate, cityName);

    cout << "Game saved. Goodbye!" << endl;
    return 0;
}
