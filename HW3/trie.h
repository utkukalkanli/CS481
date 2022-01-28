#include <string> 
#include <iostream>
#include <queue>

using namespace std; 
  
const int ALPHABET_SIZE = 26; 
  
// trie node 
struct TrieNode 
{ 
    struct TrieNode *children[ALPHABET_SIZE]; 
    struct TrieNode *parent;
    struct TrieNode *failurelink;
    char val;
    int depth;
    // isEndOfWord is true if the node represents 
    // end of a word 
    bool isEndOfWord;
    int id; 
}; 
  
// Returns new trie node (initialized to NULLs) 
// ROOT CONSTRUCTOR
struct TrieNode *getNode(void) 
{ 
    struct TrieNode *pNode =  new TrieNode; 
    pNode->isEndOfWord = false; 
    pNode->val = ' ';
    pNode->parent = pNode; // IT IS THE ROOT
    pNode->failurelink = pNode; // IT IS THE ROOT
    pNode->depth = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        pNode->children[i] = NULL; 
  
    return pNode; 
} 
// Returns new trie node (initialized to NULLs) 
struct TrieNode *getNode(struct TrieNode * parentNode) 
{ 
    struct TrieNode *pNode =  new TrieNode; 
    
    pNode->isEndOfWord = false; 
    pNode->val = ' ';
    pNode->parent = parentNode;
    pNode->depth = 0;
    for (int i = 0; i < ALPHABET_SIZE; i++) 
        pNode->children[i] = NULL; 
  
    return pNode; 
} 
  
// If not present, inserts key into trie 
// If the key is prefix of trie node, just 
// marks leaf node 
void insert(struct TrieNode *root, string key) 
{ 
    struct TrieNode *pCrawl = root; // current node we are working on 
    for (int i = 0; i < key.length(); i++) 
    { 
        int index = key[i] - 'a';  // calculating the position of the character in the english alphabet, a:0, b:1, c:2, d:4 ....
        if (!pCrawl->children[index]){ // if the ith children is not initialized, i.e it is null, create a new node, which means go deeper in the trie.
            pCrawl->children[index] = getNode(pCrawl); // we initialize ith children, now it is created and not null, but we dont insert any character
            pCrawl->children[index]->val = key[i];
            pCrawl->children[index]->depth = i + 1;
        }
        pCrawl = pCrawl->children[index]; // update pCrawl, which is the node that we are working on. this line goes deeper in the trie.
    } 
  
    // mark last node as leaf, when iterating through key is over, it is end of the word and traversal
    pCrawl->isEndOfWord = true; // after the iteration in done, it is a leaf node and end of the word.
} 
  
// Returns true if key presents in trie, else 
// false 
bool search(struct TrieNode *root, string key) 
{ 
    struct TrieNode *pCrawl = root; 
    
    for (int i = 0; i < key.length(); i++) 
    { 
        int index = key[i] - 'a'; // calculating the position of the character in the english alphabet, a:0, b:1, c:2, d:4 ....
        if (!pCrawl->children[index]) //  if ith child of the node is null
            return false; 
  
        pCrawl = pCrawl->children[index]; 
    } 
  
    return (pCrawl != NULL && pCrawl->isEndOfWord); 
}



int depthOfTree(struct TrieNode *ptr) 
{ 
    // Base case 
    if (!ptr) 
        return 0; 
  
    // Check for all children and find 
    // the maximum depth 
    int maxdepth = 0; // the depth is longest word size + 1 because root is a node too
    struct TrieNode* pCrawl = ptr;

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(pCrawl->children[i]){
            maxdepth = max(maxdepth, depthOfTree(pCrawl->children[i]));
        }
    }
    return maxdepth + 1;
} 

int isChildren(struct TrieNode * parent, char x){
    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(parent->children[i]){
            if(parent->children[i]->val == x){
                return i;
            }
        }

    }
    return -1;
}
string concatwords(struct TrieNode * current){
    string word = "";
    while(current->val != ' '){
        word = current->val + word;
        current = current->parent;
    }
    return word;
}

void printBFS(struct TrieNode * root){
    if(!root){
        return;
    }
    queue<TrieNode*> qu;
    qu.push(root);
    while(qu.size() != 0){
        TrieNode * current = (qu.front());
        cout << "id: " << current->id << " char: " << current->val << " next states: [ ";
        for(int i = 0; i < ALPHABET_SIZE; i++){
            if(current->children[i]){
                cout << current->children[i]->id << " ";
            }
        }
        cout << "]" << " fail state: " << current->failurelink->id << " output [";
        (current->isEndOfWord)? cout << concatwords(current) : cout << "";
        cout << "]" << endl;

        qu.pop();
        for(int i = 0; i < ALPHABET_SIZE; i++){
            if(current->children[i]){
                qu.push(current->children[i]);
            }
        }
    }
}


