#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>
#include <tulip/DrawingTools.h>
#include <tulip/TlpTools.h>

using namespace std;
using namespace tlp;

class BipartiteLayout : public tlp::Algorithm {
public:
  PLUGININFORMATION("BipartiteLayout", "Roden Luo", "2025-03-21",
                    "Applies bipartite layout to a subgraph of chain A-B interactions",
                    "1.0", "ProteinCraft")

  BipartiteLayout(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<string>("subgraph_name", "Name of the subgraph to layout", "ChainInteractionSubgraph");
  }

  bool check() {
    return true;
  }

  bool run() override {
    // Retrieve user parameter
    string subgraph_name = "ChainInteractionSubgraph";
    dataSet->get("subgraph_name", subgraph_name);

    // Get the subgraph
    Graph *subgraph = graph->getSubGraph(subgraph_name);
    if (subgraph == nullptr) {
      if (pluginProgress) {
        pluginProgress->setError("Subgraph '" + subgraph_name + "' not found. Please run ChainInteractionSubgraph first.");
      }
      return false;
    }

    // Get the stored node lists
    vector<node> interacting_binder_list;
    vector<node> interacting_target_list;
    
    if (!subgraph->getAttribute("interacting_binder_list", interacting_binder_list) ||
        !subgraph->getAttribute("interacting_target_list", interacting_target_list)) {
      if (pluginProgress) {
        pluginProgress->setError("Subgraph '" + subgraph_name + "' does not contain required node lists. Please run ChainInteractionSubgraph first.");
      }
      return false;
    }

    // Layout the subgraph
    LayoutProperty *sub_view_layout = subgraph->getLocalProperty<LayoutProperty>("viewLayout");
    layout_bipartite_subgraph(subgraph, interacting_binder_list, interacting_target_list, sub_view_layout);

    if (pluginProgress) {
      pluginProgress->setComment("Applied bipartite layout to subgraph '" + subgraph_name + "'.");
    }

    return true;
  }

private:
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
