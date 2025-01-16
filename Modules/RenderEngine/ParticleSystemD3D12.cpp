#include "pch.h"
#include "ParticleSystemD3D12.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include <random>

namespace EduEngine
{
	ParticleSystemD3D12::ParticleSystemD3D12(RenderDeviceD3D12* device, const RenderSettings* renderSettings) :
		m_Device(device),
		m_RenderSettings(renderSettings),
		m_LastRenderCamera(nullptr),
		m_ColorTexture(nullptr),
		m_DirtyBuffers(true),
		m_ParticleData{},
		m_Timer(0)
	{
		m_AsyncCompute = m_RenderSettings->GetAsyncComputeParticles();
		m_DrawPass = std::make_unique<ParticlesDrawPass>(m_Device);
		InitComputePass(false);
	}

	void ParticleSystemD3D12::Compute(const Timer& timer, D3D12_GPU_DESCRIPTOR_HANDLE normalTex, const SwapChain* swapChain)
	{
		if (m_ParticleData.BufferData.MaxParticles == 0)
			return;

		if (m_ParticleData.EmissionRate == 0)
			return;

		if (m_ParticleData.EmissionEnabled)
			m_Timer += timer.GetDeltaTime();

		if (m_AsyncCompute != m_RenderSettings->GetAsyncComputeParticles())
		{
			m_DirtyBuffers = true;
		}

		if (m_DirtyBuffers)
		{
			InitBuffers(m_ParticleData.BufferData.MaxParticles);
			SetParticlesData(m_ParticleData);
		}

		float timeBetweenEmit = 1.0f / m_ParticleData.EmissionRate;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);

