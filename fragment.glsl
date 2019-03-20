#version 450 core

in vec3 position;
in vec3 texCoord;
in mat3 NormalMatrix;
in vec3 direc;
in vec3 wVec;

layout (binding = 0)uniform sampler3D volumeData;
layout (binding = 1)uniform sampler1D transferFunc;

layout(location = 0)out vec4 color;

struct LightInfo{
    vec3 Ia;
    vec3 Id;
    vec3 Is;
};

uniform LightInfo Light;
uniform float shiness;
uniform float dist;

vec3 phong_illumination_model(vec3 N,vec3 curColor,vec3 curPos){
    //compute ambient
    //                  Ka  *  Ia
    vec3 ambient = curColor * Light.Ia;
    //compute diffuse
    vec3 L = normalize(vec3(0,0,-1));
    //               Kd     *    Id    *  N dot L
    vec3 diffuse = curColor * Light.Id * max(0.0,dot(N,L));
    //compute specular
    vec3 R = reflect(L,N);
    vec3 E = normalize(-curPos);
    //                    Ks          *    Is    * (R dot E)^shiness
    vec3 specular = vec3(1.0,1.0,1.0) * Light.Is * pow(max(0.0,dot(R,E)),shiness);

    return (ambient + diffuse + specular);
}

bool isInside(vec3 coord){
    if((coord.x - 0.0) * (coord.x - 1.0) > 0.0 )
        return false;
    if((coord.y - 0.0) * (coord.y - 1.0) > 0.0 )
        return false;
    if((coord.z - 0.0) * (coord.z - 1.0) > 0.0 )
        return false;
    return true;
}

void main(){
    float alpha = 0.0;
    vec4 curTexColor;
    vec3 curShade;
    color = vec4(0.0,0.0,0.0,0.0);
    vec3 curTexCoord = texCoord;
    
    while(alpha < 1.0 && isInside(curTexCoord)){
        vec3 normal = normalize(NormalMatrix*(normalize(texture(volumeData,curTexCoord).stp)));
        curTexColor = texture(transferFunc,texture(volumeData,curTexCoord).q);
        float curAlpha = curTexColor.a;
        curShade = phong_illumination_model(normal,curTexColor.rgb, position);

        color = vec4(color.rgb* alpha + curShade*(1-alpha) ,alpha+=curAlpha);
        
        curTexCoord += direc * dist;
    }
}