#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>

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
		pList.erase(0, label.size());
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
		pref = pList.substr(start + 1, end - 1);
	while (!pref.empty())
	{
		size_t index = pref.find_first_of(',');
		if (index != std::string::npos)
		{
			// last entry does not have a comma seperator so take wad is left
			size_t lastEnd = pref.find_last_of('>');
			pref.erase(lastEnd, 1);
			result.emplace_back(pref);
			return result;
		}
		result.emplace_back(pref.substr(0, index));
		pref.erase(0, index + 1);
	}
	return  result;
}

void StableMatching()
{

}

void FindIndexOfStudentInHospitalPrefs(const std::vector<std::string>& hosPrefs,  std::pair<int,std::string>& studentIdxName)
{
	for (size_t i = 0; i < hosPrefs.size(); ++i)
	{
		if (hosPrefs[i] == studentIdxName.second)
			studentIdxName.first = i;
	}
}

struct Hospital
{
	std::vector<std::string> preferences;
	std::vector<std::string> positions;
	std::string name;

	Hospital() = default;
	Hospital(std::string& _name) : preferences{}, positions{}, name(_name) {}
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
			std::string label = ExtractLabel(line);
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

	/*
	* C:<5,1,2,4,3>;M:<5,3,1,2,4>;
	  1:<C,M>;2:<C,M>;3:<C,M>;4<M,C>;5:<M,C>;
	*/
	for (const auto& student : students)
	{
		if (!student.preferences.empty())
		{
			std::string preferredHospitalName = student.preferences[0];
			auto preferredHospital = (std::find_if(hospitals.begin(), hospitals.end(), [&preferredHospitalName](const Hospital& lhs)
				{
					return (lhs.name == preferredHospitalName);
				}));

			if (preferredHospital->positions.size() < 2)
			{
				preferredHospital->positions.push_back(student.name); // pair

				// @TODO Remove preferred hospital from student's preferences
			}
			else // hospital is full
			{
				for (const auto& pos : preferredHospital->positions)
				{
					preferredHospital->preferences.back();
				}

				std::pair<int, std::string> student1{ -1 , preferredHospital->positions[0] };
				std::pair<int, std::string> student2{ -1 , preferredHospital->positions[1] };
				std::pair<int, std::string> student3{ -1 ,student.name };

				FindIndexOfStudentInHospitalPrefs(preferredHospital->preferences, student1);
				FindIndexOfStudentInHospitalPrefs(preferredHospital->preferences, student2);
				FindIndexOfStudentInHospitalPrefs(preferredHospital->preferences, student3);

				// Replace existing students with preferred student
				int leastPrefStudent = std::max(student1.first, std::max(student2.first, student3.first));
				std::string nameOfLeastPrefStudent = "";
				if (leastPrefStudent == student1.first)
					nameOfLeastPrefStudent = student1.second;
				else if (leastPrefStudent == student2.first)
					nameOfLeastPrefStudent = student2.second;
				else
					nameOfLeastPrefStudent = student.name;

				// @TODO Check if least preferred student is in preferredHospital.positions
				// If inside, remove from positions, insert new student
			}

		}
	}


	/*constexpr uint8_t numSlots = 2;
	std::vector<uint8_t> freeSlots(hospitals.size());
	std::fill_n(freeSlots.data(), freeSlots.size(), numSlots);
	std::vector<bool> freeStudents(students.size());
	std::vector<std::pair<std::string, std::string>> pairs;

	for (const auto& student : students)
	{
		uint32_t id = nameToPrefMap[student];
		std::vector<std::string>& studentPreferences = dataBase[id];

		for (const auto& studentPref : studentPreferences)
		{
			uint32_t id = nameToPrefMap[studentPref];
			if (freeSlots[id] > 0)
			{
				pairs.push_back(std::make_pair(student, studentPref));
				if (freeSlots[id] != 0)
					--freeSlots[id];
			}
		}
	}*/



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