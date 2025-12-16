#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include<Windows.h>

using namespace std;
// ===================== 
//  ANIMATION FUNCTIONS
// =====================
void typeText(const string& text, int delay = 30)
{
    for (int i = 0; i < text.length(); i++)
    {
        cout << text[i];
        Sleep(delay);
    }
    cout << endl;
}
void loadingAnimation(const string& text)
{
    cout << text;
    for (int i = 0; i < 3; i++)
    {
        cout << ".";
        Sleep(400);
    }
    cout << endl;
}

// =====================
//    ABSTRACT CLASS 
// =====================
class User
{
protected:
    string userID;
    string name;
    string email;
    string password;
    string role;

public:
    string getUserID()
    {
        return userID;
    }

    string getRole()
    {
        return role;
    }

    bool login(const string& expectedRole)
    {
        string e, p;

        cout << "Enter Email/Enrollment: ";
        cin >> e;
        cout << "Enter Password: ";
        cin >> p;
        loadingAnimation("Logging in");

        ifstream file("users.txt");

        if (!file.is_open())
        {
            cout << "Error opening users file!" << endl;
            return false;
        }

        while (file >> userID >> name >> email >> password >> role)
        {
            if (email == e && password == p && role == expectedRole)
            {
         
                cout << "Login Successful as " << role<< "!" << endl;
                file.close();
                return true;
            }
        }

        file.close();
        cout << "Invalid credentials or role mismatch!!!" << endl;
        return false;
    }

    virtual void menu() = 0;

    virtual void logout()
    {
        loadingAnimation("Logging out please wait");
        cout << "Logged out successfully from " << role << endl;
    }
};

// ===================== 
//       STUDENT 
// =====================
class Student : public User
{
private:
    vector<string> enrolledCourses;

public:
    void menu() override
    {
        int choice;

        do
        {
            cout << endl;
            cout << "===========================================" << endl;
            typeText("            STUDENT DASHBOARD");
            cout << "===========================================" << endl;
            typeText("1 - Enroll Course");
            typeText("2 - Submit Assignment");
            typeText("3 - Delete Submission");
            typeText("4 - View Grades");
            typeText("5 - Logout");
            cout << "Choose: ";
            cin >> choice;

            if (choice == 1)
            {
                enrollCourse();
            }
            else if (choice == 2)
            {
                submitAssignment();
            }
            else if (choice == 3)
            {
                deleteSubmission();
            }
            else if (choice == 4)
            {
                viewGrades();
            }

        } while (choice != 5);

        logout();
    }

    void enrollCourse()
    {
        string courseID, cid, cname;
        bool found = false;

        cout << "Enter Course ID to enroll: ";
        cin >> courseID;

        ifstream courseFile("courses.txt");

        if (!courseFile.is_open())
        {
            cout << "Courses file not found!" << endl;
            return;
        }

        while (courseFile >> cid)
        {
            getline(courseFile, cname);

            if (cid == courseID)
            {
                found = true;
                break;
            }
        }

        courseFile.close();

        if (!found)
        {
            cout << "Invalid Course ID.... Enrollment failed." << endl;
            return;
        }

        enrolledCourses.push_back(courseID);

        ofstream enrollFile("enrollments.txt", ios::app);
        enrollFile << userID << " " << courseID << endl;
        enrollFile.close();
        loadingAnimation("Enrolling");

        cout << "Enrolled Successfully!" << endl;
    }

    void submitAssignment()
    {
        string assignmentID, fileName;

        cout << "Enter Assignment ID: ";
        cin >> assignmentID;
        cin.ignore();

        cout << "Enter File Name: ";
        getline(cin, fileName);

        ofstream file("submissions.txt", ios::app);

        if (!file.is_open())
        {
            cout << "Error opening submissions file!" << endl;
            return;
        }

        file << assignmentID << " " << userID << " " << fileName << " NA" << endl;
        file.close();
        loadingAnimation("Submitting Assignment");
        cout << "Assignment Submitted!" << endl;
    }

