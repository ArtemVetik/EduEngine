#include "pch.h"
#include "ParticleSystemD3D12.h"
#include "../Graphics/DynamicUploadBuffer.h"
#include <random>

namespace EduEngine
{
	ParticleSystemD3D12::ParticleSystemD3D12(RenderDeviceD3D12* device) :
		m_Device(device),
		m_DirtyBuffers(true),
		m_MaxParticles(0),
		m_Timer(0)
	{
		m_ComputePass = std::make_unique<ParticlesComputePass>(m_Device);
		m_DrawPass = std::make_unique<ParticlesDrawPass>(m_Device);
	}

	void ParticleSystemD3D12::Compute(Camera* camera, const Timer& timer, float aspectRatio)
	{
		if (m_MaxParticles == 0)
			return;

		if (EmissionRate == 0)
			return;

		m_Timer += timer.GetDeltaTime();

		float timeBetweenEmit = 1.0f / EmissionRate;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<uint32_t> dis(0, UINT32_MAX);

		ParticlesComputePass::PassData passCB = {};
		DirectX::XMStoreFloat4x4(&passCB.MVP, DirectX::XMMatrixTranspose(camera->GetViewProjMatrix()));
		passCB.AspectRatio = aspectRatio;
		passCB.DeltaTime = timer.GetDeltaTime();
		passCB.TotalTime = timer.GetTotalTime();
		passCB.EmitCount = m_Timer / timeBetweenEmit;
		passCB.RandSeed = dis(gen);

		DynamicUploadBuffer timeUploadBuffer(m_Device, QueueID::Direct);
		timeUploadBuffer.LoadData(passCB);
		timeUploadBuffer.CreateCBV();

		ParticlesComputePass::ParticleData particleData = {};
		particleData.SpawnType = ShapeType;
		particleData.SpawnSize = ShapeSize;
		particleData.MaxParticles = m_MaxParticles;
		particleData.LifeTime = LifeTime;
		particleData.CenterPos = CenterPos;
		particleData.StartColor = StartColor;
		particleData.EndColor = EndColor;
		particleData.Velocity = Velocity;
		particleData.Acceleration = Acceleration;

		DynamicUploadBuffer particleUploadBuffer(m_Device, QueueID::Direct);
		particleUploadBuffer.LoadData(particleData);

		if (m_DirtyBuffers)
			InitBuffers(m_MaxParticles);

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		commandContext.GetCmdList()->SetComputeRootSignature(m_ComputePass->GetD3D12RootSignature());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(0, timeUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetComputeRootConstantBufferView(1, particleUploadBuffer.GetAllocation().GPUAddress);
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(2, m_ParticlesPool->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(3, m_DeadList->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(4, m_DrawList->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(5, m_DrawArgs->GetUAVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetComputeRootDescriptorTable(6, m_DeadListCounter->GetUAVView()->GetGpuHandle());

		if (m_DirtyBuffers)
		{
			commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetDeadListInitPSO());
			commandContext.GetCmdList()->Dispatch(m_MaxParticles / 32 + 1, 1, 1);
		}

		if (m_Timer >= timeBetweenEmit)
		{
			commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetEmitPSO());
			commandContext.GetCmdList()->Dispatch(passCB.EmitCount / 32 + 1, 1, 1);
			m_Timer = fmod(m_Timer, timeBetweenEmit);
		}

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawList->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COPY_DEST));

		commandContext.GetCmdList()->CopyResource(m_DrawList->GetD3D12Resource(), m_DrawListUpload->GetD3D12Resource());

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawList->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetUpdatePSO());
		commandContext.GetCmdList()->Dispatch(m_MaxParticles / 32 + 1, 1, 1);

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::UAV(m_DrawList->GetD3D12Resource()));

		commandContext.GetCmdList()->SetPipelineState(m_ComputePass->GetCopyDrawPSO());
		commandContext.GetCmdList()->Dispatch(1, 1, 1);

