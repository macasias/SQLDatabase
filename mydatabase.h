#ifndef MYDATABASE_H
#define MYDATABASE_H

#include <iostream>
#include <pqxx/pqxx> 
#include <stdlib.h>
#include <fstream> //filestream for opening .csv file
#include <string> //for getline
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string.hpp>
#include <algorithm> //for counting characters in a string
#include <unordered_set> // hash table for problem 3e courses

using namespace std;
using namespace pqxx;


class Database{
 public:
  Database();
  string meetings_foreign_key;
  string prev_instructor = "NULL";
  string SID_for_attends;
  string letter_grade;
  string major;
  connection* conn;
  void start_conn(){
    string user = getenv("USER");
    string host = "/home/" + user + "/postgres";
    string conn_param = "dbname=postgres user=" + user + " host=" + host + " port=5432";
    conn = new connection(conn_param);  
    try{
      connection conn(conn_param.c_str());
      if(conn.is_open()) cout << conn.dbname() << " has opened successfully!" << endl;
      else cout << conn.dbname() << " has failed to open." << endl;
    }
    catch(const exception &e){
      cout << e.what() << endl;
    }
  } //end start_conn

  string parse_course(string str){
    vector<string> results;
  	
    boost::replace_all(str,"'","''");
    boost::replace_all(str,"\"","'");
    boost::replace_all(str,"''","NULL");
    boost::replace_all(str,"-","','");
    //cout << str << endl;
    
    boost::algorithm::split(results, str, boost::is_any_of(","));
    boost::replace_all(str,"\n","");
    
    
    string line = "insert into courses(CID, term, subj, crse, sec, minunits, maxunits) values(";
    line+=str;
    if((int)results.size() == 6){
     line+=",";
     line+=results[5];
    }
    boost::iterator_range<string::iterator> r = boost::find_nth(str, ",", 2); 
    str.erase(r.begin(), str.end());
    meetings_foreign_key = str;

    line+=")ON CONFLICT DO NOTHING;";
    // cout << line << endl;
    return line;
  }

  string parse_meetings(string str){
    //cout << str << endl;
    boost::replace_all(str,"\"\"","NULL");
    boost::replace_all(str,"'","''");
    boost::replace_all(str,"\"","'");
    //cout << str << endl;

    std::vector<std::string> results;

    boost::algorithm::split(results, str, boost::is_any_of(","));

    //cout << "The current value of results is: " << results[0] << endl;
 
    if(results[0].find("NULL") != string::npos){
      //cout << "The prev instructor is: " << prev_instructor << endl;
//      boost::replace_all(results[0],"NULL",prev_instructor);

      results[0].assign(prev_instructor);      

      //cout << "It was set to: " << results[0] << endl;
//      if(str.find("Diaz") != string::npos)
//        cout << "Found the bitch" << endl << str << endl;
    }
    else{
      prev_instructor.erase();
      prev_instructor = results[0] + "," + results[1];
//      cout << "AND NOW THE PREV INSTR IS: " << prev_instructor << endl;
    }
    string line = "insert into meetings(instructor, type, days, time, build, room,cid,term,SUBJ) values(";

    //need to check if instructor is NULL - if so, use instructor from previous line by making global variable

    for(int i = 0; i <(int)results.size();i++){
      line+=results[i];
      if(i+1 !=(int)results.size()) line+=",";
    }
    line+= ",";
    line+= meetings_foreign_key ;
    line+=")ON CONFLICT DO NOTHING;";
   // cout << line << endl;
    return line;
  }

  string parse_students(string str){
    boost::replace_all(str,"'","''");
    boost::replace_all(str,"\"","'");
    boost::replace_all(str,"''","NULL");
    string line = "insert into students(seat, SID, surname, prefname, level, units, class, major, grade, status, email) values(";
    
    line+=str;
    line+=") ON CONFLICT DO NOTHING;";
    
    std::vector<std::string> results;
    boost::algorithm::split(results, str, boost::is_any_of(","));
    major = results[7];
    //cout << "test: " << results[8] << endl;
    if( boost::iequals(results[8],"'A+'") != 0  || boost::iequals(results[8],"'A'") != 0 )
    	letter_grade = "4.0";
    else if( boost::iequals(results[8],"'A-'") != 0 )
    	letter_grade = "3.7";
    else if( boost::iequals(results[8],"'B+'") != 0 )
    	letter_grade = "3.3";
    else if( boost::iequals(results[8],"'B'") != 0 )
    	letter_grade = "3.0";
    else if( boost::iequals(results[8],"'B-'") != 0 )
    	letter_grade = "2.7";
    else if( boost::iequals(results[8],"'C+'") != 0 )
    	letter_grade="2.3";
    else if( boost::iequals(results[8],"'C'") != 0 )
    	letter_grade="2.0";
    else if( boost::iequals(results[8],"'C-'") != 0 )
    	letter_grade="1.7";
    else if( boost::iequals(results[8],"'D+'") != 0 )
    	letter_grade="1.3";
    else if( boost::iequals(results[8],"'D'") != 0 )
    	letter_grade="1.0";
    else if( boost::iequals(results[8],"'D-'") != 0 )
    	letter_grade="0.7";
    else if( boost::iequals(results[8],"'F'") != 0 )
    	letter_grade="0.0";
    else letter_grade="NULL";
	  
//	  if(strcmp(letter_grade.c_str(),"NULL") != 0) cout << letter_grade << endl;
    boost::iterator_range<string::iterator> r = boost::find_nth(str, ",", 0);
    boost::iterator_range<string::iterator> s = boost::find_nth(str, ",", 1);

    str.erase(s.begin(),str.end());
    str.erase(str.begin(),r.begin()+1);
    
    SID_for_attends = str;

    //cout << str << endl;
    line += parse_attends();

    return line;
  }

