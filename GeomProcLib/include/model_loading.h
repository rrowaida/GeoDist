#ifndef MODEL_LOADING_H_
#define MODEL_LOADING_H_

namespace GeomProc {

// Auxiliary definitions and functions for model loading

// A temporary triangular face
struct TempFace {
    int i[3];
    int n[3];
    int t[3];
};

// A temporary quad face
struct TempQuad {
    int i[4];
    int n[4];
    int t[4];
};

} // namespace GeomProc

#endif // MODEL_LOADING_H_
