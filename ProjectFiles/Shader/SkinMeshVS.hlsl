// �\���̒�` --------------------------------------------------------------------

// �}�e���A���p�����[�^
struct DX_D3D11_CONST_MATERIAL
{
    float4 Diffuse; // �f�B�t���[�Y�J���[
    float4 Specular; // �X�y�L�����J���[
    float4 Ambient_Emissive; // �}�e���A���G�~�b�V�u�J���[ + �}�e���A���A���r�G���g�J���[ * �O���[�o���A���r�G���g�J���[

    float Power; // �X�y�L�����̋���
    float TypeParam0; // �}�e���A���^�C�v�p�����[�^0
    float TypeParam1; // �}�e���A���^�C�v�p�����[�^1
    float TypeParam2; // �}�e���A���^�C�v�p�����[�^2
};

// �}�N����` -----------------------------------

#define DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM			3			// �e�N�X�`�����W�ϊ��s��̓]�u�s��̐�
#define DX_D3D11_VS_CONST_WORLD_MAT_NUM					54			// �g���C�A���O�����X�g��œ����Ɏg�p���郍�[�J�������[���h�s��̍ő吔

// �f�[�^�^��` ---------------------------------

// ��{�p�����[�^
struct DX_D3D11_VS_CONST_BUFFER_BASE
{
    float4x4 AntiViewportMatrix; // �A���`�r���[�|�[�g�s��
    float4x4 ProjectionMatrix; // �r���[�@���@�v���W�F�N�V�����s��
    float4 ViewMatrix[3]; // ���[���h�@���@�r���[�s��
    float4 LocalWorldMatrix[3]; // ���[�J���@���@���[���h�s��

    float4 ToonOutLineSize; // �g�D�[���̗֊s���̑傫��
    float DiffuseSource; // �f�B�t���[�Y�J���[( 0.0f:�}�e���A��  1.0f:���_ )
    float SpecularSource; // �X�y�L�����J���[(   0.0f:�}�e���A��  1.0f:���_ )
    float MulSpecularColor; // �X�y�L�����J���[�l�ɏ�Z����l( �X�y�L�������������Ŏg�p )
    float Padding;
};

// ���̑��̍s��
struct DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX
{
    float4 ShadowMapLightViewProjectionMatrix[3][4]; // �V���h�E�}�b�v�p�̃��C�g�r���[�s��ƃ��C�g�ˉe�s�����Z��������
    float4 TextureMatrix[DX_D3D11_VS_CONST_TEXTURE_MATRIX_NUM][2]; // �e�N�X�`�����W����p�s��
};

// �X�L�j���O���b�V���p�́@���[�J���@���@���[���h�s��
struct DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX
{
	float4 mat[ DX_D3D11_VS_CONST_WORLD_MAT_NUM*3 ] ;					// ���[�J���@���@���[���h�s��
};



// ��{�p�����[�^
cbuffer cbD3D11_CONST_BUFFER_VS_BASE : register(b1)
{
    DX_D3D11_VS_CONST_BUFFER_BASE g_Base;
};

// ���̑��̍s��
cbuffer cbD3D11_CONST_BUFFER_VS_OTHERMATRIX : register(b2)
{
    DX_D3D11_VS_CONST_BUFFER_OTHERMATRIX g_OtherMatrix;
};

// �X�L�j���O���b�V���p�́@���[�J���@���@���[���h�s��
cbuffer cbD3D11_CONST_BUFFER_VS_LOCALWORLDMATRIX : register(b3)
{
    DX_D3D11_VS_CONST_BUFFER_LOCALWORLDMATRIX g_LocalWorldMatrix;
};

struct VS_INPUT
{
    float3 Position : POSITION; // ���W( ���[�J����� )
    float3 Normal : NORMAL0; // �@��( ���[�J����� )
    float3 Tangent : TANGENT0; // �ڐ�x�N�g��
    float3 Binormal : BINORMAL0; // �]�@���x�N�g��
    float4 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 TexCoords1 : TEXCOORD1; // �T�u�e�N�X�`�����W
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    
    // �@������
    
	// �X�L�j���O���b�V��
    int4 BlendIndices0 : BLENDINDICES0; // �{�[�������p Float�^�萔�z��C���f�b�N�X�O
    float4 BlendWeight0 : BLENDWEIGHT0; // �{�[�������p�E�G�C�g�l�O
};

// ���_�V�F�[�_�[�̏o��
struct VS_OUTPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
    float2 TexCoords0 : TEXCOORD0; // �e�N�X�`�����W
    float4 svPosition : SV_POSITION; // ���W( �v���W�F�N�V������� )    //P�Ɠ��l
    float4 Position : POSITION0; // ���W(���[���h��� )
    float4 P : POSITION1; // ���W( �v���W�F�N�V������� )               //3D��2D�ɐ��K�����ꂽ���W
    float4 VPosition : POSITION2; // ���W(���[���h���*�r���[ )
};

