/* Prepared edge zero is hidden; nonzero IDs identify shared visible edges. */
#ifndef GL_EDGE_INDICES_H
#define GL_EDGE_INDICES_H
static br_size_t GLBuildEdgeIndices(const struct v11model* model, br_uint_16* indices,
        gl_groupinfo* groups, br_uint_8* seen) {
    br_size_t count = 0;
    br_uint_16 vertex_offset = 0;
    for (br_uint_16 g = 0; g < model->ngroups; ++g) {
        const struct v11group* group = &model->groups[g];
        br_size_t start = count;
        memset(seen, 0, group->nedges);
        for (br_uint_16 f = 0; f < group->nfaces; ++f) {
            const v11face* face = &group->faces[f];
            for (br_uint_16 e = 0; e < 3; ++e) {
                br_uint_16 edge = face->edges[e];
                if (edge != 0 && !seen[edge]) {
                    seen[edge] = 1;
                    indices[count++] = face->vertices[e] + vertex_offset;
                    indices[count++] = face->vertices[(e + 1) % 3] + vertex_offset;
                }
            }
        }
        groups[g].count = (GLsizei)(count - start);
        groups[g].offset = (void*)(uintptr_t)(start * sizeof(br_uint_16));
        vertex_offset += group->nvertices;
    }
    return count;
}
#endif
