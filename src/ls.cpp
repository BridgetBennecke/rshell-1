#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

#include <iostream>

using namespace std;


//Global Variables
    bool lsFlag = false;
    bool aFlag = false;
    bool lFlag = false;
    bool rFlag = false;
int numBlocks(dirent *direntp){
    
    int num;
    struct stat statbuf2;
    if(stat(direntp->d_name, &statbuf2) == 1){
        perror("stat");
        exit(1);
    }
    else{
        num =  statbuf2.st_blocks/2;
    }
    return num;
}


void longList(dirent *direntp){
            struct stat statbuf;
            
            if(stat(direntp->d_name, &statbuf)  == 1){
		        perror("stat");
	        }
            else{
                //Outputs Permissions
                if(S_ISDIR(statbuf.st_mode)){
                	cout << "d";
            	}
                else if(S_ISLNK(statbuf.st_mode)){
                    cout << "l";
                }
            	else{
                	cout << "-";
                }
            	if(statbuf.st_mode & S_IRUSR){
		            cout << "r";
	            }
	   	        else{
		            cout << "-i";
	            }
		        if(statbuf.st_mode & S_IWUSR){
		            cout << "w";
		        }
		        else{
		            cout << "-";
		        }
		        if(statbuf.st_mode & S_IXUSR){
		            cout << "x";
		        }
		        else{
		            cout << "-";
		        }
		        if(statbuf.st_mode & S_IRGRP){
		            cout << "r";
		        }
		        else{
		            cout << "-";
		        }
		        if(statbuf.st_mode & S_IWGRP){
		            cout << "w";
		        }
		        else{
		            cout << "-";
		        }
		        if(statbuf.st_mode & S_IXGRP){
		            cout << "x";
		        }
		        else{
		            cout << "-";
		        }   
		        if(statbuf.st_mode & S_IROTH){
		            cout << "r";
		        }
		        else{
		            cout << "-";
		        }
		        if(statbuf.st_mode & S_IWOTH){
		            cout << "w";
		        }
		        else{
		            cout << "-";
		        }
		        if(statbuf.st_mode & S_IXOTH){
		            cout << "x";
		        }
		        else{
		            cout << "-";
		        }
	            cout << " "; 
	            
                //Outputs # of hard links
                cout << statbuf.st_nlink
                     << " ";

                //Outputs owner name
                
                cout << statbuf.st_uid
                     << " ";

                //Outputs group name
                cout << statbuf.st_gid
                     << " ";

                //Outputs Size
	    	    cout << statbuf.st_size
                     << " ";

                //Outputs timestamp
                //cout << statbuf.st_mtime << " ";
                time_t t = statbuf.st_mtime;
                struct tm lt;
                localtime_r(&t, &lt);
                char timbuf[80];
                strftime(timbuf, sizeof(timbuf), "%b %e %I:%M", &lt); 
                cout << timbuf << " "; 
                
                cout << direntp->d_name;
                cout << endl;
            }
}

bool isDir(char const *temp){
    char const *dirName = temp;
    DIR *dirp = opendir(dirName);
    if(dirp == NULL){
        return false;
    }
    else{
        closedir(dirp); 
       return true;
    }
}

void executeCmd(char const *temp){
    char const *dirName = temp;    
    DIR *dirp = opendir(dirName);
    if(dirp == NULL){
	    perror("opendir");
        exit(1);
    }

///////////////////////////////
//This block of code is used to
//acquire number of BLOCKS of
//'-l' flag is set
//////////////////////////////
    dirent *direntp;
    if(lFlag){
        int total = 0;
        while((direntp = readdir(dirp))){
            if(lsFlag){
                if(lFlag){
                    if(direntp->d_name[0] != '.'){
                        total += numBlocks(direntp);
                    }
                }
            }
        }
        cout << "total " << total << endl; 
    }
//////////////////////////////   
//Re-assignment of dirp variable, so that we can go through the directory again.
//////////////////////////////
    dirp = opendir(dirName); 
    while ((direntp = readdir(dirp))){
        if(direntp == NULL){
            perror("readdir");
        }
        else{
            if(lsFlag){
                //cout << "lsFlag is set" << endl;
                if(lFlag){
                    //Condition that makes it so only public files are displayed
                    if(direntp->d_name[0] != '.'){
                        longList(direntp);
                    }
                }
                else if(aFlag){
                    cout << direntp->d_name << " ";
                }
                else{
                    if(direntp->d_name[0] != '.'){
                        cout << direntp->d_name << " ";
                    }
                }
            }
        }
    }
    cout << endl;
    
    int check = closedir(dirp);
    if(check == -1){
        perror("closedir");
    }
}


