#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <queue>
#include <unordered_set>

//parse one line of text from file into single preferences list
std::vector<std::string> ParseLine(std::string line, const char spiltSymbol = ';')
{
	std::vector<std::string> pref;
	while (!line.empty())
	{
		size_t index = line.find_first_of(spiltSymbol);
		if (index == std::string::npos)
			return pref;
		pref.emplace_back(line.substr(0, index));
		line.erase(0, index + 1);
	}

	return pref;
}

std::string ExtractLabel(std::string pList)
{
	size_t index = pList.find_first_of(':');
	std::string label;
	if (index != std::string::npos)
	{
		label = pList.substr(0, index);
		pList.erase(0, label.size() + 1);
	}
	return label;
}

std::vector<std::string> ExtractPreferences(std::string pList)
{
	size_t start = pList.find_first_of('<');
	size_t end = pList.find_last_of('>');
	std::string pref;
	std::vector<std::string> result;

	if (start != std::string::npos && end != std::string::npos)
		pref = pList.substr(start + 1, end- start - 1);
	while (!pref.empty())
	{
		size_t index = pref.find_first_of(',');
		if (index != std::string::npos)
		{
			result.emplace_back(pref.substr(0,index));
		}
		else // take what is left as last element doest not have a comma
		{
			result.emplace_back(pref);
			break;
		}
		pref.erase(0, index + 1);

	}
	return result;
}


void FindIndexOfStudentInHospitalPrefs(const std::vector<std::string>& hosPrefs, std::pair<int, std::string>& studentIdxName)
{
	for (size_t i = 0; i < hosPrefs.size(); ++i)
	{
		if (hosPrefs[i] == studentIdxName.second)
			studentIdxName.first = i;
	}
}
struct StudentData
{
	uint32_t studentID{ 0 };
	int score{ 0 };
};

struct Hospital
{
	std::vector<std::string> preferences;
	std::vector<StudentData> positions;
	std::string name;

	Hospital() = default;
	Hospital(std::string& _name) : preferences{}, positions{}, name(_name) {}

	bool HasAvailablePositions() const noexcept { return positions.size() < 2; }
};

struct Student
{
	std::vector<std::string> preferences;
	std::string name;

	Student() = default;
	Student(std::string& _name) : preferences{}, name(_name) {}
};

int main(int argc, char* argv[])
{

	if (argc != 2)
	{
		printf("%s\n %s\n", "Invalid agrument count", "Provide preference list file as 2nd agrument");
	}

	std::ifstream file{ argv[1] };

	if (!file)
	{
		printf("%s:%s\n", "Unable to open file:", argv[1]);
		std::exit(0);
	}

	char temp[512];

	std::vector<Hospital> hospitals;
	std::vector<Student> students;
	int count = 0;

	while (!file.eof())
	{
		file.getline(temp, sizeof(char) * 512);
		std::string line = temp;
		std::vector<std::string> pList = ParseLine(line);

		for (const std::string& s : pList)
		{
			std::string label = ExtractLabel(s);
			std::vector<std::string> pref = ExtractPreferences(s);

			if (count == 0)
			{
				hospitals.emplace_back(label);
				hospitals.back().preferences = std::move(pref);
			}
			else
			{
				students.emplace_back(label);
				students.back().preferences = std::move(pref);
			}
		}
		++count;

	}

	/*printf("%s\n", "Hospitals");
	for (const Hospital& h : hospitals)
	{
		printf("%s:<", h.name.c_str());
		for (const std::string& p : h.preferences)
		{
			printf("%s,", p.c_str());
		}
		printf("%c\n", '>');
	}
	printf("%s\n", "Students");
	for (const Student& s : students)
	{
		printf("%s:<", s.name.c_str());
		for (const std::string& p : s.preferences)
		{
			printf("%s,", p.c_str());
		}
		printf("%c\n", '>');
	}
	return 0;*/
	/*
	* C:<5,1,2,4,3>;M:<5,3,1,2,4>;
	  1:<C,M>;2:<C,M>;3:<C,M>;4<M,C>;5:<M,C>;
	*/



	//setup queue of free students id
	//while free students not empty
	//	while freestudent preference list not empty
		//get a freestudent preference list
		//get first on preference list that has yet to propose to
		//if that choosen hospital is free 
			// accept offer add them to hospital pairs list	
		//else 
			//get the choosen hospital preferencelist
			//get score of students by using index from preferencelist
			//sort them by score remove the lowest from hospital
			//add the removed student id back to free student queue
		//remove freestudent's first preference
	//pop queue id



	auto GetStudentScore = [](const std::vector<std::string>& hospitalPref,const Student& student)
	{
		for (size_t i = 0; i < hospitalPref.size(); ++i)
		{
			if (hospitalPref[i] == student.name)
			{
				return static_cast<int>(hospitalPref.size() - i);
			}
		}
		return -100;
	};

	auto FindHospitalByName = [&hospitals](const std::string& hospitalName) ->  Hospital*
	{
		for ( Hospital& h : hospitals)
		{
			if (h.name == hospitalName)
				return &h;
		}
		return nullptr;
	}; 
	std::queue<uint32_t> freeStudents;
	std::unordered_set<uint32_t> paired;
	//populate the queue
	for (size_t i = 0; i < students.size(); ++i)
		freeStudents.emplace((uint32_t)i);
	while (!freeStudents.empty())
	{
		uint32_t studentID = freeStudents.front();
		std::vector<std::string>& pref = students[studentID].preferences;
		while (!pref.empty())
		{
			Hospital& h = *FindHospitalByName(pref[0]); 
			StudentData data{};
			data.studentID = studentID;
			if (h.HasAvailablePositions())
			{
				//make pairing
				h.positions.emplace_back(data);
				paired.insert(studentID);
				pref.erase(pref.begin());
				break;
			}
			
			{
				//place student temporary in for sorting afterwards perform removal
				h.positions.emplace_back(data);
				for (StudentData& d : h.positions)
				{
					d.score = GetStudentScore(h.preferences, students[d.studentID]);
				}
				//sort students by score // the least fav will be place at the back of vector for fast removal
				std::sort(h.positions.begin(), h.positions.end(), [](const StudentData& lhs, const StudentData& rhs) 
				{
					return lhs.score > rhs.score;
				});
				paired.insert(studentID);

				if(paired.find(studentID) == paired.end())
					freeStudents.emplace(h.positions.back().studentID);
				//unpairing
				paired.erase(h.positions.back().studentID);
				h.positions.erase(--h.positions.end());
			}
			pref.erase(pref.begin());
		}
		freeStudents.pop();
	}

	//results
	for (const Hospital& h : hospitals)
	{
		for (const StudentData& d : h.positions)
		{
			printf("Pair: %s,%s\n", h.name.c_str() ,students[d.studentID].name.c_str());
		}
	}

	/*#ifdef _DEBUG
	for (const auto& prefList : dataBase)
	{
		for (const auto& data : prefList)
		{
			printf("%s,",data.c_str());
		}
		printf("\n");
	}
	#endif*/


	return 0;
}