#include "PrariedogApp.h"
#include "Moose.h"
#include "AppFactory.h"
//#include "ModulesApp.h"
#include "MooseSyntax.h"

// App-specific includes
#include "GaussianFunction.h"
#include "RandomPointUserObject.h"
#include "EventInserter.h"
#include "GaussianUserObject.h"
#include "EventTimeStepper.h"
#include "EventInserterSource.h"
#include "EventMarker.h"
#include "CircleAverageMaterialProperty.h"
#include "CircleAverageMaterialPropertyPPS.h"
#include "MaterialDiffusion.h"
#include "MaterialSinkKernel.h"
#include "CircleMaxOriginalElementSize.h"
#include "CircleMaxOriginalElementSizePPS.h"
#include "SinkMapUserObject.h"
#include "SinkMapAux.h"
#include "SinkMapKernel.h"
#include "DiracSinkMapKernel.h"

template<>
InputParameters validParams<PrariedogApp>()
{
  InputParameters params = validParams<MooseApp>();

  params.set<bool>("use_legacy_uo_initialization") = false;
  params.set<bool>("use_legacy_uo_aux_computation") = false;
  params.set<bool>("use_legacy_output_syntax") = false;

  return params;
}

PrariedogApp::PrariedogApp(InputParameters parameters) :
    MooseApp(parameters)
{
  Moose::registerObjects(_factory);
  //ModulesApp::registerObjects(_factory);
  PrariedogApp::registerObjects(_factory);

  Moose::associateSyntax(_syntax, _action_factory);
  //ModulesApp::associateSyntax(_syntax, _action_factory);
  PrariedogApp::associateSyntax(_syntax, _action_factory);
}

PrariedogApp::~PrariedogApp()
{
}

// External entry point for dynamic application loading
extern "C" void PrariedogApp__registerApps() { PrariedogApp::registerApps(); }
void
PrariedogApp::registerApps()
{
  registerApp(PrariedogApp);
}

// External entry point for dynamic object registration
extern "C" void PrariedogApp__registerObjects(Factory & factory) { PrariedogApp::registerObjects(factory); }
void
PrariedogApp::registerObjects(Factory & factory)
{
  registerAuxKernel(SinkMapAux);

  registerDiracKernel(DiracSinkMapKernel);

  registerFunction(GaussianFunction);

  registerKernel(EventInserterSource);
  registerKernel(MaterialDiffusion);
  registerKernel(MaterialSinkKernel);
  registerKernel(SinkMapKernel);

  registerMarker(EventMarker);

  registerPostprocessor(CircleAverageMaterialPropertyPPS);
  registerPostprocessor(CircleMaxOriginalElementSizePPS);

  registerTimeStepper(EventTimeStepper);

  registerUserObject(RandomPointUserObject);
  registerUserObject(EventInserter);
  registerUserObject(GaussianUserObject);
  registerUserObject(CircleAverageMaterialProperty);
  registerUserObject(CircleMaxOriginalElementSize);
  registerUserObject(SinkMapUserObject);
}

// External entry point for dynamic syntax association
extern "C" void PrariedogApp__associateSyntax(Syntax & syntax, ActionFactory & action_factory) { PrariedogApp::associateSyntax(syntax, action_factory); }
void
PrariedogApp::associateSyntax(Syntax & /*syntax*/, ActionFactory & /*action_factory*/)
{
}
