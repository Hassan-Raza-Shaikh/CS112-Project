#include<iostream>
#include<string>
#include<fstream>
using namespace std;

int main()
{
    string filename, name, last4digits;
    int ph_number;
    cout << "Enter your name: " << endl;
    getline(cin, name);
    for (char &c : name)
    {
        c = tolower(c);
    }

    cout << "Enter your phone number: " << endl;
    cin >> ph_number;
    cin.ignore(); // Clear the input buffer

    last4digits = to_string(ph_number % 10000);
    cout << "Last 4 digits of phone number: " << last4digits << endl;

    filename = name + "_" + last4digits + ".txt";
    cout << "Filename: " << filename << endl;

    fstream ufile;

    ufile.open(filename, ios::in | ios::out | ios::app);
}