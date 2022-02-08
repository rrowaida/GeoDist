#include <graph_dist.h>
#include <glm/geometric.hpp>
#include <queue>
#include <utility>
#include <cmath>
// For debug
#include <iostream>


namespace GeomProc {


void GraphDist::ComputeVertexShortestPath(int source_index){

    // Check index of source
    if ((source_index < 0) || (source_index >= mesh_.VertexCount())){
        throw(std::ios_base::failure(std::string("Invalid source index")));
    }

    // Initialize distance field with infinite values
    dist.push_back(std::vector<float>(mesh_.VertexCount(), INFINITY));

    // Compute shortest path for source vertex

    // Initialize priority queue
    // Pair composed of distance, vertex
    typedef std::pair<float, VertexPtr> FieldPair;

    // Priority queue for using Dijkstra's algorithm efficiently
    std::priority_queue< FieldPair, std::vector <FieldPair>, std::greater<FieldPair> > pq; 
  
    // Get source to be processed
    IdType source_id = sources[source_index];
    VertexPtr source = mesh_.GetVertex(source_id);

    // Insert source to priority queue and initialize its distance to 0
    pq.push(std::make_pair(0.0, source)); 
    dist[source_index][source_id] = 0.0; 
  
    // Process the priority queue
    while (!pq.empty()){ 
        // Extract top element and its id
        VertexPtr current = pq.top().second; 
        pq.pop(); 
        int current_id = current->GetId();
  
        // Go through the neighbors of the top element
        VertexResultContainer neigh = current->GetNeighbors();
        VertexResultContainer::iterator nit, nend;
        nit = neigh.begin();
        nend = neigh.end();
        for (; nit != nend; nit++){
            // Get neighbor vertex and its id
            VertexPtr n = (*nit);
            int n_id = n->GetId();

            // Compute distance between current and its neighbor
            float weight = glm::distance(current->GetPosition(), n->GetPosition());
  
            //  Check if there is a shorter path to n through current
            if (dist[source_index][n_id] > dist[source_index][current_id] + weight){ 
                // Update distance of n
                dist[source_index][n_id] = dist[source_index][current_id] + weight; 
                pq.push(std::make_pair(dist[source_index][n_id], n)); 
            } 
        } 
    } 
}


void GraphDist::ComputeFaceShortestPath(int source_index){

    // Check index of source
    if ((source_index < 0) || (source_index >= mesh_.FaceCount())){
        throw(std::ios_base::failure(std::string("Invalid source index")));
    }

    // Initialize distance field with infinite values
    dist.push_back(std::vector<float>(mesh_.FaceCount(), INFINITY));

    // Compute shortest path for source face

    // Initialize priority queue
    // Pair composed of distance, face
    typedef std::pair<float, FacePtr> FieldPair;

    // Priority queue for using Dijkstra's algorithm efficiently
    std::priority_queue< FieldPair, std::vector <FieldPair>, std::greater<FieldPair> > pq; 
  
    // Get source to be processed
    IdType source_id = sources[source_index];
    FacePtr source = mesh_.GetFace(source_id);

    // Insert source to priority queue and initialize its distance to 0
    pq.push(std::make_pair(0.0, source)); 
    dist[source_index][source_id] = 0.0; 
  
    // Process the priority queue
    while (!pq.empty()){ 
        // Extract top element and its id
        FacePtr current = pq.top().second; 
        pq.pop(); 
        int current_id = current->GetId();
  
        // Go through the neighbors of the top element
        FaceResultContainer neigh = current->GetNeighbors();
        FaceResultContainer::iterator nit, nend;
        nit = neigh.begin();
        nend = neigh.end();
        for (; nit != nend; nit++){
            // Get neighbor vertex and its id
            FacePtr n = (*nit);
            int n_id = n->GetId();

            // Compute distance between current and its neighbor
            float weight = glm::distance(current->GetCentroid(), n->GetCentroid());
  
            //  Check if there is a shorter path to n through current
            if (dist[source_index][n_id] > dist[source_index][current_id] + weight){ 
                // Update distance of n
                dist[source_index][n_id] = dist[source_index][current_id] + weight; 
                pq.push(std::make_pair(dist[source_index][n_id], n)); 
            } 
        } 
    } 

}


void GraphDist::ComputeShortestPaths(void){

    // Reset distance fields
    dist.clear();

    // Compute distance field for each source
    if (field_type == VertexDist){
        for (int i = 0; i < sources.size(); i++){
            ComputeVertexShortestPath(i);
        }
    } else if (field_type == FaceDist){
        for (int i = 0; i < sources.size(); i++){
            ComputeFaceShortestPath(i);
        }
    } else {
        throw(std::ios_base::failure(std::string("Invalid field type")));
    }
}


} // namespace GeomProc
