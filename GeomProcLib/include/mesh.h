#ifndef MESH_H_
#define MESH_H_

#include <glm/glm.hpp>
#include <vector>
#include <map>
#include <set>

namespace GeomProc {

    // Forward declarations (classes and pointers to classes)
    class Vertex;
    class Corner;
    class Face;
    class Mesh;
    typedef class Vertex *VertexPtr;
    typedef class Corner *CornerPtr;
    typedef class Face *FacePtr;

    // Type declarations
    typedef int IdType;
    typedef glm::vec3 PositionType;
    typedef glm::vec3 NormalType;
    typedef float AreaType;
    typedef glm::vec3 ColorType;
    typedef glm::vec2 UVType;
    typedef std::map<IdType, VertexPtr> VertexContainer;
    typedef std::map<IdType, FacePtr> FaceContainer;
    typedef std::vector<FacePtr> FaceInVertexContainer;
    typedef std::set<VertexPtr> VertexResultContainer;
    typedef std::set<FacePtr> FaceResultContainer;
 
    // A vertex
    class Vertex {
        friend class Mesh;

        private:
            IdType id_;
            PositionType position_;
            NormalType normal_;
            ColorType color_;
            UVType uv_;
            FaceInVertexContainer face_;
 
        public:
            // Creation
            Vertex(IdType id);
            //Vertex(Vertex &vertex);

            // Getters and setters
            IdType GetId(void) const;
            PositionType GetPosition(void) const;
            NormalType GetNormal(void) const;
            ColorType GetColor(void) const;
            UVType GetUV(void) const;

            void SetPosition(const PositionType position);
            void SetNormal(const NormalType normal);
            void SetColor(const ColorType color);
            void SetUV(const UVType uv);

            // Face access
            IdType FaceCount(void);
            FacePtr GetFace(IdType id);

            // Face iterator
            class FaceInVertexIterator {
                public:
                    typedef FaceInVertexIterator self_type;
                    typedef FaceInVertexContainer::value_type value_type;
                    typedef FacePtr reference;
                    typedef FacePtr pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;
                    FaceInVertexIterator(void) { }
                    FaceInVertexIterator(FaceInVertexContainer::iterator it) : it_(it) { }
                    self_type operator++() { self_type i = *this; it_++; return i; }
                    self_type operator++(int junk) { it_++; return *this; }
                    self_type operator+(int step) { return it_+step; }
                    reference operator*() { return (*it_); }
                    pointer operator->() { return (*it_); }
                    bool operator==(const self_type& rhs) { return it_ == rhs.it_; }
                    bool operator!=(const self_type& rhs) { return it_ != rhs.it_; }
                private:
                    FaceInVertexContainer::iterator it_;
            };

            FaceInVertexIterator FaceBegin();
            FaceInVertexIterator FaceEnd();
 
            // Add/remove faces
            void AddFace(FacePtr face);
            void RemoveFace(IdType id);
            void RemoveFace(FacePtr face);

            // Connectivity functions
            VertexResultContainer GetNeighbors(void);
    };


    // A corner (a vertex in the context of a face)
    class Corner {
        friend class Mesh;
        friend class Face;

        private:
            VertexPtr vertex_;
            NormalType normal_;
            ColorType color_;
            UVType uv_;

        public:
            // Creation
            Corner(void);
            //Corner(Corner &corner);

            // Getters and setters
            VertexPtr GetVertex(void) const;
            PositionType GetPosition(void) const;
            NormalType GetNormal(void) const;
            ColorType GetColor(void) const;
            UVType GetUV(void) const;

            void SetVertex(const VertexPtr vertex);
            void SetPosition(const PositionType position);
            void SetNormal(const NormalType normal);
            void SetColor(const ColorType color);
            void SetUV(const UVType uv);
    };

    // A corner container
    typedef std::vector<Corner> CornerContainer;

    // A face (triangle)
    class Face {
        friend class Mesh;