    void deleteSubmission()
    {
        string assignmentIDInput;
        cout << "Enter Assignment ID to delete your submission: ";
        cin >> assignmentIDInput;

        ifstream fin("submissions.txt");
        ofstream fout("temp.txt");

        string subAssignmentID, subStudentID, fileName, grade;
        bool deleted = false;

        while (fin >> subAssignmentID >> subStudentID)
        {
            fin.ignore();
            getline(fin, fileName, ' ');
            fin >> grade;

            if (subAssignmentID == assignmentIDInput && subStudentID == userID)
            {
                deleted = true;
                continue;
            }

            fout << subAssignmentID << " " << subStudentID << " " << fileName << " " << grade << endl;
        }

        fin.close();
        fout.close();

        if (!deleted)
        {
            remove("temp.txt");
            cout << "No submission found to delete!" << endl;
            return;
        }

        remove("submissions.txt");
        if (rename("temp.txt", "submissions.txt") != 0)
                    {
                        cout << "Error Deleting file!" << endl;
                        return;
                    }
        loadingAnimation("Deleting Assignment");
        cout << "Submission deleted successfully!" << endl;
    }

    void viewGrades()
    {
        ifstream file("submissions.txt");

        string aid, sid, fname, grade;

        cout << endl;
        cout << "===========================================" << endl;
        typeText("                 GRADES");
        cout << "===========================================" << endl;

        while (file >> aid >> sid)
        {
            file.ignore();
            getline(file, fname, ' ');
            file >> grade;

            if (sid == userID)
            {
                cout << "Assignment: " << aid << " Grade: " << grade << endl;
            }
        }

        file.close();
    }
};

// ===================== 
//        FACULTY
// =====================
class Faculty : public User
{
public:
    void menu() override
    {
        int choice;

        do
        {
            cout << endl;
            cout << "===========================================" << endl;
            typeText("             FACULTY DASHBOARD");
            cout << "===========================================" << endl;
            typeText("1 - Upload Assignment");
            typeText("2 - Grade Submission");
            typeText("3 - Logout");
            cout << "Choose: ";
            cin >> choice;

            if (choice == 1)
            {
                uploadAssignment();
            }
            else if (choice == 2)
            {
                gradeSubmission();
            }

        } while (choice != 3);

        logout();
    }

    void uploadAssignment()
    {
        string courseID, assignmentID, title;

        cout << "Course ID: ";
        cin >> courseID;
        cin.ignore();

        cout << "Assignment ID: ";
        cin >> assignmentID;
        cin.ignore();
        //checking if course exists or not
        ifstream coursefile("courses.txt");
        string filecid, filecname;
        bool coursefound = false;
        while (coursefile >> filecid >> filecname)
        {
            if (filecid == courseID)
            {
                coursefound = true;
                break;
            }
        }
        coursefile.close();
        if (!coursefound)
        {
            cout << "Course not found!" << endl;
            return;
        }
        cout << "Assignment Title: ";
        getline(cin, title);

        ofstream file("assignments.txt", ios::app);

        if (!file.is_open())
        {
            cout << "Error opening assignments file!" << endl;
            return;
        }

        file << courseID << " " << assignmentID << " " << title << endl;
        file.close();
        loadingAnimation("Uploading Assignment");
        cout << "Assignment Uploaded!" << endl;
    }

