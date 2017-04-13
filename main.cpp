#include <iostream> 
#include "mydatabase.h"
#include <dirent.h> //for reading in file names in a directory

int main(){
  Database db;
  
  cout << "RESULTS FOR PROBLEM 3(a) "<< endl;
  for(int i = 1; i <= 20; i++){
    cout << "units: " << i << " percent of students: " ;
		float problem_3a_results = (float) db.problem_3a_send("SELECT COUNT(SUNITS_TOTAL) AS TOTAL \
FROM (SELECT SID, SUM(maxunits) AS SUNITS_TOTAL \
  FROM COURSES NATURAL JOIN attends \
  WHERE (SUBJ = 'ABC' OR SUBJ = 'DEF') \
  GROUP BY SID) AS \"TABLE\"\
WHERE SUNITS_TOTAL = '" + to_string(i) + "';") / db.problem_3a_send("select count(*) from students;");
   cout << 100*problem_3a_results << endl;
  }

  cout << "RESULTS FOR PROBLEM 3(b)" << endl;
  

  for(int i = 1; i <=20; i++){
		cout << "units: " << i << " average GPA: " ;
		cout << db.problem_3b_send("SELECT SID,term,avg(grade), case when avg(grade) is null then 'empty' else 'full' end ,SUM(maxunits) FROM COURSES NATURAL JOIN attends GROUP BY SID,term;",i);
   cout << endl;
  }

  cout << "RESULTS FOR PROBLEM 3(c)" << endl;
  db.problem_3c_send_max("select * from (select instructor,avg(grade) as avgr from attends natural join meetings group by instructor) as t3 order by avgr desc nulls last;");
  db.problem_3c_send_min("select * from (select instructor,avg(grade) as avgr from attends natural join meetings group by instructor) as t3 order by avgr asc nulls last;");

	cout <<"RESULTS FOR PROBLEM 3(d)" << endl;
	db.problem_3d_send_max("select * from (select instructor, courses.subj, avg(grade) as avgr \
from attends NATURAL JOIN meetings NATURAL JOIN courses \
where courses.crse >= 100 AND courses.crse < 200 AND courses.subj = 'ABC' \
group by instructor, courses.subj) as t3 \
order by avgr desc nulls last;");

  db.problem_3d_send_min("select * from (select instructor, courses.subj, avg(grade) as avgr \
from attends NATURAL JOIN meetings NATURAL JOIN courses \
where courses.crse >= 100 AND courses.crse < 200 AND courses.subj = 'ABC' \
group by instructor, courses.subj) as t3 \
order by avgr asc nulls last;");

        cout << "RESULTS FOR PROBLEM 3(e)" << endl;

	db.problem_3e_send("with t1 as (select term, cid, instructor, build, time, room, days, crse, subj from courses natural join meetings where build is not null and time is not null), t2 as(select term, cid, instructor, build, time, room, days, crse, subj from courses natural join meetings where build is not null and time is not null) select * from t1 inner join t2 on t1.time = t2.time and t1.build = t2.build and t1.room = t2.room and t1.instructor != t2.instructor and t1.term = t2.term order by t1.subj,t1.crse;");
	
	db.send_query("alter table students drop column grade;");

  cout << "RESULTS FOR PROBLEM 3(f)" << endl;

  db.problem_3f_send_max("select * from (select major,count(major) as numstudents,subj,avg(grade) as avgr from attends natural join students where subj='ABC' group by major,subj) as t3 order by avgr desc nulls last;");

  db.problem_3f_send_min("select * from (select major,count(major) as numstudents,subj,avg(grade) as avgr from attends natural join students where subj='ABC' group by major,subj) as t3 order by avgr asc nulls last;");
  
  cout << "Results for problem 3(g) " << endl;
  cout << "Percent of students who transferred in to ABC major: " << db.problem_3g_send_abc_transfer("select count(*) from (with t1 as (select * from attends), t2 as (select * from attends) select distinct t1.sid from t1 inner join t2 on t1.sid = t2.sid and t1.term < t2.term and t1.major != t2.major and t2.major like 'ABC%' and t1.major not like 'ABC%') as answer;")/db.num_students() << endl;
  
  db.problem_3g_find_top_majors("with ans as(with t1 as (select * from attends), t2 as (select * from attends) select distinct t1.sid,t1.major as prevmajor,t2.major from t1 inner join t2 on t1.sid = t2.sid and t1.term < t2.term and t1.major != t2.major and t2.major like 'ABC%' and t1.major not like 'ABC%' order by prevmajor asc) select ans.prevmajor,count(ans.prevmajor) as counting from ans group by ans.prevmajor order by counting desc limit 5  ;");
  db.close();  
  return 0;
}
