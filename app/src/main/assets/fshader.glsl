#version 300 es
precision mediump float;
in vec2 TexCoord;
out vec4 fragmentColor;
uniform sampler2D texture1;
uniform sampler2D texture2;
void main(){
//    fragmentColor = texture(texture1, TexCoord);
    fragmentColor = mix(texture(texture1, TexCoord),texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), 0.2);
}