#version 410 core


in vec2 tex_pos;

uniform vec2 resolution;
uniform sampler2D tex;
uniform vec2 direction;
out vec4 frag_color;


vec4 blur9(vec2 uv) {
  vec4 color = vec4(0.0);
  vec2 off1 = vec2(1.3846153846) * direction;
  vec2 off2 = vec2(3.2307692308) * direction;
  color += texture(tex, uv) * 0.2270270270;
  color += texture(tex, uv + (off1 / resolution)) * 0.3162162162;
  color += texture(tex, uv - (off1 / resolution)) * 0.3162162162;
  color += texture(tex, uv + (off2 / resolution)) * 0.0702702703;
  color += texture(tex, uv - (off2 / resolution)) * 0.0702702703;
  return color;
}

void main() {
	vec2 uv = vec2(gl_FragCoord.xy / resolution.xy);
	vec4 a = blur9(uv);
	//frag_color = vec4(1.0,0,0,1.0);
	frag_color = texture(tex,tex_pos);
}


