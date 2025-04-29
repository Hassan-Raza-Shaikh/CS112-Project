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
        cout << "Welcome to your House!\n";
        cout << "Here you can rest and upgrade your living space.\n";
    }
};

class Hospital {
public:
    void enter() {
        cout << "Welcome to the Hospital!\n";
        cout << "You can heal here or help others to gain eco points.\n";
    }
};

class Office {
public:
    void enter() {
        cout << "Welcome to your Office!\n";
        cout << "Work to earn funds and reputation.\n";
    }
};

class Restaurant {
public:
    void enter() {
        cout << "Welcome to the Restaurant!\n";
        cout << "Relax and socialize here.\n";
    }
};

class School {
public:
    void enter() {
        cout << "Welcome to the School!\n";
        cout << "Learn and earn eco points for knowledge.\n";
    }
};

class Bank {
public:
    void enter() {
        cout << "Welcome to the Bank!\n";
        cout << "Manage your funds wisely.\n";
    }
};

class Casino {
public:
    void enter() {
        cout << "Welcome to the Casino!\n";
        cout << "Gamble your funds for a chance to win big (or lose!).\n";
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
            instructions();
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
        instructions(); // only show for new users
        level = 1;
    }

    offline_bonus(funds, lastSaveTime);

    menu(funds, vehicle, level, lastUpdateTime);

    save_game(filename, level, ecopoints, funds, house, hospital, office, restaurant,
              school, bank, casino, vehicle, pollutionlevel, time(nullptr));

    cout << "Game saved. Goodbye!" << endl;
    return 0;
}