		ParticlesComputePass::PassData passCB = {};
		if (m_LastRenderCamera)
		{
			XMStoreFloat4x4(&passCB.WorldView, XMMatrixTranspose(XMLoadFloat4x4(&m_LastRenderCamera->GetViewMatrix())));
			XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(XMLoadFloat4x4(&m_LastRenderCamera->GetProjectionMatrix())));
			passCB.Near = m_LastRenderCamera->GetNear();
			passCB.Far = m_LastRenderCamera->GetFar();
		}
		passCB.AspectRatio = (float)swapChain->GetWidth() / (float)swapChain->GetHeight();
		passCB.DeltaTime = timer.GetDeltaTime();
		passCB.TotalTime = timer.GetTotalTime();
		passCB.EmitCount = m_Timer / timeBetweenEmit;
		passCB.RandSeed = dis(gen);
		passCB.CenterPos = m_CenterPos;

		DynamicUploadBuffer timeUploadBuffer(m_Device, m_AsyncCompute ? QueueID::Compute : QueueID::Direct);
		timeUploadBuffer.LoadData(passCB);
		timeUploadBuffer.CreateCBV();

		auto contextType = m_AsyncCompute ? D3D12_COMMAND_LIST_TYPE_COMPUTE : D3D12_COMMAND_LIST_TYPE_DIRECT;
		auto& commandContext = m_Device->GetCommandContext(contextType);

		commandContext.GetCmdList()->SetComputeRootSignature(m_ComputePass->GetD3D12RootSignature());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(0, timeUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetComputeRootConstantBufferView(1, m_ParticleDataBuffer->GetD3D12Resource()->GetGPUVirtualAddress());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(2, m_ParticlesPool->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(3, m_DeadList->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(4, m_DrawList->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(5, m_DrawArgs->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(6, m_DeadListCounter->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(7, swapChain->DepthStencilSRVView());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(8, normalTex);

		if (m_DirtyBuffers)
		{
			commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetDeadListInitPSO());
			commandContext.GetCmdList()->Dispatch(m_ParticleData.BufferData.MaxParticles / 32 + 1, 1, 1);
		}

		if (m_Timer >= timeBetweenEmit)
		{
			commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetEmitPSO());
			commandContext.GetCmdList()->Dispatch(passCB.EmitCount / 32 + 1, 1, 1);
			m_Timer = fmod(m_Timer, timeBetweenEmit);
		}

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawList->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST));

		commandContext.GetCmdList()->CopyBufferRegion(
			m_DrawList->GetD3D12Resource(),
			AlignForUavCounter(m_ParticleData.BufferData.MaxParticles * sizeof(UINT)),
			m_DrawListUpload->GetD3D12Resource(),
			0,
			sizeof(UINT)
		);

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawList->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetUpdatePSO());
		commandContext.GetCmdList()->Dispatch(m_ParticleData.BufferData.MaxParticles / 32 + 1, 1, 1);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_DrawArgs->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::UAV(m_DrawList->GetD3D12Resource()));
		commandContext.FlushResourceBarriers();

		commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetCopyDrawPSO());
		commandContext.GetCmdList()->Dispatch(1, 1, 1);

		commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(m_DrawArgs->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT));
		commandContext.FlushResourceBarriers();

		m_DirtyBuffers = false;
	}

	void ParticleSystemD3D12::Draw(Camera* camera, const Timer& timer, float aspectRatio)
	{
		if (m_ParticleData.BufferData.MaxParticles == 0)
			return;

		if (m_ParticleData.EmissionRate == 0)
			return;

		ParticlesComputePass::PassData passCB = {};
		DirectX::XMStoreFloat4x4(&passCB.MVP, DirectX::XMMatrixTranspose(camera->GetViewProjMatrix()));
		passCB.AspectRatio = aspectRatio;

		DynamicUploadBuffer timeUploadBuffer(m_Device, QueueID::Direct);
		timeUploadBuffer.LoadData(passCB);
		timeUploadBuffer.CreateCBV();

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.GetCmdList()->SetPipelineState(m_DrawPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_DrawPass->GetD3D12RootSignature());
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, timeUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(2, m_ParticlesPool->GetSRVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(3, m_DrawList->GetSRVView()->GetGpuHandle());

		if (m_ColorTexture && m_ColorTexture->GetGPUPtr())
		{
			D3D12_GPU_DESCRIPTOR_HANDLE colorTex;
			colorTex.ptr = reinterpret_cast<UINT64>(m_ColorTexture->GetGPUPtr());
			commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(4, colorTex);
		}

		commandContext.GetCmdList()->ExecuteIndirect(
			m_DrawPass->GetD3D12CommandRootSignature(),
			1,
			m_DrawArgs->GetD3D12Resource(),
			0,
			nullptr,
			0);

		m_LastRenderCamera = camera;
	}

	void ParticleSystemD3D12::SetParticlesData(ParticleStaticData data)
	{
		if (m_ParticleData.BufferData.MaxParticles != data.BufferData.MaxParticles)
			m_DirtyBuffers = true;

		if (m_ParticleData.ScreenSpaceCollision != data.ScreenSpaceCollision)
			InitComputePass(data.ScreenSpaceCollision);

		if (memcmp(&m_ParticleData.BufferData, &data.BufferData, sizeof(ParticleStaticBufferData)) == 0)
		{
			m_ParticleData = data;
			return;
		}
		
		m_ParticleData = data;

		ParticlesComputePass::ParticleData uploadData = {};
		uploadData.BufferData = data.BufferData;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = sizeof(ParticlesComputePass::ParticleData);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		m_ParticleDataBuffer = std::make_unique<BufferD3D12>(m_Device, desc, &uploadData, m_AsyncCompute ? QueueID::Compute : QueueID::Direct);
	}

	void ParticleSystemD3D12::SetCenterPos(DirectX::XMFLOAT3 pos)
	{
		m_CenterPos = pos;
	}

	void ParticleSystemD3D12::SetColorTexture(ITexture* texture)
	{
		if (m_ColorTexture)
			m_ColorTexture->Free();

		m_ColorTexture = dynamic_cast<TextureD3D12Impl*>(texture);

		if (m_ColorTexture)
			m_ColorTexture->Load();
	}

	UINT ParticleSystemD3D12::GetMaxParticles() const
	{
		return m_ParticleData.BufferData.MaxParticles;
	}

	void ParticleSystemD3D12::InitBuffers(UINT particlesCount)
	{
		m_AsyncCompute = m_RenderSettings->GetAsyncComputeParticles();

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;

#define CREATE_BUFFER(buffer, width, name)											\
		desc.Width = width;															\
		buffer = std::make_unique<BufferD3D12>(m_Device, desc,						\
								 m_AsyncCompute ? QueueID::Both : QueueID::Direct); \
		buffer->SetName(name);														\

#define CREATE_UAV(buffer, numElements, stride, counterOffset) \
		uavDesc.Buffer.NumElements = numElements;			   \
		uavDesc.Buffer.StructureByteStride = stride;		   \
		uavDesc.Buffer.CounterOffsetInBytes = counterOffset;   \
		buffer->CreateUAV(&uavDesc);						   \

#define CREATE_SRV(buffer, numElements, stride)		 \
		srvDesc.Buffer.NumElements = numElements;	 \
		srvDesc.Buffer.StructureByteStride = stride; \
		buffer->CreateSRV(&srvDesc);				 \

		CREATE_BUFFER(m_DeadList, AlignForUavCounter(particlesCount * sizeof(UINT)) + sizeof(UINT), L"ParticlesDeadList");
		CREATE_BUFFER(m_ParticlesPool, particlesCount * sizeof(Particle), L"ParticlesPool");
		CREATE_BUFFER(m_DrawList, AlignForUavCounter(particlesCount * sizeof(UINT)) + sizeof(UINT), L"ParticlesDrawList");
		CREATE_BUFFER(m_DrawArgs, AlignForUavCounter(9 * sizeof(UINT)) + sizeof(UINT), L"ParticlesDrawArgs");
		CREATE_BUFFER(m_DeadListCounter, sizeof(UINT), L"ParticlesDeadListCounter");

		CREATE_UAV(m_DeadList, particlesCount, sizeof(UINT), AlignForUavCounter(particlesCount * sizeof(UINT)));
		CREATE_UAV(m_ParticlesPool, particlesCount, sizeof(Particle), 0);
		CREATE_UAV(m_DrawList, particlesCount, sizeof(UINT), AlignForUavCounter(particlesCount * sizeof(UINT)));
		CREATE_UAV(m_DrawArgs, 9, sizeof(UINT), AlignForUavCounter(9 * sizeof(UINT)));
		CREATE_UAV(m_DeadListCounter, 1, sizeof(UINT), 0);

		CREATE_SRV(m_ParticlesPool, particlesCount, sizeof(Particle));
		CREATE_SRV(m_DrawList, particlesCount, sizeof(UINT));

		auto contextType = m_AsyncCompute ? D3D12_COMMAND_LIST_TYPE_COMPUTE : D3D12_COMMAND_LIST_TYPE_DIRECT;
		auto beforeState = m_AsyncCompute ? D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE : D3D12_RESOURCE_STATE_GENERIC_READ;
		auto& commandContext = m_Device->GetCommandContext(contextType);

		for (auto& resource : { m_DeadList.get(), m_ParticlesPool.get(), m_DrawList.get(), m_DeadListCounter.get() })
		{
			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource->GetD3D12Resource(),
				beforeState, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		}

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawArgs->GetD3D12Resource(),
			beforeState, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT));

		commandContext.FlushResourceBarriers();

#undef CREATE_BUFFER
#undef CREATE_SRV
#undef CREATE_UAV

		auto uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(UINT));
		m_DrawListUpload = std::make_unique<UploadBufferD3D12>(m_Device, uploadDesc, m_AsyncCompute ? QueueID::Both : QueueID::Direct);
	}

	void ParticleSystemD3D12::InitComputePass(bool screenSpaceCollision)
	{
		D3D_SHADER_MACRO macros[] =
		{
			{ "SCREEN_SPACE_COLLISION", screenSpaceCollision ? "1" : "0"},
			{ NULL, NULL }
		};

		m_ComputePass = std::make_unique<ParticlesComputePass>(m_Device, m_AsyncCompute ? QueueID::Compute : QueueID::Direct, macros);
	}
}