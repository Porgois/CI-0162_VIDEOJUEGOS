#ifndef EVENT_MANAGER_HPP
#define EVENT_MANAGER_HPP

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <typeindex>
#include <iostream>

#include "./event.hpp"

class IEventCallback {
    private:
        virtual void Call(Event& e) = 0;
    
    public:
        virtual ~IEventCallback() = default;
    
        void execute(Event& e) {
            Call(e);
        }
};

template<typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {
    private:
        typedef void (TOwner::*CallbackFunction)(TEvent&);

        TOwner* owner_instance;
        CallbackFunction callback_function;

        virtual void Call(Event& e) override {
            std::invoke(callback_function, owner_instance, static_cast<TEvent&>(e));
        }
    
    public:
        EventCallback(TOwner* owner_instance, CallbackFunction callback_function) {
            this->callback_function = callback_function;
            this->owner_instance = owner_instance;
        }
};

typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventManager {
    private:
        std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;
    
    public:
        EventManager() {
            std::cout << "[EVENT MANAGER] Executes constructor." << std::endl;  
        }

        ~EventManager() {
            std::cout << "[EVENT MANAGER] Executes desconstructor." << std::endl;  
        }

        void reset() {
            subscribers.clear();
        }

        template <typename TEvent, typename TOwner>
        void subscribeEvent(TOwner* owner_instance, \
            void (TOwner::*callback_function)(TEvent&)) {
            if (!subscribers[typeid(TEvent)].get()) {
                subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
            }
            
            auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(
                owner_instance, 
                callback_function
            );

            subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
        }

        template <typename TEvent, typename... TArgs>
        void emitEvent(TArgs&&... args) {
            auto handlers = subscribers[typeid(TEvent)].get();
            if (handlers) {
                for (auto it = handlers->begin(); it != handlers->end(); it++) {
                    auto handler = it->get();
                    TEvent event(std::forward<TArgs>(args)...);
                    handler->execute(event);
                }
            }
        }
};

#endif // EVENT_MANAGER__HPP