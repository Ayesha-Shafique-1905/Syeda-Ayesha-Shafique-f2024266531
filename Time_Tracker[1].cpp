#include <iostream>
#include <fstream>
#include <string>
#include <ctime>      
#include <cstdlib>   
#include <limits>    

using namespace std;


const int MAX_TASKS = 100;


int    idArr[MAX_TASKS];
bool   runningArr[MAX_TASKS];
double totalHoursArr[MAX_TASKS];
string nameArr[MAX_TASKS];
time_t startTimeArr[MAX_TASKS];


int taskCount = 0;


int nextId = 1;


int findTaskIndexById(int searchId) {
    for (int i = 0; i < taskCount; i++) {
        if (idArr[i] == searchId) {
            return i;
        }
    }
    return -1; 
}


string getFormattedTime(time_t rawTime) {
    tm *timeInfo = localtime(&rawTime);
    if (!timeInfo) {
        return "UNKNOWN_TIME";
    }

    
    char buffer[20];
    
    sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d",
            1900 + timeInfo->tm_year,
            1 + timeInfo->tm_mon,
            timeInfo->tm_mday,
            timeInfo->tm_hour,
            timeInfo->tm_min,
            timeInfo->tm_sec);

    return string(buffer);
}

// Function to list all tasks
void listTasks() {
    if (taskCount == 0) {
        cout << "No tasks available.\n";
        return;
    }

    cout << "\n--- Current Tasks ---\n";
    for (int i = 0; i < taskCount; i++) {
        cout << "ID: " << idArr[i]
             << " | Name: " << nameArr[i]
             << " | Running: " << (runningArr[i] ? "Yes" : "No")
             << " | Total Hours: " << totalHoursArr[i] << "\n";
    }
    cout << "---------------------\n";
}

// Create a new task
void createTask() 
    cin.ignore(std::numeric_limits<streamsize>::max(), '\n');

    if (taskCount >= MAX_TASKS) {
        cout << "Task limit reached. Cannot create more tasks.\n";
        return;
    }

    cout << "Enter task name: ";
    string taskName;
    getline(cin, taskName);

    // Fill arrays
    idArr[taskCount] = nextId;
    nameArr[taskCount] = taskName;
    runningArr[taskCount] = false;
    totalHoursArr[taskCount] = 0.0;
    startTimeArr[taskCount] = 0; // Not running yet

    cout << "Task created with ID " << nextId
         << " and name '" << taskName << "'\n";

    taskCount++;
    nextId++;
}

// Start a timer for a task
void startTimer() {
    listTasks();
    if (taskCount == 0) {
        return;
    }

    cout << "Enter the task ID to start: ";
    int startId;
    cin >> startId;

    int index = findTaskIndexById(startId);
    if (index == -1) {
        cout << "Task with ID " << startId << " not found.\n";
        return;
    }

    if (runningArr[index]) {
        cout << "Task is already running.\n";
        return;
    }

    runningArr[index] = true;
    startTimeArr[index] = time(NULL); // Record current time

    cout << "Started task '" << nameArr[index]
         << "' at " << getFormattedTime(startTimeArr[index]) << "\n";
}

// Stop a timer for a task
void stopTimer() {
    listTasks();
    if (taskCount == 0) {
        return;
    }

    cout << "Enter the task ID to stop: ";
    int stopId;
    cin >> stopId;

    int index = findTaskIndexById(stopId);
    if (index == -1) {
        cout << "Task with ID " << stopId << " not found.\n";
        return;
    }

    if (!runningArr[index]) {
        cout << "Task is not currently running.\n";
        return;
    }

    runningArr[index] = false;
    time_t stopTime = time(NULL);

    // Calculate difference in hours
    double seconds = difftime(stopTime, startTimeArr[index]);
    double hours = seconds / 3600.0;
    totalHoursArr[index] += hours;

    cout << "Stopped task '" << nameArr[index]
         << "' at " << getFormattedTime(stopTime) << "\n";
    cout << "Duration: " << hours << " hour(s)\n";
}

