#pragma once

class GameObject;

class Component
{
public:
    friend class GameObject;
    virtual ~Component() {}
    // Is automatically called after component creation.
    virtual void Start() = 0;

    // Is called every frame.
    virtual void Update() = 0;


    void SetActive(bool aActive) { myIsActive = aActive; }
    bool GetActive() const { return myIsActive; }
    
    // Is automatically set on component creation.
    GameObject* gameObject = nullptr;

    virtual bool Serialize(nl::json& outJsonObject) { outJsonObject; return false; }
    virtual bool Deserialize(nl::json& aJsonObject) { aJsonObject; return false; }
protected:

    bool myIsActive = true;
};

