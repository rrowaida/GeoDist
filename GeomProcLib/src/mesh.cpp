#include <mesh.h>
#include <model_loading.h>
#include <utils.h>
#include <algorithm>
#include <string>
#include <exception>
#include <fstream>
#include <sstream>
// For debug
#include <iostream>


namespace GeomProc {


Vertex::Vertex(IdType id){
    id_ = id;
}


IdType Vertex::GetId(void) const {
    return id_;
}


PositionType Vertex::GetPosition(void) const {
    return position_;
}


NormalType Vertex::GetNormal(void) const {
    return normal_;
}


ColorType Vertex::GetColor(void) const {
    return color_;
}


UVType Vertex::GetUV(void) const {
    return uv_;
}


void Vertex::SetPosition(const PositionType position){
    position_ = position;
}


void Vertex::SetNormal(const NormalType normal){
    normal_ = normal;
}


void Vertex::SetColor(const ColorType color){
    color_ = color;
}


void Vertex::SetUV(const UVType uv){
    uv_ = uv;
}

// Face access
IdType Vertex::FaceCount(void){

    return face_.size();
}


FacePtr Vertex::GetFace(IdType id){

    return face_[id];
}


Vertex::FaceInVertexIterator Vertex::FaceBegin() {
    return FaceInVertexIterator(face_.begin());
}


Vertex::FaceInVertexIterator Vertex::FaceEnd() {
    return FaceInVertexIterator(face_.end());
}


// Add/remove faces
void Vertex::AddFace(FacePtr face){

    face_.push_back(face);
}


void Vertex::RemoveFace(IdType id){

    face_.erase(face_.begin() + id);
}


void Vertex::RemoveFace(FacePtr face){
    
    FaceInVertexContainer::iterator it;
    it = std::find(face_.begin(), face_.end(), face);
    if (it != face_.end()){
        face_.erase(it);
    }
}


VertexResultContainer Vertex::GetNeighbors(void){

    // Result with the vertices in the one-ring
    VertexResultContainer result;

    // Go through faces around this vertex
    FaceInVertexContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Go through vertices in the current face
        Face::VertexIterator vit, vend;
        vit = (*fit)->VertexBegin();
        vend = (*fit)->VertexEnd();
        for (; vit != vend; vit++){
            // If vertex is not itself
            if ((*vit) != this){
                // Add vertex to result
                // The set makes sure that each element is only added once
                result.insert(*vit);
            }
        }
    }

    return result;
}


Corner::Corner(void) {

    vertex_ = NULL;
}


VertexPtr Corner::GetVertex(void) const {

    return vertex_;
}


PositionType Corner::GetPosition(void) const {

    return vertex_->GetPosition();
}


NormalType Corner::GetNormal(void) const {

    return normal_;
}


ColorType Corner::GetColor(void) const {

    return color_;
}


UVType Corner::GetUV(void) const {

    return uv_;
}


void Corner::SetVertex(const VertexPtr vertex){

    vertex_ = vertex;
}


void Corner::SetPosition(const PositionType position){

    vertex_->SetPosition(position);
}


void Corner::SetNormal(const NormalType normal){

    normal_ = normal;
}


void Corner::SetColor(const ColorType color){

    color_ = color;
}


void Corner::SetUV(const UVType uv){

    uv_ = uv;
}


Face::Face(IdType id){

    id_ = id;
    area_ = 0.0;
}


IdType Face::GetId(void) const {

    return id_;
}


NormalType Face::GetNormal(void) const {

    return normal_;
}


AreaType Face::GetArea(void) const {

    return area_;
}


ColorType Face::GetColor(void) const {

    return color_;
}


void Face::SetNormal(const NormalType normal){

    normal_ = normal;
}


void Face::SetColor(const ColorType color){

    color_ = color;
}


IdType Face::CornerCount(void) const {

    return corner_.size();
}


Corner Face::GetCorner(IdType id) const {

    return corner_[id];
}


Face::CornerIterator Face::CornerBegin() {

    return CornerIterator(corner_.begin());
}


Face::CornerIterator Face::CornerEnd() {

    return CornerIterator(corner_.end());
}


IdType Face::VertexCount(void) const {

    return corner_.size();
}


VertexPtr Face::GetVertex(IdType id) const {

    return corner_[id].GetVertex();
}


Face::VertexIterator Face::VertexBegin() {

    return VertexIterator(corner_.begin());
}


Face::VertexIterator Face::VertexEnd() {

    return VertexIterator(corner_.end());
}


