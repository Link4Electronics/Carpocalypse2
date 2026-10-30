#ifndef C2_VITA_SHADERS_H
#define C2_VITA_SHADERS_H

/* Native Cg BUFFER declarations use the exact CPU std140 layout. Explicit
 * float4 columns avoid any dependency on Cg's default matrix storage order.
 * The scene lights tail is unused by the desktop shaders too. */
#define VITA_CG_BUFFERS \
"uniform br_model_state {\n" \
" float4 model_view[4], projection[4], projection_brender[4], mvp[4];\n" \
" float4 normal_matrix[4], environment[4], map_transform[4];\n" \
" float4 surface_colour, clear_colour, eye_m;\n" \
" float ka, ks, kd, power;\n" \
" int lighting, uv_source, disable_colour_key, disable_texture;\n" \
" int fog_enabled; float depth_bias, pad1, pad2;\n" \
" float4 fog_colour; float fog_min, fog_max, alpha; int prelit;\n" \
"} br_model_state : BUFFER[0];\n" \
"uniform br_scene_state {\n" \
" float4 eye_view, clip_planes[6];\n" \
" int num_clip_planes; float yon_z; int num_lights; float pad0;\n" \
"} br_scene_state : BUFFER[1];\n" \
"float4 transform(float4 v, float4 m[4]) {\n" \
" return v.x*m[0] + v.y*m[1] + v.z*m[2] + v.w*m[3];\n" \
"}\n" \
"struct VertexOutput { float4 position:POSITION; float2 uv:TEXCOORD0;\n" \
" float4 colour:COLOR0; float3 rawPosition:TEXCOORD1;\n" \
" float3 rawNormal:TEXCOORD2; float view_z:TEXCOORD3;\n" \
" float4 clipPosition:TEXCOORD4; };\n"

static const char VITA_BRENDER_VERT[] = VITA_CG_BUFFERS
"VertexOutput main(float3 aPosition, float2 aUV, float3 aNormal, float4 aColour) {\n"
" VertexOutput o; float4 p=float4(aPosition,1);\n"
" float4 view=transform(p,br_model_state.model_view);\n"
" o.position=transform(view,br_model_state.projection);\n"
" o.position.z+=br_model_state.depth_bias*o.position.w;\n"
" o.clipPosition=transform(view,br_model_state.projection_brender);\n"
" o.view_z=view.z; o.uv=aUV; o.rawPosition=aPosition; o.rawNormal=aNormal;\n"
" o.colour=float4(1,1,1,1);\n"
" if(br_model_state.prelit==1) o.colour=float4(aColour.rgb,1);\n"
" else if(br_model_state.lighting==1) {\n"
"  if(br_scene_state.num_lights==0) {\n"
"   float4 base=br_model_state.disable_texture ? br_model_state.surface_colour : float4(1,1,1,br_model_state.surface_colour.a);\n"
"   o.colour=float4(saturate(base.rgb*br_model_state.ka),base.a);\n"
"  } else o.colour=br_model_state.surface_colour + float4(br_model_state.clear_colour.rgb,0);\n"
" }\n"
" return o;\n"
"}\n";