  string parse_attends(){
    string line = "insert into attends(SID, cid, term, SUBJ,grade,major) values(";

    line += SID_for_attends;

    line += ",";
    line += meetings_foreign_key;
    line += ",";
    line += letter_grade;
    line += ",";
    line += major;
    line += ") ON CONFLICT DO NOTHING;";

    return line;
  }

  string parse_file(string csv_file){
    string blank = "\"\"";
    string table_query;
    ifstream file;
    file.open(csv_file); 
    string line;
    while(getline(file,line,'\r')){
      if(line.find(',') == string::npos){
        continue;
      }
      else{ //not a blank, parse line to determine relation

        //if next section is Courses
        if(line.find("CID") != string::npos){
           getline(file,line,'\r');
           if(line.find(',') == string::npos) continue; //no data, so skip
           else table_query+=parse_course(line);

        }//end if find CID

        //if next section is Meetings
        else if(line.find("INSTRUCTOR(S)") != string::npos){
          while(getline(file,line,'\r')){
            if(line.find(',') == string::npos) break; //no data, so skip
            else table_query+=parse_meetings(line);
          }
        }//end else if find instructor

        //if next section is Students
        else if(line.find("SEAT") != string::npos){
          int student_count = 0;
          while(getline(file,line,'\r')){
            //if there is a blank line, end of list of students
            if(line.find(',') == string::npos){
              //if there were no students in the class
              if(student_count == 0){
                //http://stackoverflow.com/questions/18972258/index-of-nth-occurrence-of-the-string
                //remove last two insertions we just wrote by tokenizing based on ';'
                boost::iterator_range<string::iterator> r = boost::find_nth(table_query, ";", -1);
                boost::iterator_range<string::iterator> s = boost::find_nth(table_query, ";", -3);
                table_query.erase(s.begin(), r.begin());
              }
              student_count = 0; 
              break;
            } //no data, so skip
            else{ 
              table_query+=parse_students(line);
              student_count += 1;
            }
          }
        }//end else if find seat
        
      }
        
    }
    
    file.close();
    return table_query;
  } //end build_table

  void send_query(string query){
    try{
      work W(*conn);
      result R(W.exec(query));
      W.commit();
      
    }
    catch(const exception& e){
      cout << e.what() << endl;
    }

  }// end send_query
  
  int problem_3a_send(string query){
  	nontransaction N(*conn);
  	result R(N.exec(query));
  	result::const_iterator c = R.begin();
  	return c[0].as<int>();
  	
  }

  float problem_3b_send(string query,int units){
		nontransaction N(*conn);
  	result R(N.exec(query));
  	int counter = 0;
  	float sum = 0.0;
		for(result::const_iterator c = R.begin(); c != R.end(); ++c){
			if(boost::iequals(c[3].as<string>(),"empty")){
				continue;
			}
			else if(c[4].as<int>() == units){
				 sum+=c[2].as<float>();
				 counter++;
			}
		}
		return sum/counter;
  }

