#include <Config.hpp>

// Backend
uint32_t Config::dispWidth = 0;
uint32_t Config::dispHeight = 0;
int32_t Config::dispX = 0;
int32_t Config::dispY = 0;
uint32_t Config::dispBorder = 0;
char Config::dispTitle[256] = "DEAR Desktop";

void Config::readconf(){
	FILE *fin = fopen(".config.dear", "r");
	if(!fin){
		std::cerr << "[Config] No config file `.config.dear`, use default config." << std::endl;
		return;
	}
	while(!feof(fin)){
		char line[300];
		fgets(line, 300, fin);
		
		// Backend
		if(sscanf(line, "displayWidth %u", &dispWidth)){
			continue;
		};
		if(sscanf(line, "displayHeight %u", &dispHeight)){
			continue;
		};
		if(sscanf(line, "displayX %d", &dispX)){
			continue;
		};
		if(sscanf(line, "displayY %d", &dispY)){
			continue;
		};
		if(sscanf(line, "displayBorder %u", &dispBorder)){
			continue;
		};
		if(sscanf(line, "displayTitle %256[^\n]", dispTitle)){
			continue;
		};
	}
}
