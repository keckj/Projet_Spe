
#include "colormap.hpp"

namespace ColorMap {
	
	std::map<std::string, std::pair<unsigned int, float*> > multiHueColorMaps() {
		
		std::map<std::string, std::pair<unsigned int, float*> > maps;

		maps["green-white"] = std::pair<unsigned int, float*>(0,(float*)multi_hue_1);
		maps["purple-blue"] = std::pair<unsigned int, float*>(1,(float*)multi_hue_2);
		maps["blue-green"] = std::pair<unsigned int, float*>(2,(float*)multi_hue_3);
		maps["red-orange"] = std::pair<unsigned int, float*>(3,(float*)multi_hue_4);
		maps["blue-white"] = std::pair<unsigned int, float*>(4,(float*)multi_hue_5);
		maps["green-blue"] = std::pair<unsigned int, float*>(5,(float*)multi_hue_6);
		maps["red-purple"] = std::pair<unsigned int, float*>(6,(float*)multi_hue_7);
		maps["red-pink"] = std::pair<unsigned int, float*>(7,(float*)multi_hue_8);
		maps["green-yellow"] = std::pair<unsigned int, float*>(8,(float*)multi_hue_9);
		maps["blue-yellow"] = std::pair<unsigned int, float*>(9,(float*)multi_hue_10);
		maps["crimson-yellow"] = std::pair<unsigned int, float*>(10,(float*)multi_hue_11);
		maps["red-yellow"] = std::pair<unsigned int, float*>(11,(float*)multi_hue_12);
		maps["red-yellow-green"] = std::pair<unsigned int, float*>(12,(float*)div_hue_0);
		maps["red-yellow-blue"] = std::pair<unsigned int, float*>(13,(float*)div_hue_1);
		maps["red-white-black"] = std::pair<unsigned int, float*>(14,(float*)div_hue_2);
		maps["purple-white-green"] = std::pair<unsigned int, float*>(15,(float*)div_hue_3);
		maps["pink-white-green"] = std::pair<unsigned int, float*>(15,(float*)div_hue_4);
		maps["brown-white-turquoise"] = std::pair<unsigned int, float*>(16,(float*)div_hue_5);
		maps["orange-white-purple"] = std::pair<unsigned int, float*>(17,(float*)div_hue_6);
		maps["qualitative 1"] = std::pair<unsigned int, float*>(18,(float*)qualitative_hue_0);
		maps["qualitative 2"] = std::pair<unsigned int, float*>(19,(float*)qualitative_hue_1);
		maps["qualitative 3"] = std::pair<unsigned int, float*>(20,(float*)qualitative_hue_2);
		maps["qualitative 4"] = std::pair<unsigned int, float*>(21,(float*)qualitative_hue_3);

		return maps;
	}
}
