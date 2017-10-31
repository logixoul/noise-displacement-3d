varying vec2 tc;
uniform sampler2D tex;
uniform vec2 mouse;
varying vec3 pos;
varying vec3 normal;
uniform float time;
varying vec3 pos2;

vec3 _out = vec3(0.0);
vec3 c;
uniform vec2 viewportSize;

vec3 fog = vec3(0.0);
void light(vec3 L, vec3 color)
{
	//L.z=-10.0*L.z;
	//vec3 nL = normalize(L);
	vec3 L2 = L-pos;
	vec3 nN = normalize(normal);
	vec3 V = pos-(gl_ModelViewMatrixInverse*vec4(0.0,0.0,0.0,1.0)).xyz;
	float _dot = dot(normalize(reflect(V, nN)), normalize(L2));
	float _dot2 = dot(nN, normalize(L2));
	//_out += max(0.0, pow(_dot, 40.0)) * /*c * */ color * (1.0 / pow(length(L2), 2.0));
	_out += 10.0 * pow(max(0.0, _dot), 50.0) * color.xyz * (1.0 / pow(length(L2), 2.0));
	_out += max(0.0, _dot2) * c * color * (1.0 / pow(length(L2), 2.0));
	//_out += (_dot2 * .5 + .5) * c * color * (1.0 / pow(length(L2), 2.0));
	//_out += c * mix(.001*vec3(1.0, 1.1, 1.3), color, _dot * .5 + .5);

	/*
	vec4 L3 = gl_ModelViewProjectionMatrix * vec4(L, 1.0);
	L3 /= L3.w;
	L3.xy = (L3.xy * .5 + vec2(.5)) * viewportSize;
	vec3 L5 = (gl_ModelViewMatrix * vec4(L, 1.0)).xyz;
	float Ldist = length(L5);
	float gaussC = (500.0 * Ldist / 100.0) / 2.35482;
	const float pi = 3.14159265359;
	float gaussA = 1.0/(gaussC*sqrt(2.0*pi));
	float gaussC22 = 2.0 * pow(gaussC, 2.0);
	if(Ldist < length(pos2))
		fog += exp(-Ldist / 7.0) * gaussA * exp(-pow(length(L3.xy - gl_FragCoord.xy), 2.0) / gaussC22) * color;
	*/
}

float s1(float f) { return sin(f) * .5 + .5; }
float c1(float f) { return cos(f) * .5 + .5; }
float clamp01(float f) { return max(0.0, min(1.0, f)); }
//float fade(float f) { return smoothstep(
float ssin(float f) { return smoothstep(0.47, 0.53, s1(f)); }
void main()
{
	//c = vec3(1.0, 1.0, 1.0);

    c = normal * .5 + .5;

	//c *= _out_v;
	//c = vec3(mix(0.0, 1.0, (1.0/2.0)*(s1(.1*pos.x*s1(pos.x/10.0))+s1(pos.y))));
	//c = vec3(
	//	abs(.5*(sin(pos.x+0.0*texture2D(tex,pos.xy/20.0).r) + sin(pos.y))));
	//c = vec3(.5) + .5 * vec3(ssin(pos0.x) + ssin(pos0.y) + ssin(pos0.z));
	//c *= abs(pos.z) / (abs(pos.z) + 1.0);
	//light(vec3(100.0*s(t*.12), 100.0*s(t*.1+1.0), 20.0+30.0*s(t*.1+2.0)), 10000.0*vec3(1.0, 0.1, 0.1));
	float twopi_=3.14 * 2.0;
	float t = time * .1 * .0;
	light(vec3(50.0+7.0*sin(t+twopi_*0.0/3.0), 50.0+7.0*cos(t+twopi_*0.0/3.0), -6.0+30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);
	light(vec3(50.0+7.0*sin(t+twopi_*1.0/3.0), 50.0+7.0*cos(t+twopi_*1.0/3.0), -6.0+30.0*c1(t*1.1)), 200.0*vec3(1.0, 0.4, 0.3).zxy);
	light(vec3(50.0+7.0*sin(t+twopi_*2.0/3.0), 50.0+7.0*cos(t+twopi_*2.0/3.0), 10.0), 200.0*vec3(1.0, 0.1, 0.1).yzx);
	
	/*light(vec3(50.0+7.0*sin(t+twopi_*0.0/3.0), 50.0+7.0*cos(t+twopi_*0.0/3.0), -6.0+0.0*30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);
	light(vec3(50.0+7.0*sin(t+twopi_*0.5/3.0), 50.0+7.0*cos(t+twopi_*0.5/3.0), -6.0+0.0*30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);
	light(vec3(50.0+7.0*sin(t+twopi_*1.0/3.0), 50.0+7.0*cos(t+twopi_*1.0/3.0), -6.0+0.0*30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);
	light(vec3(50.0+7.0*sin(t+twopi_*1.5/3.0), 50.0+7.0*cos(t+twopi_*1.5/3.0), -6.0+0.0*30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);
	light(vec3(50.0+7.0*sin(t+twopi_*2.0/3.0), 50.0+7.0*cos(t+twopi_*2.0/3.0), -6.0+0.0*30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);
	light(vec3(50.0+7.0*sin(t+twopi_*2.5/3.0), 50.0+7.0*cos(t+twopi_*2.5/3.0), -6.0+0.0*30.0*c1(t)), 200.0*vec3(1.0, 1.0, 1.0).xyz);*/
	
	//_out += dFdy(gl_FragCoord.z);

	//_out += .01*vec3(1.0, 1.1, 1.3) * c;

	//_out = vec3(mod(z, 1.0 / 1000.0) * 1000.0);
	//_out = vec3(z);
	_out /= dFdy(gl_FragCoord.z)*10000.0;
	//_out = max(_out, vec3(0.0));
	//_out /= _out + vec3(1.0);
	//_out = mix(_out, vec3(.0) + fog, 1.0 - exp(-(1.0/30.0)*length(pos2)));
	//_out = mix(_out, vec3(0.1), clamp01((length(pos2)+0.0)/110.0));
	float luma = dot(_out, vec3(.2, .7, .1));
	float luma2 = luma / (luma + 1.0);
	_out *= luma2 / luma;
	_out = pow(_out, vec3(1.0 / 2.2));
	gl_FragColor = vec4(_out, 1.0);
}