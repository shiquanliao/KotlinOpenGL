#version 300 es
precision mediump float;
in vec3 ourColor;
in vec2 TexCoord;
out vec4 fragmentColor;
uniform sampler2D ourTexture;
void main(){
//    fragmentColor = vec4(ourColor, 1.0);
    fragmentColor = texture(ourTexture, TexCoord);
}