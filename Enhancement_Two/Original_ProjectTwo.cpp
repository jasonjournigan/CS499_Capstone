#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>

// Course class to store course data
class Course {
public:
    std::string courseNumber;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    Course(const std::string& num, const std::string& title, const std::vector<std::string>& prereqs)
        : courseNumber(num), courseTitle(title), prerequisites(prereqs) {}
};

// Function to split a string by delimiter and return a vector of strings
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

// Function to read and parse CSV file into a vector of Course objects
bool loadCoursesFromFile(const std::string& filename, std::vector<Course>& courses) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Unable to open file '" << filename << "'." << std::endl;
        return false;
    }

    courses.clear(); // Clear existing data
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() < 2) {
            std::cout << "Warning: Invalid line in file: " << line << std::endl;
            continue;
        }

        std::string courseNumber = tokens[0];
        std::string courseTitle = tokens[1];
        std::vector<std::string> prerequisites;
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (!tokens[i].empty()) {
                prerequisites.push_back(tokens[i]);
            }
        }

        courses.emplace_back(courseNumber, courseTitle, prerequisites);
    }

    file.close();
    return true;
}

// Function to print all courses in alphanumeric order
void printCourseList(const std::vector<Course>& courses) {
    if (courses.empty()) {
        std::cout << "No courses loaded. Please load a file first." << std::endl;
        return;
    }

    // Create a copy of the courses vector for sorting
    std::vector<Course> sortedCourses = courses;
    std::sort(sortedCourses.begin(), sortedCourses.end(),
        [](const Course& a, const Course& b) {
            return a.courseNumber < b.courseNumber;
        });

    std::cout << "\nList of All Courses (Alphanumeric Order):\n" << std::endl;
    for (const auto& course : sortedCourses) {
        std::cout << course.courseNumber << ": " << course.courseTitle << std::endl;
    }
}

// Function to print course information and prerequisites
void printCourseInfo(const std::vector<Course>& courses, const std::string& courseNumber) {
    if (courses.empty()) {
        std::cout << "No courses loaded. Please load a file first." << std::endl;
        return;
    }

    // Convert input to uppercase for case-insensitive comparison
    std::string upperCourseNumber = courseNumber;
    std::transform(upperCourseNumber.begin(), upperCourseNumber.end(), upperCourseNumber.begin(), ::toupper);

    for (const auto& course : courses) {
        std::string upperCurrentCourse = course.courseNumber;
        std::transform(upperCurrentCourse.begin(), upperCurrentCourse.end(), upperCurrentCourse.begin(), ::toupper);

        if (upperCurrentCourse == upperCourseNumber) {
            std::cout << "\nCourse Information:\n" << std::endl;
            std::cout << "Course Number: " << course.courseNumber << std::endl;
            std::cout << "Course Title: " << course.courseTitle << std::endl;
            std::cout << "Prerequisites: ";
            if (course.prerequisites.empty()) {
                std::cout << "None" << std::endl;
            } else {
                for (size_t i = 0; i < course.prerequisites.size(); ++i) {
                    // Find prerequisite course title
                    std::string prereqTitle = "Unknown";
                    for (const auto& c : courses) {
                        if (c.courseNumber == course.prerequisites[i]) {
                            prereqTitle = c.courseTitle;
                            break;
                        }
                    }
                    std::cout << course.prerequisites[i] << " (" << prereqTitle << ")";
                    if (i < course.prerequisites.size() - 1) {
                        std::cout << ", ";
                    }
                }
                std::cout << std::endl;
            }
            return;
        }
    }
    std::cout << "Error: Course '" << courseNumber << "' not found." << std::endl;
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
    std::vector<Course> courses;
    std::string input;

    while (true) {
        displayMenu();
        std::getline(std::cin, input);

        // Validate menu choice
        if (input != "1" && input != "2" && input != "3" && input != "9") {
            std::cout << "Error: Invalid choice. Please enter 1, 2, 3, or 9." << std::endl;
            continue;
        }

        int choice = std::stoi(input);

        if (choice == 1) {
            // Load Course Data
            std::cout << "Enter the course data file name (e.g., CS 300 ABCU_Advising_Program_Input.csv): ";
            std::getline(std::cin, input);
            if (loadCoursesFromFile(input, courses)) {
                std::cout << "File '" << input << "' loaded successfully." << std::endl;
            }
        } else if (choice == 2) {
            // Print Alphanumeric Course List
            printCourseList(courses);
        } else if (choice == 3) {
            // Print Course Information
            std::cout << "Enter the course number (e.g., CSCI101): ";
            std::getline(std::cin, input);
            if (!input.empty()) {
                printCourseInfo(courses, input);
            } else {
                std::cout << "Error: Course number cannot be empty." << std::endl;
            }
        } else if (choice == 9) {
            // Exit
            std::cout << "Exiting program. Goodbye!" << std::endl;
            break;
        }
    }

    return 0;
}