#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <set>
#include <tuple>
#include "Clipboard.h"
#include <Windows.h>




//TO-DO
//Add check for more than 2 wildcards


//Declarations
struct availableRack;
class sortDecendingAlphbetical;
std::vector<char> getRackLetters();
int getWildCount(const std::vector<char>& letters);
void removeWildFromRack(std::vector<char>& rack);
std::vector<std::string> getDictionary(const std::string filepath);
bool canBeSpelled(const std::string& word, const availableRack& rack);
std::vector<std::string> generatePossibleWords(const std::vector<std::string>& dictionary, const availableRack& rack, const unsigned int lowestLetterCount, const unsigned int highestLetterCount);
void printString(const std::string& outString);
void printToFile(const std::vector<std::string>& stringVector);
void copyEachToClipboard(const std::vector<std::string>& listToCopy);





//Definitions
struct availableRack
{
	std::vector<char> letters;
	int numberOfWild = 0;

};


std::vector<char> getRackLetters() {
	std::cout << "Enter up to 12 letters, use ? as wildcard (Max 2):\t";
	std::vector<char> letters;
	std::string s_letters;
	std::cin >> s_letters;
	for (unsigned int i = 0; i < s_letters.size(); i++)
		letters.push_back(s_letters[i]);
	return letters;
}


int getWildCount(const std::vector<char>& letters) {
	int numberOfWild = 0;
	for (unsigned int i = 0; i < letters.size(); i++) {
		if (letters[i] == '?')
			numberOfWild++;
	}
	return numberOfWild;
}


void removeWildFromRack(std::vector<char>& rack) {
	for (unsigned int i = 0; i < rack.size(); i++) {
		if (rack[i] == '?') {
			rack.erase(rack.begin() + i); //Removes wildcards from rack. Increased efficiency on checking the rack for any given word
		}
	}
}

std::vector<std::string> getDictionary(const std::string filepath) {
	std::ifstream inFile;
	inFile.open(filepath);
	std::vector<std::string> words;
	std::string currentWord;
	if (inFile.is_open()) {
		for (std::string word; std::getline(inFile, word);) {
			words.push_back(word);
		}
	}
	else {
		std::cout << "Could not find dictionary file.";
	}
	inFile.close();
	return words;
}


bool canBeSpelled(const std::string &word, const availableRack &rack) {
	std::vector<char> copiedLetters = rack.letters;
	int numberOfLettersUsed = 0;

	/*Looks at each letter in the word and determines if it finds it in the letter bank
	If it does, it will remove that letter from the bank, and add one to the letters used count*/
	for (unsigned int currentPositionInWord = 0; currentPositionInWord < word.size(); currentPositionInWord++) {
		for (unsigned int currentPossibleLetter = 0; currentPossibleLetter < copiedLetters.size(); currentPossibleLetter++) {
			if (word[currentPositionInWord] == copiedLetters[currentPossibleLetter]) {
				numberOfLettersUsed++;
				copiedLetters.erase(copiedLetters.begin() + currentPossibleLetter); //Erases currentPossibleLetter from copiedLetters
				currentPossibleLetter = copiedLetters.size();
			}
		}
	}


	/*After finding all the used letters, check if the word can be completed
	using the number of specified wildcards*/
	int numberOfNeededLetters = word.size() - numberOfLettersUsed;
	if (numberOfNeededLetters <= rack.numberOfWild)
		numberOfLettersUsed += numberOfNeededLetters;

	return (numberOfLettersUsed == word.length()) ? true : false;
}


std::vector<std::string> generatePossibleWords(const std::vector<std::string> &dictionary, const availableRack &rack ,const unsigned int lowestLetterCount, const unsigned int highestLetterCount) {
	std::vector<std::string> finalWords;
	for (unsigned int i = 0; i < dictionary.size(); i++) {
		std::string workingWord = dictionary.at(i);
		if (workingWord.size() >= lowestLetterCount && workingWord.size() <= highestLetterCount) {
			if (canBeSpelled(workingWord, rack)) {
				finalWords.push_back(workingWord);
			}
		}
	}
	return finalWords;
}


class sortDecendingAlphbetical
{
public:
	bool operator () (const std::string& lhs, const std::string& rhs) const
	{
		std::pair<size_t, std::string> lhsPair = std::make_pair(lhs.length(), lhs);
		std::pair<size_t, std::string> rhsPair = std::make_pair(rhs.length(), rhs);
		return (lhsPair.first > rhsPair.first) || (!(lhsPair.first > rhsPair.first) && (lhsPair.second < rhsPair.second));
	}
};


void printString(const std::string &outString) {
	std::cout << outString << std::endl;
}


void printToFile(const std::vector<std::string>& stringVector) {
	std::ofstream file;
	file.open("possibleWords.txt");
	for (unsigned int i = 0; i < stringVector.size(); i++) {
		file << stringVector.at(i) << std::endl;
	}
	file.close();
}


void copyEachToClipboard(const std::vector<std::string> &listToCopy) {
	HWND hwnd = GetDesktopWindow();
	for (unsigned int i = 0; i < listToCopy.size(); i++) {
		toClipboard_string(hwnd, listToCopy[i]);
		Sleep(600);
	}
}


int main() {
	availableRack rack;
	std::vector<std::string> acceptedDictionary = getDictionary("words_alpha.txt");
	//std::vector<std::string> acceptedDictionary = getDictionary("twl.txt");
	rack.letters = getRackLetters();
	rack.numberOfWild = getWildCount(rack.letters);
	removeWildFromRack(rack.letters);
	unsigned int minimumLetterCount, maximumLetterCount;
	std::cout << "Minimum number of letters for a word: ";
	std::cin >> minimumLetterCount;
	std::cout << "Maximum number of letters for a word: ";
	std::cin >> maximumLetterCount;
	std::vector<std::string> possibleWords = generatePossibleWords(acceptedDictionary, rack, minimumLetterCount, maximumLetterCount);
	std::stable_sort(possibleWords.begin(), possibleWords.end(), sortDecendingAlphbetical());
	std::for_each(possibleWords.begin(), possibleWords.end(), &printString);
	//copyEachToClipboard(possibleWords);
	printToFile(possibleWords);
	system("pause");
	//std::cin.get();
	return 0;
}