#include <iostream>
#include <sstream>
#include <list>
#include <string>
#include "sqlite/sqlite3.h"
#include <QCoreApplication>
#include<cmath>
#include <ctime>

using namespace std;

class Employee{
public:
    int id;
    string name;
    string phonenumber;
};

class WorkingHour{
public:
    int id;
    int employeeId;
    string date;
    int startTime;
    int endTime;
    bool isPresent;
    int managerId = 0;
};

class IOManagerDAO{
public:
    virtual list<Employee> getAllEmployees() = 0;
    virtual list<WorkingHour> getEmployeeWorkingHoursByIdAndDate(int employeeId, string date) = 0;
    virtual bool deleteEmployeeWorkingHourById(int staffWorkingHourId) = 0;
    virtual bool addEmployeeWorkingHour(int employeeId, string date, int startTime, int endTime, bool present) = 0;
    virtual bool editEmployeeWorkingHour(int workingHourId, int employeeId, string date, int startTime, int endTime, bool present) = 0;
    virtual WorkingHour getWorkingHourById(int id) = 0;
};

class SQLiteManager : public IOManagerDAO{
    sqlite3_stmt* stmt;
    char* errorMessage;
    sqlite3* db;
    int rc;

    public:SQLiteManager(){
        sqlite3_open("mydb.db", &db);
        createTableIfNotExists("employee(id INTEGER PRIMARY KEY AUTOINCREMENT, name VARCHAR(100), phone VARCHAR(100))");
        createTableIfNotExists("working_hour(id INTEGER PRIMARY KEY AUTOINCREMENT, employee_id INT, date VARCHAR(100), "
                               "start_time INT, end_time INT, is_present INT, manager_id INT)");
        sqlite3_close(db);
    }

    void createTableIfNotExists(string mainCommand){
        string commandStart ="CREATE TABLE IF NOT EXISTS ";
        rc = sqlite3_exec(db, (commandStart + mainCommand + ";").c_str(), NULL, NULL, &errorMessage);

        if(rc != SQLITE_OK) {
            cout << "error : "  << errorMessage;
        }
    }

    list<Employee> getAllEmployees(){

        sqlite3_open("mydb.db", &db);

        list<Employee> toBeReturned = list<Employee>();
        sqlite3_prepare_v2(db, "select id, name, phone from employee;", -1, &stmt, 0);

        Employee employee = Employee();

        while(sqlite3_step(stmt) != SQLITE_DONE){

            employee.id = sqlite3_column_int(stmt,0);
            employee.name = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            employee.phonenumber = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            toBeReturned.push_back(employee);
        }

        sqlite3_close(db);

        return toBeReturned ;
    }

    list<WorkingHour> getEmployeeWorkingHoursByIdAndDate(int employeeId, string date){

        sqlite3_open("mydb.db", &db);

        list<WorkingHour> toBeReturned = list<WorkingHour>();
        sqlite3_prepare_v2(db, ("SELECT id, employee_id, date, start_time, end_time, "
                                "is_present, manager_id FROM working_hour WHERE (employee_id="
                                + to_string(employeeId) + " AND date='" + date + "');").c_str(), -1, &stmt, 0);

        WorkingHour workingHour = WorkingHour();

        while(sqlite3_step(stmt) != SQLITE_DONE){

            workingHour.id = sqlite3_column_int(stmt,0);
            workingHour.employeeId = sqlite3_column_int(stmt,1);
            workingHour.date = convertCharRefToStringstring(sqlite3_column_text(stmt, 2));
            workingHour.startTime = sqlite3_column_int(stmt,3);
            workingHour.endTime = sqlite3_column_int(stmt,4);
            workingHour.isPresent = getIsPresentFromInt(sqlite3_column_int(stmt,5));
            workingHour.managerId = sqlite3_column_int(stmt,6);

            toBeReturned.push_back(workingHour);
        }
        sqlite3_close(db);

        return toBeReturned;
    }

    string convertCharRefToStringstring(const unsigned char* charRef){
        try {
            return reinterpret_cast<const char*>(charRef);
        } catch (...) {
            return "";
        }
    }

    bool getIsPresentFromInt(int isPresentInt){
        return isPresentInt == 1;
    }

    bool deleteEmployeeWorkingHourById(int staffWorkingHourId){
        sqlite3_open("mydb.db", &db);
        string query = "DELETE FROM working_hour WHERE id=" + to_string(staffWorkingHourId) + ";";
        rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errorMessage);
        sqlite3_close(db);

