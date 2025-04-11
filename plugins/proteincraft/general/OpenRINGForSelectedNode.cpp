#include <tulip/Algorithm.h>
#include <tulip/BooleanProperty.h>
#include <tulip/StringProperty.h>
#include <tulip/PluginProgress.h>
#include <QDebug>

using namespace tlp;

class OpenRINGForSelectedNode : public tlp::Algorithm {
public:
    PLUGININFORMATION("Open RING For Selected Node",
                     "Roden Luo",
                     "04/04/2025",
                     "Opens a RING graph for the selected node",
                     "1.0",
                     "ProteinCraft")

    OpenRINGForSelectedNode(tlp::PluginContext* context) : tlp::Algorithm(context) {
        addInParameter<std::string>("dir::base path",
                                   "Base path for RING files",
                                   "/home/luod/ProteinCraft/run/4_PD-L1/outs_RING");
    }

    bool check(std::string& errMsg) override {
        BooleanProperty* viewSelection = graph->getProperty<BooleanProperty>("viewSelection");
        StringProperty* description = graph->getProperty<StringProperty>("description");
        
        // Count selected nodes
        std::vector<node> selected_nodes;
        for (auto n : graph->nodes()) {
            if (viewSelection->getNodeValue(n)) {
                selected_nodes.push_back(n);
            }
        }
        
        if (selected_nodes.size() != 1) {
            errMsg = "Please select exactly one node.";
            return false;
        }
        
        // Check if the selected node has a description
        if (description->getNodeValue(selected_nodes[0]).empty()) {
            errMsg = "Selected node must have a description property.";
            return false;
        }
        
        return true;
    }

    bool run() override {
        std::string base_path = "/home/luod/ProteinCraft/run/4_PD-L1/outs_RING";
        if (dataSet != nullptr) {
            dataSet->get("dir::base path", base_path);
        }
        
        BooleanProperty* viewSelection = graph->getProperty<BooleanProperty>("viewSelection");
        StringProperty* description = graph->getProperty<StringProperty>("description");
        
        // Get the selected node
        node selected_node;
        for (auto n : graph->nodes()) {
            if (viewSelection->getNodeValue(n)) {
                selected_node = n;
                break;
            }
        }
        
        std::string filename = description->getNodeValue(selected_node);
        
        // Construct paths for RING files
        std::string node_file = base_path + "/" + filename + ".pdb_ringNodes";
        std::string edge_file = base_path + "/" + filename + ".pdb_ringEdges";

#ifndef NDEBUG
        qDebug() << "node_file: " << node_file.c_str();
        qDebug() << "edge_file: " << edge_file.c_str();
#endif

        // Create a new graph for the RING data
        Graph* ringGraph = newGraph();
        
        // Set up parameters for RingImport
        DataSet params;
        params.set("file::node file", node_file);
        params.set("file::edge file", edge_file);

        // Import the RING data
        std::string errMsg;
        bool success = tlp::importGraph("RingImport", params, pluginProgress, ringGraph);
        
        if (!success) {
            if (pluginProgress) {
                pluginProgress->setError(errMsg);
            }
            delete ringGraph;
            return false;
        }
        
        return true;
    }
};

PLUGIN(OpenRINGForSelectedNode)
