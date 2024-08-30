#include "hooks.hpp"

namespace utils::hooks {


void nop(void *place, const size_t length)
{
    DWORD old_protect{};
    VirtualProtect(place, length, PAGE_EXECUTE_READWRITE, &old_protect);

    std::memset(place, 0x90, length);


    VirtualProtect(place, length, old_protect, &old_protect);
    FlushInstructionCache(GetCurrentProcess(), place, length);
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


}// namespace utils::hooks