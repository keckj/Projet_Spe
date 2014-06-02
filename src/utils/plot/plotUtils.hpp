
#ifndef PLOTUTILS_H
#define PLOTUTILS_H

#include <sstream>
#include <string>

namespace utils {

	void makeGnuPlotScript(const std::string &dst, 
			const std::string &prefix, 
			const std::string &gridName,
			const std::string &suffix,
			unsigned int minPlotId, unsigned int maxPlotId,
			unsigned int xmin=0u, unsigned int xmax=100u, 
			unsigned int ymin=0u, unsigned int ymax=100u,
			float dt=0.1f,
			bool generateGif=false,  const std::string &gif_dst = "");

	void execGnuPlotScript(const std::string &src, bool blockExec=true);

	void showGif(const std::string &src);
}

#endif /* end of include guard: PLOTUTILS_H */
