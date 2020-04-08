#ifndef MISC_H
#define MISC_H

template<class T>
void SAFE_DELETE(T*& p)
{
    if(p)
    {
        delete p;
        p = nullptr;
    }
}
template<class T>
void SAFE_DELETE_ARRAY(T*& p)
{
    if(p)
    {
        delete[] p;
        p = nullptr;
    }
}
template<class T>
void SAFE_RELEASE(T*& p)
{
    if(p)
    {
        p->Release();
        p = nullptr;
    }
}

template<class T, size_t N>
constexpr inline size_t sizeOfArray(T (&array)[N])
{
    return N;
}

template<class T, size_t N>
inline void array_set(T (&array)[N], const T& v)
{
    std::fill(&array, &array + N, v);
}

template<class T, size_t N>
inline void array_setzero(T (&array)[N])
{
    memset(&array, 0, sizeof(T) * N);
}

template<class... Args>
struct type_list
{
    template<std::size_t N>
    using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
};

class scope_guards
{
public:
    scope_guards()                    = default;
    scope_guards(const scope_guards&) = delete;
    void operator=(const scope_guards&) = delete;
    scope_guards(scope_guards&&)        = delete;
    void operator=(scope_guards&&) = delete;

    template<class Callable>
    scope_guards& operator+=(Callable&& undo_func)
    {
        m_data.emplace_front(std::forward<Callable>(undo_func));
        return *this;
    }

    ~scope_guards()
    {
        for(auto& f: m_data)
        {
            try
            {
                f(); // must not throw
            }
            catch(...)
            {
            }
        }
    }

    void dismiss() noexcept { m_data.clear(); }

private:
    std::deque<std::function<void()>> m_data;
};


#define unknown(p) (p?p:"unknown")



#endif /* MISC_H */