        if(rc != SQLITE_OK){
            cout << "error in deleting an emplyee's working hour : "  << errorMessage;
            return false;
        }
        return true;
    }

    bool addEmployeeWorkingHour(int employeeId, string date, int startTime, int endTime, bool present){

        sqlite3_open("mydb.db", &db);

        string query = "INSERT INTO working_hour(employee_id, date, start_time, end_time, "
                       "is_present, manager_id) VALUES(" + to_string(employeeId) +
                " , '" + date + "', " + to_string(startTime) + ", " + to_string(endTime) +
                ", " + to_string(present ? 1 : 0) + ", 1);";
        rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errorMessage);
        sqlite3_close(db);

        if(rc != SQLITE_OK){
            cout << "error in adding an emplyee's working hour : "  << errorMessage;
            return false;
        }
        return true;
    }

    bool addEmployee(string name, string phone){

        sqlite3_open("mydb.db", &db);

        string query = "INSERT INTO employee(name, phone) VALUES('" + name +
                "' , '" + phone + "');";
        rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errorMessage);
        sqlite3_close(db);

        if(rc != SQLITE_OK){
            cout << "error in adding an emplyee : "  << errorMessage;
            return false;
        }
        return true;
    }

    bool editEmployeeWorkingHour(int workingHourId, int employeeId, string date, int startTime, int endTime, bool present){

        sqlite3_open("mydb.db", &db);

        string query = "UPDATE working_hour SET employee_id = "+to_string(employeeId)+
                ", date = '"+date+"', start_time = " + to_string(startTime)+",end_time = "+
                to_string(endTime)+",is_present = "+to_string(present ? 1 : 0)+" WHERE id = "+
                to_string(workingHourId);

        rc = sqlite3_exec(db, query.c_str(), NULL, NULL, &errorMessage);
        sqlite3_close(db);

        if(rc != SQLITE_OK){
            cout << "error in updating an emplyee's working hour : "  << errorMessage;
            return false;
        }
        return true;
    }

    WorkingHour getWorkingHourById(int id){

        sqlite3_open("mydb.db", &db);

        WorkingHour workingHour = WorkingHour();
        sqlite3_prepare_v2(db, ("SELECT id, employee_id, date, start_time, end_time, "
                                "is_present, manager_id FROM working_hour WHERE id=" +
                                to_string(id)+";").c_str(), -1, &stmt, 0);

        if( sqlite3_step(stmt) != SQLITE_DONE ){

            workingHour.id = sqlite3_column_int(stmt,0);
            workingHour.employeeId = sqlite3_column_int(stmt,1);
            workingHour.date = convertCharRefToStringstring(sqlite3_column_text(stmt, 2));
            workingHour.startTime = sqlite3_column_int(stmt,3);
            workingHour.endTime = sqlite3_column_int(stmt,4);
            workingHour.isPresent = getIsPresentFromInt(sqlite3_column_int(stmt,5));
            workingHour.managerId = sqlite3_column_int(stmt,6);

        }

        sqlite3_close(db);

        return workingHour;
    }
};

class Calendar{

    int year;
    int currentMonth = 0;

    public:Calendar(int year){
        this->year=year;
    }

    int getYear(){
        return year;
    }

    string getCurrentMonthName(){
        return getMonthName(currentMonth);
    }

    int getCurrentMonth(){
        return currentMonth;
    }

    void showMonth(){

        int current = dayNumber(1 , this->currentMonth + 1 , year);
        int days = getNumberOfDays ( this->currentMonth , year);
        string leftSpace = "     ";

        // Print the current month name
        cout << "\n" << leftSpace;
        printf("  ---------- %s %i -----------\n\n",
               getMonthName ( this->currentMonth ).c_str(), year);

        // Print the columns
        //printf("  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n");
        cout << leftSpace << "  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n";

        // Print appropriate spaces
        int k;
        cout << leftSpace;

        for (k = 0; k < current; k++)
            printf("     ");

        for (int j = 1; j <= days; j++)
        {
            printf("%5d", j);

            if (++k > 6)
            {
                k = 0;
                cout << "\n" << leftSpace;
                //printf("\n");
            }
        }

        if (k)
            printf("\n");

    }

    void setMonth(int newMonth){
        this->currentMonth = newMonth;
    }

