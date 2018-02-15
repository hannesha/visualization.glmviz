R"(
#version 150
in vec2 f_uvcoords;

out vec4 f_color;

uniform samplerBuffer tex;
uniform vec4 line_color;
uniform float scale;
uniform float length;
void main () {
    /* fragment shader oscilloscope */
    // cubic interpolation
    float i = f_uvcoords.x * length;
    int i_f = int(floor(i));
    float y1 = texelFetch(tex, i_f -1).x;
    float y2 = texelFetch(tex, i_f).x;
    float y3 = texelFetch(tex, i_f +1).x;
    float y4 = texelFetch(tex, i_f +2).x;
    float t0 = y2 - y1;
    float t1 = y4 - y3;

    float x = fract(i);
    float x2 = x * x;
    float x3 = x2 * x;

    float y = (2. * x3 -3. * x2 +1.) * y2 + (x3 -2. * x2 +x) * t0 +
              			(-2. * x3 +3. * x2) * y3 + (x3 - x2) * t1;

    y = y * 0.5 * scale + 0.5;

    float intensity = 1. - pow(smoothstep(0., 1., abs(f_uvcoords.y - y) * 50.), 0.6);

    gl_FragColor = vec4( line_color.xyz * intensity, intensity * line_color.w);
}
)"
