#include <iostream>
using namespace std;

#include "Project.h"
#include "Select.h"
#include "Join.h"

void executeQuery(Operator *root) {
  int recordCount=0;
  int pageCount=0;
  vector<Record> page;

  root->open();
  do {
    pageCount++;
    page = root->next();
    cout << "-----------------Page-----------------" << endl;
    for(const auto& record : page) {
      recordCount++;
      cout << record << endl;
    }
  } while (!page.empty());
  root->close();

  cout << "[Finished]" << endl;
  cout << "    pages  received: " << pageCount << endl;
  cout << "   records received: " << recordCount << endl;
}

void runBigQuery() {
  /*
  mysql> SELECT S_FirstName, S_LastName, F_FirstName, F_LastName FROM students JOIN faculty ON S_AdvisorID=F_FacultyID JOIN departments ON S_MajorID=D_DepartmentID WHERE D_DepartmentName="Computer Science" AND S_Year="Sr" AND S_GPA>3.88;
  +-------------+------------+-------------+------------+
  | S_FirstName | S_LastName | F_FirstName | F_LastName |
  +-------------+------------+-------------+------------+
  | Edna        | Crabtree   | Alfred      | Connolly   |
  | Joanne      | Owen       | Terri       | Kessler    |
  | Martha      | Schneider  | Dale        | Hoffman    |
  | Kate        | Simon      | Christine   | Woods      |
  | Lorraine    | Peters     | Alison      | Poe        |
  +-------------+------------+-------------+------------+
  5 rows in set (0.09 sec)
  */

  // Equivalent query operator tree
  //Project op7("bigData/faculty.data", {"F_FacultyID", "F_FirstName", "F_LastName"});
  Select  op6("bigData/students.data", "S_Year","=","Sr");
  Select  op5(&op6, "S_GPA",">", "3.88");
  Select  op4("bigData/departments.data", "D_DepartmentName","=","Computer Science");
  Join    op3(&op5, &op4, "S_MajorID","D_DepartmentID");
  Join    op2(&op3,"bigData/faculty.data", "S_AdvisorID","F_FacultyID");
  Project op1(&op2, {"S_FirstName", "S_LastName", "F_FirstName", "F_LastName"});

  executeQuery(&op1);
}

void runSimpleJoinQuery() {
  /*
  mysql> SELECT * FROM instructor JOIN department ON instructor.dept_name=department.dept_name;
  +-------+------------+------------+----------+------------+----------+-----------+
  | ID    | name       | dept_name  | salary   | dept_name  | building | budget    |
  +-------+------------+------------+----------+------------+----------+-----------+
  | 76766 | Crick      | Biology    | 72000.00 | Biology    | Watson   |  90000.00 |
  | 10101 | Srinivasan | Comp. Sci. | 65000.00 | Comp. Sci. | Taylor   | 100000.00 |
  | 45565 | Katz       | Comp. Sci. | 75000.00 | Comp. Sci. | Taylor   | 100000.00 |
  | 83821 | Brandt     | Comp. Sci. | 92000.00 | Comp. Sci. | Taylor   | 100000.00 |
  | 98345 | Kim        | Elec. Eng. | 80000.00 | Elec. Eng. | Taylor   |  85000.00 |
  | 12121 | Wu         | Finance    | 90000.00 | Finance    | Painter  | 120000.00 |
  | 76543 | Singh      | Finance    | 80000.00 | Finance    | Painter  | 120000.00 |
  | 32343 | El Said    | History    | 60000.00 | History    | Painter  |  50000.00 |
  | 58583 | Califieri  | History    | 62000.00 | History    | Painter  |  50000.00 |
  | 15151 | Mozart     | Music      | 40000.00 | Music      | Packard  |  80000.00 |
  | 22222 | Einstein   | Physics    | 95000.00 | Physics    | Watson   |  70000.00 |
  | 33456 | Gold       | Physics    | 87000.00 | Physics    | Watson   |  70000.00 |
  +-------+------------+------------+----------+------------+----------+-----------+
  12 rows in set (0.00 sec)
  */   

  Join op1("smallData/instructor.data", "smallData/department.data", "I_dept_name","D_dept_name");

  executeQuery(&op1);
}

void runSimpleSelectQuery() {
  /*
  mysql> SELECT * FROM student WHERE dept_name="Comp. Sci.";
  +-------+----------+------------+----------+
  | ID    | name     | dept_name  | tot_cred |
  +-------+----------+------------+----------+
  | 00128 | Zhang    | Comp. Sci. |      102 |
  | 12345 | Shankar  | Comp. Sci. |       32 |
  | 54321 | Williams | Comp. Sci. |       54 |
  | 76543 | Brown    | Comp. Sci. |       58 |
  +-------+----------+------------+----------+
  4 rows in set (0.01 sec)
  */
  Select op1("smallData/student.data", "S_dept_name","=","Comp. Sci.");

  executeQuery(&op1);
}

void runSimpleProjectQuery() {
  /*
  mysql> SELECT dept_name, budget FROM department;
  +------------+-------------+
  | dept_name  | budget      |
  +------------+-------------+
  | Biology    |    90000.00 |
  | Comp. Sci. |   100000.00 |
  | Elec. Eng. |    85000.00 |
  | Finance    |   120000.00 |
  | History    |    50000.00 |
  | Music      |    80000.00 |
  | Physics    |    70000.00 |
  +------------+-------------+
  7 rows in set (0.00 sec)
  */
  Project op1("smallData/department.data", {"D_dept_name","D_budget"});
  //Project op1("bigData/students.data", {"S_StudentID","S_FirstName"});

  executeQuery(&op1);
}


//int Join::tempFileCount = 0;

int main() {

  runSimpleProjectQuery();

  runSimpleSelectQuery();

  runSimpleJoinQuery();

  runBigQuery();


  return 0;
}