int main(int argc, char**argv)
{
    //cout << "argc: " << argc << endl;

    if(argc == 1){
        cout << "Error. Not enough arguments." << endl;
        exit(1);
    }
            
    int flags = 0;

    if(argv[1][0] == 'l'){
        if(argv[1][1] == 's'){
            lsFlag= true;
        }
        else{
            cout << "Error yo. Where's the s?!" << endl;
            exit(1);
        }
    }
    else{
        cout << "Error yo. That's not l bro!" << endl;
        exit(1);
    }

/////////////////////////////////
//Check if input is a file
/////////////////////////////////


    if(argc >= 2){
        for(int i = 2; i < argc; i++){
            if(argv[i][0] == '-'){
                //for(int j = 1; argv[i][j] != 0; j++){
                    //if(argv[i][j] == 'a'){
                    if(argv[i][1] == 'a'){
                        aFlag = true; 
                        //Handles '-al' case
                        if(argv[i][2] == 'l'){
                            lFlag = true;
                            //Handles '-alR' case
                            if(argv[i][3] == 'R'){
                                rFlag = true;
                            }
                            else if(argv[i][3] != 0){
                                cerr << "Error. Invalid flag(s)." << endl;
                                exit(1);
                            }
                        }
                        //Handles '-aR' case
                        else if(argv[i][2] == 'R'){
                            rFlag = true;
                            //Handles '-aRl' case
                            if(argv[i][3] == 'l'){
                                lFlag = true;
                            }
                            else if(argv[i][3] != 0){
                                cerr << "Error. Invalid flag(s)." << endl;
                                exit(1);
                            }
                        }
                        else if(argv[i][2] != 0){
                            cerr << "Error. Invalid flag(s)." << endl;
                            exit(1);
                        }
                        
                    }
                    else if(argv[i][1] == 'l'){
                        lFlag = true;
                        //Handles '-la' case
                        if(argv[i][2] == 'a'){
                            aFlag = true;
                            //Handles '-laR' case
                            if(argv[i][3] == 'R'){
                                rFlag = true;
                            }
                            else if(argv[i][3] != 0){
                                cerr << "Error. Invalid flag(s)." << endl;
                                exit(1);
                            }
                        }
                        //Handles '-lR' case
                        else if(argv[i][2] == 'R'){
                            rFlag = true;
                            //Handles '-lRa' case
                            if(argv[i][3] == 'a'){
                                aFlag = true;
                            }
                            else if(argv[i][3] != 0){
                                cerr << "Error. Invalid flag(s)." << endl;
                                exit(1);
                            }
                        }
                        else if(argv[i][2] != 0){
                            cerr << "Error. Invalid flag(s)." << endl;
                            exit(1);
                        }
                    }
                    else if(argv[i][1] == 'R'){
                        rFlag = true;
                        //Handles '-Ra' case
                        if(argv[i][2] == 'a'){
                            aFlag = true;
                            //Handles '-Ral' case
                            if(argv[i][3] == 'l'){
                                lFlag = true;
                            }
                            else if(argv[i][3] != 0){
                                cerr << "Error. Invalid flag(s)." << endl;
                                exit(1);
                            }
                        }
                        //Handles '-Rl' case
                        else if(argv[i][2] == 'l'){
                            lFlag = true;
                            //Handles '-Rla' case
                            if(argv[i][3] == 'a'){
                                aFlag = true;
                            }
                            else if(argv[i][3] != 0){
                                cerr << "Error. Invalid flag(s)." << endl;
                                exit(1);
                            }
                        }
                        else if(argv[i][2] != 0){
                            cerr << "Error. Invalid flag(s)." << endl;
                            exit(1);
                        }

                    }
                    else{
                        cout << "Error. Invalid flag(s)." << endl;
                        exit(1);
                    }
               //}
            }
            else{
                cout << "Should be error"<< endl;
            }
        }
    }

////////////////////////////////////////////////////////////////////
//Go through argv again and use stat function to see if regular file
////////////////////////////////////////////////////////////////////

//Compare(argv[i], direntp->d_name);

////////////////////////////////////////////////////////////////////
    const char* s = ".";
    executeCmd(s);
    return 0;
}
