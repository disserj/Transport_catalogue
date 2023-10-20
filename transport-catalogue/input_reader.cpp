
#include "input_reader.h"

namespace transport{

bool IsDigit(std::string& line){
    	return std::all_of(line.begin(),line.end(),[](const auto& c){return std::isdigit(c);});
}

bool NotWS(std::string& line){
    	return !std::all_of(line.begin(),line.end(),[](const auto& c){return c == ' ';});
}

void Read(std::istream& in, RequestQueue& request) {
	int count=0;
	std::string line;
	if (!std::getline(in, line)) return;
	if(IsDigit(line)){
		count = std::stoi(line);
	}
	request.ToFill.resize(count);
	for (int i = 0; i < count; i++) {
		std::string temp_line;
		if (!std::getline(in, temp_line)) return;
        	if(!temp_line.empty()){
            	temp_line = temp_line.substr(temp_line.find_first_not_of(' '), 
			temp_line.find_last_not_of(' ') - temp_line.find_first_not_of(' ') + 1 );
            	}
		request.ToFill[i] = std::move(temp_line);
	}
	if (!std::getline(in, line)) return;
	if(IsDigit(line)){
		count = std::stoi(line);
	}
	request.ToResponse.resize(count);
	for (int i = 0; i < count; i++) {
		std::string temp_line;
		if (!std::getline(in, temp_line)) return;
		if(!temp_line.empty()){
        	temp_line = temp_line.substr(temp_line.find_first_not_of(' '), 
			temp_line.find_last_not_of(' ') - temp_line.find_first_not_of(' ') + 1 );
            	}
		request.ToResponse[i] = std::move(temp_line);
	}
}

std::pair<std::vector<std::string>, bool> ReadRoute(std::string& str) {
    	std::vector<std::string> stops;
    	bool cycle = false;
    	std::string name;
    	std::string line = str;
    	char c = '>';
    	if(line.find(c) == line.npos) {
		c = '-';
	}
    	while (line.find(c) != line.npos) {
        	name = line.substr(0, line.find_first_of(c) - 1);
        	stops.push_back(name);
        	line.erase(0, line.find_first_of(c) + 2);
    	}
    	name = line.substr(0, line.npos - 1);
    	stops.push_back(name);
    	if (c == '>'){
		cycle = true;
	}
    	return make_pair(stops, cycle);
}

void FillCatalogue(RequestQueue& request, TransportCatalogue& catalogue){
    	std::vector<std::string> to_add_bus;
    	std::vector<std::string> to_add_stop;
    	std::vector<std::pair<std::string, std::string>> to_add_dist;
    	for(auto& str : request.ToFill){
        	if(str.substr(0,4) == "Stop"){
            		to_add_stop.push_back(str);
        	}
		else if(str.substr(0,3) == "Bus"){
            		to_add_bus.push_back(str);
        	}
    	}
    	for(auto& str : to_add_bus){
        	FillBuses(str, catalogue);
    	}
    	for(auto& str : to_add_stop){
        	to_add_dist.push_back(FillStops(str, catalogue));
    	}
    	for(auto& [name,str] : to_add_dist){
        	FillDistances(name, str, catalogue);
    	}  
}

std::pair<std::string,std::string> FillStops(std::string& str, TransportCatalogue& catalogue){
    	std::string name = str.substr(str.find_first_of(' ')+1, str.find_first_of(':')-5);
    	str.erase(0, str.find_first_of(':')+2);
    	double lat = std::stod(str.substr(0, str.find_first_of(',')));
    	str.erase(0, str.find_first_of(',')+2);      
    	double lng = std::stod(str.substr(0, str.find_first_of(',')));
    	if(str.find_first_of(',') == str.npos) {
		str.clear();
	}
    	else{
		str.erase(0, str.find_first_of(',')+2);
	}
    	geo::Coordinates point = { lat, lng };
    	catalogue.AddStop(name, point);
    	return {name, str};
}

void FillBuses(std::string& str, TransportCatalogue& catalogue){
    	std::string num;
    	num = str.substr(str.find_first_of(' ')+1, str.find_first_of(':')-4);
    	str.erase(0, str.find_first_of(':') + 2);
    	auto [stops, cycle] = ReadRoute(str);
    	catalogue.AddRoute(num, stops, cycle);
    	stops.clear();
}

void FillDistances(std::string& name, std::string& str, TransportCatalogue& catalogue){
        Stop* title = catalogue.FindStop(name);
        if(!str.empty() && NotWS(str)){
            while (!str.empty()) {
                int dist = 0;
                std::string stop;
                dist = std::stoi(str.substr(0, str.find_first_of("m to ")));
                str.erase(0, str.find_first_of("m to ") + 5);
                if (str.find("m to ") == str.npos) {
                    stop = str;
                    Stop* neighbor = catalogue.FindStop(stop);
                    catalogue.SetDistance(title, neighbor, dist);
                    str.clear();
                }else {
                    stop = str.substr(0, str.find_first_of(','));
                    Stop* neighbor = catalogue.FindStop(stop);
                    catalogue.SetDistance(title, neighbor, dist);
                } 
                str.erase(0, str.find_first_of(',') + 2);
            }
      }
}
    
}// exit transport    