static const char VITA_BRENDER_FRAG[] = VITA_CG_BUFFERS
"uniform sampler2D main_texture;\n"
"float3 safe_normal(float3 v) { float d=dot(v,v); return d>1e-12 ? v*rsqrt(d) : float3(0,0,0); }\n"
"float4 main(VertexOutput i):COLOR {\n"
" for(int c=0;c<6;c++) if(c<br_scene_state.num_clip_planes) clip(dot(br_scene_state.clip_planes[c],i.clipPosition));\n"
" float2 uv=i.uv;\n"
" if(br_model_state.uv_source==1 || br_model_state.uv_source==2) {\n"
"  float3 eye=br_model_state.uv_source==1 ? br_model_state.eye_m.xyz-i.rawPosition : br_model_state.eye_m.xyz;\n"
"  float3 r=reflect(safe_normal(eye),safe_normal(i.rawNormal));\n"
"  float3 wr=safe_normal(transform(float4(r,0),br_model_state.environment).xyz);\n"
"  uv=float2(0.5+atan2(wr.x,-wr.z)*0.159154943091895,0.5-wr.y*0.5);\n"
" }\n"
" uv=transform(float4(uv,1,0),br_model_state.map_transform).xy;\n"
" float4 t=br_model_state.surface_colour;\n"
" if(!br_model_state.disable_texture) {\n"
"  t=tex2D(main_texture,uv);\n"
"  if(!br_model_state.disable_colour_key && all(t.rgb==float3(0,0,0))) discard;\n"
"  clip(t.a-0.004);\n"
" }\n"
" float4 result=float4(t.rgb,br_model_state.alpha*(br_model_state.disable_texture ? 1 : t.a));\n"
" if(br_model_state.lighting==1 || br_model_state.prelit==1) result.rgb*=i.colour.rgb;\n"
" if(br_model_state.fog_enabled) {\n"
"  float d=-i.view_z/br_scene_state.yon_z;\n"
"  float f=1-exp(-(80/br_model_state.fog_max)*d*d);\n"
"  result.rgb=lerp(result.rgb,br_model_state.fog_colour.rgb,saturate(f));\n"
" }\n"
" return result;\n"
"}\n";

/* Specialised variants of the two programs above (same buffers and maths), selected per draw by
 * vita_gl.c from the uploaded model/scene state and the bound texture. Each feature that the uber
 * shader decides per fragment is compiled in only when needed:
 *   C2_CLIP  scene clip planes      C2_ENV  environment-mapped UVs (uv_source 1/2)
 *   C2_TEX   textured               C2_KEY  colour-key discard (texture has pure black texels)
 *   C2_ALPHA alpha-test discard     C2_FOG  fog         C2_MOD  vertex colour modulation
 * Without C2_KEY / C2_ALPHA the shader has no discard, which keeps the PowerVR hidden surface
 * removal working; they are dropped only when the texture has no texel that could be discarded. */
#define VITA_CG_BUFFERS_VARIANT \
"uniform br_model_state {\n" \
" float4 model_view[4], projection[4], projection_brender[4], mvp[4];\n" \
" float4 normal_matrix[4], environment[4], map_transform[4];\n" \
" float4 surface_colour, clear_colour, eye_m;\n" \
" float ka, ks, kd, power;\n" \
" int lighting, uv_source, disable_colour_key, disable_texture;\n" \
" int fog_enabled; float depth_bias, pad1, pad2;\n" \
" float4 fog_colour; float fog_min, fog_max, alpha; int prelit;\n" \
"} br_model_state : BUFFER[0];\n" \
"uniform br_scene_state {\n" \
" float4 eye_view, clip_planes[6];\n" \
" int num_clip_planes; float yon_z; int num_lights; float pad0;\n" \
"} br_scene_state : BUFFER[1];\n" \
"float4 transform(float4 v, float4 m[4]) {\n" \
" return v.x*m[0] + v.y*m[1] + v.z*m[2] + v.w*m[3];\n" \
"}\n" \
"struct VertexOutput { float4 position:POSITION; float2 uv:TEXCOORD0;\n" \
"#if C2_MOD\n float4 colour:COLOR0;\n#endif\n" \
"#if C2_ENV\n float3 rawPosition:TEXCOORD1; float3 rawNormal:TEXCOORD2;\n#endif\n" \
"#if C2_FOG\n float view_z:TEXCOORD3;\n#endif\n" \
"#if C2_CLIP\n float4 clipPosition:TEXCOORD4;\n#endif\n" \
"};\n"

