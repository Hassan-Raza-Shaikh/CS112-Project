#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>
#include "instructions.cpp"
using namespace std;

// --------- Building Classes ---------
class House {
    public:
        void enter() {
            while (true) {
                std::cout << "\n🏠 Welcome to your House!\n";
                std::cout << "1. 🛌 Sleep\n";
                std::cout << "2. 🍴 Eat\n";
                std::cout << "3. 🎮 Relax\n";
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
                        return; // Exit from house menu
                    default:
                        std::cout << "Invalid option. Try again!\n";
                }
            }
        }
    
    private:
        void sleep() {
            std::cout << "😴 Sleeping... (5 seconds)\n";
            std::this_thread::sleep_for(std::chrono::seconds(5));
            std::cout << "You feel refreshed!\n";
        }
    
        void eat() {
            std::cout << "🍽️ Eating a meal... (-20 funds, 3 seconds)\n";
            std::this_thread::sleep_for(std::chrono::seconds(3));
            std::cout << "You feel energized!\n";
            // You can also link it to actual "funds" later
        }
    
        void relax() {
            std::cout << "🎮 Relaxing... (+5 eco points, 2 seconds)\n";
            std::this_thread::sleep_for(std::chrono::seconds(2));
            std::cout << "You feel happy!\n";
            // Same here, can add eco points link
        }
    };

class Hospital {
private:
    int& funds;
public:
    Hospital(int& f) : funds(f) {}

