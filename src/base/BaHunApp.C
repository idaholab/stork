#include "BaHunApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"

// Materials
#include "BAMaterial.h"

// DiracKernels
#include "BAPolyLineSink.h"

// UserObjects
#include "BAQuantity.h"

// Postprocessors
#include "BAPlotQuantity.h"
#include "BAPiecewiseLinearSinkFlux.h"
#include "BAHalfCubicSinkFlux.h"

// BCs
#include "BAPiecewiseLinearSink.h"
#include "BAHalfCubicSink.h"

// AuxKernels
#include "BATransverseDirectionAux.h"

template<>
InputParameters validParams<BaHunApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  return params;
}

BaHunApp::BaHunApp(InputParameters parameters) :
    MooseApp(parameters)
{

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

  // UserObjects
  registerUserObject(BAQuantity);

  // Postprocessors
  registerPostprocessor(BAPlotQuantity);
  registerPostprocessor(BAPiecewiseLinearSinkFlux);
  registerPostprocessor(BAHalfCubicSinkFlux);

  // BCs
  registerBoundaryCondition(BAPiecewiseLinearSink);
  registerBoundaryCondition(BAHalfCubicSink);

  // AuxKernels
  registerAuxKernel(BATransverseDirectionAux);
}

void
BaHunApp::associateSyntax(Syntax & syntax, ActionFactory & action_factory)
{
}
