#version 330	// GLSL version

// Camera position and direction;
uniform vec3 camera_position;
uniform vec3 cameradir;

//draw colours according to height
uniform bool colourbyheight;
in float originalzcoordinate;

//render for or not
uniform bool fogenabled;

//directional or head mounted
uniform bool directional;

//apply or not apply bumps
uniform bool applybump;

//apply or not toon shading
uniform bool applytoon;


//texture vs mat property influence diffuse colour
uniform float tex_vs_mat;

// Directional light
uniform vec3 d_light_direction;
uniform vec3 d_light_a_color;
uniform float d_light_a_intensity;
uniform vec3 d_light_d_color;
uniform float d_light_d_intensity;
uniform vec3 d_light_s_color;
uniform float d_light_s_intensity;

// Head-mounted light //TODO: implement it yourself!
uniform vec3 p_light_a_color;
uniform float p_light_a_intensity;
uniform vec3 p_light_d_color;
uniform float p_light_d_intensity;
uniform vec3 p_light_s_color;
uniform float p_light_s_intensity;
uniform float klinear;
uniform float ksquared;
uniform float anglerest;
uniform float angledecrease;
// TODO: other parameters

// Object material
// Notice that all of this values may be also specified per-vertex or 
//  through a texture.
uniform vec3 material_a_color;
uniform vec3 material_d_color;
uniform vec3 material_s_color;
uniform float material_shininess;

// Sampler to access the texture
uniform sampler2D sampler;
uniform sampler2D bumpsampler;

// Per fragment texture coordinates
in vec2 cur_tex_coords;

// Per-fragment color coming from the vertex shader
in vec3 positionout;
in vec3 normalout;
in vec3 tangentout;

// Per-frgament output color
out vec4 FragColor;

const vec3 fogColor = vec3(0.5, 0.5,0.5);

