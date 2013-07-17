>>>>><>>>CSV2BIB<<<<<<<<<<<<<
>>>>>>>>>READ ME<<<<<<<<<<<<<


This program was developed in C.

It converts .csv files with citation entries into bibTeX files
This program only considers journal papers and conferecnce papers (in proceedings) as publications.

To use csv2bib: 
1- create a .csv file with this column order: 
Title of the paper, author's names, year of publication, journal or conference.

2- compile: gcc csv2bib.c -o csv2bib 

3- run: csv2bib <filename.csv> <filename.bib>


> Created by: Evelin Soares <
Supervisor: Sahudy Montenegro González
