#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>

using namespace std;
using namespace tlp;

class InteractionFilter : public tlp::Algorithm {
public:
  PLUGININFORMATION("InteractionFilter", "Roden Luo", "2025-03-21",
                    "Filters interactions between chains based on interaction type",
                    "1.0", "ProteinCraft")

  InteractionFilter(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<bool>("include_vdw", "Include VDW interactions?", "True");
    addInParameter<string>("subgraph_name", "Name of the subgraph to filter", "ChainInteractionSubgraph");
  }

  bool check() {
    return true;
  }

  bool run() override {
    // Retrieve user parameters
    bool include_vdw = false;
    string subgraph_name = "ChainInteractionSubgraph";
    dataSet->get("include_vdw", include_vdw);
    dataSet->get("subgraph_name", subgraph_name);

    // Get the subgraph
    Graph *subgraph = graph->getSubGraph(subgraph_name);
    if (subgraph == nullptr) {
      if (pluginProgress) {
        pluginProgress->setError("Subgraph '" + subgraph_name + "' not found.");
      }
      return false;
    }

    // Get properties
    StringProperty *prop_chain = graph->getProperty<StringProperty>("chain");
    StringProperty *prop_interaction = graph->getProperty<StringProperty>("interaction");

    // Clear existing edges in subgraph
    for (auto e : subgraph->getEdges()) {
      subgraph->delEdge(e);
    }

    // Get nodes from the subgraph
    set<node> subgraph_nodes;
    for (auto n : subgraph->getNodes()) {
      subgraph_nodes.insert(n);
    }

    // Add filtered edges
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      
      // Check if both nodes are in the subgraph
      if (subgraph_nodes.find(s) != subgraph_nodes.end() &&
          subgraph_nodes.find(t) != subgraph_nodes.end()) {
        
        string c1 = prop_chain->getNodeValue(s);
        string c2 = prop_chain->getNodeValue(t);
        
        // Only add edges between different chains
        if ((c1 == "A" && c2 == "B") || (c1 == "B" && c2 == "A")) {
          string inter_type = prop_interaction->getEdgeValue(e);
          if (is_interesting_interaction(inter_type, include_vdw)) {
            subgraph->addEdge(e);
          }
        }
      }
    }

    if (pluginProgress) {
      pluginProgress->setComment("Filtered interactions in subgraph '" + subgraph_name + "'.");
    }

    return true;
  }

private:
  bool is_interesting_interaction(const string &inter_type, bool include_vdw) {
    if (inter_type.empty()) {
      return false;
    }
    if (inter_type.find("VDW") == 0) {
      return include_vdw;
    }
    return true;
  }
};

PLUGIN(InteractionFilter) 