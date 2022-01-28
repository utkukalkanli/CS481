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

// d is the number of characters in the input alphabet 
#define d 4
using namespace std;

int getopt_long(int argc, char* const argv[], const char* optstring, const struct option* longopts, int*longindex);
void bruteforce(string text, string pattern);
void knuth_morris_pratt(string text, string pattern);
void failureFunction(string pattern);
void robinkarp(string text, string pattern);
int code(char nuc);
void rabin_karp(char pat[], char txt[]);
int * F;
int n; // text length 
int m; // pattern length
int BruteComp;
int KMPComp;
int RKComp;
int bftime;
int kmptime;
int rktime;
enum modes{NONE, INDEX, SEARCH};
//typedef chrono::high_resolution_clock Clock;

int main(int argc, char** argv) {
    enum modes mode;
    int index;
    int o; 
    mode = NONE;
    string inputfile = "";
    string patternfile = "";

    
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
            //strcpy(filename,&optarg);
            break;
        case 'p':
            mode = SEARCH;
            patternfile = string(optarg);
            //strcpy(filename,optarg);
            break;
        case 'q':
            cout << "no query option supported " << endl; 
            //strcpy(queryfile, optarg);
            break;
        }
    }

    ifstream file_input;
    file_input.open(inputfile.c_str(), ios::in);

    string str;
    string text;
    while (getline(file_input,str)) {
        if(str.compare(">text") == 0){
            // skip
        }
        else{
            //cout << "appending " << str << endl;
            text.append(str);
        }
    }
    file_input.close();
    //cout << "complete input string " << endl << completeInput << endl;

    ifstream file_pattern;
    file_pattern.open(patternfile.c_str(), ios::in);

    string patternMid;
    string pattern;

    while(getline(file_pattern,patternMid)){
        if(patternMid.compare(">pattern") == 0){
            // skip
        }
        else{
            //cout << "appending " << patternMid << endl;
            pattern.append(patternMid);
        }
    }
    file_pattern.close();
    //cout << "complete pattern string " << pattern << endl;

    n = text.length();
    m = pattern.length();
    // auto start_time = Clock::now();
    // bruteforce(text,pattern);
    // auto end_time = Clock::now();
    // std::cout << "Time difference:" << std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() << " microseconds" << std::endl;

    std::chrono::time_point< std::chrono::system_clock > startTimeBrute;
    std::chrono::duration< double, std::micro > elapsedTimeBrute;
	startTimeBrute = std::chrono::system_clock::now();    
    bruteforce(text,pattern);
    elapsedTimeBrute = std::chrono::system_clock::now() - startTimeBrute;
    cout << "Bruteforce runtime was " << elapsedTimeBrute.count() << endl;
    bftime = elapsedTimeBrute.count();

    std::chrono::time_point< std::chrono::system_clock > startTimeKMP;
    std::chrono::duration< double, std::micro > elapsedTimeKMP;
	startTimeKMP = std::chrono::system_clock::now();
    knuth_morris_pratt(text, pattern);
    elapsedTimeKMP = std::chrono::system_clock::now() - startTimeKMP;
    cout << "Knuth-Morris-Prath runtime was " << elapsedTimeKMP.count() << endl;
    kmptime = elapsedTimeKMP.count();

    char text_array[n + 1];
    char pattern_array[m + 1]; 
    // copying the contents of the 
    // string to char array 
    strcpy(text_array, text.c_str()); 
    strcpy(pattern_array, pattern.c_str());
    
    std::chrono::time_point< std::chrono::system_clock > startTimeRK;
    std::chrono::duration< double, std::micro > elapsedTimeRK;
	startTimeRK = std::chrono::system_clock::now();
    rabin_karp(pattern_array, text_array);
    elapsedTimeRK = std::chrono::system_clock::now() - startTimeRK;
    cout << "Rabin-Karp runtime was " << elapsedTimeRK.count() << endl;
    rktime = elapsedTimeRK.count();

    int best = min(bftime, kmptime);
    best = min(best, rktime);

    if(best == bftime){
        cout << "Best algorithm based on time is Brute Force with time score " << best << endl;
    }
    else if(best == kmptime){
        cout << "Best algorithm based on time is Knuth Morris Prath with time score " << best << endl;
    }
    else if(best == rktime){
        cout << "Best algorithm based on time is Rabin Karp with time score " << best << endl;
    }else{
        cout << "best algorithm scores are equivalent " << endl ;
    }    
    return 0;
}

void bruteforce(string text, string pattern){
    cout << "BRUTEFORCE" << endl;
    BruteComp = 0;
    int i = 1;
    while(i < n){
        string check = text.substr(i, m);
        BruteComp = BruteComp + m;
        if(pattern.compare(check) == 0){
            //i++; // ????
            cout << "match found at position " << i + 1 << endl; // i is the index position, for position we use i + 1
            // cout << "performed " << i * m << " character comparisons" << endl;
            break;
        }
        else{
            i++;
        }
    }
    cout << "performed " << BruteComp << " character comparisons" << endl;
    // cout << "bruteforce comp " << BruteComp << endl;
}


