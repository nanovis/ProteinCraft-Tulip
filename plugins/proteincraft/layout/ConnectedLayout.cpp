#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>
#include <tulip/DrawingTools.h>
#include <tulip/TlpTools.h>

using namespace std;
using namespace tlp;

class BinderTargetConnectedInteraction : public tlp::Algorithm {
public:
  PLUGININFORMATION("Binder Target Connected Interaction", "Roden Luo", "2025-03-21",
                    "Creates a new subgraph 'BinderTargetConnectedInteraction' with:\n"
                    "1) All interesting (non-covalent, user-chosen) edges between chain A and chain B,\n"
                    "2) Binder–binder edges (A–A) that are NOT included in the 'BinderIntraInteraction'\n"
                    "   subgraphs (i.e., not part of contiguous H/E interactions).\n"
                    "Nodes from chain A or B are added if they appear in at least one such edge;\n"
                    "others are excluded.",
                    "1.0", "ProteinCraft")

  BinderTargetConnectedInteraction(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<bool>("include_vdw", "Include VDW interactions in the subgraph?", "True");
  }

  bool check() {
    return true;
  }

  bool run() override {
    // Retrieve user parameter
    bool include_vdw = false;
    dataSet->get("include_vdw", include_vdw);

    // Find the BinderTargetInteraction subgraph
    Graph *binder_target_interaction_subgraph = nullptr;
    for (auto subgraph : graph->getSubGraphs()) {
      if (subgraph->getName() == "BinderTargetInteraction") {
        binder_target_interaction_subgraph = subgraph;
        break;
      }
    }

    if (!binder_target_interaction_subgraph) {
      if (pluginProgress) {
        pluginProgress->setComment("Could not find the BinderTargetInteraction subgraph.");
      }
      return true;
    }

    // Create a new subgraph
    Graph *binder_target_connected_subgraph = graph->addSubGraph("BinderTargetConnectedInteraction");

    // Copy all nodes and edges from BinderTargetInteraction
    for (auto node : binder_target_interaction_subgraph->getNodes()) {
      binder_target_connected_subgraph->addNode(node);
    }

    for (auto edge : binder_target_interaction_subgraph->getEdges()) {
      binder_target_connected_subgraph->addEdge(edge);
    }

    // Apply Stress Minimization layout
    DataSet params;
    params.set("number of iterations", 5);
    params.set("edge costs", 2);
    LayoutProperty *layout = binder_target_connected_subgraph->getLocalProperty<LayoutProperty>("viewLayout");
    binder_target_connected_subgraph->applyPropertyAlgorithm("Stress Minimization (OGDF)", layout, errMsg, &params, pluginProgress);

    if (pluginProgress) {
      pluginProgress->setComment("Created/updated 'BinderTargetConnectedInteraction' subgraph by copying BinderTargetInteraction.");
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

  bool is_covalent(const string &inter_type) {
    if (inter_type.empty()) {
      return false;
    }
    string upper_type = inter_type;
    transform(upper_type.begin(), upper_type.end(), upper_type.begin(), ::toupper);
    return (inter_type.find("COV") == 0 || upper_type.find("PEPTIDE") != string::npos);
  }
};

PLUGIN(BinderTargetConnectedInteraction)
