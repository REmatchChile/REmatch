#ifndef MAPPING_HPP
#define MAPPING_HPP

#include <utility>
#include <string>
#include <map>

namespace rematch {

struct mapping {
    using map_type = std::map<std::string, std::pair<size_t, size_t>>;

public:
    map_type data_;
    std::string &document_;

    mapping(map_type map_, std::string &doc_)
        : data_(map_), document_(doc_) {}

    map_type data() const {return data_;}

    std::string getMatch(std::string &variable) const {
        auto span = data_.at(variable);
        return document_.substr(span.first, (span.second - span.first));
    }

    std::pair<size_t, size_t> getSpan(std::string &variable) const {
        return data_.at(variable);
    }

}; // class Mapping

} // namespace rematch

#endif // MAPPING_HPP