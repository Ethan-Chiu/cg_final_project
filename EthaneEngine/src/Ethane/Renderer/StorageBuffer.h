//
//  StorageBuffer.hpp
//  EthaneEngine
//
//  Created by 邱奕翔 on 2023/5/28.
//

#pragma once

namespace Ethane {

    enum class BufferUsage
    {
        
    };

    class StorageBuffer
    {
    public:
        virtual ~StorageBuffer() {}
        
        virtual void Destroy() = 0;

        virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
        
        static Ref<StorageBuffer> Create(uint32_t size);
        static Ref<StorageBuffer> Create(void* data, uint32_t size);
    };

}
