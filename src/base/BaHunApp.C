#include "BaHunApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

// Materials
#include "BAMaterial.h"

// DiracKernels
#include "BAPolyLineSink.h"

template<>
InputParameters validParams<BaHunApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

BaHunApp::BaHunApp(const std::string & name, InputParameters parameters) :
    MooseApp(name, parameters)
{
  srand(processor_id());

  Moose::registerObjects(_factory);
  ModulesApp::registerObjects(_factory);
  BaHunApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  ModulesApp::associateSyntax(_syntax, _action_factory);
  BaHunApp::associateSyntax(_syntax, _action_factory);
}

BaHunApp::~BaHunApp()
{
}

extern "C" void BaHunApp__registerApps() { BaHunApp::registerApps(); }
void
BaHunApp::registerApps()
{
  registerApp(BaHunApp);
}

void
BaHunApp::registerObjects(Factory & factory)
{
  // Materials
  registerMaterial(BAMaterial);

  // DiracKernels
  registerDiracKernel(BAPolyLineSink);
}

void
BaHunApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
