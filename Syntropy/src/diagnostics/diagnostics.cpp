#include "diagnostics/diagnostics.h"

#include <mutex>
#include <unordered_map>

namespace syntropy 
{
    namespace diagnostics 
    {

        //////////////// CALLTRACE ////////////////

        CallTrace::CallTrace(const char* file, const char* function, int line)
            : file_(file)
            , function_(function)
            , line_(line) 
        {

        }

        std::ostream& operator<< (std::ostream &out, const syntropy::diagnostics::CallTrace& calltrace)
        {
            out << calltrace.function_ << "(" << calltrace.file_ << ":" << calltrace.line_ << ")";
            return out;
        }
        
        //////////////// STACKTRACE ////////////////

        StackTrace::StackTrace(const CallTrace& calltrace)
        {
            calls_.push_back(calltrace);
        }

        StackTrace::StackTrace(StackTrace&& other) noexcept
            : calls_(std::move(other.calls_))
        {

        }

        StackTrace& StackTrace::operator=(StackTrace other) noexcept
        {
            other.Swap(*this);
            return *this;
        }

        void StackTrace::Swap(StackTrace& other) noexcept
        {
            std::swap(calls_, other.calls_);
        }

        std::ostream& operator<< (std::ostream &out, const syntropy::diagnostics::StackTrace& stacktrace)
        {
            for (size_t index = 0; index < stacktrace.calls_.size() - 1; ++index)
            {
                out << stacktrace.calls_[index] << "\n\t";
            }
            out << stacktrace.calls_.back();

            return out;
        }

        //////////////// CONTEXT :: INNER CONTEXT ////////////////

        struct Context::InnerContext
        {

            InnerContext() = default;

            InnerContext(const HashedString& name, std::shared_ptr<InnerContext> parent);

            bool Contains(const InnerContext& other) const;

            HashedString name_;                                 ///< \brief Full name of the context.

            std::shared_ptr<InnerContext> parent_;              ///< \brief Parent context. Nullptr if root context.

        };

        Context::InnerContext::InnerContext(const HashedString& name, std::shared_ptr<InnerContext> parent)
            : name_(name)
            , parent_(parent)
        {

        }

        bool Context::InnerContext::Contains(const InnerContext& other) const
        {
            auto other_context = &other;

            while (other_context)
            {
                if (name_ == other_context->name_)
                {
                    return true;
                }
                other_context = other_context->parent_.get();       // Walk up the hierarchy
            }

            return false;
        }

        //////////////// CONTEXT :: POOL ////////////////

        class Context::Pool
        {

        public:

            static Pool& GetInstance();

            std::shared_ptr<InnerContext> GetContextByName(const HashedString& name);

            std::shared_ptr<InnerContext> GetRootContext();

        private:

            Pool();

            std::shared_ptr<Context::InnerContext> root_;                                                           ///< \brief Root context

            std::recursive_mutex mutex_;                                                                            ///< \brief Used for synchronization

            std::unordered_map<HashedString::hash_t, std::shared_ptr<InnerContext>> contexts_;                      ///< \brief List of contexts registered so far

        };

        Context::Pool::Pool()
            : root_(std::make_shared<Context::InnerContext>())
        {
            contexts_.insert(std::make_pair(root_->name_.GetHash(), root_));        // Add the root to the context list
        }

        Context::Pool& Context::Pool::GetInstance()
        {
            static Pool instance;
            return instance;
        }

        std::shared_ptr<Context::InnerContext> Context::Pool::GetContextByName(const HashedString& name)
        {

            std::unique_lock<std::recursive_mutex> lock(mutex_);

            auto it = contexts_.find(name.GetHash());

            if (it != contexts_.end())
            {
                return it->second;      // Found
            }

            // Create a new context
            
            auto index = name.GetString().find_last_of(kSeparator);

            std::string parent_name = index != std::string::npos ?
                                      name.GetString().substr(0, index) :
                                      "";

            auto context = std::make_shared<Context::InnerContext>(name, 
                                                                   GetContextByName(parent_name));      // Get the parent context (recursive)

            contexts_.insert(std::make_pair(name.GetHash(),
                                            context));

            return context;
        }

        std::shared_ptr<Context::InnerContext> Context::Pool::GetRootContext()
        {
            return root_;
        }

        //////////////// CONTEXT ////////////////

        Context::Context()
            : context_(Pool::GetInstance().GetRootContext())
        {

        }

        Context::Context(const HashedString& name)
            : context_(Pool::GetInstance().GetContextByName(name))
        {

        }

        bool Context::operator==(const Context& other) const
        {
            return context_ == other.context_;
        }

        bool Context::operator!=(const Context& other) const
        {
            return context_ != other.context_;
        }

        const HashedString& Context::GetName() const
        {
            return context_->name_;
        }

        bool Context::Contains(const Context& other) const
        {
            return context_->Contains(*other.context_);
        }

        //////////////// EVENT ////////////////

        Event::Event(std::initializer_list<Context> contexts, const StackTrace& stacktrace, Severity severity)
            : timestamp_(std::chrono::high_resolution_clock::now())
            , severity_(severity)
            , thread_id_(std::this_thread::get_id())
            , contexts_(contexts)
            , stacktrace_(stacktrace)
        {

        }

    }
}