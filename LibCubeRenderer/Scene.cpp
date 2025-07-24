#include "pch.h"
#include "include/Scene.h"

namespace CubeRenderer {

    Scene::Scene() {
        indexCount = 0;
        indices;
    }

    Vertex* Scene::CreateVertices(float width, float height, float depth, float offsetX, float offsetY, float offsetZ, float u, float v, float textureWidth, float textureHeight, size_t* sizeOut, UINT* countOut) {

        float left = -width / 2 + offsetX;
        float right = width / 2 + offsetX;

        float top = height / 2 + offsetY;
        float bottom = -height / 2 + offsetY;

        float front = -depth / 2 + offsetZ;
        float back = depth / 2 + offsetZ;

        float normalisedU = u / textureWidth;
        float normalisedV = v / textureHeight;
        float normalisedWidth = width / textureWidth;
        float normalisedHeight = height / textureHeight;
        float normalisedDepthWidth = depth / textureWidth;
        float normalisedDepthHeight = depth / textureHeight;

        Vertex vertices[] = {
            // Front face
            { left,  bottom, front, normalisedDepthWidth + normalisedU, normalisedV + normalisedDepthHeight + normalisedHeight }, // Bottom-left
            { right, bottom, front, normalisedDepthWidth + normalisedU + normalisedWidth, normalisedV + normalisedDepthHeight + normalisedHeight }, // Bottom-right
            { right, top,    front, normalisedDepthWidth + normalisedU + normalisedWidth, normalisedV + normalisedDepthHeight }, // Top-right
            { left,  top,    front, normalisedDepthWidth + normalisedU, normalisedV + normalisedDepthHeight }, // Top-left

            // Back face
            { left,  bottom, back, normalisedU + normalisedDepthWidth * 2 + normalisedWidth, normalisedV + normalisedDepthWidth + normalisedHeight },
            { right, bottom, back, normalisedU + normalisedDepthWidth * 2 + normalisedWidth * 2, normalisedV + normalisedDepthWidth + normalisedHeight },
            { right, top,    back, normalisedU + normalisedDepthWidth * 2 + normalisedWidth * 2, normalisedV + normalisedDepthHeight },
            { left,  top,    back, normalisedU + normalisedDepthWidth * 2 + normalisedWidth, normalisedV + normalisedDepthHeight },

            // Left face
            { left, bottom, front, normalisedU + normalisedDepthWidth, normalisedV + normalisedDepthHeight + normalisedHeight },
            { left, bottom, back,  normalisedU, normalisedV + normalisedDepthHeight + normalisedHeight },
            { left, top,    back,  normalisedU, normalisedV + normalisedDepthHeight },
            { left, top,    front, normalisedU + normalisedDepthWidth, normalisedV + normalisedDepthHeight },

            // Right face
            { right, bottom,  front, normalisedU + normalisedDepthWidth + normalisedWidth, normalisedV + normalisedDepthHeight + normalisedHeight },
            { right, bottom,  back,  normalisedU + normalisedDepthWidth * 2 + normalisedWidth, normalisedV + normalisedDepthHeight + normalisedHeight },
            { right, top,     back,  normalisedU + normalisedDepthWidth * 2 + normalisedWidth, normalisedV + normalisedDepthHeight },
            { right, top,     front, normalisedU + normalisedDepthWidth + normalisedWidth, normalisedV + normalisedDepthHeight },

            // Top face
            { left,  top, front, normalisedU + normalisedDepthWidth, normalisedV + normalisedDepthHeight },
            { right, top, front, normalisedU + normalisedDepthWidth + normalisedWidth, normalisedV + normalisedDepthHeight },
            { right, top, back,  normalisedU + normalisedDepthWidth + normalisedWidth, normalisedV },
            { left,  top, back,  normalisedU + normalisedDepthWidth, normalisedV },

            // Bottom face
            { left,  bottom, front, normalisedU + normalisedDepthWidth + normalisedWidth, normalisedV + normalisedDepthHeight },
            { right, bottom, front, normalisedU + normalisedDepthWidth * 2 + normalisedWidth, normalisedV + normalisedDepthHeight },
            { right, bottom, back,  normalisedU + normalisedDepthWidth * 2 + normalisedWidth, normalisedV },
            { left,  bottom, back,  normalisedU + normalisedDepthWidth + normalisedWidth, normalisedV }
        };

        *countOut = sizeof(vertices) / sizeof(Vertex);
        *sizeOut = sizeof(vertices);

        Vertex* vertexBuffer = (Vertex*)malloc(sizeof(vertices));
        if (!vertexBuffer) {
            //std::wcerr << L"Memory allocation failed in CreateVertices" << std::endl;
            //throw winrt::hresult_error(6969);
            return nullptr;
        }

        memcpy(vertexBuffer, vertices, sizeof(vertices));
        return vertexBuffer;
    }

