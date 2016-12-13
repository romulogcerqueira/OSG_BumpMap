#version 130
attribute vec3 tangent;

out vec3 lightVec;
out vec3 pos;

void main() {
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

    vec3 n = normalize(gl_NormalMatrix * gl_Normal);
    vec3 t = normalize(gl_NormalMatrix * tangent);
    vec3 b = cross(n, t);

    vec3 vertexPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    pos = vertexPosition;
    vec3 lightDir = normalize(gl_LightSource[0].position.xyz - vertexPosition);

    vec3 v;
    v.x = dot(lightDir, t);
    v.y = dot(lightDir, b);
    v.z = dot(lightDir, n);
    lightVec = normalize(v);

    vertexPosition = normalize(vertexPosition);

    gl_Position = ftransform();
}
