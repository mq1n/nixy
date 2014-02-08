/*
    The Nixy Library
    Code covered by the MIT License

    Author: mutouyun (http://darkc.at)
*/

#pragma once

#include "memory/alloc.h"
#include "pattern/prototype.h"

#include "general/general.h"
#include "typemanip/typemanip.h"
#include "utility/utility.h"
#include "algorithm/algorithm.h"
#include "preprocessor/preprocessor.h"

//////////////////////////////////////////////////////////////////////////
NX_BEG
//////////////////////////////////////////////////////////////////////////

namespace private_functor
{
    /*
        The tags of function type
    */

    struct fun_ptr_tag {};
    struct obj_ptr_tag {};

    template <typename Func_, bool = is_function<Func_>::value>
    struct tag_traits;

    template <typename Func_> struct tag_traits<Func_, true > { typedef fun_ptr_tag tag; };
    template <typename Func_> struct tag_traits<Func_, false> { typedef obj_ptr_tag tag; };

    /*
        The handler for storing a function
    */

    typedef nx::empty_t class_t;

    union handler
    {
        void(*fun_ptr)();
        void* obj_ptr;
        struct
        {
            void* this_ptr;
            void(class_t::*func_ptr)();
        } mem_ptr;
    };

    /*
        The invoker for call a function
    */

    template <typename F, typename Func_, typename This_ = nx::null_t, bool = is_function<Func_>::value
                                                                     , bool = is_pointer<Func_>::value>
    struct invoker;

    template <typename R, typename Func_>
    struct invoker<R(), Func_, nx::null_t, true, true>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<Func_>(hd.fun_ptr)))();
        }
    };

    template <typename R, typename Func_>
    struct invoker<R(), Func_, nx::null_t, false, true>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<Func_>(hd.obj_ptr)))();
        }
    };

    template <typename R, typename Func_>
    struct invoker<R(), Func_, nx::null_t, false, false>
    {
        static R invoke(handler& hd)
        {
            return (*(reinterpret_cast<Func_*>(&(hd.obj_ptr))))(); // hd.obj_ptr is not a pointer
        }
    };

    template <typename R, typename Func_, typename This_>
    struct invoker<R(), Func_, This_, true, true>
    {
        static R invoke(handler& hd)
        {
            return (reinterpret_cast<This_>(hd.mem_ptr.this_ptr)->*
                    reinterpret_cast<Func_>(hd.mem_ptr.func_ptr))();
        }
    };

    // the void()'s return value may not be void

    template <typename Func_>
    struct invoker<void(), Func_, nx::null_t, true, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<Func_>(hd.fun_ptr)))();
        }
    };

    template <typename Func_>
    struct invoker<void(), Func_, nx::null_t, false, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<Func_>(hd.obj_ptr)))();
        }
    };

    template <typename Func_>
    struct invoker<void(), Func_, nx::null_t, false, false>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (*(reinterpret_cast<Func_*>(&(hd.obj_ptr))))();
        }
    };

    template <typename Func_, typename This_>
    struct invoker<void(), Func_, This_, true, true>
    {
        static void invoke(handler& hd)
        {
            /*return*/ (reinterpret_cast<This_>(hd.mem_ptr.this_ptr)->*
                        reinterpret_cast<Func_>(hd.mem_ptr.func_ptr))();
        }
    };

#define NX_FUNCTOR_INVOKER_(n) \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), Func_, nx::null_t, true, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (*(reinterpret_cast<Func_>(hd.fun_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), Func_, nx::null_t, false, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (*(reinterpret_cast<Func_>(hd.obj_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), Func_, nx::null_t, false, false> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (*(reinterpret_cast<Func_*>(&(hd.obj_ptr))))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <typename R, NX_PP_TYPE_1(n, typename P), typename Func_, typename This_> \
    struct invoker<R(NX_PP_TYPE_1(n, P)), Func_, This_, true, true> \
    { \
        static R invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            return (reinterpret_cast<This_>(hd.mem_ptr.this_ptr)->* \
                    reinterpret_cast<Func_>(hd.mem_ptr.func_ptr))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), Func_, nx::null_t, true, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (*(reinterpret_cast<Func_>(hd.fun_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), Func_, nx::null_t, false, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (*(reinterpret_cast<Func_>(hd.obj_ptr)))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename Func_> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), Func_, nx::null_t, false, false> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (*(reinterpret_cast<Func_*>(&(hd.obj_ptr))))(NX_PP_TYPE_1(n, par)); \
        } \
    }; \
    template <NX_PP_TYPE_1(n, typename P), typename Func_, typename This_> \
    struct invoker<void(NX_PP_TYPE_1(n, P)), Func_, This_, true, true> \
    { \
        static void invoke(handler& hd, NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) \
        { \
            /*return*/ (reinterpret_cast<This_>(hd.mem_ptr.this_ptr)->* \
                        reinterpret_cast<Func_>(hd.mem_ptr.func_ptr))(NX_PP_TYPE_1(n, par)); \
        } \
    };
    NX_PP_MULT_MAX(NX_FUNCTOR_INVOKER_)
