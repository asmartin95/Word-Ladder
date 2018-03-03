/*
Project 3
Name: Alec Martin
Summary: In this project we find a word ladder between sets of 2 words and output the shortest ladder to a text file. We do this by creating
a graph and traversing the graph till we find the shortest path between the 2 words. A normal word ladder deals only with 1 character
substitutions but this word ladder also allows 1 character insertions and deletions.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <list>
#include <stack>
#include <unordered_map>
using namespace std;

typedef unordered_map<string, struct Vertex*> dictionary;

char alphabet[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
dictionary myDictionary;
dictionary::hasher string_hash = myDictionary.hash_function();
ofstream outFile;

//Vertex structure, stores the word and a num for easy lookup in an array
//also contains a list of neighbors and a pointer to a parent vertex for easy traversal back through the word ladder
struct Vertex {
	string word;
	bool isVisited;
	struct Vertex *parent;
	list<struct Vertex*> neighbors;
};

//Graph class
class Graph {
public:
	int size;

	//Initialize graph size and create visited/parent array of that size
	Graph(int a) {
		this->size = a;
	}

	//Add edge from dest to src by pushing it onto neighbors list
	//Though the graph is undirected and dest also should add src to its neighbors, the loop for finding edges will go through
	//and find a link between dest and src later and create the respective edge
	void addEdge(struct Vertex* src, struct Vertex* dest) {
		src->neighbors.push_back(dest);
	}

	//Print ladder
	//Take in 2 vertices and find if there is a ladder between them
	void printLadder(struct Vertex* src, struct Vertex* dest) {
		queue<struct Vertex*> toVisit;
		list<struct Vertex*> adjList;
		struct Vertex* currVert;

		reset();

		//add src toVist queue and mark it as visited in the visited array
		src->isVisited = true;
		toVisit.push(src);

		//While the queue is not empty set the currVertex to the front and pop the front from the queue
		while (!toVisit.empty()) {
			currVert = toVisit.front();
			toVisit.pop();

			//If the currVertex is equal to the destination vertex break from the loop
			if (currVert->word.compare(dest->word) == 0) break;

			//Else add all of the currVertex's neighbors to the queue if they havent been visited yet
			adjList = currVert->neighbors;
			while (!adjList.empty()) {
				if (adjList.front()->isVisited == false) {
					adjList.front()->isVisited = true;
					adjList.front()->parent = currVert;
					toVisit.push(adjList.front());
				}
				adjList.pop_front();
			}
		}

		//After the loop
		//Either breaked because currVertex equals dest or because dest was not found
		//If currVertex != dest we know it was not found and print no word ladder to file
		if (currVert->word.compare(dest->word) != 0) {
			outFile << "no word ladder";
		}
		//else we create a stack and retrace the vertices' parent pointers and add them to a stack
		else {
			stack<string> wordLadder;
			wordLadder.push(currVert->word);

			//If the parent array equals -1 at any point we know we found the end of the ladder and break from the loop
			while (currVert->parent != nullptr) {
				currVert = currVert->parent;
				wordLadder.push(currVert->word);
			}
			//Now we go back through the stack to output the ladder to the output file
			while (!wordLadder.empty()) {
				outFile << wordLadder.top() << " ";
				wordLadder.pop();
			}
		}
		outFile << "\r\n";
	}

	//Reset function for Graph class. Sets isVisited and parent to false and nullptr respectively
	void reset() {
		for (auto it = myDictionary.begin(); it != myDictionary.end(); it++) {
			it->second->isVisited = false;
			it->second->parent = nullptr;
		}
	}
};

int main() {
	string s, subString, insertString, deleteString;
	ifstream dictionaryFile, inputFile;
	outFile.open("output.txt");
	int count = 0;

	//Get the dictionary file name and open it
	cout << "Please enter the dictionary filename: ";
	cin >> s;
	dictionaryFile.open(s);
	while (!dictionaryFile) {
		cout << "Error: unable to open " << s << endl;
		cout << "Please enter the dictionary filename again: ";
		cin >> s;
		dictionaryFile.open(s);
	}

	//Build Dictionary
	string word;
	dictionaryFile >> word;
	while (!dictionaryFile.eof()) {
		struct Vertex* v = new Vertex{ word };
		myDictionary.insert(make_pair(word, v));
		dictionaryFile >> word;
		count++;
	}

	//Build graph
	Graph G(count);

	//Check dictionary words with potential ladder-words to see if they exist in dictionary
	unordered_map<string, struct Vertex*>::iterator itr;
	cout << "Building dictionary..." << endl;
	for (itr = myDictionary.begin(); itr != myDictionary.end(); itr++)
	{
		s = itr->second->word;

		//Find potential string substitutions
		for (int i = 0; i < s.length(); i++) {
			for (int j = 0; j < 26; j++) {
				subString = s;
				subString[i] = alphabet[j];
				if (s.compare(subString) == 0) continue; //if string is the same, skip it

				//Look for subString in myDictionary map and if found create edge between the orginal string and altered string vertex
				unordered_map<string, struct Vertex*>::const_iterator got = myDictionary.find(subString);
				if (got != myDictionary.end()) {
					G.addEdge(itr->second, got->second);
				}
			}
		}

		//Find potential string insertions
		for (int i = 0; i < s.length() + 1; i++) {
			for (int j = 0; j < 26; j++) {
				insertString = s;
				insertString.insert(i, 1, alphabet[j]);
				if (s.compare(insertString) == 0) continue; //if string is the same, skip it

				//Look for insertString in myDictionary map and if found create edge between the orginal string and altered string vertex
				unordered_map<string, struct Vertex*>::const_iterator got = myDictionary.find(insertString);
				if (got != myDictionary.end()) {
					G.addEdge(itr->second, got->second);
				}
			}
		}

		//Find potential string deletions
		for (int i = 0; i < s.length(); i++) {
			deleteString = s;
			deleteString.erase(i, 1);

			//Look for deleteString in myDictionary map and if found create edge between the orginal string and altered string vertex
			unordered_map<string, struct Vertex*>::const_iterator got = myDictionary.find(deleteString);
			if (got != myDictionary.end()) {
				G.addEdge(itr->second, got->second);
			}
		}
	}

	//Get the input file name and open it
	string filename, word1, word2;;
	int wordCount;
	cout << "\nPlease enter the input filename: ";
	cin >> filename;
	inputFile.open(filename);
	while (!inputFile) {
		cout << "Error: unable to open " << filename << endl;
		cout << "Please enter the dictionary filename again: ";
		cin >> filename;
		inputFile.open(filename);
	}

	//call ladder function on each pair of words in input file
	//since we can assume both words are in the dictionary we dont need to check them
	inputFile >> wordCount;
	for (int i = 0; i < wordCount; i++) {
		inputFile >> word1 >> word2;
		unordered_map<string, struct Vertex*>::const_iterator v1 = myDictionary.find(word1);
		unordered_map<string, struct Vertex*>::const_iterator v2 = myDictionary.find(word2);
		G.printLadder(v1->second, v2->second);
	}

	//Close file streams
	dictionaryFile.close();
	inputFile.close();
	cout << "Writing to output.txt" << endl; 
	outFile.close();

	return 0;
}