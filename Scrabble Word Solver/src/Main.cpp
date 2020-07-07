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
bool canBeSpelled(const std::string& word, availableRack rack);
std::vector<std::string> generatePossibleWords(const std::vector<std::string>& dictionary,  availableRack rack, const unsigned int lowestLetterCount, const unsigned int highestLetterCount);
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
	std::transform(s_letters.begin(), s_letters.end(), s_letters.begin(), std::tolower);
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
	std::string filePath = filepath;
	std::ifstream inFile;
	std::vector<std::string> words;
	std::string currentWord;
	while (true) {
		inFile.open(filePath);
		if (inFile.is_open()) {
			for (std::string word; std::getline(inFile, word);) {
				words.push_back(word);
			}
			break;
		}
		else {
			std::cout << "Cannot open dictionary file. Standard dictionary file is 'words_alpha.txt'" << std::endl;
			char answer;
			while (true) {
				std::cout << "Would you like to specify a dictionary file? y/n" << std::endl;
				std::cin >> answer;
				switch (answer) {
				case 'y': case 'Y':
					std::cout << "What is the name of the dictionary file?" << std::endl;
					std::cin >> filePath;
					break;
				case 'n': case 'N':
					exit(1);
				default:
					std::cout << "Invalid Selection. Please try Again." << std::endl << std::endl;
					break;
				}
				break;
			}
		}
	}
	inFile.close();
	return words;
}


bool canBeSpelled(const std::string &word, availableRack rack) {
	//std::vector<char> copiedLetters = rack.letters;
	int numberOfLettersUsed = 0;

	/*Looks at each letter in the word and determines if it finds it in the letter bank
	If it does, it will remove that letter from the bank, and add one to the letters used count*/
	for (unsigned int letterInWord = 0; letterInWord < word.size(); letterInWord++) {
		for (unsigned int LetterInRack = 0; LetterInRack < rack.letters.size(); LetterInRack++) {
			if (word[letterInWord] == rack.letters[LetterInRack]) {
				numberOfLettersUsed++;
				rack.letters.erase(rack.letters.begin() + LetterInRack); //Erases currentPossibleLetter from copiedLetters
				LetterInRack = rack.letters.size(); //Sets couter to the end of the rack
			}
		}
	}


	/*After finding all the used letters, check if the word can be completed
	using the number of specified wildcards*/
	int numberOfNeededLetters = word.size() - numberOfLettersUsed;
	if (numberOfNeededLetters <= rack.numberOfWild)
		numberOfLettersUsed += numberOfNeededLetters;

	return numberOfLettersUsed == word.length();
}


std::vector<std::string> generatePossibleWords(const std::vector<std::string> &dictionary, availableRack rack ,const unsigned int lowestLetterCount, const unsigned int highestLetterCount) {
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