		commandContext.GetCmdList()->SetPipelineState(m_DrawPass->GetD3D12PipelineState());
		commandContext.GetCmdList()->SetGraphicsRootSignature(m_DrawPass->GetD3D12RootSignature());
		commandContext.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(0, timeUploadBuffer.GetCBVDescriptorGPUHandle());
		commandContext.GetCmdList()->SetGraphicsRootConstantBufferView(1, particleUploadBuffer.GetAllocation().GPUAddress);
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(2, m_ParticlesPool->GetSRVView()->GetGpuHandle());
		commandContext.GetCmdList()->SetGraphicsRootDescriptorTable(3, m_DrawList->GetSRVView()->GetGpuHandle());

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawArgs->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT));

		commandContext.GetCmdList()->ExecuteIndirect(
			m_DrawPass->GetD3D12CommandRootSignature(),
			1,
			m_DrawArgs->GetD3D12Resource(),
			0,
			nullptr,
			0);

		commandContext.GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DrawArgs->GetD3D12Resource(),
			D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));

		m_DirtyBuffers = false;
	}

	void ParticleSystemD3D12::SetMaxParticles(UINT num)
	{
		m_MaxParticles = num;
		m_DirtyBuffers = true;
	}

	void ParticleSystemD3D12::InitBuffers(UINT particlesCount)
	{
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

#define CREATE_BUFFER(buffer, width)											 \
		desc.Width = width;														 \
		buffer = std::make_unique<BufferD3D12>(m_Device, desc, QueueID::Direct); \

#define CREATE_UAV(buffer, numElements, stride, counterOffset) \
		uavDesc.Buffer.NumElements = numElements;			   \
		uavDesc.Buffer.StructureByteStride = stride;		   \
		uavDesc.Buffer.CounterOffsetInBytes = counterOffset;   \
		buffer->CreateUAV(&uavDesc);						   \

#define CREATE_SRV(buffer, numElements, stride)		 \
		srvDesc.Buffer.NumElements = numElements;	 \
		srvDesc.Buffer.StructureByteStride = stride; \
		buffer->CreateSRV(&srvDesc);				 \

		CREATE_BUFFER(m_DeadList, AlignForUavCounter(particlesCount * sizeof(UINT)) + sizeof(UINT));
		CREATE_BUFFER(m_ParticlesPool, particlesCount * sizeof(Particle));
		CREATE_BUFFER(m_DrawList, AlignForUavCounter(particlesCount * sizeof(UINT)) + sizeof(UINT));
		CREATE_BUFFER(m_DrawArgs, AlignForUavCounter(9 * sizeof(UINT)) + sizeof(UINT));
		CREATE_BUFFER(m_DeadListCounter, sizeof(UINT));

		CREATE_UAV(m_DeadList, particlesCount, sizeof(UINT), AlignForUavCounter(particlesCount * sizeof(UINT)));
		CREATE_UAV(m_ParticlesPool, particlesCount, sizeof(Particle), 0);
		CREATE_UAV(m_DrawList, particlesCount, sizeof(UINT), AlignForUavCounter(particlesCount * sizeof(UINT)));
		CREATE_UAV(m_DrawArgs, 9, sizeof(UINT), AlignForUavCounter(9 * sizeof(UINT)));
		CREATE_UAV(m_DeadListCounter, 1, sizeof(UINT), 0);

		CREATE_SRV(m_ParticlesPool, particlesCount, sizeof(Particle));
		CREATE_SRV(m_DrawList, particlesCount, sizeof(UINT));

		auto& commandContext = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		for (auto& resource : { m_DeadList.get(), m_ParticlesPool.get(), m_DrawList.get(), m_DrawArgs.get(), m_DeadListCounter.get() })
		{
			commandContext.ResourceBarrier(CD3DX12_RESOURCE_BARRIER::Transition(resource->GetD3D12Resource(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		}
		commandContext.FlushResourceBarriers();

#undef CREATE_BUFFER
#undef CREATE_SRV
#undef CREATE_UAV

		auto uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(AlignForUavCounter(particlesCount * sizeof(UINT)) + sizeof(UINT));
		m_DrawListUpload = std::make_unique<UploadBufferD3D12>(m_Device, uploadDesc, QueueID::Direct);
	}
}