#undef NX_FUNCTOR_INVOKER_
}

/*
    The base class of functor
*/

template <typename Style_, typename Functor_, typename Invoker_>
class functor_base : public SafeBool<functor_base<Style_, Functor_, Invoker_> >
                   , public Unequal <Functor_>
{
public:
    typedef Style_   style_type;
    typedef Functor_ functor_type;
    typedef Invoker_ invoker_type;

protected:
    mutable private_functor::handler handler_;
    mutable invoker_type             invoker_;

private:
    class PlaceHolder
    {
    public:
        virtual ~PlaceHolder() {}

    public:
        virtual pvoid handler(void) const = 0;
        virtual PlaceHolder* clone(void) = 0;
    } * any_guard_;

    template <typename T>
    class Holder : public PlaceHolder
    {
    public:
        T held_;

    public:
        Holder(const T& v): held_(v) {}

    public:
        pvoid handler(void) const { return (pvoid)nx::addressof(held_); }
        PlaceHolder* clone(void) { return nx::alloc<Holder>(nx::ref(held_)); }
    };

protected:
    template <typename T>
    T* guard(const T& f)
    {
        any_guard_ = nx::alloc<Holder<T> >(nx::ref(f));
        return (T*)(any_guard_->handler());
    }

public:
    functor_base(void)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
    }

    template <typename Func_>
    functor_base(const Func_& f)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        bind(f);
    }

    template <typename Func_, typename Obj_>
    functor_base(Func_ f, Obj_* o)
        : invoker_(nx::nulptr), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        bind(f, o);
    }

    functor_base(const functor_base& fr)
        : invoker_(fr.invoker_), any_guard_(nx::nulptr)
    {
        nx::initialize(handler_);
        if (fr.any_guard_)
        {
            any_guard_ = nx::clone(fr.any_guard_);
            // only obj_ptr_tag may make a guard
            handler_.obj_ptr = any_guard_->handler();
        }
        else
            handler_ = fr.handler_;
    }

    ~functor_base()
    { nx::free(any_guard_); }

public:
    bool checkSafeBool(void) const
    {
        return !!invoker_;
    }

    void swap(functor_type& r)
    {
        nx::swap(handler_  , r.handler_);
        nx::swap(invoker_  , r.invoker_);
        nx::swap(any_guard_, r.any_guard_);
    }

    functor_type& operator=(functor_type& fr) // do swap
    {
        fr.swap(*reinterpret_cast<functor_type*>(this));
        return (*reinterpret_cast<functor_type*>(this));
    }

    friend bool operator==(const functor_type& f1, const functor_type& f2)
    {
        return nx::equal(f1.handler_, f2.handler_);
    }

protected:
    template <typename Func_>
    void assign_to(Func_ f, private_functor::fun_ptr_tag)
    {
        handler_.fun_ptr = reinterpret_cast<void(*)()>(f);
        invoker_ = &private_functor::invoker<style_type, Func_>::invoke;
    }

    template <typename Func_>
    void assign_to(Func_ f, private_functor::obj_ptr_tag)
    {
        std::memcpy(&(handler_.obj_ptr), &f, sizeof(pvoid)); // f may be not a pointer
        invoker_ = &private_functor::invoker<style_type, Func_>::invoke;
    }

    template <typename Func_, typename Obj_>
    void assign_to(Func_ f, Obj_* o)
    {
        handler_.mem_ptr.this_ptr = reinterpret_cast<void*>(o);
        handler_.mem_ptr.func_ptr = reinterpret_cast<void(private_functor::class_t::*)()>(f);
        invoker_ = &private_functor::invoker<style_type, Func_, Obj_*>::invoke;
    }

