#version 300 es
precision mediump float;
in vec3 ourColor;
in vec2 TexCoord;
out vec4 fragmentColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main(){
//    fragmentColor = texture(texture1, TexCoord);
    fragmentColor = mix(texture(texture1, TexCoord),texture(texture2, TexCoord), 0.2);
}