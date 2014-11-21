#ifndef _MBSIMFMI_FMI_VARIABLES_IMPL_H_
#define _MBSIMFMI_FMI_VARIABLES_IMPL_H_

#include <mbsim/dynamic_system_solver.h>
#include <mbsimControl/extern_signal_source.h>
#include <mbsimControl/extern_signal_sink.h>
#include <mbsim/extern_generalized_io.h>
#include <boost/algorithm/string/replace.hpp>
#include <boost/ref.hpp>
#include <boost/lexical_cast.hpp>

// local helper functions
namespace {
  //! convert a MBSim path to a FMI structured variable name
  std::string mbsimPathToFMIName(std::string path);

  //! get all links form sys, recursively
  void getAllLinks(const MBSim::DynamicSystem *sys, std::vector<MBSim::Link*> &link);
}

namespace MBSimFMI {

class Variable;

enum PlotMode {
  EachnthCompletetStep = 1,
  SampleTime           = 2
};
//! Struct holding all "hard coded" FMI variables (variables which are not part of the MBSim dss)
struct HardCodedVariables {
  std::string outputDir; // the MBSim output directory //MFMF currently unused
  int plotMode;          // the MBSim plotting mode //MFMF currently unused
  int plotEachNStep;     // plot at each n-th completed integrator step //MFMF currently unused
  double plotStepSize;   // plot in equidistand time steps //MFMF currently unused
};

//! create all FMI variables using a MBSim dss: "hard coded" and dynamic ones from dss
inline void createAllVariables(const MBSim::DynamicSystemSolver *dss, std::vector<boost::shared_ptr<Variable> > &var,
                               HardCodedVariables &hardCodedVar);

//! Type of the variable
enum Type {
  Parameter,
  Input,
  Output
};

//! Abstract base class for all FMI variables
// MISSING: create a exception wrapper with add the VR to all exceptoins throw in by this class
class Variable {
  public:
    //! FMI name
    virtual std::string getName()=0;
    //! FMI description
    virtual std::string getDescription()=0;
    //! Variable type
    virtual Type getType()=0;
    //! FMI variable datatype
    virtual char getDatatype()=0;
    //! get the current value as a string (usable to add it to e.g. XML)
    virtual std::string getValueAsString()=0;
    //! set FMI variable of type real. The default implementation throws, if not overloaded.
    virtual void setValue(const double &v) {
      throw std::runtime_error("Setting this variable is not allowed or is not of type real.");
    }
    //! get FMI variable of type real. The default implementation throws, if not overloaded.
    //! Note: the argument just exists to be able to overload all type with the name function name.
    virtual const double& getValue(const double&) {
      throw std::runtime_error("This variable is not of type real.");
    }
    //! set FMI variable of type integer. The default implementation throws, if not overloaded.
    virtual void setValue(const int &v) {
      throw std::runtime_error("Setting this variable is not allowed or is not of type integer.");
    }
    //! get FMI variable of type integer. The default implementation throws, if not overloaded.
    //! Note: the argument just exists to be able to overload all type with the name function name.
    virtual const int& getValue(const int&) {
      throw std::runtime_error("This variable is not of type integer.");
    }
    //! set FMI variable of type boolean. The default implementation throws, if not overloaded.
    virtual void setValue(const bool &v) {
      throw std::runtime_error("Setting this variable is not allowed or is not of type boolean.");
    }
    //! get FMI variable of type boolean. The default implementation throws, if not overloaded.
    //! Note: the argument just exists to be able to overload all type with the name function name.
    virtual const bool& getValue(const bool&) {
      throw std::runtime_error("This variable is not of type boolean.");
    }
    //! set FMI variable of type string. The default implementation throws, if not overloaded.
    virtual void setValue(const std::string &v) {
      throw std::runtime_error("Setting this variable is not allowed or is not of type string.");
    }
    //! get FMI variable of type string. The default implementation throws, if not overloaded.
    //! Note: the argument just exists to be able to overload all type with the name function name.
    virtual const std::string& getValue(const std::string&) {
      throw std::runtime_error("This variable is not of type string.");
    }
};



//! map c++ type to FMI datatype character
template<typename Type> struct MapTypeToFMIDatatype;
template<> struct MapTypeToFMIDatatype<double     > { static const char value='r'; };
template<> struct MapTypeToFMIDatatype<int        > { static const char value='i'; };
template<> struct MapTypeToFMIDatatype<bool       > { static const char value='b'; };
template<> struct MapTypeToFMIDatatype<std::string> { static const char value='s'; };
//! A FMI parameter which stores the value in a externally provided reference.
//! This is used for "hard coded" variables.
template<typename Datatype>
class ParameterValue : public Variable {
  public:
    ParameterValue(const std::string &name_, const std::string &desc_, Datatype &v) :
      name(name_), desc(desc_), value(v) {}
    std::string getName() { return name; }
    std::string getDescription() { return desc; }
    Type getType() { return Parameter; }
    char getDatatype() { return MapTypeToFMIDatatype<Datatype>::value; }
    std::string getValueAsString() { return boost::lexical_cast<std::string>(value); }
    void setValue(const Datatype &v) { value=v; }
    const Datatype& getValue(const Datatype&) { return value; }
  protected:
    std::string name, desc;
    Datatype &value;
};

//! FMI input variable for the force of MBSim::ExternGeneralizedIO
class ExternGeneralizedIOForce : public Variable {
  public:
    ExternGeneralizedIOForce(MBSim::ExternGeneralizedIO *io_) : io(io_) {}
    std::string getName() { return mbsimPathToFMIName(io->getPath())+".h"; }
    std::string getDescription() { return "ExternGeneralizedIO force"; }
    Type getType() { return Input; }
    char getDatatype() { return 'r'; }
    std::string getValueAsString() { return boost::lexical_cast<std::string>(getValue(double())); }
    void setValue(const double &v) { io->setGeneralizedForce(v); }
    const double& getValue(const double&) { return io->getla()(0); }
  protected:
    MBSim::ExternGeneralizedIO *io;
};

//! FMI output variable for position of MBSim::ExternGeneralizedIO
class ExternGeneralizedIOPosition : public Variable {
  public:
    ExternGeneralizedIOPosition(MBSim::ExternGeneralizedIO *io_) : io(io_) {}
    std::string getName() { return mbsimPathToFMIName(io->getPath())+".x"; }
    std::string getDescription() { return "ExternGeneralizedIO position"; }
    Type getType() { return Output; }
    char getDatatype() { return 'r'; }
    std::string getValueAsString() { return boost::lexical_cast<std::string>(getValue(double())); }
    const double& getValue(const double&) { return io->getGeneralizedPosition(); }
  protected:
    MBSim::ExternGeneralizedIO *io;
};

//! FMI output variable for velocity of MBSim::ExternGeneralizedIO
class ExternGeneralizedIOVelocity : public Variable {
  public:
    ExternGeneralizedIOVelocity(MBSim::ExternGeneralizedIO *io_) : io(io_) {}
    std::string getName() { return mbsimPathToFMIName(io->getPath())+".v"; }
    std::string getDescription() { return "ExternGeneralizedIO velocity"; }
    Type getType() { return Output; }
    char getDatatype() { return 'r'; }
    std::string getValueAsString() { return boost::lexical_cast<std::string>(getValue(double())); }
    const double& getValue(const double&) { return io->getGeneralizedVelocity(); }
  protected:
    MBSim::ExternGeneralizedIO *io;
};

//! FMI input variable for MBSim::ExternSignalSource
class ExternSignalSource : public Variable {
  public:
    ExternSignalSource(MBSimControl::ExternSignalSource *sig_) : sig(sig_) {}
    std::string getName() { return mbsimPathToFMIName(sig->getPath()); }
    std::string getDescription() { return "ExternSignalSource"; }
    Type getType() { return Input; }
    char getDatatype() { return 'r'; }
    std::string getValueAsString() { return boost::lexical_cast<std::string>(getValue(double())); }
    void setValue(const double &v) { sig->setSignal(fmatvec::VecV(1, fmatvec::INIT, v)); }
    const double& getValue(const double&) { value=sig->getSignal()(0); return value; }
  protected:
    MBSimControl::ExternSignalSource *sig;
    double value; // MISSING: replace this varaible if getSignal returns a const reference!!!
};

//! FMI output variable for MBSim::ExternSignalSink
class ExternSignalSink : public Variable {
  public:
    ExternSignalSink(MBSimControl::ExternSignalSink *sig_) : sig(sig_) {}
    std::string getName() { return mbsimPathToFMIName(sig->getPath()); }
    std::string getDescription() { return "ExternSignalSink"; }
    Type getType() { return Output; }
    char getDatatype() { return 'r'; }
    std::string getValueAsString() { return boost::lexical_cast<std::string>(getValue(double())); }
    const double& getValue(const double&) { value=sig->getSignal()(0); return value; }
  protected:
    MBSimControl::ExternSignalSink *sig;
    double value; // MISSING: replace this varaible if getSignal returns a const reference!!!
};

void createAllVariables(const MBSim::DynamicSystemSolver *dss, std::vector<boost::shared_ptr<Variable> > &var,
                        HardCodedVariables &hardCodedVar) {
  // create "hard coded" parameters

  // output directory
  var.push_back(boost::make_shared<ParameterValue<std::string> >("Output directory",
    "MBSim output directory for all files: *.mbsim.h5, *.ombv.h5, *.ombv.xml, ...", boost::ref(hardCodedVar.outputDir)));
  (*--var.end())->setValue(boost::filesystem::temp_directory_path().string()); // default value
  // plot mode
  var.push_back(boost::make_shared<ParameterValue<int> >("Plot.mode",
    "Write to *.mbsim.h5 and *.ombv.h5 files at every ...", boost::ref(hardCodedVar.plotMode)));
  (*--var.end())->setValue(int(1)); // default value
  // plot at each n-th integrator step
  var.push_back(boost::make_shared<ParameterValue<int> >("Plot.each n-th step",
    "... n-th completed integrator step", boost::ref(hardCodedVar.plotEachNStep)));
  (*--var.end())->setValue(int(5)); // default value
  // plot every dt
  var.push_back(boost::make_shared<ParameterValue<double> >("Plot.sample time",
    "... sample point with this sample time", boost::ref(hardCodedVar.plotStepSize)));
  (*--var.end())->setValue(double(0.001)); // default value
  // ADD HERE MORE HARD CODED PARAMETERS

  // create all input/output variables for links in the dss
  std::vector<MBSim::Link*> l;
  getAllLinks(dss, l);
  for(std::vector<MBSim::Link*>::const_iterator it=l.begin(); it!=l.end(); ++it) {
    // for ExternGeneralizedIO create three variables: force input, position output and velocity output
    MBSim::ExternGeneralizedIO *genIO=dynamic_cast<MBSim::ExternGeneralizedIO*>(*it);
    if(genIO) {
      var.push_back(boost::make_shared<ExternGeneralizedIOForce>(genIO));
      (*--var.end())->setValue(double(0.0)); // default value
      var.push_back(boost::make_shared<ExternGeneralizedIOPosition>(genIO));
      var.push_back(boost::make_shared<ExternGeneralizedIOVelocity>(genIO));
    }
    // for ExternSignalSource create one input variable
    MBSimControl::ExternSignalSource *sigSource=dynamic_cast<MBSimControl::ExternSignalSource*>(*it);
    if(sigSource) {
      var.push_back(boost::make_shared<ExternSignalSource>(sigSource));
      (*--var.end())->setValue(double(0.0)); // default value
    }
    // for ExternSignalSink create one output variable
    MBSimControl::ExternSignalSink *sigSink=dynamic_cast<MBSimControl::ExternSignalSink*>(*it);
    if(sigSink) {
      var.push_back(boost::make_shared<ExternSignalSink>(sigSink));
    }
    // ADD HERE MORE MBSIM TYPES WHICH SHOULD BECOME FMI INPUTS/OUTPUTS
  }
}

}

namespace {

  std::string mbsimPathToFMIName(std::string path) {
    boost::replace_all(path, "/", "."); // replace the MBSim separator / by the FMI seperator .
    return path.substr(1); // skip the starting spearotor character
  }

  void getAllLinks(const MBSim::DynamicSystem *sys, std::vector<MBSim::Link*> &link) {
    // add all link of dss
    const std::vector<MBSim::Link*> &l=sys->getLinks();
    link.insert(link.end(), l.begin(), l.end());
  
    // call recursively for all DynamicSystem
    const std::vector<MBSim::DynamicSystem*> &s=sys->getDynamicSystems();
    for(std::vector<MBSim::DynamicSystem*>::const_iterator it=s.begin(); it!=s.end(); ++it)
      getAllLinks(*it, link);
  }

}

#endif