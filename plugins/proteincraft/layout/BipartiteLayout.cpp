#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>
#include <tulip/DrawingTools.h>
#include <tulip/TlpTools.h>

using namespace std;
using namespace tlp;

class BipartiteLayout : public tlp::Algorithm {
public:
  PLUGININFORMATION("BipartiteLayout", "Roden Luo", "2025-03-21",
                    "Creates a subgraph for chain A-B interactions and sets bipartite layout",
                    "1.0", "ProteinCraft")

  BipartiteLayout(tlp::PluginContext *context) : tlp::Algorithm(context) {
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
    Graph *binder_target_sub = graph->getSubGraph("BinderTargetInteraction");
    if (binder_target_sub == nullptr) {
      binder_target_sub = graph->addSubGraph("BinderTargetInteraction");
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
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      if (binder_target_nodes.find(s) != binder_target_nodes.end() &&
          binder_target_nodes.find(t) != binder_target_nodes.end()) {
        string c1 = prop_chain->getNodeValue(s);
        string c2 = prop_chain->getNodeValue(t);
        // Only add edges between different chains
        if ((c1 == "A" && c2 == "B") || (c1 == "B" && c2 == "A")) {
          string inter_type = prop_interaction->getEdgeValue(e);
          if (is_interesting_interaction(inter_type, include_vdw)) {
            binder_target_sub->addEdge(e);
          }
        }
      }
    }

    // Layout the subgraph
    LayoutProperty *sub_view_layout = binder_target_sub->getLocalProperty<LayoutProperty>("viewLayout");
    layout_bipartite_subgraph(graph, interacting_binder_list, interacting_target_list, sub_view_layout);

    if (pluginProgress) {
      pluginProgress->setComment("Created subgraph 'BinderTargetInteraction' with bipartite layout for chain A/B interactions.");
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

  void layout_bipartite_subgraph(Graph *graph, const vector<node> &interacting_binder_list,
                                const vector<node> &interacting_target_list,
                                LayoutProperty *sub_view_layout) {
    float space_x = 1.5f;

    // First try with target nodes in original order
    for (size_t i = 0; i < interacting_target_list.size(); ++i) {
      sub_view_layout->setNodeValue(interacting_target_list[i], Coord(i * space_x, 0.0f, 0.0f));
    }

    // Place binder nodes (chain A) horizontally at y=3
    for (size_t i = 0; i < interacting_binder_list.size(); ++i) {
      sub_view_layout->setNodeValue(interacting_binder_list[i], Coord(i * space_x, 3.0f, 0.0f));
    }

    // Calculate edge lengths for original orientation
    float orig_length = calculate_edge_lengths(graph, interacting_binder_list, interacting_target_list, sub_view_layout);

    // Try reversed target nodes
    for (size_t i = 0; i < interacting_target_list.size(); ++i) {
      sub_view_layout->setNodeValue(interacting_target_list[i], Coord(i * space_x, 0.0f, 0.0f));
    }

    // Calculate edge lengths for reversed orientation
    float reversed_length = calculate_edge_lengths(graph, interacting_binder_list, interacting_target_list, sub_view_layout);

    // Choose orientation with shorter total edge length
    if (reversed_length < orig_length) {
      // Keep the reversed orientation
    } else {
      // Revert back to original orientation
      for (size_t i = 0; i < interacting_target_list.size(); ++i) {
        sub_view_layout->setNodeValue(interacting_target_list[i], Coord(i * space_x, 0.0f, 0.0f));
      }
    }
  }

  float calculate_edge_lengths(Graph *graph, const vector<node> &binder_nodes,
                             const vector<node> &target_nodes, LayoutProperty *layout) {
    float total_length = 0.0f;
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      if ((find(binder_nodes.begin(), binder_nodes.end(), s) != binder_nodes.end() &&
           find(target_nodes.begin(), target_nodes.end(), t) != target_nodes.end()) ||
          (find(binder_nodes.begin(), binder_nodes.end(), t) != binder_nodes.end() &&
           find(target_nodes.begin(), target_nodes.end(), s) != target_nodes.end())) {
        Coord s_coord = layout->getNodeValue(s);
        Coord t_coord = layout->getNodeValue(t);
        total_length += (s_coord - t_coord).norm();
      }
    }
    return total_length;
  }
};

PLUGIN(BipartiteLayout)