static const char VITA_BRENDER_VARIANT_VERT[] = VITA_CG_BUFFERS_VARIANT
"VertexOutput main(float3 aPosition, float2 aUV, float3 aNormal, float4 aColour) {\n"
" VertexOutput o; float4 p=float4(aPosition,1);\n"
" float4 view=transform(p,br_model_state.model_view);\n"
" o.position=transform(view,br_model_state.projection);\n"
" o.position.z+=br_model_state.depth_bias*o.position.w;\n"
"#if C2_CLIP\n o.clipPosition=transform(view,br_model_state.projection_brender);\n#endif\n"
"#if C2_FOG\n o.view_z=view.z;\n#endif\n"
" o.uv=aUV;\n"
"#if C2_ENV\n o.rawPosition=aPosition; o.rawNormal=aNormal;\n#endif\n"
"#if C2_MOD\n"
" o.colour=float4(1,1,1,1);\n"
" if(br_model_state.prelit==1) o.colour=float4(aColour.rgb,1);\n"
" else if(br_model_state.lighting==1) {\n"
"  if(br_scene_state.num_lights==0) {\n"
"   float4 base=br_model_state.disable_texture ? br_model_state.surface_colour : float4(1,1,1,br_model_state.surface_colour.a);\n"
"   o.colour=float4(saturate(base.rgb*br_model_state.ka),base.a);\n"
"  } else o.colour=br_model_state.surface_colour + float4(br_model_state.clear_colour.rgb,0);\n"
" }\n"
"#endif\n"
" return o;\n"
"}\n";

static const char VITA_BRENDER_VARIANT_FRAG[] = VITA_CG_BUFFERS_VARIANT
"uniform sampler2D main_texture;\n"
"float3 safe_normal(float3 v) { float d=dot(v,v); return d>1e-12 ? v*rsqrt(d) : float3(0,0,0); }\n"
"float4 main(VertexOutput i):COLOR {\n"
"#if C2_CLIP\n for(int c=0;c<6;c++) if(c<br_scene_state.num_clip_planes) clip(dot(br_scene_state.clip_planes[c],i.clipPosition));\n#endif\n"
" float2 uv=i.uv;\n"
"#if C2_ENV\n"
" {\n"
"  float3 eye=br_model_state.uv_source==1 ? br_model_state.eye_m.xyz-i.rawPosition : br_model_state.eye_m.xyz;\n"
"  float3 r=reflect(safe_normal(eye),safe_normal(i.rawNormal));\n"
"  float3 wr=safe_normal(transform(float4(r,0),br_model_state.environment).xyz);\n"
"  uv=float2(0.5+atan2(wr.x,-wr.z)*0.159154943091895,0.5-wr.y*0.5);\n"
" }\n"
"#endif\n"
" uv=transform(float4(uv,1,0),br_model_state.map_transform).xy;\n"
"#if C2_TEX\n"
" float4 t=tex2D(main_texture,uv);\n"
"#if C2_KEY\n if(all(t.rgb==float3(0,0,0))) discard;\n#endif\n"
"#if C2_ALPHA\n clip(t.a-0.004);\n#endif\n"
" float4 result=float4(t.rgb,br_model_state.alpha*t.a);\n"
"#else\n"
" float4 result=float4(br_model_state.surface_colour.rgb,br_model_state.alpha);\n"
"#endif\n"
"#if C2_MOD\n result.rgb*=i.colour.rgb;\n#endif\n"
"#if C2_FOG\n"
" {\n"
"  float d=-i.view_z/br_scene_state.yon_z;\n"
"  float f=1-exp(-(80/br_model_state.fog_max)*d*d);\n"
"  result.rgb=lerp(result.rgb,br_model_state.fog_colour.rgb,saturate(f));\n"
" }\n"
"#endif\n"
" return result;\n"
"}\n";

static const char VITA_DEFAULT_VERT[] =
"struct Output { float4 position:POSITION; float2 uv:TEXCOORD0; };\n"
"Output main(float3 aPosition, float3 aColour, float2 aUV) {\n"
" Output o; o.position=float4(aPosition,1); o.uv=aUV; return o; }\n";

static const char VITA_DEFAULT_FRAG[] =
"uniform sampler2D uSampler; uniform float uFlipVertically; uniform int uDiscardPurplePixels;\n"
"float4 main(float2 uv:TEXCOORD0):COLOR {\n"
" float4 c=tex2D(uSampler,float2(uv.x,abs(uFlipVertically-uv.y)));\n"
" if(uDiscardPurplePixels==1 && all(c.rgb==float3(1,0,1))) discard; return c; }\n";

static const char VITA_TEXT_FRAG[] =
"uniform sampler2D uSampler; uniform float4 uTextColour;\n"
"float4 main(float2 uv:TEXCOORD0):COLOR {\n"
" return float4(uTextColour.rgb,tex2D(uSampler,uv).a*uTextColour.a); }\n";

#endif
