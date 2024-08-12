#pragma once
#include "GameEngine/EngineDefines.h"

#include "Component.h"
#include "GameObjectEvent.h"

class GameObject final
{
public:
    GameObject();
    virtual ~GameObject();
    void Update();
    void SetActive(bool aActive);
    bool GetActive() const { return myIsActive; }
    void SetStatic(bool aStatic);
    bool GetStatic() const { return myIsStatic; }

    void SetName(std::string aName) { myName = aName; }
    const std::string GetName() const { return myName; }

    // COMPONENTS
    template <typename T, typename... Args>
    const std::shared_ptr<T> AddComponent(Args&&... args);

    template <typename T>
    const std::shared_ptr<T> GetComponent();

    template <typename T>
    const std::vector<std::shared_ptr<T>> GetComponents();
    // --

    // INTERNAL EVENT HANDLER
    // (The idea of this is to encourage decoupling between components and reduce the need for pulling data through 
    // GetComponent() every tick, and instead relegate that logic to an event call that polls the necessary components when needed).
    void SendEvent(const GameObjectEvent aEvent);
    void SendEvent(const GameObjectEventType aEventType, std::any aData = 0);
    // --

private:

    std::vector<std::shared_ptr<Component>> myComponents;
    bool myIsActive = true;
    bool myIsStatic = false;
    float myTimeAlive = 0;

    std::string myName;
};

template<class T, typename... Args>
const std::shared_ptr<T> GameObject::AddComponent(Args&&... args)
{
    std::shared_ptr<Component> newComponent = myComponents.emplace_back(std::make_shared<T>(args...));

    if (newComponent.get())
    {
        newComponent->gameObject = this;
        newComponent->Start();

        return std::dynamic_pointer_cast<T>(newComponent);
    }

    return std::shared_ptr<T>();
}

template<typename T>
inline const std::shared_ptr<T> GameObject::GetComponent()
{
    for (auto& comp : myComponents)
    {
        std::shared_ptr<T> castedComp = std::dynamic_pointer_cast<T>(comp);
        if (castedComp.get())
        {
            return castedComp;
        }
    }

    return std::shared_ptr<T>();
}

template<typename T>
inline const std::vector<std::shared_ptr<T>> GameObject::GetComponents()
{
    std::vector<std::shared_ptr<T>> componentVector;

    for (auto& comp : myComponents)
    {
        std::shared_ptr<T> castedComp = std::dynamic_pointer_cast<T>(comp);
        if (castedComp.get())
        {
            componentVector.emplace_back(castedComp);
        }
    }

    return componentVector;
}