bool ShareAnEdge(FacePtr face1, FacePtr face2){

    // Check if the two faces share an edge

    // Go through edges of face1
    Face::VertexIterator vit1, vend1;
    vit1 = face1->VertexBegin();
    vend1 = face1->VertexEnd();
    for (; vit1 != vend1; vit1++){
        // Get two vertices that form the current edge of face1
        VertexPtr v1a = *(vit1);
        VertexPtr v1b = NULL;
        if ((vit1 + 1) == vend1){
            v1b = *(face1->VertexBegin());
        } else {
            v1b = *(vit1 + 1);
        }
        // Go through vertices of face2
        Face::VertexIterator vit2, vend2;
        vit2 = face2->VertexBegin();
        vend2 = face2->VertexEnd();
        for (; vit2 != vend2; vit2++){
            // Get two vertices that form the current edge of face2
            // Note that for face2 we need to traverse in clockwise
            // order (the opposite order of face1), otherwise the
            // extremities of the edges won't align with each other
            VertexPtr v2a = NULL;
            VertexPtr v2b = *(vit2);
            if ((vit2 + 1) == vend2){
                v2a = *(face2->VertexBegin());
            } else {
                v2a = *(vit2 + 1);
            }
            if ((v1a == v2a) && (v1b == v2b)){
                return true;
            }
        }
    }
        
    return false;
}


FaceResultContainer Face::GetNeighbors(void){

    // Result with the neighboring faces
    FaceResultContainer result;

    // Go through all the vertices in the face
    VertexIterator vit, vend;
    vit = VertexBegin();
    vend = VertexEnd();
    for (; vit != vend; vit++){
        // Go through all faces in the current vertex
        Vertex::FaceInVertexIterator fit, fend;
        fit = (*vit)->FaceBegin();
        fend = (*vit)->FaceEnd();
        for (; fit != fend; fit++){
            // If the face is not itself
            if ((*fit) != this){
                if (ShareAnEdge(*fit, this)){
                    // Add face  to result
                    // The set makes sure that each element is only added once
                    result.insert(*fit);
                }
            }
        }
    }

    return result;
}


PositionType Face::GetCentroid(void){

    // Initialize centroid
    PositionType centroid(0.0, 0.0, 0.0);

    // Go through all the vertices in the face
    VertexIterator vit, vend;
    vit = VertexBegin();
    vend = VertexEnd();
    int count = 0;
    for (; vit != vend; vit++, count++){
        centroid += (*vit)->GetPosition();
    }
    centroid /= ((float) count);

    return centroid;
}


void Face::AddCorner(const Corner &corner){

    corner_.push_back(corner);
}


void Face::RemoveCorner(IdType id){

    corner_.erase(corner_.begin() + id);
}


//void Face::RemoveCorner(CornerPtr corner){
//
//    CornerContainer::iterator cit;
//    cit = std::find(corner_.begin(), corner_.end(), corner);
//    if (cit != corner_.end()){
//        CornerPtr temp = (*cit);
//        corner_.erase(cit);
//        delete temp;
//    }
//}


void Face::AddVertex(VertexPtr vertex){

    Corner corner;
    corner.vertex_ = vertex;
    corner_.push_back(corner);
}


void Face::RemoveVertex(IdType id){

    corner_.erase(corner_.begin() + id);
}


void Face::RemoveVertex(VertexPtr vertex){

    CornerContainer::iterator cit, cend;
    cit = corner_.begin();
    cend = corner_.end();
    for (; cit != cend; cit++){
        if ((*cit).vertex_ == vertex){
            corner_.erase(cit);
            break;
        }
    }
}


Mesh::Mesh(void){

    last_vertex_id_ = -1;
    last_face_id_ = -1;

    has_connectivity_ = false;
    has_vertex_normals_ = false;
    has_face_normals_ = false;

    color_scheme_ = NoColor;
}


Mesh::Mesh(const Mesh &mesh){

    // Copy basic variables
    has_connectivity_ = mesh.has_connectivity_;
    has_vertex_normals_ = mesh.has_vertex_normals_;
    has_face_normals_ = mesh.has_face_normals_;
    last_vertex_id_ = mesh.last_vertex_id_;
    last_face_id_ = mesh.last_face_id_;
    color_scheme_ = mesh.color_scheme_;

    // Copy all vertices and faces from one mesh to the other (deep copy)

    // Copy all vertices from one mesh to the other while allocating new
    // classes
    VertexContainer::iterator vit, vend;
    vit = vertex_.begin();
    vend = vertex_.end();
    for (; vit != vend; vit++){
        VertexPtr vertex = new Vertex(*((*vit).second));
        vertex_.insert(VertexContainer::value_type(vertex->id_, vertex));
    }
 
    // Copy all faces from one mesh to the other
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        FacePtr face = new Face(*((*fit).second));
        face_.insert(FaceContainer::value_type(face->id_, face));
    }
 
    // Fix connectivity references
    ClearConnectivity();
    ComputeConnectivity();
}


void Mesh::DeletePointers(void){

    // Delete all pointers manually allocated
    VertexContainer::iterator vit, vend;
    vit = vertex_.begin();
    vend = vertex_.end();
    for (; vit != vend; vit++){
        VertexPtr vertex = new Vertex(*((*vit).second));
        delete vertex;
    }
 
    // Copy all faces from one mesh to the other
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        FacePtr face = new Face(*((*fit).second));
        delete face;
    }
}


Mesh::~Mesh(){

    // Remove all vertices and faces that we allocated manually
    // All the rest is taken care by the destructors of the subclasses
    DeletePointers();
}


