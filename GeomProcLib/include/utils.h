#ifndef UTILS_H_
#define UTILS_H_

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <sstream>

namespace GeomProc {

    // Helper functions 

    // Color conversion
    glm::vec3 hsv2rgb(glm::vec3 in);

    // Mapping of values from one range to another
    float map_value(float val, float target_min, float target_max, float data_min, float data_max);

    void map_values(std::vector<float> &val, float target_min, float target_max, float data_min = 1, float data_max = -1);

    // String utility functions
    // Get the extension of a filename
    std::string get_extension(std::string str);
    // Get the basename of a filename
    std::string get_basename(std::string str);
    // Trim any character in to_trim from the beginning and end of str
    void string_trim(std::string str, std::string to_trim);
    // Split string into substrings according to characters in separator
    std::vector<std::string> string_split(std::string str, std::string separator);
    // Split string into substrings according to characters in separator. A
    // split is performed when one separator character is found, rather than
    // a sequence of separators
    std::vector<std::string> string_split_one_sep(std::string str, std::string separator);
    // Conversion between strings and numbers
    template <typename T> std::string num_to_str(T num){

        std::ostringstream ss;
        ss << num;
        return ss.str();
    }

    template <typename T> T str_to_num(const std::string &str){

        std::istringstream ss(str);
        T result;
        ss >> result;
        if (ss.fail()){
            throw(std::ios_base::failure(std::string("Invalid number: ")+str));
        }
        return result;
    }

} // namespace GeomProc

#endif // UTILS_H_
