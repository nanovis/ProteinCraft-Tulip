#include <tulip/TulipPluginHeaders.h>
#include <tulip/GraphTools.h>
#include <tulip/DrawingTools.h>
#include <tulip/TlpTools.h>
#include <QDebug>

using namespace std;
using namespace tlp;

class BipartiteSubgraphLayout : public tlp::Algorithm {
public:
  PLUGININFORMATION("BipartiteSubgraphLayout", "Roden Luo", "2025-03-21",
                    "Applies bipartite layout to chain A-B interactions",
                    "1.0", "ProteinCraft")

  BipartiteSubgraphLayout(tlp::PluginContext *context) : tlp::Algorithm(context) {
    addInParameter<string>("bipartite property", "Name of the property containing bipartite information", "chain");
  }

  bool check() {
    string bipartite_property_name = "chain";
    dataSet->get("bipartite property", bipartite_property_name);

    // Check if the property exists
    if (!graph->existProperty(bipartite_property_name)) {
      if (pluginProgress) {
        pluginProgress->setError("Property '" + bipartite_property_name + "' does not exist.");
      }
      return false;
    }

    // Get the property and check if it has exactly two distinct values
    PropertyInterface* prop = graph->getProperty(bipartite_property_name);
    set<string> bipartite_values;
    
    for (auto n : graph->getNodes()) {
      bipartite_values.insert(prop->getNodeStringValue(n));
    }

    if (bipartite_values.size() != 2) {
      if (pluginProgress) {
        pluginProgress->setError("Property '" + bipartite_property_name + "' must have exactly two distinct values.");
      }
      return false;
    }

    return true;
  }

  bool run() override {
    string bipartite_property_name = "chain";
    dataSet->get("bipartite property", bipartite_property_name);

    // Get the bipartite property
    PropertyInterface* prop = graph->getProperty(bipartite_property_name);
    
    // Get the two distinct values
    set<string> bipartite_values;
    for (auto n : graph->getNodes()) {
      bipartite_values.insert(prop->getNodeStringValue(n));
    }
    
    // Split nodes into two groups based on the property value
    vector<node> group1, group2;
    string value1 = *bipartite_values.begin();
    string value2 = *next(bipartite_values.begin());
    
    for (auto n : graph->getNodes()) {
      if (prop->getNodeStringValue(n) == value1) {
        group1.push_back(n);
      } else {
        group2.push_back(n);
      }
    }

    // Layout the graph
    LayoutProperty *view_layout = graph->getProperty<LayoutProperty>("viewLayout");
    layout_bipartite_graph(graph, group1, group2, view_layout);

    if (pluginProgress) {
      pluginProgress->setComment("Applied bipartite layout to chain interactions.");
    }

    return true;
  }

private:
  void layout_bipartite_graph(Graph *graph, const vector<node> &interacting_binder_list,
                            const vector<node> &interacting_target_list,
                            LayoutProperty *view_layout) {
    float space_x = 1.5f;

    #ifndef NDEBUG
    qDebug() << "Binder nodes:" << interacting_binder_list.size();
    for (const auto& n : interacting_binder_list) {
      qDebug() << "  " << n.id;
    }
    qDebug() << "Target nodes:" << interacting_target_list.size();
    for (const auto& n : interacting_target_list) {
      qDebug() << "  " << n.id;
    }
    #endif

    // First try with target nodes in original order
    for (size_t i = 0; i < interacting_target_list.size(); ++i) {
      view_layout->setNodeValue(interacting_target_list[i], Coord(i * space_x, 0.0f, 0.0f));
    }

    // Place binder nodes (chain A) horizontally at y=3
    for (size_t i = 0; i < interacting_binder_list.size(); ++i) {
      view_layout->setNodeValue(interacting_binder_list[i], Coord(i * space_x, 3.0f, 0.0f));
    }

    // Calculate edge lengths for original orientation
    float orig_length = calculate_edge_lengths(graph, interacting_binder_list, interacting_target_list, view_layout);

    // Try reversed target nodes
    for (size_t i = 0; i < interacting_target_list.size(); ++i) {
      view_layout->setNodeValue(interacting_target_list[i], Coord(i * space_x, 0.0f, 0.0f));
    }

    // Calculate edge lengths for reversed orientation
    float reversed_length = calculate_edge_lengths(graph, interacting_binder_list, interacting_target_list, view_layout);

    // Choose orientation with shorter total edge length
    if (reversed_length < orig_length) {
      // Keep the reversed orientation
    } else {
      // Revert back to original orientation
      for (size_t i = 0; i < interacting_target_list.size(); ++i) {
        view_layout->setNodeValue(interacting_target_list[i], Coord(i * space_x, 0.0f, 0.0f));
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

PLUGIN(BipartiteSubgraphLayout) 