void Mesh::Clear(void){

    // Delete data
    DeletePointers();
    vertex_.clear();
    face_.clear();

    // Reset variables
    last_vertex_id_ = -1;
    last_face_id_ = -1;

    has_connectivity_ = false;
    has_vertex_normals_ = false;
    has_face_normals_ = false;

    color_scheme_ = NoColor;
}


void Mesh::ClearConnectivity(void){

    // Loop through all vertices in the mesh and clean the pointers to
    // faces
    VertexContainer::iterator vit, vend;
    vit = vertex_.begin();
    vend = vertex_.end();
    for (; vit != vend; vit++){
        VertexPtr vertex = (*vit).second;
        vertex->face_.clear();
    }

    // Set flag
    has_connectivity_ = false;
}


void Mesh::ComputeConnectivity(void){

    // Check flag
    if (has_connectivity_){
        ClearConnectivity();
    }

    // Compute connectivity
    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        FacePtr face = (*fit).second;
        // Loop through all vertices of this face
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            // Add face to the list of faces of the vertex
            (*cit).vertex_->AddFace(face);
        }
    }

    // Set flag
    has_connectivity_ = true;
}


void Mesh::NormalizePositions(float target_min, float target_max){

    // Normalize the vertex coordinates of a mesh into a cube with
    // coordinates (target_min, target_max) along each dimension, while
    // preserving the aspect ratio of the shape
    //
    // For each coordinate x, y, and z, we obtain a range of values to
    // map, where the range for coordinate i is given by range_i = max_i
    // - min_i, where max_i is the maximum value for coordinate i for
    // all points, and min_i is defined analogously. Then, we would like
    // to map the coordinate with the largest range range_max to
    // (target_min, target_max), and map the other coordinates i by
    // keeping the proportion of range_i to range_max, so that the
    // aspect ratio of the shape is maintained.
    //
    // Thus, we would like to map a specific range_i from (max_i -
    // min_i) to (target_min*p, target_max*p), where p is the proportion
    // of range_i to the largest range, given by range_i/range_max. This
    // mapping is illustrated with the following diagram, where r is the
    // resulting mapped value:
    //
    // min_i     target_min*p        
    //   |        |
    // coord_i    r         
    //   |        |
    // max_i      target_max*p       
    //
    // This mapping problem gives the following ratio equality:
    //
    // coord_i - min_i    r -target_min*p
    // ---------------  = ------------
    // max_i - min_i      (target_max - target_min)*p
    //
    // By multiplying the fractions, and plugging in the definition of
    // p, we get the equation:
    //
    //     coord_i*(target_max - target_min) -min_i*target_max + max_i*target_min
    // r = ----------------------------------------------------------------------
    //                                 (max - min)

    // Get min and max coordinates of all vertices
    VertexContainer::iterator vit, vend;
    vit = vertex_.begin();
    vend = vertex_.end();
    VertexPtr v = (*vit).second;
    glm::vec3 min_pos = v->position_;
    glm::vec3 max_pos = v->position_;
    vit++;
    for (; vit != vend; vit++){
        v = (*vit).second;
        for (int i = 0; i < 3; i++){
            if (v->position_[i] < min_pos[i]){
                min_pos[i] = v->position_[i];
            }
            if (v->position_[i] > max_pos[i]){
                max_pos[i] = v->position_[i];
            }
        }
    }

    // Compute constants used for mapping
    glm::vec3 diff = max_pos - min_pos;
    float range = glm::max(diff[0], glm::max(diff[1], diff[2]));
    float mult_const = target_max - target_min;
    glm::vec3 add_const = -min_pos*target_max + max_pos*target_min;

    // Map vertex coordinates to new range
    vit = vertex_.begin();
    vend = vertex_.end();
    for (; vit != vend; vit++){
        v = (*vit).second;
        v->position_ = (v->position_*mult_const + add_const)/range;
    }
}


void Mesh::ComputeVertexAndFaceNormals(void){

    // Reset all vertex normals, if needed
    if (has_vertex_normals_){
        // Loop through all vertices
        VertexContainer::iterator vit, vend;
        vit = vertex_.begin();
        vend = vertex_.end();
        for (; vit != vend; vit++){
            VertexPtr vertex = (*vit).second;
            vertex->normal_ = glm::vec3(0.0, 0.0, 0.0);
        }
    }

    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Get this face
        FacePtr face = (*fit).second;

        // Get three vertices of the face
        VertexPtr v0 = face->GetVertex(0);
        VertexPtr v1 = face->GetVertex(1);
        VertexPtr v2 = face->GetVertex(2);

        // Compute face normal and area
        glm::vec3 vec0 = v1->position_ - v0->position_;
        glm::vec3 vec1 = v2->position_ - v0->position_;
        glm::vec3 n = glm::cross(vec0, vec1);
        float len = glm::length(n);
        float area = len/2.0;
        n = n / len;

        // Assign face normal and area
        face->normal_ = n;
        face->area_ = area;

        // Add face normal to normal of vertices
        v0->normal_ += area*n;
        v1->normal_ += area*n;
        v2->normal_ += area*n;
    }

    // Normalize normal vectors of vertices
    VertexContainer::iterator vit, vend;
    vit = vertex_.begin();
    vend = vertex_.end();
    for (; vit != vend; vit++){
        VertexPtr vertex = (*vit).second;
        vertex->normal_ /= glm::length(vertex->normal_);
    }

    // Set flags
    has_face_normals_ = true;
    has_vertex_normals_ = true;
}


