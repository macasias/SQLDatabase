#include <iostream> 
#include "mydatabase.h"
#include <dirent.h> //for reading in file names in a directory

int main(){
	Database db;

        db.send_query("drop table if exists students, attends, courses, meetings;");
	
	db.send_query("CREATE TABLE students\
                (\
                  seat INT,\
                  SID BIGINT,\
                  surname TEXT,\
                  prefname TEXT,\
                  level TEXT,\
                  units TEXT,\
                  class TEXT,\
                  major TEXT,\
                  grade TEXT,\
                  status TEXT,\
                  email TEXT,\
                  PRIMARY KEY (SID),\
                  UNIQUE (email)\
                );\
                \
                CREATE TABLE courses\
                (\
                  cid INT,\
                  term INT,\
                  SUBJ TEXT,\
                  CRSE INT,\
                  SEC INT,\
                  minunits FLOAT,\
		  						maxunits FLOAT,\
                  PRIMARY KEY (cid,term,SUBJ)\
                );\
                \
                CREATE TABLE meetings\
                (\
                  instructor TEXT,\
                  type TEXT,\
                  days TEXT,\
                  time TEXT,\
                  BUILD TEXT,\
                  room TEXT,\
                  cid INT,\
                  term INT,\
                  SUBJ TEXT,\
                  PRIMARY KEY (instructor,cid,term,SUBJ),\
                  FOREIGN KEY (cid,term,SUBJ) REFERENCES courses(cid,term,SUBJ) ON UPDATE CASCADE\
                \
                );\
                \
                CREATE TABLE attends\
                (\
                  SID INT,\
                  cid INT,\
                  term INT,\
                  SUBJ TEXT,\
                  grade FLOAT,\
                  major TEXT,\
                  PRIMARY KEY (SID, cid,term,SUBJ),\
                  FOREIGN KEY (SID) REFERENCES students(SID) ON UPDATE CASCADE,\
                  FOREIGN KEY (cid,term,SUBJ) REFERENCES courses(cid,term,SUBJ) ON UPDATE CASCADE\
                );");

	
  cout << "Entering .csv files to the database. Please wait..." << endl;
  
  //getting names of files in the Grades folder
  //from http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir ("Grades/")) != NULL) {
  /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
      db.send_query(db.parse_file("Grades/" + (string) ent->d_name));
  }
  closedir (dir);
} else {
  /* could not open directory */
  return EXIT_FAILURE;
}
  cout << "All .csv files uploaded to the database." << endl;
  db.close();

}
