// Color each face according to its distance from a source face
// The distance is based on the graph distance on the surface of the mesh

#include <mesh.h>
#include <graph_dist.h>
#include <utils.h>
#include <algorithm>
#include <iostream>

using namespace GeomProc;

int main(int argc, char *argv[]){

    // Handle input arguments
    if (argc < 4){
        std::cout << "test_graph_dist_faces_demo <input mesh filename> <output mesh filename> <source face>" << std::endl;
        return 1;
    }

    // Mesh object
    Mesh mesh;

    // Read the mesh
    mesh.Read(argv[1]);

    // Need connectivity information to retrieve neighbors of vertices
    mesh.ComputeConnectivity();

    // Compute shortest graph distances for source face
    GraphDist gd(mesh);
    gd.field_type = GraphDist::FaceDist;
    gd.sources.push_back(atoi(argv[3]));
    gd.ComputeShortestPaths();

    // Transform distance field into vertex colors
    // Find minimum and maximum distances (for mapping)
    float min_dist = *std::min_element(gd.dist[0].begin(), gd.dist[0].end());
    float max_dist = *std::max_element(gd.dist[0].begin(), gd.dist[0].end());
    // Go through each face of the mesh
    Mesh::FaceIterator fit, fend;
    fit = mesh.FaceBegin();
    fend = mesh.FaceEnd();
    for (; fit != fend; fit++){
        // Map distance to this face to a hue value between 0 and 0.75 (red to blue)
        float h = map_value(max_dist - gd.dist[0][(*fit)->GetId()], 0, 0.75, min_dist, max_dist);
        // Transform hue into an RGB color
        glm::vec3 color = hsv2rgb(glm::vec3(h, 0.8, 0.8));
        // Assign color to vertices of the face
        Face::VertexIterator vit, vend;
        vit = (*fit)->VertexBegin();
        vend = (*fit)->VertexEnd();
        for (; vit != vend; vit++){
            (*vit)->SetColor(color);
        }
    }

    // Write output mesh
    Mesh::WriteOptions options;
    options.write_vertex_colors = 1;
    mesh.Write(argv[2], options);
}