void Mesh::ComputeFaceNormals(void){

    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Get this face
        FacePtr face = (*fit).second;

        // Get three vertices of the face
        VertexPtr v0 = face->GetVertex(0);
        VertexPtr v1 = face->GetVertex(1);
        VertexPtr v2 = face->GetVertex(2);

        // Compute face normal and area
        glm::vec3 vec0 = v1->position_ - v0->position_;
        glm::vec3 vec1 = v2->position_ - v0->position_;
        glm::vec3 n = glm::cross(vec0, vec1);
        float len = glm::length(n);
        float area = len/2.0;
        n = n / len;

        // Assign normal and area
        face->normal_ = n;
        face->area_ = area;
    }

    // Set flag
    has_face_normals_ = true;
}


void Mesh::ManuallyAssignVertexNormals(void){

    has_vertex_normals_ = true;
}


void Mesh::ManuallyAssignFaceNormals(void){

    has_face_normals_ = true;
}


void Mesh::CopyCornerAttributesToVertices(void){

    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Get this face
        FacePtr face = (*fit).second;

        // Loop through corners
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            (*cit).vertex_->normal_ = (*cit).normal_;
            (*cit).vertex_->color_ = (*cit).color_;
            (*cit).vertex_->uv_ = (*cit).uv_;
        }
    }
}


void Mesh::CopyCornerNormalsToVertices(void){

    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Get this face
        FacePtr face = (*fit).second;

        // Loop through corners
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            (*cit).vertex_->normal_ = (*cit).normal_;
        }
    }
}


void Mesh::CopyCornerUVsToVertices(void){

    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Get this face
        FacePtr face = (*fit).second;

        // Loop through corners
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            (*cit).vertex_->uv_ = (*cit).uv_;
        }
    }
}


void Mesh::CopyVertexAttributesToCorners(void){

    // Loop through all faces in the mesh
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        // Get this face
        FacePtr face = (*fit).second;

        // Loop through corners
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            (*cit).normal_ = (*cit).vertex_->normal_;
            (*cit).color_ = (*cit).vertex_->color_;
            (*cit).uv_ = (*cit).vertex_->uv_;
        }
    }
}


void Mesh::Read(const char *filename){

    std::string fn = std::string(filename); 
    std::string ext = get_extension(fn);
    if ((ext == std::string("obj")) ||
        (ext == std::string("OBJ"))){
        ReadObj(filename);
    } else if ((ext == std::string("off")) ||
               (ext == std::string("OFF"))){
        ReadOff(filename);
    } else {
        throw(std::ios_base::failure(std::string("Error: extension \"")+ext+std::string("\" not supported")));
    }
}


