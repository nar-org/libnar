/**
 * @file    app/include/Archive.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-11
 *
 * @Copyright Nicolas DI PRIMA
 */
#ifndef APP_INCLUDE_ARCHIVE_HPP_
# define APP_INCLUDE_ARCHIVE_HPP_

# include <string>
# include <vector>

# include "Options.hpp"

using Paths = std::vector<std::string>;

class Archive : public Module
{
  public:
    Archive();

    virtual ~Archive()                  = default;
    Archive(Archive const&)             = default;
    Archive(Archive&&)                  = default;
    Archive& operator= (Archive const&) = default;
    Archive& operator= (Archive&&)      = default;

    virtual void set( po::options_description&
                    , po::positional_options_description&
                    );
    virtual void config(po::variables_map const&);

    bool extract() const;
    bool list() const;
    std::string const& filepath() const;
    Paths const& paths() const;

  private:
    std::string filepath_;
    Paths       inputs_;
    bool        extract_;
    bool        list_;
};

#endif /** ! APP_INCLUDE_ARCHIVE_HPP_  */
