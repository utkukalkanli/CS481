
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector> 


using namespace std;
const int MAX_INT = 1410065408;
void medianStringSearch(int t, int k, int n);
void generetaNucsRec(char set[], string prefix, int k);
void generateNucs(int k);
int hamming(string a, string b);
int totaldistance(string v, vector<string> dna);

char nucs[] = {'a', 'c', 'g', 't'}; 
vector<string> dna;
vector<string> allWords;



int main(int argc, char** argv) {
    //cout << argv[1] << endl;
    ifstream file(argv[1]);
    string str;
    int lineCount = 1;
    int k = -1000;
    int t = 0; // number of seqeuence
    int n = 0; // length of sequence
    while(getline(file,str)){
        if(lineCount > 100 || k > 16 || n > 500){ // constraints
            cout << "line count must be less than 100, k-mer must be less than 16, length of sequence must be less than 500" << endl;
            cout << "illegal input, exiting the program" << endl;
            exit(0);
        } 
        else if(lineCount == 1){ // if its first line
            k = stoi(str);
            lineCount++;
        }
        else{
            // its not first line and not inside constraints.
            n = str.length();
            lineCount++;
            dna.insert(dna.end(), str);
        }
    }
    t = lineCount - 2; // t is the number of sequences.
    cout << "t(num of seq) is " << t << ", k(k-mer) is " << k << ", n(length of seq) is " << n << endl;

    medianStringSearch(t,k,n);
	return 0;
}


void medianStringSearch(int t, int k, int n){
    cout << "median string search" << endl;

    int bestDistance = MAX_INT;
    generateNucs(k);
    string bestword = allWords.at(0);

    for(int i = 0; i < allWords.size(); i++){
        int currentDistance = totaldistance(allWords.at(i), dna);
        if(currentDistance < bestDistance){
            bestDistance = currentDistance;
            bestword = allWords.at(i);
        }
    }
    cout << "bestword is " << bestword << endl;
    // Create and open a text file
    ofstream resultfile("result.txt");
    // Write to the file
    resultfile << bestword;
    // Close the file
    resultfile.close();

}

void generetaNucsRec(char set[], string prefix, int k) 
{ 
    // Base case: k is 0, 
    // print prefix 
    if (k == 0) 
    { 
        transform(prefix.begin(), prefix.end(), prefix.begin(), ::toupper); 
        allWords.insert(allWords.end(),prefix);
        return; 
    } 
    // One by one add all characters  
    // from set and recursively  
    // call for k equals to k-1 
    for (int i = 0; i < 4; i++) 
    { 
        string newPrefix; 
          
        // Next character of input added 
        newPrefix = prefix + set[i]; 
          
        // k is decreased, because  
        // we have added a new character 
        generetaNucsRec(set, newPrefix, k - 1); 
    } 
} 
  
void generateNucs(int k) 
{ 
    generetaNucsRec(nucs, "", k); 
}


int totaldistance(string v, vector<string> dna){
    int totaldistance = 0;
    for(int i = 0; i < dna.size(); i++){
        //cout << dna.at(i) << endl;
        string sequence = dna.at(i);
        int min_distance = MAX_INT;
        for(int j = 0; j < sequence.length(); j++){
            //cout << v << endl;
            //cout << sequence.substr(j, v.length()) << endl;
            int distance = hamming(v, sequence.substr(j, v.length()));
            //cout << "distance is " << distance << endl;
            if(distance < min_distance){
                min_distance = distance;
            }
        }
        totaldistance = totaldistance + min_distance;
    }
    return totaldistance;
}


int hamming(string a, string b){

    if(a.length() != b.length()){
        return MAX_INT;
    }
    else{
        int distance = 0;
        for(int i = 0; i < a.length(); i++){
            if(a[i] != b[i]){
                distance++;
            }
        }
        return distance;
    }
}