    USHORT* Scene::CreateIndices(USHORT offset, size_t* sizeOut, UINT* countOut) {
        offset *= 24;

        USHORT indices[] = {
            // Front face
            2 + offset, 1 + offset, 0 + offset,
            0 + offset, 3 + offset, 2 + offset,

            // Back face
            7 + offset, 4 + offset, 5 + offset,
            5 + offset, 6 + offset, 7 + offset,

            // Left face
            8 + offset, 9 + offset, 10 + offset,
            10 + offset, 11 + offset, 8 + offset,

            // Right face
            14 + offset, 13 + offset, 12 + offset,
            12 + offset, 15 + offset, 14 + offset,

            // Top face
            18 + offset, 17 + offset, 16 + offset,
            16 + offset, 19 + offset, 18 + offset,

            // Bottom face
            20 + offset, 21 + offset, 22 + offset,
            22 + offset, 23 + offset, 20 + offset
        };

        *countOut = sizeof(indices) / sizeof(USHORT);
        *sizeOut = sizeof(indices);

        USHORT* indexBuffer = (USHORT*)malloc(*sizeOut);
        if (!indexBuffer) {
            //std::wcerr << L"Memory allocation failed in CreateIndices" << std::endl;
            //throw winrt::hresult_error(6969);

            return nullptr;
        }

        memcpy(indexBuffer, indices, *sizeOut);
        return indexBuffer;
    }

    UINT Scene::GetIndexCount()
    {
        return indexCount;
    }

    size_t Scene::GetVerticesSize()
    {
        return verticesSize;
    }

    Vertex* Scene::GetVertices()
    {
        return vertices;
    }

    USHORT* Scene::GetIndices()
    {
        return indices;
    }

    size_t Scene::GetIndicesSize()
    {
        return indicesSize;
    }


    void Scene::AddCube(float width, float height, float depth, float x, float y, float z, float u, float v, float textureWidth, float textureHeight) {
        UINT cubeIndex = cubeCount++;

        size_t newVerticesSize;
        UINT count;
        Vertex* newVertices = CreateVertices(width, height, depth, x, y, z, u, v, textureWidth, textureHeight, &newVerticesSize, &count);
        if (!newVertices) {
            //std::wcerr << L"Failed to create vertices for cube" << std::endl;
            //throw winrt::hresult_error(6969);
            return;
        }

        // Calculate the total size for vertices
        size_t totalVerticesCount = verticesSize / sizeof(Vertex) + count;
        Vertex* combinedVertices = (Vertex*)realloc(vertices, totalVerticesCount * sizeof(Vertex));
        if (!combinedVertices) {
            //std::wcerr << L"Memory reallocation failed in AddCube" << std::endl;
            //throw winrt::hresult_error(6969);
            free(newVertices);
            return;
        }

        // Copy new vertices into the combined buffer
        memcpy(combinedVertices + verticesSize / sizeof(Vertex), newVertices, count * sizeof(Vertex));

        // Update the original pointer and size
        vertices = combinedVertices;
        verticesSize = totalVerticesCount * sizeof(Vertex);

        free(newVertices);

        size_t newIndicesSize;
        UINT newIndexCount;
        USHORT* newIndices = CreateIndices(cubeIndex, &newIndicesSize, &newIndexCount);
        if (!newIndices) {
            //std::wcerr << L"Failed to create indices for cube" << std::endl;
            //std::wcerr << L"Memory reallocation failed in AddCube" << std::endl;
            //throw winrt::hresult_error(6969);

            return;
        }

        // Calculate the total size for indices
        size_t totalIndicesCount = indicesSize / sizeof(USHORT) + newIndexCount;
        USHORT* combinedIndices = (USHORT*)realloc(indices, totalIndicesCount * sizeof(USHORT));
        if (!combinedIndices) {
            //std::wcerr << L"Memory reallocation failed in AddCube" << std::endl;
            //std::wcerr << L"Memory reallocation failed in AddCube" << std::endl;
            //throw winrt::hresult_error(6969);

            free(newIndices);
            return;
        }

        // Copy new indices into the combined buffer
        memcpy(combinedIndices + indicesSize / sizeof(USHORT), newIndices, newIndexCount * sizeof(USHORT));

        // Update the original pointer and size
        indices = combinedIndices;
        indicesSize = totalIndicesCount * sizeof(USHORT);
        indexCount += newIndexCount;

        free(newIndices);
    }
}