    void setYear(int newYear){
        this->year = newYear;
    }

    int getNumberOfDays (int monthNumber, int year)
    {
        // January
        if (monthNumber == 0)
            return (31);

        // February
        if (monthNumber == 1)
        {
            // If the year is leap then February has
            // 29 days
            if (year % 400 == 0 ||
                    (year % 4 == 0 && year % 100 != 0))
                return (29);
            else
                return (28);
        }

        // March
        if (monthNumber == 2)
            return (31);

        // April
        if (monthNumber == 3)
            return (30);

        // May
        if (monthNumber == 4)
            return (31);

        // June
        if (monthNumber == 5)
            return (30);

        // July
        if (monthNumber == 6)
            return (31);

        // August
        if (monthNumber == 7)
            return (31);

        // September
        if (monthNumber == 8)
            return (30);

        // October
        if (monthNumber == 9)
            return (31);

        // November
        if (monthNumber == 10)
            return (30);

        // December
        return (31);
    }

    int getNumberOfDaysOfCurrentMonthAndYear ()
    {
        // January
        if (currentMonth == 0)
            return (31);

        // February
        if (currentMonth == 1)
        {
            // If the year is leap then February has
            // 29 days
            if (year % 400 == 0 ||
                    (year % 4 == 0 && year % 100 != 0))
                return (29);
            else
                return (28);
        }

        // March
        if (currentMonth == 2)
            return (31);

        // April
        if (currentMonth == 3)
            return (30);

        // May
        if (currentMonth == 4)
            return (31);

        // June
        if (currentMonth == 5)
            return (30);

        // July
        if (currentMonth == 6)
            return (31);

        // August
        if (currentMonth == 7)
            return (31);

        // September
        if (currentMonth == 8)
            return (30);

        // October
        if (currentMonth == 9)
            return (31);

        // November
        if (currentMonth == 10)
            return (30);

        // December
        return (31);
    }

    string getMonthName(int monthNumber)
    {
        string months[] = {"January", "February", "March",
                           "April", "May", "June",
                           "July", "August", "September",
                           "October", "November", "December"
                          };

        return (months[monthNumber]);
    }

    int dayNumber(int day, int month, int year)
    {

        static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1,
                           4, 6, 2, 4 };
        year -= month < 3;
        return ( year + year/4 - year/100 +
                 year/400 + t[month-1] + day) % 7;
    }

    void decrementMonth(){
        if(currentMonth == 0){
            currentMonth = 11;
            year --;
        }
        else{
            currentMonth --;
        }
    }

    int getActualYear() {
        time_t t = time(NULL);
         tm* tPtr = localtime(&t);
         return (tPtr->tm_year)+1900;
    }

    int getActualMonth() {
        time_t t = time(NULL);
         tm* tPtr = localtime(&t);
         return (tPtr->tm_mon)+1;
    }


    void incrementMonth(){
        if(currentMonth == 11){
            currentMonth = 0;
            year ++;
        }
        else{
            currentMonth ++;
        }
    }

    void init(){
        currentMonth = getActualMonth() -1 ;
        year = getActualYear();
    }
};

void showCalendar(int month, int year){
    Calendar calendar = Calendar(year);
    calendar.setMonth(month);
    calendar.showMonth();
}

class StaffWorkingHours{

    // 0 represents the home with all the employees
    // 1 represents the home without any employee
    // 2 represents the calendar's page
    // 3 represents the working hours for a particular date
    // 4 represents the no working hours for a particular date
    // 5 represents the pick start and end time page

    int states [6] = {0,1,2,3,4,5};
    Employee employee;
    WorkingHour workingHour;
    Calendar calendar = Calendar(2022);
    string leftSpace = "     ";
    string titleMarker = "__________";
    string elementSpacing = "  ";
    int day;

    void displayEmployee(Employee employee, int index){
        cout << leftSpace << index << elementSpacing << employee.name << elementSpacing << employee.phonenumber << "\n";
    }

    void displayWorkingHour(WorkingHour workingHour, int index){
        if(index >= 0){
            cout << leftSpace << index << elementSpacing << formatTime(workingHour.startTime) << elementSpacing << formatTime(workingHour.endTime) << elementSpacing << formatPresence(workingHour.isPresent) + "\n";
        }
        else{
            cout << elementSpacing << formatTime(workingHour.startTime) << elementSpacing << formatTime(workingHour.endTime) << elementSpacing << formatPresence(workingHour.isPresent) + "\n";
        }
    }

