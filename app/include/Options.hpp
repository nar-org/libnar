/**
 * @file    app/include/Options.hpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @Copyright Nicolas DI PRIMA
 */
#ifndef APP_INCLUDE_OPTIONS_HPP_
# define APP_INCLUDE_OPTIONS_HPP_

# include <list>
# include <memory>
# include <string>
# include <vector>

# include <boost/program_options.hpp>
# include <boost/exception/all.hpp>

namespace po = boost::program_options;

class no_error     : virtual boost::exception, virtual std::exception { };
class option_error : virtual boost::exception, virtual std::exception { };

class Module
{
  public:
    virtual void set( po::options_description&
                    , po::positional_options_description&
                    ) = 0;
    virtual void config(po::variables_map const& vm) = 0;
};

using ModuleP = std::shared_ptr<Module>;

class Options
{
  public:
    Options();

    Options(Options const&) = default;
    Options(Options&&)      = default;
    Options& operator= (Options const&) = default;
    Options& operator= (Options&&)      = default;
    virtual ~Options() = default;

    void add(ModuleP&&);
    void add(ModuleP const&);

    void run(int const, char const* const*);

  private:
    po::options_description            desc_;
    po::positional_options_description pos_;
    po::variables_map vm_;

    std::vector<ModuleP> modules_;
};

#endif /* ! APP_INCLUDE_OPTIONS_HPP_ */