public:
    template <typename Func_>
    typename nx::enable_if<is_pointer<Func_>::value || (sizeof(Func_) <= sizeof(pvoid)),
    functor_type&>::type_t bind(Func_ f)
    {
        assign_to(f, typename private_functor::tag_traits<Func_>::tag());
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename Func_>
    typename nx::enable_if<!is_pointer<Func_>::value && (sizeof(Func_) > sizeof(pvoid)),
    functor_type&>::type_t bind(const Func_& f)
    {
        assign_to(guard(f), private_functor::obj_ptr_tag());
        return (*reinterpret_cast<functor_type*>(this));
    }

    template <typename Func_, typename Obj_>
    functor_type& bind(Func_ f, Obj_* o)
    {
        assign_to(f, o);
        return (*reinterpret_cast<functor_type*>(this));
    }
};

/*
    The functor for saving function and functor
*/

template <typename F>
class functor;

template <typename R>
class functor<R()>
    : public functor_base<R(), functor<R()>, R(*)(private_functor::handler&)>
{
    typedef functor_base<R(), functor<R()>, R(*)(private_functor::handler&)> base_t;
public:
    functor(void)         : base_t() {}
    functor(nx::nulptr_t) : base_t() {}
    functor(nx::none_t)   : base_t() {}
    template <typename Func_>
    functor(const Func_& f, typename nx::enable_if<!is_sametype<Func_, int>::value, int>::type_t = 0)
        : base_t(f)
    {}
    template <typename Func_, typename Obj_>
    functor(Func_ f, Obj_* o)
        : base_t(f, o)
    {}
    functor(const functor& fr)
        : base_t(static_cast<const base_t&>(fr))
    {}
    functor(const rvalue<functor>& fr)
        : base_t()
    { nx::swap(*this, fr); }
public:
    functor& operator=(functor fr) { return base_t::operator=(fr); }
    using base_t::swap;
public:
    R operator()(void) const
    {
        return (*base_t::invoker_)(base_t::handler_);
    }
};

#define NX_FUNCTOR_(n) \
template <typename R, NX_PP_TYPE_1(n, typename P)> \
class functor<R(NX_PP_TYPE_1(n, P))> \
    : public functor_base \
    < \
        R(NX_PP_TYPE_1(n, P)), \
        functor<R(NX_PP_TYPE_1(n, P))>, \
        R(*)(private_functor::handler&, NX_PP_TYPE_1(n, typename nx::traits<P, >::param_t)) \
    > \
{ \
    typedef functor_base \
    < \
        R(NX_PP_TYPE_1(n, P)), \
        functor<R(NX_PP_TYPE_1(n, P))>, \
        R(*)(private_functor::handler&, NX_PP_TYPE_1(n, typename nx::traits<P, >::param_t)) \
    > base_t; \
public: \
    functor(void)         : base_t() {} \
    functor(nx::nulptr_t) : base_t() {} \
    functor(nx::none_t)   : base_t() {} \
    template <typename Func_> \
    functor(const Func_& f, typename nx::enable_if<!is_sametype<Func_, int>::value, int>::type_t = 0) \
        : base_t(f) \
    {} \
    template <typename Func_, typename Obj_> \
    functor(Func_ f, Obj_* o) \
        : base_t(f, o) \
    {} \
    functor(const functor& fr) \
        : base_t(static_cast<const base_t&>(fr)) \
    {} \
    functor(const rvalue<functor>& fr) \
        : base_t() \
    { nx::swap(*this, fr); } \
public: \
    functor& operator=(functor fr) { return base_t::operator=(fr); } \
    using base_t::swap; \
public: \
    R operator()(NX_PP_TYPE_2(n, typename nx::traits<P, >::param_t par)) const \
    { \
        return (*base_t::invoker_)(base_t::handler_, NX_PP_TYPE_1(n, par)); \
    } \
};
NX_PP_MULT_MAX(NX_FUNCTOR_)
#undef NX_FUNCTOR_

//////////////////////////////////////////////////////////////////////////
NX_END
//////////////////////////////////////////////////////////////////////////