    string formatTime(int time){

        if (time/10 < 1){
            return "00:0" + to_string(time);
        }

        else if (time/100 < 1){
            return "00:" + to_string(time);
        }

        else if (time/1000 < 1){
            return "0" + to_string(time/100) + ":" + to_string(time%100);
        }

        else {
            return to_string(time/1000) + to_string( (time % 1000) /100) + ":" + ((time % 100) > 10? to_string(time%100) : "0" + to_string(time%100) );
        }

    }

    string formatPresence(bool presence){

        if(presence) {
            return "present";
        }
        return "absent";

    }

    Employee getEmployeeFromListIndex(list<Employee> employees, int index){
            list<Employee>::iterator it = employees.begin();
            for(int i=0; i<index; i++){
                ++it;
            }
            return *it;
    }

    WorkingHour getWorkingHourFromListIndex(list<WorkingHour> workingHours, int index){
            list<WorkingHour>::iterator it = workingHours.begin();
            for(int i=0; i<index; i++){
                ++it;
            }
            return *it;
    }

    void goToHome(){

        string userInput = "";
        list<string> commands = {};
        int range[] = {0, 0};
        string question = "Enter an employee's number";
        string errorMessage = "Invalid Input";

        SQLiteManager sqliteManager = SQLiteManager();
        list<Employee> employees = sqliteManager.getAllEmployees();
        if(employees.size() > 0){
            range[1] = employees.size();

            system("CLS");
            displayEmployees(employees);
            cout << "\n\n" << leftSpace << question << "\n" << leftSpace;
            cin >> userInput;

            while(!listContains(commands, userInput) && !inputInRange(range, userInput) ){
                system("CLS");
                displayEmployees(employees);
                cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question << "\n" << leftSpace;
                cin >> userInput;
            }
            employee = getEmployeeFromListIndex(employees, std::stoi( userInput) - 1);
            displayState(2);
        }

        else{
            displayState(1);
        }

    }

    void goToWorkingHours(){

        string userInput = "";
        string userInput2 = "";
        list<string> commands = {"add", "back"};
        list<string> secondCommands = {"edit", "delete", "cancel"};
        int range[] = {0, 0};
        string question = "Enter 'add' to add a working hour or \n" +leftSpace+ "a working hour index to edit it.";
        string question2 = "Enter 'edit' to modify this working hour or \n" +leftSpace+ "'delete' to remove it or cancel";
        string errorMessage = "Invalid Input";

        SQLiteManager sqliteManager = SQLiteManager();
        list<WorkingHour> workingHours = sqliteManager.getEmployeeWorkingHoursByIdAndDate(employee.id, formatDate());
        if(workingHours.size() > 0){
            range[1] = workingHours.size();

            system("CLS");
            displayWorkingHours(workingHours);
            cout << "\n\n" << leftSpace << question << "\n" << leftSpace;
            cin >> userInput;

            while(!listContains(commands, userInput) && !inputInRange(range, userInput) ){
                system("CLS");
                displayWorkingHours(workingHours);
                cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question << "\n" << leftSpace;
                cin >> userInput;
            }

            if (listContains(commands, userInput)){
                if(userInput == "add"){
                    displayState(4);
                }
                else{
                    displayState(2);
                }
            }

            else{

                workingHour = getWorkingHourFromListIndex(workingHours, stoi(userInput) - 1);
                cout << "\n" << leftSpace;
                displayWorkingHour(workingHour, -1);
                cout << "\n" << leftSpace << question2 << "\n" << leftSpace;
                cin >> userInput2;

                while(!listContains(secondCommands, userInput2)){
                    system("CLS");
                    displayWorkingHours(workingHours);
                    cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question << "\n" << leftSpace << userInput << "\n\n";

                    cout << leftSpace;
                    displayWorkingHour(workingHour, -1);

                    cout << "\n" << leftSpace << errorMessage << "\n" << leftSpace << question2 << "\n" << leftSpace;
                    cin >> userInput2;
                }

                if(userInput2 == "edit"){
                    displayState(5);
                }

                else if (userInput2 == "delete") {
                    if(sqliteManager.deleteEmployeeWorkingHourById(workingHour.id)){
                        displayState(3);
                    }
                }

                else{
                    displayState(3);
                }
            }

        }
        else{
            displayState(6);
        }

    }

