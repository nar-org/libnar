/**
 * @file    app/src/Options.cpp
 * @authors Nicolas DI PRIMA <nicolas@di-prima.fr>
 * @date    2016-04-09
 *
 * @Copyright Nicolas DI PRIMA
 */

#include "Options.hpp"
#include <iostream>

Options::Options()
  : desc_("Options")
  , pos_()
  , vm_()
  , modules_()
{
}

void Options::add(ModuleP&& m) {
  assert(m);
  m->set(desc_, pos_);
  modules_.push_back(std::move(m));
}
void Options::add(ModuleP const& m) {
  assert(m);
  m->set(desc_, pos_);
  modules_.push_back(m);
}

void Options::run( int const argc
                 , char const* const* argv
                 )
{
  po::store( po::command_line_parser(argc, argv)
                  .options(desc_)
                  .positional(pos_)
                  .run()
           , vm_
           );
  po::notify(vm_);

  for (auto const& module : modules_) {
    assert(module);
    module->config(vm_);
  }
}