void Mesh::ReadObj(const char *filename){

    // Open file and check for errors
    std::ifstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Parse file
    // Empty space that can be trimmed from the beginning and end of lines
    std::string ignore(" \t\r\n");
    // Parameter separators in commands (lines)
    std::string part_separator(" \t");
    // Separator of face attributes
    std::string att_separator("/");

    // Since a face in the obj format references lists of vertices,
    // normals, and texture coordinates, we first need to store these
    // lits to allow for proper indexing

    // Temporary holder for faces
    std::vector<TempFace> faces;
    // Temporary holder for normals
    std::vector<NormalType> normals;
    // Temporary holder for uvs
    std::vector<UVType> uvs;

    // Go through each line of the file
    std::string line;
    while (std::getline(f, line)){
        // Trim whitespace from the extremities of the string
        string_trim(line, ignore);
        // Ignore lines that are comments
        if ((line.size() <= 0) ||
            (line[0] == '#')){
            continue;
        }
        // Parse string
        std::vector<std::string> part = string_split(line, part_separator);
        // Check command in the string
        if (!part[0].compare(std::string("v"))){
            if (part.size() == 4){
                PositionType position(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
                AddVertex(position);
            } else if (part.size() == 7){
                glm::vec3 position(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
                glm::vec3 color(str_to_num<float>(part[4].c_str()), str_to_num<float>(part[5].c_str()), str_to_num<float>(part[6].c_str()));
                VertexPtr vertex = AddVertex(position);
                vertex->SetColor(color);
                color_scheme_ = VertexColor;
            } else {
                throw(std::ios_base::failure(std::string("Error: v command should have exactly 3 or 6 parameters")));
            }
        } else if (!part[0].compare(std::string("vn"))){
            if (part.size() == 4){
                glm::vec3 normal(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()), str_to_num<float>(part[3].c_str()));
                normals.push_back(normal);
            } else {
                throw(std::ios_base::failure(std::string("Error: vn command should have exactly 3 parameters")));
            }
        } else if (!part[0].compare(std::string("vt"))){
            if (part.size() == 3){
                glm::vec2 tex_coord(str_to_num<float>(part[1].c_str()), str_to_num<float>(part[2].c_str()));
                uvs.push_back(tex_coord);
            } else {
                throw(std::ios_base::failure(std::string("Error: vt command should have exactly 2 parameters")));
            }
        } else if (!part[0].compare(std::string("f"))){
            if (part.size() >= 4){
                if (part.size() > 5){
                    throw(std::ios_base::failure(std::string("Error: f commands with more than 4 vertices not supported")));
                } else if (part.size() == 5){
                    // Read a quad
                    TempQuad quad;
                    for (int i = 0; i < 4; i++){
                        std::vector<std::string> fd = string_split_one_sep(part[i+1], att_separator);
                        if (fd.size() == 1){
                            quad.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            quad.t[i] = -1;
                            quad.n[i] = -1;
                        } else if (fd.size() == 2){
                            quad.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            quad.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            quad.n[i] = -1;
                        } else if (fd.size() == 3){
                            quad.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            if (std::string("").compare(fd[1]) != 0){
                                quad.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            } else {
                                quad.t[i] = -1;
                            }
                            quad.n[i] = str_to_num<float>(fd[2].c_str())-1;
                        } else {
                            throw(std::ios_base::failure(std::string("Error: f parameter should have 1 or 3 parameters separated by '/'")));
                        }
                    }
                    // Break the quad into two triangles
                    TempFace face1, face2;
                    face1.i[0] = quad.i[0]; face1.i[1] = quad.i[1]; face1.i[2] = quad.i[2];
                    face1.n[0] = quad.n[0]; face1.n[1] = quad.n[1]; face1.n[2] = quad.n[2];
                    face1.t[0] = quad.t[0]; face1.t[1] = quad.t[1]; face1.t[2] = quad.t[2];
                    face2.i[0] = quad.i[0]; face2.i[1] = quad.i[2]; face2.i[2] = quad.i[3];
                    face2.n[0] = quad.n[0]; face2.n[1] = quad.n[2]; face2.n[2] = quad.n[3];
                    face2.t[0] = quad.t[0]; face2.t[1] = quad.t[2]; face2.t[2] = quad.t[3];
                    // Add triangles to temporary list
                    faces.push_back(face1);
                    faces.push_back(face2);
                } else if (part.size() == 4){
                    // Read a triangle
                    TempFace face;
                    for (int i = 0; i < 3; i++){
                        std::vector<std::string> fd = string_split_one_sep(part[i+1], att_separator);
                        if (fd.size() == 1){
                            face.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            face.t[i] = -1;
                            face.n[i] = -1;
                        } else if (fd.size() == 2){
                            face.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            face.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            face.n[i] = -1;
                        } else if (fd.size() == 3){
                            face.i[i] = str_to_num<float>(fd[0].c_str())-1;
                            if (std::string("").compare(fd[1]) != 0){
                                face.t[i] = str_to_num<float>(fd[1].c_str())-1;
                            } else {
                                face.t[i] = -1;
                            }
                            face.n[i] = str_to_num<float>(fd[2].c_str())-1;
                        } else {
                            throw(std::ios_base::failure(std::string("Error: f parameter should have 1, 2, or 3 parameters separated by '/'")));
                        }
                    }
                    // Add triangle to temporary list
                    faces.push_back(face);
                }
            } else {
                throw(std::ios_base::failure(std::string("Error: f command should have 3 or 4 parameters")));
            }
        }
        // Ignore other commands
    }

    // Close file
    f.close();

    // Create faces from temporary faces and copy attributes
    for (unsigned int i = 0; i < faces.size(); i++){
        // Check if vertex references in face are correct
        for (int j = 0; j < 3; j++){
            if (faces[i].i[j] >= VertexCount()){
                throw(std::ios_base::failure(std::string("Error: index for triangle ")+num_to_str<int>(faces[i].i[j])+std::string(" is out of bounds")));
            }
        }
        // Add face
        FacePtr face = AddFace(faces[i].i[0], faces[i].i[1], faces[i].i[2]);
        // Add normals
        if (normals.size() > 0){
            Face::CornerIterator cit, cend;
            cit = face->CornerBegin();
            cend = face->CornerEnd();
            for (int j = 0; cit != cend; cit++, j++){
                if (faces[i].n[j] >= normals.size()){
                    throw(std::ios_base::failure(std::string("Error: index for normal ")+num_to_str<int>(faces[i].n[j])+std::string(" in face ")+num_to_str<int>(i)+std::string(" is out of bounds")));
                } else {
                    (*cit)->SetNormal(normals[faces[i].n[j]]);
                }
            }
        }
        // Add texture coordinates
        if (uvs.size() > 0){
            Face::CornerIterator cit, cend;
            cit = face->CornerBegin();
            cend = face->CornerEnd();
            for (int j = 0; cit != cend; cit++, j++){
                if (faces[i].t[j] >= uvs.size()){
                    throw(std::ios_base::failure(std::string("Error: index for texture coordinate ")+num_to_str<int>(faces[i].t[j])+std::string(" in face ")+num_to_str<int>(i)+std::string(" is out of bounds")));
                } else {
                    (*cit)->SetUV(uvs[faces[i].t[j]]);
                }
            }
        }
    }

    // Copy face attributes to vertices, if needed
    if ((normals.size() > 0) || (uvs.size() > 0)){
        CopyCornerNormalsToVertices();
        CopyCornerUVsToVertices();
        if (normals.size() > 0){
            has_vertex_normals_ = true;
            has_face_normals_ = true;
        }
    }
}


void Mesh::ReadOff(const char *filename){

    // Open file and check for errors
    std::ifstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Read header
    // Read file format identifier
    std::string line;
    std::getline(f, line);
    if (line.substr(0, 3) != "OFF"){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)+std::string(". File does not have the OFF identifier")));
    }

    // Read mesh attributes
    if (f.eof()){
        throw(std::ios_base::failure(std::string("Error: file too short")));
    }
    std::getline(f, line);
    std::istringstream header(line);
    IdType num_vertices, num_faces, num_edges;
    header >> num_vertices >> num_faces >> num_edges;

    // Read vertices
    for (int i = 0; i < num_vertices; i++){
        if (f.eof()){
            throw(std::ios_base::failure(std::string("Error: file too short")));
        }
        std::getline(f, line);
        std::istringstream ss(line);
        PositionType pos;
        ss >> pos[0] >> pos[1] >> pos[2];
        AddVertex(pos);
    }

    // Read faces
    for (int i = 0; i < num_faces; i++){
        if (f.eof()){
            throw(std::ios_base::failure(std::string("Error: file too short")));
        }
        std::getline(f, line);
        std::istringstream ss(line);
        IdType vcount, v0, v1, v2, v3;
        ss >> vcount;
        if (vcount == 3){
            ss >> v0 >> v1 >> v2;
            if ((v0 >= VertexCount()) || (v1 >= VertexCount()) || (v2 >= VertexCount())){
                throw(std::ios_base::failure(std::string("Error: index of vertex in face "+num_to_str<int>(i)+std::string(" out of range"))));
            }
            AddFace(v0, v1, v2);
        } else if (vcount == 4){
            ss >> v0 >> v1 >> v2 >> v3;
            if ((v0 >= VertexCount()) || (v1 >= VertexCount()) || (v2 >= VertexCount()) || (v3 >= VertexCount())){
                throw(std::ios_base::failure(std::string("Error: index of vertex in face "+num_to_str<int>(i)+std::string(" out of range"))));
            }

            AddFace(v0, v1, v2);
            AddFace(v0, v2, v3);
        } else {
            throw(std::ios_base::failure(std::string("Error: faces need to have exactly 3 or 4 vertices")));
        }
    }

    // Close file
    f.close();
}


