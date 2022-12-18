#include <iostream>
#include <list>
#include <string>
#include "sqlite/sqlite3.h"
#include <QCoreApplication>

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

    void showMonth(){

        int current = dayNumber(1 , this->currentMonth , year);
        int days = getNumberOfDays ( this->currentMonth , year);

        // Print the current month name
        printf("\n  ------------%s-------------\n",
               getMonthName ( this->currentMonth ).c_str());

        // Print the columns
        printf("  Sun  Mon  Tue  Wed  Thu  Fri  Sat\n");

        // Print appropriate spaces
        int k;
        for (k = 0; k < current; k++)
            printf("     ");

        for (int j = 1; j <= days; j++)
        {
            printf("%5d", j);

            if (++k > 6)
            {
                k = 0;
                printf("\n");
            }
        }

        if (k)
            printf("\n");

    }

    void setMonth(int newMonth){
        this->currentMonth = newMonth;
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

    void displayEmployee(Employee employee, int index){
        cout << leftSpace << index << elementSpacing << employee.name << elementSpacing << employee.phonenumber << "\n";
    }

    Employee getEmployeeFromListIndex(list<Employee> employees, int index){
            list<Employee>::iterator it = employees.begin();
            for(int i=0; i<index; i++){
                ++it;
            }
            return *it;
    }

    void goToHome(){

        string userInput = "";
        list<string> commands = {};
        int range[] = {1, 1};
        string question = "Enter an employee's number";
        string errorMessage = "Invalid Input";

        SQLiteManager sqliteManager = SQLiteManager();
        list<Employee> employees = sqliteManager.getAllEmployees();
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
        employee = getEmployeeFromListIndex(employees, std::stoi( userInput));
        displayState(1);
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
        cout << leftSpace << "No employee found\n";
    }

    void goToCalendar(){
        string userInput = "";
        int shownMonth = 0;
        list<string> commands = {"b", "n"};
        int range[] = {1, 1};
        string question = "Pick a day";
        string errorMessage = "Invalid Input";

        //SQLiteManager sqliteManager = SQLiteManager();
        //list<Employee> employees = sqliteManager.getAllEmployees();
        range[1] = getMonthSize(shownMonth);

        system("CLS");
        calendar.setMonth(shownMonth);
        calendar.showMonth();
        cout << "\n\n" << leftSpace << question << "\n" << leftSpace;
        cin >> userInput;

        while(!listContains(commands, userInput) && !inputInRange(range, userInput) ){
            if(inputInRange(range, userInput)){
                
            }
                system("CLS");
                calendar.setMonth(shownMonth);
                calendar.showMonth();
                cout << "\n\n" << leftSpace << errorMessage << "\n" << leftSpace << question << "\n" << leftSpace;
                cin >> userInput;
            
        }
        displayState(1);
    }

    void goToWorkingHours(){
        SQLiteManager sqliteManager = SQLiteManager();
        list<Employee> employees = sqliteManager.getAllEmployees();
        displayEmployees(employees);
    }

    void goToEmptyWorkingHours(){
        // SQLiteManager sqliteManager = SQLiteManager();
        // list<Employee> employees = sqliteManager.getAllEmployees();
        // displayEmployees(employees);
    }

    void goToStartNEndTime(){
        // SQLiteManager sqliteManager = SQLiteManager();
        // list<Employee> employees = sqliteManager.getAllEmployees();
        // displayEmployees(employees);
    }

    void displayEmployees(list<Employee> employees){
        cout << "\n" << leftSpace << titleMarker << "List of Employees" << titleMarker << "\n\n";
        int i = 1;
        for(Employee employee: employees){
            displayEmployee(employee, i);
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
                goToEmptyWorkingHours();
                break;
            }

            case 5: {
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
    //sqliteManager.addEmployee("Jayson", "6557578");
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


