#ifndef GRAPH_DIST_H_
#define GRAPH_DIST_H_

#include <mesh.h>
#include <vector>

namespace GeomProc {

    class GraphDist {
        private:
            Mesh &mesh_;
            void ComputeVertexShortestPath(int source_index);
            void ComputeFaceShortestPath(int source_index);

        public:
            enum FieldType { VertexDist, FaceDist } field_type;
            std::vector<IdType> sources;
            typedef std::vector<float> DistanceField;
            std::vector<DistanceField> dist;

            GraphDist(Mesh &mesh) : mesh_(mesh), field_type(VertexDist) {};
            void ComputeShortestPaths(void);
    };

} // namespace GeomProc

#endif // GRAPH_DIST_H_