  void problem_3c_send_max(string query){
    nontransaction N(*conn);
    result R(N.exec(query));
    float max = 0.0;
    cout << "Professors with the highest average grades" << endl;

    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      if(c[1].as<float>() >= max){
        cout << "instructor: " << c[0].as<string>() << " average grade: " << c[1].as<float>() << endl;
        max = c[1].as<float>();
      }
      else break;
    }
    
  }
  void problem_3c_send_min(string query){
    nontransaction N(*conn);
    result R(N.exec(query));
    float min = 4.0;
    cout << "Professors with the lowest average grades" << endl;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      if(c[1].as<float>() <= min){
        cout << "instructor: " << c[0].as<string>() << " average grade: " << c[1].as<float>() << endl;
        min = c[1].as<float>();
      }
      else break;
    }
  }
  
  void problem_3d_send_max(string query){
  	nontransaction N(*conn);
  	result R(N.exec(query));
  	cout << "Easiest professors in the ABC 100 level courses" << endl;
  	float max =0.0;
  	for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      if(c[2].as<float>() >= max){
        cout << "instructor: " << c[0].as<string>() << " average grade: " << c[2].as<float>() << endl;
        max = c[2].as<float>();
      }
      else break;
    } 
  
  }
  
  void problem_3d_send_min(string query){
    nontransaction N(*conn);
    result R(N.exec(query));
    float min = 4.0;
    cout << "Hardest professors in the ABC 100 level courses" << endl;
    for(result::const_iterator c = R.begin(); c != R.end(); ++c){
      if(c[2].as<float>() <= min){
        cout << "instructor: " << c[0].as<string>() << " average grade: " << c[2].as<float>() << endl;
        min = c[2].as<float>();
      }
      else break;
    }
  }
  
  void problem_3e_send(string query){
		std::unordered_set<std::string> hashtable;
    
  	nontransaction N(*conn);
  	result R(N.exec(query));
  	cout << "Courses with schedule conflicts" << endl;
  	for(result::const_iterator c = R.begin(); c != R.end(); ++c){
  		
			if( c[15].as<string>().find_first_of(c[6].as<string>()) != string::npos ){
				std::unordered_set<std::string>::const_iterator found_it = hashtable.find(c[0].as<string>() + c[17].as<string>() + c[16].as<string>());

				//if the key wasn't found in the hashtable
				if(found_it == hashtable.end()){
					//insert it into the table in a way that it'll catch duplicates
					hashtable.insert(c[0].as<string>() + c[8].as<string>() + c[7].as<string>());
					//and print out the result
					cout << "term: " << c[0].as<string>() << " courses: " << c[8].as<string>() << " " << c[7].as<int>() << ", " << c[17].as<string>() << " " << c[16].as<int>() << endl;
				}
			}
		}
       
  }
  
  void problem_3f_send_max(string query){
 		nontransaction N(*conn);
  	result R(N.exec(query));
  	float max =0.0;
  	cout << "Majors with the best average GPA for ABC courses: " << endl;
  	for(result::const_iterator c = R.begin(); c != R.end(); ++c){
  	  if(c[3].as<float>() >= max){
  		  cout<< "Major: "<< c[0].as<string>() << " average GPA: " << c[3].as<float>()<< endl;
  			max = c[3].as<float>();
  		}
  		else break;
  	}
  }
  
  void problem_3f_send_min(string query){
	  nontransaction N(*conn);
	  result R(N.exec(query));
	  float min =4.0;
	  cout << "Majors with the worst average GPA for ABC courses: " << endl;
	  for(result::const_iterator c = R.begin(); c != R.end(); ++c){
	    if(c[3].as<float>() <= min){
		    cout<< "Major: "<< c[0].as<string>() << " average GPA: " <<c [3].as<float>() << endl;
			  min = c[3].as<float>();
		  }
		  else break;
	  }
  }
  
  
  int num_students(){
  	nontransaction N(*conn);
  	int n;
	  result R(N.exec("select count(*) from students where major like 'ABC%';"));
	  for(result::const_iterator c = R.begin(); c != R.end(); ++c){
			n = c[0].as<int>();
		}
	  	return n;

  }
  
  
  float problem_3g_send_abc_transfer(string query){
 		nontransaction N(*conn);
	  result R(N.exec(query));
	  float result = 0.0;
	  for(result::const_iterator c = R.begin(); c != R.end(); ++c){
			result = c[0].as<float>();
		}
		return 100*result;
	  
  }
  
  void problem_3g_find_top_majors(string query){
  	nontransaction N(*conn);
  	result R(N.exec(query));
  	vector<string> top_majors;
  	vector<int> num_top;
  	cout <<"Top 5 majors that transferred in to ABC major: " << endl;
		for(result::const_iterator c = R.begin(); c != R.end(); ++c){
			top_majors.push_back(c[0].as<string>());
			num_top.push_back(c[1].as<int>());
			cout << "Major: " << c[0].as<string>() << " number of students: " << c[1].as<int>() << endl;
		}
		
		for(int i = 0; i < (int) top_majors.size(); i++){

		  result R(N.exec("select count(*) from students where major = '" + top_majors[i] +"';"));

			for(result::const_iterator c = R.begin(); c != R.end(); ++c){

				cout << "Percent of " << top_majors[i] << " major: " << (float)100 * num_top[i]/c[0].as<int>() << endl;
			}
			
		}
  	
  }
  
  void close(){
    cout << "Closing connection...";
    conn->disconnect();
    cout << "closed!" << endl;
  } //end close
}; //end class declaration

Database::Database(){
  start_conn();
}

#endif
