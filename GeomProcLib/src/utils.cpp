#include <utils.h>
#include <algorithm>

namespace GeomProc {


glm::vec3 hsv2rgb(glm::vec3 in){

    double h, p, q, t, f;
    long i;
    glm::vec3 out;

    h = in[0]*360.0;
    if (h >= 360.0){
        h = 0.0;
    }
    h /= 60.0;
    i = (long) h;
    f = h - i;
    p = in[2] * (1.0 - in[1]);
    q = in[2] * (1.0 - (in[1] * f));
    t = in[2] * (1.0 - (in[1] * (1.0 - f)));

    switch(i){
        case 0:
            out[0] = in[2];
            out[1] = t;
            out[2] = p;
            break;

        case 1:
            out[0] = q;
            out[1] = in[2];
            out[2] = p;
            break;

        case 2:
            out[0] = p;
            out[1] = in[2];
            out[2] = t;
            break;

        case 3:
            out[0] = p;
            out[1] = q;
            out[2] = in[2];
            break;

        case 4:
            out[0] = t;
            out[1] = p;
            out[2] = in[2];
            break;

        case 5:
        default:
            out[0] = in[2];
            out[1] = p;
            out[2] = q;
            break;
    }

    return out;
}


glm::vec3 hsv2rgb2(glm::vec3 in){

    double h, p, q, t, f;
    long i;
    glm::vec3 out;

    if (in.s <= 0.0){
        out[0] = in[2];
        out[1] = in[2];
        out[2] = in[2];
        return out;
    }

    h = in[0];
    if (h >= 360.0){
        h = 0.0;
    }
    h /= 60.0;
    i = (long) h;
    f = h - i;
    p = in[2] * (1.0 - in[1]);
    q = in[2] * (1.0 - (in[1] * f));
    t = in[2] * (1.0 - (in[1] * (1.0 - f)));

    switch(i){
        case 0:
            out[0] = in[2];
            out[1] = t;
            out[2] = p;
            break;

        case 1:
            out[0] = q;
            out[1] = in[2];
            out[2] = p;
            break;

        case 2:
            out[0] = p;
            out[1] = in[2];
            out[2] = t;
            break;

        case 3:
            out[0] = p;
            out[1] = q;
            out[2] = in[2];
            break;

        case 4:
            out[0] = t;
            out[1] = p;
            out[2] = in[2];
            break;

        case 5:
        default:
            out[0] = in[2];
            out[1] = p;
            out[2] = q;
            break;
    }

    return out;
}


float map_value(float val, float target_min, float target_max, float data_min, float data_max){

    if (val < data_min){
        val = data_min;
    }
    if (val > data_max){
        val = data_max;
    }

    // Map value
    return target_min + ((val - data_min)/(data_max - data_min))*(target_max - target_min);
}


void map_values(std::vector<float> &val, float target_min, float target_max, float data_min, float data_max){

    // Check if data min and max need to be computed
    if (data_min > data_max){
        std::vector<float>::iterator min_it = std::min_element(val.begin(), val.end());
        data_min = (*min_it);
        std::vector<float>::iterator max_it = std::max_element(val.begin(), val.end());
        data_max = (*max_it);
    }

    // Pre-compute some constants
    float range = data_max - data_min;
    float new_range = target_max - target_min;

    // Go through the vector mapping all values
    std::vector<float>::iterator vit, vend;
    vit = val.begin();
    vend = val.end();
    for (; vit != vend; vit++){
        // Clamp value
        if ((*vit) < data_min){
            (*vit) = data_min;
        }
        if ((*vit) > data_max){
            (*vit) = data_max;
        }

        // Map value
        (*vit) = target_min + (((*vit) - data_min)/range)*new_range;
    }
}


std::string get_extension(std::string str){

    if (str.size() <= 0){
        return std::string("");
    }

    int i = str.size()-1;
    for (; (i >= 0) && (str[i] != '.'); i--){
    }
    return str.substr(i+1, str.size()-i);
}


std::string get_basename(std::string str){

    // Find position of extension separator
    std::string::size_type pos;
    pos = str.find(".");
    // If separator not found, return the entire string
    if (pos == std::string::npos){
        return str;
    }
    // Return substring from the beginning until the separator
    return str.substr(0, pos);
}


void string_trim(std::string str, std::string to_trim){

    // Trim any character in to_trim from the beginning of the string str
    while ((str.size() > 0) && 
           (to_trim.find(str[0]) != std::string::npos)){
        str.erase(0);
    }

    // Trim any character in to_trim from the end of the string str
    while ((str.size() > 0) && 
           (to_trim.find(str[str.size()-1]) != std::string::npos)){
        str.erase(str.size()-1);
    }
}


std::vector<std::string> string_split(std::string str, std::string separator){

    // Initialize output
    std::vector<std::string> output;
    output.push_back(std::string(""));
    int string_index = 0;

    // Analyze string
    unsigned int i = 0;
    while (i < str.size()){
        // Check if character i is a separator
        if (separator.find(str[i]) != std::string::npos){
            // Split string
            string_index++;
            output.push_back(std::string(""));
            // Skip separators
            while ((i < str.size()) && (separator.find(str[i]) != std::string::npos)){
                i++;
            }
        } else {
            // Otherwise, copy string
            output[string_index] += str[i];
            i++;
        }
    }

    return output;
}


std::vector<std::string> string_split_one_sep(std::string str, std::string separator){

    // Initialize output
    std::vector<std::string> output;
    output.push_back(std::string(""));
    int string_index = 0;

    // Analyze string
    unsigned int i = 0;
    while (i < str.size()){
        // Check if character i is a separator
        if (separator.find(str[i]) != std::string::npos){
            // Split string
            string_index++;
            output.push_back(std::string(""));
        } else {
            // Otherwise, copy string
            output[string_index] += str[i];
        }
        i++;
    }

    return output;
}


} // namespace GeomProc