void Mesh::Write(const char *filename, const WriteOptions &options){

    std::string fn = std::string(filename); 
    std::string ext = get_extension(fn);
    if ((ext == std::string("obj")) ||
        (ext == std::string("OBJ"))){
        WriteObj(filename, options);
    } else if ((ext == std::string("off")) ||
               (ext == std::string("OFF"))){
        WriteOff(filename, options);
    } else {
        throw(std::ios_base::failure(std::string("Error: extension \"")+ext+std::string("\" not supported")));
    }
}


void Mesh::WriteObj(const char *filename, const WriteOptions &options){

    // Open file and check for errors
    std::ofstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }

    // Check flag consistency
    if (options.write_vertex_normals && options.write_face_normals){
        throw(std::ios_base::failure(std::string("Error: cannot specify both write_vertex_normals and write_face_normals")));
    }
    if (options.write_vertex_uvs && options.write_face_uvs){
        throw(std::ios_base::failure(std::string("Error: cannot specify both write_vertex_uvs and write_face_uvs")));
    }
    
    // Create material file, if needed
    if ((options.write_vertex_uvs || options.write_face_uvs) && (options.texture_name != NULL)){
        // Write material file with texture name
        std::string material_filename = get_basename(filename) + std::string(".mtl");
        std::ofstream mf;
        mf.open(material_filename.c_str());
        if (mf.fail()){
            throw(std::ios_base::failure(std::string("Error opening file ")+std::string(material_filename)));
        }
        mf << "newmtl textured" << std::endl;
        mf << "Ka 1.000 1.000 1.000" << std::endl;
        mf << "Kd 1.000 1.000 1.000" << std::endl;
        mf << "Ks 0.000 0.000 0.000" << std::endl;
        mf << "Ns 10.0" << std::endl;
        mf << "d 1.0" << std::endl;
        mf << "illum 0" << std::endl;
        mf << "map_Kd " << options.texture_name << std::endl;
        mf.close(); 
        // Request obj file to use material
        f << "mtllib " << material_filename << std::endl;
        f << "usemtl textured" << std::endl;
    }

    // Write vertices
    VertexIterator vit, vend;
    vit = VertexBegin();
    vend = VertexEnd();
    if (options.write_vertex_colors){
        // Write vertex positions with colors
        for (; vit != vend; vit++){
            VertexPtr vertex = (*vit);
            PositionType pos = vertex->GetPosition();
            ColorType color = vertex->GetColor();
            f << "v " << pos[0] << " " << pos[1] << " " << pos[2] << " " << color[0] << " " << color[1] << " " << color[2] << std::endl;
        }
    } else {
        // Write only vertex positions
        for (; vit != vend; vit++){
            VertexPtr vertex = (*vit);
            PositionType pos = vertex->GetPosition();
            f << "v " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
        }
    }

    // Write list of texture coordinates
    if (options.write_vertex_uvs){
        VertexIterator vit, vend;
        vit = VertexBegin();
        vend = VertexEnd();
        for (; vit != vend; vit++){
            UVType uv = (*vit)->GetUV();
            f << "vt " << uv[0] << " " << uv[1] << std::endl;
        }
    } else if (options.write_face_uvs){
        FaceIterator fit, fend;
        fit = FaceBegin();
        fend = FaceEnd();
        for (; fit != fend; fit++){
            Face::CornerIterator cit, cend;
            cit = (*fit)->CornerBegin();
            cend = (*fit)->CornerEnd();
            for (; cit != cend; cit++){
                UVType uv = (*cit)->GetUV();
                f << "vt " << uv[0] << " " << uv[1] << std::endl;
            }
        }
    }

    // Write list of normals
    if (options.write_vertex_normals){
        VertexIterator vit, vend;
        vit = VertexBegin();
        vend = VertexEnd();
        for (; vit != vend; vit++){
            NormalType normal = (*vit)->GetNormal();
            f << "vn " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
        }
    } else if (options.write_face_normals){
        FaceIterator fit, fend;
        fit = FaceBegin();
        fend = FaceEnd();
        for (; fit != fend; fit++){
            Face::CornerIterator cit, cend;
            cit = (*fit)->CornerBegin();
            cend = (*fit)->CornerEnd();
            for (; cit != cend; cit++){
                NormalType normal = (*cit)->GetNormal();
                f << "vn " << normal[0] << " " << normal[1] << " " << normal[2] << std::endl;
            }
        }
    }

    // Write faces
    FaceIterator fit, fend;
    fit = FaceBegin();
    fend = FaceEnd();
    if ((!options.write_vertex_normals) &&
        (!options.write_face_normals) && 
        (!options.write_vertex_uvs) && 
        (!options.write_face_uvs)){
        // Write only vertex indices
        for (; fit != fend; fit++){
            // Get the current face
            FacePtr face = (*fit);
            // Write face
            f << "f " << face->GetVertex(0)->GetId()+1 << " " << face->GetVertex(1)->GetId()+1 << " " << face->GetVertex(2)->GetId()+1 << std::endl;
        }
    } else {
        // Write more attributes
        // Check which attribute separators we need to write
        std::string sep1, sep2;
        sep1 = "/";
        sep2 = "/";
        if ((options.write_vertex_uvs || options.write_face_uvs) && 
            (!(options.write_vertex_normals || options.write_face_normals))){
            sep2 = "";
        }
        // Write faces
        for (int findex = 0; fit != fend; fit++, findex++){
            // Get the current face
            FacePtr face = (*fit);
            // Get face attributes
            // Positions
            std::string v0, v1, v2, t0, t1, t2, n0, n1, n2;
            v0 = num_to_str<IdType>(face->GetVertex(0)->GetId() + 1);
            v1 = num_to_str<IdType>(face->GetVertex(1)->GetId() + 1);
            v2 = num_to_str<IdType>(face->GetVertex(2)->GetId() + 1);
            // Texture coordinates
            if (options.write_vertex_uvs){
                t0 = v0;
                t1 = v1;
                t2 = v2;
            } else if (options.write_face_uvs){
                t0 = num_to_str<IdType>(findex*3 + 1);
                t1 = num_to_str<IdType>(findex*3 + 1 + 1);
                t2 = num_to_str<IdType>(findex*3 + 2 + 1);
            } else {
                t0 = "";
                t1 = "";
                t2 = "";
            }
            // Normals
            if (options.write_vertex_normals){
                n0 = v0;
                n1 = v1;
                n2 = v2;
            } else if (options.write_face_normals){
                n0 = num_to_str<IdType>(findex*3 + 1);
                n1 = num_to_str<IdType>(findex*3 + 1 + 1);
                n2 = num_to_str<IdType>(findex*3 + 2 + 1);
            } else {
                n0 = "";
                n1 = "";
                n2 = "";
            }
            // Write the face with all the attributes
            f << "f " << v0 << sep1 << t0 << sep2 << n0 << " " << v1 << sep1 << t1 << sep2 << n1 << " " << v2 << sep1 << t2 << sep2 << n2 << std::endl;
        }
    }

    // Close file
    f.close();
}


