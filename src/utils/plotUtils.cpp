
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "log.hpp"

using namespace log4cpp;

namespace utils {

	void makeGnuPlotScript(const std::string &dst, 
			const std::string &prefix, 
			const std::string &gridName,
			const std::string &suffix,
			unsigned int minPlotId, unsigned int maxPlotId,
			unsigned int xmin, unsigned int xmax, 
			unsigned int ymin, unsigned int ymax,
			float dt,
			bool generateGif,  const std::string &gif_dst) {

		std::ofstream file(dst, std::ios::out | std::ios::trunc);
		
		if(!file.good()) {
			log_console->errorStream() << "Failed to open file " << dst << " !";
			file.close();
			exit(EXIT_FAILURE);
		}

		file << "#" << dst << "\n";
		file << "reset\n";
		file << "set xrange [" << xmin << ":" << xmax << "]\n";
		file << "set yrange [" << ymin << ":" << ymax << "]\n";

		if(generateGif) {
			file << "set term gif animate\n";
			file << "set output '" << gif_dst << "'\n";
		}
		else {
			file << "do for [i=1:2] {\n\t";
		}
		
		file << "do for [i=" << minPlotId << ":" << maxPlotId << "] {\n";
		if(!generateGif) file << "\t";
		file << "\tplot '" << prefix  << gridName << "'.i.'" << suffix << "' matrix with image;\n";
		if(!generateGif) file << "\t\tpause " << dt << ";\n";
		if(!generateGif) file << "\t}\n";

		file << "}\n";
		file << "#End of " << dst << "\n";
		file.close();
	}

	void execGnuPlotScript(const std::string &src, bool blockExec) {
		std::stringstream command;
		command << "gnuplot " << src;
	
		if(blockExec) {
				log_console->infoStream() << "Generating gif from " << src << "...";
				system(command.str().c_str());
				log_console->infoStream() << "Gif " << src << " generated.";
		}
		else {
			pid_t child_pid = fork();
			if(child_pid == 0) {
				system(command.str().c_str());
				log_console->infoStream() << "Gif " << src << " generated.";
				exit(0);
			}
			else {
				log_console->infoStream() << "Generating gif from " << src << " with thread " << child_pid << "...";
			}
		}
	}

	void showGif(const std::string &src) {
		std::stringstream command;
		command << "gifview -a " << src;
		
		pid_t child_pid = fork();
		if(child_pid == 0) {
			system(command.str().c_str());
			exit(0);
		}
	}
	
}
