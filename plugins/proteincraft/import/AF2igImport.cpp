/**
 * This file is part of ProteinCraft (https://github.com/nanovis/ProteinCraft)
 *
 * Authors: Roden Luo
 *
 * ProteinCraft builds on top of Tulip.
 *
 */

#include <tulip/TulipPluginHeaders.h>
#include <string>

using namespace std;
using namespace tlp;

/** Import module documentation */
// MyImportModule is just an example

class AF2igImport : public ImportModule {
public:

  // This line is used to pass information about the current plug-in.
  PLUGININFORMATION("AF2igImport",
                    "Roden Luo",
                    "2025/04/10",
                    "Import AF2ig files",
                    "1.0",
                    "ProteinCraft")

  // The constructor below has to be defined,
  // it is the right place to declare the parameters
  // needed by the algorithm,
  //   addInParameter<ParameterType>("Name","Help string","Default value");
  // and declare the algorithm dependencies too.
  //   addDependency("name", "version");
  AF2igImport(tlp::PluginContext* context) : ImportModule(context) {
  }

  // Define the destructor only if needed
  // ~MyImportModule() {
  // }

  // The import method is the starting point of your import module.
  // The returned value must be true if it succeeded.
  bool importGraph() override {
    return true;
  }
};
// This second line will be used to register your algorithm in tulip using the information given above.
PLUGIN(AF2igImport)