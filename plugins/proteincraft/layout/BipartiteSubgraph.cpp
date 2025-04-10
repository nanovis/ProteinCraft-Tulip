#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>
#include <tulip/View.h>
#include "../../perspective/GraphPerspective/include/GraphPerspective.h"

using namespace std;
using namespace tlp;

class BipartiteSubgraph : public tlp::Algorithm {
public:
  PLUGININFORMATION("BipartiteSubgraph", "Roden Luo", "2025-03-21",
                    "Creates a subgraph containing only edges between different chains",
                    "1.0", "ProteinCraft")

  BipartiteSubgraph(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<bool>("include VDW", "Include VDW interactions in the subgraph?", "True");
    addInParameter<string>("bipartite property", "Name of the property containing bipartite information", "chain");
  }

  bool check() {
    return true;
  }

  bool run() override {
    // Retrieve user parameters
    bool include_vdw = false;
    string bipartite_property_name = "chain";
    dataSet->get("include VDW", include_vdw);
    dataSet->get("bipartite property", bipartite_property_name);

    // Get properties from graph
    StringProperty *prop_bipartite = graph->getProperty<StringProperty>(bipartite_property_name);
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
    int edge_count = 0;
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      
      string c1 = prop_bipartite->getNodeValue(s);
      string c2 = prop_bipartite->getNodeValue(t);
      
      // Only add edges between different parts
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
          edge_count++;
        }
      }
    }

    if (edge_count == 0) {
      if (pluginProgress) {
        pluginProgress->setError("No edges were added to the subgraph. Check if there are any valid inter-chain interactions.");
      }
      delete inter_chain_sub;
      return false;
    }

    if (pluginProgress) {
      pluginProgress->setComment("Created subgraph 'BipartiteSubgraph' containing only inter-chain interactions.");
    }

    // Apply BipartiteSubgraphLayout to the subgraph
    DataSet parameters;
    string errorMessage;
    if (!inter_chain_sub->applyAlgorithm("BipartiteSubgraphLayout", errorMessage, &parameters, pluginProgress)) {
        pluginProgress->setError("Failed to apply BipartiteSubgraphLayout: " + errorMessage);
        return false;
    }

    // Create Node Link Diagram view
    View* nldv = PluginLister::getPluginObject<View>("Node Link Diagram view");
    if (nldv) {
      nldv->setupUi();
      nldv->setGraph(inter_chain_sub);
      
      // Create DataSet with labelScaled set to true under Display section
      DataSet viewState;
      DataSet displayParams;
      displayParams.set("labelScaled", true);
      viewState.set("Display", displayParams);
      nldv->setState(viewState);

      GraphPerspective* graphPerspective = Perspective::typedInstance<GraphPerspective>();
      if (graphPerspective) {
        graphPerspective->addPanel(nldv);
      }
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