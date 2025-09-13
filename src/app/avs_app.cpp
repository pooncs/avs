
    #include <iostream>
    #include "avs/graph/graph.hpp"
    #include "avs/graph/executor.hpp"
    #include "avs/graph/registry.hpp"
    #include "avs/nodes/math_nodes.hpp"

    using namespace avs::graph;

    int main(){
      Graph g;
      auto n1 = Registry::instance().create("Counter");
      auto n2 = Registry::instance().create("Add");
      auto n3 = Registry::instance().create("Counter");
      NodeId id1 = g.add_node(n1);
      NodeId id2 = g.add_node(n2);
      NodeId id3 = g.add_node(n3);
      g.add_edge(id1, id2);
      g.add_edge(id2, id3);

      GraphExecutor ex(g, ExecOptions{4, true});
      auto r1 = ex.run();
      auto r2 = ex.run(); // tests persistent state
      if (!r1.ok || !r2.ok){
        std::cerr << "Execution errors
";
        return 1;
      }
      std::cout << "OK
";
      return 0;
    }
