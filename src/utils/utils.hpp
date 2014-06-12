
#ifndef UTILS_H
#define UTILS_H

#define SQUARE(X) ((X)*(X))

#include "headers.hpp"
#include <vector>
#include <limits>

namespace utils {

	const std::string toStringMemory(unsigned long bytes);

	template <typename T>
	const std::string toStringVec3(const T &vx, const T &vy, const T &vz, const std::string sep = ",") {
		std::stringstream ss;
		ss << "(" << vx << sep << vy << sep << vz << ")";
		const std::string str(ss.str());
		return str;
	}
	
	template <typename T>
	const std::string toStringDimension(const T &width, const T &height, const T &length) {
		std::stringstream ss;
		ss << width << "x" << height << "x" << length;
		const std::string str(ss.str());
		return str;
	}

	template <typename T>
		T abs(T val) {
			return (val < T(0) ? -val : val);
		}


	template <typename T>
		bool areEqual(T val1, T val2) {
			return (abs<T>(val2 - val1) <= std::numeric_limits<T>::epsilon() * std::max(abs<T>(val1), abs<T>(val2)));
		}

	template<typename A, typename B>
		std::pair<B,A> flip_pair(const std::pair<A,B> &p)
		{
			return std::pair<B,A>(p.second, p.first);
		}

	template<typename A, typename B>
		std::map<B,A> flip_map(const std::map<A,B> &src)
		{
			std::map<B,A> dst;
			std::transform(src.begin(), src.end(), std::inserter(dst, dst.begin()), 
					flip_pair<A,B>);
			return dst;
		}

	template<typename A, typename B>
		std::vector<std::pair<B,A> > mapToReversePairVector(const std::map<A,B> &src) {
			std::vector<std::pair<B,A> > dst;

			auto it = src.begin();
			while(it != src.end()) {
				dst.push_back(std::pair<B,A>(it->second, it->first));
				++it;
			}

			return dst;
		}
}

#endif /* end of include guard: UTILS_H */
