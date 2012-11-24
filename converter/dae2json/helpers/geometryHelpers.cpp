
#include <stdio.h>
#include <stdlib.h>
#include "geometryHelpers.h"

namespace JSONExport
{
    unsigned int* createTrianglesFromPolylist(unsigned int *verticesCount /* array containing the count for each array of indices per face */,
                                              unsigned int *polylist /* array containing the indices of a face */,
                                              unsigned int count /* count of entries within the verticesCount array */,
                                              unsigned int *triangulatedIndicesCount /* number of indices in returned array */) {
        //destination buffer size
        unsigned int indicesCount = 0;
        for (unsigned int i = 0 ; i < count ; i++) {
            indicesCount += (verticesCount[i] - 2) * 3;
        }

        if (triangulatedIndicesCount) {
            *triangulatedIndicesCount = indicesCount;
        }

        unsigned int *triangleIndices = (unsigned int*)malloc(sizeof(unsigned int) * indicesCount);
        unsigned int offsetDestination = 0;
        unsigned int offsetSource = 0;
        
        for (unsigned int i = 0 ; i < count ; i++) {
            unsigned int trianglesCount = verticesCount[i] - 2;
            
            unsigned int firstIndex = polylist[0];
            offsetSource = 1;
            
            for (unsigned k = 0 ; k < trianglesCount ; k++) {
                triangleIndices[offsetDestination] = firstIndex;
                triangleIndices[offsetDestination + 1] = polylist[offsetSource];
                triangleIndices[offsetDestination + 2] = polylist[offsetSource + 1];
                //printf("%d %d %d\n",triangleIndices[offsetDestination], triangleIndices[offsetDestination +1], triangleIndices[offsetDestination +2]);
                offsetSource += 1;
                offsetDestination += 3;
            }
            offsetSource += 1;
            
            polylist += verticesCount[i];
        }
        
        
        return triangleIndices;
    }

}