#ifndef BA_HUNAPP_H
#define BA_HUNAPP_H

#include "MooseApp.h"

class BaHunApp;

template<>
InputParameters validParams<BaHunApp>();

class BaHunApp : public MooseApp
{
public:
  BaHunApp(InputParameters parameters);
  virtual ~BaHunApp();

  static void registerApps();
  static void registerObjects(Factory & factory);
  static void associateSyntax(Syntax & syntax, ActionFactory & action_factory);
};

#endif /* BA_HUNAPP_H */
