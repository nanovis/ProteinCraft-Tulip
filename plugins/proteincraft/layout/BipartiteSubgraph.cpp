#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>

using namespace std;
using namespace tlp;

class BipartiteSubgraph : public tlp::Algorithm {
public:
  PLUGININFORMATION("BipartiteSubgraph", "Roden Luo", "2025-03-21",
                    "Creates a subgraph containing only edges between different chains",
                    "1.0", "ProteinCraft")

  BipartiteSubgraph(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<bool>("include_vdw", "Include VDW interactions in the subgraph?", "True");
  }

  bool check() {
    return true;
  }

  bool run() override {
    // Retrieve user parameter
    bool include_vdw = false;
    dataSet->get("include_vdw", include_vdw);

    // Get properties from graph
    StringProperty *prop_chain = graph->getProperty<StringProperty>("chain");
    StringProperty *prop_interaction = graph->getProperty<StringProperty>("interaction");

    // Create or reset subgraph
    Graph *inter_chain_sub = graph->getSubGraph("BipartiteSubgraph");
    if (inter_chain_sub == nullptr) {
      inter_chain_sub = graph->addSubGraph("BipartiteSubgraph");
    } else {
      // clear existing content
      for (auto n : inter_chain_sub->getNodes()) {
        inter_chain_sub->delNode(n);
      }
      for (auto e : inter_chain_sub->getEdges()) {
        inter_chain_sub->delEdge(e);
      }
    }

    // Add nodes and edges - only inter-chain interactions
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      
      string c1 = prop_chain->getNodeValue(s);
      string c2 = prop_chain->getNodeValue(t);
      
      // Only add edges between different chains
      if (c1 != c2) {
        string inter_type = prop_interaction->getEdgeValue(e);
        if (is_interesting_interaction(inter_type, include_vdw)) {
          // Add nodes if not already in subgraph   
          if (!inter_chain_sub->isElement(s)) {
            inter_chain_sub->addNode(s);
          }
          if (!inter_chain_sub->isElement(t)) {
            inter_chain_sub->addNode(t);
          }
          inter_chain_sub->addEdge(e);
        }
      }
    }

    if (pluginProgress) {
      pluginProgress->setComment("Created subgraph 'BipartiteSubgraph' containing only inter-chain interactions.");
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

PLUGIN(BipartiteSubgraph) 