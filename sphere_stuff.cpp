void generateSphereData(std::vector<GLfloat>& vertices, 
                        std::vector<GLfloat>& indices,
                        float radius, 
                        unsigned int stackCount, 
                        unsigned int sectorCount) {
    for (unsigned int stack = 0; stack <= stackCount; ++stack) {
        float stackAngle = M_PI / 2 - stack * M_PI / stackCount; // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (unsigned int sector = 0; sector <= sectorCount; ++sector) {
            float sectorAngle = sector * 2 * M_PI / sectorCount; // from 0 to 2pi

            float x = xy * cosf(sectorAngle); // r * cos(u) * cos(v)
            float y = xy * sinf(sectorAngle); // r * cos(u) * sin(v)

            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    // Generate indices
    for (unsigned int stack = 0; stack < stackCount; ++stack) {
        unsigned int k1 = stack * (sectorCount + 1); // beginning of current stack
        unsigned int k2 = k1 + sectorCount + 1; // beginning of next stack

        for (unsigned int sector = 0; sector < sectorCount; ++sector, ++k1, ++k2) {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            if (stack != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (stack != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