void Mesh::WriteOff(const char *filename, const WriteOptions &options){

    // Open file and check for errors
    std::ofstream f;
    f.open(filename);
    if (f.fail()){
        throw(std::ios_base::failure(std::string("Error opening file ")+std::string(filename)));
    }
   
    // Write header
    f << "OFF" << std::endl;
    f << VertexCount() << " " << FaceCount() << " 0" << std::endl;

    // Write vertices
    VertexIterator vit, vend;
    vit = VertexBegin();
    vend = VertexEnd();
    for (; vit != vend; vit++){
        VertexPtr vertex = (*vit);
        PositionType pos = vertex->GetPosition();
        f << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
    }

    // Write faces
    FaceIterator fit, fend;
    fit = FaceBegin();
    fend = FaceEnd();
    for (; fit != fend; fit++){
        // Get the current face
        FacePtr face = (*fit);
        // Write face
        f << "3 " << face->GetVertex(0)->GetId() << " " << face->GetVertex(1)->GetId() << " " << face->GetVertex(2)->GetId() << std::endl;
    }

    // Close file
    f.close();
}


IdType Mesh::VertexCount(void) const {

    return vertex_.size();
}


IdType Mesh::FaceCount(void) const {

    return face_.size();
}


VertexPtr Mesh::GetVertex(IdType id) const {

    // Use at since the function is 'const', while [] does not return
    // a constant
    return vertex_.at(id);
}


