class noncopyable
{
private:
    noncopyable(const noncopyable& x) = delete;
    noncopyable& operator=(const noncopyable& x) = delete;
public:
    noncopyable() = default;
};