#pragma once

#include "ethpch.h"
#include "Ethane/Core/Base.h"

namespace Ethane {

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved, 
		AppTick, AppUpdate, AppRender, 
		KeyPressed, KeyReleased, KeyTyped, 
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication     = BIT(0), 
		EventCategoryInput           = BIT(1),
		EventCategoryKeyboard        = BIT(2),
		EventCategoryMouse           = BIT(3),
		EventCategoryMouseButton     = BIT(4)
	};

#define EVENT_CLASS_TYPE(eventtype) static EventType GetStaticType() {return EventType::eventtype;}\
				virtual EventType GetEventType() const override {return GetStaticType();}\
								virtual const char* GetName() const override {return #eventtype;}

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override {return category;}

	class Event
	{
	public:
		virtual ~Event() = default;

		bool Handled = false;
		//For Debug
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	
	};

	class EventDispatcher
	{
		template<typename Ty>
		using EventFn = std::function<bool(Ty&)>;

	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template<typename Ty>
		bool Dispatch(EventFn<Ty> func)
		{
			if (m_Event.GetEventType() == Ty::GetStaticType())
			{
				m_Event.Handled = func(*(Ty*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

}