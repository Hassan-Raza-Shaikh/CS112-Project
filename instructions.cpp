#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

// Typing effect: character-by-character printing
void typingEffect(const string& text, int speed = 30) {
    for (char c : text) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(speed));
    }
    cout << endl;
}

// Different printing styles
void printTitle(const string& title) {
    cout << "\033[1;32m"; // Green
    typingEffect(title, 20);
    cout << "\033[0m";
}

void printSubHeading(const string& heading) {
    cout << "\033[1;34m"; // Blue
    typingEffect(heading, 25);
    cout << "\033[0m";
}

void printWarning(const string& warning) {
    cout << "\033[1;31m"; // Red
    typingEffect(warning, 30);
    cout << "\033[0m";
}

void printTips(const string& tip) {
    cout << "\033[1;36m"; // Cyan
    typingEffect(tip, 25);
    cout << "\033[0m";
}

void printNormal(const string& line) {
    typingEffect(line, 30);
}

// 📄 Function to display instructions
void displayInstructions(const string& filename) {
    ifstream file(filename);
    string line;

    if (file.is_open()) {
        while (getline(file, line)) {
            if (line.find("WELCOME") != string::npos) {
                printTitle(line);
            }
            else if (line.find("HOW TO PLAY:") != string::npos ||
                     line.find("ECO-POINTS SYSTEM:") != string::npos ||
                     line.find("LEVELS BASED ON ECO-POINTS:") != string::npos ||
                     line.find("YOUR GOAL:") != string::npos ||
                     line.find("TIPS:") != string::npos) {
                printSubHeading(line);
            }
            else if (line.find("WARNING") != string::npos) {
                printWarning(line);
            }
            else if (line.find("-") == 0) {
                printTips(line);
            }
            else {
                printNormal(line);
            }
        }
        file.close();
    } else {
        cout << "Error opening the instruction file!" << endl;
    }

    cout << "\n\033[1;36mPress Enter to continue...\033[0m";
    cin.ignore();
    cin.get();
}

// 🚀 Main Function
int main() {
    string filename = "eco_city_instructions.txt";

    // Call the display function
    displayInstructions(filename);

    // After instructions, you can continue with your game logic
    cout << "\nStarting Smart Eco City Simulation... 🌎🏙️" << endl;

    return 0;
}
