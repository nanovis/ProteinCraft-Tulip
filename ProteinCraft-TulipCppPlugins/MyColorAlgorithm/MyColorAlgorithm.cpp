#include <tulip/TulipPluginHeaders.h>
#include <string>

using namespace std;
using namespace tlp;

/** Algorithm documentation */
// MyColorAlgorithm is just an example

class MyColorAlgorithm : public ColorAlgorithm {
public:

  // This line is used to pass information about the current plug-in.
  PLUGININFORMATION("My Color Algorithm",
                    "Deng Luo",
                    "19/03/25",
                    "A few words describing what kind of action the plug-in realizes",
                    "Plug-in version",
                    "Name of the Sub-menu under which the plug-in should be classified")

  // The constructor below has to be defined,
  // it is the right place to declare the parameters
  // needed by the algorithm,
  //   addInParameter<ParameterType>("Name","Help string","Default value");
  // and declare the algorithm dependencies too.
  //   addDependency("name", "version");
  MyColorAlgorithm(const PluginContext* context) : ColorAlgorithm(context) {
  }

  // Define the destructor only if needed
  // ~MyColorAlgorithm() {
  // }

  // Define the check method only if needed.
  // It can be used to check topological properties of the graph,
  // metric properties on graph elements or anything else you need.
  // bool check(std::string & errorMsg) {
  //   errorMsg="";
  //   return true;
  // }

  // The run method is the main method:
  //     - It will be called out if the pre-condition method (bool check (..)) returned true.
  //     - It is the starting point of your algorithm.
  // The returned value must be true if your algorithm succeeded.
  bool run() override {
    return true;
  }
};
// This second line will be used to register your algorithm in tulip using the information given above.
PLUGIN(MyColorAlgorithm)
