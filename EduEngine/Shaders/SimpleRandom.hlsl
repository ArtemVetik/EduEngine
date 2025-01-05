
uint hash(uint x)
{
    x ^= x >> 16;
    x *= 0x85ebca6b;
    x ^= x >> 13;
    x *= 0xc2b2ae35;
    x ^= x >> 16;
    return x;
}

float random(uint seed)
{
    return hash(seed) / 4294967296.0f; // 4294967296.0f = 2^32
}

float random(uint seed, float min, float max)
{
    return min + random(seed) * (max - min);
}

float3 randomUnitVector(uint seed)
{
    // Generating random values for azimuth and zenith
    float u = random(seed);
    float v = random(seed + 1);

    // Converting random values to spherical coordinates
    float theta = 2.0 * 3.14159265359 * u; // Azimuth angle
    float phi = acos(2.0 * v - 1.0); // Zenith angle

    // Converting spherical coordinates to Cartesian coordinates
    float x = sin(phi) * cos(theta);
    float y = sin(phi) * sin(theta);
    float z = cos(phi);

    return float3(x, y, z);
}

float randomNoise(float3 position)
{
    uint seed = asuint(position.x) * 73856093 ^ asuint(position.y) * 19349663 ^ asuint(position.z) * 83492791;
    return random(seed);
}