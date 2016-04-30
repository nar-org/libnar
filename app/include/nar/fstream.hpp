/**
 * @file    app/include/nar/fstream.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-11
 *
 * @Copyright Nicolas DI PRIMA
 */
#ifndef APP_INCLUDE_NAR_FSTREAM_HPP_
# define APP_INCLUDE_NAR_FSTREAM_HPP_

# include <fstream>
# include <memory>
# include <vector>

# include "nar/generic/header.hpp"
# include "nar/serialisation.hpp"
# include "nar/error.hpp"

namespace nar {

class ofstream
{
  public:
    ofstream()                       = default;
    ~ofstream()                      = default;
    ofstream(ofstream&&)             = default;
    ofstream& operator= (ofstream&&) = default;

    ofstream(ofstream const&)             = delete;
    ofstream& operator= (ofstream const&) = delete;

    explicit ofstream(char const*);
    explicit ofstream(std::string const&);

    void open(char const*        filename);
    void open(std::string const& filename);
    bool is_open() const;
    void close();

    ofstream& flush();

    ofstream& padding(std::uint64_t);
    ofstream& append(std::string const&);

  private:
    std::ofstream ofs_;
};

} /* ! namspace nar */

#endif /** ! APP_INCLUDE_NAR_FSTREAM_HPP_  */
