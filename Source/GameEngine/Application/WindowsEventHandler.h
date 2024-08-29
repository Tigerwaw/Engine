class WindowsEventHandler
{
public:
    friend class Application;

    enum class WindowsEvent
    {
        DropFiles = 0x0233,
        COUNT
    };

    WindowsEventHandler();
    ~WindowsEventHandler();

    void AddEvent(WindowsEvent aEventType, std::function<void(MSG)> aFunction);

private:
    void HandleMessage(MSG* aMSG);
    std::unordered_map<unsigned, std::vector<std::function<void(MSG)>>> myEvents;
};