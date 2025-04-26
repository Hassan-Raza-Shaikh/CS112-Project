#include<iostream>
#include<string>
#include<fstream>
using namespace std;

void filecreation_opening(string &filename, fstream &ufile, int &level)
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
        ufile.clear(); // Clear the fail state
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
    }
}



void instructions()
{
    fstream instructions_file;
    instructions_file.open("instructions.txt", ios::in);
    if (!instructions_file)
    {
        cout << "Error opening instructions file." << endl;
        return;
    }
    string line;
    while(getline(instructions_file, line))
    {
        cout << line << endl;
    }
    instructions_file.close();
}

void check_ecopoints(int &ecopoints, int &level)
{
    if(ecopoints == 0)
    {
        level = 0;
    }
    else if(ecopoints >= 100)
    {
        level = 1;
    }
    else if(ecopoints >= 200)
    {
        level = 2;
    }
    else if(ecopoints >= 300)
    {
        level = 3;
    }
    else if(ecopoints >= 400)
    {
        level = 4;
    }
    else if(ecopoints >= 500)
    {
        level = 5;
    }
    else if(ecopoints >= 600)
    {
        level = 6;
    }
    else if(ecopoints >= 700)
    {
        level = 7;
    }
    else if(ecopoints >= 800)
    {
        level = 8;
    }
    else if(ecopoints >= 900)
    {
        level = 9;
    }
    else
    {
        level = 10;
    }
    
}

int main()
{
    string filename;
    fstream ufile;
    int level;
    int ecopoints=0;
    filecreation_opening(filename, ufile, level);
    if(level == 0)
    {
        instructions();
    }
    ufile.close();

    return 0;

}