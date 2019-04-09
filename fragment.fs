uniform sampler2D u_Texture;

uniform vec3 lightPosition = {0.0, 10.0, 0.0};
uniform vec3 lightColor = {0.9,0.33,0.06};
uniform vec3 u_Cam;
uniform vec3 ambientAmount;
uniform vec3 diffuseAmount;
uniform vec3 specularAmount;
uniform float u_Shiny;

varying vec3 v_Normal;
varying vec2 v_UV;
varying vec3 FragPos;

void main()
{
    vec4 sample = texture2D(u_Texture, v_UV);
    vec3 ambient = ambientAmount * lightColor;

    const int levels = 3; // number of colors that can be produced
    const float scaleFactor = 1.0 / levels;

    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = floor(diff * levels + 1) * diffuseAmount * lightColor * scaleFactor;

    vec3 viewDir = normalize(u_Cam - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shiny * 2);
    vec3 specular = spec * specularAmount * lightColor;

    float edgeDetection = (dot(viewDir, norm) > 0.25) ? 1.0 : 0.0;
    float specMask = (pow(dot(normalize(lightDir + viewDir), norm), u_Shiny) > 0.01) ? 1 : 0;

    vec3 amount = ambient + diffuse + specular * specMask;
    gl_FragColor = vec4(amount, 1.0) * sample * edgeDetection;
}