You can find visual representation of Trie for the example input on Trie.pdf.
My index IDs starts from 0 and it follows a Breadth First manner. For every 
node, left of the node has ID value, right of the node has failure link value.
For the leftest a node, ID is 1, failure link is 0 which is root.
For e node, at the end of once, ID of node is 37, failure link is on e with
ID number 3.
My Trie constructor does not include words in the pattern. It only includes words in text.
Such as car, or rotate does not shown in build tree output.Trie is build from text, then
Pattern is checked whether it is in the text.
Please does not use characters such as i ö ü etc. Use only english alphabet, a-b-c-d-e-f-g-h-ı-j-k-l-m-n-o-p-q-r-s-t-u-v-w-x-y-z, and lower case.
max text input size is 300 words

you can compile with: make all  
run with: make run 
	  make run2
	  make run3

for custom input: ./hw3 -i inputname.txt
