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
#include <tulip/LayoutProperty.h>
#include <tulip/ColorProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/DoubleProperty.h>
#include "../../perspective/GraphPerspective/include/GraphPerspective.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

using namespace std;
using namespace tlp;

/** Import module documentation */
// MyImportModule is just an example

class AF2igUmapImport : public ImportModule {
public:

    // This line is used to pass information about the current plug-in.
    PLUGININFORMATION("AF2igUmapImport",
                      "Roden Luo",
                      "2025/04/10",
                      "Import AF2ig and UMAP files",
                      "1.0",
                      "ProteinCraft")

    // The constructor below has to be defined,
    // it is the right place to declare the parameters
    // needed by the algorithm,
    //   addInParameter<ParameterType>("Name","Help string","Default value");
    // and declare the algorithm dependencies too.
    //   addDependency("name", "version");
    AF2igUmapImport(tlp::PluginContext* context) : ImportModule(context) {
        addInParameter<string>("CSV file", "Path to the CSV file to import", "/home/luod/ProteinCraft/run/4_PD-L1/outs_AF2ig_score.csv");
        addInParameter<string>("graph name", "The name to give to the imported graph", "4_PD-L1");
        addInParameter<string>("UMAP file", "Path to the UMAP coordinates CSV file", "/home/luod/ProteinCraft/run/4_PD-L1/outs_RF_umap.csv");
        addInParameter<double>("scale factor", "Scaling factor for the UMAP coordinates", "1.0");
        addInParameter<Color>("node color", "Color for the nodes in the visualization", "(255, 95, 95, 255)");
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
        string umapFile;
        double scaleFactor;
        Color nodeColor;
        
        dataSet->get("CSV file", csvFile);
        dataSet->get("graph name", graphName);
        dataSet->get("UMAP file", umapFile);
        dataSet->get("scale factor", scaleFactor);
        dataSet->get("node color", nodeColor);

        if (csvFile.empty()) {
            pluginProgress->setError("No CSV file path was provided.");
            return false;
        }

        // Set graph name
        if (!graphName.empty()) {
            graph->setName("AF2igUmap_" + graphName);
        }
        else {
            graph->setName("AF2igUmap");
        }

        // Get standard Tulip properties
        LayoutProperty* viewLayout = graph->getLocalProperty<LayoutProperty>("viewLayout");
        ColorProperty* viewColor = graph->getLocalProperty<ColorProperty>("viewColor");
        StringProperty* viewLabel = graph->getLocalProperty<StringProperty>("viewLabel");
        
        // Create properties for UMAP coordinates
        DoubleProperty* xCoord = graph->getLocalProperty<DoubleProperty>("umapX");
        DoubleProperty* yCoord = graph->getLocalProperty<DoubleProperty>("umapY");

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

        // Import UMAP coordinates if file is provided
        if (!umapFile.empty()) {
            ifstream umapFileStream(umapFile);
            if (umapFileStream.is_open()) {
                // Read UMAP header
                getline(umapFileStream, header);
                vector<string> umapColumns;
                stringstream umapSS(header);
                while (getline(umapSS, item, ',')) {
                    umapColumns.push_back(item);
                }

                // Verify required columns
                bool hasX = false, hasY = false, hasFilename = false;
                for (const auto& col : umapColumns) {
                    if (col == "X") hasX = true;
                    if (col == "Y") hasY = true;
                    if (col == "filename") hasFilename = true;
                }

                if (!hasX || !hasY || !hasFilename) {
                    pluginProgress->setError("UMAP file missing required columns (X, Y, filename)");
                } else {
                    // Process UMAP data
                    while (getline(umapFileStream, line)) {
                        stringstream ss(line);
                        vector<string> values;
                        string value;
                        while (getline(ss, value, ',')) {
                            values.push_back(value);
                        }

                        if (values.size() != umapColumns.size()) continue;

                        // Find the node with matching filename
                        string filename = values[find(umapColumns.begin(), umapColumns.end(), "filename") - umapColumns.begin()];
                        // Remove .pdb extension for matching
                        string baseFilename = filename.substr(0, filename.find(".pdb"));
                        
                        node n;
                        for (auto n : graph->getNodes()) {
                            string description = stringProps["description"]->getNodeValue(n);
                            // Check if description starts with the base filename
                            if (description.find(baseFilename) == 0) {
                                // Set UMAP coordinates
                                double x = stod(values[find(umapColumns.begin(), umapColumns.end(), "X") - umapColumns.begin()]);
                                double y = stod(values[find(umapColumns.begin(), umapColumns.end(), "Y") - umapColumns.begin()]);
                                
                                xCoord->setNodeValue(n, x);
                                yCoord->setNodeValue(n, y);
                                
                                // Position node using scaled coordinates
                                Coord coord(x * scaleFactor, y * scaleFactor, 0.0);
                                viewLayout->setNodeValue(n, coord);
                                
                                // Set node color
                                viewColor->setNodeValue(n, nodeColor);
                                break;
                            }
                        }
                    }
                }
            }
        }

        // Create and configure parallel coordinates view
        View* pcv = PluginLister::getPluginObject<View>("ProteinCraft Parallel Coordinates view");
        if (pcv) {
            pcv->setupUi();
            pcv->setGraph(graph);
            
            DataSet viewState;
            DataSet selectedProps;
            
            // Set selected properties
            selectedProps.set("0", string("binder_aligned_rmsd"));
            selectedProps.set("1", string("pae_interaction"));
            selectedProps.set("2", string("plddt_total"));
            selectedProps.set("3", string("inter_chain_total"));
            selectedProps.set("4", string("inter_chain_without_vdw"));
            selectedProps.set("5", string("binder_target_bonds"));
            selectedProps.set("6", string("binder_target_bonds_largest_component"));
            selectedProps.set("7", string("binder_target_bonds_no_vdw"));
            selectedProps.set("8", string("binder_target_bonds_no_vdw_largest_component"));
            selectedProps.set("9", string("inter_chain_hbond"));
            selectedProps.set("10", string("inter_chain_vdw"));
            selectedProps.set("11", string("inter_chain_other"));
            selectedProps.set("12", string("binder_components_bonds"));
            selectedProps.set("13", string("binder_components_bonds_without_vdw"));

            // Set property order
            DataSet selectedPropsOrder;
            for (int i = 0; i < 14; ++i) {
                selectedPropsOrder.set(to_string(i), i != 0 && i != 1);
            }

            viewState.set("selectedProperties", selectedProps);
            viewState.set("selectedPropertiesOrder", selectedPropsOrder);

            pcv->setState(viewState);
        }

        // Create and configure Node Link Diagram view
        View* nldv = PluginLister::getPluginObject<View>("Node Link Diagram view");
        if (nldv) {
            nldv->setupUi();
            nldv->setGraph(graph);
            nldv->setState(DataSet());
        }

        // Add the panels to the workspace
        GraphPerspective* graphPerspective = Perspective::typedInstance<GraphPerspective>();
        if (graphPerspective) {
            if (pcv) {
                graphPerspective->addPanel(pcv);
            }
            if (nldv) {
                graphPerspective->addPanel(nldv);
            }
            
            if (pcv && nldv) {
                graphPerspective->switchToSplitMode();
            }
        }

        return true;
    }
};
// This second line will be used to register your algorithm in tulip using the information given above.
PLUGIN(AF2igUmapImport)