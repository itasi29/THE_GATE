SamplerState smp : register(s0);
Texture2D<float4> tex : register(t0);
Texture2D<float4> rt : register(t1);
Texture2D<float4> dissolve : register(t2);

cbuffer UserData : register(b5)
{
    float3 center;
    float radius;
    float3 dir;
    float size;
    float frame;
    bool isCreate;
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

static const uint UINT_MAX = 0xffffffffu;

uint XorShift(uint val)
{
    val ^= (val << 13);
    val ^= (val >> 17);
    val ^= (val << 15);
    return val;
}

float Random(float val)
{
    float ref = float(XorShift(asuint(val))) / float(UINT_MAX);
    return ref;
}

float3 GetNearPos(float3 c, float3 d, float s, float3 p)
{
    float3 hDir = d * s * 0.5;
    float3 start = c + hDir;
    float3 end = c - hDir;
    
    float3 sToE = end - start;
    float3 sToP = p - start;
    float rate = dot(sToE, sToP) / (sToE.x * sToE.x + sToE.y * sToE.y + sToE.z * sToE.z);
    rate = max(min(rate, 1.0), 0.0);
    
    return start + sToE * rate;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // RT�̉摜�擾�ꏊ�v�Z
    float2 location;
    location.x = input.P.x / input.P.w;
    location.y = input.P.y / input.P.w;
    location.y *= -1;
    location.x = (location.x + 1) * 0.5;
    location.y = (location.y + 1) * 0.5;
    
    // �Q�[�g�̉��ɋ߂Â��قǘg�����o��
    float3 near = GetNearPos(center, dir, size, input.Position.xyz);
    float dis = length(near - input.Position.xyz);
    float rate = (dis / radius);
    // �g�����������炷���߂Ɏ擾
    float val = frame * 0.001;
    float rand = dissolve.Sample(smp, float2(input.TexCoords0.x + sin(val), input.TexCoords0.y + cos(val))).r;
    rand = rand * 2 - 1;
    // �g�����o���ꍇ
    if (rate > 0)
    {
        // �͈͂�������
        rate = pow(rate, 5);
        // ��������
        rate = rate * 0.9 + rand * 0.1;
    }
    // �ǉ��F(���C��)
    float4 addMain = float4(0, 0, 0, 1);
    if (!isCreate)
    {
        addMain.rbg = dissolve.Sample(smp, float2(input.TexCoords0.y + sin(val + 1.73), input.TexCoords0.x + cos(val + 2.78))).rbg * -1;
    }
    // �ǉ��F(�g��)
    float add = rand * rate * 0.5;
    float4 addBase = float4(add, add, add, 1);
    
    float4 main = rt.Sample(smp, location);
    float4 base = tex.Sample(smp, location);
    return lerp(main + addMain, base + addBase, rate);
}