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
    addInParameter<StringCollection>("grouping style", "Style for grouping nodes", "elegant;keep order", true,
                                    "elegant<br/>keep order");
    addInParameter<StringCollection>("layout orientation", "Orientation of the bipartite layout", "horizontal;vertical", true,
                                    "horizontal<br/>vertical");
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
    
    // Get the grouping style
    StringCollection grouping_style("elegant;keep order");
    grouping_style.setCurrent(0);
    dataSet->get("grouping style", grouping_style);

    // elegant style first
    for (auto n : graph->getNodes()) {
      if (prop->getNodeStringValue(n) == value1) {
        group1.push_back(n);
      } else {
        group2.push_back(n);
      }
    }
    if (grouping_style.getCurrent() != 0) {
      // keep order style
      // sort group1 and group2 by id
      sort(group1.begin(), group1.end(), [](const node& a, const node& b) {
        return a.id < b.id;
      });
      sort(group2.begin(), group2.end(), [](const node& a, const node& b) {
        return a.id < b.id;
      });
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
  void layout_bipartite_graph(Graph *graph, const vector<node> &group1,
                            const vector<node> &group2,
                            LayoutProperty *view_layout) {
    // Get layout orientation parameter
    StringCollection layout_orientation("horizontal;vertical");
    layout_orientation.setCurrent(0); // Default to horizontal
    dataSet->get("layout orientation", layout_orientation);

    float space = 1.5f;

    #ifndef NDEBUG
    qDebug() << "Group1 nodes:" << group1.size();
    for (const auto& n : group1) {
      qDebug() << "  " << n.id;
    }
    qDebug() << "Group2 nodes:" << group2.size();
    for (const auto& n : group2) {
      qDebug() << "  " << n.id;
    }
    #endif

    if (layout_orientation.getCurrent() == 0) { // Horizontal layout
      // Calculate total width needed
      float total_width = std::max(group1.size(), group2.size()) * space;
      
      // Center the nodes horizontally
      float x_offset = total_width / 2.0f;
      
      // Place group2 nodes horizontally at y=0, centered around x=0
      for (size_t i = 0; i < group2.size(); ++i) {
        view_layout->setNodeValue(group2[i], Coord(i * space - x_offset, 0.0f, 0.0f));
      }

      // Place group1 nodes horizontally at y=3.0, centered around x=0
      for (size_t i = 0; i < group1.size(); ++i) {
        view_layout->setNodeValue(group1[i], Coord(i * space - x_offset, 3.0f, 0.0f));
      }

      // Calculate edge lengths for original orientation
      float orig_length = calculate_edge_lengths(graph, group1, group2, view_layout);

      // Try reversed group2 nodes by placing them in reverse order
      for (size_t i = 0; i < group2.size(); ++i) {
        size_t reversed_i = group2.size() - 1 - i;
        view_layout->setNodeValue(group2[i], Coord(reversed_i * space - x_offset, 0.0f, 0.0f));
      }

      // Calculate edge lengths for reversed orientation
      float reversed_length = calculate_edge_lengths(graph, group1, group2, view_layout);

      // Choose orientation with shorter total edge length
      if (reversed_length >= orig_length) {
        // Revert back to original orientation
        for (size_t i = 0; i < group2.size(); ++i) {
          view_layout->setNodeValue(group2[i], Coord(i * space - x_offset, 0.0f, 0.0f));
        }
      }
    } else { // Vertical layout
      // Calculate total height needed
      float total_height = std::max(group1.size(), group2.size()) * space;
      
      // Center the nodes vertically
      float y_offset = total_height / 2.0f;
      
      // Place group2 nodes vertically at x=0, centered around y=0
      for (size_t i = 0; i < group2.size(); ++i) {
        view_layout->setNodeValue(group2[i], Coord(0.0f, y_offset - i * space, 0.0f));
      }

      // Place group1 nodes vertically at x=3.0, centered around y=0
      for (size_t i = 0; i < group1.size(); ++i) {
        view_layout->setNodeValue(group1[i], Coord(3.0f, y_offset - i * space, 0.0f));
      }

      // Calculate edge lengths for original orientation
      float orig_length = calculate_edge_lengths(graph, group1, group2, view_layout);

      // Try reversed group2 nodes by placing them in reverse order
      for (size_t i = 0; i < group2.size(); ++i) {
        size_t reversed_i = group2.size() - 1 - i;
        view_layout->setNodeValue(group2[i], Coord(0.0f, y_offset - reversed_i * space, 0.0f));
      }

      // Calculate edge lengths for reversed orientation
      float reversed_length = calculate_edge_lengths(graph, group1, group2, view_layout);

      // Choose orientation with shorter total edge length
      if (reversed_length >= orig_length) {
        // Revert back to original orientation
        for (size_t i = 0; i < group2.size(); ++i) {
          view_layout->setNodeValue(group2[i], Coord(0.0f, y_offset - i * space, 0.0f));
        }
      }
    }
  }

  float calculate_edge_lengths(Graph *graph, const vector<node> &group1,
                             const vector<node> &group2, LayoutProperty *layout) {
    float total_length = 0.0f;
    for (auto e : graph->getEdges()) {
      node s = graph->source(e);
      node t = graph->target(e);
      if ((find(group1.begin(), group1.end(), s) != group1.end() &&
           find(group2.begin(), group2.end(), t) != group2.end()) ||
          (find(group1.begin(), group1.end(), t) != group1.end() &&
           find(group2.begin(), group2.end(), s) != group2.end())) {
        Coord s_coord = layout->getNodeValue(s);
        Coord t_coord = layout->getNodeValue(t);
        total_length += (s_coord - t_coord).norm();
      }
    }
    return total_length;
  }
};

PLUGIN(BipartiteSubgraphLayout) 