void BFS(struct TrieNode *root){
    if(!root){
        return;
    }
    int id = 0;
    queue<TrieNode*> qu;
    qu.push(root);
    int height = 0;
    while(qu.size() != 0){
        TrieNode * current = (qu.front());
        if(current->depth <= 1){
            current->failurelink = current->parent;
            current->id = id;
            id++;
        }
        else{
            // failure link computation
            TrieNode * vp = current->parent;
            char x = current->val;
            TrieNode * w = vp->failurelink;
            int childindex = 0;
            childindex = x - 'a'; 

            while(isChildren(w,x) == -1 && w->val != ' '){
                w = w->failurelink;
            }
            int isChildrenValue = isChildren(w,x);
            if(isChildrenValue != -1){
                current->failurelink = w->children[isChildrenValue];
            }
            else{
                current->failurelink = root;
            }
            current->id = id;
            id++;
        }
        qu.pop();
        for(int i = 0; i < ALPHABET_SIZE; i++){
            if(current->children[i]){
                qu.push(current->children[i]);
            }
        }
    }
    printBFS(root);
}
bool doesPathExist(TrieNode* current, char x){
    while(current->val != ' '){ // while it is not root
        if(current->val == x){
            return true;
        }
        current = current->failurelink;
    }
    return false;
}

void AhoCorasick(struct TrieNode * root, vector<string> pattern, string text){
    int l = 0;
    int c = 0;
    int n = text.length();
    TrieNode * w = root; 
    while(c < n){
        int childIndex = isChildren(w, text[c]);
        while(childIndex != -1){
            string word = concatwords(w);
            for(int i = 0; i < pattern.size(); i++){
                if(word.compare(pattern[i]) == 0 && word.compare("") != 0){
                    cout << "keyword: " << word << " index: " << l <<  endl;
                }
            }
            childIndex = isChildren(w, text[c]);
            if(childIndex == -1)
                break;
            w = w->children[childIndex];
            c++;
        }
        if(text[c] == ' '){
            c++;
        }
        w = w->failurelink; // w = nw
        string dummy = concatwords(w);
        l = c - dummy.size();
    }
}


// void AhoCorasick(struct TrieNode * root, string pattern[], string text){
//     int l = 0;
//     int c = 0;
//     int lp = 0;
//     int m = text.length();
//     //cout << "text is " << text << endl;
//     int patternsize = 0;
//     //cout << sizeof(pattern)/sizeof(pattern[0]) << endl;
    
//     TrieNode * w = root;
//     cout << "root" << w->id << endl;
//     cout << "textc: " << text[c] << endl;
//     do{
//         //string word = concatwords(w);  //concat all words until w from the root
//         //cout << "word is " << word << endl;
//         // while text[c] is an edge of trie 
//         cout << "textc " << text[c] << endl;
//         int childindex = isChildren(w, text[c]);
//         cout << " childindex" << childindex << endl;
//         cout << "dpe" << doesPathExist(w, text[c]) << endl;
//         while(childindex != -1 || doesPathExist(w, text[c])){
//             string word = concatwords(w);
//             cout << "word:" << word << endl;
//             cout << "girdi " << endl;
//             for(int i = 0; i < 7; i++){ // burası yanlış bence
//                 cout << "girdi 2" << endl;
//                 if(word.compare(pattern[i]) == 0){
//                     cout << "pattern found: " << pattern[i] << "at position " << c << endl;
//                 }
//             }
//             w = w->children[childindex];
//             cout << "c:" << c;
//             c++;
//             cout << "-->" << c << endl;
//             cout << "childindex:" << childindex;
//             childindex = isChildren(w, text[c]);
//             cout << "-->" << childindex << endl;

//         }
//         cout << "çıktı " << endl;
//         w = w->failurelink;
//         TrieNode * dum = w;
//         //cout << dum->depth << " " << dum->val << " " << dum->id << endl;
//         // while(dum->val != '!'){
//         //     lp++;
//         //     dum = dum->parent;
//         // }
//         //cout << "dum is" << dum << endl;
//         l = c - lp;
//     }while(c < m);
// }
        // do(
        //     childindex = isChildren(w, text[c]);
        //     for(int i = 0; i < sizeof(pattern) / sizeof(pattern[0]); i++){
        //         if(word.compare(pattern[i]) == 0){
        //             cout << "pattern found: " << pattern[i] << "at position " << c << endl;
        //         }
        //     }
        //     w = w->children[childindex];
        //     c++;
        // )while(childindex != -1);
        // w = w->failurelink;

        // while(isChildren(w, text[c]) != -1){
        //     // look for pattern array and w concatenated, if they match
        //     for(int i = 0; i < m; i++){
        //         if(word.compare(pattern[i]) == 0){ // string comparison böyle olur mu ????
                    
        //         }
        //     }
        //    //w = w->children;
        //     c++;
        // }
        //w = w->failurelink;
        // l = c - lp(w)



