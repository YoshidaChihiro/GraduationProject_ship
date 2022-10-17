#include "ComputeWrapper.h"

#include <cassert>

#include "DirectXLib.h"
#define InterlockedGetValue(object) InterlockedCompareExchange(object, 0, 0)

ComputeWrapper* ComputeWrapper::GetInstance()
{
    static ComputeWrapper instance;
    return &instance;
}

void ComputeWrapper::Initialize()
{
    dev = DirectXLib::GetInstance()->GetDevice();

    D3D12_COMMAND_QUEUE_DESC queueDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE };
    auto result = dev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
    assert(SUCCEEDED(result));
    result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&cmdAllocator));
    assert(SUCCEEDED(result));

    result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
    assert(SUCCEEDED(result));

    result = dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    assert(SUCCEEDED(result));

    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr)
    {
        assert(SUCCEEDED(result));
    }

    //m_threadData[threadIndex].pContext = this;
    //m_threadData[threadIndex].threadIndex = threadIndex;

    //threadHandle = CreateThread(
    //    nullptr,
    //    0,
    //    reinterpret_cast<LPTHREAD_START_ROUTINE>(ThreadProc),
    //    reinterpret_cast<void*>(this),
    //    CREATE_SUSPENDED,
    //    nullptr);

    //ResumeThread(threadHandle);


    //Create();
}

void ComputeWrapper::End()
{
    //InterlockedExchange(&m_terminating, 1);
    //WaitForMultipleObjects(1, &threadHandle, TRUE, INFINITE);

    //CloseHandle(threadHandle);
    //CloseHandle(fenceEvent);

}

//void ComputeWrapper::Create()
//{
//    HRESULT result;
//    // Create compute resources.
//    D3D12_COMMAND_QUEUE_DESC queueDesc = { D3D12_COMMAND_LIST_TYPE_COMPUTE, 0, D3D12_COMMAND_QUEUE_FLAG_NONE };
//    result = dev->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&cmdQueue));
//    assert(SUCCEEDED(result));
//
//	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE, IID_PPV_ARGS(&cmdAllocator));
//    assert(SUCCEEDED(result));
//
//	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COMPUTE, cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
//    assert(SUCCEEDED(result));
//
//	result = dev->CreateFence(0, D3D12_FENCE_FLAG_SHARED, IID_PPV_ARGS(&fence));
//    assert(SUCCEEDED(result));
//
//
//}

void ComputeWrapper::AsyncComputeThreadProc()
{
    ID3D12CommandQueue* pCommandQueue = cmdQueue.Get();
    ID3D12CommandAllocator* pCommandAllocator = cmdAllocator.Get();
    ID3D12GraphicsCommandList* pCommandList = cmdList.Get();
    ID3D12Fence* pFence = fence.Get();

    //while (0 == InterlockedGetValue(&m_terminating))
    //{
        // Run the particle simulation.
        if (shades.size() != 0)
            shades[0]->Dispatch();

        // Close and execute the command list.
        auto result = pCommandList->Close();
        assert(SUCCEEDED(result));

        ID3D12CommandList* ppCommandLists[] = { pCommandList };

        //PIXBeginEvent(pCommandQueue, 0, L"Thread %d: Iterate on the particle simulation", threadIndex);
        pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
        //PIXEndEvent(pCommandQueue);

        // Wait for the compute shader to complete the simulation.
        UINT64 threadFenceValue = InterlockedIncrement(&m_threadFenceValue);
        result = pCommandQueue->Signal(pFence, threadFenceValue);
        assert(SUCCEEDED(result));
        result = pFence->SetEventOnCompletion(threadFenceValue, fenceEvent);
        assert(SUCCEEDED(result));

        WaitForSingleObject(fenceEvent, INFINITE);

        // Wait for the render thread to be done with the SRV so that
        // the next frame in the simulation can run.
        UINT64 renderContextFenceValue = InterlockedGetValue(&DirectXLib::GetInstance()->GetCurrentFenceVal());
        if (DirectXLib::GetInstance()->GetFence()->GetCompletedValue() < renderContextFenceValue)
        {
            result = pCommandQueue->Wait(DirectXLib::GetInstance()->GetFence().Get(), renderContextFenceValue);
            assert(SUCCEEDED(result));
            //InterlockedExchange(&DirectXLib::GetInstance()->GetCurrentFenceVal(), 0);
        }

        // Prepare for the next frame.
        result = pCommandAllocator->Reset();
        assert(SUCCEEDED(result));
    	
        result = pCommandList->Reset(pCommandAllocator, ComputeShade::GetPipeline());
        assert(SUCCEEDED(result));

    //}

    //return 0;
}