// Add manual entry (in hours) to a task
void addManualEntry() {
    listTasks();
    if (taskCount == 0) {
        return;
    }

    cout << "Enter the task ID for manual entry: ";
    int manualId;
    cin >> manualId;

    int index = findTaskIndexById(manualId);
    if (index == -1) {
        cout << "Task with ID " << manualId << " not found.\n";
        return;
    }

    double hours;
    cout << "Enter number of hours to add manually (e.g., 1.5): ";
    cin >> hours;

    totalHoursArr[index] += hours;

    cout << "Added " << hours << " hour(s) to '"
         << nameArr[index] << "'. Total Hours: "
         << totalHoursArr[index] << "\n";
}

// Save tasks to a file
void saveTasksToFile() {
    ofstream ofs("tasks_data.txt");
    if (!ofs.is_open()) {
        cout << "Error opening file for saving.\n";
        return;
    }

    // Format (one task per line):
    // ID|Name|Running(0/1)|TotalHours
    for (int i = 0; i < taskCount; i++) {
        ofs << idArr[i] << "|"
            << nameArr[i] << "|"
            << (runningArr[i] ? 1 : 0) << "|"
            << totalHoursArr[i] << "\n";
    }

    ofs.close();
    cout << "Tasks saved to tasks_data.txt\n";
}

// Load tasks from a file
void loadTasksFromFile() {
    ifstream ifs("tasks_data.txt");
    if (!ifs.is_open()) {
        cout << "No existing task data found (tasks_data.txt not found).\n";
        return;
    }

    // Reset current tasks
    taskCount = 0;
    nextId = 1;

    string line;
    while (getline(ifs, line)) {
        if (line.empty()) {
            continue;
        }

        // We expect: ID|NAME|RUNNING|TOTAL_HOURS
        // Let's parse this naively using find + substr
        int pos1 = line.find('|');
        if (pos1 == (int)string::npos) continue;
        int pos2 = line.find('|', pos1 + 1);
        if (pos2 == (int)string::npos) continue;
        int pos3 = line.find('|', pos2 + 1);
        if (pos3 == (int)string::npos) continue;

        string idStr      = line.substr(0, pos1);
        string nameStr    = line.substr(pos1 + 1, pos2 - (pos1 + 1));
        string runningStr = line.substr(pos2 + 1, pos3 - (pos2 + 1));
        string hoursStr   = line.substr(pos3 + 1);

        int   readId     = atoi(idStr.c_str());
        bool  readRunning = (runningStr == "1");
        double readHours  = atof(hoursStr.c_str());

        // Store in arrays
        idArr[taskCount]        = readId;
        nameArr[taskCount]      = nameStr;
        runningArr[taskCount]   = readRunning;
        totalHoursArr[taskCount] = readHours;
        startTimeArr[taskCount] = 0; // Not storing startTime from file

        // Update nextId so it won't conflict with existing
        if (readId >= nextId) {
            nextId = readId + 1;
        }

        taskCount++;
        if (taskCount >= MAX_TASKS) {
            break; // We won't load more than MAX_TASKS
        }
    }

    ifs.close();
    cout << "Tasks loaded from tasks_data.txt\n";
}

int main() {
    loadTasksFromFile();

    while (true) {
        cout << "\n========== TIME TRACKER MENU ==========\n"
             << "1. List All Tasks\n"
             << "2. Create New Task\n"
             << "3. Start Timer for Task\n"
             << "4. Stop Timer for Task\n"
             << "5. Add Time Manually\n"
             << "6. Save Tasks to File\n"
             << "7. Exit\n"
             << "=======================================\n"
             << "Enter your choice: ";

        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                listTasks();
                break;
            case 2:
                createTask();
                break;
            case 3:
                startTimer();
                break;
            case 4:
                stopTimer();
                break;
            case 5:
                addManualEntry();
                break;
            case 6:
                saveTasksToFile();
                break;
            case 7:
                // Optionally save before exit
                saveTasksToFile();
                cout << "Exiting the application...\n";
                return 0;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }

    return 0;
}
