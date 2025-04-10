/**
 * This file is part of ProteinCraft (https://github.com/nanovis/ProteinCraft)
 *
 * Authors: Roden Luo
 *
 * ProteinCraft builds on top of Tulip.
 *
 */

#include <tulip/TulipPluginHeaders.h>
#include <tulip/Graph.h>
#include <tulip/PropertyTypes.h>
#include <tulip/View.h>
#include <tulip/DataSet.h>
#include <tulip/PluginProgress.h>
#include <tulip/GraphView.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

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
        addInParameter<string>("CSV file", "Path to the CSV file to import", "/home/luod/ProteinCraft/run/4_PD-L1/outs_AF2ig_score.csv");
        addInParameter<string>("graph name", "The name to give to the imported graph", "4_PD-L1");
    }

    // Define the destructor only if needed
    // ~MyImportModule() {
    // }

    // The import method is the starting point of your import module.
    // The returned value must be true if it succeeded.
    bool importGraph() override {
        // Get parameters
        string csvFile;
        string graphName;
        dataSet->get("CSV file", csvFile);
        dataSet->get("graph name", graphName);

        if (csvFile.empty()) {
            pluginProgress->setError("No CSV file path was provided.");
            return false;
        }

        // Set graph name
        graph->setName("AF2ig_" + graphName);

        // Open CSV file
        ifstream file(csvFile);
        if (!file.is_open()) {
            pluginProgress->setError("Could not open CSV file: " + csvFile);
            return false;
        }

        // Read header
        string header;
        getline(file, header);
        vector<string> columns;
        stringstream ss(header);
        string item;
        while (getline(ss, item, ',')) {
            columns.push_back(item);
        }

        // Create properties
        map<string, DoubleProperty*> numericProps;
        map<string, StringProperty*> stringProps;

        for (const auto& col : columns) {
            if (col == "description") {
                stringProps[col] = graph->getLocalProperty<StringProperty>(col);
            } else {
                numericProps[col] = graph->getLocalProperty<DoubleProperty>(col);
            }
        }

        // Initialize all properties
        for (auto& prop : stringProps) {
            prop.second->setAllNodeValue("");
        }
        for (auto& prop : numericProps) {
            prop.second->setAllNodeValue(0.0);
        }

        // Read data rows
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            vector<string> values;
            string value;
            while (getline(ss, value, ',')) {
                values.push_back(value);
            }

            if (values.size() != columns.size()) {
                continue; // Skip malformed rows
            }

            node n = graph->addNode();

            // Set properties
            for (size_t i = 0; i < columns.size(); ++i) {
                const string& col = columns[i];
                if (col == "description") {
                    stringProps[col]->setNodeValue(n, values[i]);
                } else {
                    try {
                        numericProps[col]->setNodeValue(n, stod(values[i]));
                    } catch (...) {
                        numericProps[col]->setNodeValue(n, 0.0);
                    }
                }
            }
        }

//        // Create and configure parallel coordinates view
//        View* pcv = View::createView("ProteinCraft Parallel Coordinates view", graph);
//        if (pcv) {
//            DataSet viewState;
//
//            // Set selected properties
//            DataSet selectedProps;
//            selectedProps.set("0", "binder_aligned_rmsd");
//            selectedProps.set("1", "pae_interaction");
//            selectedProps.set("2", "plddt_total");
//            selectedProps.set("3", "inter_chain_total");
//            selectedProps.set("4", "inter_chain_without_vdw");
//            selectedProps.set("5", "binder_target_bonds");
//            selectedProps.set("6", "binder_target_bonds_largest_component");
//            selectedProps.set("7", "binder_target_bonds_no_vdw");
//            selectedProps.set("8", "binder_target_bonds_no_vdw_largest_component");
//            selectedProps.set("9", "inter_chain_hbond");
//            selectedProps.set("10", "inter_chain_vdw");
//            selectedProps.set("11", "inter_chain_other");
//            selectedProps.set("12", "binder_components_bonds");
//            selectedProps.set("13", "binder_components_bonds_without_vdw");
//
//            // Set property order
//            DataSet selectedPropsOrder;
//            for (int i = 0; i < 14; ++i) {
//                selectedPropsOrder.set(to_string(i), i != 0 && i != 1);
//            }
//
//            viewState.set("selectedProperties", selectedProps);
//            viewState.set("selectedPropertiesOrder", selectedPropsOrder);
//
//            pcv->setState(viewState);
//        }

        return true;
    }
};
// This second line will be used to register your algorithm in tulip using the information given above.
PLUGIN(AF2igImport)