        private:
            IdType id_;
            NormalType normal_;
            AreaType area_;
            ColorType color_;
            CornerContainer corner_;
 
            // Add/remove corners
            void AddCorner(const Corner &corner);
            void RemoveCorner(IdType id);

            // Add/remove vertices
            void AddVertex(VertexPtr vertex);
            void RemoveVertex(IdType id);
            void RemoveVertex(VertexPtr vertex);

        public:
            // Creation
            Face(IdType id);
            //Face(Face &face);

            // Getters and setters
            IdType GetId(void) const;
            NormalType GetNormal(void) const;
            AreaType GetArea(void) const;
            ColorType GetColor(void) const;

            void SetNormal(const NormalType normal);
            void SetColor(const ColorType color);

            // Corner access
            IdType CornerCount(void) const;
            Corner GetCorner(IdType id) const;

            // Corner iterator
            class CornerIterator {
                public:
                    typedef CornerIterator self_type;
                    typedef CornerContainer::value_type value_type;
                    typedef Corner *reference;
                    typedef Corner *pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;
                    CornerIterator(void) { }
                    CornerIterator(CornerContainer::iterator it) : it_(it) { }
                    self_type operator++() { self_type i = *this; it_++; return i; }
                    self_type operator++(int junk) { it_++; return *this; }
                    self_type operator+(int step) { return it_+step; }
                    reference operator*() { return &(*it_); }
                    pointer operator->() { return &(*it_); }
                    bool operator==(const self_type& rhs) { return it_ == rhs.it_; }
                    bool operator!=(const self_type& rhs) { return it_ != rhs.it_; }
                private:
                    CornerContainer::iterator it_;
            };

            CornerIterator CornerBegin();
            CornerIterator CornerEnd();
 
            // Vertex access
            IdType VertexCount(void) const;
            VertexPtr GetVertex(IdType id) const;

            // Vertex iterator
            class VertexIterator {
                public:
                    typedef VertexIterator self_type;
                    typedef CornerContainer::value_type value_type;
                    typedef VertexPtr reference;
                    typedef VertexPtr pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;
                    VertexIterator(void) { }
                    VertexIterator(CornerContainer::iterator it) : it_(it) { }
                    self_type operator++() { self_type i = *this; it_++; return i; }
                    self_type operator++(int junk) { it_++; return *this; }
                    self_type operator+(int step) { return it_+step; }
                    reference operator*() { return (*it_).GetVertex(); }
                    pointer operator->() { return (*it_).GetVertex(); }
                    bool operator==(const self_type& rhs) { return it_ == rhs.it_; }
                    bool operator!=(const self_type& rhs) { return it_ != rhs.it_; }
                private:
                    CornerContainer::iterator it_;
            };

            VertexIterator VertexBegin(); 
            VertexIterator VertexEnd();
 
            // Connectivity functions
            FaceResultContainer GetNeighbors(void);

            // Geometry functions
            PositionType GetCentroid(void);
    };
 
    // A mesh
    class Mesh {
        public:
            // Color schemes
            enum ColorScheme { NoColor, VertexColor, FaceColor };

        private:
            // List of vertices
            VertexContainer vertex_;
            // List of faces
            FaceContainer face_;
            // Flags
            bool has_connectivity_;
            bool has_vertex_normals_;
            bool has_face_normals_;
            // Id of last element added
            IdType last_vertex_id_;
            IdType last_face_id_;
            // Color scheme
            ColorScheme color_scheme_;

            void DeletePointers(void);

        public:
            // Constructor and destructor
            Mesh(void);
            Mesh(const Mesh &mesh);
            ~Mesh();

            // Remove all elements from the mesh
            void Clear(void);
     
            // Connectivity-related functions
            void ClearConnectivity(void);
            void ComputeConnectivity(void);

            // Geometry-related functions
            void NormalizePositions(float target_min = -1.0, float target_max = 1.0);
            void ComputeVertexAndFaceNormals(void);
            void ComputeFaceNormals(void);
            void ManuallyAssignVertexNormals(void);
            void ManuallyAssignFaceNormals(void);

