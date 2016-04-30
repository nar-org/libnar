/**
 * @file    app/src/main.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @Copyright Nicolas DI PRIMA
 */

#include <iostream>
#include <string>
#include <vector>

#include "nar/roundup.hpp"

#include "Options.hpp"
#include "Archive.hpp"
#include "nar/fstream.hpp"

class MainOption : public Module
{
  public:
    explicit MainOption(std::string const& program_name)
      : program_name_(program_name)
      , desc_(nullptr)
    {
    }

    virtual void set( po::options_description& desc
                    , po::positional_options_description& pos) {
      desc.add_options()
        ("help,h",  "produce help message")
        ("version", "show program version")
        ;
      desc_ = &desc;
    }
    virtual void config(po::variables_map const& vm) {
      if (vm.count("help")) {
        assert(desc_);
        std::cout
          << "Usage:" << std::endl
          << "  " << program_name_ << " -a <file> PATH [PATH...]" << std::endl
          << "  " << program_name_ << " -a <file> -x [PATH]" << std::endl
          << std::endl
          << *desc_;
        throw no_error();
      }
      if (vm.count("version")) {
        std::cout
          << program_name_
            << " Copyright (c) 2014-2016, All Rights Reserved."
              << std::endl
            << "current-version v"
              << nar::current_format_version
              << std::endl
            << "supported-version v"
              << nar::compatible_format_version
              << std::endl;
        throw no_error();
      }
    }

  private:
    std::string const program_name_;
    po::options_description const* desc_;
};

int main(int const argc, char const* const* argv)
{
  Options options;

  auto main_opts = std::make_shared<MainOption>(argv[0]);
  auto arch_opts = std::make_shared<Archive>();

  try {
    // register all the modules...
    options.add(main_opts);
    options.add(arch_opts);

    // parse modules options
    options.run(argc, argv);
  } catch (no_error const&) { return 0;
  } catch (option_error const&) { return 1;
  } catch (std::exception const& e) {
    std::cerr << e.what() << std::endl;
    return 2;
  } catch (...) {
    std::cerr << "unknown error..." << std::endl;
    return 3;
  }

  if (arch_opts->list()) {
    std::ifstream ifs( arch_opts->filepath()
                     , std::ios_base::in | std::ios_base::binary
                     );
    try {
      while (ifs.good()) {
        nar::header::generic::header h;
        ifs >> h;
        switch (h.magic) {
          case nar::known_magic::narh<std::uint64_t>:
            std::cout
                << "[ NARH ]" << std::endl
                << "  flags: " << std::hex << h.flags << std::endl
                << "  size1: " << std::dec << h.length_1 << std::endl
                << "  size2: " << std::dec << h.length_2 << std::endl;
            break;
          case nar::known_magic::file<std::uint64_t>:
            {
              std::vector<char> data(h.length_1, 0);
              ifs.read(data.data(), data.size());
              if (ifs.gcount() != h.length_1) { /* TODO(nicolas) error */ }
              std::string filename(data.data(), data.size());

              ifs.ignore( nar::PADDING64(h.length_1)
                        + nar::ROUND_UP64(h.length_2)
                        );
              std::cout
                << "[ FILE ]" << std::endl
                << "  flags: " << std::hex << h.flags << std::endl
                << "  path:  " << filename << std::endl
                << "  size:  " << std::dec << h.length_2 << std::endl;
              break;
            }
          default:
            std::cout
              << "[******]: unkown NAR header ("
                << std::hex << h.magic << ")" << std::endl
              << "  flags:    " << std::hex << h.flags << std::endl
              << "  lenght_1: " << std::dec << h.length_1 << std::endl
              << "  lenght_2: " << std::dec << h.length_2 << std::endl;
            ifs.ignore( nar::ROUND_UP64(h.length_1)
                      + nar::ROUND_UP64(h.length_2)
                      );
            break;
        }
        ifs.peek();
      }
    } catch (boost::exception const& e) {
      std::cerr << boost::diagnostic_information(e) << std::endl;
      return 1;
    }
  } else if (arch_opts->extract()) {
    // extract has been called
    //
    // There is only one archive. And potentially only one directory target.
    //
    // TODO(nicolas): provide an iterator over the archive. Every File ITEM:
    //                push then in a concurrent queue awaiting to start the
    //                pipeline.
    //
    // '[ FILE ] ....' (all Item Header) + File Offset
    //          |
    //          | (in a temporary location?)
    //          V
    // Decompression (if needed)
    //          |
    //          | (in a temporary location?)
    //          V
    // Unciphering (if needed)
    //          |
    //          | (in a temporary location?)
    //          V
    // Copy result into the appropriate location
  } else {
    nar::ofstream ofs(arch_opts->filepath());

    for (std::string const& input : arch_opts->paths()) {
      ofs.append(input);
    }
  }

  return 0;
}
