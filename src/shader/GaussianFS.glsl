#version 410 core


const vec2 resolution = vec2(640,480);
const vec2 direction_h = vec2(1,0);
const vec2 direction_v = vec2(0,1);
const vec2 offset1_h = vec2(1.3846153846) * direction_h/ resolution;
const vec2 offset2_h = vec2(3.2307692308) * direction_h/ resolution;
const vec2 offset1_v = vec2(1.3846153846) * direction_v/ resolution;
const vec2 offset2_v = vec2(3.2307692308) * direction_v/ resolution;

uniform int mode_direction;
uniform sampler2D tex;
out vec4 frag_color;


vec4 blur9_h(vec2 uv) {
  vec4 color = texture(tex, uv) * 0.2270270270;
  color += texture(tex, uv + offset1_h) * 0.3162162162;
  color += texture(tex, uv - offset1_h) * 0.3162162162;
  color += texture(tex, uv + offset2_h) * 0.0702702703;
  color += texture(tex, uv - offset2_h) * 0.0702702703;
  return color;
}

vec4 blur9_v(vec2 uv) {
  vec4 color = texture(tex, uv) * 0.2270270270;
  color += texture(tex, uv + offset1_v) * 0.3162162162;
  color += texture(tex, uv - offset1_v) * 0.3162162162;
  color += texture(tex, uv + offset2_v) * 0.0702702703;
  color += texture(tex, uv - offset2_v) * 0.0702702703;
  return color;
}

void main() {
	vec2 uv = vec2(gl_FragCoord.xy / resolution.xy);
	if (mode_direction == 0)
		frag_color = blur9_h(uv);
	 else if (mode_direction == 1)
		frag_color = blur9_v(uv);
}
