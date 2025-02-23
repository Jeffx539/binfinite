#include "hooks.hpp"
#include <functional>

namespace utils::hooks {


void nop(void *place, const size_t length)
{
    DWORD old_protect{};
    VirtualProtect(place, length, PAGE_EXECUTE_READWRITE, &old_protect);

    std::memset(place, 0x90, length);


    VirtualProtect(place, length, old_protect, &old_protect);
    FlushInstructionCache(GetCurrentProcess(), place, length);
}

void *assemble(const std::function<void(assembler &)> &asm_function)
{
    static asmjit::JitRuntime runtime;

    asmjit::CodeHolder code;
    code.init(runtime.environment());

    assembler a(&code);

    asm_function(a);

    void *result = nullptr;
    auto err_result = runtime.add(&result, &code);

    if (err_result != asmjit::ErrorCode::kErrorOk) {
        printf("ASMJIT ERROR: %s\n", asmjit::DebugUtils::errorAsString(err_result));
    }

    return result;
}


void nop(const size_t place, const size_t length) { nop(reinterpret_cast<void *>(place), length); }


void *initialize_min_hook()
{
    static class min_hook_init
    {
      public:
        min_hook_init()
        {
            if (MH_Initialize() != MH_OK) { throw std::runtime_error("Failed to initialize MinHook"); }
        }

        ~min_hook_init() { MH_Uninitialize(); }
    } min_hook_init;
    return &min_hook_init;
}


detour::detour() { (void)initialize_min_hook(); }

detour::detour(const size_t place, void *target) : detour(reinterpret_cast<void *>(place), target) {}

detour::detour(void *place, void *target) : detour() { this->create(place, target); }

detour::~detour() { this->clear(); }

void detour::enable() { MH_EnableHook(this->place_); }

void detour::disable() { MH_DisableHook(this->place_); }

void detour::create(void *place, void *target)
{
    this->clear();
    this->place_ = place;

    if (MH_CreateHook(this->place_, target, &this->original_) != MH_OK) { throw std::runtime_error("Unable to hook"); }

    this->enable();
}

void detour::create(const size_t place, void *target) { this->create(reinterpret_cast<void *>(place), target); }

void detour::clear()
{
    if (this->place_) { MH_RemoveHook(this->place_); }

    this->place_ = nullptr;
    this->original_ = nullptr;
    this->moved_data_ = {};
}


void *detour::get_place() const { return this->place_; }

void *detour::get_original() const { return this->original_; }


	void assembler::pushad64()
{
    this->push(rax);
    this->push(rcx);
    this->push(rdx);
    this->push(rbx);
    this->push(rsp);
    this->push(rbp);
    this->push(rsi);
    this->push(rdi);

    this->push(r8);
    this->push(r9);
    this->push(r10);
    this->push(r11);
    this->push(r12);
    this->push(r13);
    this->push(r14);
    this->push(r15);
}

void assembler::popad64()
{
    this->pop(r15);
    this->pop(r14);
    this->pop(r13);
    this->pop(r12);
    this->pop(r11);
    this->pop(r10);
    this->pop(r9);
    this->pop(r8);

    this->pop(rdi);
    this->pop(rsi);
    this->pop(rbp);
    this->pop(rsp);
    this->pop(rbx);
    this->pop(rdx);
    this->pop(rcx);
    this->pop(rax);
}

void assembler::prepare_stack_for_call()
{
    const auto reserve_callee_space = this->newLabel();
    const auto stack_unaligned = this->newLabel();

    this->test(rsp, 0xF);
    this->jnz(stack_unaligned);

    this->sub(rsp, 0x8);
    this->push(rsp);

    this->push(rax);
    this->mov(rax, ptr(rsp, 8, 8));
    this->add(rax, 0x8);
    this->mov(ptr(rsp, 8, 8), rax);
    this->pop(rax);

    this->jmp(reserve_callee_space);

    this->bind(stack_unaligned);
    this->push(rsp);

    this->bind(reserve_callee_space);
    this->sub(rsp, 0x40);
}

void assembler::restore_stack_after_call()
{
    this->lea(rsp, ptr(rsp, 0x40));
    this->pop(rsp);
}

asmjit::Error assembler::call(void *target) { return Assembler::call(reinterpret_cast<size_t>(target)); }

asmjit::Error assembler::jmp(void *target) { return Assembler::jmp(reinterpret_cast<size_t>(target)); }




}// namespace utils::hooks