FacePtr Mesh::GetFace(IdType id) const {

    return face_.at(id);
}


Mesh::VertexIterator Mesh::VertexBegin() {

    return VertexIterator(vertex_.begin());
}


Mesh::VertexIterator Mesh::VertexEnd() {

    return VertexIterator(vertex_.end());
}


Mesh::FaceIterator Mesh::FaceBegin() {

    return FaceIterator(face_.begin());
}


Mesh::FaceIterator Mesh::FaceEnd() {

    return FaceIterator(face_.end());
}


VertexPtr Mesh::AddVertex(PositionType position){

    last_vertex_id_++;
    VertexPtr vertex = new Vertex(last_vertex_id_);
    vertex->SetPosition(position);
    vertex_.insert(VertexContainer::value_type(last_vertex_id_, vertex));
    return vertex;
}


FacePtr Mesh::AddFace(VertexPtr v1, VertexPtr v2, VertexPtr v3){

    // Add face to face container
    last_face_id_++;
    FacePtr face = new Face(last_face_id_);
    face->AddVertex(v1);
    face->AddVertex(v2);
    face->AddVertex(v3);
    face_.insert(FaceContainer::value_type(last_face_id_, face));

    // Add pointers to vertices back to face, if required
    if (has_connectivity_){
        v1->AddFace(face);
        v2->AddFace(face);
        v3->AddFace(face);
    }

    // Return the new face
    return face;
}


FacePtr Mesh::AddFace(IdType v1, IdType v2, IdType v3){

    return AddFace(GetVertex(v1), GetVertex(v2), GetVertex(v3));
}


VertexPtr Mesh::RemoveVertex(IdType id){

    VertexContainer::iterator vit = vertex_.find(id); 
    VertexPtr vertex = vit->second;
    vertex_.erase(vit);
    return vertex;
}


void Mesh::RemoveVertex(VertexPtr vertex){

    vertex_.erase(vertex_.find(vertex->id_));
}


FacePtr Mesh::RemoveFace(IdType id){

    // Get the face to be removed
    FaceContainer::iterator fit = face_.find(id);
    FacePtr face = (*fit).second;

    // Remove pointers to vertices back to face, if required
    if (has_connectivity_){
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            (*cit).vertex_->RemoveFace(face);
        }
    }

    // Remove the face
    face_.erase(fit);

    // Return face
    return face;
}


void Mesh::RemoveFace(FacePtr face){

    // Remove pointers to vertices back to face, if required
    if (has_connectivity_){
        CornerContainer::iterator cit, cend;
        cit = face->corner_.begin();
        cend = face->corner_.end();
        for (; cit != cend; cit++){
            (*cit).vertex_->RemoveFace(face);
        }
    }

    // Remove the face
    face_.erase(face_.find(face->id_));
}


void Mesh::ReindexIds(void){

    // Reset ids
    last_vertex_id_ = -1;
    last_face_id_ = -1;

    // Assign ids sequentially to vertices
    VertexContainer::iterator vit, vend;
    vit = vertex_.begin();
    vend = vertex_.end();
    for (; vit != vend; vit++){
        VertexPtr vertex = (*vit).second;
        // Remove vertex from container
        vertex_.erase(vertex->id_);
        // Assign new sequential id
        last_vertex_id_++;
        vertex->id_ = last_vertex_id_;
        // Add vertex back to the container
        vertex_.insert(VertexContainer::value_type(vertex->id_, vertex));
    }

    // Assign ids sequentially to faces
    FaceContainer::iterator fit, fend;
    fit = face_.begin();
    fend = face_.end();
    for (; fit != fend; fit++){
        FacePtr face = (*fit).second;
        // Remove face from container
        face_.erase(face->id_);
        // Assign new sequential id
        last_face_id_++;
        face->id_ = last_face_id_;
        // Add vertex back to the container
        face_.insert(FaceContainer::value_type(face->id_, face));
    }
}


Mesh::ColorScheme Mesh::GetColorScheme(void) const {

    return color_scheme_;
}


void Mesh::SetColorScheme(ColorScheme color_scheme){

    color_scheme_ = color_scheme;
}


} // namespace GeomProc
