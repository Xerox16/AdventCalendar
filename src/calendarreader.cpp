#include "../include/calendarreader.h"

#include <iostream>

CalendarReader::CalendarReader() 
	:descriptions_(), contents_(), backgrounds_() {
}

CalendarReader::~CalendarReader() {
}

int CalendarReader::read(std::ifstream& istream) {
	std::string bufferStr;
	std::vector<std::string> bufferVec;

	while(std::getline(istream, bufferStr)) {
		bufferVec.push_back(bufferStr);
	}

	if(bufferVec.size() != 48) {
		std::cerr<<"Either incorrect number of entries in content file or file corrupt!"<<std::endl;
		return -1;
	}

	for(int i = 0; i < 48; i += 2) {
		descriptions_.push_back(bufferVec[i]);
		//erase tab at beginning of content entry
		contents_.push_back(bufferVec[i+1].erase(0,0));
	}

	return 0;
}

const std::string& CalendarReader::getDescription(int day) {
	return descriptions_.at(day);
}

const std::string& CalendarReader::getContent(int day) {
	return contents_.at(day);
}


