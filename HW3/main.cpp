#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector> 
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <math.h>
#include <chrono>
#include <cmath>
#include <cstring>
#include <algorithm>
#include "trie.h"
#include <vector>

using namespace std;
#define MAX_NUM_OF_INPUT 300

int getopt_long(int argc, char* const argv[], const char* optstring, const struct option* longopts, int*longindex);
enum modes{NONE, INDEX, SEARCH};

int main(int argc, char** argv) {  
    cout << "homework 3 " << endl;
    // Input keys (use only 'a' through 'z' and lower case) 
    vector<string> patterns;
    vector<string> text;
    enum modes mode;
    int index;
    int o; 
    mode = NONE;
    string inputfile = "";

    static struct option long_options[] = {
            {"input", required_argument, 0, 'i'},
            {"pattern", required_argument, 0, 'p'},
            {"query", required_argument, 0, 'q'},
            {0,0,0,0}
    };

    while((o = getopt_long(argc, argv, "i:p:q", long_options, &index)) != -1){
        switch (o)
        {
        case 'i':
            inputfile = string(optarg);
            break;
        case 'p':
            mode = SEARCH;
            break;
        case 'q':
            cout << "no query option supported " << endl; 
            break;
        }
    }

    ifstream file_input;
    file_input.open(inputfile.c_str(), ios::in);
    int linecount = 0;
    string str;
    string patternmid;
    string textmid;
    char text_arr[MAX_NUM_OF_INPUT] ;
    char pattern_arr[MAX_NUM_OF_INPUT];

    while (getline(file_input,str)) {
        if(linecount == 0){
            patternmid.append(str);
        }
        else if(linecount == 1){
            textmid.append(str);
        }
        linecount++;
    }
    file_input.close();
    cout << "complete pattern: " << patternmid << endl;
    cout << "complete text: " << textmid << endl;

    strcpy(text_arr, textmid.c_str()); 
    strcpy(pattern_arr, patternmid.c_str());
    char * tokenizer;
    tokenizer = strtok(pattern_arr, " ");
    while(tokenizer != NULL){
        patterns.push_back(tokenizer);
        tokenizer = strtok (NULL, " ");
    }

    tokenizer = strtok(text_arr, " ");
    while(tokenizer != NULL){
        text.push_back(tokenizer);
        tokenizer = strtok (NULL, " ");
    }
    struct TrieNode *root = getNode(); 
  
    // // Construct trie 
    for (int i = 0; i < text.size(); i++) {
        insert(root, text[i]);
    } 
    cout << "depth of tree " << depthOfTree(root) << endl;
    cout << "Build tree" << endl;
    cout << "-----------------------------------------------------------" << endl;
    BFS(root);
    cout << "Search" << endl;
    cout << "-----------------------------------------------------------" << endl;
    AhoCorasick(root,patterns,textmid);
    return 0;
}
