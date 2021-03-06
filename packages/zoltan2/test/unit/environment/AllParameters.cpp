// @HEADER
//
// ***********************************************************************
//
//   Zoltan2: A package of combinatorial algorithms for scientific computing
//                  Copyright 2012 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Karen Devine      (kddevin@sandia.gov)
//                    Erik Boman        (egboman@sandia.gov)
//                    Siva Rajamanickam (srajama@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
//
// Testing parameters.  Serial test.

#include <Zoltan2_config.h>
#include <Zoltan2_Environment.hpp>
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_DefaultComm.hpp>
#include <string>

using namespace std;
using std::cout;
using std::cerr;
using std::endl;
using std::string;

//
// For all parameters:
//   parameter name, a valid value, an invalid value

// FileNameValidator - a number is invalid

#define NUMFN 3
static string fnParams[NUMFN][3]={
  {"debug_output_file", "temp.txt", "5"},
  {"timer_output_file", "timerInfo.txt", "10.3"},
  {"memory_output_file", "memory.txt", "3.33"}
};

// Value value is any integer - a string is invalid

#define NUMANYINT 3 
static string anyIntParams[NUMANYINT][3]={
  {"num_global_parts", "12", "invalid_value"},
  {"num_local_parts", "1", "invalid_value"},
  {"random_seed", "9999", "invalid_value"},
};

// Value value is a particular string

#define NUMSTR 25
static string strParams[NUMSTR][3]={
  {"error_check_level", "basic_assertions", "invalid_assertion_request"},
  {"debug_level", "basic_status", "invalid_status"},
  {"timer_type", "no_timers", "invalid_timers"},
  {"debug_output_stream", "cout", "invalid_stream"},
  {"timer_output_stream", "/dev/null", "invalid_stream"},
  {"memory_output_stream", "cerr", "invalid_stream"},
  {"debug_procs", "all", "not_a_valid_list_of_any_type"},
  {"mj_parts", "2,3,4", "not_a_valid_list_of_any_type"},
  {"memory_procs", "2-10", "not_a_valid_list_of_any_type"},
  {"speed_versus_quality", "balance", "invalid_option"},
  {"memory_versus_speed", "memory", "invalid_option"},
  {"order_method", "rcm", "invalid_method"},
  {"order_package", "amd", "invalid_package"},
  {"compute_metrics", "true", "maybe"},
  {"topology", "2,3,6", "invalid_option"},
  {"randomize_input", "1", "22"},
  {"partitioning_objective", "balance_object_weight", "invalid_objective"},
  {"partitioning_approach", "repartition", "invalid_approach"},
  {"objects_to_partition", "graph_vertices", "invalid_objects"},
  {"model", "graph", "invalid_model"},
  {"algorithm", "rcb", "invalid_algorithm"},
  {"rectilinear", "true", "invalid_option"},
  {"symmetrize_input", "transpose", "invalid_option"},
  {"subset_graph", "1", "invalid_option"},
  {"imbalance_tolerance", "1.1", "invalid_option"}
};


template <typename T>
int testInvalidValue( Teuchos::ParameterList &pl, 
  string paramName, T badValue)
{
  Teuchos::ParameterList validParameters;
  pl.set(paramName, badValue);
  cout << endl;
  cout << paramName << " = " << badValue << endl;

  bool failed = false;
  try{
    Zoltan2::createValidatorList(pl, validParameters);
    pl.validateParametersAndSetDefaults(validParameters);
  }
  catch(std::exception &e){
    cout << "Correctly generated an error:" << endl;
    cout << e.what() << endl;
    failed = true;
  }

  if (!failed){
    cerr << "Bad parameter value was not detected in parameter list." << endl;
    return 1;
  }
  return 0;
}

  // Print out all the documentation

int main(int argc, char *argv[])
{
  Teuchos::GlobalMPISession session(&argc, &argv);
  Teuchos::RCP<const Teuchos::Comm<int> > comm =
    Teuchos::DefaultComm<int>::getComm();

  int rank = comm->getRank();

  if (rank > 0)
    return 0;

  // Create a valid parameter list.

  Teuchos::ParameterList validParameters;
  Teuchos::ParameterList myParams("testParameterList");

  for (int i=0; i < NUMSTR; i++){
    myParams.set(strParams[i][0], strParams[i][1]);
  }

  for (int i=0; i < NUMANYINT; i++){
    istringstream iss(anyIntParams[i][1]);
    int paramValue;
    iss >> paramValue;
    myParams.set(anyIntParams[i][0], paramValue);
  }

  for (int i=0; i < NUMFN; i++){
    myParams.set(fnParams[i][0], fnParams[i][1]);
  }

  Teuchos::ParameterList origParams(myParams);

  // Normally an application would not call this.  The
  // Environment object will validate the entered parameters.

  try{
    Zoltan2::createValidatorList(myParams, validParameters);
    myParams.validateParametersAndSetDefaults(validParameters);
    Zoltan2::Environment::convertStringToInt(myParams);
  }
  catch(std::exception &e){
    std::cerr << "Validate parameters generated an error:" << endl;
    std::cerr << e.what() << endl;
    std::cerr << "FAIL" << endl;
    return 1;
  }

  cout << endl;
  cout << "Parameters after validation: " << endl;
  cout << myParams << endl;

  // Try invalid parameter values

  for (int i=0; i < NUMSTR; i++){
    Teuchos::ParameterList badParams(origParams);
    int fail = 
      testInvalidValue<string>(badParams, strParams[i][0], strParams[i][2]);
    if (fail){
      cout << "FAIL" << endl;
      return 1;
    }
  }

  for (int i=0; i < NUMANYINT; i++){
    Teuchos::ParameterList badParams(origParams);
    int fail = 
       testInvalidValue<string>(badParams, anyIntParams[i][0], anyIntParams[i][2]);
    if (fail){
      cout << "FAIL" << endl;
      return 1;
    }
  }

  for (int i=0; i < NUMFN; i++){
    Teuchos::ParameterList badParams(origParams);
    istringstream iss(fnParams[i][2]);
    double badVal;
    iss >> badVal;
    int fail = 
       testInvalidValue<double>(badParams, fnParams[i][0], badVal);
    if (fail){
      cout << "FAIL" << endl;
      return 1;
    }
  }


  // Print out all the documentation

  cout << endl;
  cout << "Parameter documentation:" << endl;
  Zoltan2::printListDocumentation(validParameters, cout, std::string()); 

  cout << "PASS"  << endl;
  return 0;
}
