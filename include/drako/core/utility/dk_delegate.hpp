#pragma once
#ifndef DRAKO_DELEGATE_HPP
#define DRAKO_DELEGATE_HPP

namespace drako
{
    // ABSTRACT CLASS TEMPLATE
    // Base class of delegates objects.
    //
    template <typename Return, typename ...Args>
    class Delegate
    {
    public:

    private:
    };


    // CLASS TEMPLATE
    // Delegate object that supports a single callable target.
    //
    template <typename Return, typename ...Args>
    class SinglecastDelegate
    {
        typedef void* InstancePtr;
        typedef void(*InternalFunction)(InstancePtr, int);
        typedef std::pair<InstancePtr, InternalFunction> Stub;

        // turns a free function into our internal function stub
        template <void(*Function)(int)>
        static inline void FunctionStub(InstancePtr, int ARG0)
        {
            // we don't need the instance pointer because we're dealing with free functions
            return (Function)(ARG0);
        }

        // turns a member function into our internal function stub
        template <typename C, void (C::* Function)(int)>
        static inline void ClassMethodStub(InstancePtr instance, int ARG0)
        {
            // cast the instance pointer back into the original class instance
            return (static_cast<C*>(instance)->*Function)(ARG0);
        }

    public:

        explicit SinglecastDelegate()
            : _callable(nullptr, nullptr)
        {
        }

       // Binds a free function
        template <void(*Function)(int)>
        void Bind()
        {
            _callable.first = nullptr;
            _callable.second = &FunctionStub<Function>;
        }

        // Binds a class method
        template <typename C, void (C::* Function)(int)>
        void Bind(C* instance)
        {
            _callable.first = instance;
            _callable.second = &ClassMethodStub<C, Function>;
        }

        // Invokes the delegate
        void Invoke(int ARG0) const
        {
            //ME_ASSERT(m_func.second != nullptr, "Cannot invoke unbound delegate. Call Bind() first.")();
            return _callable.second(_callable.first, ARG0);
        }

    private:

        Stub _callable;
    };


    // CLASS TEMPLATE
    // Delegate object that supports multiple callable targets.
    //
    template <typename Return, typename ...Args>
    class MulticastDelegate
    {
        typedef void* InstancePtr;
        typedef void(*Function)(Args...);

    public:

        explicit MulticastDelegate(const InstancePtr& instance, const Function& func)
            : _instance(instance)
            , _callable(func)
        {
        }

       // Invokes the delegate with the given arguments.
        void Invoke(ARG0 arg0) const
        {
            _callable(_instance, arg0);
        }

    private:

        InstancePtr _instance;
        Function    _callable;
    };

} // namespace drako

#endif // !DRAKO_DELEGATE_HPP