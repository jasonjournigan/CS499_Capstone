#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <regex>

// Course class to store course data
class Course {
public:
    std::string courseNumber;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    Course(const std::string& num, const std::string& title, const std::vector<std::string>& prereqs)
        : courseNumber(num), courseTitle(title), prerequisites(prereqs) {}
};

// Function to split a string by delimiter
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Function to validate course number format (e.g., CSCI101)
bool isValidCourseNumber(const std::string& courseNumber) {
    std::regex pattern("^[A-Z]{4}[0-9]{3}$"); // e.g., CSCI101
    return std::regex_match(courseNumber, pattern);
}

// Function to read and parse CSV file into an unordered_map and sorted vector
bool loadCoursesFromFile(const std::string& filename, std::unordered_map<std::string, Course>& courseMap, std::vector<Course>& sortedCourses) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Unable to open file '" << filename << "'." << std::endl;
        return false;
    }

    courseMap.clear();
    sortedCourses.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() < 2) {
            std::cout << "Warning: Invalid line in file: " << line << std::endl;
            continue;
        }

        std::string courseNumber = tokens[0];
        if (!isValidCourseNumber(courseNumber)) {
            std::cout << "Warning: Invalid course number format: " << courseNumber << std::endl;
            continue;
        }

        std::string courseTitle = tokens[1];
        std::vector<std::string> prerequisites;
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty() && isValidCourseNumber(tokens[i])) {
                prerequisites.push_back(tokens[i]);
            }
        }

        Course course(courseNumber, courseTitle, prerequisites);
        courseMap[courseNumber] = course;
        sortedCourses.push_back(course);
    }

    // Sort the vector once during loading
    std::sort(sortedCourses.begin(), sortedCourses.end(),
        [](const Course& a, const Course& b) {
            return a.courseNumber < b.courseNumber;
        });

    file.close();
    return true;
}

// Function to print all courses in alphanumeric order
void printCourseList(const std::vector<Course>& sortedCourses) {
    if (sortedCourses.empty()) {
        std::cout << "No courses loaded. Please load a file first." << std::endl;
        return;
    }

    std::cout << "\nList of All Courses (Alphanumeric Order):\n" << std::endl;
    for (const auto& course : sortedCourses) {
        std::cout << course.courseNumber << ": " << course.courseTitle << std::endl;
    }
}

// Function to print course information and prerequisites
void printCourseInfo(const std::unordered_map<std::string, Course>& courseMap, const std::string& courseNumber) {
    if (courseMap.empty()) {
        std::cout << "No courses loaded. Please load a file first." << std::endl;
        return;
    }

    // Validate course number format
    std::string upperCourseNumber = courseNumber;
    std::transform(upperCourseNumber.begin(), upperCourseNumber.end(), upperCourseNumber.begin(), ::toupper);
    if (!isValidCourseNumber(upperCourseNumber)) {
        std::cout << "Error: Invalid course number format. Must be like CSCI101." << std::endl;
        return;
    }

    auto it = courseMap.find(upperCourseNumber);
    if (it == courseMap.end()) {
        std::cout << "Error: Course '" << courseNumber << "' not found." << std::endl;
        return;
    }

    const Course& course = it->second;
    std::cout << "\nCourse Information:\n" << std::endl;
    std::cout << "Course Number: " << course.courseNumber << std::endl;
    std::cout << "Course Title: " << course.courseTitle << std::endl;
    std::cout << "Prerequisites: ";
    if (course.prerequisites.empty()) {
        std::cout << "None" << std::endl;
    } else {
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            auto prereqIt = courseMap.find(course.prerequisites[i]);
            std::string prereqTitle = (prereqIt != courseMap.end()) ? prereqIt->second.courseTitle : "Unknown";
            std::cout << course.prerequisites[i] << " (" << prereqTitle << ")";
            if (i < course.prerequisites.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}

// Function to display the menu
void displayMenu() {
    std::cout << "\nABCU Advising Assistance Program\n" << std::endl;
    std::cout << "1. Load Course Data" << std::endl;
    std::cout << "2. Print Alphanumeric Course List" << std::endl;
    std::cout << "3. Print Course Information" << std::endl;
    std::cout << "9. Exit" << std::endl;
    std::cout << "\nEnter your choice (1, 2, 3, or 9): ";
}

int main() {
    std::unordered_map<std::string, Course> courseMap;
    std::vector<Course> sortedCourses;
    std::string input;

    while (true) {
        displayMenu();
        std::getline(std::cin, input);

        if (input != "1" && input != "2" && input != "3" && input != "9") {
            std::cout << "Error: Invalid choice. Please enter 1, 2, 3, or 9." << std::endl;
            continue;
        }

        int choice = std::stoi(input);

        if (choice == 1) {
            std::cout << "Enter the course data file name (e.g., CS 300 ABCU_Advising_Program_Input.csv): ";
            std::getline(std::cin, input);
            if (loadCoursesFromFile(input, courseMap, sortedCourses)) {
                std::cout << "File '" << input << "' loaded successfully." << std::endl;
            }
        } else if (choice == 2) {
            printCourseList(sortedCourses);
        } else if (choice == 3) {
            std::cout << "Enter the course number (e.g., CSCI101): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                printCourseInfo(courseMap, input);
            } else {
                std::cout << "Error: Course number cannot be empty." << std::endl;
            }
        } else if (choice == 9) {
            std::cout << "Exiting program. Goodbye!" << std::endl;
            break;
        }
    }

    return 0;
}