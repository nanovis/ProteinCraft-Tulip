#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>

using namespace std;
using namespace tlp;

class BinderTargetSubgraph : public tlp::Algorithm {
public:
  PLUGININFORMATION("BinderTargetSubgraph", "Roden Luo", "2025-03-21",
                    "Creates a subgraph for chain A-B interactions",
                    "1.0", "ProteinCraft")

  BinderTargetSubgraph(tlp::PluginContext *context) : tlp::Algorithm(context) {
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
    IntegerProperty *prop_position = graph->getProperty<IntegerProperty>("position");
    StringProperty *prop_interaction = graph->getProperty<StringProperty>("interaction");

    // Separate chain A (binder) vs chain B (target)
    vector<node> binder_nodes;
    vector<node> target_nodes;

    for (auto n : graph->getNodes()) {
      string c = prop_chain->getNodeValue(n);
      if (c == "A") {  // binder
        binder_nodes.push_back(n);
      } else if (c == "B") {  // target
        target_nodes.push_back(n);
      }
    }

    // Sort them by residue number
    sort(binder_nodes.begin(), binder_nodes.end(), 
         [&](const node &n1, const node &n2) {
           return prop_position->getNodeValue(n1) < prop_position->getNodeValue(n2);
         });
    sort(target_nodes.begin(), target_nodes.end(),
         [&](const node &n1, const node &n2) {
           return prop_position->getNodeValue(n1) < prop_position->getNodeValue(n2);
         });

    // Identify chain B subset that interacts with chain A
    set<node> interacting_binder_set;
    set<node> interacting_target_set;

    for (auto e : graph->getEdges()) {
      string inter_type = prop_interaction->getEdgeValue(e);
      if (!is_interesting_interaction(inter_type, include_vdw)) {
        continue;
      }

      node n1 = graph->source(e);
      node n2 = graph->target(e);
      string c1 = prop_chain->getNodeValue(n1);
      string c2 = prop_chain->getNodeValue(n2);

      if (c1 == "A" && c2 == "B") {
        interacting_binder_set.insert(n1);
        interacting_target_set.insert(n2);
      } else if (c1 == "B" && c2 == "A") {
        interacting_binder_set.insert(n2);
        interacting_target_set.insert(n1);
      }
    }

    vector<node> interacting_binder_list(interacting_binder_set.begin(), interacting_binder_set.end());
    vector<node> interacting_target_list(interacting_target_set.begin(), interacting_target_set.end());

    sort(interacting_binder_list.begin(), interacting_binder_list.end(),
         [&](const node &n1, const node &n2) {
           return prop_position->getNodeValue(n1) < prop_position->getNodeValue(n2);
         });
    sort(interacting_target_list.begin(), interacting_target_list.end(),
         [&](const node &n1, const node &n2) {
           return prop_position->getNodeValue(n1) < prop_position->getNodeValue(n2);
         });

    // Create or reset subgraph
    Graph *binder_target_sub = graph->getSubGraph("BinderTargetSubgraph");
    if (binder_target_sub == nullptr) {
      binder_target_sub = graph->addSubGraph("BinderTargetSubgraph");
    } else {
      // clear existing content
      for (auto n : binder_target_sub->getNodes()) {
        binder_target_sub->delNode(n);
      }
      for (auto e : binder_target_sub->getEdges()) {
        binder_target_sub->delEdge(e);
      }
    }

    // Add nodes
    set<node> binder_target_nodes(interacting_binder_list.begin(), interacting_binder_list.end());
    binder_target_nodes.insert(interacting_target_list.begin(), interacting_target_list.end());
    for (auto n : binder_target_nodes) {
      binder_target_sub->addNode(n);
    }

    // Add edges - only inter-chain interactions
    int edge_count = 0;
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      if (binder_target_nodes.find(s) != binder_target_nodes.end() &&
          binder_target_nodes.find(t) != binder_target_nodes.end()) {
        string c1 = prop_chain->getNodeValue(s);
        string c2 = prop_chain->getNodeValue(t);
        
        string inter_type = prop_interaction->getEdgeValue(e);
        if (is_interesting_interaction(inter_type, include_vdw)) {
          binder_target_sub->addEdge(e);
          edge_count++;
        }
      }
    }

    if (edge_count == 0) {
      if (pluginProgress) {
        pluginProgress->setError("No edges were added to the subgraph. Check if there are any valid inter-chain interactions.");
      }
      delete binder_target_sub;
      return false;
    }

    // Store the interacting node lists as graph properties for the layout plugin
    binder_target_sub->setAttribute("interacting_binder_list", interacting_binder_list);
    binder_target_sub->setAttribute("interacting_target_list", interacting_target_list);

    // Apply Stress Minimization layout
    // tlp::DataSet layoutParams;
    // layoutParams.set("number of iterations", 5);
    // layoutParams.set("edge costs", 2);
    std::string err;
    LayoutProperty* layout = binder_target_sub->getProperty<LayoutProperty>("viewLayout");
    // binder_target_sub->applyPropertyAlgorithm("Stress Minimization (OGDF)", layout, err, &layoutParams, pluginProgress);

    if (pluginProgress) {
      pluginProgress->setComment("Created subgraph 'BinderTargetSubgraph' for chain A/B interactions.");
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

PLUGIN(BinderTargetSubgraph) 