    void enter() {
        while (true) {
            std::cout << "\n🏥 Welcome to the Hospital!\n";
            std::cout << "1. 💉 Get Treatment (Costs 50 funds)\n";
            std::cout << "2. 🚪 Exit Hospital\n";
            std::cout << "Choose an option: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 1) {
                if (funds >= 50) {
                    std::cout << "💉 Treating... (3 seconds)\n";
                    funds -= 50;
                    std::this_thread::sleep_for(std::chrono::seconds(3));
                    std::cout << "You feel much better! (Funds left: " << funds << ")\n";
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

class Office {
    private:
        int& funds;
    public:
        Office(int& f) : funds(f) {}
    
        void enter() {
            while (true) {
                std::cout << "\n🏢 Welcome to your Office!\n";
                std::cout << "1. 💼 Work (+100 funds)\n";
                std::cout << "2. 🚪 Exit Office\n";
                std::cout << "Choose an option: ";
    
                int choice;
                std::cin >> choice;
                std::cin.ignore();
    
                if (choice == 1) {
                    std::cout << "💼 Working... (5 seconds)\n";
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    funds += 100;
                    std::cout << "You earned 100 funds! (Total funds: " << funds << ")\n";
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

    class Restaurant {
        private:
            int& funds;
            int& ecoPoints;
        public:
            Restaurant(int& f, int& e) : funds(f), ecoPoints(e) {}
        
            void enter() {
                while (true) {
                    std::cout << "\n🍴 Welcome to the Restaurant!\n";
                    std::cout << "1. 🍔 Eat Premium Meal (Costs 50 funds, +20 ecoPoints)\n";
                    std::cout << "2. 🚪 Exit Restaurant\n";
                    std::cout << "Choose an option: ";
        
                    int choice;
                    std::cin >> choice;
                    std::cin.ignore();
        
                    if (choice == 1) {
                        if (funds >= 50) {
                            std::cout << "🍽️ Eating... (4 seconds)\n";
                            funds -= 50;
                            ecoPoints += 20;
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

class School {
    private:
        int& ecoPoints;
    public:
        School(int& e) : ecoPoints(e) {}
    
        void enter() {
            while (true) {
                std::cout << "\n🎓 Welcome to School!\n";
                std::cout << "1. 📚 Study (+10 ecoPoints)\n";
                std::cout << "2. 🚪 Exit School\n";
                std::cout << "Choose an option: ";
    
                int choice;
                std::cin >> choice;
                std::cin.ignore();
    
                if (choice == 1) {
                    std::cout << "📚 Studying... (4 seconds)\n";
                    std::this_thread::sleep_for(std::chrono::seconds(4));
                    ecoPoints += 10;
                    std::cout << "You gained 10 ecoPoints! (Total ecoPoints: " << ecoPoints << ")\n";
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

    class Bank {
        private:
            int& funds;
        public:
            Bank(int& f) : funds(f) {}
        
            void enter() {
                while (true) {
                    std::cout << "\n🏦 Welcome to the Bank!\n";
                    std::cout << "1. 💰 Deposit and get interest (+2% funds)\n";
                    std::cout << "2. 🚪 Exit Bank\n";
                    std::cout << "Choose an option: ";
        
                    int choice;
                    std::cin >> choice;
                    std::cin.ignore();
        
                    if (choice == 1) {
                        std::cout << "💰 Depositing and waiting for interest... (5 seconds)\n";
                        std::this_thread::sleep_for(std::chrono::seconds(5));
                        int interest = (funds * 2) / 100;
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
                        std::cout << "1. 🎲 Gamble (Bet 100 funds)\n";
                        std::cout << "2. 🚪 Exit Casino\n";
                        std::cout << "Choose an option: ";
            
                        int choice;
                        std::cin >> choice;
                        std::cin.ignore();
            
                        if (choice == 1) {
                            if (funds >= 100) {
                                funds -= 100;
                                int roll = std::rand() % 2;
                                std::cout << "🎲 Rolling... (3 seconds)\n";
                                std::this_thread::sleep_for(std::chrono::seconds(3));
            
                                if (roll == 1) {
                                    std::cout << "🎉 You WON 200 funds!\n";
                                    funds += 200;
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


// ---------- File Saving & Loading ----------
void save_game(string filename, int level, int ecopoints, int funds, int house, int hospital, int office, int restaurant,
               int school, int bank, int casino, int vehicle, int pollutionlevel, time_t lastSaveTime)
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
        ufile.close();
    }
    else
    {
        cout << "Error saving game!" << endl;
    }
}

bool load_game(string filename, int &level, int &ecopoints, int &funds,
               int &house, int &hospital, int &office, int &restaurant,
               int &school, int &bank, int &casino, int &vehicle, int &pollutionlevel, time_t &lastSaveTime)
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
        return false;
    }
}

// ----------- Fund Management -----------
void offline_bonus(int &funds, time_t &lastSaveTime)
{
    time_t now = time(nullptr);
    double seconds_passed = difftime(now, lastSaveTime);
    double days_passed = seconds_passed / (60 * 60 * 24);

    int bonus = static_cast<int>(days_passed * 1000);
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
    if (difftime(now, lastUpdateTime) >= 60) // 60 seconds
    {
        funds += 10;
        lastUpdateTime = now;
        cout << "(+10 funds for being active!) Current funds: " << funds << endl;
    }
}

void transport_delay(int vehicle)
{
    if (vehicle == 0) // Walking
    {
        cout << "Walking... Please wait 10 seconds.\n";
        this_thread::sleep_for(chrono::seconds(10));
    }
    else if (vehicle == 1) // Cycling
    {
        cout << "Cycling... Please wait 5 seconds.\n";
        this_thread::sleep_for(chrono::seconds(5));
    }
    else if (vehicle == 2) // Car
    {
        cout << "Driving... Immediate arrival!\n";
    }
}

// ------------ Main Menu ------------
void menu(int &funds, int &vehicle, int &level, time_t &lastUpdateTime)
{
    while (true)
    {
        update_funds_periodically(funds, lastUpdateTime);

        cout << "\n--- Eco City Menu ---\n";
        cout << "1. Go to House\n";
        cout << "2. Go to Hospital\n";
        cout << "3. Go to Office\n";
        cout << "4. Go to Restaurant\n";
        cout << "5. Go to School\n";
        cout << "6. Go to Bank\n";
        cout << "7. Go to Casino\n";
        cout << "8. Change Transport Mode (Walk / Cycle / Car)\n";
        cout << "9. View Instructions\n";
        cout << "0. Save and Exit\n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
        case 1:
            transport_delay(vehicle);
            House().enter();
            break;
        case 2:
            transport_delay(vehicle);
            Hospital().enter();
            break;
        case 3:
            transport_delay(vehicle);
            Office().enter();
            break;
        case 4:
            transport_delay(vehicle);
            Restaurant().enter();
            break;
        case 5:
            transport_delay(vehicle);
            School().enter();
            break;
        case 6:
            transport_delay(vehicle);
            Bank().enter();
            break;
        case 7:
            transport_delay(vehicle);
            Casino().enter();
            break;
        case 8:
            cout << "Choose your transport:\n";
            cout << "0. Walk\n1. Cycle\n2. Car\nChoice: ";
            cin >> vehicle;
            break;
        case 9:
            displayInstructions("eco_city_instructions.txt");
            break;
        case 0:
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
                                school, bank, casino, vehicle, pollutionlevel, lastSaveTime);

    if (!fileExists)
    {
        displayInstructions("eco_city_instructions.txt"); // only show for new users
        level = 1;
    }

    offline_bonus(funds, lastSaveTime);

    menu(funds, vehicle, level, lastUpdateTime);

    save_game(filename, level, ecopoints, funds, house, hospital, office, restaurant,
              school, bank, casino, vehicle, pollutionlevel, time(nullptr));

    cout << "Game saved. Goodbye!" << endl;
    return 0;
}
