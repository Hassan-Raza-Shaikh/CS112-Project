// copied the contents of the basic file
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "instructions.cpp"
using namespace std;

void filecreation_opening(string &filename, fstream &ufile, int &level, int &ecopoints, int &funds,
                          int &house, int &hospital, int &office, int &restaurant,
                          int &school, int &bank, int &casino, int &vehicle, int &pollutionlevel)
{
    string name, last4digits;
    string pin;
    cout << "Enter your name: " << endl;
    getline(cin, name);
    for (char &c : name)
    {
        c = tolower(c);
    }

    cout << "Enter your pin (4 digits) " << endl;
    cin >> pin;
    cin.ignore();

    filename = name + "_" + pin + ".txt";
    cout << "Filename: " << filename << endl;

    ufile.open(filename, ios::in);

    if (ufile.fail())
    {
        cout << "File not found. Creating a new file..." << endl;
        ufile.clear();                  // Clear the fail state
        ufile.open(filename, ios::out); // Create new file

        // Initialize the file with basic information
        ufile << "name:\n";
        ufile << "level:0\n";
        ufile << "ecopoints:0\n";
        ufile << "funds:500\n";
        ufile << "house-1\n";
        ufile << "hospital-0\n";
        ufile << "office-0\n";
        ufile << "restaurant-0\n";
        ufile << "school-0\n";
        ufile << "bank-0\n";
        ufile << "casino-0\n";
        ufile << "vehicle-0\n";
        ufile << "pollutionlevel-0\n";

        ufile.close(); // Close after writing

        // Reopen for reading and writing
        ufile.open(filename, ios::in | ios::out);
    }
    else
    {
        cout << "File found and opened successfully!" << endl;
        string line;
        while (getline(ufile, line))
        {
            stringstream ss(line);
            string label;
            ss >> label; // Read the label (e.g., "name:", "level:", etc.)

            if (label == "level:")
            {
                ss >> level;
            }
            else if (label == "ecopoints:")
            {
                ss >> ecopoints;
            }
            else if (label == "funds:")
            {
                ss >> funds;
            }
            else if (label == "house-1")
            {
                ss >> house;
            }
            else if (label == "hospital-0")
            {
                ss >> hospital;
            }
            else if (label == "office-0")
            {
                ss >> office;
            }
            else if (label == "restaurant-0")
            {
                ss >> restaurant;
            }
            else if (label == "school-0")
            {
                ss >> school;
            }
            else if (label == "bank-0")
            {
                ss >> bank;
            }
            else if (label == "casino-0")
            {
                ss >> casino;
            }
            else if (label == "vehicle-0")
            {
                ss >> vehicle;
            }
            else if (label == "pollutionlevel-0")
            {
                ss >> pollutionlevel;
            }
        }

        ufile.close(); // Close the file
    }
}
void endgame()
{
    cout << "Game Over! You have reached the maximum level." << endl;
    exit(0);
}
void instructions() {
    
    string filename = "eco_city_instructions.txt";
    //string filename = "C:/Users/hamzz/OneDrive/Documents/GitHub/CS112-Project/eco_city_instructions.txt"; // for hamza
    
    displayInstructions(filename);
}
void check_ecopoints(int &ecopoints, int &level)
{
    if (ecopoints == 0)
    {
        level = 0;
    }
    else if (ecopoints >= 100)
    {
        level = 1;
    }
    else if (ecopoints >= 200)
    {
        level = 2;
    }
    else if (ecopoints >= 300)
    {
        level = 3;
    }
    else if (ecopoints >= 400)
    {
        level = 4;
    }
    else if (ecopoints >= 500)
    {
        level = 5;
    }
    else if (ecopoints >= 600)
    {
        level = 6;
    }
    else if (ecopoints >= 700)
    {
        level = 7;
    }
    else if (ecopoints >= 800)
    {
        level = 8;
    }
    else if (ecopoints >= 900)
    {
        level = 9;
    }
    else
    {
        level = 10;
        endgame();
    }
}


class buildings
{
 /*   protected:
    string name;
    int capacity;
    bool greenCertified;
    int maintenanceCost;
    int ecoImpact;
    
    virtual void displayInfo()  {
        cout << "Building: " << name
             << " | Capacity: " << capacity
             << " | Green Certified: " << (greenCertified ? "Yes" : "No")
             << " | Maintenance: $" << maintenanceCost
             << " | Eco Impact: " << ecoImpact << endl;
    }*/
};
class houses : public buildings
{

};
class offices : public buildings
{

};
class schools : public buildings
{

};
class hospitals : public buildings
{
    /*  1. create a function that allows the user to name the hospital
         and specify the capacity of the hospital( how many patients can be admitted)
        2. On reaching higher levels, user can upgrade the hopital by installing solar panels,
         and or trees to enhance the overall environment 
        3. eco points will be proportional to the capacity increased and hence levels will be upgraded
        4. money needed to maintain the hospital
        5. a function that keeps track of admitted patients and healed patients 

            private:
                int capacity;
                bool greenCertified; // whether it's an eco-hospital
                int healingPower; // how much it improves citizen health
            public:
                hospitals(string name = "General Hospital") 
                    : buildings(name, 200, 20), capacity(100), greenCertified(false), healingPower(15) {}
            
                void healCitizens(vector<int>& citizensHealth) {
                    for (auto& health : citizensHealth) {
                        health += healingPower;
                        if (health > 100) health = 100; // Max cap
                    }
                    typingEffect("🏥 Citizens have been healed! Health improved. 💚", 25);
                }
            
                void upgradeHospital()  {
                    capacity += 50;
                    healingPower += 5;
                    maintenanceCost += 50;
                    ecoImpact += 5; // More impact if upgraded eco-friendly
                    typingEffect("🏥 Hospital upgraded! More citizens treated, better healing!", 25);
                }
            
                void upgradeToGreenHospital() {
                    if (!greenCertified) {
                        greenCertified = true;
                        ecoImpact += 20; // Huge boost!
                        maintenanceCost -= 50; // Solar-powered reduces costs
                        typingEffect("🌱 Congratulations! Your hospital is now Green Certified! ♻️", 25);
                    } else {
                        typingEffect("🏥 This hospital is already Green Certified!", 20);
                    }
                }
            
                void displayInfo() const override {
                    cout << "Building: " << name
                         << " | Capacity: " << capacity
                         << " | Green Certified: " << (greenCertified ? "Yes" : "No")
                         << " | Maintenance: $" << maintenanceCost
                         << " | Eco Impact: " << ecoImpact << endl;
                }
            };
            
    */

};
class restaurants : public buildings
{

};
class banks : public buildings
{

};
class casinos : public buildings
{

};
class transport
{

};
class walk : public transport
{

};
class cycle : public transport
{

};
class car : public transport
{

};

int main()
{
    int level = 0, ecopoints = 0, funds = 500;
    int house = 0, hospital = 0, office = 0, restaurant = 0;
    int school = 0, bank = 0, casino = 0, vehicle = 0, pollutionlevel = 0;
    string filename;
    fstream ufile;
    filecreation_opening(filename, ufile, level, ecopoints, funds,
                         house, hospital, office, restaurant,
                         school, bank, casino, vehicle, pollutionlevel);
    if (level == 0)
    {
        instructions();
    }
    ufile.close();
    cout << ecopoints<< funds<<
    house<< hospital<< office<< restaurant<<
    school<< bank<< casino<< vehicle<< pollutionlevel;
    return 0;
}
