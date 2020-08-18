#ifndef ENUMUTIL_H
#define ENUMUTIL_H


/**
 * @brief Utility class to handle flags.
 *
 * This class can be used to handle flags of a same enumeration type.<br/>
 * It is meant to be used as an argument for functions and member methods and
 * as part of events.<br/>
 * `Flags<E>` objects can be easily _or-ed_ and _and-ed_ with other instances of
 * the same type or with instances of the type `E` (that is, the actual flag
 * type), thus converted to the underlying type when needed.
 */
template<typename E>
class Flags final {
    using InnerType = std::underlying_type_t<E>;

    constexpr InnerType toInnerType(E flag) const noexcept { return static_cast<InnerType>(flag); }

public:
    using Type = InnerType;

    /**
     * @brief Utility factory method to pack a set of values all at once.
     * @return A valid instance of Flags instantiated from values `V`.
     */
    template<E... V>
    static constexpr Flags<E> from() {
        return (Flags<E>{} | ... | V);
    }

    /**
     * @brief Constructs a Flags object from a value of the enum `E`.
     * @param flag A value of the enum `E`.
     */
    constexpr Flags(E flag) noexcept: flags{toInnerType(flag)} {}

    /**
     * @brief Constructs a Flags object from an instance of the underlying type
     * of the enum `E`.
     * @param f An instance of the underlying type of the enum `E`.
     */
    constexpr Flags(Type f): flags{f} {}

    /**
     * @brief Constructs an uninitialized Flags object.
     */
    constexpr Flags(): flags{} {}

    constexpr Flags(const Flags &f) noexcept: flags{f.flags} {  }
    constexpr Flags(Flags &&f) noexcept: flags{std::move(f.flags)} {  }

    ~Flags() noexcept { static_assert(std::is_enum_v<E>); }

    constexpr Flags & operator=(const Flags &f) noexcept {
        flags = f.flags;
        return *this;
    }

    constexpr Flags & operator=(Flags &&f) noexcept {
        flags = std::move(f.flags);
        return *this;
    }

    /**
     * @brief Or operator.
     * @param f A valid instance of Flags.
     * @return This instance _or-ed_ with `f`.
     */
    constexpr Flags operator|(const Flags &f) const noexcept { return Flags{flags | f.flags}; }

    /**
     * @brief Or operator.
     * @param flag A value of the enum `E`.
     * @return This instance _or-ed_ with `flag`.
     */
    constexpr Flags operator|(E flag) const noexcept { return Flags{flags | toInnerType(flag)}; }

    /**
     * @brief And operator.
     * @param f A valid instance of Flags.
     * @return This instance _and-ed_ with `f`.
     */
    constexpr Flags operator&(const Flags &f) const noexcept { return Flags{flags & f.flags}; }

    /**
     * @brief And operator.
     * @param flag A value of the enum `E`.
     * @return This instance _and-ed_ with `flag`.
     */
    constexpr Flags operator&(E flag) const noexcept { return Flags{flags & toInnerType(flag)}; }

    /**
     * @brief Checks if this instance is initialized.
     * @return False if it's uninitialized, true otherwise.
     */
    explicit constexpr operator bool() const noexcept { return !(flags == InnerType{}); }

    /**
     * @brief Casts the instance to the underlying type of `E`.
     * @return An integral representation of the contained flags.
     */
    constexpr operator Type() const noexcept { return flags; }

private:
    InnerType flags;
};

#endif /* ENUMUTIL_H */