void main() { 
	vec3 normal_nn;

	if(applybump){
	 mat3 TBN =mat3(
        tangentout,
        cross(normalout,tangentout),
        normalout
    );

	vec3 badnormal =  2.0*vec3(texture2D(bumpsampler, cur_tex_coords)) -1.0;
	normal_nn = normalize(TBN * badnormal);
	}
	else{
	 normal_nn = normalize(normalout);	
	}
   
	vec3 view_dir_nn = normalize(camera_position - positionout);
	vec3 fcolor;
	float dist = length(camera_position - positionout);

	float procentageoftexture = tex_vs_mat;

	if(dist > 200.0) {procentageoftexture = min(procentageoftexture, clamp((220.0 - dist) / 20.0, 0.0, 1.0)); }              // if we are sufficiantly far away from object use only material 

	vec3 diffusecolor =  procentageoftexture * vec3(texture2D(sampler, cur_tex_coords)) + (1.0 - procentageoftexture) * material_d_color;   // blend the material and texture
	if(colourbyheight){
		diffusecolor = mix(vec3(1.0,0.0,0.0), vec3(1.0,1.0,0.0), smoothstep(0.0,300.0,originalzcoordinate));
		diffusecolor = mix(diffusecolor, vec3(0.0,0.0,1.0), smoothstep(300.0,650.0,originalzcoordinate));
		}
	// parameters for toon shading
	const int levels = 10;
	const float scaleFactor = 1.0 / levels;

	if(directional){
		// --- directional light ----
		// compute the required values and vectors
		// notice that input variables cannot be modified, so copy them first
		//vec3 d_light_dir_nn = normalize(mat3(vertextransformation)*d_light_direction);	
		
		vec3 d_light_dir_nn = normalize(d_light_direction);	
		
		float dot_d_light_normal = dot(-d_light_dir_nn, normal_nn);   // notice the minus!
		vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal_nn;
		// should be already normalized, but we "need" to correct numerical errors
		d_reflected_dir_nn = normalize(d_reflected_dir_nn); 
	
		// compute the color contribution	
		vec3 color;
		vec3 amb_color = clamp(
				material_a_color * d_light_a_color * d_light_a_intensity,
				0.0, 1.0);
		vec3 diff_color = clamp(
				diffusecolor * dot_d_light_normal * d_light_d_intensity * d_light_d_color,
				0.0, 1.0);      //added d_light_d_color because it should be there according to me and the presentation
		vec3 spec_color = clamp(
				material_s_color *  d_light_s_color * d_light_s_intensity *
				pow(clamp(dot(d_reflected_dir_nn, view_dir_nn),0.0,1.0), material_shininess),    // clamp to get rid of case when the cosinus is negative
				0.0, 1.0);     //added d_light_d_color because it should be there according to me and the presentation
		

		float edgeDetection = 1;
		float speccoef = 1;
		if(applytoon){
			edgeDetection = (dot(view_dir_nn, normal_nn) > 0.3) ? 1 : 0;    // make the surfaces in large angle dark(if toon shading is enabled)
			diff_color = clamp(
				diffusecolor * d_light_d_intensity * d_light_d_color * floor(dot_d_light_normal * levels) * scaleFactor,  //make steps in colour
				0.0, 1.0);

			speccoef = (pow(clamp(dot(d_reflected_dir_nn, view_dir_nn),0.0,1.0), material_shininess)> 0.4) ? 1 : 0;   // limit specular light
		}

	

		color = clamp(
				amb_color + diff_color + spec_color * speccoef,
				0.0, 1.0);    

		fcolor = color * edgeDetection;
	}
	else{
		// TODO: do the same for the headlight!
		// notice that for the headlight dot(view_dir, light_dir) = ...

		//p_light_dir_nn = view_dir_nn;
	
		vec3 p_light_dir_nn = -view_dir_nn;
	
		float dot_p_light_normal = dot(-p_light_dir_nn, normal_nn);   // notice the minus!
		vec3 p_reflected_dir_nn = p_light_dir_nn + 2. * dot_p_light_normal * normal_nn;
		// should be already normalized, but we "need" to correct numerical errors
		p_reflected_dir_nn = normalize(p_reflected_dir_nn); 
	
		//decrease intensity with distance(calculate coeficient of decreasing)
		float distancecoef = 1 / (1.0f + klinear*dist+ksquared*dist*dist);
		
		//restrict light to angle
		float intensitydeccoef=distancecoef;
		float lightToSurfaceAngle =dot(p_light_dir_nn, normalize(cameradir));
		intensitydeccoef *= pow(lightToSurfaceAngle,angledecrease);
		if(degrees(acos(lightToSurfaceAngle)) > anglerest){
			intensitydeccoef = 0.0;
		}

		


		// compute the color contribution
		vec3 color;
		vec3 amb_color = clamp(
				material_a_color * p_light_a_color * p_light_a_intensity * intensitydeccoef,
				0.0, 1.0);
		vec3 diff_color = clamp(
				diffusecolor * p_light_d_intensity * intensitydeccoef* p_light_d_color * dot_p_light_normal,
				0.0, 1.0);
		vec3 spec_color = clamp(
				material_s_color *  p_light_s_color *  p_light_s_intensity * intensitydeccoef *
				pow(clamp(dot(p_reflected_dir_nn, view_dir_nn),0.0,1.0), material_shininess),
				0.0, 1.0);
		

		float edgeDetection = 1;
		float speccoef = 1;
		if(applytoon){
			edgeDetection = (dot(view_dir_nn, normal_nn) > 0.3) ? 1 : 0;    // make the surfaces in large angle dark(if toon shading is enabled)
			diff_color = clamp(
				diffusecolor * p_light_d_intensity * intensitydeccoef* p_light_d_color * floor(dot_p_light_normal * levels) * scaleFactor,     //make steps in colour
				0.0, 1.0);

			speccoef = (pow(clamp(dot(p_reflected_dir_nn, view_dir_nn),0.0,1.0), material_shininess)> 0.4) ? 1 : 0;   // limit specular light
		}

		

		color = clamp(
				amb_color + diff_color + spec_color * speccoef,
				0.0, 1.0);    

		// pass the result 
		fcolor = edgeDetection * color;
	}
	float FogDensity = 0.05;

	float fogFactor = 1;
	if(fogenabled) 
		fogFactor = 1.0 /exp(dist * FogDensity);
    fogFactor = clamp( fogFactor, 0.0, 1.0 );
 
    //if you inverse color in glsl mix function you have to
    //put 1.0 - fogFactor
    vec3 finalcolour = mix(fogColor, fcolor, fogFactor);

	// Set the output color according to the input
    FragColor = vec4(finalcolour,1.0);
}