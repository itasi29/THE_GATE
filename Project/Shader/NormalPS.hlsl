SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> norm : register(t1);
// �����_�V�F�[�_����ʂ��Ă���摜(���f���{�̂������Ă���摜)
// ���������n���Ă���摜

struct PointLight
{
    float3 pos;      // �ʒu
    float range;     // �L���͈�
    float intensity; // ����
    float3 color;    // �F
};

cbuffer LightBuffer : register(b4)
{
    PointLight lights[50];
    int lightNum;
}

struct PS_INPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float2 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 svPosition : SV_POSITION; // ���W( �v���W�F�N�V������� )
    float4 Position : POSITION0; // ���W( ���[���h��� )
    float4 P : POSITION1; // ���W( �v���W�F�N�V������� )
    float4 VPosition : POSITION2; // ���W( ���[���h��� * �r���[ )
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 depht : SV_Target1;
};

PS_OUTPUT main(PS_INPUT input) : SV_TARGET
{
    // �@���}�b�v����@���x�N�g�����擾
    float3 nmap = norm.Sample(smp, input.TexCoords0).rgb;
    nmap = normalize(nmap * 2 - 1);
    float3 N = normalize(nmap.x * input.Tangent - nmap.y * input.Binormal + nmap.z * input.Normal);
    
    PS_OUTPUT o;
    o.color = float4(N * 0.5 + 0.5, 1);
//    return o;
    
    float3 finalColor = float3(0, 0, 0);
    // �e�|�C���g���C�g����̉e�����v�Z
    for (int i = 0; i < lightNum; i++)
    {
        // ���C�g�܂ł̃x�N�g��
        float3 lightDir = lights[i].pos - input.Position.xyz;
        float distance = length(lightDir);
        
        // ���C�g�܂ł̋������L���͈͊O�Ȃ�X�L�b�v
        if (distance > lights[i].range) continue;
        
        // �����ɂ�錸��
        float attenuation = saturate(1 - distance / lights[i].range);
        
        // ���K��
        lightDir = normalize(lightDir);
        // ���C�g�̉e�������Z
        float NdotL = dot(N, lightDir);
        finalColor += lights[i].color * lights[i].intensity * attenuation * NdotL;
    }
    const float ambient = 0.35;
    float3 ambColor = float3(ambient, ambient, ambient);
    finalColor = saturate(finalColor + ambColor);
    
    float4 col = tex.Sample(smp, input.TexCoords0);
    
    PS_OUTPUT outdata;
    outdata.color = float4(col.rgb * finalColor, col.a);
// �[�x�l
    float dep = input.P.z / input.P.w; // w�Ŋ����Ă����Ȃ���0�`1�ɂȂ�Ȃ�
    dep = pow(dep, 20);
    outdata.depht = float4(dep, dep, dep, 1.0);
    
    return outdata;
}