#ifndef _CALENDAR_READER_H_
#define _CALENDAR_READER_H_

#include <vector>
#include <fstream>
#include <string>

class CalendarReader {
public:
	CalendarReader();
	~CalendarReader();
	int read(std::ifstream& istream);
    void reset();
	const std::string& getDescription(int day);
	const std::string& getContent(int day);

private:
	std::vector<std::string> descriptions_, contents_, backgrounds_;
};

#endif