VS_OUTPUT main(VS_INPUT VSInput)
{
    VS_OUTPUT VSOutput;
    float4 lLocalPosition;
    float4 lWorldPosition;
    float4 lViewPosition;
    
    int4 lBoneFloatIndex = VSInput.BlendIndices0;
    float4 lLocalWorldMatrix[3];
    // �d�݂Â����Z
    lLocalWorldMatrix[0] = g_LocalWorldMatrix.mat[lBoneFloatIndex.x + 0] * VSInput.BlendWeight0.x;
    lLocalWorldMatrix[1] = g_LocalWorldMatrix.mat[lBoneFloatIndex.x + 1] * VSInput.BlendWeight0.x;
    lLocalWorldMatrix[2] = g_LocalWorldMatrix.mat[lBoneFloatIndex.x + 2] * VSInput.BlendWeight0.x;

    lLocalWorldMatrix[0] += g_LocalWorldMatrix.mat[lBoneFloatIndex.y + 0] * VSInput.BlendWeight0.y;
    lLocalWorldMatrix[1] += g_LocalWorldMatrix.mat[lBoneFloatIndex.y + 1] * VSInput.BlendWeight0.y;
    lLocalWorldMatrix[2] += g_LocalWorldMatrix.mat[lBoneFloatIndex.y + 2] * VSInput.BlendWeight0.y;

    lLocalWorldMatrix[0] += g_LocalWorldMatrix.mat[lBoneFloatIndex.z + 0] * VSInput.BlendWeight0.z;
    lLocalWorldMatrix[1] += g_LocalWorldMatrix.mat[lBoneFloatIndex.z + 1] * VSInput.BlendWeight0.z;
    lLocalWorldMatrix[2] += g_LocalWorldMatrix.mat[lBoneFloatIndex.z + 2] * VSInput.BlendWeight0.z;

    lLocalWorldMatrix[0] += g_LocalWorldMatrix.mat[lBoneFloatIndex.w + 0] * VSInput.BlendWeight0.w;
    lLocalWorldMatrix[1] += g_LocalWorldMatrix.mat[lBoneFloatIndex.w + 1] * VSInput.BlendWeight0.w;
    lLocalWorldMatrix[2] += g_LocalWorldMatrix.mat[lBoneFloatIndex.w + 2] * VSInput.BlendWeight0.w;
    

    // ���W�v�Z( ���[�J�����r���[���v���W�F�N�V���� )
    lLocalPosition.xyz = VSInput.Position;
    lLocalPosition.w = 1.0f;
    lWorldPosition.x = dot(lLocalPosition, lLocalWorldMatrix[0]);
    lWorldPosition.y = dot(lLocalPosition, lLocalWorldMatrix[1]);
    lWorldPosition.z = dot(lLocalPosition, lLocalWorldMatrix[2]);
    lWorldPosition.w = 1.0f;

    lViewPosition.x = dot(lWorldPosition, g_Base.ViewMatrix[0]);
    lViewPosition.y = dot(lWorldPosition, g_Base.ViewMatrix[1]);
    lViewPosition.z = dot(lWorldPosition, g_Base.ViewMatrix[2]);
    lViewPosition.w = 1.0f;

    VSOutput.Position.x = dot(lViewPosition, g_Base.ProjectionMatrix[0]);
    VSOutput.Position.y = dot(lViewPosition, g_Base.ProjectionMatrix[1]);
    VSOutput.Position.z = dot(lViewPosition, g_Base.ProjectionMatrix[2]);
    VSOutput.Position.w = dot(lViewPosition, g_Base.ProjectionMatrix[3]);

    VSOutput.Position = lWorldPosition;
    VSOutput.VPosition = lViewPosition;
    VSOutput.P = mul(lViewPosition, g_Base.ProjectionMatrix);
    VSOutput.svPosition = VSOutput.P;
    VSOutput.TexCoords0 = VSInput.TexCoords0;
    
    float4 normal = float4(VSInput.Normal, 0);
    VSOutput.Normal.x = dot(normal, lLocalWorldMatrix[0]);
    VSOutput.Normal.y = dot(normal, lLocalWorldMatrix[1]);
    VSOutput.Normal.z = dot(normal, lLocalWorldMatrix[2]);
    VSOutput.Normal = normalize(VSOutput.Normal);

    float4 tangent = float4(VSInput.Tangent, 0);
    VSOutput.Tangent.x = dot(tangent, lLocalWorldMatrix[0]);
    VSOutput.Tangent.y = dot(tangent, lLocalWorldMatrix[1]);
    VSOutput.Tangent.z = dot(tangent, lLocalWorldMatrix[2]);
    VSOutput.Tangent = normalize(VSOutput.Tangent);

    float4 binormal = float4(VSInput.Binormal, 0);
    VSOutput.Binormal.x = dot(binormal, lLocalWorldMatrix[0]);
    VSOutput.Binormal.y = dot(binormal, lLocalWorldMatrix[1]);
    VSOutput.Binormal.z = dot(binormal, lLocalWorldMatrix[2]);
    VSOutput.Binormal = normalize(VSOutput.Binormal);
    
    return VSOutput;
}