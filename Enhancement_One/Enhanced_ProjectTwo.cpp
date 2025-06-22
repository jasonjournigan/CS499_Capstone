#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_map>
#include <cctype>

// Course class to store course data
class Course {
public:
    std::string courseNumber;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    Course(const std::string& num, const std::string& title, const std::vector<std::string>& prereqs)
        : courseNumber(num), courseTitle(title), prerequisites(prereqs) {}
};

// Function to trim whitespace from a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

// Function to validate course number format (e.g., CSCI101)
bool isValidCourseNumber(const std::string& courseNum) {
    if (courseNum.length() < 5 || courseNum.length() > 8) return false;
    for (char c : courseNum) {
        if (!std::isalnum(c)) return false;
    }
    return true;
}

// Function to split a string by delimiter and return a vector of strings
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (std::getline(ss, token, delimiter)) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    return tokens;
}

// Function to read and parse CSV file into a vector and hash map of Course objects
bool loadCoursesFromFile(const std::string& filename, std::vector<Course>& courses, 
                        std::unordered_map<std::string, Course>& courseMap) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Unable to open file '" << filename << "'. Please check the file path." << std::endl;
        return false;
    }

    courses.clear();
    courseMap.clear();
    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        ++lineNumber;
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        std::vector<std::string> tokens = split(line, ',');
        if (tokens.size() < 2) {
            std::cout << "Warning: Invalid line " << lineNumber << " in file: '" << line 
                      << "'. Expected at least 2 fields (course number, title)." << std::endl;
            continue;
        }

        std::string courseNumber = trim(tokens[0]);
        std::string courseTitle = trim(tokens[1]);
        if (!isValidCourseNumber(courseNumber)) {
            std::cout << "Warning: Invalid course number in line " << lineNumber 
                      << ": '" << courseNumber << "'. Skipping." << std::endl;
            continue;
        }

        std::vector<std::string> prerequisites;
        for (size_t i = 2; i < tokens.size(); ++i) {
            std::string prereq = trim(tokens[i]);
            if (!prereq.empty() && isValidCourseNumber(prereq)) {
                prerequisites.push_back(prereq);
            } else if (!prereq.empty()) {
                std::cout << "Warning: Invalid prerequisite '" << prereq 
                          << "' in line " << lineNumber << ". Skipping prerequisite." << std::endl;
            }
        }

        Course course(courseNumber, courseTitle, prerequisites);
        courses.emplace_back(course);
        courseMap[courseNumber] = course;
    }

    file.close();
    if (courses.empty()) {
        std::cout << "Error: No valid courses loaded from '" << filename << "'." << std::endl;
        return false;
    }
    std::cout << "Successfully loaded " << courses.size() << " courses from '" << filename << "'." << std::endl;
    return true;
}

// Function to print all courses in alphanumeric order
void printCourseList(const std::vector<Course>& courses) {
    if (courses.empty()) {
        std::cout << "No courses loaded. Please load a file first." << std::endl;
        return;
    }

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

// Function to print course information and prerequisites using hash map
void printCourseInfo(const std::unordered_map<std::string, Course>& courseMap, 
                     const std::string& courseNumber) {
    if (courseMap.empty()) {
        std::cout << "No courses loaded. Please load a file first." << std::endl;
        return;
    }

    std::string upperCourseNumber = trim(courseNumber);
    std::transform(upperCourseNumber.begin(), upperCourseNumber.end(), upperCourseNumber.begin(), ::toupper);

    auto it = courseMap.begin();
    for (; it != courseMap.end(); ++it) {
        std::string upperCurrentCourse = it->first;
        std::transform(upperCurrentCourse.begin(), upperCurrentCourse.end(), upperCurrentCourse.begin(), ::toupper);
        if (upperCurrentCourse == upperCourseNumber) {
            break;
        }
    }

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
    std::vector<Course> courses;
    std::unordered_map<std::string, Course> courseMap;
    std::string input;

    while (true) {
        displayMenu();
        std::getline(std::cin, input);
        input = trim(input);

        if (input != "1" && input != "2" && input != "3" && input != "9") {
            std::cout << "Error: Invalid choice. Please enter 1, 2, 3, or 9." << std::endl;
            continue;
        }

        int choice = std::stoi(input);

        if (choice == 1) {
            std::cout << "Enter the course data file name (e.g., CS 300 ABCU_Advising_Program_Input.csv): ";
            std::getline(std::cin, input);
            input = trim(input);
            if (input.empty()) {
                std::cout << "Error: File name cannot be empty." << std::endl;
                continue;
            }
            loadCoursesFromFile(input, courses, courseMap);
        } else if (choice == 2) {
            printCourseList(courses);
        } else if (choice == 3) {
            std::cout << "Enter the course number (e.g., CSCI101): ";
            std::getline(std::cin, input);
            input = trim(input);
            if (input.empty() || !isValidCourseNumber(input)) {
                std::cout << "Error: Invalid course number. Must be alphanumeric (e.g., CSCI101)." << std::endl;
                continue;
            }
            printCourseInfo(courseMap, input);
        } else if (choice == 9) {
            std::cout << "Exiting program. Goodbye!" << std::endl;
            break;
        }
    }

    return 0;
}