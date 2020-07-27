#version 300 es
precision mediump float;
in vec3 ourColor;
out vec4 fragmentColor;
void main(){
    fragmentColor = vec4(ourColor, 1.0);
}