#pragma once

#include "cxx.h"

#include <cstdint>

namespace mod::patch
{
    void clear_DC_IC_Cache(void *ptr, uint32_t size);
    void writeStandardBranches(void *address, void *functionStart, void *functionBranchBack);
    void writeBranch(void *ptr, void *destination);
    void writeBranchBL(void *ptr, void *destination);
    void writeBranchMain(void *ptr, void *destination, uint32_t branch);
    void replaceEvtInstr(int32_t *instr, uint16_t opc);
    void writeEvtBranch(int32_t *instr, const int32_t *destination, bool async = false);

    // Replaces the data at `destination` with an arbitrary patch.
    void writePatch(void *destination, const void *patch_start, const void *patch_end);
    void writePatch(void *destination, const void *patch_start, uint32_t patch_len);
    void writePatch(void *destination, uint32_t patch_data);

    // Wrapper for writing pair of branches, one from the start of existing code
    // into new code, and one from the end of new code into existing code.
    void writeBranchPair(void *original_start, void *original_end, void *new_start, void *new_end);
    // Same, but assumes the original start and end points are adjacent.
    void writeBranchPair(void *original_start, void *new_start, void *new_end);

    template<typename Func, typename Dest>
    void writeBranch(Func func, Dest dst)
    {
        writeBranch(reinterpret_cast<void *>(func), reinterpret_cast<void *>(dst));
    }

    template<typename Func, typename Dest>
    void writeBranchBL(Func func, Dest dst)
    {
        writeBranchBL(reinterpret_cast<void *>(func), reinterpret_cast<void *>(dst));
    }

    template<typename Ptr, typename FuncStart, typename FuncEnd>
    void writeStandardBranches(Ptr ptr, FuncStart funcStart, FuncEnd funcEnd)
    {
        writeStandardBranches(reinterpret_cast<void *>(ptr),
                              reinterpret_cast<void *>(funcStart),
                              reinterpret_cast<void *>(funcEnd));
    }

    template<typename Func, typename Dest>
    Func hookFunction(Func function, Dest destination)
    {
        uint32_t *instructions = reinterpret_cast<uint32_t *>(function);
        uint32_t *trampoline = new uint32_t[2];

        // Original instruction
        trampoline[0] = instructions[0];
        clear_DC_IC_Cache(&trampoline[0], sizeof(uint32_t));

        // Branch to original function past hook
        writeBranch(&trampoline[1], &instructions[1]);

        // Write actual hook
        writeBranch(&instructions[0], reinterpret_cast<void *>(static_cast<Func>(destination)));

        return reinterpret_cast<Func>(trampoline);
    }

    void writeIntWithCache(void *ptr, uint32_t value);
} // namespace mod::patch
