#include <iostream>
#include <string>
#include <optional>
#include <fstream>
#include <vector>
#include <set>
#include <map>
using namespace std;

const char DELIMITER = ';';

struct Args {
	string convertingType;
	string inputFileName;
	string outputFileName;
};

optional<Args> ParseArgs(int argc, char* argv[])
{
	Args args;
	args.convertingType = argv[1];
	args.inputFileName = argv[2];
	args.outputFileName = argv[3];
	return args;
}

vector<string> split(string str, char delimiter)
{
	bool endOfStr = false;
	vector<string> splitVector;
	size_t currPos = 0;
	while (!endOfStr)
	{
		size_t nextPos = str.find(delimiter, currPos);
		if (nextPos == str.npos)
			endOfStr = true;
		if (nextPos - currPos != 0)
		{
			string value = str.substr(currPos, nextPos - currPos);
			splitVector.push_back(value);
			currPos = nextPos + 1;
		}
		else
		{
			currPos++;
			splitVector.push_back("-");
		}
	}	
	return splitVector;
}

vector<vector<string>> readMealyData(ifstream& input, set<string> &transitions, set<string> &statements)
{
	string str = "";
	getline(input, str);
	vector<string> strSplit = split(str, DELIMITER);
	vector<vector<string>> data(strSplit.size());
	while (true)
	{
		for (int i = 0; i < data.size(); i++)
		{
			data[i].push_back(strSplit[i]);
			if (strSplit[i].find("/") != str.npos)
			{
				transitions.insert(strSplit[i]);
				statements.insert(split(strSplit[i], '/')[0]);
			}
		}
		if (getline(input, str))
			strSplit = split(str, DELIMITER);
		else
			break;
	}
	return data;
}

int main(int argc, char* argv[])
{
	auto args = ParseArgs(argc, argv);
	ifstream input;
	input.open(args->inputFileName);
	ofstream output;
	output.open(args->outputFileName);
	string convertingType = args->convertingType;
	
	if (convertingType == "mealy-to-moore")
	{
		set<string> mealyTransitions;
		set<string> mealyStatements;
		map<string, string> transitionsMap;
		vector<vector<string>> mealyTable = readMealyData(input, mealyTransitions, mealyStatements);
		for (int i = 1; i < mealyTable.size(); i++)
		{
			if (mealyStatements.count(mealyTable[i][0]) == 0)
			{
				mealyTransitions.insert(mealyTable[i][0] + "/-");
			}
		}
		vector<vector<string>> mooreTable;
		mooreTable.push_back({ "-", "-" });
		for (int i = 1; i < mealyTable[0].size(); i++)
		{
			mooreTable[0].push_back(mealyTable[0][i]);
		}
		int i = 0;
		for (auto it = mealyTransitions.begin(); it != mealyTransitions.end(); it++)
		{
			transitionsMap[*it] = "q" + to_string(i);
			i++;
		}
		int j = 0;
		for (auto it = mealyTransitions.begin(); it != mealyTransitions.end(); it++)
		{
			vector<string> transition = split(*it, '/');
			vector<string> statement{ transition[1], transitionsMap[*it] };
			j = 0;
			while (mealyTable[j][0] != transition[0])
			{
				j++;
			}
			for (int k = 1; k < mealyTable[j].size(); k++)
			{
				if (mealyTable[j][k] != "-")
				{
					statement.push_back(transitionsMap[mealyTable[j][k]]);
				}
				else
				{
					statement.push_back("-");
				}
			}
			mooreTable.push_back(statement);
		}
		for (int i = 0; i < mooreTable[0].size(); i++)
		{
			for (int j = 0; j < mooreTable.size(); j++)
			{
				if (mooreTable[j][i] != "-")
				{
					output << mooreTable[j][i];
				}
				else if (j != 0 && i == 0)
				{
					output << "-";
				}
				if (j != mooreTable.size() + 1)
				{
					output << ';';
				}
			}
			output << "\n";
		}
	}
	else if (convertingType == "moore-to-mealy")
	{
		string str = "";
		vector<string> strSplit;
		vector<string> transitions;
		getline(input, str);
		strSplit = split(str, DELIMITER);
		transitions = strSplit;
		getline(input, str);
		strSplit = split(str, DELIMITER);
		output << ';';
		for (int i = 1; i < transitions.size(); i++)
		{
			output << strSplit[i] << ';';
			transitions[i] = strSplit[i] + "/" + transitions[i];
		}
		output << "\n";
		getline(input, str);
		strSplit = split(str, DELIMITER);
		while (true)
		{
			output << strSplit[0] << ';';
			for (int i = 1; i < strSplit.size(); i++)
			{
				if (strSplit[i] != "-")
				{
					int j = 0;
					while (strSplit[i] != split(transitions[j], '/')[0])
					{
						j++;
					}
					output << transitions[j];
				}
				else
					output << "-";
				output << ';';
			}
			output << "\n";
			if (getline(input, str))
				strSplit = split(str, DELIMITER);
			else
				break;
		}
	}

	return 0;
}