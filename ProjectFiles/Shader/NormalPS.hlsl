SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> norm : register(t1);
// �����_�V�F�[�_����ʂ��Ă���摜(���f���{�̂������Ă���摜)
// ���������n���Ă���摜

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
    PS_OUTPUT outdata;
    
    float3 light = normalize(float3(1, 0.5, 1));
    
    float3 nmap = norm.Sample(smp, input.TexCoords0).rbg;
    nmap = nmap * 2 - 1;
    float3 N = normalize(nmap.x * input.Tangent - nmap.y * input.Binormal + nmap.z * input.Normal);
    
    const float ambient = 0.25;
    float bright = clamp(dot(N, -light), 0.25, 1.0);
    float4 col = tex.Sample(smp, input.TexCoords0);
    
    // ���邳�W��
    const float lightCoe = 2;
    
    outdata.color = float4(col.rgb * bright * lightCoe, col.a);
    // �[�x�l
    float dep = input.P.z / input.P.w; // w�Ŋ����Ă����Ȃ���0�`1�ɂȂ�Ȃ�
    dep = pow(dep, 20);
    outdata.depht = float4(dep, dep, dep, 1.0);
    
    return outdata;
}