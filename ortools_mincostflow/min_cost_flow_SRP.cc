// Copyright 2018 Google LLC
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ortools/graph/min_cost_flow.h"

#include "ortools/base/logging.h"

#include <iostream>

namespace operations_research {
struct Arc {
  std::pair<NodeIndex, NodeIndex> nodes;
  FlowQuantity capacity;
  FlowQuantity unit_cost;
};

using namespace std;

void SolveMinCostFlow() {

  cout << "Esto esta funcionando... " << endl;
  int vertices, aristas;
  cin >> vertices >> aristas;
  cout << "vertices: " << vertices << endl;
  cout << "aristas: " << aristas << endl;

  StarGraph graph(vertices, aristas);
  MinCostFlow min_cost_flow(&graph);

  int origen, destino, capacidad, costo;
  for (int i=0; i<aristas; i++) {
    cin >> origen >> destino >> capacidad >> costo;
    ArcIndex arc = graph.AddArc(origen, destino);
    min_cost_flow.SetArcCapacity(arc, capacidad);
    min_cost_flow.SetArcUnitCost(arc, costo);
  }

  int supply;
  for (int i=0; i<vertices; i++) {
    cin >> origen >> supply;
    min_cost_flow.SetNodeSupply(origen, supply);
  }

  LOG(INFO) << "Solving min cost flow with: " << graph.num_nodes()
            << " nodes, and " << graph.num_arcs() << " arcs.";

  // Find the maximum flow between node 0 and node 4.
  min_cost_flow.Solve();
  if (MinCostFlow::OPTIMAL != min_cost_flow.status()) {
    LOG(FATAL) << "Solving the max flow is not optimal!";
  }
  FlowQuantity total_flow_cost = min_cost_flow.GetOptimalCost();
  LOG(INFO) << "Minimum cost flow: " << total_flow_cost;
  LOG(INFO) << "";
  LOG(INFO) << "Arc   : Flow / Capacity / Cost";
  for (int i = 0; i < aristas; ++i) {
    LOG(INFO) << graph.Tail(i) << " -> " << graph.Head(i) << ": "
              << min_cost_flow.Flow(i) << " / " << min_cost_flow.Capacity(i)
              << " / " << min_cost_flow.UnitCost(i);
  }
}
}  // namespace operations_research

int main(int argc, char** argv) {
  google::InitGoogleLogging(argv[0]);
  absl::SetFlag(&FLAGS_logtostderr, 1);
  operations_research::SolveMinCostFlow();
  return EXIT_SUCCESS;
}