void knuth_morris_pratt(string text, string pattern){
    cout << "KNUTH-MORRIS-PRATH" << endl;
    KMPComp = 0;
    F = new int[pattern.length()];
    for(int i = 0; i < pattern.length(); i++){
        F[i] = 0;
    }
    failureFunction(pattern);
    // for(int i = 0; i < pattern.length(); i++){
    //     cout << F[i] << " ";
    // }
    // cout << "" << endl;

    int i = 0;
    int j = 0;
    bool flag = false;
    while(i < n){
        KMPComp++;
        if(text[i] == pattern[j]){
            KMPComp++;
            if(j == m - 1){
                // return i - j; // match position
                cout << "match found at position " << i - j + 1 << endl; // i - j gives index, i - j + 1 gives position
                flag = true;
                break;
            }
            else{
                i++;
                j++;
            }
        }
        else{
            if(j > 0){
                j = F[j - 1];
            }
            else{
                i++;
                j = 0;
            }
        }
    }
    if(!flag){
        cout << "no match found " << endl;
    }
    cout << "performed " << KMPComp << " character comparison " << endl;
}

void failureFunction(string pattern){
    int i = 1;
    int j = 0;
    while(i < m){
        KMPComp++;
        if(pattern[i] == pattern[j]){
            F[i] = j + 1;
            i++;
            j++;
        }
        else if(j > 0){
            j = F[j - 1];
        }
        else{
            F[i] = 0;
            i++;
        }
    }
}

void robinkarp(string text, string pattern){
    cout << "robin-karp" << endl;
    cout << "m is " << m << endl;
    int prime = 101;
    int c = 1; 
    bool found = false;
    // The value of h would be "pow(d, M-1)%q" 
    for (int i = 0; i < m - 1; i++){
        c = (c * d) % prime; 
    }    
    // A:0, C:1, G:2, T:3
    // Q = 997
    cout << "q is: " << prime << endl;
    cout << "c is 1: " << c << endl;
    cout << "c is 2: " <<  c << endl;

    int fp = 0;
    int ft = 0;
    // for(int i = 0; i < m; i++){
    //     fp += fmod( (pow(4,i) * code(pattern[m - i - 1])), prime);
    //     ft += fmod( (pow(4,i) * code(text[m - i - 1])), prime);
    // }
    for(int i = 0; i < m - 1; i++){
        fp = fmod( 4 * fp + code(pattern[i]), prime);
        ft = fmod( 4 * ft + code(text[i]), prime);
    }
    cout << "fp is " << fp << endl;
    cout << "ft is " << ft << endl;
    cout << "---------" << endl;

    int position;
    for(int s = 0; s < n - m; s++){
        if(fp == ft){
            cout << "pattern occurs with shift " << s << endl;
            position = s + 1;
            found = true;
            break;
        }
        ft = fmod(((ft - (code(text[s]) * c)) * 4) + code(text[s + m]), prime);
        cout << "fp is " << fp << endl;
        cout << "ft is " << ft << endl;
    }
    if(found){
        cout << "exact match found at position " << position << endl;
    }
    else{
        cout << "no exact macth found" << endl;
    }

}


void rabin_karp(char pat[], char txt[]){ 
    cout << "RABIN-KARP" << endl;
    int q = 101;
    int j; 
    int fp = 0; // hash value for pattern 
    int ft = 0; // hash value for txt 
    int h = 1; 
    bool found = false;
    // The value of h would be "pow(d, M-1)%q" 
    for (int i = 0; i < m - 1; i++){
        h = (h * d) % q; 
    }
        
    // Hashed version of patterns
    for (int i = 0; i < m; i++){ 
        fp = (d * fp + pat[i]) % q; 
        ft = (d * ft + txt[i]) % q; 
    }
 
    // Slide the pattern over text one by one 
    for (int i = 0; i <= n - m; i++) { 
        // Check the hash values of current window of text and pattern. If the hash values match then only check the characters on by one 
        if ( fp == ft ) { 
            /* Check for characters one by one */
            for (j = 0; j < m; j++) { 
                RKComp++;
                if (txt[i+j] != pat[j]) 
                    break; 
            }
            // if p == t and pat[0...M-1] = txt[i, i+1, ...i+M-1] 
            if (j == m) {
                cout << "Match found at position "<< i + 1 << endl; 
                found = true;
            }
                
    }
        // Calculate hash value for next window of text by remove leadig digit, add trailing digit
        if ( i < n - m ) { 
            ft = ( d * (ft - txt[i] * h ) + txt[i + m]) % q; 
    
            // negative value of t disturbes the result convert it to positive
            if (ft < 0){
                ft = (ft + q); 
            } 
        } 
    } 
    if(!found){
        cout << "no match found" << endl;
    }
    cout << "performed " << RKComp << " character comparison " << endl;
} 

int code(char nuc){
    if(nuc == 'A'){
        return 0;
    }
    else if(nuc == 'C'){
        return 1;
    }
    else if(nuc == 'G'){
        return 2;
    }
    else if(nuc == 'T'){
        return 3;
    }
    else{
        return -1;
    }
}


    // PRIME GENERATOR
    // for (int i = m; i < 100; i++) 
    // {
    //     bool primetrue = true;
    //     for (int j = 2; j * j <= i; j++)
    //     {
    //         if (i % j == 0) 
    //         {
    //             primetrue =false;
    //             break;    
    //         }
    //     }   
    //     if(primetrue){
    //         prime = (long double)i;
    //         break;
    //     }
    // }
    // cout << "our prime " << prime << endl;