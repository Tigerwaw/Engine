#pragma once

class GameObject;
struct GameObjectEvent;

class Component
{
public:
    friend class GameObject;
    virtual ~Component() {}
    // Is automatically called after component creation.
    virtual void Start() = 0;

    // Is called every frame.
    virtual void Update() = 0;


    void SetActive(bool aActive) { myShouldUpdate = aActive; }
    bool GetActive() const { return myShouldUpdate; }
    GameObject* GetParent() { return myParent; }
protected:
    virtual void ReceiveEvent(const GameObjectEvent& aEvent) { aEvent; }

    // Is automatically set on component creation.
    GameObject* myParent = nullptr;
    bool myShouldUpdate = true;
};

