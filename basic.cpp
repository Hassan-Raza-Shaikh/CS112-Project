#include<iostream>
#include<string>
#include<fstream>
using namespace std;

void filecreation_opening(string &filename, fstream &ufile, int &level)
{
    string name, last4digits;
    int ph_number;
    cout << "Enter your name: " << endl;
    getline(cin, name);
    for (char &c : name)
    {
        c = tolower(c);
    }

    cout << "Enter your pin (4 digits) " << endl;
    cin >> ph_number;
    cin.ignore();

    last4digits = to_string(ph_number % 10000);
    cout << "Last 4 digits of phone number: " << last4digits << endl;

    filename = name + "_" + last4digits + ".txt";
    cout << "Filename: " << filename << endl;

    ufile.open(filename, ios::in | ios::out | ios::app);
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

void check ecopoints(int &ecopoints, int &level)
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