    bool listContains( list<string> list, string toBeSearched){
        if(list.size() > 0){
            return (std::find(list.begin(), list.end(), toBeSearched) != list.end());
        }
        return false;
    }

    bool inputInRange(int range[], string userInput){
        if(range[0] == range[1]){
            return false;
        }
        try {
            int intInput = std::stoi( userInput );
            for(int i = range[0]; i <= range[1]; i ++){
                if(intInput == i){
                    return true;
                }
            }
        } catch (...) {
            return false;
        }
        return false;
    }

    void goToEmptyHome(){
        system("CLS");
        cout << leftSpace << "No employee found\n" << leftSpace;
    }

    void goToCalendar(){

        string userInput = "";
        list<string> commands = {"n", "p", "back"};
        int range[] = {1, 1};
        string question = "Choose the date to view the working hours of " + employee.name;
        string errorMessage = "Invalid Input";
        string description = "press 'p' to view the previous month and 'n' to view the next month";

        calendar.init();
        range[1] = calendar.getNumberOfDaysOfCurrentMonthAndYear();

        system("CLS");
        calendar.showMonth();

        cout << "\n\n" << leftSpace << question << "\n" << leftSpace << description << "\n" << leftSpace;
        cin >> userInput;

        while(!inputInRange(range, userInput) ){

            if(listContains(commands, userInput)){
                if(userInput == "p"){
                    calendar.decrementMonth();
                }
                else if (userInput == "n"){
                    calendar.incrementMonth();
                }
                else {
                    break;
                }
                range[1] = calendar.getNumberOfDaysOfCurrentMonthAndYear();
                system("CLS");
                calendar.showMonth();
                cout << "\n\n" << leftSpace << question << "\n" << leftSpace << description << "\n" << leftSpace;
                cin >> userInput;
            }
            else{
                system("CLS");
                calendar.showMonth();
                cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question << "\n" << leftSpace << description << "\n" << leftSpace;
                cin >> userInput;
            }
        }
        if(userInput == "back"){
            displayState(0);
        }
        else{
            day = std::stoi( userInput );
            displayState(3);
        }



    }

    void goToAddWorkingHour(){

        string userInput1 = "";
        string userInput2 = "";
        string userInput3 = "";
        list<string> commands = {"a", "p"};
        int range[] = {0, 2400};
        string question1 = "Enter the start time of " + employee.name + " on the " + to_string(day) + " "+ calendar.getCurrentMonthName() + " " + to_string(calendar.getYear());
        string question2 = "Enter the end time of " + employee.name + " on the " + to_string(day) + " "+ calendar.getCurrentMonthName() + " " + to_string(calendar.getYear());
        string question3 = "Enter 'p' for present or 'a' for absent";
        string errorMessage = "Invalid Input";
        string secondErrorMessage = "The start time cannot be greater than the end time";
        string description = "Please separate the hour and the minutes by a colon(:)";
        char separator = ':';

        system("CLS");

        cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n" << leftSpace;
        cin >> userInput1;

        while(!inputInRange(range, removeChar (userInput1 , separator) )){
            system("CLS");
            cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n" << leftSpace;
            cin >> userInput1;
        }

        workingHour.startTime = stoi(removeChar (userInput1 , separator));

        cout << "\n\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n" << leftSpace;
        cin >> userInput2;

        while(!inputInRange(range, removeChar (userInput2 , separator) )){
            //while(!inputInRange(range, removeChar (userInput2 , separator) ) || workingHour.startTime > workingHour.endTime){

            //if ( workingHour.startTime > workingHour.endTime ) {
            //    system("CLS");
            //    cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n" << leftSpace << userInput1;
            //    cout << "\n\n" << leftSpace << secondErrorMessage << "\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n" << leftSpace;
            //    cin >> userInput2;
            //}

            //else{
                system("CLS");
                cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n" << leftSpace << userInput1;
                cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n" << leftSpace;
                cin >> userInput2;
            //}

        }

        workingHour.endTime = stoi(removeChar (userInput2 , separator));

        cout << "\n\n" << leftSpace << question3 << "\n" << leftSpace;
        cin >> userInput3;

        while(!listContains(commands, userInput3)){
            system("CLS");
            cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n" << leftSpace << userInput1;
            cout << "\n\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n" << leftSpace << userInput2;
            cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question3 << "\n" << leftSpace;
            cin >> userInput3;
        }

        workingHour.isPresent = userInput3 == "p";

        //day = std::stoi( userInput );
        //cout << "\n\n" << leftSpace << workingHour.startTime << leftSpace << workingHour.endTime << leftSpace << to_string(workingHour.isPresent);
        SQLiteManager sqliteManager;
        if(sqliteManager.addEmployeeWorkingHour( employee.id, formatDate(), workingHour.startTime, workingHour.endTime, workingHour.isPresent )){
            displayState(3);
        }
        //displayState(3);

    }

