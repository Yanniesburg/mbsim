// includes
#include <fmiinstance.h>
#include <string>

// include the fmi header
extern "C" {
  #define MODEL_IDENTIFIER mbsim
  #include <3rdparty/fmiModelFunctions.h>
}

// use namespaces
using namespace std;
using namespace MBSimFMI;

// define all FMI function as C functions
extern "C" {

  // global FMI function.
  const char* fmiGetModelTypesPlatform() {
    return "standard32";
  }

  // global FMI function.
  const char* fmiGetVersion() {
    return "1.0";
  }

  // FMI instantiate function: just calls the FMIInstance ctor
  // Convert exceptions to FMI logger calls and return no instance.
  fmiComponent fmiInstantiateModel(fmiString instanceName_, fmiString GUID, fmiCallbackFunctions functions, fmiBoolean loggingOn) {
    try {
      return new FMIInstance(instanceName_, GUID, functions, loggingOn);
    }
    // note: we can not use the instance here since the creation has failed
    catch(const exception &ex) {
      string instanceName=instanceName_; // passing instanceName_ to logger is not allowed acording the FMI standard
      functions.logger(NULL, instanceName.c_str(), fmiError, "error", ex.what());
      return NULL;
    }
    catch(...) {
      string instanceName=instanceName_; // passing instanceName_ to logger is not allowed acording the FMI standard
      functions.logger(NULL, instanceName.c_str(), fmiError, "error", "Unknown error");
      return NULL;
    }
  }

  // FMI free instance function: just calls the FMIInstance dtor.
  // No exception handling needed since the dtor must not throw.
  void fmiFreeModelInstance(fmiComponent c) {
    // must not throw
    delete static_cast<FMIInstance*>(c);
  }

  // All other FMI functions: just calls the corresponding member function in FMIInstance
  // Convert exceptions to call of logError which itself passed these to the FMI logge and return with fmiError.
  #define FMIFUNC(fmiFuncName, instanceMemberName, Sig, sig) \
  fmiStatus fmiFuncName Sig { \
    FMIInstance *instance=static_cast<FMIInstance*>(c); \
    try { \
      instance->instanceMemberName sig; \
      return fmiOK; \
    } \
    catch(const exception &ex) { \
      instance->logException(ex); \
      return fmiError; \
    } \
    catch(...) { \
      instance->logException(runtime_error("Unknwon error")); \
      return fmiError; \
    } \
  }

  // All other FMI function (see above macro)
  FMIFUNC(fmiSetDebugLogging, setDebugLogging,
    (fmiComponent c, fmiBoolean loggingOn),
    (loggingOn))

  FMIFUNC(fmiSetTime, setTime,
    (fmiComponent c, fmiReal time),
    (time))

  FMIFUNC(fmiSetContinuousStates, setContinuousStates,
    (fmiComponent c, const fmiReal x[], size_t nx),
    (x, nx))

  FMIFUNC(fmiCompletedIntegratorStep, completedIntegratorStep,
    (fmiComponent c, fmiBoolean* callEventUpdate),
    (callEventUpdate))

  // all fmiSetXXX function map to the overloaded setValue function

  FMIFUNC(fmiSetReal, setValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiReal value[]),
    <double BOOST_PP_COMMA() fmiReal>(vr, nvr, value))

  FMIFUNC(fmiSetInteger, setValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiInteger value[]),
    <int BOOST_PP_COMMA() fmiInteger>(vr, nvr, value))

  FMIFUNC(fmiSetBoolean, setValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiBoolean value[]),
    <bool BOOST_PP_COMMA() fmiBoolean>(vr, nvr, value))

  FMIFUNC(fmiSetString, setValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, const fmiString value[]),
    <string BOOST_PP_COMMA() fmiString>(vr, nvr, value))

  FMIFUNC(fmiInitialize, initialize,
    (fmiComponent c, fmiBoolean toleranceControlled, fmiReal relativeTolerance, fmiEventInfo* eventInfo),
    (toleranceControlled, relativeTolerance, eventInfo))

  FMIFUNC(fmiGetDerivatives, getDerivatives,
    (fmiComponent c, fmiReal derivatives[], size_t nx),
    (derivatives, nx))

  FMIFUNC(fmiGetEventIndicators, getEventIndicators,
    (fmiComponent c, fmiReal eventIndicators[], size_t ni),
    (eventIndicators, ni))

  // all fmiGetXXX function map to the overloaded getValue function

  FMIFUNC(fmiGetReal, getValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiReal value[]),
    <double BOOST_PP_COMMA() fmiReal>(vr, nvr, value))

  FMIFUNC(fmiGetInteger, getValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiInteger value[]),
    <int BOOST_PP_COMMA() fmiInteger>(vr, nvr, value))

  FMIFUNC(fmiGetBoolean, getValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiBoolean value[]),
    <bool BOOST_PP_COMMA() fmiBoolean>(vr, nvr, value))

  FMIFUNC(fmiGetString, getValue,
    (fmiComponent c, const fmiValueReference vr[], size_t nvr, fmiString value[]),
    <string BOOST_PP_COMMA() fmiString>(vr, nvr, value))

  FMIFUNC(fmiEventUpdate, eventUpdate,
    (fmiComponent c, fmiBoolean intermediateResults, fmiEventInfo* eventInfo),
    (intermediateResults, eventInfo))

  FMIFUNC(fmiGetContinuousStates, getContinuousStates,
    (fmiComponent c, fmiReal states[], size_t nx),
    (states, nx))

  FMIFUNC(fmiGetNominalContinuousStates, getNominalContinuousStates,
    (fmiComponent c, fmiReal x_nominal[], size_t nx),
    (x_nominal, nx))

  FMIFUNC(fmiGetStateValueReferences, getStateValueReferences,
    (fmiComponent c, fmiValueReference vrx[], size_t nx),
    (vrx, nx))

  FMIFUNC(fmiTerminate, terminate,
    (fmiComponent c),
    ())

}