            // Attributes manipulation
            void CopyCornerAttributesToVertices(void);
            void CopyCornerNormalsToVertices(void);
            void CopyCornerUVsToVertices(void);
            void CopyVertexAttributesToCorners(void);

            // File I/O
            struct WriteOptions {
                bool write_vertex_normals;
                bool write_face_normals;
                bool write_vertex_colors;
                bool write_vertex_uvs;
                bool write_face_uvs;
                char *texture_name;
                WriteOptions(void) {
                    write_vertex_normals = 0;
                    write_face_normals = 0;
                    write_vertex_colors = 0;
                    write_vertex_uvs = 0;
                    write_face_uvs = 0;
                    texture_name = NULL;
                }
            };
            void Read(const char *filename);
            void ReadObj(const char *filename);
            void ReadOff(const char *filename);

            void Write(const char *filename, const WriteOptions &options = WriteOptions());
            void WriteObj(const char *filename, const WriteOptions &options = WriteOptions());
            void WriteOff(const char *filename, const WriteOptions &options = WriteOptions());

            // Elements
            IdType VertexCount(void) const;
            IdType FaceCount(void) const;
            VertexPtr GetVertex(IdType id) const;
            FacePtr GetFace(IdType id) const;
 
            // Iterators
            class VertexIterator {
                public:
                    typedef VertexIterator self_type;
                    typedef VertexContainer::value_type value_type;
                    typedef VertexPtr reference;
                    typedef VertexPtr pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;
                    VertexIterator(void) { }
                    VertexIterator(VertexContainer::iterator it) : it_(it) { }
                    self_type operator++() { self_type i = *this; it_++; return i; }
                    self_type operator++(int junk) { it_++; return *this; }
                    reference operator*() { return it_->second; }
                    pointer operator->() { return it_->second; }
                    bool operator==(const self_type& rhs) { return it_ == rhs.it_; }
                    bool operator!=(const self_type& rhs) { return it_ != rhs.it_; }
                private:
                    VertexContainer::iterator it_;
            };

            class FaceIterator {
                public:
                    typedef FaceIterator self_type;
                    typedef FaceContainer::value_type value_type;
                    typedef FacePtr reference;
                    typedef FacePtr pointer;
                    typedef std::forward_iterator_tag iterator_category;
                    typedef int difference_type;
                    FaceIterator(void) { }
                    FaceIterator(FaceContainer::iterator it) : it_(it) { }
                    self_type operator++() { self_type i = *this; it_++; return i; }
                    self_type operator++(int junk) { it_++; return *this; }
                    reference operator*() { return it_->second; }
                    pointer operator->() { return it_->second; }
                    bool operator==(const self_type& rhs) { return it_ == rhs.it_; }
                    bool operator!=(const self_type& rhs) { return it_ != rhs.it_; }
                private:
                    FaceContainer::iterator it_;
            };

            // Iterators
            VertexIterator VertexBegin();
            VertexIterator VertexEnd();

            FaceIterator FaceBegin();
            FaceIterator FaceEnd();

            // Add elements to the mesh
            VertexPtr AddVertex(PositionType position = PositionType(0.0, 0.0, 0.0));
            FacePtr AddFace(VertexPtr v1, VertexPtr v2, VertexPtr v3);
            FacePtr AddFace(IdType v1, IdType v2, IdType v3);

            // Remove elements from the mesh
            VertexPtr RemoveVertex(IdType id);
            void RemoveVertex(VertexPtr vertex);
            FacePtr RemoveFace(IdType id);
            void RemoveFace(FacePtr face);
            void ReindexIds(void);

            // Color schemes
            ColorScheme GetColorScheme(void) const;
            void SetColorScheme(ColorScheme color_scheme);
    };

} // namespace GeomProc

#endif // MESH_H_