    string formatDate(){
        return to_string(day) +"-" + to_string(calendar.getCurrentMonth()) + "-" + to_string(calendar.getYear());
    }

    string formatDateForDisplay(){
        return to_string(day) +"-" + to_string(calendar.getCurrentMonth() + 1) + "-" + to_string(calendar.getYear());
    }

    string removeChar(string input, char separator){

        string toBeReturned = "";
        int separatorOccurence = 0;

        for (int i = 0; i < input.length(); i++) {

            if( input[i] != separator ){
              toBeReturned += input[i];
            }
            else{
                separatorOccurence ++;
            }

        }

        if(separatorOccurence != 1){
            return "-1";
        }

        return toBeReturned;
    }

    void goToEmptyWorkingHours(){
        string userInput = "";
        list<string> commands = {"add", "back"};
        string errorMessage = "Invalid Input";

        system("CLS");
        cout << "\n" << leftSpace << titleMarker << "No working hour for " << employee.name << " on " << formatDateForDisplay()
             <<  titleMarker << "\n\n\n" << leftSpace << "Enter 'add' to add a working hour\n" << leftSpace;
        cin >> userInput;

        while(!listContains(commands, userInput)){
            system("CLS");
            cout << "\n" << leftSpace << titleMarker << "No working hour for " << employee.name << " on " << formatDateForDisplay()
                 <<  titleMarker << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << "Enter 'add' to add a working hour\n" << leftSpace;
            cin >> userInput;
        }

        if (userInput == "add"){
            displayState(4);
        }
        else{
            displayState(2);
        }

    }

    void goToEditWorkingHour(){
        string userInput1 = "";
        string userInput2 = "";
        string userInput3 = "";
        list<string> commands = {"a", "p"};
        int range[] = {0, 2400};
        string question1 = "Edit the start time of " + employee.name + " on the " + to_string(day) + " "+ calendar.getCurrentMonthName() + " " + to_string(calendar.getYear());
        string question2 = "Edit the end time of " + employee.name + " on the " + to_string(day) + " "+ calendar.getCurrentMonthName() + " " + to_string(calendar.getYear());
        string question3 = "Enter 'p' for present or 'a' for absent";
        string errorMessage = "Invalid Input";
        string secondErrorMessage = "The start time cannot be greater than the end time";
        string description = "Please separate the hour and the minutes by a colon(:)";
        char separator = ':';

        system("CLS");

        cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous start time: "
             << formatTime(workingHour.startTime) << "\n" << leftSpace << "new start time: ";

        cin >> userInput1;

        while(!inputInRange(range, removeChar (userInput1 , separator) )){

            system("CLS");

            cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous start time: "
                 << formatTime(workingHour.startTime) << "\n" << leftSpace << "new start time: ";

            cin >> userInput1;

        }

        cout << "\n\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous end time: "
             << formatTime(workingHour.endTime) << "\n" << leftSpace << "new end time: ";

        cin >> userInput2;

        while(!inputInRange(range, removeChar (userInput2 , separator) )){

            system("CLS");

            cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous start time: "
                 << formatTime(workingHour.startTime) << "\n" << leftSpace << "new start time: " << userInput1;

            cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous end time: "
                 << formatTime(workingHour.endTime) << "\n" << leftSpace << "new end time: ";

            cin >> userInput2;

        }

        cout << "\n\n" << leftSpace << question3 << "\n" << leftSpace << "previous presence: "
             << formatPresence(workingHour.isPresent) << "\n\n" << leftSpace << "new presence: ";
        cin >> userInput3;

        while(!listContains(commands, userInput3)){
            system("CLS");
            cout << "\n\n" << leftSpace << question1 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous start time: "
                 << formatTime(workingHour.startTime) << "\n" << leftSpace << "new start time: " << userInput1;

            cout << "\n\n\n" << leftSpace << question2 << "\n" << leftSpace << description << "\n\n" << leftSpace << "previous end time: "
                 << formatTime(workingHour.endTime) << "\n" << leftSpace << "new end time: " << userInput2;

            cout << "\n\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question3 << "\n" << leftSpace << "previous presence: "
                 << formatPresence(workingHour.isPresent) << "\n\n" << leftSpace << "new presence: ";

            cin >> userInput3;
        }

        workingHour.startTime = stoi(removeChar (userInput1 , separator));
        workingHour.endTime = stoi(removeChar (userInput2 , separator));
        workingHour.isPresent = userInput3 == "p";

        //day = std::stoi( userInput );
        //cout << "\n\n" << leftSpace << workingHour.startTime << leftSpace << workingHour.endTime << leftSpace << to_string(workingHour.isPresent);
        SQLiteManager sqliteManager;
        if(sqliteManager.editEmployeeWorkingHour( workingHour.id, employee.id, formatDate(), workingHour.startTime, workingHour.endTime, workingHour.isPresent )){
            displayState(3);
        }
    }

