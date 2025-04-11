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
#include <tulip/IntegerProperty.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

using namespace std;
using namespace tlp;

class RingImport : public ImportModule {
public:
    PLUGININFORMATION("RingImport",
                      "Roden Luo",
                      "2025/03/21",
                      "Import RING node and edge files",
                      "1.0",
                      "ProteinCraft")

    RingImport(tlp::PluginContext* context) : ImportModule(context) {
        addInParameter<string>("node file", "Path to the RING node file", "/home/luod/ProteinCraft/run/4_PD-L1/outs_RING/try1_7_dldesign_0_cycle1_af2pred.pdb_ringNodes");
        addInParameter<string>("edge file", "Path to the RING edge file", "/home/luod/ProteinCraft/run/4_PD-L1/outs_RING/try1_7_dldesign_0_cycle1_af2pred.pdb_ringEdges");
    }

    bool importGraph() override {
        // Get parameters
        string nodeFile;
        string edgeFile;
        
        dataSet->get("node file", nodeFile);
        dataSet->get("edge file", edgeFile);

        if (nodeFile.empty() || edgeFile.empty()) {
            pluginProgress->setError("Both node and edge files must be provided.");
            return false;
        }

        // Set graph name
        string nodeFileBaseName = nodeFile.substr(nodeFile.find_last_of("/\\") + 1);
        string fileBase = nodeFileBaseName.substr(0, nodeFileBaseName.find(".pdb_ringNodes"));
        graph->setName("RING_" + fileBase);

        // Create properties
        StringProperty* chainProp = graph->getLocalProperty<StringProperty>("chain");
        IntegerProperty* positionProp = graph->getLocalProperty<IntegerProperty>("position");
        StringProperty* residueProp = graph->getLocalProperty<StringProperty>("residue");
        StringProperty* typeProp = graph->getLocalProperty<StringProperty>("resType");
        StringProperty* dsspProp = graph->getLocalProperty<StringProperty>("dssp");
        IntegerProperty* degreeProp = graph->getLocalProperty<IntegerProperty>("degree");
        DoubleProperty* bfactorProp = graph->getLocalProperty<DoubleProperty>("bfactor_CA");
        StringProperty* pdbFileProp = graph->getLocalProperty<StringProperty>("pdbFileName");
        IntegerProperty* modelProp = graph->getLocalProperty<IntegerProperty>("model");
        StringProperty* nodeIdProp = graph->getLocalProperty<StringProperty>("nodeId");
        DoubleProperty* xProp = graph->getLocalProperty<DoubleProperty>("x");
        DoubleProperty* yProp = graph->getLocalProperty<DoubleProperty>("y");
        DoubleProperty* zProp = graph->getLocalProperty<DoubleProperty>("z");
        LayoutProperty* viewLayout = graph->getLocalProperty<LayoutProperty>("viewLayout");
        StringProperty* viewLabel = graph->getLocalProperty<StringProperty>("viewLabel");
        IntegerProperty* viewShape = graph->getLocalProperty<IntegerProperty>("viewShape");
        ColorProperty* viewColor = graph->getLocalProperty<ColorProperty>("viewColor");

        // Map for storing NodeId -> node mapping
        map<string, node> nodeMap;

        // Amino acid 3-letter to 1-letter mapping
        map<string, string> AA_3TO1 = {
            {"ALA", "A"}, {"ARG", "R"}, {"ASN", "N"}, {"ASP", "D"},
            {"CYS", "C"}, {"GLN", "Q"}, {"GLU", "E"}, {"GLY", "G"},
            {"HIS", "H"}, {"ILE", "I"}, {"LEU", "L"}, {"LYS", "K"},
            {"MET", "M"}, {"PHE", "F"}, {"PRO", "P"}, {"SER", "S"},
            {"THR", "T"}, {"TRP", "W"}, {"TYR", "Y"}, {"VAL", "V"}
        };

        // Read node file
        ifstream nodeStream(nodeFile);
        if (!nodeStream.is_open()) {
            pluginProgress->setError("Could not open node file: " + nodeFile);
            return false;
        }

        // Read header
        string header;
        getline(nodeStream, header);
        vector<string> columns;
        stringstream ss(header);
        string item;
        while (getline(ss, item, '\t')) {
            columns.push_back(item);
        }

        // Get column indices
        map<string, size_t> colIndex;
        for (size_t i = 0; i < columns.size(); ++i) {
            colIndex[columns[i]] = i;
        }

        // Process nodes
        string line;
        while (getline(nodeStream, line)) {
            if (line.empty()) continue;

            vector<string> tokens;
            stringstream ss(line);
            string token;
            while (getline(ss, token, '\t')) {
                tokens.push_back(token);
            }

            if (tokens.size() <= colIndex["Model"]) continue;

            node n = graph->addNode();

            // Set properties
            nodeIdProp->setNodeValue(n, tokens[colIndex["NodeId"]]);
            chainProp->setNodeValue(n, tokens[colIndex["Chain"]]);
            residueProp->setNodeValue(n, tokens[colIndex["Residue"]]);
            typeProp->setNodeValue(n, tokens[colIndex["Type"]]);
            dsspProp->setNodeValue(n, tokens[colIndex["Dssp"]]);
            pdbFileProp->setNodeValue(n, tokens[colIndex["pdbFileName"]]);

            // Convert numeric values
            try {
                positionProp->setNodeValue(n, stoi(tokens[colIndex["Position"]]));
                degreeProp->setNodeValue(n, stoi(tokens[colIndex["Degree"]]));
                bfactorProp->setNodeValue(n, stod(tokens[colIndex["Bfactor_CA"]]));
                double x = stod(tokens[colIndex["x"]]);
                double y = stod(tokens[colIndex["y"]]);
                double z = stod(tokens[colIndex["z"]]);
                modelProp->setNodeValue(n, stoi(tokens[colIndex["Model"]]));

                // Set coordinates
                viewLayout->setNodeValue(n, Coord(x, y, z));
                xProp->setNodeValue(n, x);
                yProp->setNodeValue(n, y);
                zProp->setNodeValue(n, z);
            } catch (...) {
                // Set default values if conversion fails
                positionProp->setNodeValue(n, 0);
                degreeProp->setNodeValue(n, 0);
                bfactorProp->setNodeValue(n, 0.0);
                viewLayout->setNodeValue(n, Coord(0, 0, 0));
                xProp->setNodeValue(n, 0.0);
                yProp->setNodeValue(n, 0.0);
                zProp->setNodeValue(n, 0.0);
                modelProp->setNodeValue(n, 0);
            }

            // Set view label
            string oneLetter = AA_3TO1.count(tokens[colIndex["Residue"]]) ? 
                             AA_3TO1[tokens[colIndex["Residue"]]] : "X";
            viewLabel->setNodeValue(n, to_string(positionProp->getNodeValue(n)) + ":" + oneLetter);

            // Set node shape based on DSSP
            string dssp = tokens[colIndex["Dssp"]];
            if (dssp == "E") {
                viewShape->setNodeValue(n, 18);
            } else if (dssp == "H") {
                viewShape->setNodeValue(n, 14);
            } else {
                viewShape->setNodeValue(n, 15);
            }

            // Set node color based on chain
            string chain = tokens[colIndex["Chain"]];
            if (chain == "A") {
                viewColor->setNodeValue(n, Color(129, 109, 249, 255));
            } else if (chain == "B") {
                viewColor->setNodeValue(n, Color(251, 134, 134, 255));
            }

            // Store node mapping
            nodeMap[tokens[colIndex["NodeId"]]] = n;
        }

        // Create edges between consecutive residues
        map<string, map<int, node>> chainNodes;
        for (const auto& [nodeId, n] : nodeMap) {
            string chain = chainProp->getNodeValue(n);
            int pos = positionProp->getNodeValue(n);
            chainNodes[chain][pos] = n;
        }

        for (const auto& [chain, positions] : chainNodes) {
            vector<int> sortedPositions;
            for (const auto& [pos, _] : positions) {
                sortedPositions.push_back(pos);
            }
            sort(sortedPositions.begin(), sortedPositions.end());

            for (size_t i = 0; i < sortedPositions.size() - 1; ++i) {
                int pos1 = sortedPositions[i];
                int pos2 = sortedPositions[i + 1];
                if (pos2 == pos1 + 1) {
                    node n1 = positions.at(pos1);
                    node n2 = positions.at(pos2);
                    edge e = graph->addEdge(n1, n2);
                    viewColor->setEdgeValue(e, Color(20, 20, 20, 255));
                }
            }
        }

        // Read edge file
        ifstream edgeStream(edgeFile);
        if (!edgeStream.is_open()) {
            pluginProgress->setError("Could not open edge file: " + edgeFile);
            return false;
        }

        // Read header
        getline(edgeStream, header);
        columns.clear();
        ss = stringstream(header);
        while (getline(ss, item, '\t')) {
            columns.push_back(item);
        }

        // Get column indices
        colIndex.clear();
        for (size_t i = 0; i < columns.size(); ++i) {
            colIndex[columns[i]] = i;
        }

        // Create edge properties
        StringProperty* interactionProp = graph->getLocalProperty<StringProperty>("interaction");
        DoubleProperty* distanceProp = graph->getLocalProperty<DoubleProperty>("distance");
        DoubleProperty* angleProp = graph->getLocalProperty<DoubleProperty>("angle");
        StringProperty* atom1Prop = graph->getLocalProperty<StringProperty>("atom1");
        StringProperty* atom2Prop = graph->getLocalProperty<StringProperty>("atom2");
        StringProperty* donorProp = graph->getLocalProperty<StringProperty>("donor");
        StringProperty* positiveProp = graph->getLocalProperty<StringProperty>("positive");
        StringProperty* cationProp = graph->getLocalProperty<StringProperty>("cation");
        StringProperty* orientationProp = graph->getLocalProperty<StringProperty>("orientation");
        IntegerProperty* edgeModelProp = graph->getLocalProperty<IntegerProperty>("edgeModel");

        // Process edges
        while (getline(edgeStream, line)) {
            if (line.empty()) continue;

            vector<string> tokens;
            stringstream ss(line);
            string token;
            while (getline(ss, token, '\t')) {
                tokens.push_back(token);
            }

            if (tokens.size() <= colIndex["Model"]) continue;

            string node1Id = tokens[colIndex["NodeId1"]];
            string node2Id = tokens[colIndex["NodeId2"]];

            if (nodeMap.count(node1Id) == 0 || nodeMap.count(node2Id) == 0) continue;

            node n1 = nodeMap[node1Id];
            node n2 = nodeMap[node2Id];
            edge e = graph->addEdge(n1, n2);

            // Set edge properties
            interactionProp->setEdgeValue(e, tokens[colIndex["Interaction"]]);
            atom1Prop->setEdgeValue(e, tokens[colIndex["Atom1"]]);
            atom2Prop->setEdgeValue(e, tokens[colIndex["Atom2"]]);
            donorProp->setEdgeValue(e, tokens[colIndex["Donor"]]);
            positiveProp->setEdgeValue(e, tokens[colIndex["Positive"]]);
            cationProp->setEdgeValue(e, tokens[colIndex["Cation"]]);
            orientationProp->setEdgeValue(e, tokens[colIndex["Orientation"]]);

            // Set edge color based on interaction type
            string interaction = tokens[colIndex["Interaction"]];
            if (interaction.find("COV") == 0) {
                viewColor->setEdgeValue(e, Color(20, 20, 20, 255));
            } else if (interaction.find("VDW") == 0) {
                viewColor->setEdgeValue(e, Color(180, 180, 180, 255));
            } else if (interaction.find("HBOND") == 0) {
                viewColor->setEdgeValue(e, Color(61, 119, 176, 255));
            } else {
                viewColor->setEdgeValue(e, Color(255, 28, 77, 255));
            }

            // Convert numeric values
            try {
                distanceProp->setEdgeValue(e, stod(tokens[colIndex["Distance"]]));
                angleProp->setEdgeValue(e, stod(tokens[colIndex["Angle"]]));
                edgeModelProp->setEdgeValue(e, stoi(tokens[colIndex["Model"]]));
            } catch (...) {
                distanceProp->setEdgeValue(e, 0.0);
                angleProp->setEdgeValue(e, 0.0);
                edgeModelProp->setEdgeValue(e, 0);
            }
        }

        return true;
    }
};

PLUGIN(RingImport)