    void gradeSubmission()
    {
        string aid, sid, gradeInput;

        cout << "Assignment ID: ";
        cin >> aid;

        cout << "Student ID: ";
        cin >> sid;

        ifstream assignFile("assignments.txt");
        string fileCid, fileAid, fileTitle;
        bool assignmentFound = false;

        while (assignFile >> fileCid >> fileAid)
        {
            getline(assignFile, fileTitle);//this getline is used here because assignment title consist of spaces so to read the whole name...

            if (fileAid == aid)
            {
                assignmentFound = true;
                break;
            }
        }

        assignFile.close();

        if (!assignmentFound)
        {
            cout << "Invalid Assignment ID!" << endl;
            return;
        }

        ifstream fin("submissions.txt");
        ofstream fout("temp.txt");

        string subAid, subSid, fileName, fileGrade;
        bool submissionFound = false;

        while (fin >> subAid >> subSid)
        {
            fin.ignore();
            getline(fin, fileName);
            fin >> fileGrade;

            if (subAid == aid && subSid == sid)
            {
                submissionFound = true;
                cout << "Enter Grade: ";
                cin >> gradeInput;

                fout << subAid << " " << subSid << " " << fileName << " " << gradeInput << endl;
            }
            else
            {
                fout << subAid << " " << subSid << " " << fileName << " " << fileGrade << endl;
            }
        }

        fin.close();
        fout.close();

        if (!submissionFound)
        {
            remove("temp.txt");
            cout << "No submission found!" << endl;
            return;
        }

        remove("submissions.txt");
        if (rename("temp.txt", "submissions.txt") != 0)//rename function returns bool value 
                    {
                        cout << "Error updating submission file!" << endl;
                        return;
                    }
        loadingAnimation("Grading Submission");
        cout << "Graded Successfully!" << endl;
    }
};

// ===================== 
//         ADMIN 
// =====================
class Admin : public User
{
public:
    void menu() override
    {
        int choice;

        do
        {
            cout << endl;
            cout << "===========================================" << endl;
            typeText("              ADMIN DASHBOARD");
            cout << "===========================================" << endl;
            typeText("1 - Add User");
            typeText("2 - Create Course");
            typeText("3 - Logout");
            cout << "Choose: ";
            cin >> choice;

            if (choice == 1)
            {
                addUser();
            }
            else if (choice == 2)
            {
                createCourse();
            }

        } while (choice != 3);

        logout();
    }

    void addUser()
    {
        string id, name, email, pass, role;

        cout << "Enter ID: ";
        cin >> id;
        cout << "Enter Name: ";
        cin>>name;

        cout << "Enter Email: ";
        cin >> email;
        cout << "Enter Password: ";
        cin>>pass;
        cout << "Enter Role: ";
        cin >> role;

        ofstream file("users.txt", ios::app);
        file << id << " " << name << " " << email << " " << pass << " " << role << endl;
        file.close();
        loadingAnimation("Adding user");
        cout << "User Added!" << endl;
    }

    void createCourse()
    {
        string cid, cname;

        cout << "Course ID: ";
        cin >> cid;
        cin.ignore();

        cout << "Course Name: ";
        getline(cin, cname);

        ofstream file("courses.txt", ios::app);
        file << cid << " " << cname << endl;
        file.close();
        loadingAnimation("Creating course");
        cout << "Course Created!" << endl;
    }
};

// =====================
//         MAIN 
// =====================
int main()
{
    int choice;

    do
    {
        cout << "===========================================" << endl;
        typeText("        LEARNING MANAGEMENT SYSTEM         ");
        cout << "===========================================" << endl;
        typeText("1 - Login as Student");
        typeText("2 - Login as Faculty");
        typeText("3 - Login as Admin");
        typeText("4 - Exit LMS");
        cout << "Choose Role: ";
        cin >> choice;

        User* currentuser = nullptr;

        if (choice == 1)
        {
            Student* s = new Student();
            if (s->login("student"))
            {
                currentuser = s;
            }
            else
            {
                delete s;//will free the memory in case login failed
            }
        }
        else if (choice == 2)
        {
            Faculty* f = new Faculty();
            if (f->login("faculty"))
            {
                currentuser = f;
            }
            else
            {
                delete f;
            }
        }
        else if (choice == 3)
        {
            Admin* a = new Admin();
            if (a->login("admin"))
            {
                currentuser = a;
            }
            else
            {
                delete a;
            }
        }

        // if login successful open menu
        if (currentuser != nullptr)
        {
            currentuser->menu();
            delete currentuser; // free memory after logout
        }

    } while (choice != 4);

    typeText("Exiting LMS... Goodbye!");
    return 0;
}







