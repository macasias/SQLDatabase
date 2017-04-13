AUTHORS:
  Michael Casias
  Jarred Stein

HOW TO USE:
  This program assumes the .csv files are in a folder called Grades. 
  Run the 'make' command to compile both .cpp files. Run 'make build' to create the tables using the .csv files in the Grades directory. 
  Run the 'make test' command to run the querying program which will query the tables built from 'make build'.
  'make clean' will delete the .out files created by the makefile.

SOURCES CITED:
http://stackoverflow.com/questions/18972258/index-of-nth-occurrence-of-the-string
Remove last two insertions we just wrote by tokenizing based on ';'

http://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
To get the .csv filenames

http://www.cplusplus.com/reference/unordered_map/unordered_map/unordered_map/
To create an unordered map, from the C++ constructor documentation example