    void goToStartNEndTime(){
        SQLiteManager sqliteManager = SQLiteManager();
        list<Employee> employees = sqliteManager.getAllEmployees();
        displayEmployees(employees);
    }

    void displayEmployees(list<Employee> employees){
        cout << "\n" << leftSpace << titleMarker << "List of Employees" << titleMarker << "\n\n";
        int i = 1;
        for(Employee employee: employees){
            displayEmployee(employee, i);
            i++;
        }
    }

    void displayWorkingHours(list<WorkingHour> workingHours){
        cout << "\n" << leftSpace << titleMarker << "List of working hours for " << employee.name << " on " << formatDateForDisplay() << titleMarker << "\n\n";
        int i = 1;
        for(WorkingHour workingHour: workingHours){
            displayWorkingHour(workingHour, i);
            i++;
        }
    }

    void displayState(int state){

        switch (state) {
            case 0: {
                goToHome();
                break;
            }

            case 1: {
                goToEmptyHome();
                break;
            }

            case 2: {
                goToCalendar();
                break;
            }

            case 3: {
                goToWorkingHours();
                break;
            }

            case 4: {
                goToAddWorkingHour();
                break;
            }

            case 5: {
                goToEditWorkingHour();
                break;
            }

            case 6: {
                goToEmptyWorkingHours();
                break;
            }

            case 7: {
                goToStartNEndTime();
                break;
            }
        }
    }

public:int main(int argc, char *argv[])
    {
        //SQLiteManager sqliteManager = SQLiteManager();
        //list<Employee> employees = sqliteManager.getAllEmployees();
        displayState(0);
        //displayEmployees(employees);
        /*list<WorkingHour> workingHours = sqliteManager.getEmployeeWorkingHoursByIdAndDate(1, "yesterday");
        sqliteManager.addEmployeeWorkingHour(1, "yesterday", 2000, 2100, true);
        for(WorkingHour workingHour: workingHours){
            //displayEmployee(employee);
            cout << "id : " << workingHour.id << "start time : " << workingHour.startTime << "end time : " << workingHour.endTime;
            cout << "\n";
        }*/

        // Calendar calendar = Calendar(2022);
        // calendar.setMonth(3);
        // calendar.showMonth();

        QCoreApplication a(argc, argv);
        return a.exec();
    }
};

int main(int argc, char *argv[])
{
    //SQLiteManager sqliteManager = SQLiteManager();
    //sqliteManager.addEmployee("Florian", "654896897");
    //list<Employee> employees = sqliteManager.getAllEmployees();
    //displayState(0);
    //displayEmployees(employees);
    /*list<WorkingHour> workingHours = sqliteManager.getEmployeeWorkingHoursByIdAndDate(1, "yesterday");
    sqliteManager.addEmployeeWorkingHour(1, "yesterday", 2000, 2100, true);
    for(WorkingHour workingHour: workingHours){
        //displayEmployee(employee);
        cout << "id : " << workingHour.id << "start time : " << workingHour.startTime << "end time : " << workingHour.endTime;
        cout << "\n";
    }*/

    // Calendar calendar = Calendar(2022);
    // calendar.setMonth(3);
    // calendar.showMonth();

    //QCoreApplication a(argc, argv);
    //return a.exec();
    StaffWorkingHours staffWorkingHours = StaffWorkingHours();
    staffWorkingHours.main(argc, argv);
}


